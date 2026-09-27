// Enhanced Python bindings for NLM
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
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

namespace py = pybind11;
namespace nlm {

// ========== PYTHON BINDINGS ENHANCEMENTS ==========

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigValidationError : public NLMError {
public:
    explicit ConfigValidationError(const std::string& msg) : NLMError("Configuration validation error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Pythonic configuration access helper
class PythonConfigHelper {
public:
    explicit PythonConfigHelper(std::shared_ptr<Config> config) : config_(config) {}
    
    // Get configuration value with default
    template<typename T>
    T get(const std::string& key, T defaultValue = T()) const {
        auto val = config_->get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> keys() const {
        return config_->getKeys();
    }
    
    // Set value with fluent API
    PythonConfigHelper& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigHelper& setInt(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigHelper& setDouble(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigHelper& setBool(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    // Load from Python dict (JSON-like)
    void loadFromDict(const py::dict& pyDict) {
        for (auto it : pyDict) {
            std::string key = py::cast<std::string>(it.first);
            py::object value = it.second;
            
            if (value.is(py::bool_())) {
                config_->set(key, value.cast<bool>());
            } else if (value.is(py::int_())) {
                config_->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config_->set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                config_->set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                py::list pyList = value.cast<py::list>();
                std::vector<double> doubleList;
                for (auto item : pyList) {
                    doubleList.push_back(item.cast<double>());
                }
                config_->set(key, doubleList);
            }
        }
    }
    
    // Convert to Python dict
    py::dict toDict() const {
        py::dict pyDict;
        for (const auto& key : config_->getKeys()) {
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                pyDict[key.c_str()] = val.value();
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    pyDict[key.c_str()] = intVal.value();
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        pyDict[key.c_str()] = doubleVal.value();
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            pyDict[key.c_str()] = boolVal.value();
                        }
                    }
                }
            }
        }
        return pyDict;
    }
    
    // Validate configuration
    std::string validate() const {
        auto result = config_->validate();
        return result.toString();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }

private:
    std::shared_ptr<Config> config_;
};

// Enhanced brain simulation with Pythonic interface
class PythonBrainWrapper {
public:
    PythonBrainWrapper(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Initialize brain with optional parameters
    bool initialize(const py::dict& optionalParams = py::dict()) {
        // Apply optional parameters
        for (auto it : optionalParams) {
            std::string key = py::cast<std::string>(it.first);
            py::object value = it.second;
            
            try {
                if (value.is(py::int_())) {
                    brain_->getConfig()->set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    brain_->getConfig()->set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    brain_->getConfig()->set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    brain_->getConfig()->set(key, value.cast<std::string>());
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to set parameter ") + key + ": " + e.what());
            }
        }
        
        return brain_->initialize();
    }
    
    // Step simulation with flexible input
    void step(py::object currentStep, py::object currentTime = py::none()) {
        SimulationStep step = py::cast<SimulationStep>(currentStep);
        
        if (currentTime.is(py::none())) {
            brain_->step(step);
        } else {
            Timestamp time = py::cast<Timestamp>(currentTime);
            brain_->step(step, time);
        }
    }
    
    // Inject sensory input from various sources
    void injectSensoryInput(py::dict inputData) {
        // Create a simple SensoryInput from dictionary
        // This would be more sophisticated in a real implementation
        std::vector<float> values;
        for (auto it : inputData) {
            py::object value = it.second;
            if (value.is(py::float_())) {
                values.push_back(value.cast<float>());
            } else if (value.is(py::int_())) {
                values.push_back(static_cast<float>(value.cast<int>()));
            }
        }
        
        // Convert to SensoryInput (simplified for bindings)
        SensoryInput input;
        input.setData(values);
        brain_->receiveSensoryInput(input);
    }
    
    // Get current simulation statistics
    py::dict getStatistics() const {
        py::dict stats;
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["region_count"] = brain_->getRegionCount();
        
        return stats;
    }
    
    // Get configuration helper
    PythonConfigHelper getConfigHelper() const {
        return PythonConfigHelper(brain_->getConfig());
    }
    
    // Get underlying brain (advanced usage)
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    // Reset simulation
    void reset() {
        brain_->reset();
    }
    
    // Save brain state
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }

private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain wrapper
class PythonAgentBrainWrapper {
public:
    PythonAgentBrainWrapper(std::shared_ptr<Brain> brain) : agentBrain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world
    void initialize(const SimpleWorld& world) {
        agentBrain_->initialize(world);
    }
    
    // Process sensory input from Python dict
    void processSensoryInput(const py::dict& perceptData) {
        // Convert dict to SensoryPercept (simplified)
        SensoryPercept percept;
        
        // Vision
        if (perceptData.contains("vision")) {
            auto visionData = perceptData["vision"].cast<std::vector<float>>();
            percept.setVision(visionData);
        }
        
        // Touch
        if (perceptData.contains("touch")) {
            auto touchData = perceptData["touch"].cast<std::vector<float>>();
            percept.setTouch(touchData);
        }
        
        // Internal
        if (perceptData.contains("internal")) {
            auto internalData = perceptData["internal"].cast<std::vector<float>>();
            percept.setInternal(internalData);
        }
        
        // Proprioception
        if (perceptData.contains("proprioception")) {
            auto proprioceptionData = perceptData["proprioception"].cast<std::vector<float>>();
            percept.setProprioception(proprioceptionData);
        }
        
        agentBrain_->processSensoryInput(percept);
    }
    
    // Decode motor command
    MotorCommand decodeMotorCommand() {
        return agentBrain_->decodeMotorCommand();
    }
    
    // Get neuromodulation levels
    py::dict getNeuromodulationLevels() const {
        py::dict levels;
        levels["dopamine"] = agentBrain_->getNeuromodulationLevel();
        levels["curiosity"] = agentBrain_->getCuriosityLevel();
        levels["novelty"] = agentBrain_->getNoveltyLevel();
        levels["prediction_error"] = agentBrain_->getPredictionError();
        return levels;
    }
    
    // Apply reward modulation
    void applyRewardModulation(float reward, float predictedReward) {
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain_->getDevelopmentalStage();
    }
    
    // Enable/disable features
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
    
    // Check feature status
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
    
    // Reset agent for new episode
    void reset() {
        agentBrain_->reset();
    }
    
    // Get underlying agent brain
    std::shared_ptr<AgentBrain> getAgentBrain() const {
        return agentBrain_;
    }

private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Pythonic world wrapper
class PythonWorldWrapper {
public:
    PythonWorldWrapper() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Reset world
    void reset() {
        world_->reset();
    }
    
    // Set agent start position
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Update world
    void update(double timestep) {
        world_->update(timestep);
    }
    
    // Apply motor command
    void applyMotorCommand(MotorCommand cmd, double currentTime) {
        world_->applyMotorCommand(cmd, currentTime);
    }
    
    // Get sensory percept
    py::dict getSensoryPercept() const {
        auto percept = world_->getSensoryPercept();
        py::dict data;
        
        // Vision
        auto vision = percept.getVision();
        if (!vision.empty()) {
            data["vision"] = vision;
        }
        
        // Touch
        auto touch = percept.getTouch();
        if (!touch.empty()) {
            data["touch"] = touch;
        }
        
        // Internal
        auto internal = percept.getInternal();
        if (!internal.empty()) {
            data["internal"] = internal;
        }
        
        // Proprioception
        auto proprioception = percept.getProprioception();
        if (!proprioception.empty()) {
            data["proprioception"] = proprioception;
        }
        
        data["timestamp"] = percept.getTimestamp();
        return data;
    }
    
    // Get agent body
    py::dict getAgentBody() const {
        auto body = world_->getAgentBody();
        py::dict data;
        data["x"] = body.x;
        data["y"] = body.y;
        data["orientation"] = body.orientation;
        data["velocityX"] = body.velocityX;
        data["velocityY"] = body.velocityY;
        data["angularVelocity"] = body.angularVelocity;
        data["energy"] = body.energy;
        data["health"] = body.health;
        data["age"] = body.age;
        data["isMoving"] = body.isMoving;
        data["isTurning"] = body.isTurning;
        data["lastActionTime"] = body.lastActionTime;
        return data;
    }
    
    // Add world object
    void addObject(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        WorldObject obj(x, y, type, value, radius);
        world_->addObject(obj);
    }
    
    // Remove object
    void removeObject(float x, float y) {
        world_->removeObject(x, y);
    }
    
    // Check valid position
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world dimensions
    py::dict getDimensions() const {
        py::dict dims;
        dims["width"] = world_->getWidth();
        dims["height"] = world_->getHeight();
        dims["visionWidth"] = world_->getVisionWidth();
        dims["visionHeight"] = world_->getVisionHeight();
        return dims;
    }
    
    // Get simulation time
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    // Set random seed
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    // Get random seed
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world_;
    }

private:
    std::shared_ptr<SimpleWorld> world_;
};

// Pythonic factory functions
inline std::shared_ptr<Config> createConfig() {
    return std::make_shared<Config>();
}

inline std::shared_ptr<PythonConfigHelper> createConfigHelper(std::shared_ptr<Config> config) {
    return std::make_shared<PythonConfigHelper>(config);
}

inline std::shared_ptr<PythonBrainWrapper> createBrain(std::shared_ptr<Config> config) {
    return std::make_shared<PythonBrainWrapper>(config);
}

inline std::shared_ptr<PythonWorldWrapper> createWorld() {
    return std::make_shared<PythonWorldWrapper>();
}

inline std::shared_ptr<PythonAgentBrainWrapper> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<PythonAgentBrainWrapper>(brain);
}

// Main module binding function
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        A comprehensive Python interface for the NLM neural simulation framework.
        Provides advanced configuration management, simulation control,
        and enhanced error handling for both beginners and advanced users.
    )pbdoc";
    
    // Register exception types
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    
    // Bind NeuronId
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
    
    // Bind SynapseId
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
    
    // Bind RegionId
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
    
    // Bind PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    // Bind enumerations
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
    
    // Enhanced Config class with Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromDict", [](Config& self, const py::dict& pyDict) {
            // Convert Python dict to config entries
            for (auto it : pyDict) {
                std::string key = py::cast<std::string>(it.first);
                py::object value = it.second;
                
                if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::int_())) {
                    self.set(key, value.cast<int64_t>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    py::list pyList = value.cast<py::list>();
                    std::vector<double> doubleList;
                    for (auto item : pyList) {
                        doubleList.push_back(item.cast<double>());
                    }
                    self.set(key, doubleList);
                }
            }
        }, py::arg("dict"), "Load configuration from Python dictionary")
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
        .def("validate", &Config::validate,
             "Validate current configuration against schema")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments (C++ main)")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::cast_error("Unsupported type in configuration value");
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with type conversion")
        .def("get", [](const Config& self, const std::string& key) {
            // Try all supported types
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) return py::cast(stringVal.value());
            
            auto intVal = self.get<int64_t>(key);
            if (intVal.has_value()) return py::cast(intVal.value());
            
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) return py::cast(doubleVal.value());
            
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) return py::cast(boolVal.value());
            
            return py::none();
        }, py::arg("key"), "Get configuration value (returns Python object)")
        .def("getOr", [](const Config& self, const std::string& key, py::object defaultValue) {
            // Get value or return default (Python object)
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) return stringVal.value();
            
            auto intVal = self.get<int64_t>(key);
            if (intVal.has_value()) return intVal.value();
            
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) return doubleVal.value();
            
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) return boolVal.value();
            
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or return default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("setData", [](SensoryInput& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set sensory data from vector");
    
    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as flat vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as flat vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
    
    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
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
        });
    
    // Enhanced WorldObject class
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
    
    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
    
    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
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
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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
    
    // Enhanced Brain class
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
             "Log brain status");
    
    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
    
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createConfigHelper", [](std::shared_ptr<Config> config) -> std::shared_ptr<PythonConfigHelper> {
        return std::make_shared<PythonConfigHelper>(config);
    }, py::arg("config"), "Create configuration helper with fluent API");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<PythonBrainWrapper> {
        return std::make_shared<PythonBrainWrapper>(config);
    }, py::arg("config"), "Create an enhanced brain simulation");
    
    m.def("createWorld", []() -> std::shared_ptr<PythonWorldWrapper> {
        return std::make_shared<PythonWorldWrapper>();
    }, "Create an enhanced world simulation");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PythonAgentBrainWrapper> {
        return std::make_shared<PythonAgentBrainWrapper>(brain);
    }, py::arg("brain"), "Create an enhanced agent brain interface");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Example usage and utilities
    m.def("example_basic_usage", []() {
        std::cout << R"PYTHON
# Basic NLM usage example
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStatistics()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")

brain.reset()
PYTHON" << std::endl;
    }, "Show basic usage example");
    
    m.def("example_advanced_usage", []() {
        std::cout << R"PYTHON
# Advanced NLM usage example
import pynlm
import json

# Create configuration with advanced settings
config_dict = {
    "neuron_count": 5000,
    "simulation_timestep": 0.0005,
    "region_count": 4,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.025,
    "log_level": "DEBUG",
    "log_to_file": True,
    "visualization_enabled": False
}

config = pynlm.createDefaultConfig()
config.loadFromDict(config_dict)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced step with time
for step in range(500):
    brain.step(step, step * 0.001)

# Use agent brain with world
world = pynlm.createWorld()
world.configure(100, 100, 16, 16)
world.setAgentStart(50, 50)

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Process sensory input
percept_data = {
    "vision": [0.1, 0.5, 0.9] * 16 * 16,
    "touch": [0.0, 0.3, 0.7],
    "internal": [0.5, -0.2, 0.8],
    "proprioception": [1.0, 0.0, 0.5]
}
agent_brain.processSensoryInput(percept_data)

# Decode action
action = agent_brain.decodeMotorCommand()

# Get world state
body = world.getAgentBody()
print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
print(f"Action: {action}")

brain.reset()
world.reset()
PYTHON" << std::endl;
    }, "Show advanced usage example");
    
    m.def("version", []() {
        return "pynlm 0.1.0 (Enhanced)";
    }, "Get version information");
}

} // namespace nlm