// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Checker.h"
#include "Checkers SFML.h"
#include <iostream>
#include <array>

extern sf::RenderWindow window;
extern Checker* board[8][8];
extern Game theGame;
extern std::array<Checker, 24> checkers;

/* Проверяет должна ли шашка атаковать
ПОБОЧНЫЙ ЭФФЕКТ: если шашка должна атаковать, то ее свойство mustAttack меняется в true
Возвращает результат проверки */
bool Checker::getMustAttack()
{
	if (this->king)
		return mustAttack = isKingAttack();
	return mustAttack = (checkAttackLeftUp(board) || checkAttackRightUp(board) || checkAttackLeftDown(board) || checkAttackRightDown(board));
}

/* Проверяет можно ли атаковать влево-вверх
Передается указатель на доску
Возвращает успешность проверки */
bool Checker::checkAttackLeftUp(Checker* const board[8][8]) const
{
	if (posX < 2 || posY < 2) // можно ли сходить физически?
		return false;
	return (board[posX - 1][posY - 1] != nullptr
		&& board[posX - 1][posY - 1]->getOwner() != this->getOwner() && board[posX - 2][posY - 2] == nullptr);
}

/* Проверяет можно ли атаковать вправо-вверх
Передается указатель на доску
Возвращает успешность проверки */
bool Checker::checkAttackRightUp(Checker* const board[8][8]) const
{
	if (posX < 2 || posY > 5)
		return false;
	return (board[posX - 1][posY + 1] != nullptr
		&& board[posX - 1][posY + 1]->getOwner() != this->getOwner() && board[posX - 2][posY + 2] == nullptr);
}

/* Проверяет можно ли атаковать влево-вниз
Передается указатель на доску
Возвращает успешность проверки */
bool Checker::checkAttackLeftDown(Checker* const board[8][8]) const
{
	if (posY < 2 || posX > 5)
		return false;
	return (board[posX + 1][posY - 1] != nullptr
		&& board[posX + 1][posY - 1]->getOwner() != this->getOwner() && board[posX + 2][posY - 2] == nullptr);
}

/* Проверяет можно ли атаковать вправо-внизы
Передается указатель на доску
Возвращает успешность проверки */
bool Checker::checkAttackRightDown(Checker* const board[8][8]) const
{
	if (posY > 5 || posX > 5)
		return false;
	return (board[posX + 1][posY + 1] != nullptr
		&& board[posX + 1][posY + 1]->getOwner() != this->getOwner() && board[posX + 2][posY + 2] == nullptr);
}

/* Проверяет нужно ли атаковать как дамка
ПОБОЧНЫЙ ЭФФЕКТ: меняет поле sideToAttack и pEnemy
Возвращает успешность проверки */
bool Checker::isKingAttack()
{
	if (king == false)
		return false;
	bool check = false;
	bool obstacle = false; // препятствие
	int i = posX + 1, j = posY + 1;
	for (; (i < 7 && j < 7) && !check && !obstacle; ++i, ++j) // проверка линии вниз вправо
	{
		if (board[i][j] != nullptr)
		{
			if (board[i][j]->getOwner() != owner && board[i + 1][j + 1] == nullptr)
			{
				check = true;
				pEnemy = board[i][j];
				sideToAttack = "rightDown";
			}
			else
				obstacle = true;
		}
	}
	if (check)
		return check;

	obstacle = false;
	i = posX - 1;
	j = posY - 1;
	for (; (i > 0 && j > 0) && !check && !obstacle; --i, --j) // проверка линии вверх влево
	{
		if (board[i][j] != nullptr)
			if (board[i][j]->getColor() != color && board[i - 1][j - 1] == nullptr)
			{
				check = true;
				pEnemy = board[i][j];
				sideToAttack = "leftUp";
			}
			else
				obstacle = true;
	}
	if (check)
		return check;

	obstacle = false;
	i = posX - 1;
	j = posY + 1;
	for (; (i > 0 && j < 7) && !check && !obstacle; --i, ++j) // проверка линии вверх вправо
	{
		if (board[i][j] != nullptr)
			if (board[i][j]->getOwner() != owner && board[i - 1][j + 1] == nullptr)
			{
				check = true;
				pEnemy = board[i][j];
				sideToAttack = "rightUp";
			}
			else
				obstacle = true;
	}
	if (check)
		return check;

	obstacle = false;
	i = posX + 1;
	j = posY - 1;
	for (; (i < 7 && j > 0) && !check && !obstacle; ++i, --j) // проверка линии вниз влево
	{
		if (board[i][j] != nullptr)
			if (board[i][j]->getColor() != color && board[i + 1][j - 1] == nullptr)
			{
				check = true;
				pEnemy = board[i][j];
				sideToAttack = "leftDown";
			}
			else
				obstacle = true;
	}
	return check;
}

