//
// Created by Adilkhan Kussidenov on 4/4/21.
//

#include "draw_path.h"



sfml_draw::sfml_draw(double sizeOfRect_) {
    sizeOfRect = sizeOfRect_;
}


void sfml_draw::launch(const Map& map, sf::RenderWindow& window) {

    //растягивание карты под размер окна
    //sf::View view(sf::FloatRect(0., 0., (float)width + 1, (float)height + 1));
    //window.setView(view);
    //sf::CircleShape shape(100.f);
    //shape.setFillColor(sf::Color::Yellow);
    draw_obstacles(window, map);
    //draw_path(window, (double)sizeOfRect);
}

void sfml_draw::draw_obstacles(sf::RenderWindow& window, const Map& map) {
    int ** map_obstacles = map.getGrid();

    int height = map.getMapHeight();
    int width = map.getMapWidth();

    if (window.isOpen()) {
        window.clear(sf::Color::White);
        for (int i = 0; i < height && window.isOpen(); ++i) {
            for (int j = 0; j < width && window.isOpen(); ++j) {
                if (map_obstacles[i][j]) {
                    if (window.isOpen()) {
                        sf::RectangleShape quad(sf::Vector2f(sizeOfRect, sizeOfRect));
                        quad.setFillColor(sf::Color::Black);
                        quad.setPosition((double) j * sizeOfRect, (float) i * sizeOfRect);
                        window.draw(quad);
                    }
                }
            }
        }
        window.display();
    }
}

void sfml_draw::draw_path( sf::RenderWindow& window, sf::Event event, const std::list<NodeRstar> &lppath) {
    for (auto &node_cur : lppath) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //std::cout << node_cur.j << ' ' << node_cur.i << '\n';
        if (window.isOpen()) {
            sf::RectangleShape quad(sf::Vector2f(sizeOfRect, sizeOfRect));
            quad.setFillColor(sf::Color::Blue);
            quad.setPosition((double) node_cur.j * sizeOfRect, (float) node_cur.i * sizeOfRect);
            window.draw(quad);
            window.display();
        }
    }
}

void sfml_draw::draw_local_path( sf::RenderWindow& window, sf::Event event,const std::shared_ptr<std::list<Node>>& lppath) {
    for (auto &node_cur : *lppath) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (window.isOpen()) {
            double sizeOfRect_quad = sizeOfRect;
            if (sizeOfRect_quad < 5) {
                sizeOfRect_quad *= 2;
            }
            sf::RectangleShape quad(sf::Vector2f(sizeOfRect_quad, sizeOfRect_quad));
            quad.setFillColor(sf::Color::Yellow);
            quad.setPosition((double) node_cur.j * sizeOfRect, (float) node_cur.i * sizeOfRect);
            window.draw(quad);
        }
    }
    //draw first and last vertex
    {
        sf::RectangleShape quad(sf::Vector2f(sizeOfRect, sizeOfRect));
        quad.setFillColor(sf::Color::Red);
        quad.setPosition((double) lppath->begin()->j * sizeOfRect, (float) lppath->begin()->i * sizeOfRect);
        window.draw(quad);
        window.display();
    }
    {
        sf::RectangleShape quad(sf::Vector2f(sizeOfRect, sizeOfRect));
        quad.setFillColor(sf::Color::Red);
        quad.setPosition((double) lppath->back().j * sizeOfRect, (float) lppath->back().i * sizeOfRect);
        window.draw(quad);
        window.display();
    }
}


void sfml_draw::draw_point(sf::RenderWindow &window, sf::Event event, int i, int j, int color) {
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    if (window.isOpen()) {
        sf::RectangleShape quad(sf::Vector2f(sizeOfRect, sizeOfRect));
        if (color == 1) {
            quad.setFillColor(sf::Color::Blue);
        } else if (color == 2) {
            quad.setFillColor(sf::Color::Red);
        } else if (color == 3) {
            quad.setFillColor(sf::Color::Green);
        } else if (color == 4) {
            quad = sf::RectangleShape(sf::Vector2f(10 * sizeOfRect, 10 * sizeOfRect));
        }
        quad.setPosition((double) j * sizeOfRect, (float) i * sizeOfRect);
        window.draw(quad);
        window.display();
    }
}







