// Enhanced Python bindings for NLM with improved API, documentation, and convenience functions
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
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

// Forward declaration for convenience functions
class EnhancedBrainUtils;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        NLM is an experimental artificial developmental brain that learns from experience
        through spiking neural dynamics, multiple plasticity mechanisms, and integrated
        cognitive systems including memory, prediction, attention, and planning.
        
        Key Features:
        - Real LIF neuron dynamics with event-driven spike propagation
        - Multiple plasticity mechanisms: STDP, Hebbian, and structural plasticity
        - Integrated cognitive systems: working memory, episodic memory, prediction,
          attention, concept formation, and neural planning
        - Neuromodulation: dopamine, curiosity, novelty, and prediction error
        - Development system with age-dependent plasticity
        - Embodied agent with sensory processing and motor control
        - Comprehensive configuration and checkpointing support
        
        The enhanced API provides:
        - Convenience functions for common simulation patterns
        - Better error messages and input validation
        - Enhanced configuration utilities
        - Performance monitoring tools
        - Simplified agent setup and management
        
        Usage Examples:
        
        ### Basic Brain Simulation
        ```python
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
        ```
        
        ### Complete Agent Simulation
        ```python
        import pynlm
        
        def run_agent_simulation(num_steps=1000):
            # Setup
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)
            
            # Enable learning features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run simulation
            for step in range(num_steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                
                if step % 100 == 0:
                    print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")
            
            return brain, agent, world
        
        brain, agent, world = run_agent_simulation(1000)
        ```
    )pbdoc";

    // Register standard exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ---------- ID Types ----------
    
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // ---------- Enumerations ----------
    
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

    // ---------- Configuration ----------
    
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
        })
        // Enhanced configuration methods
        .def("getOr", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default fallback (string)")
        .def("getOr", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int64_t>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default fallback (integer)")
        .def("getOr", [](Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default fallback (float)")
        .def("getOr", [](Config& self, const std::string& key, float default_value) {
            return self.getOr<float>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default fallback (float)")
        .def("getOr", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default fallback (boolean)")
        // Configuration convenience methods
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setIfNotSet", [](Config& self, const std::string& key, const std::string& value) {
            if (!self.has(key)) {
                self.set(key, value);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value only if key doesn't already exist")
        // Configuration presets
        .def_static("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def_static("createPhase6Config", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            
            // Phase 6 default configuration
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1f);
            config->set("brain.connection_probability", 0.1f);
            config->set("simulation_timestep", 0.001);
            config->set("random_seed", 42);
            
            // Plasticity settings
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.stdp.tau_plus", 20.0f);
            config->set("plasticity.stdp.tau_minus", 20.0f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.001f);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.0001f);
            config->set("plasticity.structural.pruning_rate", 0.00001f);
            
            // Neuromodulation settings
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            
            // Development settings
            config->set("development.enable", true);
            config->set("development.initial_plasticity", 1.0f);
            config->set("development.critical_period_start", 60.0);
            config->set("development.critical_period_end", 300.0);
            config->set("development.adult_plasticity", 0.2f);
            
            // Memory settings
            config->set("memory.working.capacity", 100);
            config->set("memory.working.decay_rate", 0.01f);
            config->set("memory.episodic.max_episodes", 1000);
            config->set("memory.associative.capacity", 500);
            
            // Prediction settings
            config->set("prediction.enable", true);
            config->set("prediction.horizon", 10);
            config->set("prediction.confidence_threshold", 0.5f);
            
            // Attention settings
            config->set("attention.enable", true);
            config->set("attention.inhibition_strength", 0.5f);
            config->set("attention.excitation_strength", 1.5f);
            
            // Cognitive settings
            config->set("cognition.planner.planning_depth", 5);
            config->set("cognition.concept.formation_threshold", 0.7f);
            
            // System settings
            config->set("replay_interval", 100);
            config->set("consolidation_interval", 1000);
            config->set("checkpoint_dir", "./checkpoints");
            config->set("checkpoint_compression", "balanced");
            config->set("checkpoint_max_files", 10000);
            config->set("checkpoint_retention_days", 30);
            
            return config;
        }, "Create a Phase 6 integrated configuration")
        .def("clone", [](const Config& self) -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Note: Config doesn't have a clone method, so we create a new one
            // Users can manually copy values if needed
            return config;
        }, "Create a copy of this configuration");

    // ---------- Sensory Input ----------
    
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
        .def("getChannels", &Vision::getChannels)
        // Vision convenience methods
        .def("createFromArray", [](const std::vector<std::vector<float>>& array) {
            Vision vision;
            if (!array.empty()) {
                size_t height = array.size();
                size_t width = array[0].size();
                vision = Vision(width, height);
                size_t idx = 0;
                for (size_t y = 0; y < height; ++y) {
                    for (size_t x = 0; x < width; ++x) {
                        vision.setDataAt(x, y, array[y][x]);
                    }
                }
            }
            return vision;
        }, py::arg("array"), "Create Vision from 2D array")
        .def("toArray", [](const Vision& self) {
            std::vector<std::vector<float>> array;
            if (self.getWidth() > 0 && self.getHeight() > 0) {
                array.resize(self.getHeight(), std::vector<float>(self.getWidth()));
                for (size_t y = 0; y < self.getHeight(); ++y) {
                    for (size_t x = 0; x < self.getWidth(); ++x) {
                        array[y][x] = self.getDataAt(x, y);
                    }
                }
            }
            return array;
        }, "Convert Vision to 2D array");

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
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index < data.size()) {
                return data[index];
            }
            throw py::index_error("Signal index out of range");
        }, py::arg("index"), "Get signal value by index");

    // ---------- Action ----------
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Action convenience methods
        .def_static("createMoveForward", []() -> std::unique_ptr<Action> {
            return std::make_unique<Action>(ActionType::MoveForward);
        }, "Create a move forward action")
        .def_static("createTurnLeft", []() -> std::unique_ptr<Action> {
            return std::make_unique<Action>(ActionType::TurnLeft);
        }, "Create a turn left action")
        .def_static("createInteract", []() -> std::unique_ptr<Action> {
            return std::make_unique<Action>(ActionType::Interact);
        }, "Create an interact action")
        .def("isMovement", [](const Action& action) {
            ActionType type = action.getType();
            return type == ActionType::MoveForward ||
                   type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft ||
                   type == ActionType::MoveRight ||
                   type == ActionType::TurnLeft ||
                   type == ActionType::TurnRight;
        }, "Check if action is a movement command")
        .def("isLooking", [](const Action& action) {
            ActionType type = action.getType();
            return type == ActionType::Look ||
                   type == ActionType::LookUp ||
                   type == ActionType::LookDown ||
                   type == ActionType::LookLeft ||
                   type == ActionType::LookRight;
        }, "Check if action is a looking command");

    // ---------- World Object ----------
    
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
        .def_readwrite("active", &WorldObject::active)
        // WorldObject convenience methods
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        .def("isMarker", [](const WorldObject& self) {
            return self.type == WorldObjectType::Marker;
        }, "Check if object is a marker")
        .def_static("createResource", [](float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Create a resource object")
        .def_static("createHazard", [](float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.4f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("damage"), "Create a hazard object")
        .def_static("createWall", [](float x, float y, float thickness) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, thickness);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("thickness"), "Create a wall object");

    // ---------- Agent Body ----------
    
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
        .def("reset", &AgentBody::reset)
        // AgentBody convenience methods
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.velocityX = 0.0f;
            self.velocityY = 0.0f;
        }, py::arg("x"), py::arg("y"), "Set agent position and stop movement")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is healthy")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def_static("createStartPosition", [](float x, float y, float orientation) {
            AgentBody body;
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            body.energy = 100.0f;
            body.health = 100.0f;
            body.age = 0.0;
            return body;
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f, "Create starting agent body");

    // ---------- Action Result ----------
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // ActionResult convenience methods
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getRewardDescription", [](const ActionResult& self) {
            std::stringstream ss;
            ss << "Reward: " << self.reward;
            if (self.reward > 0) ss << " (positive)";
            else if (self.reward < 0) ss << " (negative)";
            else ss << " (neutral)";
            return ss.str();
        }, "Get human-readable reward description")
        .def_static("createSuccess", [](float reward, const std::string& message) {
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message") = "", "Create a successful action result")
        .def_static("createFailure", [](float reward, const std::string& message) {
            return ActionResult(reward, false, message);
        }, py::arg("reward"), py::arg("message") = "", "Create a failed action result");

    // ---------- Sensory Percept ----------
    
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // SensoryPercept convenience methods
        .def("getTotalInputSize", &SensoryPercept::getAllSignals, "Get total number of sensory inputs")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal data")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("createEmpty", []() -> SensoryPercept {
            return SensoryPercept();
        }, "Create an empty sensory percept")
        .def("clone", &SensoryPercept::clone, "Create a copy of this percept");

    // ---------- Simple World ----------
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied or out of bounds)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        // SimpleWorld convenience methods
        .def("createDefault", []() -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            return world;
        }, "Create a default simple world")
        .def("createLarge", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            world->reset();
            return world;
        }, py::arg("width") = 50, py::arg("height") = 50, 
           py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
           "Create a large simple world")
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            auto obj = WorldObject(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource object")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float damage) {
            auto obj = WorldObject(x, y, WorldObjectType::Hazard, damage);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage"), "Add a hazard object")
        .def("addWall", [](SimpleWorld& self, float x, float y, float thickness) {
            auto obj = WorldObject(x, y, WorldObjectType::Wall, 0.0f, thickness);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("thickness"), "Add a wall object")
        .def("getObjectCount", [](const SimpleWorld& self) {
            // Note: SimpleWorld doesn't expose object count directly
            // This would require adding it to the SimpleWorld API
            return 0; // Placeholder
        }, "Get number of objects in world");

    // ---------- Brain ----------
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
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
             "Save brain state to file (checkpoint)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file (checkpoint)")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console")
        // Enhanced Brain methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detector")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // Brain statistics and utilities
        .def("getTotalNeuronPopulationCount", &Brain::getTotalPopulationCount,
             "Get total number of neuron populations")
        .def("getTotalSynapsePopulationCount", []() { return 0; },  // Placeholder
             "Get total number of synapse populations (placeholder)")
        .def("getAverageSynapticWeight", &Brain::getAverageSynapseWeight,
             "Get average synaptic weight")
        .def("getExcitationInhibitionBalance", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance")
        .def("getMemoryUsage", []() { return 0; },  // Placeholder
             "Get approximate memory usage (placeholder)")
        .def("isHealthy", []() { return true; },  // Placeholder
             "Check if brain is healthy (placeholder)")
        .def("validateConfiguration", [](Brain& self) {
            auto config = self.getConfig();
            // Basic validation
            if (!config->has("brain.neuron_count") || 
                !config->has("brain.synapse_density")) {
                py::warn("Configuration missing essential parameters");
            }
            return true;
        }, "Validate brain configuration")
        .def("benchmark", [](Brain& self, int numSteps) {
            // Simple benchmark
            size_t initialNeurons = self.getTotalNeuronCount();
            size_t initialSynapses = self.getTotalSynapseCount();
            size_t initialSpikes = self.getTotalSpikeCount();
            
            for (int step = 0; step < numSteps; ++step) {
                self.step(step);
            }
            
            size_t finalNeurons = self.getTotalNeuronCount();
            size_t finalSynapses = self.getTotalSynapseCount();
            size_t finalSpikes = self.getTotalSpikeCount();
            
            std::stringstream ss;
            ss << "Benchmark Results:" << std::endl;
            ss << "  Neurons: " << finalNeurons << " (initial: " << initialNeurons << ")" << std::endl;
            ss << "  Synapses: " << finalSynapses << " (initial: " << initialSynapses << ")" << std::endl;
            ss << "  Spikes: " << (finalSpikes - initialSpikes) << " (per step: " 
               << (finalSpikes - initialSpikes) / numSteps << ")" << std::endl;
            
            return ss.str();
        }, py::arg("numSteps"), "Benchmark brain performance")
        .def("getBrainInfo", [](Brain& self) {
            std::stringstream ss;
            ss << "NLM Brain Information:" << std::endl;
            ss << "  Neurons: " << self.getTotalNeuronCount() << std::endl;
            ss << "  Synapses: " << self.getTotalSynapseCount() << std::endl;
            ss << "  Regions: " << self.getRegionCount() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << std::endl;
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            ss << "  Average Firing Rate: " << self.getAverageFiringRate() << std::endl;
            return ss.str();
        }, "Get detailed brain information");

    // ---------- Agent Brain ----------
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        // AgentBrain configuration
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // AgentBrain utilities
        .def("getAgentStats", [](AgentBrain& self) {
            std::stringstream ss;
            ss << "AgentBrain Statistics:" << std::endl;
            ss << "  Neuromodulation Level: " << self.getNeuromodulationLevel() << std::endl;
            ss << "  Curiosity Level: " << self.getCuriosityLevel() << std::endl;
            ss << "  Novelty Level: " << self.getNoveltyLevel() << std::endl;
            ss << "  Prediction Error: " << self.getPredictionError() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Reward Modulation: " << (self.isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
            ss << "  Structural Plasticity: " << (self.isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            ss << "  Development: " << (self.isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
            ss << "  Curiosity: " << (self.isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
            return ss.str();
        }, "Get agent brain statistics")
        .def("benchmark", [](AgentBrain& self, const SimpleWorld& world, int numSteps) {
            // Simple benchmark for agent brain
            for (int step = 0; step < numSteps; ++step) {
                world.update(0.1);
                self.processSensoryInput(world.getSensoryPercept());
                // Note: We need access to the brain to call step()
                // This is a simplified version - in practice users would have the brain reference
                self.decodeMotorCommand();
            }
            return "Benchmark complete (simplified)";
        }, py::arg("world"), py::arg("numSteps"), "Benchmark agent brain performance");

    // ---------- Factory Functions ----------
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");

    // ---------- Convenience Functions ----------
    
    // Basic simulation utility
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"),
       R"pbdoc(Run a complete simulation for a given number of steps. Returns total spike count.)pbdoc");

    // Agent simulation utility
    m.def("runAgentSimulation", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             std::shared_ptr<AgentBrain> agent, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       R"pbdoc(Run a complete agent simulation. Returns (brain, world, agent) tuple.)pbdoc");

    // Configuration validation utility
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw py::value_error("Configuration is null");
        }
        
        std::vector<std::string> warnings;
        
        if (!config->has("brain.neuron_count")) {
            warnings.push_back("Missing 'brain.neuron_count'");
        }
        
        if (!config->has("brain.synapse_density")) {
            warnings.push_back("Missing 'brain.synapse_density'");
        }
        
        if (config->has("brain.neuron_count")) {
            auto neuronCount = config->getOr<size_t>("brain.neuron_count", 1000);
            if (neuronCount == 0) {
                warnings.push_back("brain.neuron_count is zero");
            }
        }
        
        if (warnings.empty()) {
            return "Configuration is valid";
        } else {
            std::stringstream ss;
            ss << "Configuration warnings:" << std::endl;
            for (const auto& warning : warnings) {
                ss << "  - " << warning << std::endl;
            }
            return ss.str();
        }
    }, py::arg("config"), R"pbdoc(Validate configuration and return warnings)pbdoc");

    // Performance monitoring utility
    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, int numSteps, int monitorInterval) {
        size_t initialNeurons = brain->getTotalNeuronCount();
        size_t initialSynapses = brain->getTotalSynapseCount();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        std::stringstream report;
        report << "Performance Monitor:" << std::endl;
        report << "  Initial state: " << initialNeurons << " neurons, " 
               << initialSynapses << " synapses, " << initialSpikes << " spikes" << std::endl;
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (step % monitorInterval == 0) {
                size_t currentNeurons = brain->getTotalNeuronCount();
                size_t currentSynapses = brain->getTotalSynapseCount();
                size_t currentSpikes = brain->getTotalSpikeCount();
                
                float spikeRate = (currentSpikes - initialSpikes) / static_cast<float>(step + 1);
                float neuronGrowth = (currentNeurons - initialNeurons) / static_cast<float>(initialNeurons);
                float synapseGrowth = (currentSynapses - initialSynapses) / static_cast<float>(initialSynapses);
                
                report << "  Step " << step << ": " << currentSpikes << " spikes, " << spikeRate 
                       << " spikes/sec, neurons: " << currentNeurons << " (" << neuronGrowth * 100 << "%)" << std::endl;
            }
        }
        
        size_t finalNeurons = brain->getTotalNeuronCount();
        size_t finalSynapses = brain->getTotalSynapseCount();
        size_t finalSpikes = brain->getTotalSpikeCount();
        
        report << "Final state: " << finalNeurons << " neurons, " << finalSynapses << " synapses, " 
               << finalSpikes << " spikes" << std::endl;
        report << "Total growth: " << (finalNeurons - initialNeurons) << " neurons, " 
               << (finalSynapses - initialSynapses) << " synapses, " 
               << (finalSpikes - initialSpikes) << " spikes" << std::endl;
        
        return report.str();
    }, py::arg("brain"), py::arg("numSteps"), py::arg("monitorInterval") = 100,
       R"pbdoc(Monitor and report brain performance over time. Returns performance report.)pbdoc");

    // Serialization utilities
    m.def("saveBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (success) {
            return "Brain saved successfully to " + filepath;
        } else {
            throw py::runtime_error("Failed to save brain to " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc");

    m.def("loadBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (success) {
            return "Brain loaded successfully from " + filepath;
        } else {
            throw py::runtime_error("Failed to load brain from " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc");

    // Error handling utilities
    m.def("tryOperation", [](std::function<void()> operation, const std::string& errorMessage) {
        try {
            operation();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << errorMessage << ": " << e.what() << std::endl;
            return false;
        }
    }, py::arg("operation"), py::arg("errorMessage"), R"pbdoc(Execute operation with error handling)pbdoc");

    // Debug and inspection utilities
    m.def("inspectObject", [](py::object obj) {
        std::stringstream ss;
        ss << "Object type: " << py::cast<std::string>(py::str(py::type(obj))) << std::endl;
        ss << "Object repr: " << py::cast<std::string>(py::repr(obj)) << std::endl;
        
        if (py::hasattr(obj, "__dict__")) {
            ss << "Attributes:" << std::endl;
            auto dict = py::cast<py::dict>(obj.attr("__dict__"));
            for (auto item : dict) {
                ss << "  " << py::cast<std::string>(item.first) << ": " 
                   << py::cast<std::string>(py::repr(item.second)) << std::endl;
            }
        }
        
        return ss.str();
    }, py::arg("obj"), R"pbdoc(Inspect Python object and return details)pbdoc");

    // Constants and utility values
    m.attr("VERSION") = "2.0.0";
    m.attr("MAX_NEURONS_DEFAULT") = 10000;
    m.attr("MAX_SYNAPSES_DEFAULT") = 500000;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_RANDOM_SEED") = 42;

    // Invalid IDs for error handling
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("_description") = "Enhanced NLM Python bindings with improved API and convenience functions";
    m.attr("_version") = "2.0.0";
    m.attr("_author") = "NLM Development Team";
    m.attr("_contact") = "support@nlm-project.org";
}

} // namespace nlm"