#ifndef OPEN_CONTAINER
#define OPEN_CONTAINER

#include <unordered_map>
#include <set>
#include <climits>
#include "node.h"
#include "gl_const.h"


class open_container {
public:

    explicit open_container(int breakingties_);

    struct compare {
        int breakingties;

        explicit compare(int &br) {
            breakingties = br;
        }

        bool operator()(const Node &lhs,
                        const Node &rhs) const {
            if (breakingties == CN_SP_BT_GMIN ) {
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

    std::unordered_map<std::pair<int, int>, std::set<Node>::iterator, hash_pair> open_map;

    std::set<Node, compare> open_heap;

    Node get_min();

    void insert(const Node &new_node);

    void erase(const std::unordered_map<std::pair<int, int>,
            std::set<Node>::iterator,
            open_container::hash_pair>::iterator &it);

    //void change_if(Node &neighbour);
};

#endif