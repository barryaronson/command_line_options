#include <iostream>

#include "command_line_options.h"

using namespace std;
using namespace command_line_options;

int main(int argc, char *const argv[]) {
  const char *usage[] = {"[OPTION]...", "--help", nullptr};
  const char *description = "Test harness for 'command_line_options' classes";
  const char *example[] = {"-n 10 -s string --double=3.141 -o4\n"
                           "Note no space allowed for short options "
                           "with optional argumanets",
                           nullptr};

  option_description<int> int_arg('n', "number", "=NUM\tOption with integer argumnent",
                                  required_argument, 0);
  option_description<string> string_arg('s', "string", "=STRING\tOption with string argumnent",
                                        required_argument, "default");
  option_description<double> double_arg('d', "double", "=NUM\tOption with double argumnent",
                                        required_argument, 3.141);
  option_description<int> opt_int_arg('o', "optional", "[=NUM]\tOption with optional int argumnent",
                                      optional_argument, 1);
  option_description<int> int_arg_long_only(
      0, "long-only", "=NUM\tLong option only with integer argumnent", required_argument, 0);
  option_description help('h', "help", "\t\tHelp");

  command_line options(
      argc, argv, usage, description, example,
      {&int_arg, &string_arg, &double_arg, &opt_int_arg, &int_arg_long_only, &help});
  if (help.present == true) {
    cout << options.help() << endl;
  } else {
    cout << "int_arg = " << int_arg.get_value() << endl;
    cout << "string_arg = " << string_arg.get_value() << endl;
    cout << "double_arg = " << double_arg.get_value() << endl;
    cout << "opt_int_arg = " << opt_int_arg.get_value() << endl;
    cout << "int_arg_long_only = " << int_arg_long_only.get_value() << endl;
  }

  return EXIT_SUCCESS;
}