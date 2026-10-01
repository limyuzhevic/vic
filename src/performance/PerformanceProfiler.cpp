#include "PerformanceProfiler.hpp"
#include <thread>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

PerformanceProfiler::PerformanceProfiler() 
    : profilingEnabled_(false)
    , exportOnStop_(true)
    , profilingInterval_(1000)
    , stepCount_(0)
    , totalSessionTime_(0.0)
{
    sessionStartTime_ = std::chrono::steady_clock::now();
    NLM_LOG_INFO("Performance profiler initialized");
}

PerformanceProfiler::~PerformanceProfiler() {
    if (profilingEnabled_ && exportOnStop_) {
        exportProfileDataInternal();
    }
    NLM_LOG_INFO("Performance profiler shutdown");
}

void PerformanceProfiler::startProfile() {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    sessionStartTime_ = std::chrono::steady_clock::now();
    stepCount_ = 0;
    totalSessionTime_ = 0.0;
    
    // Reset profile data
    currentProfile_ = ProfileData();
    eventProfiles_.clear();
    timerStartTimes_.clear();
    timerAccumulatedTimes_.clear();
    timerCallCounts_.clear();
    
    NLM_LOG_INFO("Performance profiling started");
}

void PerformanceProfiler::stopProfile() {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - sessionStartTime_);
    totalSessionTime_ = duration.count() / 1000.0;
    
    if (exportOnStop_) {
        exportProfileDataInternal();
    }
    
    NLM_LOG_INFO("Performance profiling stopped - Total time: " + formatTime(totalSessionTime_));
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(profileMutex_);
    
    currentProfile_ = ProfileData();
    eventProfiles_.clear();
    timerStartTimes_.clear();
    timerAccumulatedTimes_.clear();
    timerCallCounts_.clear();
    stepCount_ = 0;
    totalSessionTime_ = 0.0;
    
    NLM_LOG_INFO("Performance profiler reset");
}

void PerformanceProfiler::startTimer(const std::string& component) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    timerStartTimes_[component] = getCurrentTimestamp();
}

void PerformanceProfiler::stopTimer(const std::string& component) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    auto it = timerStartTimes_.find(component);
    if (it == timerStartTimes_.end()) return;
    
    double startTime = it->second;
    double endTime = getCurrentTimestamp();
    double duration = endTime - startTime;
    
    timerAccumulatedTimes_[component] += duration;
    timerCallCounts_[component]++;
    
    // Update profile data based on component
    if (component == "neuron_update") {
        currentProfile_.neuronUpdateTime += duration;
    } else if (component == "spike_processing") {
        currentProfile_.spikeProcessingTime += duration;
    } else if (component == "memory_update") {
        currentProfile_.memoryUpdateTime += duration;
    } else if (component == "neuromodulation") {
        currentProfile_.neuromodulationTime += duration;
    } else if (component == "plasticity") {
        currentProfile_.plasticityTime += duration;
    } else if (component == "prediction") {
        currentProfile_.predictionTime += duration;
    } else if (component == "concept_formation") {
        currentProfile_.conceptFormationTime += duration;
    } else if (component == "attention") {
        currentProfile_.attentionTime += duration;
    } else if (component == "development") {
        currentProfile_.developmentTime += duration;
    } else {
        currentProfile_.stepTime += duration;
    }
}

void PerformanceProfiler::updateBrainStats(const ProfileData& stats) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    currentProfile_.neuronCount = stats.neuronCount;
    currentProfile_.synapseCount = stats.synapseCount;
    currentProfile_.spikeCount = stats.spikeCount;
    currentProfile_.memoryTraceCount = stats.memoryTraceCount;
    currentProfile_.episodicMemoryCount = stats.episodicMemoryCount;
    currentProfile_.predictionErrorCount = stats.predictionErrorCount;
}

void PerformanceProfiler::updateMemoryStats(const MemoryProfile& mem) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    memoryProfile_ = mem;
}

void PerformanceProfiler::updateSystemStats(const SystemProfile& sys) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    systemProfile_ = sys;
}

