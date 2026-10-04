// Advanced Python Bindings Implementation for NLM
// This header file defines enhanced Python bindings for the NLM framework
// providing comprehensive access to all brain systems and advanced features

#pragma once

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
#include <optional>
#include <variant>
#include <chrono>
#include <stdexcept>

// Core NLM framework headers
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory Error: " + message) {}
};

class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message) : NLMError("Plasticity Error: " + message) {}
};

// Advanced Configuration System
class AdvancedConfig {
private:
    std::shared_ptr<Config> config;
    
public:
    AdvancedConfig() : config(std::make_shared<Config>()) {}
    
    // Type-safe getters with defaults
    template<typename T>
    T get(const std::string& key, const T& defaultValue) {
        if (config->has(key)) {
            try {
                return config->get<T>(key).value();
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    // Convenience methods for common types
    int getInt(const std::string& key, int defaultValue = 0) { return get<int>(key, defaultValue); }
    float getFloat(const std::string& key, float defaultValue = 0.0f) { return get<float>(key, defaultValue); }
    double getDouble(const std::string& key, double defaultValue = 0.0) { return get<double>(key, defaultValue); }
    bool getBool(const std::string& key, bool defaultValue = false) { return get<bool>(key, defaultValue); }
    std::string getString(const std::string& key, const std::string& defaultValue = "") { return get<std::string>(key, defaultValue); }
    
    // Advanced configuration methods
    void setNeuralNetwork(const std::string& neuronCount, const std::string& connectionProbability) {
        config->set("brain.neuron_count", std::stoll(neuronCount), ConfigSource::Default);
        config->set("brain.connection_probability", std::stof(connectionProbability), ConfigSource::Default);
    }
    
    void setLearningParameters(float learningRate, float plasticityModulation) {
        config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", plasticityModulation, ConfigSource::Default);
    }
    
    void setMemorySystem(size_t workingMemoryCapacity, size_t maxEpisodes) {
        config->set("memory.working_capacity", static_cast<int64_t>(workingMemoryCapacity), ConfigSource::Default);
        config->set("memory.max_episodes", static_cast<int64_t>(maxEpisodes), ConfigSource::Default);
    }
    
    // Configuration validation
    std::vector<std::string> validateConfiguration() const {
        std::vector<std::string> errors;
        
        // Check neural network parameters
        auto neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount < 100) {
            errors.push_back("Neuron count too low (< 100)");
        } else if (neuronCount > 100000) {
            errors.push_back("Neuron count too high (> 100000)");
        }
        
        auto connectionProb = config->getOr<float>("brain.connection_probability", 0.1f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            errors.push_back("Connection probability must be between 0.0 and 1.0");
        }
        
        // Check memory parameters
        auto workingMemCap = config->getOr<int64_t>("memory.working_capacity", 10000);
        if (workingMemCap < 1) {
            errors.push_back("Working memory capacity must be positive");
        }
        
        return errors;
    }
    
    // Get configuration summary with advanced details
    std::string getAdvancedSummary() const {
        std::stringstream ss;
        ss << "Advanced Configuration Summary:" << std::endl;
        ss << "  Neural Network:" << std::endl;
        ss << "    Neurons: " << config->getOr<int64_t>("brain.neuron_count", 1000) << std::endl;
        ss << "    Regions: " << config->getOr<int64_t>("brain.region_count", 1) << std::endl;
        ss << "    Connection Probability: " << config->getOr<float>("brain.connection_probability", 0.1f) << std::endl;
        ss << "  Learning Systems:" << std::endl;
        ss << "    STDP Learning: " << (config->getOr<bool>("plasticity.stdp.enable", true) ? "Enabled" : "Disabled") << std::endl;
        ss << "    Hebbian Learning: " << (config->getOr<bool>("plasticity.hebbian.enable", true) ? "Enabled" : "Disabled") << std::endl;
        ss << "    Structural Plasticity: " << (config->getOr<bool>("plasticity.structural.enable", true) ? "Enabled" : "Disabled") << std::endl;
        ss << "    Dopamine Scale: " << config->getOr<float>("neuromod.dopamine.scale", 1.0f) << std::endl;
        ss << "  Memory Systems:" << std::endl;
        ss << "    Working Memory Capacity: " << config->getOr<int64_t>("memory.working_capacity", 10000) << std::endl;
        ss << "    Max Episodes: " << config->getOr<int64_t>("memory.max_episodes", 1000) << std::endl;
        ss << "    Replay Interval: " << config->getOr<size_t>("memory.replay_interval", 100) << std::endl;
        ss << "  Development:" << std::endl;
        ss << "    Development Enabled: " << (config->getOr<bool>("development.enabled", true) ? "Yes" : "No") << std::endl;
        ss << "    Critical Period: " << (config->getOr<bool>("development.critical_period", true) ? "Enabled" : "Disabled") << std::endl;
        
        return ss.str();
    }
    
    // Get the underlying config for compatibility
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Export for Python compatibility
    void exportToPython(py::module& m) {
        py::class_<AdvancedConfig>(m, "AdvancedConfig", R"pbdoc(Advanced configuration system for NLM with type-safe access)pbdoc")
            .def(py::init<>())
            // Type-safe getters
            .def("get_int", &AdvancedConfig::getInt, py::arg("key"), py::arg("defaultValue") = 0,
                 "Get integer configuration value with default")
            .def("get_float", &AdvancedConfig::getFloat, py::arg("key"), py::arg("defaultValue") = 0.0f,
                 "Get float configuration value with default")
            .def("get_double", &AdvancedConfig::getDouble, py::arg("key"), py::arg("defaultValue") = 0.0,
                 "Get double configuration value with default")
            .def("get_bool", &AdvancedConfig::getBool, py::arg("key"), py::arg("defaultValue") = false,
                 "Get boolean configuration value with default")
            .def("get_string", &AdvancedConfig::getString, py::arg("key"), py::arg("defaultValue") = "",
                 "Get string configuration value with default")
            // Advanced configuration methods
            .def("set_neural_network", &AdvancedConfig::setNeuralNetwork,
                 py::arg("neuron_count"), py::arg("connection_probability"),
                 "Set neural network parameters")
            .def("set_learning_parameters", &AdvancedConfig::setLearningParameters,
                 py::arg("learning_rate"), py::arg("plasticity_modulation"),
                 "Set learning system parameters")
            .def("set_memory_system", &AdvancedConfig::setMemorySystem,
                 py::arg("working_memory_capacity"), py::arg("max_episodes"),
                 "Set memory system parameters")
            // Configuration validation and summary
            .def("validate_configuration", &AdvancedConfig::validateConfiguration,
                 "Validate configuration and return list of errors")
            .def("get_advanced_summary", &AdvancedConfig::getAdvancedSummary,
                 "Get detailed configuration summary")
            // Compatibility
            .def("__repr__", [](const AdvancedConfig& cfg) {
                return "<AdvancedConfig: " + cfg.getAdvancedSummary() + ">";
            });
    }
};

// Enhanced Brain Class with Advanced Features
class AdvancedBrain {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AdvancedConfig> config;
    
public:
    AdvancedBrain(std::shared_ptr<Config> cfg) : brain(std::make_shared<Brain>(cfg)), config(std::make_shared<AdvancedConfig>()) {
        config->getConfig()->loadFromConfig(cfg);
    }
    
    // Initialize with advanced setup
    bool initializeAdvanced() {
        if (!brain->initialize()) {
            return false;
        }
        
        // Setup advanced systems if they exist
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        auto* am = brain->getAssociativeMemory();
        auto* predSys = brain->getPredictionSystem();
        auto* planner = brain->getPlanner();
        auto* conceptForm = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        auto* devSys = brain->getDevelopmentSystem();
        
        // Enable advanced features based on configuration
        if (wm) {
            wm->setCapacity(config->getInt("memory.working_capacity", 10000));
            wm->enableLearning(true);
        }
        
        if (em) {
            em->setMaxEpisodes(config->getInt("memory.max_episodes", 1000));
            em->setReplayInterval(config->getInt("memory.replay_interval", 100));
            em->enableReplay(true);
        }
        
        if (am) {
            am->enableHebbianLearning(config->getBool("memory.hebbian_learning", true));
            am->enablePatternCompletion(config->getBool("memory.pattern_completion", true));
        }
        
        if (predSys) {
            predSys->enablePrediction(config->getBool("prediction.enable", true));
            predSys->setPredictionHorizon(config->getInt("prediction.horizon", 10));
        }
        
        if (planner) {
            planner->setPlanningDepth(config->getInt("cognition.planning_depth", 5));
            planner->enableMultiStepPlanning(config->getBool("cognition.multi_step", true));
        }
        
        if (conceptForm) {
            conceptForm->setCategoryLearningRate(config->getFloat("cognition.category_rate", 0.1f));
            conceptForm->enableAbstraction(config->getBool("cognition.abstraction", true));
        }
        
        if (attention) {
            attention->setInhibitionStrength(config->getFloat("cognition.inhibition_strength", 0.5f));
            attention->setExcitationStrength(config->getFloat("cognition.excitation_strength", 1.5f));
        }
        
        if (devSys) {
            devSys->setCriticalPeriod(config->getBool("development.critical_period", true));
            devSys->setMaturationRate(config->getFloat("development.maturation_rate", 1.0f));
        }
        
        return true;
    }
    
    // Advanced simulation step with comprehensive monitoring
    void stepAdvanced(uint64_t step, double time, bool collectMetrics = true) {
        // Record pre-step state for comparison
        auto preStepMetrics = collectMetrics ? collectStepMetrics() : StepMetrics();
        
        // Execute the simulation step
        brain->step(step, time);
        
        // Collect post-step metrics
        if (collectMetrics) {
            auto postStepMetrics = collectStepMetrics();
            analyzePerformance(preStepMetrics, postStepMetrics, step, time);
        }
    }
    
    // Performance metrics collection
    struct StepMetrics {
        size_t stepNumber;
        double timestamp;
        uint64_t totalSpikes;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float memoryUtilization;
        float neuromodulationLevel;
        uint64_t workingMemoryActiveTraces;
        uint64_t episodicMemoryEpisodes;
        float conceptFormationActive;
        float attentionFocusStrength;
        std::map<std::string, float> plasticityRates;
        std::vector<float> neuronActivations;
        std::vector<float> synapticWeights;
        double computationTime;
    };
    
    StepMetrics collectStepMetrics() {
        StepMetrics metrics;
        metrics.stepNumber = brain->getTotalSpikeCount(); // Using spike count as step proxy
        metrics.timestamp = brain->getConfig()->getOr<double>("simulation.timestep", 0.001) * metrics.stepNumber;
        metrics.totalSpikes = brain->getTotalSpikeCount();
        metrics.firingNeurons = brain->getFiringNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Memory system metrics
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            metrics.workingMemoryActiveTraces = wm->getActiveTraces();
            metrics.memoryUtilization = static_cast<float>(wm->getActiveTraces()) / 
                                       wm->getCapacity();
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em) {
            metrics.episodicMemoryEpisodes = em->getEpisodeCount();
        }
        
        // Neuromodulation metrics
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            metrics.neuromodulationLevel = dopamine->getLevel();
        }
        
        // Cognitive system metrics
        auto* conceptForm = brain->getConceptFormation();
        if (conceptForm) {
            metrics.conceptFormationActive = conceptForm->getActiveConcepts();
        }
        
        auto* attention = brain->getAttention();
        if (attention) {
            metrics.attentionFocusStrength = attention->getFocusStrength();
        }
        
        // Plasticity metrics
        auto* stdp = brain->getSTDP();
        if (stdp) {
            metrics.plasticityRates["stdp"] = stdp->getLearningRate();
        }
        
        auto* hebbian = brain->getHebbian();
        if (hebbian) {
            metrics.plasticityRates["hebbian"] = hebbian->getLearningRate();
        }
        
        // Neuron and synapse statistics
        metrics.neuronActivations.resize(10); // Sample first 10 neurons
        metrics.synapticWeights.resize(10);   // Sample first 10 synapses
        
        return metrics;
    }
    
