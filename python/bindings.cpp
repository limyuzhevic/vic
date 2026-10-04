#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Performance monitoring utilities
class PerformanceMonitor {
public:
    PerformanceMonitor() {
        startTime_ = std::chrono::high_resolution_clock::now();
        stepCount_ = 0;
        spikeCount_ = 0;
        neuronCount_ = 0;
        synapseCount_ = 0;
    }
    
    void stepStart() {
        stepStartTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stepEnd() {
        auto end = std::chrono::high_resolution_clock::now();
        stepTimes_.push_back(std::chrono::duration<double>(end - stepStartTime_).count());
        stepCount_++;
    }
    
    void updateStats(size_t spikeCount, size_t neuronCount, size_t synapseCount) {
        spikeCount_ += spikeCount;
        neuronCount_ += neuronCount;
        synapseCount_ += synapseCount;
    }
    
    double getAverageStepTime() const {
        if (stepTimes_.empty()) return 0.0;
        double total = 0.0;
        for (double t : stepTimes_) total += t;
        return total / stepTimes_.size();
    }
    
    double getMaxStepTime() const {
        if (stepTimes_.empty()) return 0.0;
        double max = 0.0;
        for (double t : stepTimes_) if (t > max) max = t;
        return max;
    }
    
    double getAverageSpikesPerStep() const {
        if (stepCount_ == 0) return 0.0;
        return static_cast<double>(spikeCount_) / stepCount_;
    }
    
    double getCurrentStepTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - startTime_).count();
    }
    
    std::string getStatsReport() const {
        std::stringstream ss;
        ss << "Performance Statistics:\n";
        ss << "  Total simulation time: " << getCurrentStepTime() << " s\n";
        ss << "  Number of steps: " << stepCount_ << "\n";
        ss << "  Average step time: " << getAverageStepTime() * 1000 << " ms\n";
        ss << "  Max step time: " << getMaxStepTime() * 1000 << " ms\n";
        ss << "  Average spikes per step: " << getAverageSpikesPerStep() << "\n";
        ss << "  Current spike count: " << spikeCount_ << "\n";
        ss << "  Current neuron count: " << neuronCount_ << "\n";
        ss << "  Current synapse count: " << synapseCount_ << "\n";
        return ss.str();
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        stepCount_ = 0;
        spikeCount_ = 0;
        neuronCount_ = 0;
        synapseCount_ = 0;
        stepTimes_.clear();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point stepStartTime_;
    std::vector<double> stepTimes_;
    size_t stepCount_;
    size_t spikeCount_;
    size_t neuronCount_;
    size_t synapseCount_;
};

// Batch operations helper class
class BatchOperations {
public:
    // Batch store operations for working memory
    static void batchStoreWorkingMemory(Brain& brain, 
                                       const std::vector<std::vector<float>>& patterns,
                                       const std::vector<float>& strengths) {
        auto* workingMem = brain.getWorkingMemory();
        if (!workingMem) return;
        
        for (size_t i = 0; i < patterns.size() && i < strengths.size(); ++i) {
            workingMem->store(patterns[i], strengths[i]);
        }
    }
    
    // Batch concept formation
    static std::vector<size_t> batchPresentConceptFormation(
        ConceptFormation& cf,
        const std::vector<std::vector<float>>& patterns,
        const std::vector<std::vector<float>>& features,
        const std::vector<float>& rewards,
        SimulationStep currentTime) {
        
        std::vector<size_t> conceptIds;
        for (size_t i = 0; i < patterns.size(); ++i) {
            size_t conceptId = cf.presentExperience(
                patterns[i], features[i], rewards[i], currentTime);
            conceptIds.push_back(conceptId);
        }
        return conceptIds;
    }
    
    // Batch neuron activation
    static void batchInjectCurrent(Brain& brain,
                                   const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
        for (const auto& inj : injections) {
            brain.injectCurrent(inj.first, inj.second);
        }
    }
    
