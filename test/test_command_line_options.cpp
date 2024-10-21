/*
Test harness for command_line_options. Compile with:
g++ --std=c++17 -o clo -g -O0 -Wall test_command_line_options.cpp
*/
#include "command_line_options.h"

#include <iostream>

using namespace std;
using namespace command_line_options;

int main(int argc, char *const argv[]) {

  // define the command line options
  option_value<int> int_arg('n', "number", required_argument, 0);
  option_value<string> string_arg('s', "string", required_argument, "default");
  option_value<double> double_arg('d', "double", required_argument, 3.141);
  option_value<int> opt_int_arg('o', "optional", optional_argument, 1);
  option_value<int> int_arg_long_only(0, "long-only", required_argument, 0);
  option_value version('v', "version");

  // parse the command line options
  const command_line options(
      argc, argv, {&int_arg, &string_arg, &double_arg, &opt_int_arg, &int_arg_long_only, &version});
  cout << "int_arg = " << int_arg.get_value() << endl;
  cout << "string_arg = " << string_arg.get_value() << endl;
  cout << "double_arg = " << double_arg.get_value() << endl;
  cout << "opt_int_arg = " << opt_int_arg.get_value() << endl;
  cout << "int_arg_long_only = " << int_arg_long_only.get_value() << endl;

  if (version.present) {
    cout << "version = 1.0\n";
  }

  return EXIT_SUCCESS;
}