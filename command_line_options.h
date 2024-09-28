#pragma once

#include <cstring>
#include <filesystem>
#include <getopt.h>
#include <vector>

namespace command_line_options {

// \cond

/*
For generating a unique option ID when there is no short option.
*/
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

/*
Allows handling options without knowing the type of there argument. See `option_value`.
*/
class option_base : public option /* `option` is from `getopt.h` */ {
public:
  option_base(int short_option, const char *long_option, int argument_required)
      : option{long_option, argument_required, nullptr, short_option}, present(false) {}
  virtual void set_value(const char *str) = 0;
  bool present; // true if this option was present on the command line
};

/*
Represents the help text for a particular option.
*/
class option_help {
public:
  option_help(const char *argument, const char *help)
      : argument_text(argument ? argument : ""), help_text(help ? help : "") {}
  const char *argument_text; // the format of an option argument; e.g., "=NUM"
  const char *help_text;     // a brief description of the option; e.g., "Number of lines"
};

// \endcond

/*! \class option_value
    \brief Represents a command line option with an optional argument value.

    Two forms exist: with an argument and without. For more information, see
    https://www.gnu.org/software/libc/manual/html_node/Getopt.html.
*/
template <typename T = int> class option_value : public option_base {
public:
  /*! \brief Represents an individual command line option with an argument (possibly optional).
      \param short_option Short option character or 0 for no short option
      \param long_option Long option name
      \param argument_required Either `no_argument`, `required_argument`, or `optional_argument`
      \param default_value Default value of option argument or 0 if `no_argument`
      \returns Nothing
  */
  option_value(int short_option, const char *long_option, int argument_required, T default_value)
      : option_base((short_option == 0) ? option_ID()() : short_option, long_option,
                    argument_required) {
    argument_value = default_value;
  }

  /*! \brief Represents an individual command line option with no argument.
      \param short_option Short option character or 0 for no short option
      \param long_option Long option name
      \returns Nothing
  */
  option_value(int short_option, const char *long_option)
      : option_value(short_option, long_option, no_argument, 0) {}

  /*! \brief Gets the option argument value
      \returns Argument value
  */
  T get_value() const { return argument_value; }

private:
  void set_value(const char *str) {
    std::istringstream iss(str);
    iss >> argument_value;
  }
  T argument_value;
};

/*! \class option_description
    \brief Represents a command line option with an optional argument value and help text.

    Two forms exist: with an argument and without. For more information, see
    https://www.gnu.org/software/libc/manual/html_node/Getopt.html.
*/
template <typename T = int> class option_description : public option_value<T>, public option_help {
public:
  /*! \brief Represents an individual command line option with help text and an optional argument.
      \param short_option Short option character or 0 for no short option
      \param long_option Long option name
      \param argument_required Either `no_argument`, `required_argument`, or `optional_argument`
      \param default_value Default value of option argument
      \param argument_help Help text describing form of option argument
      \param brief_help Help text giving a brief description of option
      \returns Nothing
  */
  option_description(int short_option, const char *long_option, int argument_required,
                     T default_value, const char *argument_help, const char *brief_help)
      : option_value<T>(short_option, long_option, argument_required, default_value),
        option_help(argument_help, brief_help) {};

  /*! \param short_option Short option character or 0 for no short option
      \param long_option Long option name
      \param argument_help Form of option argument
      \param brief_help Brief description of option
      \returns Nothing
  */
  option_description(int short_option, const char *long_option, const char *argument_help,
                     const char *brief_help)
      : option_description(short_option, long_option, no_argument, 0, argument_help, brief_help) {}
};

class help_message;

/*! \class command_line
    \brief Parses command line options described by either `option_value` or `option_description`.
    \details If a command line option is present, its presence is noted in the corresponding object
    from `options` (see constructor) along with its argument (if present). Each object in `options`
    is a pointer to an `object_description` or an `object_value` (mixing types is undefined). Both
    are passed as a pointer to their base class (`object_base`) so that they can be handled
    generically.
*/
class command_line {
  friend help_message;

public:
  /*! \brief Constructor
      \param argc As passed to `main()`
      \param argv As passed to `main()`
      \param options List of allowed options: either all of type `object_description` or all of type
      `object_value` (do not use their base class `option_base` directly).
      \returns Nothing
  */
  command_line(int argc, char *const argv[], std::initializer_list<option_base *> options)
      : value_list(options) {

    const size_t options_size = options.size();
    char *short_options =
        new char[(options_size * 3) + 1]; // 3 for optional arguments (e.g., "o::")
    option *long_options = new option[options_size + 1];

    // synthesize the arguments to `getopt_long()`
    convert_options_to_strings(short_options, long_options);

    // parse command line options noting the presence of each and saving its argument value
    parse_options(argc, argv, short_options, long_options);

    delete long_options;
    delete short_options;
  }

