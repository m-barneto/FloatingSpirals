#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;

#include "ParticleSim.h"
#include "Renderer.h"

int main() {
    RenderWindow window(VideoMode::getFullscreenModes()[0], "Floating Spiral Triangles", Style::Fullscreen, ContextSettings::ContextSettings(0U, 0U, 8U));

    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(144);
    



    window.setTitle(std::string("Seed: "));



    ParticleSim sim(window.getSize().x, window.getSize().y, 144, 100);

    Renderer renderer(&window);

    bool paused = false;

    Clock deltaClock;
    float accumulator = 0.f;
    float currentTime = 0.f;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyPressed) {
                switch (e.key.code) {
                case Keyboard::Space:
                    paused = !paused;
                    break;
                case Keyboard::Escape:
                    window.close();
                }
            }
        }
        if (paused) {
            sleep(milliseconds(1));

            float dt = deltaClock.restart().asSeconds();
            continue;
        }

        float dt = deltaClock.restart().asSeconds();
        accumulator += dt;
        currentTime += dt;
        while (accumulator >= sim.resolution) {
            sim.update(currentTime);
            accumulator -= sim.resolution;
        }

        RectangleShape fade(Vector2f(window.getSize().x, window.getSize().y));
        fade.setFillColor(Color(0, 0, 0, 20));
        window.draw(fade);
        //window.clear(Color::Black);
        renderer.render(sim.particles, sim.noise, currentTime);
        window.display();
    }
}
