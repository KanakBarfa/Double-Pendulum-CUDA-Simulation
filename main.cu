#include <iostream>
#include <cuda_runtime.h>
#include <SFML/Graphics.hpp>
#include <algorithm>
#define M_PII 3.14159265358979323846f

const float time_step = 0.01f;
const float gravity = 9.81f;
const float l = 1.0f;

using namespace std;

__device__ void calc_derivatives(float4 s, float4 &out)
{
    float delta = s.x - s.y;
    float sin_delta, cos_delta;
    sincosf(delta, &sin_delta, &cos_delta);
    float sin_t1, cos_t1;
    sincosf(s.x, &sin_t1, &cos_t1);

    float inv_denom = 1.0f / (l * (2.0f + 2.0f * sin_delta * sin_delta));
    float z2_l = s.z * s.z * l;
    float w2_l = s.w * s.w * l;

    out.x = s.z;
    out.y = s.w;
    out.z = (-gravity * (3.0f * sin_t1 + sinf(s.x - 2.0f * s.y)) -
             2.0f * sin_delta * (w2_l + z2_l * cos_delta)) *
            inv_denom;
    out.w = (2.0f * sin_delta * (2.0f * z2_l + 2.0f * gravity * cos_t1 + w2_l * cos_delta)) * inv_denom;
}

__global__ void sim(uchar4 *d_out, int N, int MAX_ITER)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= N || y >= N)
        return;

    int idx = y * N + x;

    float scale = 2.0f / (N - 1);
    float4 s;
    s.x = M_PII * (-1.0f + y * scale);
    s.y = M_PII * (-1.0f + x * scale);
    s.z = 0.0f;
    s.w = 0.0f;

    float4 k1, k2, k3, k4, temp;
    int counter = 0;
    
    const float dt_half = 0.5f * time_step;
    const float dt_sixth = time_step / 6.0f;

    while (counter < MAX_ITER)
    {
        counter++;

        calc_derivatives(s, k1);

        temp.x = s.x + dt_half * k1.x;
        temp.y = s.y + dt_half * k1.y;
        temp.z = s.z + dt_half * k1.z;
        temp.w = s.w + dt_half * k1.w;
        calc_derivatives(temp, k2);

        temp.x = s.x + dt_half * k2.x;
        temp.y = s.y + dt_half * k2.y;
        temp.z = s.z + dt_half * k2.z;
        temp.w = s.w + dt_half * k2.w;
        calc_derivatives(temp, k3);

        temp.x = s.x + time_step * k3.x;
        temp.y = s.y + time_step * k3.y;
        temp.z = s.z + time_step * k3.z;
        temp.w = s.w + time_step * k3.w;
        calc_derivatives(temp, k4);

        s.x += dt_sixth * (k1.x + 2.0f * k2.x + 2.0f * k3.x + k4.x);
        s.y += dt_sixth * (k1.y + 2.0f * k2.y + 2.0f * k3.y + k4.y);
        s.z += dt_sixth * (k1.z + 2.0f * k2.z + 2.0f * k3.z + k4.z);
        s.w += dt_sixth * (k1.w + 2.0f * k2.w + 2.0f * k3.w + k4.w);

        if (fabsf(s.x) > M_PII || fabsf(s.y) > M_PII)
        {
            break;
        }
    }

    float t = counter / static_cast<float>(MAX_ITER);
    t = powf(t, 0.4f);
    uchar4 color;
    color.x = static_cast<std::uint8_t>(255.0f * fminf(t * 3.0f, 1.0f));
    color.y = static_cast<std::uint8_t>(255.0f * fminf(t * 3.0f - 1.0f, 1.0f));
    color.z = static_cast<std::uint8_t>(255.0f * fminf(t * 3.0f - 2.0f, 1.0f));
    color.w = 255;
    d_out[idx] = color;
}

void save_image(const uint8_t *pixels, const std::string &filename, unsigned int N)
{
    sf::Image image({N, N}, pixels);
    bool success = image.saveToFile(filename);
    if (!success)
    {
        std::cerr << "Failed to save image to " << filename << '\n';
    }
}

int main(int argc, char *argv[])
{
    unsigned int N = 1000;
    unsigned int MAX_ITER = 1000;
    if (argc > 1) {
        N = std::stoi(argv[1]);
    }
    if (argc > 2) {
        MAX_ITER = std::stoi(argv[2]);
    }

    uchar4 *d_out;
    float ms;
    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    uint8_t *final;
    cudaMallocHost(&final, N * N * 4 * sizeof(uint8_t));
    cudaMalloc(&d_out, N * N * sizeof(uchar4));

    cudaEventRecord(start);

    dim3 block(16, 16);
    dim3 grid((N + block.x - 1) / block.x, (N + block.y - 1) / block.y);
    sim<<<grid, block>>>(d_out, N, MAX_ITER);

    cudaEventRecord(end);
    cudaEventSynchronize(end);
    cudaEventElapsedTime(&ms, start, end);

    cout << "Elapsed: " << ms << " ms\n";
    cudaMemcpy(final, d_out, N * N * sizeof(uchar4), cudaMemcpyDeviceToHost);
    save_image(final, "output.png", N);

    cudaEventDestroy(start);
    cudaEventDestroy(end);
    cudaFree(d_out);
    cudaFreeHost(final);
}