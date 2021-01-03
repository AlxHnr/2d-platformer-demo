/** @file
 * Contains the doctest main function.
 */

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <SDL_assert.h>

int main(const int arg_count, const char *const *const args) {
  SDL_SetAssertionHandler(
      [](const SDL_AssertData *data, void *) -> SDL_AssertState {
        const auto message = std::string{"Assertion failed: "} + data->condition + ": " +
                             data->function + "(): " + data->filename + ":" +
                             std::to_string(data->linenum);
        throw std::runtime_error{message};
      },
      nullptr);
  return doctest::Context{arg_count, args}.run();
}
