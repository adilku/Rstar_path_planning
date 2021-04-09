#include "search_rstar.h"
#include "open_container.h"
#include "open_rstar.h"


Search_Rstar::Search_Rstar() {
//set defaults here
}

Search_Rstar::~Search_Rstar() {}

std::vector<std::pair<int, int>> Search_Rstar::get_dots(int x, int y, int delta) {
    //generate dots on radius delta from (X, Y);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distr(0, 2 * M_PI);
    std::vector<std::pair<int, int>> dots;
    for (size_t i = 0; i < CNT_RND_DOTS; ++i) {
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
Search_Rstar::checkNeighbours(Node_rstar &v, const Map &map, const EnvironmentOptions &options,
                              std::vector<Node_rstar> &neighbours) {
    double hWeight = options.hweight;
    for (auto &micro_node : DISALLOWED_DIAG_MOVES) {
        int i = micro_node.first;
        int j = micro_node.second;
        if (map.CellOnGrid(v.i + i, v.j + j) && map.CellIsTraversable(v.i + i, v.j + j)) {
            neighbours.emplace_back(v.i + i, v.j + j, 0,
                                    v.g + CN_CELL_SIZE +
                                    hWeight * get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options),
                                    v.g + CN_CELL_SIZE,
                                    get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options), &v, nullptr);
        }
    }
    if (options.allowdiagonal) {
        for (auto &cur_point : ALLOWED_DIAG_MOVES) {
            int i = cur_point.first;
            int j = cur_point.second;
            if (map.CellOnGrid(v.i + i, v.j + j) && map.CellIsTraversable(v.i + i, v.j + j)) {
                if (options.allowdiagonal) {
                    bool vert = map.CellOnGrid(v.i, v.j + j) && map.CellIsTraversable(v.i, v.j + j);
                    bool horiz = map.CellOnGrid(v.i + i, v.j) && map.CellIsTraversable(v.i + i, v.j);
                    if ((vert && !horiz && options.cutcorners) ||
                        (!vert && horiz && options.cutcorners) ||
                        (!vert && !horiz && options.allowsqueeze) ||
                        (vert && horiz)
                            ) {
                        neighbours.emplace_back(v.i + i, v.j + j, 0,
                                                v.g + CN_SQRT_TWO +
                                                hWeight *
                                                get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(),
                                                              options),
                                                v.g + CN_SQRT_TWO,
                                                get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options),
                                                &v, nullptr);
                    }
                }
            }
        }
    }
}

void
Search_Rstar::checkRandomNeighbours(Node_rstar &v, const Map &map, const EnvironmentOptions &options,
                              std::vector<std::pair<int, int>> &neighbours) {
    std::vector<std::pair<int, int>> dots = get_dots(v.i, v.j, 4);
    for (auto &dot : dots) {
        if (map.CellOnGrid(dot.first, dot.second) && map.CellIsTraversable(dot.first, dot.second)) {
            neighbours.emplace_back(dot.first, dot.second);
        }
    }
}


void Search_Rstar::updateState(Node_rstar state, open_rstar &open, const EnvironmentOptions &options, const Map &map) {
    double W = options.hweight;
    if (state.g > W * get_heuristic(map.getCoordinatesStart(), {state.i, state.j}, options)) {
        auto it = open.open_map.find({state.i, state.j});
        if (it != open.open_map.end()) {
            open.erase(it);
            open.insert(Node_rstar(state.i, state.j, 1, state.g +
            W * get_heuristic({state.i, state.j}, map.getCoordinatesGoal(), options),
            state.g, state.g, state.parent, state.predecessors_nodes
            ));
        } else if (it == open.open_map.end()) {
            open.insert(Node_rstar(state.i, state.j, 1, state.g +
                                                        W * get_heuristic({state.i, state.j}, map.getCoordinatesGoal(), options),
                                   state.g, state.g, state.parent, state.predecessors_nodes
            ));
        }
    } else {
        auto it = open.open_map.find({state.i, state.j});
        if (it != open.open_map.end()) {
            open.erase(it);
            open.insert(Node_rstar(state.i, state.j, 0, state.g +
                                                        W * get_heuristic({state.i, state.j}, map.getCoordinatesGoal(), options),
                                   state.g, state.g, state.parent, state.predecessors_nodes
            ));
        } else if (it == open.open_map.end()) {
            open.insert(Node_rstar(state.i, state.j, 0, state.g +
                                                        W * get_heuristic({state.i, state.j}, map.getCoordinatesGoal(), options),
                                   state.g, state.g, state.parent, state.predecessors_nodes
            ));
        }
    }
}

