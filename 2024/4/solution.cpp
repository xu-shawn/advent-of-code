#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

struct Data;
struct Direction;
struct Position;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);
int  search_all_matches(const std::vector<std::string>&, const std::string&, const Position);
bool cross_xmas_match(const std::vector<std::string>&, const Position);
bool match_success(const std::vector<std::string>&, const std::string&, Position, const Direction);

constexpr Position operator+(const Position, const Direction) noexcept;
Position&          operator+=(Position&, const Direction) noexcept;

struct Direction {
    const int x;
    const int y;

    constexpr Direction(const int a, const int b) :
        x(a),
        y(b) {}
};

constexpr Direction operator+(const Direction& first, const Direction& second) {
    return Direction(first.x + second.x, first.y + second.y);
}

constexpr Direction N{1, 0};
constexpr Direction S{-1, 0};
constexpr Direction E{0, 1};
constexpr Direction W{0, -1};

constexpr Direction NE = N + E;
constexpr Direction NW = N + W;
constexpr Direction SE = S + E;
constexpr Direction SW = S + W;

constexpr std::array<Direction, 8> ALL_DIRECTIONS{N, S, E, W, NE, NW, SE, SW};

struct Position {
    int x;
    int y;
};

constexpr Position operator+(const Position pos, const Direction direction) noexcept {
    return Position{pos.x + direction.x, pos.y + direction.y};
}

Position& operator+=(Position& pos, const Direction direction) noexcept {
    pos.x += direction.x;
    pos.y += direction.y;
    return pos;
}

struct Data {
    std::vector<std::string> data;

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
        result.data.push_back(std::move(line));
    }

    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(const Data& data) {
    using std::size;

    int ans = 0;

    for (int x = 0; x < size(data.data); x++)
        for (int y = 0; y < size(data.data[0]); y++)
            ans += search_all_matches(data.data, "XMAS", Position{x, y});

    std::cout << ans << std::endl;
}

void solve_q2(const Data& data) {
    using std::size;

    int ans = 0;

    for (int x = 1; x + 1 < size(data.data); x++)
        for (int y = 1; y + 1 < size(data.data[0]); y++)
            ans += cross_xmas_match(data.data, Position{x, y});

    std::cout << ans << std::endl;
}

int search_all_matches(const std::vector<std::string>& map,
                       const std::string&              to_search,
                       const Position                  position) {
    using std::size;

    int ans = 0;

    for (const Direction curr_direction : ALL_DIRECTIONS)
        ans += match_success(map, to_search, position, curr_direction);

    return ans;
}

bool cross_xmas_match(const std::vector<std::string>& map, const Position position) {
    const auto [x, y] = position;

    if (x < 1 || y < 1 || x + 1 >= size(map) || y + 1 >= size(map.at(0)))
        return false;

    return (match_success(map, "MAS", Position{x + 1, y + 1}, SW)
            || match_success(map, "SAM", Position{x + 1, y + 1}, SW))
        && (match_success(map, "MAS", Position{x - 1, y + 1}, NW)
            || match_success(map, "SAM", Position{x - 1, y + 1}, NW));
}

bool match_success(const std::vector<std::string>& map,
                   const std::string&              to_search,
                   Position                        position,
                   const Direction                 direction) {
    using std::size;

    for (const char character : to_search)
    {
        if (position.x < 0 || position.y < 0 || position.x >= size(map)
            || position.y >= size(map.at(0)))
            return false;

        if (map[position.x][position.y] != character)
            return false;

        position += direction;
    }

    return true;
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
