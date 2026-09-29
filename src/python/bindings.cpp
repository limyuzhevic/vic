#ifndef PYBIND11_MODULE_H
#define PYBIND11_MODULE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/json.h>
#include <pybind11/iostream.h>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include "core/Constants.hpp"
#include "core/Validation/Validation.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"

namespace py = pybind11;

namespace pynlm {

// Forward declarations
class PythonicConfig;
class PythonicSensoryInput;
class PythonicAction;
class PythonicBrain;
class PythonicWorld;
class PythonicAgentBrain;

// PythonicConfig: Enhanced configuration interface
class PythonicConfig {
public:
    std::shared_ptr<nlm::Config> config_;
    
    PythonicConfig() : config_(std::make_shared<nlm::Config>()) {}
    PythonicConfig(std::shared_ptr<nlm::Config> config) : config_(config) {}
    
    // Property-style access
    void set(const std::string& key, const py::object& value) {
        config_->set(key, convertToConfigValue(value), nlm::ConfigSource::Runtime);
    }
    
    template<typename T>
    void set(const std::string& key, T value) {
        config_->set(key, convertToConfigValue(value), nlm::ConfigSource::Runtime);
    }
    
    py::object get(const std::string& key) const {
        auto value = config_->get(key);
        if (value) {
            return convertFromConfigValue(*value);
        } else {
            throw py::key_error("Key not found: " + key);
        }
    }
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        auto value = config_->get<T>(key);
        return value ? *value : defaultValue;
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    void remove(const std::string& key) {
        config_->remove(key);
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    // Configuration file operations
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Configuration validation
    bool validate() const {
        auto& validator = nlm::validation::ConfigValidator::getInstance();
        auto result = validator.validateConfig(*config_);
        return result.valid;
    }
    
    std::string getValidationErrors() const {
        auto& validator = nlm::validation::ConfigValidator::getInstance();
        return validator.collectValidationErrors(*config_);
    }
    
    // Configuration parameter validation
    template<typename T>
    bool validateParameter(const std::string& key, const T& value) const {
        // Implement parameter-specific validation
        return true;
    }
    
    // Type conversion helpers
    static nlm::ConfigValue convertToConfigValue(const py::object& obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int64_t>();
        } else if (py::isinstance<py::float>(obj)) {
            return obj.cast<double>();
        } else if (py::isinstance<py::bool_>(obj)) {
            return obj.cast<bool>();
        } else if (py::isinstance<py::str>(obj)) {
            return obj.cast<std::string>();
        } else if (py::isinstance<py::list>(obj)) {
            auto list = obj.cast<std::vector<double>>();
            return list;
        } else if (py::isinstance<py::tuple>(obj)) {
            auto tuple = obj.cast<std::vector<double>>();
            return tuple;
        } else {
            throw py::type_error("Unsupported Python type for configuration value");
        }
    }
    
