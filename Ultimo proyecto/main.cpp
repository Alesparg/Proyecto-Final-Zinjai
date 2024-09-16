#include <iostream>
#include <vector>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <algorithm>

using namespace std;

const int WIDTH = 30;
   const int HEIGHT = 15; 
const int MAX_ENEMIGOS = 6; 

void  setColor(int color) {
	  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void  resetColor() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}




class   GameObject {
protected:
	  int x, y; 
public:
	
	  GameObject(int _x, int _y) : x(_x), y(_y) {}
	 virtual void move() = 0;
	 virtual void draw() const = 0;
	   int getX() const { return x; }
	int getY() const { return y; }
	 virtual ~GameObject() = default;
};


class Nave : public GameObject {
private:
	int lives;
	char direction;
public:
	Nave(int _x, int _y, int _lives) : GameObject(_x, _y), lives(_lives), direction(' ') {}
	
	void   move() override {
		 char input = _getch();
		 switch (input) {
		  case 'w': if (y > 0) y--; direction = 'w'; break;
		case 's': if (y < HEIGHT - 1) y++; direction = 's'; break;
		case 'a': if (x > 0) x--; direction = 'a'; break;
		  case 'd': if (x < WIDTH - 1) x++; direction = 'd'; break;
		case 'q': lives = 0; break;
		default: break;
		}
	}
	
	
	    void draw() const override {
		setColor(10); // Verde
		cout << 'Y';
		resetColor();
	}
	
	void   loseLife() {
		if (lives > 0) {
			lives--;
		}
	}
	
	
	
	
	
	  bool isAlive() const {
		return lives > 0;
	}
	
	
	int   getLives() const {
		return lives;
	}
	
	
	
	
	char getDirection() const {
		return direction;
	}
};




class Disparo : public GameObject {
public:
	char direction;
public:
	Disparo(int _x, int _y, char _direction) : GameObject(_x, _y), direction(_direction) {}
	
	void move() override {
		switch (direction) {
		case 'w': y -= 3; break; 		case 's': y += 3; break;
		case 'a': x -= 3; break;
		case 'd': x += 3; break;
		default: x += 3; break;
		}
	}
	
	void draw() const override {
		setColor(7); // Blanco
		cout << '|';
		resetColor();
	}
};




class   Enemigo : public GameObject {
private:
	int points;
public:
	Enemigo(int _x, int _y, int _points) : GameObject(_x, _y), points(_points) {}
	
	void move() override {
		x--;
	}
	
	void   draw() const override {
		 setColor(12); // Rojo
		cout << 'X';
		resetColor();
	}
	
	
	
	
	int  getPoints() const { return points; }
};

class Meteorito : public GameObject {
public:
	  Meteorito(int _x, int _y) : GameObject(_x, _y) {}
	
	
	void move() override {
		y++;
	}
	
	void  draw() const override {
		  setColor(14); 
		cout << 'o';
		resetColor();
		
		
	} 
};




class GameManager {
private:
	Nave* player;
	vector<Enemigo*> enemigos;
	vector<Meteorito*> meteoritos;
	vector<Disparo*> disparos;
	int score;
	string message; 
	clock_t messageStartTime;
	
	void createEnemies() {
		while (enemigos.size() < MAX_ENEMIGOS) {
			enemigos.push_back(new Enemigo(WIDTH - 1, rand() % HEIGHT, 10));
		}
	}
	
	  void createMeteoritos() {
		if (rand() % 10 < 2) {
			  meteoritos.push_back(new Meteorito(rand() % WIDTH, 0));
		}
	}
	
