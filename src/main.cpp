#include <SDL2/SDL.h>

#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <optional>
#include <random>
#include <vector>

constexpr float kMaxWorldSizeX = 100.0;
constexpr float kMaxWorldSizeY = 100.0;

constexpr int kWidth = 1920;
constexpr int kHeight = 1080;

static std::mt19937 rng{2104};

struct Rocket {
  float x, y;
  glm::vec2 speed;
  glm::vec2 acc;
};

struct SDL_Triangle {
  std::array<SDL_Vertex, 3> vertices;
};

SDL_Triangle get_rocket_triangle(const Rocket& r, float width, float height) {
  float x = r.x / kMaxWorldSizeX * width;
  float y = r.y / kMaxWorldSizeY * height;

  y = height - y;

  const SDL_Vertex v1 = {
      SDL_FPoint{x, y},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0, 0},
  };

  const SDL_Vertex v2 = {
      SDL_FPoint{x - 5, y - 5},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0, 0},
  };

  const SDL_Vertex v3 = {
      SDL_FPoint{x + 5, y - 5},
      SDL_Color{255, 0, 0, 255},
      SDL_FPoint{0, 0},
  };

  return SDL_Triangle{.vertices = {v1, v2, v3}};
}

class RocketWorld {
 public:
  RocketWorld(int rocket_count) {
    std::uniform_real_distribution<float> dis(0.0, 100.0);
    std::uniform_real_distribution<float> dis2(0.0, 0.9);

    rockets_.reserve(rocket_count);
    triangles_graphics_.reserve(rocket_count);

    for (int i = 0; i < rocket_count; ++i) {
      const float x = static_cast<float>(0 /*dis(rng)*/);
      const float y = static_cast<float>(dis(rng));
      const glm::vec2 speed{30, 4};
      const glm::vec2 acc{0, -9.8};
      rockets_.push_back(Rocket{x, y, speed, acc});
      triangles_graphics_.push_back(
          get_rocket_triangle(rockets_.back(), kWidth, kHeight));
    }
  }

  std::vector<Rocket>& get_mutable_rockets() { return rockets_; }
  std::vector<SDL_Triangle>& get_triangles_graphics() {
    return triangles_graphics_;
  }
  size_t get_rocket_count() const { return rockets_.size(); }

 private:
  std::vector<Rocket> rockets_;
  std::vector<SDL_Triangle> triangles_graphics_;
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
      for (size_t idx = 0; idx < physics_world_.get_rocket_count(); ++idx) {
        Rocket& r = physics_world_.get_mutable_rockets()[idx];
        r.x += r.speed.x * dt;
        r.y += r.speed.y * dt;
        r.speed.x += r.acc.x * dt;
        r.speed.y += r.acc.y * dt;
        physics_world_.get_triangles_graphics()[idx] =
            get_rocket_triangle(r, kWidth, kHeight);
      }

      last_update_ = std::chrono::system_clock::now();
      SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer_);
      SDL_RenderGeometry(renderer_, nullptr,
                         reinterpret_cast<const SDL_Vertex*>(
                             physics_world_.get_triangles_graphics().data()),
                         3 * physics_world_.get_rocket_count(), nullptr, 0);
      SDL_RenderPresent(renderer_);
    }
  }

 private:
  SDLWrapper(int width, int height, SDL_Window* window, SDL_Renderer* renderer)
      : width_(width),
        height_(height),
        window_(window),
        renderer_(renderer),
        last_update_(std::nullopt),
        physics_world_(1'000) {}

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
  SDLWrapper sdl_program = SDLWrapper::Create(kWidth, kHeight);
  sdl_program.render();
  return 0;
}