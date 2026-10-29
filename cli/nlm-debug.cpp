#!/usr/bin/env cpp
// NLM Advanced Debugging and Profiling Tools
// Provides comprehensive debugging capabilities for NLM systems

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <deque>
#include <cmath>

// NLM includes
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/dynamics/SpikeSystem.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Types/Types.hpp"

namespace nlm {

// Debug Event Types
enum class DebugEventType {
    SpikeGenerated,
    SpikeProcessed,
    MemoryStored,
    MemoryRetrieved,
    PlasticityUpdated,
    NeuromodulationChanged,
    LearningOccurred,
    CheckpointSaved,
    CheckpointLoaded,
    ErrorOccurred,
    PerformanceSample,
    ResourceUsage
};

// Debug Event Structure
struct DebugEvent {
    DebugEventType type;
    SimulationStep step;
    Timestamp timestamp;
    std::string description;
    std::map<std::string, std::string> metadata;
    double duration; // In milliseconds
    
    DebugEvent(DebugEventType t, SimulationStep s, Timestamp ts, const std::string& desc)
        : type(t), step(s), timestamp(ts), description(desc), duration(0.0) {}
};

// Performance Counter
struct PerformanceCounter {
    std::string name;
    std::chrono::steady_clock::time_point startTime;
    double totalDuration;
    uint64_t samples;
    double minDuration;
    double maxDuration;
    double avgDuration;
    
    PerformanceCounter(const std::string& n) 
        : name(n), startTime(std::chrono::steady_clock::now()), 
          totalDuration(0.0), samples(0), minDuration(std::numeric_limits<double>::max()),
          maxDuration(0.0), avgDuration(0.0) {}
    
    void start() {
        startTime = std::chrono::steady_clock::now();
    }
    
    void stop() {
        auto endTime = std::chrono::steady_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        totalDuration += duration;
        samples++;
        minDuration = std::min(minDuration, duration);
        maxDuration = std::max(maxDuration, duration);
        avgDuration = totalDuration / samples;
    }
    
    std::string report() const {
        std::ostringstream oss;
        oss << name << ": samples=" << samples 
            << ", avg=" << std::fixed << std::setprecision(3) << avgDuration << "ms "
            << "(min=" << minDuration << "ms, max=" << maxDuration << "ms)";
        return oss.str();
    }
};

// Debug Event Logger
class DebugEventLogger {
public:
    std::vector<DebugEvent> events;
    SimulationStep currentStep;
    std::chrono::steady_clock::time_point sessionStart;
    std::string sessionFile;
    
    DebugEventLogger() : currentStep(0), sessionStart(std::chrono::steady_clock::now()) {}
    
    void logEvent(DebugEventType type, const std::string& description,
                  const std::map<std::string, std::string>& metadata = {}) {
        DebugEvent event(type, currentStep, 
                        static_cast<Timestamp>(currentStep * 0.001), description);
        event.metadata = metadata;
        
        // Calculate duration since last event of same type
        double duration = 0.0; // Simplified - in real implementation would track timestamps
        event.duration = duration;
        
        events.push_back(event);
        
        // Log to console
        std::cout << "[" << currentStep << "] " << debugEventTypeToString(type) 
                  << " - " << description << std::endl;
        
        // Save to session file if set
        if (!sessionFile.empty()) {
            saveToFile();
        }
    }
    
    void incrementStep() {
        currentStep++;
    }
    
    void setSessionFile(const std::string& filepath) {
        sessionFile = filepath;
        saveToFile();
    }
    
