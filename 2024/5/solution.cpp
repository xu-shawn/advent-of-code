#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct Data;

Data             parse_from(std::fstream&&);
void             solve(Data&&);
void             solve_q1(const Data&);
void             solve_q2(const Data&);
bool             valid_order(const std::vector<int>::const_iterator,
                             const std::vector<int>::const_iterator,
                             const std::unordered_map<int, std::vector<int>>&);
bool             elem_in_valid_order(const std::vector<int>::const_iterator,
                                     const std::vector<int>::const_iterator,
                                     const std::unordered_map<int, std::vector<int>>&);
std::vector<int> valid_reordering(const std::vector<int>::const_iterator,
                                  const std::vector<int>::const_iterator,
                                  const std::unordered_map<int, std::vector<int>>&);

struct Data {
    std::unordered_map<int, std::vector<int>> dependency;
    std::vector<std::vector<int>>             queries;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    Data        result;
    std::string line;

    while (file >> line)
    {
        if (line.find('|') != std::string::npos)
        {
            int dependency = std::stoi(line.substr(0, 2));
            int dependent  = std::stoi(line.substr(3, 5));

            result.dependency[dependent].push_back(dependency);
        }

        else if (line.find(',') != std::string::npos)
        {
            std::vector<int> this_query;
            std::size_t      pos;
            std::string      token;

            while ((pos = line.find(',')) != std::string::npos)
            {
                token = line.substr(0, pos);
                this_query.push_back(std::stoi(token));
                line.erase(0, pos + 1);
            }

            this_query.push_back(std::stoi(line));

            result.queries.push_back(std::move(this_query));
        }
    }

    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(const Data& data) {
    using std::size;
    using std::cbegin;
    using std::cend;

    int ans = 0;

    for (const auto& query : data.queries)
        if (valid_order(cbegin(query), cend(query), data.dependency))
            ans += query.at(size(query) / 2);

    std::cout << ans << std::endl;
}

void solve_q2(const Data& data) {
    using std::size;
    using std::cbegin;
    using std::cend;

    int ans = 0;

    for (const auto& query : data.queries)
        if (!valid_order(cbegin(query), cend(query), data.dependency))
            ans +=
              valid_reordering(cbegin(query), cend(query), data.dependency).at(size(query) / 2);

    std::cout << ans << std::endl;
}

bool valid_order(const std::vector<int>::const_iterator           it_begin,
                 const std::vector<int>::const_iterator           it_end,
                 const std::unordered_map<int, std::vector<int>>& dependency) {
    for (auto it = it_begin; it != it_end; ++it)
        if (!elem_in_valid_order(it, it_end, dependency))
            return false;

    return true;
}

bool elem_in_valid_order(const std::vector<int>::const_iterator           curr,
                         const std::vector<int>::const_iterator           end,
                         const std::unordered_map<int, std::vector<int>>& dependency) {
    if (dependency.find(*curr) == dependency.end())
        return true;

    for (const auto dependency_item : dependency.at(*curr))
        if (std::find(curr, end, dependency_item) != end)
            return false;

    return true;
}

std::vector<int> valid_reordering(const std::vector<int>::const_iterator           begin,
                                  const std::vector<int>::const_iterator           end,
                                  const std::unordered_map<int, std::vector<int>>& dependencies) {
    using std::cbegin;
    using std::cend;

    std::unordered_set<int> to_process{begin, end};
    std::vector<int>        new_query;

    while (!to_process.empty())
    {
        for (const auto ele : to_process)
        {
            auto dependency = dependencies.find(ele);

            if (dependency == dependencies.end()
                || std::all_of(
                  cbegin(dependency->second), cend(dependency->second),
                  [&to_process](int value) { return to_process.find(value) == cend(to_process); }))
            {
                new_query.push_back(ele);
                to_process.erase(ele);
                break;
            }
        }
    }

    return new_query;
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
