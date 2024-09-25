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

  option_description<int> int_arg("=NUM\tOption with integer argumnent", "number", 'n',
                                  required_argument, 0);
  option_description<string> string_arg("=STRING\tOption with string argumnent", "string", 's',
                                        required_argument, "default");
  option_description<double> double_arg("=NUM\tOption with double argumnent", "double", 'd',
                                        required_argument, 3.141);
  option_description<int> opt_int_arg("[=NUM]\tOption with optional int argumnent", "optional", 'o',
                                      optional_argument, 1);
  option_description help("\t\tHelp", "help", 'h');

  command_line options(argc, argv, usage, description, example,
                       {&int_arg, &string_arg, &double_arg, &opt_int_arg, &help});
  if (help.present == true) {
    cout << options.help() << endl;
  } else {
    cout << "int_arg = " << int_arg.get_value() << endl;
    cout << "string_arg = " << string_arg.get_value() << endl;
    cout << "double_arg = " << double_arg.get_value() << endl;
    cout << "opt_int_arg = " << opt_int_arg.get_value() << endl;
  }

  return EXIT_SUCCESS;
}