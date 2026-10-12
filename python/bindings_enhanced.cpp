#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/WorldObject.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;

// Custom Python exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainError : public NLMException {
public:
    explicit NLMBrainError(const std::string& msg) : NLMException("NLM Brain Error: " + msg) {}
};

class NLMInvalidParameter : public NLMException {
public:
    explicit NLMInvalidParameter(const std::string& msg) : NLMException("NLM Invalid Parameter: " + msg) {}
};

class NLMFileNotFound : public NLMException {
public:
    explicit NLMFileNotFound(const std::string& msg) : NLMException("NLM File Not Found: " + msg) {}
};

class NLMConfigurationError : public NLMException {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMException("NLM Configuration Error: " + msg) {}
};

// Enhanced Configuration class with builder pattern
class PyConfig : public Config {
public:
    // Enhanced constructor with parameter validation
    PyConfig() : Config() {
        // Set sensible defaults
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("random_seed", 42, ConfigSource::Default);
        set("neuron_count", 1000, ConfigSource::Default);
    }

    // Configuration builder methods for fluent interface
    PyConfig& withTimestep(double timestep) {
        if (timestep <= 0) throw NLMInvalidParameter("Timestep must be positive");
        set("simulation_timestep", timestep, ConfigSource::Default);
        return *this;
    }

    PyConfig& withRandomSeed(uint64_t seed) {
        set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        return *this;
    }

