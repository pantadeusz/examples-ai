/*
 * Dijkstra algorithm implementation as an example for students
 * 
 *        Tadeusz Puźniakowski
 * 
 * Compilation
 *  g++ -std=c++0x dijkstra-path.cpp -o dijkstra
 * 
 * */

#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

// this represents infinity
#define INF ((int)(1 << (8 * sizeof(int) - 3)))

/**
 *  The graph representation - adjacency matrix
 * */
typedef vector<vector<int>> graph_t;

/**
 * The dijkstra implementation. G is a graph in 
 */
pair<vector<double>, map<int, int>> dijkstra(graph_t& G, int initialNode);

/**
 * initialNode - the initial node
 * G - graph
 * */
pair<vector<double>, map<int, int>> dijkstra(graph_t& G, int initialNode)
{
    int node;                               ///< current node
    int iterNum = 0;                        ///< steps counter
    vector<double> distG(G[0].size(), INF); ///< loss value for nodes (koszt dotarcia do węzła)
    set<int> unvisited;                     ///< unvisited nodes
    map<int, int> prev;

    for (int i = 0; i < distG.size(); i++) { ///< itinialization, set every node to have INF cost, and to be unvisited
        unvisited.insert(i);
    }
    unvisited.erase(initialNode); ///< we visited initial node
    distG[initialNode] = 0;       ///< as well as its cost is 0
    prev[initialNode] = -1;       ///< initial node have no previously visited

    node = initialNode; ///< the first node - we start from it
    /// we only repeat if there are unvisited nodes.
    while (unvisited.size() > 0) {
        /// check every unvisited node, update distG (cost) table
        for (auto unvisitedNode : unvisited) {
            /// if the edge (node-unvisitedNode) from node is with less cost then the current best, update:
            if ((distG[unvisitedNode] > G[node][unvisitedNode] + distG[node])) {
                distG[unvisitedNode] = G[node][unvisitedNode] + distG[node];
                prev[unvisitedNode] = node;
            }
        }
        int nnode = -1;                        ///< nnode - the node with least cost value from node
        for (auto unvisitedNode : unvisited) { ///< check every unvisited node, update distG (cost) table, and find shortest uptil now (nnode)
            if (nnode == -1) nnode = unvisitedNode;
            /// if the cost is better (lower), then we update nnode
            if (distG[unvisitedNode] < distG[nnode]) {
                nnode = unvisitedNode;
            }
        }
        unvisited.erase(node); ///< we will not come back to node
        node = nnode;          ///< we go to the node with least cost
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

int main()
{
    int e = INF;
    graph_t G = {
        {e, 1, e, e, 1},
        {1, e, 3, 1, e},
        {e, 3, e, 1, e},
        {e, 1, 1, e, 1},
        {1, e, e, 1, e}};
    for (const auto row : G) {
        for (const auto e : row) {
            cout << ((e < INF) ? to_string(e) : " ") << ",";
        }
        cout << endl;
    }
    cout << endl;
    for (int firstV = 0; firstV < (G[0].size()); firstV++) {
        cout << "costs to reach vertice from " << firstV << " are:" << endl;
        auto path = dijkstra(G, firstV);
        for (const auto& e : path.first) {
            cout << e << " ";
        }
        cout << endl;
        cout << "the 'came from' for every vertice leading to " << firstV << ":" << endl;
        for (auto e : path.second) {
            std::cout << "{" << e.first << " : " << e.second << " } " << std::endl;
        }
        int current=3;
        while (current != -1) {
            std::cout << current << " ";
            current = path.second.at(current);
        }
        std::cout << std::endl;
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