    void saveToFile() {
        if (sessionFile.empty()) return;
        
        std::ofstream file(sessionFile, std::ios::app);
        if (!file.is_open()) return;
        
        for (const auto& event : events) {
            file << "[" << event.step << "] " << debugEventTypeToString(event.type)
                 << " - " << event.description << " (" << event.duration << "ms)" << std::endl;
            for (const auto& pair : event.metadata) {
                file << "  " << pair.first << "=" << pair.second << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
    }
    
    std::string getSessionStats() const {
        std::ostringstream oss;
        oss << "Debug Session Statistics:" << std::endl;
        oss << "  Total events: " << events.size() << std::endl;
        oss << "  Current step: " << currentStep << std::endl;
        
        std::map<DebugEventType, size_t> eventCounts;
        for (const auto& event : events) {
            eventCounts[event.type]++;
        }
        
        oss << "  Events by type:" << std::endl;
        for (const auto& pair : eventCounts) {
            oss << "    " << debugEventTypeToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }
    
private:
    std::string debugEventTypeToString(DebugEventType type) {
        switch (type) {
            case DebugEventType::SpikeGenerated: return "SPIKE_GENERATED";
            case DebugEventType::SpikeProcessed: return "SPIKE_PROCESSED";
            case DebugEventType::MemoryStored: return "MEMORY_STORED";
            case DebugEventType::MemoryRetrieved: return "MEMORY_RETRIEVED";
            case DebugEventType::PlasticityUpdated: return "PLASTICITY_UPDATED";
            case DebugEventType::NeuromodulationChanged: return "NEUROMODULATION_CHANGED";
            case DebugEventType::LearningOccurred: return "LEARNING_OCCURRED";
            case DebugEventType::CheckpointSaved: return "CHECKPOINT_SAVED";
            case DebugEventType::CheckpointLoaded: return "CHECKPOINT_LOADED";
            case DebugEventType::ErrorOccurred: return "ERROR_OCCURRED";
            case DebugEventType::PerformanceSample: return "PERFORMANCE_SAMPLE";
            case DebugEventType::ResourceUsage: return "RESOURCE_USAGE";
            default: return "UNKNOWN";
        }
    }
};

// Performance Profiler
class PerformanceProfiler {
public:
    std::vector<PerformanceCounter> counters;
    std::vector<double> sampleTimestamps;
    SimulationStep currentStep;
    bool profilingEnabled;
    
    PerformanceProfiler() : currentStep(0), profilingEnabled(false) {}
    
    void startCounter(const std::string& name) {
        counters.push_back(PerformanceCounter(name));
        counters.back().start();
    }
    
    void stopCounter(const std::string& name) {
        for (auto& counter : counters) {
            if (counter.name == name) {
                counter.stop();
                break;
            }
        }
    }
    
    void incrementStep() {
        currentStep++;
        sampleTimestamps.push_back(static_cast<double>(currentStep));
        
        // Sample performance counters every N steps
        if (currentStep % 100 == 0) {
            samplePerformanceCounters();
        }
    }
    
    void samplePerformanceCounters() {
        // Create performance sample event
        std::ostringstream metadata;
        for (const auto& counter : counters) {
            metadata << " " << counter.name << "=" << counter.avgDuration << "ms";
        }
        
        // In real implementation, this would log to debug event logger
        std::cout << "[" << currentStep << "] PERFORMANCE_SAMPLE - Average times:" << metadata.str() << std::endl;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Report ===" << std::endl;
        oss << "Step range: 0 - " << currentStep << std::endl;
        oss << "Samples collected: " << sampleTimestamps.size() << std::endl;
        oss << std::endl;
        
        oss << "Performance Counters:" << std::endl;
        for (const auto& counter : counters) {
            oss << "  " << counter.report() << std::endl;
        }
        
        return oss.str();
    }
};

// Spike Event Analyzer
class SpikeEventAnalyzer {
public:
    struct SpikeStatistics {
        SimulationStep step;
        size_t totalSpikes;
        size_t sensorySpikes;
        size_t internalSpikes;
        size_t motorSpikes;
        float avgFiringRate;
        float maxFiringRate;
        float minFiringRate;
        float coefficientOfVariation;
    };
    
    std::vector<SpikeStatistics> spikeStats;
    SimulationStep currentStep;
    size_t totalSpikesGenerated;
    
    SpikeEventAnalyzer() : currentStep(0), totalSpikesGenerated(0) {}
    
    void recordSpike(SimulationStep step, NeuronType neuronType, float weight) {
        totalSpikesGenerated++;
        
        // Create spike statistics for this step
        SpikeStatistics stats;
        stats.step = step;
        stats.totalSpikes = 1; // Each call is one spike
        stats.sensorySpikes = (neuronType == NeuronType::Sensory) ? 1 : 0;
        stats.internalSpikes = (neuronType == NeuronType::Internal) ? 1 : 0;
        stats.motorSpikes = (neuronType == NeuronType::Motor) ? 1 : 0;
        stats.avgFiringRate = weight; // Simplified - would need to track over time
        stats.maxFiringRate = weight;
        stats.minFiringRate = weight;
        stats.coefficientOfVariation = 0.0f;
        
        spikeStats.push_back(stats);
    }
    
    void incrementStep() {
        currentStep++;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Spike Event Analysis ===" << std::endl;
        oss << "Total spikes generated: " << totalSpikesGenerated << std::endl;
        oss << "Analysis steps: 0 - " << currentStep << std::endl;
        oss << std::endl;
        
        if (spikeStats.empty()) {
            oss << "No spike data available" << std::endl;
            return oss.str();
        }
        
        oss << "Spike Statistics Summary:" << std::endl;
        
        // Aggregate statistics
        size_t totalSensorySpikes = 0, totalInternalSpikes = 0, totalMotorSpikes = 0;
        float totalFiringRate = 0.0f;
        
        for (const auto& stats : spikeStats) {
            totalSensorySpikes += stats.sensorySpikes;
            totalInternalSpikes += stats.internalSpikes;
            totalMotorSpikes += stats.motorSpikes;
            totalFiringRate += stats.avgFiringRate;
        }
        
        oss << "  Sensory spikes: " << totalSensorySpikes << std::endl;
        oss << "  Internal spikes: " << totalInternalSpikes << std::endl;
        oss << "  Motor spikes: " << totalMotorSpikes << std::endl;
        oss << "  Average firing rate: " << (totalFiringRate / spikeStats.size()) << std::endl;
        
        // Show last 10 spike events
        oss << "\nLast 10 spike events:" << std::endl;
        size_t startIdx = std::max(0UL, spikeStats.size() - 10);
        for (size_t i = startIdx; i < spikeStats.size(); ++i) {
            const auto& stats = spikeStats[i];
            oss << "  Step " << stats.step 
                << " (type=" << neuronTypeToString(static_cast<NeuronType>(stats.internalSpikes > 0 ? 1 : 
                                                                                                  stats.motorSpikes > 0 ? 2 : 0))
                << ", spikes=" << stats.totalSpikes << ")" << std::endl;
        }
        
        return oss.str();
    }
    
private:
    std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Internal: return "Internal";
            case NeuronType::Motor: return "Motor";
            default: return "Unknown";
        }
    }
};

// Memory System Debugger
class MemorySystemDebugger {
public:
    struct MemoryTrace {
        SimulationStep step;
        NeuronId neuronId;
        float activation;
        std::string memoryType;
    };
    