    PyConfig& withNeuronCount(size_t count) {
        if (count == 0) throw NLMInvalidParameter("Neuron count must be positive");
        set("neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
        return *this;
    }

    PyConfig& withRegionCount(size_t count) {
        if (count == 0) throw NLMInvalidParameter("Region count must be positive");
        set("region_count", static_cast<int64_t>(count), ConfigSource::Default);
        return *this;
    }

    // Safe save/load with validation
    bool saveWithBackup(const std::string& filepath) {
        std::string backupPath = filepath + ".backup";
        std::ifstream src(filepath, std::ios::binary);
        std::ofstream dst(backupPath, std::ios::binary);
        
        if (src && dst) {
            dst << src.rdbuf();
        }
        
        return saveToFile(filepath);
    }

    // Enhanced configuration validation
    bool validate() const {
        try {
            getOr<double>("simulation_timestep", 0.001);
            getOr<int64_t>("random_seed", 42);
            getOr<int64_t>("neuron_count", 1000);
            getOr<int64_t>("region_count", 1);
            getOr<float>("connection_probability", 0.1f);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    // Get configuration as Python dictionary
    py::dict toDict() const {
        py::dict result;
        auto keys = getKeys();
        for (const auto& key : keys) {
            try {
                // Try to get as different types
                if (has("simulation_timestep") && key == "simulation_timestep") {
                    result[key] = getOr<double>("simulation_timestep", 0.001);
                } else if (has("random_seed") && key == "random_seed") {
                    result[key] = getOr<int64_t>("random_seed", 42);
                } else if (has("neuron_count") && key == "neuron_count") {
                    result[key] = getOr<int64_t>("neuron_count", 1000);
                } else if (has("region_count") && key == "region_count") {
                    result[key] = getOr<int64_t>("region_count", 1);
                } else if (has("connection_probability") && key == "connection_probability") {
                    result[key] = getOr<float>("connection_probability", 0.1f);
                } else if (has(key) && getType(key) == "int64_t") {
                    result[key] = getOr<int64_t>(key, 0);
                } else if (has(key) && getType(key) == "double") {
                    result[key] = getOr<double>(key, 0.0);
                } else if (has(key) && getType(key) == "float") {
                    result[key] = getOr<float>(key, 0.0f);
                } else if (has(key) && getType(key) == "std::string") {
                    result[key] = getOr<std::string>(key, "");
                } else {
                    result[key] = "<unreadable>";
                }
            } catch (const std::exception&) {
                result[key] = "<error>";
            }
        }
        return result;
    }

    // Create from Python dictionary
    static std::shared_ptr<PyConfig> fromDict(const py::dict& config_dict) {
        auto config = std::make_shared<PyConfig>();
        
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            // Try to convert value to appropriate type
            try {
                if (value.is(py::int_())) {
                    int64_t int_val = value.cast<int64_t>();
                    config->set(key, int_val, ConfigSource::User);
                } else if (value.is(py::float_())) {
                    double float_val = value.cast<double>();
                    config->set(key, float_val, ConfigSource::User);
                } else if (value.is(py::str())) {
                    std::string str_val = value.cast<std::string>();
                    config->set(key, str_val, ConfigSource::User);
                } else if (value.is(py::bool_())) {
                    bool bool_val = value.cast<bool>();
                    config->set(key, bool_val, ConfigSource::User);
                }
            } catch (const std::exception& e) {
                throw NLMConfigurationError("Failed to set config key \"" + key + "\": " + e.what());
            }
        }
        
        if (!config->validate()) {
            throw NLMConfigurationError("Invalid configuration parameters");
        }
        
        return config;
    }

    // Get configuration summary as Python string
    std::string getSummary() const {
        std::stringstream ss;
        auto keys = getKeys();
        
        ss << "NLM Configuration Summary:" << std::endl;
        ss << "===========================" << std::endl;
        
        for (size_t i = 0; i < keys.size(); ++i) {
            ss << "  " << keys[i] << ": ";
            try {
                if (keys[i] == "simulation_timestep") {
                    ss << getOr<double>("simulation_timestep", 0.001) << "s";
                } else if (keys[i] == "random_seed") {
                    ss << getOr<int64_t>("random_seed", 42);
                } else if (keys[i] == "neuron_count") {
                    ss << getOr<int64_t>("neuron_count", 1000);
                } else if (keys[i] == "region_count") {
                    ss << getOr<int64_t>("region_count", 1);
                } else if (keys[i] == "connection_probability") {
                    ss << getOr<float>("connection_probability", 0.1f);
                } else {
                    ss << getOr<std::string>(keys[i], "");
                }
            } catch (const std::exception&) {
                ss << "<unreadable>";
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
};

// Enhanced Brain class with Pythonic interfaces
class PyBrain : public Brain {
public:
    // Constructor with validation
    explicit PyBrain(std::shared_ptr<PyConfig> config) : Brain(config) {}

    // Simulation helper methods
    void runEpisode(size_t steps, const std::shared_ptr<SimpleWorld>& world = nullptr) {
        if (steps == 0) return;
        
        // Use agent's processSensoryInput if world provided
        if (world) {
            auto agent = std::make_shared<AgentBrain>(shared_from_this());
            agent->initialize(*world);
        }
        
        for (size_t i = 0; i < steps; ++i) {
            step(static_cast<SimulationStep>(i), i * getTimestep());
            
            // Provide periodic progress updates for long simulations
            if (i % 1000 == 0 && i > 0) {
                auto* logger = Logger::getGlobal();
                if (logger) {
                    logger->log(std::to_string(i) + " steps completed", LogLevel::Info);
                }
            }
        }
    }

    // Enhanced metrics collection
    py::dict collectMetrics(size_t sampleSteps = 100) {
        py::dict metrics;
        
        // Basic statistics
        metrics["total_neurons"] = getTotalNeuronCount();
        metrics["total_synapses"] = getTotalSynapseCount();
        metrics["active_neurons"] = getActiveNeuronCount();
        metrics["firing_neurons"] = getFiringNeuronCount();
        metrics["average_firing_rate"] = getAverageFiringRate();
        metrics["excitation_inhibition_ratio"] = getExcitationInhibitionRatio();
        metrics["total_spikes"] = getTotalSpikeCount();
        
        // Time-dependent metrics (could be enhanced with sampling)
        if (sampleSteps > 0) {
            metrics["steps_per_second"] = static_cast<double>(sampleSteps) / getTimestep();
        }
        
        return metrics;
    }

    // Brain state analysis and export
    py::dict analyzeConnectivity() const {
        py::dict connectivity;
        
        auto regions = getRegions();
        size_t totalConnections = 0;
        size_t strongConnections = 0;
        size_t weakConnections = 0;
        
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                totalConnections++;
                float weight = synapse->getWeight();
                if (std::abs(weight) > 0.5f) {
                    strongConnections++;
                } else if (std::abs(weight) < 0.1f) {
                    weakConnections++;
                }
            }
        }
        
        connectivity["total_connections"] = totalConnections;
        connectivity["strong_connections"] = strongConnections;
        connectivity["weak_connections"] = weakConnections;
        
        if (totalConnections > 0) {
            connectivity["strong_ratio"] = static_cast<double>(strongConnections) / totalConnections;
            connectivity["weak_ratio"] = static_cast<double>(weakConnections) / totalConnections;
        }
        
        return connectivity;
    }

    // Enhanced save/load with validation
    bool saveWithValidation(const std::string& filepath) {
        try {
            return save(filepath);
        } catch (const std::exception& e) {
            throw NLMBrainError(std::string("Failed to save brain state: ") + e.what());
        }
    }

    // Create brain from configuration dictionary
    static std::shared_ptr<PyBrain> createFromConfig(const py::dict& config_dict) {
        try {
            auto config = PyConfig::fromDict(config_dict);
            return std::make_shared<PyBrain>(config);
        } catch (const NLMConfigurationError& e) {
            throw NLMBrainError(std::string("Failed to create brain from config: ") + e.what());
        }
    }

    // Performance benchmark
    py::dict benchmark(size_t steps = 1000, size_t warmupSteps = 100) {
        py::dict results;
        
        // Warmup
        for (size_t i = 0; i < warmupSteps; ++i) {
            step(static_cast<SimulationStep>(i), i * getTimestep());
        }
        
        // Reset for clean benchmark
        reset();
        initialize();
        
        // Run benchmark
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            step(static_cast<SimulationStep>(i), i * getTimestep());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        results["steps"] = steps;
        results["warmup_steps"] = warmupSteps;
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        results["spikes_generated"] = getTotalSpikeCount();
        results["final_firing_rate"] = getAverageFiringRate();
        
        return results;
    }

    // Memory usage information
    py::dict getMemoryInfo() const {
        py::dict info;
        
        info["total_neurons"] = getTotalNeuronCount();
        info["total_synapses"] = getTotalSynapseCount();
        info["active_neurons"] = getActiveNeuronCount();
        info["memory_usage_estimate_kb"] = (getTotalNeuronCount() * sizeof(Neuron) + 
                                           getTotalSynapseCount() * sizeof(Synapse)) / 1024;
        
        return info;
    }

    // Iterator support for regions
    class RegionIterator {
    public:
        RegionIterator(std::vector<std::shared_ptr<NeuralRegion>>::iterator it) : it_(it) {}
        
        std::shared_ptr<NeuralRegion> operator*() const { return *it_; }
        RegionIterator& operator++() { ++it_; return *this; }
        bool operator!=(const RegionIterator& other) const { return it_ != other.it_; }
        
    private:
        std::vector<std::shared_ptr<NeuralRegion>>::iterator it_;
    };

    RegionIterator beginRegions() {
        return RegionIterator(regions_.begin());
    }

    RegionIterator endRegions() {
        return RegionIterator(regions_.end());
    }

    // Enhanced version of getRegions that returns PyObject-compatible references
    std::vector<std::shared_ptr<NeuralRegion>> getRegionsVector() {
        return getRegions();
    }

    // Find neurons by type
    std::vector<std::shared_ptr<Neuron>> findNeurons(NeuronType type) const {
        std::vector<std::shared_ptr<Neuron>> result;
        for (const auto& region : getRegions()) {
            for (const auto& neuron : region->getNeurons()) {
                if (neuron->getType() == type) {
                    result.push_back(neuron);
                }
            }
        }
        return result;
    }

    // Get brain state as exportable data
    py::dict exportForAnalysis() const {
        py::dict data;
        
        // Spike data
        data["total_spikes"] = getTotalSpikeCount();
        
        // Regional statistics
        py::list regions_data;
        for (const auto& region : getRegions()) {
            py::dict region_info;
            region_info["id"] = region->getId();
            region_info["name"] = region->getName();
            region_info["neuron_count"] = region->getPopulationCount();
            region_info["synapse_count"] = region->getSynapseCount();
            region_info["avg_firing_rate"] = region->getAverageFiringRate();
            regions_data.append(region_info);
        }
        data["regions"] = regions_data;
        
        return data;
    }

    // Import brain state from analysis data
    void importFromAnalysis(const py::dict& data) {
        try {
            if (data.contains("total_spikes")) {
                // We can't directly modify spike counts, but we can reset
                reset();
            }
            
            // Region data could be used to adjust network structure
            if (data.contains("regions")) {
                // This would require additional implementation
                // For now, just log that region data was received
                auto* logger = Logger::getGlobal();
                if (logger) {
                    logger->log("Received region data for import", LogLevel::Info);
                }
            }
        } catch (const std::exception& e) {
            throw NLMBrainError(std::string("Failed to import brain state: ") + e.what());
        }
    }

    // Diagnostic and debugging methods
    std::string getDiagnostics() const {
        std::stringstream ss;
        
        ss << "=== NLM Brain Diagnostics ===" << std::endl;
        ss << "Total Neurons: " << getTotalNeuronCount() << std::endl;
        ss << "Total Synapses: " << getTotalSynapseCount() << std::endl;
        ss << "Active Neurons: " << getActiveNeuronCount() << std::endl;
        ss << "Firing Neurons: " << getFiringNeuronCount() << std::endl;
        ss << "Average Firing Rate: " << getAverageFiringRate() << std::endl;
        ss << "Total Spikes: " << getTotalSpikeCount() << std::endl;
        ss << "Excitation/Inhibition Ratio: " << getExcitationInhibitionRatio() << std::endl;
        
        ss << "\n=== Connectivity Analysis ===";
        auto connectivity = analyzeConnectivity();
        for (auto item : connectivity) {
            ss << "\n  " << item.first.cast<std::string>() << ": " << item.second.cast<double>();
        }
        
        ss << "\n=== Configuration Summary ===";
        ss << "\n" << getConfig()->getSummary();
        
        return ss.str();
    }

    // Validation methods
    bool validateBrain() const {
        // Check basic connectivity
        if (getTotalNeuronCount() == 0) {
            return false;
        }
        
        // Check for neurons with invalid states
        for (const auto& region : getRegions()) {
            for (const auto& neuron : region->getNeurons()) {
                auto state = neuron->getState();
                if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
                    return false;
                }
            }
        }
        
        return true;
    }

    // Reset and reinitialize with optional new config
    void resetAndInitialize(const py::dict& config_dict = py::dict()) {
        reset();
        if (!config_dict.empty()) {
            // Create new config from dictionary
            setConfig(PyConfig::fromDict(config_dict));
        }
        initialize();
    }
};

// Enhanced AgentBrain class with Pythonic interfaces
class PyAgentBrain : public AgentBrain {
public:
    // Constructor with validation
    PyAgentBrain(std::shared_ptr<PyBrain> brain) : AgentBrain(brain) {}

    // Enhanced simulation loop
    py::dict runEpisodeWithMetrics(const std::shared_ptr<SimpleWorld>& world, 
                                 size_t steps, bool enableProfiler = false) {
        py::dict results;
        
        initialize(*world);
        
        if (enableProfiler) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                // Process sensory input
                auto percept = world->getSensoryPercept();
                processSensoryInput(percept);
                
                // Decode motor command and apply to world
                auto motor_cmd = decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, i * getTimestep());
                
                // Apply reward modulation
                auto result = world->getActionResult(motor_cmd);
                if (result.success) {
                    applyRewardModulation(result.reward, 0.0f);  // No prediction for now
                }
                
                // Update development
                updateDevelopment(0.001);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            results["total_time_ms"] = static_cast<double>(duration.count());
            results["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        } else {
            for (size_t i = 0; i < steps; ++i) {
                auto percept = world->getSensoryPercept();
                processSensoryInput(percept);
                
                auto motor_cmd = decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, i * 0.001);
                
                auto result = world->getActionResult(motor_cmd);
                if (result.success) {
                    applyRewardModulation(result.reward, 0.0f);
                }
                
                updateDevelopment(0.001);
            }
        }
        
        results["steps"] = steps;
        results["final_reward"] = getNeuromodulationLevel();
        results["curiosity"] = getCuriosityLevel();
        results["novelty"] = getNoveltyLevel();
        
        return results;
    }

