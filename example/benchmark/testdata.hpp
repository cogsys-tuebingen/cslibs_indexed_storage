#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <array>

namespace data
{
struct Point
{
    double x;
    double y;
    double z;
    double weight;
};

inline Point getSample(const std::vector<double> &values)
{
    Point s;
    s.x = values[0];
    s.y = values[1];
    s.z = values[2];
    s.weight = values[3];
    return s;
}
template<typename T>
void getLineContent(const std::string &s,
                    std::vector<T> &values)
{
    std::stringstream ss(s);
    T value;
    while(ss >> value) {
        values.push_back(value);
    }
}
using Points = std::vector<Point>;
Points getTestdata(const std::string& file)
{
    static const std::array<double, 3> OFFSETS_X{-10.0, 0.0, 10.0};
    static const std::array<double, 3> OFFSETS_Y{-10.0, 0.0, 10.0};
    static const auto VGA = 640 * 480;
    static const auto IN_DATASET = 1000;
    static const auto OFFSETS = 3 * 3;
    static const auto LOAD_PCT = 0.66;
    static const std::size_t LOAD_FACTOR = (VGA / IN_DATASET / OFFSETS) * LOAD_PCT;
    Points samples;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
    {
        std::vector<double> values;
        getLineContent(line, values);
        Point ref = getSample(values);
        for (int i = 0; i < LOAD_FACTOR; ++i)
            for (double off_x : OFFSETS_X)
                for (double off_y : OFFSETS_Y)
                {
                    Point cur = ref;
                    cur.x += off_x;
                    cur.y += off_y;
                    samples.emplace_back(cur);
                }
    }
    return samples;
}
Points getTestdataSmall(const std::string& file)
{
    static const std::array<double, 2> OFFSETS_X{-10.0, 10.0};
    Points samples;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
    {
        std::vector<double> values;
        getLineContent(line, values);
        Point ref = getSample(values);
        for (double off_x : OFFSETS_X)
        {
            Point cur = ref;
            cur.x += off_x;
            samples.emplace_back(cur);
        }
    }
    return samples;
}
}