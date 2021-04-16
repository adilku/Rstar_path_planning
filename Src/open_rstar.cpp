//
// Created by Adilkhan Kussidenov on 4/5/21.
//

#include "open_rstar.h"
#include <iostream> //debug

NodeRstar open_rstar::check_min() {
    return *open_heap.begin();
}

NodeRstar open_rstar::get_min() {
    const auto min_val = open_heap.begin();
    const NodeRstar min_v = *(open_heap.begin());
    open_heap.erase(min_val);
    open_map.erase({min_val->i, min_val->j});
    return min_v;
}

void open_rstar::erase(
        const std::unordered_map<std::pair<int, int>, std::set<NodeRstar>::iterator, open_rstar::hash_pair>::iterator &it) {
    open_heap.erase(it->second);
    open_map.erase(it);
}


void open_rstar::insert(const NodeRstar &new_node) {
    auto temp = open_heap.insert(new_node);
    open_map.insert({{new_node.i, new_node.j}, temp.first});
}

open_rstar::open_rstar(int breakingties_) : open_heap(compare(breakingties_)) {}

bool open_rstar::empty() const {
    return open_heap.empty();
}
