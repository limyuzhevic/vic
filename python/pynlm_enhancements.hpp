// Pythonic API enhancements for NLM Python bindings
// Additional Pythonic features and utilities

#ifndef NLM_PYTHONIC_ENHANCEMENTS_H
#define NLM_PYTHONIC_ENHANCEMENTS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <tuple>

namespace py = pybind11;
namespace nlm {

// Context manager for configuration management
class ConfigContextManager {
public:
    ConfigContextManager(std::shared_ptr<Config> config) : config_(config) {}
    
    void save(const std::string& filepath) {
        if (config_) {
            config_->saveToFile(filepath);
        }
    }
    
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    void setValue(const std::string& key, const std::string& value) {
        if (config_) {
            config_->set(key, value, ConfigSource::Default);
        }
    }
    
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const {
        if (config_) {
            return config_->getOr(key, defaultValue);
        }
        return defaultValue;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Context manager for simulation
class BrainSimulationManager {
public:
    BrainSimulationManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) 
        : brain_(brain), world_(world) {}
    
    void runSteps(int steps) {
        for (int step = 0; step < steps; ++step) {
            if (brain_ && world_) {
                auto percept = world_->getSensoryPercept();
                brain_->receiveSensoryInput(percept);
                
                auto action = brain_->produceAction();
                world_->applyMotorCommand(action);
                
                brain_->step(step, step * 0.001);
                world_->update(0.001);
            }
        }
    }
    
    void runContinuous(int steps, std::function<void(int)> callback) {
        for (int step = 0; step < steps; ++step) {
            if (brain_ && world_) {
                auto percept = world_->getSensoryPercept();
                brain_->receiveSensoryInput(percept);
                
                auto action = brain_->produceAction();
                world_->applyMotorCommand(action);
                
                brain_->step(step, step * 0.001);
                world_->update(0.001);
                
                if (callback) {
                    callback(step);
                }
            }
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    void reset() {
        if (brain_) {
            brain_->reset();
        }
        if (world_) {
            world_->reset();
        }
    }
    
    std::string getStatusReport() const {
        std::stringstream ss;
        if (brain_) {
            ss << "Brain Status:" << std::endl;
            ss << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
            ss << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
            ss << "  Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
            ss << "  Firing Rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
            ss << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
            ss << std::endl;
        }
        if (world_) {
            ss << "World Status:" << std::endl;
            ss << "  Width: " << world_->getWidth() << std::endl;
            ss << "  Height: " << world_->getHeight() << std::endl;
            ss << "  Simulation Time: " << world_->getSimulationTime() << "s" << std::endl;
        }
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
};

// Agent context for managing agent behavior
class AgentContext {
public:
    AgentContext(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) 
        : agent_(agent), world_(world) {}
    
    void processPercept(const SensoryPercept& percept) {
        if (agent_) {
            agent_->processSensoryInput(percept);
        }
    }
    
    Action decodeAction() {
        if (agent_) {
            return agent_->decodeMotorCommand();
        }
        return Action();
    }
    
    void applyReward(float reward, float predictedReward = 0.0f) {
        if (agent_) {
            agent_->applyRewardModulation(reward, predictedReward);
        }
    }
    
    struct NeuromodulationState {
        float dopamine;
        float curiosity;
        float novelty;
        float predictionError;
        
        NeuromodulationState() : dopamine(0.0f), curiosity(0.0f), novelty(0.0f), predictionError(0.0f) {}
    };
    
    NeuromodulationState getNeuromodulationState() const {
        NeuromodulationState state;
        if (agent_) {
            state.dopamine = agent_->getNeuromodulationLevel();
            state.curiosity = agent_->getCuriosityLevel();
            state.novelty = agent_->getNoveltyLevel();
            state.predictionError = agent_->getPredictionError();
        }
        return state;
    }
    
    void enableAllFeatures() {
        if (agent_) {
            agent_->enableRewardModulation(true);
            agent_->enableStructuralPlasticity(true);
            agent_->enableDevelopment(true);
            agent_->enableCuriosity(true);
        }
    }
    
    void disableAllFeatures() {
        if (agent_) {
            agent_->enableRewardModulation(false);
            agent_->enableStructuralPlasticity(false);
            agent_->enableDevelopment(false);
            agent_->enableCuriosity(false);
        }
    }
    
    bool isEnabled() const {
        return agent_ != nullptr;
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

// Serialization helper functions
class SerializationHelper {
public:
    static bool saveBrainToPython(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) {
            return false;
        }
        return brain->save(filepath);
    }
    
    static std::shared_ptr<Brain> loadBrainFromPython(const std::string& filepath, std::shared_ptr<Config> config = nullptr) {
        auto brain = config ? std::make_shared<Brain>(config) : std::make_shared<Brain>(std::make_shared<Config>());
        
        if (!brain->load(filepath)) {
            return nullptr;
        }
        
        return brain;
    }
    
    static std::string brainToJson(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            return "{}";
        }
        
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"neuron_count\": " << brain->getTotalNeuronCount() << ",\n";
        ss << "  \"synapse_count\": " << brain->getTotalSynapseCount() << ",\n";
        ss << "  \"active_neurons\": " << brain->getActiveNeuronCount() << ",\n";
        ss << "  \"firing_neurons\": " << brain->getFiringNeuronCount() << ",\n";
        ss << "  \"average_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        ss << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
        ss << "  \"developmental_stage\": \"" << getDevelopmentalStageName(brain->getDevelopmentalStage()) << "\"\n";
        ss << "}";
        return ss.str();
    }
    
    static std::shared_ptr<Brain> jsonToBrain(const std::string& json, std::shared_ptr<Config> config = nullptr) {
        // Simple JSON parsing (would use a proper JSON library in production)
        // This is a placeholder implementation
        if (config) {
            return std::make_shared<Brain>(config);
        }
        return std::make_shared<Brain>(std::make_shared<Config>());
    }
    
private:
    static std::string getDevelopmentalStageName(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// Pythonic Brain wrapper for enhanced API
class PythonicBrain {
public:
    PythonicBrain() = default;
    PythonicBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Chainable methods for fluent interface
    PythonicBrain& addRegion(const std::string& name) {
        if (brain_) {
            brain_->addRegion(name);
        }
        return *this;
    }
    
    PythonicBrain& setConfiguration(const std::string& key, const std::string& value) {
        if (brain_) {
            brain_->getConfig()->set(key, value, ConfigSource::Default);
        }
        return *this;
    }
    
    PythonicBrain& enablePlasticity(bool enable) {
        if (brain_) {
            auto* region = brain_->getRegion(RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(enable, true, true);
                }
            }
        }
        return *this;
    }
    
    // Pythonic property access
    int getNeuronCount() const { return brain_ ? brain_->getTotalNeuronCount() : 0; }
    int getSynapseCount() const { return brain_ ? brain_->getTotalSynapseCount() : 0; }
    int getActiveNeuronCount() const { return brain_ ? brain_->getActiveNeuronCount() : 0; }
    float getFiringRate() const { return brain_ ? brain_->getAverageFiringRate() : 0.0f; }
    int getTotalSpikes() const { return brain_ ? brain_->getTotalSpikeCount() : 0; }
    
    // Pythonic statistics
    std::tuple<int, int, int, float, int> getStatistics() const {
        if (!brain_) {
            return std::make_tuple(0, 0, 0, 0.0f, 0);
        }
        return std::make_tuple(
            brain_->getTotalNeuronCount(),
            brain_->getTotalSynapseCount(),
            brain_->getActiveNeuronCount(),
            brain_->getAverageFiringRate(),
            brain_->getTotalSpikeCount()
        );
    }
    
    std::string getReport() const {
        if (!brain_) {
            return "No brain initialized.";
        }
        
        std::stringstream ss;
        ss << "=== NLM Brain Report ===" << std::endl;
        ss << "Neuron Count: " << brain_->getTotalNeuronCount() << std::endl;
        ss << "Synapse Count: " << brain_->getTotalSynapseCount() << std::endl;
        ss << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        ss << "Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        ss << "Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
        ss << "Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
        ss << "Developmental Stage: " << getDevelopmentalStageName(brain_->getDevelopmentalStage()) << std::endl;
        ss << "Excitation/Inhibition Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
        return ss.str();
    }
    
    // Pythonic iteration
    class RegionIterator {
    public:
        RegionIterator(std::shared_ptr<Brain> brain, size_t index) : brain_(brain), index_(index) {}
        
        bool hasNext() const {
            return brain_ && index_ < brain_->getRegionCount();
        }
        
        std::shared_ptr<NeuralRegion> next() {
            if (!hasNext()) {
                return nullptr;
            }
            auto regionIds = brain_->getRegionIds();
            if (index_ < regionIds.size()) {
                auto* region = brain_->getRegion(regionIds[index_]);
                index_++;
                return std::shared_ptr<NeuralRegion>(region);
            }
            return nullptr;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        size_t index_;
    };
    
    RegionIterator getRegionIterator() {
        return RegionIterator(brain_, 0);
    }
    
    // Pythonic filtering
    std::vector<NeuronType> getNeuronTypes() const {
        std::vector<NeuronType> types;
        if (!brain_) {
            return types;
        }
        
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) {
            return types;
        }
        
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            types.push_back(neuron->getType());
        }
        
        // Remove duplicates
        std::sort(types.begin(), types.end());
        types.erase(std::unique(types.begin(), types.end()), types.end());
        return types;
    }
    
    // Pythonic callbacks
    void onSpike(int neuronId, std::function<void(int)> callback) {
        // Would integrate with spike system
        // Placeholder implementation
    }
    
    // Pythonic memory management
    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    bool isValid() const {
        return brain_ != nullptr && brain_->getConfig() != nullptr;
    }
    
    // Pythonic string representation
    std::string toString() const {
        if (!brain_) {
            return "PythonicBrain[null]";
        }
        return "PythonicBrain[neurons=" + std::to_string(brain_->getTotalNeuronCount()) + "]";
    }
    
    // Conversion to Pythonic types
    static PythonicBrain fromBrain(std::shared_ptr<Brain> brain) {
        return PythonicBrain(brain);
    }
    
    std::shared_ptr<Brain> toBrain() const {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Register Pythonic enhancements
inline void registerPythonicEnhancements(py::module_& m) {
    // Context managers
    py::class_<ConfigContextManager>(m, "ConfigContextManager")
        .def(py::init<std::shared_ptr<Config>>())
        .def("save", &ConfigContextManager::save, "Save configuration to file")
        .def("getConfig", &ConfigContextManager::getConfig, "Get configuration")
        .def("setValue", &ConfigContextManager::setValue, "Set configuration value")
        .def("getValue", &ConfigContextManager::getValue, "Get configuration value");
    
    py::class_<BrainSimulationManager>(m, "BrainSimulationManager")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("runSteps", &BrainSimulationManager::runSteps, "Run simulation steps")
        .def("runContinuous", &BrainSimulationManager::runContinuous, "Run continuous simulation")
        .def("getBrain", &BrainSimulationManager::getBrain, "Get brain")
        .def("getWorld", &BrainSimulationManager::getWorld, "Get world")
        .def("reset", &BrainSimulationManager::reset, "Reset simulation")
        .def("getStatusReport", &BrainSimulationManager::getStatusReport, "Get status report");
    
    py::class_<AgentContext>(m, "AgentContext")
        .def(py::init<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>())
        .def("processPercept", &AgentContext::processPercept, "Process sensory percept")
        .def("decodeAction", &AgentContext::decodeAction, "Decode motor command")
        .def("applyReward", &AgentContext::applyReward, "Apply reward modulation")
        .def("getNeuromodulationState", &AgentContext::getNeuromodulationState, "Get neuromodulation state")
        .def("enableAllFeatures", &AgentContext::enableAllFeatures, "Enable all features")
        .def("disableAllFeatures", &AgentContext::disableAllFeatures, "Disable all features")
        .def("isEnabled", &AgentContext::isEnabled, "Check if enabled");
    
    // NeuromodulationState struct
    py::class_<AgentContext::NeuromodulationState>(m, "NeuromodulationState")
        .def(py::init<>())
        .def_readwrite("dopamine", &AgentContext::NeuromodulationState::dopamine)
        .def_readwrite("curiosity", &AgentContext::NeuromodulationState::curiosity)
        .def_readwrite("novelty", &AgentContext::NeuromodulationState::novelty)
        .def_readwrite("predictionError", &AgentContext::NeuromodulationState::predictionError);
    
    py::class_<SerializationHelper>(m, "SerializationHelper")
        .def_static("saveBrainToPython", &SerializationHelper::saveBrainToPython, "Save brain to Python format")
        .def_static("loadBrainFromPython", &SerializationHelper::loadBrainFromPython, "Load brain from Python format")
        .def_static("brainToJson", &SerializationHelper::brainToJson, "Convert brain to JSON")
        .def_static("jsonToBrain", &SerializationHelper::jsonToBrain, "Convert JSON to brain");
    
    py::class_<PythonicBrain>(m, "PythonicBrain")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Brain>>())
        .def("addRegion", &PythonicBrain::addRegion, "Add a neural region")
        .def("setConfiguration", &PythonicBrain::setConfiguration, "Set configuration")
        .def("enablePlasticity", &PythonicBrain::enablePlasticity, "Enable plasticity")
        .def("getNeuronCount", &PythonicBrain::getNeuronCount, "Get neuron count")
        .def("getSynapseCount", &PythonicBrain::getSynapseCount, "Get synapse count")
        .def("getActiveNeuronCount", &PythonicBrain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringRate", &PythonicBrain::getFiringRate, "Get firing rate")
        .def("getTotalSpikes", &PythonicBrain::getTotalSpikes, "Get total spike count")
        .def("getStatistics", &PythonicBrain::getStatistics, "Get statistics tuple")
        .def("getReport", &PythonicBrain::getReport, "Get formatted report")
        .def("getRegionIterator", &PythonicBrain::getRegionIterator, "Get region iterator")
        .def("getNeuronTypes", &PythonicBrain::getNeuronTypes, "Get neuron types")
        .def("onSpike", &PythonicBrain::onSpike, "Register spike callback")
        .def("reset", &PythonicBrain::reset, "Reset brain")
        .def("isValid", &PythonicBrain::isValid, "Check if brain is valid")
        .def("toString", &PythonicBrain::toString, "String representation")
        .def_static("fromBrain", &PythonicBrain::fromBrain, "Create PythonicBrain from Brain")
        .def("toBrain", &PythonicBrain::toBrain, "Convert to Brain");
    
    // Helper functions
    m.def("createDefaultConfigManager", []() {
        auto config = std::make_shared<Config>();
        return ConfigContextManager(config);
    }, "Create a default configuration manager");
    
    m.def("createSimulationManager", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        return BrainSimulationManager(brain, world);
    }, py::arg("brain"), py::arg("world"), "Create a simulation manager");
    
    m.def("createAgentContext", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        return AgentContext(agent, world);
    }, py::arg("agent"), py::arg("world"), "Create an agent context");
    
    m.def("createPythonicBrain", [](std::shared_ptr<Brain> brain) {
        return PythonicBrain(brain);
    }, py::arg("brain"), "Create a PythonicBrain from Brain");
}

// Helper function to get developmental stage name
inline std::string getDevelopmentalStageName(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

} // namespace nlm

#endif // NLM_PYTHONIC_ENHANCEMENTS_H
