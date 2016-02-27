#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class command_line_parser
{
public:
    class abstract_option
    {
    public:
        virtual ~abstract_option();
        virtual void apply(std::string const& rhs) = 0;
    };

    using option_handle = std::shared_ptr<abstract_option>;

    template <typename T>
    class option
    : public abstract_option
    {
    public:
        void apply(std::string const& rhs)
        {
            T value;
            std::istringstream stream(rhs);
            stream >> value;
            m_values.push_back(std::move(value));
        }

        bool defined() const
        {
            return !m_values.empty();
        }

        std::size_t count() const
        {
            return m_values.size();
        }

        T const& get(std::size_t i=0) const
        {
            return m_values.at(i);
        }

        std::vector<T> const& range() const
        {
            return m_values;
        }
    private:
        std::vector<T> m_values;
    };

    enum class type
    {
        optional,
        required
    };

    command_line_parser();
    ~command_line_parser();

    template <typename T>
    std::shared_ptr<option<T> const> add(type option_type, char short_name, std::string long_name, std::string description)
    {
        auto result = std::make_shared<option<T>>();
        register_option(result, option_type, short_name, long_name, description);
        return result;
    }

    bool run(int argn, char* argv[]);

    void print_help(std::ostream& out) const;

private:

    void register_option(option_handle option, type option_type,
                         char short_name, std::string long_name, std::string description);

    std::unordered_set<option_handle> required_options() const;

    struct option_description
    {
        type option_type;
        char short_name;
        std::string long_name;
        std::string description;
        option_handle option;
    };

    option_handle find_short(char short_name);
    option_handle find_long(std::string const& long_name);

    std::unordered_map<char, option_handle> m_short_name_lookup;
    std::unordered_map<std::string, option_handle> m_long_name_lookup;
    std::vector<option_description> m_description_list;
};
