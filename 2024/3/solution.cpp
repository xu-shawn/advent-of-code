#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

struct Data;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);
int  evaluate_string(const std::string&);
int  evaluate_instruction(const std::smatch&);

struct Data {
    std::string data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    Data               result;
    std::ostringstream read_stream;
    std::string        line;

    read_stream << "do()";
    while (file >> line)
        read_stream << line;
    read_stream << "don't()";

    result.data = read_stream.str();
    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(const Data& data) { std::cout << evaluate_string(data.data) << std::endl; }

void solve_q2(const Data& data) {
    using std::cbegin;
    using std::cend;

    static const std::regex enabling_regex(R"regex(do\(\).+?don't\(\))regex");

    int ans = 0;

    const auto match_begin =
      std::sregex_iterator(cbegin(data.data), cend(data.data), enabling_regex);
    const auto match_end = std::sregex_iterator();

    for (auto curr_match = match_begin; curr_match != match_end; ++curr_match)
        ans += evaluate_string(curr_match->str());

    std::cout << ans << std::endl;
}

int evaluate_string(const std::string& data) {
    using std::cbegin;
    using std::cend;

    static const std::regex mul_regex(R"regex(mul\((\d+),(\d+)\))regex");

    int ans = 0;

    const auto match_begin = std::sregex_iterator(cbegin(data), cend(data), mul_regex);
    const auto match_end   = std::sregex_iterator();

    for (std::sregex_iterator curr_match = match_begin; curr_match != match_end; ++curr_match)
        ans += evaluate_instruction(*curr_match);

    return ans;
}

int evaluate_instruction(const std::smatch& instruction) {
    return std::stoi(instruction[1]) * std::stoi(instruction[2]);
}

int main() { solve(parse_from(std::fstream("data.txt"))); }
