#include "command_line_options.h"
#include <fstream>
#include <iostream>

int main(int argc, char *const argv[]) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }

  // create the command line options w/ help text
  command_line_options::option_description number('n', "number", required_argument, 10, "=NUM",
                                                  "Number of lines");
  command_line_options::option_description version('v', "version", nullptr, "Version");
  command_line_options::option_description help('h', "help", nullptr, "Help");

  // parse the command line options
  command_line_options::command_line options(argc, argv, {&number, &version, &help});

  // look for help option
  if (help.present == true) { // create help message
    const char *usage[] = {"[OPTION]...", "--help", nullptr};
    const char *description = "Example with hel for 'command_line_options' classes";
    const char *example[] = {"-n 10", "--number=10", "-vn10", nullptr};
    command_line_options::help_message help_msg(argc, argv, usage, description, example, options);
    std::cout << help_msg << std::endl;
    return EXIT_SUCCESS;
  }

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