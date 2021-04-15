#include "search_rstar.h"
#include "open_container.h"
#include "open_rstar.h"



Search_Rstar::Search_Rstar() {
//set defaults here
}

Search_Rstar::~Search_Rstar() {}

std::vector<std::pair<int, int>> Search_Rstar::get_dots(int x, int y, int delta, const EnvironmentOptions &options) {
    //generate dots on radius delta from (X, Y);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distr(0, 2 * M_PI);
    std::vector<std::pair<int, int>> dots;
    for (size_t i = 0; i < options.numberofstates; ++i) {
        double phi = distr(generator);
        int x_ = (int) (cos(phi) * (double)delta);
        int y_ = (int) (sin(phi) * (double)delta);
        dots.emplace_back(x + x_,y + y_);
    }

    return dots;

};

double Search_Rstar::get_heuristic(Point from, Point to, const EnvironmentOptions &options) const {
    if (options.metrictype == CN_SP_MT_DIAG) {
        return CN_CELL_SIZE * abs(abs(to.i - from.i) - abs(to.j - from.j)) +
               CN_SQRT_TWO * std::min(abs(to.i - from.i), abs(to.j - from.j));
    } else if (options.metrictype == CN_SP_MT_MANH) {

        return CN_CELL_SIZE * (abs(from.i - to.i) + abs(from.j - to.j));
    } else if (options.metrictype == CN_SP_MT_EUCL) {
        return CN_CELL_SIZE * sqrt((to.i - from.i) * (to.i - from.i) + (to.j - from.j) * (to.j - from.j));
    } else if (options.metrictype == CN_SP_MT_CHEB) {
        return std::max(abs(from.i - to.i), abs(from.j - to.j));
    }
    return 0;
}



void
Search_Rstar::CheckRandomNeighbours(NodeRstar &v, const Map &map, const EnvironmentOptions &options,
                                    std::vector<std::pair<int, int>> &neighbours) {
    std::vector<std::pair<int, int>> dots = get_dots(v.i, v.j, 8, options);
    for (auto &dot : dots) {
        if (map.CellOnGrid(dot.first, dot.second) && map.CellIsTraversable(dot.first, dot.second)) {
            neighbours.emplace_back(dot.first, dot.second);
        }
    }
}



void Search_Rstar::updateState(NodeRstar state, open_rstar &open, const EnvironmentOptions &options, const Map &map) {
    double W = options.hweight;
    Point start = map.getCoordinatesStart();
    Point goal = map.getCoordinatesGoal();
    state.F = state.g + get_heuristic({state.i, state.j}, goal, options);
    if (state.g > W * get_heuristic(start, {state.i, state.j}, options) ||
            (state.bp == nullptr && state.avoid == 1)) {
        state.avoid = 1;
        auto it = open.open_map.find({state.i, state.j});
        if (it == open.open_map.end()) {
            //insert
            open.insert(state);
        } else {
            //update
            open.erase(it);
            open.insert(state);
        }
    } else {
        state.avoid = 0;
        auto it = open.open_map.find({state.i, state.j});
        if (it == open.open_map.end()) {
            //insert
            open.insert(state);
        } else {
            //update
            open.erase(it);
            open.insert(state);
        }
    }
}

void
Search_Rstar::ReevaluteState(ILogger * Logger, NodeRstar state, open_rstar &open, const EnvironmentOptions &options, const Map &map) {
    Search cur_search;
    double W = options.hweight;
    Point start = map.getCoordinatesStart();
    SearchResult local_res = cur_search.startSearch(Logger, map, options, {state.i, state.j}, {state.bp->i, state.bp->j});
    if (local_res.pathfound) {
        state.path_to_bp = local_res.lppath;
        state.C_low = local_res.pathlength;
    }
    if (state.path_to_bp == nullptr || state.bp->g + state.C_low > W * get_heuristic(start, {state.i, state.j}, options)) {
        NodeRstar min_state;
        double min_value = INT_MAX;

        for (auto &i : *state.predecessors_nodes) {
            if (min_value > i.first.g + i.second) {
                min_value = i.first.g + i.second;
                state.bp = i.first.bp;
                state.C_low = i.first.C_low;
                state.path_to_bp = i.first.path_to_bp;
            }
        }

        state.avoid = 1;
    }
    state.g = state.bp->g  + state.C_low;
    updateState(state, open, options, map);
}



