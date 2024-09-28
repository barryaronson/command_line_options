# command_line_options
A header-only set of generic classes for processing Gnu style command line options using `getopt_long()`. See:
[title](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html)

All functions are inline. However, the lengthy ones will generally only be invoked once resulting in no penalty in terms of code space.

There are two classes for describing an option: one with associated help text (`option_description`) and one without help text (`option_value`). Processing options input on the command line when the program was invoked is handled by `command_line`. A help message can be generated with `help_message`.

The option argument value stored in either `option_description` or `option_value` is based on the template type. No type checking is performed so reasonable care needs to be taken when selecting a type. Typically, all command line options can be covered with an `int`, `double`, or `std::string`. Container types are not necessary because the rules for `getopt_long()` do not allow for multiple arguments to an option.

There are two example programs, one with help text (`first_n_w_help.cpp`) and one without (`first_n.cpp`). The test drivers are `test_command_line_options.cpp` and `test_command_line_options_w_help.cpp`. See the comment at the top of each file for build instructions.

Basically, all that is needed to process command line options is:
1. Include the `command_line_options.h` header file,
2. Construct one or more `option_value` (minimally) or `option_description` (optionally) objects,
3. Construct a `command_line` object with the objects created in step 2,
4. Retrieve presence and/or value of each option from its corresponding object as needed,
5. Create and display a help message with `help_message` if needed (requires use of `option_description`).