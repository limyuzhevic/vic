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

    // Enhanced configuration management with validation
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
        
        // Configuration validation and error handling
        .def("validate", [](Config& self) {
            py::dict errors;
            py::dict warnings;
            
            // Validate required sections
            std::vector<std::string> requiredSections = {
                "brain.neuron_count",
                "brain.synapse_density",
                "simulation.timestep"
            };
            
            for (const auto& section : requiredSections) {
                if (!self.has(section)) {
                    warnings[section] = "Using default value";
                }
            }
            
            // Validate value ranges
            auto validateInt = [&self](const std::string& key, int min, int max) {
                if (auto valueOpt = self.get<int>(key)) {
                    int value = *valueOpt;
                    if (value < min) {
                        errors[key] = "Value too small, minimum is " + std::to_string(min);
                    } else if (value > max) {
                        errors[key] = "Value too large, maximum is " + std::to_string(max);
                    }
                }
            };
            
            auto validateFloat = [&self](const std::string& key, float min, float max) {
                if (auto valueOpt = self.get<double>(key)) {
                    double value = *valueOpt;
                    if (value < min) {
                        errors[key] = "Value too small, minimum is " + std::to_string(min);
                    } else if (value > max) {
                        errors[key] = "Value too large, maximum is " + std::to_string(max);
                    }
                }
            };
            
            // Validate specific values
            validateInt("brain.neuron_count", 100, 100000);
            validateFloat("brain.synapse_density", 0.01f, 1.0f);
            validateFloat("brain.connection_probability", 0.0f, 1.0f);
            validateFloat("simulation.timestep", 0.0001f, 1.0f);
            
            if (errors.empty()) {
                errors["__status"] = "Valid";
            } else {
                errors["__status"] = "Invalid";
            }
            
            return errors;
        }, "Validate configuration and return errors/warnings")
        
        // Configuration cloning and merging
        .def("clone", []() {
            auto config = std::make_shared<Config>();
            return config;
        }, "Create a copy of this configuration")
        
        // Merge configurations
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                self.set(key, other.getOr<int>(key, 0), ConfigSource::Runtime);
            }
        }, py::arg("other"), "Merge another configuration into this one")
        
        // Fluent API helpers for advanced users
        .def("setBrainConfig", [](Config& self, size_t neuronCount, float synapseDensity) {
            self.set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
            self.set("brain.synapse_density", synapseDensity, ConfigSource::User);
            return self;
        }, py::arg("neuronCount"), py::arg("synapseDensity"),
           "Set brain configuration parameters (returns self for fluent API)")
        
        .def("setPlasticityConfig", [](Config& self, bool enableSTDP, bool enableHebbian) {
            self.set("plasticity.stdp.enable", enableSTDP, ConfigSource::User);
            self.set("plasticity.hebbian.enable", enableHebbian, ConfigSource::User);
            return self;
        }, py::arg("enableSTDP"), py::arg("enableHebbian"),
           "Set plasticity configuration parameters (returns self for fluent API)")
        
        .def("setNeuromodulationConfig", [](Config& self, bool enableCuriosity, bool enableNovelty) {
            self.set("neuromod.curiosity.enable", enableCuriosity, ConfigSource::User);
            self.set("neuromod.novelty.enable", enableNovelty, ConfigSource::User);
            return self;
        }, py::arg("enableCuriosity"), py::arg("enableNovelty"),
           "Set neuromodulation configuration parameters (returns self for fluent API)")
        
        .def("setDevelopmentConfig", [](Config& self, float initialPlasticity, float adultPlasticity) {
            self.set("development.stage.initial_plasticity", initialPlasticity, ConfigSource::User);
            self.set("development.stage.adult_plasticity", adultPlasticity, ConfigSource::User);
            return self;
        }, py::arg("initialPlasticity"), py::arg("adultPlasticity"),
           "Set development configuration parameters (returns self for fluent API)")
        
        // Configuration comparison
        .def("equals", [](const Config& self, const Config& other) {
            auto selfKeys = self.getKeys();
            auto otherKeys = other.getKeys();
            
            if (selfKeys.size() != otherKeys.size()) return false;
            
            for (const auto& key : selfKeys) {
                if (!other.has(key)) return false;
                
                auto selfOpt = self.get<ConfigValue>(key);
                auto otherOpt = other.get<ConfigValue>(key);
                
                if (!selfOpt || !otherOpt) return false;
                if (*selfOpt != *otherOpt) return false;
            }
            
            return true;
        }, py::arg("other"), "Compare this configuration with another")
        
        // Deep copy
        .def("deepCopy", [](const Config& self) {
            auto copy = std::make_shared<Config>();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (auto valueOpt = self.get<ConfigValue>(key)) {
                    copy->set(key, *valueOpt, ConfigSource::Runtime);
                }
            }
            return copy;
        }, "Create a deep copy of this configuration");

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
    
    // World convenience methods for advanced users
    m.def("runCompleteSimulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
           std::shared_ptr<AgentBrain> agent, size_t numSteps) {
            for (size_t step = 0; step < numSteps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.1);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Safely get reward signal from world
                float reward = 0.0f;
                auto& internal = percept.getInternal();
                if (!internal.empty()) {
                    reward = internal[0];
                }
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
            }
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        "Run a complete agent simulation loop with brain, world, and agent interaction");
    
    m.def("createExperimentConfig", 
        [](const std::string& agentType, const std::string& environmentType, size_t steps) {
            auto config = std::make_shared<Config>();
            
            // Agent configuration
            if (agentType == "advanced") {
                config->set("brain.neuron_count", 2000, ConfigSource::User);
                config->set("brain.synapse_density", 0.15f, ConfigSource::User);
                config->set("plasticity.stdp.enable", true, ConfigSource::User);
                config->set("plasticity.hebbian.enable", true, ConfigSource::User);
                config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::User);
                config->set("neuromod.curiosity.enable", true, ConfigSource::User);
                config->set("neuromod.novelty.enable", true, ConfigSource::User);
            }
            
            // Environment configuration  
            if (environmentType == "complex") {
                config->set("world.max_resources", 10, ConfigSource::User);
                config->set("world.max_hazards", 5, ConfigSource::User);
                config->set("world.reward_scale", 2.0f, ConfigSource::User);
                config->set("world.punishment_scale", 1.5f, ConfigSource::User);
            }
            
            // Development configuration
            config->set("development.stage.initial_plasticity", 1.0f, ConfigSource::User);
            config->set("development.stage.adult_plasticity", 0.1f, ConfigSource::User);
            config->set("development.synapse_growth_rate", 0.00005f, ConfigSource::User);
            
            config->set("simulation.timesteps", steps, ConfigSource::User);
            
            return config;
        }, py::arg("agentType") = "basic", py::arg("environmentType") = "simple", py::arg("steps") = 1000,
        "Create a complete configuration for agent-environment experiments");

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

    // Agent body state
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

    // Action result from world
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory percept data
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

        m.def("createAgentConfig", 
        [](bool enableLearning, bool enableCuriosity, bool enableDevelopment) {
            auto config = std::make_shared<Config>();
            
            // Learning configuration
            config->set("agent.learning.enabled", enableLearning, ConfigSource::User);
            config->set("agent.learning.reward_modulation", enableLearning, ConfigSource::User);
            
            // Curiosity configuration
            config->set("agent.curiosity.enabled", enableCuriosity, ConfigSource::User);
            config->set("agent.curiosity.threshold", 0.3f, ConfigSource::User);
            
            // Development configuration
            config->set("agent.development.enabled", enableDevelopment, ConfigSource::User);
            config->set("agent.development.maturation_rate", 0.001f, ConfigSource::User);
            
            return config;
        }, py::arg("enableLearning") = true, py::arg("enableCuriosity") = true, 
           py::arg("enableDevelopment") = true, "Create an agent configuration");
    
    // Environment configuration helper
    m.def("createSimpleWorldConfig", 
        [](size_t width, size_t height, size_t visionWidth, size_t visionHeight,
           float maxEnergy, float resourceGain) {
            auto config = std::make_shared<Config>();
            config->set("world.width", static_cast<float>(width), ConfigSource::User);
            config->set("world.height", static_cast<float>(height), ConfigSource::User);
            config->set("world.vision_width", visionWidth, ConfigSource::User);
            config->set("world.vision_height", visionHeight, ConfigSource::User);
            config->set("world.max_energy", maxEnergy, ConfigSource::User);
            config->set("world.resource_energy_gain", resourceGain, ConfigSource::User);
            config->set("world.energy_decay_rate", 0.01f, ConfigSource::User);
            config->set("world.max_resources", 8, ConfigSource::User);
            config->set("world.max_hazards", 3, ConfigSource::User);
            return config;
        }, py::arg("width") = 20, py::arg("height") = 20, 
           py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
           py::arg("maxEnergy") = 100.0f, py::arg("resourceGain") = 30.0f,
           "Create a simple world configuration");
    
    // Complete simulation helper for advanced users
    m.def("runAdvancedAgentSimulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
           std::shared_ptr<AgentBrain> agent, size_t numSteps, bool enableLearning,
           bool enableCuriosity, bool enableDevelopment) {
            
            // Enable subsystems based on configuration
            agent->enableRewardModulation(enableLearning);
            agent->enableCuriosity(enableCuriosity);
            agent->enableDevelopment(enableDevelopment);
            
            // Run simulation with advanced logging
            for (size_t step = 0; step < numSteps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.1);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward if learning enabled
                if (enableLearning && result.success) {
                    agent->applyRewardModulation(result.reward, 0.0f);
                }
                
                // Update development if enabled
                if (enableDevelopment) {
                    agent->updateDevelopment(0.1);
                }
                
                // Log progress every 100 steps
                if (step % 100 == 0) {
                    std::cout << "Step " << step 
                              << " | Firing neurons: " << brain->getFiringNeuronCount()
                              << " | Dopamine: " << agent->getNeuromodulationLevel()
                              << " | Curiosity: " << agent->getCuriosityLevel()
                              << " | Reward: " << result.reward
                              << std::endl;
                }
            }
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
           py::arg("enableLearning") = true, py::arg("enableCuriosity") = true,
           py::arg("enableDevelopment") = true,
           "Run an advanced agent simulation with learning, curiosity, and development");
    
    // Error handling and recovery utilities
    m.def("safeBrainStep", 
        [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
            try {
                brain->step(step, time);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error in brain step: " << e.what() << std::endl;
                brain->reset();
                return false;
            }
        }, py::arg("brain"), py::arg("step"), py::arg("time"),
        "Safely execute a brain step with error recovery");
    
    m.def("safeWorldUpdate", 
        [](std::shared_ptr<SimpleWorld> world, double timestep) {
            try {
                world->update(timestep);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error in world update: " << e.what() << std::endl;
                world->reset();
                return false;
            }
        }, py::arg("world"), py::arg("timestep"),
        "Safely update world state with error recovery");
    
    m.def("safeAgentProcess", 
        [](std::shared_ptr<AgentBrain> agent, const SensoryPercept& percept) {
            try {
                agent->processSensoryInput(percept);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error in agent processing: " << e.what() << std::endl;
                agent->reset();
                return false;
            }
        }, py::arg("agent"), py::arg("percept"),
        "Safely process sensory input with error recovery");
    
    m.def("validateBrainConfig", 
        [](std::shared_ptr<Config> config) {
            py::dict validationResult;
            bool isValid = true;
            std::string errorMessages;
            
            // Validate brain configuration
            if (!config->has("brain.neuron_count")) {
                isValid = false;
                errorMessages += "Missing brain.neuron_count; ";
            } else {
                auto neuronCountOpt = config->get<int64_t>("brain.neuron_count");
                if (neuronCountOpt && (*neuronCountOpt < 100 || *neuronCountOpt > 100000)) {
                    isValid = false;
                    errorMessages += "brain.neuron_count out of range (100-100000); ";
                }
            }
            
            if (!config->has("brain.synapse_density")) {
                isValid = false;
                errorMessages += "Missing brain.synapse_density; ";
            } else {
                auto densityOpt = config->get<double>("brain.synapse_density");
                if (densityOpt && (*densityOpt < 0.01 || *densityOpt > 1.0)) {
                    isValid = false;
                    errorMessages += "brain.synapse_density out of range (0.01-1.0); ";
                }
            }
            
            if (!config->has("simulation.timestep")) {
                isValid = false;
                errorMessages += "Missing simulation.timestep; ";
            } else {
                auto timestepOpt = config->get<double>("simulation.timestep");
                if (timestepOpt && (*timestepOpt < 0.0001 || *timestepOpt > 1.0)) {
                    isValid = false;
                    errorMessages += "simulation.timestep out of range (0.0001-1.0); ";
                }
            }
            
            validationResult["valid"] = isValid;
            validationResult["errors"] = errorMessages;
            
            if (isValid) {
                // Apply defaults for missing values
                if (!config->has("brain.neuron_count")) {
                    config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
                }
                if (!config->has("brain.synapse_density")) {
                    config->set("brain.synapse_density", 0.1f, ConfigSource::Runtime);
                }
                if (!config->has("simulation.timestep")) {
                    config->set("simulation.timestep", 0.001, ConfigSource::Runtime);
                }
            }
            
            return validationResult;
        }, py::arg("config"), "Validate and fix brain configuration");
    
    m.def("createErrorRecoveryConfig", 
        []() {
            auto config = std::make_shared<Config>();
            
            // Configure for robust operation
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
            config->set("simulation.timestep", 0.001, ConfigSource::Default);
            
            // Enable safety features
            config->set("brain.safety.enabled", true, ConfigSource::Default);
            config->set("brain.safety.max_spike_rate", 1000, ConfigSource::Default);
            config->set("brain.safety.max_energy", 1000.0f, ConfigSource::Default);
            
            // Enable recovery mechanisms
            config->set("brain.recovery.enabled", true, ConfigSource::Default);
            config->set("brain.recovery.checkpoint_interval", 1000, ConfigSource::Default);
            config->set("brain.recovery.auto_reset_on_error", true, ConfigSource::Default);
            
            return config;
        }, "Create a configuration optimized for error recovery and robust operation");
    
    // Performance monitoring and optimization
    m.def("profileBrainStep", 
        [](std::shared_ptr<Brain> brain, size_t steps) {
            py::dict profile;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            profile["steps"] = steps;
            profile["total_time_ms"] = static_cast<double>(duration.count());
            profile["avg_time_per_step_ms"] = static_cast<double>(duration.count()) / steps;
            profile["steps_per_second"] = (steps * 1000.0) / duration.count();
            
            // Collect brain metrics
            profile["final_neurons"] = brain->getTotalNeuronCount();
            profile["final_synapses"] = brain->getTotalSynapseCount();
            profile["total_spikes"] = brain->getTotalSpikeCount();
            profile["firing_rate"] = brain->getAverageFiringRate();
            
            return profile;
        }, py::arg("brain"), py::arg("steps"),
        "Profile brain performance over multiple steps");
}

} // namespace nlm
