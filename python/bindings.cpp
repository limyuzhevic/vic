#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent sensing capabilities. This method must be called before using the world.")
        .def("reset", &SimpleWorld::reset, "Reset the world to its initial state. This restores the agent to the start position and clears world objects.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position in the world.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world simulation by the given timestep. This processes physics, decay, and environmental changes.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command from the agent to the world. This updates the agent's position and state based on the action.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept that the agent perceives. This includes vision, touch, and internal state information.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state including position, velocity, energy, and other physical properties.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object (resource, hazard, wall, or marker) to the simulation.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove a world object at the specified coordinates.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (not occupied by walls or boundaries).")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get the world width in simulation units.")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get the world height in simulation units.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get the maximum energy capacity of the agent.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy capacity of the agent.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate per timestep.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate per timestep.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get the current random seed.")
        // Additional convenience methods for easier usage
        .def("getEnergy", [] (const SimpleWorld& self) {
            return self.getAgentBody().energy;
        }, "Get the agent's current energy level.")
        .def("getPosition", [] (const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get the agent's current position as (x, y) tuple.")
        .def("getHealth", [] (const SimpleWorld& self) {
            return self.getAgentBody().health;
        }, "Get the agent's current health level.")
        .def("isAlive", [] (const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.energy > 0.0f && body.health > 0.0f;
        }, "Check if the agent is still alive (has energy and health).")
        .def("getNearestObject", [](SimpleWorld& self, float x, float y) {
            for (auto& obj : self.getObjects()) {
                if (!obj.active) continue;
                float dx = obj.x - x;
                float dy = obj.y - y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < obj.radius) {
                    return py::make_tuple(obj.x, obj.y, obj.type, obj.value);
                }
            }
            return py::make_tuple(0.0f, 0.0f, 0, 0.0f);  // Empty tuple for no object
        }, py::arg("x"), py::arg("y"),
             "Find the nearest object to the given coordinates. Returns (x, y, type, value) or (0, 0, 0, 0) if none found.");
    
    // Add convenience exception types for better error handling
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    
    // Add module-level helper functions
    m.def("createWorldConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for world simulation
        config->set("world.width", 20);
        config->set("world.height", 20);
        config->set("world.vision_width", 8);
        config->set("world.vision_height", 8);
        config->set("world.max_energy", 100.0f);
        config->set("world.energy_decay_rate", 0.001f);
        config->set("world.random_seed", 42);
        return config;
    }, "Create a world configuration with sensible defaults.")
    
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                         std::shared_ptr<AgentBrain> agent, int maxSteps) {
        if (!brain || !world || !agent) {
            throw InvalidArgumentError("Brain, world, and agent must be provided");
        }
        
        agent->initialize(*world);
        
        for (int step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (!agent->isAlive()) {
                agent->reset();
                world->reset();
                break;
            }
        }
        
        return py::make_tuple(brain->getTotalSpikeCount(), 
                             brain->getAverageFiringRate(),
                             agent->getNeuromodulationLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("maxSteps"),
       "Run a complete simulation episode. Returns (total_spikes, avg_firing_rate, neuromodulation_level).")
    
    m.def("checkBrainHealth", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw InvalidArgumentError("Brain must be provided");
        }
        
        auto health = brain->logStatus();
        return health;
    }, py::arg("brain"),
       "Check brain health and return status information as a string.")
    
    // Add convenience factory functions
    m.def("createSimpleTestBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a simple test brain with default configuration for testing and learning.")
    
    m.def("createSimpleTestWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10, 10);
        
        // Add some test objects
        auto resource = std::make_shared<WorldObject>(5, 5, WorldObjectType::Resource, 10.0f);
        world->addObject(*resource);
        
        auto hazard = std::make_shared<WorldObject>(15, 15, WorldObjectType::Hazard, -5.0f);
        world->addObject(*hazard);
        
        return world;
    }, "Create a simple test world with default configuration and test objects for testing and learning.")
    
    m.def("createSimpleTestAgent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw InvalidArgumentError("Brain must be provided");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, py::arg("brain"),
       "Create a simple test agent with the given brain.")
    
    m.def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                            const std::vector<std::string>& names) {
        if (brains.size() != names.size()) {
            throw InvalidArgumentError("Brains and names must have the same size");
        }
        
        std::string result = "Brain Comparison:\n";
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                result += names[i] + ": " + 
                         std::to_string(brains[i]->getTotalNeuronCount()) + " neurons, " +
                         std::to_string(brains[i]->getTotalSynapseCount()) + " synapses, " +
                         std::to_string(brains[i]->getTotalSpikeCount()) + " spikes\n";
            } else {
                result += names[i] + ": NULL brain\n";
            }
        }
        return result;
    }, py::arg("brains"), py::arg("names"),
       "Compare multiple brains and return a summary string. Useful for batch processing.")
    
    // Add utility constants and enums for easier access
    m.attr("DEFAULT_VISION_SIZE") = py::make_tuple(16, 16);
    m.attr("DEFAULT_WORLD_SIZE") = py::make_tuple(20, 20);
    m.attr("MAX_WORLD_OBJECTS") = 1000;
    
    // Add module-level statistics
    m.def("getModuleInfo", []() {
        return py::make_tuple(
            "NLM Python Bindings v0.1.0",
            "Build date: " __DATE__,
            "C++ standard: C++20",
            "Features: Complete brain simulation with memory, prediction, cognition, and neuromodulation"
        );
    }, "Get information about the NLM Python module.");
    
    // Add training/evaluation utilities
    m.def("evaluateBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, int steps) {
        if (!brain || !world || !agent) {
            throw InvalidArgumentError("Brain, world, and agent must be provided");
        }
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        
        std::vector<float> rewards;
        std::vector<float> firingRates;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from world (energy is a proxy for reward)
            float reward = agent->getNeuromodulationLevel();
            rewards.push_back(reward);
            firingRates.push_back(brain->getAverageFiringRate());
            
            if (!agent->isAlive()) {
                break;
            }
        }
        
        // Calculate statistics
        float avgReward = 0.0f, avgFiringRate = 0.0f;
        if (!rewards.empty()) {
            float sumRewards = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
            float sumFiringRates = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
            avgReward = sumRewards / rewards.size();
            avgFiringRate = sumFiringRates / firingRates.size();
        }
        
        return py::make_tuple(
            avgReward,
            avgFiringRate,
            rewards.size(),
            firingRates.size()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Evaluate a brain's performance over a simulation. Returns (avg_reward, avg_firing_rate, steps_completed, valid_steps).")
    
    m.def("benchmarkBrains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.empty()) {
            throw InvalidArgumentError("At least one brain must be provided");
        }
        
        std::string results = "Brain Benchmark Results:\n";
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (!brains[i]) {
                results += "Brain " + std::to_string(i) + ": NULL\n";
                continue;
            }
            
            auto brain = brains[i];
            results += "Brain " + std::to_string(i) + ": " +
                       std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                       std::to_string(brain->getTotalSynapseCount()) + " synapses\n";
            
            // Simple benchmark: step 10 times
            for (int step = 0; step < 10; ++step) {
                brain->step(step);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results += "\nTotal benchmark time: " + std::to_string(duration.count()) + " ms\n";
        return results;
    }, py::arg("brains"),
       "Benchmark multiple brains for performance comparison. Returns a detailed results string.")
    
    m.def("saveBrainToFile", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw InvalidArgumentError("Brain must be provided");
        }
        if (filepath.empty()) {
            throw InvalidArgumentError("Filepath must be provided");
        }
        
        bool success = brain->save(filepath);
        if (!success) {
            throw ConfigurationError("Failed to save brain to file: " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
       "Save a brain to a file. Returns True on success, raises ConfigurationError on failure.")
    
    m.def("loadBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        if (filepath.empty()) {
            throw InvalidArgumentError("Filepath must be provided");
        }
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        bool success = brain->load(filepath);
        if (!success) {
            throw ConfigurationError("Failed to load brain from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"), py::return_value_policy::automatic,
       "Load a brain from a file. Returns Brain object or raises ConfigurationError on failure.")
    
    // Add comprehensive error checking and validation
    m.def("validateBrain", [](std::shared_ptr<Brain> brain) -> bool {
        if (!brain) {
            return false;
        }
        
        // Check basic brain properties
        if (brain->getTotalNeuronCount() == 0) {
            return false;
        }
        
        // Try to get configuration
        try {
            auto config = brain->getConfig();
            if (!config) {
                return false;
            }
        } catch (...) {
            return false;
        }
        
        return true;
    }, py::arg("brain"),
       "Validate a brain object. Returns True if the brain is valid and ready for use, False otherwise.")
    
    m.def("getSystemInfo", []() {
        return py::make_dict(
            "version", "0.1.0",
            "cpp_standard", "C++20",
            "features", py::list({
                "Real spiking neural computation",
                "Event-driven dynamics",
                "Working memory system",
                "Episodic memory system",
                "Associative memory system",
                "Prediction system",
                "Neural planning",
                "Concept formation",
                "Attention system",
                "Dopamine neuromodulation",
                "Curiosity system",
                "Novelty detection",
                "Prediction error",
                "Developmental stages",
                "Structural plasticity",
                "Reward-based learning",
                "Checkpoint system"
            }),
            "configuration_options", py::list({
                "neuron_count", "region_count", "connection_probability",
                "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
                "synaptogenesis_rate", "pruning_rate",
                "simulation_timestep", "replay_interval", "consolidation_interval",
                "random_seed", "checkpoint_dir"
            }),
            "sensory_modalities", py::list({
                "vision", "touch", "internal", "proprioception", "audio"
            }),
            "motor_commands", py::list({
                "move_forward", "move_backward", "turn_left", "turn_right",
                "look_left", "look_right", "interact", "wait"
            }),
            "neuron_types", py::list({
                "excitatory", "inhibitory", "modulatory", "sensory", "motor", "internal"
            })
        );
    }, "Get comprehensive information about the NLM system.")
    
    // Add convenience module-level constants
    m.def("getDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Get a default configuration object.")
    
    m.def("setRandomSeed", [](int seed) {
        std::srand(static_cast<unsigned int>(seed));
    }, py::arg("seed"),
       "Set the random seed for reproducible simulations.")
    
    // Add simulation utilities
    m.def("runSimulationEpisode", [](std::shared_ptr<Brain> brain, 
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    int steps) {
        if (!brain || !world || !agent) {
            throw InvalidArgumentError("Brain, world, and agent must be provided");
        }
        
        agent->initialize(*world);
        
        // Track statistics
        std::vector<float> spikeCounts;
        std::vector<float> firingRates;
        std::vector<float> neuromodulationLevels;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            spikeCounts.push_back(static_cast<float>(brain->getTotalSpikeCount()));
            firingRates.push_back(brain->getAverageFiringRate());
            neuromodulationLevels.push_back(agent->getNeuromodulationLevel());
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (!agent->isAlive()) {
                agent->reset();
                world->reset();
                break;
            }
        }
        
        return py::make_tuple(spikeCounts, firingRates, neuromodulationLevels);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a simulation episode and return detailed time-series data.")
    
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configuration optimized for exploration and curiosity-driven learning
        config->set("brain.cognition.planner.enabled", true);
        config->set("brain.memory.episodic.enabled", true);
        config->set("brain.memory.working.capacity", 50);
        config->set("brain.plasticity.stdp.enable", true);
        config->set("brain.development.enable", true);
        config->set("brain.neuromodulation.curiosity.enabled", true);
        config->set("brain.neuromodulation.novelty.enabled", true);
        config->set("brain.prediction.enabled", true);
        config->set("environment.reward_scale", 1.0f);
        config->set("environment.exploration_bonus", 0.5f);
        return config;
    }, "Create an exploration-oriented configuration for curiosity-driven learning.")
    
    m.def("createExploitationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configuration optimized for exploitation and efficient task performance
        config->set("brain.cognition.planner.enabled", true);
        config->set("brain.memory.episodic.enabled", true);
        config->set("brain.memory.working.capacity", 100);
        config->set("brain.plasticity.stdp.enable", true);
        config->set("brain.development.enable", false);  // Less plasticity for stability
        config->set("brain.neuromodulation.curiosity.enabled", false);
        config->set("brain.neuromodulation.novelty.enabled", false);
        config->set("brain.prediction.enabled", true);
        config->set("environment.reward_scale", 2.0f);
        config->set("environment.exploration_bonus", 0.0f);
        return config;
    }, "Create an exploitation-oriented configuration for stable task performance.")
    
    // Add batch processing utilities
    m.def("batchProcessBrains", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                 const std::vector<std::string>& names,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent,
                                 int steps) {
        if (brains.size() != names.size()) {
            throw InvalidArgumentError("Brains and names must have the same size");
        }
        
        std::string results = "Batch Processing Results:\n";
        std::vector<float> finalSpikes;
        std::vector<float> finalRates;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (!brains[i] || !world || !agent) {
                results += names[i] + ": ERROR - Invalid brain or components\n";
                continue;
            }
            
            agent->initialize(*world);
            
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brains[i]->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                if (!agent->isAlive()) {
                    agent->reset();
                    world->reset();
                    break;
                }
            }
            
            finalSpikes.push_back(static_cast<float>(brains[i]->getTotalSpikeCount()));
            finalRates.push_back(brains[i]->getAverageFiringRate());
            
            results += names[i] + ": " +
                       std::to_string(brains[i]->getTotalNeuronCount()) + " neurons, " +
                       std::to_string(brains[i]->getTotalSynapseCount()) + " synapses, " +
                       std::to_string(finalSpikes.back()) + " spikes, " +
                       std::to_string(finalRates.back()) + " avg firing rate\n";
        }
        
        return py::make_tuple(results, finalSpikes, finalRates);
    }, py::arg("brains"), py::arg("names"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Batch process multiple brains. Returns (results_string, spike_counts, firing_rates).")
    
    // Add visualization utilities
    m.def("brainToString", [](std::shared_ptr<Brain> brain) -> std::string {
        if (!brain) {
            return "NULL brain";
        }
        
        std::stringstream ss;
        ss << "Brain State Summary:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Pending Spikes: " << brain->getPendingSpikeEventCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        return ss.str();
    }, py::arg("brain"),
       "Get a string representation of brain state for debugging and visualization.")
    
    m.def("worldToString", [](std::shared_ptr<SimpleWorld> world) -> std::string {
        if (!world) {
            return "NULL world";
        }
        
        std::stringstream ss;
        ss << "World State Summary:" << std::endl;
        ss << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
        ss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
        ss << "  Vision Size: " << world->getVisionWidth() << " x " << world->getVisionHeight() << std::endl;
        
        auto body = world->getAgentBody();
        ss << "  Agent Position: (" << body.x << ", " << body.y << ")" << std::endl;
        ss << "  Agent Energy: " << body.energy << std::endl;
        ss << "  Agent Health: " << body.health << std::endl;
        ss << "  Agent Age: " << body.age << std::endl;
        ss << "  Agent Alive: " << (body.energy > 0.0f && body.health > 0.0f ? "Yes" : "No") << std::endl;
        
        return ss.str();
    }, py::arg("world"),
       "Get a string representation of world state for debugging and visualization.")
    
    m.def("agentToString", [](std::shared_ptr<AgentBrain> agent) -> std::string {
        if (!agent) {
            return "NULL agent";
        }
        
        std::stringstream ss;
        ss << "Agent Brain State Summary:" << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        ss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
        ss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
        ss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
        ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
        ss << "  Reward Modulation Enabled: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
        ss << "  Structural Plasticity Enabled: " << (agent->isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
        ss << "  Development Enabled: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
        ss << "  Curiosity Enabled: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
        
        return ss.str();
    }, py::arg("agent"),
       "Get a string representation of agent brain state for debugging and visualization.")
    
    // Add comprehensive error checking and validation utilities
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) -> bool {
        if (!config) {
            return false;
        }
        
        bool valid = true;
        
        // Check required basic settings
        if (config->getOr<size_t>("neuron_count", 0) == 0) {
            valid = false;
        }
        
        if (config->getOr<size_t>("region_count", 0) == 0) {
            valid = false;
        }
        
        // Check for reasonable values
        auto neuronCount = config->getOr<size_t>("neuron_count", 0);
        if (neuronCount > 1000000) {
            valid = false;
        }
        
        auto regionCount = config->getOr<size_t>("region_count", 0);
        if (regionCount > 1000) {
            valid = false;
        }
        
        return valid;
    }, py::arg("config"),
       "Validate a configuration object. Returns True if the configuration is valid.")
    
    m.def("compareSystems", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent) -> std::string {
        if (!brain || !world || !agent) {
            return "ERROR: All components (brain, world, agent) must be provided";
        }
        
        std::stringstream ss;
        ss << "=== NLM System Comparison ===" << std::endl;
        ss << std::endl;
        
        ss << "--- Brain System ---" << std::endl;
        ss << brainToString(brain);
        ss << std::endl;
        
        ss << "--- World System ---" << std::endl;
        ss << worldToString(world);
        ss << std::endl;
        
        ss << "--- Agent System ---" << std::endl;
        ss << agentToString(agent);
        ss << std::endl;
        
        return ss.str();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Get a comprehensive comparison of all three system components (brain, world, agent).")
    
    // Add interactive utilities
    m.def("runInteractiveDemo", []() {
        std::string demo = 
            "=== NLM Interactive Demo ===\n\n" +
            "This demo shows how to create and use NLM components:\n\n" +
            "1. Creating a brain:\n" +
            "   brain = pynlm.createBrain(pynlm.createDefaultConfig())\n" +
            "   brain.initialize()\n\n" +
            "2. Creating a world:\n" +
            "   world = pynlm.createSimpleWorld()\n" +
            "   world.configure(20, 20, 8, 8)\n\n" +
            "3. Creating an agent:\n" +
            "   agent = pynlm.createAgentBrain(brain)\n" +
            "   agent.initialize(world)\n\n" +
            "4. Running a simulation:\n" +
            "   for step in range(100):\n" +
            "       world.update(0.1)\n" +
            "       agent.processSensoryInput(world.getSensoryPercept())\n" +
            "       brain.step(step)\n" +
            "       action = agent.decodeMotorCommand()\n" +
            "       world.applyMotorCommand(action, world.getSimulationTime())\n\n" +
            "5. Getting statistics:\n" +
            "   print('Spikes:', brain.getTotalSpikeCount())\n" +
            "   print('Firing Rate:', brain.getAverageFiringRate())\n" +
            "   print('Neuromodulation:', agent.getNeuromodulationLevel())\n\n" +
            "=== Demo Complete ===";
        return demo;
    }, "Get a string showing an interactive demo of NLM usage.");
    
    // Add testing utilities
    m.def("testModule", []() {
        std::vector<std::string> tests;
        
        // Test 1: Create brain
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            tests.push_back("✓ Brain creation: PASSED");
        } catch (...) {
            tests.push_back("✗ Brain creation: FAILED");
        }
        
        // Test 2: Create world
        try {
            auto world = std::make_shared<SimpleWorld>();
            tests.push_back("✓ World creation: PASSED");
        } catch (...) {
            tests.push_back("✗ World creation: FAILED");
        }
        
        // Test 3: Create agent
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            tests.push_back("✓ Agent creation: PASSED");
        } catch (...) {
            tests.push_back("✗ Agent creation: FAILED");
        }
        
        // Test 4: Module info
        try {
            auto info = m.attr("getModuleInfo")();
            tests.push_back("✓ Module info: PASSED");
        } catch (...) {
            tests.push_back("✗ Module info: FAILED");
        }
        
        std::string result = "NLM Module Test Results:\n";
        for (const auto& test : tests) {
            result += test + "\n";
        }
        
        return result;
    }, "Run comprehensive module tests and return results.");
    
    // Add detailed documentation
    m.doc() = R"pbdoc(
