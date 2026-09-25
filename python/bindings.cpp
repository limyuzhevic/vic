#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for enhanced error handling
class BrainCreationError : public std::runtime_error {
public:
    explicit BrainCreationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ProfilingError : public std::runtime_error {
public:
    explicit ProfilingError(const std::string& msg) : std::runtime_error(msg) {}
};

class ExperimentError : public std::runtime_error {
public:
    explicit ExperimentError(const std::string& msg) : std::runtime_error(msg) {}
};

class SerializationError : public std::runtime_error {
public:
    explicit SerializationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Advanced configuration builder helper
class ConfigBuilder {
public:
    std::shared_ptr<Config> config;
    
    ConfigBuilder() : config(std::make_shared<Config>()) {}
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::vector<float>& value) {
        config->set(key, std::string(value.begin(), value.end()));
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value, ConfigSource source) {
        config->set(key, value, source);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config;
    }
    
    std::shared_ptr<Config> buildWithDefault(const std::string& preset) {
        config->set("preset", preset);
        return config;
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    std::chrono::steady_clock::time_point start_time;
    std::map<std::string, std::chrono::steady_clock::time_point> phase_times;
    std::vector<std::chrono::duration<double>> step_times;
    size_t step_count = 0;
    mutable size_t active_neurons = 0;
    mutable size_t active_synapses = 0;
    
    PerformanceMonitor() {
        start_time = std::chrono::steady_clock::now();
    }
    
    void startPhase(const std::string& phase_name) {
        phase_times[phase_name] = std::chrono::steady_clock::now();
    }
    
    double endPhase(const std::string& phase_name) {
        auto it = phase_times.find(phase_name);
        if (it == phase_times.end()) {
            throw ProfilingError("Phase not started: " + phase_name);
        }
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(end_time - it->second);
        phase_times.erase(it);
        return duration.count();
    }
    
    void recordStepTime(double time_seconds) {
        step_times.push_back(std::chrono::duration<double>(time_seconds));
        step_count++;
    }
    
    void updateNeuralCounts(size_t neurons, size_t synapses) const {
        active_neurons = neurons;
        active_synapses = synapses;
    }
    
    double getAverageStepTime() const {
        if (step_times.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& t : step_times) sum += t.count();
        return sum / step_times.size();
    }
    
    double getTotalSimulationTime() const {
        auto end_time = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(end_time - start_time).count();
    }
    
    size_t getStepCount() const { return step_count; }
    size_t getActiveNeuronCount() const { return active_neurons; }
    size_t getActiveSynapseCount() const { return active_synapses; }
    
    std::map<std::string, double> getPhaseTimes() const {
        std::map<std::string, double> result;
        for (const auto& pair : phase_times) {
            auto end_time = std::chrono::steady_clock::now();
            result[pair.first] = std::chrono::duration<double>(end_time - pair.second).count();
        }
        return result;
    }
    
    std::string getPerformanceReport() const {
        std::stringstream ss;
        ss << "Performance Report:" << std::endl;
        ss << "Total Simulation Time: " << getTotalSimulationTime() << " s" << std::endl;
        ss << "Number of Steps: " << getStepCount() << std::endl;
        ss << "Average Step Time: " << getAverageStepTime() << " s" << std::endl;
        ss << "Active Neurons: " << getActiveNeuronCount() << std::endl;
        ss << "Active Synapses: " << getActiveSynapseCount() << std::endl;
        
        auto phase_times = getPhaseTimes();
        if (!phase_times.empty()) {
            ss << "Phase Times:" << std::endl;
            for (const auto& pair : phase_times) {
                ss << "  " << pair.first << ": " << pair.second << " s" << std::endl;
            }
        }
        
        return ss.str();
    }
};

// Batch processing utilities
class BatchProcessor {
public:
    struct BatchResult {
        size_t steps_processed;
        size_t actions_produced;
        size_t sensory_inputs_processed;
        double total_time;
        std::vector<double> step_times;
        std::vector<size_t> spike_counts;
        
        BatchResult() : steps_processed(0), actions_produced(0), 
                       sensory_inputs_processed(0), total_time(0.0) {}
    };
    
    BatchProcessor() = default;
    
