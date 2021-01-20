/*
 * Dijkstra algorithm implementation as an example for students
 * 
 *        Tadeusz Puźniakowski
 * 
 * Compilation
 *  g++ -std=c++0x dijkstra-dot.cpp -o dijkstra
 * 
 * This program uses graphviz for graph generation
 *  sudo apt install graphviz
 * */

#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// this represents infinity
#define INF ((int)(1 << (8 * sizeof(int) - 3)))

// The graph representation - adjacency matrix
typedef vector<vector<int>> graph_t;


/**
 * generates graphviz data from graph. It takes G - graph nad labes - labels.
 */
string g2d(const graph_t& G, const vector<double>& labels = vector<double>(), const int currentNode = -1);

/**
 * this function generates graph and saves it to file. It is used to save steps of calculations in files
 */
void tempStepShow(const graph_t& G, const vector<double>& d, int i, const int currentNode = -1);

/**
 * The dijkstra implementation. G is a graph in 
 */
vector<double> dijkstra(graph_t& G, int initialNode);


void tempStepShow(const graph_t& G, const vector<double>& d, const int i, const int currentNode)
{
    string ofn = "g_" + to_string(i) + ".dot";
    ofstream out(ofn);
    out << g2d(G, d, currentNode);
    out.close();
    system(string("dot -Tpng " + ofn + " -o " + ofn + ".png").c_str());
}


/**
 * initialNode - the initial node
 * G - graph
 * */
vector<double> dijkstra(graph_t& G, int initialNode)
{
    int node;                          ///< current node
    int iterNum = 0;                   ///< steps counter
    vector<double> distG(G[0].size()); ///< loss value for nodes (koszt dotarcia do węzła)
    set<int> unvisited;                ///< unvisited nodes

    for (int i = 0; i < distG.size(); i++) { ///< itinialization, set every node to have INF cost, and to be unvisited
        distG[i] = INF;
        unvisited.insert(i);
    }
    unvisited.erase(initialNode); ///< we visited initial node
    distG[initialNode] = 0;       ///< as well as its cost is 0

    node = initialNode; ///< the first node - we start from it
    /// we only repeat if there are unvisited nodes.
    while (unvisited.size() > 0) {
        tempStepShow(G, distG, iterNum++, node); ///< save current algorithm state
                                                 /// check every unvisited node, update distG (cost) table
        for (auto unvisitedNode : unvisited) {
            /// if the edge (node-unvisitedNode) from node is with less cost then the current best, update:
            if ((distG[unvisitedNode] > G[node][unvisitedNode] + distG[node])) {
                distG[unvisitedNode] = G[node][unvisitedNode] + distG[node];
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
    return distG;
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
 * 0 [ 1   1   0   0  1  ]
 * 1 [ 1   1   1   1  0  ]
 * 2 [ 0   1   1   1  0  ]
 * 3 [ 0   1   1   1  1  ]
 * 4 [ 1   0   0   1  1  ]
 * 
 * */

string g2d(const graph_t& G, const vector<double>& labels, const int currentNode)
{
    std::stringstream ret;
    int w = G[0].size();
    int h = G.size();
    assert(w == h);
    ret << "graph G\n{\noverlap=\"false\";\nsplines=\"true\";\nlayout=sfdp;\nrankdir=LR;\nremincross=\"true\";\n";

    if (labels.size() == h) {
        for (int y = 0; y < h; y++) {
            double v = (labels[y] < INF) ? labels[y] : numeric_limits<double>::infinity();
            if (currentNode == y) {
                ret << y << "[label=\"#v" << y << "(" << v << ")#\"];\n";
            } else {
                ret << y << "[label=\"v" << y << "(" << v << ")\"];\n";
            }
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = y + 1; x < w; x++) {
            if ((G[y][x] > 0) && (G[y][x] < INF)) {
                ret << x << " -- " << y << "[label=\"" << (G[y][x]) << "\",weight=\"" << (G[y][x]) << "\"];\n";
            }
        }
    }
    ret << "}\n";
    return ret.str();
}

int main()
{
    int e = INF;
    graph_t G = {
        {1, 1, e, e, 1},
        {1, 1, 3, 1, e},
        {e, 3, 1, 1, e},
        {e, 1, 1, 1, 1},
        {1, e, e, 1, 1}};
    ofstream out("g.dot");
    //out << g2d(G);
    out << g2d(G, dijkstra(G, 0));
    out.close();
    system("dot -Tpng g.dot -o g.png");

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
