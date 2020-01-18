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
}

