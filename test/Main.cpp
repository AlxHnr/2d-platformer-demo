/** @file
 * Contains the doctest main function.
 */

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <SDL_assert.h>

int main(const int arg_count, char **args) {
  SDL_SetAssertionHandler([](const SDL_AssertData *, void *) { return SDL_ASSERTION_ABORT; },
                          nullptr);
  return doctest::Context{arg_count, args}.run();
}