    struct MemoryEpisode {
        SimulationStep timestamp;
        std::vector<MemoryTrace> traces;
        float reward;
        std::string action;
    };
    
    std::vector<MemoryTrace> recentTraces;
    std::vector<MemoryEpisode> episodes;
    SimulationStep currentStep;
    
    MemorySystemDebugger() : currentStep(0) {}
    
    void recordWorkingMemoryTrace(SimulationStep step, NeuronId neuron, float activation) {
        MemoryTrace trace{step, neuron, activation, "Working"};
        recentTraces.push_back(trace);
        
        // Keep only recent traces
        if (recentTraces.size() > 1000) {
            recentTraces.erase(recentTraces.begin());
        }
    }
    
    void recordEpisodicEpisode(SimulationStep step, const std::vector<MemoryTrace>& traces,
                               float reward, const std::string& action) {
        MemoryEpisode episode{step, traces, reward, action};
        episodes.push_back(episode);
        
        // Keep only recent episodes
        if (episodes.size() > 100) {
            episodes.erase(episodes.begin());
        }
    }
    
    void incrementStep() {
        currentStep++;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Memory System Debug ===" << std::endl;
        oss << "Current step: " << currentStep << std::endl;
        oss << "Active working memory traces: " << recentTraces.size() << std::endl;
        oss << "Stored episodes: " << episodes.size() << std::endl;
        oss << std::endl;
        
        if (!recentTraces.empty()) {
            oss << "Recent working memory traces:" << std::endl;
            size_t showCount = std::min(10UL, recentTraces.size());
            for (size_t i = recentTraces.size() - showCount; i < recentTraces.size(); ++i) {
                const auto& trace = recentTraces[i];
                oss << "  Step " << trace.step << ", Neuron " << trace.neuronId 
                    << ", Activation: " << trace.activation << std::endl;
            }
        }
        
        if (!episodes.empty()) {
            oss << "\nRecent episodic memory episodes:" << std::endl;
            size_t showCount = std::min(5UL, episodes.size());
            for (size_t i = episodes.size() - showCount; i < episodes.size(); ++i) {
                const auto& episode = episodes[i];
                oss << "  Step " << episode.timestamp 
                    << ", Reward: " << episode.reward 
                    << ", Action: " << episode.action
                    << ", Traces: " << episode.traces.size() << std::endl;
            }
        }
        
        return oss.str();
    }
};

// Neuromodulation Debugger
class NeuromodulationDebugger {
public:
    struct NeuromodulationState {
        SimulationStep step;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
    };
    
