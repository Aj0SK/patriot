#include <SDL2/SDL.h>

#include <chrono>
#include <vector>

constexpr std::chrono::duration UPDATE_DURATION = std::chrono::milliseconds(10);

struct Rocket {
  float x, y;
};

std::vector<SDL_Vertex> get_rocket_triangle(const Rocket& r) {
  const SDL_Vertex a = {
      SDL_FPoint{r.x, r.y},
      SDL_Color{0, 255, 0, 255},
      SDL_FPoint{0},
  };

  const SDL_Vertex b = {
      SDL_FPoint{r.x - 5, r.y - 5},
      SDL_Color{0, 255, 0, 255},
      SDL_FPoint{0},
  };

  const SDL_Vertex c = {
      SDL_FPoint{r.x + 5, r.y - 5},
      SDL_Color{0, 255, 0, 255},
      SDL_FPoint{0},
  };

  return {a, b, c};
}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window =
      SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       800, 600, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  srand(2104);

  auto last_update = std::chrono::system_clock::now();

  float mult = 0.0;

  std::vector<SDL_Vertex> verts;
  verts.reserve(3 * 100);

  for (int i = 0; i < 100; ++i) {
    const float x = static_cast<float>(rand() % 800);
    const float y = static_cast<float>(rand() % 600);
    Rocket r{x, y};
    auto to_add = get_rocket_triangle(r);
    for (SDL_Vertex v : to_add) verts.push_back(v);
  }

  bool running = true;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if ((SDL_QUIT == ev.type) ||
          (SDL_KEYDOWN == ev.type &&
           SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {
        running = false;
        break;
      }
    }

    if (auto now = std::chrono::system_clock::now();
        now - last_update > UPDATE_DURATION) {
      for (SDL_Vertex& vertex : verts) {
        vertex.position.y += mult;
      }
      const float dt = 1.0 / 100.0;
      mult += 2 * dt * 10.0;
      last_update = now;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr,
                       0);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}