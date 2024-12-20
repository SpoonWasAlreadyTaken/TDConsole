#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <conio.h>


using namespace std;

// global variables

int sizeX = 26;
int sizeY = 26;
char display[26][26];

int wave;
int hitPoints;
string timer;

bool gameRunning = true;

int fireCount = 0;


// functions

void Initialize();
void Draw();
void UpdateEnemies();
int RandomNumber(int min, int max);
void CreateEnemy(int x, int y, int speed, int hp);
void FireWeapon(int explosionSize);
float Distance(int x1, int x2);

// classes

class Enemy
{
public:
	int posX;
	int posY;
	int moveX;
	int moveY;
	bool dead;

	Enemy(int x, int y, int speed, int hp)
	{
		posX = x;
		posY = y;
		health = hp;
		moveSpeed = speed;
		dead = false;
		steps = 0;
		moveX = 0;
		moveY = 0;
	}

	void UpdateHealth(int change)
	{
		health += change;
		if (health < 1)
		{
			dead = true;
		}
	}

	void Move()
	{
		moveX = 0;
		moveY = 0;
		steps += moveSpeed;

		while (steps > 10)
		{
			steps -= 10;

			if (sizeX / 2 - 1 != posX)
			{
				if (sizeX / 2 - 1 > posX)
				{
					moveX++;
				}
				else
				{
					moveX--;
				}
			}

			if (sizeY / 2 - 1 != posY)
			{
				if (sizeY / 2 - 1 > posY)
				{
					moveY++;
				}
				else
				{
					moveY--;
				}
			}
			
		}
	}

private:
	int health;
	int moveSpeed;
	int steps;
};

vector<Enemy> enemies;

int main()
{
	Initialize();
	thread thEnemy(UpdateEnemies);
	thread thDraw(Draw);
	thread thWeapon(FireWeapon, 3);

	while (gameRunning)
	{
		if (hitPoints < 1)
		{
			gameRunning = false;
		}

		if (_kbhit)
		{
			if (_getch() == 32)
			{
				fireCount++;
			}
		}
	}

	thEnemy.join();
	thDraw.join();
	thWeapon.join();

	system("CLS");

	cout << "GAME OVER!!!";
}


void Initialize()
{
	hitPoints = 100;
	wave = 0;

	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			display[i][j] = ' ';
		}
	}

	display[12][12]= '#';
}

void Draw()
{
	string output;
	while (gameRunning)
	{
		output = "HP: " + to_string(hitPoints) + "     Wave: " + to_string(wave) + "\n";

		for (int i = 0; i < sizeY; i++)
		{
			for (int j = 0; j < sizeX; j++)
			{
				output += display[i][j];
				output += " ";
			}
			output += "\n";
		}
		cout << output;
		Sleep(150);
		system("CLS");
	}
}

void UpdateEnemies()
{
	int toSpawn = 0;

	while (gameRunning)
	{
		if (enemies.empty())
		{
			wave++;
			toSpawn = wave;
		}

		if (toSpawn > 0)
		{
			switch (int pos = RandomNumber(1, 4))
			{
			case 1:
				CreateEnemy(RandomNumber(0, 25), 0, 1, 5);
				break;
			case 2:
				CreateEnemy(RandomNumber(0, 25), 25, 1, 5);
				break;
			case 3:
				CreateEnemy(0, RandomNumber(0, 25), 1, 5);
				break;
			case 4:
				CreateEnemy(25, RandomNumber(0, 25), 1, 5);
				break;
			}

			toSpawn--;
		}

		if (!enemies.empty())
		{
			for (int i = 0; i < enemies.size(); i++)
			{
				if (enemies.empty())
				{
					break;
				}
				if (enemies.at(i).posX == sizeX / 2 - 1 && enemies.at(i).posY == sizeY / 2 - 1)
				{
					enemies.erase(enemies.begin() + i);
					hitPoints--;
				}
			}
		}

		if (!enemies.empty())
		{
			for (int i = 0; i < enemies.size(); i++)
			{
				bool canMove = true;
				display[enemies.at(i).posY][enemies.at(i).posX] = ' ';

				enemies.at(i).Move();
				
				for (int j = 0; j < enemies.size(); j++)
				{
					if (i != j)
					{
						if (enemies.at(j).posX == enemies.at(i).posX + enemies.at(i).moveX && enemies.at(j).posY == enemies.at(i).posY + enemies.at(i).moveY)
						{
							canMove = false;
						}
					}
				}

				if (canMove)
				{
					enemies.at(i).posX += enemies.at(i).moveX;
					enemies.at(i).posY += enemies.at(i).moveY;
				}
				
				if (enemies.at(i).posX != sizeX / 2 - 1 || enemies.at(i).posY != sizeY / 2 - 1)
				{
					display[enemies.at(i).posY][enemies.at(i).posX] = '@';
				}
			}
		}

		Sleep(33);
	}
}