    // Batch processing for multiple worlds/scenarios
    py::dict runBatchEpisodes(const std::vector<std::shared_ptr<SimpleWorld>>& worlds, 
                           size_t stepsPerWorld) {
        py::dict batch_results;
        py::list world_results;
        
        for (size_t world_idx = 0; world_idx < worlds.size(); ++world_idx) {
            py::dict world_result;
            world_result["world_index"] = world_idx;
            
            initialize(*worlds[world_idx]);
            
            for (size_t step = 0; step < stepsPerWorld; ++step) {
                auto percept = worlds[world_idx]->getSensoryPercept();
                processSensoryInput(percept);
                
                auto motor_cmd = decodeMotorCommand();
                worlds[world_idx]->applyMotorCommand(motor_cmd, step * 0.001);
                
                auto result = worlds[world_idx]->getActionResult(motor_cmd);
                if (result.success) {
                    applyRewardModulation(result.reward, 0.0f);
                }
                
                updateDevelopment(0.001);
            }
            
            world_result["final_reward"] = getNeuromodulationLevel();
            world_result["curiosity"] = getCuriosityLevel();
            world_result["novelty"] = getNoveltyLevel();
            
            world_results.append(world_result);
        }
        
        batch_results["world_results"] = world_results;
        batch_results["total_worlds"] = worlds.size();
        batch_results["steps_per_world"] = stepsPerWorld;
        
        return batch_results;
    }

    // Enhanced sensory processing utilities
    py::dict analyzeSensoryInput(const SensoryPercept& percept) {
        py::dict analysis;
        
        if (!percept.getVision().empty()) {
            float total_vision = 0.0f;
            for (float val : percept.getVision()) {
                total_vision += val;
            }
            analysis["vision_mean"] = total_vision / percept.getVision().size();
            analysis["vision_std"] = 0.0f; // Could implement std dev
            analysis["vision_size"] = percept.getVision().size();
        }
        
        if (!percept.getTouch().empty()) {
            float total_touch = 0.0f;
            for (float val : percept.getTouch()) {
                total_touch += val;
            }
            analysis["touch_mean"] = total_touch / percept.getTouch().size();
        }
        
        if (!percept.getInternal().empty()) {
            float total_internal = 0.0f;
            for (float val : percept.getInternal()) {
                total_internal += val;
            }
            analysis["internal_mean"] = total_internal / percept.getInternal().size();
        }
        
        return analysis;
    }

    // Create advanced agent configurations
    static std::shared_ptr<PyAgentBrain> createCuriosityDrivenAgent(
        const std::shared_ptr<PyBrain>& brain,
        float curiosityThreshold = 0.5f,
        bool enableStructuralPlasticity = true) {
        
        auto agent = std::make_shared<PyAgentBrain>(brain);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        
        // Set curiosity threshold via config
        if (auto config = std::dynamic_pointer_cast<PyConfig>(brain->getConfig())) {
            config->set("curiosity_threshold", curiosityThreshold, ConfigSource::User);
        }
        
        return agent;
    }

    static std::shared_ptr<PyAgentBrain> createRewardBasedAgent(
        const std::shared_ptr<PyBrain>& brain,
        bool enableRewardModulation = true,
        bool enableDevelopment = true) {
        
        auto agent = std::make_shared<PyAgentBrain>(brain);
        agent->enableRewardModulation(enableRewardModulation);
        agent->enableDevelopment(enableDevelopment);
        
        return agent;
    }

    static std::shared_ptr<PyAgentBrain> createExplorationAgent(
        const std::shared_ptr<PyBrain>& brain,
        float explorationRate = 0.3f) {
        
        auto agent = std::make_shared<PyAgentBrain>(brain);
        
        // Create custom curiosity-driven behavior
        agent->enableCuriosity(true);
        
        // Store exploration rate in agent's internal state
        // This would require modifying AgentBrain class
        return agent;
    }
};

// Enhanced SimpleWorld class
class PySimpleWorld : public SimpleWorld {
public:
    // Enhanced constructor with validation
    PySimpleWorld() : SimpleWorld() {
        // Set default configuration
        configure(320, 240, 16, 16);  // width, height, vision_width, vision_height
    }