// Выполняет процедуру анимации по коэффициентам для направления движения
void Checker::animation(int x_coef, int y_coef, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
	Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores, int delay = 20)
{
	for (int i = 0; i < 15; ++i)
	{
		this->move(float(window.getSize().y) / 120 * x_coef, float(window.getSize().y) / 120 * y_coef);
		window.clear();
		drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, &scores);
		window.draw(sprite);
		drawCursor(s_cursor);
		window.display();
		sleep(sf::milliseconds(delay));
	}
}

/* Выполняет процедуру атаки для шашки
Возвращает успешность */
bool Checker::attack(const std::string& side, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
	sf::Sound& attackSound, Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores,
	Checker* enemy, sf::Vector2i& posMouseOnBoard)
{
	if (side == "leftUp")
	{
		board[posX - 2][posY - 2] = board[posX][posY];
		board[posX][posY] = nullptr;
		animation(-1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX - 1][posY - 1]->move(-1500.f, -1500.f);
		if (theGame.getVolume())
			attackSound.play();
		animation(-1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX - 1][posY - 1]->posX = -10;
		board[posX - 1][posY - 1]->posY = -10;
		board[posX - 1][posY - 1] = nullptr;
		posX += -2;
		posY += -2;
		return true;
	}
	else if (side == "rightUp")
	{
		board[posX - 2][posY + 2] = board[posX][posY];
		board[posX][posY] = nullptr;
		animation(1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX - 1][posY + 1]->move(-1500.f, -1500.f);
		if (theGame.getVolume())
			attackSound.play();
		animation(1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX - 1][posY + 1]->posX = -10;
		board[posX - 1][posY + 1]->posY = -10;
		board[posX - 1][posY + 1] = nullptr;
		posX += -2;
		posY += 2;
		return true;
	}
	else if (side == "leftDown")
	{
		board[posX + 2][posY - 2] = board[posX][posY];
		board[posX][posY] = nullptr;
		animation(-1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX + 1][posY - 1]->move(-1500.f, -1500.f);
		if (theGame.getVolume())
			attackSound.play();
		animation(-1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX + 1][posY - 1]->posX = -10;
		board[posX + 1][posY - 1]->posY = -10;
		board[posX + 1][posY - 1] = nullptr;
		posX += 2;
		posY += -2;
		return true;
	}
	else if (side == "rightDown")
	{
		board[posX + 2][posY + 2] = board[posX][posY];
		board[posX][posY] = nullptr;
		animation(1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX + 1][posY + 1]->move(-1500.f, -1500.f);
		if (theGame.getVolume())
			attackSound.play();
		animation(1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores);
		board[posX + 1][posY + 1]->posX = -10;
		board[posX + 1][posY + 1]->posY = -10;
		board[posX + 1][posY + 1] = nullptr;
		posX += 2;
		posY += 2;
		return true;
	}
	else if (this->getKing())
	{
		if (enemy == nullptr)
			return false;
		std::cout << "king attack\n";
		board[posMouseOnBoard.x][posMouseOnBoard.y] = board[posX][posY];
		board[posX][posY] = nullptr;
		for (int i = 0; i <= 12; ++i)
		{
			this->move(float(window.getSize().y) / 200 * (posMouseOnBoard.y - posY), float(window.getSize().y) / 200 * (posMouseOnBoard.x - posX));
			drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, &scores);
			window.draw(sprite);
			drawCursor(s_cursor);
			window.display();
			sleep(sf::milliseconds(10));
		}
		enemy->move(-1500.f, -1500.f);
		if (theGame.getVolume())
			attackSound.play();
		for (int i = 0; i < 12; ++i)
		{
			this->move(float(window.getSize().y) / 200 * (posMouseOnBoard.y - posY), float(window.getSize().y) / 200 * (posMouseOnBoard.x - posX));
			drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, &scores);
			window.draw(sprite);
			drawCursor(s_cursor);

			window.display();
			sleep(sf::milliseconds(10));
		}
		board[enemy->posX][enemy->posY] = nullptr;
		enemy->posX = -10;
		enemy->posY = -10;
		posX = posMouseOnBoard.x;
		posY = posMouseOnBoard.y;
		return true;
	}
	else 
		return false;
}

