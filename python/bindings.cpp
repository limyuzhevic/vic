#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

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

// Python-friendly exception types for better error handling
class PythonRuntimeError : public std::runtime_error {
public:
    PythonRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PythonValueError : public std::runtime_error {
public:
    PythonValueError(const std::string& msg) : std::runtime_error(msg) {}
};

class PythonConfigurationError : public std::runtime_error {
public:
    PythonConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for Python-friendly APIs
namespace python_bindings {
    
    // Python-friendly brain configuration builder
    std::shared_ptr<Config> create_config_with_settings(
        uint64_t neuron_count = 1000,
        uint64_t region_count = 1,
        float synapse_density = 0.1f,
        float random_seed = 42.0f
    ) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(region_count), nlm::ConfigSource::Default);
        config->set("connection_probability", synapse_density, nlm::ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(random_seed), nlm::ConfigSource::Default);
        return config;
    }
    
    // Helper for running a complete simulation episode
    void run_simulation_episode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        uint64_t num_steps,
        bool enable_learning = false
    ) {
        if (!brain || !world || !agent) {
            throw PythonRuntimeError("Invalid simulation components - all must be non-null");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
            try {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward if configured
                if (enable_learning) {
                    float reward = 0.0f;
                    const auto& internal = percept.getInternal();
                    if (!internal.empty()) {
                        reward = internal[0];
                    }
                    agent->applyRewardModulation(reward, 0.0f);
                }
                
            } catch (const std::exception& e) {
                throw PythonRuntimeError(std::string("Simulation error at step ") + 
                                       std::to_string(step) + ": " + e.what());
            }
        }
    }
    
    // Convert C++ vector to Python list with optional filtering
    template<typename T>
    py::list vector_to_list(const std::vector<T>& vec, 
                           std::function<bool(const T&)> filter = nullptr) {
        py::list result;
        for (const auto& item : vec) {
            if (!filter || filter(item)) {
                result.append(item);
            }
        }
        return result;
    }
    
    // Get subset of neurons by type
    std::vector<Neuron*> get_neurons_by_type(std::shared_ptr<Brain> brain, NeuronType type) {
        if (!brain) throw PythonRuntimeError("Brain is null");
        std::vector<Neuron*> result;
        for (const auto& region : brain->getRegions()) {
            for (auto* pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        result.push_back(neuron);
                    }
                }
            }
        }
        return result;
    }
    
    // Get average firing rate by neuron type
    float get_average_firing_rate_by_type(std::shared_ptr<Brain> brain, NeuronType type) {
        auto neurons = get_neurons_by_type(brain, type);
        if (neurons.empty()) return 0.0f;
        
        float total_rate = 0.0f;
        for (Neuron* neuron : neurons) {
            total_rate += neuron->getFiringRate();
        }
        return total_rate / static_cast<float>(neurons.size());
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Pythonic APIs with convenience methods
        - Enhanced error handling with descriptive Python exceptions
        - Simulation helpers for common use cases
        - Type-safe configuration builders
        - Performance monitoring utilities
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<PythonRuntimeError>(m, "RuntimeError");
    py::register_exception<PythonValueError>(m, "ValueError");
    py::register_exception<PythonConfigurationError>(m, "ConfigurationError");

    // ===================================================================
    // CONFIGURATION SYSTEM
    // ===================================================================
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Manages brain parameters, settings, and configuration values.
        Provides type-safe access to configuration keys and values.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    // Factory function for creating configured configs
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createConfigWithSettings", &python_bindings::create_config_with_settings,
          py::arg("neuron_count") = 1000,
          py::arg("region_count") = 1,
          py::arg("synapse_density") = 0.1f,
          py::arg("random_seed") = 42.0f,
          "Create a configured config with specified parameters");

    // ===================================================================
    // CORE TYPES AND IDENTIFIERS
    // ===================================================================
    
    // NeuronId class - now with Python-friendly methods
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides read-only access to neuron ID and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &NeuronId::value)
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

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations with Python-friendly access
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

    // ===================================================================
    // SENSORY INPUT SYSTEM
    // ===================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Provides interface for accessing sensory data like vision, audio, and internal signals.
    )pbdoc")
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
        .def("getWidthHeight", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get (width, height) tuple");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getSampleCount", &Audio::getNumSamples, "Alias for getNumSamples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount);

    // ===================================================================
    // MOTOR SYSTEM
    // ===================================================================
    
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
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + 
                   std::to_string(action.getParameters().size()) + ">";
        });

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

    // ===================================================================
    // WORLD SYSTEM
    // ===================================================================
    
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
        .def("__repr__", [](const WorldObject& obj) {
            std::string type_name;
            switch (obj.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
                default: type_name = "Unknown";
            }
            return "<WorldObject: " + type_name + " at (" + std::to_string(obj.x) + "," + 
                   std::to_string(obj.y) + ") radius=" + std::to_string(obj.radius) + ">";
        });

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
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get (x, y) position tuple")
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get (vx, vy) velocity tuple");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides environment for agent interaction, sensory input, and action execution.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor properties")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not a wall)")
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
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getVisionSize", [](const SimpleWorld& world) {
            return std::make_pair(world.getWidth() / 2, world.getHeight() / 2);
        }, "Get vision dimensions (approximately)")
        .def("getAgentPosition", [](const SimpleWorld& world) {
            auto body = world.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getResourceCount", &SimpleWorld::getResourceCount,
             "Get number of resources in world")
        .def("getHazardCount", &SimpleWorld::getHazardCount,
             "Get number of hazards in world");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ===================================================================
    // AGENT BRAIN INTERFACE
    // ===================================================================
    
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
        .def("getText", &SensoryPercept::getText)
        .def("setText", &SensoryPercept::setText, py::arg("text"))
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("getSignalCount", &SensoryPercept::getSignalCount,
             "Get total number of signals in percept");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Handles sensory transduction and motor decoding for agent-environment interaction.
    )pbdoc")
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
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities (firing rates, etc.)")
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
             "Encode text into neuron pattern (Phase 6 feature)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal neuron firing rates")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get human-readable thought trace")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get number of learned concepts")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get number of social observations")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent's behavior for social learning")
        .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
             "Get number of planner overrides")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
             "Get number of planner agreements")
        .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
             "Get number of concept biases")
        .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
             "Get number of self biases")
        .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
             "Get number of social biases");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ===================================================================
    // BRAIN CORE SYSTEM
    // ===================================================================
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Core processing unit with neurons, synapses, and learning systems.
        Coordinates all neural computation and plasticity.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getWorkingMemory", &Brain::getWorkingMemory, py::return_value_policy::reference_internal, "Get working memory system")
        .def("getConceptFormation", &Brain::getConceptFormation, py::return_value_policy::reference_internal, "Get concept formation system")
        .def("getPlanner", &Brain::getPlanner, py::return_value_policy::reference_internal, "Get neural planner")
        .def("getSTDP", &Brain::getSTDP, py::return_value_policy::reference_internal, "Get STDP plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity, py::return_value_policy::reference_internal, "Get structural plasticity system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, py::return_value_policy::reference_internal, "Get development system")
        .def("getNeuromodulationSystem", &Brain::getNeuromodulationSystem, py::return_value_policy::reference_internal, "Get neuromodulation system")
        .def("getPredictionSystem", &Brain::getPredictionSystem, py::return_value_policy::reference_internal, "Get prediction system")
        .def("getMemorySystems", &Brain::getMemorySystems, py::return_value_policy::reference_internal, "Get memory systems")
        .def("getRandomGenerator", &Brain::getRandomGenerator, py::return_value_policy::reference_internal, "Get random number generator")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        });

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // ===================================================================
    // SIMULATION HELPERS AND UTILITIES
    // ===================================================================
    
    m.def("run_simulation_episode", &python_bindings::run_simulation_episode,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_learning") = false,
          "Run a complete simulation episode with error handling");

    m.def("vectorToList", &python_bindings::vector_to_list<float>,
          py::arg("vec"), py::arg("filter") = nullptr,
          "Convert C++ vector to Python list with optional filtering");

    m.def("get_neurons_by_type", &python_bindings::get_neurons_by_type,
          py::arg("brain"), py::arg("type"), "Get neurons of specific type");

    m.def("get_average_firing_rate_by_type", &python_bindings::get_average_firing_rate_by_type,
          py::arg("brain"), py::arg("type"), "Get average firing rate for neurons of specific type");

    // ===================================================================
    // WORLD OBJECT TYPE ENUMERATION
    // ===================================================================
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ===================================================================
    // INVALID ID CONSTANTS
    // ===================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===================================================================
    // MODULE ATTRIBUTES
    // ===================================================================
    
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
}

} // namespace nlm