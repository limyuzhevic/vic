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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Advanced agent control
        .def("setMaxExploration", &AgentBrain::setMaxExploration,
             py::arg("max_exploration"),
             "Set maximum exploration level (0.0 to 1.0)")
        .def("getMaxExploration", &AgentBrain::getMaxExploration,
             "Get current maximum exploration level")
        .def("setTemperature", &AgentBrain::setTemperature,
             py::arg("temperature"),
             "Set exploration temperature for action selection (0.0 to 1.0)")
        .def("getTemperature", &AgentBrain::getTemperature,
             "Get current exploration temperature")
        .def("setEpsilon", &AgentBrain::setEpsilon,
             py::arg("epsilon"),
             "Set epsilon-greedy exploration rate")
        .def("getEpsilon", &AgentBrain::getEpsilon,
             "Get current epsilon-greedy exploration rate")
        
        // State snapshot and comparison
        .def("createSnapshot", &AgentBrain::createSnapshot,
             "Create a snapshot of current brain state")
        .def("compareSnapshots", &AgentBrain::compareSnapshots,
             py::arg("snapshot1"), py::arg("snapshot2"),
             "Compare two state snapshots")
        .def("applySnapshot", &AgentBrain::applySnapshot,
             py::arg("snapshot"),
             "Apply a state snapshot to reset brain")
        
        // Advanced neuromodulation
        .def("setDopamineLevel", &AgentBrain::setDopamineLevel,
             py::arg("level"),
             "Set dopamine level directly")
        .def("getDopamineLevel", &AgentBrain::getDopamineLevel,
             "Get current dopamine level")
        .def("getCuriositySignal", &AgentBrain::getCuriositySignal,
             "Get raw curiosity signal before thresholds")
        .def("getNoveltySignal", &AgentBrain::getNoveltySignal,
             "Get raw novelty signal before decay")
        .def("getPredictionErrorSignal", &AgentBrain::getPredictionErrorSignal,
             "Get raw prediction error signal")
        
        // Development control
        .def("setDevelopmentRate", &AgentBrain::setDevelopmentRate,
             py::arg("rate"),
             "Set development rate modifier (0.0 to 2.0)")
        .def("getDevelopmentRate", &AgentBrain::getDevelopmentRate,
             "Get current development rate modifier")
        .def("setCriticalPeriodEnd", &AgentBrain::setCriticalPeriodEnd,
             py::arg("time"),
             "Set end time of critical period")
        .def("getCriticalPeriodEnd", &AgentBrain::getCriticalPeriodEnd,
             "Get end time of critical period");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("saveConfig", [](const Config& cfg, const std::string& filepath) {
        cfg.saveToFile(filepath);
    }, py::arg("cfg"), py::arg("filepath"), "Save configuration to file");

    m.def("loadConfig", [](Config& cfg, const std::string& filepath) {
        return cfg.loadFromFile(filepath);
    }, py::arg("cfg"), py::arg("filepath"), "Load configuration from file");

    m.def("getConfigSummary", [](const Config& cfg) {
        return cfg.summary();
    }, py::arg("cfg"), "Get configuration summary");

    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, int steps, float timestep) {
        for (int i = 0; i < steps; ++i) {
            world->update(static_cast<double>(timestep));
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, static_cast<double>(i * timestep));
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep"),
       "Run complete simulation with brain-world interaction");

    m.def("analyzeBrainConnectivity", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "Brain Connectivity Analysis:\n";
        oss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        oss << "  Pending spike events: " << brain->getPendingSpikeEventCount() << "\n";
        oss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        oss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << ":1\n";
        return oss.str();
    }, py::arg("brain"), "Analyze brain connectivity and statistics");

    m.def("analyzeAgentPerformance", [](std::shared_ptr<AgentBrain> agent, 
                                        std::shared_ptr<SimpleWorld> world, 
                                        int steps) {
        float totalReward = 0.0f;
        float totalPredictionError = 0.0f;
        float avgCuriosity = 0.0f;
        float avgNovelty = 0.0f;
        
        for (int i = 0; i < steps; ++i) {
            totalReward += agent->getNeuromodulationLevel();
            totalPredictionError += std::abs(agent->getPredictionError());
            avgCuriosity += agent->getCuriosityLevel();
            avgNovelty += agent->getNoveltyLevel();
        }
        
        int count = std::max(steps, 1);
        avgCuriosity /= count;
        avgNovelty /= count;
        
        std::ostringstream oss;
        oss << "Agent Performance Analysis:\n";
        oss << "  Average neuromodulation level: " << totalReward / count << "\n";
        oss << "  Average prediction error: " << totalPredictionError / count << "\n";
        oss << "  Average curiosity level: " << avgCuriosity << "\n";
        oss << "  Average novelty level: " << avgNovelty << "\n";
        oss << "  Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
        oss << "  Reward modulation: " << (agent->isRewardModulationEnabled() ? "enabled" : "disabled") << "\n";
        oss << "  Structural plasticity: " << (agent->isStructuralPlasticityEnabled() ? "enabled" : "disabled") << "\n";
        oss << "  Curiosity: " << (agent->isCuriosityEnabled() ? "enabled" : "disabled") << "\n";
        return oss.str();
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), 
       "Analyze agent performance over simulation steps");

    m.def("visualizeNetworkState", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "=== Network State Visualization ===\n";
        oss << "Brain Regions:\n";
        
        for (const auto& regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                oss << "  Region " << regionId.index() << " (" << region->getName() << "):\n";
                oss << "    Neurons: " << region->getTotalNeuronCount() << "\n";
                oss << "    Populations: " << region->getPopulationCount() << "\n";
                oss << "    Synapses: " << region->getSynapseCount() << "\n";
                oss << "    Avg weight: " << region->getAverageSynapticWeight() << "\n";
            }
        }
        
        oss << "\nGlobal Statistics:\n";
        oss << "  Total neurons across all regions: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total synapses across all regions: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Spike system pending: " << brain->getPendingSpikeEventCount() << "\n";
        
        return oss.str();
    }, py::arg("brain"), "Generate ASCII visualization of network state");

    m.def("exportBrainData", [](std::shared_ptr<Brain> brain, const std::string& prefix) {
        std::ostringstream oss;
        oss << "Brain Data Export\n";
        oss << "==================\n";
        oss << "Export prefix: " << prefix << "\n";
        oss << "Timestamp: " << brain->getTotalSpikeCount() << " spikes\n";
        oss << "\nRegion Summary:\n";
        
        for (const auto& regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                oss << "  Region " << regionId.index() << ": " << region->getName() << "\n";
                oss << "    Neuron count: " << region->getTotalNeuronCount() << "\n";
                oss << "    Synapse count: " << region->getSynapseCount() << "\n";
                oss << "    Average synaptic weight: " << region->getAverageSynapticWeight() << "\n";
            }
        }
        
        oss << "\nGlobal Metrics:\n";
        oss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        oss << "  Pending spike events: " << brain->getPendingSpikeEventCount() << "\n";
        oss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        oss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << ":1\n";
        
        oss << "\nMemory Systems:\n";
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            oss << "  Working memory traces: " << workingMem->getActiveTraces() << "\n";
        }
        
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            oss << "  Episodic memory episodes: " << episodicMem->getEpisodeCount() << "\n";
        }
        
        oss << "\nNeuromodulation:\n";
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            oss << "  Dopamine level: " << dopamine->getLevel() << "\n";
            oss << "  Plasticity factor: " << dopamine->getPlasticityFactor() << "\n";
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            oss << "  Curiosity level: " << curiosity->getLevel() << "\n";
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            oss << "  Novelty level: " << novelty->getLevel() << "\n";
        }
        
        auto* predError = brain->getPredictionErrorSignal();
        if (predError) {
            oss << "  Prediction error: " << predError->getError() << "\n";
        }
        
        oss << "\nDevelopment:\n";
        oss << "  Current stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        return oss.str();
    }, py::arg("brain"), py::arg("prefix"), "Export comprehensive brain data as string");

} // namespace nlm
