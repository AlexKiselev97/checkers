#ifndef CHECKERS_SFML_H_
#define CHECKERS_SFML_H_

#include "Checker.h"
#include <iostream>
#include <array>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

extern std::array<Checker, 24> checkers;
extern Checker* board[8][8];
extern sf::RenderWindow window;
extern const int templateBoard[8][8];
extern Game theGame;

/* Выполняет шифрование Цезаря
Возвращает зашифрованную строку */
static std::string caesarCipher(std::string s, int offset)
{
	for_each(s.begin(), s.end(), [offset](char& c) { c = c + offset; });
	return s;
}

/* Преобразовывает строку sf::String в std::string
Возвращает преобразованную строку */
static std::string toStdString(sf::String input)
{
	auto s = input.toWideString();
	std::string result;
	for (auto& c : s)
		if (c >= 1040 && c <= 1103) // латиница
			result.push_back(char(c - 848)); // в std латиницу
		else
			result.push_back(c);
	return result;
}

// Отрисовывает спрайт курсора на текущей позиции мыши
static void drawCursor(sf::Sprite& s_cursor)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
	s_cursor.setPosition(mousePosition.x, mousePosition.y);
	window.draw(s_cursor);
}

// Отрисовывает фон, доску, пешки, статус, кнопку меню, отмену хода
static void drawGame(sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text* scores = nullptr)
{
	window.draw(s_background);
	window.draw(s_board);
	for (int i = 0; i < 24; ++i)
		if (checkers[i].posX != -10)
			checkers[i].draw(window);
	statusButton.draw();
	menuButton.draw();
	if (theGame.getPlayAlone())
	{
		cancelStepButton.draw();
		window.draw(cancelStepArrow);
	}
	if (scores != nullptr)
		window.draw(*scores);
}

