#include <iostream>
#include <cmath>
#include <numbers>
#include <array>
#define M_PII 3.14159265358979323846f
#define MAX_ITER 5000000
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

    float theta1, theta2;
    if (!(std::cin >> theta1 >> theta2)) return 0;

    std::array<float, 4> state = {
        theta1 * M_PII / 180.0f,
        theta2 * M_PII / 180.0f,
        0.0f,
        0.0f
    };

    int counter = 0;

    while (true) {
        rk4_step(state);
        counter++;

        if (state[0] < -M_PII || state[1] < -M_PII ||
            state[0] > M_PII || state[1] > M_PII) {
            break;
        }

        if (counter > MAX_ITER) {
            std::cout << "Simulation stabilized without flipping.\n";
            break;
        }
    }

    std::cout << "Final angles (degrees): " << state[0] * 180.0f / M_PII << " "
              << state[1] * 180.0f / M_PII << '\n';
    std::cout << "Number of iterations: " << counter << '\n';
    return 0;
}