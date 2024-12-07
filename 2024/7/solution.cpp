#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

struct Data;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);
template<typename Iterator, typename T, typename... Ops>
bool is_possible(const T&, Iterator, Iterator, Ops...);
template<typename Iterator, typename T, typename... Ops>
bool search_for_possible(const T&, const T&, const Iterator, const Iterator, Ops...);

struct Concatenate {
    std::uint64_t operator()(uint64_t a, uint64_t b) const {
        std::string s = std::to_string(a) + std::to_string(b);
        return std::stoull(s);
    }
};

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result{};

    std::istringstream stream{str};

    for (std::string token{}; std::getline(stream, token, delim);)
    {
        if (token.empty())
            continue;

        result.push_back(token);
    }

    return result;
}

struct Data {
    std::vector<std::pair<std::uint64_t, std::vector<std::uint64_t>>> data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    Data        result;
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string>   tokens = split(line, ' ');
        std::vector<std::uint64_t> resources;

        tokens[0].pop_back();
        const std::int64_t target = std::stoull(tokens[0]);

        for (int i = 1; i < size(tokens); i++)
            resources.push_back(std::stoull(tokens[i]));

        result.data.emplace_back(target, std::move(resources));
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
                        std::plus<std::uint64_t>{}, std::multiplies<std::uint64_t>{}))
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
                        std::plus<std::uint64_t>{}, std::multiplies<std::uint64_t>{},
                        Concatenate{}))
            ans += row.first;
    }

    std::cout << ans << std::endl;
}

template<typename Iterator, typename T, typename... Ops>
bool search_for_possible(
  const T& target, const T& cur, const Iterator next_element, const Iterator end, Ops... ops) {
    if (next_element == end)
    {
        return target == cur;
    }

    if (cur > target)
    {
        return false;
    }

    return (
      search_for_possible(target, ops(cur, *next_element), std::next(next_element), end, ops...)
      || ...);
}

template<typename Iterator, typename T, typename... Ops>
bool is_possible(const T& target, const Iterator begin, const Iterator end, Ops... ops) {
    if (begin == end)
    {
        return false;
    }

    static_assert(sizeof...(ops) >= 1);

    return search_for_possible(target, *begin, std::next(begin), end, ops...);
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
