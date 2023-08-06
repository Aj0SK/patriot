#include <SDL2/SDL.h>

#include <chrono>
#include <glm/glm.hpp>
#include <vector>

constexpr std::chrono::duration UPDATE_DURATION = std::chrono::milliseconds(10);

struct Rocket {
  float x, y;
  float speed;
  glm::vec2 heading;
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

class RocketWorld {
 public:
  RocketWorld(int rocket_count) {
    rockets_.reserve(rocket_count);

    for (int i = 0; i < 100; ++i) {
      const float x = static_cast<float>(rand() % 800);
      const float y = static_cast<float>(rand() % 600);
      rockets_.push_back(Rocket{x, y, 1});
    }
  }

  std::vector<Rocket>& get_mutable_rockets() { return rockets_; }
  size_t get_rocket_count() const { return rockets_.size(); }

 private:
  std::vector<Rocket> rockets_;
};

class SDLWrapper {
 public:
  static SDLWrapper Create() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window =
        SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return SDLWrapper(window, renderer);
  }

  SDLWrapper(SDL_Window* window, SDL_Renderer* renderer)
      : window_(window),
        renderer_(renderer),
        last_update_(std::chrono::_V2::system_clock::duration::min()),
        physics_world_(20) {}

  ~SDLWrapper() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }

  void render() {
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

      std::vector<SDL_Vertex> vertices;
      vertices.reserve(3 * physics_world_.get_rocket_count());
      for (Rocket& r : physics_world_.get_mutable_rockets()) {
        r.y += r.speed;
        for (SDL_Vertex v : get_rocket_triangle(r)) vertices.push_back(v);
      }

      last_update_ = std::chrono::system_clock::now();
      SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer_);
      SDL_RenderGeometry(renderer_, nullptr, vertices.data(), vertices.size(),
                         nullptr, 0);
      SDL_RenderPresent(renderer_);
    }
  }

 private:
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::chrono::_V2::system_clock::time_point last_update_;

  // physic world part
  RocketWorld physics_world_;
};

int main() {
  srand(2104);
  SDLWrapper sdl_program = SDLWrapper::Create();
  sdl_program.render();
  return 0;
}