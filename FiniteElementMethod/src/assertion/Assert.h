#pragma once

#include "logger/logger.h"

/// <summary>
/// Assertion macro for validating conditions during debugging.
/// In debug builds, if the condition is false, logs an error message
/// with details and aborts the program. In release builds, does nothing.
/// </summary>
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

/// <summary>
/// Verification macro for checking conditions in debug builds.
/// In debug builds, if the condition is false, triggers an assertion failure.
/// In release builds, does nothing.
/// </summary>
#ifdef _DEBUG

#define FEM_VERIFY(condition) FEM_ASSERT(condition, #condition)

#else

#define FEM_VERIFY(condition) ((void)0)

#endif