/* Выполняет процедуру хода для шашки
Возвращает успешность */
bool Checker::step(const std::string& side, sf::Sprite& s_background, sf::Sprite& s_board, Button& statusButton, Button& menuButton,
	Button& cancelStepButton, sf::Sprite& cancelStepArrow, sf::Sprite& s_cursor, sf::Text& scores, const sf::Vector2i& pos)
{
	if (side == "rightUp")
	{
		board[posX - 1][posY + 1] = board[posX][posY];
		board[posX][posY] = nullptr;
		posX -= 1;
		posY += 1;
		animation(1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores, 30);
		return true;
	}
	else if (side == "leftUp")
	{
		board[posX - 1][posY - 1] = board[posX][posY];
		board[posX][posY] = nullptr;
		posX -= 1;
		posY -= 1;
		animation(-1, -1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores, 30);
		return true;
	}
	else if (side == "leftDown")
	{
		board[posX + 1][posY - 1] = board[posX][posY];
		board[posX][posY] = nullptr;
		posX += 1;
		posY -= 1;
		animation(-1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores, 30);
		return true;
	}
	else if (side == "rightDown")
	{
		board[posX + 1][posY + 1] = board[posX][posY];
		board[posX][posY] = nullptr;
		posX += 1;
		posY += 1;
		animation(1, 1, s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, s_cursor, scores, 30);
		return true;
	}
	else if (this->getKing() && side != "error")
	{
		for (int i = 0; i < 30; ++i)
		{
			this->move(float(window.getSize().y) / 240 * (pos.y - posY), float(window.getSize().y) / 240 * (pos.x - posX));
			drawGame(s_background, s_board, statusButton, menuButton, cancelStepButton, cancelStepArrow, &scores);
			drawCursor(s_cursor);
			window.display();
			sleep(sf::milliseconds(33 / (pos.y - posY)));
		}
		board[pos.x][pos.y] = board[posX][posY];
		board[posX][posY] = nullptr;
		posX = pos.x;
		posY = pos.y;
		return true;
	}
	else
		return false;
}

/* Проверяет в какую сторону пешка может сходить
Возвращает сторону для пешки или фразу I'm a King в случае успеха
или же error в случае неудачи */
std::string Checker::checkStep(sf::Vector2i& pos) const
{
	std::cout << pos.x << " ___ " << pos.y << std::endl;
	if (board[pos.x][pos.y] == nullptr)
	{
		if (this->getKing())
		{
			std::cout << "check step for king\n";
			if (this->checkStepKing(pos))
				return "I'm a King";
			else
				return "error";
		}
		else
			if (theGame.getPlayersTurn() == Game::Player::Human)
			{
				if (this->posX - pos.x == 1 && this->posY - pos.y == 1)
					return "leftUp";
				else
					if (this->posX - pos.x == 1 && this->posY - pos.y == -1)
						return "rightUp";
					else
						return "error";
			}
			else
			{
				if (this->posX - pos.x == -1 && this->posY - pos.y == 1)
					return "leftDown";
				else
					if (this->posX - pos.x == -1 && this->posY - pos.y == -1)
						return "rightDown";
					else
						return "error"; // cant step
			}
	}
	else
		return "error"; // cant step
}

/* Проверяет может ли дамка атаковать в данную позицию доски
Возвращает успешность проверки */
bool Checker::checkStepKing(const sf::Vector2i& posOnBoard) const
{
	bool check = true;
	if (abs(posOnBoard.x - posX) == abs(posOnBoard.y - posY)) // если клик на черное поле
	{
		int x = posOnBoard.x < posX ? posOnBoard.x + 1 : posX + 1;
		int y = posOnBoard.y < posY ? posOnBoard.y + 1 : posY + 1;
		int maxX = posOnBoard.x > posX ? posOnBoard.x : posX;
		bool mainDiagonal = ((posOnBoard.x - posX) > 0 && (posOnBoard.y - posY) > 0) // true - главная диагональ
			|| ((posOnBoard.x - posX) < 0 && (posOnBoard.y - posY) < 0);
		for (int j = 1; x < maxX; ++x, ++j, ++y) // проверить на отсутствие чего либо до цели
		{
			if (mainDiagonal) // если главная диагональ
			{
				if (board[x][y] != nullptr)
					check = false;
			}
			else
			{
				if (posOnBoard.x > posX) // вниз влево
				{
					if (board[x][posY - j] != nullptr)
						check = false;
				}
				else // вниз вправо
					if (board[x][posOnBoard.y - j] != nullptr)
						check = false;
			}
		}
		return check;
	}
	return false;
}

