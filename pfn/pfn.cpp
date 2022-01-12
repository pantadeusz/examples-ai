#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

/*
  MATH PORTION OF THE CODE
*/
using point2d = std::array<double, 2>;
inline point2d operator+(const point2d a, const point2d b)
{
    return {a[0] + b[0], a[1] + b[1]};
}
inline point2d operator*(const point2d a, const double b)
{
    return {a[0] * b, a[1] * b};
}
inline point2d operator-(const point2d a, const point2d b)
{
    return a + (b * -1.0);
}
inline point2d operator%(const point2d a, const point2d b)
{
    return {a[0] * b[0], a[1] * b[1]};
}
inline double operator*(const point2d a, const point2d b)
{
    auto r = a % b;
    return std::accumulate(r.begin(), r.end(), 0.0);
}
inline double length(const point2d a)
{
    return std::sqrt(a * a);
}
inline std::ostream& operator<<(std::ostream& o, const point2d a)
{
    o << a[0] << " " << a[1];
    return o;
}

////

point2d derivative(std::function<double(point2d)> f, point2d x, double d = 1.52588e-05)
{
    point2d dx = {d, 0.0};
    point2d dy = {0.0, d};
    return {
        (f(x + dx * 0.5) - f(x - dx * 0.5)) / d,
        (f(x + dy * 0.5) - f(x - dy * 0.5)) / d};
}


int main(int argc, char** argv)
{
    point2d destination = {0.0, 0.0};
    point2d currentPosition = {10.0, 1.0};
    double velocity = 0.1;
    double acceleration = 0.1;

    std::vector<std::pair<point2d, double>> obstacles = {};
    for (double x = 4.6; x < 5.7; x += 0.01) {
        for (double y = -3.6; y < 3.7; y += 0.01) {
            obstacles.push_back({{x,y},(argc>1)?std::stod(argv[1]):1.0});
        }
    }

    auto field = [&](point2d p) -> double {
        double obstaclefield = 0;
        for (const auto& obstacle : obstacles) {
            double distanceToObstacle = length(p - obstacle.first);
            obstaclefield += obstacle.second/ (distanceToObstacle*distanceToObstacle);
        }
        return length(destination - p) + obstaclefield;
    };


//    currentPosition
    point2d currentVelocity = {0.0, 0.0};

    for (int i = 0; i < 1000; i++) {
        point2d dp = derivative(field, currentPosition);
        dp = dp * (1.0 / length(dp));
        dp = dp * acceleration;

        currentVelocity = currentVelocity - dp;
        if (length(currentVelocity) > velocity) currentVelocity = (currentVelocity * (1.0 / (length(currentVelocity))))*velocity;
        currentPosition = currentPosition + currentVelocity;
        std::cout << currentPosition << std::endl;
    }

    return 0;
}