#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <utility>

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

// Forward declarations for helper classes
class SimpleWorld;
class AgentBrain;

/**
 * Configure pybind11 to automatically handle common C++ types
 * This provides better Python integration and type hints
 */

// Helper function to create Python type stubs for complex classes
void addPythonicInterface(py::module_& m);

// Pythonic Brain interface with simplified API
class PythonicBrain {
public:
    PythonicBrain(std::shared_ptr<Config> config);
    ~PythonicBrain();
    
    // Initialization
    bool initialize() { return brain_->initialize(); }
    void reset() { brain_->reset(); }
    
    // Basic simulation
    void step(int step) { brain_->step(step); }
    void step_with_time(int step, double time) { brain_->step(step, time); }
    
    // Statistics shortcuts
    size_t total_neurons() const { return brain_->getTotalNeuronCount(); }
    size_t total_synapses() const { return brain_->getTotalSynapseCount(); }
    size_t firing_neurons() const { return brain_->getFiringNeuronCount(); }
    float average_firing_rate() const { return brain_->getAverageFiringRate(); }
    double excitation_inhibition_ratio() const { return brain_->getExcitationInhibitionRatio(); }
    
    // Sensory integration
    void inject_sensory_input(const SensoryPercept& percept) {
        brain_->receiveSensoryInput(percept);
    }
    
    // Motor output
    Action produce_action() { return *brain_->produceAction(); }
    
    // State management
    void save_state(const std::string& filepath) { brain_->save(filepath); }
    bool load_state(const std::string& filepath) { return brain_->load(filepath); }
    
    // Configuration access
    std::shared_ptr<Config> get_config() const { return brain_->getConfig(); }
    
    // Development
    DevelopmentalStage developmental_stage() const { return brain_->getDevelopmentalStage(); }
    void set_developmental_stage(DevelopmentalStage stage) { brain_->setDevelopmentalStage(stage); }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for simulation episodes
class SimulationEpisode {
public:
    SimulationEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    ~SimulationEpisode();
    
    void run(int steps, double timestep = 0.1);
    double get_total_reward() const { return total_reward_; }
    int get_steps_completed() const { return steps_completed_; }
    
    // Context manager protocol
    SimulationEpisode& __enter__() { return *this; }
    void __exit__(int, const std::string&, const std::string&) {}
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    double total_reward_;
    int steps_completed_;
};

// Simplified configuration helper
class PythonConfig {
public:
    static std::shared_ptr<Config> create_default() {
        return std::make_shared<Config>();
    }
    
    static std::shared_ptr<Config> from_dict(const py::dict& config_dict);
    static py::dict to_dict(const Config& config);
    
    // Preset configurations
    static std::shared_ptr<Config> minimal() {
        auto config = create_default();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        return config;
    }
    
    static std::shared_ptr<Config> research() {
        auto config = create_default();
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        return config;
    }
    
    static std::shared_ptr<Config> large_scale() {
        auto config = create_default();
        config->set("brain.neuron_count", 10000);
        config->set("brain.region_count", 4);
        config->set("brain.connection_probability", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        return config;
    }
};

// Helper for sensory input
class SensoryHelper {
public:
    static SensoryPercept create_percept(const py::dict& data);
    static py::dict percept_to_dict(const SensoryPercept& percept);
    
    // Common percept types
    static SensoryPercept create_vision(int width, int height, int channels, 
                                       const std::vector<float>& data);
    static SensoryPercept create_audio(int sample_rate, const std::vector<float>& samples);
    static SensoryPercept create_internal(const std::vector<float>& signals);
};

// Advanced agent interface
class AdvancedAgent {
public:
    AdvancedAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    void initialize(int width = 20, int height = 20, 
                   int vision_width = 8, int vision_height = 8);
    
    // Step with automatic sensing and acting
    Action step(double timestep = 0.1);
    
    // Subsystem control
    void enable_subsystem(const std::string& name, bool enabled);
    bool is_subsystem_enabled(const std::string& name) const;
    
    // Statistics and monitoring
    double curiosity_level() const { return agent_->getCuriosityLevel(); }
    double novelty_level() const { return agent_->getNoveltyLevel(); }
    double neuromodulation_level() const { return agent_->getNeuromodulationLevel(); }
    double prediction_error() const { return agent_->getPredictionError(); }
    
    // Development tracking
    DevelopmentalStage current_stage() const { return agent_->getDevelopmentalStage(); }
    double development_progress() const;
    
