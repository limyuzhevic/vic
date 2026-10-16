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

// Python exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

// Python wrapper for Config class
class ConfigWrapper {
private:
    std::shared_ptr<Config> config_;
    
public:
    explicit ConfigWrapper(const std::shared_ptr<Config>& config) : config_(config) {
        if (!config_) {
            throw ConfigurationError("Config cannot be null");
        }
    }
    
    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Get or default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Set configuration value
    void set(const std::string& key, const py::object& value) {
        if (py::isinstance<py::int_>(value)) {
            config_->set(key, value.cast<int64_t>());
        } else if (py::isinstance<py::float_>(value)) {
            config_->set(key, value.cast<double>());
        } else if (py::isinstance<py::bool_>(value)) {
            config_->set(key, value.cast<bool>());
        } else if (py::isinstance<py::str>(value)) {
            config_->set(key, value.cast<std::string>());
        } else if (py::isinstance<py::list>(value)) {
            auto list = value.cast<std::vector<double>>();
            config_->set(key, list);
        } else if (py::isinstance<py::tuple>(value)) {
            auto tuple = value.cast<std::vector<int64_t>>();
            config_->set(key, tuple);
        } else {
            throw ConfigurationError("Unsupported value type for key: " + key);
        }
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Load from file
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Save to file
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Load from command line arguments
    bool loadFromArgs(const std::vector<std::string>& args) {
        // Convert Python list to C-style argv
        std::vector<char*> argv;
        argv.reserve(args.size() + 1);
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        return config_->loadFromArgs(static_cast<int>(argv.size()), argv.data());
    }
    
    // Configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // String representation
    std::string repr() const {
        return "<Config: " + summary() + ">";
    }
    
    // Access the underlying config for advanced usage
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
};

// Python wrapper for Brain class with better API
class BrainWrapper {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    explicit BrainWrapper(const std::shared_ptr<Brain>& brain) : brain_(brain) {
        if (!brain_) {
            throw SimulationError("Brain cannot be null");
        }
    }
    
    // Initialize brain
    void initialize() {
        if (!brain_->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
    }
    
    // Perform simulation step
    void step(const SimulationStep& currentStep) {
        brain_->step(currentStep);
    }
    
    void stepWithTime(const SimulationStep& currentStep, const Timestamp& currentTime) {
        brain_->step(currentStep, currentTime);
    }
    
    // Get statistics
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
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
    
    // Development system
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory() const {
        return brain_->getWorkingMemory();
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() const {
        return brain_->getEpisodicMemory();
    }
    
    NeuralAssociativeMemory* getAssociativeMemory() const {
        return brain_->getAssociativeMemory();
    }
    
    // Prediction system
    PredictionSystem* getPredictionSystem() const {
        return brain_->getPredictionSystem();
    }
    
    // Cognition systems
    NeuralPlanner* getPlanner() const {
        return brain_->getPlanner();
    }
    
    ConceptFormation* getConceptFormation() const {
        return brain_->getConceptFormation();
    }
    
    AttentionalSelection* getAttention() const {
        return brain_->getAttention();
    }
    
    // Neuromodulation systems
    Dopamine* getDopamine() const {
        return brain_->getDopamine();
    }
    
    Curiosity* getCuriosity() const {
        return brain_->getCuriosity();
    }
    
    Novelty* getNovelty() const {
        return brain_->getNovelty();
    }
    
    PredictionError* getPredictionError() const {
        return brain_->getPredictionErrorSignal();
    }
    
    // Region management
    RegionId addRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        auto* region = brain_->getRegion(id);
        if (!region) {
            throw SimulationError("Region not found");
        }
        return region;
    }
    
    // Reset and checkpointing
    void reset() {
        brain_->reset();
    }
    
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Logging
    void logStatus() const {
        brain_->logStatus();
    }
    
    // String representation
    std::string repr() const {
        return "<Brain: " + std::to_string(getTotalNeuronCount()) + " neurons, " + 
               std::to_string(getTotalSynapseCount()) + " synapses>";
    }
    
    // Get config
    std::shared_ptr<const Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Access the underlying brain for advanced usage
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
};

// Python wrapper for AgentBrain
class AgentBrainWrapper {
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    
public:
    explicit AgentBrainWrapper(const std::shared_ptr<AgentBrain>& agentBrain) : agentBrain_(agentBrain) {
        if (!agentBrain_) {
            throw SimulationError("AgentBrain cannot be null");
        }
    }
    
