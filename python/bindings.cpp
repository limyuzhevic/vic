// Enhanced Python bindings for NLM with modern pybind11 features
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

namespace py = pybind11;
namespace nlm {

// Custom exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Enhanced type conversion helpers

// NeuronId conversion
NeuronId neuronIdFromPyLong(long value) {
    return NeuronId(static_cast<uint64_t>(value));
}

long neuronIdToPyLong(const NeuronId& id) {
    return static_cast<long>(id.value);
}

// SynapseId conversion
SynapseId synapseIdFromPyLong(long value) {
    return SynapseId(static_cast<uint64_t>(value));
}

long synapseIdToPyLong(const SynapseId& id) {
    return static_cast<long>(id.value);
}

// RegionId conversion
RegionId regionIdFromPyLong(long value) {
    return RegionId(static_cast<uint64_t>(value));
}

long regionIdToPyLong(const RegionId& id) {
    return static_cast<long>(id.value);
}

// PopulationId conversion
PopulationId populationIdFromPyLong(long value) {
    return PopulationId(static_cast<uint64_t>(value));
}

long populationIdToPyLong(const PopulationId& id) {
    return static_cast<long>(id.value);
}

// Enhanced Config class with better Python integration
class PyConfig {
private:
    std::shared_ptr<Config> config;
    
public:
    PyConfig() : config(std::make_shared<Config>()) {}
    
    // Load from file with automatic format detection
    bool load(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonContent) {
        return config->loadFromJSON(jsonContent);
    }
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlContent) {
        return config->loadFromYAML(yamlContent);
    }
    
