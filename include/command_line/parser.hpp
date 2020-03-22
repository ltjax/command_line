#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>

namespace command_line
{


class exception : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class malformed : public exception
{
public:
    using exception::exception;
};

class missing_required : public exception
{
public:
    using exception::exception;
};

class option_index_out_of_bounds : public exception
{
public:
    using exception::exception;
};

template <typename T>
struct converter
{
    T operator()(std::string const& rhs) const
    {
        T value;
        std::istringstream stream(rhs);
        stream>>value;
        if (stream.fail())
        {
            throw malformed("Value " + rhs + " cannot be converted to " + typeid(T).name());
        }
        return value;
    }
};

template <>
struct converter<std::string>
{
    std::string operator()(std::string const& rhs) const
    {
        return rhs;
    }
};

class abstract_option
{
public:
    virtual ~abstract_option();
    virtual void apply(std::string const& rhs)=0;
    virtual bool recognize()=0;
};

using option_handle=std::shared_ptr<abstract_option>;

template <typename T>
class option
    : public abstract_option
{
public:
    option() = default;

    option(option const&) = delete;
    option(option&&) = delete;
    void operator=(option const&) = delete;
    void operator=(option&&) = delete;

    template <typename P>
    explicit option(P&& default_value)
    : m_default({std::forward<P>(default_value)})
    {
    }

    void apply(std::string const& rhs) override
    {
        m_values.push_back(std::move(converter<T>()(rhs)));
    }

    bool recognize() override
    {
        return true;
    }

    [[nodiscard]] bool defined() const
    {
        return !m_values.empty() || m_default;
    }

    [[nodiscard]] std::size_t count() const
    {
        return m_values.size();
    }

    T const& get(std::size_t i=0) const
    {
        if (m_default.empty())
        {
            if (i >= m_values.size())
            {
                throw option_index_out_of_bounds("Out of bounds index " + std::to_string(i) + " to option with "
                    + std::to_string(m_values.size()) + " values.");
            }
            return m_values[i];
        }

        return (i<m_values.size()) ? m_values[i] : m_default.front();
    }

    std::vector<T> const& range() const
    {
        if (m_values.empty())
          return m_default;
        return m_values;
    }
private:
    std::vector<T> m_values;
    std::vector<T> m_default; // Only every carries 0 or 1 elements
};

template <>
class option<void>
    : public abstract_option
{
public:
    void apply(std::string const& /*rhs*/) override
    {
    }

    bool recognize() override
    {
        ++m_count;
        return false;
    }

    [[nodiscard]] std::size_t count() const
    {
        return m_count;
    }

private:
    std::size_t m_count=0;

};

class parser
{
public:

    parser();
    ~parser();

    /** Add a mandatory parameter.
        There will be an error in run() if a mandatory option was not specified.
    */
    template <typename T>
    std::shared_ptr<option<T> const> mandatory(char short_name, std::string long_name, std::string description)
    {
        return add_option<T>(std::make_shared<option<T>>(), requirement::mandatory, short_name,
                std::move(long_name), std::move(description));
    }

    /** Add an optional parameter with a default.
    */
    template <typename T, typename P>
    std::shared_ptr<option<T> const> optional(char short_name, std::string long_name, std::string description, P&& default_value)
    {
        // Append information about the defaults
        std::ostringstream str;
        str<<" (default="<<default_value<<")";
        description += str.str();

        return add_option<T>(std::make_shared<option<T>>(std::forward<P>(default_value)), requirement::optional, short_name,
                std::move(long_name), std::move(description));
    }

    /** Add an optional parameter without a default.
    */
    template <typename T>
    std::shared_ptr<option<T> const> optional(char short_name, std::string long_name, std::string description)
    {
        return add_option<T>(std::make_shared<option<T>>(), requirement::optional, short_name, long_name, description);
    }

    void run(int argn, char* argv[]);
    void run(std::vector<std::string> parameters);

    void print_help(std::ostream& out) const;

private:
    enum class requirement
    {
        optional,
        mandatory
    };

    template <typename T>
    std::shared_ptr<option<T> const> add_option(std::shared_ptr<option<T>> which, requirement option_type,
            char short_name, std::string long_name, std::string description)
    {
        register_option(which, option_type, short_name, std::move(long_name), std::move(description));
        return which;
    }

    void register_option(option_handle option, requirement option_type,
                         char short_name, std::string long_name, std::string description);

    option_handle process(option_handle current, std::string const& parameter,
                         std::unordered_set<option_handle>& required);

    std::unordered_set<option_handle> required_options() const;

    option_handle recognize(option_handle current);

    struct option_description
    {
        requirement option_type;
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

}