#include <iostream>

#include "command_line_options.h"

using namespace std;
using namespace command_line_options;

void print_option(option_generic *lo) {
  cout << "flag = " << lo->flag << "\thas_arg = " << lo->has_arg
       << "\thelp_string = " << lo->help_string << "\tname = " << lo->name
       << "\tpresent = " << lo->present << "\tval = " << lo->val << endl;
}

int main(int argc, char *const argv[]) {
  option_description<int> int_arg('n', "number", required_argument, 0,
                                  "=NUM\tOption with integer argument");
  option_description<string> string_arg('s', "string", required_argument, "default",
                                        "=STRING\tOption with string argument");
  option_description<double> double_arg('d', "double", required_argument, 3.141,
                                        "=NUM\tOption with double argument");
  option_description<int> opt_int_arg('o', "optional", optional_argument, 1,
                                      "[=NUM]\tOption with optional int argument");
  option_description<int> int_arg_long_only(0, "long-only", required_argument, 0,
                                            "=NUM\t\tLong option only with integer argument");
  option_description help('h', "help", "\t\tHelp");

  const command_line options(
      argc, argv, {&int_arg, &string_arg, &double_arg, &opt_int_arg, &int_arg_long_only, &help});

  const char *usage[] = {"[OPTION]...", "--help", nullptr};
  const char *description = "Test harness for 'command_line_options' classes";
  const char *example[] = {"-n 10 -s string --double=3.141 -o4\n"
                           "Note no space allowed for short options "
                           "with optional arguments",
                           nullptr};
  help_message help_msg(argc, argv, usage, description, example, options);

  if (help.present) {
    cout << help_msg << endl;
  } else {
    cout << "int_arg = " << int_arg.get_value() << endl;
    cout << "string_arg = " << string_arg.get_value() << endl;
    cout << "double_arg = " << double_arg.get_value() << endl;
    cout << "opt_int_arg = " << opt_int_arg.get_value() << endl;
    cout << "int_arg_long_only = " << int_arg_long_only.get_value() << endl;

    auto lo = options.find_long_option(("double"));
    print_option(lo);
    auto so = options.find_short_option('n');
    print_option(so);
  }

  return EXIT_SUCCESS;
}