    std::vector<NeuromodulationState> neuromodulationHistory;
    SimulationStep currentStep;
    
    NeuromodulationDebugger() : currentStep(0) {}
    
    void recordNeuromodulationState(SimulationStep step, 
                                     float dopamine, float curiosity, 
                                     float novelty, float predictionError) {
        NeuromodulationState state{step, dopamine, curiosity, novelty, predictionError};
        neuromodulationHistory.push_back(state);
        
        // Keep only recent history
        if (neuromodulationHistory.size() > 1000) {
            neuromodulationHistory.erase(neuromodulationHistory.begin());
        }
    }
    
    void incrementStep() {
        currentStep++;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Neuromodulation Debug ===" << std::endl;
        oss << "Current step: " << currentStep << std::endl;
        oss << "Neuromodulation states recorded: " << neuromodulationHistory.size() << std::endl;
        oss << std::endl;
        
        if (neuromodulationHistory.empty()) {
            oss << "No neuromodulation data available" << std::endl;
            return oss.str();
        }
        
        oss << "Neuromodulation levels (last 10 samples):" << std::endl;
        size_t showCount = std::min(10UL, neuromodulationHistory.size());
        for (size_t i = neuromodulationHistory.size() - showCount; i < neuromodulationHistory.size(); ++i) {
            const auto& state = neuromodulationHistory[i];
            oss << "  Step " << state.step 
                << ": Dopamine=" << state.dopamineLevel
                << ", Curiosity=" << state.curiosityLevel
                << ", Novelty=" << state.noveltyLevel
                << ", PredictionError=" << state.predictionError << std::endl;
        }
        
        // Calculate averages
        float totalDopamine = 0.0f, totalCuriosity = 0.0f, totalNovelty = 0.0f, totalPredictionError = 0.0f;
        size_t count = neuromodulationHistory.size();
        
        for (const auto& state : neuromodulationHistory) {
            totalDopamine += state.dopamineLevel;
            totalCuriosity += state.curiosityLevel;
            totalNovelty += state.noveltyLevel;
            totalPredictionError += state.predictionError;
        }
        
        oss << std::endl << "Neuromodulation averages:" << std::endl;
        oss << "  Average dopamine: " << (totalDopamine / count) << std::endl;
        oss << "  Average curiosity: " << (totalCuriosity / count) << std::endl;
        oss << "  Average novelty: " << (totalNovelty / count) << std::endl;
        oss << "  Average prediction error: " << (totalPredictionError / count) << std::endl;
        
        return oss.str();
    }
};

// Comprehensive Debug Tool
class NLMDebugTool {
public:
    DebugEventLogger eventLogger;
    PerformanceProfiler profiler;
    SpikeEventAnalyzer spikeAnalyzer;
    MemorySystemDebugger memoryDebugger;
    NeuromodulationDebugger neuromodDebug;
    
