// Performance Profiling Header
// File: src/performance/Profiler.hpp
#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfileResult {
        std::string name;
        double duration_ms;
        uint64_t memory_usage;
        int64_t cpu_cycles;
        int call_count;
        double avg_duration_ms;
        std::string file;
        int line;
        
        ProfileResult(const std::string& n, double dur, uint64_t mem = 0, int64_t cycles = 0, 
                     int count = 1, const std::string& f = "", int l = 0)
            : name(n), duration_ms(dur), memory_usage(mem), cpu_cycles(cycles), 
              call_count(count), avg_duration_ms(dur / count), file(f), line(l) {}
    };

    class ProfilerScope {
    public:
        ProfilerScope(PerformanceProfiler& profiler, const std::string& name, 
                     const std::string& file = "", int line = 0);
        ~ProfilerScope();
        
        void stop();
        double getDuration() const;
        
    private:
        PerformanceProfiler& profiler_;
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_time_;
        bool stopped_ = false;
        std::string file_;
        int line_;
    };

    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void start(const std::string& name, const std::string& file = "", int line = 0);
    void stop(const std::string& name);
    void pause(const std::string& name);
    void resume(const std::string& name);
    
    // Manual timing
    void record(const std::string& name, double duration_ms, 
                uint64_t memory_usage = 0, int64_t cpu_cycles = 0,
                const std::string& file = "", int line = 0);
    
    // Get results
    const std::vector<ProfileResult>& getResults() const { return results_; }
    std::vector<ProfileResult> getResultsByDuration() const;
    std::vector<ProfileResult> getResultsByCount() const;
    
    // Report and analysis
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    void reset();
    
    // Configuration
    void setSamplingRate(double samples_per_second);
    void setMemoryTracking(bool enabled);
    void setCpuTracking(bool enabled);
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    struct ProfileSession {
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point last_pause_time;
        double total_duration_ms = 0.0;
        bool is_paused = false;
        int call_count = 0;
        uint64_t memory_samples = 0;
        int64_t cpu_cycles = 0;
    };
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ProfileSession> active_sessions_;
    std::vector<ProfileResult> results_;
    double sampling_rate_hz_ = 1000.0; // Default sampling rate
    bool memory_tracking_enabled_ = true;
    bool cpu_tracking_enabled_ = true;
    
    double getCurrentMemoryUsage() const;
    int64_t getCurrentCpuCycles() const;
    void cleanupCompletedSessions();
};

// Global profiler instance
PerformanceProfiler& getGlobalProfiler();

// Macro for easy profiling
#define PROFILE_SCOPE(name) static nlm::PerformanceProfiler::ProfilerScope profile_scope(name, __FILE__, __LINE__)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__func__)
#define PROFILE_OPERATION(name) auto profile_operation = nlm::getGlobalProfiler().start(name, __FILE__, __LINE__)

// Specialized profilers for different components
class ConfigProfiler {
public:
    static void profileJsonParse(const std::string& content, const std::string& extension);
    static void profileYamlParse(const std::string& content);
    static void profileCommandLineParse(int argc, char** argv);
    static void profileEnvironmentParse();
    static void profileValueAccess(const std::string& key);
    static void profileValidation(const std::vector<nlm::ConfigEntry>& entries);
};

class LoggerProfiler {
public:
    static void profileLogEntry(const LogEntry& entry);
    static void profileLogFormat(const std::string& message, LogLevel level);
    static void profileFileWrite(const std::string& filepath);
};

class RandomProfiler {
public:
    static void profileUniformInt(int min, int max);
    static void profileUniformReal(float min, float max);
    static void profileNormal(float mean, float stddev);
    static void profileBernoulli(float probability);
    static void profileShuffle(const std::vector<int>& data);
};

class NeuronProfiler {
public:
    static void profileLIFStep(const NeuronState& state, double dt);
    static void profileSynapticInput(const std::vector<float>& inputs);
    static void profileSpikeDetection(const NeuronState& state);
    static void profileRefractoryUpdate(uint32_t remaining, uint32_t period);
};

class SynapseProfiler {
public:
    static void profileTransmission(const std::vector<float>& weights);
    static void profilePlasticityUpdate(float delta, float learningRate);
    static void profileDelayProcessing(const std::vector<float>& delays);
};

class MemoryProfiler {
public:
    static void profileWorkingMemoryAccess(const std::string& key, bool isWrite);
    static void profileEpisodicMemorySearch(const std::string& pattern);
    static void profileConsolidation(const std::string& memoryId);
    static void profileCompetition(const std::vector<float>& strengths);
};

class NeuromodulatorProfiler {
public:
    static void profileRewardPredictionError(float reward, float value);
    static void profileCuriosityUpdate(float uncertainty, float novelty);
    static void profileNoveltyDetection(const std::vector<float>& patterns);
    static void profilePredictionError(float prediction, float actual);
};

class PlasticityProfiler {
public:
    static void profileSTDPUpdate(const std::vector<Timestamp>& spikes, float deltaT);
    static void profileHebbianLearning(const std::vector<float>& pre, const std::vector<float>& post);
    static void profileStructuralPlasticity(const std::string& operation);
    static void profileHomeostaticRegulation(float meanWeight);
};

class PredictionProfiler {
public:
    static void profileForwardModel(const std::vector<float>& state, const std::vector<float>& action);
    static void profileInference(const std::vector<float>& observations);
    static void profilePlanning(const std::vector<std::vector<float>>& trajectories);
    static void profileConfidenceCalculation(const std::vector<float>& predictions);
};
