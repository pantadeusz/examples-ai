/*
 A* Algorithm example

 Tadeusz Puźniakowski
 2016, 2017, 2018, 2019, 2020
 MIT License
 */
#include "thirdparty/lodepng.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <unordered_map>
#include <numeric>
#include <queue>
#include <set>
#include <vector>

using std::function;
using std::list;
using std::unordered_map;
using std::max_element;
using std::priority_queue;
using std::remove_if;
using std::reverse;
using std::set;
using std::string;
using std::vector;

/**
 * @brief generate result path
 *
 * @param came_from the dictionary that represents the move directions. key is
 * destination, and value is the source node
 * @param best
 * @param goal
 * @return Container
 */
template <class Node, class Container>
Container reconstructPath(unordered_map<Node, Node>& came_from, Node goal)
{
    Container reconstructed_path;
    Node current = goal;
    reconstructed_path.push_back(current);
    while (came_from.count(current) > 0) {
        current = came_from[current];
        reconstructed_path.push_back(current);
    }
    reverse(reconstructed_path.begin(),
        reconstructed_path.end()); 
    return reconstructed_path;
}

/**
 * @brief calculates shortest path using A* algorithm
 * 
 * @tparam Node the node type
 * @tparam Path the path type. can be for example std::list<Node>
 * @param start the start node 
 * @param goal the goal to reach
 * @param dist distance function
 * @param h heuristic function
 * @param accessible_verts the function that returns accesible nodes from the given node
 * @return Path the generated path
 */
template <class Node, class Path = std::list<Node>>
Path searchPath(
    const Node& start, ///< start point
    const Node& goal,  ///< goal point
    const function<double(const Node&, const Node&)>
        dist,                                           ///< actual distance between adjacent points
    const function<double(const Node&, const Node&)> h, ///< heuristic function
    const function<list<Node>(const Node&)>
        accessible_verts ///< returns accessible vertices
)
{
    set<Node> closedSet;
    unordered_map<Node, Node> came_from;
    unordered_map<Node, double> g_score;
    unordered_map<Node, double> f_score;
    set<Node> openSet;
    openSet.insert(start);
    g_score[start] = 0;                  ///< distance from start
    f_score[start] = 0 + h(start, goal); ///< estimate distancd to goal

    while (openSet.size() > 0) {
        /// searching openSet element with lowest f_score and saving it to "best"
        const Node best = *std::max_element(openSet.begin(), openSet.end(), [&f_score](Node best, Node b) { return f_score[best] > f_score[b]; });
        openSet.erase(best); ///< we took the best, so it is no longer in open set

        if (best == goal)
            return reconstructPath<Node, Path>(came_from, goal);

        closedSet.insert(best);
        /// check every possible direction
        for (const Node& neighbor : accessible_verts(best)) {
            /// not in closed set?
            if (closedSet.count(neighbor) == 0) {
                /// calculate temporary t_g_score that is the sum of g_score of current
                /// node (best) and actual distance between (best-neighbour)
                double t_g_score = g_score[best] + dist(neighbor, best);
                /// if the neighbor does not exist, we assume that it is with inf value
                if ((g_score.count(neighbor) == 0) || (t_g_score < g_score[neighbor])) {
                    came_from[neighbor] = best;
                    g_score[neighbor] = t_g_score;
                    f_score[neighbor] = g_score[neighbor] + h(neighbor, goal);
                }
                /// we should put neighbor to current openSet - it can be evaluated later
                openSet.insert(neighbor);
            }
        }
    }
    return {};
    /// no path found
}

////////////////////////// END OF A*  /////////////////////

/**
 * class representing node in graph. In this particular case it is point_t.
 * */

class point_t : public std::array<int, 2>
{
public:
    point_t(int x = 0, int y = 0) : std::array<int, 2>()
    {
        (*this)[0] = x;
        (*this)[1] = y;
    };
};
namespace std {
/**
 * @brief hash function for point_t
 */
template <>
struct hash<point_t> {
    size_t operator()(const point_t& k) const
    {
        return ((k[0] << sizeof(k[0]) * 4) | (k[0] >> sizeof(k[0]) * 4)) ^ k[1];
    }
};
} // namespace std

/**
 * class representing path
 * */
class path_t : public list<point_t>
{
};

/**
 * we could use some different tile type. Here is just a char that represents
 * tile.
 * */
typedef unsigned char tile_t;
/**
 * helper class for image support. This just wraps lodepng library in more OOP
 * way.
 * */
class image_t : public vector<tile_t>
{
public:
    unsigned int width;
    unsigned int height;
    tile_t& operator()(const point_t& p) { return (*this)[p[1] * width + p[0]]; }
    tile_t operator()(const point_t& p) const
    {
        return (*this)[p[1] * width + p[0]];
    }

    void save(const string& fname)
    {
        image_t image = *this;
        unsigned error =
            lodepng::encode(fname, (unsigned char*)image.data(), image.width,
                image.height, LodePNGColorType::LCT_GREY, 8);
        if (error)
            throw std::runtime_error(lodepng_error_text(error));
    }

    static image_t load(const string& fname)
    {
        image_t image;
        unsigned error = lodepng::decode(image, image.width, image.height, fname,
            LodePNGColorType::LCT_GREY, 8);
        if (error)
            throw std::runtime_error(lodepng_error_text(error));
        return image;
    }
};

int main(int argc, char** argv)
{
    auto img = image_t::load("sample.png");

    point_t start(21, 17);
    point_t goal(211, 132);

    /**
   * function that returns the list of accessible vertices from the current.
   * It takes current coordinates and returns coordinate shifted by the
   * direction. It also checks if we are in the image or outside.
   * */
    const function<list<point_t>(const point_t&)> accessible_verts =
        [&](const point_t& best) -> list<point_t> {
        list<point_t> ret = {{+0, -1}, {-1, -1}, {-1, +0}, {-1, +1},
            {+0, +1}, {+1, +1}, {+1, +0}, {+1, -1}};
        for (auto& e : ret)
            e = {best[0] + e[0], best[1] + e[1]};
        ret.erase(remove_if(ret.begin(), ret.end(),
                      [&img](const point_t& candidate_node) {
                          // remove everything that is incorrect
                          return (candidate_node[0] < 0) ||
                                 (candidate_node[1] < 0) ||
                                 (candidate_node[0] >= img.width) ||
                                 (candidate_node[1] >= img.height) ||
                                 (img(candidate_node) >= 128);
                      }),
            ret.end());
        return ret;
    };
    auto heuristic_f = [&](const point_t& a, const point_t& b) -> double {
        return ::sqrt((a[0] - b[0]) * (a[0] - b[0]) +
                      (a[1] - b[1]) * (a[1] - b[1]));
    };
    auto dist_f = heuristic_f;

    for (auto p : searchPath<point_t>(start, goal, dist_f, heuristic_f,
             accessible_verts)) {
        static int color = 0;
        color = (color + 16) % 128;
        img(p) = 128 + color; //; ///< draw path
    }
    img.save("result.png");
    return 0;
}
