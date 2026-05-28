#include <iostream>
#include <cmath>
#include <numbers>
#include <array>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#define M_PII 3.14159265358979323846f
#define MAX_ITER 500000

const int N = 200;
const float g = 9.81f;
const float mass = 1.0f;
const float timeStep = 0.01f;
const float l = 1.0f;

inline std::array<float, 4> calc_derivatives(const std::array<float, 4>& state) {
    float t1 = state[0];
    float t2 = state[1];
    float w1 = state[2];
    float w2 = state[3];

    float delta = t1 - t2;
    float denom = l * (3.0f - std::cos(2.0f * t1 - 2.0f * t2));

    float a1 = (-g * (3.0f * std::sin(t1) + std::sin(t1 - 2.0f * t2)) -
                2.0f * std::sin(delta) * (w2 * w2 * l + w1 * w1 * l * std::cos(delta))) / denom;

    float a2 = (2.0f * std::sin(delta) * (2.0f * w1 * w1 * l + 2.0f * g * std::cos(t1) +
                w2 * w2 * l * std::cos(delta))) / denom;

    return {w1, w2, a1, a2};
}

void rk4_step(std::array<float, 4>& state) {
    std::array<float, 4> k1 = calc_derivatives(state);

    std::array<float, 4> state_k2;
    for (int i = 0; i < 4; ++i) state_k2[i] = state[i] + 0.5f * timeStep * k1[i];
    std::array<float, 4> k2 = calc_derivatives(state_k2);

    std::array<float, 4> state_k3;
    for (int i = 0; i < 4; ++i) state_k3[i] = state[i] + 0.5f * timeStep * k2[i];
    std::array<float, 4> k3 = calc_derivatives(state_k3);

    std::array<float, 4> state_k4;
    for (int i = 0; i < 4; ++i) state_k4[i] = state[i] + timeStep * k3[i];
    std::array<float, 4> k4 = calc_derivatives(state_k4);

    for (int i = 0; i < 4; ++i) {
        state[i] += (timeStep / 6.0f) * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    sf::RenderWindow window(sf::VideoMode({N, N}), "Double Pendulum Simulation");

    std::vector<std::uint8_t> pixels(N * N * 4);

    for (int i = 0; i < N * N * 4; ++i) {
        pixels[i] = 255;
    }

    float thetas1[N*N], thetas2[N*N];
    std::array<float, 4> states[N*N];

    for (int i = 0; i < N ; ++i) {
        for (int j = 0; j < N; ++j) {
            int idx = i * N + j;
            thetas1[idx] = -180.0f + (360.0f * i )/ (N-1);
            thetas2[idx] = -180.0f + (360.0f * j )/ (N-1);
            states[idx] = {thetas1[idx] * M_PII / 180.0f, thetas2[idx] * M_PII / 180.0f, 0.0f, 0.0f};
        }
    }

    int counter = 0;
    sf::Texture texture;
    texture.resize({N, N});
    texture.update(pixels.data());
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        counter++;
        if (counter > MAX_ITER) {
            std::cout << "Simulation stabilized without flipping.\n";
            break;
        }


        for (int i = 0; i < N*N; ++i) {
            rk4_step(states[i]);

            if (std::abs(states[i][0]) > M_PII || std::abs(states[i][1]) > M_PII) {
                pixels[4*i] = 0;
                pixels[4*i + 1] = 0;
                pixels[4*i + 2] = 0;
                pixels[4*i + 3] = 255;
            }
        }
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.setTitle("Iteration: " + std::to_string(counter));
        window.display();
    }
    return 0;
}