	void updateObjects() {
		for (auto& disparo : disparos) disparo->move();
		  for (auto& enemigo : enemigos) enemigo->move();
		for (auto& meteorito : meteoritos) meteorito->move();
		
		
		enemigos.erase(remove_if(enemigos.begin(), enemigos.end(),
								 [](Enemigo* e) {
									 bool outOfBounds = e->getX() < 0;
								 	 if (outOfBounds) delete e;
									 return outOfBounds;
								 }), enemigos.end());
		
		
		
		
		meteoritos.erase(remove_if (meteoritos.begin(), meteoritos.end(),
								   [](Meteorito* m) {
									   bool outOfBounds = m->getY() >= HEIGHT;
									   if (outOfBounds) delete m;
									   return outOfBounds;
								   }), meteoritos.end());
		
		
		  createEnemies();
		 createMeteoritos();
	}
	void checkCollisions() {
		for (auto it = disparos.begin(); it != disparos.end();) {
			bool removed = false;
			for (auto et = enemigos.begin(); et != enemigos.end();) {
				
				int startX = (*it)->getX();
				int endX = startX + ((*it)->direction == 'a' ? -3 : (*it)->direction == 'd' ? 3 : 0);
				int startY = (*it)->getY();
				int endY = startY + ((*it)->direction == 'w' ? -3 : (*it)->direction == 's' ? 3 : 0);
				
				
				  if ((*et)->getX() >= min(startX, endX) && (*et)->getX() <= max(startX, endX) &&
					(*et)->getY() >= min(startY, endY) && (*et)->getY() <= max(startY, endY)) {
					  score += (*et)->getPoints();
					delete *et;
					  et = enemigos.erase(et);
					removed = true;
					
					  const vector<string> messages = {"Nice shoot!", "Cool!", "Awesome!"};
					  message = messages[rand() % messages.size()];
					messageStartTime = clock();
					break;
				}  else {
						++et;
					}
			}
			if (removed) {
				  delete *it;
				it = disparos.erase(it);
			}  else {
				++it;
				
				
				
				
			}
		}
		
		for     (auto it = meteoritos.begin(); it != meteoritos.end();) {
			if ((*it)->getX() == player->getX() && (*it)->getY() == player->getY()) {
				player->loseLife();
				  delete *it;
				it = meteoritos.erase(it);
				if (!player->isAlive()) return;
			} else {
				++it;
			}
		}
		
		for   (auto it = enemigos.begin(); it != enemigos.end();) {
			if ((*it)->getX() == player->getX() && (*it)->getY() == player->getY()) {
				player->loseLife();
				 delete *it;
				  it = enemigos.erase(it);
				if (!player->isAlive()) return;
			}   else {
				++it;
			}
		}
		
		
		createEnemies();
	}
	
	
	void drawObjects() const {
		
		system("cls");
		  cout << "Nave en (" << player->getX() << ", " << player->getY() << ")" << endl;
		cout << "Puntuación: " << score << endl;
		cout << "Vidas restantes: " << player->getLives() << endl;
		
		  for (int i = 0; i < HEIGHT; ++i) {
			for (int j = 0; j < WIDTH; ++j) {
				bool drawn = false;
				
				for (const auto& disparo : disparos) {
					if (i == disparo->getY() && j == disparo->getX()) {
						disparo->draw();
						drawn = true;
						break;
						
						
					}
				}
				if  (drawn) continue;
				
				for   (const auto& enemigo : enemigos) {
					if (i == enemigo->getY() && j == enemigo->getX()) {
						  enemigo->draw();
						drawn = true;
						break;
						
						
						
					}
				}
				if (drawn) continue;
				
				for   (const auto& meteorito : meteoritos) {
					if (i == meteorito->getY() && j == meteorito->getX()) {
						meteorito->draw();
						drawn = true;
						break;
					}
				}
				 if (drawn)   continue;
				
				if (i == player->getY() && j == player->getX()) {
					player->draw();
				} else if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
					setColor(9); // Azul para el borde del mapa
					cout << '#';
					resetColor();
				} else {
					cout << ' ';
					
				}
				
			}
			cout << endl;
		}
		
		
		
		if   (clock() - messageStartTime < 1500) {
			cout << message << endl;
		}
		
		cout << "Controles: w (arriba), s (abajo), a (izquierda), d (derecha), q (salir)" << endl;
	}
	
public:
		GameManager() : player(new Nave(WIDTH / 2, HEIGHT / 2, 5)), score(0), message(""), messageStartTime(0) {
			srand(static_cast<unsigned int>(time(nullptr)));
			createEnemies();
		}
		
		~GameManager() {
			delete player;
			  for (auto enemigo : enemigos) delete enemigo;
			for (auto meteorito : meteoritos) delete meteorito;
			for (auto disparo : disparos) delete disparo;
		}
		
		void  run() {
			   while (player->isAlive()) {
			  	if   (_kbhit()) {
					 player->move();
					 if (player->getDirection()  != ' ') {
						disparos.push_back(new Disparo(player->getX(), player->getY(), player->getDirection()));
						
					}
				}
				 updateObjects();
				  checkCollisions();
				  drawObjects();
				Sleep(100);
			}
			
			system("cls");
			  cout << "¡Juego terminado!" << endl;
			  cout << "Puntuación final: " << score << endl;
			system("pause"); 
		}
};

int main() {
	GameManager game;
	game.run();
	return 0;
}     
