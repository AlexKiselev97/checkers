#ifndef CHECKER_H_
#define CHECKER_H_

#include "Game.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct CheckerCondition
{
	int posX, posY;
	sf::Vector2f spritePos;
	bool king;
};

class Checker
{
private:
	sf::Sprite sprite;
	bool king = false;
	bool mustAttack = false;
	Game::Player owner;
	char color;
	int rating = 0;
	Checker* pEnemy = nullptr; // указатель на срубаемую шашку (для дамок)
	std::string sideToAttack = "";
	std::string bestSideToStep = "";
	CheckerCondition oldCondition;
public:
	int posX, posY;
	friend void checkersInit(const float& beginBoard, sf::Texture& t_white, sf::Texture& t_black);
	/// GETTERS
	auto getEnemy() { return pEnemy; }
	auto getSideToAttack() { return sideToAttack; }
	auto getBestSideToStep() { return bestSideToStep; }
	auto getKing() const { return king; }
	auto getAttack() const { return mustAttack; }
	auto getRating() { return rating; }
	auto getOwner() const { return owner; }
	auto getColor() { return color; }
	bool getMustAttack();
	auto getGB() { return sprite.getGlobalBounds(); }
	/// SETTERS
	void setOwner(Game::Player o) { owner = o; }
	void setColor(char c) { color = c; }
	void setAttack(bool b) { mustAttack = b; }
	void setTexture(sf::Texture& t) { sprite.setTexture(t); }
	void setScale(const sf::Vector2f& v) { sprite.setScale(v); }
	void setPosition(float x, float y) { sprite.setPosition(x, y); }
	/// CONDITION
	void saveCondition();
	void returnCondition();
	/// METHODS
	void makeKing(const sf::Texture& t_blackKing, const sf::Texture& t_whiteKing, const sf::Texture& t_activeBlackKing, const sf::Texture& t_activeWhiteKing);
	bool step(const std::string& side, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
		Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores, const sf::Vector2i& pos = sf::Vector2i(0, 0));
	bool attack(const std::string& side, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
		sf::Sound& attackSound, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores,
		Checker* enemy = nullptr, sf::Vector2i& posMouseOnBoard = sf::Vector2i(0, 0));
	void findBestStep();
	void move(float x, float y) { sprite.move(x, y); }
	void draw(sf::RenderWindow& w) { w.draw(sprite); }
	void animation(int x_coef, int y_coef, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
		Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores, int delay);
	/// CHECK ATTACK
	bool checkAttackLeftUp(Checker* const board[8][8]) const;
	bool checkAttackRightUp(Checker* const board[8][8]) const;
	bool checkAttackLeftDown(Checker* const board[8][8]) const;
	bool checkAttackRightDown(Checker* const board[8][8]) const;
	std::string checkAttack(const sf::Vector2i& pos) const;
	bool isKingAttack();
	Checker* checkKingAttack(const sf::Vector2i& pos) const;
	/// CHECK STEP
	bool canStepLeftDown() const;
	bool canStepRightDown() const;
	bool canStepLeftUp() const;
	bool canStepRightUp() const;
	bool canKingStep() const;
	std::string checkStep(sf::Vector2i& pos) const;
	bool checkStepKing(const sf::Vector2i& posOnBoard) const;

	bool operator== (Checker& ch) { return posX == ch.posX && posY == ch.posY; }
};
#endif