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
              "Log brain status")
         
         // Advanced Brain convenience methods
         .def("quickRun", [](Brain& brain, size_t steps) {
             for (size_t i = 0; i < steps; ++i) {
                 brain.step(i);
             }
         }, py::arg("steps"), "Quick run of brain simulation for given number of steps")
         
         .def("runWithInput", [](Brain& brain, const SensoryInput& input, size_t steps) {
             brain.receiveSensoryInput(input);
             for (size_t i = 0; i < steps; ++i) {
                 brain.step(i);
             }
         }, py::arg("input"), py::arg("steps"), "Run brain simulation with initial sensory input")
         
         .def("getBrainStats", [](const Brain& brain) {
             std::map<std::string, size_t> stats;
             stats["total_neurons"] = brain.getTotalNeuronCount();
             stats["total_synapses"] = brain.getTotalSynapseCount();
             stats["regions"] = brain.getRegionCount();
             stats["active_neurons"] = brain.getActiveNeuronCount();
             stats["firing_neurons"] = brain.getFiringNeuronCount();
             stats["total_spikes"] = brain.getTotalSpikeCount();
             stats["e_i_ratio"] = static_cast<size_t>(brain.getExcitationInhibitionRatio() * 100);
             return stats;
         }, "Get comprehensive brain statistics")
         
         .def("getPerformanceMetrics", [](const Brain& brain) {
             return py::make_tuple(
                 brain.getAverageFiringRate(),
                 brain.getExcitationInhibitionRatio(),
                 brain.getTotalSpikeCount() / static_cast<float>(std::max(size_t(1), brain.getTotalNeuronCount()))
             );
         }, "Get performance metrics: firing_rate, e_i_ratio, spikes_per_neuron")
         
         .def("isHealthy", [](const Brain& brain) {
             // Consider brain healthy if it has reasonable activity and connectivity
             float firingRate = brain.getAverageFiringRate();
             size_t totalNeurons = brain.getTotalNeuronCount();
             size_t totalSynapses = brain.getTotalSynapseCount();
             
             bool reasonableFiring = firingRate > 0.1f && firingRate < 100.0f;
             bool reasonableConnectivity = totalSynapses > 0 && totalSynapses < totalNeurons * 10;
             
             return reasonableFiring && reasonableConnectivity;
         }, "Check if brain is in a healthy state")
         
         .def("resetWithConfig", [](Brain& brain, std::shared_ptr<Config> config) {
             // Create a new brain with the given config
             return std::make_shared<Brain>(config);
         }, py::arg("config"), "Reset brain with new configuration (returns new brain)")
         
         .def("configureSimulation", [](Brain& brain, double timestep, size_t maxSteps) {
             auto config = brain.getConfig();
             config->set("simulation_timestep", timestep);
             config->set("max_simulation_steps", static_cast<int64_t>(maxSteps));
             return true;
         }, py::arg("timestep"), py::arg("maxSteps"), "Configure simulation parameters")
         
         .def("getDevelopmentStatus", [](const Brain& brain) {
             auto config = brain.getConfig();
             return std::make_tuple(
                 static_cast<int>(brain.getDevelopmentalStage()),
                 config->getOr<int64_t>("development_enabled", 0) > 0,
                 config->getOr<int64_t>("plasticity_enabled", 1) > 0
             );
         }, "Get development status information")
         
         .def("exportForAnalysis", &Brain::save,
              py::arg("filepath"),
              "Export brain state to file for external analysis")
         
         .def("importFromFile", &Brain::load,
              py::arg("filepath"),
              "Import brain state from file")
         
         .def("getIntegrationStatus", [](const Brain& brain) {
             std::map<std::string, bool> status;
             
             status["working_memory"] = brain.getWorkingMemory() != nullptr;
             status["episodic_memory"] = brain.getEpisodicMemory() != nullptr;
             status["prediction_system"] = brain.getPredictionSystem() != nullptr;
             status["planner"] = brain.getPlanner() != nullptr;
             status["concept_formation"] = brain.getConceptFormation() != nullptr;
             status["attention"] = brain.getAttention() != nullptr;
             status["development"] = brain.getDevelopmentSystem() != nullptr;
             status["dopamine"] = brain.getDopamine() != nullptr;
             status["curiosity"] = brain.getCuriosity() != nullptr;
             status["novelty"] = brain.getNovelty() != nullptr;
             
             return status;
         }, "Get integration status of all brain systems")
         
         .def("printSystemStatus", [](Brain& brain) {
             auto config = brain.getConfig();
             std::cout << "=== NLM Brain Status Report ===" << std::endl;
             std::cout << "Phase: " << config->getOr<int64_t>("phase", 6) << std::endl;
             std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
             std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
             std::cout << "Regions: " << brain.getRegionCount() << std::endl;
             std::cout << "Active: " << brain.getActiveNeuronCount() << std::endl;
             std::cout << "Firing: " << brain.getFiringNeuronCount() << std::endl;
             std::cout << "Spikes: " << brain.getTotalSpikeCount() << std::endl;
             std::cout << "Firing Rate: " << brain.getAverageFiringRate() << std::endl;
             std::cout << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
             std::cout << "Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
             
             // Check system integration
             std::cout << "\nSystem Integration:" << std::endl;
             std::cout << "  Working Memory: " << (brain.getWorkingMemory() ? "✅" : "❌") << std::endl;
             std::cout << "  Episodic Memory: " << (brain.getEpisodicMemory() ? "✅" : "❌") << std::endl;
             std::cout << "  Prediction System: " << (brain.getPredictionSystem() ? "✅" : "❌") << std::endl;
             std::cout << "  Planner: " << (brain.getPlanner() ? "✅" : "❌") << std::endl;
             std::cout << "  Concept Formation: " << (brain.getConceptFormation() ? "✅" : "❌") << std::endl;
             std::cout << "  Attention: " << (brain.getAttention() ? "✅" : "❌") << std::endl;
             std::cout << "  Development: " << (brain.getDevelopmentSystem() ? "✅" : "❌") << std::endl;
         }, "Print comprehensive brain system status report")

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
         
         // Advanced convenience methods
         .def("enableAllFeatures", &AgentBrain::enableRewardModulation, py::arg("enable"))
         .def("enableAllFeatures", [](AgentBrain& agent) {
             agent.enableRewardModulation(true);
             agent.enableStructuralPlasticity(true);
             agent.enableDevelopment(true);
             agent.enableCuriosity(true);
         }, "Enable all learning features at once")
         .def("enableAllFeatures", [](AgentBrain& agent, bool enable) {
             agent.enableRewardModulation(enable);
             agent.enableStructuralPlasticity(enable);
             agent.enableDevelopment(enable);
             agent.enableCuriosity(enable);
         }, "Enable/disable all learning features")
         
         .def("getAllNeuromodulationLevels", &AgentBrain::getNeuromodulationLevel,
              "Get current neuromodulation level")
         .def("getAllCuriosityLevels", [](const AgentBrain& agent) {
             return py::make_tuple(agent.getCuriosityLevel(), agent.getNoveltyLevel(), agent.getPredictionError());
         }, "Get all curiosity-related levels")
         
         .def("simulateEpisode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, size_t numSteps) {
             float totalReward = 0.0f;
             for (size_t step = 0; step < numSteps; ++step) {
                 // Get observation from world
                 SensoryPercept percept = world->getSensoryPercept();
                 
                 // Process sensory input
                 agent.processSensoryInput(percept);
                 
                 // Get motor command
                 MotorCommand cmd = agent.decodeMotorCommand();
                 
                 // Apply action to world
                 world->applyMotorCommand(cmd, world->getSimulationTime());
                 
                 // Compute reward (simplified)
                 float reward = world->getAgentBody().health > 0 ? 0.1f : -0.1f;
                 totalReward += reward;
                 
                 // Apply reward modulation
                 agent.applyRewardModulation(reward, 0.0f);
                 
                 // Update development (simplified)
                 agent.updateDevelopment(0.1f);
             }
             return totalReward;
         }, py::arg("world"), py::arg("numSteps"), "Run a complete episode of simulation")
         
         .def("trainModel", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, size_t numEpisodes) {
             float totalRewards = 0.0f;
             for (size_t episode = 0; episode < numEpisodes; ++episode) {
                 totalRewards += agent.simulateEpisode(world, 100);
             }
             return totalRewards / numEpisodes;
         }, py::arg("world"), py::arg("numEpisodes"), "Train the model over multiple episodes")
         
         .def("getStats", [](const AgentBrain& agent) {
             return py::make_tuple(
                 agent.getCuriosityLevel(),
                 agent.getNoveltyLevel(),
                 agent.getPredictionError(),
                 agent.getNeuromodulationLevel(),
                 agent.getDevelopmentalStage()
             );
         }, "Get comprehensive statistics about the agent's state")
         
         .def("resetForNewEpisode", &AgentBrain::reset,
              "Reset agent for a new episode with fresh state")
         
         .def("exportBrainState", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              "Export the complete brain state for analysis or persistence")
         
         .def("setLearningRate", &AgentBrain::enableRewardModulation,
              py::arg("enable"),
              "Set learning rate (alias for enableRewardModulation)")
         
         .def("setPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"),
              "Set structural plasticity (alias for enableStructuralPlasticity)")
         
         .def("setDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"),
              "Set development mode (alias for enableDevelopment)")
         
         .def("setExploration", &AgentBrain::enableCuriosity,
              py::arg("enable"),
              "Set exploration mode (alias for enableCuriosity)")
         
         .def("getBrainSize", [](const AgentBrain& agent) {
             auto brain = agent.getBrain();
             return std::make_tuple(
                 brain->getTotalNeuronCount(),
                 brain->getTotalSynapseCount(),
                 brain->getRegionCount()
             );
         }, "Get basic brain size statistics")
         
         .def("getActivityStats", [](const AgentBrain& agent) {
             auto brain = agent.getBrain();
             return py::make_tuple(
                 brain->getFiringNeuronCount(),
                 brain->getAverageFiringRate(),
                 brain->getTotalSpikeCount(),
                 brain->getExcitationInhibitionRatio()
             );
         }, "Get current neural activity statistics")
         
         .def("getDevelopmentalStats", [](const AgentBrain& agent) {
             return std::make_tuple(
                 static_cast<int>(agent.getDevelopmentalStage()),
                 agent.getNeuromodulationLevel(),
                 agent.getCuriosityLevel()
             );
         }, "Get developmental and neuromodulation statistics")
         
         .def("hasMemorySystems", [](const AgentBrain& agent) {
             auto brain = agent.getBrain();
             return std::make_tuple(
                 brain->getWorkingMemory() != nullptr,
                 brain->getEpisodicMemory() != nullptr,
                 brain->getPredictionSystem() != nullptr,
                 brain->getPlanner() != nullptr,
                 brain->getConceptFormation() != nullptr,
                 brain->getAttention() != nullptr
             );
         }, "Check which cognitive systems are available")
         
         .def("getMemoryStats", [](const AgentBrain& agent) {
             auto brain = agent.getBrain();
             std::map<std::string, size_t> stats;
             
             if (auto* wm = brain->getWorkingMemory()) {
                 stats["working_memory_traces"] = wm->getActiveTraces();
             }
             if (auto* em = brain->getEpisodicMemory()) {
                 stats["episodic_memory_episodes"] = em->getEpisodeCount();
             }
             if (auto* ps = brain->getPredictionSystem()) {
                 stats["prediction_error"] = static_cast<size_t>(ps->getPredictionError() * 100);
             }
             
             return stats;
         }, "Get detailed memory system statistics")
         
         .def("printReport", [](AgentBrain& agent) {
             auto brain = agent.getBrain();
             std::cout << "=== NLM Agent Report ===" << std::endl;
             std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
             std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
             std::cout << "Regions: " << brain->getRegionCount() << std::endl;
             std::cout << "Firing: " << brain->getFiringNeuronCount() << std::endl;
             std::cout << "Avg Firing Rate: " << brain->getAverageFiringRate() << std::endl;
             std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
             std::cout << "Neuromodulation: " << agent.getNeuromodulationLevel() << std::endl;
             std::cout << "Curiosity: " << agent.getCuriosityLevel() << std::endl;
             std::cout << "Novelty: " << agent.getNoveltyLevel() << std::endl;
             std::cout << "Prediction Error: " << agent.getPredictionError() << std::endl;
             std::cout << "Development: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
         }, "Print comprehensive agent report to console")

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