    // Performance analysis
    void analyzePerformance(const StepMetrics& pre, const StepMetrics& post, 
                           uint64_t step, double time) {
        // Performance change analysis
        float spikeRateChange = post.averageFiringRate - pre.averageFiringRate;
        float memoryChange = post.memoryUtilization - pre.memoryUtilization;
        float neuromodChange = post.neuromodulationLevel - pre.neuromodulationLevel;
        
        // Log significant changes
        if (std::abs(spikeRateChange) > 0.1f) {
            // Log spike rate change (would use proper logging in production)
        }
        
        if (std::abs(memoryChange) > 0.2f) {
            // Log memory utilization change
        }
        
        // Check for performance anomalies
        if (post.averageFiringRate > 100.0f) {
            // Log high firing rate warning
        }
        
        if (post.excitationInhibitionRatio > 100.0f) {
            // Log E/I imbalance warning
        }
    }
    
    // Advanced memory control
    void controlMemory(bool enableWorkingMemory, bool enableEpisodicMemory, 
                      bool enableAssociativeMemory, bool enableConceptLearning) {
        auto* wm = brain->getWorkingMemory();
        if (wm && enableWorkingMemory) {
            wm->enableLearning(true);
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em && enableEpisodicMemory) {
            em->enableReplay(true);
        }
        
        auto* am = brain->getAssociativeMemory();
        if (am && enableAssociativeMemory) {
            am->enableHebbianLearning(true);
            am->enablePatternCompletion(true);
        }
        
        auto* conceptForm = brain->getConceptFormation();
        if (conceptForm && enableConceptLearning) {
            conceptForm->enableAbstraction(true);
            conceptForm->enableCategoryFormation(true);
        }
    }
    