    BatchResult runSteps(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                       size_t num_steps, std::shared_ptr<PerformanceMonitor> monitor = nullptr) {
        BatchResult result;
        auto start_time = std::chrono::steady_clock::now();
        
        for (size_t i = 0; i < num_steps; ++i) {
            auto step_start = std::chrono::steady_clock::now();
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            brain->receiveSensoryInput(percept);
            
            // Step the brain
            brain->step(i);
            
            // Produce action
            auto action = brain->produceAction();
            
            // Apply action to world
            if (action) {
                result.actions_produced++;
                world->applyMotorCommand(action->getType(), i);
            }
            
            // Update world
            world->update(i);
            
            auto step_end = std::chrono::steady_clock::now();
            double step_time = std::chrono::duration<double>(step_end - step_start).count();
            result.step_times.push_back(step_time);
            
            if (monitor) {
                monitor->recordStepTime(step_time);
            }
            
            result.steps_processed++;
            result.sensory_inputs_processed++;
        }
        
        auto end_time = std::chrono::steady_clock::now();
        result.total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        return result;
    }
    
    BatchResult runEpisodes(std::shared_ptr<AgentBrain> agent_brain,
                           std::shared_ptr<SimpleWorld> world,
                           size_t num_episodes,
                           size_t steps_per_episode,
                           std::shared_ptr<PerformanceMonitor> monitor = nullptr) {
        BatchResult total_result;
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            world->reset();
            agent_brain->initialize(world);
            
            BatchResult episode_result = runSteps(agent_brain->getBrain(), world, steps_per_episode, monitor);
            
            total_result.steps_processed += episode_result.steps_processed;
            total_result.actions_produced += episode_result.actions_produced;
            total_result.sensory_inputs_processed += episode_result.sensory_inputs_processed;
            total_result.total_time += episode_result.total_time;
            total_result.step_times.insert(total_result.step_times.end(), 
                                          episode_result.step_times.begin(), 
                                          episode_result.step_times.end());
            total_result.spike_counts.insert(total_result.spike_counts.end(),
                                          episode_result.spike_counts.begin(),
                                          episode_result.spike_counts.end());
        }
        
        return total_result;
    }
    
    std::string getBatchReport(const BatchResult& result) const {
        std::stringstream ss;
        ss << "Batch Processing Report:" << std::endl;
        ss << "Steps Processed: " << result.steps_processed << std::endl;
        ss << "Actions Produced: " << result.actions_produced << std::endl;
        ss << "Sensory Inputs Processed: " << result.sensory_inputs_processed << std::endl;
        ss << "Total Time: " << result.total_time << " s" << std::endl;
        ss << "Average Step Time: " << (result.steps_processed > 0 ? result.total_time / result.steps_processed : 0.0) << " s" << std::endl;
        ss << "Steps per Second: " << (result.total_time > 0 ? result.steps_processed / result.total_time : 0.0) << std::endl;
        
        if (!result.step_times.empty()) {
            double sum = 0.0, min_time = result.step_times[0], max_time = result.step_times[0];
            for (double t : result.step_times) {
                sum += t;
                min_time = std::min(min_time, t);
                max_time = std::max(max_time, t);
            }
            ss << "Step Time Statistics:" << std::endl;
            ss << "  Minimum: " << min_time << " s" << std::endl;
            ss << "  Maximum: " << max_time << " s" << std::endl;
            ss << "  Standard Deviation: " << (result.steps_processed > 1 ? 
                std::sqrt(((sum * sum) / result.steps_processed) - (sum / result.steps_processed) * (sum / result.steps_processed)) : 0.0) << " s" << std::endl;
        }
        
        return ss.str();
    }
};

// Serialization utilities
class Serializer {
public:
    static bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            throw SerializationError("Failed to save brain state: " + std::string(e.what()));
        }
    }
    
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            throw SerializationError("Failed to load brain state: " + std::string(e.what()));
        }
    }
    
    static bool saveConfig(std::shared_ptr<Config> config, const std::string& filepath) {
        try {
            return config->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw SerializationError("Failed to save configuration: " + std::string(e.what()));
        }
    }
    
    static bool loadConfig(std::shared_ptr<Config> config, const std::string& filepath) {
        try {
            return config->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw SerializationError("Failed to load configuration: " + std::string(e.what()));
        }
    }
    
    static std::string getBrainSnapshot(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Brain Snapshot:" << std::endl;
        ss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "Excitatory/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "Configuration Summary: " << brain->getConfig()->summary() << std::endl;
        
        return ss.str();
    }
};

