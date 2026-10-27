#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exceptions for better error handling
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public NLMRuntimeError {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMMemoryError : public NLMRuntimeError {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMRuntimeError(msg) {}
};

// Context manager for simulation episodes
class SimulationEpisode {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    double timestep_;
    bool initialized_;

public:
    SimulationEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                     std::shared_ptr<AgentBrain> agent, double timestep = 0.1)
        : brain_(brain), world_(world), agent_(agent), timestep_(timestep), initialized_(false) {}

    ~SimulationEpisode() {
        try {
            reset();
        } catch (...) {
            // Ignore exceptions in destructor
        }
    }

    SimulationEpisode& configureWorld(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
        return *this;
    }

    SimulationEpisode& setAgentStart(double x, double y) {
        world_->setAgentStart(x, y);
        return *this;
    }

    SimulationEpisode& enableFeatures(bool rewardMod, bool structuralPlasticity, 
                                     bool development, bool curiosity) {
        if (agent_) {
            agent_->enableRewardModulation(rewardMod);
            agent_->enableStructuralPlasticity(structuralPlasticity);
            agent_->enableDevelopment(development);
            agent_->enableCuriosity(curiosity);
        }
        return *this;
    }

    SimulationEpisode& runSteps(size_t numSteps) {
        if (!initialized_) {
            if (world_) world_->reset();
            if (agent_) agent_->initialize(*world_);
            initialized_ = true;
        }

        for (size_t step = 0; step < numSteps; ++step) {
            world_->update(timestep_);
            
            auto percept = world_->getSensoryPercept();
            if (agent_) agent_->processSensoryInput(percept);
            
            brain_->step(step, world_->getSimulationTime());
            
            auto action = agent_ ? agent_->decodeMotorCommand() : nullptr;
            if (action && world_) {
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
        }
        return *this;
    }

    SimulationEpisode& runUntil(double maxTime) {
        if (!initialized_) {
            if (world_) world_->reset();
            if (agent_) agent_->initialize(*world_);
            initialized_ = true;
        }

        while (world_ && world_->getSimulationTime() < maxTime) {
            world_->update(timestep_);
            
            auto percept = world_->getSensoryPercept();
            if (agent_) agent_->processSensoryInput(percept);
            
            size_t step = static_cast<size_t>(world_->getSimulationTime() / timestep_);
            brain_->step(step, world_->getSimulationTime());
            
            auto action = agent_ ? agent_->decodeMotorCommand() : nullptr;
            if (action) {
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
        }
        return *this;
    }

    SimulationEpisode& runWhile(double duration, std::function<bool()> condition) {
        if (!initialized_) {
            if (world_) world_->reset();
            if (agent_) agent_->initialize(*world_);
            initialized_ = true;
        }

        double startTime = world_->getSimulationTime();
        double endTime = startTime + duration;
        
        while (world_ && world_->getSimulationTime() < endTime && condition()) {
            world_->update(timestep_);
            
            auto percept = world_->getSensoryPercept();
            if (agent_) agent_->processSensoryInput(percept);
            
            size_t step = static_cast<size_t>(world_->getSimulationTime() / timestep_);
            brain_->step(step, world_->getSimulationTime());
            
            auto action = agent_ ? agent_->decodeMotorCommand() : nullptr;
            if (action) {
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
        }
        return *this;
    }

    void reset() {
        if (world_) world_->reset();
        if (brain_) brain_->reset();
        if (agent_) agent_->reset();
        initialized_ = false;
    }

    // Getters for statistics
    size_t getTotalNeurons() const { return brain_ ? brain_->getTotalNeuronCount() : 0; }
    size_t getTotalSynapses() const { return brain_ ? brain_->getTotalSynapseCount() : 0; }
    size_t getFiringNeurons() const { return brain_ ? brain_->getFiringNeuronCount() : 0; }
    float getAverageFiringRate() const { return brain_ ? brain_->getAverageFiringRate() : 0.0f; }
    float getExcitationInhibitionRatio() const { return brain_ ? brain_->getExcitationInhibitionRatio() : 0.0f; }
    size_t getTotalSpikes() const { return brain_ ? brain_->getTotalSpikeCount() : 0; }
    std::string getDevelopmentalStage() const {
        if (!brain_) return "Initial";
        switch (brain_->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }

    // World properties
    size_t getWorldWidth() const { return world_ ? world_->getWidth() : 0; }
    size_t getWorldHeight() const { return world_ ? world_->getHeight() : 0; }
    double getSimulationTime() const { return world_ ? world_->getSimulationTime() : 0.0; }
    size_t getVisionWidth() const { return world_ ? world_->getVisionWidth() : 0; }
    size_t getVisionHeight() const { return world_ ? world_->getVisionHeight() : 0; }

    // Agent statistics
    float getCuriosityLevel() const { return agent_ ? agent_->getCuriosityLevel() : 0.0f; }
    float getNoveltyLevel() const { return agent_ ? agent_->getNoveltyLevel() : 0.0f; }
    float getPredictionError() const { return agent_ ? agent_->getPredictionError() : 0.0f; }
    float getNeuromodulationLevel() const { return agent_ ? agent_->getNeuromodulationLevel() : 0.0f; }

    // Agent operations
    void applyRewardModulation(float reward, float predictedReward = 0.0f) {
        if (agent_) agent_->applyRewardModulation(reward, predictedReward);
    }

    void updateDevelopment(double timestep) {
        if (agent_) agent_->updateDevelopment(timestep);
    }

    // Configuration access
    std::shared_ptr<Config> getConfig() const {
        return brain_ ? brain_->getConfig() : nullptr;
    }

    // State management
    bool saveState(const std::string& filepath) const {
        return brain_ ? brain_->save(filepath) : false;
    }

    bool loadState(const std::string& filepath) {
        return brain_ ? brain_->load(filepath) : false;
    }

    // Memory and performance monitoring
    struct PerformanceStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t firingNeurons;
        float firingRate;
        size_t totalSpikes;
        double memoryUsageMB;
        double simulationTimePerStep;
        size_t stepCount;
    };

    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        stats.totalNeurons = getTotalNeurons();
        stats.totalSynapses = getTotalSynapses();
        stats.firingNeurons = getFiringNeurons();
        stats.firingRate = getAverageFiringRate();
        stats.totalSpikes = getTotalSpikes();
        // Placeholder for memory usage - could be enhanced
        stats.memoryUsageMB = 0.0;
        stats.simulationTimePerStep = 0.0;
        stats.stepCount = static_cast<size_t>(getSimulationTime() / timestep_);
        return stats;
    }

    // Visualization helpers
    struct PerceptionSnapshot {
        std::vector<float> vision;
        std::vector<float> touch;
        std::vector<float> internal;
        std::vector<float> proprioception;
        double timestamp;
        std::string toString() const {
            return "Vision: " + std::to_string(vision.size()) + " values, " +
                   "Touch: " + std::to_string(touch.size()) + " values, " +
                   "Internal: " + std::to_string(internal.size()) + " values";
        }
    };

    PerceptionSnapshot getPerceptionSnapshot() const {
        PerceptionSnapshot snap;
        if (world_) {
            auto percept = world_->getSensoryPercept();
            snap.vision = percept.getVision();
            snap.touch = percept.getTouch();
            snap.internal = percept.getInternal();
            snap.proprioception = percept.getProprioception();
            snap.timestamp = world_->getSimulationTime();
        }
        return snap;
    }

    // Comparison operators for results
    bool operator<(const SimulationEpisode& other) const {
        return getSimulationTime() < other.getSimulationTime();
    }

    bool operator>(const SimulationEpisode& other) const {
        return getSimulationTime() > other.getSimulationTime();
    }

    explicit operator bool() const {
        return initialized_ && brain_ && world_ && agent_;
    }
};

// Batch simulation manager for running multiple experiments
class SimulationBatch {
private:
    struct EpisodeConfig {
        std::string name;
        std::shared_ptr<Config> config;
        size_t steps;
        double maxTime;
        std::function<bool()> condition;
        std::unordered_map<std::string, std::any> parameters;
    };

    std::vector<EpisodeConfig> episodes_;
    std::vector<std::shared_ptr<SimulationEpisode>> results_;
    size_t currentEpisode_;
    bool verbose_;
    std::function<void(const std::string&)> progressCallback_;

public:
    SimulationBatch() : currentEpisode_(0), verbose_(false) {}

    SimulationBatch& addEpisode(const std::string& name, std::shared_ptr<Config> config) {
        EpisodeConfig episode;
        episode.name = name;
        episode.config = config;
        episode.steps = 1000;
        episode.maxTime = 100.0;
        episode.condition = []() { return true; };
        episodes_.push_back(episode);
        return *this;
    }

    SimulationBatch& setSteps(size_t steps, size_t index = std::numeric_limits<size_t>::max()) {
        if (index == std::numeric_limits<size_t>::max()) {
            for (auto& episode : episodes_) {
                episode.steps = steps;
            }
        } else if (index < episodes_.size()) {
            episodes_[index].steps = steps;
        }
        return *this;
    }

    SimulationBatch& setMaxTime(double maxTime, size_t index = std::numeric_limits<size_t>::max()) {
        if (index == std::numeric_limits<size_t>::max()) {
            for (auto& episode : episodes_) {
                episode.maxTime = maxTime;
            }
        } else if (index < episodes_.size()) {
            episodes_[index].maxTime = maxTime;
        }
        return *this;
    }

    SimulationBatch& setCondition(std::function<bool()> condition, size_t index = std::numeric_limits<size_t>::max()) {
        if (index == std::numeric_limits<size_t>::max()) {
            for (auto& episode : episodes_) {
                episode.condition = condition;
            }
        } else if (index < episodes_.size()) {
            episodes_[index].condition = condition;
        }
        return *this;
    }

    SimulationBatch& setVerbose(bool verbose) {
        verbose_ = verbose;
        return *this;
    }

    SimulationBatch& setProgressCallback(std::function<void(const std::string&)> callback) {
        progressCallback_ = callback;
        return *this;
    }

    std::vector<std::shared_ptr<SimulationEpisode>> run() {
        results_.clear();
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            currentEpisode_ = i;
            const auto& episode = episodes_[i];
            
            if (verbose_) {
                std::cout << "Running episode " << (i + 1) << "/" << episodes_.size() 
                         << ": " << episode.name << std::endl;
            }
            
            if (progressCallback_) {
                progressCallback_("Starting episode: " + episode.name);
            }

            // Create brain, world, and agent
            auto brain = std::make_shared<Brain>(episode.config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);

            // Create and run simulation episode
            auto simEpisode = std::make_shared<SimulationEpisode>(brain, world, agent, 0.1);
            
            if (episode.condition) {
                simEpisode->runWhile(episode.maxTime, episode.condition);
            } else {
                simEpisode->runSteps(episode.steps);
            }
            
            results_.push_back(simEpisode);
            
            if (verbose_) {
                std::cout << "Completed episode " << (i + 1) << ": " << episode.name
                         << " - Neurons: " << simEpisode->getTotalNeurons()
                         << ", Spikes: " << simEpisode->getTotalSpikes() << std::endl;
            }
            
            if (progressCallback_) {
                progressCallback_("Completed episode: " + episode.name);
            }
        }
        
        currentEpisode_ = 0;
        return results_;
    }

    size_t getEpisodeCount() const { return episodes_.size(); }
    size_t getCurrentEpisode() const { return currentEpisode_; }
    std::vector<std::shared_ptr<SimulationEpisode>> getResults() const { return results_; }

    // Analysis methods
    struct BatchSummary {
        size_t totalSteps;
        double totalSimulationTime;
        size_t totalNeurons;
        float averageFiringRate;
        size_t totalSpikes;
        std::vector<float> curiosityLevels;
        std::vector<float> noveltyLevels;
        std::map<std::string, float> performanceMetrics;
    };

    BatchSummary analyzeResults() const {
        BatchSummary summary;
        summary.totalSteps = 0;
        summary.totalSimulationTime = 0.0;
        summary.totalNeurons = 0;
        summary.averageFiringRate = 0.0f;
        summary.totalSpikes = 0;
        
        size_t validEpisodes = 0;
        
        for (const auto& episode : results_) {
            if (episode) {
                summary.totalSteps += episode->getSimulationTime() / 0.1; // Assuming timestep 0.1
                summary.totalSimulationTime += episode->getSimulationTime();
                if (episode->getTotalNeurons() > summary.totalNeurons) {
                    summary.totalNeurons = episode->getTotalNeurons();
                }
                summary.averageFiringRate += episode->getAverageFiringRate();
                summary.totalSpikes += episode->getTotalSpikes();
                
                if (episode->getCuriosityLevel() > 0.0f) {
                    summary.curiosityLevels.push_back(episode->getCuriosityLevel());
                }
                if (episode->getNoveltyLevel() > 0.0f) {
                    summary.noveltyLevels.push_back(episode->getNoveltyLevel());
                }
                
                validEpisodes++;
            }
        }
        
        if (validEpisodes > 0) {
            summary.averageFiringRate /= validEpisodes;
        }
        
        // Calculate performance metrics
        if (summary.totalSteps > 0) {
            summary.performanceMetrics["steps_per_second"] = summary.totalSteps / summary.totalSimulationTime;
        }
        
        return summary;
    }

    // Save results to file
    bool saveResults(const std::string& filepath) const {
        // Simple JSON serialization for results
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"episodes\": [\n";
        for (size_t i = 0; i < results_.size(); ++i) {
            file << "    {\n";
            file << "      \"name\": \"" << episodes_[i].name << "\",\n";
            file << "      \"neurons\": " << results_[i]->getTotalNeurons() << ",\n";
            file << "      \"spikes\": " << results_[i]->getTotalSpikes() << ",\n";
            file << "      \"time\": " << results_[i]->getSimulationTime() << ",\n";
            file << "      \"firing_rate\": " << results_[i]->getAverageFiringRate() << ",\n";
            file << "      \"curiosity\": " << results_[i]->getCuriosityLevel() << ",\n";
            file << "      \"novelty\": " << results_[i]->getNoveltyLevel() << "\n";
            if (i < results_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        file << "  ],\n";
        file << "  \"summary\": {\n";
        auto summary = analyzeResults();
        file << "    \"total_steps\": " << summary.totalSteps << ",\n";
        file << "    \"total_time\": " << summary.totalSimulationTime << ",\n";
        file << "    \"avg_firing_rate\": " << summary.averageFiringRate << "\n";
        file << "  }\n";
        file << "}\n";
        
        file.close();
        return true;
    }

    // Compare episodes
    std::vector<size_t> getBestEpisodes(size_t count = 1) const {
        std::vector<std::pair<double, size_t>> scores;
        
        for (size_t i = 0; i < results_.size(); ++i) {
            if (results_[i]) {
                double score = results_[i]->getTotalSpikes() + 
                              results_[i]->getAverageFiringRate() * 100 +
                              results_[i]->getCuriosityLevel() * 10;
                scores.push_back({score, i});
            }
        }
        
        std::sort(scores.begin(), scores.end(), std::greater<>());
        
        std::vector<size_t> bestIndices;
        for (size_t i = 0; i < std::min(count, scores.size()); ++i) {
            bestIndices.push_back(scores[i].second);
        }
        
        return bestIndices;
    }

    // Clear all data
    void clear() {
        episodes_.clear();
        results_.clear();
        currentEpisode_ = 0;
    }
};

// Configuration factory functions
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

std::shared_ptr<Config> createConfigWithPreset(const std::string& preset) {
    auto config = std::make_shared<Config>();
    
    if (preset == "small") {
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1);
        config->set("simulation_timestep", 0.001);
    } else if (preset == "medium") {
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("simulation_timestep", 0.001);
    } else if (preset == "large") {
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.05);
        config->set("simulation_timestep", 0.001);
    } else if (preset == "research") {
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.02);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
    }
    
    return config;
}

