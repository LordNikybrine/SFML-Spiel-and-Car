#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <Windows.h>
#include <thread>

int main() {
    std::string comPort;
    std::cout << "Enter the COM port (e.g., COM5): ";
    std::cin >> comPort;

    std::wstring comPortWide = L"\\\\.\\" + std::wstring(comPort.begin(), comPort.end());

    bool xBigger100 = true;
    bool zBigger100 = true;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Joystick Input");

    // Joystick-Verbindung überprüfen
    if (!sf::Joystick::isConnected(0)) {
        std::cout << "Kein Joystick verbunden!" << std::endl;
        return 1;
    }

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

    COMMTIMEOUTS timeout = { 0 };
    timeout.ReadIntervalTimeout = 50;
    timeout.ReadTotalTimeoutConstant = 50;
    timeout.ReadTotalTimeoutMultiplier = 50;
    timeout.WriteTotalTimeoutConstant = 50;
    timeout.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(serialHandle, &timeout);

    while (window.isOpen()) {
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

            // Convert integer values to strings
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

            // Send data over the serial port
            DWORD bytesWritten;
            WriteFile(serialHandle, xAxisStr.c_str(), xAxisStr.length(), &bytesWritten, NULL);
            WriteFile(serialHandle, " ", 1, &bytesWritten, NULL); // Separator
            WriteFile(serialHandle, gasStr.c_str(), gasStr.length(), &bytesWritten, NULL);
            WriteFile(serialHandle, "\n", 1, &bytesWritten, NULL); // New line
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }

        window.clear(); 

        window.display();

    }

    return 0;
}