int RandomNumber(int min, int max) // generates a random number based on input parameters
{
	random_device rd;
	uniform_int_distribution<> distr(min, max);
	return distr(rd);
}

void CreateEnemy(int x, int y, int speed, int hp)
{
	Enemy newEnemy(x, y, speed, hp);
	
	enemies.push_back(newEnemy);
}

void FireWeapon(int explosionSize)
{
	while (gameRunning)
	{
		while (fireCount > 0)
		{
			int nearestEnemy = 0;
			int explosionX;
			int explosionY;

			if (!enemies.empty())
			{
				for (int i = 0; i < enemies.size(); i++)
				{

					if (sqrt((Distance(enemies.at(i).posX, sizeX / 2 - 1) * (Distance(enemies.at(i).posX, sizeX / 2 - 1)) + (Distance(enemies.at(i).posY, sizeY / 2 - 1) * 2) * Distance(enemies.at(i).posY, sizeY / 2 - 1)) < sqrt((Distance(enemies.at(nearestEnemy).posX, sizeX / 2 - 1) * (Distance(enemies.at(nearestEnemy).posX, sizeX / 2 - 1)) + (Distance(enemies.at(nearestEnemy).posY, sizeY / 2 - 1) * 2) * Distance(enemies.at(nearestEnemy).posY, sizeY / 2 - 1)))))
					{
						nearestEnemy = i;
					}
				}
			}

			//explosion
			explosionX = enemies.at(nearestEnemy).posX;
			explosionY = enemies.at(nearestEnemy).posY;

			int arstX = explosionX - explosionSize;
			int arstY = explosionY - explosionSize;
			int aretX = explosionX + explosionSize;
			int aretY = explosionY + explosionSize;
			if (arstX < 0)
			{
				arstX = 0;
			}
			if (arstY < 0)
			{
				arstY = 0;
			}
			if (arstX >= sizeX)
			{
				aretX = sizeX - 1;
			}
			if (arstY >= sizeY)
			{
				aretY = sizeY - 1;
			}


			for (int i = 0; i < explosionSize; i++)
			{
				for (int t = 0; t < 2; t++)
				{
					for (int y = arstY; y < aretY; y++)
					{
						for (int x = arstX; x < aretX; x++)
						{
							if (sqrt((Distance(explosionX, x) * Distance(explosionX, x)) + sqrt((Distance(explosionY, y) * Distance(explosionY, y)))) < i)
							{
								display[y][x] = '+';
							}
						}
					}
					Sleep(30);
					for (int y = arstY; y < aretY; y++)
					{
						for (int x = arstX; x < aretX; x++)
						{
							if (sqrt((Distance(explosionX, x) * Distance(explosionX, x)) + sqrt((Distance(explosionY, y) * Distance(explosionY, y)))) < i)
							{
								display[y][x] = ' ';
							}
						}
					}
				}
			}

			fireCount--;
			Sleep(50);
		}
		Sleep(33);
	}
}

float Distance(int x1, int x2)
{
	return abs(x1 - x2);
}