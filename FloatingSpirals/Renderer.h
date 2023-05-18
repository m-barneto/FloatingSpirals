#pragma once
class Renderer {
private:
    RenderWindow* window;
public:
    Renderer(RenderWindow* window) {
        this->window = window;
    }

    void render(const std::vector<ParticleSim::Particle>& particles) {
        CircleShape cs;
        cs.setRadius(5.f);
        for (int i = 0; i < particles.size(); i++) {
            cs.setPosition(particles[i].pos.x, particles[i].pos.y);
            window->draw(cs);
        }

    }
};