#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Helper functions and utilities for Python bindings

// Helper function to create Pythonic error classes
py::object createRuntimeError(const std::string& message) {
    return py::cast(std::runtime_error(message));
}

// Helper function to format neuron stats for Python
std::string formatNeuronStats(const Brain& brain) {
    std::stringstream ss;
    ss << "Brain Statistics:\n";
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    ss << "  Total Spikes: " << brain.getTotalSpikeCount() << "\n";
    ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    ss << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage());
    return ss.str();
}

// Context manager for brain sessions
class BrainSession {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
public:
    BrainSession(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, std::shared_ptr<AgentBrain> a)
        : brain(b), world(w), agent(a) {}
    
    void runSimulation(int steps, float timestep = 0.1f) {
        for (int i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }
    
    std::string getStats() const {
        return formatNeuronStats(*brain);
    }
    
    void reset() {
        if (brain) brain->reset();
        if (world) world->reset();
        if (agent) agent->reset();
    }
};

// Pythonic property system for better API
struct PythonicBrainWrapper {
    std::shared_ptr<Brain> brain;
    
    PythonicBrainWrapper(std::shared_ptr<Brain> b) : brain(b) {}
    
    // Pythonic properties
    size_t getTotalNeuronCount() const { return brain->getTotalNeuronCount(); }
    size_t getTotalSynapseCount() const { return brain->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain->getFiringNeuronCount(); }
    size_t getTotalSpikeCount() const { return brain->getTotalSpikeCount(); }
    float getAverageFiringRate() const { return brain->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain->getExcitationInhibitionRatio(); }
    int getDevelopmentalStage() const { return static_cast<int>(brain->getDevelopmentalStage()); }
    
    // Convenience methods
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Brain Summary - Neurons: " << brain->getTotalNeuronCount()
           << ", Spikes: " << brain->getTotalSpikeCount()
           << ", Stage: " << static_cast<int>(brain->getDevelopmentalStage());
        return ss.str();
    }
    
    std::string getDetailedStats() const {
        return formatNeuronStats(*brain);
    }
    
    void enableAllSystems() {
        if (auto config = brain->getConfig()) {
            config->set("neuromodulation.enabled", true);
            config->set("memory.working.enabled", true);
            config->set("cognition.enabled", true);
            config->set("development.enabled", true);
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Key Features:
        - Neural network simulation with LIF dynamics
        - Multi-system integration (memory, cognition, neuromodulation)
        - Environment interaction (sensory, motor)
        - Developmental plasticity and learning
        - Advanced Pythonic API with properties and context managers
        - Comprehensive error handling and validation
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

    // Enhanced Pythonic brain interface with properties and convenience methods
    py::class_<PythonicBrainWrapper>(m, "PythonicBrain", R"pbdoc(Enhanced Pythonic brain interface with properties and convenience methods)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("getSummary", &PythonicBrainWrapper::getSummary,
             "Get a concise summary of brain state")
        .def("getTotalNeuronCount", &PythonicBrainWrapper::getTotalNeuronCount)
        .def("getTotalSynapseCount", &PythonicBrainWrapper::getTotalSynapseCount)
        .def("getActiveNeuronCount", &PythonicBrainWrapper::getActiveNeuronCount)
        .def("getFiringNeuronCount", &PythonicBrainWrapper::getFiringNeuronCount)
        .def("getTotalSpikeCount", &PythonicBrainWrapper::getTotalSpikeCount)
        .def("getAverageFiringRate", &PythonicBrainWrapper::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &PythonicBrainWrapper::getExcitationInhibitionRatio)
        .def("getDevelopmentalStage", &PythonicBrainWrapper::getDevelopmentalStage)
        .def("getDetailedStats", &PythonicBrainWrapper::getDetailedStats,
             "Get detailed statistics about the brain (internal neurons, synapses, firing rates)")
        .def("enableAllSystems", &PythonicBrainWrapper::enableAllSystems,
             "Enable all NLM systems (neuromodulation, memory, cognition, development)");

    // Utility functions for Python
    m.def("formatNeuronStats", &formatNeuronStats,
          "Format brain statistics into a readable string", py::arg("brain"));

    m.def("createBrainSession", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<BrainSession>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    "Create a brain session context manager for running simulations");

    // Convenience factory functions
    m.def("createOptimizedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("performance.optimization_level", "high");
        config->set("memory.pooling", true);
        config->set("neural.dynamics.method", "lfp");
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        return config;
    }, "Create an optimized configuration for performance");

    // Advanced simulation utilities
    m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int steps, float timestep = 0.1f) {
        auto session = std::make_shared<BrainSession>(brain, world, agent);
        session->runSimulation(steps, timestep);
        return session->getStats();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    py::arg("steps"), py::arg("timestep") = 0.1f,
    "Run a complete agent simulation with automatic statistics collection");

