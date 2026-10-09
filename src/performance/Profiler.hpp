#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

// Profile data structures
struct FunctionProfile {
    std::string name;
    std::string category;
    double total_time_ms = 0.0;
    double self_time_ms = 0.0;  // Time excluding children
    uint64_t call_count = 0;
    double average_time_ms = 0.0;
    double min_time_ms = std::numeric_limits<double>::max();
    double max_time_ms = 0.0;
    std::vector<double> sample_times;  // Individual sample times
    
    // Nested function calls
    std::map<std::string, FunctionProfile> children;
    
    FunctionProfile() = default;
    FunctionProfile(const std::string& n, const std::string& c = "") : name(n), category(c) {}
};

struct MemoryProfile {
    uint64_t timestamp_ms = 0;
    double memory_mb = 0.0;
    double memory_growth_mb = 0.0;
    uint64_t allocation_count = 0;
    uint64_t deallocation_count = 0;
    std::map<std::string, uint64_t> allocation_types;
    std::string stack_trace;
};

struct AlgorithmProfile {
    std::string algorithm_name;
    std::string complexity_category;
    uint64_t input_size = 0;
    double execution_time_ms = 0.0;
    double memory_usage_mb = 0.0;
    double time_complexity_estimate = 0.0;
    double space_complexity_estimate = 0.0;
    std::string optimization_suggestions;
};

struct TimelineEvent {
    uint64_t timestamp_ms = 0;
    std::string event_name;
    std::string event_type;
    double duration_ms = 0.0;
    std::string thread_id;
    std::map<std::string, std::string> metadata;
    std::string stack_trace;
};

struct HotPathData {
    std::vector<std::pair<std::string, double>> hot_functions;  // function name, cumulative time
    std::vector<std::pair<std::string, double>> hot_calls;  // function name, call count
    std::vector<std::pair<std::string, double>> hot_memory;  // function name, memory usage
    double total_time_ms = 0.0;
    uint64_t total_calls = 0;
    double bottleneck_percentage = 0.0;
};

struct SpikeEventProfile {
    uint64_t timestamp_ms = 0;
    double spike_frequency_hz = 0.0;
    uint32_t spike_count = 0;
    double spike_amplitude_mean = 0.0;
    double spike_amplitude_std = 0.0;
    std::vector<double> inter_spike_intervals;
    std::string pattern_type;  // "regular", "burst", "irregular", "pause"
    double burst_detection_score = 0.0;
    std::string analysis_notes;
};

struct SystemCallProfile {
    std::string call_name;
    uint64_t call_count = 0;
    double total_time_ms = 0.0;
    double average_time_ms = 0.0;
    std::string call_signature;
    std::string stack_trace;
    uint64_t memory_allocated = 0;
    uint64_t memory_freed = 0;
};

