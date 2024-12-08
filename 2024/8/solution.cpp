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
struct Position;
struct Direction;
struct PositionHasher;
struct IAntinodeGenerator;

Data parse_from(std::fstream&&);
void solve(Data&&);
void solve_q1(const Data&);
void solve_q2(const Data&);

constexpr Position  operator+(const Position, const Direction) noexcept;
constexpr Position& operator+=(Position&, const Direction) noexcept;
constexpr bool      operator==(const Position, const Position) noexcept;

constexpr Direction operator-(const Position, const Position) noexcept;

constexpr bool in_bounds(const Position, std::size_t, std::size_t) noexcept;

void insert_antinodes(std::unordered_set<Position, PositionHasher>&,
                      const std::vector<Position>&,
                      const IAntinodeGenerator&);

struct Direction {
    int x;
    int y;

    constexpr Direction(const int a, const int b) :
        x(a),
        y(b) {}
};


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

constexpr Position& operator-=(Position& pos, const Direction direction) noexcept {
    pos.x -= direction.x;
    pos.y -= direction.y;
    return pos;
}

constexpr Direction operator-(const Position left, const Position right) noexcept {
    return Direction{left.x - right.x, left.y - right.y};
}

struct Data {
    std::unordered_map<char, std::vector<Position>> antennas;
    std::size_t                                     width;
    std::size_t                                     height;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

struct IAntinodeGenerator {
    virtual void insert_antinodes(std::unordered_set<Position, PositionHasher>&,
                                  const Position,
                                  const Position) const = 0;
    virtual ~IAntinodeGenerator()                       = default;
};

struct EquidistantAntinodeGenerator: IAntinodeGenerator {
    EquidistantAntinodeGenerator(std::size_t w, std::size_t h) :
        width(w),
        height(h) {}
    virtual void insert_antinodes(std::unordered_set<Position, PositionHasher>&,
                                  const Position,
                                  const Position) const final;
    virtual ~EquidistantAntinodeGenerator() = default;

   private:
    std::size_t width;
    std::size_t height;
};

struct ColinearAntinodeGenerator: IAntinodeGenerator {
    ColinearAntinodeGenerator(std::size_t w, std::size_t h) :
        width(w),
        height(h) {}
    virtual void insert_antinodes(std::unordered_set<Position, PositionHasher>&,
                                  const Position,
                                  const Position) const final;
    virtual ~ColinearAntinodeGenerator() = default;

   private:
    std::size_t width;
    std::size_t height;
};

constexpr bool in_bounds(const Position pos, std::size_t height, std::size_t width) noexcept {
    return pos.x >= 0 && pos.y >= 0 && pos.x < height && pos.y < width;
}

void EquidistantAntinodeGenerator::insert_antinodes(
  std::unordered_set<Position, PositionHasher>& container,
  const Position                                a,
  const Position                                b) const {
    Position tmp = a + (a - b);
    if (in_bounds(tmp, height, width))
        container.insert(tmp);
    tmp = b + (b - a);
    if (in_bounds(tmp, height, width))
        container.insert(b + (b - a));
}

void ColinearAntinodeGenerator::insert_antinodes(
  std::unordered_set<Position, PositionHasher>& container,
  const Position                                a,
  const Position                                b) const {
    Direction dir = a - b;
    Position  tmp = a;
    while (in_bounds(tmp, height, width))
    {
        container.insert(tmp);
        tmp += dir;
    }
    tmp = b;
    while (in_bounds(tmp, height, width))
    {
        container.insert(tmp);
        tmp -= dir;
    }
}


Data parse_from(std::fstream&& file) {
    using std::size;

    Data        result;
    std::string line;
    int         line_count = 0;

    while (file >> line)
    {
        for (int i = 0; i < size(line); i++)
        {
            if (line[i] == '.')
                continue;

            result.antennas[line[i]].emplace_back(Position{line_count, i});
        }

        result.width = size(line);
        line_count++;
    }

    result.height = line_count;

    return result;
}

void solve(Data&& data) {
    solve_q1(data);
    solve_q2(data);
}

void solve_q1(const Data& data) {
    using std::cbegin;
    using std::cend;
    using std::size;

    std::unordered_set<Position, PositionHasher> antinodes{0, PositionHasher{data.width}};

    for (const auto& frequency : data.antennas)
        insert_antinodes(antinodes, frequency.second,
                         EquidistantAntinodeGenerator{data.width, data.height});

    std::cout << size(antinodes) << std::endl;
}

void solve_q2(const Data& data) {
    using std::cbegin;
    using std::cend;
    using std::size;

    std::unordered_set<Position, PositionHasher> antinodes{0, PositionHasher{data.width}};

    for (const auto& frequency : data.antennas)
        insert_antinodes(antinodes, frequency.second,
                         ColinearAntinodeGenerator{data.width, data.height});

    std::cout << size(antinodes) << std::endl;
}

void insert_antinodes(std::unordered_set<Position, PositionHasher>& to_insert,
                      const std::vector<Position>&                  antennas,
                      const IAntinodeGenerator&                     gen) {

    for (int i = 0; i < size(antennas); i++)
        for (int j = i + 1; j < size(antennas); j++)
            gen.insert_antinodes(to_insert, antennas[i], antennas[j]);
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
