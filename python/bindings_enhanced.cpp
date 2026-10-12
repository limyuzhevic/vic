#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Performance profiler for timing and resource monitoring
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double totalTimeMs = 0.0;
        double cpuTimeMs = 0.0;
        size_t memoryUsageBytes = 0;
        size_t spikeCount = 0;
        float firingRate = 0.0f;
        std::vector<double> timestepTimes;
    };

    PerformanceProfiler() : startTime_(std::chrono::high_resolution_clock::now()) {}

    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        lastStepTime_ = startTime_;
    }

    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime_);
        metrics_.totalTimeMs = duration.count();
    }

    void step() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timestepDuration = std::chrono::duration<double, std::milli>(currentTime - lastStepTime_);
        metrics_.timestepTimes.push_back(timestepDuration.count());
        metrics_.cpuTimeMs += timestepDuration.count();
        lastStepTime_ = currentTime;
    }

    PerformanceMetrics getMetrics() const { return metrics_; }

    double getAverageStepTime() const {
        if (metrics_.timestepTimes.empty()) return 0.0;
        double sum = std::accumulate(metrics_.timestepTimes.begin(), metrics_.timestepTimes.end(), 0.0);
        return sum / metrics_.timestepTimes.size();
    }

    double getMinStepTime() const {
        if (metrics_.timestepTimes.empty()) return 0.0;
        return *std::min_element(metrics_.timestepTimes.begin(), metrics_.timestepTimes.end());
    }

    double getMaxStepTime() const {
        if (metrics_.timestepTimes.empty()) return 0.0;
        return *std::max_element(metrics_.timestepTimes.begin(), metrics_.timestepTimes.end());
    }

    void reset() {
        metrics_ = PerformanceMetrics();
        startTime_ = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    PerformanceMetrics metrics_;
};

// Advanced convenience functions for research
class ResearchUtilities {
public:
    // Batch processing for running multiple experiments
    static std::vector<Phase6IntegrationResult> runBatchExperiments(
        const std::vector<Phase6Config>& configs,
        const std::function<bool(const Phase6Config&)>& filter = nullptr) {
        std::vector<Phase6IntegrationResult> results;
        for (const auto& config : configs) {
            if (filter && !filter(config)) continue;
            Phase6IntegratedExperiment experiment;
            results.push_back(experiment.run(config));
        }
        return results;
    }

    // Statistical analysis utilities
    static std::map<std::string, double> computeStatistics(const std::vector<double>& data) {
        std::map<std::string, double> stats;
        if (data.empty()) return stats;

        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();

        double sq_sum = 0.0;
        double min_val = data[0];
        double max_val = data[0];
        for (double val : data) {
            sq_sum += val * val;
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }

        double variance = sq_sum / data.size() - mean * mean;
        double stddev = std::sqrt(variance);

        stats["mean"] = mean;
        stats["stddev"] = stddev;
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["variance"] = variance;
        return stats;
    }

    // Memory usage tracking
    static size_t getMemoryUsage() {
        // Simple approximation - in production use platform-specific APIs
        return 0;
    }

    // Data recording for analysis
    template<typename T>
    static void recordExperiment(const std::string& filename, const T& data) {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (file.is_open()) {
            // Simple serialization - use proper format in production
            file.write(reinterpret_cast<const char*>(&data), sizeof(T));
        }
    }

    // Neural pattern analysis
    static std::vector<float> analyzePatternSimilarity(
        const std::vector<float>& pattern1,
        const std::vector<float>& pattern2) {
        std::vector<float> similarities;
        if (pattern1.size() != pattern2.size()) return similarities;

        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;

        for (size_t i = 0; i < pattern1.size(); ++i) {
            dotProduct += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }

        float similarity = dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
        return {similarity, dotProduct, norm1, norm2};
    }
};

