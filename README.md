# CUDA Based Double Pendulum Fractal Renderer


## Preview
<div align="center">
  <video src="output.mp4" controls="controls" muted="muted" autoplay="autoplay" loop="loop" width="400">
    Your browser does not support the video tag.
  </video>
</div>

## Brief
This uses GPU accelerated 4th order Runge-Kutta method for simulation as when i tried doing this project on cpu, it was very slow. Currently takes just 0.5 seconds for 1000x1000 render for 5000 iterations

## Building and Running

Use `make run`

## 🛠️ Prerequisites

To build and run this project, you need an NVIDIA GPU and the following dependencies installed on your system:

* **CUDA Toolkit** (for the `nvcc` compiler)
* **A C++17 compatible host compiler** (e.g., `g++`)
* **FFmpeg** (for video rendering)
* **SFML** (for realtime rendering window)
