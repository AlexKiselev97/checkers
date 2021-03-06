// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <SFML/Graphics.hpp>
#include "Checker.h"
#include "Checkers SFML.h"
#include <iostream>
#include <ctime>
#include <clocale>
#include <array>
#include <algorithm>

sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Checkers by Alex Kiselev", sf::Style::Fullscreen);
Checker* board[8][8]; // доска с указателями на шашки
std::array<Checker, 24> checkers; // непосредственно шашки
Game theGame;
const unsigned int FONT_SIZE = window.getSize().x / 38;
const auto AI_DELAY = 5000; // задержка ожидания бота в мс

int main()
{
	std::setlocale(LC_ALL, "Russian");
	srand(time(0)); // true random
	/// WINDOW INIT
	window.setMouseCursorVisible(false); // скрываем системный курсор
	window.setFramerateLimit(60); // ограничение 60 фпс
	window.setVerticalSyncEnabled(true);
	// дисплей с соотношением сторон 4:3 или 5:4
	bool squareDisplay = float(window.getSize().x) / float(window.getSize().y) < 1.76f;

	sf::Image icon;
	icon.loadFromFile("images/png2ico.png");
	window.setIcon(192, 192, icon.getPixelsPtr());

	sf::Font font;
	font.loadFromFile("cambria.ttf");
	theGame.initProfile(font, FONT_SIZE);
	/// TEXTURES
	sf::Texture t_board; // текстура доски
	t_board.loadFromFile("images/board.png");

	sf::Texture t_background; // текстура фона
	t_background.loadFromFile("images/background.png");

	sf::Texture t_button; // текстура кнопки
	t_button.loadFromFile("images/button.png");
	t_button.setSmooth(true);

	sf::Texture t_squareButton; // текстура квадратной кнопки 
	t_squareButton.loadFromFile("images/squareButton.png");
	t_squareButton.setSmooth(true);

	sf::Texture t_cursor; // текстура курсора
	t_cursor.loadFromFile("images/cursor2_gray.png");
	t_cursor.setSmooth(true);

	sf::Texture t_arrow;
	t_arrow.loadFromFile("images/cancel_step3_gray.png");
	t_arrow.setSmooth(true);

	sf::Texture t_volumeOn;
	t_volumeOn.loadFromFile("images/volumeOn.png");
	sf::Texture t_volumeOff;
	t_volumeOff.loadFromFile("images/volumeOff.png");

	sf::Texture t_colorWhite;
	t_colorWhite.loadFromFile("images/color_white.png");
	sf::Texture t_colorBlack;
	t_colorBlack.loadFromFile("images/color_black.png");
	sf::Texture t_colorRand;
	t_colorRand.loadFromFile("images/color_rand.png");

	sf::Texture t_white, t_black, t_blackKing, t_whiteKing; // текстуры пешек
	t_white.loadFromFile("images/white.png");
	t_white.setSmooth(true);
	t_black.loadFromFile("images/black.png");
	t_black.setSmooth(true);
	t_blackKing.loadFromFile("images/blackKing.png");
	t_blackKing.setSmooth(true);
	t_whiteKing.loadFromFile("images/whiteKing.png");
	t_whiteKing.setSmooth(true);

	sf::Texture t_activeBlack, t_activeWhite, t_activeWhiteKing, t_activeBlackKing; // подсвеченные текстуры пешек
	t_activeBlack.loadFromFile("images/activeBlack.png");
	t_activeBlack.setSmooth(true);
	t_activeWhite.loadFromFile("images/activeWhite.png");
	t_activeWhite.setSmooth(true);
	t_activeWhiteKing.loadFromFile("images/activeWhiteKing.png");
	t_activeWhiteKing.setSmooth(true);
	t_activeBlackKing.loadFromFile("images/activeBlackKing.png");
	t_activeBlackKing.setSmooth(true);
	/// SPRITES
	sf::Sprite s_cursor(t_cursor); // курсор
	s_cursor.setScale(window.getSize().x / (s_cursor.getGlobalBounds().width * 28.f),
		window.getSize().x / (s_cursor.getGlobalBounds().height * 28.f));

	sf::Sprite s_board(t_board); // шахматная доска
	s_board.setScale(sf::Vector2f(float(window.getSize().y) / float(t_board.getSize().y),
		float(window.getSize().y) / float(t_board.getSize().y)));
	if (!squareDisplay)
		theGame.setBeginBoard((window.getSize().x - window.getSize().y) / 2);
	s_board.setPosition(sf::Vector2f(theGame.getBeginBoard(), 0.f));

	sf::Sprite s_background(t_background);
	s_background.setScale(sf::Vector2f(float(window.getSize().x) / float(t_background.getSize().x),
		float(window.getSize().y) / float(t_background.getSize().y)));

	sf::Sprite bgForStatistic(t_squareButton);
	if (squareDisplay)
		bgForStatistic.setScale(window.getSize().x / bgForStatistic.getGlobalBounds().width / 1.87f,
			window.getSize().y / bgForStatistic.getGlobalBounds().height / 2.7f);
	else
		bgForStatistic.setScale(window.getSize().x / bgForStatistic.getGlobalBounds().width / 1.87f,
			window.getSize().y / bgForStatistic.getGlobalBounds().height / 2.f);
	bgForStatistic.setPosition(window.getSize().x / 11, window.getSize().y / 2.4f);
	/// MENU
	auto menuButtonScale = sf::Vector2f(window.getSize().x / (t_button.getSize().x * 3.3f),
		window.getSize().y / (t_button.getSize().y * 5.5f));
	auto roundedButtonScale = sf::Vector2f(window.getSize().x / (t_button.getSize().x * 5.3f),
		window.getSize().y / (t_button.getSize().y * 5.5f));

	Button returnButton;
	returnButton.initText(L"ВЕРНУТЬСЯ В ИГРУ", font, FONT_SIZE);
	returnButton.initSprite(t_button);
	returnButton.setSpriteScale(menuButtonScale);
	returnButton.setSpritePos((window.getSize().x - returnButton.getSpriteGB().width) / 2,
		window.getSize().y / 6 - returnButton.getSpriteGB().height / 3.5f);
	returnButton.setTextPos((window.getSize().x - returnButton.getTextGB().width) / 2,
		window.getSize().y / 6);

	Button startOneButton;
	startOneButton.initText(L"ОДИН ИГРОК", font, FONT_SIZE);
	startOneButton.initSprite(t_button);
	startOneButton.setSpriteScale(menuButtonScale);
	startOneButton.setSpritePos((window.getSize().x - startOneButton.getSpriteGB().width) / 2,
		window.getSize().y / 6 * 2 - startOneButton.getSpriteGB().height / 3.5f);
	startOneButton.setTextPos((window.getSize().x - startOneButton.getTextGB().width) / 2,
		window.getSize().y / 6 * 2);

	Button startTwoButton;
	startTwoButton.initText(L"ДВА ИГРОКА", font, FONT_SIZE);
	startTwoButton.initSprite(t_button);
	startTwoButton.setSpriteScale(menuButtonScale);
	startTwoButton.setSpritePos((window.getSize().x - startTwoButton.getSpriteGB().width) / 2,
		window.getSize().y / 6 * 3 - startTwoButton.getSpriteGB().height / 3.5f);
	startTwoButton.setTextPos((window.getSize().x - startTwoButton.getTextGB().width) / 2,
		window.getSize().y / 6 * 3);

	Button settingsButton;
	settingsButton.initText(L"НАСТРОЙКИ", font, FONT_SIZE);
	settingsButton.setTextPos((window.getSize().x - settingsButton.getTextGB().width) / 2,
		window.getSize().y / 6 * 4);
	settingsButton.initSprite(t_button);
	settingsButton.setSpriteScale(menuButtonScale);
	settingsButton.setSpritePos((window.getSize().x - settingsButton.getSpriteGB().width) / 2,
		window.getSize().y / 6 * 4 - settingsButton.getSpriteGB().height / 3.5f);

	Button exitButton;
	exitButton.initText(L"ВЫХОД", font, FONT_SIZE);
	exitButton.setTextPos((window.getSize().x - exitButton.getTextGB().width) / 2, window.getSize().y / 6 * 5);
	exitButton.initSprite(t_button);
	exitButton.setSpriteScale(menuButtonScale);
	exitButton.setSpritePos((window.getSize().x - exitButton.getSpriteGB().width) / 2,
		window.getSize().y / 6 * 5 - exitButton.getSpriteGB().height / 3.5f);

	Button profileButton;
	profileButton.initText(L"ПРОФИЛЬ", font, FONT_SIZE);
	profileButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - profileButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3);
	profileButton.initSprite(t_button);
	profileButton.setSpriteScale(roundedButtonScale);
	profileButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - profileButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3 - profileButton.getSpriteGB().height / 3.5f);
	/// PROFILE
	sf::Vector2f profileFieldScale = sf::Vector2f(float(window.getSize().x) / (t_button.getSize().x * 6.f),
		float(window.getSize().y) / (t_button.getSize().y * 15.f));

	Button loginNameButton;
	loginNameButton.initSprite(t_squareButton);
	loginNameButton.setSpriteScale(profileFieldScale);
	loginNameButton.setSpritePos(window.getSize().x / 10, window.getSize().y / 6);
	loginNameButton.initText(L"ЛОГИН", font, FONT_SIZE);
	loginNameButton.setTextPos(window.getSize().x / 10 + loginNameButton.getSpriteGB().width / 17,
		window.getSize().y / 6 + loginNameButton.getSpriteGB().height / 4);

	Button loginPassButton;
	loginPassButton.initSprite(t_squareButton);
	loginPassButton.setSpriteScale(profileFieldScale);
	loginPassButton.setSpritePos((window.getSize().x - loginPassButton.getSpriteGB().width) / 2, window.getSize().y / 6);
	loginPassButton.initText(L"ПАРОЛЬ", font, FONT_SIZE);
	loginPassButton.setTextPos((window.getSize().x - loginPassButton.getSpriteGB().width) / 2 + loginPassButton.getSpriteGB().width / 17,
		window.getSize().y / 6 + loginPassButton.getSpriteGB().height / 4);

	Button loginButton;
	loginButton.initSprite(t_button);
	loginButton.setSpriteScale(roundedButtonScale);
	loginButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - loginButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 - loginButton.getSpriteGB().height / 1.5f);
	loginButton.initText(L"ВОЙТИ", font, FONT_SIZE);
	loginButton.setTextPos(loginButton.getSpriteGB().left + (loginButton.getSpriteGB().width - loginButton.getTextGB().width) / 2,
		loginButton.getSpriteGB().top + (loginButton.getSpriteGB().height - loginButton.getTextGB().height * 1.5f) / 2);

	Button resetButton;
	resetButton.initSprite(t_button);
	resetButton.setSpriteScale(roundedButtonScale);
	resetButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - resetButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4*2 - resetButton.getSpriteGB().height / 1.5f);
	resetButton.initText(L"СБРОСИТЬ", font, FONT_SIZE);
	resetButton.setTextPos(resetButton.getSpriteGB().left + (resetButton.getSpriteGB().width - resetButton.getTextGB().width) / 2,
		resetButton.getSpriteGB().top + (resetButton.getSpriteGB().height - resetButton.getTextGB().height * 1.5f) / 2);
	/// SETTINGS
	sf::Sprite s_volume(t_volumeOn);
	s_volume.setScale(window.getSize().x / (t_volumeOn.getSize().x * 5.f),
		window.getSize().x / (t_volumeOn.getSize().y * 5.f));
	s_volume.setPosition(window.getSize().x / 4, window.getSize().y / 3.f);

	sf::Sprite s_color(t_colorRand);
	s_color.setScale(window.getSize().x / (t_colorWhite.getSize().x * 5.f),
		window.getSize().x / (t_colorWhite.getSize().y * 7.f));
	s_color.setPosition(window.getSize().x / 2.f, s_volume.getGlobalBounds().top + s_volume.getGlobalBounds().height / 2 - s_color.getGlobalBounds().height / 2);
	/// OTHER BUTTONS
	Button menuButton;
	menuButton.initText(L"МЕНЮ", font, FONT_SIZE);
	menuButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - menuButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3);
	menuButton.initSprite(t_button);
	menuButton.setSpriteScale(roundedButtonScale);
	menuButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - menuButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3 - menuButton.getSpriteGB().height / 3.5f);

	Button toMenuButton;
	toMenuButton.initSprite(t_button);
	toMenuButton.setSpriteScale(roundedButtonScale);
	toMenuButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - toMenuButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3 - toMenuButton.getSpriteGB().height / 3.5f);
	toMenuButton.initText(L"НАЗАД", font, FONT_SIZE);
	toMenuButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - toMenuButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 4 * 3);

	Button cancelStepButton;
	cancelStepButton.initSprite(t_squareButton);
	if (squareDisplay)
		cancelStepButton.setSpriteScale(sf::Vector2f(window.getSize().x / cancelStepButton.getSpriteGB().width / 8.1f,
			window.getSize().y / cancelStepButton.getSpriteGB().height / 6.5f));
	else
		cancelStepButton.setSpriteScale(sf::Vector2f(window.getSize().x / cancelStepButton.getSpriteGB().width / 8.1f,
			window.getSize().y / cancelStepButton.getSpriteGB().height / 4.575f));
	cancelStepButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - cancelStepButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 12);
	cancelStepButton.initText(L"3", font, FONT_SIZE* 1.3f);
	cancelStepButton.setTextPos(cancelStepButton.getSpriteGB().left + (cancelStepButton.getSpriteGB().width - cancelStepButton.getTextGB().width) / 2 - cancelStepButton.getTextGB().width / 5,
		cancelStepButton.getSpriteGB().top + (cancelStepButton.getSpriteGB().height - cancelStepButton.getTextGB().height) / 2 - cancelStepButton.getTextGB().height / 4);

	sf::Sprite s_cancelStepArrow(t_arrow);
	s_cancelStepArrow.setScale(sf::Vector2f(window.getSize().x / (s_cancelStepArrow.getGlobalBounds().width * 10.f),
		window.getSize().x / (s_cancelStepArrow.getGlobalBounds().height * 10.f)));
	s_cancelStepArrow.setPosition(cancelStepButton.getSpriteGB().left + (cancelStepButton.getSpriteGB().width - s_cancelStepArrow.getGlobalBounds().width) / 2,
		cancelStepButton.getSpriteGB().top + (cancelStepButton.getSpriteGB().height - s_cancelStepArrow.getGlobalBounds().height) / 2);

	Button statusButton;
	statusButton.initText(L"ХОД ЧЕРНЫХ", font, FONT_SIZE / 3 * 2);
	statusButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - statusButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 2 - statusButton.getTextGB().height / 2);
	statusButton.initSprite(t_squareButton);
	statusButton.setSpriteScale(sf::Vector2f(float(window.getSize().x) / 8 / t_squareButton.getSize().x,
		float(window.getSize().y) / 8 / t_squareButton.getSize().y));
	statusButton.setSpritePos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - statusButton.getSpriteGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		window.getSize().y / 2 - statusButton.getSpriteGB().height / 2);

	sf::Text scores;
	scores.setFont(font);
	scores.setString(L"0\n\n\n\n0");
	scores.setCharacterSize(FONT_SIZE * 3);
	scores.setFillColor(sf::Color::White);
	scores.setPosition(theGame.getBeginBoard() / 6, window.getSize().y / 11);
	/// SOUNDS
	sf::SoundBuffer attackBuffer;
	attackBuffer.loadFromFile("sounds/attack.wav");
	sf::Sound attackSound(attackBuffer);

	sf::SoundBuffer stepBuffer;
	stepBuffer.loadFromFile("sounds/step.wav");
	sf::Sound stepSound(stepBuffer);

	/// OTHER
	sf::RectangleShape karetka(sf::Vector2f(loginNameButton.getTextGB().height, 2));
	karetka.rotate(90.f);
	std::clock_t start_AI_delay = std::clock();
	std::clock_t start = 0;
	bool karetkaVisible = false;
	std::string copyPass; // копия вводимого пароля
	while (window.isOpen()) // основной цикл программы
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		sf::Event event;
		while (window.pollEvent(event))
		{ // обработка событий
			if (event.type == sf::Event::Closed || theGame.getWindowClose())
			{
				theGame.addProfileTime();
				theGame.saveProfile();
				window.close();
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (theGame.getLoginNameActivity())
				{
					float oldWidth = loginNameButton.getTextGB().width;
					if (event.text.unicode == 13 || event.text.unicode == 9)
					{ // enter or tab key
						theGame.setLoginNameActivity(false);
						theGame.setLoginPassActivity(true);
						copyPass.clear();
						loginPassButton.setString(L"");
						karetka.setPosition(loginPassButton.getTextGB().left + FONT_SIZE / 8, loginPassButton.getTextGB().top + loginNameButton.getSpriteGB().height / 7.5f);
					}
					else if (event.text.unicode == '\b')
					{ // backspace key
						sf::String temp = loginNameButton.getString();
						if (temp.getSize() > 0)
						{
							temp.erase(temp.getSize() - 1);
							loginNameButton.setString(temp);
						}
					}
					else
					{ // input text
						if (loginNameButton.getString().getSize() < 13)
							loginNameButton.setString(loginNameButton.getString() + event.text.unicode);
					}
					karetka.move(loginNameButton.getTextGB().width - oldWidth, 0);
				}
				else if (theGame.getLoginPassActivity())
				{
					float oldWidth = loginPassButton.getTextGB().width;
					if (event.text.unicode == 13 || event.text.unicode == 9)
					{ // enter or tab key
						theGame.setLoginPassActivity(false);
						karetkaVisible = false;
					}
					else if (event.text.unicode == '\b')
					{ // backspace key
						sf::String temp = loginPassButton.getString();
						if (temp.getSize() > 0)
						{
							copyPass.pop_back();
							temp.erase(temp.getSize() - 1);
							loginPassButton.setString(temp);
						}
					}
					else
					{ // input text
						if (loginPassButton.getString().getSize() < 13)
						{
							loginPassButton.setString(loginPassButton.getString() + '*');
							copyPass.push_back(event.text.unicode);
						}
					}
					karetka.move(loginPassButton.getTextGB().width - oldWidth, 0);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (theGame.getGameOn())
					{
						if (s_board.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
							if (!squareDisplay)
								theGame.clickOnBoard(mousePosition, s_background, s_board, statusButton, menuButton, attackSound, stepSound, t_activeBlackKing, t_activeWhiteKing, t_activeBlack, t_activeWhite, t_blackKing, t_whiteKing, t_black, t_white, s_cursor, cancelStepButton, s_cancelStepArrow, &scores);
							else
								theGame.clickOnBoard(mousePosition, s_background, s_board, statusButton, menuButton, attackSound, stepSound, t_activeBlackKing, t_activeWhiteKing, t_activeBlack, t_activeWhite, t_blackKing, t_whiteKing, t_black, t_white, s_cursor, cancelStepButton, s_cancelStepArrow);
						if (menuButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.flipGameOn();
						if (cancelStepButton.getSpriteGB().contains(mousePosition.x, mousePosition.y) && theGame.getPlayAlone())
							if (theGame.getWinner() == Game::Nobody && theGame.cancelStep(statusButton, s_board))
								cancelStepButton.setString(std::to_wstring(theGame.getAmountCancel()));
					}
					else if (theGame.getMenuOn())
					{
						if (returnButton.getSpriteGB().contains(mousePosition.x, mousePosition.y) && theGame.getReturnButtonActivity())
							theGame.flipGameOn();
						if (startOneButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.startGame(1, returnButton, startOneButton, startTwoButton, exitButton, statusButton,
								cancelStepButton, s_board, t_white, t_black);
						if (startTwoButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.startGame(2, returnButton, startOneButton, startTwoButton, exitButton, statusButton,
								cancelStepButton, s_board, t_white, t_black);
						if (settingsButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.flipSettingsActivity();
						if (exitButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))	
							theGame.setWindowClose(true);
						if (profileButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
						{
							theGame.flipProfileActivity();
							theGame.addProfileTime();
							theGame.updateProfileField();
						}
					}
					else if (theGame.getProfileActivity())
					{
						if (loginNameButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
						{
							theGame.setLoginNameActivity(true);
							loginNameButton.setString(L"");
							copyPass.clear();
							loginPassButton.setString(L"");
							karetka.setPosition(loginNameButton.getTextGB().left + FONT_SIZE / 8, loginNameButton.getTextGB().top + loginNameButton.getSpriteGB().height / 7.5f);
						}
						if (loginPassButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
						{
							theGame.setLoginNameActivity(false);
							theGame.setLoginPassActivity(true);
							copyPass.clear();
							loginPassButton.setString(L"");
							karetka.setPosition(loginPassButton.getTextGB().left + FONT_SIZE / 8, loginPassButton.getTextGB().top + loginNameButton.getSpriteGB().height / 7.5f);
						}
						if (loginButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
						{
							theGame.setLoginPassActivity(false);
							karetkaVisible = false;
							if (theGame.login(loginNameButton, copyPass))
								theGame.clockTime();
							
						}
						if (toMenuButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.flipProfileActivity();
						if (resetButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.resetProfile();
					}
					else if (theGame.getSettingsActivity())
					{
						if (toMenuButton.getSpriteGB().contains(mousePosition.x, mousePosition.y))
							theGame.flipSettingsActivity();
						if (s_volume.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
						{
							theGame.flipVolume();
							if (theGame.getVolume())
							{
								s_volume.setTexture(t_volumeOn);
								attackSound.play();
							}
							else
								s_volume.setTexture(t_volumeOff);
						}
						if (s_color.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
						{
							if (s_color.getTexture() == &t_colorRand)
							{
								s_color.setTexture(t_colorWhite);
								theGame.setSettingColor('w');
							}
							else if (s_color.getTexture() == &t_colorWhite)
							{
								s_color.setTexture(t_colorBlack);
								theGame.setSettingColor('b');
							}
							else
							{
								s_color.setTexture(t_colorRand);
								theGame.setSettingColor('r');
							}
						}
					}
				}
			}
		}
		if (theGame.getPlayAlone() && theGame.getAiTurn() && theGame.getWinner() == Game::Nobody && theGame.getGameOn())
		{ // ход ИИ
			if (start_AI_delay == -1)
				start_AI_delay = std::clock();
			else if (std::clock() - start_AI_delay > AI_DELAY) // задержка в 5 секунд
			{
				theGame.setStepCompleted(false);
				if (theGame.setAnybodyMustAttack(checkMustAttack(theGame.get_pAttackChecker())))
				{
					if (!squareDisplay)
						AIattack(s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, s_cancelStepArrow, &scores);
					else
						AIattack(s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, s_cancelStepArrow);
				}
				else
				{
					if (!squareDisplay)
						AIstep(s_background, s_board, statusButton, menuButton, s_cursor, mousePosition, cancelStepButton, s_cancelStepArrow, &scores);
					else
						AIstep(s_background, s_board, statusButton, menuButton, s_cursor, mousePosition, cancelStepButton, s_cancelStepArrow);
					if (theGame.getStepCompleted() && theGame.getVolume())
						stepSound.play();
				}
			}
		}
		if (theGame.getGameOn() && theGame.getWinner() == Game::Nobody)
		{ // Обработка игры
			if (theGame.getStepCompleted() && !(checkMustAttack(theGame.get_pAttackChecker()) && theGame.getAnybodyMustAttack()))
			{ // Смена хода
				for (int i = 0; i < 24; ++i)
					checkers[i].setAttack(false);
				unclickChecker(t_blackKing, t_whiteKing, t_black, t_white);
				if (theGame.getPlayersTurn() == Game::Human)
					theGame.setHumanDoneStep(true);
				else
					theGame.setHumanDoneStep(false);
				theGame.set_pSelectedChecker(nullptr);
				theGame.set_pAttackChecker(nullptr);
				theGame.setStepCompleted(false);
				start_AI_delay = -1;
				theGame.changeTurn(statusButton, s_board);
				statusButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - statusButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
					window.getSize().y / 2 - statusButton.getTextGB().height / 2);
				theGame.setCheckerSelected(false);
			}
			int howMuchEatDown = std::count_if(checkers.begin(), checkers.begin() + 12,
				[](Checker& checker) { return checker.posX == -10; });
			int howMuchEatUp = std::count_if(checkers.begin() + 12, checkers.end(),
				[](Checker& checker) { return checker.posX == -10; });
			scores.setString(std::to_string(howMuchEatUp) + "\n\n\n\n" + std::to_string(howMuchEatDown));
			checkersToKing(t_blackKing, t_whiteKing, t_activeBlackKing, t_activeWhiteKing);
			if (!checkMustAttack())
			{ // Определение победителя
				theGame.whoIsWinner();
				if (theGame.getWinner() != Game::Nobody)
				{
					if (theGame.getWinner() == Game::Human)
					{
						if (theGame.getPlayerColor(Game::Human) == Game::White)
							statusButton.setString(L"ВЫИГРАЛИ\n   БЕЛЫЕ");
						else
							statusButton.setString(L"ВЫИГРАЛИ\n  ЧЕРНЫЕ");
						theGame.addWin();
					}
					else
					{
						if (theGame.getPlayerColor(Game::AI) == Game::White)
							statusButton.setString(L"ВЫИГРАЛИ\n   БЕЛЫЕ");
						else
							statusButton.setString(L"ВЫИГРАЛИ\n  ЧЕРНЫЕ");
						theGame.addLose();
					}
					statusButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - statusButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
						window.getSize().y / 2 - statusButton.getTextGB().height / 2);
					theGame.saveProfile();
				}
			}
		}
		/// ОТРИСОВКА
		window.clear();
		if (theGame.getGameOn())
		{ // отрисовать игру
			if (!squareDisplay)
				drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, s_cancelStepArrow, &scores);
			else
				drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, s_cancelStepArrow);
		}
		else if (theGame.getSettingsActivity())
		{ // отрисовать настройки
			window.draw(s_background);
			toMenuButton.draw();
			window.draw(s_volume);
			window.draw(s_color);
		}
		else if (theGame.getProfileActivity())
		{ // отрисовать профиль
			window.draw(s_background);
			window.draw(bgForStatistic);
			theGame.drawProfile();
			loginNameButton.draw();
			loginPassButton.draw();
			loginButton.draw();
			toMenuButton.draw();
			resetButton.draw();
			if (theGame.getLoginNameActivity() || theGame.getLoginPassActivity())
			{ // отрисовка каретки
				if (std::clock() - start >= 500)
				{
					start = std::clock();
					karetkaVisible = !karetkaVisible;
				}
				if (karetkaVisible)
					window.draw(karetka);
			}
		}
		else
		{ // отрисовать меню
			window.draw(s_background);
			if (theGame.getReturnButtonActivity())
				returnButton.draw();
			startOneButton.draw();
			startTwoButton.draw();
			settingsButton.draw();
			exitButton.draw();
			profileButton.draw();
		}
		drawCursor(s_cursor);
		window.display();
	}
	return 0;
}