#pragma once

#define DEBUG_MODE

#ifdef DEBUG_MODE

#define ASSERT(condition, message) _STL_VERIFY(condition, message)

#else

#define ASSERT(condition, message) condition

#endif