// Enhanced configuration system with validation
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}

    // Validation system
    bool validate() const {
        bool valid = true;

        // Check required parameters
        if (!config_->has("brain.neuron_count")) {
            NLM_LOG_WARNING("Missing brain.neuron_count");
            valid = false;
        }

        // Validate parameter ranges
        auto neuronCount = config_->getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount < 100) {
            NLM_LOG_WARNING("brain.neuron_count should be at least 100");
            valid = false;
        } else if (neuronCount > 100000) {
            NLM_LOG_WARNING("brain.neuron_count should not exceed 100000");
            valid = false;
        }

        // Validate plasticity parameters
        auto stdpLearningRate = config_->getOr<float>("plasticity.stdp.learning_rate", 0.001f);
        if (stdpLearningRate < 0.0f || stdpLearningRate > 1.0f) {
            NLM_LOG_WARNING("plasticity.stdp.learning_rate should be between 0.0 and 1.0");
            valid = false;
        }

        return valid;
    }

    // Schema documentation
    std::string getSchema() const {
        std::stringstream ss;
        ss << "=== NLM Configuration Schema ===\n\n";

        ss << "=== BRAIN CONFIGURATION ===\n";
        ss << "brain.neuron_count (int): Total number of neurons [100, 100000]\n";
        ss << "brain.synapse_density (float): Synapse connectivity probability [0.0, 1.0]\n";
        ss << "brain.connection_probability (float): Connection probability [0.0, 1.0]\n";
        ss << "brain.v_thresh (float): Neuron threshold potential (mV)\n";
        ss << "brain.v_rest (float): Resting potential (mV)\n";
        ss << "brain.v_reset (float): Reset potential after spike (mV)\n\n";

        ss << "=== PLASTICITY CONFIGURATION ===\n";
        ss << "plasticity.stdp.enable (bool): Enable STDP plasticity\n";
        ss << "plasticity.stdp.learning_rate (float): STDP learning rate [0.0, 1.0]\n";
        ss << "plasticity.stdp.tau_plus (float): STDP LTP time constant (ms)\n";
        ss << "plasticity.stdp.tau_minus (float): STDP LTD time constant (ms)\n";
        ss << "plasticity.hebbian.enable (bool): Enable Hebbian learning\n";
        ss << "plasticity.structural.enable (bool): Enable structural plasticity\n\n";

        ss << "=== NEUROMODULATION CONFIGURATION ===\n";
        ss << "neuromod.dopamine.scale (float): Dopamine modulation scale\n";
        ss << "neuromod.curiosity.enable (bool): Enable curiosity-driven exploration\n";
        ss << "neuromod.novelty.enable (bool): Enable novelty detection\n\n";

        return ss.str();
    }

    // Type-safe parameter access
    int64_t getNeuronCount() const { return config_->getOr<int64_t>("brain.neuron_count", 1000); }
    void setNeuronCount(int64_t count) { config_->set("brain.neuron_count", count); }

    float getConnectionProbability() const { return config_->getOr<float>("brain.connection_probability", 0.05f); }
    void setConnectionProbability(float prob) { config_->set("brain.connection_probability", prob); }

    bool getSTDPEnabled() const { return config_->getOr<bool>("plasticity.stdp.enable", true); }
    void setSTDPEnabled(bool enable) { config_->set("plasticity.stdp.enable", enable); }

    float getSTDPLearningRate() const { return config_->getOr<float>("plasticity.stdp.learning_rate", 0.001f); }
    void setSTDPLearningRate(float rate) { config_->set("plasticity.stdp.learning_rate", rate); }

    // Configuration manager methods
    std::shared_ptr<Config> getInternalConfig() { return config_; }
    void loadFromFile(const std::string& filepath) { config_->loadFromFile(filepath); }
    void saveToFile(const std::string& filepath) { config_->saveToFile(filepath); }

private:
    std::shared_ptr<Config> config_;
};

// Advanced brain operations for research
class AdvancedBrain {
public:
    // Neural pattern manipulation
    static void injectPattern(std::shared_ptr<Brain> brain,
                               const std::vector<float>& pattern,
                               NeuronType type = NeuronType::Internal) {
        size_t neuronCount = brain->getTotalNeuronCount();
        for (size_t i = 0; i < pattern.size() && i < neuronCount; ++i) {
            if (pattern[i] > 0.0f) {
                // Find appropriate neuron and inject current
                auto neurons = brain->getRegion(RegionId(0))->getAllNeurons();
                if (i < neurons.size()) {
                    neurons[i]->injectCurrent(pattern[i] * 100.0f);
                }
            }
        }
    }