    // Learning and adaptation
    void apply_reward_modulation(double reward, double prediction = 0.0);
    void update_development(double timestep);
    
    // Sensory and motor interfaces
    SensoryPercept get_sensory_percept() const { return world_->getSensoryPercept(); }
    Action decode_motor_command() { return agent_->decodeMotorCommand(); }
    
    std::shared_ptr<Brain> get_brain() const { return agent_->getBrain(); }
    std::shared_ptr<SimpleWorld> get_world() const { return world_; }
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

// Utility functions
void registerHelpers(py::module_& m);

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
    Advanced Python API for NLM (Neural Learning Machine)
    
    This module provides an enhanced, Pythonic interface to the NLM neural simulation framework.
    It builds upon the basic bindings with additional utilities, context managers,
    and simplified APIs for common use cases.
    
    Features:
    - Pythonic interfaces with properties and simplified methods
    - Context managers for simulation episodes
    - Preset configurations for different scales
    - Advanced agent interface with automatic sensing/acting
    - Enhanced sensory input utilities
    - Development and learning monitoring
    - Better error handling and documentation
    
    Usage example:
        import pynlm_advanced as nlm
        
        # Create an agent with research-grade brain
        agent = nlm.AdvancedAgent(nlm.Presets.research())
        
        # Run a simulation episode
        with nlm.SimulationEpisode(agent.get_brain(), agent.get_world()) as episode:
            episode.run(1000, 0.1)
            
        print(f"Total reward: {episode.get_total_reward()}")
    )pbdoc";
    
    // Register basic types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Neuron IDs
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc")
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
    
    // Synapse IDs
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=);
    
