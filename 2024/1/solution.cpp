#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <unordered_map>

struct parse_result_type;

parse_result_type parse_from_file(std::fstream&&);
void              calculate(parse_result_type&&);
void              solve_q1(parse_result_type&);
void              solve_q2(parse_result_type&);

struct parse_result_type {
    std::vector<int> row1;
    std::vector<int> row2;

    parse_result_type()                                    = default;
    parse_result_type(const parse_result_type&)            = delete;
    parse_result_type& operator=(const parse_result_type&) = delete;
    parse_result_type(parse_result_type&&)                 = default;
    parse_result_type& operator=(parse_result_type&&)      = default;
};

parse_result_type parse_from_file(std::fstream&& file) {
    parse_result_type res;
    int               a, b;

    while (file >> a >> b)
    {
        res.row1.push_back(a);
        res.row2.push_back(b);
    }

    return res;
}

void calculate(parse_result_type&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(parse_result_type& data) {
    using std::begin;
    using std::end;
    using std::size;

    std::sort(begin(data.row1), end(data.row1));
    std::sort(begin(data.row2), end(data.row2));

    int ans = 0;

    for (std::size_t i = 0; i < size(data.row1); i++)
        ans += std::abs(data.row1[i] - data.row2[i]);

    std::cout << ans << std::endl;
}

void solve_q2(parse_result_type& data) {
    std::unordered_map<int, int> counter;
    int                          ans = 0;

    for (const auto& x : data.row2)
        counter[x]++;

    for (const auto& x : data.row1)
        ans += counter[x] * x;

    std::cout << ans << std::endl;
}

int main() { calculate(parse_from_file(std::fstream("data.txt"))); }
