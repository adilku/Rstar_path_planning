#include "open_container.h"

Node open_container::get_min() {
    auto min_val = open_heap.begin();
    open_heap.erase(min_val);
    open_map.erase({min_val->i, min_val->j});
    return *min_val;
}

void open_container::erase(
        const std::unordered_map<std::pair<int, int>, std::set<Node>::iterator, open_container::hash_pair>::iterator &it) {
    open_heap.erase(it->second);
    open_map.erase(it);
}

void open_container::insert(const Node &new_node) {
    auto temp = open_heap.insert(new_node);
    open_map.insert({{new_node.i, new_node.j}, temp.first});
}

open_container::open_container(int breakingties_) : open_heap(compare(breakingties_)) {}