/* Инициализирует пешки по шаблону в templateBoard.
Каждая пешка получает цвет, текстуру, владельца.
Адрес пешки копируется на доску.
Устанавливается scale, позиция, координаты, обнуляются свойства. */
static void checkersInit(const float& beginBoard, sf::Texture& t_white, sf::Texture& t_black)
{
	for (int i = 0, k = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
		{
			if (templateBoard[i][j] == -1)
			{
				if (theGame.getPlayerColor(Game::AI) == Game::White)
				{
					checkers[k].setTexture(t_white);
					checkers[k].setColor('w');
				}
				else
				{
					checkers[k].setTexture(t_black);
					checkers[k].setColor('b');
				}
				checkers[k].setOwner(Game::Player::AI);
			}
			else if (templateBoard[i][j] == 1)
			{
				if (theGame.getPlayerColor(Game::Human) == Game::Black)
				{
					checkers[k].setTexture(t_black);
					checkers[k].setColor('b');
				}
				else
				{
					checkers[k].setTexture(t_white);
					checkers[k].setColor('w');
				}
				checkers[k].setOwner(Game::Player::Human);
			}
			if (templateBoard[i][j] != 0)
			{
				board[i][j] = &checkers[k];
				checkers[k].setScale(sf::Vector2f(float(window.getSize().y / 8) / t_white.getSize().y,
					float(window.getSize().y) / 8 / t_white.getSize().y));
				checkers[k].setPosition(0, 0);
				checkers[k].move(beginBoard + float(window.getSize().y) / 8 * j, // двигаем на нужную клетку
					float(window.getSize().y) / 8 * i);
				checkers[k].posX = i;
				checkers[k].posY = j;
				checkers[k].king = false;
				checkers[k].mustAttack = false;
				checkers[k].pEnemy = nullptr;
				checkers[k].rating = 0;
				++k;
			}
			else
				board[i][j] = nullptr;
		}
}

/* Устанавливает подсвеченную текстуру по позиции на доске
Возвращает успешность */
static bool clickChecker(sf::Vector2i& pos, sf::Texture& t_activeBlackKing, sf::Texture& t_activeWhiteKing, sf::Texture& t_activeBlack, sf::Texture& t_activeWhite)
{
	if (board[pos.x][pos.y] == nullptr)
		return false;
	if (board[pos.x][pos.y]->getOwner() == theGame.getPlayersTurn())
	{
		if (board[pos.x][pos.y]->getKing())
		{
			if (board[pos.x][pos.y]->getColor() == 'b')
				board[pos.x][pos.y]->setTexture(t_activeBlackKing);
			else
				board[pos.x][pos.y]->setTexture(t_activeWhiteKing);
			return true;
		}
		else
		{
			if (board[pos.x][pos.y]->getColor() == 'b')
				board[pos.x][pos.y]->setTexture(t_activeBlack);
			else
				board[pos.x][pos.y]->setTexture(t_activeWhite);
			return true;
		}
	}
	return false;
}

/* Проверяет должен ли кто-либо атаковать
При передаче указателя на шашку проверяет должна ли атаковать именно эта шашка
ПОБОЧНЫЙ ЭФФЕКТ: если шашка должна атаковать, то ее свойство mustAttack меняется в true
Возвращает результат проверки */
static bool checkMustAttack(Checker* ch = nullptr)
{
	bool check = false;
	if (theGame.getPlayersTurn() == Game::Human)
	{
		for (int i = 12; i < 24; ++i)
			if (checkers[i].posX != -10 && checkers[i].getMustAttack() && (ch == nullptr || checkers[i] == *ch))
				check = true;
	}
	else
	{
		for (int i = 0; i < 12; ++i)
			if (checkers[i].posX != -10 && checkers[i].getMustAttack() && (ch == nullptr || checkers[i] == *ch))
				check = true;
	}
	return check;
}

/* Проверяет находится ли шашка с координатами i, j под возможной атакой пешки
Возвращает результат проверки */
static bool checkNotDanger(int i, int j)
{
	if (i == -10) // вне поля
		return true;

	if (i == 0 || i == 7 || j == 0 || j == 7) // с краю доски
		return true;

	bool mainDiagonalCheck = ((board[i + 1][j + 1] != nullptr && board[i - 1][j - 1] != nullptr) // с обоих сторон не пусто
		|| (board[i + 1][j + 1] == nullptr && board[i - 1][j - 1] == nullptr) // или с обоих сторон пусто
		|| (board[i + 1][j + 1] != nullptr && board[i + 1][j + 1]->getOwner() == board[i][j]->getOwner()) // или с одной стороны своя
		|| (board[i - 1][j - 1] != nullptr && board[i - 1][j - 1]->getOwner() == board[i][j]->getOwner())); // или с другой стороны своя

	bool secondaryDiagonalCheck = ((board[i - 1][j + 1] != nullptr && board[i + 1][j - 1] != nullptr) // с обоих сторон не пусто
		|| (board[i - 1][j + 1] == nullptr && board[i + 1][j - 1] == nullptr) // или с обоих сторон пусто
		|| (board[i - 1][j + 1] != nullptr && board[i - 1][j + 1]->getOwner() == board[i][j]->getOwner()) // или с одной стороны своя
		|| (board[i + 1][j - 1] != nullptr && board[i + 1][j - 1]->getOwner() == board[i][j]->getOwner())); // или другой стороны своя

	return mainDiagonalCheck && secondaryDiagonalCheck;
}

/* Преобразовывает позицию мыши в позицию на доске
Возвращает координаты клетки */
static sf::Vector2i toBoardPos(sf::Vector2i v)
{
	v.x -= theGame.getBeginBoard();
	v.x = trunc(v.x / (window.getSize().y / 8)); // здесь все верно
	v.y = trunc(v.y / (window.getSize().y / 8));
	std::swap(v.x, v.y);
	return v;
}

// Меняет текстуру на неподсвеченную в зависимости от типа пешки
static void unclickChecker(sf::Texture& t_blackKing, sf::Texture& t_whiteKing, sf::Texture& t_black, sf::Texture& t_white)
{
	if (theGame.get_pSelectedChecker()->getKing())
	{
		if (theGame.get_pSelectedChecker()->getColor() == 'b')
			theGame.get_pSelectedChecker()->setTexture(t_blackKing);
		else
			theGame.get_pSelectedChecker()->setTexture(t_whiteKing);
	}
	else
	{
		if (theGame.get_pSelectedChecker()->getColor() == 'b')
			theGame.get_pSelectedChecker()->setTexture(t_black);
		else
			theGame.get_pSelectedChecker()->setTexture(t_white);
	}
}

/* Проверяет на самоклик (клик на уже активную пешку)
Возвращает true - не самоклик или нулевой указатель
или false при самоклике */
static bool clickSelf(const sf::Vector2i& mousePos, Checker* activeChecker, sf::Texture& t_blackKing, sf::Texture& t_whiteKing, sf::Texture& t_black, sf::Texture& t_white)
{
	if (activeChecker == nullptr)
		return true;
	if (activeChecker->getGB().contains(mousePos.x, mousePos.y))
	{
		unclickChecker(t_blackKing, t_whiteKing, t_black, t_white);
		return false;
	}
	return true;
}

// Вызывает метод makeKing для всех пешек, которые достигли дамочного поля
static void checkersToKing(sf::Texture& t_blackKing, sf::Texture& t_whiteKing, sf::Texture& t_activeBlackKing, sf::Texture& t_activeWhiteKing)
{
	for (int i = 0; i < 24; ++i)
	{
		if (!checkers[i].getKing() && checkers[i].getOwner() == Game::Human && checkers[i].posX == 0)
			checkers[i].makeKing(t_blackKing, t_whiteKing, t_activeBlackKing, t_activeWhiteKing);
		else if (!checkers[i].getKing() && checkers[i].getOwner() == Game::AI && checkers[i].posX == 7)
			checkers[i].makeKing(t_blackKing, t_whiteKing, t_activeBlackKing, t_activeWhiteKing);
	}
}

/* Считает количество жертв начиная с состояния шашки c
Возвращает количество жертв */
static int countVictim(Checker& c, Checker* tempBoard[8][8])
{
	int rating = 0;
	while (c.checkAttackLeftUp(tempBoard) || c.checkAttackRightUp(tempBoard) || c.checkAttackLeftDown(tempBoard) || c.checkAttackRightDown(tempBoard))
	{
		if (c.checkAttackLeftDown(tempBoard))
		{
			++rating;
			tempBoard[c.posX + 1][c.posY - 1] = nullptr;
			c.posX += 2;
			c.posY -= 2;
		}
		if (c.checkAttackRightDown(tempBoard))
		{
			++rating;
			tempBoard[c.posX + 1][c.posY + 1] = nullptr;
			c.posX += 2;
			c.posY += 2;
		}
		if (c.checkAttackLeftUp(tempBoard))
		{
			++rating;
			tempBoard[c.posX - 1][c.posY - 1] = nullptr;
			c.posX -= 2;
			c.posY -= 2;
		}
		if (c.checkAttackRightUp(tempBoard))
		{
			++rating;
			tempBoard[c.posX - 1][c.posY + 1] = nullptr;
			c.posX -= 2;
			c.posY += 2;
		}
	}
	return rating;
}

/* Ищет длиннейшую цепочку атак
Возвращает верную сторону для атаки */
static std::string findSeries(Checker c)
{
	int ratingLeftDown = 0, ratingLeftUp = 0, ratingRightDown = 0, ratingRightUp = 0;
	Checker backupChecker = c;
	if (c.checkAttackLeftDown(board))
	{
		Checker* tempBoard[8][8]; // делаем копию доски
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				tempBoard[i][j] = board[i][j];
		++ratingLeftDown;
		tempBoard[c.posX + 1][c.posY - 1] = nullptr; // атакуем шашку
		c.posX += 2; // перемещаемся
		c.posY -= 2;
		ratingLeftDown += countVictim(c, tempBoard);
	}
	c = backupChecker;
	if (c.checkAttackRightDown(board))
	{
		Checker* tempBoard[8][8]; // делаем копию доски
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				tempBoard[i][j] = board[i][j];
		++ratingRightDown;
		tempBoard[c.posX + 1][c.posY + 1] = nullptr; // атакуем шашку
		c.posX += 2; // перемещаемся
		c.posY += 2;
		ratingRightDown += countVictim(c, tempBoard);
	}
	c = backupChecker;
	if (c.checkAttackLeftUp(board))
	{
		Checker* tempBoard[8][8]; // делаем копию доски
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				tempBoard[i][j] = board[i][j];
		++ratingLeftUp;
		tempBoard[c.posX - 1][c.posY - 1] = nullptr; // атакуем шашку
		c.posX -= 2; // перемещаемся
		c.posY -= 2;
		ratingLeftUp += countVictim(c, tempBoard);
	}
	c = backupChecker;
	if (c.checkAttackRightUp(board))
	{
		Checker* tempBoard[8][8]; // делаем копию доски
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				tempBoard[i][j] = board[i][j];
		++ratingRightUp;
		tempBoard[c.posX - 1][c.posY + 1] = nullptr; // атакуем шашку
		c.posX -= 2; // перемещаемся
		c.posY += 2;
		ratingRightUp += countVictim(c, tempBoard);
	}
	std::cout << "LD = " << ratingLeftDown << ", RD = " << ratingRightDown << ", LU = " << ratingLeftUp << ", RU = " << ratingRightUp << std::endl;
	std::string maxSide = "leftUp";
	int maxRating = ratingLeftUp;
	if (ratingRightUp > maxRating)
	{
		maxSide = "rightUp";
		maxRating = ratingRightUp;
	}
	if (ratingLeftDown >= maxRating)
	{
		maxSide = "leftDown";
		maxRating = ratingLeftDown;
	}
	if (ratingRightDown >= maxRating)
	{
		maxSide = "rightDown";
		maxRating = ratingRightDown;
	}
	std::cout << maxSide << std::endl;
	return maxSide;
}

/* Делает попытку атаковать шашкой из позиции board[x][y]
Если это возможно, то цепочка найдена
Делает откат состояния и атакует в позицию для цепочки */
static void attempKingAttack(Checker& checker, int x, int y, const Checker& backupChecker, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton, sf::Sound& attackSound,
	sf::Sprite& s_cursor, sf::Vector2i mousePosition, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text& scores)
{
	checker.posX = x;
	checker.posY = y;
	if (checker.checkAttackLeftUp(board) || checker.checkAttackRightUp(board) || checker.checkAttackLeftDown(board) || checker.checkAttackRightDown(board) || checker.isKingAttack())
	{
		checker = backupChecker;
		theGame.setStepCompleted(checker.attack("king", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, scores, checker.getEnemy(), sf::Vector2i(x, y)));
	}
}

/* Выполняет процедуру атаки игровым соперником
Если дамка, то ищет цепочку и атакует,
иначе ищет безопасный ход,
иначе просто атакует.
Если пешка, то ищет цепочку и атакует по лучшему сценарию */
static void AIattack(sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton, sf::Sound& attackSound,
	sf::Sprite& s_cursor, sf::Vector2i mousePosition, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text* scores = nullptr)
{
	for (int i = 0; i < 12; ++i)
	{
		if (checkers[i].posX != -10 && checkers[i].getAttack())
		{
			if (checkers[i].getKing())
			{
				Checker backupChecker = checkers[i];
				if (checkers[i].getSideToAttack() == "rightDown")
				{
					for (int x = checkers[i].getEnemy()->posX + 1, y = checkers[i].getEnemy()->posY + 1; x < 8 && y < 8 && board[x][y] == nullptr && !theGame.getStepCompleted(); ++x, ++y)
					{ // проверка на цепочку
						attempKingAttack(checkers[i], x, y, backupChecker, s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, cancelStepArrow, *scores);
					}
					if (!theGame.getStepCompleted())
					{ // если цепочка не была найдена ищем безопасный вариант атаки
						checkers[i] = backupChecker;
						for (int x = checkers[i].getEnemy()->posX + 1, y = checkers[i].getEnemy()->posY + 1; x < 8 && y < 8 && board[x][y] == nullptr && !theGame.getStepCompleted(); ++x, ++y)
							if (board[x][y] == nullptr && (x == 7 || y == 7 || checkNotDanger(x, y)))
								theGame.setStepCompleted(checkers[i].attack("king", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores, checkers[i].getEnemy(), sf::Vector2i(x, y)));
					}
					if (!theGame.getStepCompleted()) // если совсем все плохо, то просто атакуем
						theGame.setStepCompleted(checkers[i].attack("rightDown", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores));
				}
				if (checkers[i].getSideToAttack() == "leftDown")
				{
					for (int x = checkers[i].getEnemy()->posX + 1, y = checkers[i].getEnemy()->posY - 1; x < 8 && y >= 0 && board[x][y] == nullptr && !theGame.getStepCompleted(); ++x, --y)
					{ // проверка на цепочку
						attempKingAttack(checkers[i], x, y, backupChecker, s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, cancelStepArrow, *scores);
					}
					if (!theGame.getStepCompleted())
					{ // если цепочка не была найдена ищем безопасный вариант атаки
						checkers[i] = backupChecker;
						for (int x = checkers[i].getEnemy()->posX + 1, y = checkers[i].getEnemy()->posY - 1; x < 8 && y >= 0 && board[x][y] == nullptr && !theGame.getStepCompleted(); ++x, --y)
							if (board[x][y] == nullptr && (x == 7 || y == 0 || checkNotDanger(x, y)))
								theGame.setStepCompleted(checkers[i].attack("king", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores, checkers[i].getEnemy(), sf::Vector2i(x, y)));
					}
					if (!theGame.getStepCompleted()) // если совсем все плохо, то просто атакуем
						theGame.setStepCompleted(checkers[i].attack("leftDown", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores));
				}

				if (checkers[i].getSideToAttack() == "rightUp")
				{
					for (int x = checkers[i].getEnemy()->posX - 1, y = checkers[i].getEnemy()->posY + 1; x >= 0 && y < 8 && board[x][y] == nullptr && !theGame.getStepCompleted(); --x, ++y)
					{ // проверка на цепочку
						attempKingAttack(checkers[i], x, y, backupChecker, s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, cancelStepArrow, *scores);
					}
					if (!theGame.getStepCompleted())
					{ // если цепочка не была найдена ищем безопасный вариант атаки
						checkers[i] = backupChecker;
						for (int x = checkers[i].getEnemy()->posX - 1, y = checkers[i].getEnemy()->posY + 1; x >= 0 && y < 8 && board[x][y] == nullptr && !theGame.getStepCompleted(); --x, ++y)
							if (board[x][y] == nullptr && (x == 0 || y == 7 || checkNotDanger(x, y)))
								theGame.setStepCompleted(checkers[i].attack("king", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores, checkers[i].getEnemy(), sf::Vector2i(x, y)));
					}
					if (!theGame.getStepCompleted()) // если совсем все плохо, то просто атакуем
						theGame.setStepCompleted(checkers[i].attack("rightUp", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores));
				}

				if (checkers[i].getSideToAttack() == "leftUp")
				{
					for (int x = checkers[i].getEnemy()->posX - 1, y = checkers[i].getEnemy()->posY - 1; x >= 0 && y >= 0 && board[x][y] == nullptr && !theGame.getStepCompleted(); --x, --y)
					{ // проверка на цепочку
						attempKingAttack(checkers[i], x, y, backupChecker, s_background, s_board, statusButton, menuButton, attackSound, s_cursor, mousePosition, cancelStepButton, cancelStepArrow, *scores);
					}
					if (!theGame.getStepCompleted())
					{ // если цепочка не была найдена ищем безопасный вариант атаки
						checkers[i] = backupChecker;
						for (int x = checkers[i].getEnemy()->posX - 1, y = checkers[i].getEnemy()->posY - 1; x >= 0 && y >= 0 && board[x][y] == nullptr && !theGame.getStepCompleted(); --x, --y)
							if (board[x][y] == nullptr && (x == 0 || y == 0 || checkNotDanger(x, y)))
								theGame.setStepCompleted(checkers[i].attack("king", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores, checkers[i].getEnemy(), sf::Vector2i(x, y)));
					}
					if (!theGame.getStepCompleted()) // если совсем все плохо, то просто атакуем
						theGame.setStepCompleted(checkers[i].attack("leftUp", s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores));
				}
			}
			else //не дамка
			{
				std::string sideToAttack = findSeries(checkers[i]);
				theGame.setStepCompleted(checkers[i].attack(sideToAttack, s_background, s_board, statusButton, menuButton, attackSound, cancelStepButton, cancelStepArrow, s_cursor, *scores));
			}
			if (theGame.getStepCompleted())
			{
				theGame.set_pAttackChecker(&checkers[i]);
				theGame.set_pSelectedChecker(&checkers[i]);
				break;
			}
		}
	}
}

/* Проверяет находится ли какая-либо шашка под атакой и можно ли ее защитить
Возвращает пару из указателя на шашку, которая должна защищать и сторону хода */
static std::pair<Checker*, std::string> mustDefense()
{
	for (int i = 0; i < 12; ++i)
	{
		// нельзя закрыть или безопасно
		if (checkers[i].posX < 2 || checkers[i].posX == 7 || checkers[i].posY == 7 || checkers[i].posY == 0)
			continue;
		// Верхняя может закрыть
		if (board[checkers[i].posX + 1][checkers[i].posY + 1] != nullptr && board[checkers[i].posX + 1][checkers[i].posY + 1]->getOwner() != checkers[i].getOwner())
		{ // Right-Down это враг
			if (board[checkers[i].posX - 1][checkers[i].posY - 1] == nullptr) //сзади пусто
				if (board[checkers[i].posX - 2][checkers[i].posY] != nullptr && board[checkers[i].posX - 2][checkers[i].posY]->getOwner() == checkers[i].getOwner())
				{ // Сверху своя шашка
					return std::make_pair(board[checkers[i].posX - 2][checkers[i].posY], "leftDown");
				}
		}
		// Верхняя может закрыть
		if (board[checkers[i].posX + 1][checkers[i].posY - 1] != nullptr && board[checkers[i].posX + 1][checkers[i].posY - 1]->getOwner() != checkers[i].getOwner())
		{ // Left-Down это враг
			if (board[checkers[i].posX - 1][checkers[i].posY + 1] == nullptr) // сзади пусто
				if (board[checkers[i].posX - 2][checkers[i].posY] != nullptr && board[checkers[i].posX - 2][checkers[i].posY]->getOwner() == checkers[i].getOwner())
				{ // Сверху своя шашка
					return std::make_pair(board[checkers[i].posX - 2][checkers[i].posY], "rightDown");
				}
		}
		// Может закрыть шашка слева
		if (checkers[i].posY > 1)
			if (board[checkers[i].posX + 1][checkers[i].posY + 1] != nullptr && board[checkers[i].posX + 1][checkers[i].posY + 1]->getOwner() != checkers[i].getOwner())
			{ // Right-Down это враг
				if (board[checkers[i].posX - 1][checkers[i].posY - 1] == nullptr) // сзади пусто
					if (board[checkers[i].posX - 2][checkers[i].posY - 2] != nullptr && board[checkers[i].posX - 2][checkers[i].posY - 2]->getOwner() == checkers[i].getOwner())
					{ // Слева своя шашка
						return std::make_pair(board[checkers[i].posX - 2][checkers[i].posY - 2], "rightDown");
					}
			}
		// Может закрыть шашка справа
		if (checkers[i].posY < 6)
			if (board[checkers[i].posX + 1][checkers[i].posY - 1] != nullptr && board[checkers[i].posX + 1][checkers[i].posY - 1]->getOwner() != checkers[i].getOwner())
			{ // Left-Down это враг
				if (board[checkers[i].posX - 1][checkers[i].posY + 1] == nullptr) // сзади пусто
					if (board[checkers[i].posX - 2][checkers[i].posY + 2] != nullptr && board[checkers[i].posX - 2][checkers[i].posY + 2]->getOwner() == checkers[i].getOwner())
					{ // Справа своя шашка
						return std::make_pair(board[checkers[i].posX - 2][checkers[i].posY + 2], "leftDown");
					}
			}
	}
	return std::make_pair(nullptr, "");
}

/* Выполняет процедуру хода игровым соперником
Если нужно обороняться - обороняется
Иначе ищет самую лучшую шашку для хода и ходит */
static void AIstep(sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton, sf::Sprite& s_cursor, sf::Vector2i mousePosition, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Text* scores = nullptr)
{
	std::pair<Checker*, std::string> defenseChecker = mustDefense();
	if (defenseChecker.first != nullptr) // если нужно обороняться
	{
		std::cout << "in must defense\n";
		theGame.setStepCompleted(defenseChecker.first->step(defenseChecker.second, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, *scores, mousePosition));
		if (theGame.getStepCompleted())
			theGame.set_pSelectedChecker(defenseChecker.first);
	}
	else
	{
		std::cout << "in AI step\n";
		for (int i = 0; i < 12; ++i)
		{
			if (checkers[i].posX != -10)
				checkers[i].findBestStep();
		}
		std::cout << "finded all best step\n";
		Checker* best = nullptr;
		int i = 0;
		while (best == nullptr && i < 12)
		{ // устанавливаем начальную шашку
			if (checkers[i].posX != -10)
				best = &checkers[i];
			++i;
		}
		std::cout << "set begining for search best\n";
		for (; i < 12; ++i)
		{ // ищем лучшую
			if (checkers[i].posX != -10 && checkers[i].getRating() > best->getRating())
				best = &checkers[i];
		}
		std::cout << "finded best\n";
		if (!best->getBestSideToStep().empty())
		{
			std::cout << "best is queen? " << best->getKing() << " " << best->getBestSideToStep() << std::endl;
			if (best->getKing())
			{
				std::cout << "queen step: " << best->getBestSideToStep()[0] - '0' << ' ' << best->getBestSideToStep()[1] - '0' << std::endl;
				theGame.setStepCompleted(best->step(best->getBestSideToStep(), s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, *scores, sf::Vector2i(best->getBestSideToStep()[0] - '0', best->getBestSideToStep()[1] - '0')));
			}
			else
				theGame.setStepCompleted(best->step(best->getBestSideToStep(), s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, *scores));
			if (theGame.getStepCompleted())
				theGame.set_pSelectedChecker(best);
			std::cout << "AI step done\n";
		}
		else // если ходов не найдено
		{ // пока не сделан ход
			while (!theGame.getStepCompleted())
			{
				i = rand() % 12; // выбираем случайную
				if (checkers[i].posX != -10 && checkers[i].posX < 7)
				{ 
					std::string side;
					if (rand() % 2) // рандомим сторону
					{ // можно сходить right-down
						if (checkers[i].posX < 7 && checkers[i].posY < 7 && board[checkers[i].posX + 1][checkers[i].posY + 1] == nullptr)
							side = "rightDown";
					}
					else // можно сходить left-down
						if (checkers[i].posX < 7 && checkers[i].posY > 0 && board[checkers[i].posX + 1][checkers[i].posY - 1] == nullptr)
							side = "leftDown";
					theGame.setStepCompleted(checkers[i].step(side, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, *scores, mousePosition));
					if (theGame.getStepCompleted())
						theGame.set_pSelectedChecker(&checkers[i]);
				}
			}
			std::cout << "AI step done by random\n";
		}
	}
}
#endif