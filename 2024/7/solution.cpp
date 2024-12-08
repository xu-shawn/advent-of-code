#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
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
struct concatenate {
    static_assert(std::is_integral_v<T>);

    T operator()(const T a, const T b) const {
        T shift = 1;
        while (shift < a)
            shift *= 10;
        return a * shift + b;
    }
};

struct Data {
    std::vector<std::pair<std::uint64_t, std::vector<std::uint64_t>>> data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    using std::size;

    Data                       result;
    std::string                line;
    std::vector<std::uint64_t> resources;

    while (std::getline(file, line))
    {
        std::size_t pos = 0;

        const std::uint64_t target = std::stoull(line, &pos);
        std::uint64_t       temp   = 0;

        for (size_t i = pos + 2; i < size(line); i++)
        {
            if (!std::isdigit(line[i]))
            {
                resources.push_back(temp);
                temp = 0;
            }

            else
            {
                temp *= 10;
                temp += line[i] - '0';
            }
        }

        resources.push_back(temp);

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

    std::int64_t ans = 0;
    for (const auto& row : data.data)
    {
        if (is_possible(row.first, std::cbegin(row.second), std::cend(row.second),
                        std::multiplies<std::uint64_t>{}, std::plus<std::uint64_t>{}))
            ans += row.first;
    }

    std::cout << ans << std::endl;
}

void solve_q2(const Data& data) {
    using std::cbegin;
    using std::cend;

    std::int64_t ans = 0;
    for (const auto& row : data.data)
    {
        if (is_possible(row.first, std::cbegin(row.second), std::cend(row.second),
                        concatenate<std::uint64_t>{}, std::multiplies<std::uint64_t>{},
                        std::plus<std::uint64_t>{}))
            ans += row.first;
    }

    std::cout << ans << std::endl;
}

template<typename Iterator, typename T, typename... Ops>
bool search_for_possible(
  const T target, const T cur, const Iterator next_element, const Iterator end, Ops... ops) {
    if (next_element == end)
        return target == cur;

    if (cur > target)
        return false;

    return (
      search_for_possible(target, ops(cur, *next_element), std::next(next_element), end, ops...)
      || ...);
}

template<typename Iterator, typename T, typename... Ops>
bool is_possible(const T target, const Iterator begin, const Iterator end, Ops... ops) {
    if (begin == end)
        return false;

    static_assert(sizeof...(ops) >= 1);

    return search_for_possible(target, *begin, std::next(begin), end, ops...);
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
