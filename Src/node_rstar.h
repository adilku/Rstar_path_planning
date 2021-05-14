//
// Created by Adilkhan Kussidenov on 4/5/21.
//

#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H

#include <list>
#include <utility>
#include <vector>
#include <iostream> //debug
#include <memory>
#include <map>
#include "node.h"

struct NodeRstar {

    int i, j; //grid cell coordinates
    int avoid;
    double F, g; //f-, g- and h-values of the search node
    NodeRstar * bp; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)
    std::shared_ptr<std::map<double, NodeRstar> > predecessors_nodes; // predecessors_nodes with C_low(path_s'_s)
    std::shared_ptr<std::list<Node>> path_to_bp; // path tp backpointer
    double C_low; // 'edge' to bp

    NodeRstar() {
        i = -1;
        j = -1;
        avoid = 0;
        F = 0;
        g = 0;
        bp = nullptr;
        predecessors_nodes = std::make_shared<std::map<double, NodeRstar> >();
        path_to_bp = nullptr;
        C_low = 0;
    }

    NodeRstar(int i_, int j_, int avoid_, double F_, double g_, NodeRstar* bp_,
              std::shared_ptr<std::map<double, NodeRstar> > predecessors_nodes_,
              std::shared_ptr<std::list<Node>> path_to_bp_, double C_low_) {
        i = i_;
        j = j_;
        avoid = avoid_;
        F = F_;
        g = g_;
        bp = bp_;
        predecessors_nodes = std::move(predecessors_nodes_);
        path_to_bp = std::move(path_to_bp_);
        C_low = C_low_;
    }


    //operator for priority_queue
    bool operator<(const NodeRstar &other) const {
        return std::tuple(avoid, F, g, i, j) < std::tuple(other.avoid, other.F, other.g, other.i, other.j);
    }

    bool operator>= (const NodeRstar &other) const {
        return std::tuple(avoid, F) >= std::tuple(other.avoid, other.F);
    }

    //NodeRstar& operator=(const NodeRstar &other) = default;

    ~NodeRstar() {
        //delete path_to_bp;
        //delete bp;
    }

};


#endif //ADILKHAN_KUSSIDENOV_ASEARCH_NODE_RSTAR_H