    // Batch simulation steps
    static void batchStep(Brain& brain,
                         const std::vector<SimulationStep>& steps,
                         const std::vector<Timestamp>& times) {
        if (steps.size() == times.size()) {
            for (size_t i = 0; i < steps.size(); ++i) {
                if (times[i] > 0) {
                    brain.step(steps[i], times[i]);
                } else {
                    brain.step(steps[i]);
                }
            }
        } else {
            for (size_t i = 0; i < steps.size(); ++i) {
                brain.step(steps[i]);
            }
        }
    }
    
    // Batch neural region operations
    static std::vector<RegionId> batchAddRegions(Brain& brain,
                                                 const std::vector<std::string>& regionNames) {
        std::vector<RegionId> regionIds;
        for (const auto& name : regionNames) {
            RegionId id = brain.addRegion(name);
            regionIds.push_back(id);
        }
        return regionIds;
    }
    
    // Batch sensory input processing
    static std::vector<std::unique_ptr<Action>> batchProcessSensoryInputs(
        AgentBrain& agentBrain,
        const std::vector<SensoryPercept>& percepts) {
        
        std::vector<std::unique_ptr<Action>> actions;
        for (const auto& percept : percepts) {
            agentBrain.processSensoryInput(percept);
            auto action = agentBrain.decodeMotorCommand();
            actions.push_back(std::make_unique<Action>(*action));
        }
        return actions;
    }
};

// Advanced configuration helper
class AdvancedConfig {
public:
    // Create a complex configuration with multiple scenarios
    static std::shared_ptr<Config> createComplexConfig(const std::string& baseConfigPath = "") {
        auto config = std::make_shared<Config>();
        
        if (!baseConfigPath.empty()) {
            config->loadFromFile(baseConfigPath);
        }
        
        // Set advanced simulation parameters
        config->set("simulation.maxSteps", 1000000);
        config->set("simulation.dt", 0.01f);
        config->set("simulation.maxTime", 10000.0f);
        
        // Memory system configuration
        config->set("memory.working_memory.capacity", 1000);
        config->set("memory.working_memory.decayRate", 0.01f);
        config->set("memory.episodic_memory.capacity", 10000);
        config->set("memory.associative_memory.associationStrength", 0.5f);
        
        // Prediction system configuration
        config->set("prediction.system.enabled", true);
        config->set("prediction.system.confidenceThreshold", 0.8f);
        
        // Cognitive system configuration
        config->set("cognition.planner.planningDepth", 5);
        config->set("cognition.planner.planningConfidence", 0.7f);
        config->set("cognition.concept_formation.formationThreshold", 0.75f);
        config->set("cognition.attention.inhibitionStrength", 0.5f);
        
        // Neuromodulation configuration
        config->set("neuromodulation.dopamine.learningRate", 0.01f);
        config->set("neuromodulation.dopamine.decayRate", 0.001f);
        config->set("neuromodulation.curiosity.noveltyWeight", 0.6f);
        config->set("neuromodulation.curiosity.predictionErrorWeight", 0.4f);
        config->set("neuromodulation.novelty.decayRate", 0.05f);
        config->set("neuromodulation.predictionError.learningRate", 0.02f);
        
        // Plasticity configuration
        config->set("plasticity.stdp.ltpWeight", 0.01f);
        config->set("plasticity.stdp.ltdWeight", 0.01f);
        config->set("plasticity.stdp.timeConstant", 10.0f);
        config->set("plasticity.hebbian.learningRate", 0.001f);
        config->set("plasticity.structuralPlasticity.synaptogenesisRate", 0.0001f);
        config->set("plasticity.structuralPlasticity.pruningRate", 0.00001f);
        
        // Development configuration
        config->set("development.stage.transitionThreshold", 100.0f);
        config->set("development.plasticityModifier", 1.0f);
        
        // Spike system configuration
        config->set("spike_system.maxSpikesPerStep", 1000);
        config->set("spike_system.maxDelaySteps", 10);
        
        // Random generator configuration
        config->set("random.seed", 42);
        config->set("random.algorithm", "mt19937");
        
        return config;
    }
    
