/*
 A* Algorithm example

 Tadeusz Puźniakowski
 2016, 2017, 2018
 MIT License
 */
#include "thirdparty/lodepng.cpp"
#include "thirdparty/lodepng.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <vector>

using std::list;
using std::map;
using std::max_element;
using std::set;
using std::vector;
using std::string;
using std::reverse;
using std::function;
using std::remove_if;

/**
 * class representing node in graph. In this particular case it is point_t.
 * */

class point_t : public std::array<int, 2> {
public:
  point_t() : std::array<int, 2>() {
    (*this)[0] = 0;
    (*this)[1] = 0;
  };
  point_t(int x, int y) : std::array<int, 2>() {
    (*this)[0] = x;
    (*this)[1] = y;
  };
};

/**
 * class representing paht
 * */
class path_t : public list<point_t> {};

/**
 * we could use some different tile type. Here is just a char that represents
 * tile.
 * */
typedef unsigned char tile_t;
/**
 * helper class for image support. This just wraps lodepng library in more OOP
 * way.
 * */
class image_t : public vector<tile_t> {
public:
  unsigned int width;
  unsigned int height;
  tile_t &operator()(const point_t &p) { return (*this)[p[1] * width + p[0]]; }
  tile_t operator()(const point_t &p) const {
    return (*this)[p[1] * width + p[0]];
  }

  void save(const string &fname) {
    image_t image = *this;
    unsigned error =
        lodepng::encode(fname, (unsigned char *)image.data(), image.width,
                        image.height, LodePNGColorType::LCT_GREY, 8);
    if (error)
      throw std::runtime_error(lodepng_error_text(error));
  }

  static image_t load(const string &fname) {
    image_t image;
    unsigned error = lodepng::decode(image, image.width, image.height, fname,
                                     LodePNGColorType::LCT_GREY, 8);
    if (error)
      throw std::runtime_error(lodepng_error_text(error));
    return image;
  }
};

/// generate result path
auto reconstructPath = [](map<point_t, point_t> &came_from, point_t best,
                          point_t goal) -> path_t {
  path_t reconstructed_path;
  point_t current = goal;
  reconstructed_path.push_back(current);
  while (came_from.count(current) > 0) {
    current = came_from[current];
    reconstructed_path.push_back(current);
  }
  reverse(reconstructed_path.begin(),reconstructed_path.end());    // 9 8 7 6 5 4 3 2 1
  return reconstructed_path;
};

path_t searchPath(const point_t &start, ///< start point
                  const point_t &goal,  ///< goal point
                  const function<float(const point_t &, const point_t &)>
                      dist, ///< actual distance between adjacent points
                  const function<float(const point_t &, const point_t &)>
                      h, ///< heuristic function
                  const function<list<point_t>(const point_t &)>
                      accessible_verts ///< returns accessible vertices
) {
  set<point_t> closedSet;
  set<point_t> openSet;
  map<point_t, point_t> came_from;
  map<point_t, float> g_score;
  map<point_t, float> f_score;

  openSet.insert(start);
  g_score[start] = 0;                  ///< distance from start
  f_score[start] = 0 + h(start, goal); ///< estimate distancd to goal

  while (openSet.size() > 0) {
    /// searching openSet element with lowest f_score and saving it to "best"
    point_t best = *max_element(
        openSet.begin(), openSet.end(),
        [&f_score](point_t best, point_t b) { return f_score[best] > f_score[b]; });
    openSet.erase(best); ///< we took the best, so it is no longer in open set
    if (best == goal)
      return reconstructPath(came_from, best, goal);
    closedSet.insert(best);
    /// check every possible direction
    for (const point_t &neighbor : accessible_verts(best)) {
      /// not in closed set?
      if (closedSet.count(neighbor) == 0) {
        /// calculate temporary t_g_score that is the sum of g_score of current
        /// node (best) and actual distance between (best-neighbour)
        float t_g_score = g_score[best] + dist(neighbor, best);
        /// we should put neighbor to current openSet - it can be evaluated
        /// later
        openSet.insert(neighbor);
        /// if the neighbor does not exist, we assume that it is with inf value
        if ((g_score.count(neighbor) == 0) || (t_g_score < g_score[neighbor])) {
          came_from[neighbor] = best;
          g_score[neighbor] = t_g_score;
          f_score[neighbor] = g_score[neighbor] + h(neighbor, goal);
        }
      }
    }
  }
  return {}; /// no path found
}

int main(int argc, char **argv) {
  auto img = image_t::load("sample.png");

  point_t start(21, 17);
  point_t goal(211, 132);

/**
 * function that returns the list of accessible vertices from the current.
 * It takes current coordinates and returns coordinate shifted by the direction. It also
 * checks if we are in the image or outside.
 * */
  const function<list<point_t>(const point_t &)> accessible_verts =
      [&](const point_t &best) -> list<point_t> {
    list<point_t> ret = {{+0, -1}, {-1, -1}, {-1, +0}, {-1, +1},
                              {+0, +1}, {+1, +1}, {+1, +0}, {+1, -1}};
    for (auto &e : ret)
      e = {best[0] + e[0], best[1] + e[1]};
    ret.erase(remove_if(ret.begin(), ret.end(),
                             [&img](const point_t &candidate_node) {
                               return (candidate_node[0] < 0) ||
                                      (candidate_node[1] < 0) ||
                                      (candidate_node[0] >= img.width) ||
                                      (candidate_node[1] >= img.height) ||
                                      (img(candidate_node) >=
                                       128); // put your condition here
                             }),
              ret.end());
    return ret;
  };
  auto heuristic_f = [&](const point_t &a, const point_t &b) -> float {
    return ::sqrt((a[0] - b[0]) * (a[0] - b[0]) +
                  (a[1] - b[1]) * (a[1] - b[1]));
  };
  auto dist_f = heuristic_f;

  for (auto p :
       searchPath(start, goal, dist_f, heuristic_f, accessible_verts)) {
    img(p) = 128; ///< draw path
  }
  img.save("result.png");
  return 0;
}
