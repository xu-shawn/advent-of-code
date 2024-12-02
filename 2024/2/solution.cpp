#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <valarray>
#include <vector>

struct Data;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(Data&);
void solve_q2(Data&);
bool is_safe(const std::vector<int>&);
bool is_safe_2(const std::vector<int>&);

struct Data {
    std::vector<std::vector<int>> data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    std::vector<int> line;
    Data             result;
    int              num;

    while (file >> num)
    {
        line.push_back(num);

        if (file.peek() == '\n' || file.eof())
        {
            result.data.push_back(std::move(line));
            line.clear();
        }
    }

    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(Data& data) {
    using std::cbegin;
    using std::cend;

    int ans = 0;

    std::cout << std::reduce(cbegin(data.data), cend(data.data), 0,
                             [](int acc, const std::vector<int> row) { return acc + is_safe(row); })
              << std::endl;
}

void solve_q2(Data& data) {
    using std::cbegin;
    using std::cend;

    int ans = 0;

    std::cout << std::reduce(
      cbegin(data.data), cend(data.data), 0, [](int acc, const std::vector<int> row) {
          return acc + is_safe_2(row);
      }) << std::endl;
}

bool is_safe(const std::vector<int>& row) {
    using std::size;

    int prev_diff = row.at(1) - row.at(0);
    int curr_diff;

    for (int i = 1; i < size(row); i++)
    {
        curr_diff = row[i] - row[i - 1];

        if (curr_diff * prev_diff <= 0)
            return false;

        if (std::abs(curr_diff) < 1 || std::abs(curr_diff) > 3)
            return false;
    }

    return true;
}

bool is_safe_2(const std::vector<int>& row) {
    using std::size;

    if (is_safe(row))
        return true;

    std::vector<int> new_row;

    new_row.reserve(size(row) - 1);

    for (int i = 0; i < size(row); i++)
    {
        for (int j = 0; j < size(row); j++)
        {
            if (j == i)
                continue;

            new_row.push_back(row[j]);
        }

        if (is_safe(new_row))
            return true;

        new_row.clear();
    }

    return false;
}

int main() { solve(parse_from(std::fstream("data.txt"))); }