void PerformanceProfiler::recordEvent(const std::string& eventName, double duration) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    if (!profilingEnabled_) return;
    
    for (auto& event : eventProfiles_) {
        if (event.eventName == eventName) {
            event.totalTime += duration;
            event.callCount++;
            return;
        }
    }
    
    EventProfile newEvent;
    newEvent.eventName = eventName;
    newEvent.totalTime = duration;
    newEvent.callCount = 1;
    eventProfiles_.push_back(newEvent);
}

void PerformanceProfiler::updateEventProfiles() {
    // Merge timer data into event profiles
    for (const auto& pair : timerAccumulatedTimes_) {
        auto it = std::find_if(eventProfiles_.begin(), eventProfiles_.end(),
                              [&pair](const EventProfile& e) { return e.eventName == pair.first; });
        
        if (it == eventProfiles_.end()) {
            EventProfile newEvent;
            newEvent.eventName = pair.first;
            newEvent.totalTime = pair.second;
            newEvent.callCount = timerCallCounts_[pair.first];
            eventProfiles_.push_back(newEvent);
        } else {
            it->totalTime = pair.second;
            it->callCount = timerCallCounts_[pair.first];
        }
    }
}

void PerformanceProfiler::updateSystemProfile() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - systemProfile_.lastUpdate);
    systemProfile_.lastUpdate = currentTime;
    
    double timeSeconds = duration.count() / 1000.0;
    if (timeSeconds > 0.0) {
        systemProfile_.stepRate = static_cast<double>(stepCount_) / timeSeconds;
        systemProfile_.spikeRate = static_cast<double>(currentProfile_.spikeCount) / timeSeconds;
        
        // Calculate activity level (normalized)
        double totalActivity = currentProfile_.neuronUpdateTime + currentProfile_.spikeProcessingTime +
                             currentProfile_.memoryUpdateTime + currentProfile_.neuromodulationTime;
        if (totalActivity > 0.0) {
            systemProfile_.activityLevel = std::min(1.0, totalActivity / 1000.0);  // Normalize to 0-1
        }
    }
}

void PerformanceProfiler::exportProfileDataInternal() const {
    if (outputFile_.empty()) return;
    
    std::string extension = outputFile_.substr(outputFile_.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "csv") {
        exportCSV(outputFile_);
    } else if (extension == "json") {
        exportJSON(outputFile_);
    } else {
        // Default to CSV
        std::string csvFile = outputFile_;
        if (outputFile_.find_last_of('.') == std::string::npos) {
            csvFile += ".csv";
        }
        exportCSV(csvFile);
    }
}

