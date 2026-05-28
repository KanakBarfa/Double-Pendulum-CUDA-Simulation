#include <iostream>
#include <cuda_runtime.h>
#include <vector>
#include <SFML/Graphics.hpp>
#define N 1000
#define MAX_ITER 50000
#define M_PII 3.14159265358979323846f

const float time_step = 0.01f;
const float gravity = 9.81f;
const float l = 1.0f;

using namespace std;

__device__ void calc_derivatives(const float *s, float *out)
{
    float t1 = s[0];
    float t2 = s[1];
    float w1 = s[2];
    float w2 = s[3];

    float delta = t1 - t2;
    float denom = l * (3.0f - cosf(2.0f * t1 - 2.0f * t2));

    out[0] = w1;
    out[1] = w2;
    out[2] = (-gravity * (3.0f * sinf(t1) + sinf(t1 - 2.0f * t2)) -
              2.0f * sinf(delta) * (w2 * w2 * l + w1 * w1 * l * cosf(delta))) /
             denom;
    out[3] = (2.0f * sinf(delta) * (2.0f * w1 * w1 * l + 2.0f * gravity * cosf(t1) + w2 * w2 * l * cosf(delta))) / denom;
}

__global__ void sim(const float *state, int *d_iter)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx >= N * N)
        return;

    int base = idx * 4;
    float s[4];
    s[0] = state[base];
    s[1] = state[base + 1];
    s[2] = state[base + 2];
    s[3] = state[base + 3];

    float k1[4], k2[4], k3[4], k4[4], temp[4];

    int counter = 0;
    while (counter < MAX_ITER)
    {
        counter++;

        calc_derivatives(s, k1);

        for (int i = 0; i < 4; ++i)
            temp[i] = s[i] + 0.5f * time_step * k1[i];
        calc_derivatives(temp, k2);

        for (int i = 0; i < 4; ++i)
            temp[i] = s[i] + 0.5f * time_step * k2[i];
        calc_derivatives(temp, k3);

        for (int i = 0; i < 4; ++i)
            temp[i] = s[i] + time_step * k3[i];
        calc_derivatives(temp, k4);

        for (int i = 0; i < 4; ++i)
        {
            s[i] += (time_step / 6.0f) * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]);
        }

        if (fabsf(s[0]) > M_PII || fabsf(s[1]) > M_PII)
        {
            d_iter[idx] = counter;
            return;
        }
    }
    d_iter[idx] = MAX_ITER;
}

__global__ void init(float *state, int *d_iter)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if (idx < N * N)
    {
        state[idx << 2] = M_PII * (-1.0f + (2.0f * (idx / N)) / (N - 1));
        state[(idx << 2) + 1] = M_PII * (-1.0f + (2.0f * (idx % N)) / (N - 1));
        state[(idx << 2) + 2] = 0.0f;
        state[(idx << 2) + 3] = 0.0f;
        d_iter[idx] = -1;
    }
}

void render_graph(int *final)
{
    sf::RenderWindow window(sf::VideoMode({N, N}), "Double Pendulum Simulation");
    std::vector<std::uint8_t> pixels(N * N * 4, 255);
    sf::Texture texture;
    bool flag = texture.resize({N, N});
    texture.update(pixels.data());
    sf::Sprite sprite(texture);

    int counter = 0;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        counter++;
        if (counter > MAX_ITER)
        {
            break;
        }
        for (int i = 0; i < N * N; i++)
        {
            if (final[i] == counter)
            {
                pixels[4 * i] = 255 * (counter / static_cast<float>(MAX_ITER));
                pixels[4 * i + 1] = 255 - 255 * (counter / static_cast<float>(MAX_ITER));
                pixels[4 * i + 2] = 0;
                pixels[4 * i + 3] = 255;
            }
        }
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.setTitle("Iteration: " + std::to_string(counter));
        window.display();
    }
}

int main()
{
    int *d_iterations;
    int *final;
    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaMalloc(&d_iterations, N * N * sizeof(int));
    cudaMallocHost(&final, N * N * sizeof(int));

    float *state, ms;
    cudaMalloc(&state, N * N * 4 * sizeof(float));

    cudaEventRecord(start);

    init<<<(N * N + 255) / 256, 256>>>(state, d_iterations);
    sim<<<(N * N + 255) / 256, 256>>>(state, d_iterations);

    cudaEventRecord(end);

    cudaMemcpy(final, d_iterations, N * N * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventElapsedTime(&ms, start, end);

    cout << "Elapsed: " << ms << " ms\n";
    cudaEventDestroy(start);
    cudaEventDestroy(end);
    cudaFree(state);
    cudaFree(d_iterations);

    // Rendering
    render_graph(final);

    cudaFreeHost(final);
}