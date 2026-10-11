// PerformanceProfiler.cpp implementation

#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace nlm {
namespace profiling {

// Thread-local static member definitions for CPUProfiler
thread_local std::vector<uint64_t> CPUProfiler::current_call_stack_;
thread_local std::map<std::string, uint64_t> CPUProfiler::thread_function_times_;
std::unordered_map<uint64_t, std::string> CPUProfiler::thread_names_;

// Additional implementations for accessor methods

// CPUProfiler accessor methods
const std::vector<CallStackInfo>& CPUProfiler::getCallStacks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sampled_call_stacks_;
}

const std::vector<CPUProfiler::CPUStats>& CPUProfiler::getCpuStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cpu_stats_;
}

// MemoryProfiler accessor methods
const std::vector<MemoryInfo>& MemoryProfiler::getMemoryEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memory_events_;
}

// NetworkProfiler accessor methods
const std::vector<NetworkInfo>& NetworkProfiler::getNetworkEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return network_events_;
}

// IOProfiler accessor methods
const std::vector<IOInfo>& IOProfiler::getIOEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return io_events_;
}

// GPUProfiler accessor methods
const std::vector<GPUInfo>& GPUProfiler::getGpusEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return gpu_events_;
}

// TimelineProfiler accessor methods
const std::vector<TimelineEvent>& TimelineProfiler::getTimelineEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return timeline_events_;
}

// EventProfiler accessor methods
const std::vector<SystemEvent>& EventProfiler::getSystemEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return system_events_;
}

} // namespace profiling
} // namespace nlm

// Example usage and demonstration
void demonstrate_performance_profiling() {
    std::cout << "=== NLM Performance Profiling Demonstration ===" << std::endl;
    
    // Start performance profiling
    nlm::profiling::start_performance_profiling();
    
    // Enable specific profilers
    nlm::profiling::enable_profiler("cpu_profiler", true);
    nlm::profiling::enable_profiler("memory_profiler", true);
    nlm::profiling::enable_profiler("timeline_profiler", true);
    
    // Start a timeline event
    nlm::profiling::start_profiling_event("neural_network_init", "initialization");
    
    // Perform some work (placeholder)
    for (int i = 0; i < 100; ++i) {
        nlm::profiling::sample_performance();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // End the timeline event
    nlm::profiling::end_profiling_event("neural_network_init", "initialization");
    
    // Log some events
    nlm::profiling::log_profiling_event("info", "simulation_step", "Completed neural simulation step", "info");
    
    // Stop profiling
    nlm::profiling::stop_performance_profiling();
    
    // Print performance summary
    nlm::profiling::print_performance_summary();
}

// Command-line interface for performance profiling
void performance_profiling_cli(int argc, char* argv[]) {
    std::cout << "NLM Performance Profiling CLI" << std::endl;
    std::cout << "Usage: nlm_profile [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --start                   Start performance profiling" << std::endl;
    std::cout << "  --stop                    Stop performance profiling" << std::endl;
    std::cout << "  --enable <profiler>       Enable a specific profiler" << std::endl;
    std::cout << "  --disable <profiler>      Disable a specific profiler" << std::endl;
    std::cout << "  --sample                  Take a single sample" << std::endl;
    std::cout << "  --event-start <name>      Start a timeline event" << std::endl;
    std::cout << "  --event-end <name>        End a timeline event" << std::endl;
    std::cout << "  --log <event> <message>   Log an event" << std::endl;
    std::cout << "  --summary                 Print performance summary" << std::endl;
    std::cout << "  --save <filename>         Save performance data" << std::endl;
    std::cout << "  --demo                    Run demonstration" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_performance_profiling();
        }
    }
}