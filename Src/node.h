#ifndef NODE_H
#define NODE_H

//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

struct Node {
    int i, j; //grid cell coordinates
    double F, g, H; //f-, g- and h-values of the search node
    Node *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)

    Node() : i(-1), j(-1), F(0), g(0), H(0), parent(nullptr) {}

    Node(int i_, int j_, double F_, double g_, double H_, Node *parent_)
            : i(i_), j(j_), F(F_), g(g_), H(H_), parent(parent_) {}

    //operator for priority_queue
    bool operator<(const Node &other) const {
        return F < other.F;
    }

    bool operator>(const Node &other) const {
        return F > other.F;
    }

    Node& operator=(const Node &other) = default;

};

#endif
