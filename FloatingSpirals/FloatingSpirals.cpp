#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;

#include "ParticleSim.h"
#include "Renderer.h"

int main() {
    RenderWindow window(VideoMode(800, 600), "Floating Spiral Triangles");

    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);



    window.setTitle(std::string("Seed: "));



    ParticleSim sim(800, 600);

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
                }
            }
        }
        if (paused) {
            sleep(milliseconds(1));
            continue;
        }

        float dt = deltaClock.restart().asSeconds();
        accumulator += dt;
        currentTime += dt;
        while (accumulator >= sim.resolution) {
            sim.update(currentTime);
            accumulator -= sim.resolution;
        }

        window.clear(Color::Black);
        renderer.render(sim.particles);
        window.display();
    }
}
