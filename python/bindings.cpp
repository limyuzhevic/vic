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
             "Configure world dimensions and sensory capabilities")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object (Resource, Hazard, etc.)")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world generation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed")
        // New: Enhanced world methods
        .def("getVision", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getVision();
        }, "Get vision data as list of floats")
        .def("getTouch", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getTouch();
        }, "Get touch data as list of floats")
        .def("getInternalSignals", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getInternal();
        }, "Get internal signals as list of floats")
        .def("getObjects", [](SimpleWorld& self) {
            std::vector<WorldObject> objects;
            for (const auto& obj : self.getObjects()) {
                objects.push_back(obj);
            }
            return objects;
        }, "Get all active world objects")
        .def("getAgentEnergy", [](SimpleWorld& self) {
            return self.getAgentBody().energy;
        }, "Get current agent energy")
        .def("getAgentHealth", [](SimpleWorld& self) {
            return self.getAgentBody().health;
        }, "Get current agent health")

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
             "Log brain status")
        // New: Python-friendly debugging methods
        .def("getNeuronStatistics", [](const Brain& self) {
            return std::make_tuple(self.getTotalNeuronCount(),
                                   self.getTotalSynapseCount(),
                                   self.getFiringNeuronCount(),
                                   self.getActiveNeuronCount());
        }, "Get basic neuron statistics as a tuple")
        .def("getDetailedStats", [](Brain& self) {
            nlohmann::json stats;
            stats["neurons.total"] = self.getTotalNeuronCount();
            stats["neurons.firing"] = self.getFiringNeuronCount();
            stats["neurons.active"] = self.getActiveNeuronCount();
            stats["neurons.spikes"] = self.getTotalSpikeCount();
            stats["synapses.total"] = self.getTotalSynapseCount();
            stats["development.stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats.dump();
        }, "Get detailed brain statistics as JSON")

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

// Python-friendly helper functions
namespace {
    // Convenience function for running a simple agent simulation
    struct AgentSimulationHelper {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<AgentBrain> agent;
        bool rewardEnabled = false;
        bool curiosityEnabled = false;
        bool developmentEnabled = false;
        bool checkpointEnabled = false;
        std::string checkpointFile;
        int maxSteps = 1000;
        
        AgentSimulationHelper() = default;
        
        // Configuration and setup
        void configure(const std::string& config_file = "") {
            auto config = std::make_shared<Config>();
            if (!config_file.empty()) {
                config->loadFromFile(config_file);
            }
            brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            world = std::make_shared<SimpleWorld>();
            world->configure(20.0f, 20.0f, 8, 8);
            world->reset();
            world->setAgentStart(10.0f, 10.0f);
            
            agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
        }
        
        // Run simulation with default settings
        std::vector<std::map<std::string, float>> run() {
            std::vector<std::map<std::string, float>> history;
            
            for (int step = 0; step < maxSteps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process in brain
                agent->processSensoryInput(percept);
                brain->step(step);
                
                // Get action and apply
                auto motor_cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
                
                // Apply neuromodulation if enabled
                if (rewardEnabled) {
                    float reward = 0.0f;
                    if (!percept.getInternal().empty()) {
                        reward = percept.getInternal()[0]; // Energy level
                    }
                    agent->applyRewardModulation(reward, 0.0f);
                }
                
                if (developmentEnabled) {
                    agent->updateDevelopment(0.1);
                }
                
                // Record statistics
                if (step % 10 == 0) {
                    std::map<std::string, float> stats;
                    stats["step"] = static_cast<float>(step);
                    stats["time"] = world->getSimulationTime();
                    stats["energy"] = agent->getBrain()->getTotalNeuronCount();
                    stats["firing"] = brain->getFiringNeuronCount();
                    stats["spikes"] = brain->getTotalSpikeCount();
                    stats["curiosity"] = agent->getCuriosityLevel();
                    stats["novelty"] = agent->getNoveltyLevel();
                    stats["dopamine"] = agent->getNeuromodulationLevel();
                    history.push_back(stats);
                }
                
                // Checkpoint if enabled
                if (checkpointEnabled && step % 100 == 0) {
                    brain->save(checkpointFile + "_step_" + std::to_string(step) + ".bin");
                }
            }
            
            return history;
        }
        
        // Run episode with complete episode tracking
        std::tuple<int, float, float, std::vector<std::map<std::string, float>>> runEpisode(int steps) {
            auto history = run();
            int total_spikes = 0;
            float avg_firing = 0.0f;
            float total_curiosity = 0.0f;
            int count = 0;
            
            for (const auto& stats : history) {
                total_spikes += stats.at("spikes");
                avg_firing += stats.at("firing");
                total_curiosity += stats.at("curiosity");
                count++;
            }
            
            if (count > 0) {
                avg_firing /= count;
                total_curiosity /= count;
            }
            
            return std::make_tuple(total_spikes, avg_firing, total_curiosity, history);
        }
    };
}

// Create a helper function for Python
m.def("runAgentSimulation", [](const std::string& config_file = "") {
    auto helper = std::make_unique<AgentSimulationHelper>();
    helper->configure(config_file);
    return helper->run();
}, py::arg("config_file") = "",
     "Run a complete agent simulation with default settings and return history");

m.def("runEpisode", [](const std::string& config_file = "", int steps = 1000) {
    auto helper = std::make_unique<AgentSimulationHelper>();
    helper->configure(config_file);
    helper->maxSteps = steps;
    return helper->runEpisode(steps);
}, py::arg("config_file") = "", py::arg("steps") = 1000,
     "Run a single episode simulation and return statistics");

m.def("runPhase6Demo", []() {
    // Import the Phase 6 demo function
    std::cout << "Phase 6 demo not directly available in Python bindings" << std::endl;
    std::cout << "Use runEpisode() for agent simulations instead." << std::endl;
    return std::vector<std::map<std::string, float>>();
}, "Run Phase 6 integration demo (placeholder)");

m.def("createQuickBrain", []() -> std::shared_ptr<Brain> {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.connection_probability", 0.1);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}, "Create a quick, ready-to-use brain with sensible defaults");

m.def("createQuickWorld", []() -> std::shared_ptr<SimpleWorld> {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20.0f, 20.0f, 8, 8);
    world->reset();
    world->setAgentStart(10.0f, 10.0f);
    return world;
}, "Create a quick, ready-to-use world with default settings");

m.def("createQuickAgent", []() -> std::shared_ptr<AgentBrain> {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    auto agent = std::make_shared<AgentBrain>(brain);
    return agent;
}, "Create a quick, ready-to-use agent brain");

m.def("validateConfiguration", [](const std::string& config_file) -> bool {
    auto config = std::make_shared<Config>();
    try {
        config->loadFromFile(config_file);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return false;
    }
}, py::arg("config_file"),
     "Validate configuration file and return success status");

m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps = 100) -> double {
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    return duration.count() / 1000.0; // Return seconds
}, py::arg("brain"), py::arg("steps") = 100,
     "Benchmark brain performance over specified steps (returns seconds)");

