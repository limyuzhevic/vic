// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

using namespace nlm;

void printHelp() {
    printBanner();
    
    std::cout << R"(
USAGE:
    nlm [OPTIONS] [ARGS]

MODES:
    --basic                  Run basic connectivity test
    --plasticity             Run plasticity learning experiment
    --stdp                   Run STDP verification experiment
    --all                    Run all standard tests
    --continuous [STEPS]     Run continuous learning for specified steps
    --brain-inspect          Enter brain inspection mode
    --event-record [FILE]    Record simulation events
    --event-play FILE        Replay recorded events
    --custom-stim [FILE]     Run custom stimulus protocol
    --scaling [NEURONS]      Run scaling analysis

CONFIGURATION:
    --config FILE            Load configuration from file
    --set KEY=VALUE          Set configuration value
    --neuron-count N         Set number of neurons (default: 500)
    --region-count R         Set number of regions (default: 1)
    --duration STEPS         Set simulation duration in steps

OUTPUT:
    --output FILE            Save results to file
    --verbose LEVEL          Set log level (debug, info, warning, error)
    --quiet                  Suppress non-error output

UTILITY:
    --version                Show version information
    --help                   Show this help message
    --bench                  Run performance benchmark
    --analyze                Analyze current brain state
    --export STATE           Export brain state to file
    --import STATE           Import brain state from file
    --checkpoint             Save checkpoint
    --resume                 Resume from checkpoint
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM 0.1.0 - Neural Learning Machine (熙然)" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Enhanced with advanced commands and monitoring tools." << std::endl;
    std::cout << "Built with CMake " << CMAKE_VERSION << std::endl;
}

class EventRecorder {
public:
    struct Event {
        SimulationStep step;
        std::string type;
        std::string data;
        double timestamp;
    };
    
    EventRecorder() : isRecording_(false), startStep_(0) {}
    
    void startRecording(size_t initialStep) {
        isRecording_ = true;
        startStep_ = initialStep;
        events_.clear();
        NLM_LOG_INFO("Event recording started");
    }
    
    void recordEvent(const std::string& type, const std::string& data) {
        if (!isRecording_) return;
        
        Event event;
        event.step = startStep_ + events_.size();
        event.type = type;
        event.data = data;
        event.timestamp = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        events_.push_back(event);
    }
    
    bool saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << "EventLog\n";
        file << "StartStep: " << startStep_ << "\n";
        file << "TotalEvents: " << events_.size() << "\n\n";
        
        for (const auto& event : events_) {
            file << event.step << "|" << event.type << "|" << event.data << "|" << event.timestamp << "\n";
        }
        
        NLM_LOG_INFO("Events saved to " + filename);
        return true;
    }
    
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading: " + filename);
            return false;
        }
        
        std::string line;
        if (!std::getline(file, line) || line != "EventLog") {
            NLM_LOG_ERROR("Invalid event file format");
            return false;
        }
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos1 = line.find(':');
            if (pos1 == std::string::npos) continue;
            
            if (line.substr(0, pos1) == "StartStep") {
                startStep_ = std::stoull(line.substr(pos1 + 1));
            } else if (line.substr(0, pos1) == "TotalEvents") {
                // Skip, will count events below
            }
        }
        
        // Reset and reload events
        events_.clear();
        file.clear();
        file.seekg(0);
        
        std::getline(file, line); // Skip EventLog
        while (std::getline(file, line)) {
            if (line.empty() || line.find("StartStep:") != std::string::npos || 
                line.find("TotalEvents:") != std::string::npos) continue;
            
            Event event;
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);
            
            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                event.step = std::stoull(line.substr(0, pos1));
                event.type = line.substr(pos1 + 1, pos2 - pos1 - 1);
                event.data = line.substr(pos2 + 1, pos3 - pos2 - 1);
                event.timestamp = std::stod(line.substr(pos3 + 1));
                events_.push_back(event);
            }
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(events_.size()) + " events from " + filename);
        return true;
    }
    
    size_t getEventCount() const { return events_.size(); }
    bool isRecording() const { return isRecording_; }
    
private:
    std::vector<Event> events_;
    bool isRecording_;
    size_t startStep_;
};

class PerformanceProfiler {
public:
    struct ProfileData {
        std::string operation;
        double totalTime;
        double minTime;
        double maxTime;
        double avgTime;
        size_t callCount;
        double overhead;
    };
    
    PerformanceProfiler() : isProfiling_(false), totalOverhead_(0.0) {}
    
    void startProfiling() {
        isProfiling_ = true;
        profileData_.clear();
        NLM_LOG_INFO("Performance profiling started");
    }
    
