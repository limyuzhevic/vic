#pragma once

#include "Logger.hpp"
#include "DebugLogger.hpp"

namespace nlm {

// Enhanced logging macros that include debugging capabilities
#define NLM_LOG_TRACE(msg) nlm::getDebugLogger().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_TRACE_WITH_CONTEXT(msg, context) nlm::getDebugLogger().logWithContext(nlm::LogLevel::Trace, msg, context, 0, __FILE__, __LINE__, __func__)

#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG_WITH_CONTEXT(msg, context) nlm::getDebugLogger().logWithContext(nlm::LogLevel::Debug, msg, context, 0, __FILE__, __LINE__, __func__)

#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_WITH_CONTEXT(msg, context) nlm::getDebugLogger().logWithContext(nlm::LogLevel::Info, msg, context, 0, __FILE__, __LINE__, __func__)

#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_WITH_CONTEXT(msg, context) nlm::getDebugLogger().logWithContext(nlm::LogLevel::Warning, msg, context, 0, __FILE__, __LINE__, __func__)

#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_WITH_CONTEXT(msg, context) nlm::getDebugLogger().logWithContext(nlm::LogLevel::Error, msg, context, 0, __FILE__, __LINE__, __func__)

#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Performance logging macros
#define NLM_PERF_START(operation) nlm::PerformanceTracker perfTracker_##__LINE__(operation)
#define NLM_PERF_STOP(operation) perfTracker_##__LINE__.stop()

// Debug variable macros
#define NLM_SET_DEBUG_VAR(name, value) nlm::getDebugLogger().setDebugVariable(name, value)
#define NLM_GET_DEBUG_VAR(name) nlm::getDebugLogger().getDebugVariable(name)

// Debug session management macros
#define NLM_START_DEBUG_SESSION() nlm::getDebugLogger().startDebugSession()
#define NLM_END_DEBUG_SESSION(sessionId) nlm::getDebugLogger().endDebugSession(sessionId)

// Context logging macros
#define NLM_LOG_CONTEXT(level, msg, context) nlm::getDebugLogger().logWithContext(level, msg, context, 0, __FILE__, __LINE__, __func__)

// Convenience macros for common operations
#define NLM_PROFILE_FUNCTION() nlm::PerformanceTracker profileTracker_##__LINE__(__func__)
#define NLM_PROFILE_SCOPE(operation) nlm::PerformanceTracker scopeTracker_##__LINE__(operation)

} // namespace nlm
