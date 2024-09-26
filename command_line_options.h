#pragma once

#include <cstring>
#include <filesystem>
#include <getopt.h>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace command_line_options {

class option_ID {
public:
  int operator()() { return option_ID_counter++; } // call operator
private:
  static int option_ID_counter;
};

#ifndef COMMAND_LINE_OPTIONS_OPTION_ID_COUNTER
#define COMMAND_LINE_OPTIONS_OPTION_ID_COUNTER
int option_ID::option_ID_counter = 0x100; // leaves room for the single character options
#endif

class option_generic : public option /* 'option' is from 'getopt.h' */ {
public:
  option_generic(int short_option, const char *long_option, int argument_required,
                 const char *help = 0)
      : help_string(help), present(false),
        option{long_option, argument_required, nullptr, short_option} {}
  virtual void set_value(const char *str) = 0;
  const char *help_string;
  bool present;
};

template <typename T = int> class option_description : public option_generic {
public:
  /*! \fn option_description::option_description(int short_option, const char *long_option, int
    argument_required, T default_value, const char *help = 0)
    \brief Represents an individual command line option with argument or optional argument.
    \param short_option Short option character or 0 for no short option
    \param long_option Long option name
    \param argument_required
    // 'no_argument' (or 0) if the option does not take an argument;
    // 'required_argument' (or 1) if the option requires an argument;
    // or 'optional_argument' (or 2)
    \param default_value Default value of option argument
    \param help Brief description of option
    \return Nothing
  */
  option_description(int short_option, const char *long_option, int argument_required,
                     T default_value, const char *help = 0)
      : option_generic((short_option == 0) ? option_ID()() : short_option, long_option,
                       argument_required, help) {
    argument_value = default_value;
  };

  /*! \fn option_description::option_description(int short_option, const char *long_option, const
     char *help)
      \brief Represents an individual command line option with no argument.
      \param short_option Short option character or 0 for no short option
      \param long_option Long option name
      \param help Brief description of option
      \return Nothing
  */
  option_description(int short_option, const char *long_option, const char *help = 0)
      : option_description(short_option, long_option, no_argument, 0, help) {}

  /*! \fn set_value(const char *str)
      \brief Sets the option argument value
      \param str Pointer to character string representation of value (e.g., "1", "3.141", "text")
      \return Nothing
  */
  void set_value(const char *str) {
    std::istringstream iss(str);
    iss >> argument_value;
  }

  /*! \fn get_value()
      \brief Gets the option argument value
      \return Argument value
  */
  T get_value() const { return argument_value; }

private:
  T argument_value;
};

class help_message;

/*! \class command_line
    \brief Parses command line options.

    If a command line option is present, its presence is stored in the corresponding object from
   'values' along with its argument (if present).
*/
class command_line {
  friend help_message;

public:
  /*! \fn command_line::command_line()
      \brief Parses command line tokens and interprets values.
      \param argc As passed to program.
      \param argv As passed to program.
      \param values List of allowed options
      \return Nothing
  */
  command_line(int argc, char *const argv[], std::initializer_list<option_generic *> values)
      : value_list(values) {

    const size_t values_size = values.size();
    char *short_options = new char[(values_size * 3) + 1]; // 3 for optional arguments (e.g., "o::")
    option *long_options = new option[values_size + 1];

    // synthesize the arguments to 'getopt_long()'
    convert_options_to_strings(short_options, long_options);

    // parse options on the command line and save the option argument values
    parse_options(argc, argv, short_options, long_options);

    delete long_options;
    delete short_options;
  }

  auto find_short_option(int short_option) const {
    for (auto v : value_list) {
      if (v->val == short_option) {
        return v;
      }
    }

    throw std::runtime_error("Unknown short option.\n");
  }

  auto find_long_option(const char *long_option) const {
    for (auto v : value_list) {
      if (strcmp(v->name, long_option) == 0) {
        return v;
      }
    }

    throw std::runtime_error("Unknown long option.\n");
  }

private:
  void convert_options_to_strings(char *short_options, option *long_options) const {
    int short_index = 0;
    int long_index = 0;

    for (auto v : value_list) { // go through all option objects and build input to getopt_long()
      const option *o = (option *)v;
      if (o->val < 256) {                      // otherwise, it only has a long option
        short_options[short_index++] = o->val; // short option letter
        switch (o->has_arg) {
        case optional_argument: // short option letter followed by '::' for optional argument
          short_options[short_index++] = ':';
        case required_argument: // short option letter followed by ':' for mandatory argument
          short_options[short_index++] = ':';
        case no_argument: // short option letter only (no argument); no ':' or '::'
          break;
        default:
          throw std::runtime_error("Invalid argument option\n");
          break;
        }
      }
      long_options[long_index++] = *o;
    }

    short_options[short_index] = '\0';
    long_options[long_index] = {0, 0, 0, 0};
    // these should now conform to what is required by 'getopt_long()'
  }

  void parse_options(int argc, char *const argv[], char *short_options,
                     option *long_options) const {
    int c = 0;
    int optionIndex = 0;

    while ((c = getopt_long(argc, argv, short_options, long_options,
                            &optionIndex)) != -1) // -1 = end of mOptions
    {
      auto option = find_short_option(c); // get the option description

      option->present = true; // option was present on command line

      switch (option->has_arg) { // set the option argument value if necessary
      case no_argument:
        break;
      case optional_argument:
        if (!optarg) {
          break;
        }
      case required_argument:
        option->set_value(optarg);
        break;
      default:
        throw std::runtime_error("Invalid argument option\n");
      }
    }
  }

  std::vector<option_generic *> value_list;
};

class help_message {
  friend std::ostream &operator<<(std::ostream &os, const help_message &hm);

public:
  /*! \fn command_line::command_line()
      \brief Parses command line tokens and interprets values.
      \param argc As passed to program.
      \param argv As passed to program.
      \param usage An array of usages without the program name terminated by a NULL pointer.
      \param description Displayed after usages.
      // Note: See class option_description for option descriptions
      \param example An array of examples terminated by a NULL pointer.
      \param values List of allowed options
      \return Nothing
  */
  help_message(int argc, char *const argv[], const char *usage[], const char *description,
               const char *example[], const command_line &command_line_obj)
      : help_msg("") {
    // add usage
    help_msg += "Usage:\t";
    std::filesystem::path p(argv[0]);
    if (usage) {
      for (int i = 0; usage[i]; ++i) {
        if (i > 0) {
          help_msg += '\t';
        }
        help_msg += p.filename();
        help_msg += ' ';
        help_msg += usage[i];
        help_msg += "\n";
      }
    }

    // add description
    if (description) {
      help_msg += description;
      help_msg += "\n";
    }

    //  add examples
    if (example) {
      for (int i = 0; example[i]; ++i) {
        if (i == 0) {
          help_msg += "Example:\t";
        } else {
          help_msg += '\t';
        }
        help_msg += p.filename();
        help_msg += ' ';
        help_msg += example[i];
        help_msg += "\n";
      }
    }

    // add option descriptions
    help_msg += "\nMandatory arguments to long options are mandatory for short options too.\n";
    for (auto v : command_line_obj.value_list) {
      if (v->val < 256) { // has short option
        help_msg += '-';
        help_msg += v->val;
        help_msg += ", --";
      } else { // has long option only
        help_msg += "--";
      }
      help_msg += v->name;
      help_msg += v->help_string;
      help_msg += "\n";
    }
  }

  std::string &help() { return help_msg; }

private:
  std::string help_msg;
};

std::ostream &operator<<(std::ostream &os, help_message &hm) {
  os << hm.help();
  return os;
}

} // namespace command_line_options