# NLM Python Bindings

## Overview

NLM (Neural Learning Machine) is a computational brain simulator for artificial intelligence research. The Python bindings provide a simple interface to create, configure, and run neural simulations.

## Key Features

- **Real spiking neural computation**: Based on LIF neuron models with event-driven dynamics
- **Integrated memory systems**: Working memory, episodic memory, and associative memory
- **Prediction system**: Forward models and prediction error computation
- **Cognition systems**: Planning, concept formation, and attention
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Development**: Plasticity changes across developmental stages
- **Reward learning**: Reward-modulated synaptic plasticity
- **Checkpoint system**: Save/load brain states
- **Complete brain loop**: World interaction, sensory processing, and motor output

## Quick Start

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Available Classes

- **Brain**: The central neural processing unit
- **Config**: Configuration management
- **SimpleWorld**: Simulation environment
- **AgentBrain**: Interface between brain and world
- **SensoryPercept**: Agent's sensory input
- **Vision**: Visual sensory input
- **Audio**: Audio sensory input
- **InternalSignals**: Internal state signals
- **Action**: Motor commands
- **WorldObject**: Objects in the world
- **AgentBody**: Agent's physical state

## Configuration Options

The following configuration keys can be set:

- `"neuron_count"`: Total number of neurons
- `"region_count"`: Number of brain regions
- `"connection_probability"`: Probability of connections between neurons
- `"stdp_ltp_weight"`: STDP long-term potentiation weight
- `"stdp_ltd_weight"`: STDP long-term depression weight
- `"stdp_tau"`: STDP time constant
- `"synaptogenesis_rate"`: Rate of new synapse formation
- `"pruning_rate"`: Rate of synapse removal
- `"simulation_timestep"`: Time step for simulation
- `"replay_interval"`: Interval for memory replay
- `"consolidation_interval"`: Interval for memory consolidation
- `"random_seed"`: Random seed for reproducibility
- `"checkpoint_dir"`: Directory for checkpoint files

