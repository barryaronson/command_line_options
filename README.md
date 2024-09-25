# command_line_options
A generic class for Linux/Unix style command line options using 'getopt_long()'. See:
https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html

To build debug version of test harnas:
g++ --std=c++17 -o clo -g -O0  test_command_line_options.cpp