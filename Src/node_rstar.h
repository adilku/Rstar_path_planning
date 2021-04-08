//
// Created by Adilkhan Kussidenov on 4/5/21.
//

#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H

#include <list>
#include <vector>

struct Node_rstar {



    int i, j; //grid cell coordinates
    double F, g, H; //f-, g- and h-values of the search node
    Node_rstar *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)

    std::vector<Node_rstar> * predecessors_nodes;
    std::list<Node_rstar>* path_to_bp;


    Node_rstar() : i(-1), j(-1), F(0), g(0), H(0), parent(nullptr), predecessors_nodes(nullptr) {}

    Node_rstar(int i_, int j_, double F_, double g_, double H_, Node_rstar *parent_)
            : i(i_), j(j_), F(F_), g(g_), H(H_), parent(parent_), predecessors_nodes(nullptr) {}

    //operator for priority_queue
    bool operator<(const Node_rstar &other) const {
        return F < other.F;
    }

    bool operator>(const Node_rstar &other) const {
        return F > other.F;
    }

    Node_rstar& operator=(const Node_rstar &other) = default;

};


#endif //ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
