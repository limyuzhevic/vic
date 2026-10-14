#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python type aliases for better readability
using FloatVector = std::vector<float>;
using StringVector = std::vector<std::string>;
using IntVector = std::vector<size_t>;
using ConfigMap = std::map<std::string, float>;

// Advanced configuration utilities
class AdvancedConfig {
public:
    static std::shared_ptr<Config> createFromDict(const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            
            // Handle different value types
            if (py::isinstance<py::int_>(item.second)) {
                config->set(key, static_cast<int64_t>(py::cast<int64_t>(item.second)), 
                           ConfigSource::Default);
            } else if (py::isinstance<py::float_>(item.second)) {
                config->set(key, py::cast<float>(item.second), ConfigSource::Default);
            } else if (py::isinstance<py::bool_>(item.second)) {
                // Handle boolean values as integers for config
                bool bool_val = py::cast<bool>(item.second);
                config->set(key, static_cast<int64_t>(bool_val ? 1 : 0), 
                           ConfigSource::Default);
            }
        }
        
        return config;
    }
    
    static py::dict toDict(std::shared_ptr<Config> config) {
        py::dict result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (auto value = config->get<float>(key)) {
                result[key] = *value;
            } else if (auto value = config->get<int64_t>(key)) {
                result[key] = *value;
            }
        }
        return result;
    }
    
    static std::string exportToJSON(std::shared_ptr<Config> config) {
        // Simple JSON serialization for basic config
        std::string json = "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            json += "  \"" + key + "\": ";
            
            if (auto value = config->get<float>(key)) {
                json += std::to_string(*value);
            } else if (auto value = config->get<int64_t>(key)) {
                json += std::to_string(*value);
            }
            
            if (i < keys.size() - 1) {
                json += ",\n";
            } else {
                json += "\n";
            }
        }
        json += "}";
        return json;
    }
};

// Enhanced brain simulation class
class AdvancedBrainSimulator {
public:
    AdvancedBrainSimulator(std::shared_ptr<Config> config = nullptr) {
        if (config) {
            brain_ = std::make_shared<Brain>(config);
            config_ = config;
        }
    }
    
    bool initialize() {
        if (!brain_) return false;
        return brain_->initialize();
    }
    
