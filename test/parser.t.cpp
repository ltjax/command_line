#include <catch2/catch.hpp>
#include <command_line/parser.hpp>

TEST_CASE("Can parse an optional string parameter")
{
    command_line::parser parser;
    auto param = parser.optional<std::string>('s', "thestring", "just a test");

    SECTION("short form, joined")
    {
        parser.run({ "-sactual" });
    }
    SECTION("short form, separated")
    {
        parser.run({ "-s", "actual" });
    }
    SECTION("long form")
    {
        parser.run({ "--thestring", "actual" });
    }

    REQUIRE(param->get() == "actual");
}