    // Initialize with world
    void initialize(const SimpleWorld& world) {
        agentBrain_->initialize(world);
    }
    
    // Process sensory input
    void processSensoryInput(const SensoryPercept& percept) {
        agentBrain_->processSensoryInput(percept);
    }
    
    // Decode motor command
    MotorCommand decodeMotorCommand() {
        return agentBrain_->decodeMotorCommand();
    }
    
    // Get sizes
    size_t getSensoryInputSize() const {
        return agentBrain_->getSensoryInputSize();
    }
    
    size_t getMotorOutputSize() const {
        return agentBrain_->getMotorOutputSize();
    }
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward) {
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Development
    void updateDevelopment(double timestep) {
        agentBrain_->updateDevelopment(timestep);
    }
    
    // Get levels
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
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain_->getDevelopmentalStage();
    }
    
    // Reset
    void reset() {
        agentBrain_->reset();
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
    
    // Get flags
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
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const {
        return agentBrain_->getBrain();
    }
    
    // String representation
    std::string repr() const {
        return "<AgentBrain: sensory_size=" + std::to_string(getSensoryInputSize()) + 
               ", motor_size=" + std::to_string(getMotorOutputSize()) + ">";
    }
};

// Python wrapper for SimpleWorld
class SimpleWorldWrapper {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    explicit SimpleWorldWrapper(const std::shared_ptr<SimpleWorld>& world) : world_(world) {
        if (!world_) {
            throw SimulationError("World cannot be null");
        }
    }
    
