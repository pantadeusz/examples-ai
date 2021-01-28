#include "distance_t.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
/**
 * Tadeusz Puźniakowski, MIT License, 2021
 * */

using namespace raspigcd;

using point2d_t = generic_position_t<double, 2>;

auto calculate_attr = [](auto p, auto dest) {
    return (dest - p).length();
};

auto calculate_rep = [](auto p, auto obstacles) {
    double sum = 0.0;
    for (auto& dest : obstacles) {
        if ((dest - p).length() < 20) {
            sum += 50.0 * (20 - (dest - p).length()) / 20.0;
        }
        //sum += 500.0 / (10 + (dest - p).length());
    }
    return sum;
};

auto calculate_field = [](auto p, auto dest, auto obstacles) {
    return calculate_attr(p, dest) + calculate_rep(p, obstacles);
};

int main(int argc, char** argv)
{
    point2d_t destination = {0, 0};
    point2d_t position = {50, 50};
    std::vector<point2d_t> moves_available = {
        {0, 1},
        {1, 1},
        {1, 0},
        {1, -1},
        {0, -1},
        {-1, -1},
        {-1, 0},
        {-1, 1}};
    std::vector<point2d_t> obstacles = {{20, 25}, {-15, 20}, {-33, -20}};

    if ((argc > 1) && (argv[1][0] == 'f')) {
        for (double x = -60; x < 60; x+=4) {
            for (double y = -60; y < 60; y+=4) {
                position[0] = x;
                position[1] = y;
                std::cout << x << " " << y << " " << calculate_field(position, destination, obstacles) << std::endl;
            }
            std::cout << std::endl;
        }
    } else {
        double errors = 0;
        while (((position - destination).length() > 2) && (errors < 2)) {

            auto nposition = position + *std::max_element(moves_available.begin(), moves_available.end(), [=](auto a, auto b) {
                return calculate_field(position + a, destination, obstacles) > calculate_field(position + b, destination, obstacles);
            });
            if (calculate_field(nposition, destination, obstacles) > calculate_field(position, destination, obstacles)) {
                errors++;
            } else {
                errors *= 0.96;
            }
            position = nposition;
            std::cout << position[0] << " " << position[1] << " " << calculate_field(position, destination, obstacles) << std::endl;
        }
    }

    return 0;
}