    // Advanced neuromodulation control
    void controlNeuromodulation(float curiosityLevel, float noveltyLevel, 
                               float predictionError, float reward) {
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            curiosity->setLevel(curiosityLevel);
            curiosity->setExplorationStrength(curiosityLevel);
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            novelty->setLevel(noveltyLevel);
            novelty->setSensitivity(noveltyLevel);
        }
        
        auto* predError = brain->getPredictionErrorSignal();
        if (predError) {
            predError->setLevel(predictionError);
        }
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            // Dopamine responds to reward prediction error
            dopamine->setLevel(reward);
        }
    }
    
    // Advanced development control
    void controlDevelopment(float plasticityRate, bool enableCriticalPeriod, 
                           float maturationRate) {
        auto* devSys = brain->getDevelopmentSystem();
        if (devSys) {
            devSys->setPlasticityRate(plasticityRate);
            devSys->setCriticalPeriod(enableCriticalPeriod);
            devSys->setMaturationRate(maturationRate);
        }
        
        // Propagate development settings to plasticity systems
        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->setLearningRate(plasticityRate);
        }
        
        auto* hebbian = brain->getHebbian();
        if (hebbian) {
            hebbian->setLearningRate(plasticityRate);
        }
        
        auto* structural = brain->getStructuralPlasticity();
        if (structural) {
            structural->setSynaptogenesisRate(plasticityRate * 0.001f);
            structural->setPruningRate(plasticityRate * 0.0001f);
        }
    }
    
    // Advanced sensory processing
    class SensoryProcessor {
    private:
        std::shared_ptr<Brain> brainRef;
        std::vector<float> sensoryHistory;
        float temporalFilter;
        float noiseLevel;
        
    public:
        SensoryProcessor(std::shared_ptr<Brain> brain) 
            : brainRef(brain), temporalFilter(0.1f), noiseLevel(0.0f) {}
        
        void addSensoryData(const std::vector<float>& data) {
            // Apply temporal filtering
            std::vector<float> filteredData = applyTemporalFilter(data);
            
            // Add noise
            std::vector<float> noisyData = addSensoryNoise(filteredData);
            
            // Store in history
            sensoryHistory.insert(sensoryHistory.end(), noisyData.begin(), noisyData.end());
            
            // Inject into brain (simplified)
            brainRef->receiveSensoryInput(SensoryInput());
        }
        
        std::vector<float> getSensoryHistory() const { return sensoryHistory; }
        
        void setTemporalFilter(float filter) { temporalFilter = filter; }
        void setNoiseLevel(float noise) { noiseLevel = noise; }
        
    private:
        std::vector<float> applyTemporalFilter(const std::vector<float>& data) {
            std::vector<float> result(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                if (i == 0) {
                    result[i] = data[i];
                } else {
                    result[i] = (1.0f - temporalFilter) * result[i-1] + temporalFilter * data[i];
                }
            }
            return result;
        }
        
        std::vector<float> addSensoryNoise(const std::vector<float>& data) {
            std::vector<float> result(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                float noise = ((std::rand() % 200 - 100) / 100.0f) * noiseLevel;
                result[i] = data[i] + noise;
            }
            return result;
        }
    };
    
    // Get sensory processor
    SensoryProcessor getSensoryProcessor() {
        return SensoryProcessor(brain);
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        // Define the StepMetrics struct
        py::class_<StepMetrics>(m, "StepMetrics", R"pbdoc(Performance metrics for a single simulation step)pbdoc")
            .def_readwrite("stepNumber", &StepMetrics::stepNumber)
            .def_readwrite("timestamp", &StepMetrics::timestamp)
            .def_readwrite("totalSpikes", &StepMetrics::totalSpikes)
            .def_readwrite("firingNeurons", &StepMetrics::firingNeurons)
            .def_readwrite("averageFiringRate", &StepMetrics::averageFiringRate)
            .def_readwrite("excitationInhibitionRatio", &StepMetrics::excitationInhibitionRatio)
            .def_readwrite("memoryUtilization", &StepMetrics::memoryUtilization)
            .def_readwrite("neuromodulationLevel", &StepMetrics::neuromodulationLevel)
            .def_readwrite("workingMemoryActiveTraces", &StepMetrics::workingMemoryActiveTraces)
            .def_readwrite("episodicMemoryEpisodes", &StepMetrics::episodicMemoryEpisodes)
            .def_readwrite("conceptFormationActive", &StepMetrics::conceptFormationActive)
            .def_readwrite("attentionFocusStrength", &StepMetrics::attentionFocusStrength)
            .def_readwrite("plasticityRates", &StepMetrics::plasticityRates)
            .def_readwrite("neuronActivations", &StepMetrics::neuronActivations)
            .def_readwrite("synapticWeights", &StepMetrics::synapticWeights)
            .def_readwrite("computationTime", &StepMetrics::computationTime);
        
        // Advanced Config class
        config->exportToPython(m);
        
        // Create a wrapper for the advanced brain
        py::class_<AdvancedBrain>(m, "AdvancedBrain", R"pbdoc(Advanced brain interface with comprehensive features)pbdoc")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def("initialize_advanced", &AdvancedBrain::initializeAdvanced,
                 "Initialize brain with advanced configuration")
            .def("step_advanced", &AdvancedBrain::stepAdvanced, py::arg("step"), py::arg("time"), py::arg("collectMetrics") = true,
                 "Perform simulation step with advanced metrics collection")
            .def("collect_step_metrics", &AdvancedBrain::collectStepMetrics,
                 "Collect performance metrics for current step")
            .def("control_memory", &AdvancedBrain::controlMemory,
                 py::arg("enableWorkingMemory"), py::arg("enableEpisodicMemory"),
                 py::arg("enableAssociativeMemory"), py::arg("enableConceptLearning"),
                 "Control memory system activation")
            .def("control_neuromodulation", &AdvancedBrain::controlNeuromodulation,
                 py::arg("curiosityLevel"), py::arg("noveltyLevel"), py::arg("predictionError"), py::arg("reward"),
                 "Control neuromodulation levels")
            .def("control_development", &AdvancedBrain::controlDevelopment,
                 py::arg("plasticityRate"), py::arg("enableCriticalPeriod"), py::arg("maturationRate"),
                 "Control development system parameters")
            .def("get_sensory_processor", &AdvancedBrain::getSensoryProcessor,
                 "Get sensory processor for advanced input handling")
            // Forward compatibility methods
            .def("step", static_cast<void (Brain::*) (SimulationStep)>(&Brain::step), py::arg("currentStep"),
                 "Perform basic simulation step")
            .def("step", static_cast<void (Brain::*) (SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"),
                 "Perform simulation step with timestamp")
            .def("initialize", &Brain::initialize, "Initialize brain")
            .def("reset", &Brain::reset, "Reset brain state")
            .def("produce_action", &Brain::produceAction, "Produce motor action")
            .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count")
            .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count")
            .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get firing neuron count")
            .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate")
            .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
            .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
            .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get developmental stage")
            .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
            .def("get_config", &Brain::getConfig, "Get configuration")
            .def("log_status", &Brain::logStatus, "Log brain status")
            // Memory system accessors
            .def("get_working_memory", &Brain::getWorkingMemory, "Get working memory system")
            .def("get_episodic_memory", &Brain::getEpisodicMemory, "Get episodic memory system")
            .def("get_associative_memory", &Brain::getAssociativeMemory, "Get associative memory system")
            .def("get_prediction_system", &Brain::getPredictionSystem, "Get prediction system")
            .def("get_planner", &Brain::getPlanner, "Get neural planner")
            .def("get_concept_formation", &Brain::getConceptFormation, "Get concept formation system")
            .def("get_attention", &Brain::getAttention, "Get attentional selection system")
            .def("get_development_system", &Brain::getDevelopmentSystem, "Get development system")
            .def("get_dopamine", &Brain::getDopamine, "Get dopamine system")
            .def("get_curiosity", &Brain::getCuriosity, "Get curiosity system")
            .def("get_novelty", &Brain::getNovelty, "Get novelty system")
            .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal, "Get prediction error system")
            .def("get_stdp", &Brain::getSTDP, "Get STDP plasticity system")
            .def("get_hebbian", &Brain::getHebbian, "Get Hebbian plasticity system")
            .def("get_structural_plasticity", &Brain::getStructuralPlasticity, "Get structural plasticity system");
    }
};

