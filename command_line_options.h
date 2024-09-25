#pragma once

#include <getopt.h>
#include <sstream>
#include <string>
#include <vector>

namespace command_line_options {

class option_generic : public option /* 'option' is from 'getopt.h' */ {
public:
  option_generic(const char *help, const char *long_option, int short_option, int argument_required)
      : help_string(help), present(false),
        option{long_option, argument_required, nullptr, short_option} {}
  virtual void set_value(const char *str) = 0;
  const char *help_string;
  bool present;
};

template <typename T = int> class option_description : public option_generic {
public:
  /*! \fn option_description::option_description()
      \brief Represents an individual command line option.
      \param help Brief description of option
      \param long_option Long option name
      \param short_option Short option character or unique value
      \param argument_required
      // no_argument (or 0) if the option does not take an argument;
      // required_argument (or 1) if the option requires an argument;
      // or optional_argument (or 2)
      \return Nothing
  */
  option_description(const char *help, const char *long_option, int short_option,
                     int argument_required, T default_value)
      : option_generic(help, long_option, short_option, argument_required) {
    argument_value = default_value;
  };
  option_description(const char *help, const char *long_option, int short_option)
      : option_description(help, long_option, short_option, no_argument, 0) {}
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
      \param example An array of examples terminated by a NULL pointer.
      \return Nothing
  */
  command_line(int argc, char *const argv[], const char *usage[], const char *description,
               const char *example[], std::initializer_list<option_generic *> values);

  std::string &help() { return help_msg; }

private:
  void build_help_msg(char *const argv[], const char *usage[], const char *description,
                      const char *example[], std::initializer_list<option_generic *> values);
  auto find_option(int short_option);
  void convert_options_to_strings(char *short_options, option *long_options);
  void parse_options(int argc, char *const argv[], char *short_options, option *long_options);
  std::vector<option_generic *> value_list;
  std::string help_msg;
};

} // namespace command_line_options