#include "command_line_parser.hpp"

namespace {

std::string spaces(std::size_t n)
{
    std::string result;
    for (int i=0; i<n; ++i)
        result += ' ';
    return result;
}

}


void command_line_parser::print_help(std::ostream& out) const
{
    std::size_t longest_long_name_length=0;
    for (auto&& description : m_description_list)
        longest_long_name_length = std::max(longest_long_name_length, description.long_name.length());

    for (auto&& each : m_description_list)
    {
        auto padding = longest_long_name_length-each.long_name.length();

        out << '-' << each.short_name << ", --" << each.long_name << spaces(padding);
        out << " : " << each.description;

        if (each.option_type == type::required)
            out << " (REQUIRED)";

        out << std::endl;
    }
}

command_line_parser::command_line_parser() = default;
command_line_parser::~command_line_parser() = default;

command_line_parser::abstract_option::~abstract_option()=default;

std::unordered_set<command_line_parser::option_handle> command_line_parser::required_options() const
{
    std::unordered_set<command_line_parser::option_handle> result;

    for (auto&& description : m_description_list)
    {
        if (description.option_type == type::required)
            result.insert(description.option);
    }

    return result;
}

bool command_line_parser::run(int argn, char *argv[])
{
    std::shared_ptr<abstract_option> current;
    auto required = required_options();

    for(int parameter_index=1; parameter_index < argn; ++parameter_index)
    {
        std::string parameter = argv[parameter_index];
        if (parameter.empty())
            continue;

        if (parameter.front() == '-')
        {
            if (parameter.size() < 2)
                throw std::runtime_error("Parameter introducer, but no parameter given");

            if (parameter[1] == '-')
            {
                current = find_long(parameter.substr(2));
            }
            else
            {
                current = find_short(parameter[1]);

                // no space between option and value, apply directly
                if (parameter.size() > 2)
                {
                    current->apply(parameter.substr(2));
                    required.erase(current);
                    current=nullptr;
                }
            }
        }
        else if (current != nullptr)
        {
            current->apply(parameter);
            required.erase(current);
        }
        else
        {
            throw std::runtime_error("Unrecognized parameter: " + parameter);
        }
    }

    if (!required.empty())
    {
        throw std::runtime_error("One or more required options were not set");
    }
}

void command_line_parser::register_option(command_line_parser::option_handle option,
                                          command_line_parser::type option_type,
                                          char short_name, std::string long_name, std::string description)
{
    m_short_name_lookup[short_name] = option;
    m_long_name_lookup[long_name] = option;
    m_description_list.push_back({option_type, short_name, std::move(long_name), std::move(description), std::move(option)});
}

command_line_parser::option_handle command_line_parser::find_short(char short_name)
{
    return m_short_name_lookup.at(short_name);
}

command_line_parser::option_handle command_line_parser::find_long(const std::__cxx11::string &long_name)
{
    return m_long_name_lookup.at(long_name);
}

