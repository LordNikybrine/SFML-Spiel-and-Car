#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <Windows.h>
#include <thread>

int main() {
	bool xBigger100 = true;
	bool zBigger100 = true;

	std::cout << "In the following window the color GREEN = xAxis" << std::endl
		<< "and the color BLUE = zAxis/gas" << std::endl
		<< std::endl
		<< "To end the programm press ESC" << std::endl;

	std::string comPort;
	std::cout << "Enter the COM port (e.g., COM5): ";
	std::cin >> comPort;
	while (!(comPort.compare(0, 3, "COM") == 0)) {
		std::cout << "Port needs to start with COM." << std::endl << "COM Port: ";
		std::cin >> comPort;
	}

	std::wstring comPortWide = L"\\\\.\\" + std::wstring(comPort.begin(), comPort.end());

	sf::Joystick::update();
	if (!sf::Joystick::isConnected(0)) {
		std::cout << "Kein Joystick verbunden!" << std::endl;
		return 1;
	}

	sf::RenderWindow window(sf::VideoMode(800, 600), "Joystick Input");

	//port configuriern & öffnen
	HANDLE serialHandle;

	serialHandle = CreateFile(comPortWide.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serialHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to open the serial port." << std::endl;
		return 1;
	}

	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 11520;
	serialParams.ByteSize = 8;
	serialParams.StopBits = TWOSTOPBITS;
	serialParams.Parity = NOPARITY;
	SetCommState(serialHandle, &serialParams);

	//write und read haben 50ms timeout
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serialHandle, &timeout);

	sf::RectangleShape xAxisBackground(sf::Vector2f(200, 20));  // Width and height of the bar
	xAxisBackground.setFillColor(sf::Color::White);
	xAxisBackground.setPosition(50, 50);  // Adjust the position as needed

	sf::RectangleShape gasBarBackground(sf::Vector2f(199, 20));
	gasBarBackground.setFillColor(sf::Color::White);
	gasBarBackground.setPosition(50, 100);

	sf::RectangleShape xAxisBar(sf::Vector2f(200, 20));  // Width and height of the bar
	xAxisBar.setFillColor(sf::Color::Green);
	xAxisBar.setPosition(50, 50);  // Adjust the position as needed

	sf::RectangleShape gasBar(sf::Vector2f(200, 20));
	gasBar.setFillColor(sf::Color::Blue);
	gasBar.setPosition(50, 100);  // Adjust the position as needed

	while (!(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) && window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			// Beenden des Programms beim Schließen des Fensters
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			// Abrufen der X-Achsenposition
			int xAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X) + 100;
			int gas = sf::Joystick::getAxisPosition(0, sf::Joystick::Z) + 100;

			// Ausgabe der Position auf der Konsole
			//std::cout << "X-Achsenposition: " << xAxis << " Z dings: " << gas << std::endl;

			if (xAxis < 100) {
				xBigger100 = false;
			}
			if (gas < 100) {
				zBigger100 = false;
			}
			std::string xAxisStr = std::to_string(xAxis);
			std::string gasStr = std::to_string(gas);

			xAxisStr = "1" + xAxisStr;
			gasStr = "2" + gasStr;

			if (!xBigger100) {
				xAxisStr = std::to_string(xAxis);
				xAxisStr.insert(xAxisStr.begin(), '1');
				while (xAxisStr.length() < 4) {
					xAxisStr.insert(1, "0");
				}
				xBigger100 = true;
			}

			if (!zBigger100) {
				gasStr = std::to_string(gas);
				gasStr.insert(gasStr.begin(), '2');
				while (gasStr.length() < 4) {
					gasStr.insert(1, "0");
				}
				zBigger100 = true;
			}

			std::cout << xAxisStr << ", " << gasStr << std::endl;

			// daten senden
			DWORD bytesWritten;
			WriteFile(serialHandle, xAxisStr.c_str(), xAxisStr.length(), &bytesWritten, NULL);
			WriteFile(serialHandle, " ", 1, &bytesWritten, NULL); // Separator
			WriteFile(serialHandle, gasStr.c_str(), gasStr.length(), &bytesWritten, NULL);
			WriteFile(serialHandle, "\n", 1, &bytesWritten, NULL); // New line
			std::this_thread::sleep_for(std::chrono::milliseconds(75));


			xAxisBar.setSize(sf::Vector2f(xAxis, 20));
			gasBar.setSize(sf::Vector2f(gas, 20));

			window.clear(sf::Color::Black);

			// Draw the bars in the window
			window.draw(xAxisBackground);
			window.draw(gasBarBackground);
			window.draw(xAxisBar);
			window.draw(gasBar);

			window.display();
		}
	}
	return 0;
}