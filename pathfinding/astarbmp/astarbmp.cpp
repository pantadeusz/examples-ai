/*
 A* Algorithm example

 Tadeusz Puźniakowski
 2016, 2017, 2018
 MIT License
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

class point_t : public std::array<int, 2> {
public:
	point_t() : std::array<int, 2>() {
		( *this )[0] = 0;
		( *this )[1] = 0;
	};
	point_t( int x, int y ) : std::array<int, 2>() {
		( *this )[0] = x;
		( *this )[1] = y;
	};
};
namespace std {
template <>
struct hash<point_t> {
	size_t operator()( const point_t& p ) const {
		return ( ( p[0] << 12 ) ^ p[1] );
	}
};
}

class path_t : public std::list< point_t > {
};

class image_t : public vector < tile_t > {
public:
	unsigned int width;
	unsigned int height;
	tile_t &operator()( const point_t &p ) {
		return ( *this )[p[1] * width + p[0]];
	}
	tile_t operator()( const point_t &p ) const {
		return ( *this )[p[1] * width + p[0]];
	}
};

path_t searchPath (
	const point_t &start, ///< start point
	const point_t &goal, ///< goal point
	const std::function < float ( const point_t &, const point_t & )> dist, ///< heuristic function
	const std::function < float ( const point_t &, const point_t & )> h, ///< heuristic function
	const std::function < std::list < point_t > ( const point_t & )> accessible_verts ///< returns accessible vertices
) {
	set<point_t > closedSet;
	set<point_t > openSet;
	map<point_t, point_t > came_from;
	map<point_t, float > g_score;
	map<point_t, float > f_score;


	openSet.insert( start );
	g_score[start] = 0;
	f_score[start] = 0 + h( start, goal );

	/// generate result path
	auto reconstructPath = [&]( point_t best )->path_t {
		path_t path;
		path_t pathInvert;
		point_t current = goal;
		pathInvert.push_back( current );
		while ( came_from.count( current ) > 0 ) {
			current = came_from[current];
			pathInvert.push_back( current );
		}
		for ( ; pathInvert.size() > 0; pathInvert.pop_back() ) {
			path.push_back( pathInvert.back() );
		}
		return path;
	};

	auto popBestFromOpenSet = [&]() {
		point_t best = *openSet.begin();
		for ( auto &b : openSet ) {
			if ( f_score[best] > f_score[b] ) best = b;
		}
		openSet.erase( best );
		return best;
	};

	while ( openSet.size() > 0 ) {
		/// searching openSet element with lowest f_score and saving it to "best"
		point_t best = popBestFromOpenSet();
		if ( best == goal ) return reconstructPath( best );
		closedSet.insert( best );
		/// check every possible direction
		for ( const point_t & neighbor : accessible_verts( best ) ) {
			/// not in closed set?
			if (  closedSet.count( neighbor ) == 0 )  {
				/// calculate temporary t_g_score that is the sum of g_score of current node (best) 
				// and actual distance between (best-toCheck)
				float t_g_score = g_score[best] + dist( neighbor, best );
				/// we should put neighbor to current openSet - it can be evaluated later
				openSet.insert( neighbor );
				/// if the neighbor does not exist, we assume that it is with inf value
				if ( (g_score.count( neighbor ) == 0) || (t_g_score < g_score[neighbor]) ) {
					came_from[neighbor] = best;
					g_score[neighbor] = t_g_score;
					f_score[neighbor] = g_score[neighbor] + h( neighbor, goal );
				}
			}
		}
	}
	return {}; /// no path found
}



void savePng( const std::string & fname, image_t image ) {
	unsigned error = lodepng::encode( fname, ( unsigned char * )image.data(), image.width, image.height, LodePNGColorType::LCT_GREY, 8 );
	if( error ) throw std::runtime_error( lodepng_error_text( error ) );
}

image_t loadPng( const std::string & fname ) {
	image_t image;
	unsigned error = lodepng::decode( image, image.width, image.height, fname, LodePNGColorType::LCT_GREY, 8 );
	if( error ) throw std::runtime_error( lodepng_error_text( error ) );
	return image;
}

int main ( int argc, char **argv ) {
	image_t img = loadPng ( "sample.png" );

	point_t start( 21, 17 );
	point_t goal( 211, 132 );

	int pnext [][3] = {
		{+0, -1, 0},
		{-1, -1, 0},
		{-1, +0, 0},
		{-1, +1, 0},
		{+0, +1, 0},
		{+1, +1, 0},
		{+1, +0, 0},
		{+1, -1, 0}
	};
	const std::function < std::list < point_t > ( const point_t & )> accessible_verts  = [&]( const point_t & best ) -> std::list < point_t > {
		std::list < point_t > ret;
		for ( int i = 0; i < 8; i++ ) {
			point_t toCheck( best[0] + pnext[i][0], best[1] + pnext[i][1] );
			/// can we go there?
			if ( ( toCheck[0] >= 0 ) && ( toCheck[1] >= 0 ) && ( toCheck[0] < img.width ) && ( toCheck[1] < img.height ) ) {
				/// is it ok to walk on it, and it is not in closedSet?
				if ( img( toCheck ) < 128 ) {
					ret.push_back( toCheck );
				}
			}
		}
		return ret;

	};
	auto heuristic_f = [&]( const point_t &a, const point_t &b )->float {
		return ::sqrt( ( a[0] - b[0] ) * ( a[0] - b[0] ) + ( a[1] - b[1] ) * ( a[1] - b[1] ) );
	};
	auto dist_f = [&]( const point_t &a, const point_t &b )->float {
		return ::sqrt( ( a[0] - b[0] ) * ( a[0] - b[0] ) + ( a[1] - b[1] ) * ( a[1] - b[1] ) );
	};
	path_t foundPath = searchPath ( start, goal, dist_f, heuristic_f, accessible_verts );
	for ( auto &p : foundPath ) {
		img( p ) = 128; ///< draw path
	}
	savePng( "result.png", img );
	return 0;
}
