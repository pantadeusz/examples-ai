#include "distance_t.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
/**
 * Tadeusz Puźniakowski, MIT License, 2021
 * */

using namespace raspigcd;

using p2d = generic_position_t<double, 2>;

auto field_attr = [](auto p, auto dest) {
    return (dest - p).length();
};

auto field_rep = [](auto p, auto dest) {
    return 200.0 / (1.0 + std::sqrt((dest - p).length()));
};

auto calculate_field = [](auto p, auto dest, auto repel) {
    double s = field_attr(p, dest);
    for (auto e : repel) {
        s += field_rep(p, e);
    }
    return s;
};

int main(int argc, char** argv)
{
    p2d destination = {100, 100};
    p2d position = {0, 0};
    std::vector<p2d> moves_available = {
        {0, 1},
        {1, 1},
        {1, 0},
        {1, -1},
        {0, -1},
        {-1, -1},
        {-1, 0},
        {-1, 1}};
    std::vector<p2d> obstacles = {{50, 51}};

    if (argc == 1) {
        for (int i = 0; i < 200; i++) {
            auto mmove = std::min_element(moves_available.begin(),
                moves_available.end(), [&](auto a, auto b) {
                    return (calculate_field(a + position, destination, obstacles) < calculate_field(b + position, destination, obstacles));
                });
            position = position + *mmove;
            std::cout << position[0] << " " << position[1] << " 0" << std::endl;
        }
    } else {
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                position[0] = x;
                position[1] = y;

                std::cout << position[0] << " " << position[1] << " " << calculate_field(position, destination, obstacles) << std::endl;
            }
            std::cout << std::endl;
        }
    }
    return 0;
}