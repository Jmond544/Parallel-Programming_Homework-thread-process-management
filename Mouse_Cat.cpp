#include <thread>
#include <iostream>
#include <cmath>
#include <string>
#include <atomic>
#include <conio.h>
#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN() std::system("cls")
#else
#include <cstdlib>
#define CLEAR_SCREEN() std::system("clear")
#endif

const int BOARD_SIZE = 20;
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

void displayBoard(const Animal &cat, const Animal &mouse)
{
  CLEAR_SCREEN();
  for (int i = 0; i < BOARD_SIZE; ++i)
  {
    for (int j = 0; j < BOARD_SIZE; ++j)
    {
      if (i == cat.y && j == cat.x)
        std::cout << "😺 ";
      else if (i == mouse.y && j == mouse.x)
        std::cout << "🐭 ";
      else
        std::cout << ". ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << std::endl;
  if (finished)
  {
    std::cout << "¡El gato ha atrapado al ratón!" << std::endl;
    std::cout << std::endl;
  }
}

void catThreadFunction(Animal &cat, const Animal &mouse)
{
  while (!finished)
  {
    char c = getch();
    if (c == 'a' && cat.x > 0)
      cat.x -= 1;
    else if (c == 'd' && cat.x < BOARD_SIZE - 1)
      cat.x += 1;
    else if (c == 'w' && cat.y > 0)
      cat.y -= 1;
    else if (c == 's' && cat.y < BOARD_SIZE - 1)
      cat.y += 1;

    displayBoard(cat, mouse);
  }
}

void mouseThreadFunction(Animal &mouse, const Animal &cat)
{
  while (!finished)
  {
    mouse.x += mouse.direction;
    if (mouse.x <= 0 || mouse.x >= BOARD_SIZE - 1)
      mouse.direction *= -1;

    displayBoard(cat, mouse);
    if (distance(cat.x, cat.y, mouse.x, mouse.y) < DISTANCE_THRESHOLD)
    {
      displayBoard(cat, mouse);
      std::cout << "¡El gato ha atrapado al ratón!" << std::endl;
      finished = true;
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
}

int main()
{
  CLEAR_SCREEN();
  Animal cat = {1, 1, 0};
  Animal mouse = {10, 10, -1};

  std::thread catThread(catThreadFunction, std::ref(cat), std::ref(mouse));
  std::thread mouseThread(mouseThreadFunction, std::ref(mouse), std::ref(cat));

  catThread.join();
  mouseThread.join();

  return 0;
}
