#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_SEARCH_RSTAR_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_SEARCH_RSTAR_H


#include "ilogger.h"
#include "searchresult.h"
#include "searchresult_rstar.h"
#include "environmentoptions.h"
#include "SFML-master/include/SFML/Graphics.hpp"
#include "search.h"
#include <list>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>
#include <optional>
#include <unordered_set>
#include <map>
#include <tuple>
#include <random>
#include "draw_path.h"
#include "open_rstar.h"


class Search_Rstar {
public:
    Search_Rstar();

    ~Search_Rstar(void);

    SearchResult_rstar startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);

protected:

    struct hash_pair {
        size_t operator()(const std::pair<int, int> &p) const {
            return 1ll * INT_MAX * p.first + p.second;
        }
    };



    SearchResult_rstar sresult; //This will store the search result
    std::list<NodeRstar> lppath, hppath; //
    std::unordered_map<std::pair<int, int>, NodeRstar, hash_pair> close_map;

    static double get_heuristic(Point from, Point to, const EnvironmentOptions &options);

    static void CheckRandomNeighbours(NodeRstar &v, const Map &map, const EnvironmentOptions &options, std::vector<std::pair<int, int>> &neighbours);

    void makePrimaryPath(std::shared_ptr<NodeRstar> curNode);

    void makeSecondaryPath();

    void updateState(NodeRstar &state, open_rstar &open, const EnvironmentOptions &options, const Map &map);

    void ReevaluteState(ILogger * Logger, NodeRstar &state, open_rstar &open, const EnvironmentOptions &options, const Map &map);
};



#endif //ADILKHAN_KUSSIDENOV_ASEARCH_SEARCH_RSTAR_H