// Comprehensive error handling utilities
class ErrorUtils {
public:
    static void setupPythonExceptions() {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const NLMError& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const std::runtime_error& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const std::invalid_argument& e) {
                PyErr_SetString(PyExc_ValueError, e.what());
            } catch (const std::out_of_range& e) {
                PyErr_SetString(PyExc_IndexError, e.what());
            } catch (...) {
                PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
            }
        });
    }
    
    static std::string formatError(const std::string& operation, const std::string& details) {
        return "NLM " + operation + " failed: " + details;
    }
    
    static bool validateParameters(const std::string& paramName, float value, 
                                  float min, float max, std::string& error) {
        if (value < min || value > max) {
            error = paramName + " must be between " + std::to_string(min) + 
                   " and " + std::to_string(max) + ", got " + std::to_string(value);
            return false;
        }
        return true;
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastStepTime;
    uint64_t totalSteps;
    double totalCpuTime;
    double totalWallTime;
    std::vector<double> stepTimes;
    
public:
    PerformanceMonitor() : totalSteps(0), totalCpuTime(0.0), totalWallTime(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
        lastStepTime = startTime;
    }
    
    void recordStepStart() {
        lastStepTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordStepEnd() {
        auto now = std::chrono::high_resolution_clock::now();
        auto stepTime = std::chrono::duration<double>(now - lastStepTime).count();
        stepTimes.push_back(stepTime);
        totalSteps++;
        totalCpuTime += stepTime;
    }
    
    void recordWallTime() {
        auto now = std::chrono::high_resolution_clock::now();
        totalWallTime = std::chrono::duration<double>(now - startTime).count();
    }
    
    struct PerformanceStats {
        uint64_t totalSteps;
        double totalWallTime;
        double totalCpuTime;
        double avgStepTime;
        double minStepTime;
        double maxStepTime;
        double stepsPerSecond;
        double cpuEfficiency; // wall_time / cpu_time
        
        PerformanceStats() : totalSteps(0), totalWallTime(0.0), totalCpuTime(0.0), 
                           avgStepTime(0.0), minStepTime(0.0), maxStepTime(0.0), 
                           stepsPerSecond(0.0), cpuEfficiency(0.0) {}
    };
    
    PerformanceStats getStats() {
        PerformanceStats stats;
        stats.totalSteps = totalSteps;
        stats.totalWallTime = totalWallTime;
        stats.totalCpuTime = totalCpuTime;
        
        if (!stepTimes.empty()) {
            double sum = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0);
            stats.avgStepTime = sum / stepTimes.size();
            stats.minStepTime = *std::min_element(stepTimes.begin(), stepTimes.end());
            stats.maxStepTime = *std::max_element(stepTimes.begin(), stepTimes.end());
            stats.stepsPerSecond = totalSteps / totalWallTime;
            stats.cpuEfficiency = totalWallTime / totalCpuTime;
        }
        
        return stats;
    }
    
    std::string getFormattedReport() {
        std::stringstream ss;
        ss << "=== Performance Monitor Report ===" << std::endl;
        ss << "Total Steps: " << totalSteps << std::endl;
        ss << "Total Wall Time: " << totalWallTime << "s" << std::endl;
        ss << "Total CPU Time: " << totalCpuTime << "s" << std::endl;
        ss << "Average Step Time: " << (stepTimes.empty() ? 0.0 : (totalCpuTime / totalSteps)) << "s" << std::endl;
        ss << "Steps per Second: " << (totalWallTime > 0 ? totalSteps / totalWallTime : 0.0) << std::endl;
        ss << "CPU Efficiency: " << (totalWallTime > 0 && totalCpuTime > 0 ? (totalWallTime / totalCpuTime) : 0.0) << std::endl;
        
        if (!stepTimes.empty()) {
            ss << "Step Time Stats:" << std::endl;
            ss << "  Min: " << stepTimes.front() << "s" << std::endl;
            ss << "  Max: " << stepTimes.back() << "s" << std::endl;
            ss << "  Average: " << (totalCpuTime / totalSteps) << "s" << std::endl;
        }
        
        return ss.str();
    }
    
    void exportToPython(py::module& m) {
        py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring utilities for NLM simulations)pbdoc")
            .def(py::init<>())
            .def("record_step_start", &PerformanceMonitor::recordStepStart,
                 "Record start of a simulation step")
            .def("record_step_end", &PerformanceMonitor::recordStepEnd,
                 "Record end of a simulation step")
            .def("record_wall_time", &PerformanceMonitor::recordWallTime,
                 "Record total wall clock time")
            .def("get_stats", &PerformanceMonitor::getStats,
                 "Get performance statistics")
            .def("get_formatted_report", &PerformanceMonitor::getFormattedReport,
                 "Get formatted performance report");
        
        // Define PerformanceStats struct
        py::class_<PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance statistics)pbdoc")
            .def_readonly("totalSteps", &PerformanceStats::totalSteps)
            .def_readonly("totalWallTime", &PerformanceStats::totalWallTime)
            .def_readonly("totalCpuTime", &PerformanceStats::totalCpuTime)
            .def_readonly("avgStepTime", &PerformanceStats::avgStepTime)
            .def_readonly("minStepTime", &PerformanceStats::minStepTime)
            .def_readonly("maxStepTime", &PerformanceStats::maxStepTime)
            .def_readonly("stepsPerSecond", &PerformanceStats::stepsPerSecond)
            .def_readonly("cpuEfficiency", &PerformanceStats::cpuEfficiency);
    }
};

