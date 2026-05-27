#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace std;

const float g = 9.81;       // Acceleration due to gravity
const float mass = 1.0;     // Mass of the object
const float timeStep = 0.01; // Time step for simulation
const float l = 2.0;        // Length of the pendulum

struct state{
    float theta; // Angle in degrees
    float omega; // Angular velocity
    float x;     // X position
    float y;     // Y position
};

inline void calc_position(float& theta, float& omega, float& x, float& y)
{
    x = sin(theta);
    y = l * cos(theta);
    omega -= timeStep * g * x / l;
    theta += omega * timeStep;
    x *= l;
}

int main()
{
    float theta = 45.0; // Initial angle in degrees
    float omega = 0.0;  // Initial angular velocity
    float x, y;        // Position of the pendulum bob

    cout << "Enter the angle in degrees: ";
    cin >> theta;
    theta = theta * M_PI / 180.0; // Convert to radians
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Pendulum Simulation");
    // window.setFramerateLimit(1/timeStep);
    
    // The pendulum
    sf::CircleShape bob(10);
    bob.setFillColor(sf::Color::Red);

    //The joint
    sf::CircleShape joint(5);
    joint.setFillColor(sf::Color::Blue);
    joint.setPosition({400 - 5, 300 - 5});

    // The rod
    sf::Vertex line[] =
    {
        sf::Vertex{sf::Vector2f(400, 300)},
        sf::Vertex{sf::Vector2f(400, 300)}
    };

    sf::Clock fpsClock;
    int frameCount = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        
        calc_position(theta, omega, x, y);
        
        bob.setPosition({400 + x * 100 - 10, 300 + y * 100 - 10});
        line[1].position = sf::Vector2f(400 + x * 100, 300 + y * 100);

        ++frameCount;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f)
        {
            window.setTitle("Pendulum Simulation - FPS: " + std::to_string(frameCount));
            frameCount = 0;
            fpsClock.restart();
        }
        
        window.clear();
        window.draw(line,2,sf::PrimitiveType::Lines);
        window.draw(bob);
        window.draw(joint);
        window.display();
    }
}