struct ProfileSession {
    std::string session_id;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    bool is_active = false;
    std::map<std::string, FunctionProfile> function_profiles;
    std::vector<MemoryProfile> memory_profiles;
    std::vector<AlgorithmProfile> algorithm_profiles;
    std::vector<TimelineEvent> timeline_events;
    std::vector<HotPathData> hot_path_data;
    std::vector<SpikeEventProfile> spike_event_profiles;
    std::map<std::string, SystemCallProfile> system_call_profiles;
    std::map<std::string, double> global_metrics;
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Disable copying and moving
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) = delete;
    Profiler& operator=(Profiler&&) = delete;
    
    // Session management
    std::string startSession(const std::string& session_name);
    void stopSession(const std::string& session_id);
    void pauseSession(const std::string& session_id);
    void resumeSession(const std::string& session_id);
    bool isSessionActive(const std::string& session_id) const;
    
    // Function profiling
    void profileFunctionEnter(const std::string& function_name, const std::string& category = "");
    void profileFunctionExit(const std::string& function_name);
    void profileFunctionCall(const std::string& function_name, const std::string& category = "", std::function<void()> func);
    
    // Memory profiling
    void profileMemoryAlloc(const std::string& alloc_site, uint64_t size, const std::string& stack_trace = "");
    void profileMemoryDealloc(const std::string& alloc_site, uint64_t size);
    void profileMemoryUsage(double memory_mb, const std::string& context = "");
    
    // Algorithm profiling
    void profileAlgorithm(const std::string& algorithm_name, uint64_t input_size,
                         std::function<void()> algorithm_func);
    void profileAlgorithm(const std::string& algorithm_name, uint64_t input_size,
                         double execution_time_ms, double memory_usage_mb);
    
    // Timeline profiling
    void profileTimelineEvent(const std::string& event_name, const std::string& event_type = "",
                             double duration_ms = 0.0, const std::string& thread_id = "");
    void profileTimelineMarker(const std::string& marker_name,
                              const std::map<std::string, std::string>& metadata = {});
    
    // Spike event profiling
    void profileSpikeEvent(uint64_t timestamp_ms, uint32_t spike_count,
                          const std::vector<double>& inter_spike_intervals = {});
    void profileSpikePattern(const std::string& pattern_type, double confidence_score);
    
    // System call profiling
    void profileSystemCallEnter(const std::string& call_name, const std::string& signature = "");
    void profileSystemCallExit(const std::string& call_name, double duration_ms, uint64_t memory_delta = 0);
    
    // Data retrieval
    ProfileSession getSession(const std::string& session_id) const;
    std::vector<ProfileSession> getAllSessions() const;
    FunctionProfile getFunctionProfile(const std::string& session_id, const std::string& function_name) const;
    std::vector<FunctionProfile> getAllFunctionProfiles(const std::string& session_id) const;
    std::vector<MemoryProfile> getMemoryProfiles(const std::string& session_id) const;
    std::vector<AlgorithmProfile> getAlgorithmProfiles(const std::string& session_id) const;
    std::vector<TimelineEvent> getTimelineEvents(const std::string& session_id) const;
    HotPathData getHotPathData(const std::string& session_id) const;
    std::vector<SpikeEventProfile> getSpikeEventProfiles(const std::string& session_id) const;
    std::map<std::string, SystemCallProfile> getSystemCallProfiles(const std::string& session_id) const;
    
    // Hot path analysis
    HotPathData analyzeHotPaths(const std::string& session_id) const;
    std::vector<std::string> identifyBottlenecks(const std::string& session_id, double threshold_percent = 5.0) const;
    
    // Spike event analysis
    SpikeEventProfile analyzeSpikePattern(const std::string& session_id, uint64_t step) const;
    std::vector<SpikeEventProfile> analyzeSpikePatternAcrossSteps(const std::string& session_id, uint64_t steps) const;
    std::string classifySpikePattern(const std::vector<double>& inter_spike_intervals) const;
    
    // System call analysis
    std::map<std::string, SystemCallProfile> analyzeSystemCalls(const std::string& session_id) const;
    std::string getMostExpensiveSystemCall(const std::string& session_id) const;
    std::string getMostFrequentSystemCall(const std::string& session_id) const;
    
    // Performance analysis
    double calculateOverallPerformanceScore(const std::string& session_id) const;
    std::map<std::string, double> getPerformanceMetrics(const std::string& session_id) const;
    std::string getPerformanceReport(const std::string& session_id) const;
    
    // Memory analysis
    struct MemoryLeakReport {
        bool leak_detected = false;
        double leak_rate_mb_per_step = 0.0;
        uint64_t leak_size_mb = 0;
        std::string leak_source_function;
        std::string recommendation;
    };
    
    MemoryLeakReport detectMemoryLeaks(const std::string& session_id) const;
    
    // Configuration
    void setMaxSamplesPerFunction(size_t max_samples) { max_samples_per_function_ = max_samples; }
    void setSamplingIntervalMs(uint64_t interval_ms) { sampling_interval_ms_ = interval_ms; }
    void enableRealTimeProfiling(bool enable) { real_time_profiling_ = enable; }
    
    // Data export
    bool exportProfile(const std::string& session_id, const std::string& filepath) const;
    bool exportHotPathReport(const std::string& session_id, const std::string& filepath) const;
    bool exportTimelineReport(const std::string& session_id, const std::string& filepath) const;
    bool exportMemoryReport(const std::string& session_id, const std::string& filepath) const;
    bool exportSpikeEventReport(const std::string& session_id, const std::string& filepath) const;
    bool exportSystemCallReport(const std::string& session_id, const std::string& filepath) const;
    
    // Cleanup
    void cleanupSession(const std::string& session_id);
    void cleanupOldSessions(uint64_t max_age_ms = 3600000); // 1 hour default
    
    // Statistics
    size_t getSessionCount() const { return sessions_.size(); }
    size_t getActiveSessionCount() const;
    std::vector<std::string> getActiveSessionIds() const;
    uint64_t getTotalFunctionCalls(const std::string& session_id) const;
    double getTotalExecutionTime(const std::string& session_id) const;
    
private:
    mutable std::mutex profiling_mutex_;
    std::unordered_map<std::string, ProfileSession> sessions_;
    std::atomic<uint64_t> next_session_id_ = 0;
    
    // Current profiling state
    struct ProfilingState {
        std::chrono::steady_clock::time_point enter_time;
        std::string current_function;
        std::string current_category;
        std::unordered_map<std::string, uint64_t> call_stack;
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> function_entry_times;
    } current_state_;
    
    size_t max_samples_per_function_ = 1000;
    uint64_t sampling_interval_ms_ = 100;
    bool real_time_profiling_ = false;
    
    // Internal helper methods
    std::string generateSessionId() const;
    void updateFunctionProfile(FunctionProfile& profile, double duration_ms);
    void addTimelineEvent(ProfileSession& session, const TimelineEvent& event);
    void addMemoryProfile(ProfileSession& session, const MemoryProfile& profile);
    void addAlgorithmProfile(ProfileSession& session, const AlgorithmProfile& profile);
    void addSpikeEventProfile(ProfileSession& session, const SpikeEventProfile& profile);
    void addSystemCallProfile(ProfileSession& session, const SystemCallProfile& profile);
    
    // Analysis helpers
    static void analyzeFunctionHotPath(const FunctionProfile& profile, HotPathData& hot_path_data, 
                                      double total_time_ms, double threshold_percent);
    static std::string classifySpikePattern(const std::vector<double>& inter_spike_intervals,
                                           double burst_threshold = 0.1);
    
    // Export helpers
    static std::string functionProfileToJSON(const FunctionProfile& profile);
    static std::string timelineEventToJSON(const TimelineEvent& event);
    static std::string memoryProfileToJSON(const MemoryProfile& profile);
};

} // namespace nlm
