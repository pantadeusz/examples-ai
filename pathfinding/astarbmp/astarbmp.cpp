/*
 A* Algorithm example
 
 Tadeusz Puźniakowski
 2016

 */
#include "thirdparty/lodepng.h"
#include "thirdparty/lodepng.cpp"
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <array>
#include <cmath>
#include <functional>
#include <numeric>

using std::vector;
using std::list;
using std::set;
using std::map;

typedef unsigned char tile_t;

class point_t : public std::array<int,2> {
	public:
	point_t() : std::array<int,2>() {(*this)[0] = 0; (*this)[1] = 0;};
	point_t(int x, int y) : std::array<int,2>() {(*this)[0] = x; (*this)[1] = y;};
};
namespace std {
	template <>
	struct hash<point_t> {
		size_t operator()( const point_t& p ) const {
			return ((p[0] << 12) ^ p[1]);
		}
	};
}

class path_t : public std::list< point_t > {
};

class image_t : public vector < tile_t > {
	public:
	unsigned int width;
	unsigned int height;
	tile_t &operator()(const point_t &p) {
		return (*this)[p[1]*width+p[0]];
	}
	tile_t operator()(const point_t &p) const {
		return (*this)[p[1]*width+p[0]];
	}
};

path_t searchPath (
		const point_t &start, ///< start point
		const point_t &goal, ///< goal point
		const image_t &world,  ///< the map (actually it is a bitmap)
		int limitD,  ///< the maximal depth of path, or -1
		const std::function < bool (const tile_t &) > accessible, ///< function determining if the point is "walkable"
		const std::function < float (const point_t &,const point_t &)> h ///< heuristic function
	) {
	path_t path;
	set<point_t > closedSet;
	set<point_t > openSet;
	map<point_t, point_t > came_from;
	map<point_t, float > g_score;
	map<point_t, float > f_score;
	int pnext [][3]= {
			{+0,-1,0},
			{-1,-1,0},
			{-1,+0,0},
			{-1,+1,0},
			{+0,+1,0},
			{+1,+1,0},
			{+1,+0,0},
			{+1,-1,0}
			};

	openSet.insert(start);
	g_score[start] = 0;
	f_score[start] = 0 + h(start, goal);

	while (openSet.size() > 0) {
		/// searching openSet element with lowest f_score and saving it to "best"
		point_t toCheck;
		point_t best = *openSet.begin();
		for (auto &b : openSet) {
				if (f_score[best] > f_score[b]) best = b;
		}
		if (best == goal) {
			path_t pathInvert;
			point_t current = goal;
			pathInvert.push_back(current);
			while (came_from.count(current) > 0) {
				current = came_from[current];
				pathInvert.push_back(current);
			}
			for (; pathInvert.size() > 0; pathInvert.pop_back()) {
				path.push_back(pathInvert.back());
			}
			return path;
		}

		openSet.erase(best);
		closedSet.insert(best);
		//if (openSet.size() > 300) return vector<point_t>(); // we can limit open set, so to terminate if it is greater than given 
		if ((limitD > 0) && (f_score[best] > limitD)) return path_t();

		/// check every possible direction
		for (int i = 0; i < 8; i++) {
			toCheck[0] = best[0] + pnext[i][0];
			toCheck[1] = best[1] + pnext[i][1];
			/// can we go there?
			if ((toCheck[0] >= 0) && (toCheck[1] >= 0) && (toCheck[0] < world.width) && (toCheck[1] < world.height)) {
				/// is it ok to walk on it, and it is not in closedSet?
				if (accessible(world(toCheck)) && (closedSet.count(toCheck) == 0)) {
					/// calculate temporary t_g_score that is the sum of g_score of current node (best) and actual distance between (best-toCheck)
					float t_g_score = g_score[best] + h(toCheck,best);
					/// we should put toCheck to current openSet - it can be evaluated later
					openSet.insert(toCheck);
					/// if the toCheck is newly added, then set verry high score to it
					if (g_score.count(toCheck) == 0) g_score[toCheck] = 9999999;
					if (t_g_score < g_score[toCheck]) {
						came_from[toCheck] = best;
						g_score[toCheck] = t_g_score;
						f_score[toCheck] = g_score[toCheck] + h(toCheck, goal);
					}
				}
			}
		}
	}

	return path;
}

void savePng( const std::string &fname, image_t image ) {
	unsigned error = lodepng::encode( fname, ( unsigned char * )image.data(), image.width, image.height, LodePNGColorType::LCT_GREY, 8 );
	if( error ) throw std::runtime_error( lodepng_error_text( error ) );
}

image_t loadPng( const std::string &fname ) {
	image_t image;
	unsigned error = lodepng::decode( image, image.width, image.height, fname, LodePNGColorType::LCT_GREY, 8 );
	if( error ) throw std::runtime_error( lodepng_error_text( error ) );
	return image;
}

int main (int argc, char **argv) {
	image_t img = loadPng ("sample.png");

		point_t start(21,17); 
		point_t goal(211,132);

path_t foundPath = searchPath (
		start, 
		goal,
		img, -1, 
		[&](const tile_t &p)->bool { return (p < 128); },
		[&](const point_t &a,const point_t &b)->float {
			return ::sqrt((a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]));
		}
);
	for (auto &p : foundPath) {
		img(p) = 128;
	}
	savePng("result.png",img);
	return 0;
}
