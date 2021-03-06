#ifndef GAME_H_
#define GAME_H_

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

static const int templateBoard[8][8] =
{ 0,-1, 0,-1, 0,-1, 0,-1,
 -1, 0,-1, 0,-1, 0,-1, 0,
 0,-1, 0,-1, 0,-1, 0,-1,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 1, 0, 1, 0, 1, 0,
 0, 1, 0, 1, 0, 1, 0, 1,
 1, 0, 1, 0, 1, 0, 1, 0 };

class Checker;
class Button;

struct Profile
{
	sf::Text title;
	sf::Text winrateText;
	sf::Text winText;
	sf::Text loseText;
	sf::Text timeText;
	std::string name;
	float winrate;
	int win;
	int lose;
	int time;
};

class Game
{
public:
	enum Player { Human, AI, Nobody };
	enum Color { Black, White };
private:
	char playerColor = 'r'; // цвет шашек для игрока
	bool volume = true; // on/off
	float beginBoard = 0.f; // начало доски по горизонтали
	bool returnButtonActivity = false; // показывать ли кнопку вернуться
	bool windowClose = false; // флаг закрытия окна
	/// ACTIVITIES
	bool settingsActivity = false;
	bool profileActivity = false;
	bool loginNameActivity = false;
	bool loginPassActivity = false;
	/// GAME PROPERTIES
	bool gameOn = false; // игра идет?
	bool checkerSelected = false; // шашка выбрана?
	bool anybodyMustAttack = false; // кто либо должен атаковать
	bool stepCompleted = false; // шаг завершен
	bool humanDoneStep = false;
	int numberOfPlayers = 0;
	Checker* pSelectedChecker = nullptr; // указатель на выбраную шашку
	Checker* pAttackChecker = nullptr; // указатель на атакующую шашку
	Color players[2]{ Black, White }; // цвета игроков
	Player playersTurn = Nobody; // очередь хода
	Player winner = Nobody;
	int amountCancel; // количество отмен хода
	/// PROFILE
	Profile profile;
	std::clock_t startTime; // время для отсчета времени в игре
public:
	/// VOLUME FLAG
	auto getVolume() const { return volume; }
	void flipVolume() { volume = !volume; }
	/// SETTINGS ACTIVITY FLAG
	auto getSettingsActivity() const { return settingsActivity; }
	void flipSettingsActivity() { settingsActivity = !settingsActivity; }
	/// PROFILE ACTIVITY FLAG
	auto getProfileActivity() const { return profileActivity; }
	void flipProfileActivity() { profileActivity = !profileActivity; }
	/// LOGIN NAME ACTIVITY FLAG
	auto getLoginNameActivity() const { return loginNameActivity; }
	void setLoginNameActivity(bool b) { loginNameActivity = b; }
	/// LOGIN PASSWORD ACTIVITY FLAG
	auto getLoginPassActivity() const { return loginPassActivity; }
	void setLoginPassActivity(bool b) { loginPassActivity = b; }
	/// GAME ON FLAG
	auto getGameOn() const { return gameOn; }
	void setGameOn(bool b) { gameOn = b; }
	void flipGameOn() { gameOn = !gameOn; }
	/// CHECKER SELECTED FLAG
	auto getCheckerSelected() const { return checkerSelected; }
	void setCheckerSelected(bool b) { checkerSelected = b; }
	/// ANYBODY MUST ATTACK FLAG
	auto getAnybodyMustAttack() const { return anybodyMustAttack; }
	auto setAnybodyMustAttack(bool b) { return anybodyMustAttack = b; }
	/// STEP COMPLETED FLAG
	auto getStepCompleted() const { return stepCompleted; }
	auto setStepCompleted(bool b) { return stepCompleted = b; }
	/// WINDOW CLOSE FLAG
	auto getWindowClose() const { return windowClose; }
	void setWindowClose(bool b) { windowClose = b; }
	/// BEGIN BOARD
	auto getBeginBoard() const { return beginBoard; }
	void setBeginBoard(float bb) { beginBoard = bb; }
	/// ATTACK CHECKER POINTER
	auto get_pAttackChecker() const { return pAttackChecker; }
	void set_pAttackChecker(Checker* p) { pAttackChecker = p; }
	/// SELECTED CHECKER POINTER
	auto get_pSelectedChecker() const { return pSelectedChecker; }
	void set_pSelectedChecker(Checker* p) { pSelectedChecker = p; }
	/// OTHER GETTERS
	auto getWinner() const { return winner; }
	auto getReturnButtonActivity() const { return returnButtonActivity; }
	auto getHumanTurn() const { return playersTurn == Player::Human; }
	auto getAiTurn() const { return playersTurn == Player::AI; }
	auto getPlayersTurn() const { return playersTurn; }
	auto getMenuOn() const { return !gameOn && !settingsActivity && !profileActivity; }
	auto getPlayAlone() const { return numberOfPlayers == 1; }
	auto getPlayerColor(Player g) const { return players[g]; }
	auto getAmountCancel() const { return amountCancel; }
	/// OTHER SETTERS
	void setHumanDoneStep(bool b) { humanDoneStep = b; }
	void setSettingColor(char c) { playerColor = c; }
	/// METHODS
	void changeTurn(Button& turnText, sf::Sprite& s_board);
	Game::Player whoIsWinner();
	void startGame(int playerNumber, Button& returnButton, Button& startOneButton, Button& startTwoButton, Button& exitButton,
		Button& cancelStepButton, Button& statusButton, sf::Sprite& s_board, sf::Texture& t_white, sf::Texture& t_black);
	bool cancelStep(Button& statusButton, sf::Sprite& s_board);
	void clickOnBoard(sf::Vector2i& mousePos, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
		sf::Sound& attackSound, sf::Sound& stepSound, sf::Texture& t_activeBlackKing, sf::Texture& t_activeWhiteKing,
		sf::Texture& t_activeBlack, sf::Texture& t_activeWhite, sf::Texture& t_blackKing, sf::Texture& t_whiteKing, sf::Texture& t_black,
		sf::Texture& t_white, sf::Sprite& s_cursor, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text* scores = nullptr);
	/// PROFILE METHODS
	void initProfile(sf::Font& f, int size);
	void drawProfile() const;
	void updateProfileField();
	bool login(Button& login, std::string& pass);
	void addProfileTime();
	void clockTime();
	void saveProfile() const;
	void resetProfile();
	void addWin() { profile.win++; }
	void addLose() { profile.lose++; }
};

class Button
{
private:
	sf::Sprite sprite;
	sf::Text text;
public:
	void draw();
	sf::String getString() { return text.getString(); }
	// Sprite
	void initSprite(const sf::Texture& texture) { sprite.setTexture(texture); }
	void setSpritePos(float x, float y) { sprite.setPosition(x, y); }
	void setSpriteScale(const sf::Vector2f& factors) { sprite.setScale(factors.x, factors.y); }
	void moveSprite(float x, float y) { sprite.move(sf::Vector2f(x, y)); }
	sf::FloatRect getSpriteGB() { return sprite.getGlobalBounds(); } // get global bounds
	// Text
	void initText(const sf::String& string, const sf::Font& font, unsigned int characterSize = 30, const sf::Color& color = sf::Color::White);
	void setTextPos(float x, float y) { text.setPosition(x, y); }
	void setTextScale(const sf::Vector2f& factors) { text.setScale(factors.x, factors.y); }
	void setString(const sf::String& str) { text.setString(str); }
	sf::FloatRect getTextGB() { return text.getGlobalBounds(); } // get global bounds
};
#endif