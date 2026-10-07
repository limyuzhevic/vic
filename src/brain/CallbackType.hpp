#pragma once

namespace nlm {

// Callback types
enum class CallbackType {
    SimulationStep,                       // Called on each simulation step
    SimulationStart,                      // Called when simulation starts
    SimulationEnd,                        // Called when simulation ends
    Error,                               // Called when an error occurs
    Warning,                             // Called when a warning occurs
    Progress,                            // Called to report progress
    MemoryUsage,                        // Called for memory usage updates
    Performance,                        // Called for performance metrics
    Custom                             // Custom callback type
};

// Progress callback type (simplified version)
typedef std::function<void(float)> ProgressCallback;

// Error callback type
typedef std::function<void(const BrainError&)> ErrorCallback;

// Metric callback type
typedef std::function<void(const std::string&, double)> MetricCallback;

// Simulation step callback type
typedef std::function<void(const SimulationStep&)> SimulationStepCallback;

// Memory callback type
typedef std::function<void(size_t, size_t)> MemoryCallback;

// Performance callback type
typedef std::function<void(double, double)> PerformanceCallback;

} // namespace nlm