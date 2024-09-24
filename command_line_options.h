#pragma once

#include <getopt.h>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T> T string_to(const char *str) {
  std::istringstream iss(str);
  T result;
  iss >> result;
  return result;
}

class option_value : public option {
public:
  option_value(const char *long_option, int short_option, int argument_required)
      : option{long_option, argument_required, nullptr, short_option} {}
  virtual void set_value(const char *str) = 0;
};

template <typename T = int> class option_description : public option_value {
public:
  /*! \fn option_description::option_description()
      \brief Represents an individual command line option.
      \param help_string As passed to programe.
      \param long_option Long option name
      \param short_option Short option character or unique value
      \param argument_required
      // no_argument (or 0) if the option does not take an argument;
      // required_argument (or 1) if the option requires an argument;
      // or optional_argument (or 2)
      \return Nothing
  */
  option_description(const char *help_string, const char *long_option, int short_option,
                     int argument_required, T default_value)
      : option_value(long_option, short_option, argument_required) {
    argument_value = default_value;
  };
  void set_value(const char *str) {
    std::istringstream iss(str);
    iss >> argument_value;
  }
  T get_value() { return argument_value; }

private:
  T argument_value;
};

/*! \class command_line
    \brief Parses command line options.

    Each attribute string is mapped to a unique enum Attribute.
*/
class command_line {
public:
  /*! \fn command_line::command_line()
      \brief Parses command line tokens and interprets values.
      \param argc As passed to programe.
      \param argv As passed to programe.
      \param usage An array of usages without the program name terminated by a NULL pointer.
      \param description Displayed after usages.
      // Note: See class option_description for option descriptions
      \param example An array of example terminated by a NULL pointer.
      \return Nothing
  */
  command_line(int argc, char *const argv[], const char *usage[], const char *description,
               const char *example[], std::initializer_list<option_value *> values);
  void help();

private:
  auto find_option(int short_option);
  void convert_options_to_strings(char *short_options, option *long_options);
  std::vector<option_value *> value_list;
};