/* Проверяет в какую сторону пешка может атаковать
Возвращает сторону или error */
std::string Checker::checkAttack(const sf::Vector2i& pos) const
{
	if (!this->getAttack())
		return "error"; //эта пешка не может атаковать
	else
	{
		if (this->checkAttackLeftUp(board) && (pos.x == this->posX - 2) && (pos.y == this->posY - 2))
			return "leftUp";
		else
			if (this->checkAttackRightUp(board) && (pos.x == this->posX - 2) && (pos.y == this->posY + 2))
				return "rightUp";
			else
				if (this->checkAttackLeftDown(board) && (pos.x == this->posX + 2) && (pos.y == this->posY - 2))
					return "leftDown";
				else
					if (this->checkAttackRightDown(board) && (pos.x == this->posX + 2) && (pos.y == this->posY + 2))
						return "rightDown";
					else
						return "error";
	}
}

/* Проверяет может ли дамка атаковать в данную позицию
Возвращает адрес атакуемой пешки в случае удачной проверки
или nullptr в случае неудачи */
Checker* Checker::checkKingAttack(const sf::Vector2i& pos) const
{
	if (board[pos.x][pos.y] != nullptr)
		return nullptr;
	if (abs(this->posX - pos.x) != abs(this->posY - pos.y))
		return nullptr;
	std::cout << "check queen attack\n";
	bool check = false;
	bool obstacle = false;
	int amount = 0;
	Checker* enemy = nullptr;
	if (pos.x > this->posX)
	{
		if (pos.y > this->posY)
		{
			int i = this->posX + 1, j = this->posY + 1;
			for (; (i < pos.x && j < pos.y) && !obstacle && amount < 2; ++i, ++j) // проверка линии вниз вправо
			{
				if (board[i][j] != nullptr)
				{
					++amount;
					if (board[i][j]->getOwner() != this->getOwner() && board[i + 1][j + 1] == nullptr)
					{
						check = true;
						enemy = board[i][j];
					}
					else
						obstacle = true;
				}
			}
			if (check && !obstacle && amount < 2)
				return enemy;
			else
				return nullptr;
		}
		else
		{
			int i = this->posX + 1, j = this->posY - 1;
			for (; (i < pos.x && j > pos.y) && !obstacle && amount < 2; ++i, --j) // проверка линии вниз влево
			{
				if (board[i][j] != nullptr)
				{
					++amount;
					if (board[i][j]->getOwner() != this->getOwner() && board[i + 1][j - 1] == nullptr)
					{
						check = true;
						enemy = board[i][j];
					}
					else
						obstacle = true;
				}
			}
			if (check && !obstacle && amount < 2)
				return enemy;
			else
				return nullptr;
		}
	}
	else
	{
		if (pos.y > this->posY)
		{
			int i = this->posX - 1, j = this->posY + 1;
			for (; (i > pos.x && j < pos.y) && !obstacle && amount < 2; --i, ++j) // проверка линии вверх вправо
			{
				if (board[i][j] != nullptr)
				{
					++amount;
					if (board[i][j]->getOwner() != this->getOwner() && board[i - 1][j + 1] == nullptr)
					{
						check = true;
						enemy = board[i][j];
					}
					else
						obstacle = true;
				}
			}
			if (check && !obstacle && amount < 2)
				return enemy;
			else
				return nullptr;
		}
		else
		{
			int i = this->posX - 1, j = this->posY - 1;
			for (; (i > pos.x && j > pos.y) && !obstacle && amount < 2; --i, --j) // проверка линии вверх влево
			{
				if (board[i][j] != nullptr)
				{
					++amount;
					if (board[i][j]->getOwner() != this->getOwner() && board[i - 1][j - 1] == nullptr)
					{
						check = true;
						enemy = board[i][j];
					}
					else
						obstacle = true;
				}
			}
			if (check && !obstacle && amount < 2)
				return enemy;
			else
				return nullptr;
		}
	}
	return nullptr;
}

