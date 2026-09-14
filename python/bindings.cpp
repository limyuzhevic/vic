#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
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

// Version information
constexpr const char* NLM_VERSION = "1.0.0";
constexpr const char* NLM_BUILD = "Phase 6.3 - Advanced Bindings";

namespace py = pybind11;
namespace nlm {

// Brain configuration APIs
static void setBrainParameter(std::shared_ptr<Brain> brain, const std::string& key, const ConfigValue& value) {
    brain->getConfig()->set(key, value);
}

static std::optional<ConfigValue> getBrainParameter(std::shared_ptr<Brain> brain, const std::string& key) {
    return brain->getConfig()->get<ConfigValue>(key);
}

static bool loadBrainBatchConfiguration(std::shared_ptr<Brain> brain, const std::map<std::string, ConfigValue>& configs) {
    bool success = true;
    for (const auto& [key, value] : configs) {
        brain->getConfig()->set(key, value);
    }
    return success;
}

// Advanced brain operations
static size_t batchStep(std::shared_ptr<Brain> brain, size_t steps, double timestep = 0.0) {
    if (timestep == 0.0) {
        timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
    }
    for (size_t i = 0; i < steps; ++i) {
        brain->step(static_cast<SimulationStep>(i), i * timestep);
    }
    return steps;
}

static bool saveBrainStateAdvanced(std::shared_ptr<Brain> brain, const std::string& filepath, 
                                    bool compress = true, bool metadata = true) {
    try {
        return brain->save(filepath);
    } catch (const std::exception& e) {
        std::cerr << "Error saving brain state: " << e.what() << std::endl;
        return false;
    }
}

static bool loadBrainStateAdvanced(std::shared_ptr<Brain> brain, const std::string& filepath) {
    try {
        return brain->load(filepath);
    } catch (const std::exception& e) {
        std::cerr << "Error loading brain state: " << e.what() << std::endl;
        return false;
    }
}

static std::map<std::string, double> getBrainStatistics(std::shared_ptr<Brain> brain) {
    std::map<std::string, double> stats;
    stats["total_neurons"] = brain->getTotalNeuronCount();
    stats["total_synapses"] = brain->getTotalSynapseCount();
    stats["active_neurons"] = brain->getActiveNeuronCount();
    stats["firing_neurons"] = brain->getFiringNeuronCount();
    stats["total_spikes"] = brain->getTotalSpikeCount();
    stats["average_firing_rate"] = brain->getAverageFiringRate();
    stats["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
    stats["pending_spike_events"] = brain->getPendingSpikeEventCount();
    
    if (auto* workingMem = brain->getWorkingMemory()) {
        stats["working_memory_traces"] = workingMem->getActiveTraces();
    }
    
    if (auto* episodicMem = brain->getEpisodicMemory()) {
        stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
    }
    
    if (auto* dopamine = brain->getDopamine()) {
        stats["dopamine_level"] = dopamine->getLevel();
    }
    
    stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
    
    return stats;
}

// Enhanced AgentBrain APIs
static void enableAllNeuromodulators(std::shared_ptr<AgentBrain> agent, bool enable = true) {
    agent->enableRewardModulation(enable);
    agent->enableCuriosity(enable);
    // Note: Novelty is currently integrated with curiosity
    agent->enableDevelopment(enable);
    agent->enableStructuralPlasticity(enable);
}

static void setDevelopmentStage(std::shared_ptr<AgentBrain> agent, DevelopmentalStage stage) {
    if (auto* brain = agent->getBrain()) {
        brain->setDevelopmentalStage(stage);
    }
}

static std::map<std::string, double> getBrainStatus(std::shared_ptr<AgentBrain> agent) {
    std::map<std::string, double> status;
    
    if (auto* brain = agent->getBrain()) {
        auto stats = getBrainStatistics(brain);
        status.insert(stats.begin(), stats.end());
        
        // Add AgentBrain-specific status
        status["dopamine_level"] = agent->getNeuromodulationLevel();
        status["curiosity_level"] = agent->getCuriosityLevel();
        status["novelty_level"] = agent->getNoveltyLevel();
        status["prediction_error"] = agent->getPredictionError();
        status["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
    }
    
    return status;
}

// World configuration APIs
static std::shared_ptr<SimpleWorld> createAdvancedWorld(float width, float height, 
                                                          size_t visionWidth, size_t visionHeight,
                                                          float maxEnergy, float energyDecayRate,
                                                          float timestep) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, visionWidth, visionHeight);
    world->setMaxEnergy(maxEnergy);
    world->setEnergyDecayRate(energyDecayRate);
    return world;
}

static bool worldConfigure(std::shared_ptr<SimpleWorld> world, 
                           const std::map<std::string, double>& config) {
    bool success = true;
    for (const auto& [key, value] : config) {
        if (key == "max_energy") {
            world->setMaxEnergy(static_cast<float>(value));
        } else if (key == "energy_decay_rate") {
            world->setEnergyDecayRate(static_cast<float>(value));
        } else if (key == "random_seed") {
            world->setRandomSeed(static_cast<uint64_t>(value));
        }
    }
    return success;
}

// Utility functions
static std::string getVersionInfo() {
    std::ostringstream versionInfo;
    versionInfo << "NLM (Neural Learning Machine) " << NLM_VERSION << " (" << NLM_BUILD << ")";
    return versionInfo.str();
}

static double benchmarkBrain(std::shared_ptr<Brain> brain, size_t steps = 100, double timestep = 0.0) {
    if (timestep == 0.0) {
        timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    batchStep(brain, steps, timestep);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return static_cast<double>(duration.count()) / 1000000.0; // Return seconds
}

static bool validateConfig(const std::map<std::string, ConfigValue>& config) {
    bool valid = true;
    for (const auto& [key, value] : config) {
        // Basic validation - can be extended
        if (key.empty()) {
            std::cerr << "Config key cannot be empty" << std::endl;
            valid = false;
            continue;
        }
        
        // Validate value types
        if (std::holds_alternative<int>(value) && std::get<int>(value) > 1000000) {
            std::cerr << "Config key '" << key << "' has suspiciously large integer value" << std::endl;
            valid = false;
        }
        
        if (std::holds_alternative<double>(value) && std::get<double>(value) < -1e6) {
            std::cerr << "Config key '" << key << "' has suspiciously small negative double value" << std::endl;
            valid = false;
        }
    }
    return valid;
}

// Context Managers
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                      std::shared_ptr<SimpleWorld> world, bool verbose = false)
        : brain_(brain), agent_(agent), world_(world), verbose_(verbose), 
          timestep_(brain ? brain->getConfig()->getOr<double>("simulation_timestep", 0.001) : 0.001),
          step_count_(0) {
        if (verbose_) {
            std::cout << "[SimulationContext] Initialized simulation with timestep = " << timestep_ << std::endl;
        }
    }
    
