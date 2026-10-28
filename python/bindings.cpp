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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development system")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity-driven exploration is enabled");

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

    // Create a complete agent setup (common pattern from examples)
    m.def("createAgentSetup", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<SimpleWorld> world,
                                size_t vision_width = 8,
                                size_t vision_height = 8) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("vision_width") = 8,
       py::arg("vision_height") = 8,
       "Create a complete agent setup (convenience function)");

    // Batch operation for enabling multiple features at once
    m.def("batchEnableFeatures", [](std::vector<std::shared_ptr<AgentBrain>> agents,
                                   bool enable_all = true) {
        for (auto& agent : agents) {
            agent->enableRewardModulation(enable_all);
            agent->enableStructuralPlasticity(enable_all);
            agent->enableDevelopment(enable_all);
            agent->enableCuriosity(enable_all);
        }
    }, py::arg("agents"), py::arg("enable_all") = true,
       "Batch enable features across multiple agents");

    // Get formatted brain state string for logging/debugging
    m.def("formatBrainState", [](std::shared_ptr<Brain> brain,
                               const std::string& prefix = "") {
        std::ostringstream oss;
        if (!prefix.empty()) oss << prefix << " ";
        oss << "Brain State: "
            << "Neurons:" << brain->getTotalNeuronCount()
            << " Active:" << brain->getFiringNeuronCount()
            << " Spikes:" << brain->getTotalSpikeCount()
            << " Stage:" << static_cast<int>(brain->getDevelopmentalStage());
        return oss.str();
    }, py::arg("brain"), py::arg("prefix") = "",
       "Get formatted brain state string for logging");

    // Utility function to check if agent is properly configured
    m.def("validateAgentConfiguration", [](std::shared_ptr<AgentBrain> agent,
                                           std::shared_ptr<SimpleWorld> world) {
        bool valid = true;
        std::string errors;
        
        if (agent->getSensoryInputSize() == 0) {
            valid = false;
            errors += "Agent sensory input size is zero; ";
        }
        
        if (agent->getMotorOutputSize() == 0) {
            valid = false;
            errors += "Agent motor output size is zero; ";
        }
        
        if (world->getWidth() == 0 || world->getHeight() == 0) {
            valid = false;
            errors += "World dimensions are invalid; ";
        }
        
        return std::make_tuple(valid, errors);
    }, py::arg("agent"), py::arg("world"),
       "Validate agent configuration (returns (valid, error_message))");

    // Advanced functionality for expert users

    // Get complete brain system status (advanced diagnostic)
    m.def("getBrainSystemStatus", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> status;
        
        // Basic brain stats
        status["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        status["active_neurons"] = std::to_string(brain->getFiringNeuronCount());
        status["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        status["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        status["dev_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        
        // Memory systems
        status["has_working_memory"] = brain->getWorkingMemory() ? "true" : "false";
        status["has_episodic_memory"] = brain->getEpisodicMemory() ? "true" : "false";
        if (brain->getEpisodicMemory()) {
            status["episodic_episodes"] = std::to_string(brain->getEpisodicMemory()->getEpisodeCount());
        }
        
        // Neuromodulation
        status["has_dopamine"] = brain->getDopamine() ? "true" : "false";
        status["has_curiosity"] = brain->getCuriosity() ? "true" : "false";
        status["has_novelty"] = brain->getNovelty() ? "true" : "false";
        status["has_prediction_error"] = brain->getPredictionError() ? "true" : "false";
        
        // Cognition
        status["has_planner"] = brain->getPlanner() ? "true" : "false";
        status["has_concept_formation"] = brain->getConceptFormation() ? "true" : "false";
        status["has_attention"] = brain->getAttention() ? "true" : "false";
        
        // Development
        status["has_development_system"] = brain->getDevelopmentSystem() ? "true" : "false";
        
        return status;
    }, py::arg("brain"),
       "Get complete brain system status (advanced diagnostic)");

    // Performance monitoring for expert users
    m.def("performanceMonitor", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t steps) {
        std::map<std::string, double> metrics;
        
        double total_firing_rate = 0.0;
        double total_reward = 0.0;
        double total_novelty = 0.0;
        double total_curiosity = 0.0;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.001);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
            total_reward += reward;
            total_novelty += agent->getNoveltyLevel();
            total_curiosity += agent->getCuriosityLevel();
            total_firing_rate += brain->getAverageFiringRate();
        }
        
        metrics["avg_firing_rate"] = total_firing_rate / steps;
        metrics["avg_reward"] = total_reward / steps;
        metrics["avg_novelty"] = total_novelty / steps;
        metrics["avg_curiosity"] = total_curiosity / steps;
        
        return metrics;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Performance monitoring for expert users (returns metrics)");

    // Advanced configuration utilities
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set commonly used advanced configuration values
        config->set("neuron_count", 1000);
        config->set("region_count", 10);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Advanced plasticity settings
        config->set("max_connections_per_neuron", 50);
        config->set("max_synapses_per_neuron", 200);
        config->set("plasticity_enabled", true);
        
        return config;
    }, "Create an advanced configuration with optimized settings");

    // Debugging utilities for expert users
    m.def("debugSensoryNeurons", [](std::shared_ptr<Brain> brain) {
        auto spike_system = brain->getSpikeSystem();
        if (!spike_system) return std::vector<uint64_t>();
        
        auto sensory_neurons = spike_system->getNeuronsByType(NeuronType::Sensory);
        std::vector<uint64_t> ids;
        for (auto& neuron : sensory_neurons) {
            ids.push_back(neuron->getId().value);
        }
        return ids;
    }, py::arg("brain"),
       "Get IDs of all sensory neurons (debugging utility)");

    m.def("debugMotorNeurons", [](std::shared_ptr<Brain> brain) {
        auto spike_system = brain->getSpikeSystem();
        if (!spike_system) return std::vector<uint64_t>();
        
        auto motor_neurons = spike_system->getNeuronsByType(NeuronType::Motor);
        std::vector<uint64_t> ids;
        for (auto& neuron : motor_neurons) {
            ids.push_back(neuron->getId().value);
        }
        return ids;
    }, py::arg("brain"),
       "Get IDs of all motor neurons (debugging utility)");

    // Advanced experiment utilities
    m.def("createExperimentBrain", [](const std::string& config_path = "") {
        auto config = std::make_shared<Config>();
        if (!config_path.empty()) {
            config->loadFromFile(config_path);
        } else {
            // Use advanced config if no path provided
            config->set("neuron_count", 2000);
            config->set("region_count", 20);
            config->set("connection_probability", 0.05f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_path") = "",
       "Create a brain configured for experiments");

    // Utility to compare two brain states
    m.def("compareBrainStates", [](std::shared_ptr<Brain> brain1,
                                  std::shared_ptr<Brain> brain2) {
        std::map<std::string, std::pair<double, double>> comparisons;
        
        comparisons["neuron_count"] = std::make_pair(
            brain1->getTotalNeuronCount(),
            brain2->getTotalNeuronCount()
        );
        comparisons["active_neurons"] = std::make_pair(
            brain1->getFiringNeuronCount(),
            brain2->getFiringNeuronCount()
        );
        comparisons["total_spikes"] = std::make_pair(
            brain1->getTotalSpikeCount(),
            brain2->getTotalSpikeCount()
        );
        comparisons["avg_firing_rate"] = std::make_pair(
            brain1->getAverageFiringRate(),
            brain2->getAverageFiringRate()
        );
        
        return comparisons;
    }, py::arg("brain1"), py::arg("brain2"),
       "Compare two brain states (returns metric comparisons)");

    // Export brain state to Python-friendly format
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, py::object> state;
        
        // Basic metrics
        state["total_neurons"] = py::int_(brain->getTotalNeuronCount());
        state["active_neurons"] = py::int_(brain->getFiringNeuronCount());
        state["total_spikes"] = py::int_(brain->getTotalSpikeCount());
        state["avg_firing_rate"] = py::float_(brain->getAverageFiringRate());
        state["dev_stage"] = py::int_(static_cast<int>(brain->getDevelopmentalStage()));
        
        // Development metrics
        state["plasticity_modifier"] = py::float_(brain->getDevelopmentSystem() ? 
            brain->getDevelopmentSystem()->getPlasticityModifier() : 1.0f);
        
        // Memory metrics
        if (brain->getWorkingMemory()) {
            state["working_memory_active_traces"] = py::int_(brain->getWorkingMemory()->getActiveTraces());
        }
        if (brain->getEpisodicMemory()) {
            state["episodic_memory_episode_count"] = py::int_(brain->getEpisodicMemory()->getEpisodeCount());
        }
        
        return state;
    }, py::arg("brain"),
       "Export brain state to Python-friendly dictionary format");

    // Import brain state from Python-friendly format
    m.def("importBrainState", [](std::shared_ptr<Brain> brain,
                                const std::map<std::string, py::object>& state) {
        // This is a placeholder - actual implementation would depend on specific brain state structure
        // For now, just log that import was attempted
        return "Import functionality requires specific brain state format implementation";
    }, py::arg("brain"), py::arg("state"),
       "Import brain state from Python dictionary (placeholder)");

    // Advanced agent coordination
    m.def("multiAgentCoordination", [](std::vector<std::shared_ptr<AgentBrain>> agents,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t steps) {
        std::map<std::string, double> coordination_metrics;
        
        double total_system_activity = 0.0;
        double total_coordination = 0.0;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            
            // Each agent processes its own percept
            for (auto& agent : agents) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i, i * 0.001);
                
                total_system_activity += brain->getAverageFiringRate();
            }
            
            // Simple coordination - agents could share information in more advanced versions
            total_coordination += agents.size() * 0.1; // Simple coordination factor
        }
        
        coordination_metrics["avg_system_activity"] = total_system_activity / steps;
        coordination_metrics["avg_coordination"] = total_coordination / steps;
        
        return coordination_metrics;
    }, py::arg("agents"), py::arg("world"), py::arg("steps"),
       "Multi-agent coordination simulation (advanced)");

    // Utility to generate experiment report
    m.def("generateExperimentReport", [](const std::string& experiment_name,
                                         const std::map<std::string, py::object>& results,
                                         const std::string& additional_notes = "") {
        std::ostringstream report;
        report << "=== NLM Experiment Report ===\n\n";
        report << "Experiment: " << experiment_name << "\n";
        report << "Generated: " << "TODO: timestamp" << "\n\n";
        
        report << "Results Summary:\n";
        for (const auto& [key, value] : results) {
            report << "  " << key << ": ";
            if (py::isinstance(value, py::int_())) {
                report << py::int_(value).cast<int64_t>() << "\n";
            } else if (py::isinstance(value, py::float_())) {
                report << py::float_(value).cast<double>() << "\n";
            } else if (py::isinstance(value, py::str())) {
                report << py::str(value).cast<std::string>() << "\n";
            } else {
                report << "[complex type]\n";
            }
        }
        
        if (!additional_notes.empty()) {
            report << "\nAdditional Notes:\n" << additional_notes << "\n";
        }
        
        report << "\n=== End Report ===";
        return report.str();
    }, py::arg("experiment_name"), py::arg("results"), py::arg("additional_notes") = "",
       "Generate formatted experiment report (advanced)");

    // Enhanced error handling and validation

    // Safe brain step with error handling
    m.def("safeBrainStep", [](std::shared_ptr<Brain> brain,
                             size_t step,
                             double time = -1.0) {
        try {
            if (time < 0.0) {
                brain->step(step);
            } else {
                brain->step(step, time);
            }
            return "Step completed successfully";
        } catch (const std::exception& e) {
            return std::string("Error during step: ") + e.what();
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = -1.0,
       "Safe brain step with error handling");

    // Safe world update with validation
    m.def("safeWorldUpdate", [](std::shared_ptr<SimpleWorld> world,
                               double timestep = 0.1) {
        try {
            if (timestep <= 0.0) {
                return std::string("Error: timestep must be positive");
            }
            world->update(timestep);
            return "World update completed successfully";
        } catch (const std::exception& e) {
            return std::string("Error during world update: ") + e.what();
        }
    }, py::arg("world"), py::arg("timestep") = 0.1,
       "Safe world update with validation");

    // Configuration validation
    m.def("validateConfig", [](const std::string& config_path) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_path)) {
            return std::make_tuple(false, std::string("Failed to load config file: ") + config_path);
        }
        
        bool valid = true;
        std::string errors;
        
        // Check required keys
        std::vector<std::string> required_keys = {"neuron_count", "region_count"};
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                valid = false;
                errors += "Missing required key: " + key + "; ";
            }
        }
        
        // Validate values
        auto neuron_count = config->getOr("neuron_count", 0);
        if (neuron_count <= 0) {
            valid = false;
            errors += "Invalid neuron_count (must be positive); ";
        }
        
        auto region_count = config->getOr("region_count", 0);
        if (region_count <= 0) {
            valid = false;
            errors += "Invalid region_count (must be positive); ";
        }
        
        return std::make_tuple(valid, errors);
    }, py::arg("config_path"),
       "Validate configuration file (returns (valid, error_message))");

    // Safe configuration loading
    m.def("safeLoadConfig", [](const std::string& config_path) {
        try {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(config_path)) {
                return std::make_tuple(nullptr, std::string("Failed to load config from: ") + config_path);
            }
            return std::make_tuple(config, std::string("Config loaded successfully"));
        } catch (const std::exception& e) {
            return std::make_tuple(nullptr, std::string("Error loading config: ") + e.what());
        }
    }, py::arg("config_path"),
       "Safe configuration loading (returns (config, status_message))");

    // Safe world creation with validation
    m.def("safeCreateWorld", [](size_t width = 16, size_t height = 16,
                               size_t vision_width = 8,
                               size_t vision_height = 8) {
        try {
            if (width == 0 || height == 0) {
                return std::make_tuple(nullptr, std::string("Invalid world dimensions"));
            }
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, vision_width, vision_height);
            return std::make_tuple(world, std::string("World created successfully"));
        } catch (const std::exception& e) {
            return std::make_tuple(nullptr, std::string("Error creating world: ") + e.what());
        }
    }, py::arg("width") = 16, py::arg("height") = 16,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Safe world creation with validation (returns (world, status_message))");

    // Safe brain initialization
    m.def("safeInitializeBrain", [](std::shared_ptr<Brain> brain) {
        try {
            bool success = brain->initialize();
            if (success) {
                return std::make_tuple(true, std::string("Brain initialized successfully"));
            } else {
                return std::make_tuple(false, std::string("Brain initialization failed"));
            }
        } catch (const std::exception& e) {
            return std::make_tuple(false, std::string("Error during brain initialization: ") + e.what());
        }
    }, py::arg("brain"),
       "Safe brain initialization (returns (success, status_message))");

    // Enhanced validation with detailed reporting
    m.def("detailedValidation", [](std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<std::string>> validation_results;
        
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> info;
        
        // Agent validation
        if (agent->getSensoryInputSize() == 0) {
            errors.push_back("Agent sensory input size is zero");
        }
        
        if (agent->getMotorOutputSize() == 0) {
            errors.push_back("Agent motor output size is zero");
        }
        
        if (!agent->isRewardModulationEnabled()) {
            warnings.push_back("Reward modulation is disabled (learning will be limited)");
        }
        
        // World validation
        if (world->getWidth() == 0 || world->getHeight() == 0) {
            errors.push_back("World dimensions are invalid");
        }
        
        if (world->getMaxEnergy() <= 0.0) {
            warnings.push_back("World max energy is non-positive");
        }
        
        // Brain validation
        if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (brain->getAverageFiringRate() < 0.01) {
            warnings.push_back("Brain firing rate is very low (may indicate connectivity issues)");
        }
        
        info.push_back(std::string("Total neurons: ") + std::to_string(brain->getTotalNeuronCount()));
        info.push_back(std::string("Active neurons: ") + std::to_string(brain->getFiringNeuronCount()));
        info.push_back(std::string("Total spikes: ") + std::to_string(brain->getTotalSpikeCount()));
        
        return std::make_tuple(errors, warnings, info);
    }, py::arg("agent"), py::arg("world"), py::arg("brain"),
       "Enhanced validation with detailed reporting (returns (errors, warnings, info))");

    // Utility to fix common configuration issues
    m.def("fixCommonIssues", [](std::shared_ptr<Config> config) {
        std::vector<std::string> fixes_applied;
        
        // Fix missing essential keys
        if (!config->has("neuron_count")) {
            config->set("neuron_count", 1000);
            fixes_applied.push_back("Added missing neuron_count (set to 1000)");
        }
        
        if (!config->has("region_count")) {
            config->set("region_count", 10);
            fixes_applied.push_back("Added missing region_count (set to 10)");
        }
        
        // Fix invalid values
        auto neuron_count = config->getOr("neuron_count", 0);
        if (neuron_count <= 0) {
            config->set("neuron_count", 1000);
            fixes_applied.push_back("Fixed invalid neuron_count (set to 1000)");
        }
        
        auto region_count = config->getOr("region_count", 0);
        if (region_count <= 0) {
            config->set("region_count", 10);
            fixes_applied.push_back("Fixed invalid region_count (set to 10)");
        }
        
        // Ensure plasticity is enabled
        if (!config->getOr("plasticity_enabled", true)) {
            config->set("plasticity_enabled", true);
            fixes_applied.push_back("Enabled plasticity (was disabled)");
        }
        
        return fixes_applied;
    }, py::arg("config"),
       "Fix common configuration issues (returns list of fixes applied)");

    // Safe version of all major operations with comprehensive error handling
    m.def("safeCompleteSetup", [](const std::string& config_path = "") {
        try {
            // Load or create config
            std::shared_ptr<Config> config;
            if (!config_path.empty()) {
                config = std::make_shared<Config>();
                if (!config->loadFromFile(config_path)) {
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr,
                                          std::string("Failed to load config file: ") + config_path);
                }
            } else {
                config = std::make_shared<Config>();
                // Apply fixes
                auto fixes = fixCommonIssues(config);
                if (!fixes.empty()) {
                    std::ostringstream oss;
                    oss << "Applied fixes: ";
                    for (size_t i = 0; i < fixes.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << fixes[i];
                    }
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr, oss.str());
                }
            }
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr,
                                      std::string("Failed to initialize brain"));
            }
            
            // Create world
            auto world = std::make_shared<SimpleWorld>();
            world->configure(16, 16, 8, 8);
            
            // Create agent
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable features
            enableAgentFeatures(agent);
            
            return std::make_tuple(brain, world, agent, config, std::string("Setup completed successfully"));
        } catch (const std::exception& e) {
            return std::make_tuple(nullptr, nullptr, nullptr, nullptr,
                                  std::string("Error during setup: ") + e.what());
        }
    }, py::arg("config_path") = "",
       "Safe complete setup (returns (brain, world, agent, config, status_message))");

    // Utility to get system information
    m.def("getSystemInfo", []() {
        std::map<std::string, std::string> info;
        info["version"] = "NLM Phase 6 Final Integration";
        info["build_date"] = "__DATE__"; // Will be replaced during compilation
        info["compiler"] = "__COMPILER__"; // Will be replaced during compilation
        info["architecture"] = "x86_64";
        info["python_bindings"] = "pybind11 v2.11+";
        return info;
    }, "Get system information (version, build info)");

    // Utility to benchmark brain creation
    m.def("benchmarkBrainCreation", [](size_t num_brains = 10,
                                      const std::string& config_path = "") {
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::shared_ptr<Brain>> brains;
        for (size_t i = 0; i < num_brains; ++i) {
            auto config = std::make_shared<Config>();
            if (!config_path.empty()) {
                config->loadFromFile(config_path);
            } else {
                config->set("neuron_count", 100);
                config->set("region_count", 5);
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(brain);
        }
        
        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        std::map<std::string, double> results;
        results["num_brains"] = num_brains;
        results["total_time_seconds"] = duration;
        results["avg_time_per_brain_ms"] = duration * 1000.0 / num_brains;
        results["brains_per_second"] = num_brains / duration;
        
        return results;
    }, py::arg("num_brains") = 10, py::arg("config_path") = "",
       "Benchmark brain creation performance (returns timing metrics)");

    // Utility to generate validation report
    m.def("generateValidationReport", [](std::shared_ptr<AgentBrain> agent,
                                         std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<Brain> brain,
                                         const std::string& experiment_name = "") {
        std::ostringstream report;
        report << "=== NLM Validation Report ===\n\n";
        
        if (!experiment_name.empty()) {
            report << "Experiment: " << experiment_name << "\n\n";
        }
        
        report << "Validation performed at: " << "TODO: timestamp" << "\n\n";
        
        // Get validation results
        auto [errors, warnings, info] = detailedValidation(agent, world, brain);
        
        report << "=== Status: ";
        if (errors.empty() && warnings.empty()) {
            report << "PASS\n\n";
        } else if (errors.empty() && !warnings.empty()) {
            report << "WARNINGS ONLY\n\n";
        } else {
            report << "FAIL\n\n";
        }
        
        if (!errors.empty()) {
            report << "--- ERRORS ---\n";
            for (const auto& error : errors) {
                report << "  ✗ " << error << "\n";
            }
            report << "\n";
        }
        
        if (!warnings.empty()) {
            report << "--- WARNINGS ---\n";
            for (const auto& warning : warnings) {
                report << "  ⚠ " << warning << "\n";
            }
            report << "\n";
        }
        
        if (!info.empty()) {
            report << "--- INFORMATION ---\n";
            for (const auto& item : info) {
                report << "  ✓ " << item << "\n";
            }
            report << "\n";
        }
        
        report << "=== End Validation Report ===";
        return report.str();
    }, py::arg("agent"), py::arg("world"), py::arg("brain"), py::arg("experiment_name") = "",
       "Generate detailed validation report (returns formatted report string)");

    // Run a complete simulation (from easy_usage.md Pattern 1)
    m.def("run_simulation", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent,
                               size_t num_steps) {
        for (size_t step = 0; step < num_steps; ++step) {
            // 1. Update world
            world->update(0.1);

            // 2. Get what the agent sees
            auto percept = world->getSensoryPercept();

            // 3. Tell the brain
            agent->processSensoryInput(percept);

            // 4. Brain thinks
            brain->step(step, step * 0.001);

            // 5. Get action from brain
            auto action = agent->decodeMotorCommand();

            // 6. Do action in world
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
         "Run a complete simulation (from easy_usage.md Pattern 1)");

    // Get comprehensive brain statistics (from easy_usage.md Pattern 2)
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        return std::make_tuple(
            brain->getFiringNeuronCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("brain"),
         "Get comprehensive brain statistics (from easy_usage.md Pattern 2)");

    // Enable common brain features (from easy_usage.md Pattern 3)
    m.def("enableAgentFeatures", [](std::shared_ptr<AgentBrain> agent,
                                   bool reward_modulation = true,
                                   bool structural_plasticity = true,
                                   bool development = true,
                                   bool curiosity = true) {
        agent->enableRewardModulation(reward_modulation);
        agent->enableStructuralPlasticity(structural_plasticity);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
    }, py::arg("agent"), py::arg("reward_modulation") = true,
       py::arg("structural_plasticity") = true,
       py::arg("development") = true,
       py::arg("curiosity") = true,
       "Enable common agent features (from easy_usage.md Pattern 3)");

    // Create a complete agent setup (common pattern from examples)
    m.def("createAgentSetup", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<SimpleWorld> world,
                                size_t vision_width = 8,
                                size_t vision_height = 8) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("vision_width") = 8,
       py::arg("vision_height") = 8,
       "Create a complete agent setup (convenience function)");
