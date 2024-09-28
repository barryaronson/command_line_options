/*
Example program showing how to process command line options including providing a help message.
Compile with:
g++ --std=c++17 -o first_n_w_help -g -O0 -Wall first_n_w_help.cpp
*/
#include "command_line_options.h"
#include <fstream>
#include <iostream>

int main(int argc, char *const argv[]) {
  // create the command line options w/ help text
  command_line_options::option_description number('n', "number", required_argument, 10, "=NUM",
                                                  "Number of lines");
  command_line_options::option_description version('v', "version", nullptr, "Version");
  command_line_options::option_description help('h', "help", nullptr, "Help");

  // parse the command line options
  command_line_options::command_line options(argc, argv, {&number, &version, &help});

  // look for help option
  if (help.present == true) {
    // create help message
    const char *usage[] = {"[OPTION]...", "--help", nullptr};
    const char *description = "Example with hel for 'command_line_options' classes";
    const char *example[] = {"-n 10", "--number=10", "-vn10", nullptr};
    command_line_options::help_message help_msg(argc, argv, usage, description, example, options);

    // display help message
    std::cout << help_msg << std::endl;
    return EXIT_SUCCESS;
  }

  // display version if requested
  if (version.present == true) {
    std::cout << "first_n_w_help 1.0\n";
  }

  // for reading from the file
  int filename_index = options.first_non_option(); // 'argv[]' index to filename
  if (filename_index >= argc) {
    return EXIT_FAILURE;
  }
  std::fstream input(argv[filename_index]); // open file to be read
  const size_t buf_size = 1024;
  char *buf = new char[buf_size]; // buffer for reading each line

  // get number of lines to display from command line option or default value
  const int number_of_lines = number.get_value();

  // read the first 'n' lines from the file and display them
  for (int i = 0; !input.eof() && i < number_of_lines; ++i) {
    input.getline(buf, buf_size);
    std::cout << buf << std::endl;
  }

  return EXIT_SUCCESS;
}