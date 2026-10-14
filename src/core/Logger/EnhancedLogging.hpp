#pragma once

#include "../Logging.hpp"

// Enhanced logging system for NLM
// This header provides access to the advanced logging and metrics collection classes

namespace nlm {

// Initialize the enhanced logging system
void initializeLoggingSystem();

// Shutdown the enhanced logging system
void shutdownLoggingSystem();

// Global instances for convenience (in production, these would be managed by a proper DI container)
extern SystemMonitor globalSystemMonitor;
extern NeuralPerformanceTracker globalNeuralTracker;
extern LogAggregator globalLogAggregator;
extern StructuredLogOutput globalStructuredOutput;
extern PerformanceBenchmark globalBenchmark;

} // namespace nlm
