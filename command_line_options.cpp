#include <filesystem>
#include <stdexcept>
#include <string>

#include "command_line_options.h"

auto command_line_options::command_line::find_option(int short_option) {
  for (auto v : value_list) {
    if (v->val == short_option) {
      return v;
    }
  }

  throw std::runtime_error("Unknown short option.\n");
}

command_line_options::command_line::command_line(int argc, char *const argv[], const char *usage[],
                                                 const char *description, const char *example[],
                                                 std::initializer_list<option_generic *> values)
    : help_msg(""), value_list(values) {
  build_help_msg(argv, usage, description, example, values);

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

void command_line_options::command_line::build_help_msg(
    char *const argv[], const char *usage[], const char *description, const char *example[],
    std::initializer_list<option_generic *> values) {

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
  for (auto v : value_list) {
    help_msg += '-';
    help_msg += v->val;
    help_msg += ", --";
    help_msg += v->name;
    help_msg += v->help_string;
    help_msg += "\n";
  }
}

void command_line_options::command_line::convert_options_to_strings(char *short_options,
                                                                    option *long_options) {
  int short_index = 0;
  int long_index = 0;

  for (auto v : value_list) {
    const option *o = (option *)v;
    short_options[short_index++] = o->val; // short option letter
    switch (o->has_arg) {
    case optional_argument: // short option letter followed by '::'
      short_options[short_index++] = ':';
    case required_argument: // short option letter followed by ':'
      short_options[short_index++] = ':';
    case no_argument:
      break;
    default:
      throw std::runtime_error("Invalid argument option\n");
      break;
    }
    long_options[long_index++] = *o;
  }

  short_options[short_index] = '\0';
  long_options[long_index] = {0, 0, 0, 0};
}

void command_line_options::command_line::parse_options(int argc, char *const argv[],
                                                       char *short_options, option *long_options) {
  int c = 0;
  int optionIndex = 0;

  while ((c = getopt_long(argc, argv, short_options, long_options,
                          &optionIndex)) != -1) // -1 = end of mOptions
  {
    auto option = find_option(c); // get the option description

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
    default:
      throw std::runtime_error("Invalid argument option\n");
    }
  }
}