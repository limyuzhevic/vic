# NLM Python Bindings - Basic pybind11 module
// This module provides Python access to the core NLM (Neural Learning Machine) functionality
// It exposes the main brain interface and configuration system to Python users

#include "core/Constants.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>

namespace py = pybind11;

// Forward declarations for Python bindings
class PyBrain;

// Configuration wrapper class for Python
class PyConfig {
public:
    PyConfig() : config_(std::make_shared<nlm::Config>()) {}
    
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->template get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->template getOr<T>(key, defaultValue);
    }
    
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    // Get the underlying config for brain creation
    std::shared_ptr<nlm::Config> getConfig() {
        return config_;
    }
    
private:
    std::shared_ptr<nlm::Config> config_;
};

// Brain wrapper class for Python
class PyBrain {
public:
    PyBrain(std::shared_ptr<nlm::Config> config) {
        brain_ = std::make_shared<nlm::Brain>(config);
    }
    
    bool initialize() {
        return brain_->initialize();
    }
    
    void step(SimulationStep step) {
        brain_->step(step);
    }
    
    void step(SimulationStep step, Timestamp time) {
        brain_->step(step, time);
    }
    
    void reset() {
        brain_->reset();
    }
    
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Statistics
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    // Developmental stage
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(nlm::DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Memory systems
    nlm::NeuralWorkingMemory* getWorkingMemory() {
        return brain_->getWorkingMemory();
    }
    
    nlm::NeuralEpisodicMemory* getEpisodicMemory() {
        return brain_->getEpisodicMemory();
    }
    
    nlm::NeuralAssociativeMemory* getAssociativeMemory() {
        return brain_->getAssociativeMemory();
    }
    
    // Prediction system
    nlm::PredictionSystem* getPredictionSystem() {
        return brain_->getPredictionSystem();
    }
    
    // Cognition systems
    nlm::NeuralPlanner* getPlanner() {
        return brain_->getPlanner();
    }
    
    nlm::ConceptFormation* getConceptFormation() {
        return brain_->getConceptFormation();
    }
    
    nlm::AttentionalSelection* getAttention() {
        return brain_->getAttention();
    }
    
    // Development system
    nlm::DevelopmentSystem* getDevelopmentSystem() {
        return brain_->getDevelopmentSystem();
    }
    
    // Neuromodulation systems
    nlm::Dopamine* getDopamine() {
        return brain_->getDopamine();
    }
    
    nlm::Curiosity* getCuriosity() {
        return brain_->getCuriosity();
    }
    
    nlm::Novelty* getNovelty() {
        return brain_->getNovelty();
    }
    
    nlm::PredictionError* getPredictionErrorSignal() {
        return brain_->getPredictionErrorSignal();
    }
    
    // Configuration
    std::shared_ptr<const nlm::Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Utility methods
    void logStatus() const {
        brain_->logStatus();
    }
    
    // Get the underlying brain for direct access
    std::shared_ptr<nlm::Brain> getBrain() {
        return brain_;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
};

// Agent wrapper for Python (simplified interface)
class PyAgent {
public:
    PyAgent(std::shared_ptr<PyBrain> brain) {
        // In a full implementation, this would create the AgentBrain
        // For now, we'll provide a basic interface
        pyBrain_ = brain;
    }
    
    // Basic agent functionality
    void processSensoryInput(const std::vector<float>& percept) {
        // Process sensory input - simplified for Python
        if (auto brain = pyBrain_->getBrain()) {
            // This would normally call the actual agent processing
            // For now, just log the input
            pyBrain_->logStatus();
        }
    }
    
    std::vector<float> decodeMotorCommand() {
        // Return simple motor command (placeholder)
        std::vector<float> motorCommand(6, 0.0f);  // 6 possible actions
        return motorCommand;
    }
    
    void applyRewardModulation(float reward, float predictedReward) {
        // Apply reward modulation
    }
    
    void updateDevelopment(double timestep) {
        // Update development
    }
    
    // Configuration and status
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return pyBrain_->getDevelopmentalStage();
    }
    
    float getNeuromodulationLevel() const {
        if (auto dopamine = pyBrain_->getBrain()->getDopamine()) {
            return dopamine->getLevel();
        }
        return 0.0f;
    }
    
    float getCuriosityLevel() const {
        if (auto curiosity = pyBrain_->getBrain()->getCuriosity()) {
            // This would be implemented in the full Curiosity system
            return 0.5f;  // Placeholder
        }
        return 0.0f;
    }
    
    float getPredictionError() const {
        if (auto predError = pyBrain_->getBrain()->getPredictionErrorSignal()) {
            // This would be implemented in the full PredictionError system
            return 0.0f;  // Placeholder
        }
        return 0.0f;
    }
    
    // Subsystem enable/disable
    void enableRewardModulation(bool enable) {
        // Enable/disable reward modulation
    }
    
    void enableCuriosity(bool enable) {
        // Enable/disable curiosity
    }
    
    void enableDevelopment(bool enable) {
        // Enable/disable development
    }
    
private:
    std::shared_ptr<PyBrain> pyBrain_;
};

// Factory functions for Python
PyConfig createDefaultConfig() {
    PyConfig config;
    
    // Set default values similar to main.cpp
    config.getConfig()->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.getConfig()->set("simulation_timestep", nlm::SimulationConstants::DEFAULT_TIMESTEP, nlm::ConfigSource::Default);
    config.getConfig()->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config.getConfig()->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config.getConfig()->set("connection_probability", nlm::PlasticityConstants::HEBBIAN_THRESHOLD, nlm::ConfigSource::Default);
    
    // STDP parameters
    config.getConfig()->set("stdp_ltp_weight", nlm::PlasticityConstants::STDP_LTP_WEIGHT, nlm::ConfigSource::Default);
    config.getConfig()->set("stdp_ltd_weight", nlm::PlasticityConstants::STDP_LTD_WEIGHT, nlm::ConfigSource::Default);
    config.getConfig()->set("stdp_tau", nlm::PlasticityConstants::STDP_TIME_CONSTANT, nlm::ConfigSource::Default);
    
    // Structural plasticity parameters
    config.getConfig()->set("synaptogenesis_rate", nlm::DevelopmentConstants::SYNAPTONGENESIS_BASE_RATE, nlm::ConfigSource::Default);
    config.getConfig()->set("pruning_rate", nlm::DevelopmentConstants::PRUNING_BASE_RATE, nlm::ConfigSource::Default);
    
    return config;
}

PyBrain createBrain(PyConfig& config) {
    return PyBrain(config.getConfig());
}

PyAgent createAgentBrain(PyBrain& brain) {
    return PyAgent(std::make_shared<PyBrain>(brain));
}

// Simple world interface for Python
class PySimpleWorld {
public:
    PySimpleWorld() {
        // In a full implementation, this would create the actual world
    }
    