void Search_Rstar::ReevaluteState(ILogger *Logger,
                                  Node_rstar state,
                                  open_rstar &open,
                                  const EnvironmentOptions &options,
                                  const Map &map) {
    Search cur_search;
    double W = options.hweight;
    auto local_search = cur_search.startSearch(Logger, map, options, {(*state.parent).i, (*state.parent).j},
                                              {state.i, state.j});
    if (local_search.pathfound) {
        state.path_to_bp = local_search.lppath;
        //render.draw_local_path(window, event, local_search.lppath);
        state.C_low = local_search.pathlength;
    } else {
        state.path_to_bp = nullptr;
    }

    if (state.path_to_bp == nullptr ||
    (*state.parent).g + state.C_low > W * get_heuristic(map.getCoordinatesStart(), {state.i, state.j}, options)) {
        double min_val = INT_MAX;
        for (auto& cur : *state.predecessors_nodes) {
            if (cur.second + cur.first.g < min_val) {
                min_val = cur.second + cur.first.g;
                state.parent = &cur.first;
                state.C_low = cur.second;
            }
        }
        state.avoid = 1;
    }
    state.g = state.parent->g + state.C_low;
    updateState(state, open, options, map);
}


SearchResult_rstar Search_Rstar::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {



    // (node_size * map) / screen = 0.5 -> node_size = (0.5 screen) / map
    double coef = 0.7;


    // нужно реализовать новый контейнер для опена, (bool, cost)
    // + в каждой ноде хранить путь path до предыдущей bp(s). (path_bp(s), s)
    //


    size_t width = map.getMapWidth();
    size_t height = map.getMapHeight();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int sizeOfRect = (int) (coef * desktop.width / std::min(map.getMapWidth(), map.getMapHeight()));
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

        Node_rstar *searchedGoal = nullptr;

        int cnt_steps = 0;

        open.insert(
                Node_rstar(start.i, start.j, 0,  1 * get_heuristic(start, goal, options), 0, get_heuristic(start, goal, options),
                     nullptr, nullptr
                ));

        Node_rstar goal_vert(goal.i, goal.j, 1, INT_MAX, 0, 0, nullptr, nullptr);

        auto rnd_dots = get_dots(start.i, start.j, 4);

        for (auto i : rnd_dots) {
            //std::cout << i.first << ' ' << i.second << '\n';
            render.draw_point(window, event, i.first, i.second, 2);
        }

        while (!open.open_heap.empty() && (goal_vert >= open.check_min())) {
            auto goal_in_open = open.open_map.find({goal.i, goal.j});
            if (goal_in_open != open.open_map.end()) {
                if (*(goal_in_open->second) < open.check_min()) {
                    break;
                }
            }
            ++cnt_steps;
            auto state = open.get_min();
            if (state.i != start.i && state.j != start.j && state.path_to_bp == nullptr) {
                ReevaluteState(Logger, state, open, options, map);
            } else {
                close_map[{state.i, state.j}] = state;
                render.draw_point(window, event, state.i, state.j, 1);
                if (state.i == goal.i && state.j == goal.j && state.path_to_bp != nullptr) {
                    std::cout << "debug path\n";
                    //std::cout << state.path_to_bp.size() << '\n';
                    //render.draw_local_path(window, event, state.path_to_bp);
                    break;
                }
                std::cout << "debug in search_rstar.cpp" << state.i << ' ' << state.j << '\n';
                std::vector<std::pair<int, int>> neighbours;
                checkRandomNeighbours(state, map, options, neighbours);
                if (get_heuristic({state.i, state.j}, {goal.i, goal.j}, options) < 4) {
                    neighbours.emplace_back(goal.i, goal.j);
                    //continue;
                    //break;
                }
                for (auto &neighbour : neighbours) {
                    if (close_map[{neighbour.first, neighbour.second}].i == -1) {
                        auto find_el = open.open_map.find({neighbour.first, neighbour.second});
                        Node_rstar state_to;
                        state_to.path_to_bp = nullptr;
                        state_to.C_low = get_heuristic({state.i, state.j}, {neighbour.first, neighbour.second}, options);
                        if (find_el == open.open_map.end()) {
                            state_to.i = neighbour.first;
                            state_to.j = neighbour.second;
                            state_to.g = INT_MAX;
                            state_to.parent = nullptr;
                        } else {
                            state_to = *find_el->second;
                        }
                        if (state_to.parent == nullptr || state.g + state.C_low < state_to.g) {
                            state_to.g = state.g + state.C_low;
                            state_to.predecessors_nodes->push_back({state, state_to.C_low});
                            state_to.parent = &state;
                            updateState(state_to, open, options, map);
                        }
                    }
                }
            }
            std::cout << "debug in search_rstar.cpp open size = " << open.check_min().F << "\n";
            /*
            close_map[{cur_el.i, cur_el.j}] = cur_el;
            int cur_I = cur_el.i;
            int cur_J = cur_el.j;

            auto cur_v = &close_map[{cur_I, cur_J}];
            if (cur_v->i == goal.i && cur_v->j == goal.j) {
                searchedGoal = &close_map[{cur_v->i, cur_v->j}];
                break;
            }
            std::vector<Node_rstar> neighbours;
            checkNeighbours(*cur_v, map, options, neighbours);
            for (auto &neighbour : neighbours) {
                if (close_map[{neighbour.i, neighbour.j}].i == -1) {
                    auto it = open.open_map.find({neighbour.i, neighbour.j});
                    if (it != open.open_map.end() && it->second->g > neighbour.g) {
                        open.erase(it);
                        open.insert(neighbour);
                        render.draw_point(window, event, neighbour.i, neighbour.j, 1);
                    } else if (it == open.open_map.end()) {
                        open.insert(neighbour);
                        render.draw_point(window, event, neighbour.i, neighbour.j, 1);
                    }
                }
            }
             */
        }
        /*
        sresult.pathfound = (searchedGoal != nullptr);
        if (searchedGoal != nullptr) {
            sresult.pathlength = searchedGoal->g;
        } else {
            sresult.pathlength = 0;
        }
        sresult.nodescreated = open.open_heap.size() + cnt_steps;
        sresult.numberofsteps = cnt_steps;
        if (sresult.pathfound) {
            makePrimaryPath(searchedGoal);
            makeSecondaryPath();
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        sresult.time = (std::chrono::duration<double>(endTime - startTime)).count();
        sresult.hppath = &hppath;
        sresult.lppath = &lppath;
        window.display();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        render.draw_path(window, event, sresult);
         */
    }

    while (window.isOpen()) {
        //sf::Event event;
        //std::cout << "debug in search_rstar.cpp\n"; // debug in search_rstar.cpp
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return sresult;
}

void Search_Rstar::makePrimaryPath(Node_rstar *curNode) {
    while (curNode) {
        lppath.push_front(*curNode);
        curNode = curNode->parent;
    }
}

void Search_Rstar::makeSecondaryPath() {
    if (!lppath.empty()) {
        hppath.push_front(*lppath.begin());
        if (lppath.size() != 1) {
            auto cur = std::next(lppath.begin());
            auto prev = lppath.begin();
            for (auto it = next(lppath.begin(), 2); it != lppath.end(); it = next(it)) {
                int dx_1 = it->i - cur->i;
                int dx_2 = cur->i - prev->i;
                int dy_1 = it->j - cur->j;
                int dy_2 = cur->j - prev->j;
                if (!(dx_1 == dx_2 && dy_1 == dy_2)) {
                    hppath.push_back(*cur);
                }
                prev = cur;
                cur = it;
            }
        }
    }

}