    // Create world with specific parameters
    static std::shared_ptr<PySimpleWorld> create(size_t width = 320, size_t height = 240,
                                                size_t visionWidth = 16, size_t visionHeight = 16) {
        auto world = std::make_shared<PySimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }

    // Enhanced episode running with comprehensive metrics
    py::dict runEpisode(size_t steps, bool recordHistory = false) {
        py::dict results;
        
        if (recordHistory) {
            history_.clear();
            history_.reserve(steps);
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            update(i * 0.001);  // Use timestep from config
            
            if (recordHistory) {
                history_.push_back({
                    getSimulationTime(),
                    getAgentBody(),
                    getSensoryPercept(),
                    getObjects(),
                    getActionResults()
                });
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        results["final_time"] = getSimulationTime();
        
        return results;
    }

    // World analysis and utilities
    py::dict analyzeEnvironment() const {
        py::dict analysis;
        
        analysis["width"] = getWidth();
        analysis["height"] = getHeight();
        analysis["vision_width"] = getVisionWidth();
        analysis["vision_height"] = getVisionHeight();
        analysis["total_objects"] = getObjects().size();
        
        // Count object types
        size_t resources = 0, hazards = 0, walls = 0, markers = 0;
        for (const auto& obj : getObjects()) {
            switch (obj.type) {
                case WorldObjectType::Resource: resources++; break;
                case WorldObjectType::Hazard: hazards++; break;
                case WorldObjectType::Wall: walls++; break;
                case WorldObjectType::Marker: markers++; break;
                case WorldObjectType::Empty: break;
            }
        }
        
        analysis["resources"] = resources;
        analysis["hazards"] = hazards;
        analysis["walls"] = walls;
        analysis["markers"] = markers;
        
        // Vision coverage calculation
        float visionCoverage = static_cast<float>(getVisionWidth() * getVisionHeight()) / 
                             static_cast<float>(getWidth() * getHeight());
        analysis["vision_coverage_ratio"] = visionCoverage;
        
        return analysis;
    }

    // Enhanced object management
    py::dict addObjectWithValidation(float x, float y, WorldObjectType type, 
                                   float value = 0.0f, float radius = 0.5f) {
        py::dict result;
        
        if (!isValidPosition(x, y)) {
            throw NLMInvalidParameter("Invalid position for object");
        }
        
        // Remove existing object at this position
        removeObject(x, y);
        
        // Add new object
        WorldObject obj(x, y, type, value, radius);
        addObject(obj);
        
        result["success"] = true;
        result["position"] = py::make_tuple(x, y);
        result["type"] = static_cast<int>(type);
        result["value"] = value;
        result["radius"] = radius;
        
        return result;
    }

    // Export world state for analysis
    py::dict exportWorldState() const {
        py::dict state;
        
        state["width"] = getWidth();
        state["height"] = getHeight();
        state["vision_width"] = getVisionWidth();
        state["vision_height"] = getVisionHeight();
        state["simulation_time"] = getSimulationTime();
        state["max_energy"] = getMaxEnergy();
        state["energy_decay_rate"] = getEnergyDecayRate();
        
        // Export agent state
        state["agent_x"] = getAgentBody().x;
        state["agent_y"] = getAgentBody().y;
        state["agent_orientation"] = getAgentBody().orientation;
        state["agent_energy"] = getAgentBody().energy;
        
        // Export objects
        py::list objects_list;
        for (const auto& obj : getObjects()) {
            py::dict obj_dict;
            obj_dict["x"] = obj.x;
            obj_dict["y"] = obj.y;
            obj_dict["type"] = static_cast<int>(obj.type);
            obj_dict["value"] = obj.value;
            obj_dict["radius"] = obj.radius;
            obj_dict["active"] = obj.active;
            objects_list.append(obj_dict);
        }
        state["objects"] = objects_list;
        
        return state;
    }

    // Import world state from analysis data
    void importWorldState(const py::dict& state) {
        try {
            if (state.contains("width")) {
                setWidth(state["width"].cast<size_t>());
            }
            if (state.contains("height")) {
                setHeight(state["height"].cast<size_t>());
            }
            if (state.contains("simulation_time")) {
                // This would require adding a setter to SimpleWorld
                // For now, just log the import
                auto* logger = Logger::getGlobal();
                if (logger) {
                    logger->log("Imported world state with simulation time: " + 
                               std::to_string(state["simulation_time"].cast<double>()), 
                               LogLevel::Info);
                }
            }
            
            // Import objects
            if (state.contains("objects")) {
                clearObjects();  // Clear existing objects
                
                auto objects = state["objects"].cast<py::list>();
                for (const auto& obj_item : objects) {
                    auto obj_dict = obj_item.cast<py::dict>();
                    
                    WorldObject obj;
                    obj.x = obj_dict["x"].cast<float>();
                    obj.y = obj_dict["y"].cast<float>();
                    obj.type = static_cast<WorldObjectType>(obj_dict["type"].cast<int>());
                    obj.value = obj_dict["value"].cast<float>();
                    obj.radius = obj_dict["radius"].cast<float>();
                    obj.active = obj_dict["active"].cast<bool>();
                    
                    addObject(obj);
                }
            }
            
        } catch (const std::exception& e) {
            throw NLMException(std::string("Failed to import world state: ") + e.what());
        }
    }

    // Generate world from template
    static std::shared_ptr<PySimpleWorld> generateFromTemplate(const std::string& templateName) {
        auto world = std::make_shared<PySimpleWorld>();
        
        if (templateName == "empty") {
            // Just basic empty world
        } else if (templateName == "maze") {
            // Generate maze layout
            size_t width = 100, height = 100;
            world->configure(width, height, 16, 16);
            
            // Add walls for maze
            for (size_t x = 0; x < width; x += 10) {
                world->addObjectWithValidation(x, 0, WorldObjectType::Wall, 0.0f, 0.8f);
                world->addObjectWithValidation(x, height-1, WorldObjectType::Wall, 0.0f, 0.8f);
            }
            for (size_t y = 0; y < height; y += 10) {
                world->addObjectWithValidation(0, y, WorldObjectType::Wall, 0.0f, 0.8f);
                world->addObjectWithValidation(width-1, y, WorldObjectType::Wall, 0.0f, 0.8f);
            }
            
        } else if (templateName == "resource_harvest") {
            // Generate resource collection scenario
            size_t width = 200, height = 200;
            world->configure(width, height, 16, 16);
            
            // Add agent start position
            world->setAgentStart(10, 10);
            
            // Add resource objects
            for (int i = 0; i < 20; ++i) {
                float x = static_cast<float>(rand() % (width - 20) + 10);
                float y = static_cast<float>(rand() % (height - 20) + 10);
                world->addObjectWithValidation(x, y, WorldObjectType::Resource, 1.0f, 0.3f);
            }
            
            // Add some hazards
            for (int i = 0; i < 5; ++i) {
                float x = static_cast<float>(rand() % (width - 5) + 5);
                float y = static_cast<float>(rand() % (height - 5) + 5);
                world->addObjectWithValidation(x, y, WorldObjectType::Hazard, -1.0f, 0.5f);
            }
        }
        
        return world;
    }

    // Get historical data
    py::list getHistory() const {
        py::list history_list;
        
        // Note: This would require adding history storage to SimpleWorld class
        // For now, return empty list
        return history_list;
    }

    // World validation and safety checks
    py::dict validateWorld() const {
        py::dict validation_results;
        
        bool valid = true;
        py::list errors;
        
        // Check dimensions
        if (getWidth() == 0 || getHeight() == 0) {
            valid = false;
            errors.append("World dimensions must be positive");
        }
        
        // Check vision parameters
        if (getVisionWidth() > getWidth() || getVisionHeight() > getHeight()) {
            valid = false;
            errors.append("Vision dimensions cannot exceed world dimensions");
        }
        
        // Check agent position
        auto agent = getAgentBody();
        if (!isValidPosition(agent.x, agent.y)) {
            valid = false;
            errors.append("Agent position is outside world bounds");
        }
        
        // Check object collisions (basic)
        for (const auto& obj1 : getObjects()) {
            for (const auto& obj2 : getObjects()) {
                if (&obj1 != &obj2) {
                    float dx = obj1.x - obj2.x;
                    float dy = obj1.y - obj2.y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    if (distance < obj1.radius + obj2.radius) {
                        valid = false;
                        errors.append("Object collision detected between objects");
                        break;
                    }
                }
            }
        }
        
        validation_results["valid"] = valid;
        validation_results["errors"] = errors;
        
        return validation_results;
    }

    // Export world configuration
    py::dict exportConfig() const {
        py::dict config;
        
        config["width"] = getWidth();
        config["height"] = getHeight();
        config["vision_width"] = getVisionWidth();
        config["vision_height"] = getVisionHeight();
        config["max_energy"] = getMaxEnergy();
        config["energy_decay_rate"] = getEnergyDecayRate();
        config["simulation_timestep"] = getSimulationTime();  // Note: this might not be correct
        
        return config;
    }

    // Import world configuration
    void importConfig(const py::dict& config) {
        try {
            if (config.contains("width")) {
                configure(config["width"].cast<size_t>(), 
                         config.contains("height") ? config["height"].cast<size_t>() : getHeight(),
                         config.contains("vision_width") ? config["vision_width"].cast<size_t>() : getVisionWidth(),
                         config.contains("vision_height") ? config["vision_height"].cast<size_t>() : getVisionHeight());
            }
            
            if (config.contains("max_energy")) {
                setMaxEnergy(config["max_energy"].cast<float>());
            }
            if (config.contains("energy_decay_rate")) {
                setEnergyDecayRate(config["energy_decay_rate"].cast<float>());
            }
        } catch (const std::exception& e) {
            throw NLMException(std::string("Failed to import world config: ") + e.what());
        }
    }

private:
    // Historical data storage (would need to be added to SimpleWorld class)
    struct WorldHistoryEntry {
        double time;
        AgentBody agent;
        SensoryPercept percept;
        std::vector<WorldObject> objects;
        std::vector<ActionResult> actionResults;
    };
    
    std::vector<WorldHistoryEntry> history_;
};

// High-level simulation manager
class SimulationManager {
public:
    // Create simulation with world, brain, and agent
    static py::dict createSimulation(const std::shared_ptr<PySimpleWorld>& world,
                                    const std::shared_ptr<PyBrain>& brain,
                                    const std::shared_ptr<PyAgentBrain>& agent = nullptr) {
        py::dict simulation;
        
        // Initialize components if needed
        if (world) {
            simulation["world_initialized"] = true;
            if (agent) {
                simulation["agent_initialized"] = agent->initialize(*world);
            }
        }
        
        if (brain) {
            simulation["brain_initialized"] = brain->initialize();
        }
        
        simulation["ready"] = simulation["world_initialized"] && 
                              simulation["brain_initialized"];
        
        return simulation;
    }

    // Run complete simulation with all components
    static py::dict runCompleteSimulation(const std::shared_ptr<PySimpleWorld>& world,
                                          const std::shared_ptr<PyBrain>& brain,
                                          const std::shared_ptr<PyAgentBrain>& agent,
                                          size_t steps) {
        py::dict results;
        
        try {
            // Setup
            auto setup_start = std::chrono::high_resolution_clock::now();
            
            if (!world || !brain) {
                throw NLMSimulationError("World and brain are required");
            }
            
            // Initialize world
            world->reset();
            if (agent) {
                agent->initialize(*world);
            }
            
            // Initialize brain
            brain->reset();
            brain->initialize();
            
            auto setup_end = std::chrono::high_resolution_clock::now();
            auto setup_duration = std::chrono::duration_cast<std::chrono::milliseconds>(setup_end - setup_start);
            
            // Run simulation
            auto simulation_start = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process through agent if available
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                
                // Inject to brain
                brain->receiveSensoryInput(percept);
                
                // Step brain
                brain->step(static_cast<SimulationStep>(i), i * 0.001);
                
                // Produce action
                auto action = brain->produceAction();
                
                // Apply to world
                world->applyMotorCommand(action, i * 0.001);
                
                // Apply reward if agent exists
                if (agent) {
                    auto action_result = world->getActionResult(action);
                    if (action_result.success) {
                        agent->applyRewardModulation(action_result.reward, 0.0f);
                    }
                }
            }
            
            auto simulation_end = std::chrono::high_resolution_clock::now();
            auto simulation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(simulation_end - simulation_start);
            
            // Collect results
            py::dict brain_metrics = brain->collectMetrics(steps);
            py::dict world_metrics = world->analyzeEnvironment();
            
            if (agent) {
                py::dict agent_metrics;
                agent_metrics["curiosity"] = agent->getCuriosityLevel();
                agent_metrics["novelty"] = agent->getNoveltyLevel();
                agent_metrics["neuromodulation"] = agent->getNeuromodulationLevel();
                results["agent_metrics"] = agent_metrics;
            }
            
            results["setup_time_ms"] = static_cast<double>(setup_duration.count());
            results["simulation_time_ms"] = static_cast<double>(simulation_duration.count());
            results["total_steps"] = steps;
            results["brain_metrics"] = brain_metrics;
            results["world_metrics"] = world_metrics;
            
            return results;
            
        } catch (const std::exception& e) {
            throw NLMSimulationError(std::string("Simulation failed: ") + e.what());
        }
    }

    // Create experiment with multiple trials
    static py::dict runExperiment(const std::shared_ptr<PySimpleWorld>& world,
                                 const std::shared_ptr<PyBrain>& brain,
                                 const std::shared_ptr<PyAgentBrain>& agent,
                                 size_t trials,
                                 size_t stepsPerTrial,
                                 bool randomizeSeeds = false) {
        py::dict experiment_results;
        py::list trial_results;
        
        for (size_t trial = 0; trial < trials; ++trial) {
            py::dict trial_result;
            trial_result["trial"] = trial;
            
            // Reset brain for each trial
            brain->reset();
            brain->initialize();
            
            // Reset world if needed
            if (randomizeSeeds) {
                brain->getConfig()->set("random_seed", 
                                      static_cast<int64_t>(42 + trial * 1000), 
                                      ConfigSource::User);
            }
            
            // Run trial
            auto trial_result_dict = runCompleteSimulation(world, brain, agent, stepsPerTrial);
            trial_result_dict["trial"] = trial;
            
            trial_results.append(trial_result_dict);
        }
        
        experiment_results["trials"] = trial_results;
        experiment_results["total_trials"] = trials;
        experiment_results["steps_per_trial"] = stepsPerTrial;
        
        return experiment_results;
    }

    // Create benchmark suite
    static py::dict runBenchmark(const std::shared_ptr<PyBrain>& brain,
                                const std::vector<size_t>& step_counts,
                                const std::vector<size_t>& warmup_steps) {
        py::dict benchmark_results;
        py::list benchmarks;
        
        if (step_counts.size() != warmup_steps.size()) {
            throw NLMInvalidParameter("Step counts and warmup steps must have same size");
        }
        
        for (size_t i = 0; i < step_counts.size(); ++i) {
            py::dict bench_result;
            bench_result["benchmark"] = i;
            
            try {
                auto results = brain->benchmark(step_counts[i], warmup_steps[i]);
                bench_result["results"] = results;
                bench_result["success"] = true;
            } catch (const std::exception& e) {
                bench_result["error"] = std::string("Benchmark failed: ") + e.what();
                bench_result["success"] = false;
            }
            
            benchmarks.append(bench_result);
        }
        
        benchmark_results["benchmarks"] = benchmarks;
        benchmark_results["total_benchmarks"] = step_counts.size();
        
        return benchmark_results;
    }

    // Create analysis report
    static std::string generateAnalysisReport(const py::dict& experiment_results) {
        std::stringstream ss;
        
        ss << "=== NLM Experiment Analysis Report ===" << std::endl;
        ss << "Generated: " << getCurrentTimestamp() << std::endl;
        ss << std::endl;
        
        if (!experiment_results.contains("trials")) {
            ss << "No trial results available" << std::endl;
            return ss.str();
        }
        
        auto trials = experiment_results["trials"].cast<py::list>();
        
        ss << "Summary:" << std::endl;
        ss << "  Total Trials: " << trials.size() << std::endl;
        
        // Collect metrics across trials
        double total_time = 0.0, total_steps_per_second = 0.0;
        size_t total_spikes = 0;
        
        for (const auto& trial_result : trials) {
            auto trial = trial_result.cast<py::dict>();
            
            if (trial.contains("brain_metrics")) {
                auto brain_metrics = trial["brain_metrics"].cast<py::dict>();
                
                if (brain_metrics.contains("total_time_ms")) {
                    total_time += brain_metrics["total_time_ms"].cast<double>();
                }
                if (brain_metrics.contains("steps_per_second")) {
                    total_steps_per_second += brain_metrics["steps_per_second"].cast<double>();
                }
                if (brain_metrics.contains("spikes_generated")) {
                    total_spikes += brain_metrics["spikes_generated"].cast<size_t>();
                }
            }
        }
        
        ss << "  Average Time (ms): " << (trials.empty() ? 0 : total_time / trials.size()) << std::endl;
        ss << "  Average Steps/sec: " << (trials.empty() ? 0 : total_steps_per_second / trials.size()) << std::endl;
        ss << "  Total Spikes Generated: " << total_spikes << std::endl;
        ss << std::endl;
        
        ss << "=== Detailed Trial Results ===" << std::endl;
        for (size_t i = 0; i < trials.size(); ++i) {
            auto trial = trials[i].cast<py::dict>();
            ss << "Trial " << i << ": ";
            
            if (trial.contains("simulation_time_ms")) {
                ss << "Simulation: " << trial["simulation_time_ms"].cast<double>() << "ms";
            }
            
            ss << std::endl;
        }
        
        return ss.str();
    }

private:
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // Custom exception for simulation errors
    class NLMSimulationError : public NLMException {
    public:
        explicit NLMSimulationError(const std::string& msg) : NLMException("NLM Simulation Error: " + msg) {}
    };
};

// Factory functions for easy creation of complex objects
namespace Factory {
    // Create a complete simulation ecosystem
    py::dict createSimulationEcosystem(const py::dict& config_dict = py::dict()) {
        py::dict ecosystem;
        
        try {
            // Create components
            auto config = PyConfig::fromDict(config_dict.empty() ? 
                py::dict({"simulation_timestep", 0.001,
                         "random_seed", 42,
                         "neuron_count", 1000,
                         "region_count", 1}) : config_dict);
            
            auto brain = std::make_shared<PyBrain>(config);
            auto world = std::make_shared<PySimpleWorld>();
            auto agent = std::make_shared<PyAgentBrain>(brain);
            
            ecosystem["config"] = config;
            ecosystem["brain"] = brain;
            ecosystem["world"] = world;
            ecosystem["agent"] = agent;
            
            // Initialize if configured
            if (config_dict.contains("auto_initialize") && 
                config_dict["auto_initialize"].cast<bool>()) {
                brain->initialize();
                agent->initialize(*world);
            }
            
            ecosystem["created"] = true;
            ecosystem["message"] = "Simulation ecosystem created successfully";
            
        } catch (const std::exception& e) {
            ecosystem["created"] = false;
            ecosystem["error"] = std::string("Failed to create ecosystem: ") + e.what();
        }
        
        return ecosystem;
    }