    // Statistical analysis of neural activity
    static std::map<std::string, double> analyzeNeuralActivity(std::shared_ptr<Brain> brain) {
        std::map<std::string, double> analysis;

        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        analysis["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();

        auto region = brain->getRegion(RegionId(0));
        if (region) {
            size_t totalNeurons = region->getAllNeurons().size();
            analysis["region_neuron_count"] = static_cast<double>(totalNeurons);
        }

        return analysis;
    }

    // Memory research utilities
    static void storeExperienceForMemory(std::shared_ptr<Brain> brain,
                                          const std::vector<float>& sensoryPattern,
                                          const std::vector<float>& actionPattern,
                                          float reward) {
        // Store sensory pattern in working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory && !sensoryPattern.empty()) {
            workingMemory->store(NeuronId(0), *std::min_element(sensoryPattern.begin(), sensoryPattern.end()));
        }

        // Store in episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            EpisodicMemoryItem item;
            item.timestamp = SimulationStep(0);
            item.neurons = {NeuronId(0), NeuronId(1)};
            item.values = sensoryPattern;
            item.metadata = "reward=" + std::to_string(reward);
            episodicMemory->storeEpisode(item);
        }
    }

    // Development research
    static void advanceDevelopmentStage(std::shared_ptr<Brain> brain,
                                        DevelopmentalStage newStage) {
        brain->setDevelopmentalStage(newStage);

        // Update related systems based on development stage
        if (newStage >= DevelopmentalStage::Maturation) {
            // Enable higher-level cognitive functions
            // This would involve enabling planning, concept formation, etc.
        }
    }

    // Neuromodulation research
    static void conductNeuromodulationExperiment(std::shared_ptr<Brain> brain,
                                                   const std::string& experimentType,
                                                   float parameter) {
        if (experimentType == "dopamine_reward") {
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                dopamine->signalReward(parameter);
            }
        } else if (experimentType == "curiosity_drive") {
            auto* curiosity = brain->getCuriosity();
            if (curiosity) {
                curiosity->setExplorationRate(parameter);
            }
        } else if (experimentType == "novelty_detection") {
            auto* novelty = brain->getNovelty();
            if (novelty) {
                novelty->setSensitivity(parameter);
            }
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides advanced research APIs for neuroscience and artificial intelligence.

        This enhanced version includes:
        - Advanced neuromodulation system APIs
        - Memory systems direct access
        - Prediction system for predictive coding research
        - Cognition and development system APIs
        - Performance profiling and analysis tools
        - Configuration validation and schema system
        - Research utilities for batch experiments and statistical analysis
        - High-level convenience functions for common research tasks
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic types and enums (unchanged from original)
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

    // Enhanced configuration system
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration system with validation and schema)")
        .def(py::init<>())
        .def("validate", &EnhancedConfig::validate,
             "Validate configuration parameters and ranges")
        .def("getSchema", &EnhancedConfig::getSchema,
             "Get configuration schema documentation")
        .def("getNeuronCount", &EnhancedConfig::getNeuronCount,
             "Get neuron count")
        .def("setNeuronCount", &EnhancedConfig::setNeuronCount,
             py::arg("count"), "Set neuron count")
        .def("getConnectionProbability", &EnhancedConfig::getConnectionProbability,
             "Get connection probability")
        .def("setConnectionProbability", &EnhancedConfig::setConnectionProbability,
             py::arg("prob"), "Set connection probability")
        .def("getSTDPEnabled", &EnhancedConfig::getSTDPEnabled,
             "Get STDP enabled status")
        .def("setSTDPEnabled", &EnhancedConfig::setSTDPEnabled,
             py::arg("enable"), "Set STDP enabled status")
        .def("getSTDPLearningRate", &EnhancedConfig::getSTDPLearningRate,
             "Get STDP learning rate")
        .def("setSTDPLearningRate", &EnhancedConfig::setSTDPLearningRate,
             py::arg("rate"), "Set STDP learning rate")
        .def("loadFromFile", &EnhancedConfig::loadFromFile,
             py::arg("filepath"), "Load configuration from file")
        .def("saveToFile", &EnhancedConfig::saveToFile,
             py::arg("filepath"), "Save configuration to file");

    // Enhanced configuration factory
    m.def("createEnhancedConfig", []() -> std::shared_ptr<EnhancedConfig> {
        return std::make_shared<EnhancedConfig>();
    }, "Create an enhanced configuration object");

    // Original Config class (unchanged)
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Original brain class with enhanced methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
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
        // Enhanced brain methods for research
        .def("analyzeNeuralActivity", &AdvancedBrain::analyzeNeuralActivity,
             py::return_value_policy::copy,
             "Analyze neural activity and return statistics")
        .def("storeExperience", &AdvancedBrain::storeExperienceForMemory,
             py::arg("sensory_pattern"), py::arg("action_pattern"), py::arg("reward"),
             "Store experience for memory systems")
        .def("advanceDevelopmentStage", &AdvancedBrain::advanceDevelopmentStage,
             py::arg("new_stage"), "Advance to a new developmental stage");

    // Enhanced agent brain interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
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

    // Enhanced world system
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
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

    // Performance profiler
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance profiler for timing and resource monitoring)")
        .def(py::init<>())
        .def("start", &PerformanceProfiler::start, "Start profiling")
        .def("stop", &PerformanceProfiler::stop, "Stop profiling")
        .def("step", &PerformanceProfiler::step, "Record a timestep")
        .def("getMetrics", &PerformanceProfiler::getMetrics,
             py::return_value_policy::copy, "Get performance metrics")
        .def("getAverageStepTime", &PerformanceProfiler::getAverageStepTime,
             "Get average step time")
        .def("getMinStepTime", &PerformanceProfiler::getMinStepTime,
             "Get minimum step time")
        .def("getMaxStepTime", &PerformanceProfiler::getMaxStepTime,
             "Get maximum step time")
        .def("reset", &PerformanceProfiler::reset, "Reset profiler");

    // Research utilities
    m.def("runBatchExperiments", &ResearchUtilities::runBatchExperiments,
          py::arg("configs"), py::arg("filter") = nullptr,
          "Run multiple experiments with different configurations");

    m.def("computeStatistics", &ResearchUtilities::computeStatistics,
          py::arg("data"), "Compute statistics from data vector");

    m.def("analyzePatternSimilarity", &ResearchUtilities::analyzePatternSimilarity,
          py::arg("pattern1"), py::arg("pattern2"),
          "Analyze similarity between two patterns");

    // Research agent factory
    m.def("createAgentForResearch", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create an agent brain interface for research");

    // Convenience functions for research
    m.def("createResearchBrain", [](int64_t neuronCount = 1000, bool enablePlasticity = true) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("connection_probability", 0.05f);
        if (enablePlasticity) {
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        }

        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count") = 1000, py::arg("enable_plasticity") = true,
       "Create a research-ready brain with common settings");

    m.def("runResearchExperiment", [](std::shared_ptr<Brain> brain,
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      int steps) {
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);

            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);

            brain->step(step, step * 0.001);

            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());

            // Apply some reward modulation
            auto reward = percept.getInternal();
            if (!reward.empty()) {
                agent->applyRewardModulation(reward[0], 0.0f);
            }

            agent->updateDevelopment(0.1);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a standard research experiment");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Documentation and examples
    m.def("getResearchExamples", []() {
        return R"pbdoc(
            # Research Examples for NLM Python API

            ## Example 1: Neuromodulation Experiment
            ```python
            import pynlm

            # Create brain and agent
            brain = pynlm.createResearchBrain(500, True)
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            agent = pynlm.createAgentForResearch(brain, world)

            # Enable neuromodulation
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)

            # Run experiment
            for step in range(100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            ```

            ## Example 2: Memory Research
            ```python
            import pynlm

            # Create brain with enhanced configuration
            config = pynlm.createEnhancedConfig()
            config.setNeuronCount(1000)
            config.setConnectionProbability(0.1)

            brain = pynlm.createBrain(config.getInternalConfig())
            brain.initialize()

            # Store experiences for memory research
            for step in range(50):
                pattern = [float(i % 10) / 10.0 for i in range(10)]
                brain.AdvancedBrain.storeExperience(brain, pattern, pattern, 0.5)
                brain.step(step)
            ```

            ## Example 3: Performance Profiling
            ```python
            import pynlm

            brain = pynlm.createResearchBrain()
            profiler = pynlm.PerformanceProfiler()

            profiler.start()
            for step in range(100):
                brain.step(step)
                profiler.step()
            profiler.stop()

            metrics = profiler.getMetrics()
            print(f"Average step time: {profiler.getAverageStepTime():.3f}ms")
            print(f"Total spikes: {metrics.spikeCount}")
            ```
        )pbdoc";
    }, "Get research examples and documentation");

    // Research protocol templates
    m.def("getResearchProtocolTemplates", []() {
        return R"pbdoc(
            # Research Protocol Templates

            ## Template 1: Developmental Research Protocol
            """
            Protocol: Developmental Studies in NLM

            Purpose: Study how developmental stages affect learning and behavior

            Steps:
            1. Initialize brain with developmental system enabled
            2. Set initial developmental stage (e.g., Initial)
            3. Run baseline experiments
            4. Advance developmental stages gradually
            5. Repeat experiments at each stage
            6. Compare results across stages
            """

            ## Template 2: Neuromodulation Experiment Protocol
            """
            Protocol: Reward Prediction Error Studies

            Purpose: Study how dopamine affects learning

            Steps:
            1. Create agent with reward modulation enabled
            2. Define task with variable rewards
            3. Record neural activity during task performance
            4. Manipulate reward prediction errors
            5. Analyze neural correlates of prediction errors
            """

            ## Template 3: Memory Consolidation Protocol
            """
            Protocol: Sleep/wake Memory Consolidation

            Purpose: Study how memory consolidation occurs during rest periods

            Steps:
            1. Encode information during learning phase
            2. Allow rest period (no input)
            3. Test recall after rest
            4. Vary rest duration
            5. Compare memory strength across conditions
            """
        )pbdoc";
    }, "Get research protocol templates");

    // Advanced mathematical operations
    m.def("computePatternSimilarityMatrix", [](const std::vector<std::vector<float>>& patterns) {
        std::vector<std::vector<float>> similarityMatrix;
        for (size_t i = 0; i < patterns.size(); ++i) {
            std::vector<float> row;
            for (size_t j = 0; j < patterns.size(); ++j) {
                float sim = ResearchUtilities::analyzePatternSimilarity(patterns[i], patterns[j])[0];
                row.push_back(sim);
            }
            similarityMatrix.push_back(row);
        }
        return similarityMatrix;
    }, py::arg("patterns"), "Compute similarity matrix for multiple patterns");

    // Configuration schema and validation
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        EnhancedConfig enhancedConfig;
        enhancedConfig.config_ = config;
        return enhancedConfig.validate();
    }, py::arg("config"), "Validate configuration parameters");

    m.def("getConfigurationSchema", [](std::shared_ptr<Config> config) {
        EnhancedConfig enhancedConfig;
        enhancedConfig.config_ = config;
        return enhancedConfig.getSchema();
    }, py::arg("config"), "Get configuration schema documentation");

    // Research data utilities
    m.def("exportForAnalysis", [](std::shared_ptr<Brain> brain,
                                 const std::string& filename,
                                 const std::string& format = "csv") {
        // Export brain state for external analysis
        std::map<std::string, double> analysis = AdvancedBrain::analyzeNeuralActivity(brain);

        if (format == "csv") {
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "metric,value\n";
                for (const auto& pair : analysis) {
                    file << pair.first << "," << pair.second << "\n";
                }
            }
        }
    }, py::arg("brain"), py::arg("filename"), py::arg("format") = "csv",
       "Export brain analysis data for external tools");
}

} // namespace nlm