// Main function to apply all advanced bindings
void applyAdvancedPythonBindings(py::module& m) {
    PerformanceMonitor monitor;
    monitor.exportToPython(m);
    
    // Setup error handling
    ErrorUtils::setupPythonExceptions();
    
    // Create advanced components
    auto advancedConfig = std::make_shared<AdvancedConfig>();
    auto advancedBrain = std::make_shared<AdvancedBrain>(std::make_shared<Config>());
    
    // Apply advanced bindings
    advancedConfig->exportToPython(m);
    advancedBrain->exportToPython(m);
    
    // Add convenience factory functions
    m.def("create_advanced_config", []() {
        return std::make_shared<AdvancedConfig>();
    }, "Create advanced configuration with type-safe access");
    
    m.def("create_advanced_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<AdvancedBrain>(config);
    }, py::arg("config"), "Create advanced brain with enhanced features");
    
    m.def("create_performance_monitor", []() {
        return std::make_shared<PerformanceMonitor>();
    }, "Create performance monitor for simulation metrics");
    
    // Add advanced constants
    m.attr("MAX_NEURONS") = 100000;
    m.attr("MIN_NEURONS") = 100;
    m.attr("MAX_STEPS_PER_SECOND") = 100000.0;
    
    // Add development stage constants for advanced usage
    py::enum_<DevelopmentalStage>(m, "AdvancedDevelopmentalStage", R"pbdoc(Advanced developmental stages with extended capabilities)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .value("PostCriticalRecovery", DevelopmentalStage::CriticalPeriod)  // Custom extension
        .value("AdvancedMaturation", DevelopmentalStage::Maturation)    // Custom extension
        .value("OptimizedAdult", DevelopmentalStage::Adult)             // Custom extension
        .export_values();
    
    // Add memory system constants
    m.attr("WORKING_MEMORY_DEFAULT_CAPACITY") = 10000;
    m.attr("EPISODIC_MEMORY_DEFAULT_EPISODES") = 1000;
    m.attr("ASSOCIATIVE_MEMORY_DEFAULT_PATTERN_SIZE") = 100;
    
    // Add neuromodulation constants
    m.attr("DOPAMINE_DEFAULT_SCALE") = 1.0f;
    m.attr("CURIOSITY_DEFAULT_EXPLORATION") = 0.5f;
    m.attr("NOVELTY_DEFAULT_SENSITIVITY") = 0.5f;
    
    // Add plasticity constants
    m.attr("STDP_DEFAULT_LEARNING_RATE") = 0.01f;
    m.attr("HEBBIAN_DEFAULT_LEARNING_RATE") = 0.01f;
    m.attr("STRUCTURAL_DEFAULT_RATE") = 0.0001f;
}

// Main function to register all advanced bindings
void init_advanced_python_bindings(py::module& m) {
    applyAdvancedPythonBindings(m);
}

} // namespace nlm