SearchResult_rstar Search_Rstar::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    // (node_size * map) / screen = 0.5 -> node_size = (0.5 screen) / map
    double coef = 0.5;
    size_t width = map.getMapWidth();
    size_t height = map.getMapHeight();
    //---------------visual
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int sizeOfRect = (int) (coef * std::min(desktop.height, desktop.width) / std::min(map.getMapWidth(), map.getMapHeight()));
    sfml_draw render(sizeOfRect);
    sf::RenderWindow window(sf::VideoMode(sizeOfRect * width, sizeOfRect * height, desktop.bitsPerPixel),
                            "R star visualizer", sf::Style::Close | sf::Style::Titlebar, settings);

    //DEBUG local path
    Search cur_search;
    SearchResult local_path = cur_search.startSearch(Logger, map, options, map.getCoordinatesStart(), map.getCoordinatesGoal());

    //std::cout << "debug in searchrstar.cpp pathfound" << local_path.pathlength << '\n';

    sf::Event event;

    if (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        render.launch(map, window);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        render.draw_local_path(window, event, local_path.lppath);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //sf::sleep(sf::milliseconds(10000));

        open_rstar open(options.breakingties);
        auto startTime = std::chrono::high_resolution_clock::now();
        Point start = map.getCoordinatesStart();
        Point goal = map.getCoordinatesGoal();
        NodeRstar *searchedGoal = nullptr;
        int cnt_steps = 0;

        NodeRstar start_state(start.i, start.j, 0,
                              get_heuristic(start, goal, options),
                              0,
                              nullptr,
                              std::make_shared<std::vector<std::pair<NodeRstar, double>>>(),
                              nullptr,
                              0);

        NodeRstar goal_state(goal.i, goal.j, 1,
                             INT_MAX,
                             INT_MAX,
                             nullptr,
                             std::make_shared<std::vector<std::pair<NodeRstar, double>>>(),
                             nullptr,
                             0);

        open.insert(start_state); // inserted start state

        while (!open.empty() && goal_state >= open.check_min()) {
            if (open.open_map.find({goal.i, goal.j}) != open.open_map.end()) {
                if (*(open.open_map.find({goal.i, goal.j})->second) >= open.check_min()) {
                    break;
                }
            }
            if (open.open_map.size() != open.open_heap.size()) {
                std::cout << "pizedc\n";
                break;
            }
            NodeRstar s = open.get_min();
            render.draw_point(window, event, s.i, s.j, 2);
            if (s.i != start.i && s.j != start.j && s.path_to_bp == nullptr) {
                ReevaluteState(Logger, s, open, options, map);
            } else {
                close_map[{s.i, s.j}] = s;
                std::vector<std::pair<int, int>> neighbours;
                CheckRandomNeighbours(s, map, options, neighbours);

                if (get_heuristic({s.i, s.j}, goal, options) <= 8) {
                    neighbours.emplace_back(goal.i, goal.j);
                    //break;
                }
                for (auto &s_to : neighbours) {
                    if (close_map[{s_to.first, s_to.second}].i == -1) {
                        //check visited first time or not;
                        NodeRstar s_to_node;
                        s_to_node.i = s_to.first;
                        s_to_node.j = s_to.second;
                        s_to_node.path_to_bp = nullptr;
                        s_to_node.C_low = get_heuristic({s.i, s.j}, {s_to_node.i, s_to_node.j}, options);
                        if (open.open_map.find({s_to.first, s_to.second}) == open.open_map.end()) {
                            //visited first time
                            s_to_node.g = INT_MAX;
                            s_to_node.bp = nullptr;
                        } else {
                            //visited before
                            s_to_node = *(open.open_map.find({s_to.first, s_to.second})->second);
                        }
                        if (s_to_node.bp == nullptr || s.g + s_to_node.C_low < s_to_node.g) {
                            s_to_node.g = s.g + s_to_node.C_low;
                            s_to_node.bp = std::make_shared<NodeRstar>(s);
                            updateState(s_to_node, open, options, map);
                        }
                    }
                }
            }
        }


        /*
        window.display();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        render.draw_path(window, event, sresult);
         */
    }

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return sresult;
}



