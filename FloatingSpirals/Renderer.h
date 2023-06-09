﻿#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

#include "lib/delaunator.h"
#include "lib/FastNoise.h"


class Renderer {
private:
    RenderWindow* window;
    int iteration;
    float currentTime;
    std::vector<float> getCoords(std::vector<ParticleSim::Particle> particles) {
        std::vector<float> coords;
        coords.reserve(particles.size() * 2);
        for (int i = 0; i < particles.size(); i++) {
            coords.emplace_back(particles[i].pos.x);
            coords.emplace_back(particles[i].pos.y);
        }
        return coords;
    }
    inline float dist(const Vector2f& a, const Vector2f& b) {
        return sqrtf(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
    }
    inline float distSquared(const Vector2f& p1, const Vector2f& p2) {
        // Distance between two points without the square root (helps performance)
        // ((x2-x1)^2 + (y1-y2)^2)
        return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
    }
    // https://www.tutorialspoint.com/program-for-point-of-intersection-of-two-lines-in-cplusplus
    inline Vector2f intersection(Vector2f A, Vector2f B, Vector2f C, Vector2f D) {
        // Line AB represented as a1x + b1y = c1
        double a = B.y - A.y;
        double b = A.x - B.x;
        double c = a * (A.x) + b * (A.y);
        // Line CD represented as a2x + b2y = c2
        double a1 = D.y - C.y;
        double b1 = C.x - D.x;
        double c1 = a1 * (C.x) + b1 * (C.y);
        double det = a * b1 - a1 * b;
        double x = (b1 * c - b * c1) / det;
        double y = (a * c1 - a1 * c) / det;
        return Vector2f(x, y);
    }
    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
    inline Vector2f lerpPoint(Vector2f a, Vector2f b, float t) {
        return Vector2f(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
    }
    inline Color noiseColor(Vector2f point, const FastNoise& noise) {
        float r, g, b, h, s, v;
        h = noise.GetNoise(point.x, point.y, currentTime) * 180.f + 180.f;
        s = 1.f;
        v = .9f;
        HSVtoRGB(r, g, b, h, s, v);
        
        return Color(r * 255, g * 255, b * 255); // h / 360.f * 255.f
    }
    void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
        float fC = fV * fS; // Chroma
        float fHPrime = fmod(fH / 60.0, 6);
        float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
        float fM = fV - fC;

        if (0 <= fHPrime && fHPrime < 1) {
            fR = fC;
            fG = fX;
            fB = 0;
        } else if (1 <= fHPrime && fHPrime < 2) {
            fR = fX;
            fG = fC;
            fB = 0;
        } else if (2 <= fHPrime && fHPrime < 3) {
            fR = 0;
            fG = fC;
            fB = fX;
        } else if (3 <= fHPrime && fHPrime < 4) {
            fR = 0;
            fG = fX;
            fB = fC;
        } else if (4 <= fHPrime && fHPrime < 5) {
            fR = fX;
            fG = 0;
            fB = fC;
        } else if (5 <= fHPrime && fHPrime < 6) {
            fR = fC;
            fG = 0;
            fB = fX;
        } else {
            fR = 0;
            fG = 0;
            fB = 0;
        }

        fR += fM;
        fG += fM;
        fB += fM;
    }
public:
    Renderer(RenderWindow* window) {
        this->window = window;
        iteration = 0;
    }

    void render(const std::vector<ParticleSim::Particle>& particles, const FastNoise& noise, const float& cT) {
        currentTime = cT;
        iteration++;
        std::vector<float> coords = getCoords(particles);
        delaunator::Delaunator d(coords);


        float distanceSquared = 1000000.f;
        std::vector<Vertex> vertices;
        // Iterate over triangles generated by delaunator
        for (int i = 0; i < d.triangles.size(); i += 3) {
            // Organize points of each triangle into something easier to work with
            Vector2f points[3] = {
                Vector2f(d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1]),
                Vector2f(d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1]),
                Vector2f(d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1])
            };


            // Line 1
            vertices.push_back(Vertex(points[0], noiseColor(points[0], noise)));
            vertices.push_back(Vertex(points[1], noiseColor(points[1], noise)));

            // Line 2
            vertices.push_back(Vertex(points[1], noiseColor(points[1], noise)));
            vertices.push_back(Vertex(points[2], noiseColor(points[2], noise)));

            // Line 3
            vertices.push_back(Vertex(points[2], noiseColor(points[2], noise)));
            vertices.push_back(Vertex(points[0], noiseColor(points[0], noise)));
        }

        int iterations = 64;
        float t = 1.f / 16.f;


        // go over each triangle and apply spiral algorithm
        for (int i = 0; i < d.triangles.size(); i += 3) {
            // Organize points of each triangle into something easier to work with
            Vector2f points[3] = {
                Vector2f(d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1]),
                Vector2f(d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1]),
                Vector2f(d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1])
            };


            for (int j = 0; j < iterations; j++) {
                // Take first line
                Vector2f p1 = points[0];
                // Get point on second line at t
                Vector2f p2 = lerpPoint(points[1], points[2], t);
                // Add line to lines
                vertices.push_back(Vertex(p1, noiseColor(p1, noise)));
                vertices.push_back(Vertex(p2, noiseColor(p2, noise)));

                // start at p2 and go to midpoint on line 2-3?
                Vector2f p3 = p2;
                Vector2f p4 = lerpPoint(points[2], points[0], t);
                vertices.push_back(Vertex(p3, noiseColor(p3, noise)));
                vertices.push_back(Vertex(p4, noiseColor(p4, noise)));

                Vector2f p5 = p4;
                Vector2f p6 = lerpPoint(p1, p2, t);
                vertices.push_back(Vertex(p5, noiseColor(p5, noise)));
                vertices.push_back(Vertex(p6, noiseColor(p6, noise)));

                points[0] = p2;
                points[1] = p4;
                points[2] = p6;
            }
        }

        window->draw(&vertices[0], vertices.size(), PrimitiveType::Lines);
    }
};