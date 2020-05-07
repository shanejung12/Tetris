// Tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <SFML/Graphics.hpp>

#include <iostream>

const int width = 320;
const int height = 480;

const int nCols = 10;
const int nRows = 20;

const int unit = 18;


struct Tetromino {
	int identifier;
	sf::Vector2i squares[4];
}; 

class Game {
private:
	int board[nCols][nRows] = { 0 };
	
public:
	Tetromino t;
	Game();
	int spawnTetromino();
	bool dropTetromino();
	bool canMoveThere(int, int);
	void drawBoard(sf::RenderWindow&);
	void drawTetromino(sf::RenderWindow&);
	void userInput(sf::RenderWindow&, sf::Event&);
	void addToBoard();
	void checkBoardForCompleteRows();
	bool checkRow(int);
};


sf::Sprite blocks[7];

int pieces[7][4]{
	1,2,3,4,
	1,2,4,6,
	1,3,4,6,
	2,3,4,5,
	1,3,5,7,
	1,2,3,5,
	1,3,4,5
};

int main()
{

	sf::Texture texture;
	texture.loadFromFile("Images/tiles.png");

	sf::Clock clock;

	for (int i = 0; i < 7; i++) {
		blocks[i].setTexture(texture);
		blocks[i].setTextureRect(sf::IntRect(i*unit, 0, unit, unit));
	}
	srand(time(NULL));
	Game g;

	sf::RenderWindow window(sf::VideoMode(width, height), "Tetris");
	while (window.isOpen())
	{

		g.spawnTetromino();
		do {
			clock.restart();
			sf::Event e;
			while (clock.getElapsedTime().asSeconds() < 0.8f) {
				while (window.pollEvent(e)) {
					g.userInput(window, e);
					window.clear(sf::Color::White);
					g.drawBoard(window);
					g.drawTetromino(window);
					window.display();
				}
				window.clear(sf::Color::White);
				g.drawBoard(window);
				g.drawTetromino(window);
				window.display();
			}
		} while (g.dropTetromino());
		g.addToBoard();
		g.drawBoard(window);
		g.checkBoardForCompleteRows();
	}

	return 0;
}

void Game::userInput(sf::RenderWindow& window, sf::Event& e)
{
	int x = 0;
	int y = 0;
	bool r = false;

	if (e.type == sf::Event::Closed) window.close();

	else if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::Left)         x = -1;
		else if (e.key.code == sf::Keyboard::Right)   x = 1;
		else if (e.key.code == sf::Keyboard::Down)    y = 1;
		else if (e.key.code == sf::Keyboard::Up)      r = true;
	}
	if (canMoveThere(x, y))
		for (int i = 0; i < 4; i++) {
			t.squares[i].x += x;
			t.squares[i].y += y;
		}
	if (r) {
		sf::Vector2i pivot = t.squares[2];
		int dx, dy;
		for (int i = 0; i < 4; i++) {
			if (i == 2) continue;
			dx = t.squares[i].x - pivot.x;
			dy = t.squares[i].y - pivot.y;
			t.squares[i].x = pivot.x + dy;
			t.squares[i].y = pivot.y - dx;
		}
		if (!canMoveThere(0, 0))
			for (int i = 0; i < 4; i++){
				if (i == 2) continue;
				dx = t.squares[i].x - pivot.x;
				dy = t.squares[i].y - pivot.y;
				t.squares[i].x = pivot.x - dy;
				t.squares[i].y = pivot.y + dx;
			}
	}
}

Game::Game() {
	for (int col = 0; col < nCols; col++)
		for (int row = 0; row < nRows; row++)
			board[col][row] = 0;
}

int Game::spawnTetromino() {
	int randomNum = rand() % 7;
	t.identifier = randomNum + 1;
	int blocksPlaced = 0;

	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 2; i++) {
			if (board[6 + i][j] != 0) return -1;
			if (blocksPlaced == 4) return 0;
			int target = 2 * j + i + 1;
			if (pieces[randomNum][blocksPlaced] == target) {
				sf::Vector2i v(6+i,j);
				t.squares[blocksPlaced] = v;
				blocksPlaced++;
			}
		}
	return 0;
}

void Game::drawBoard(sf::RenderWindow& window) {
	for (int i = 0; i < nCols; i++)
		for (int j = 0; j < nRows; j++)
		{
			if (board[i][j] == -1) continue;

			sf::Sprite b = blocks[board[i][j]-1];
			b.setPosition(i*unit, j*unit);
			window.draw(b);
		}
}
void Game::drawTetromino(sf::RenderWindow& window) {
	for (int i = 0; i < 4; i++) {
		sf::Sprite b = blocks[t.identifier-1];
		b.setPosition(t.squares[i].x*unit, t.squares[i].y*unit);
		window.draw(b);
	}
}

bool Game::dropTetromino()
{
	if (canMoveThere(0, 1)) { for (int i = 0; i < 4; i++) t.squares[i].y++; return true;}
	return false;
}

bool Game::canMoveThere(int dx, int dy) {
	for (int i = 0; i < 4; i++) {
		int x = t.squares[i].x;
		int y = t.squares[i].y;
		if (x + dx < 0 || x + dx > nCols|| y + dy>= nRows) {
			return false; 
		}
		else if (board[x+dx][y+dy]) return false; 
	}
	return true;
}

void Game::addToBoard() {
	for (int i = 0; i < 4; i++) {
		int x = t.squares[i].x;
		int y = t.squares[i].y;
		board[x][y] = t.identifier;
	}
	return;
}

void Game::checkBoardForCompleteRows() {
	int completeRows = 0;
	for (int i = 0; i < 4; i++) {
		int row = t.squares[i].y; 
		if (checkRow(row)) {
			completeRows++;
			for (; row > 0; row--)
				for (int col = 0; col < nCols; col++) board[col][row] = board[col][row - 1];
		} 
	}
}

bool Game::checkRow(int row) {
	for (int col = 0; col < nCols; col++) if (!board[col][row]) return false;
	return true;
}