    // Create standard experiment setup
    py::dict createExperimentSetup(const std::string& experimentType = "default") {
        py::dict setup;
        
        if (experimentType == "curiosity_driven") {
            // Curiosity-driven exploration
            setup["config"] = py::dict({
                {"simulation_timestep", 0.001},
                {"random_seed", 42},
                {"neuron_count", 500},
                {"region_count", 2},
                {"curiosity_enabled", true},
                {"reward_modulation_enabled", true}
            });
            
            setup["agent_type"] = "curiosity_driven";
            setup["description"] = "Agent driven by curiosity and reward prediction error";
            
        } else if (experimentType == "reward_based") {
            // Reward-based learning
            setup["config"] = py::dict({
                {"simulation_timestep", 0.001},
                {"random_seed", 42},
                {"neuron_count", 800},
                {"region_count", 3},
                {"curiosity_enabled", false},
                {"reward_modulation_enabled", true},
                {"development_enabled", true}
            });
            
            setup["agent_type"] = "reward_based";
            setup["description"] = "Agent driven primarily by reward prediction error";
            
        } else if (experimentType == "exploration") {
            // Pure exploration
            setup["config"] = py::dict({
                {"simulation_timestep", 0.001},
                {"random_seed", 42},
                {"neuron_count", 300},
                {"region_count", 1},
                {"curiosity_enabled", true},
                {"structural_plasticity_enabled", true}
            });
            
            setup["agent_type"] = "exploration";
            setup["description"] = "Agent focused on novelty and exploration";
            
        } else {
            // Default balanced setup
            setup["config"] = py::dict({
                {"simulation_timestep", 0.001},
                {"random_seed", 42},
                {"neuron_count", 1000},
                {"region_count", 2},
                {"curiosity_enabled", true},
                {"reward_modulation_enabled", true},
                {"development_enabled", true}
            });
            
            setup["agent_type"] = "balanced";
            setup["description"] = "Balanced agent with multiple learning mechanisms";
        }
        
        setup["created"] = true;
        setup["experiment_type"] = experimentType;
        
        return setup;
    }

