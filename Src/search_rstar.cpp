#include "search_rstar.h"
#include "open_container.h"
#include "open_rstar.h"


Search_Rstar::Search_Rstar() {
//set defaults here
}

Search_Rstar::~Search_Rstar() = default;



void
Search_Rstar::CheckRandomNeighbours(NodeRstar &v, const Map &map, const EnvironmentOptions &options,
                                    std::vector<std::pair<int, int>> &neighbours) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distr(0, 2 * M_PI);
    std::vector<std::pair<int, int>> dots;
    int delta = options.radius;
    while ((int) neighbours.size() != options.numberofstates) {
        double phi = distr(generator);
        int x_ = (int) (cos(phi) * (double) delta);
        int y_ = (int) (sin(phi) * (double) delta);
        int dot_to_x = v.i + x_;
        int dot_to_y = v.j + y_;
        if (map.CellOnGrid(dot_to_x, dot_to_y) && map.CellIsTraversable(dot_to_x, dot_to_y)) {
            neighbours.emplace_back(dot_to_x, dot_to_y);
        }
    }
}

double Search_Rstar::get_heuristic(Point from, Point to, const EnvironmentOptions &options) {
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


void Search_Rstar::updateState(NodeRstar &state, open_rstar &open, const EnvironmentOptions &options, const Map &map) {
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
Search_Rstar::ReevaluteState(ILogger *Logger, NodeRstar &state, open_rstar &open, const EnvironmentOptions &options,
                             const Map &map) {
    Search cur_search;
    double W = options.hweight;
    Point start = map.getCoordinatesStart();
    SearchResult local_res = cur_search.startSearch(Logger, map, options, {state.i, state.j},
                                                    {state.bp->i, state.bp->j});
    if (local_res.pathfound) {
        state.path_to_bp = local_res.lppath;
        state.C_low = local_res.pathlength;
    }
    if (state.path_to_bp == nullptr ||
        state.bp->g + state.C_low > W * get_heuristic(start, {state.i, state.j}, options)) {
        NodeRstar min_state;
        if (state.path_to_bp->size() > 1) {
            state.bp = std::make_shared<NodeRstar>(state.predecessors_nodes->begin()->second);
        }
        state.avoid = 1;
    }
    state.g = state.bp->g + state.C_low;
    updateState(state, open, options, map);
}


SearchResult_rstar Search_Rstar::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    // (node_size * map) / screen = 0.5 -> node_size = (0.5 screen) / map

    size_t width = map.getMapWidth();
    size_t height = map.getMapHeight();

    //------------------visual init
#ifdef VISUAL_MODE
    double coef = 0.5;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int sizeOfRect = 1.8 * (int) (coef * std::min(desktop.height, desktop.width) / std::min(map.getMapWidth(), map.getMapHeight()));
    sfml_draw render(sizeOfRect);
    sf::RenderWindow window(sf::VideoMode(sizeOfRect * width, sizeOfRect * height, desktop.bitsPerPixel),
                            "R star visualizer", sf::Style::Close | sf::Style::Titlebar, settings);
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    render.launch(map, window);
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
#endif
    //-----------------end visual init

   // search test
   /*
   Search new_search;
   auto temp = new_search.startSearch(Logger, map, options, map.getCoordinatesStart(), map.getCoordinatesGoal());
   std::cout << "nodes a* " << temp.numberofsteps << ' ' << temp.nodescreated << '\n';
   std::cout << "time a* " << temp.time << '\n';
   std::cout << "len a* " << temp.pathlength << '\n';

   std::cout << new_search.close_map.size() << '\n';
   for (auto i : new_search.close_map) {
       render.draw_point(window, event, i.first.first, i.first.second, 2);
   }
   */
   //return sresult;

    auto startTime = std::chrono::high_resolution_clock::now();
    open_rstar open(options.breakingties);
    Point start = map.getCoordinatesStart();
    Point goal = map.getCoordinatesGoal();
    NodeRstar start_state(start.i, start.j, 0,
                          get_heuristic(start, goal, options),
                          0,
                          nullptr,
                          std::make_shared<std::map<double, NodeRstar> >(),
                          nullptr,
                          0);

    NodeRstar goal_state(goal.i, goal.j, 1,
                         INT_MAX,
                         INT_MAX,
                         nullptr,
                         std::make_shared<std::map<double, NodeRstar> >(),
                         nullptr,
                         0);

    open.insert(start_state); // inserted start state
    int cnt_steps = 0;
    while (!open.empty() && goal_state >= open.check_min()) {
        ++cnt_steps;
        if (open.open_map.find({goal.i, goal.j}) != open.open_map.end()) {
            if (*(open.open_map.find({goal.i, goal.j})->second) < open.check_min()) {
                break; //
            }
        }
        NodeRstar s = open.get_min();
#ifdef VISUAL_MODE
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //render.draw_point(window, event, s.i, s.j, 2);
#endif
        if (!(s.i == start.i && s.j == start.j) && s.path_to_bp == nullptr) {
            ReevaluteState(Logger, s, open, options, map);
        } else {
            close_map[{s.i, s.j}] = s;
#ifdef VISUAL_MODE
            render.draw_point(window, event, s.i, s.j, 2);
#endif
            if (s.i == goal.i && s.j == goal.j) {
                //path found
                sresult.pathfound = true;
                goal_state = s;
                break;
            }
            std::vector<std::pair<int, int>> neighbours;
            CheckRandomNeighbours(s, map, options, neighbours);

            if (get_heuristic({s.i, s.j}, goal, options) <= options.radius) {
                neighbours.emplace_back(goal.i, goal.j);
                //break;
            }
            for (auto &s_to : neighbours) {
                if (close_map[{s_to.first, s_to.second}].i == -1) {
                    NodeRstar s_to_node;
                    s_to_node.i = s_to.first;
                    s_to_node.j = s_to.second;
                    s_to_node.path_to_bp = nullptr;
                    s_to_node.C_low = get_heuristic({s.i, s.j}, {s_to_node.i, s_to_node.j}, options);
                    //check visited first time or not;
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
                        s_to_node.predecessors_nodes->insert({s_to_node.C_low, s});
                        updateState(s_to_node, open, options, map);
                    }
                }
            }
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    sresult.time = (std::chrono::duration<double>(endTime - startTime)).count();
    makePrimaryPath(std::make_shared<NodeRstar>(goal_state));

    if (sresult.pathfound) {
        sresult.pathlength = goal_state.g;
        sresult.lppath = &lppath;
        sresult.hppath = &lppath;
        sresult.nodescreated = cnt_steps + open.open_heap.size();
        sresult.numberofsteps = cnt_steps;
    }

    //render.draw_path(window, event, lppath);

#ifdef VISUAL_MODE
    for (auto &i : lppath) {
        if (i.path_to_bp) {
            render.draw_local_path(window, event, i.path_to_bp);
        }
    }
    sf::Image Screenshot;
    Screenshot = window.capture();
    Screenshot.saveToFile("../../Examples/path_visual.jpg");
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
#endif

    return sresult;
}

void Search_Rstar::makePrimaryPath(std::shared_ptr<NodeRstar> curNode) {
    lppath.push_front(*curNode);
    while (curNode) {
        if (curNode->path_to_bp != nullptr) {
            std::list<Node> path_to_bp = *curNode->path_to_bp;
            for (auto cur = path_to_bp.begin(); cur != std::prev(path_to_bp.end()); ++cur) {
                NodeRstar to_node = *curNode;
                to_node.i = cur->i;
                to_node.j = cur->j;
                lppath.push_front(to_node);
            }
        }
        curNode = curNode->bp;
    }
}