    ~SimulationContext() {
        if (verbose_) {
            std::cout << "[SimulationContext] Simulation ended after " << step_count_ << " steps" << std::endl;
        }
        reset();
    }
    
    void step() {
        if (!world_ || !brain_ || !agent_) return;
        
        world_->update(timestep_);
        auto percept = world_->getSensoryPercept();
        agent_->processSensoryInput(percept);
        brain_->step(static_cast<SimulationStep>(step_count_), step_count_ * timestep_);
        auto action = agent_->decodeMotorCommand();
        world_->applyMotorCommand(action, step_count_ * timestep_);
        
        step_count_++;
        
        if (verbose_) {
            std::cout << "[SimulationContext] Step " << (step_count_ - 1) << " completed" << std::endl;
        }
    }
    
    void reset() {
        if (brain_) brain_->reset();
        if (agent_) agent_->reset();
        if (world_) world_->reset();
        step_count_ = 0;
        
        if (verbose_) {
            std::cout << "[SimulationContext] Reset all components" << std::endl;
        }
    }
    
    void setTimestep(double timestep) {
        timestep_ = timestep;
        if (verbose_) {
            std::cout << "[SimulationContext] Timestep set to " << timestep_ << std::endl;
        }
    }
    
    double getTimestep() const { return timestep_; }
    