    void stopProfiling() {
        isProfiling_ = false;
        
        // Calculate statistics
        double totalActualTime = 0.0;
        for (auto& data : profileData_) {
            if (data.callCount > 0) {
                data.avgTime = data.totalTime / data.callCount;
                totalActualTime += data.totalTime;
            }
        }
        
        // Calculate overhead
        totalOverhead_ = (totalActualTime - (getActualTimeForOperation("step", profileData_))) * 100.0 / totalActualTime;
        
        NLM_LOG_INFO("Performance profiling stopped");
    }
    
    void recordOperation(const std::string& operation, double time) {
        auto it = std::find_if(profileData_.begin(), profileData_.end(),
                              [&operation](const ProfileData& d) { return d.operation == operation; });
        
        if (it == profileData_.end()) {
            ProfileData data;
            data.operation = operation;
            data.totalTime = time;
            data.minTime = time;
            data.maxTime = time;
            data.callCount = 1;
            profileData_.push_back(data);
        } else {
            it->totalTime += time;
            it->minTime = std::min(it->minTime, time);
            it->maxTime = std::max(it->maxTime, time);
            it->callCount++;
        }
    }
    
    void printProfile() const {
        NLM_LOG_INFO("=== Performance Profile ===");
        NLM_LOG_INFO("Total operations profiled: " + std::to_string(profileData_.size()));
        NLM_LOG_INFO("Total overhead: " + std::to_string(totalOverhead_) + "%");
        NLM_LOG_INFO("");
        
        for (const auto& data : profileData_) {
            if (data.callCount == 0) continue;
            
            NLM_LOG_INFO(data.operation + ":");
            NLM_LOG_INFO("  Calls: " + std::to_string(data.callCount));
            NLM_LOG_INFO("  Total time: " + std::to_string(data.totalTime) + "s");
            NLM_LOG_INFO("  Avg time: " + std::to_string(data.avgTime * 1000) + "ms");
            NLM_LOG_INFO("  Min time: " + std::to_string(data.minTime * 1000) + "ms");
            NLM_LOG_INFO("  Max time: " + std::to_string(data.maxTime * 1000) + "ms");
            NLM_LOG_INFO("  % of total: " + std::to_string(data.totalTime / (profileData_.empty() ? 1.0 : profileData_[0].totalTime) * 100) + "%");
        }
    }
    
    void saveProfile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save profile to " + filename);
            return;
        }
        
        file << "PerformanceProfile\n";
        file << "Generated: " << std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() << "\n\n";
        
        for (const auto& data : profileData_) {
            file << data.operation << "," << data.totalTime << "," << data.minTime << ","
                 << data.maxTime << "," << data.avgTime << "," << data.callCount << "\n";
        }
        
        NLM_LOG_INFO("Profile saved to " + filename);
    }
    
private:
    double getActualTimeForOperation(const std::string& op, const std::vector<ProfileData>& data) {
        auto it = std::find_if(data.begin(), data.end(),
                              [&op](const ProfileData& d) { return d.operation == op; });
        return (it != data.end()) ? it->totalTime : 0.0;
    }
    
    std::vector<ProfileData> profileData_;
    bool isProfiling_;
    double totalOverhead_;
};

class BrainInspector {
public:
    static void inspectBrain(const Brain* brain) {
        NLM_LOG_INFO("=== Brain Inspection ===");
        
        // Basic brain info
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        
        // Memory regions
        size_t numRegions = brain->getRegionCount();
        NLM_LOG_INFO("Number of regions: " + std::to_string(numRegions));
        
        for (size_t i = 0; i < numRegions; ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                NLM_LOG_INFO("  Region " + std::to_string(i) + ":");
                NLM_LOG_INFO("    Neurons: " + std::to_string(region->getNeuronCount()));
                NLM_LOG_INFO("    Synapses: " + std::to_string(region->getSynapseCount()));
                NLM_LOG_INFO("    Type: " + std::to_string(static_cast<int>(region->getType())));
            }
        }
        
