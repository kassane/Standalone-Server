#ifndef TESTS_ASSERT_HPP
#define TESTS_ASSERT_HPP

#include <cstdlib>

#define ASSERT(e) ((void)((e) ? ((void)0) : ((void)fprintf(stderr, "Assertion failed: (%s), function %s, file %s, line %u.\n", #e, __func__, __FILE__, __LINE__), abort())))

#endif /* TESTS_ASSERT_HPP */
