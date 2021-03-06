// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"
#include "Checkers SFML.h"
#include <iostream>
#include <fstream>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

extern sf::RenderWindow window;
extern std::array<Checker, 24> checkers;
extern Checker* board[8][8];
extern Game theGame;

/* Инициализирует профиль
Устанавливает шрифт и размер текста, задает позиции
Устанавливает заголовок, winrate, win, lose, time */
void Game::initProfile(sf::Font & f, int size)
{
	profile.title.setString(L"ВХОД НЕ ВЫПОЛНЕН");
	profile.winrateText.setString(L"ПРОЦЕНТ ПОБЕД: ");
	profile.winText.setString(L"ИГР ВЫИГРАНО: ");
	profile.loseText.setString(L"ИГР ПРОИГРАНО: ");
	profile.timeText.setString(L"В ИГРЕ: ");
	profile.title.setFont(f);
	profile.winrateText.setFont(f);
	profile.winText.setFont(f);
	profile.loseText.setFont(f);
	profile.timeText.setFont(f);
	profile.title.setCharacterSize(size);
	profile.winrateText.setCharacterSize(size);
	profile.winText.setCharacterSize(size);
	profile.loseText.setCharacterSize(size);
	profile.timeText.setCharacterSize(size);
	profile.title.setFillColor(sf::Color::White);
	profile.winrateText.setFillColor(sf::Color::White);
	profile.winText.setFillColor(sf::Color::White);
	profile.loseText.setFillColor(sf::Color::White);
	profile.timeText.setFillColor(sf::Color::White);
	profile.title.setPosition((window.getSize().x - profile.title.getGlobalBounds().width) / 2, window.getSize().y / 13);
	profile.winrateText.setPosition(window.getSize().x / 8, window.getSize().y / 2);
	profile.winText.setPosition(window.getSize().x / 8, profile.winrateText.getGlobalBounds().top + profile.winrateText.getGlobalBounds().height * 1.5f);
	profile.loseText.setPosition(window.getSize().x / 8, profile.winText.getGlobalBounds().top + profile.winrateText.getGlobalBounds().height * 1.5f);
	profile.timeText.setPosition(window.getSize().x / 8, profile.loseText.getGlobalBounds().top + profile.winrateText.getGlobalBounds().height * 1.5f);
}

// Отрисовывает все элементы профиля
void Game::drawProfile() const
{
	window.draw(profile.title);
	window.draw(profile.winrateText);
	window.draw(profile.winText);
	window.draw(profile.loseText);
	window.draw(profile.timeText);
}

// Инициализирует новую игру в зависимости от режима и настроек
void Game::startGame(int playersNumber, Button& returnButton, Button& startOneButton, Button& startTwoButton, Button& exitButton,
	Button& statusButton, Button& cancelStepButton, sf::Sprite& s_board, sf::Texture& t_white, sf::Texture& t_black)
{
	/// установка позиций интерфейса
	returnButtonActivity = true;
	numberOfPlayers = playersNumber;
	gameOn = true;
	players[Human] = Color::Black; // нижний игрок
	players[AI] = Color::White; // верхний игрок
	amountCancel = 3;
	cancelStepButton.setString(L"3");
	switch (playerColor)
	{
	case 'r':
		if (rand() % 2) // рандом цвета игроков
			std::swap(players[0], players[1]);
		break;
	case 'w':
		players[Human] = White;
		players[AI] = Black;
		break;
	case 'b':
		players[Human] = Black;
		players[AI] = White;
		break;
	}
	
	!(rand() % 2) ? playersTurn = Player::Human : playersTurn = Player::AI;
	this->changeTurn(statusButton, s_board);
	
	pSelectedChecker = nullptr;
	pAttackChecker = nullptr;
	checkerSelected = false;
	stepCompleted = false;
	anybodyMustAttack = false;
	winner = Player::Nobody;
	checkersInit(beginBoard, t_white, t_black);
}

/* Отменяет ход, если это доступно
Возвращает успешность отмены */
bool Game::cancelStep(Button& statusButton, sf::Sprite& s_board)
{
	if (amountCancel > 0 && humanDoneStep)
	{
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
			{
				board[i][j] = nullptr;
			}
		for_each(checkers.begin(), checkers.end(), [](Checker& ch) { ch.returnCondition(); });
		--amountCancel;
		changeTurn(statusButton, s_board);
		humanDoneStep = false;
		return true;
	}
	return false;
}

