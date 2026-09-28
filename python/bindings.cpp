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
        
        // Enhanced batch processing methods
        .def("runBatch", [](Brain& self, SimulationStep start_step, size_t num_steps) {
            for (SimulationStep i = start_step; i < start_step + num_steps; ++i) {
                self.step(i);
            }
        }, py::arg("start_step"), py::arg("num_steps"),
             "Run a batch of simulation steps efficiently")
        
        .def("runEpisode", [](Brain& self, size_t num_steps, bool record_stats = false) {
            std::vector<std::map<std::string, double>> stats;
            for (SimulationStep i = 0; i < static_cast<SimulationStep>(num_steps); ++i) {
                self.step(i);
                if (record_stats) {
                    std::map<std::string, double> step_stats;
                    step_stats["step"] = i;
                    step_stats["firing_neuron_count"] = self.getFiringNeuronCount();
                    step_stats["spike_count"] = self.getTotalSpikeCount();
                    step_stats["average_firing_rate"] = self.getAverageFiringRate();
                    stats.push_back(step_stats);
                }
            }
            return stats;
        }, py::arg("num_steps") = 100, py::arg("record_stats") = false,
             "Run a complete simulation episode with optional statistics recording")
        
        .def("runWithRewards", [](Brain& self, size_t num_steps, 
                                const std::vector<double>& rewards) {
            if (rewards.size() < num_steps) {
                throw std::runtime_error("Reward vector size must match num_steps");
            }
            std::vector<double> cumulative_rewards;
            double cumulative = 0.0;
            for (SimulationStep i = 0; i < static_cast<SimulationStep>(num_steps); ++i) {
                self.step(i);
                cumulative += rewards[i];
                cumulative_rewards.push_back(cumulative);
            }
            return cumulative_rewards;
        }, py::arg("num_steps"), py::arg("rewards"),
             "Run simulation while accumulating rewards")
        
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
        
        // Advanced brain manipulation methods
        .def("createCustomNeuron", [](Brain& self, NeuronType type, float x = 0.0f, float y = 0.0f, float z = 0.0f, float radius = 1.0f) {
            auto region = self.getSpikeSystem()->getRegion();
            return region->createNeuron(type, x, y, z, radius);
        }, py::arg("type"), py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("z") = 0.0f, py::arg("radius") = 1.0f,
             "Create a custom neuron with specified parameters")
        
        .def("createCustomSynapse", [](Brain& self, NeuronId pre, NeuronId post, 
                                      SynapseType type, float weight = 1.0f,
                                      float delay = 1.0f) {
            auto region = self.getSpikeSystem()->getRegion();
            region->addSynapse(pre, post, type, weight, delay);
        }, py::arg("pre"), py::arg("post"), py::arg("type"), py::arg("weight") = 1.0f,
            py::arg("delay") = 1.0f,
             "Create a custom synapse between two neurons")
        
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f,
             py::arg("delay") = 1,
             "Add connection between neural regions")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove connection between neural regions")
        .def("getExcitatoryInhibitionRatio", &Brain::getExcitatoryInhibitionRatio,
              "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
              "Get count of pending spike events")
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
        
        // ========== ADVANCED BRAIN MANIPULATION ==========
        
        // Custom neuron creation and manipulation
        .def("createCustomNeuron", [](Brain& self, const std::string& name = "",
                                   NeuronType type = NeuronType::Internal,
                                   float x = 0.0f, float y = 0.0f, float z = 0.0f, float radius = 1.0f) {
            return self.createCustomNeuron(name, type, x, y, z, radius);
        }, py::arg("name") = "", py::arg("type") = NeuronType::Internal,
           py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("z") = 0.0f, py::arg("radius") = 1.0f,
              "Create a custom neuron with specified parameters")
        
        .def("setNeuronProperties", [](Brain& self, NeuronId neuron, const std::string& name = "",
                                    NeuronType type = NeuronType::Internal,
                                    float threshold = -55.0f, float restingPotential = -70.0f, float resetPotential = -70.0f,
                                    float leakConductance = 10.0f, uint32_t refractoryPeriod = 5,
                                    float adaptationRate = 0.01f, bool plasticityEnabled = true) {
            return self.setNeuronProperties(neuron, name, type, threshold, restingPotential, resetPotential,
                                          leakConductance, refractoryPeriod, adaptationRate, plasticityEnabled);
        }, py::arg("neuron"), py::arg("name") = "", py::arg("type") = NeuronType::Internal,
           py::arg("threshold") = -55.0f, py::arg("resting_potential") = -70.0f, py::arg("reset_potential") = -70.0f,
           py::arg("leak_conductance") = 10.0f, py::arg("refractory_period") = 5, py::arg("adaptation_rate") = 0.01f,
           py::arg("plasticity_enabled") = true,
              "Set neuron properties after creation")
        
        // Advanced synapse management
        .def("createCustomSynapse", [](Brain& self, NeuronId pre, NeuronId post,
                                       SynapseType type = SynapseType::Excitatory,
                                       float weight = 1.0f, float delay = 1, bool plasticityEnabled = true,
                                       float initialEfficacy = 0.0f) {
            return self.createCustomSynapse(pre, post, type, weight, delay, plasticityEnabled, initialEfficacy);
        }, py::arg("pre"), py::arg("post"), py::arg("type") = SynapseType::Excitatory,
           py::arg("weight") = 1.0f, py::arg("delay") = 1, py::arg("plasticity_enabled") = true,
           py::arg("initial_efficacy") = 0.0f,
              "Create a custom synapse between two neurons")
        
        .def("modifySynapseProperties", [](Brain& self, SynapseId synapse, float weight = -1.0f, float delay = -1,
                                           bool plasticityEnabled = true, float efficacy = -1.0f) {
            return self.modifySynapseProperties(synapse, weight, delay, plasticityEnabled, efficacy);
        }, py::arg("synapse"), py::arg("weight") = -1.0f, py::arg("delay") = -1, py::arg("plasticity_enabled") = true,
           py::arg("efficacy") = -1.0f,
              "Modify synapse properties after creation")
        
        .def("getInterRegionConnections", &Brain::getInterRegionConnections,
              "Get all inter-region connections")
        
        // Brain structure manipulation
        .def("addCustomRegion", [](Brain& self, const std::string& name = "",
                                  NeuronType defaultType = NeuronType::Internal,
                                  bool enableStructuralPlasticity = true) {
            return self.addCustomRegion(name, defaultType, enableStructuralPlasticity);
        }, py::arg("name") = "", py::arg("default_type") = NeuronType::Internal,
           py::arg("enable_structural_plasticity") = true,
              "Create a region with specific properties")
        
        .def("createNeuronPopulation", [](Brain& self, RegionId region, const std::string& name = "",
                                         NeuronType type = NeuronType::Excitatory, size_t size = 10) {
            return self.createNeuronPopulation(region, name, type, size);
        }, py::arg("region"), py::arg("name") = "", py::arg("type") = NeuronType::Excitatory,
           py::arg("size") = 10,
              "Create a neuron population with specific characteristics")
        
        .def("connectRegions", [](Brain& self, RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1,
                                 const PlasticityFlags& plasticityFlags = PlasticityFlags()) {
            return self.connectRegions(source, target, weight, delay, plasticityFlags);
        }, py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
           py::arg("plasticity_flags") = PlasticityFlags(),
              "Create inter-region connections with specific parameters")
        
        // Advanced simulation control
        .def("stepWithDetailedStats", [](Brain& self, SimulationStep currentStep) {
            auto detailedStats = self.stepWithDetailedStats(currentStep);
            // Convert to Python-friendly format
            py::dict result;
            for (const auto& [step, stats] : detailedStats) {
                py::dict stepDict;
                for (const auto& [key, value] : stats) {
                    stepDict[key] = value;
                }
                result[std::to_string(step)] = stepDict;
            }
            return result;
        }, py::arg("currentStep"),
              "Get detailed statistics for each simulation step")
        
        .def("setCustomTimestep", &Brain::setCustomTimestep, py::arg("timestep"),
              "Override simulation timestep dynamically")
        
        .def("pauseResumeSimulation", &Brain::pauseResumeSimulation, py::arg("pause"),
              "Control simulation execution")
        
        .def("isSimulationPaused", &Brain::isSimulationPaused,
              "Check if simulation is paused")
        
        // Enhanced export/import
        .def("exportConnectivityMatrix", &Brain::exportConnectivityMatrix,
              "Export brain connectivity as matrix (string)")
        
        .def("exportNeuralActivity", &Brain::exportNeuralActivity,
              "Export neural activity patterns (string)")
        
        .def("exportDetailedState", &Brain::exportDetailedState,
              "Export comprehensive brain state (string)")
        
        // Advanced configuration
        .def("mergeConfig", &Brain::mergeConfig, py::arg("otherConfig"), py::arg("overwriteExisting") = true,
              "Merge multiple configuration sources")
        
        .def("validateConfig", &Brain::validateConfig,
              "Validate configuration parameters")
        
        .def("getConfigStats", &Brain::getConfigStats,
              "Get statistics about current configuration")
