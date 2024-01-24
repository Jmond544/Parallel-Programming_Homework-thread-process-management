#include <thread>
#include <iostream>
#include <conio.h>
#include <cmath>
#include <string>
#include <atomic>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define CLEAR_SCREEN() std::system("cls")
#else
#include <cstdlib>
#define CLEAR_SCREEN() std::system("clear")
#endif

const int BOARD_SIZE = 100;
const double DISTANCE_THRESHOLD = 2.0;

std::atomic<bool> finished(false);

struct Animal
{
  int x, y;
  int direction;
};

double distance(int x1, int y1, int x2, int y2)
{
  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

std::string twoDigits(int n)
{
  if (n < 10)
  {
    return "0" + std::to_string(n);
  }
  else
  {
    return std::to_string(n);
  }
}

void catThreadFunction(Animal& cat) {
  while (!finished) {

    gotoxy(10, 10);
    std::cout << "Gato: " << twoDigits(cat.x) << ", " << twoDigits(cat.y) << std::endl;

    char c = getch();
    if (c == 'a') {
      if(cat.x > 0)
        cat.x -= 1;
    } else if (c == 'd') {
      if(cat.x < 99)
        cat.x += 1;
    } else if (c == 'w') {
      if(cat.y < 99)
        cat.y += 1;
    } else if (c == 's') {
      if(cat.y > 0)
        cat.y -= 1;
    }

  }
}

void mouseThreadFunction(Animal& mouse, const Animal& cat) {
  while (!finished) {
    mouse.x += mouse.direction;
    if (mouse.x <= 0 || mouse.x > BOARD_SIZE) {
      mouse.direction *= -1;
    }

    if (distance(cat.x, cat.y, mouse.x, mouse.y) < DISTANCE_THRESHOLD) {
        gotoxy(10, 15);
        std::cout << "¡El gato ha atrapado al ratón!" << std::endl;
        finished = true;
        break;
    }

    gotoxy(10, 5);
    std::cout << "Ratón: " << twoDigits(mouse.x) << ", " << twoDigits(mouse.y) << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
}

int main()
{
  CLEAR_SCREEN();
  Animal cat = {1, 1, 1};
  Animal mouse = {50, 50, -1};

  std::thread catThread(catThreadFunction, std::ref(cat));

  std::thread mouseThread(mouseThreadFunction, std::ref(mouse), std::ref(cat));

  catThread.join();
  mouseThread.join();

  return 0;
}
