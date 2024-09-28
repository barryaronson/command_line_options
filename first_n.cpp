/* Example program showing how to process command line options.
Compile with:
g++ --std=c++17 -o first_n -g -O0 -Wall first_n.cpp
*/                                                                                                 \
#include "command_line_options.h"
#include <fstream>
#include <iostream>

int main(int argc, char *const argv[]) {
  // create the command line options
  command_line_options::option_value<int> number('n', "number", required_argument, 10);
  command_line_options::option_value version('v', "version");

  // parse the command line options
  command_line_options::command_line options(argc, argv, {&number, &version});

  // display version if requested
  if (version.present == true) {
    std::cout << "first_n 1.0\n";
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