#pragma once

#include "logger/logger.h"

#ifdef _DEBUG

#define FEM_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                LOG_ERROR("Assertion failed: {}", message); \
                LOG_ERROR("  Condition: {}", #condition); \
                LOG_ERROR("  File: {}:{}", __FILE__, __LINE__); \
                LOG_ERROR("  Function: {}", __FUNCTION__); \
                std::abort(); \
            } \
        } while(0)
#else

#define FEM_ASSERT(condition, message) ((void)0)

#endif

#ifdef _DEBUG

#define FEM_VERIFY(condition) FEM_ASSERT(condition, #condition)

#else

#define FEM_VERIFY(condition) ((void)0)

#endif
