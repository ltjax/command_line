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
    SECTION("long form, separated")
    {
        parser.run({ "--thestring", "actual" });
    }
    SECTION("long form, joined")
    {
        parser.run({ "--thestring=actual" });
    }

    REQUIRE(param->get() == "actual");
}

TEST_CASE("Unknown long option is malformed")
{
    command_line::parser parser;
    REQUIRE_THROWS_AS(parser.run({ "--unknown", "value" }), command_line::malformed);
}

TEST_CASE("Unknown short option is malformed")
{
    command_line::parser parser;
    REQUIRE_THROWS_AS(parser.run({ "-uvalue" }), command_line::malformed);
}

TEST_CASE("Missing required value throws")
{
    command_line::parser parser;
    auto param = parser.mandatory<std::string>('r', "required", "required description");
    REQUIRE_THROWS_AS(parser.run({}), command_line::missing_required);
}

TEST_CASE("Integer parameter")
{
    command_line::parser parser;
    auto param = parser.optional<std::uint32_t>('i', "number", "number description");
    SECTION("can be parsed")
    {
        parser.run({ "--number", "123456" });
        REQUIRE(param->get() == 123456);
    }
    SECTION("throws on invalid")
    {
        REQUIRE_THROWS_AS(parser.run({ "--number=thisisnotanumber" }), command_line::malformed);
    }
    SECTION("access to non-existant throws")
    {
        parser.run({});
        REQUIRE_THROWS_AS(param->get(), command_line::option_index_out_of_bounds);
    }
}

TEST_CASE("Retrieve default value")
{
    command_line::parser parser;
    auto param = parser.optional<std::string>('a', "args", "description", "this_is_the_default");
    parser.run({});
    SECTION("using get()")
    {
        REQUIRE(param->get() == "this_is_the_default");
    }
    SECTION("using range()")
    {
        REQUIRE(param->range() == std::vector<std::string>{"this_is_the_default"});
    }
}

TEST_CASE("Use only long names")
{
    command_line::parser parser;
    auto param = parser.optional<std::string>("long_name", "description", "this_is_the_default");
    parser.run({"--long_name=this_is_the_value"});
    REQUIRE(param->get() == "this_is_the_value");
}

TEST_CASE("Optional without default is not defined")
{
    command_line::parser parser;
    auto param = parser.optional<std::string>("long_name", "description");
    parser.run({});
    REQUIRE_FALSE(param->defined());
}

TEST_CASE("Boolean parameter")
{
    command_line::parser parser;
    auto toggle = parser.optional<bool>("some_boolean", "description", false);

    SECTION("Default can be read")
    {
        parser.run({});
        REQUIRE(toggle->get() == false);
    }
    SECTION("Can be set false via command line")
    {
        parser.run({"--some_boolean=false"});
        REQUIRE(toggle->get() == false);
    }
    SECTION("Can be set true via command line")
    {
        parser.run({"--some_boolean=true"});
        REQUIRE(toggle->get() == true);
    }
    SECTION("Prints help as false, not 0")
    {
        using Catch::Matchers::Contains;
        std::ostringstream str;
        parser.print_help(str);
        REQUIRE_THAT(str.str(), Contains("default=false"));
    }
}