// Обрабатывает нажатие на доску
void Game::clickOnBoard(sf::Vector2i& mousePos, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton,
	Button& menuButton, sf::Sound& attackSound, sf::Sound& stepSound, sf::Texture& t_activeBlackQueen, sf::Texture& t_activeWhiteQueen,
	sf::Texture& t_activeBlack, sf::Texture& t_activeWhite, sf::Texture& t_blackQueen, sf::Texture& t_whiteQueen,
	sf::Texture& t_black, sf::Texture& t_white, sf::Sprite& s_cursor, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text* scores)
{
	if (winner == Player::Nobody && gameOn)
	{
		if (!getPlayAlone() || getPlayAlone() && playersTurn == Player::Human)
		{
			// конвертация в позицию на доске
			sf::Vector2i posOnBoard = toBoardPos(mousePos);
			
			if (anybodyMustAttack = checkMustAttack(pAttackChecker))
			{ // если должен атаковать игрок цвета playerTurn
				if (!checkerSelected)
				{
					if (board[posOnBoard.x][posOnBoard.y] != nullptr && board[posOnBoard.x][posOnBoard.y]->getAttack())
						if (clickChecker(posOnBoard, t_activeBlackQueen, t_activeWhiteQueen, t_activeBlack, t_activeWhite))
						{
							checkerSelected = true;
							pAttackChecker = board[posOnBoard.x][posOnBoard.y];
							pSelectedChecker = board[posOnBoard.x][posOnBoard.y];
						}
				}
				else
				{
					checkerSelected = clickSelf(mousePos, pAttackChecker, t_blackQueen, t_whiteQueen, t_black, t_white);
					if (checkerSelected)
					{
						if (getPlayAlone())
							std::for_each(checkers.begin(), checkers.end(), [](Checker& ch) { ch.saveCondition(); });
						if (pAttackChecker->getKing())
						{
							Checker* enemy = pAttackChecker->checkKingAttack(posOnBoard);
							stepCompleted = pAttackChecker->attack("queen", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores, enemy, posOnBoard);
						}
						else
						{
							std::string sideToAttack = pAttackChecker->checkAttack(posOnBoard);
							stepCompleted = pAttackChecker->attack(sideToAttack, s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores);
						}
					}
				}
			}
			else
			{ // если не должен атаковать
				if (!checkerSelected)
				{
					if (clickChecker(posOnBoard, t_activeBlackQueen, t_activeWhiteQueen, t_activeBlack, t_activeWhite))
					{
						checkerSelected = true;
						pSelectedChecker = board[posOnBoard.x][posOnBoard.y];
					}
				}
				else
				{ // пешка выбрана
					if (board[posOnBoard.x][posOnBoard.y] != nullptr)
					{ // была выбрана другая пешка
						if (board[posOnBoard.x][posOnBoard.y]->getOwner() == pSelectedChecker->getOwner())
						{
							checkerSelected = clickSelf(mousePos, pSelectedChecker, t_blackQueen, t_whiteQueen, t_black, t_white);
							if (checkerSelected)
							{
								unclickChecker(t_blackQueen, t_whiteQueen, t_black, t_white);
								clickChecker(posOnBoard, t_activeBlackQueen, t_activeWhiteQueen, t_activeBlack, t_activeWhite);
								pSelectedChecker = board[posOnBoard.x][posOnBoard.y];
							}
						}

					}
					else
					{ // клик на пустую клетку
						if (getPlayAlone())
							std::for_each(checkers.begin(), checkers.end(), [](Checker& ch) { ch.saveCondition(); });
						std::string sideToStep = pSelectedChecker->checkStep(posOnBoard);
						stepCompleted = pSelectedChecker->step(sideToStep, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, *scores, posOnBoard);
						if (stepCompleted && volume)
							stepSound.play();
					}
				}
			}
		}
	}
}

/* Меняет очередь хода
Изменяет надпись в статусе */
void Game::changeTurn(Button& statusButton, sf::Sprite& s_board)
{
	if (playersTurn == Player::Human)
		playersTurn = Player::AI;
	else
		playersTurn = Player::Human;

	if (this->getPlayAlone())
	{
		if (theGame.getPlayersTurn() == Game::Human)
		{
			if (checkMustAttack(theGame.get_pAttackChecker()))
				statusButton.setString(L"ВЫ АТАКУЕТЕ");
			else
				statusButton.setString(L"ВЫ ХОДИТЕ");
		}
		else
			statusButton.setString(L"          ХОД\nПРОТИВНИКА");
	}
	else
	{
		if (statusButton.getString() == L"ХОД БЕЛЫХ")
			statusButton.setString(L"ХОД ЧЕРНЫХ");
		else
			statusButton.setString(L"ХОД БЕЛЫХ");
	}
	statusButton.setTextPos((window.getSize().x - theGame.getBeginBoard() - s_board.getGlobalBounds().width) / 2 - statusButton.getTextGB().width / 2 + theGame.getBeginBoard() + s_board.getGlobalBounds().width,
		statusButton.getSpriteGB().top + (statusButton.getSpriteGB().height - statusButton.getTextGB().height) / 2);
}

/* Выясняет кто является победителем
ПОБОЧНЫЙ ЭФФЕКТ: устанавливает в поле winner победителя
Возвращает победителя */
Game::Player Game::whoIsWinner()
{
	bool playerTwoCanStep = false;
	for (int i = 0; i < 12; ++i)
	{
		if (checkers[i].posX != -10)
		{
			if (checkers[i].getKing() && checkers[i].canKingStep())
			{
				playerTwoCanStep = true;
				break;
			}
			if (checkers[i].canStepLeftDown()
				|| checkers[i].canStepRightDown())
			{
				playerTwoCanStep = true;
				break;
			}
		}

	}
	if (!playerTwoCanStep)
		return winner = Game::Player::Human;
	bool playerOneCanStep = false;
	for (int i = 12; i < 24; ++i)
	{
		if (checkers[i].posX != -10)
		{
			if (checkers[i].getKing() && checkers[i].canKingStep())
			{
				playerOneCanStep = true;
				break;
			}
			if (checkers[i].canStepLeftUp()
				|| checkers[i].canStepRightUp())
			{
				playerOneCanStep = true;
				break;
			}
		}

	}
	if (!playerOneCanStep)
		return winner = Game::Player::AI;
	else
		return winner = Game::Player::Nobody;
}