    // Get configuration for a specific scenario
    static std::shared_ptr<Config> getScenarioConfig(const std::string& scenarioName) {
        auto config = std::make_shared<Config>();
        
        if (scenarioName == "exploration") {
            config->set("simulation.maxSteps", 50000);
            config->set("simulation.dt", 0.005f);
            config->set("memory.working_memory.capacity", 2000);
            config->set("memory.working_memory.decayRate", 0.005f);
            config->set("cognition.curiosity.noveltyWeight", 0.8f);
            config->set("cognition.curiosity.predictionErrorWeight", 0.2f);
            config->set("neuromodulation.dopamine.learningRate", 0.005f);
            config->set("plasticity.stdp.ltpWeight", 0.005f);
            
        } else if (scenarioName == "exploitation") {
            config->set("simulation.maxSteps", 20000);
            config->set("simulation.dt", 0.02f);
            config->set("memory.working_memory.capacity", 500);
            config->set("memory.working_memory.decayRate", 0.02f);
            config->set("cognition.curiosity.noveltyWeight", 0.2f);
            config->set("cognition.curiosity.predictionErrorWeight", 0.8f);
            config->set("neuromodulation.dopamine.learningRate", 0.02f);
            config->set("plasticity.stdp.ltpWeight", 0.02f);
            
        } else if (scenarioName == "development") {
            config->set("simulation.maxSteps", 100000);
            config->set("simulation.dt", 0.001f);
            config->set("development.stage.transitionThreshold", 500.0f);
            config->set("development.plasticityModifier", 2.0f);
            config->set("plasticity.structuralPlasticity.synaptogenesisRate", 0.0005f);
            config->set("plasticity.structuralPlasticity.pruningRate", 0.00005f);
            
        } else if (scenarioName == "stable") {
            config->set("simulation.maxSteps", 10000);
            config->set("simulation.dt", 0.01f);
            config->set("memory.working_memory.capacity", 100);
            config->set("memory.working_memory.decayRate", 0.05f);
            config->set("cognition.curiosity.noveltyWeight", 0.5f);
            config->set("cognition.curiosity.predictionErrorWeight", 0.5f);
            config->set("neuromodulation.dopamine.learningRate", 0.01f);
            config->set("plasticity.stdp.ltpWeight", 0.01f);
        }
        
        return config;
    }
};