    size_t getStepCount() const { return step_count_; }
    
    std::map<std::string, double> getStatus() const {
        std::map<std::string, double> status;
        status["step_count"] = step_count_;
        status["timestep"] = timestep_;
        
        if (brain_) {
            auto stats = getBrainStatistics(brain_);
            status.insert(stats.begin(), stats.end());
        }
        
        return status;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    double timestep_;
    size_t step_count_;
    bool verbose_;
};

class WorldContext {
public:
    WorldContext(std::shared_ptr<SimpleWorld> world, bool verbose = false)
        : world_(world), verbose_(verbose) {
        if (verbose_) {
            std::cout << "[WorldContext] World context initialized" << std::endl;
        }
    }
    
    ~WorldContext() {
        if (verbose_) {
            std::cout << "[WorldContext] World context released" << std::endl;
        }
    }
    
    void configureWorld(float width, float height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
        if (verbose_) {
            std::cout << "[WorldContext] World configured: " << width << "x" << height 
                      << ", vision: " << visionWidth << "x" << visionHeight << std::endl;
        }
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
        if (verbose_) {
            std::cout << "[WorldContext] Agent start set to (" << x << ", " << y << ")" << std::endl;
        }
    }
    
    const SensoryPercept& getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    const AgentBody& getAgentBody() const {
        return world_->getAgentBody();
    }
    