## Examples

See `docs/HOW_TO_USE.md` for comprehensive examples and documentation.

## License

MIT License. See LICENSE file for details.

## Copyright

Research project — See docs for scientific background.
    )pbdoc";
    
    // Add error definitions
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
}

namespace nlm {

// Custom exception classes
class InvalidArgumentError : public std::runtime_error {
public:
    explicit InvalidArgumentError(const std::string& msg) : std::runtime_error("Invalid Argument: " + msg) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error("Configuration Error: " + msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error("Simulation Error: " + msg) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& msg) : std::runtime_error("Memory Error: " + msg) {}
};

// Additional helper functions
std::string brainToString(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return "NULL brain";
    }
    
    std::stringstream ss;
    ss << "Brain State Summary:" << std::endl;
    ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    ss << "  Pending Spikes: " << brain->getPendingSpikeEventCount() << std::endl;
    ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    return ss.str();
}

std::string worldToString(std::shared_ptr<SimpleWorld> world) {
    if (!world) {
        return "NULL world";
    }
    
    std::stringstream ss;
    ss << "World State Summary:" << std::endl;
    ss << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
    ss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
    ss << "  Vision Size: " << world->getVisionWidth() << " x " << world->getVisionHeight() << std::endl;
    
    auto body = world->getAgentBody();
    ss << "  Agent Position: (" << body.x << ", " << body.y << ")" << std::endl;
    ss << "  Agent Energy: " << body.energy << std::endl;
    ss << "  Agent Health: " << body.health << std::endl;
    ss << "  Agent Age: " << body.age << std::endl;
    ss << "  Agent Alive: " << (body.energy > 0.0f && body.health > 0.0f ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

std::string agentToString(std::shared_ptr<AgentBrain> agent) {
    if (!agent) {
        return "NULL agent";
    }
    
    std::stringstream ss;
    ss << "Agent Brain State Summary:" << std::endl;
    ss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    ss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
    ss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
    ss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
    ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
    ss << "  Reward Modulation Enabled: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    ss << "  Structural Plasticity Enabled: " << (agent->isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
    ss << "  Development Enabled: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    ss << "  Curiosity Enabled: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

} // namespace nlm

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