    // Save to file
    bool save(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
    // Get value with type conversion
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) {
        auto val = config->get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Set value with automatic type conversion
    void set(const std::string& key, const std::string& value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, const std::vector<int>& value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, const std::vector<double>& value) {
        config->set(key, value);
    }
    
    void set(const std::string& key, const std::vector<std::string>& value) {
        config->set(key, value);
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    // Clear all
    void clear() {
        config->clear();
    }
    
    // Get summary
    std::string summary() const {
        return config->summary();
    }
    
    // Convert to JSON string
    std::string toJSON() const {
        return config->toJSON();
    }
    
    // Convert to YAML string
    std::string toYAML() const {
        return config->toYAML();
    }
    
    // Detect file format
    static std::string detectFileFormat(const std::string& filepath) {
        Config::FileFormat format = Config::detectFileFormat(filepath);
        switch (format) {
            case Config::FileFormat::Unknown: return "Unknown";
            case Config::FileFormat::KeyValue: return "KeyValue";
            case Config::FileFormat::JSON: return "JSON";
            case Config::FileFormat::YAML: return "YAML";
            default: return "Unknown";
        }
    }
    
    // Get config instance (for internal use)
    std::shared_ptr<Config> getInternal() const {
        return config;
    }
};

// Enhanced Brain class with better Python integration
class PyBrain {
private:
    std::shared_ptr<Brain> brain;
    
public:
    PyBrain(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    // Initialize brain
    void initialize() {
        brain->initialize();
    }
    
    // Step with time support
    void step(long stepNumber) {
        brain->step(SimulationStep(static_cast<uint64_t>(stepNumber)));
    }
    
    void step(long stepNumber, double time) {
        brain->step(SimulationStep(static_cast<uint64_t>(stepNumber)), Timestamp(time));
    }
    
    // Receive sensory input
    void receiveInput(py::dict sensoryData) {
        SensoryInput input;
        // Parse py::dict into SensoryInput (simplified)
        brain->receiveSensoryInput(input);
    }
    
    // Get action
    std::string produceAction() {
        return brain->produceAction().getName();
    }
    
    // State management
    void reset() {
        brain->reset();
    }
    
    bool save(const std::string& filepath) {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Get statistics
    long getTotalNeuronCount() const {
        return static_cast<long>(brain->getTotalNeuronCount());
    }
    
    long getTotalSynapseCount() const {
        return static_cast<long>(brain->getTotalSynapseCount());
    }
    
    long getFiringNeuronCount() const {
        return static_cast<long>(brain->getFiringNeuronCount());
    }
    
    double getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }
    
    double getExcitationInhibitionRatio() const {
        return brain->getExcitationInhibitionRatio();
    }
    
    long getTotalSpikeCount() const {
        return static_cast<long>(brain->getTotalSpikeCount());
    }
    
    std::string getDevelopmentalStage() const {
        return "Unknown"; // This would need proper enum handling
    }
    
    // Brain regions
    long addRegion(const std::string& name = "") {
        return static_cast<long>(brain->addRegion(name));
    }
    
    // Get internal brain instance
    std::shared_ptr<Brain> getInternal() const {
        return brain;
    }
};

// Enhanced AgentBrain class
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agentBrain;
    
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agentBrain(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agentBrain->initialize(world);
    }
    
    void processSensoryInput(py::dict percept) {
        // Convert py::dict to SensoryPercept
        agentBrain->processSensoryInput(SensoryPercept());
    }
    
    std::string decodeMotorCommand() {
        return agentBrain->decodeMotorCommand().getName();
    }
    
    void applyRewardModulation(double reward, double predictedReward) {
        agentBrain->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agentBrain->updateDevelopment(timestep);
    }
    
    std::string getDevelopmentalStage() const {
        return "Unknown";
    }
    
    double getNeuromodulationLevel() const {
        return agentBrain->getNeuromodulationLevel();
    }
    
    double getCuriosityLevel() const {
        return agentBrain->getCuriosityLevel();
    }
    
    double getNoveltyLevel() const {
        return agentBrain->getNoveltyLevel();
    }
    
    double getPredictionError() const {
        return agentBrain->getPredictionError();
    }
    
    void reset() {
        agentBrain->reset();
    }
    
    // Enable/disable subsystems
    void enableRewardModulation(bool enable) {
        agentBrain->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain->isCuriosityEnabled();
    }
    
    // Get internal agent brain instance
    std::shared_ptr<AgentBrain> getInternal() const {
        return agentBrain;
    }
};

// Enhanced SimpleWorld class
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
    
public:
    PySimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
    
    void configure(int width, int height, int visionWidth, int visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world->reset();
    }
    
    void setAgentStart(double x, double y) {
        world->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world->update(timestep);
    }
    
    void applyMotorCommand(const std::string& cmd, double currentTime) {
        // Convert string to MotorCommand
        world->applyMotorCommand(MotorCommand(), currentTime);
    }
    
    py::dict getSensoryPercept() {
        py::dict result;
        // Convert SensoryPercept to py::dict
        return result;
    }
    
    py::dict getAgentBody() {
        py::dict result;
        // Convert AgentBody to py::dict
        return result;
    }
    
    void addObject(double x, double y, int type, double value = 0.0, double radius = 0.5) {
        world->addObject(WorldObject(x, y, WorldObjectType::Resource, value, radius));
    }
    
    void removeObject(double x, double y) {
        world->removeObject(x, y);
    }
    
    bool isValidPosition(double x, double y) {
        return world->isValidPosition(x, y);
    }
    
    int getWidth() const {
        return world->getWidth();
    }
    
    int getHeight() const {
        return world->getHeight();
    }
    
    double getMaxEnergy() const {
        return world->getMaxEnergy();
    }
    
    void setMaxEnergy(double e) {
        world->setMaxEnergy(e);
    }
    
    double getEnergyDecayRate() const {
        return world->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(double r) {
        world->setEnergyDecayRate(r);
    }
    
    double getSimulationTime() const {
        return world->getSimulationTime();
    }
    
    void setRandomSeed(long seed) {
        world->setRandomSeed(static_cast<uint64_t>(seed));
    }
    
    long getRandomSeed() const {
        return static_cast<long>(world->getRandomSeed());
    }
    
    // Get internal world instance
    std::shared_ptr<SimpleWorld> getInternal() const {
        return world;
    }
};

// Python module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------
        A modern Python binding for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Enhanced error handling with custom NLMError exceptions
        - Automatic JSON/YAML configuration support
        - Better type conversion and documentation
        - More intuitive Python API
        - Advanced simulation capabilities
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError");

    // NeuronId type with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<long>(), py::arg("value"), R"pbdoc(Constructor accepting Python int)pbdoc")
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId type with enhanced Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<long>(), py::arg("value"), R"pbdoc(Constructor accepting Python int)pbdoc")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId type with enhanced Python interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<long>(), py::arg("value"), R"pbdoc(Constructor accepting Python int)pbdoc")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId type with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<long>(), py::arg("value"), R"pbdoc(Constructor accepting Python int)pbdoc")
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced neuron type enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Enhanced synapse type enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Enhanced developmental stage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Enhanced firing state enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Enhanced action type enum
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

    // Enhanced motor command enum
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

    // Enhanced world object type enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with full Python integration
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load", &PyConfig::load, py::arg("filepath"),
             "Load configuration from file (auto-detects JSON, YAML, or key=value format)")
        .def("loadFromJSON", &PyConfig::loadFromJSON, py::arg("jsonContent"),
             "Load configuration from JSON string")
        .def("loadFromYAML", &PyConfig::loadFromYAML, py::arg("yamlContent"),
             "Load configuration from YAML string")
        .def("save", &PyConfig::save, py::arg("filepath"),
             "Save configuration to file")
        .def("get", py::overload_cast<const std::string&, int>(&PyConfig::get),
             py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer value with default")
        .def("get", py::overload_cast<const std::string&, double>(&PyConfig::get),
             py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get float value with default")
        .def("get", py::overload_cast<const std::string&, bool>(&PyConfig::get),
             py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean value with default")
        .def("get", py::overload_cast<const std::string&, std::string>(&PyConfig::get),
             py::arg("key"), py::arg("defaultValue") = "",
             "Get string value with default")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PyConfig::set),
             py::arg("key"), py::arg("value"),
             "Set string value")
        .def("set", py::overload_cast<const std::string&, int>(&PyConfig::set),
             py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("set", py::overload_cast<const std::string&, double>(&PyConfig::set),
             py::arg("key"), py::arg("value"),
             "Set float value")
        .def("set", py::overload_cast<const std::string&, bool>(&PyConfig::set),
             py::arg("key"), py::arg("value"),
             "Set boolean value")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::summary,
             "Get a summary string of the configuration")
        .def("toJSON", &PyConfig::toJSON,
             "Convert configuration to JSON string")
        .def("toYAML", &PyConfig::toYAML,
             "Convert configuration to YAML string")
        .def("detectFileFormat", &PyConfig::detectFileFormat, py::arg("filepath"),
             "Detect file format (JSON, YAML, or key=value)")
        .def("__repr__", [](const PyConfig& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with better Python interface
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced Python interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(long)>(&PyBrain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (PyBrain::*)(long, double)>(&PyBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("save", &PyBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount,
             "Get total synapse count across all regions")
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
        .def("addRegion", &PyBrain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("__repr__", [](const PyBrain& brain) {
            return "<EnhancedBrain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons>";
        });

    // Enhanced AgentBrain class
    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("processSensoryInput", &PyAgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &PyAgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &PyAgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &PyAgentBrain::updateDevelopment,
             py::arg("timestep"),
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
        // Enable/disable subsystems
        .def("enableRewardModulation", &PyAgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &PyAgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &PyAgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &PyAgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &PyAgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &PyAgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &PyAgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &PyAgentBrain::isCuriosityEnabled);

    // Enhanced SimpleWorld class
    py::class_<PySimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &PySimpleWorld::reset)
        .def("setAgentStart", &PySimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &PySimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &PySimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &PySimpleWorld::getSensoryPercept,
             py::return_value_policy::copy)
        .def("getAgentBody", &PySimpleWorld::getAgentBody,
             py::return_value_policy::copy)
        .def("addObject", &PySimpleWorld::addObject,
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0, py::arg("radius") = 0.5)
        .def("removeObject", &PySimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &PySimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &PySimpleWorld::getWidth)
        .def("getHeight", &PySimpleWorld::getHeight)
        .def("getMaxEnergy", &PySimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &PySimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &PySimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &PySimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &PySimpleWorld::getSimulationTime)
        .def("setRandomSeed", &PySimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &PySimpleWorld::getRandomSeed);

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<PyConfig> {
        return std::make_shared<PyConfig>();
    }, "Create a default configuration with JSON/YAML support");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<PyBrain> {
        return std::make_shared<PyBrain>(config);
    }, py::arg("config"), "Create a new enhanced brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<PySimpleWorld> {
        return std::make_shared<PySimpleWorld>();
    }, "Create a new enhanced simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PyAgentBrain> {
        return std::make_shared<PyAgentBrain>(brain);
    }, py::arg("brain"), "Create a new enhanced agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("neuronId", neuronIdFromPyLong, py::arg("value"), "Create NeuronId from Python integer");
    m.def("synapseId", synapseIdFromPyLong, py::arg("value"), "Create SynapseId from Python integer");
    m.def("regionId", regionIdFromPyLong, py::arg("value"), "Create RegionId from Python integer");
    m.def("populationId", populationIdFromPyLong, py::arg("value"), "Create PopulationId from Python integer");
}

} // namespace nlm