    // Configuration utilities
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (config->has("neural.region_count") && config->get<int64_t>("neural.region_count") <= 0) {
            errors.push_back("neural.region_count must be positive");
        }
        
        if (config->has("performance.simulation_steps") && config->get<int64_t>("performance.simulation_steps") < 0) {
            errors.push_back("performance.simulation_steps cannot be negative");
        }
        
        if (config->has("brain.boundary_check.enabled") && !config->get<bool>("brain.boundary_check.enabled")) {
            warnings.push_back("Boundary checking is disabled - may cause simulation errors");
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("config"),
    "Validate configuration and return list of errors and warnings");

    m.def("printConfigSummary", [](std::shared_ptr<Config> config) {
        return config->summary();
    }, py::arg("config"),
    "Get a human-readable summary of configuration settings");

    m.def("exportConfigToJson", [](std::shared_ptr<Config> config, const std::string& filepath) {
        return config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"),
    "Export configuration to JSON file");

    m.def("importConfigFromJson", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"),
    "Import configuration from JSON file");

// Add Brain session context manager for Pythonic workflow management
    py::class_<BrainSession>(m, "BrainSession", R"pbdoc(Context manager for brain simulation sessions)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("runSimulation", &BrainSession::runSimulation,
             "Run a complete simulation session", py::arg("steps"), py::arg("timestep") = 0.1f)
        .def("getStats", &BrainSession::getStats,
             "Get detailed statistics about the session")
        .def("reset", &BrainSession::reset,
             "Reset the session state")
        .def("__enter__", &BrainSession::__enter__)
        .def("__exit__", &BrainSession::__exit__);

    // Add Pythonic brain wrapper for enhanced API
    py::class_<PythonicBrainWrapper>(m, "PythonicBrain", R"pbdoc(Enhanced Pythonic brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("getSummary", &PythonicBrainWrapper::getSummary,
             "Get a concise summary of brain state")
        .def("getTotalNeuronCount", &PythonicBrainWrapper::getTotalNeuronCount)
        .def("getTotalSynapseCount", &PythonicBrainWrapper::getTotalSynapseCount)
        .def("getActiveNeuronCount", &PythonicBrainWrapper::getActiveNeuronCount)
        .def("getFiringNeuronCount", &PythonicBrainWrapper::getFiringNeuronCount)
        .def("getTotalSpikeCount", &PythonicBrainWrapper::getTotalSpikeCount)
        .def("getAverageFiringRate", &PythonicBrainWrapper::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &PythonicBrainWrapper::getExcitationInhibitionRatio)
        .def("getDevelopmentalStage", &PythonicBrainWrapper::getDevelopmentalStage)
        .def("getDetailedStats", &PythonicBrainWrapper::getDetailedStats,
             "Get detailed statistics about the brain")
        .def("enableAllSystems", &PythonicBrainWrapper::enableAllSystems,
             "Enable all NLM systems (neuromodulation, memory, cognition, development)");

    // Add utility functions
    m.def("formatNeuronStats", &formatNeuronStats,
          "Format brain statistics into a readable string", py::arg("brain"));

    m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int steps, float timestep = 0.1f) {
        auto session = std::make_shared<BrainSession>(brain, world, agent);
        session->runSimulation(steps, timestep);
        return session->getStats();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    py::arg("steps"), py::arg("timestep") = 0.1f,
    "Run a complete agent simulation with automatic statistics collection");