    // Configure world
    void configure(size_t width, size_t height, size_t visionWidth = 8, size_t visionHeight = 8) {
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
    void applyMotorCommand(const MotorCommand& cmd, Timestamp currentTime) {
        world_->applyMotorCommand(cmd, currentTime);
    }
    
    // Get percept
    const SensoryPercept& getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    // Get agent body
    const AgentBody& getAgentBody() const {
        return world_->getAgentBody();
    }
    
    // Add/remove objects
    void addObject(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    void removeObject(float x, float y) {
        world_->removeObject(x, y);
    }
    
    // Check position
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world dimensions
    size_t getWidth() const {
        return world_->getWidth();
    }
    
    size_t getHeight() const {
        return world_->getHeight();
    }
    
    // Get energy settings
    float getMaxEnergy() const {
        return world_->getMaxEnergy();
    }
    
    void setMaxEnergy(float e) {
        world_->setMaxEnergy(e);
    }
    
    float getEnergyDecayRate() const {
        return world_->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(float r) {
        world_->setEnergyDecayRate(r);
    }
    
    // Random seed
    void setRandomSeed(uint32_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint32_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    // Time
    Timestamp getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    // String representation
    std::string repr() const {
        return "<SimpleWorld: " + std::to_string(getWidth()) + "x" + std::to_string(getHeight()) + ">";
    }
};

// Convenience functions
namespace utils {
    // Create a simple simulation runner
    py::function createSimulationRunner(
        const std::shared_ptr<Brain>& brain,
        const std::shared_ptr<SimpleWorld>& world,
        const std::shared_ptr<AgentBrain>& agent,
        double timestep = 0.1,
        int maxSteps = 1000
    ) {
        return py::cpp_function([brain, world, agent, timestep, maxSteps]() {
            for (int step = 0; step < maxSteps; ++step) {
                // Update world
                world->update(timestep);
                
                // Get percept and process
                const auto& percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Brain step
                brain->step(step);
                
                // Get action and apply
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply neuromodulation based on reward
                auto body = world->getAgentBody();
                agent->applyRewardModulation(body.energy, 0.0f);
            }
        });
    }
    
    // Create a random seed
    uint32_t createRandomSeed() {
        std::random_device rd;
        return static_cast<uint32_t>(rd());
    }
    
    // Format simulation statistics
    std::string formatStats(const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "Brain Statistics:\n";
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        return ss.str();
    }
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config) {
        // Basic validation - can be extended
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (key.empty()) {
                return false;
            }
        }
        return true;
    }
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        A more Pythonic interface for the NLM C++ neural simulation framework.
        
        This module provides enhanced Python classes with better error handling,
        convenience functions, and a more intuitive API designed for Python developers.
        
        Key Features:
        - Pythonic wrapper classes (ConfigWrapper, BrainWrapper, etc.)
        - Better error handling with specific exception types
        - Convenience functions and factory methods
        - Context manager support
        - Enhanced documentation
        - Simulation orchestration helpers
    )pbdoc";
    
    // Register exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register ID types
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
    
    // Convenience function for creating default config
    m.def("createDefaultConfig", []() -> std::shared_ptr<ConfigWrapper> {
        auto config = std::make_shared<Config>();
        return std::make_shared<ConfigWrapper>(config);
    }, "Create a default configuration with Pythonic interface");
    
    // Enhanced factory functions
    m.def("createBrain", [](std::shared_ptr<ConfigWrapper> config) -> std::shared_ptr<BrainWrapper> {
        if (!config) {
            throw ConfigurationError("Config cannot be null");
        }
        auto brain = std::make_shared<Brain>(config->getConfig());
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorldWrapper> {
        auto world = std::make_shared<SimpleWorld>();
        return std::make_shared<SimpleWorldWrapper>(world);
    }, "Create a simple 2D world");
    
    m.def("createAgentBrain", [](std::shared_ptr<BrainWrapper> brain) -> std::shared_ptr<AgentBrainWrapper> {
        if (!brain) {
            throw SimulationError("Brain cannot be null");
        }
        auto agent = std::make_shared<AgentBrain>(brain->getBrain());
        return std::make_shared<AgentBrainWrapper>(agent);
    }, py::arg("brain"), "Create an agent brain interface");
    
    // Legacy compatibility - expose original classes
    py::class_<Config>(m, "ConfigLegacy", R"pbdoc(Legacy config class - use ConfigWrapper instead)pbdoc")
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
    
    py::class_<Brain>(m, "BrainLegacy", R"pbdoc(Legacy brain class - use BrainWrapper instead)pbdoc")
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
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state");
    
    py::class_<AgentBrain>(m, "AgentBrainLegacy", R"pbdoc(Legacy agent brain class - use AgentBrainWrapper instead)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command");
    
    py::class_<SimpleWorld>(m, "SimpleWorldLegacy", R"pbdoc(Legacy world class - use SimpleWorldWrapper instead)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
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
             "Get agent body state");
    
    // Utility functions
    m.def("createRandomSeed", &utils::createRandomSeed,
          "Create a random seed for reproducible simulations");
    
    m.def("formatStats", &utils::formatStats,
          py::arg("brain"), "Format brain statistics as a string");
    
    m.def("validateConfig", &utils::validateConfig,
          py::arg("config"), "Validate configuration object");
    
    // Version info
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__description__") = R"pbdoc(
        Enhanced Python bindings for NLM (Neural Learning Machine)
        
        This version provides:
        - Pythonic wrapper classes (ConfigWrapper, BrainWrapper, etc.)
        - Better error handling with specific exception types
        - Convenience functions and factory methods
        - Context manager support
        - Enhanced documentation
        - Simulation orchestration helpers
    )pbdoc";
    
    // Reserved error types for advanced error handling
    py::class_<NLMError>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<MemoryError>(m, "MemoryError", py::module_local())
        .def(py::init<const std::string&>());
    }
    
} // namespace nlm

// Additional functions for enhanced bindings
namespace pybind11 {
    template<typename T>
    py::object to_python_object(const std::optional<T>& value) {
        if (value) {
            return py::cast(*value);
        } else {
            return py::none();
        }
    }
}