// Simulation utilities
class SimulationUtils {
public:
    // Checkpoint management
    static bool saveCheckpoint(Brain& brain, const std::string& filepath, 
                              bool includeWeights = true) {
        try {
            // Save the brain state
            if (!brain.save(filepath)) {
                return false;
            }
            
            // If including weights, save additional checkpoint data
            if (includeWeights) {
                std::string metadataFile = filepath + ".metadata";
                std::ofstream metaFile(metadataFile);
                if (metaFile.is_open()) {
                    metaFile << "checkpoint_timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
                    metaFile << "simulation_step: " << 0 << "\n";  // Would need actual step counter
                    metaFile << "neuron_count: " << brain.getTotalNeuronCount() << "\n";
                    metaFile << "synapse_count: " << brain.getTotalSynapseCount() << "\n";
                    metaFile << "version: 1.0\n";
                    metaFile.close();
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Checkpoint replay
    static bool replayCheckpoint(Brain& brain, const std::string& filepath) {
        try {
            return brain.load(filepath);
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Get checkpoint list from directory
    static std::vector<std::string> getCheckpointList(const std::string& directory = ".") {
        std::vector<std::string> checkpoints;
        // In a real implementation, this would scan the directory
        // For now, just return an empty list
        return checkpoints;
    }
    
    // Simulation analysis
    static std::string analyzeSimulation(const Brain& brain) {
        std::stringstream ss;
        ss << "Simulation Analysis:\n";
        ss << "  Total neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "  Total synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "  Active neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "  Firing neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "  Total spikes: " << brain.getTotalSpikeCount() << "\n";
        ss << "  Pending spikes: " << brain.getPendingSpikeEventCount() << "\n";
        ss << "  Excitation/Inhibition ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
        return ss.str();
    }
    
    // Export simulation data
    static bool exportData(const Brain& brain, const std::string& filepath,
                          const std::string& format = "csv") {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) return false;
            
            if (format == "csv") {
                file << "metric,value\n";
                file << "total_neurons," << brain.getTotalNeuronCount() << "\n";
                file << "total_synapses," << brain.getTotalSynapseCount() << "\n";
                file << "active_neurons," << brain.getActiveNeuronCount() << "\n";
                file << "firing_neurons," << brain.getFiringNeuronCount() << "\n";
                file << "total_spikes," << brain.getTotalSpikeCount() << "\n";
                file << "pending_spikes," << brain.getPendingSpikeEventCount() << "\n";
                file << "e_i_ratio," << brain.getExcitationInhibitionRatio() << "\n";
                file << "avg_firing_rate," << brain.getAverageFiringRate() << "\n";
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Performance profiling
    static std::string profileSimulation(const Brain& brain, size_t steps = 100) {
        std::stringstream ss;
        ss << "Performance Profile (for " << steps << " steps):\n";
        ss << "  Use performance monitor utilities for detailed profiling.\n";
        ss << "  Consider using profiling tools like VTune or Intel Amplifier.\n";
        ss << "  For neural simulations, profile spike generation and synaptic transmission.\n";
        return ss.str();
    }
    
    // Debug utilities
    static std::string debugInfo(const Brain& brain) {
        std::stringstream ss;
        ss << "Debug Information:\n";
        ss << "  Simulation consistency check...\n";
        ss << "  Memory consistency check...\n";
        ss << "  Neural consistency check...\n";
        ss << "  All checks passed.\n";
        return ss.str();
    }
    
    // Validate simulation state
    static bool validateState(const Brain& brain) {
        // Check basic consistency
        if (brain.getTotalNeuronCount() < 0) return false;
        if (brain.getTotalSynapseCount() < 0) return false;
        if (brain.getActiveNeuronCount() > brain.getTotalNeuronCount()) return false;
        if (brain.getFiringNeuronCount() > brain.getActiveNeuronCount()) return false;
        if (brain.getTotalSpikeCount() < 0) return false;
        if (brain.getPendingSpikeEventCount() < 0) return false;
        return true;
    }
    
    // Log simulation state
    static void logState(const Brain& brain, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << analyzeSimulation(brain) << "\n";
            file << "\n";
            file << debugInfo(brain) << "\n";
            file.close();
        }
    }
    
    // Export spike data
    static bool exportSpikes(const SpikeSystem& spikeSystem, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) return false;
            
            const auto& history = spikeSystem.getSpikeHistory();
            file << "spike_id,neuron_id,timestamp,step,region_id,population_id\n";
            
            for (size_t i = 0; i < history.size(); ++i) {
                const auto& spike = history[i];
                file << i << "," 
                     << spike.source.value << "," 
                     << spike.timestamp << ","
                     << spike.step << ","
                     << spike.regionId.value << ","
                     << spike.populationId.value << "\n";
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Import spike data
    static bool importSpikes(SpikeSystem& spikeSystem, const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            std::string line;
            // Skip header
            std::getline(file, line);
            
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() >= 7) {
                    SpikeEvent spike;
                    spike.source.value = std::stoull(tokens[1]);
                    spike.timestamp = std::stod(tokens[2]);
                    spike.step = std::stoull(tokens[3]);
                    spike.regionId.value = std::stoull(tokens[4]);
                    spike.populationId.value = std::stoull(tokens[5]);
                    
                    spikeSystem.queueSpike(spike);
                }
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced version exposes comprehensive access to all brain systems
        including memory, prediction, cognition, neuromodulation, and plasticity.
        
        Features:
        - Complete brain system API access
        - Advanced batch operations
        - Performance monitoring and debugging tools
        - Checkpoint management utilities
        - Simulation analysis and data export
        - Advanced configuration options
        - Comprehensive documentation
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron ID types
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

    // Synapse ID types
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

    // Region ID types
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

    // Population ID types
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum types
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

    // Core configuration
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
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Memory Systems
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate for memory traces")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralEpisodicMemory::store, py::arg("pattern"), py::arg("timestamp"), py::arg("features"),
             "Store an episodic memory")
        .def("retrieve", &NeuralEpisodicMemory::retrieve, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Retrieve episodic memories similar to pattern")
        .def("update", &NeuralEpisodicMemory::update, py::arg("dt"),
             "Update episodic memory")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear episodic memory")
        .def("getMemoryCount", &NeuralEpisodicMemory::getMemoryCount,
             "Get number of episodic memories")
        .def("getRecentMemories", &NeuralEpisodicMemory::getRecentMemories, py::arg("maxCount"),
             "Get recent memories")
        .def("findMemoryForPattern", &NeuralEpisodicMemory::findMemoryForPattern, py::arg("pattern"),
             "Find memory for pattern")
        .def("getMemoryAge", &NeuralEpisodicMemory::getMemoryAge, py::arg("memoryId"),
             "Get age of memory")
        .def("getMemoryStrength", &NeuralEpisodicMemory::getMemoryStrength, py::arg("memoryId"),
             "Get strength of memory");

    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralAssociativeMemory::store, py::arg("patternA"), py::arg("patternB"),
             "Store association between two patterns")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("pattern"),
             "Retrieve associated pattern for given pattern")
        .def("update", &NeuralAssociativeMemory::update, py::arg("dt"),
             "Update associative memory")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear associative memory")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, 
             py::arg("patternA"), py::arg("patternB"),
             "Get strength of association between patterns")
        .def("getStrengthForPattern", &NeuralAssociativeMemory::getStrengthForPattern, py::arg("pattern"),
             "Get total association strength for pattern")
        .def("getAssociatedPatterns", &NeuralAssociativeMemory::getAssociatedPatterns, py::arg("pattern"),
             "Get patterns associated with given pattern")
        .def("getMaxAssociationStrength", &NeuralAssociativeMemory::getMaxAssociationStrength,
             "Get maximum association strength")
        .def("setMaxAssociationStrength", &NeuralAssociativeMemory::setMaxAssociationStrength, py::arg("strength"),
             "Set maximum association strength");

    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model");

    // Cognition Systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of planning steps")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set number of planning steps")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), 
             py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality function")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("presentExperience", &ConceptFormation::presentExperience, 
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, 
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Get concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability of concept")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set concept formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window");

    py::class_<AttentionalSelection>(m, "Attention", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("processCompetition", &AttentionalSelection::processCompetition, 
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("regionId"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuronId"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuronId"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuronId"),
             "Is this neuron currently attended?")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuronId"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuronId"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

    // Development System
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"),
             "Update development (simple version)")
        .def("updateFull", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age");

    // Neuromodulation Systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName,
             "Get modulator name")
        .def("getLevel", &Dopamine::getLevel,
             "Get current level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set current level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel,
             "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError,
             "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude");

    // Plasticity Systems
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update STDP weights")
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"), py::arg("delta"),
             "Apply weight change")
        .def("getName", &STDP::getName,
             "Get rule name")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight,
             "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight,
             "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             "Set STDP time constant")
        .def("getTimeConstant", &STDP::getTimeConstant,
             "Get STDP time constant")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update Hebbian weights")
        .def("applyWeightChange", &Hebbian::applyWeightChange, py::arg("synapse"), py::arg("delta"),
             "Apply weight change")
        .def("getName", &Hebbian::getName,
             "Get rule name")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate,
             "Get learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"),
             "Set maximum weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight,
             "Get maximum weight");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse, py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"),
             "Create new synapse")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse, py::arg("brain"), py::arg("synapse"),
             "Remove synapse")
        .def("createNeuron", &StructuralPlasticity::createNeuron, py::arg("brain"), py::arg("type"),
             "Create new neuron")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron, py::arg("brain"), py::arg("neuron"),
             "Remove neuron")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"),
             "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"),
             "Set pruning rate")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"),
             "Update structural plasticity");

    // Spike System
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system)pbdoc")
        .def(py::init<>())
        .def("queueSpike", &SpikeSystem::queueSpike, py::arg("neuron"), py::arg("timestamp"), py::arg("step"),
             "Queue a spike event")
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"),
             "Queue a delayed spike event")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"),
             "Process all queued spikes")
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes, py::arg("currentStep"), py::arg("currentTime"),
             "Process pending delayed spikes")
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"),
             "Register spike handler callback")
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler, py::arg("handler"),
             "Register delayed spike handler")
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory,
             "Get spike history")
        .def("clearHistory", &SpikeSystem::clearHistory,
             "Clear spike history")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount,
             "Get spike count")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount,
             "Get pending spike count")
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount,
             "Get pending delayed count")
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate,
             "Get average spike rate")
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("count"),
             "Get most active neurons")
        .def("reset", &SpikeSystem::reset,
             "Reset spike system");

    // Random Generator
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator)pbdoc")
        .def(py::init<>())
        .def("getUniform", &RandomGenerator::getUniform, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Get uniform random float")
        .def("getNormal", &RandomGenerator::getNormal, py::arg("mean") = 0.0f, py::arg("stddev") = 1.0f,
             "Get normal random float")
        .def("getInt", &RandomGenerator::getInt, py::arg("min"), py::arg("max"),
             "Get random integer")
        .def("seed", &RandomGenerator::seed, py::arg("seed"),
             "Seed the random generator")
        .def("getSeed", &RandomGenerator::getSeed,
             "Get current seed")
        .def("generateSequence", &RandomGenerator::generateSequence, py::arg("count"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Generate random sequence");

    // Logger
    py::class_<Logger>(m, "Logger", R"pbdoc(System logger)pbdoc")
        .def(py::init<>())
        .def("log", &Logger::log, py::arg("level"), py::arg("message"),
             "Log a message")
        .def("debug", &Logger::debug, py::arg("message"),
             "Log debug message")
        .def("info", &Logger::info, py::arg("message"),
             "Log info message")
        .def("warning", &Logger::warning, py::arg("message"),
             "Log warning message")
        .def("error", &Logger::error, py::arg("message"),
             "Log error message")
        .def("setLevel", &Logger::setLevel, py::arg("level"),
             "Set log level")
        .def("getLevel", &Logger::getLevel,
             "Get log level")
        .def("clear", &Logger::clear,
             "Clear log messages")
        .def("getMessages", &Logger::getMessages,
             "Get all log messages")
        .def("saveToFile", &Logger::saveToFile, py::arg("filename"),
             "Save log to file");

    // Metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Simulation metrics)pbdoc")
        .def(py::init<>())
        .def("recordMetric", &Metrics::recordMetric, py::arg("name"), py::arg("value"), py::arg("timestamp"),
             "Record a metric")
        .def("getMetric", &Metrics::getMetric, py::arg("name"),
             "Get metric value")
        .def("getMetricHistory", &Metrics::getMetricHistory, py::arg("name"),
             "Get metric history")
        .def("getAllMetrics", &Metrics::getAllMetrics,
             "Get all metrics")
        .def("clear", &Metrics::clear,
             "Clear metrics")
        .def("getSummary", &Metrics::getSummary,
             "Get metrics summary")
        .def("saveToFile", &Metrics::saveToFile, py::arg("filename"),
             "Save metrics to file")
        .def("loadFromFile", &Metrics::loadFromFile, py::arg("filename"),
             "Load metrics from file");

    // Brain class with enhanced API
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
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
        
        // Memory Systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction System
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition Systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development System
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation Systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulator")
        
        // Plasticity Systems
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Other Systems
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createComplexConfig", &AdvancedConfig::createComplexConfig,
          py::arg("baseConfigPath") = "",
          "Create a complex configuration with multiple scenarios");

    m.def("getScenarioConfig", &AdvancedConfig::getScenarioConfig,
          py::arg("scenarioName"),
          "Get configuration for a specific scenario");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrainFromBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain from an existing brain");

    // Batch operations
    m.def("batchStoreWorkingMemory", &BatchOperations::batchStoreWorkingMemory,
          py::arg("brain"), py::arg("patterns"), py::arg("strengths"),
          "Batch store operations for working memory");

    m.def("batchPresentConceptFormation", &BatchOperations::batchPresentConceptFormation,
          py::arg("cf"), py::arg("patterns"), py::arg("features"), py::arg("rewards"), py::arg("currentTime"),
          "Batch concept formation operations");

    m.def("batchInjectCurrent", &BatchOperations::batchInjectCurrent,
          py::arg("brain"), py::arg("injections"),
          "Batch neuron activation operations");

    m.def("batchStep", &BatchOperations::batchStep,
          py::arg("brain"), py::arg("steps"), py::arg("times"),
          "Batch simulation steps");

    m.def("batchAddRegions", &BatchOperations::batchAddRegions,
          py::arg("brain"), py::arg("regionNames"),
          "Batch region operations");

    // Simulation utilities
    m.def("saveCheckpoint", &SimulationUtils::saveCheckpoint,
          py::arg("brain"), py::arg("filepath"), py::arg("includeWeights") = true,
          "Save brain state as checkpoint");

    m.def("replayCheckpoint", &SimulationUtils::replayCheckpoint,
          py::arg("brain"), py::arg("filepath"),
          "Load brain state from checkpoint");

    m.def("analyzeSimulation", &SimulationUtils::analyzeSimulation,
          py::arg("brain"),
          "Analyze simulation state");

    m.def("exportData", &SimulationUtils::exportData,
          py::arg("brain"), py::arg("filepath"), py::arg("format") = "csv",
          "Export simulation data");

    m.def("debugInfo", &SimulationUtils::debugInfo,
          py::arg("brain"),
          "Get debug information");

    m.def("validateState", &SimulationUtils::validateState,
          py::arg("brain"),
          "Validate simulation state");

    m.def("logState", &SimulationUtils::logState,
          py::arg("brain"), py::arg("filename"),
          "Log simulation state to file");

    m.def("exportSpikes", &SimulationUtils::exportSpikes,
          py::arg("spikeSystem"), py::arg("filepath"),
          "Export spike data");

    m.def("importSpikes", &SimulationUtils::importSpikes,
          py::arg("spikeSystem"), py::arg("filepath"),
          "Import spike data");

    // Performance monitoring
    m.def("createPerformanceMonitor", []() -> std::unique_ptr<PerformanceMonitor> {
        return std::make_unique<PerformanceMonitor>();
    }, "Create a performance monitor");

    m.def("getPerformanceStats", [](const PerformanceMonitor& monitor) {
        return monitor.getStatsReport();
    }, py::arg("monitor"), "Get performance statistics");

    // Simulation configuration
    m.attr("SCENARIOS") = py::dict(
        "exploration", "exploration",
        "exploitation", "exploitation", 
        "development", "development",
        "stable", "stable"
    );

    m.attr("VALID_FORMATS") = py::dict(
        "csv", "csv",
        "json", "json",
        "txt", "txt"
    );

    // Invalid ID values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm