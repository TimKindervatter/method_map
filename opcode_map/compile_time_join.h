#pragma once

#include <string_view>
#include <array>

template <std::string_view const&... Strings>
struct join
{
private:
    // Join all strings into a single std::array of chars
    static constexpr auto impl() noexcept
    {
        constexpr std::size_t joined_string_length = (Strings.size() + ... + 0);
        std::array<char, joined_string_length + 1> joined_string_array{};

        auto append = [i = 0, &joined_string_array](auto const& s) mutable 
        {
            for (auto c : s)
            {
                joined_string_array[i++] = c;
            }
        };

        (append(Strings), ...);
        joined_string_array[joined_string_length] = 0;
        return joined_string_array;
    }

    // Give the joined string static storage
    static constexpr auto m_joined_string_array = impl();

public:
    // View as a std::string_view
    static constexpr std::string_view value{ m_joined_string_array.data(), m_joined_string_array.size() - 1 };
};


// Helper to get the value out
template <std::string_view const&... Strings>
static constexpr auto join_v = join<Strings...>::value;