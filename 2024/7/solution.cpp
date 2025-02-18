#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Data;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);

template<typename Iterator, typename T, typename... Ops>
bool is_possible(const T, Iterator, Iterator, Ops...);

template<typename Iterator, typename T, typename... Ops>
bool search_for_possible(const T, const T, const Iterator, const Iterator, Ops...);

template<typename T>
constexpr T power(const T base, const std::size_t exponent);

template<typename T>
constexpr std::size_t num_digits(T num);

template<typename T, std::size_t... Is>
constexpr std::array<T, sizeof...(Is)>
make_power_sequence(T base, std::integer_sequence<std::size_t, Is...>);

template<typename T>
constexpr T power(const T base, const std::size_t exponent) {
    if (exponent == 0)
        return static_cast<T>(1);

    if (exponent == 1)
        return base;

    return power(base, exponent / 2) * power(base, exponent / 2) * power(base, exponent % 2);
}

template<typename T>
constexpr std::size_t num_digits(T num) {
    std::size_t counter = 0;

    while (num)
    {
        num /= 10;
        counter++;
    }

    return std::max<std::size_t>(counter, 1);
}

template<typename T, std::size_t... Is>
constexpr std::array<T, sizeof...(Is)>
make_power_sequence(T base, std::integer_sequence<std::size_t, Is...>) {
    return {power(base, Is)...};
}

template<typename T>
struct Extended {
    static_assert(std::is_integral_v<T>);
    T           value;
    std::size_t num_digits;

    Extended(const T v, const std::size_t nd) :
        value(v),
        num_digits(nd) {}

    explicit operator T() const noexcept { return value; }
};

template<typename T>
struct extended_plus {
    static_assert(std::is_integral_v<T>);

    T operator()(const T a, const T b) const noexcept { return a + b; }
    T operator()(const T a, const Extended<T> b) const noexcept { return a + static_cast<T>(b); }
};


template<typename T>
struct extended_multiplies {
    static_assert(std::is_integral_v<T>);

    T operator()(const T a, const T b) const noexcept { return a * b; }
    T operator()(const T a, const Extended<T> b) const noexcept { return a * static_cast<T>(b); }
};


template<typename T>
struct extended_concatenate {
    static_assert(std::is_integral_v<T>);

    static constexpr std::array<T, num_digits(std::numeric_limits<T>::max())> LOOKUP =
      make_power_sequence<T>(
        10, std::make_integer_sequence<std::size_t, num_digits(std::numeric_limits<T>::max())>());

    T operator()(const T a, const T b) const noexcept { return a * LOOKUP[num_digits(b)] + b; }
    T operator()(const T a, const Extended<T> b) const noexcept {
        return a * LOOKUP[b.num_digits] + static_cast<T>(b);
    }
};

struct Data {
    std::vector<std::pair<std::uint64_t, std::vector<Extended<std::uint64_t>>>> data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    using std::size;

    Data                                 result;
    std::string                          line;
    std::vector<Extended<std::uint64_t>> resources;

    while (std::getline(file, line))
    {
        std::size_t pos = 0;

        const std::uint64_t target = std::stoull(line, &pos);
        std::uint64_t       temp   = 0;
        std::size_t         len    = 0;

        for (size_t i = pos + 2; i < size(line); i++)
        {
            if (!std::isdigit(line[i]))
            {
                resources.emplace_back(temp, len);
                temp = len = 0;
            }

            else
            {
                temp *= 10;
                temp += line[i] - '0';
                len++;
            }
        }

        resources.emplace_back(temp, len);
        result.data.emplace_back(target, std::move(resources));
        resources.clear();
    }

    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(const Data& data) {
    using std::cbegin;
    using std::cend;

    std::uint64_t ans = 0;
    for (const auto& row : data.data)
        if (is_possible(row.first, cbegin(row.second), cend(row.second),
                        extended_multiplies<std::uint64_t>{}, extended_plus<std::uint64_t>{}))
            ans += row.first;

    std::cout << ans << std::endl;
}

void solve_q2(const Data& data) {
    using std::cbegin;
    using std::cend;

    std::uint64_t ans = 0;
    for (const auto& row : data.data)
        if (is_possible(row.first, cbegin(row.second), cend(row.second),
                        extended_concatenate<std::uint64_t>{}, extended_multiplies<std::uint64_t>{},
                        extended_plus<std::uint64_t>{}))
            ans += row.first;

    std::cout << ans << std::endl;
}

template<typename Iterator, typename T, typename... Ops>
bool search_for_possible(
  const T target, const T cur, const Iterator next_element, const Iterator end, Ops... ops) {
    if (cur > target)
        return false;

    if (next_element == end)
        return target == cur;

    return (
      search_for_possible(target, ops(cur, *next_element), std::next(next_element), end, ops...)
      || ...);
}

template<typename Iterator, typename T, typename... Ops>
bool is_possible(const T target, const Iterator begin, const Iterator end, Ops... ops) {
    if (begin == end)
        return false;

    static_assert(sizeof...(ops) >= 1);

    return search_for_possible(target, static_cast<T>(*begin), std::next(begin), end, ops...);
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
