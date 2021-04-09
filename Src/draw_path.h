//
// Created by Adilkhan Kussidenov on 4/4/21.
//

#ifndef ADILKHAN_KUSSIDENOV_ASEARCH_DRAW_PATH_H
#define ADILKHAN_KUSSIDENOV_ASEARCH_DRAW_PATH_H


#include "search_rstar.h"


class sfml_draw {
public:

    double sizeOfRect;

    sfml_draw(double sizeOfRect_);

    void launch(const Map& map, sf::RenderWindow& temp);

    void draw_obstacles(sf::RenderWindow& temp, const Map& map);

    void draw_path(sf::RenderWindow& window, sf::Event event, const SearchResult_rstar& result);
    void draw_local_path(sf::RenderWindow& window, sf::Event event, std::list<Node> *lppath);

    void draw_point(sf::RenderWindow& window, sf::Event event, int i, int j, int color);

};



#endif //ADILKHAN_KUSSIDENOV_ASEARCH_DRAW_PATH_H
