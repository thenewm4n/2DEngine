#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class NewShape
{
public:
	std::unique_ptr<sf::Shape> shape;
	bool draw = true;
	int velocityX;
	int velocityY;
	std::string text;

public:
	// Constructor needs to take in all parameters

	// NewShape(std::string shapeName)
	// {
	//		shape(shapeName);
	// }
};

int main(int argc, char* argv[])
{
	std::ifstream file("config.txt");
	if (!file.is_open())
	{
		std::cerr << "Error opening file." << std::endl;
		exit(-2);
	}

	sf::Font myFont;
	if (!myFont.loadFromFile("fonts/tech.ttf"))
	{
		std::cerr << "Could not load font!\n";
		exit(-1);
	}

	// Vector of shapes
	std::vector<std::unique_ptr<NewShape>> shapes;

	// For window; defaults to these unless changed by file
	int wWidth = 1920;
	int wHeight = 1080;

	// For font
	std::string fontFilename;
	int textSize;
	int textColour[3];

	std::string line;
	std::string firstElement;
	/*
	while (std::getline(file, line))
	{
		std::string shapeText;
		float positionX, positionY, velocityX, velocityY, radius, width, height;
		int red, green, blue;

		// Decomposes line
		std::istringstream lineStream(line);
		if (lineStream >> firstElement)
		{
			if (firstElement == "Window")
			{
				lineStream >> wWidth >> wHeight;
			}
			else if (firstElement == "Font")
			{
				lineStream >> fontFilename >> textSize >> textColour[0] >> textColour[1] >> textColour[2];
			}
			else if (firstElement == "Rectangle")
			{
				std::unique_ptr<NewShape> shapePointer = std::make_unique<NewShape>(); // parameters go in brackets

				// Set attributes according to file 
				// lineStream >> shapeText >> positionX >> positionY >> velocityX >> velocityY >> red >> green >> blue >> width >> height
			}
			else if (firstElement == "Circle")
			{
				std::unique_ptr<NewShape> shapePointer = std::make_unique<NewShape>(); // parameters go in brackets

				// Set attributes according to file 
				// lineStream >> shapeText >> positionX >> positionY >> velocityX >> velocityY >> red >> green >> blue >> radius
			}
		}
	}
	*/

	file.close();
	
	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
	window.setFramerateLimit(60);

	ImGui::SFML::Init(window);
	sf::Clock deltaClock;

	ImGui::GetStyle().ScaleAllSizes(5.0f);	// Scales imgui GUI
	ImGui::GetIO().FontGlobalScale = 2.0f;		// Scale imgui text size

	float c[3] = { 0.0f, 1.0f, 1.0f };	// imgui requires RGB values as floats from 0-1

	float circleRadius = 50.0f;
	int circleSegments = 32;		// the number of segments to draw the circle with i.e. how detailed?
	float circleSpeedX = 1.0f;
	float circleSpeedY = 0.5f;
	bool drawCircle = true;
	bool drawText = true;

	// This will get replaced by dynamic instantiation of objects, I believe
	sf::CircleShape circle(circleRadius, circleSegments);
	circle.setPosition(10.0f, 10.0f);


	// Text that will be for each shape
	sf::Text text("Sample Text", myFont, 24);

	text.setPosition(0, wHeight - (float)text.getCharacterSize());

	// Array for imgui text
	char displayString[255] = "Sample Text";

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Passes the event to imgui; needs to know whether it has been clicked
			ImGui::SFML::ProcessEvent(window, event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				std::cout << "Key pressed with code = " << event.key.code << "\n";

				// Examples of changing direction of shape when X is pressed
				if (event.key.code == sf::Keyboard::X)
				{
					circleSpeedX *= -1.0f;
				}
			}
		}

		// Updates imgui for this frame with the time the last frame took; .restart() returns the sf::Time object and restarts the clock
		ImGui::SFML::Update(window, deltaClock.restart());

		// Draw the UI
		ImGui::Begin("Window title");
		ImGui::Text("Window text!");
		ImGui::Checkbox("Draw Circle", &drawCircle);
		ImGui::SameLine();
		ImGui::Checkbox("Draw Text", &drawText);
		ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
		ImGui::SliderInt("Sides", &circleSegments, 3, 64);
		ImGui::ColorEdit3("Color Circle", c);
		ImGui::InputText("Text", displayString, 255);
		if (ImGui::Button("Set Text"))			// if the button "Set Text" is clicked, set the text to the input text
		{
			text.setString(displayString);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Circle"))
		{
			circle.setPosition(0, 0);
		}
		ImGui::End();

		circle.setFillColor(sf::Color(c[0] * 255, c[1] * 255, c[2] * 255));		// convert from a value 0-1 to 0-255
		circle.setPointCount(circleSegments);
		circle.setRadius(circleRadius);

		// Moves the circle, as long as the x and y positions are within from
		circle.setPosition(circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY);

		window.clear();
		if (drawCircle)
		{
			window.draw(circle);
		}
		if (drawText)
		{
			window.draw(text);
		}
		ImGui::SFML::Render(window);		// Render the imgui GUI to the SFML window
		window.display();			// Switches the front and back buffers
	}

	return 0;
}

// void setWindowAndFont (