// Visualization helpers
class VisualizationHelper {
public:
    static std::string generateNetworkTopology(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Network Topology:" << std::endl;
        ss << "Regions: " << brain->getRegionCount() << std::endl;
        
        auto region_ids = brain->getRegionIds();
        for (const auto& region_id : region_ids) {
            auto region = brain->getRegion(region_id);
            if (region) {
                ss << "  Region " << region_id.index() << ": " << region->getName() << std::endl;
                ss << "    Neurons: " << region->getNeuronCount() << std::endl;
                ss << "    Synapses: " << region->getSynapseCount() << std::endl;
            }
        }
        
        return ss.str();
    }
    
    static std::string generateActivityHeatmap(std::shared_ptr<Brain> brain, 
                                              double threshold = 0.5) {
        std::stringstream ss;
        ss << "Activity Heatmap (threshold=" << threshold << "):" << std::endl;
        
        size_t total_active = brain->getActiveNeuronCount();
        size_t total_neurons = brain->getTotalNeuronCount();
        
        if (total_neurons > 0) {
            double activity_ratio = static_cast<double>(total_active) / total_neurons;
            ss << "Overall Activity: " << (activity_ratio * 100) << "%" << std::endl;
            
            // Generate simple ASCII heatmap
            int heatmap_size = 20;
            int active_count = static_cast<int>(activity_ratio * heatmap_size);
            for (int i = 0; i < heatmap_size; ++i) {
                if (i < active_count) {
                    ss << "#";
                } else {
                    ss << ".";
                }
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generateSensorySpectrogram(std::shared_ptr<SensoryPercept> percept) {
        std::stringstream ss;
        ss << "Sensory Spectrogram:" << std::endl;
        
        auto vision = percept->getVision();
        if (vision) {
            ss << "Vision: " << vision->getWidth() << "x" << vision->getHeight() << 
                  "x" << vision->getChannels() << std::endl;
        }
        
        auto audio = percept->getAudio();
        if (audio) {
            ss << "Audio: " << audio->getSampleRate() << " Hz, " << 
                  audio->getNumSamples() << " samples" << std::endl;
        }
        
        auto internal = percept->getInternal();
        if (internal) {
            ss << "Internal: " << internal->getSignals().size() << " signals" << std::endl;
        }
        
        auto proprioception = percept->getProprioception();
        if (proprioception) {
            ss << "Proprioception: " << proprioception->getSignals().size() << " signals" << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generateDevelopmentTimeline(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Development Timeline:" << std::endl;
        
        auto stages = {
            std::make_pair("Initial", DevelopmentalStage::Initial),
            std::make_pair("Critical Period", DevelopmentalStage::CriticalPeriod),
            std::make_pair("Maturation", DevelopmentalStage::Maturation),
            std::make_pair("Adult", DevelopmentalStage::Adult),
            std::make_pair("Aging", DevelopmentalStage::Aging)
        };
        
        for (const auto& pair : stages) {
            bool is_current = brain->getDevelopmentalStage() == pair.second;
            ss << "  " << (is_current ? "->" : "  ") << pair.first << std::endl;
        }
        
        return ss.str();
    }
};

// Experiment utilities
class ExperimentManager {
public:
    static std::shared_ptr<Experiment> createExperiment(const std::string& name, 
                                                       std::shared_ptr<Config> config) {
        try {
            auto experiment = std::make_shared<Experiment>(name, config);
            return experiment;
        } catch (const std::exception& e) {
            throw ExperimentError("Failed to create experiment: " + std::string(e.what()));
        }
    }
    
    static std::shared_ptr<ExperimentRunner> createExperimentRunner(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<PerformanceMonitor> monitor = nullptr) {
        try {
            auto runner = std::make_shared<ExperimentRunner>(experiment, monitor);
            return runner;
        } catch (const std::exception& e) {
            throw ExperimentError("Failed to create experiment runner: " + std::string(e.what()));
        }
    }
    
    static std::string getExperimentSummary(std::shared_ptr<Experiment> experiment) {
        std::stringstream ss;
        ss << "Experiment Summary:" << std::endl;
        ss << "Name: " << experiment->getName() << std::endl;
        ss << "Description: " << experiment->getDescription() << std::endl;
        ss << "Status: " << static_cast<int>(experiment->getStatus()) << std::endl;
        ss << "Start Time: " << experiment->getStartTime().time_since_epoch().count() << std::endl;
        ss << "End Time: " << experiment->getEndTime().time_since_epoch().count() << std::endl;
        ss << "Results: " << experiment->getResults().size() << " metrics" << std::endl;
        
        return ss.str();
    }
    
    static std::string getRunnerStatistics(std::shared_ptr<ExperimentRunner> runner) {
        std::stringstream ss;
        ss << "Experiment Runner Statistics:" << std::endl;
        
        auto stats = runner->getStatistics();
        ss << "Experiments Completed: " << stats.experiments_completed << std::endl;
        ss << "Experiments Failed: " << stats.experiments_failed << std::endl;
        ss << "Total Steps Processed: " << stats.total_steps_processed << std::endl;
        ss << "Average Steps per Second: " << stats.average_steps_per_second << std::endl;
        ss << "Total Simulation Time: " << stats.total_simulation_time << " s" << std::endl;
        
        return ss.str();
    }
    
    static void runExperiment(std::shared_ptr<ExperimentRunner> runner) {
        try {
            runner->run();
        } catch (const std::exception& e) {
            throw ExperimentError("Failed to run experiment: " + std::string(e.what()));
        }
    }
    
    static void runExperimentBatch(std::vector<std::shared_ptr<ExperimentRunner>> runners) {
        std::vector<std::future<void>> futures;
        
        for (auto& runner : runners) {
            futures.push_back(std::async(std::launch::async, [runner]() {
                try {
                    runner->run();
                } catch (const std::exception& e) {
                    // Log error but don't stop other experiments
                    std::cerr << "Experiment error: " << e.what() << std::endl;
                }
            }));
        }
        
        // Wait for all experiments to complete
        for (auto& future : futures) {
            future.wait();
        }
    }
};

// Neuroscience-specific helper functions
namespace NeuroscienceHelpers {
    
    static std::string analyzeNetworkTopology(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Neural Network Topology Analysis:" << std::endl;
        
        auto regions = brain->getRegions();
        ss << "Number of regions: " << regions.size() << std::endl;
        
        size_t total_neurons = 0;
        size_t total_synapses = 0;
        size_t excitatory_neurons = 0;
        size_t inhibitory_neurons = 0;
        
        for (const auto& region : regions) {
            total_neurons += region->getNeuronCount();
            total_synapses += region->getSynapseCount();
            
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                if (neuron->getType() == NeuronType::Excitatory) {
                    excitatory_neurons++;
                } else if (neuron->getType() == NeuronType::Inhibitory) {
                    inhibitory_neurons++;
                }
            }
        }
        
        ss << "Total neurons: " << total_neurons << std::endl;
        ss << "Total synapses: " << total_synapses << std::endl;
        ss << "Excitatory neurons: " << excitatory_neurons << " (" <<
              (total_neurons > 0 ? (excitatory_neurons * 100.0 / total_neurons) : 0.0) << "%)" << std::endl;
        ss << "Inhibitory neurons: " << inhibitory_neurons << " (" <<
              (total_neurons > 0 ? (inhibitory_neurons * 100.0 / total_neurons) : 0.0) << "%)" << std::endl;
        ss << "Excitation/Inhibition ratio: " <<
              (inhibitory_neurons > 0 ? static_cast<double>(excitatory_neurons) / inhibitory_neurons : 0.0) << std::endl;
        
        return ss.str();
    }
    
    static std::string analyzeDevelopmentPattern(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Development Pattern Analysis:" << std::endl;
        
        auto stage = brain->getDevelopmentalStage();
        ss << "Current developmental stage: " << static_cast<int>(stage) << std::endl;
        
        // Analyze brain metrics across development
        ss << "Brain metrics:" << std::endl;
        ss << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Total spike count: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  Excitation/inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        // Predict next stage based on current metrics
        DevelopmentalStage next_stage = stage;
        if (stage == DevelopmentalStage::Initial) {
            next_stage = DevelopmentalStage::CriticalPeriod;
        } else if (stage == DevelopmentalStage::CriticalPeriod) {
            next_stage = DevelopmentalStage::Maturation;
        } else if (stage == DevelopmentalStage::Maturation) {
            next_stage = DevelopmentalStage::Adult;
        } else if (stage == DevelopmentalStage::Adult) {
            next_stage = DevelopmentalStage::Aging;
        }
        
        ss << "Predicted next stage: " << static_cast<int>(next_stage) << std::endl;
        
        return ss.str();
    }
    
    static std::string analyzePlasticityState(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Plasticity State Analysis:" << std::endl;
        
        auto stdp = brain->getSTDP();
        if (stdp) {
            ss << "STDP plasticity:" << std::endl;
            ss << "  Learning rate: " << stdp->getLearningRate() << std::endl;
            ss << "  LTP threshold: " << stdp->getLTPThreshold() << std::endl;
            ss << "  LTD threshold: " << stdp->getLTDThreshold() << std::endl;
            ss << "  Maximum weight: " << stdp->getMaxWeight() << std::endl;
            ss << "  Minimum weight: " << stdp->getMinWeight() << std::endl;
            ss << "  Trace decay rate: " << stdp->getTraceDecayRate() << std::endl;
        }
        
        auto hebbian = brain->getHebbian();
        if (hebbian) {
            ss << "Hebbian plasticity:" << std::endl;
            ss << "  Learning rate: " << hebbian->getLearningRate() << std::endl;
            ss << "  Threshold: " << hebbian->getThreshold() << std::endl;
            ss << "  Maximum weight: " << hebbian->getMaxWeight() << std::endl;
            ss << "  Minimum weight: " << hebbian->getMinWeight() << std::endl;
        }
        
        auto structural = brain->getStructuralPlasticity();
        if (structural) {
            ss << "Structural plasticity:" << std::endl;
            ss << "  Growth rate: " << structural->getGrowthRate() << std::endl;
            ss << "  Pruning rate: " << structural->getPruningRate() << std::endl;
            ss << "  Maximum connections: " << structural->getMaxConnections() << std::endl;
            ss << "  Connection probability: " << structural->getConnectionProbability() << std::endl;
        }
        
        return ss.str();
    }
}

// Debug and profiling utilities
class Debugger {
public:
    static void dumpNeuronStates(std::shared_ptr<Brain> brain, 
                                const std::string& filepath = "neuron_states.txt") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw SerializationError("Could not open file for writing: " + filepath);
        }
        
        file << "Neuron States Dump" << std::endl;
        file << "====================" << std::endl;
        
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            file << "Region: " << region->getName() << " (ID: " << region->getId().index() << ")" << std::endl;
            file << "  Neurons: " << region->getNeuronCount() << std::endl;
            
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                file << "    Neuron " << neuron->getId().index() << 
                         ", Type: " << static_cast<int>(neuron->getType()) << 
                         ", State: " << static_cast<int>(neuron->getState()) << 
                         ", Potential: " << neuron->getPotential() << 
                         ", Refractory: " << neuron->getRefractoryTime() << std::endl;
            }
        }
        
        file.close();
    }
    
