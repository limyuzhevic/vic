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
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::runtime_error>(m, "ConfigurationError");

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
             "Save brain state to file. Returns True on success, False if save failed.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success, False if load failed.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name. Returns the new region's ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list")
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
             "Get excitation/inhibition balance ratio (high positive = excitation dominant, negative = inhibition dominant)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across all neurons")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. Use getDevelopmentalStage() to see current stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status including statistics and system information")
        // Additional statistics methods for advanced users
        .def("getMemoryStats", [](Brain& brain) {
            auto* mem = brain.getWorkingMemory();
            if (mem) {
                return std::make_tuple(
                    mem->getActiveTraces(),
                    mem->getCapacity(),
                    mem->getUtilization()
                );
            }
            return std::make_tuple(size_t(0), size_t(0), 0.0f);
        }, "Get working memory statistics as (active_traces, capacity, utilization_ratio)")
        .def("getDevelopmentProgress", [](Brain& brain) {
            auto stage = brain.getDevelopmentalStage();
            float progress = 0.0f;
            switch(stage) {
                case DevelopmentalStage::Initial: progress = 0.0f; break;
                case DevelopmentalStage::CriticalPeriod: progress = 0.25f; break;
                case DevelopmentalStage::Maturation: progress = 0.5f; break;
                case DevelopmentalStage::Adult: progress = 0.75f; break;
                case DevelopmentalStage::Aging: progress = 1.0f; break;
            }
            return std::make_tuple(static_cast<int>(stage), progress);
        }, "Get developmental stage and progress as (stage_int, progress_0_to_1)")
        .def("getNeuromodulationStats", [](Brain& brain) {
            auto* dopamine = brain.getDopamine();
            auto* novelty = brain.getNovelty();
            auto* curiosity = brain.getCuriosity();
            return std::make_tuple(
                dopamine ? dopamine->getLevel() : 0.0f,
                novelty ? novelty->getLevel() : 0.0f,
                curiosity ? curiosity->getLevel() : 0.0f
            );
        }, "Get neuromodulation levels as (dopamine, novelty, curiosity)")
        .def("getPredictionStats", [](Brain& brain) {
            auto* prediction = brain.getPredictionSystem();
            if (prediction) {
                return std::make_tuple(
                    prediction->getPredictionError(),
                    prediction->getConfidence(),
                    prediction->getNextStateConfidence()
                );
            }
            return std::make_tuple(0.0f, 0.0f, 0.0f);
        }, "Get prediction system statistics as (error, confidence, next_state_confidence)")
        .def("runIntegrationTest", [](Brain& brain) {
            // Run a quick integration test to verify all systems work
            bool ok = true;
            auto* workingMem = brain.getWorkingMemory();
            if (!workingMem) ok = false;
            
            auto* episodicMem = brain.getEpisodicMemory();
            if (!episodicMem) ok = false;
            
            auto* prediction = brain.getPredictionSystem();
            if (!prediction) ok = false;
            
            auto* planner = brain.getPlanner();
            if (!planner) ok = false;
            
            return ok;
        }, "Run integration test to verify all cognitive systems are connected. Returns True if integration successful.")
        .def("getLifetimeStats", [](Brain& brain) {
            // Collect comprehensive lifetime statistics
            return std::make_tuple(
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getTotalSpikeCount(),
                brain.getAverageFiringRate(),
                brain.getExcitationInhibitionRatio()
            );
        }, "Get comprehensive lifetime statistics as (neuron_count, synapse_count, total_spikes, avg_firing_rate, e_i_ratio)");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with a world. This sets up the sensory-motor loop.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get the expected sensory input size for the current world configuration")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get the expected motor output size for the current world configuration")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept from world and inject it into the brain's sensory neurons. This is the main perception step in the agent's loop.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into a motor command. The brain evaluates its motor neuron population and selects an action based on highest activity.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. The reward modulates synaptic plasticity (STDP) based on the difference between predicted and actual rewards.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. The brain's developmental stage affects plasticity rates and learning capabilities.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage of the brain (Initial, CriticalPeriod, Maturation, Adult, Aging)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current overall neuromodulation level affecting brain plasticity and learning")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level driving exploration of novel stimuli")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level of current sensory input (high = new/exciting stimuli)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal from the prediction system (difference between predicted and actual outcomes)")
        .def("reset", &AgentBrain::reset,
             "Reset agent to initial state for a new episode. Clears internal states and memory traces.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object for direct access to neural simulation")
        // Enable/Disable subsystem methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward-based modulation (affects learning from rewards)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity (growth/pruning of connections)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable developmental changes over time")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        // Status checking methods
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled")
        // Advanced AgentBrain methods
        .def("getCuriosityDrivenExploration", [](AgentBrain& agent) {
            float curiosity = agent.getCuriosityLevel();
            float novelty = agent.getNoveltyLevel();
            float predictionError = agent.getPredictionError();
            // Simple curiosity metric: weighted sum of components
            return curiosity * 0.5f + novelty * 0.3f + predictionError * 0.2f;
        }, "Get curiosity-driven exploration score (higher = more likely to explore novel things)")
        .def("getLearningCapacity", [](AgentBrain& agent) {
            float neuromodLevel = agent.getNeuromodulationLevel();
            auto stage = agent.getBrain()->getDevelopmentalStage();
            // Learning capacity depends on neuromodulation and developmental stage
            float stageFactor = 0.5f;
            switch(stage) {
                case DevelopmentalStage::CriticalPeriod: stageFactor = 2.0f; break;
                case DevelopmentalStage::Initial: stageFactor = 1.5f; break;
                case DevelopmentalStage::Adult: stageFactor = 1.0f; break;
                case DevelopmentalStage::Aging: stageFactor = 0.5f; break;
            }
            return neuromodLevel * stageFactor;
        }, "Get current learning capacity (higher = more capable of learning)")
        .def("getActionStatistics", [](AgentBrain& agent) {
            // Note: This would require tracking action statistics, returning a simple placeholder
            return std::make_tuple(0.0f, 0.0f, 0.0f); // exploration_rate, exploitation_rate, novelty_seeking
        }, "Get action selection statistics as (exploration_rate, exploitation_rate, novelty_seeking)")

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration. Returns a new Config object with default settings.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. The config parameter can be created using createDefaultConfig() or loaded from file.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple 2D world for NLM simulation. The world can be configured with dimensions and sensory capabilities.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. This connects a brain to a world for perception-action loops.");

    // Factory methods with error checking and validation
    m.def("createBrainFromFile", [](const std::string& configFile) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            throw std::runtime_error("Failed to load configuration from file: " + configFile);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configFile"), "Create a brain with configuration loaded from a JSON file. Raises RuntimeError if file cannot be loaded.");

    m.def("createBrainWithCustomSettings", [](const std::unordered_map<std::string, py::object>& settings) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : settings) {
            const std::string& key = pair.first;
            py::object value = pair.second;
            
            try {
                if (value.is(py::int_())) {
                    config->set(key, value.cast<int64_t>(), ConfigSource::Default);
                } else if (value.is(py::float_())) {
                    config->set(key, value.cast<float>(), ConfigSource::Default);
                } else if (value.is(py::str())) {
                    config->set(key, value.cast<std::string>(), ConfigSource::Default);
                } else if (value.is(py::bool_())) {
                    config->set(key, value.cast<bool>(), ConfigSource::Default);
                } else {
                    throw std::runtime_error("Unsupported value type for config key: " + key);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Error setting config key " + key + ": " + e.what());
            }
        }
        return std::make_shared<Brain>(config);
    }, py::arg("settings"), "Create a brain with custom configuration settings. The settings parameter is a dictionary mapping config keys to values.");

    m.def("getVersionInfo", []() {
        return std::make_tuple("0.1.0", "Python bindings", "pybind11 v2.11.0");
    }, "Get version information. Returns (version, bindings_type, pybind11_version).");

    m.def("validateConfiguration", [](const std::unordered_map<std::string, py::object>& settings) {
        auto config = std::make_shared<Config>();
        std::vector<std::string> errors;
        
        for (const auto& pair : settings) {
            const std::string& key = pair.first;
            py::object value = pair.second;
            
            try {
                if (value.is(py::int_())) {
                    config->set(key, value.cast<int64_t>(), ConfigSource::Default);
                } else if (value.is(py::float_())) {
                    config->set(key, value.cast<float>(), ConfigSource::Default);
                } else if (value.is(py::str())) {
                    config->set(key, value.cast<std::string>(), ConfigSource::Default);
                } else if (value.is(py::bool_())) {
                    config->set(key, value.cast<bool>(), ConfigSource::Default);
                } else {
                    errors.push_back("Unsupported value type for key: " + key);
                }
            } catch (const std::exception& e) {
                errors.push_back("Error setting key " + key + ": " + e.what());
            }
        }
        
        return std::make_tuple(errors.empty(), errors);
    }, py::arg("settings"), "Validate a configuration settings dictionary. Returns (is_valid, error_messages).");

    // Utility functions for working with IDs
    m.def("createNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create a NeuronId with the given value.");

    m.def("createSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create a SynapseId with the given value.");

    m.def("createRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create a RegionId with the given value.");

    m.def("createPopulationId", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Create a PopulationId with the given value.");

    // Constants for development stages
    m.attr("INITIAL_STAGE") = static_cast<int>(DevelopmentalStage::Initial);
    m.attr("CRITICAL_PERIOD_STAGE") = static_cast<int>(DevelopmentalStage::CriticalPeriod);
    m.attr("MATURATION_STAGE") = static_cast<int>(DevelopmentalStage::Maturation);
    m.attr("ADULT_STAGE") = static_cast<int>(DevelopmentalStage::Adult);
    m.attr("AGING_STAGE") = static_cast<int>(DevelopmentalStage::Aging);

    // Constants for neuron types
    m.attr("EXCITATORY_NEURON") = static_cast<int>(NeuronType::Excitatory);
    m.attr("INHIBITORY_NEURON") = static_cast<int>(NeuronType::Inhibitory);
    m.attr("SENSORY_NEURON") = static_cast<int>(NeuronType::Sensory);
    m.attr("MOTOR_NEURON") = static_cast<int>(NeuronType::Motor);
    m.attr("INTERNAL_NEURON") = static_cast<int>(NeuronType::Internal);
    m.attr("MODULATORY_NEURON") = static_cast<int>(NeuronType::Modulatory);

    // Constants for action types
    m.attr("MOVE_FORWARD_ACTION") = static_cast<int>(ActionType::MoveForward);
    m.attr("MOVE_BACKWARD_ACTION") = static_cast<int>(ActionType::MoveBackward);
    m.attr("TURN_LEFT_ACTION") = static_cast<int>(ActionType::TurnLeft);
    m.attr("TURN_RIGHT_ACTION") = static_cast<int>(ActionType::TurnRight);
    m.attr("INTERACT_ACTION") = static_cast<int>(ActionType::Interact);
    m.attr("EAT_ACTION") = static_cast<int>(ActionType::Eat);

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
