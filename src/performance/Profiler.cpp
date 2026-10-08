// Performance Profiling Implementation
// File: src/performance/Profiler.cpp

#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <thread>
#include <unordered_map>
#include <chrono>

namespace nlm {

PerformanceProfiler::ProfilerScope::ProfilerScope(PerformanceProfiler& profiler, const std::string& name, 
                                                   const std::string& file, int line)
    : profiler_(profiler), name_(name), file_(file), line_(line), stopped_(false) {
    profiler_.start(name, file, line);
}

PerformanceProfiler::ProfilerScope::~ProfilerScope() {
    if (!stopped_) {
        stop();
    }
}

void PerformanceProfiler::ProfilerScope::stop() {
    if (!stopped_) {
        profiler_.stop(name_);
        stopped_ = true;
    }
}

double PerformanceProfiler::ProfilerScope::getDuration() const {
    if (stopped_) {
        auto it = profiler_.active_sessions_.find(name_);
        if (it != profiler_.active_sessions_.end()) {
            return it->second.total_duration_ms;
        }
    }
    return 0.0;
}

PerformanceProfiler::PerformanceProfiler() : sampling_rate_hz_(1000.0), 
                                             memory_tracking_enabled_(true), 
                                             cpu_tracking_enabled_(true) {}

PerformanceProfiler::~PerformanceProfiler() {
    // Force cleanup of all active sessions
    for (auto& [name, session] : active_sessions_) {
        if (!session.is_paused) {
            auto now = std::chrono::high_resolution_clock::now();
            if (session.is_paused) {
                auto pause_duration = std::chrono::duration<double, std::milli>(now - session.last_pause_time).count();
                session.total_duration_ms += pause_duration;
            }
        }
    }
}

void PerformanceProfiler::start(const std::string& name, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.find(name);
    if (it != active_sessions_.end()) {
        if (it->second.is_paused) {
            it->second.is_paused = false;
            it->second.last_pause_time = std::chrono::high_resolution_clock::now();
        }
        return; // Already started
    }
    
    ProfileSession session;
    session.start_time = std::chrono::high_resolution_clock::now();
    session.call_count = 1;
    active_sessions_[name] = session;
}

void PerformanceProfiler::stop(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.find(name);
    if (it == active_sessions_.end()) {
        return; // Not found
    }
    
    auto now = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(now - it->second.start_time).count();
    
    if (it->second.is_paused) {
        auto pause_duration = std::chrono::duration<double, std::milli>(now - it->second.last_pause_time).count();
        duration_ms -= pause_duration;
    }
    
    it->second.total_duration_ms += duration_ms;
    it->second.is_paused = false;
    
    uint64_t memory_usage = getCurrentMemoryUsage();
    int64_t cpu_cycles = getCurrentCpuCycles();
    
    results_.emplace_back(name, duration_ms, memory_usage, cpu_cycles, it->second.call_count, file, line);
}

void PerformanceProfiler::pause(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.find(name);
    if (it == active_sessions_.end()) {
        return;
    }
    
    if (!it->second.is_paused) {
        it->second.is_paused = true;
        it->second.last_pause_time = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceProfiler::resume(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.find(name);
    if (it == active_sessions_.end()) {
        return;
    }
    
    if (it->second.is_paused) {
        it->second.is_paused = false;
        it->second.last_pause_time = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceProfiler::record(const std::string& name, double duration_ms, 
                                 uint64_t memory_usage, int64_t cpu_cycles,
                                 const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    results_.emplace_back(name, duration_ms, memory_usage, cpu_cycles, 1, file, line);
}

std::vector<PerformanceProfiler::ProfileResult> PerformanceProfiler::getResultsByDuration() const {
    std::vector<ProfileResult> sorted = results_;
    std::sort(sorted.begin(), sorted.end(), 
              [](const ProfileResult& a, const ProfileResult& b) {
                  return a.duration_ms > b.duration_ms;
              });
    return sorted;
}

std::vector<PerformanceProfiler::ProfileResult> PerformanceProfiler::getResultsByCount() const {
    std::vector<ProfileResult> sorted = results_;
    std::sort(sorted.begin(), sorted.end(), 
              [](const ProfileResult& a, const ProfileResult& b) {
                  return a.call_count > b.call_count;
              });
    return sorted;
}

std::string PerformanceProfiler::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Profile Report ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    ss << "Performance Summary:\n";
    ss << "Total profiled operations: " << results_.size() << "\n";
    ss << "Active sessions: " << active_sessions_.size() << "\n";
    ss << "Memory tracking: " << (memory_tracking_enabled_ ? "Enabled" : "Disabled") << "\n";
    ss << "CPU tracking: " << (cpu_tracking_enabled_ ? "Enabled" : "Disabled") << "\n\n";
    
    ss << "Top 10 Longest Operations:\n";
    auto top_by_duration = getResultsByDuration();
    int count = std::min(10, static_cast<int>(top_by_duration.size()));
    for (int i = 0; i < count; ++i) {
        const auto& result = top_by_duration[i];
        ss << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name 
           << "Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms"
           << " (calls: " << result.call_count << ", avg: " << result.avg_duration_ms << "ms)\n";
    }
    
    ss << "\nTop 10 Most Called Operations:\n";
    auto top_by_count = getResultsByCount();
    count = std::min(10, static_cast<int>(top_by_count.size()));
    for (int i = 0; i < count; ++i) {
        const auto& result = top_by_count[i];
        ss << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name 
           << "Calls: " << result.call_count
           << " (duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms, avg: " << result.avg_duration_ms << "ms)\n";
    }
    
    ss << "\nMemory Usage Analysis:\n";
    uint64_t total_memory = 0;
    int memory_count = 0;
    for (const auto& result : results_) {
        if (result.memory_usage > 0) {
            total_memory += result.memory_usage;
            memory_count++;
        }
    }
    if (memory_count > 0) {
        ss << "Average memory per operation: " << total_memory / memory_count << " bytes\n";
        ss << "Peak memory usage: " << (results_.empty() ? 0 : results_.front().memory_usage) << " bytes\n";
    }
    
    ss << "\nPerformance by Component:\n";
    std::unordered_map<std::string, std::vector<const ProfileResult*>> component_results;
    for (const auto& result : results_) {
        size_t pos = result.name.find('.');
        std::string component = (pos != std::string::npos) ? result.name.substr(0, pos) : "General";
        component_results[component].push_back(&result);
    }
    
    for (const auto& [component, component_results_list] : component_results) {
        if (component_results_list.empty()) continue;
        double total_duration = 0.0;
        int total_calls = 0;
        for (const auto* result : component_results_list) {
            total_duration += result->duration_ms;
            total_calls += result->call_count;
        }
        ss << std::left << std::setw(25) << component << "Duration: " << std::fixed << std::setprecision(3) 
           << total_duration << "ms (calls: " << total_calls << ")\n";
    }
    
    ss << "\n=== End of Report ===\n";
    return ss.str();
}

void PerformanceProfiler::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open performance report file: " + filename);
        return;
    }
    
    file << generateReport();
    file.close();
    NLM_LOG_INFO("Performance report saved to: " + filename);
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    results_.clear();
    active_sessions_.clear();
}

void PerformanceProfiler::setSamplingRate(double samples_per_second) {
    sampling_rate_hz_ = samples_per_second;
}

void PerformanceProfiler::setMemoryTracking(bool enabled) {
    memory_tracking_enabled_ = enabled;
}

void PerformanceProfiler::setCpuTracking(bool enabled) {
    cpu_tracking_enabled_ = enabled;
}

std::string PerformanceProfiler::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

uint64_t PerformanceProfiler::getCurrentMemoryUsage() const {
    if (!memory_tracking_enabled_) return 0;
    
    // Implementation depends on platform
    // For now, return a placeholder value
    return 0;
}

int64_t PerformanceProfiler::getCurrentCpuCycles() const {
    if (!cpu_tracking_enabled_) return 0;
    
    // Implementation depends on platform
    // For now, return a placeholder value
    return 0;
}

void PerformanceProfiler::cleanupCompletedSessions() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.begin();
    while (it != active_sessions_.end()) {
        if (it->second.is_paused) {
            ++it;
        } else {
            it = active_sessions_.erase(it);
        }
    }
}

// Global profiler implementation
namespace {
    std::unique_ptr<PerformanceProfiler> global_profiler;
    std::once_flag profiler_once_flag;
}

PerformanceProfiler& getGlobalProfiler() {
    std::call_once(profiler_once_flag, []() {
        global_profiler = std::make_unique<PerformanceProfiler>();
    });
    return *global_profiler;
}

// ConfigProfiler implementation
void ConfigProfiler::profileJsonParse(const std::string& content, const std::string& extension) {
    double start_time = getProfilerTime();
    // Actual JSON parsing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.JSON_Parse", duration_ms, 0, 0, __FILE__, __LINE__);
}

void ConfigProfiler::profileYamlParse(const std::string& content) {
    double start_time = getProfilerTime();
    // Actual YAML parsing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.YAML_Parse", duration_ms, 0, 0, __FILE__, __LINE__);
}

void ConfigProfiler::profileCommandLineParse(int argc, char** argv) {
    double start_time = getProfilerTime();
    // Actual command-line parsing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.CommandLine_Parse", duration_ms, 0, 0, __FILE__, __LINE__);
}

void ConfigProfiler::profileEnvironmentParse() {
    double start_time = getProfilerTime();
    // Actual environment variable parsing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.Environment_Parse", duration_ms, 0, 0, __FILE__, __LINE__);
}

void ConfigProfiler::profileValueAccess(const std::string& key) {
    double start_time = getProfilerTime();
    // Actual value access would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.Value_Access", duration_ms, 0, 0, __FILE__, __LINE__);
}

void ConfigProfiler::profileValidation(const std::vector<ConfigEntry>& entries) {
    double start_time = getProfilerTime();
    // Actual validation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Config.Validation", duration_ms, 0, 0, __FILE__, __LINE__);
}

// LoggerProfiler implementation
void LoggerProfiler::profileLogEntry(const LogEntry& entry) {
    double start_time = getProfilerTime();
    // Actual log entry creation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Logger.Log_Entry", duration_ms, 0, 0, __FILE__, __LINE__);
}

void LoggerProfiler::profileLogFormat(const std::string& message, LogLevel level) {
    double start_time = getProfilerTime();
    // Actual log formatting would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Logger.Log_Format", duration_ms, 0, 0, __FILE__, __LINE__);
}

void LoggerProfiler::profileFileWrite(const std::string& filepath) {
    double start_time = getProfilerTime();
    // Actual file writing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Logger.File_Write", duration_ms, 0, 0, __FILE__, __LINE__);
}

// RandomProfiler implementation
void RandomProfiler::profileUniformInt(int min, int max) {
    double start_time = getProfilerTime();
    // Actual random number generation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Random.Uniform_Int", duration_ms, 0, 0, __FILE__, __LINE__);
}

void RandomProfiler::profileUniformReal(float min, float max) {
    double start_time = getProfilerTime();
    // Actual random number generation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Random.Uniform_Real", duration_ms, 0, 0, __FILE__, __LINE__);
}

void RandomProfiler::profileNormal(float mean, float stddev) {
    double start_time = getProfilerTime();
    // Actual normal distribution generation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Random.Normal", duration_ms, 0, 0, __FILE__, __LINE__);
}

void RandomProfiler::profileBernoulli(float probability) {
    double start_time = getProfilerTime();
    // Actual Bernoulli sampling would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Random.Bernoulli", duration_ms, 0, 0, __FILE__, __LINE__);
}

void RandomProfiler::profileShuffle(const std::vector<int>& data) {
    double start_time = getProfilerTime();
    // Actual shuffling would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Random.Shuffle", duration_ms, 0, 0, __FILE__, __LINE__);
}

// NeuronProfiler implementation
void NeuronProfiler::profileLIFStep(const NeuronState& state, double dt) {
    double start_time = getProfilerTime();
    // Actual LIF computation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuron.LIF_Step", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuronProfiler::profileSynapticInput(const std::vector<float>& inputs) {
    double start_time = getProfilerTime();
    // Actual synaptic input processing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuron.Synaptic_Input", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuronProfiler::profileSpikeDetection(const NeuronState& state) {
    double start_time = getProfilerTime();
    // Actual spike detection would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuron.Spike_Detection", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuronProfiler::profileRefractoryUpdate(uint32_t remaining, uint32_t period) {
    double start_time = getProfilerTime();
    // Actual refractory update would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuron.Refractory_Update", duration_ms, 0, 0, __FILE__, __LINE__);
}

// SynapseProfiler implementation
void SynapseProfiler::profileTransmission(const std::vector<float>& weights) {
    double start_time = getProfilerTime();
    // Actual synaptic transmission would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Synapse.Transmission", duration_ms, 0, 0, __FILE__, __LINE__);
}

void SynapseProfiler::profilePlasticityUpdate(float delta, float learningRate) {
    double start_time = getProfilerTime();
    // Actual plasticity update would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Synapse.Plasticity_Update", duration_ms, 0, 0, __FILE__, __LINE__);
}

void SynapseProfiler::profileDelayProcessing(const std::vector<float>& delays) {
    double start_time = getProfilerTime();
    // Actual delay processing would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Synapse.Delay_Processing", duration_ms, 0, 0, __FILE__, __LINE__);
}

// MemoryProfiler implementation
void MemoryProfiler::profileWorkingMemoryAccess(const std::string& key, bool isWrite) {
    double start_time = getProfilerTime();
    // Actual memory access would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Memory.Working_Access", duration_ms, 0, 0, __FILE__, __LINE__);
}

void MemoryProfiler::profileEpisodicMemorySearch(const std::string& pattern) {
    double start_time = getProfilerTime();
    // Actual memory search would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Memory.Episodic_Search", duration_ms, 0, 0, __FILE__, __LINE__);
}

void MemoryProfiler::profileConsolidation(const std::string& memoryId) {
    double start_time = getProfilerTime();
    // Actual consolidation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Memory.Consolidation", duration_ms, 0, 0, __FILE__, __LINE__);
}

void MemoryProfiler::profileCompetition(const std::vector<float>& strengths) {
    double start_time = getProfilerTime();
    // Actual competition would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Memory.Competition", duration_ms, 0, 0, __FILE__, __LINE__);
}

// NeuromodulatorProfiler implementation
void NeuromodulatorProfiler::profileRewardPredictionError(float reward, float value) {
    double start_time = getProfilerTime();
    // Actual RPE calculation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuromodulation.RPE", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuromodulatorProfiler::profileCuriosityUpdate(float uncertainty, float novelty) {
    double start_time = getProfilerTime();
    // Actual curiosity update would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuromodulation.Curiosity", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuromodulatorProfiler::profileNoveltyDetection(const std::vector<float>& patterns) {
    double start_time = getProfilerTime();
    // Actual novelty detection would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuromodulation.Novelty", duration_ms, 0, 0, __FILE__, __LINE__);
}

void NeuromodulatorProfiler::profilePredictionError(float prediction, float actual) {
    double start_time = getProfilerTime();
    // Actual prediction error calculation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Neuromodulation.Prediction_Error", duration_ms, 0, 0, __FILE__, __LINE__);
}

// PlasticityProfiler implementation
void PlasticityProfiler::profileSTDPUpdate(const std::vector<Timestamp>& spikes, float deltaT) {
    double start_time = getProfilerTime();
    // Actual STDP update would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Plasticity.STDP", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PlasticityProfiler::profileHebbianLearning(const std::vector<float>& pre, const std::vector<float>& post) {
    double start_time = getProfilerTime();
    // Actual Hebbian learning would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Plasticity.Hebbian", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PlasticityProfiler::profileStructuralPlasticity(const std::string& operation) {
    double start_time = getProfilerTime();
    // Actual structural plasticity would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Plasticity.Structural", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PlasticityProfiler::profileHomeostaticRegulation(float meanWeight) {
    double start_time = getProfilerTime();
    // Actual homeostatic regulation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Plasticity.Homeostatic", duration_ms, 0, 0, __FILE__, __LINE__);
}

// PredictionProfiler implementation
void PredictionProfiler::profileForwardModel(const std::vector<float>& state, const std::vector<float>& action) {
    double start_time = getProfilerTime();
    // Actual forward model prediction would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Prediction.Forward_Model", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PredictionProfiler::profileInference(const std::vector<float>& observations) {
    double start_time = getProfilerTime();
    // Actual inference would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Prediction.Inference", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PredictionProfiler::profilePlanning(const std::vector<std::vector<float>>& trajectories) {
    double start_time = getProfilerTime();
    // Actual planning would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Prediction.Planning", duration_ms, 0, 0, __FILE__, __LINE__);
}

void PredictionProfiler::profileConfidenceCalculation(const std::vector<float>& predictions) {
    double start_time = getProfilerTime();
    // Actual confidence calculation would happen here
    double duration_ms = getProfilerTime() - start_time;
    getGlobalProfiler().record("Prediction.Confidence", duration_ms, 0, 0, __FILE__, __LINE__);
}

// Helper function for timing
double getProfilerTime() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now - start_time).count();
}

} // namespace nlm