    // Region IDs
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index);
    
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
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Basic classes
    py::class_<Config>(m, "Config", R"pbdoc(NLM configuration system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getOr", &Config::getOr,
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default fallback")
        .def("set", &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get configuration summary");
    
    // Simplified Pythonic interfaces
    py::class_<PythonicBrain>(m, "Brain", R"pbdoc(Simplified neural brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &PythonicBrain::initialize,
             "Initialize the brain")
        .def("reset", &PythonicBrain::reset,
             "Reset brain state")
        .def("step", &PythonicBrain::step,
             py::arg("step"), "Perform simulation step")
        .def("step_with_time", &PythonicBrain::step_with_time,
             py::arg("step"), py::arg("time"),
             "Perform simulation step with timestamp")
        .def("total_neurons", &PythonicBrain::total_neurons,
             "Get total number of neurons")
        .def("total_synapses", &PythonicBrain::total_synapses,
             "Get total number of synapses")
        .def("firing_neurons", &PythonicBrain::firing_neurons,
             "Get number of currently firing neurons")
        .def("average_firing_rate", &PythonicBrain::average_firing_rate,
             "Get average firing rate across all neurons")
        .def("excitation_inhibition_ratio", &PythonicBrain::excitation_inhibition_ratio,
             "Get excitation/inhibition balance ratio")
        .def("save_state", &PythonicBrain::save_state,
             py::arg("filepath"), "Save brain state to file")
        .def("load_state", &PythonicBrain::load_state,
             py::arg("filepath"), "Load brain state from file")
        .def("developmental_stage", &PythonicBrain::developmental_stage,
             "Get current developmental stage")
        .def("set_developmental_stage", &PythonicBrain::set_developmental_stage,
             py::arg("stage"), "Set developmental stage");
    
    // Advanced agent interface
    py::class_<AdvancedAgent>(m, "AdvancedAgent", R"pbdoc(Advanced agent interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("initialize", &AdvancedAgent::initialize,
             py::arg("width") = 20, py::arg("height") = 20,
             py::arg("vision_width") = 8, py::arg("vision_height") = 8,
             "Initialize agent with world configuration")
        .def("step", &AdvancedAgent::step,
             py::arg("timestep") = 0.1,
             "Perform one simulation step (sense -> act)")
        .def("enable_subsystem", &AdvancedAgent::enable_subsystem,
             py::arg("name"), py::arg("enabled"),
             "Enable/disable agent subsystems")
        .def("is_subsystem_enabled", &AdvancedAgent::is_subsystem_enabled,
             py::arg("name"), "Check if subsystem is enabled")
        .def("curiosity_level", &AdvancedAgent::curiosity_level,
             "Get curiosity-driven exploration level")
        .def("novelty_level", &AdvancedAgent::novelty_level,
             "Get novelty detection level")
        .def("neuromodulation_level", &AdvancedAgent::neuromodulation_level,
             "Get current neuromodulation level")
        .def("prediction_error", &AdvancedAgent::prediction_error,
             "Get prediction error signal")
        .def("current_stage", &AdvancedAgent::current_stage,
             "Get current developmental stage")
        .def("development_progress", &AdvancedAgent::development_progress,
             "Get development progress as fraction")
        .def("apply_reward_modulation", &AdvancedAgent::apply_reward_modulation,
             py::arg("reward"), py::arg("prediction") = 0.0,
             "Apply reward-based neuromodulation")
        .def("update_development", &AdvancedAgent::update_development,
             py::arg("timestep"), "Update developmental state")
        .def("get_sensory_percept", &AdvancedAgent::get_sensory_percept,
             "Get current sensory perception")
        .def("decode_motor_command", &AdvancedAgent::decode_motor_command,
             "Decode motor command from neural activity")
        .def("get_brain", &AdvancedAgent::get_brain,
             "Get underlying brain object")
        .def("get_world", &AdvancedAgent::get_world,
             "Get world object");
    
    // Simulation episode (context manager)
    py::class_<SimulationEpisode>(m, "SimulationEpisode", R"pbdoc(Simulation episode with context manager)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("__enter__", &SimulationEpisode::__enter__,
             "Context manager entry")
        .def("__exit__", &SimulationEpisode::__exit__,
             "Context manager exit")
        .def("run", &SimulationEpisode::run,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run simulation for specified steps")
        .def("total_reward", &SimulationEpisode::get_total_reward,
             "Get total reward accumulated during episode")
        .def("steps_completed", &SimulationEpisode::get_steps_completed,
             "Get number of steps completed");
    
    // Configuration helpers
    py::class_<PythonConfig>(m, "Presets", R"pbdoc(Preset configuration presets)pbdoc", py::static_attribute_holder_tag())
        .def_static("minimal", &PythonConfig::minimal,
                   "Create minimal configuration (100 neurons)")
        .def_static("research", &PythonConfig::research,
                    "Create research-grade configuration (1000 neurons)")
        .def_static("large_scale", &PythonConfig::large_scale,
                   "Create large-scale configuration (10000 neurons)")
        .def_static("create_default", &PythonConfig::create_default,
                   "Create default empty configuration");
    
    // Sensory helpers
    py::class_<SensoryHelper>(m, "Sensory", R"pbdoc(Sensory input utilities)pbdoc", py::static_attribute_holder_tag())
        .def_static("create_vision", &SensoryHelper::create_vision,
                   py::arg("width"), py::arg("height"), py::arg("channels") = 3,
                   py::arg("data"), "Create vision sensory input")
        .def_static("create_audio", &SensoryHelper::create_audio,
                   py::arg("sample_rate"), py::arg("samples"),
                   "Create audio sensory input")
        .def_static("create_internal", &SensoryHelper::create_internal,
                   py::arg("signals"), "Create internal signals sensory input");
    
    // World and agent brain
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width") = 8, py::arg("vision_height") = 8,
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"), "Update world simulation")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get current sensory perception")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get agent body state")
        .def("width", &SimpleWorld::getWidth,
             "Get world width")
        .def("height", &SimpleWorld::getHeight,
             "Get world height");
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity");
    
    // Action and sensory percept
    py::class_<Action>(m, "Action", R"pbdoc(Motor action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception data)pbdoc")
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
    
    // Agent body
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
    
    // Action types
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
    
    // Motor commands
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // World object
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
    
    // Convenience factory functions
    m.def("createDefaultConfig", &PythonConfig::create_default,
          R"pbdoc(Create default configuration)pbdoc");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<PythonicBrain>(config);
    }, py::arg("config"), R"pbdoc(Create brain with configuration)pbdoc");
    
    m.def("createSimpleWorld", &SimpleWorld::configure,
          py::arg("width"), py::arg("height"),
          py::arg("vision_width") = 8, py::arg("vision_height") = 8,
          R"pbdoc(Create and configure simple world)pbdoc");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create agent brain interface)pbdoc");
    
    // Module metadata
    m.attr("__version__") = "2.0.0-advanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Advanced Python API for NLM neural simulation framework";
}}

} // namespace nlm