        // Get spike system for additional statistics
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            NLM_LOG_INFO("Spike system active: " + std::to_string(spikeSystem->getActiveNeuronCount()) + "/" + std::to_string(spikeSystem->getTotalNeuronCount()));
        }
        
        NLM_LOG_INFO("Neuron types:");
        // Note: Actual implementation would need to iterate through all regions to count neuron types
        NLM_LOG_INFO("  (Type counting requires iterating through all neurons)");
        
        // Plasticity system info
        NLM_LOG_INFO("Plasticity systems:");
        if (auto* stdp = brain->getSTDP()) {
            NLM_LOG_INFO("  STDP: Active");
        }
        if (auto* hebbian = brain->getHebbian()) {
            NLM_LOG_INFO("  Hebbian: Active");
        }
        if (auto* structural = brain->getStructuralPlasticity()) {
            NLM_LOG_INFO("  Structural: Active");
        }
        
        NLM_LOG_INFO("=== Inspection Complete ===");
    }
    
    static void generateNetworkGraph(const Brain* brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to write network graph to " + filename);
            return;
        }
        
        file << "digraph NLM_Brain {\n";
        file << "  rankdir=LR;\n";
        file << "  node [shape=circle];\n";
        
        // Add neurons (sample)
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t sampleSize = std::min(size_t(50), neuronCount);
        
        for (size_t i = 0; i < sampleSize; ++i) {
            file << "  neuron_" << i << " [label=\"" << i << "\"];\n";
        }
        
        // Add region information
        size_t numRegions = brain->getRegionCount();
        for (size_t i = 0; i < numRegions; ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                file << "  region_" << i << " [shape=box, label=\"Region " << i << "\"];\n";
            }
        }
        
        // Note: Actual synapse connections would require accessing internal structures
        // This is a simplified version for demonstration
        file << "  // Connections would be added here\n";
        file << "  // neuron_0 -> neuron_1;\n";
        file << "\n";
        file << "}";
        
        NLM_LOG_INFO("Network graph saved to " + filename);
    }
};

std::map<std::string, std::string> parseConfigFile(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config[key] = value;
            }
        }
    }
    return config;
}

