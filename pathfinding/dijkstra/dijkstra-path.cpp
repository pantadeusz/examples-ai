/*
 * Dijkstra algorithm implementation as an example for students
 * 
 *        Tadeusz Puźniakowski
 * 
 * Compilation
 *  g++ -std=c++0x dijkstra-path.cpp -o dijkstra
 * 
 * */

#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <fstream>
#include <limits>
#include <set>
#include <tuple>
#include <map>
#include <string>

using namespace std;

// this represents infinity
#define INF 0

/**
 *  The graph representation - adjacency matrix
 * */
typedef vector < vector < int > > graph_t;

/**
 * The dijkstra implementation. G is a graph in 
 */
pair<vector < double >, map< int, int > >  dijkstra( graph_t &G , int initialNode );

/**
 * initialNode - the initial node
 * G - graph
 * */
pair<vector < double >, map< int, int > > dijkstra( graph_t &G , int initialNode ) {
	int node; ///< current node
	int iterNum=0; 	///< steps counter
	vector < double > distG(G[0].size()); ///< loss value for nodes (koszt dotarcia do węzła)
	set<int> unvisited;  ///< unvisited nodes
	map<int,int> prev;
	
	for (int i = 0; i < distG.size(); i++) { ///< itinialization, set every node to have INF cost, and to be unvisited
		distG[i] = INF;
		unvisited.insert(i);
	}
    unvisited.erase(initialNode); ///< we visited initial node
	distG[initialNode] = 0;   ///< as well as its cost is 0
	prev[initialNode] = -1; ///< initial node have no previously visited

	node = initialNode; ///< the first node - we start from it
	/// we only repeat if there are unvisited nodes.
	while (unvisited.size() > 0) {
            int nnode = -1; ///< nnode - the node with least cost value from node
            for (auto unvisitedNode : unvisited) { ///< check every unvisited node, update distG (cost) table, and find shortest uptil now (nnode)
				if (G[node][unvisitedNode] != INF) { ///< we only take into account possible routes - there is an edge between node and unvisitedNode
					/// if the edge (node-unvisitedNode) from node is with less cost then the current best, update:
					if ((distG[unvisitedNode] == INF) || (distG[unvisitedNode] > G[node][unvisitedNode] + distG[node])) {
						distG[unvisitedNode] = G[node][unvisitedNode] + distG[node];
						prev[unvisitedNode] = node;
					}
					if (nnode == -1) nnode = unvisitedNode;
					/// if the cost is better (lower), then we update nnode
					if ((distG[unvisitedNode] < distG[nnode]) || (distG[nnode] == INF)) {
						nnode = unvisitedNode;
					}
				}
            }
            unvisited.erase(node); ///< we will not come back to node
            node = nnode; ///< we go to the node with least cost
	};
	/// return cost table
	return {distG, prev};
}

/*
 * (Adjacency matrix - macierz sąsiedztwa)
 * 
 * Przykład reprezentacji grafu
 * 
 *       0  ---  4
 *     /       /  
 *   1 ----- 3
 *     \   /
 *       2
 * 
 *     0   1   2   3  4
 * 0 [ 0   1   0   0  1  ]
 * 1 [ 1   0   1   1  0  ]
 * 2 [ 0   1   0   1  0  ]
 * 3 [ 0   1   1   0  1  ]
 * 4 [ 1   0   0   1  0  ]
 * 
 * */

int main() {
	int e = INF;
	graph_t G = {
			{ e, 3, e, e, 1 }, 
			{ 3, e, 3, 1, e }, 
			{ e, 3, e, 1, e }, 
			{ e, 1, 1, e, 1 }, 
			{ 1, e, e, 1, e } 
			} ;
	for (const auto row: G) {
		for (const auto e : row) {
			cout << e << ",";
		}
		cout << endl;
	}
	cout << endl;
	cout << "costs to reach vertice from 0 are:" << endl;
    auto path = dijkstra( G , 0 );
    for (const auto &e: path.first) {
		cout << e << " ";
	}
	cout << endl;
	cout << "the 'came from' for every vertice leading to 0:" << endl;
	for (auto e: path.second) {
		std::cout << "{" << e.first << " : " << e.second << " } " << std::endl;
	}
    
	
	return 0;
}



/*graph_t G = {{e,1,e,1,1,e,e,1,e,e,e,1,1,e},
              {1,e,e,1,1,1,e,1,e,e,e,1,e,e},
              {e,e,e,1,1,1,e,1,e,1,1,1,1,e},
              {1,1,1,e,e,e,1,e,1,1,e,e,e,1},
              {1,1,1,e,e,e,e,e,e,1,e,e,e,e},
              {e,1,1,e,e,e,e,e,e,e,e,e,e,e},
              {e,e,e,1,e,e,e,e,e,1,e,e,e,1},
              {1,1,1,e,e,e,e,e,1,1,e,e,e,e},
              {e,e,e,1,e,e,e,1,e,e,e,1,e,e},
              {e,e,1,1,1,e,1,1,e,e,e,e,e,e},
              {e,e,1,e,e,e,e,e,e,e,e,e,e,e},
              {1,1,1,e,e,e,e,e,1,e,e,e,e,e},
              {1,e,1,e,e,e,e,e,e,e,e,e,e,e},
              {e,e,e,1,e,e,1,e,e,e,e,e,e,e}}; */
