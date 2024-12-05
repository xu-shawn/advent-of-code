#include <array>
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
bool             invalid_order(const std::vector<int>::const_iterator&,
                               const std::unordered_map<int, std::vector<int>>&,
                               const std::vector<int>&);
std::vector<int> valid_reordering(const std::unordered_map<int, std::vector<int>>&,
                                  const std::vector<int>&);

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
            size_t           pos;
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
    using std::end;

    int ans = 0;

    for (const auto& query : data.queries)
    {
        bool success = true;

        for (auto it = cbegin(query); it != cend(query); ++it)
        {
            if (invalid_order(it, data.dependency, query))
            {
                success = false;
                break;
            }
        }

        if (success)
            ans += query.at(size(query) / 2);
    }

    std::cout << ans << std::endl;
}

void solve_q2(const Data& data) {
    using std::size;
    using std::cbegin;
    using std::end;

    int ans = 0;

    for (const auto& query : data.queries)
    {
        bool success = true;

        for (auto it = cbegin(query); it != cend(query); ++it)
        {
            if (invalid_order(it, data.dependency, query))
            {
                success = false;
                break;
            }
        }

        if (!success)
            ans += valid_reordering(data.dependency, query).at(size(query) / 2);
    }

    std::cout << ans << std::endl;
}

bool invalid_order(const std::vector<int>::const_iterator&          it,
                   const std::unordered_map<int, std::vector<int>>& dependency,
                   const std::vector<int>&                          query) {

    if (dependency.find(*it) == dependency.end())
        return false;

    for (const auto dependency_item : dependency.at(*it))
    {
        if (std::find(it, cend(query), dependency_item) != cend(query))
            return true;
    }

    return false;
}

std::vector<int> valid_reordering(const std::unordered_map<int, std::vector<int>>& dependencies,
                                  const std::vector<int>&                          query) {
    std::unordered_set<int> to_process;
    std::vector<int>        new_query;

    for (const auto ele : query)
        to_process.insert(ele);

    while (!to_process.empty())
    {
        for (const auto ele : to_process)
        {
            if (dependencies.find(ele) == dependencies.end())
            {
                new_query.push_back(ele);
                to_process.erase(ele);
                break;
            }

            bool can_add = true;

            for (const auto dependency : dependencies.at(ele))
            {
                if (to_process.find(dependency) != to_process.end())
                {
                    can_add = false;
                    break;
                }
            }

            if (can_add)
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
