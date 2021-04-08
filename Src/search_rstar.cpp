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
Search_Rstar::checkNeighbours(Node_rstar &v, const Map &map, const EnvironmentOptions &options, std::vector<Node_rstar> &neighbours) {
    double hWeight = options.hweight;
    for (auto &micro_node : DISALLOWED_DIAG_MOVES) {
        int i = micro_node.first;
        int j = micro_node.second;
        if (map.CellOnGrid(v.i + i, v.j + j) && map.CellIsTraversable(v.i + i, v.j + j)) {
            neighbours.emplace_back(v.i + i, v.j + j,
                                    v.g + CN_CELL_SIZE +
                                    hWeight * get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options),
                                    v.g + CN_CELL_SIZE,
                                    get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options), &v);
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
                        neighbours.emplace_back(v.i + i, v.j + j,
                                                v.g + CN_SQRT_TWO +
                                                hWeight *
                                                get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(),
                                                              options),
                                                v.g + CN_SQRT_TWO,
                                                get_heuristic({v.i + i, v.j + j}, map.getCoordinatesGoal(), options),
                                                &v);
                    }
                }
            }
        }
    }
}


SearchResult_rstar Search_Rstar::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {



    // (node_size * map) / screen = 0.5 -> node_size = (0.5 screen) / map
    double coef = 0.4;


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
    //SearchResult local_path = cur_search.startSearch(Logger, map, options, map.getCoordinatesStart(), map.getCoordinatesGoal());



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


        //render.draw_path(window, event, local_path);

        //sf::sleep(sf::milliseconds(10000));

        open_rstar open(options.breakingties);

        auto startTime = std::chrono::high_resolution_clock::now();

        Point start = map.getCoordinatesStart();
        Point goal = map.getCoordinatesGoal();

        Node_rstar *searchedGoal = nullptr;

        int cnt_steps = 0;

        open.insert(
                Node_rstar(start.i, start.j, 1 * get_heuristic(start, goal, options), 0, get_heuristic(start, goal, options),
                     nullptr
                ));

        auto rnd_dots = get_dots(start.i, start.j, 4);

        for (auto i : rnd_dots) {
            //std::cout << i.first << ' ' << i.second << '\n';
            render.draw_point(window, event, i.first, i.second, 2);
        }

        while (!open.open_heap.empty()) {

            ++cnt_steps;
            auto cur_el = open.get_min();
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
        }

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
