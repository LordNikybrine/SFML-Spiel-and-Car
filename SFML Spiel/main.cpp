#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>

class Game {
public:
	//public funktionen der klasse
	Game();
	void run();

private:
	//private funktionen der klasse 
	void processEvents();
	void check_player_movement();
	void enemys();
	void check_for_hit();
	void updateHitsText();
	void shot();
	void update();
	void render();
	void updateShot();
	bool checkCollision();

private:

	// fenster erstellen, spieler (rectangle erstellen), gegner erstellen und startgeschwindigkeit und direction für gegner erstellen
	sf::RenderWindow window;
	sf::RectangleShape rectangle;
	sf::CircleShape bullet;
	sf::CircleShape enemy;
	sf::Vector2f direction = { 0.2f, 0.2f };

	// font, texture und text erstellen
	sf::Font font;
	sf::Texture texture;
	sf::Text hitsText;

	// startzeit erstellen
	std::chrono::steady_clock::time_point startTime;

	// wichtig zum hits zählen
	bool wasColliding = false;

	bool shott = false;

	//berüjhrugs variable deklarieren
	int hits = 0;

	//gegner und spieler geschwindigkeit einstellen
	float enemySpeed = 0.5;
	float playerSpeed = 0.2;

};

Game::Game() : window(sf::VideoMode(1680 / 2, 1050 / 2), "Spiel", sf::Style::Default, sf::ContextSettings(0, 0, 8)) {
	//Antialiasing auf stufe 8 einstellen
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;


	// Schriftart und spieler Textur laden
	if (!font.loadFromFile("./assets/Roboto-Regular.ttf")) {
		//! Irgendwie fehler behandeln
	}
	hitsText.setFont(font);
	hitsText.setCharacterSize(24);
	hitsText.setFillColor(sf::Color::White);
	hitsText.setPosition(10, 10);

	if (!texture.loadFromFile("./assets/player.png")) {
		//! Irgendwie fehler behandeln
	}
	rectangle.setPosition(sf::Vector2f(250, 250));
	texture.setSmooth(true);
	rectangle.setTexture(&texture);

	// spieler und gegner farbe und größe setzen
	enemy.setRadius(25);
	enemy.setFillColor(sf::Color::Red);
	rectangle.setSize(sf::Vector2f(50, 50));
	rectangle.setPosition(sf::Vector2f(250, 250));

	//startzeit setzen
	startTime = std::chrono::steady_clock::now();

	/*enemySpeed = window.getSize().x * 0.0004;
	playerSpeed = window.getSize().x * 0.0002;*/
}

void Game::processEvents() {
	//event erstellen und nach events fragen
	sf::Event event;
	while (window.pollEvent(event)) {
		// wenn event = window::closed das febster schließen
		if (event.type == sf::Event::Closed) {
			window.close();
		}
		if (event.type == sf::Event::Resized) {
			//TODO: proportionen beibehalten
		}
	}
}

void Game::run() {
	//spiel starten/ausführen
	while (window.isOpen()) {
		processEvents();
		update();
		render();
	}
}

void Game::check_player_movement() {
	//spieler bewegungen ausführen
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		rectangle.move(-playerSpeed, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		rectangle.move(playerSpeed, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		rectangle.move(0, -playerSpeed);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		rectangle.move(0, playerSpeed);

	//spieler innerhalb des fensters lassen
	if (rectangle.getPosition().x < 0) {
		rectangle.setPosition(0, rectangle.getPosition().y);
	}
	if (rectangle.getPosition().x > window.getSize().x - 50) {
		rectangle.setPosition(window.getSize().x - 50, rectangle.getPosition().y);
	}

	if (rectangle.getPosition().y < 0) {
		rectangle.setPosition(rectangle.getPosition().x, 0);
	}
	if (rectangle.getPosition().y > window.getSize().y - 50) {
		rectangle.setPosition(rectangle.getPosition().x, window.getSize().y - 50);
	}
}

void Game::enemys() {
	// aktuelle zeit checken
	auto currentTime = std::chrono::steady_clock::now();

	// differenz zwischen jetzt und startzeit
	double elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

	// Geschwindigkeit basierend auf der vergangenen Spielzeit anpassen
	float adjustedEnemySpeed = enemySpeed + (elapsedTime / 60.0);
	if (adjustedEnemySpeed >= 2) {
		adjustedEnemySpeed = 2;
	}

	//gegner bewegen
	enemy.move(direction.x * adjustedEnemySpeed, direction.y * adjustedEnemySpeed);
	//enemy.move(direction.x * enemySpeed, direction.y * enemySpeed);

	//dafür sorgen, dass er im fenster abprallt
	if (enemy.getPosition().x < 0 || enemy.getPosition().x > window.getSize().x) {
		direction.x = -direction.x;
	}

	if (enemy.getPosition().y < 0 || enemy.getPosition().y > window.getSize().y) {
		direction.y = -direction.y;
	}

	//schießen / schuss updaten
	updateShot();
	int elapsedTimeInt = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
	if (!(elapsedTimeInt % 2) && !shott && elapsedTimeInt != 0) {
		shott = true;
		std::cout << " schießen ";
		shot();
	}
	else if (elapsedTimeInt % 2) {
		shott = false;
	}
}

void Game::shot() {
	int x = enemy.getPosition().x;
	int y = enemy.getPosition().y;
	bullet.setRadius(15);
	bullet.setFillColor(sf::Color::Blue);
	bullet.setPosition(x, y);

	x += direction.x * 2;
	y += direction.y * 2;

	bullet.setPosition(x, y);
}

void Game::updateShot() {
	if (bullet.getRadius() > 0) {
		float bulletSpeed = 2.0;

		bullet.move(direction.x * bulletSpeed, direction.y * bulletSpeed);
	}
}

bool Game::checkCollision() {
	if (enemy.getGlobalBounds().intersects(rectangle.getGlobalBounds()) || bullet.getGlobalBounds().intersects(rectangle.getGlobalBounds())) {
		return true;
	}

	return false;
}
// hits zählen
void Game::check_for_hit(){
	bool isColliding = checkCollision();

	if (isColliding && !wasColliding) {
		hits++;
	}

	wasColliding = isColliding;
}

void Game::updateHitsText() {
	//hits als text anzeigen
	hitsText.setString("Hits: " + std::to_string(hits));
}

void Game::update() {
	// alles aktualisieren
	check_player_movement();
	enemys();
	check_for_hit();
	updateHitsText();
}

void Game::render() {
	//erst bildschrim leeren
	window.clear(sf::Color::Black);
	//alles zeichnen
	window.draw(bullet);
	window.draw(enemy);
	window.draw(rectangle);
	window.draw(hitsText);
	//das gezeichnete anzeigen
	window.display();
}

int main() {
	//spiel starten
	Game game;
	game.run();

	return 0;
}