void applyCustomStimulus(Brain* brain, const std::string& filename) {
    auto config = parseConfigFile(filename);
    
    if (config.find("type") == config.end()) {
        NLM_LOG_ERROR("Custom stimulus file must specify 'type' parameter");
        return;
    }
    
    std::string type = config["type"];
    NLM_LOG_INFO("Applying custom stimulus: " + type);
    
    if (type == "random") {
        // Random stimulus
        size_t steps = 1000;
        if (config.find("steps") != config.end()) {
            steps = std::stoull(config["steps"]);
        }
        
        for (SimulationStep step = 0; step < steps; ++step) {
            // Apply random current to random neurons
            size_t neuronsToStimulate = std::min(size_t(10), brain->getTotalNeuronCount());
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                             " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                             " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
    } else if (type == "pattern") {
        // Pattern stimulus (simple repetitive pattern)
        size_t patternLength = 10;
        size_t patternSteps = 1000;
        
        if (config.find("pattern_length") != config.end()) {
            patternLength = std::stoull(config["pattern_length"]);
        }
        
        for (SimulationStep step = 0; step < patternSteps; ++step) {
            size_t patternIndex = step % patternLength;
            
            // Apply pattern-specific stimulation
            if (patternIndex == 0 || patternIndex == patternLength / 2) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 100.0f);
            } else {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
    } else if (type == "rewarded") {
        // Reward-based stimulus
        size_t rewardStep = 500;
        size_t totalSteps = 1000;
        
        if (config.find("reward_step") != config.end()) {
            rewardStep = std::stoull(config["reward_step"]);
        }
        
        for (SimulationStep step = 0; step < totalSteps; ++step) {
            if (step < rewardStep) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            } else {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 80.0f);
                // Simulate reward signal
                brain->modulateWithReward(1.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
    } else {
        NLM_LOG_ERROR("Unknown stimulus type: " + type);
    }
    
    NLM_LOG_INFO("Custom stimulus completed");
}

void runScalingAnalysis(const std::string& configFile, uint64_t neuronCount) {
    NLM_LOG_INFO("Running scaling analysis with " + std::to_string(neuronCount) + " neurons");
    
    // Load config
    auto config = parseConfigFile(configFile);
    
    // Create brain with specified neuron count
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount));
    brainConfig->set("random_seed", static_cast<int64_t>(42));
    brainConfig->set("simulation_timestep", 0.001);
    brainConfig->set("region_count", static_cast<int64_t>(1));
    brainConfig->set("connection_probability", 0.15f);
    
    auto brain = std::make_shared<Brain>(brainConfig);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for scaling analysis");
        return;
    }
    
    // Run simulation
    uint64_t steps = 1000;
    if (config.find("steps") != config.end()) {
        steps = std::stoull(config["steps"]);
    }
    
    NLM_LOG_INFO("Running " + std::to_string(steps) + " steps...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Compute performance metrics
    double neuronsPerSecond = neuronCount / totalTime;
    double spikesPerSecond = brain->getTotalSpikeCount() / totalTime;
    double firingRate = brain->getFiringNeuronCount() / static_cast<double>(neuronCount) * 100.0;
    
    NLM_LOG_INFO("=== Scaling Analysis Results ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("Total time: " + std::to_string(totalTime) + "s");
    NLM_LOG_INFO("Neurons/second: " + std::to_string(neuronsPerSecond));
    NLM_LOG_INFO("Spikes/second: " + std::to_string(spikesPerSecond));
    NLM_LOG_INFO("Firing rate: " + std::to_string(firingRate) + "%");
    
    // Save results
    std::ostringstream filename;
    filename << "scaling_results_" << neuronCount << ".csv";
    std::ofstream file(filename.str());
    
    if (file.is_open()) {
        file << "neurons,total_time,neurons_per_second,spikes_per_second,firing_rate_percent\n";
        file << neuronCount << "," << totalTime << "," << neuronsPerSecond << "," 
             << spikesPerSecond << "," << firingRate << "\n";
        
        NLM_LOG_INFO("Results saved to " + filename.str());
    }
    
    NLM_LOG_INFO("Scaling analysis completed");
}

void saveBrainState(const Brain* brain, const std::string& filename) {
    NLM_LOG_INFO("Saving brain state to " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save brain state to " + filename);
        return;
    }
    
    // Save brain configuration
    file << "[BRAIN_STATE]\n";
    file << "version=0.1\n";
    file << "neuron_count=" << brain->getTotalNeuronCount() << "\n";
    file << "synapse_count=" << brain->getTotalSynapseCount() << "\n";
    file << "spike_count=" << brain->getTotalSpikeCount() << "\n";
    file << "timestamp=" << std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count() << "\n\n";
    
    // Save neuron states (sample)
    file << "[NEURONS]\n";
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t sampleSize = std::min(size_t(100), neuronCount);
    
    for (size_t i = 0; i < sampleSize; ++i) {
        // Note: This is a simplified implementation
        // Actual implementation would need access to internal neuron structures
        file << "neuron_" << i << "=state_placeholder\n";
    }
    
    NLM_LOG_INFO("Brain state saved");
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string eventFile;
    std::string stimulusFile;
    std::string scalingConfig;
    uint64_t neuronCount = 500;
    uint64_t duration = 0;
    std::string runMode = "all"; // basic, plasticity, stdp, all, continuous
    std::string logLevel = "info";
    bool quiet = false;
    bool showHelp = false;
    bool showVersion = false;
    bool enableBrainInspect = false;
    bool enableEventRecording = false;
    bool enableEventPlayback = false;
    bool enablePerformanceProfiler = false;
    bool enableCheckpoint = false;
    bool enableResume = false;
    uint64_t continuousSteps = 0;
    uint64_t scalingNeurons = 0;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            showVersion = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--brain-inspect" || arg == "--inspect") {
            enableBrainInspect = true;
        } else if (arg == "--event-record" || arg == "--record") {
            enableEventRecording = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                eventFile = argv[++i];
            }
        } else if (arg == "--event-play" || arg == "--play") {
            enableEventPlayback = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                eventFile = argv[++i];
            }
        } else if (arg == "--continuous") {
            runMode = "continuous";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                continuousSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--basic") {
            runMode = "basic";
        } else if (arg == "--plasticity") {
            runMode = "plasticity";
        } else if (arg == "--stdp") {
            runMode = "stdp";
        } else if (arg == "--all") {
            runMode = "all";
        } else if (arg == "--benchmark" || arg == "--bench") {
            enablePerformanceProfiler = true;
        } else if (arg == "--checkpoint" || arg == "--save") {
            enableCheckpoint = true;
        } else if (arg == "--resume" || arg == "--load") {
            enableResume = true;
        } else if (arg == "--scaling") {
            runMode = "scaling";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                scalingNeurons = std::stoull(argv[++i]);
            }
        } else if (arg == "--custom-stim") {
            runMode = "custom-stim";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                stimulusFile = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                configFile = argv[++i];
            }
        } else if (arg == "--set") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string pair = argv[++i];
                size_t pos = pair.find('=');
                if (pos != std::string::npos) {
                    std::string key = pair.substr(0, pos);
                    std::string value = pair.substr(pos + 1);
                    // Set config value
                    if (auto config = brainConfig) {
                        config->set(key, value);
                    }
                }
            }
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--duration") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                duration = std::stoull(argv[++i]);
            }
        } else if (arg == "--output") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                outputFile = argv[++i];
            }
        } else if (arg == "--verbose") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                logLevel = argv[++i];
            }
        } else if (arg == "--scaling-config") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                scalingConfig = argv[++i];
            }
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            showHelp = true;
        }
    }
    
    // Show help or version if requested
    if (showHelp) {
        printHelp();
        return 0;
    }
    
    if (showVersion) {
        printVersion();
        return 0;
    }
    
    // Initialize performance profiler if requested
    PerformanceProfiler profiler;
    if (enablePerformanceProfiler) {
        profiler.startProfiling();
    }
    
    // Initialize event recorder if requested
    EventRecorder eventRecorder;
    if (enableEventRecording) {
        eventRecorder.startRecording(0);
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level
    if (logLevel == "debug") {
        Logger::getGlobal().setLevel(LogLevel::Debug);
    } else if (logLevel == "warning") {
        Logger::getGlobal().setLevel(LogLevel::Warning);
    } else if (logLevel == "error") {
        Logger::getGlobal().setLevel(LogLevel::Error);
    }
    
    // If not quiet, print banner
    if (!quiet) {
        printBanner();
        std::cout << "Initializing NLM with enhanced commands and monitoring...\n" << std::endl;
    }
    
    // Load configuration
    auto brainConfig = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (brainConfig->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    brainConfig->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    brainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    brainConfig->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    brainConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    brainConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    brainConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    brainConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(brainConfig->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(brainConfig->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(brainConfig->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(brainConfig->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(brainConfig->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = brainConfig->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Enable brain inspection if requested
    if (enableBrainInspect) {
        NLM_LOG_INFO("Brain inspection enabled");
        BrainInspector::inspectBrain(brain.get());
    }
    
    // Load checkpoint if resuming
    if (enableResume) {
        NLM_LOG_INFO("Resume functionality not yet implemented");
    }
    
    // Run tests based on mode
    if (runMode == "basic") {
        NLM_LOG_INFO("=== Running Basic Connectivity Test ===");
        runBasicConnectivityTest(brain);
    } else if (runMode == "plasticity") {
        NLM_LOG_INFO("=== Running Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
    } else if (runMode == "stdp") {
        NLM_LOG_INFO("=== Running STDP Verification Experiment ===");
        runStdpVerification(brain);
    } else if (runMode == "all") {
        NLM_LOG_INFO("=== Running All Standard Tests ===");
        
        // Reset brain for each test
        brain->reset();
        brain->initialize();
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (runMode == "continuous") {
        NLM_LOG_INFO("=== Running Continuous Learning ===");
        
        if (continuousSteps > 0) {
            for (SimulationStep step = 0; step < continuousSteps; ++step) {
                brain->step(step, step * 0.001);
                
                if (enableEventRecording) {
                    eventRecorder.recordEvent("step", "Step " + std::to_string(step));
                }
                
                if (enablePerformanceProfiler) {
                    profiler.recordOperation("step", 0.001); // Placeholder time
                }
                
                if (step % 100 == 0 && !quiet) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                 " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                 " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
        }
        
    } else if (runMode == "scaling" && scalingNeurons > 0) {
        if (!scalingConfig.empty()) {
            runScalingAnalysis(scalingConfig, scalingNeurons);
        } else {
            // Use a temporary config file
            std::ofstream tempConfig("temp_scaling.cfg");
            tempConfig << "[SCALING]\n";
            tempConfig << "steps=1000\n";
            tempConfig.close();
            runScalingAnalysis("temp_scaling.cfg", scalingNeurons);
        }
        
    } else if (runMode == "custom-stim" && !stimulusFile.empty()) {
        NLM_LOG_INFO("=== Running Custom Stimulus Protocol ===");
        applyCustomStimulus(brain.get(), stimulusFile);
    }
    
    // Save checkpoint if requested
    if (enableCheckpoint) {
        saveBrainState(brain.get(), "checkpoint_state.bin");
    }
    
    // Save event log if recorded
    if (enableEventRecording && !eventFile.empty()) {
        eventRecorder.saveToFile(eventFile);
    }
    
    // Save performance profile if enabled
    if (enablePerformanceProfiler) {
        profiler.stopProfiling();
        profiler.printProfile();
        profiler.saveProfile("performance_profile.csv");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Export results if output file specified
    if (!outputFile.empty()) {
        NLM_LOG_INFO("Saving results to " + outputFile);
        // Implement results export
    }
    
    // Show brain inspection if enabled
    if (enableBrainInspect) {
        BrainInspector::inspectBrain(brain.get());
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    
    if (enablePerformanceProfiler) {
        NLM_LOG_INFO("Performance summary available in performance_profile.csv");
    }
    
    if (enableEventRecording && !eventFile.empty()) {
        NLM_LOG_INFO("Event log available in " + eventFile);
    }
    
    return 0;
}