std::string PerformanceProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Profile ===\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n";
    oss << "Total Steps: " << stepCount_ << "\n";
    oss << "Total Time: " << formatTime(totalSessionTime_) << "\n";
    oss << "Steps per Second: " << (totalSessionTime_ > 0.0 ? stepCount_ / totalSessionTime_ : 0.0) << "\n\n";
    
    oss << "=== Component Performance ===\n";
    oss << "Neuron Updates: " << formatTime(currentProfile_.neuronUpdateTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.neuronUpdateTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Spike Processing: " << formatTime(currentProfile_.spikeProcessingTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.spikeProcessingTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Memory Updates: " << formatTime(currentProfile_.memoryUpdateTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.memoryUpdateTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Neuromodulation: " << formatTime(currentProfile_.neuromodulationTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.neuromodulationTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Plasticity: " << formatTime(currentProfile_.plasticityTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.plasticityTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Prediction: " << formatTime(currentProfile_.predictionTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.predictionTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Concept Formation: " << formatTime(currentProfile_.conceptFormationTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.conceptFormationTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Attention: " << formatTime(currentProfile_.attentionTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.attentionTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    oss << "Development: " << formatTime(currentProfile_.developmentTime) << " ("
        << (currentProfile_.totalCpuTime() > 0.0 ? currentProfile_.developmentTime / currentProfile_.totalCpuTime() * 100.0 : 0.0) << "% of CPU)\n";
    
    oss << "\n=== System Statistics ===\n";
    oss << "Neuron Count: " << currentProfile_.neuronCount << "\n";
    oss << "Synapse Count: " << currentProfile_.synapseCount << "\n";
    oss << "Total Spikes: " << currentProfile_.spikeCount << "\n";
    oss << "Memory Traces: " << currentProfile_.memoryTraceCount << "\n";
    oss << "Episodic Memory Episodes: " << currentProfile_.episodicMemoryCount << "\n";
    oss << "Prediction Errors: " << currentProfile_.predictionErrorCount << "\n";
    
    oss << "\n=== Integration Analysis ===\n";
    oss << "Integration Efficiency: " << formatPercentage(currentProfile_.getIntegrationEfficiency()) << "\n";
    oss << "CPU Utilization: " << formatPercentage(getSystemProfile().cpuUsage) << "\n";
    oss << "Memory Usage: " << formatMemory(memoryProfile_.currentUsage) << "\n";
    
    oss << "\n=== Event Profiles ===\n";
    for (const auto& event : eventProfiles_) {
        oss << event.eventName << ": " << formatTime(event.totalTime) << " ("
            << event.callCount << " calls, avg " << formatTime(event.avgTime()) << ")\n";
    }
    
    return oss.str();
}

void PerformanceProfiler::exportProfileData(const std::string& filename) const {
    exportCSV(filename);
}

void PerformanceProfiler::exportCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV file: " + filename);
        return;
    }
    
    file << formatCSVHeader() << "\n";
    
    double timestamp = 0.0;
    for (size_t i = 0; i < stepCount_; i += profilingInterval_) {
        file << formatCSVRow(timestamp, currentProfile_) << "\n";
        timestamp += profilingInterval_ * 0.001;  // Convert steps to seconds
    }
    
    file.close();
    NLM_LOG_INFO("Performance data exported to CSV: " + filename);
}

void PerformanceProfiler::exportJSON(const std::string& filename) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    oss << "  \"steps\": " << stepCount_ << ",\n";
    oss << "  \"total_time\": " << totalSessionTime_ << ",\n";
    oss << "  \"step_rate\": " << (totalSessionTime_ > 0.0 ? stepCount_ / totalSessionTime_ : 0.0) << ",\n";
    oss << "  \"profile_data\": {\n";
    
    // Component times
    oss << "    \"neuron_update\": " << currentProfile_.neuronUpdateTime << ",\n";
    oss << "    \"spike_processing\": " << currentProfile_.spikeProcessingTime << ",\n";
    oss << "    \"memory_update\": " << currentProfile_.memoryUpdateTime << ",\n";
    oss << "    \"neuromodulation\": " << currentProfile_.neuromodulationTime << ",\n";
    oss << "    \"plasticity\": " << currentProfile_.plasticityTime << ",\n";
    oss << "    \"prediction\": " << currentProfile_.predictionTime << ",\n";
    oss << "    \"concept_formation\": " << currentProfile_.conceptFormationTime << ",\n";
    oss << "    \"attention\": " << currentProfile_.attentionTime << ",\n";
    oss << "    \"development\": " << currentProfile_.developmentTime << ",\n";
    
    oss << "  },\n";
    oss << "  \"system_stats\": {\n";
    oss << "    \"cpu_usage\": " << systemProfile_.cpuUsage << ",\n";
    oss << "    \"memory_usage\": " << memoryProfile_.currentUsage << ",\n";
    oss << "    \"step_rate\": " << systemProfile_.stepRate << ",\n";
    oss << "    \"spike_rate\": " << systemProfile_.spikeRate << ",\n";
    oss << "    \"activity_level\": " << systemProfile_.activityLevel << "\n";
    
    oss << "  },\n";
    oss << "  \"event_profiles\": [\n";
    
    for (size_t i = 0; i < eventProfiles_.size(); ++i) {
        const auto& event = eventProfiles_[i];
        oss << "    {\"name\": \"" << event.eventName << "\", \"total_time\": " 
            << event.totalTime << ", \"call_count\": " << event.callCount << \"}";
        if (i < eventProfiles_.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filename);
        return;
    }
    
    file << oss.str();
    file.close();
    NLM_LOG_INFO("Performance data exported to JSON: " + filename);
}

std::string PerformanceProfiler::analyzeIntegrationEfficiency() const {
    double integrationScore = currentProfile_.getIntegrationEfficiency();
    double memoryScore = memoryProfile_.currentUsage / (1024 * 1024);  // MB
    double systemScore = systemProfile_.activityLevel * 100;
    
    double overallScore = (integrationScore + memoryScore + systemScore) / 3.0;
    
    std::ostringstream oss;
    oss << "=== Integration Efficiency Analysis ===\n";
    oss << "Integration Score: " << formatPercentage(integrationScore) << "\n";
    oss << "Memory Efficiency: " << formatMemory(memoryProfile_.currentUsage) << " ("
        << (memoryProfile_.currentUsage / (1024 * 1024)) << " MB)\n";
    oss << "System Activity: " << formatPercentage(systemProfile_.activityLevel * 100) << "\n";
    oss << "Overall Integration Score: " << formatPercentage(overallScore) << "\n\n";
    
    if (overallScore > 80.0) {
        oss << "✅ EXCELLENT: High integration efficiency and system balance\n";
    } else if (overallScore > 60.0) {
        oss << "✅ GOOD: Acceptable integration efficiency with room for improvement\n";
    } else if (overallScore > 40.0) {
        oss << "⚠️  MODERATE: Integration needs attention and optimization\n";
    } else {
        oss << "❌ POOR: Integration efficiency is critically low\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeSystemBalance() const {
    std::ostringstream oss;
    oss << "=== System Balance Analysis ===\n";
    
    // Calculate component balance
    double totalCpuTime = currentProfile_.getTotalCpuTime();
    if (totalCpuTime <= 0.0) {
        oss << "No CPU time recorded\n";
        return oss.str();
    }
    
    oss << "Component Distribution (percentage of CPU time):\n";
    
    struct Component {
        std::string name;
        double time;
        double percentage;
    };
    
    std::vector<Component> components = {
        {"Neuron Updates", currentProfile_.neuronUpdateTime},
        {"Spike Processing", currentProfile_.spikeProcessingTime},
        {"Memory Updates", currentProfile_.memoryUpdateTime},
        {"Neuromodulation", currentProfile_.neuromodulationTime},
        {"Plasticity", currentProfile_.plasticityTime},
        {"Prediction", currentProfile_.predictionTime},
        {"Concept Formation", currentProfile_.conceptFormationTime},
        {"Attention", currentProfile_.attentionTime},
        {"Development", currentProfile_.developmentTime},
        {"Other", currentProfile_.stepTime}
    };
    
    for (const auto& comp : components) {
        double percentage = (comp.time / totalCpuTime) * 100.0;
        oss << "  " << comp.name << ": " << formatPercentage(percentage) << "\n";
    }
    
    oss << "\nBalance Assessment:\n";
    
    // Check for imbalances
    bool hasImbalance = false;
    for (const auto& comp : components) {
        if (comp.percentage > 50.0) {
            oss << "  ⚠️  " << comp.name << " dominates CPU usage ("
                << formatPercentage(comp.percentage) << ")\n";
            hasImbalance = true;
        }
    }
    
    if (!hasImbalance) {
        oss << "  ✅ Good balance across components\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeBottlenecks() const {
    std::ostringstream oss;
    oss << "=== Bottleneck Analysis ===\n";
    
    // Check for potential bottlenecks
    bool hasBottleneck = false;
    
    // Memory bottleneck
    if (memoryProfile_.currentUsage > (512 * 1024 * 1024)) {  // 512MB
        oss << "  🔴 MEMORY BOTTLENECK: High memory usage ("
            << formatMemory(memoryProfile_.currentUsage) << ")\n";
        hasBottleneck = true;
    }
    
    // CPU bottleneck
    double totalCpuTime = currentProfile_.getTotalCpuTime();
    if (totalCpuTime > totalSessionTime_ * 2.0) {  // CPU time > 2x wall time (multi-threading)
        oss << "  🔴 CPU BOTTLENECK: High CPU utilization with potential thread contention\n";
        hasBottleneck = true;
    }
    
    // Spike processing bottleneck
    if (currentProfile_.spikeProcessingTime > totalCpuTime * 0.5) {
        oss << "  🔴 SPIKE PROCESSING BOTTLENECK: Spike processing consumes most CPU time\n";
        hasBottleneck = true;
    }
    
    // Low integration efficiency bottleneck
    if (currentProfile_.getIntegrationEfficiency() < 30.0) {
        oss << "  🔴 INTEGRATION BOTTLENECK: Low integration efficiency suggests system disconnect\n";
        hasBottleneck = true;
    }
    
    if (!hasBottleneck) {
        oss << "  ✅ No significant bottlenecks detected\n";
    }
    
    oss << "\nRecommendations:\n";
    
    if (memoryProfile_.currentUsage > (512 * 1024 * 1024)) {
        oss << "  - Consider memory optimization (pools, sparse storage)\n";
    }
    
    if (currentProfile_.spikeProcessingTime > totalCpuTime * 0.5) {
        oss << "  - Optimize spike processing (event queues, batching)\n";
    }
    
    if (currentProfile_.getIntegrationEfficiency() < 30.0) {
        oss << "  - Improve system integration (better coordination between components)\n";
    }
    
    return oss.str();
}

void PerformanceProfiler::configure(bool enableProfiling, const std::string& outputFile,
                                    size_t interval, bool exportOnStop) {
    profilingEnabled_ = enableProfiling;
    outputFile_ = outputFile;
    profilingInterval_ = interval;
    exportOnStop_ = exportOnStop;
    
    if (enableProfiling) {
        NLM_LOG_INFO("Performance profiler configured: enabled=" << enableProfiling
                    << ", output_file=" << outputFile_ << ", interval=" << interval);
    }
}

std::string PerformanceProfiler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string PerformanceProfiler::formatTime(double seconds) {
    if (seconds < 0.001) {
        return std::to_string(seconds * 1000000) + " μs";
    } else if (seconds < 1.0) {
        return std::to_string(seconds * 1000) + " ms";
    } else {
        return std::to_string(seconds) + " s";
    }
}

std::string PerformanceProfiler::formatMemory(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < (1024 * 1024)) {
        return std::to_string(bytes / 1024.0) + " KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    }
}

std::string PerformanceProfiler::formatCSVHeader() const {
    return "timestamp,neuron_count,synapse_count,spikes,memory_traces,episodic_episodes,prediction_errors,"
           "neuron_time,spike_time,memory_time,neuromod_time,plasticity_time,prediction_time,concept_time,attention_time,development_time,step_time,integration_efficiency";
}

std::string PerformanceProfiler::formatCSVRow(double timestamp, const ProfileData& profile) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << timestamp << ",";
    oss << profile.neuronCount << ",";
    oss << profile.synapseCount << ",";
    oss << profile.spikeCount << ",";
    oss << profile.memoryTraceCount << ",";
    oss << profile.episodicMemoryCount << ",\";
    oss << profile.predictionErrorCount << ",\";
    oss << profile.neuronUpdateTime << ",";
    oss << profile.spikeProcessingTime << ",";
    oss << profile.memoryUpdateTime << ",\";
    oss << profile.neuromodulationTime << ",\";
    oss << profile.plasticityTime << ",\";
    oss << profile.predictionTime << ",\";
    oss << profile.conceptFormationTime << ",\";
    oss << profile.attentionTime << ",\";
    oss << profile.developmentTime << ",\";
    oss << profile.stepTime << ",";
    oss << profile.getIntegrationEfficiency();
    
    return oss.str();
}

// BrainProfiler implementation

BrainProfiler::BrainProfiler(std::shared_ptr<PerformanceProfiler> profiler)
    : profiler_(profiler)
    , componentProfilingEnabled_(false)
    , stepLoggingEnabled_(false)
    , stepCount_(0)
    , totalSimulationTime_(0.0)
{
    if (profiler_) {
        NLM_LOG_INFO("Brain profiler initialized");
    }
}

BrainProfiler::~BrainProfiler() {
    if (profiler_) {
        NLM_LOG_INFO("Brain profiler shutdown");
    }
}

void BrainProfiler::startStepProfiling(SimulationStep step, Timestamp time) {
    if (!profiler_ || !componentProfilingEnabled_) return;
    
    stepCount_++;
    stepStartTime_ = std::chrono::steady_clock::now();
    
    // Start component timers
    profiler_->startTimer("neuron_update");
    profiler_->startTimer("spike_processing");
    profiler_->startTimer("memory_update");
    profiler_->startTimer("neuromodulation");
    profiler_->startTimer("plasticity");
    profiler_->startTimer("prediction");
    profiler_->startTimer("concept_formation");
    profiler_->startTimer("attention");
    profiler_->startTimer("development");
    profiler_->startTimer("step");
    
    if (stepLoggingEnabled_) {
        NLM_LOG_DEBUG("Step " << step << " started at time " << time);
    }
}

void BrainProfiler::endStepProfiling() {
    if (!profiler_ || !componentProfilingEnabled_) return;
    
    // Calculate step duration
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - stepStartTime_);
    totalSimulationTime_ += duration.count() / 1000.0;
    
    // Stop component timers
    profiler_->stopTimer("neuron_update");
    profiler_->stopTimer("spike_processing");
    profiler_->stopTimer("memory_update");
    profiler_->stopTimer("neuromodulation");
    profiler_->stopTimer("plasticity");
    profiler_->stopTimer("prediction");
    profiler_->stopTimer("concept_formation");
    profiler_->stopTimer("attention");
    profiler_->stopTimer("development");
    profiler_->stopTimer("step");
    
    // Record brain step event
    profiler_->recordEvent("brain_step", duration.count() / 1000.0);
    
    if (stepLoggingEnabled_) {
        NLM_LOG_DEBUG("Step completed in " << duration.count() / 1000.0 << " seconds");
    }
}

void BrainProfiler::profileMemorySystems(const ProfileData& stats) {
    if (!profiler_) return;
    profiler_->updateBrainStats(stats);
}

void BrainProfiler::profileNeuromodulation(const ProfileData& stats) {
    if (!profiler_) return;
    // Neuromodulation is part of step timing
}

void BrainProfiler::profilePredictionSystem(const ProfileData& stats) {
    if (!profiler_) return;
    // Prediction system is part of step timing
}

void BrainProfiler::profileCognitiveSystems(const ProfileData& stats) {
    if (!profiler_) return;
    // Cognitive systems are part of step timing
}

void BrainProfiler::profileDevelopmentSystem(const ProfileData& stats) {
    if (!profiler_) return;
    // Development system is part of step timing
}

std::string BrainProfiler::getIntegrationEfficiencyReport() const {
    if (!profiler_) return "Performance profiler not available";
    
    return profiler_->analyzeIntegrationEfficiency();
}

std::string BrainProfiler::getSystemBalanceReport() const {
    if (!profiler_) return "Performance profiler not available";
    
    return profiler_->analyzeSystemBalance();
}

std::string BrainProfiler::getPerformanceBottleneckReport() const {
    if (!profiler_) return "Performance profiler not available";
    
    return profiler_->analyzeBottlenecks();
}
}

// PerformanceUtils implementation

namespace PerformanceUtils {
    std::string formatNumber(size_t number) {
        std::stringstream ss;
        ss << number;
        return ss.str();
    }
    
    std::string formatPercentage(double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << value << "%";
        return ss.str();
    }
    
    std::string formatTimeDelta(double seconds) {
        if (seconds < 0.001) {
            return std::to_string(seconds * 1000000) + "μs";
        } else if (seconds < 1.0) {
            return std::to_string(seconds * 1000) + "ms";
        } else {
            return std::to_string(seconds) + "s";
        }
    }
    
    void setProfileOutputDirectory(const std::string& dir) {
        // TODO: Implement directory setup
    }
    
    void setBenchmarkResultsDirectory(const std::string& dir) {
        // TODO: Implement directory setup
    }
}

} // namespace nlm
