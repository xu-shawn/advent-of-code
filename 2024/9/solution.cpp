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
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct Data;

Data             parse_from(std::fstream&&);
void             solve(Data&&);
void             solve_q1(const Data&);
void             solve_q2(const Data&);
std::vector<int> condense(const std::string&);
std::vector<int> unfragmented_condense(const std::string&);
std::uint64_t    checksum(const std::vector<int>&);

struct Data {
    std::string data;

    Data()                       = default;
    Data(const Data&)            = delete;
    Data& operator=(const Data&) = delete;
    Data(Data&&)                 = default;
    Data& operator=(Data&&)      = default;
};

Data parse_from(std::fstream&& file) {
    using std::size;

    Data result;

    file >> result.data;

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

    std::cout << checksum(condense(data.data)) << std::endl;
}

void solve_q2(const Data& data) {
    using std::cbegin;
    using std::cend;
    using std::size;

    std::cout << checksum(unfragmented_condense(data.data)) << std::endl;
}

std::vector<int> condense(const std::string& disk_map) {
    using std::size;

    bool             free_space    = false;
    int              curr_file_num = 0;
    std::vector<int> disk;

    for (const auto ch : disk_map)
    {
        int len = ch - '0';
        if (free_space)
            for (int i = 0; i < len; i++)
                disk.push_back(-1);
        else
        {
            for (int i = 0; i < len; i++)
                disk.push_back(curr_file_num);

            curr_file_num++;
        }

        free_space = !free_space;
    }

    std::size_t next_free = 0;
    std::size_t next_data = size(disk) - 1;

    while (next_free < size(disk) && disk[next_free] != -1)
    {
        next_free++;
    }

    while (next_free < next_data && disk[next_data] == -1)
    {
        disk.pop_back();
        next_data--;
    }

    while (next_free < next_data)
    {
        std::swap(disk[next_free], disk[next_data]);

        while (next_free < size(disk) && disk[next_free] != -1)
            next_free++;

        while (next_free < next_data && disk[next_data] == -1)
        {
            disk.pop_back();
            next_data--;
        }
    }

    return disk;
}

std::vector<int> unfragmented_condense(const std::string& disk_map) {
    using std::size;

    bool                             free_space    = false;
    int                              curr_file_num = 0;
    std::vector<std::pair<int, int>> labeled_disk_map;

    for (const auto ch : disk_map)
    {
        int len = ch - '0';
        if (free_space)
            labeled_disk_map.push_back(std::make_pair(-1, len));
        else
        {
            labeled_disk_map.push_back(std::make_pair(curr_file_num, len));
            curr_file_num++;
        }

        free_space = !free_space;
    }

    for (int i = curr_file_num - 1; i >= 0; i--)
    {
        int curr_index;

        for (int j = size(labeled_disk_map) - 1; j >= 0; j--)
        {
            if (labeled_disk_map[j].first == i)
            {
                curr_index = j;
                break;
            }
        }

        for (int j = 0; j < curr_index; j++)
        {
            if (labeled_disk_map[j].first == -1
                && labeled_disk_map[j].second >= labeled_disk_map[curr_index].second)
            {
                const auto curr_file = labeled_disk_map[curr_index];
                const auto remaining_space =
                  std::make_pair(-1, labeled_disk_map[j].second - curr_file.second);
                labeled_disk_map[curr_index].first = -1;
                labeled_disk_map.erase(std::next(labeled_disk_map.begin(), j));
                labeled_disk_map.insert(std::next(labeled_disk_map.begin(), j),
                                        {curr_file, remaining_space});
                break;
            }
        }
    }

    std::vector<int> disk;

    for (const auto [label, size] : labeled_disk_map)
    {
        if (label == -1)
            for (int i = 0; i < size; i++)
                disk.push_back(-1);
        else
            for (int i = 0; i < size; i++)
                disk.push_back(label);
    }

    return disk;
}

std::uint64_t checksum(const std::vector<int>& disk) {
    using std::size;

    std::uint64_t sum = 0;

    for (int i = 0; i < size(disk); i++)
    {
        if (disk[i] != -1)
            sum += i * disk[i];
    }

    return sum;
}

int main() { solve(parse_from(std::fstream{"data.txt"})); }
