#include <SDL2/SDL.h>

#include <chrono>
#include <glm/glm.hpp>
#include <optional>
#include <random>
#include <vector>

constexpr float kMaxWorldSizeX = 100.0;
constexpr float kMaxWorldSizeY = 100.0;

static std::mt19937 rng{2104};

struct Rocket {
  float x, y;
  glm::vec2 speed;
  glm::vec2 acc;
};

std::vector<SDL_Vertex> get_rocket_triangle(const Rocket& r, float width,
                                            float height) {
  float x = r.x / kMaxWorldSizeX * width;
  float y = r.y / kMaxWorldSizeY * height;

  y = height - y;

  const SDL_Vertex v1 = {
      SDL_FPoint{x, y},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0},
  };

  const SDL_Vertex v2 = {
      SDL_FPoint{x - 5, y - 5},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0},
  };

  const SDL_Vertex v3 = {
      SDL_FPoint{x + 5, y - 5},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0},
  };

  return {v1, v2, v3};
}

class RocketWorld {
 public:
  RocketWorld(int rocket_count) {
    rockets_.reserve(rocket_count);

    std::uniform_real_distribution<float> dis(0.0, 100.0);
    std::uniform_real_distribution<float> dis2(0.0, 0.9);

    for (int i = 0; i < 100; ++i) {
      const float x = static_cast<float>(0 /*dis(rng)*/);
      const float y = static_cast<float>(dis(rng));
      const glm::vec2 speed{30, 4};
      const glm::vec2 acc{0, -9.8};
      rockets_.push_back(Rocket{x, y, speed, acc});
    }
  }

  std::vector<Rocket>& get_mutable_rockets() { return rockets_; }
  size_t get_rocket_count() const { return rockets_.size(); }

 private:
  std::vector<Rocket> rockets_;
};

class SDLWrapper {
 public:
  static SDLWrapper Create(int width, int height) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, width,
                                          height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return SDLWrapper(width, height, window, renderer);
  }

  SDLWrapper(int width, int height, SDL_Window* window, SDL_Renderer* renderer)
      : width_(width),
        height_(height),
        window_(window),
        renderer_(renderer),
        last_update_(std::nullopt),
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

      const auto now = std::chrono::system_clock::now();
      if (!last_update_.has_value()) {
        last_update_ = now;
      }

      float milli_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                             now - *last_update_)
                             .count();
      float dt = milli_diff / 1000.0;
      std::vector<SDL_Vertex> vertices;
      vertices.reserve(3 * physics_world_.get_rocket_count());
      for (Rocket& r : physics_world_.get_mutable_rockets()) {
        r.x += r.speed.x * dt;
        r.y += r.speed.y * dt;
        r.speed.x += r.acc.x * dt;
        r.speed.y += r.acc.y * dt;
        for (SDL_Vertex v : get_rocket_triangle(r, width_, height_)) {
          vertices.push_back(v);
        }
      }

      last_update_ = std::chrono::system_clock::now();
      SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer_);
      SDL_RenderGeometry(renderer_, nullptr, vertices.data(), vertices.size(),
                         nullptr, 0);
      SDL_RenderPresent(renderer_);
    }
  }

 private:
  int width_, height_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::optional<std::chrono::_V2::system_clock::time_point> last_update_;

  // physic world part
  RocketWorld physics_world_;
};

int main() {
  srand(2104);
  SDLWrapper sdl_program = SDLWrapper::Create(1920, 1080);
  sdl_program.render();
  return 0;
}