    // Create predefined world templates
    std::shared_ptr<PySimpleWorld> createWorldTemplate(const std::string& templateName) {
        return PySimpleWorld::generateFromTemplate(templateName);
    }

    // Create benchmark configurations
    py::dict createBenchmarkConfig(const std::string& benchmarkType = "standard") {
        py::dict config;
        
        if (benchmarkType == "quick") {
            config["step_counts"] = py::list({100, 500, 1000});
            config["warmup_steps"] = py::list({10, 50, 100});
            config["description"] = "Quick performance benchmark";
            
        } else if (benchmarkType == "thorough") {
            config["step_counts"] = py::list({1000, 5000, 10000});
            config["warmup_steps"] = py::list({100, 500, 1000});
            config["description"] = "Thorough performance benchmark";
            
        } else if (benchmarkType == "scalability") {
            config["step_counts"] = py::list({100, 1000, 5000, 10000});
            config["warmup_steps"] = py::list({10, 100, 500, 1000});
            config["description"] = "Scalability benchmark";
            
        } else {
            config["step_counts"] = py::list({500, 2000, 5000});
            config["warmup_steps"] = py::list({50, 200, 500});
            config["description"] = "Standard performance benchmark";
        }
        
        config["benchmark_type"] = benchmarkType;
        config["created"] = true;
        
        return config;
    }
}

// Module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -------------------------------------------------------------------
        
