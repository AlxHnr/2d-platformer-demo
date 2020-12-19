/** @file
 * Main playground for testing code.
 */

#include <iostream>
#include <string_view>
#include <SDL.h>
#include <memory>

namespace
{
void throwSDLError(std::string_view message)
{
  throw std::runtime_error{std::string{"Error: "}.append(message).append(": ").append(SDL_GetError())};
}

/** Wrap the given pointer in a unique pointer using the matching SDL function for release. */
template <typename T>
auto wrapPointer(T *ptr) {
  struct Deleter {
    void operator()(SDL_Window *window) { SDL_DestroyWindow(window); };
    void operator()(SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); };
  };
  return std::unique_ptr<T, Deleter>{ptr};
}
}

int main()
{
  struct Context {
    Context() {
      if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        throwSDLError("Failed to initialize SDL2");
      }
    }

    ~Context() {
      SDL_Quit();
    }
  } context{};

  SDL_Window *raw_window = nullptr;
  SDL_Renderer *raw_renderer = nullptr;
  if(SDL_CreateWindowAndRenderer(1280, 800, 0, &raw_window, &raw_renderer) != 0){
    throwSDLError("Failed to create window and renderer");
  }
  auto window = wrapPointer(raw_window);
  auto renderer = wrapPointer(raw_renderer);

  SDL_RenderClear(renderer.get());
  SDL_RenderPresent(renderer.get());

  bool program_running = true;
  while(program_running)
  {
    SDL_Event event;
    while(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        program_running = false;
        break;
      }
    }
  }

  std::cout << "Hello World" << std::endl;
}
