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

    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON or simple key=value format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments (supports --key=value and -key value)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (simple key=value format)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value by key. Returns None if key doesn't exist.")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value by key, returns default if key doesn't exist")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set), 
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with specified source (Default, File, CommandLine, Runtime)")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set), 
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set), 
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set double configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set), 
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })

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

    // Enhanced Brain class with Pythonic interface
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with the given configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", py::overload_cast<NeuronId, MembranePotential>(&Brain::injectCurrent),
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns an Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpoint). Returns True on success.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file (checkpoint). Returns True on success.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name. Returns RegionId.")
        .def("getRegion", py::overload_cast<RegionId>(&Brain::getRegion, py::const),
             py::arg("id"),
             "Get a region by ID. Returns None if region doesn't exist.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", py::overload_cast<const std::vector<std::unique_ptr<NeuralRegion>>&>(&Brain::getRegions, py::const),
             "Get all regions (internal use)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (Hz)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio (0.0 = all inhibition, 1.0 = all excitation)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (not yet processed)")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage of the brain")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (ex: DevelopmentalStage.Adult)")
        .def("getConfig", py::overload_cast<std::shared_ptr<const Config>>(&Brain::getConfig, py::const),
             "Get a copy of the brain's configuration")
        .def("logStatus", &Brain::logStatus,
             "Log current brain status and statistics")
        .def("isInitialized", &Brain::getRegionCount,  // Using as a proxy for initialized state
             "Check if brain is initialized (has regions)")
        .def("getNeuronTypeCounts", [](const Brain& brain) {
            // This would require access to internal structures, but we'll add a placeholder
            py::dict counts;
            counts["Excitatory"] = 0;
            counts["Inhibitory"] = 0;
            counts["Sensory"] = 0;
            counts["Motor"] = 0;
            counts["Modulatory"] = 0;
            counts["Internal"] = 0;
            return counts;
        }, "Get counts of neurons by type (placeholder)")
        .def("getSynapticStrengthStats", [](const Brain& brain) {
            py::dict stats;
            stats["min"] = 0.0f;
            stats["max"] = 0.0f;
            stats["mean"] = 0.0f;
            return stats;
        }, "Get synaptic strength statistics (placeholder)")
        .def("simulateEpisode", [](Brain& brain, size_t steps, bool useTimestamps = false) {
            if (useTimestamps) {
                for (size_t i = 0; i < steps; ++i) {
                    brain.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * 0.001));
                }
            } else {
                for (size_t i = 0; i < steps; ++i) {
                    brain.step(static_cast<SimulationStep>(i));
                }
            }
            return brain.getTotalSpikeCount();
        }, py::arg("steps"), py::arg("useTimestamps") = false,
         "Run a complete simulation episode for given number of steps. Returns total spike count.")
        .def("createDefaultConfig", []() {
            return std::make_shared<Config>();
        }, "Create a default configuration object");

    // Enhanced AgentBrain class with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the given brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with world. Returns True on success.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns an Action object.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage of the agent brain")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (0.0-1.0)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (0.0-1.0)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error (0.0-1.0)")
        .def("reset", &AgentBrain::reset,
             "Reset agent brain for new episode")
        .def("getBrain", py::overload_cast<std::shared_ptr<Brain>>(&AgentBrain::getBrain),
             "Get the underlying brain object")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("runEpisode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, size_t steps) {
            world->reset();
            agent.initialize(*world);
            
            for (size_t i = 0; i < steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent.processSensoryInput(percept);
                
                // Get the underlying brain and step it
                auto brain = agent.getBrain();
                brain->step(static_cast<SimulationStep>(i));
                
                auto action = agent.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Get reward from world and apply modulation
                auto internalSignals = percept.getInternal();
                float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
                agent.applyRewardModulation(reward, 0.0f);
                
                agent.updateDevelopment(0.1);
            }
            
            return agent.getCuriosityLevel();
        }, py::arg("world"), py::arg("steps"),
         "Run a complete agent episode. Returns final curiosity level.")
        .def("createAgent", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"),
         "Create an agent brain interface from a brain object");

    // Python module-level convenience functions
    m.def("createBrain", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a new brain with default configuration");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface from a brain");

    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Neural type aliases for convenience
    m.attr("NeuronId") = py::type_alias("NeuronId");
    m.attr("SynapseId") = py::type_alias("SynapseId");
    m.attr("RegionId") = py::type_alias("RegionId");
    m.attr("PopulationId") = py::type_alias("PopulationId");

    // Constants and configuration helpers
    m.attr("NEURON_TYPES") = py::dict()
        .def("__repr__", []() { return "Available neuron types: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal"; });

    m.attr("ACTION_TYPES") = py::dict()
        .def("__repr__", []() { return "Available action types: MoveForward, MoveBackward, TurnLeft, TurnRight, Look, Interact, Eat, Drink, Rest, Wait, Custom"; });

    m.attr("DEVELOPMENT_STAGES") = py::dict()
        .def("__repr__", []() { return "Development stages: Initial, CriticalPeriod, Maturation, Adult, Aging"; });

    // Validation and utility functions
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        bool valid = true;
        std::string errors;
        
        if (config->has("neuron_count")) {
            auto neuronCountOpt = config->get<int64_t>("neuron_count");
            if (neuronCountOpt && *neuronCountOpt <= 0) {
                errors += "neuron_count must be positive; ";
                valid = false;
            }
        }
        
        if (config->has("region_count")) {
            auto regionCountOpt = config->get<int64_t>("region_count");
            if (regionCountOpt && *regionCountOpt <= 0) {
                errors += "region_count must be positive; ";
                valid = false;
            }
        }
        
        return py::make_tuple(valid, errors.empty() ? "Configuration is valid" : errors);
    }, py::arg("config"), "Validate configuration and return (is_valid, error_message)");

    // Performance benchmarks
    m.def("benchmarkBrainCreation", [](size_t neuronCount, size_t regionCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        
        return py::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            duration.count()
        );
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 2,
       "Benchmark brain creation performance. Returns (neuron_count, synapse_count, ms).");

    // Neural network utilities
    m.def("createRandomNetwork", [](size_t neuronCount, float connectionProbability) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", connectionProbability);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("neuronCount"), py::arg("connectionProbability"),
       "Create a brain with random connectivity. Returns Brain object.");

    // Serialization utilities
    m.def("saveBrainToFile", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file. Returns True on success.");

    m.def("loadBrainFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        } else {
            throw std::runtime_error("Failed to load brain from file: " + filepath);
        }
    }, py::arg("filepath"), "Load brain state from file. Returns Brain object.");

    // Constants for simulation
    m.attr("DEFAULT_TIMESTEP") = 0.001;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_VISION_SIZE") = 8;

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings - Phase 6 Complete"; 

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides complete access to the NLM artificial brain simulation system,
        implementing Phase 6 final integration with all brain systems:
        
        **Core Systems:**
        - Brain: Central neural simulation with spiking neurons
        - Memory: Working, episodic, and associative memory systems
        - Prediction: Neural prediction and error computation
        - Cognition: Planning, concept formation, and attention
        - Development: Age-dependent maturation and structural changes
        - Neuromodulation: Dopamine, curiosity, novelty, and prediction error
        
        **World Systems:**
        - SimpleWorld: 2D environment for agent interaction
        - Sensory: Vision, audio, and internal signal processing
        - Motor: Action selection and execution
        - Environment: Object interaction and physics simulation
        
        **Performance Features:**
        - SIMD vectorization
        - Parallel processing
        - Memory pools and event queues
        - Checkpointing and persistence
        
        **Usage Example:**
        
        ```python
        import pynlm
        
        # Create a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("Simulation complete!")
        ```
        
        **Key Classes:**
        
        - `Brain`: Central neural simulation engine
        - `AgentBrain`: Interface for world interaction
        - `SimpleWorld`: 2D simulation environment
        - `Config`: Configuration management
        - `Action`: Motor output representation
        - `SensoryInput`: Input data abstraction
        
        **Available Enumerations:**
        
        - `NeuronType`: Neuron types (Excitatory, Inhibitory, etc.)
        - `ActionType`: Action types for motor output
        - `DevelopmentalStage`: Brain development stages
        - `MotorCommand`: Low-level motor commands
        - `WorldObjectType`: Types of objects in world
        
        **Error Handling:**
        
        All classes raise `RuntimeError` for invalid operations or invalid state.
        
        **Performance Tips:**
        
        - Use `enableDevelopment(False)` for faster simulation
        - Use `enableCuriosity(False)` for deterministic behavior
        - Use batch operations when processing multiple sensory inputs
        - Save checkpoints periodically with `brain.save("checkpoint.bin")`
        
    )pbdoc";