m.def("printBrainStats", [](const std::shared_ptr<Brain>& brain) {
    std::cout << "=== BRAIN STATISTICS ===" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Active: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
}, "Print formatted brain statistics to console");

m.def("printWorldStats", [](const std::shared_ptr<SimpleWorld>& world) {
    std::cout << "=== WORLD STATISTICS ===" << std::endl;
    std::cout << "Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
    std::cout << "Simulation Time: " << world->getSimulationTime() << std::endl;
    std::cout << "Max Energy: " << world->getMaxEnergy() << std::endl;
    std::cout << "Energy Decay: " << world->getEnergyDecayRate() << std::endl;
    auto body = world->getAgentBody();
    std::cout << "Agent Energy: " << body.energy << std::endl;
    std::cout << "Agent Health: " << body.health << std::endl;
    std::cout << "Agent Age: " << body.age << std::endl;
}, "Print formatted world statistics to console");

m.def("createCompleteSetup", []() {
    std::cout << "=== CREATING COMPLETE NLM SETUP ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    std::cout << "✓ Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20.0f, 20.0f, 8, 8);
    world->reset();
    world->setAgentStart(10.0f, 10.0f);
    std::cout << "✓ World configured (20x20) with agent at center" << std::endl;
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    std::cout << "✓ Agent brain created and initialized" << std::endl;
    
    // Enable key subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    std::cout << "✓ Key subsystems enabled (reward, curiosity, development)" << std::endl;
    
    std::cout << "=== SETUP COMPLETE ===" << std::endl;
    std::cout << "Ready to run simulation with: agent.processSensoryInput(world.getSensoryPercept())" << std::endl;
    
    return std::make_tuple(brain, world, agent);
}, "Create a complete NLM setup with brain, world, and agent (returns tuple)");

m.def("__version__", []() { return "0.1.0"; }, "Get NLM Python bindings version");

m.def("getAvailableVersions", []() {
    py::dict versions;
    versions["python"] = "0.1.0";
    versions["cpp"] = "Phase 6";
    versions["api"] = "Enhanced";
    return versions;
}, "Get version information for all components");

m.def("help", []() {
    std::cout << "=== NLM Python Bindings Help ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Quick Start:" << std::endl;
    std::cout << "  brain, world, agent = pynlm.createCompleteSetup()" << std::endl;
    std::cout << "  for step in range(100):" << std::endl;
    std::cout << "      world.update(0.1)" << std::endl;
    std::cout << "      agent.processSensoryInput(world.getSensoryPercept())" << std::endl;
    std::cout << "      brain.step(step)" << std::endl;
    std::cout << "      world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Statistics:" << std::endl;
    std::cout << "  pynlm.printBrainStats(brain)" << std::endl;
    std::cout << "  pynlm.printWorldStats(world)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Benchmark:" << std::endl;
    std::cout << "  time = pynlm.benchmarkBrain(brain, 100)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Run episode:" << std::endl;
    std::cout << "  spikes, avg_firing, curiosity, history = pynlm.runEpisode(1000)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more help, see: docs/HOW_TO_USE.md" << std::endl;
}, "Print NLM Python bindings help information");

} // namespace nlm
