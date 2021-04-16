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

        bool operator()(const NodeRstar &lhs,
                        const NodeRstar &rhs) const {
            return std::tie(lhs.avoid, lhs.F, lhs.g, lhs.i, lhs.j) < std::tie(rhs.avoid, rhs.F, rhs.g, rhs.i, rhs.j);
        }
    };

    struct hash_pair {
        size_t operator()(const std::pair<int, int> &p) const {
            return 1ll * INT_MAX * p.first + p.second;
        }
    };

    std::unordered_map<std::pair<int, int>, std::set<NodeRstar>::iterator, hash_pair> open_map;

    std::set<NodeRstar, compare> open_heap;

    NodeRstar check_min();

    NodeRstar get_min();

    bool empty() const;

    void insert(const NodeRstar &new_node);

    void erase(const std::unordered_map<std::pair<int, int>,
            std::set<NodeRstar>::iterator,
            open_rstar::hash_pair>::iterator &it);

};

#endif //ADILKHAN_KUSSIDENOV_ASEARCH_OPEN_RSTAR_H