    NLMDebugTool() {
        // Initialize components
    }
    
    void startDebugging(const std::string& sessionFile = "") {
        if (!sessionFile.empty()) {
            eventLogger.setSessionFile(sessionFile);
        }
        
        std::cout << "=== NLM Advanced Debug Tool ===" << std::endl;
        std::cout << "Debug session started" << std::endl;
        if (!sessionFile.empty()) {
            std::cout << "Session file: " << sessionFile << std::endl;
        }
        std::cout << std::endl;
        
        // Enable performance profiling
        profiler.profilingEnabled = true;
    }
    
    void stopDebugging() {
        std::cout << std::endl;
        std::cout << "=== Debug Session Summary ===" << std::endl;
        
        // Generate comprehensive reports
        std::cout << eventLogger.getSessionStats() << std::endl;
        std::cout << profiler.generateReport() << std::endl;
        std::cout << spikeAnalyzer.generateReport() << std::endl;
        std::cout << memoryDebugger.generateReport() << std::endl;
        std::cout << neuromodDebug.generateReport() << std::endl;
        
        // Save session data
        eventLogger.saveToFile();
        
        std::cout << std::endl << "Debug session completed" << std::endl;
    }
    
    void incrementSimulationStep() {
        eventLogger.incrementStep();
        profiler.incrementStep();
        spikeAnalyzer.incrementStep();
        memoryDebugger.incrementStep();
        neuromodDebug.incrementStep();
    }
    
    void recordSpike(SimulationStep step, NeuronType neuronType, float weight) {
        spikeAnalyzer.recordSpike(step, neuronType, weight);
        eventLogger.logEvent(DebugEventType::SpikeGenerated, 
                           "Spike generated by " + neuronTypeToString(neuronType) + " neuron");
    }
    
    void recordMemoryOperation(SimulationStep step, const std::string& operation, 
                               const std::map<std::string, std::string>& metadata = {}) {
        DebugEventType eventType;
        if (operation == "store") eventType = DebugEventType::MemoryStored;
        else if (operation == "retrieve") eventType = DebugEventType::MemoryRetrieved;
        else return;
        
        eventLogger.logEvent(eventType, "Memory " + operation + " operation", metadata);
        
        // Update memory debuggers
        if (operation == "store") {
            // In real implementation, would extract actual neuron ID and activation
            MemorySystemDebugger::MemoryTrace trace{step, 0, 1.0f, "Working"};
            memoryDebugger.recentTraces.push_back(trace);
        }
    }
    
    void recordPlasticityUpdate(SimulationStep step, const std::string& details) {
        eventLogger.logEvent(DebugEventType::PlasticityUpdated, details);
    }
    
    void recordNeuromodulationChange(SimulationStep step, const std::string& details) {
        eventLogger.logEvent(DebugEventType::NeuromodulationChanged, details);
        
        // In real implementation, would extract actual neuromodulation levels
        NeuromodulationDebugger::NeuromodulationState state{step, 0.5f, 0.3f, 0.2f, 0.1f};
        neuromodDebug.neuromodulationHistory.push_back(state);
    }
    
    void recordLearningEvent(SimulationStep step, const std::string& details) {
        eventLogger.logEvent(DebugEventType::LearningOccurred, details);
    }
    
