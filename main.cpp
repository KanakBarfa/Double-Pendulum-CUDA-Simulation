#include <iostream>
#include <cmath>
#include <deque>
#include <SFML/Graphics.hpp>
using namespace std;

const float g = 9.81;        // Acceleration due to gravity
const float mass = 1.0;      // Mass of the object
const float timeStep = 0.01; // Time step for simulation
const float l = 1.0;         // Length of the pendulum

inline void calc_position(float &theta1, float &theta2, float &omega1,
                          float &omega2, float &x1, float &y1, float &x2, float &y2)
{
    float alpha1 = (-g * (3.0f * sin(theta1) + sin(theta1 - 2.0f * theta2)) - 
                    2.0f * sin(theta1 - theta2) * (omega2 * omega2 * l + omega1 * omega1 * l * cos(theta1 - theta2))) / 
                   (l * (3.0f - cos(2.0f * theta1 - 2.0f * theta2)));

    float alpha2 = (2.0f * sin(theta1 - theta2) * (2.0f * omega1 * omega1 * l + 
                    2.0f * g * cos(theta1) + omega2 * omega2 * l * cos(theta1 - theta2))) / 
                   (l * (3.0f - cos(2.0f * theta1 - 2.0f * theta2)));

    omega1 += alpha1 * timeStep;
    omega2 += alpha2 * timeStep;

    theta1 += omega1 * timeStep;
    theta2 += omega2 * timeStep;

    x1 = l * sin(theta1);
    y1 = l * cos(theta1);
    x2 = l * sin(theta2) + x1;
    y2 = l * cos(theta2) + y1;

}

int main()
{
    float theta1 = 45.0;  // Initial angle in degrees
    float theta2 = 45.0;  // Initial angle in degrees
    float omega1 = 0.0;   // Initial angular velocity
    float omega2 = 0.0;   // Initial angular velocity
    float x1, y1, x2, y2; // Positions of the pendulum bobs

    cout << "Enter the angle in degrees: ";
    cin >> theta1 >> theta2;
    theta1 = theta1 * M_PI / 180.0; // Convert to radians
    theta2 = theta2 * M_PI / 180.0; // Convert to radians

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Pendulum Simulation");
    // window.setFramerateLimit(1 / timeStep);

    // The pendulum
    sf::CircleShape bob1(10), bob2(10);
    bob1.setFillColor(sf::Color::Red);
    bob2.setFillColor(sf::Color::Red);

    // The joint
    sf::CircleShape joint(5);
    joint.setFillColor(sf::Color::Blue);
    joint.setPosition({400 - 5, 300 - 5});

    // The rod
    sf::Vertex line1[] =
        {
            sf::Vertex{sf::Vector2f(400, 300)},
            sf::Vertex{sf::Vector2f(400, 300)}};
    sf::Vertex line2[] =
        {
            sf::Vertex{sf::Vector2f(400, 300)},
            sf::Vertex{sf::Vector2f(400, 300)}};

    sf::Clock fpsClock;
    int frameCount = 0;

    float px,py;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        px = x2;
        py = y2;

        calc_position(theta1, theta2, omega1, omega2, x1, y1, x2, y2);

        bob1.setPosition({400 + x1 * 100 - 10, 300 + y1 * 100 - 10});
        line1[1].position = line2[0].position = sf::Vector2f(400 + x1 * 100, 300 + y1 * 100);
        bob2.setPosition({400 + x2 * 100 - 10, 300 + y2 * 100 - 10});
        line2[1].position = sf::Vector2f(400 + x2 * 100, 300 + y2 * 100);

        sf::Vertex trailLine[] =
        {
            sf::Vertex{sf::Vector2f(400 + px * 100, 300 + py * 100)},
            sf::Vertex{sf::Vector2f(400 + x2 * 100, 300 + y2 * 100)}};

        ++frameCount;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f)
        {
            window.setTitle("Pendulum Simulation - FPS: " + std::to_string(frameCount));
            frameCount = 0;
            fpsClock.restart();
        }

        // window.clear();
        // window.draw(line1, 2, sf::PrimitiveType::Lines);
        // window.draw(line2, 2, sf::PrimitiveType::Lines);
        // window.draw(bob1);
        // window.draw(bob2);
        if (frameCount % 5 == 0)
        {window.draw(trailLine, 2, sf::PrimitiveType::Lines);}
        // window.draw(joint);
        window.display();
    }
}