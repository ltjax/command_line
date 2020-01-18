# command_line
A portable command line parser for C++17

This is a small command line parser with the following goals:
* Do not duplicate the option name
* Initialize option values directly
* No dependencies other than the standard library

Here's a small sample:

```c++
command_line::parser Parser;
auto PortSetting = Parser.optional<std::uint16_t>('p', "port", "On which port to run", DefaultPort);
Parser.run(argc, argv);
auto Port = PortSetting->get();
```