    static void dumpSynapseWeights(std::shared_ptr<Brain> brain,
                                  const std::string& filepath = "synapse_weights.txt") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw SerializationError("Could not open file for writing: " + filepath);
        }
        
        file << "Synapse Weights Dump" << std::endl;
        file << "=====================" << std::endl;
        
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            file << "Region: " << region->getName() << std::endl;
            
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                file << "  Neuron " << neuron->getId().index() << ": " << neuron->getOutgoingSynapseCount() << " outgoing synapses" << std::endl;
                
                auto synapses = neuron->getOutgoingSynapses();
                for (const auto& synapse : synapses) {
                    file << "    -> Neuron " << synapse->getTargetId().index() << 
                             ", Weight: " << synapse->getWeight() << 
                             ", Type: " << static_cast<int>(synapse->getType()) << std::endl;
                }
            }
        }
        
        file.close();
    }
    
    static void dumpConfiguration(std::shared_ptr<Config> config,
                                 const std::string& filepath = "config_dump.txt") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw SerializationError("Could not open file for writing: " + filepath);
        }
        
        file << "Configuration Dump" << std::endl;
        file << "===================" << std::endl;
        file << config->summary() << std::endl;
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            file << key << ": ";
            
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                file << value.value() << std::endl;
            } else {
                file << "(not found)" << std::endl;
            }
        }
        
        file.close();
    }
    
    static std::string profileBrain(std::shared_ptr<Brain> brain, 
                                   size_t steps = 100) {
        std::stringstream ss;
        ss << "Brain Profiling Report" << std::endl;
        ss << "======================" << std::endl;
        
        // Create monitor
        auto monitor = std::make_shared<PerformanceMonitor>();
        
        // Create world for testing
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 32, 32);
        
        // Create agent brain
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(world);
        
        // Create batch processor
        auto batch_processor = std::make_shared<BatchProcessor>();
        
        // Run steps
        auto result = batch_processor->runSteps(brain, world, steps, monitor);
        
        // Add neural counts
        monitor->updateNeuralCounts(brain->getActiveNeuronCount(), brain->getTotalSynapseCount());
        
        // Generate report
        ss << monitor->getPerformanceReport() << std::endl;
        ss << batch_processor->getBatchReport(result) << std::endl;
        
        return ss.str();
    }
    
    static std::string analyzeMemoryUsage(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Memory Usage Analysis" << std::endl;
        ss << "=====================" << std::endl;
        
        ss << "Neural memory systems:" << std::endl;
        
        auto working_memory = brain->getWorkingMemory();
        if (working_memory) {
            ss << "  Working Memory: " << working_memory->getCapacity() << " items, " <<
                  working_memory->getCurrentSize() << " current" << std::endl;
        }
        
        auto episodic_memory = brain->getEpisodicMemory();
        if (episodic_memory) {
            ss << "  Episodic Memory: " << episodic_memory->getMaxEvents() << " max events, " <<
                  episodic_memory->getCurrentEvents() << " current" << std::endl;
        }
        
        auto associative_memory = brain->getAssociativeMemory();
        if (associative_memory) {
            ss << "  Associative Memory: " << associative_memory->getMaxPatterns() << " max patterns, " <<
                  associative_memory->getCurrentPatterns() << " current" << std::endl;
        }
        
        ss << std::endl;
        ss << "Prediction system:" << std::endl;
        
        auto prediction_system = brain->getPredictionSystem();
        if (prediction_system) {
            ss << "  Prediction Error: " << prediction_system->getPredictionError() << std::endl;
            ss << "  Confidence: " << prediction_system->getConfidence() << std::endl;
        }
        
        ss << std::endl;
        ss << "Development system:" << std::endl;
        
        auto development_system = brain->getDevelopmentSystem();
        if (development_system) {
            ss << "  Development Stage: " << static_cast<int>(development_system->getDevelopmentalStage()) << std::endl;
            ss << "  Maturity Level: " << development_system->getMaturityLevel() << std::endl;
        }
        
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Advanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides advanced functionality for brain simulation,
        including factory methods, configuration builders, performance monitoring,
        neuroscience-specific helpers, debug and profiling tools, batch processing,
        enhanced error handling, serialization/deserialization, visualization
        helpers, and experiment utilities.
        
        Key Features:
        - Factory methods for different brain types and configurations
        - Advanced configuration builders
        - Performance monitoring utilities
        - Neuroscience-specific helper functions
        - Debug and profiling tools
        - Batch processing methods
        - Enhanced error handling and exception management
        - Serialization/deserialization utilities
        - Visualization helpers
        - Experiment utilities
        
        Classes:
        - ConfigBuilder: Advanced configuration builder
        - PerformanceMonitor: Performance monitoring utilities
        - BatchProcessor: Batch processing utilities
        - Serializer: Serialization utilities
        - VisualizationHelper: Visualization helpers
        - ExperimentManager: Experiment utilities
        - Debugger: Debug and profiling utilities
        
        Neuroscience Helpers:
        - analyzeNetworkTopology: Analyze neural network topology
        - analyzeDevelopmentPattern: Analyze developmental patterns
        - analyzePlasticityState: Analyze plasticity system state
        
        Errors:
        - BrainCreationError: Error during brain creation
        - ConfigurationError: Error during configuration
        - ProfilingError: Error during profiling
        - ExperimentError: Error during experiments
        - SerializationError: Error during serialization
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](int /* ec */, const std::exception* e) {
        if (strcmp(e->what(), "BrainCreationError") == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Brain creation error");
        } else if (strcmp(e->what(), "ConfigurationError") == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Configuration error");
        } else if (strcmp(e->what(), "ProfilingError") == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Profiling error");
        } else if (strcmp(e->what(), "ExperimentError") == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Experiment error");
        } else if (strcmp(e->what(), "SerializationError") == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Serialization error");
        }
    });

    // Advanced Configuration Builder
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Advanced configuration builder)")
        .def(py::init<>())
        .def("set", [](ConfigBuilder& self, const std::string& key, const std::string& value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](ConfigBuilder& self, const std::string& key, int value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](ConfigBuilder& self, const std::string& key, double value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](ConfigBuilder& self, const std::string& key, bool value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("setWithSource", [](ConfigBuilder& self, const std::string& key, const std::string& value, ConfigSource source) {
            return self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"))
        .def("build", &ConfigBuilder::build)
        .def("buildWithPreset", [](ConfigBuilder& self, const std::string& preset) {
            return self.buildWithDefault(preset);
        }, py::arg("preset"));

    // Performance Monitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring utilities)")
        .def(py::init<>())
        .def("startPhase", &PerformanceMonitor::startPhase, py::arg("phase_name"))
        .def("endPhase", &PerformanceMonitor::endPhase, py::arg("phase_name"))
        .def("recordStepTime", &PerformanceMonitor::recordStepTime, py::arg("time_seconds"))
        .def("updateNeuralCounts", &PerformanceMonitor::updateNeuralCounts, py::arg("neurons"), py::arg("synapses"))
        .def("getAverageStepTime", &PerformanceMonitor::getAverageStepTime)
        .def("getTotalSimulationTime", &PerformanceMonitor::getTotalSimulationTime)
        .def("getStepCount", &PerformanceMonitor::getStepCount)
        .def("getActiveNeuronCount", &PerformanceMonitor::getActiveNeuronCount)
        .def("getActiveSynapseCount", &PerformanceMonitor::getActiveSynapseCount)
        .def("getPhaseTimes", &PerformanceMonitor::getPhaseTimes)
        .def("getPerformanceReport", &PerformanceMonitor::getPerformanceReport);

    // Batch Processor
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch processing utilities)")
        .def(py::init<>())
        .def("runSteps", [](BatchProcessor& self, std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                          size_t num_steps, std::shared_ptr<PerformanceMonitor> monitor) {
            return self.runSteps(brain, world, num_steps, monitor);
        }, py::arg("brain"), py::arg("world"), py::arg("num_steps"), py::arg("monitor") = nullptr)
        .def("runEpisodes", [](BatchProcessor& self, std::shared_ptr<AgentBrain> agent_brain,
                             std::shared_ptr<SimpleWorld> world,
                             size_t num_episodes,
                             size_t steps_per_episode,
                             std::shared_ptr<PerformanceMonitor> monitor) {
            return self.runEpisodes(agent_brain, world, num_episodes, steps_per_episode, monitor);
        }, py::arg("agent_brain"), py::arg("world"), py::arg("num_episodes"), 
           py::arg("steps_per_episode"), py::arg("monitor") = nullptr)
        .def("getBatchReport", [](BatchProcessor& self, const BatchProcessor::BatchResult& result) {
            return self.getBatchReport(result);
        }, py::arg("result"));

    // Serializer
    py::class_<Serializer>(m, "Serializer", R"pbdoc(Serialization utilities)")
        .def_static("saveBrainState", &Serializer::saveBrainState, py::arg("brain"), py::arg("filepath"))
        .def_static("loadBrainState", &Serializer::loadBrainState, py::arg("brain"), py::arg("filepath"))
        .def_static("saveConfig", &Serializer::saveConfig, py::arg("config"), py::arg("filepath"))
        .def_static("loadConfig", &Serializer::loadConfig, py::arg("config"), py::arg("filepath"))
        .def_static("getBrainSnapshot", &Serializer::getBrainSnapshot, py::arg("brain"));

    // Visualization Helper
    py::class_<VisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Visualization helpers)")
        .def_static("generateNetworkTopology", &VisualizationHelper::generateNetworkTopology, py::arg("brain"))
        .def_static("generateActivityHeatmap", &VisualizationHelper::generateActivityHeatmap, 
                    py::arg("brain"), py::arg("threshold") = 0.5)
        .def_static("generateSensorySpectrogram", &VisualizationHelper::generateSensorySpectrogram, py::arg("percept"))
        .def_static("generateDevelopmentTimeline", &VisualizationHelper::generateDevelopmentTimeline, py::arg("brain"));

    // Experiment Manager
    py::class_<ExperimentManager>(m, "ExperimentManager", R"pbdoc(Experiment utilities)")
        .def_static("createExperiment", &ExperimentManager::createExperiment, 
                    py::arg("name"), py::arg("config"))
        .def_static("createExperimentRunner", &ExperimentManager::createExperimentRunner,
                    py::arg("experiment"), py::arg("monitor") = nullptr)
        .def_static("getExperimentSummary", &ExperimentManager::getExperimentSummary, py::arg("experiment"))
        .def_static("getRunnerStatistics", &ExperimentManager::getRunnerStatistics, py::arg("runner"))
        .def_static("runExperiment", &ExperimentManager::runExperiment, py::arg("runner"))
        .def_static("runExperimentBatch", &ExperimentManager::runExperimentBatch, py::arg("runners"));

    // Debugger
    py::class_<Debugger>(m, "Debugger", R"pbdoc(Debug and profiling utilities)")
        .def_static("dumpNeuronStates", &Debugger::dumpNeuronStates, 
                    py::arg("brain"), py::arg("filepath") = "neuron_states.txt")
        .def_static("dumpSynapseWeights", &Debugger::dumpSynapseWeights,
                    py::arg("brain"), py::arg("filepath") = "synapse_weights.txt")
        .def_static("dumpConfiguration", &Debugger::dumpConfiguration,
                    py::arg("config"), py::arg("filepath") = "config_dump.txt")
        .def_static("profileBrain", &Debugger::profileBrain,
                    py::arg("brain"), py::arg("steps") = 100)
        .def_static("analyzeMemoryUsage", &Debugger::analyzeMemoryUsage, py::arg("brain"));

    // Neuroscience Helpers
    m.def("analyzeNetworkTopology", &NeuroscienceHelpers::analyzeNetworkTopology, py::arg("brain"))
        .def("analyzeDevelopmentPattern", &NeuroscienceHelpers::analyzeDevelopmentPattern, py::arg("brain"))
        .def("analyzePlasticityState", &NeuroscienceHelpers::analyzePlasticityState, py::arg("brain"));

    // Advanced factory methods
    m.def("createConfig", []() { return std::make_shared<Config>(); })
        .def("createConfigWithPreset", [](const std::string& preset) {
            auto config = std::make_shared<Config>();
            config->set("preset", preset);
            return config;
        }, py::arg("preset"))
        .def("createConfigBuilder", []() { return ConfigBuilder(); })
        .def("createBrain", &createBrain, py::arg("config"))
        .def("createBrainWithType", [](const std::string& brain_type, std::shared_ptr<Config> config) {
            if (brain_type == "standard") {
                return createBrain(config);
            } else if (brain_type == "minimal") {
                auto minimal_config = std::make_shared<Config>();
                minimal_config->set("neuron_count", 100);
                minimal_config->set("synapse_density", 0.1);
                return createBrain(minimal_config);
            } else if (brain_type == "complex") {
                auto complex_config = std::make_shared<Config>();
                complex_config->set("neuron_count", 10000);
                complex_config->set("synapse_density", 0.5);
                complex_config->set("learning_rate", 0.01);
                complex_config->set("plasticity_enabled", true);
                return createBrain(complex_config);
            } else {
                throw BrainCreationError("Unknown brain type: " + brain_type);
            }
        }, py::arg("brain_type"), py::arg("config"))
        .def("createSimpleWorld", &createSimpleWorld)
        .def("createAgentBrain", &createAgentBrain, py::arg("brain"))
        .def("createBatchProcessor", []() { return std::make_shared<BatchProcessor>(); })
        .def("createPerformanceMonitor", []() { return std::make_shared<PerformanceMonitor>(); })
        .def("createExperimentRunner", [](std::shared_ptr<Experiment> experiment) {
            return std::make_shared<ExperimentRunner>(experiment);
        }, py::arg("experiment"))
        .def("createSerializer", []() { return std::make_shared<Serializer>(); })
        .def("createDebugger", []() { return std::make_shared<Debugger>(); });

    // Enhanced error classes
    py::register_exception<BrainCreationError>(m, "BrainCreationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ProfilingError>(m, "ProfilingError");
    py::register_exception<ExperimentError>(m, "ExperimentError");
    py::register_exception<SerializationError>(m, "SerializationError");

    // Legacy compatibility functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // NeuronType enum values
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum values
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum values
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // ConfigSource enum values
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // Existing classes (maintaining backward compatibility)
    // [All existing class definitions from the original bindings.cpp]
    
    // Re-register existing classes for backward compatibility
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def("py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Integration system getters for comprehensive testing
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getNeuralPlanner", &Brain::getNeuralPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced factory methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // NeuronId, SynapseId, RegionId, PopulationId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::register_exception<std::runtime_error>(m, "RuntimeError");
}

} // namespace nlm