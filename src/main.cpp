#include <SDL2/SDL.h>

#include <vector>

int main(int argc, char** argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window =
      SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       800, 600, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  const std::vector<SDL_Vertex> verts = {
      {
          SDL_FPoint{400, 150},
          SDL_Color{255, 0, 0, 255},
          SDL_FPoint{0},
      },
      {
          SDL_FPoint{200, 450},
          SDL_Color{0, 0, 255, 255},
          SDL_FPoint{0},
      },
      {
          SDL_FPoint{600, 450},
          SDL_Color{0, 255, 0, 255},
          SDL_FPoint{0},
      },
  };

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