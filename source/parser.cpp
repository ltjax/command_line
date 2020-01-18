#include "command_line/parser.hpp"
#include <algorithm>

using namespace command_line;

namespace {

std::string spaces(std::size_t n)
{
    return std::string(n, ' ');
}

void throw_takes_no_value(std::string const& name)
{
    throw malformed("Option \"--" + name + "\" does not take a value.");
}

}


void parser::print_help(std::ostream& out) const
{
    std::size_t longest_long_name_length=0;
    for (auto&& description : m_description_list)
        longest_long_name_length = std::max(longest_long_name_length, description.long_name.length());

    for (auto&& each : m_description_list)
    {
        auto padding = longest_long_name_length-each.long_name.length();

        out << '-' << each.short_name << ", --" << each.long_name << spaces(padding);
        out << " : " << each.description;

        if (each.option_type == requirement::mandatory)
            out << " (REQUIRED)";

        out << std::endl;
    }
}

parser::parser() = default;
parser::~parser() = default;

abstract_option::~abstract_option()=default;

std::unordered_set<option_handle> parser::required_options() const
{
    std::unordered_set<option_handle> result;

    for (auto&& description : m_description_list)
    {
        if (description.option_type == requirement::mandatory)
            result.insert(description.option);
    }

    return result;
}

void parser::run(std::vector<std::string> parameters)
{
    std::shared_ptr<abstract_option> current;
    auto required = required_options();

    for (auto const& parameter : parameters)
    {
        if (parameter.empty())
            continue;

        current = process(current, parameter, required);
    }

    if (!required.empty())
    {
        throw missing_required("One or more required options were not set");
    }
}

void parser::run(int argn, char *argv[])
{
    std::vector<std::string> parameters;

    for(int parameter_index=1; parameter_index < argn; ++parameter_index)
    {
        parameters.push_back(argv[parameter_index]);
    }

    run(std::move(parameters));
}

option_handle parser::recognize(option_handle current)
{
    if (current && !current->recognize())
    {
        current.reset();
    }

    return current;
}

option_handle parser::process(option_handle current, std::string const& parameter, std::unordered_set<option_handle>& required)
{
    if (parameter.front()=='-')
    {
        if (parameter.size()<2)
            throw malformed("Option introducer, but no name given");

        if (parameter[1]=='-')
        {
            auto content = parameter.substr(2);
            auto position = content.find("=");
            if (position != std::string::npos)
            {
                // name and value separated by equals
                auto name = content.substr(0, position);
                auto value = content.substr(position + 1);
                current = recognize(find_long(name));
                if (current == nullptr)
                {
                    throw_takes_no_value(name);
                }
                current->apply(value);
                required.erase(current);
                current = nullptr;
            }
            else
            {
                // name and value separated by space
                current = recognize(find_long(content));
            }

        }
        else
        {
            current=recognize(find_short(parameter[1]));

            // no space between option and value, apply directly
            if (parameter.size()>2)
            {
                if (current == nullptr)
                {
                    throw_takes_no_value({ parameter[1] });
                }
                current->apply(parameter.substr(2));
                required.erase(current);
                current=nullptr;
            }
        }
    }
    else if (current!=nullptr)
    {
        current->apply(parameter);
        required.erase(current);
    }
    else
    {
        throw malformed("Unrecognized option: " + parameter);
    }

    return current;
}

void parser::register_option(option_handle option,
                             requirement option_type,
                             char short_name, std::string long_name, std::string description)
{
    m_short_name_lookup[short_name] = option;
    m_long_name_lookup[long_name] = option;
    m_description_list.push_back({option_type, short_name, std::move(long_name), std::move(description), std::move(option)});
}

option_handle parser::find_short(char short_name)
{
    auto found = m_short_name_lookup.find(short_name);
    return found != m_short_name_lookup.end() ? found->second : nullptr;
}

option_handle parser::find_long(std::string const& long_name)
{
    auto found = m_long_name_lookup.find(long_name);
    return found != m_long_name_lookup.end() ? found->second : nullptr;
}

