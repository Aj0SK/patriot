#include <SDL2/SDL.h>

#include <array>
#include <chrono>
#include <cmath>
#include <glm/glm.hpp>
#include <optional>
#include <random>
#include <vector>

constexpr float kMaxWorldSizeX = 100.0;
constexpr float kMaxWorldSizeY = 100.0;

constexpr int kRocketCount = 10;

constexpr int kWidth = 1920;
constexpr int kHeight = 1080;

static std::mt19937 rng{2104};

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct Rocket {
  float x, y;
  glm::vec2 speed;
  glm::vec2 acc;
  Color c;
};

struct SDL_Triangle {
  std::array<SDL_Vertex, 3> vertices;
};

SDL_Triangle get_rocket_triangle(const Rocket& r, float width, float height) {
  const float x = r.x / kMaxWorldSizeX * width;
  const float y = height - r.y / kMaxWorldSizeY * height;

  const float speed_length =
      std::sqrt(r.speed.x * r.speed.x + r.speed.y * r.speed.y);
  const float t_len = 15;

  float x1 = x - (t_len * r.speed.x - t_len * r.speed.y) / speed_length;
  float y1 = y - (-t_len * r.speed.y - t_len * r.speed.x) / speed_length;
  float x2 = x - (t_len * r.speed.x + t_len * r.speed.y) / speed_length;
  float y2 = y - (-t_len * r.speed.y + t_len * r.speed.x) / speed_length;

  const SDL_Vertex v1 = {
      SDL_FPoint{x, y},
      {r.c.r, r.c.g, r.c.b, 255},
      SDL_FPoint{0, 0},
  };
  const SDL_Vertex v2 = {
      SDL_FPoint{x1, y1},
      {r.c.r, r.c.g, r.c.b, 255},
      SDL_FPoint{0, 0},
  };
  const SDL_Vertex v3 = {
      SDL_FPoint{x2, y2},
      {r.c.r, r.c.g, r.c.b, 255},
      SDL_FPoint{0, 0},
  };
  return SDL_Triangle{.vertices = {v1, v2, v3}};
}

class RocketWorld {
 public:
  RocketWorld(int rocket_count) : rocket_count_(rocket_count) {
    std::uniform_real_distribution<float> dis(0.0, 100.0);
    std::uniform_int_distribution<uint8_t> dis_col(0, 255);
    std::uniform_real_distribution<float> dis2(-15.0, 15.0);

    rockets_.reserve(rocket_count);
    triangles_.reserve(rocket_count);

    for (int i = 0; i < rocket_count; ++i) {
      const float x = static_cast<float>(dis(rng));
      const float y = static_cast<float>(1 /*dis(rng)*/);
      const glm::vec2 speed{dis2(rng), 30};
      const glm::vec2 acc{0, -9.8};
      const Color color{
          .r = dis_col(rng), .g = dis_col(rng), .b = dis_col(rng)};
      rockets_.push_back(Rocket{x, y, speed, acc, color});
      triangles_.push_back(
          get_rocket_triangle(rockets_.back(), kWidth, kHeight));
    }
  }

  std::vector<Rocket>& get_mutable_rockets() { return rockets_; }
  std::vector<SDL_Triangle>& get_triangles() { return triangles_; }
  size_t get_rocket_count() const { return rockets_.size(); }

 private:
  size_t rocket_count_;
  std::vector<Rocket> rockets_;
  std::vector<SDL_Triangle> triangles_;
  std::vector<Color> colors_;
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
        physics_world_.get_triangles()[idx] =
            get_rocket_triangle(r, kWidth, kHeight);
      }

      last_update_ = std::chrono::system_clock::now();
      SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer_);
      SDL_RenderGeometry(renderer_, nullptr,
                         reinterpret_cast<const SDL_Vertex*>(
                             physics_world_.get_triangles().data()),
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
        physics_world_(kRocketCount) {}

 private:
  int width_, height_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::optional<std::chrono::_V2::system_clock::time_point> last_update_;

  // physic world part
  RocketWorld physics_world_;
};

int main() {
  SDLWrapper sdl_program = SDLWrapper::Create(kWidth, kHeight);
  sdl_program.render();
  return 0;
}