    static py::object convertFromConfigValue(const nlm::ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return py::int_(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return py::float(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return py::bool_(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            return py::str(std::get<std::string>(value));
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return py::list(std::get<std::vector<double>>(value));
        } else {
            throw py::type_error("Unsupported config value type");
        }
    }
    
    // Convenience methods for common configuration parameters
    void setNeuronCount(size_t count) {
        set("brain.neuron_count", static_cast<int64_t>(count));
    }
    
    void setSimulationTimestep(double timestep) {
        set("simulation_timestep", timestep);
    }
    
    void setConnectionProbability(float prob) {
        set("connection_probability", static_cast<double>(prob));
    }
    
    size_t getNeuronCount() const {
        return get<size_t>("brain.neuron_count", 1000);
    }
    
    double getSimulationTimestep() const {
        return get<double>("simulation_timestep", 0.001);
    }
    
    float getConnectionProbability() const {
        return get<float>("connection_probability", 0.1f);
    }
};

// PythonicSensoryInput: Enhanced sensory input interface
class PythonicSensoryInput {
public:
    std::shared_ptr<nlm::SensoryInput> sensoryInput_;
    
    PythonicSensoryInput() : sensoryInput_(std::make_shared<nlm::SensoryInput>()) {}
    PythonicSensoryInput(std::shared_ptr<nlm::SensoryInput> sensoryInput) 
        : sensoryInput_(sensoryInput) {}
    
    // Vision property
    py::array getVision() const {
        const auto& vision = sensoryInput_->getVision();
        return py::array(py::cast(vision));
    }
    
    void setVision(const py::array& vision) {
        auto visionVec = vision.cast<std::vector<float>>();
        sensoryInput_->setVision(visionVec);
    }
    
    // Touch property
    py::array getTouch() const {
        const auto& touch = sensoryInput_->getTouch();
        return py::array(py::cast(touch));
    }
    
    void setTouch(const py::array& touch) {
        auto touchVec = touch.cast<std::vector<float>>();
        sensoryInput_->setTouch(touchVec);
    }
    
    // Internal property
    py::array getInternal() const {
        const auto& internal = sensoryInput_->getInternal();
        return py::array(py::cast(internal));
    }
    
    void setInternal(const py::array& internal) {
        auto internalVec = internal.cast<std::vector<float>>();
        sensoryInput_->setInternal(internalVec);
    }
    
    // Proprioception property
    py::array getProprioception() const {
        const auto& proprioception = sensoryInput_->getProprioception();
        return py::array(py::cast(proprioception));
    }
    
    void setProprioception(const py::array& proprioception) {
        auto proprioceptionVec = proprioception.cast<std::vector<float>>();
        sensoryInput_->setProprioception(proprioceptionVec);
    }
    
    // All signals property
    py::array getAllSignals() const {
        const auto& signals = sensoryInput_->getAllSignals();
        return py::array(py::cast(signals));
    }
    
    // Timestamp property
    double getTimestamp() const {
        return sensoryInput_->getTimestamp();
    }
    
    void setTimestamp(double timestamp) {
        sensoryInput_->setTimestamp(timestamp);
    }
    
    // Configuration
    void setVisionDimensions(size_t width, size_t height) {
        sensoryInput_->setVisionDimensions(width, height);
    }
    
    std::pair<size_t, size_t> getVisionDimensions() const {
        return sensoryInput_->getVisionDimensions();
    }
    
    void setTouchSize(size_t size) {
        sensoryInput_->setTouchSize(size);
    }
    
    size_t getTouchSize() const {
        return sensoryInput_->getTouchSize();
    }
    
    void setInternalSize(size_t size) {
        sensoryInput_->setInternalSize(size);
    }
    
    size_t getInternalSize() const {
        return sensoryInput_->getInternalSize();
    }
    
    void setProprioceptionSize(size_t size) {
        sensoryInput_->setProprioceptionSize(size);
    }
    
    size_t getProprioceptionSize() const {
        return sensoryInput_->getProprioceptionSize();
    }
};

// PythonicAction: Enhanced action interface
class PythonicAction {
public:
    std::shared_ptr<nlm::Action> action_;
    
    PythonicAction() : action_(std::make_shared<nlm::Action>()) {}
    PythonicAction(std::shared_ptr<nlm::Action> action) : action_(action) {}
    
    // Reward property
    float getReward() const {
        return action_->getReward();
    }
    
    void setReward(float reward) {
        action_->setReward(reward);
    }
    
    // Success property
    bool isSuccess() const {
        return action_->isSuccess();
    }
    
    void setSuccess(bool success) {
        action_->setSuccess(success);
    }
    
    // Message property
    std::string getMessage() const {
        return action_->getMessage();
    }
    
    void setMessage(const std::string& message) {
        action_->setMessage(message);
    }
    
    // Conversion to/from Python dictionary
    py::dict toDict() const {
        py::dict result;
        result["reward"] = action_->getReward();
        result["success"] = action_->isSuccess();
        result["message"] = action_->getMessage();
        return result;
    }
    
    void fromDict(const py::dict& dict) {
        if (py::hasattr(dict, "reward")) {
            setReward(py::cast<float>(dict["reward"]));
        }
        if (py::hasattr(dict, "success")) {
            setSuccess(py::cast<bool>(dict["success"]));
        }
        if (py::hasattr(dict, "message")) {
            setMessage(py::cast<std::string>(dict["message"]));
        }
    }
    
    // Action execution
    void execute() {
        action_->execute();
    }
    
    bool isExecutable() const {
        return action_->isExecutable();
    }
};

// PythonicBrain: Enhanced brain interface
class PythonicBrain {
public:
    std::shared_ptr<nlm::Brain> brain_;
    
    PythonicBrain() = default;
    PythonicBrain(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
    
    // Initialization
    bool initialize() {
        return brain_->initialize();
    }
    
    // Simulation
    void step(SimulationStep step) {
        brain_->step(step);
    }
    
    void step(SimulationStep step, Timestamp timestamp) {
        brain_->step(step, timestamp);
    }
    
    // Sensory input
    void receiveSensoryInput(const PythonicSensoryInput& sensoryInput) {
        brain_->receiveSensoryInput(*sensoryInput.sensoryInput_);
    }
    
    // Motor output
    PythonicAction produceAction() {
        return PythonicAction(brain_->produceAction());
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
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    size_t getPendingSpikeEventCount() const {
        return brain_->getPendingSpikeEventCount();
    }
    
    // State management
    void save(const std::string& filepath) {
        brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    void reset() {
        brain_->reset();
    }
    
    // Development stage
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    // Neuron and synapse access
    PythonicSensoryInput getSensoryInput() {
        return PythonicSensoryInput(brain_->getSensoryInput());
    }
    
    // Configuration
    void setConfig(const PythonicConfig& config) {
        brain_->setConfig(*config.config_);
    }
    
    PythonicConfig getConfig() const {
        return PythonicConfig(brain_->getConfig());
    }
    
    // Error handling
    bool isValid() const {
        // Implement brain validation
        return true;
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "Brain Status:\n";
        oss << "  Total Neurons: " << getTotalNeuronCount() << "\n";
        oss << "  Total Synapses: " << getTotalSynapseCount() << "\n";
        oss << "  Firing Neurons: " << getFiringNeuronCount() << "\n";
        oss << "  Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
        oss << "  Total Spikes: " << getTotalSpikeCount() << "\n";
        oss << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage());
        return oss.str();
    }
    
    // Advanced features
    // Add advanced features here
};

// PythonicWorld: Enhanced world interface
class PythonicWorld {
public:
    std::shared_ptr<nlm::SimpleWorld> world_;
    
    PythonicWorld() : world_(std::make_shared<nlm::SimpleWorld>()) {}
    PythonicWorld(std::shared_ptr<nlm::SimpleWorld> world) : world_(world) {}
    
    // Configuration
    void configure(size_t width, size_t height, 
                   size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    size_t getWidth() const { return world_->getWidth(); }
    size_t getHeight() const { return world_->getHeight(); }
    size_t getVisionWidth() const { return world_->getVisionWidth(); }
    size_t getVisionHeight() const { return world_->getVisionHeight(); }
    
    // Control
    void reset() {
        world_->reset();
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    // Agent interaction
    void setAgentStart(double x, double y) {
        world_->setAgentStart(x, y);
    }
    
    void applyMotorCommand(nlm::MotorCommand command, double currentTime) {
        world_->applyMotorCommand(command, currentTime);
    }
    
    // State access
    PythonicSensoryInput getSensoryPercept() {
        return PythonicSensoryInput(world_->getSensoryPercept());
    }
    
    PythonicAction getActionResult() {
        return PythonicAction(world_->getActionResult());
    }
    
    PythonicAction getLastAction() {
        return PythonicAction(world_->getLastAction());
    }
    
    // World objects
    void addObject(const nlm::WorldObject& object) {
        world_->addObject(object);
    }
    
    void removeObject(double x, double y) {
        world_->removeObject(x, y);
    }
    
    bool isValidPosition(double x, double y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Properties
    double getMaxEnergy() const { return world_->getMaxEnergy(); }
    void setMaxEnergy(double energy) { world_->setMaxEnergy(energy); }
    
    size_t getObjectCount() const { return world_->getObjectCount(); }
    
    // Simulation time
    void setTime(double time) { world_->setTime(time); }
    
    // Error handling
    bool isValid() const {
        // Implement world validation
        return true;
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "World Status:\n";
        oss << "  Width: " << getWidth() << "\n";
        oss << "  Height: " << getHeight() << "\n";
        oss << "  Vision Dimensions: " << getVisionWidth() << "x" << getVisionHeight() << "\n";
        oss << "  Simulation Time: " << getSimulationTime() << "s\n";
        oss << "  Objects: " << getObjectCount() << "\n";
        oss << "  Max Energy: " << getMaxEnergy();
        return oss.str();
    }
};

// PythonicAgentBrain: Enhanced agent brain interface
class PythonicAgentBrain {
public:
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    
    PythonicAgentBrain() = default;
    PythonicAgentBrain(std::shared_ptr<nlm::AgentBrain> agentBrain) 
        : agentBrain_(agentBrain) {}
    
    // Configuration
    void initialize(const PythonicWorld& world) {
        agentBrain_->initialize(*world.world_);
    }
    
    // Sensory processing
    void processSensoryInput(const PythonicSensoryInput& percept) {
        agentBrain_->processSensoryInput(*percept.sensoryInput_);
    }
    
    // Motor decoding
    nlm::MotorCommand decodeMotorCommand() {
        return agentBrain_->decodeMotorCommand();
    }
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward) {
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Development
    void updateDevelopment(double timestep) {
        agentBrain_->updateDevelopment(timestep);
    }
    
    // Configuration flags
    void enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain_->isCuriosityEnabled();
    }
    
    // Statistics
    float getNeuromodulationLevel() const {
        return agentBrain_->getNeuromodulationLevel();
    }
    
    float getCuriosityLevel() const {
        return agentBrain_->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agentBrain_->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agentBrain_->getPredictionError();
    }
    
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain_->getDevelopmentalStage();
    }
    
    // Reset
    void reset() {
        agentBrain_->reset();
    }
    
    // Brain access
    std::shared_ptr<nlm::Brain> getBrain() const {
        return agentBrain_->getBrain();
    }
    
    // Error handling
    bool isValid() const {
        // Implement agent brain validation
        return true;
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "AgentBrain Status:\n";
        oss << "  Neuromodulation: " << getNeuromodulationLevel() << "\n";
        oss << "  Curiosity: " << getCuriosityLevel() << "\n";
        oss << "  Novelty: " << getNoveltyLevel() << "\n";
        oss << "  Prediction Error: " << getPredictionError() << "\n";
        oss << "  Development Stage: " << static_cast<int>(getDevelopmentalStage());
        return oss.str();
    }
};

// Helper functions for creating enhanced objects
inline PythonicConfig createEnhancedConfig() {
    return PythonicConfig();
}

inline PythonicSensoryInput createEnhancedSensoryInput() {
    return PythonicSensoryInput();
}

inline PythonicAction createEnhancedAction() {
    return PythonicAction();
}

inline PythonicBrain createEnhancedBrain() {
    return PythonicBrain();
}

inline PythonicWorld createEnhancedWorld() {
    return PythonicWorld();
}

inline PythonicAgentBrain createEnhancedAgentBrain(const PythonicBrain& brain) {
    return PythonicAgentBrain(std::make_shared<nlm::AgentBrain>(*brain.brain_));
}

// Enhanced Python API registration
inline void registerEnhancedPythonAPI(py::module& m) {
    // PythonicConfig
    py::class_<PythonicConfig>(m, "PythonicConfig")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, const py::object&>(&PythonicConfig::set))
        .def("set", py::overload_cast<const std::string&, int64_t>(&PythonicConfig::template set<int64_t>))
        .def("set", py::overload_cast<const std::string&, double>(&PythonicConfig::template set<double>))
        .def("set", py::overload_cast<const std::string&, bool>(&PythonicConfig::template set<bool>))
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PythonicConfig::template set<std::string>))
        .def("get", py::overload_cast<const std::string&>(&PythonicConfig::get))
        .def("get", py::overload_cast<const std::string&, int64_t>(&PythonicConfig::template get<int64_t>))
        .def("get", py::overload_cast<const std::string&, double>(&PythonicConfig::template get<double>))
        .def("get", py::overload_cast<const std::string&, bool>(&PythonicConfig::template get<bool>))
        .def("get", py::overload_cast<const std::string&, std::string>(&PythonicConfig::template get<std::string>))
        .def("has", &PythonicConfig::has)
        .def("getKeys", &PythonicConfig::getKeys)
        .def("remove", &PythonicConfig::remove)
        .def("clear", &PythonicConfig::clear)
        .def("summary", &PythonicConfig::summary)
        .def("loadFromFile", &PythonicConfig::loadFromFile)
        .def("saveToFile", &PythonicConfig::saveToFile)
        .def("validate", &PythonicConfig::validate)
        .def("getValidationErrors", &PythonicConfig::getValidationErrors)
        // Convenience methods
        .def("setNeuronCount", &PythonicConfig::setNeuronCount)
        .def("setSimulationTimestep", &PythonicConfig::setSimulationTimestep)
        .def("setConnectionProbability", &PythonicConfig::setConnectionProbability)
        .def("getNeuronCount", &PythonicConfig::getNeuronCount)
        .def("getSimulationTimestep", &PythonicConfig::getSimulationTimestep)
        .def("getConnectionProbability", &PythonicConfig::getConnectionProbability);
    
    // PythonicSensoryInput
    py::class_<PythonicSensoryInput>(m, "PythonicSensoryInput")
        .def(py::init<>())
        .def("getVision", &PythonicSensoryInput::getVision)
        .def("setVision", &PythonicSensoryInput::setVision)
        .def("getTouch", &PythonicSensoryInput::getTouch)
        .def("setTouch", &PythonicSensoryInput::setTouch)
        .def("getInternal", &PythonicSensoryInput::getInternal)
        .def("setInternal", &PythonicSensoryInput::setInternal)
        .def("getProprioception", &PythonicSensoryInput::getProprioception)
        .def("setProprioception", &PythonicSensoryInput::setProprioception)
        .def("getAllSignals", &PythonicSensoryInput::getAllSignals)
        .def("getTimestamp", &PythonicSensoryInput::getTimestamp)
        .def("setTimestamp", &PythonicSensoryInput::setTimestamp)
        .def("setVisionDimensions", &PythonicSensoryInput::setVisionDimensions)
        .def("getVisionDimensions", &PythonicSensoryInput::getVisionDimensions)
        .def("setTouchSize", &PythonicSensoryInput::setTouchSize)
        .def("getTouchSize", &PythonicSensoryInput::getTouchSize)
        .def("setInternalSize", &PythonicSensoryInput::setInternalSize)
        .def("getInternalSize", &PythonicSensoryInput::getInternalSize)
        .def("setProprioceptionSize", &PythonicSensoryInput::setProprioceptionSize)
        .def("getProprioceptionSize", &PythonicSensoryInput::getProprioceptionSize);
    
    // PythonicAction
    py::class_<PythonicAction>(m, "PythonicAction")
        .def(py::init<>())
        .def("getReward", &PythonicAction::getReward)
        .def("setReward", &PythonicAction::setReward)
        .def("isSuccess", &PythonicAction::isSuccess)
        .def("setSuccess", &PythonicAction::setSuccess)
        .def("getMessage", &PythonicAction::getMessage)
        .def("setMessage", &PythonicAction::setMessage)
        .def("toDict", &PythonicAction::toDict)
        .def("fromDict", &PythonicAction::fromDict)
        .def("execute", &PythonicAction::execute)
        .def("isExecutable", &PythonicAction::isExecutable);
    
    // PythonicBrain
    py::class_<PythonicBrain>(m, "PythonicBrain")
        .def(py::init<>())
        .def("initialize", &PythonicBrain::initialize)
        .def("step", py::overload_cast<SimulationStep>(&PythonicBrain::step))
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&PythonicBrain::step))
        .def("receiveSensoryInput", &PythonicBrain::receiveSensoryInput)
        .def("produceAction", &PythonicBrain::produceAction)
        .def("getTotalNeuronCount", &PythonicBrain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &PythonicBrain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &PythonicBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &PythonicBrain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &PythonicBrain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &PythonicBrain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &PythonicBrain::getPendingSpikeEventCount)
        .def("save", &PythonicBrain::save)
        .def("load", &PythonicBrain::load)
        .def("reset", &PythonicBrain::reset)
        .def("getDevelopmentalStage", &PythonicBrain::getDevelopmentalStage)
        .def("setConfig", &PythonicBrain::setConfig)
        .def("getConfig", &PythonicBrain::getConfig)
        .def("isValid", &PythonicBrain::isValid)
        .def("getStatus", &PythonicBrain::getStatus);
    