    // Configuration validation and utilities
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (config->has("neural.region_count") && config->get<int64_t>("neural.region_count") <= 0) {
            errors.push_back("neural.region_count must be positive");
        }
        
        if (config->has("performance.simulation_steps") && config->get<int64_t>("performance.simulation_steps") < 0) {
            errors.push_back("performance.simulation_steps cannot be negative");
        }
        
        if (config->has("brain.boundary_check.enabled") && !config->get<bool>("brain.boundary_check.enabled")) {
            warnings.push_back("Boundary checking is disabled - may cause simulation errors");
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("config"),
    "Validate configuration and return list of errors and warnings");

    m.def("printConfigSummary", [](std::shared_ptr<Config> config) {
        return config->summary();
    }, py::arg("config"),
    "Get a human-readable summary of configuration settings");

    m.def("exportConfigToJson", [](std::shared_ptr<Config> config, const std::string& filepath) {
        return config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"),
    "Export configuration to JSON file");

    m.def("importConfigFromJson", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"),
    "Import configuration from JSON file");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createOptimizedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("performance.optimization_level", "high");
        config->set("memory.pooling", true);
        config->set("neural.dynamics.method", "lfp");
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        return config;
    }, "Create an optimized configuration for performance");

    // World and agent factory functions
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Simulation utilities
    m.def("runSimulationWithMonitoring", [](std::shared_ptr<Brain> brain,
                                           std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent,
                                           int steps, float timestep = 0.1f) {
        auto session = std::make_shared<BrainSession>(brain, world, agent);
        session->runSimulation(steps, timestep);
        return session->getStats();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    py::arg("steps"), py::arg("timestep") = 0.1f,
    "Run simulation with monitoring and statistics collection");

    m.def("runEpisode", [](std::shared_ptr<Brain> brain,
                          std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent,
                          int steps, float timestep = 0.1f) {
        auto session = std::make_shared<BrainSession>(brain, world, agent);
        session->runSimulation(steps, timestep);
        return session->getStats();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    py::arg("steps"), py::arg("timestep") = 0.1f,
    "Run complete episode with automatic state management");

    // Convenience factory functions
    m.def("createBrainSession", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<BrainSession>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    "Create a brain session context manager for running simulations");

    // Advanced utilities
    m.def("benchmarkSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int steps, float timestep = 0.1f) {
        auto session = std::make_shared<BrainSession>(brain, world, agent);
        session->runSimulation(steps, timestep);
        auto stats = session->getStats();
        stats["benchmark_mode"] = true;
        stats["timestep"] = timestep;
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    py::arg("steps"), py::arg("timestep") = 0.1f,
    "Run benchmark simulation and return performance metrics");

    m.def("createExperimentSession", [](std::shared_ptr<Brain> brain,
                                       std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<BrainSession>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
    "Create experiment session for controlled trials");

    // Error handling and validation
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (config->has("neural.region_count") && config->get<int64_t>("neural.region_count") <= 0) {
            errors.push_back("neural.region_count must be positive");
        }
        
        if (config->has("performance.simulation_steps") && config->get<int64_t>("performance.simulation_steps") < 0) {
            errors.push_back("performance.simulation_steps cannot be negative");
        }
        
        if (config->has("brain.boundary_check.enabled") && !config->get<bool>("brain.boundary_check.enabled")) {
            warnings.push_back("Boundary checking is disabled - may cause simulation errors");
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("config"),
    "Validate configuration and return list of errors and warnings");

    // Constants and helper functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