/* Ищет лучшую позицию для хода
ПОБОЧНЫЙ ЭФФЕКТ: Изменяет поле bestSideToStep */
void Checker::findBestStep()
{
	if (this->getKing())
	{
		std::cout << "finding best step for king in " << posX << ' ' << posY << "\n";
		rating = 0;
		if (!checkNotDanger(posX, posY))
		{
			for (int i = posX + 1, j = posY + 1; i < 8 && j < 8; ++i, ++j)
			{ // rightDown side check
				if (board[i][j] != nullptr)
					break;
				if (checkNotDanger(i, j))
				{
					std::cout << std::to_string(i) + std::to_string(j);
					bestSideToStep = std::to_string(i) + std::to_string(j);
					rating = 80;
					std::cout << "RD queen find\n";
					break;
				}
			}
			for (int i = posX + 1, j = posY - 1; i < 8 && j >= 0 && rating != 80; ++i, --j)
			{ // leftDown side check
				if (board[i][j] != nullptr)
					break;
				if (checkNotDanger(i, j))
				{
					std::cout << std::to_string(i) + std::to_string(j);
					bestSideToStep = std::to_string(i) + std::to_string(j);
					rating = 80;
					std::cout << "LD queen find\n";
					break;
				}
			}
			for (int i = posX - 1, j = posY + 1; i >= 0 && j < 8 && rating != 80; --i, ++j)
			{ // rightUp side check
				if (board[i][j] != nullptr)
					break;
				if (checkNotDanger(i, j))
				{
					std::cout << std::to_string(i) + std::to_string(j);
					bestSideToStep = std::to_string(i) + std::to_string(j);
					rating = 80;
					std::cout << "RU queen find\n";
					break;
				}
			}
			for (int i = posX - 1, j = posY - 1; i >= 0 && j >= 0 && rating != 80; --i, --j)
			{ // leftUp side check
				if (board[i][j] != nullptr)
					break;
				if (checkNotDanger(i, j))
				{
					std::cout << std::to_string(i) + std::to_string(j);
					bestSideToStep = std::to_string(i) + std::to_string(j);
					rating = 80;
					std::cout << "LU queen find\n";
					break;
				}
			}
		}
		else
		{
			if (posX < 7 && board[7][7 - posX + posY] == nullptr && checkStepKing(sf::Vector2i(7, 7 - posX + posY)))
			{
				std::cout << "find step for queen " << 7 << ' ' << 7 - posX + posY << std::endl;
				bestSideToStep = "7" + std::to_string(7 - posX + posY);
				rating = 5;
			}
			else
			{
				for (int i = 0; i < 8; ++i)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (board[i][j] == nullptr && (i != posX && j != posY) && checkStepKing(sf::Vector2i(i, j)) && checkNotDanger(i, j))
						{
							std::cout << "find step for queen " << i << ' ' << j << std::endl;
							bestSideToStep = std::to_string(i) + std::to_string(j);
							rating = 1;
							break;
						}
					}
					if (rating == 1)
						break;
				}
			}
		}
		if (rating == 0)
			bestSideToStep.clear();
	}
	else
	{
		std::cout << "finding best step for " << posX << " " << posY << "\n";
		int ratingRightDown = 0;
		if (posX < 7 && posY < 7 && board[posX + 1][posY + 1] == nullptr)
		{
			std::cout << "RD find...\n";
			if (checkNotDanger(posX, posY))
				ratingRightDown += 1;
			bool check = true;
			board[posX][posY] = nullptr;
			posX += 1; posY += 1;
			board[posX][posY] = this;
			for (int i = 0; i < 12; ++i)
			{
				if (checkers[i].posX != -10)
				{
					if (!checkNotDanger(checkers[i].posX, checkers[i].posY))
						check = false;
				}
			}
			board[posX][posY] = nullptr;
			posX -= 1; posY -= 1;
			board[posX][posY] = this;
			if (check)
			{ // не угрожает опасность
				ratingRightDown += 1;
				if (posX == 5)
					ratingRightDown += 50;
				else
				{
					if (posX < 5 && board[posX + 3][posY + 1] != nullptr && board[posX + 3][posY + 1]->getOwner() != owner)
						ratingRightDown += 10;
					if (posX < 5 && posY < 5 && board[posX + 3][posY + 3] != nullptr && board[posX + 3][posY + 3]->getOwner() != owner)
						ratingRightDown += 10;
				}
				if (posX == 6)
					ratingRightDown += 100;
			}
		}

		int ratingLeftDown = 0;
		if (posX < 7 && posY > 0 && board[posX + 1][posY - 1] == nullptr)
		{
			std::cout << "LD find...\n";
			if (checkNotDanger(posX, posY))
				ratingLeftDown += 1;
			bool check = true;
			board[posX][posY] = nullptr;
			posX += 1; posY -= 1;
			board[posX][posY] = this;
			for (int i = 0; i < 12; ++i)
			{
				if (checkers[i].posX != -10)
				{
					if (!checkNotDanger(checkers[i].posX, checkers[i].posY))
						check = false;
				}
			}
			board[posX][posY] = nullptr;
			posX -= 1; posY += 1;
			board[posX][posY] = this;
			if (check)
			{ // не угрожает опасность
				ratingLeftDown += 1;
				if (posX == 5)
					ratingLeftDown += 50;
				else
				{
					if (posX < 5 && board[posX + 3][posY - 1] != nullptr && board[posX + 3][posY - 1]->getOwner() != owner)
						ratingLeftDown += 10;
					if (posX < 5 && posY > 2 && board[posX + 3][posY - 3] != nullptr && board[posX + 3][posY - 3]->getOwner() != owner)
						ratingLeftDown += 10;
				}
				if (posX == 6)
					ratingLeftDown += 100;
			}

		}

		if (ratingLeftDown >= ratingRightDown)
		{
			bestSideToStep = "leftDown";
			rating = ratingLeftDown;
		}
		else
		{
			bestSideToStep = "rightDown";
			rating = ratingRightDown;
		}
		if (rating == 0)
			bestSideToStep.clear();
	}
}