    // PythonicWorld
    py::class_<PythonicWorld>(m, "PythonicWorld")
        .def(py::init<>())
        .def("configure", &PythonicWorld::configure)
        .def("getWidth", &PythonicWorld::getWidth)
        .def("getHeight", &PythonicWorld::getHeight)
        .def("getVisionWidth", &PythonicWorld::getVisionWidth)
        .def("getVisionHeight", &PythonicWorld::getVisionHeight)
        .def("reset", &PythonicWorld::reset)
        .def("update", &PythonicWorld::update)
        .def("getSimulationTime", &PythonicWorld::getSimulationTime)
        .def("setAgentStart", &PythonicWorld::setAgentStart)
        .def("applyMotorCommand", &PythonicWorld::applyMotorCommand)
        .def("getSensoryPercept", &PythonicWorld::getSensoryPercept)
        .def("getActionResult", &PythonicWorld::getActionResult)
        .def("getLastAction", &PythonicWorld::getLastAction)
        .def("addObject", &PythonicWorld::addObject)
        .def("removeObject", &PythonicWorld::removeObject)
        .def("isValidPosition", &PythonicWorld::isValidPosition)
        .def("getMaxEnergy", &PythonicWorld::getMaxEnergy)
        .def("setMaxEnergy", &PythonicWorld::setMaxEnergy)
        .def("getObjectCount", &PythonicWorld::getObjectCount)
        .def("setTime", &PythonicWorld::setTime)
        .def("isValid", &PythonicWorld::isValid)
        .def("getStatus", &PythonicWorld::getStatus);
    