    void configure(int width, int height, int visionWidth, int visionHeight) {
        // Configure world dimensions
    }
    
    void reset() {
        // Reset world to initial state
    }
    
    void update(double timestep) {
        // Update world state
    }
    
    // Placeholder methods for Python interface
    std::vector<float> getSensoryPercept() {
        return std::vector<float>(256, 0.0f);  // Vision placeholder
    }
    
    void setAgentStart(float x, float y) {
        // Set agent starting position
    }
    
    void applyMotorCommand(const std::vector<float>& command, double time) {
        // Apply motor command to world
    }
    
    double getSimulationTime() const {
        return 0.0;  // Placeholder
    }
    
    // Properties
    int getWidth() const { return 20; }
    int getHeight() const { return 20; }
    float getVisionWidth() const { return 8.0f; }
    float getVisionHeight() const { return 8.0f; }
    
    float getMaxEnergy() const { return 100.0f; }
    void setMaxEnergy(float energy) { /* placeholder */ }
};

PySimpleWorld createSimpleWorld() {
    return PySimpleWorld();
}

// Enumeration wrappers for Python
void bindEnumerations(py::module& m) {
    // NeuronType
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    // DevelopmentalStage
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // ActionType
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight)
        .value("Wait", nlm::ActionType::Wait)
        .export_values();
    