/* Сохраняет состояние шашки для отмены хода
Сохраняет поля king, posX, posY, sprite.position */
void Checker::saveCondition()
{
	oldCondition.king = king;
	oldCondition.posX = posX;
	oldCondition.posY = posY;
	oldCondition.spritePos = sprite.getPosition();
}

/* Возвращает состояние до предыдущего хода
Устанавливает поля king, posX, posY, sprite.position и указатель на доске */
void Checker::returnCondition()
{
	king = oldCondition.king;
	posX = oldCondition.posX;
	posY = oldCondition.posY;
	sprite.setPosition(oldCondition.spritePos);
	board[posX][posY] = this;
}

/* Превращает шашку в дамку
Устанавливает поле king, а также устанавливает текстуру */
void Checker::makeKing(const sf::Texture& t_blackQueen, const sf::Texture& t_whiteQueen, const sf::Texture& t_activeBlackQueen, const sf::Texture& t_activeWhiteQueen)
{
	king = true;
	if (mustAttack)
	{
		if (color == 'b')
			sprite.setTexture(t_activeBlackQueen);
		else
			sprite.setTexture(t_activeWhiteQueen);
	}
	else
	{
		if (color == 'b')
			sprite.setTexture(t_blackQueen);
		else
			sprite.setTexture(t_whiteQueen);
	}
}

/* Проверяет можно ли сходить влево-вниз
Возвращает успешность проверки */
bool Checker::canStepLeftDown() const
{
	if (posX < 7 && posY > 0)
		if (board[posX + 1][posY - 1] == nullptr)
			return true;
	return false;
}

/* Проверяет можно ли сходить вправо-вниз
Возвращает успешность проверки */
bool Checker::canStepRightDown() const
{
	if (posX < 7 && posY < 7)
		if (board[posX + 1][posY + 1] == nullptr)
			return true;
	return false;
}

/* Проверяет можно ли сходить влево-вверх
Возвращает успешность проверки */
bool Checker::canStepLeftUp() const
{
	if (posX > 0 && posY > 0)
		if (board[posX - 1][posY - 1] == nullptr)
			return true;
	return false;
}

/* Проверяет можно ли сходить вправо-вниз
Возвращает успешность проверки */
bool Checker::canStepRightUp() const
{
	if (posX > 0 && posY < 7)
		if (board[posX - 1][posY + 1] == nullptr)
			return true;
	return false;
}

/* Проверяет можно ли сходить в какую-либо сторону
Возвращает успешность проверки */
bool Checker::canKingStep() const
{
	return canStepLeftDown() || canStepLeftUp() || canStepRightDown() || canStepRightUp();
}
