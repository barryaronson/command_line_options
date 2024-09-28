#include "command_line_options.h"
#include <fstream>
#include <iostream>

int main(int argc, char *const argv[]) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }

  // create the command line options
  command_line_options::option_value<int> number('n', "number", required_argument, 10);
  command_line_options::option_value version('v', "version");

  // parse the command line options
  command_line_options::command_line options(argc, argv, {&number, &version});

  // read the first 'n' lines from the file and display them
  std::fstream input(argv[argc - 1]);
  const size_t buf_size = 1024;
  char *buf = new char[buf_size];

  for (int i = 0; !input.eof() && i < number.get_value(); ++i) {
    input.getline(buf, buf_size);
    std::cout << buf << std::endl;
  }

  return EXIT_SUCCESS;
}