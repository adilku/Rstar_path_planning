//
// Created by Adilkhan Kussidenov on 4/5/21.
//

#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H

#include <list>
#include <vector>
#include <iostream> //debug

struct Node_rstar {



    int i, j; //grid cell coordinates
    int avoid;
    double F, g, H; //f-, g- and h-values of the search node
    Node_rstar *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)

    std::vector<std::pair<Node_rstar, double>>* predecessors_nodes; // predecessors_nodes with C_low(path_s'_s)
    std::list<Node>* path_to_bp;
    double C_low;


    Node_rstar() : i(-1), j(-1), avoid(0), F(0), g(0), H(0), parent(nullptr),
    predecessors_nodes(new std::vector<std::pair<Node_rstar, double>>) {}

    Node_rstar(int i_, int j_, int avoid_, double F_, double g_, double H_, Node_rstar *parent_,
               std::vector<std::pair<Node_rstar, double>> *predecessors_nodes_)
            : i(i_), j(j_), avoid(avoid_), F(F_), g(g_), H(H_), parent(parent_), predecessors_nodes(predecessors_nodes_) {}

    //operator for priority_queue
    bool operator<(const Node_rstar &other) const {
        return std::tuple(avoid, F, g, i, j) < std::tuple(other.avoid, other.F, other.g, other.i, other.j);
    }

    bool operator>= (const Node_rstar &other) const {
        std::cout << "debug in node_rstar.h " << avoid << ' ' << other.avoid << " | " << F << ' ' << other.F << '\n';
        return std::tuple(avoid, F) >= std::tuple(other.avoid, other.F);
    }

    Node_rstar& operator=(const Node_rstar &other) = default;

};


#endif //ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