// Обновляет поля статистики профиля
void Game::updateProfileField()
{
	if (profile.win + profile.lose == 0)
		profile.winrate = 0.f;
	else
		profile.winrate = float(profile.win) / (profile.win + profile.lose);
	std::wstring winrate = std::to_wstring(profile.winrate * 100);
	while (winrate.size() > 5)
		winrate.erase(5);
	profile.winrateText.setString(L"ПРОЦЕНТ ПОБЕД: " + winrate + L"%");
	profile.winText.setString(L"ИГР ВЫИГРАНО: " + std::to_wstring(profile.win));
	profile.loseText.setString(L"ИГР ПРОИГРАНО: " + std::to_wstring(profile.lose));
	profile.timeText.setString(L"В ИГРЕ: " + std::to_wstring(profile.time / 60) + L" ЧАСОВ, " + std::to_wstring(profile.time % 60) + L" МИНУТ");
}

/* Выполняет процедуру входа в профиль
Возвращает успешность входа */
bool Game::login(Button& loginButton, std::string& passButton)
{
	std::cout << "click on login\n";
	std::ifstream fin("users.txt");
	std::string login = toStdString(loginButton.getString());
	std::size_t hash = std::hash<std::string>()(passButton + login);
	bool loginFinded = false;
	std::string in;
	while (std::getline(fin, in))
	{
		if (login == in)
		{
			loginFinded = true;
			break;
		}
	}
	if (loginFinded)
	{
		std::cout << "login finded\n";
		std::cout << hash << std::endl;
		bool hashFinded = false;
		std::size_t hashIn;
		fin >> hashIn;
		hashFinded = (hash == hashIn);
		fin.close();
		if (hashFinded)
		{
			profile.name = login;
			std::cout << "hash finded\n";
			profile.title.setString(L"Добро пожаловать, " + loginButton.getString());
			profile.title.setPosition((window.getSize().x - profile.title.getGlobalBounds().width) / 2, window.getSize().y / 13);
			std::ifstream fprofile("profiles/" + login + ".txt");
			std::string temp_win, temp_lose, temp_time;
			fprofile >> temp_win >> temp_lose >> temp_time;
			fprofile.close();
			profile.win = std::stoi(caesarCipher(temp_win, -10));
			profile.lose = std::stoi(caesarCipher(temp_lose, -10));
			profile.time = std::stoi(caesarCipher(temp_time, -10));
			updateProfileField();
			return true;
		}
		else
		{
			profile.title.setString(L"Неверный пароль");
			profile.title.setPosition((window.getSize().x - profile.title.getGlobalBounds().width) / 2, window.getSize().y / 13);
			return false;
		}
	}
	else
	{
		fin.close();
		std::cout << "new acc\n";
		std::ofstream fout;
		fout.open("users.txt", std::ofstream::app);
		fout << login << std::endl;
		fout << hash << std::endl;
		fout.close();
		profile.name = login;
		profile.title.setString(L"Аккаунт " + loginButton.getString() + L" создан");
		profile.title.setPosition((window.getSize().x - profile.title.getGlobalBounds().width) / 2, window.getSize().y / 13);
		profile.winrate = 0;
		profile.win = 0;
		profile.lose = 0;
		profile.time = 0;
		updateProfileField();
		saveProfile();
		return false;
	}
}

// Добавляет время проведенное в игре
void Game::addProfileTime()
{
	profile.time += (std::clock() - startTime) / 1000 / 60; // минуты
	clockTime();
}

// Начинает отсчет времени в игре
void Game::clockTime()
{
	startTime = std::clock();
}

// Сохраняет профиль в файл (поддерживается шифрование)
void Game::saveProfile() const
{
	std::ofstream fprofile;
	fprofile.open("profiles/" + profile.name + ".txt");
	fprofile << caesarCipher(std::to_string(profile.win), 10) << ' ' << caesarCipher(std::to_string(profile.lose), 10) << ' ' << caesarCipher(std::to_string(profile.time), 10) << std::endl;
	fprofile.close();
}

// Сбрасывает победы и поражения профиля
void Game::resetProfile()
{
	profile.lose = 0;
	profile.win = 0;
	updateProfileField();
}

// Button class
void Button::initText(const sf::String& string, const sf::Font& font, unsigned int characterSize, const sf::Color& color)
{
	text.setString(string);
	text.setFont(font);
	text.setCharacterSize(characterSize);
	text.setFillColor(color);
}

void Button::draw()
{
	window.draw(sprite);
	window.draw(text);
}