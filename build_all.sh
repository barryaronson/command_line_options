echo Building first_n_w_help
g++ --std=c++17 -o first_n_w_help -g -O0 -Wall -I include example/first_n_w_help.cpp
echo Building first_n
g++ --std=c++17 -o first_n -g -O0 -Wall -I include example/first_n.cpp
echo Building cloh
g++ --std=c++17 -o cloh -g -O0 -Wall -I include test/test_command_line_options_w_help.cpp
echo Building clo
g++ --std=c++17 -o clo -g -O0 -Wall -I include test/test_command_line_options.cpp
echo Done