// Helper functions for common operations
namespace ExperimentUtils {
    // Run a simple learning experiment
    SimulationEpisode runLearningExperiment(const std::string& name, 
                                           size_t steps,
                                           std::shared_ptr<Config> config = nullptr) {
        if (!config) {
            config = createDefaultConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto episode = SimulationEpisode(brain, world, agent, 0.1);
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        episode.runSteps(steps);
        
        return episode;
    }

    // Run an exploration experiment
    SimulationEpisode runExplorationExperiment(const std::string& name,
                                                size_t steps,
                                                double explorationFactor,
                                                std::shared_ptr<Config> config = nullptr) {
        if (!config) {
            config = createDefaultConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto episode = SimulationEpisode(brain, world, agent, 0.1);
        world->configure(30, 30, 12, 12);
        world->setAgentStart(15.0, 15.0);
        
        // Enable exploration features
        agent->enableCuriosity(true);
        agent->enableNoveltyDetection(true);
        agent->enableDevelopment(true);
        
        // Set curiosity level based on exploration factor
        // This would require adding a setCuriosityLevel method
        
        episode.runSteps(steps);
        return episode;
    }

    // Run a developmental experiment
    SimulationEpisode runDevelopmentalExperiment(const std::string& name,
                                                  size_t steps,
                                                  bool enableStructuralPlasticity,
                                                  std::shared_ptr<Config> config = nullptr) {
        if (!config) {
            config = createDefaultConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto episode = SimulationEpisode(brain, world, agent, 0.1);
        world->configure(15, 15, 6, 6);
        world->setAgentStart(7.5, 7.5);
        
        // Enable developmental features
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        agent->enableCuriosity(true);
        
        episode.runSteps(steps);
        return episode;
    }

    // Batch experiment runner
    std::vector<SimulationEpisode> runBatchExperiment(const std::string& baseName,
                                                       const std::vector<size_t>& stepCounts,
                                                       const std::vector<std::string>& configs = {}) {
        std::vector<SimulationEpisode> results;
        
        for (size_t i = 0; i < stepCounts.size(); ++i) {
            std::string configName = (i < configs.size()) ? configs[i] : "default";
            std::shared_ptr<Config> config;
            
            if (configName == "default") {
                config = createDefaultConfig();
            } else if (configName == "small") {
                config = createConfigWithPreset("small");
            } else if (configName == "medium") {
                config = createConfigWithPreset("medium");
            } else if (configName == "large") {
                config = createConfigWithPreset("large");
            } else if (configName == "research") {
                config = createConfigWithPreset("research");
            }
            
            std::string episodeName = baseName + "_run_" + std::to_string(i + 1);
            
            auto episode = runLearningExperiment(episodeName, stepCounts[i], config);
            results.push_back(episode);
        }
        
        return results;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python API for the NLM C++ neural simulation framework.
        
        This module provides:
        - Pythonic simulation episode management
        - Batch experiment running capabilities
        - Advanced configuration options
        - Debugging and profiling tools
        - Experiment management utilities
        
        Key features:
        - Context manager for simulation episodes
        - Batch experiment execution
        - Performance monitoring
        - Result analysis and comparison
        - Experiment factory functions
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError");

    // NeuronId class with Pythonic methods
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId class with Pythonic methods
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId class
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId class
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

    // Config class with enhanced Python interface
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
        })
        // Pythonic property access
        .def("get", [](Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"),
             "Get configuration value as generic type")
        .def("get_or", [](Config& self, const std::string& key, const std::any& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const std::any& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (Pythonic)")
        .def("set_int", &Config::set, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set_double", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set double configuration value")
        .def("set_bool", &Config::set, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("set_string", &Config::set, py::arg("key"), py::arg("value"),
             "Set string configuration value");

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with Pythonic interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        })
        .def("__str__", [](const Action& action) {
            return "Action(" + action.getName() + ")";
        });

    // WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ", " + std::to_string(static_cast<int>(obj.type)) + ">";
        });

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with Pythonic methods
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept: vision=" << percept.getVisionWidth() << "x" << percept.getVisionHeight()
               << ", touch=" << percept.getTouch().size()
               << ", internal=" << percept.getInternal().size() << ">";
            return ss.str();
        });

    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + ", time=" +
                   std::to_string(world.getSimulationTime()) + ">";
        });

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain: " << brain.getTotalNeuronCount() << " neurons, "
               << brain.getTotalSynapseCount() << " synapses, "
               << brain.getFiringNeuronCount() << " firing, "
               << brain.getAverageFiringRate() << " Hz>";
            return ss.str();
        });

    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
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
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("__repr__", [](const AgentBrain& agent) {
            std::stringstream ss;
            ss << "<AgentBrain: " << (agent.isRewardModulationEnabled() ? "reward " : "")
               << (agent.isCuriosityEnabled() ? "curiosity " : "")
               << (agent.isDevelopmentEnabled() ? "development " : "")
               << (agent.isStructuralPlasticityEnabled() ? "structural " : "")
               << "enabled>";
            return ss.str();
        });

    // Factory functions
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

    // Enhanced simulation utilities
    py::class_<SimulationEpisode>(m, "SimulationEpisode", 
        R"pbdoc(A context manager for running simulation episodes with convenient Python APIs.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, 
                      std::shared_ptr<AgentBrain>, double>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("timestep") = 0.1,
             "Create a simulation episode context manager")
        .def("configureWorld", &SimulationEpisode::configureWorld, 
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions (fluent API)")
        .def("setAgentStart", &SimulationEpisode::setAgentStart, 
             py::arg("x"), py::arg("y"),
             "Set agent starting position (fluent API)")
        .def("enableFeatures", &SimulationEpisode::enableFeatures,
             py::arg("rewardMod"), py::arg("structuralPlasticity"), 
             py::arg("development"), py::arg("curiosity"),
             "Enable brain features (fluent API)")
        .def("runSteps", &SimulationEpisode::runSteps, py::arg("numSteps"),
             "Run simulation for specified number of steps (fluent API)")
        .def("runUntil", &SimulationEpisode::runUntil, py::arg("maxTime"),
             "Run simulation until specified time (fluent API)")
        .def("runWhile", &SimulationEpisode::runWhile, py::arg("duration"), py::arg("condition"),
             "Run simulation while condition is true (fluent API)")
        .def("reset", &SimulationEpisode::reset, "Reset simulation episode")
        .def("getTotalNeurons", &SimulationEpisode::getTotalNeurons, "Get total neuron count")
        .def("getTotalSynapses", &SimulationEpisode::getTotalSynapses, "Get total synapse count")
        .def("getFiringNeurons", &SimulationEpisode::getFiringNeurons, "Get firing neuron count")
        .def("getAverageFiringRate", &SimulationEpisode::getAverageFiringRate, "Get average firing rate")
        .def("getTotalSpikes", &SimulationEpisode::getTotalSpikes, "Get total spike count")
        .def("getDevelopmentalStage", &SimulationEpisode::getDevelopmentalStage, "Get developmental stage")
        .def("getWorldWidth", &SimulationEpisode::getWorldWidth, "Get world width")
        .def("getWorldHeight", &SimulationEpisode::getWorldHeight, "Get world height")
        .def("getSimulationTime", &SimulationEpisode::getSimulationTime, "Get simulation time")
        .def("getVisionWidth", &SimulationEpisode::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SimulationEpisode::getVisionHeight, "Get vision height")
        .def("getCuriosityLevel", &SimulationEpisode::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &SimulationEpisode::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &SimulationEpisode::getPredictionError, "Get prediction error")
        .def("getNeuromodulationLevel", &SimulationEpisode::getNeuromodulationLevel, "Get neuromodulation level")
        .def("applyRewardModulation", &SimulationEpisode::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward") = 0.0f,
             "Apply reward modulation")
        .def("updateDevelopment", &SimulationEpisode::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("saveState", &SimulationEpisode::saveState, py::arg("filepath"),
             "Save brain state to file")
        .def("loadState", &SimulationEpisode::loadState, py::arg("filepath"),
             "Load brain state from file")
        .def("getPerformanceStats", &SimulationEpisode::getPerformanceStats,
             "Get performance statistics")
        .def("getPerceptionSnapshot", &SimulationEpisode::getPerceptionSnapshot,
             "Get sensory perception snapshot")
        .def("__enter__", [](SimulationEpisode& self) { return &self; })
        .def("__exit__", [](SimulationEpisode& self, py::args) {
            try {
                self.reset();
            } catch (...) {
                // Ignore exceptions in cleanup
            }
        });

    // Batch simulation manager
    py::class_<SimulationBatch>(m, "SimulationBatch", 
        R"pbdoc(A batch simulation manager for running multiple experiments efficiently.)pbdoc")
        .def(py::init<>())
        .def("addEpisode", &SimulationBatch::addEpisode, py::arg("name"), py::arg("config"),
             "Add a new episode to the batch")
        .def("setSteps", [](SimulationBatch& self, size_t steps, size_t index) {
            self.setSteps(steps, index);
        }, py::arg("steps"), py::arg("index") = std::numeric_limits<size_t>::max(),
             "Set steps for episodes (all or specific index)")
        .def("setMaxTime", [](SimulationBatch& self, double maxTime, size_t index) {
            self.setMaxTime(maxTime, index);
        }, py::arg("max_time"), py::arg("index") = std::numeric_limits<size_t>::max(),
             "Set max time for episodes (all or specific index)")
        .def("setCondition", [](SimulationBatch& self, std::function<bool()> condition, size_t index) {
            self.setCondition(condition, index);
        }, py::arg("condition"), py::arg("index") = std::numeric_limits<size_t>::max(),
             "Set condition for episodes (all or specific index)")
        .def("setVerbose", &SimulationBatch::setVerbose, py::arg("verbose"),
             "Set verbose output mode")
        .def("setProgressCallback", &SimulationBatch::setProgressCallback, py::arg("callback"),
             "Set progress callback function")
        .def("run", &SimulationBatch::run,
             "Run all episodes in the batch")
        .def("getEpisodeCount", &SimulationBatch::getEpisodeCount, "Get number of episodes")
        .def("getCurrentEpisode", &SimulationBatch::getCurrentEpisode, "Get current episode index")
        .def("getResults", &SimulationBatch::getResults, "Get batch results")
        .def("analyzeResults", &SimulationBatch::analyzeResults, "Analyze batch results")
        .def("saveResults", &SimulationBatch::saveResults, py::arg("filepath"),
             "Save batch results to file")
        .def("getBestEpisodes", &SimulationBatch::getBestEpisodes, py::arg("count") = 1,
             "Get indices of best episodes")
        .def("clear", &SimulationBatch::clear, "Clear batch data");

    // Experiment utility functions
    m.def("runLearningExperiment", &ExperimentUtils::runLearningExperiment,
          py::arg("name"), py::arg("steps"), py::arg("config") = nullptr,
          "Run a learning experiment (utility function)");

    m.def("runExplorationExperiment", &ExperimentUtils::runExplorationExperiment,
          py::arg("name"), py::arg("steps"), py::arg("explorationFactor"), py::arg("config") = nullptr,
          "Run an exploration experiment (utility function)");

    m.def("runDevelopmentalExperiment", &ExperimentUtils::runDevelopmentalExperiment,
          py::arg("name"), py::arg("steps"), py::arg("enableStructuralPlasticity"), py::arg("config") = nullptr,
          "Run a developmental experiment (utility function)");

    m.def("runBatchExperiment", &ExperimentUtils::runBatchExperiment,
          py::arg("base_name"), py::arg("step_counts"), py::arg("configs") = std::vector<std::string>(),
          "Run a batch of experiments (utility function)");

    // Constants and utility functions
    m.def("createConfigWithPreset", &createConfigWithPreset, py::arg("preset"),
          "Create configuration with predefined presets (small, medium, large, research)");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and info
    m.attr("__version__") = "0.1.0-enhanced";
}

} // namespace nlm