  /*! \brief For processing command line tokens after the options
      \returns Index to first non-option in `argv[]` array
  */
  int first_non_option() const { return optind; }

private:
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

  void convert_options_to_strings(char *short_options, option *long_options) const {
    int short_index = 0;
    int long_index = 0;

    for (auto v : value_list) { // go through all option objects and build input to getopt_long()
      const option *o = (option *)v;
      if (o->val < 256) {                      // otherwise, it only has a long option
        short_options[short_index++] = o->val; // short option letter
        switch (o->has_arg) {
        case optional_argument: // short option letter followed by `::` for optional argument
          short_options[short_index++] = ':';
        case required_argument: // short option letter followed by `:` for mandatory argument
          short_options[short_index++] = ':';
        case no_argument: // short option letter only (no argument); no `:` or `::`
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
    // these should now conform to what is required by `getopt_long()`
  }

  void parse_options(int argc, char *const argv[], char *short_options,
                     option *long_options) const {
    int c = 0;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) // -1 = end of mOptions
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

  std::vector<option_base *> value_list;
};

/*! \class help_message
    \brief Builds a help message (requires use of `option_description`).
    \details Builds a help message that can be used for printing when `-h` or `--help` is invoked on
    the command line.
*/
class help_message {
  /*! \brief Outputs the constructed help message to an `ostream` object.
      \param os `ostream` object
      \param hm `help_message` object
      \returns `ostream` object with help message
  */
  friend std::ostream &operator<<(std::ostream &os, const help_message &hm);

public:
  /*! \brief Generates a help message.
      \param argc As passed to program.
      \param argv As passed to program.
      \param usage An array of usages without the program name terminated by a NULL pointer.
      \param description Displayed after usages.
      \param example An array of examples terminated by a NULL pointer.
      \param command_line The `command_line` object used to process command line options
      \returns Nothing
  */
  help_message(int argc, char *const argv[], const char *usage[], const char *description,
               const char *example[], const command_line &command_line_obj)
      : help_msg("") {

    // add usage
    std::filesystem::path p(argv[0]);
    if (usage) {
      for (int i = 0; usage[i]; ++i) {
        if (i == 0) {
          help_msg += "Usage:  ";
        } else {
          help_msg += "        ";
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
          help_msg += "Example:  ";
        } else {
          help_msg += "          ";
        }
        help_msg += p.filename();
        help_msg += ' ';
        help_msg += example[i];
        help_msg += "\n";
      }
    }

    // calculate maximum argument width for formatting
    size_t max_argument_width = 0;
    for (auto v : command_line_obj.value_list) {
      const option_help *o = dynamic_cast<option_help *>(v);
      if (o == nullptr) { // this is only done for the first time (here)
        throw std::runtime_error("Option description does not contain help text.\n");
      }
      const size_t argument_width = strlen(v->name) + strlen(o->argument_text) + 2;
      if (argument_width > max_argument_width) {
        max_argument_width = argument_width;
      }
    }

    // add option descriptions
    help_msg += "\nMandatory arguments to long options are mandatory for short options too.\n";
    for (auto v : command_line_obj.value_list) {
      const option_help *o = dynamic_cast<option_help *>(v); // tested above; will still be safe
      size_t argument_width_delta =
          max_argument_width - (strlen(v->name) + strlen(o->argument_text));
      if (v->val < 256) { // has short option
        help_msg += '-';
        help_msg += v->val;
        help_msg += ", --";
      } else {                     // has long option only
        argument_width_delta += 4; // to accommodate for the missing short option
        help_msg += "--";
      }

      help_msg += v->name;
      help_msg += o->argument_text;

      for (size_t i = 0; i < argument_width_delta; ++i)
        help_msg += ' ';

      help_msg += o->help_text;
      help_msg += '\n';
    }
  }

  /*! \brief Gets the constructed help message.
      \returns Help message
  */
  std::string &help() { return help_msg; }

private:
  std::string help_msg;
};

std::ostream &operator<<(std::ostream &os, help_message &hm) {
  os << hm.help();
  return os;
}

} // namespace command_line_options