#include <iostream>
#include <cmath>
#include <deque>
#include <SFML/Graphics.hpp>
using namespace std;

const float g = 9.81;        // Acceleration due to gravity
const float mass = 1.0;      // Mass of the object
const float timeStep = 0.01; // Time step for simulation
const float l = 1.0;         // Length of the pendulum
const int resolution = 1000;     // Number of points in the graph

inline void calc_position(float &theta1, float &theta2, float &omega1,
                          float &omega2)
{
    float alpha1 = (-g * (3.0f * sin(theta1) + sin(theta1 - 2.0f * theta2)) -
                    2.0f * sin(theta1 - theta2) * (omega2 * omega2 * l + omega1 * omega1 * l * cos(theta1 - theta2))) /
                   (l * (3.0f - cos(2.0f * theta1 - 2.0f * theta2)));

    float alpha2 = (2.0f * sin(theta1 - theta2) * (2.0f * omega1 * omega1 * l + 2.0f * g * cos(theta1) + omega2 * omega2 * l * cos(theta1 - theta2))) /
                   (l * (3.0f - cos(2.0f * theta1 - 2.0f * theta2)));

    omega1 += alpha1 * timeStep;
    omega2 += alpha2 * timeStep;

    theta1 += omega1 * timeStep;
    theta2 += omega2 * timeStep;
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    float theta1 = 1.0;  // Initial angle in degrees
    float theta2 = 5.0;  // Initial angle in degrees
    float omega1 = 0.0;   // Initial angular velocity
    float omega2 = 0.0;   // Initial angular velocity

    // cout << "Enter the angle in degrees: ";
    cin >> theta1 >> theta2;
    theta1 = theta1 * M_PI / 180.0; // Convert to radians
    theta2 = theta2 * M_PI / 180.0; // Convert to radians

    float ptheta1, ptheta2;

    int counter = 0;

    while (true)
    {

        ptheta1 = theta1;
        ptheta2 = theta2;

        calc_position(theta1, theta2, omega1, omega2);

        counter++;

        // cout<<counter<<" "<<theta1<<" "<<theta2<<'\n';
        if (theta1 < - M_PI || theta2 < - M_PI)
        {
            break;
        }

        if (theta1 > M_PI || theta2 > M_PI)
        {
            break;
        }

    }
    cout << "Final angles (degrees): " << theta1 * 180.0 / M_PI << " " << theta2 * 180.0 / M_PI << '\n';
    cout << "Number of iterations: " << counter << '\n';
    return 0;
}