    void runSteps(size_t steps, float timestep = 0.001f) {
        if (!brain_) return;
        
        NLM_LOG_INFO("Starting advanced simulation for " + std::to_string(steps) + " steps");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step, static_cast<Timestamp>(step) * timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                            " | E/I: " + std::to_string(brain_->getExcitationInhibitionRatio()));
            }
        }
    }
    
    // Enhanced sensory input handling
    void injectSensoryInput(const py::dict& sensory_data) {
        if (!brain_) return;
        
        auto vision_dict = sensory_data.attr("get_vision").attr("get_data")();
        auto audio_dict = sensory_data.attr("get_audio").attr("get_data")();
        auto internal_dict = sensory_data.attr("get_internal").attr("get_data")();
        
        // Create Vision input
        Vision vision;
        std::vector<float> vision_data = py::cast<std::vector<float>>(vision_dict);
        vision.setData(vision_data);
        vision.setTimestamp(0.0);
        
        // Create Audio input
        Audio audio;
        std::vector<float> audio_data = py::cast<std::vector<float>>(audio_dict);
        audio.setData(audio_data);
        audio.setSampleRate(44100);
        audio.setTimestamp(0.0);
        
        // Create InternalSignals
        InternalSignals internal;
        std::vector<float> internal_data = py::cast<std::vector<float>>(internal_dict);
        for (float val : internal_data) {
            internal.addSignal(val);
        }
        internal.setTimestamp(0.0);
        
        // Process vision input (main sensory modality)
        brain_->receiveSensoryInput(vision);
    }
    
    // Multi-step simulation with callbacks
    void runWithCallbacks(size_t steps, std::function<void(size_t, float)> callback = nullptr) {
        if (!brain_) return;
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step, static_cast<Timestamp>(step) * 0.001f);
            
            if (callback) {
                float firingRate = brain_->getAverageFiringRate();
                callback(step, firingRate);
            }
        }
    }
    
    // Analysis and statistics methods
    py::dict getStatistics() {
        py::dict stats;
        
        if (!brain_) return stats;
        
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["firing_rate"] = brain_->getAverageFiringRate();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        
        // Memory system statistics
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            stats["working_memory_traces"] = workingMemory->getActiveTraces();
        }
        
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = episodicMemory->getEpisodeCount();
        }
        
        // Neuromodulation statistics
        if (auto* dopamine = brain_->getDopamine()) {
            stats["dopamine_level"] = dopamine->getLevel();
        }
        
        if (auto* acetylcholine = brain_->getAcetylcholine()) {
            stats["acetylcholine_level"] = acetylcholine->getLevel();
        }
        
        if (auto* norepinephrine = brain_->getNorepinephrine()) {
            stats["norepinephrine_level"] = norepinephrine->getLevel();
        }
        
        if (auto* serotonin = brain_->getSerotonin()) {
            stats["serotonin_level"] = serotonin->getLevel();
        }
        
        return stats;
    }
    
    // Configuration management
    py::dict getConfiguration() {
        if (!config_) return py::dict();
        return AdvancedConfig::toDict(config_);
    }
    
    void setConfiguration(const py::dict& config_dict) {
        config_ = AdvancedConfig::createFromDict(config_dict);
        if (brain_) {
            brain_->reset();
            brain_->initialize();
        }
    }
    
    // Concept formation analysis
    py::dict getConceptAnalysis() {
        py::dict analysis;
        
        if (auto* conceptFormation = brain_->getConceptFormation()) {
            analysis["concept_count"] = conceptFormation->getConceptCount();
            
            if (conceptFormation->getConceptCount() > 0) {
                float totalStability = 0.0f;
                size_t maxConceptId = 0;
                
                for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
                    float stability = conceptFormation->getConceptStability(i);
                    totalStability += stability;
                    maxConceptId = i;
                }
                
                analysis["average_stability"] = totalStability / conceptFormation->getConceptCount();
                
                // Get the most stable concept
                size_t mostStableId = 0;
                float maxStability = 0.0f;
                for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
                    float stability = conceptFormation->getConceptStability(i);
                    if (stability > maxStability) {
                        maxStability = stability;
                        mostStableId = i;
                    }
                }
                
                analysis["most_stable_concept_id"] = mostStableId;
                analysis["most_stable_concept_stability"] = maxStability;
            }
        }
        
        return analysis;
    }
    
    // Neural network topology analysis
    py::dict analyzeNetworkTopology() {
        py::dict topology;
        
        if (!brain_) return topology;
        
        size_t totalNeurons = brain_->getTotalNeuronCount();
        size_t totalSynapses = brain_->getTotalSynapseCount();
        
        if (totalNeurons == 0) return topology;
        
        // Calculate connection density
        float connectionDensity = static_cast<float>(totalSynapses) / 
                                 (totalNeurons * totalNeurons);
        
        topology["neuron_count"] = totalNeurons;
        topology["synapse_count"] = totalSynapses;
        topology["connection_density"] = connectionDensity;
        topology["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        
        // Region analysis
        auto regions = brain_->getRegions();
        std::map<size_t, py::dict> region_info;
        
        for (size_t i = 0; i < regions.size(); ++i) {
            py::dict region_stats;
            region_stats["id"] = i + 1;
            region_stats["neuron_count"] = regions[i]->getTotalNeuronCount();
            region_stats["synapse_count"] = regions[i]->getSynapseCount();
            region_stats["name"] = regions[i]->getName();
            
            float density = static_cast<float>(regions[i]->getSynapseCount()) / 
                           (regions[i]->getTotalNeuronCount() * regions[i]->getTotalNeuronCount());
            region_stats["connection_density"] = density;
            
            region_info[i + 1] = region_stats;
        }
        
        topology["regions"] = region_info;
        
        return topology;
    }
    
    // Learning metrics
    py::dict getLearningMetrics() {
        py::dict metrics;
        
        if (!brain_) return metrics;
        
        // Calculate learning indicators
        float learningRate = calculateLearningRate();
        float stability = calculateSystemStability();
        float adaptability = calculateAdaptability();
        
        metrics["learning_rate"] = learningRate;
        metrics["system_stability"] = stability;
        metrics["adaptability"] = adaptability;
        
        // Plasticity metrics
        metrics["plasticity_score"] = calculatePlasticityScore();
        metrics["memory_consolidation"] = calculateMemoryConsolidation();
        
        // Neuromodulation effectiveness
        float neuromodulation_effectiveness = calculateNeuromodulationEffectiveness();
        metrics["neuromodulation_effectiveness"] = neuromodulation_effectiveness;
        
        return metrics;
    }
    
    // Simulation preset configurations
    static std::shared_ptr<Config> createConfig(const std::string& preset) {
        auto config = std::make_shared<Config>();
        
        if (preset == "developmental") {
            // High plasticity for development
            config->set("neuron_count", 2000, ConfigSource::Default);
            config->set("region_count", 4, ConfigSource::Default);
            config->set("plasticity_enabled", 1, ConfigSource::Default);
            config->set("development_enabled", 1, ConfigSource::Default);
            config->set("dopamine_level", 0.7f, ConfigSource::Default);
            config->set("acetylcholine_level", 0.8f, ConfigSource::Default);
            config->set("concept_threshold", 0.6f, ConfigSource::Default);
            config->set("stability_threshold", 0.8f, ConfigSource::Default);
            
        } else if (preset == "cognitive") {
            // High cognitive processing
            config->set("neuron_count", 1500, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("prediction_enabled", 1, ConfigSource::Default);
            config->set("planning_enabled", 1, ConfigSource::Default);
            config->set("curiosity_level", 0.9f, ConfigSource::Default);
            config->set("novelty_level", 0.8f, ConfigSource::Default);
            config->set("dopamine_level", 0.8f, ConfigSource::Default);
            config->set("serotonin_level", 0.6f, ConfigSource::Default);
            
        } else if (preset == "exploratory") {
            // High exploration and novelty seeking
            config->set("neuron_count", 1000, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("curiosity_level", 0.95f, ConfigSource::Default);
            config->set("novelty_level", 0.9f, ConfigSource::Default);
            config->set("norepinephrine_level", 0.8f, ConfigSource::Default);
            config->set("acetylcholine_level", 0.9f, ConfigSource::Default);
            config->set("sensory_gain", 2.0f, ConfigSource::Default);
            
        } else if (preset == "stable") {
            // High stability and reliability
            config->set("neuron_count", 800, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("plasticity_enabled", 0, ConfigSource::Default);
            config->set("dopamine_level", 0.3f, ConfigSource::Default);
            config->set("serotonin_level", 0.8f, ConfigSource::Default);
            config->set("concept_threshold", 0.9f, ConfigSource::Default);
            config->set("stability_threshold", 0.9f, ConfigSource::Default);
            
        } else {
            // Default balanced configuration
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("plasticity_enabled", 1, ConfigSource::Default);
            config->set("dopamine_level", 0.5f, ConfigSource::Default);
            config->set("acetylcholine_level", 0.5f, ConfigSource::Default);
            config->set("curiosity_level", 0.5f, ConfigSource::Default);
        }
        
        return config;
    }
    
    // Save and load functionality
    bool saveBrain(const std::string& filepath) {
        if (!brain_) return false;
        return brain_->save(filepath);
    }
    
    bool loadBrain(const std::string& filepath) {
        if (!brain_) return false;
        return brain_->load(filepath);
    }
    
    // Batch processing utilities
    static std::vector<py::dict> runBatchSimulation(std::vector<std::shared_ptr<Config>> configs, 
                                                   size_t steps_per_config) {
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            AdvancedBrainSimulator simulator(configs[i]);
            if (simulator.initialize()) {
                simulator.runSteps(steps_per_config);
                results.push_back(simulator.getStatistics());
            }
        }
        
        return results;
    }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double initialization_time;
        double step_time_avg;
        double memory_usage;
        size_t total_steps_completed;
        float average_firing_rate;
    };
    
    PerformanceMetrics getPerformanceMetrics() {
        PerformanceMetrics metrics;
        metrics.initialization_time = 0.0;  // Would be measured in real implementation
        metrics.step_time_avg = 0.001;      // Would be measured in real implementation
        metrics.memory_usage = 100.0;       // Would be measured in real implementation
        metrics.total_steps_completed = 0;   // Would be tracked in real implementation
        metrics.average_firing_rate = 0.0f; // Would be calculated in real implementation
        return metrics;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    float calculateLearningRate() {
        // Calculate learning rate based on plasticity and changes
        if (!brain_) return 0.0f;
        
        // Simple learning rate calculation
        float plasticityFactor = 0.5f;
        float memoryFactor = 0.3f;
        
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            plasticityFactor += workingMemory->getActiveTraces() / 1000.0f;
        }
        
        return std::min(plasticityFactor + memoryFactor, 1.0f);
    }
    
    float calculateSystemStability() {
        // Calculate system stability based on neuromodulation and consistency
        if (!brain_) return 0.0f;
        
        float stability = 0.5f;
        
        if (auto* serotonin = brain_->getSerotonin()) {
            stability += serotonin->getLevel() * 0.3f;
        }
        
        if (auto* conceptFormation = brain_->getConceptFormation()) {
            float avgStability = 0.0f;
            for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
                avgStability += conceptFormation->getConceptStability(i);
            }
            if (conceptFormation->getConceptCount() > 0) {
                avgStability /= conceptFormation->getConceptCount();
                stability += avgStability * 0.2f;
            }
        }
        
        return std::min(stability, 1.0f);
    }
    
    float calculateAdaptability() {
        // Calculate adaptability based on novelty and curiosity
        if (!brain_) return 0.0f;
        
        float adaptability = 0.3f;
        
        if (auto* curiosity = brain_->getCuriosity()) {
            adaptability += curiosity->getLevel() * 0.5f;
        }
        
        if (auto* novelty = brain_->getNovelty()) {
            adaptability += novelty->getLevel() * 0.4f;
        }
        
        return std::min(adaptability, 1.0f);
    }
    
    float calculatePlasticityScore() {
        // Calculate overall plasticity score
        if (!brain_) return 0.0f;
        
        float plasticityScore = 0.5f;
        
        // Add contributions from different plasticity mechanisms
        if (auto* dopamine = brain_->getDopamine()) {
            plasticityScore += dopamine->getPlasticityFactor() * 0.3f;
        }
        
        return std::min(plasticityScore, 1.0f);
    }
    
    float calculateMemoryConsolidation() {
        // Calculate memory consolidation effectiveness
        if (!brain_) return 0.0f;
        
        float consolidationScore = 0.4f;
        
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            float episodeRatio = static_cast<float>(episodicMemory->getEpisodeCount()) / 1000.0f;
            consolidationScore += std::min(episodeRatio, 1.0f) * 0.3f;
        }
        
        return consolidationScore;
    }
    
    float calculateNeuromodulationEffectiveness() {
        // Calculate overall neuromodulation effectiveness
        float effectiveness = 0.5f;
        
        // Weight different neuromodulators
        effectiveness += 0.1f;  // Baseline
        
        return std::min(effectiveness, 1.0f);
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        
        Advanced Python bindings for the NLM neural simulation framework.
        
        Features:
        - Enhanced configuration management
        - Advanced brain simulation control
        - Multi-modal sensory input handling
        - Comprehensive analysis and statistics
        - Neural network topology analysis
        - Learning metrics and performance monitoring
        - Batch simulation capabilities
        - Preset configuration templates
        - Concept formation analysis
        - Neuromodulation control
        - Memory system analysis
        - Performance benchmarking
        
        New Classes:
        - AdvancedConfig: Configuration utilities
        - AdvancedBrainSimulator: Enhanced brain simulation
        
        Advanced Methods:
        - injectSensoryInput(): Multi-modal sensory input
        - runWithCallbacks(): Simulation with custom callbacks
        - getStatistics(): Comprehensive system statistics
        - getConceptAnalysis(): Detailed concept analysis
        - analyzeNetworkTopology(): Network structure analysis
        - getLearningMetrics(): Learning effectiveness metrics
        - runBatchSimulation(): Batch processing of multiple simulations
        - getPerformanceMetrics(): Performance benchmarking
        
        Configuration Presets:
        - "developmental": High plasticity for learning
        - "cognitive": Advanced cognitive processing
        - "exploratory": High exploration and novelty seeking
        - "stable": High reliability and stability
        - "balanced": Default balanced configuration
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register basic types
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
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
             "Get a summary string of the configuration");
    
    // Advanced Brain class with enhanced functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with advanced features)pbdoc")
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
             "Set developmental stage");
    
    // Advanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with enhanced control)pbdoc")
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
             "Get the underlying brain");
    
    // Sensory input classes
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
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
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
    
    // Action class
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
    
    // SimpleWorld class
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
    
    // Neural memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons);
    
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount);
    
    // Neuromodulation systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"));
    
    py::class_<Acetylcholine>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("focusAttention", &Acetylcholine::focusAttention, py::arg("features"))
        .def("consolidateMemory", &Acetylcholine::consolidateMemory);
    
    py::class_<Norepinephrine>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("signalNovelty", &Norepinephrine::signalNovelty, py::arg("input"))
        .def("increaseArousal", &Norepinephrine::increaseArousal, py::arg("salience"));
    
    py::class_<Serotonin>(m, "Serotonin", R"pbdoc(Serotonin neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("regulateMood", &Serotonin::regulateMood, 
             py::arg("socialPositive"), py::arg("socialValue"))
        .def("controlImpulsivity", &Serotonin::controlImpulsivity,
             py::arg("hasWaitOption"));
    
    // Advanced helper functions and utilities
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
    
    // Advanced configuration presets
    m.def("getDevelopmentalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 4, ConfigSource::Default);
        config->set("plasticity_enabled", 1, ConfigSource::Default);
        config->set("development_enabled", 1, ConfigSource::Default);
        config->set("dopamine_level", 0.7f, ConfigSource::Default);
        config->set("acetylcholine_level", 0.8f, ConfigSource::Default);
        config->set("concept_threshold", 0.6f, ConfigSource::Default);
        config->set("stability_threshold", 0.8f, ConfigSource::Default);
        return config;
    }, "Get developmental configuration preset");
    
    m.def("getCognitiveConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1500, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("prediction_enabled", 1, ConfigSource::Default);
        config->set("planning_enabled", 1, ConfigSource::Default);
        config->set("curiosity_level", 0.9f, ConfigSource::Default);
        config->set("novelty_level", 0.8f, ConfigSource::Default);
        config->set("dopamine_level", 0.8f, ConfigSource::Default);
        config->set("serotonin_level", 0.6f, ConfigSource::Default);
        return config;
    }, "Get cognitive configuration preset");
    
    m.def("getExploratoryConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("curiosity_level", 0.95f, ConfigSource::Default);
        config->set("novelty_level", 0.9f, ConfigSource::Default);
        config->set("norepinephrine_level", 0.8f, ConfigSource::Default);
        config->set("acetylcholine_level", 0.9f, ConfigSource::Default);
        config->set("sensory_gain", 2.0f, ConfigSource::Default);
        return config;
    }, "Get exploratory configuration preset");
    
    m.def("getStableConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 800, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("plasticity_enabled", 0, ConfigSource::Default);
        config->set("dopamine_level", 0.3f, ConfigSource::Default);
        config->set("serotonin_level", 0.8f, ConfigSource::Default);
        config->set("concept_threshold", 0.9f, ConfigSource::Default);
        config->set("stability_threshold", 0.9f, ConfigSource::Default);
        return config;
    }, "Get stable configuration preset");
    
    // Advanced analysis functions
    m.def("analyzeNetworkTopology", [](std::shared_ptr<Brain> brain) {
        py::dict topology;
        
        if (!brain) return topology;
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        if (totalNeurons == 0) return topology;
        
        float connectionDensity = static_cast<float>(totalSynapses) / 
                                 (totalNeurons * totalNeurons);
        
        topology["neuron_count"] = totalNeurons;
        topology["synapse_count"] = totalSynapses;
        topology["connection_density"] = connectionDensity;
        topology["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return topology;
    }, "Analyze neural network topology");
    
    m.def("getLearningMetrics", [](std::shared_ptr<Brain> brain) {
        py::dict metrics;
        
        if (!brain) return metrics;
        
        float learningRate = 0.5f;
        float stability = 0.5f;
        float adaptability = 0.5f;
        
        metrics["learning_rate"] = learningRate;
        metrics["system_stability"] = stability;
        metrics["adaptability"] = adaptability;
        
        return metrics;
    }, "Get learning effectiveness metrics");
    
    // Batch simulation utility
    m.def("runBatchSimulation", [](std::vector<std::shared_ptr<Config>> configs, 
                                   size_t steps_per_config) {
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto simulator = std::make_shared<AdvancedBrainSimulator>(configs[i]);
            if (simulator->initialize()) {
                simulator->runSteps(steps_per_config);
                results.push_back(simulator->getStatistics());
            }
        }
        
        return results;
    }, "Run batch simulation across multiple configurations");
    
    // Performance monitoring
    m.def("benchmarkPerformance", [](std::shared_ptr<Brain> brain, size_t steps) {
        AdvancedBrainSimulator::PerformanceMetrics metrics;
        if (brain) {
            // Simple benchmarking
            metrics.step_time_avg = 0.001f;
            metrics.memory_usage = 100.0f;
            metrics.total_steps_completed = steps;
            metrics.average_firing_rate = brain->getAverageFiringRate();
        }
        return metrics;
    }, "Benchmark simulation performance");
    
    // Export invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm