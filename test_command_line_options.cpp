#include <iostream>

#include "command_line_options.h"

using namespace std;

int main(int argc, char *const argv[]) {
  const char *usage[] = {"[OPTION]...", "--help", nullptr};
  const char *description = "Test harness for 'command_line_options' class";
  const char *example[] = {"-n 10 -s string -d 3.141", nullptr};

  option_description<int> int_arg("Option with integer argumnent", "number", 'n', required_argument,
                                  0);
  option_description<string> string_arg("Option with string argumnent", "string", 's',
                                        required_argument, "");
  option_description<double> double_arg("Option with double argumnent", "double", 'd',
                                        required_argument, 2.0);
  option_description help("Help", "help", 'h');

  command_line options(argc, argv, usage, description, example,
                       {&int_arg, &string_arg, &double_arg, &help});
  options.help();

  return EXIT_SUCCESS;
}