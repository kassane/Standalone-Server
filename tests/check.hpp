#ifndef CHECK_HPP
#define CHECK_HPP

#include <cstdlib>

#define ASSERT(cond) if (!(cond)) { std::abort(); }

#endif /* CHECK_HPP */