    void recordCheckpointEvent(SimulationStep step, const std::string& operation, 
                               const std::string& file) {
        DebugEventType eventType = (operation == "save") ? DebugEventType::CheckpointSaved : DebugEventType::CheckpointLoaded;
        eventLogger.logEvent(eventType, "Checkpoint " + operation + " " + file);
    }
    
    void recordError(SimulationStep step, const std::string& errorMessage, 
                     const std::map<std::string, std::string>& context = {}) {
        eventLogger.logEvent(DebugEventType::ErrorOccurred, errorMessage, context);
    }
    
private:
    std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Internal: return "Internal";
            case NeuronType::Motor: return "Motor";
            default: return "Unknown";
        }
    }
};

// Debug Command for CLI
class DebugCommand : public CLICommand {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { 
        return "Launch advanced debugging and profiling tools"; 
    }
    std::string getUsage() const override { 
        return "nlm debug <start|stop|snapshot> [--session-file file] [--output-format json|txt]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Subcommand required (start/stop/snapshot)" << std::endl;
            return false;
        }
        
        std::string subcommand = args[0];
        std::string sessionFile;
        std::string outputFormat = "txt";
        
        // Parse arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--session-file" && i + 1 < args.size()) {
                sessionFile = args[++i];
            } else if (args[i] == "--output-format" && i + 1 < args.size()) {
                outputFormat = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        if (subcommand == "start") {
            return startDebugging(sessionFile);
        } else if (subcommand == "stop") {
            return stopDebugging();
        } else if (subcommand == "snapshot") {
            return generateSnapshot(outputFormat);
        } else {
            std::cerr << "Error: Unknown subcommand '" << subcommand << "'" << std::endl;
            return false;
        }
    }
    
private:
    bool startDebugging(const std::string& sessionFile) {
        // In real implementation, this would start the debug tool
        std::cout << "Starting NLM debug tool..." << std::endl;
        if (!sessionFile.empty()) {
            std::cout << "Session file: " << sessionFile << std::endl;
        }
        
        // Create debug tool instance
        NLMDebugTool debugTool;
        debugTool.startDebugging(sessionFile);
        
        std::cout << "Debug tool ready. Use appropriate hooks in your NLM code to record events." << std::endl;
        std::cout << "Available debug events: spikes, memory operations, plasticity updates, neuromodulation, learning, checkpoints, errors" << std::endl;
        
        return true;
    }
    
    bool stopDebugging() {
        // In real implementation, this would stop the debug tool
        std::cout << "Stopping NLM debug tool..." << std::endl;
        // Debug tool would generate final reports
        return true;
    }
    
    bool generateSnapshot(const std::string& outputFormat) {
        std::cout << "=== NLM Debug Snapshot ===" << std::endl;
        std::cout << "Output format: " << outputFormat << std::endl;
        
        if (outputFormat == "json") {
            generateJSONSnapshot();
        } else {
            generateTextSnapshot();
        }
        
        return true;
    }
    
    void generateJSONSnapshot() {
        std::cout << "JSON snapshot data would be generated here" << std::endl;
        std::cout << "{\"debug_events\": [...], \"performance_counters\": [...], \"spike_analysis\": {...}}" << std::endl;
    }
    
    void generateTextSnapshot() {
        std::cout << "=== Current Debug State ===" << std::endl;
        std::cout << "Step: 0 (example)" << std::endl;
        std::cout << "Events recorded: 0 (example)" << std::endl;
        std::cout << "Performance counters: 0 (example)" << std::endl;
        std::cout << "Spike statistics: 0 (example)" << std::endl;
        std::cout << "Memory traces: 0 (example)" << std::endl;
        std::cout << "Neuromodulation data: 0 (example)" << std::endl;
    }
};

} // namespace nlm

// Entry point for debug tool
int main(int argc, char** argv) {
    try {
        // Initialize NLM logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create and run debug command
        nlm::DebugCommand debugCmd;
        
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        bool success = debugCmd.execute(args);
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}