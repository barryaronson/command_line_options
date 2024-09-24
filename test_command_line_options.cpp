#include <iostream>

#include "command_line_options.h"

using namespace std;

#include "myclass.h" // todo - remove

/*
static struct option longOptions[] = {{"run", required_argument, 0, 'r'},
                                      {"stations", required_argument, 0, 's'},
                                      {"trucks", required_argument, 0, 't'},
                                      {"help", no_argument, 0, 'h'},
                                      {0, 0, 0, 0}};
*/
int main(int argc, char *const argv[]) {
  if (argc < 2) {
    cerr << "Insufficient arguments\n";
    return EXIT_FAILURE;
  }
  const char *usage[] = {nullptr};
  const char *description = "";
  const char *example[] = {nullptr};

  option_description<int> trucks("trucks - int", "trucks", 't', required_argument, 0);
  option_description<string> run("run - string", "run", 'r', required_argument, "");
  option_description<double> station("stations - string", "stations", 's', required_argument, 2.0);

  command_line options(argc, argv, usage, description, example, {&station, &run, &trucks});

  return EXIT_SUCCESS;
}