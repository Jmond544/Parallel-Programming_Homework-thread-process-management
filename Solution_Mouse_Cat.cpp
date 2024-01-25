#include <thread>
#include <iostream>
#include <cmath>
#include <string>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN() std::system("cls")
#else
#include <cstdlib>
#include <unistd.h>
#define CLEAR_SCREEN() std::system("clear")
#endif

const int BOARD_SIZE = 30;
const double DISTANCE_THRESHOLD = 2.0;

std::atomic<bool> finished(false);
std::atomic<int> numberCaught(0);
std::mutex displayMutex;

struct Animal
{
  int x, y;
  int direction;
  bool caught = false;
};

double distance(int x1, int y1, int x2, int y2)
{
  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

bool allCaught(const std::vector<Animal> &mice)
{
  for (const auto &mouse : mice)
  {
    if (!mouse.caught)
      return false;
  }
  return true;
}

void displayBoard(const Animal &cat, const std::vector<Animal> &mice)
{
  std::lock_guard<std::mutex> lock(displayMutex);
  CLEAR_SCREEN();
  for (int i = 0; i < BOARD_SIZE; ++i)
  {
    for (int j = 0; j < BOARD_SIZE; ++j)
    {
      if (i == cat.y && j == cat.x)
        std::cout << "😺 ";
      else
      {
        bool mousePresent = false;
        for (const auto &mouse : mice)
        {
          if (i == mouse.y && j == mouse.x)
          {
            std::cout << "🐭 ";
            mousePresent = true;
            break;
          }
        }
        if (!mousePresent)
          std::cout << " . ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "* Ratones atrapados: [" << numberCaught << "]" << std::endl;

  finished = allCaught(mice);

  if (finished)
  {
    std::cout << "¡El gato ha atrapado a todos los ratones!" << std::endl;
    std::cout << std::endl;
  }
}

void catThreadFunction(Animal &cat, const std::vector<Animal> &mice)
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

    displayBoard(cat, mice);
  }
}

void mouseThreadFunction(Animal &mouse, const Animal &cat, std::vector<Animal> &mice)
{
  while (!mouse.caught)
  {
    int random = rand() % 5;

    switch (random)
    {
    case 0:
      if (mouse.y > 0)
        mouse.y -= 1;
      break;
    case 1:
      if (mouse.y < BOARD_SIZE - 1)
        mouse.y += 1;
      break;
    case 2:
      if (mouse.x > 0)
        mouse.x -= 1;
      break;
    case 3:
      if (mouse.x < BOARD_SIZE - 1)
        mouse.x += 1;
      break;
    }

    if (distance(cat.x, cat.y, mouse.x, mouse.y) < DISTANCE_THRESHOLD)
    {
      displayBoard(cat, {mice});
      numberCaught++;
      mouse.caught = true;
      break;
    }

    displayBoard(cat, {mice});
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

int main()
{
  CLEAR_SCREEN();
  srand(static_cast<unsigned int>(time(nullptr)));

  Animal cat = {1, 1, 0};
  std::vector<Animal> mice = {{10, 10, -1}, {5, 5, -1}, {10, 5, -1}, {19, 16, -1}}; // Puedes agregar más ratones aquí

  std::thread catThread(catThreadFunction, std::ref(cat), std::ref(mice));
  std::vector<std::thread> mouseThreads;

  for (auto &mouse : mice)
  {
    mouseThreads.emplace_back(mouseThreadFunction, std::ref(mouse), std::ref(cat), std::ref(mice));
  }

  while (!finished)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (!catThread.joinable())
      break;

    for (auto &mouseThread : mouseThreads)
    {
      if (!mouseThread.joinable())
        break;
    }
  }

  catThread.join();
  for (auto &mouseThread : mouseThreads)
  {
    mouseThread.join();
  }

  return 0;
}