    // WorldObjectType
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();
}

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM - Neural Learning Machine Python bindings";
    m.attr("__version__") = "0.1.0";
    
    // Bind enumerations
    bindEnumerations(m);
    
    // Bind configuration classes
    py::class_<PyConfig>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &PyConfig::loadFromFile)
        .def("save_to_file", &PyConfig::saveToFile)
        .def("has", &PyConfig::has)
        .def("get", &PyConfig::get<std::string>, py::arg("key"))
        .def("get_or", &PyConfig::getOr<std::string>, py::arg("key"), py::arg("default"))
        .def("set", &PyConfig::set<std::string>, py::arg("key"), py::arg("value"))
        .def("set", &PyConfig::set<int>, py::arg("key"), py::arg("value"))
        .def("set", &PyConfig::set<double>, py::arg("key"), py::arg("value"))
        .def("set", &PyConfig::set<bool>, py::arg("key"), py::arg("value"))
        .def("get_keys", &PyConfig::getKeys)
        .def("summary", &PyConfig::summary);
    
    // Bind brain classes
    py::class_<PyBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &PyBrain::initialize)
        .def("step", &PyBrain::step)
        .def("step", &PyBrain::step, py::arg("step"), py::arg("time"))
        .def("reset", &PyBrain::reset)
        .def("save", &PyBrain::save, py::arg("filepath"))
        .def("load", &PyBrain::load, py::arg("filepath"))
        .def("get_total_neuron_count", &PyBrain::getTotalNeuronCount)
        .def("get_total_synapse_count", &PyBrain::getTotalSynapseCount)
        .def("get_firing_neuron_count", &PyBrain::getFiringNeuronCount)
        .def("get_total_spike_count", &PyBrain::getTotalSpikeCount)
        .def("get_average_firing_rate", &PyBrain::getAverageFiringRate)
        .def("get_excitation_inhibition_ratio", &PyBrain::getExcitationInhibitionRatio)
        .def("get_developmental_stage", &PyBrain::getDevelopmentalStage)
        .def("set_developmental_stage", &PyBrain::setDevelopmentalStage)
        .def("log_status", &PyBrain::logStatus);
    
    // Bind agent class
    py::class_<PyAgent>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<PyBrain>>())
        .def("process_sensory_input", &PyAgent::processSensoryInput)
        .def("decode_motor_command", &PyAgent::decodeMotorCommand)
        .def("apply_reward_modulation", &PyAgent::applyRewardModulation)
        .def("update_development", &PyAgent::updateDevelopment)
        .def("get_developmental_stage", &PyAgent::getDevelopmentalStage)
        .def("get_neuromodulation_level", &PyAgent::getNeuromodulationLevel)
        .def("get_curiosity_level", &PyAgent::getCuriosityLevel)
        .def("get_prediction_error", &PyAgent::getPredictionError)
        .def("enable_reward_modulation", &PyAgent::enableRewardModulation)
        .def("enable_curiosity", &PyAgent::enableCuriosity)
        .def("enable_development", &PyAgent::enableDevelopment);
    
    // Bind world class
    py::class_<PySimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure)
        .def("reset", &PySimpleWorld::reset)
        .def("update", &PySimpleWorld::update)
        .def("get_sensory_percept", &PySimpleWorld::getSensoryPercept)
        .def("set_agent_start", &PySimpleWorld::setAgentStart)
        .def("apply_motor_command", &PySimpleWorld::applyMotorCommand)
        .def("get_simulation_time", &PySimpleWorld::getSimulationTime)
        .def_readonly("width", &PySimpleWorld::getWidth)
        .def_readonly("height", &PySimpleWorld::getHeight)
        .def_readonly("vision_width", &PySimpleWorld::getVisionWidth)
        .def_readonly("vision_height", &PySimpleWorld::getVisionHeight)
        .def_readonly("max_energy", &PySimpleWorld::getMaxEnergy);
    
    // Factory functions
    m.def("create_default_config", &createDefaultConfig, "Create default NLM configuration");
    m.def("create_brain", &createBrain, "Create and initialize a brain from config", py::arg("config"));
    m.def("create_agent_brain", &createAgentBrain, "Create agent interface for brain", py::arg("brain"));
    m.def("create_simple_world", &createSimpleWorld, "Create a simple simulation world");
    
    // Module version
    m.attr("__version__") = "0.1.0";
}