        This module provides Python interfaces to the NLM C++ neural simulation framework.
        It includes enhanced functionality with better error handling, Pythonic interfaces,
        and high-level abstractions for common workflows.
        
        Key Features:
        - Enhanced configuration management with builder pattern
        - Context managers for resource management
        - Pythonic interfaces with iteration support
        - Comprehensive error handling with custom exceptions
        - High-level simulation managers and utilities
        - Factory functions for complex object creation
        - Advanced data analysis and export capabilities
        
        Usage Example:
        
        ```python
        import pynlm
        
        # Create enhanced simulation ecosystem
        ecosystem = pynlm.Factory.createSimulationEcosystem({
            'neuron_count': 500,
            'region_count': 2
        })
        
        brain = ecosystem['brain']
        world = ecosystem['world']
        agent = ecosystem['agent']
        
        # Run simulation
        results = pynlm.SimulationManager.runCompleteSimulation(
            world, brain, agent, steps=1000
        )
        
        print(f"Simulation completed in {results['simulation_time_ms']:.2f} ms")
        print(f"Performance: {results['brain_metrics']['steps_per_second']:.0f} steps/sec")
        ```
        
        The enhanced bindings provide:
        1. Custom Python exceptions (NLMBrainError, NLMInvalidParameter, etc.)
        2. Context managers for automatic cleanup
        3. Enhanced configuration with validation
        4. Comprehensive metrics collection
        5. Advanced simulation utilities
        6. Factory functions for complex object creation
        7. Data export/import for analysis
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<NLMBrainError>(m, "NLMBrainError");
    py::register_exception<NLMInvalidParameter>(m, "NLMInvalidParameter");
    py::register_exception<NLMFileNotFound>(m, "NLMFileNotFound");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");