    // PythonicAgentBrain
    py::class_<PythonicAgentBrain>(m, "PythonicAgentBrain")
        .def(py::init<>())
        .def("initialize", &PythonicAgentBrain::initialize)
        .def("processSensoryInput", &PythonicAgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &PythonicAgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &PythonicAgentBrain::applyRewardModulation)
        .def("updateDevelopment", &PythonicAgentBrain::updateDevelopment)
        .def("enableRewardModulation", &PythonicAgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &PythonicAgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &PythonicAgentBrain::enableDevelopment)
        .def("enableCuriosity", &PythonicAgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &PythonicAgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &PythonicAgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &PythonicAgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &PythonicAgentBrain::isCuriosityEnabled)
        .def("getNeuromodulationLevel", &PythonicAgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &PythonicAgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &PythonicAgentBrain::getNoveltyLevel)
        .def("getPredictionError", &PythonicAgentBrain::getPredictionError)
        .def("getDevelopmentalStage", &PythonicAgentBrain::getDevelopmentalStage)
        .def("reset", &PythonicAgentBrain::reset)
        .def("getBrain", &PythonicAgentBrain::getBrain)
        .def("isValid", &PythonicAgentBrain::isValid)
        .def("getStatus", &PythonicAgentBrain::getStatus);
    
