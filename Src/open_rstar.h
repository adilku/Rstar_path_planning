//
// Created by Adilkhan Kussidenov on 4/5/21.
//

#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_OPEN_RSTAR_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_OPEN_RSTAR_H


#include <unordered_map>
#include <set>
#include <climits>
#include "node.h"
#include "node_rstar.h"
#include "gl_const.h"


class open_rstar {
public:

    explicit open_rstar(int breakingties_);




    struct compare {
        int breakingties;

        explicit compare(int &br) {
            breakingties = br;
        }

        bool operator()(const Node_rstar &lhs,
                        const Node_rstar &rhs) const {
            if (breakingties == CN_SP_BT_GMIN) {
                return std::tuple(lhs.F, lhs.g, lhs.i, lhs.j) < std::tuple(rhs.F, rhs.g, rhs.i, rhs.j);
            } else {
                return std::tuple(lhs.F, rhs.g, lhs.i, lhs.j) < std::tuple(rhs.F, lhs.g, rhs.i, rhs.j);
            }
        }
    };

    struct hash_pair {
        size_t operator()(const std::pair<int, int> &p) const {
            return 1ll * INT_MAX * p.first + p.second;
        }
    };

    std::unordered_map<std::pair<int, int>, std::set<Node_rstar>::iterator, hash_pair> open_map;

    std::set<Node_rstar, compare> open_heap;

    Node_rstar get_min();

    void insert(const Node_rstar &new_node);

    void erase(const std::unordered_map<std::pair<int, int>,
            std::set<Node_rstar>::iterator,
            open_rstar::hash_pair>::iterator &it);

};

#endif //ADILKHAN_KUSSIDENOV_ASEARCH_OPEN_RSTAR_H
