#ifndef SEARCH_H
#define SEARCH_H

#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <list>
#include <vector>
#include <math.h>
#include <limits>
#include <chrono>
#include <optional>
#include <unordered_set>
#include <map>
#include <tuple>

class Search {
public:
    Search();

    ~Search(void);

    SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options,
                             Point start_, Point finish_);

protected:

    struct hash_pair {
        size_t operator()(const std::pair<int, int> &p) const {
            return 1ll * INT_MAX * p.first + p.second;
        }
    };

    SearchResult sresult; //This will store the search result
    std::list<Node> lppath, hppath; //
    std::unordered_map<std::pair<int, int>, Node, hash_pair> close_map;

    double get_heuristic(Point from, Point to, const EnvironmentOptions &options) const;

    void checkNeighbours(Node &v, const Map &map, const EnvironmentOptions &options, std::vector<Node> &neighbours);

    void makePrimaryPath(Node *curNode);

    void makeSecondaryPath();
};

#endif