    // Helper functions
    m.def("createEnhancedConfig", &createEnhancedConfig);
    m.def("createEnhancedSensoryInput", &createEnhancedSensoryInput);
    m.def("createEnhancedAction", &createEnhancedAction);
    m.def("createEnhancedBrain", &createEnhancedBrain);
    m.def("createEnhancedWorld", &createEnhancedWorld);
    m.def("createEnhancedAgentBrain", &createEnhancedAgentBrain);
    
    // Enhanced configuration validation
    m.def("validateConfig", [](const PythonicConfig& config) {
        return config.validate();
    });
    
    m.def("getConfigValidationErrors", [](const PythonicConfig& config) {
        return config.getValidationErrors();
    });
    
    // Error handling
    m.def("handleError", [](const py::exception& error) {
        nlm::error::ErrorHandler::handleError(error);
    });
    
    // Validation framework
    m.def("validateAll", [](const PythonicConfig& config) {
        return nlm::validation::ValidationManager::validateAll(*config.config_);
    });
    
    m.def("getAllValidationErrors", [](const PythonicConfig& config) {
        return nlm::validation::ValidationManager::getAllValidationErrors();
    });
}

} // namespace pynlm

PYBIND11_MODULE(pynlm, m) {
    // Register enhanced Python API
    pynlm::registerEnhancedPythonAPI(m);
    
    // Register basic NLM classes for backward compatibility
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("set", &nlm::Config::set)
        .def("get", &nlm::Config::get)
        .def("getOr", &nlm::Config::getOr)
        .def("has", &nlm::Config::has)
        .def("remove", &nlm::Config::remove)
        .def("getKeys", &nlm::Config::getKeys)
        .def("clear", &nlm::Config::clear);
    
    py::class_<nlm::SensoryInput>(m, "SensoryInput")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryInput::getVision)
        .def("setVision", &nlm::SensoryInput::setVision)
        .def("getTouch", &nlm::SensoryInput::getTouch)
        .def("setTouch", &nlm::SensoryInput::setTouch)
        .def("getInternal", &nlm::SensoryInput::getInternal)
        .def("setInternal", &nlm::SensoryInput::setInternal)
        .def("getProprioception", &nlm::SensoryInput::getProprioception)
        .def("setProprioception", &nlm::SensoryInput::setProprioception)
        .def("getAllSignals", &nlm::SensoryInput::getAllSignals)
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp)
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp);
    
    py::class_<nlm::Action>(m, "Action")
        .def(py::init<>())
        .def("getReward", &nlm::Action::getReward)
        .def("setReward", &nlm::Action::setReward)
        .def("isSuccess", &nlm::Action::isSuccess)
        .def("setSuccess", &nlm::Action::setSuccess)
        .def("getMessage", &nlm::Action::getMessage)
        .def("setMessage", &nlm::Action::setMessage);
    
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", py::overload_cast<SimulationStep>(&nlm::Brain::step))
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&nlm::Brain::step))
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("reset", &nlm::Brain::reset)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage);
    
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure)
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getActionResult", &nlm::SimpleWorld::getActionResult)
        .def("getLastAction", &nlm::SimpleWorld::getLastAction)
        .def("addObject", &nlm::SimpleWorld::addObject)
        .def("removeObject", &nlm::SimpleWorld::removeObject)
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy)
        .def("getObjectCount", &nlm::SimpleWorld::getObjectCount)
        .def("setTime", &nlm::SimpleWorld::setTime);
    
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("reset", &nlm::AgentBrain::reset)
        .def("getBrain", &nlm::AgentBrain::getBrain);
    
    // Enum definitions
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal);
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging);
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker);
    
    py::enum_<nlm::MotorCommand>(m, "MotorCommand")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait);
    
    // Convenience function for creating default brain
    m.def("createBrain", [](const py::object& config) {
        auto pythonicConfig = py::cast<PythonicConfig>(config);
        return PythonicBrain(std::make_shared<nlm::Brain>(pythonicConfig.config_));
    });
    
    // Convenience function for creating default world
    m.def("createSimpleWorld", []() {
        return PythonicWorld();
    });
    
    // Convenience function for creating agent brain
    m.def("createAgentBrain", [](const PythonicBrain& brain) {
        return PythonicAgentBrain(std::make_shared<nlm::AgentBrain>(brain.brain_));
    });
    
    // Error handling convenience functions
    m.def("handleError", [](const NLMError& error) {
        nlm::error::ErrorHandler::handleError(error);
    });
    
    // Configuration validation convenience functions
    m.def("validateConfig", [](const PythonicConfig& config) {
        return nlm::validation::ValidationManager::validateAll(*config.config_);
    });
    
    m.def("getAllValidationErrors", []() {
        return nlm::validation::ValidationManager::getAllValidationErrors();
    });
    
    // Statistics and reporting
    m.def("getStatistics", [](const PythonicBrain& brain) {
        std::ostringstream oss;
        oss << "Brain Statistics:\n";
        oss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        oss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        oss << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        oss << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
        oss << "  Total Spikes: " << brain.getTotalSpikeCount() << "\n";
        return oss.str();
    });
    
    m.def("getWorldStatistics", [](const PythonicWorld& world) {
        std::ostringstream oss;
        oss << "World Statistics:\n";
        oss << "  Width: " << world.getWidth() << "\n";
        oss << "  Height: " << world.getHeight() << "\n";
        oss << "  Simulation Time: " << world.getSimulationTime() << "s\n";
        oss << "  Objects: " << world.getObjectCount() << "\n";
        return oss.str();
    });
    
    m.def("getAgentBrainStatistics", [](const PythonicAgentBrain& agentBrain) {
        return agentBrain.getStatus();
    });
    
    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with type hints, validation, and improved error handling";
}

#undef PYBIND11_MODULE_H