    void update(double timestep) {
        world_->update(timestep);
        if (verbose_) {
            std::cout << "[WorldContext] World updated for " << timestep << " seconds" << std::endl;
        }
    }
    
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    std::map<std::string, double> getWorldInfo() const {
        std::map<std::string, double> info;
        info["width"] = world_->getWidth();
        info["height"] = world_->getHeight();
        info["max_energy"] = world_->getMaxEnergy();
        info["energy_decay_rate"] = world_->getEnergyDecayRate();
        info["simulation_time"] = world_->getSimulationTime();
        return info;
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
    bool verbose_;
};

// Performance optimizations
static void enableVerboseLogging(bool enable) {
    if (enable) {
        Logger::setGlobal(std::make_shared<ConsoleLogger>(LogLevel::Debug));
        Logger::getGlobal().setLevel(LogLevel::Debug);
        std::cout << "[Performance] Verbose logging enabled" << std::endl;
    } else {
        Logger::getGlobal().setLevel(LogLevel::Info);
        std::cout << "[Performance] Verbose logging disabled" << std::endl;
    }
}

static void setSimulationTimestep(std::shared_ptr<Brain> brain, double timestep) {
    if (brain) {
        brain->getConfig()->set("simulation_timestep", timestep);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Advanced Edition - Includes Context Managers, Batch Operations,
        and Enhanced Configuration APIs for power users.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Version information
    m.def("get_version_info", &getVersionInfo, R"pbdoc(
        Get detailed version information about NLM.
        
        Returns:
            str: Version information string
    )pbdoc");

    // Type definitions
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

    // Brain configuration APIs
    m.def("setBrainParameter", &setBrainParameter, py::arg("brain"), py::arg("key"), py::arg("value"),
          "Configure a brain parameter");
    
    m.def("getBrainParameter", &getBrainParameter, py::arg("brain"), py::arg("key"),
          "Get a brain parameter value");
    
    m.def("loadBrainBatchConfiguration", &loadBrainBatchConfiguration, py::arg("brain"), py::arg("config"),
          "Load multiple brain configuration parameters at once");

    // Advanced brain operations
    m.def("batchStep", &batchStep, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.0,
          "Efficiently perform multiple simulation steps");
    
    m.def("saveBrainState", &saveBrainStateAdvanced, py::arg("brain"), py::arg("filepath"),
          py::arg("compress") = true, py::arg("metadata") = true,
          "Save brain state with enhanced error handling");
    
    m.def("loadBrainState", &loadBrainStateAdvanced, py::arg("brain"), py::arg("filepath"),
          "Load brain state with enhanced error handling");
    
    m.def("getBrainStatistics", &getBrainStatistics, py::arg("brain"),
          "Get comprehensive brain metrics and statistics");

    // Enhanced AgentBrain APIs
    m.def("enableAllNeuromodulators", &enableAllNeuromodulators, py::arg("agent"), py::arg("enable") = true,
          "Enable all neuromodulators (reward, curiosity, novelty, development, plasticity) at once");
    
    m.def("setDevelopmentStage", &setDevelopmentStage, py::arg("agent"), py::arg("stage"),
          "Manually set developmental stage");
    
    m.def("getBrainStatus", &getBrainStatus, py::arg("agent"),
          "Get detailed brain status information including stats and neuromodulation levels");

    // World configuration APIs
    m.def("createAdvancedWorld", &createAdvancedWorld, py::arg("width"), py::arg("height"),
          py::arg("visionWidth"), py::arg("visionHeight"), py::arg("maxEnergy") = 100.0f,
          py::arg("energyDecayRate") = 0.01f, py::arg("timestep") = 0.1,
          "Create a world with custom configuration");
    
    m.def("worldConfigure", &worldConfigure, py::arg("world"), py::arg("config"),
          "Batch world configuration");

    // Utility functions
    m.def("benchmarkBrain", &benchmarkBrain, py::arg("brain"), py::arg("steps") = 100,
          py::arg("timestep") = 0.0,
          "Benchmark brain performance over multiple steps");
    
    m.def("validateConfig", &validateConfig, py::arg("config"),
          "Validate configuration parameters");

    // Performance optimizations
    m.def("enableVerboseLogging", &enableVerboseLogging, py::arg("enable") = true,
          "Enable/disable verbose logging for debugging");
    
    m.def("setSimulationTimestep", &setSimulationTimestep, py::arg("brain"), py::arg("timestep"),
          "Set the simulation timestep");

    // Context managers
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(
        Context manager for automatic brain/step/reset management.
        
        Provides a Pythonic interface for running complete simulation
        sequences with automatic cleanup.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, 
                       std::shared_ptr<SimpleWorld>, bool>(),
             py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("verbose") = false)
        .def("step", &SimulationContext::step, "Perform one simulation step")
        .def("reset", &SimulationContext::reset, "Reset all components")
        .def("set_timestep", &SimulationContext::setTimestep, "Set the simulation timestep",
             py::arg("timestep"))
        .def("get_timestep", &SimulationContext::getTimestep, "Get the current timestep")
        .def("get_step_count", &SimulationContext::getStepCount, "Get the current step count")
        .def("get_status", &SimulationContext::getStatus, "Get current simulation status");

    py::class_<WorldContext>(m, "WorldContext", R"pbdoc(
        Context manager for automatic world lifecycle management.
        
        Provides a Pythonic interface for world configuration and management.
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>, bool>(),
             py::arg("world"), py::arg("verbose") = false)
        .def("configure_world", &WorldContext::configureWorld, "Configure world dimensions",
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("set_agent_start", &WorldContext::setAgentStart, "Set agent starting position",
             py::arg("x"), py::arg("y"))
        .def("get_sensory_percept", &WorldContext::getSensoryPercept,
             "Get current sensory percept")
        .def("get_agent_body", &WorldContext::getAgentBody, "Get current agent body state")
        .def("update", &WorldContext::update, "Update world state",
             py::arg("timestep"))
        .def("get_simulation_time", &WorldContext::getSimulationTime,
             "Get current simulation time")
        .def("get_world_info", &WorldContext::getWorldInfo,
             "Get detailed world information");

    // Existing simple API (backward compatibility)
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python-style documentation
    m.attr("__version__") = NLM_VERSION;
    m.attr("__build__") = NLM_BUILD;
}

} // namespace nlm

