#include <array>
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <vector>

struct Data;
struct Direction;
struct Position;
struct PositionHasher;
struct DirectedPositon;
struct DirectedPositonHasher;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);
std::unordered_set<Position, PositionHasher>
     simulate_walk(const std::vector<std::vector<char>>& map, Position curr_position);
bool will_halt(const std::vector<std::vector<char>>&, Position, const Position);

constexpr Position  operator+(const Position, const Direction) noexcept;
constexpr Position& operator+=(Position&, const Direction) noexcept;
constexpr bool      operator==(const Position rhs, const Position lhs) noexcept;

constexpr Direction operator+(const Direction, const Direction) noexcept;
constexpr bool      operator==(const Direction rhs, const Direction lhs) noexcept;

struct Direction {
    int x;
    int y;

    constexpr Direction(const int a, const int b) :
        x(a),
        y(b) {}
};

constexpr Direction operator+(const Direction first, const Direction second) noexcept {
    return Direction(first.x + second.x, first.y + second.y);
}

constexpr bool operator==(const Direction lhs, const Direction rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr Direction N{-1, 0};
constexpr Direction S{1, 0};
constexpr Direction E{0, -1};
constexpr Direction W{0, 1};

constexpr std::array<Direction, 4> ALL_DIRECTIONS{N, S, E, W};

struct Position {
    int x;
    int y;
};

struct PositionHasher {
    constexpr PositionHasher(size_t w) :
        width(w) {}

    constexpr size_t operator()(const Position& pos) const { return pos.x * width + pos.y; }

   private:
    size_t width;
};

struct DirectedPosition {
    Position  pos;
    Direction dir;
};

struct DirectedPositionHasher {
    constexpr DirectedPositionHasher(size_t w, size_t h) :
        width(w),
        height(h) {}

    constexpr size_t operator()(const DirectedPosition& dpos) const {
        int dir_index = 0;

        if (dpos.dir == N)
            dir_index = 0;

        else if (dpos.dir == E)
            dir_index = 1;

        else if (dpos.dir == S)
            dir_index = 2;

        else if (dpos.dir == W)
            dir_index = 3;

        return dir_index * width * height + dpos.pos.x * width + dpos.pos.y;
    }

   private:
    size_t width;
    size_t height;
};

constexpr bool operator==(const Position lhs, const Position rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr Position operator+(const Position pos, const Direction direction) noexcept {
    return Position{pos.x + direction.x, pos.y + direction.y};
}

constexpr Position& operator+=(Position& pos, const Direction direction) noexcept {
    pos.x += direction.x;
    pos.y += direction.y;
    return pos;
}

constexpr bool operator==(const DirectedPosition lhs, const DirectedPosition rhs) noexcept {
    return lhs.pos == rhs.pos && lhs.dir == rhs.dir;
}

constexpr Direction clockwise(const Direction dir) noexcept { return Direction{dir.y, -dir.x}; }

struct Data {
    std::vector<std::vector<char>> map;
    Position                       start_position;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    using std::begin;
    using std::end;
    using std::cbegin;
    using std::cend;

    Data        result;
    std::string line;

    while (file >> line)
    {
        result.map.emplace_back(cbegin(line), cend(line));
        auto start_loc = std::find(begin(result.map.back()), end(result.map.back()), '^');

        if (start_loc != end(result.map.back()))
        {
            result.start_position =
              Position{static_cast<int>(size(result.map) - 1),
                       static_cast<int>(std::distance(begin(result.map.back()), start_loc))};

            *start_loc = '.';
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
    std::cout << size(simulate_walk(data.map, data.start_position)) << std::endl;
}

void solve_q2(const Data& data) {
    using std::size;

    int ans = 0;

    auto relevant_positions = simulate_walk(data.map, data.start_position);
    relevant_positions.erase(data.start_position);

    for (const auto blocker : relevant_positions)
    {
        if (!will_halt(data.map, data.start_position, blocker))
            ans++;
    }

    std::cout << ans << std::endl;
}

std::unordered_set<Position, PositionHasher>
simulate_walk(const std::vector<std::vector<char>>& map, Position curr_position) {
    using std::size;

    Direction curr_direction = N;

    std::unordered_set<Position, PositionHasher> visited{11, PositionHasher(size(map[0]))};

    while (true)
    {
        const Position next_position = curr_position + curr_direction;

        visited.insert(curr_position);

        if (next_position.x < 0 || next_position.y < 0 || next_position.x >= size(map)
            || next_position.y >= size(map[0]))
            return visited;

        if (map[next_position.x][next_position.y] == '#')
        {
            curr_direction = clockwise(curr_direction);
            continue;
        }

        curr_position += curr_direction;
    }
}

bool will_halt(const std::vector<std::vector<char>>& map,
               Position                              curr_position,
               const Position                        blocker) {
    using std::size;

    Direction                                                    curr_direction = N;
    std::unordered_set<DirectedPosition, DirectedPositionHasher> lookup{
      11, DirectedPositionHasher(size(map[0]), size(map))};
    std::unordered_set<Position, PositionHasher> visited{11, PositionHasher(size(map[0]))};

    while (true)
    {
        if (lookup.find(DirectedPosition{curr_position, curr_direction}) != lookup.end())
            return false;

        lookup.insert(DirectedPosition{curr_position, curr_direction});

        const Position next_position = curr_position + curr_direction;

        if (next_position.x < 0 || next_position.y < 0 || next_position.x >= size(map)
            || next_position.y >= size(map[next_position.x]))
            return true;

        if (map[next_position.x][next_position.y] == '#' || next_position == blocker)
        {
            curr_direction = clockwise(curr_direction);
            continue;
        }

        curr_position += curr_direction;
    }
}


int main() { solve(parse_from(std::fstream{"data.txt"})); }