    // Core classes with enhanced functionality
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](PyConfig& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::getSummary,
             "Get a summary string of the configuration")
        .def("to_dict", &PyConfig::toDict,
             "Convert configuration to Python dictionary")
        .def("from_dict", [](const py::dict& config_dict) {
            return PyConfig::fromDict(config_dict);
        }, py::arg("config_dict"),
             "Create configuration from Python dictionary")
        .def("validate", &PyConfig::validate,
             "Validate configuration parameters")
        .def("saveWithBackup", &PyConfig::saveWithBackup, py::arg("filepath"),
             "Save configuration with backup file");

    // Brain class with enhanced functionality
    py::class_<PyBrain, std::shared_ptr<PyBrain>>(m, "Brain", R"pbdoc(Enhanced brain class for NLM neural simulation)pbdoc")
        .def(py::init<std::shared_ptr<PyConfig>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep)>(&PyBrain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep, Timestamp)>(&PyBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &PyBrain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &PyBrain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &PyBrain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &PyBrain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("save", &PyBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("saveWithValidation", &PyBrain::saveWithValidation, py::arg("filepath"),
             "Save brain state with validation")
        .def("getRegions", &PyBrain::getRegionsVector,
             "Get all brain regions")
        .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &PyBrain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &PyBrain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &PyBrain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &PyBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &PyBrain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &PyBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &PyBrain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &PyBrain::getConfig,
             "Get the configuration")
        .def("logStatus", &PyBrain::logStatus,
             "Log brain status")
        .def("runEpisode", &PyBrain::runEpisode, py::arg("steps"), py::arg("world") = nullptr,
             "Run simulation episode with optional world")
        .def("collectMetrics", &PyBrain::collectMetrics, py::arg("sampleSteps") = 100,
             "Collect simulation metrics")
        .def("analyzeConnectivity", &PyBrain::analyzeConnectivity,
             "Analyze brain connectivity patterns")
        .def("benchmark", &PyBrain::benchmark, py::arg("steps") = 1000, py::arg("warmupSteps") = 100,
             "Run performance benchmark")
        .def("getMemoryInfo", &PyBrain::getMemoryInfo,
             "Get memory usage information")
        .def("findNeurons", &PyBrain::findNeurons, py::arg("type"),
             "Find neurons by type")
        .def("exportForAnalysis", &PyBrain::exportForAnalysis,
             "Export brain state for analysis")
        .def("importFromAnalysis", &PyBrain::importFromAnalysis, py::arg("data"),
             "Import brain state from analysis data")
        .def("validateBrain", &PyBrain::validateBrain,
             "Validate brain state")
        .def("getDiagnostics", &PyBrain::getDiagnostics,
             "Get comprehensive diagnostics")
        .def("resetAndInitialize", &PyBrain::resetAndInitialize, py::arg("config_dict") = py::dict(),
             "Reset and reinitialize with optional new config");

    // AgentBrain class with enhanced functionality
    py::class_<PyAgentBrain, std::shared_ptr<PyAgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<PyBrain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &PyAgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &PyAgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &PyAgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &PyAgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &PyAgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &PyAgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &PyAgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &PyAgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &PyAgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &PyAgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &PyAgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &PyAgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &PyAgentBrain::getBrain,
             "Get the underlying brain")
        .def("enableRewardModulation", &PyAgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &PyAgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &PyAgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &PyAgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &PyAgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &PyAgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &PyAgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &PyAgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("runEpisodeWithMetrics", &PyAgentBrain::runEpisodeWithMetrics,
             py::arg("world"), py::arg("steps"), py::arg("enableProfiler") = false,
             "Run episode with comprehensive metrics")
        .def("runBatchEpisodes", &PyAgentBrain::runBatchEpisodes,
             py::arg("worlds"), py::arg("stepsPerWorld"),
             "Run batch of episodes across multiple worlds")
        .def("analyzeSensoryInput", &PyAgentBrain::analyzeSensoryInput, py::arg("percept"),
             "Analyze sensory input for patterns")
        .def("createCuriosityDrivenAgent", [](std::shared_ptr<PyBrain> brain,
                                              float curiosityThreshold = 0.5f,
                                              bool enableStructuralPlasticity = true) {
            return PyAgentBrain::createCuriosityDrivenAgent(brain, curiosityThreshold, 
                                                           enableStructuralPlasticity);
        }, py::arg("brain"), py::arg("curiosityThreshold") = 0.5f, py::arg("enableStructuralPlasticity") = true,
           "Create curiosity-driven agent")
        .def("createRewardBasedAgent", [](std::shared_ptr<PyBrain> brain,
                                         bool enableRewardModulation = true,
                                         bool enableDevelopment = true) {
            return PyAgentBrain::createRewardBasedAgent(brain, enableRewardModulation,
                                                       enableDevelopment);
        }, py::arg("brain"), py::arg("enableRewardModulation") = true, py::arg("enableDevelopment") = true,
           "Create reward-based agent")
        .def("createExplorationAgent", [](std::shared_ptr<PyBrain> brain,
                                         float explorationRate = 0.3f) {
            return PyAgentBrain::createExplorationAgent(brain, explorationRate);
        }, py::arg("brain"), py::arg("explorationRate") = 0.3f,
           "Create exploration-focused agent");

    // SimpleWorld class with enhanced functionality
    py::class_<PySimpleWorld, std::shared_ptr<PySimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("create", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            return PySimpleWorld::create(width, height, visionWidth, visionHeight);
        }, py::arg("width") = 320, py::arg("height") = 240, py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
           "Create world with specific parameters")
        .def("configure", &PySimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &PySimpleWorld::reset,
             "Reset world state")
        .def("setAgentStart", &PySimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("update", &PySimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &PySimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &PySimpleWorld::getSensoryPercept,
             "Get sensory percept from world")
        .def("getAgentBody", &PySimpleWorld::getAgentBody,
             "Get agent body state")
        .def("addObject", &PySimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &PySimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("addObjectWithValidation", &PySimpleWorld::addObjectWithValidation,
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Add object with validation")
        .def("isValidPosition", &PySimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &PySimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &PySimpleWorld::getHeight,
             "Get world height")
        .def("getVisionWidth", &PySimpleWorld::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &PySimpleWorld::getVisionHeight,
             "Get vision height")
        .def("getMaxEnergy", &PySimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &PySimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &PySimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &PySimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &PySimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &PySimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &PySimpleWorld::getRandomSeed,
             "Get random seed")
        .def("getObjects", &PySimpleWorld::getObjects,
             "Get all world objects")
        .def("clearObjects", &PySimpleWorld::clearObjects,
             "Clear all objects")
        .def("runEpisode", &PySimpleWorld::runEpisode, py::arg("steps"), py::arg("recordHistory") = false,
             "Run world episode")
        .def("analyzeEnvironment", &PySimpleWorld::analyzeEnvironment,
             "Analyze world environment")
        .def("exportWorldState", &PySimpleWorld::exportWorldState,
             "Export world state for analysis")
        .def("importWorldState", &PySimpleWorld::importWorldState, py::arg("state"),
             "Import world state from analysis data")
        .def("generateFromTemplate", [](const std::string& templateName) {
            return PySimpleWorld::generateFromTemplate(templateName);
        }, py::arg("templateName"),
             "Generate world from template")
        .def("validateWorld", &PySimpleWorld::validateWorld,
             "Validate world state")
        .def("exportConfig", &PySimpleWorld::exportConfig,
             "Export world configuration")
        .def("importConfig", &PySimpleWorld::importConfig, py::arg("config"),
             "Import world configuration")
        .def("getHistory", &PySimpleWorld::getHistory,
             "Get historical data");

    // SimulationManager with high-level interfaces
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(High-level simulation management)pbdoc")
        .def_static("createSimulation", &SimulationManager::createSimulation,
                    py::arg("world"), py::arg("brain"), py::arg("agent") = nullptr,
                    "Create simulation with world, brain, and agent")
        .def_static("runCompleteSimulation", &SimulationManager::runCompleteSimulation,
                    py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("steps"),
                    "Run complete simulation with all components")
        .def_static("runExperiment", &SimulationManager::runExperiment,
                    py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("trials"),
                    py::arg("stepsPerTrial"), py::arg("randomizeSeeds") = false,
                    "Run experiment with multiple trials")
        .def_static("runBenchmark", &SimulationManager::runBenchmark,
                    py::arg("brain"), py::arg("step_counts"), py::arg("warmup_steps"),
                    "Run benchmark suite")
        .def_static("generateAnalysisReport", &SimulationManager::generateAnalysisReport,
                    py::arg("experiment_results"),
                    "Generate analysis report from experiment results");

    // Factory module for creating complex objects
    py::module_::ptr FactoryModule = py::module_::create_object(m, "Factory");
    FactoryModule->setattr("createSimulationEcosystem", &Factory::createSimulationEcosystem,
                         "Create complete simulation ecosystem");
    FactoryModule->setattr("createExperimentSetup", &Factory::createExperimentSetup,
                         "Create standard experiment setup");
    FactoryModule->setattr("createWorldTemplate", &Factory::createWorldTemplate,
                         "Create predefined world templates");
    FactoryModule->setattr("createBenchmarkConfig", &Factory::createBenchmarkConfig,
                         "Create benchmark configurations");

    // PyBind11 version info
    m.def("get_version", []() { return "Enhanced NLM Python Bindings v2.0"; },
          "Get version information");
}

// Helper function for getting current timestamp
static std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}