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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // ==========================================================
    // NEW UTILITY FUNCTIONS FOR ADVANCED USERS
    // ==========================================================

    // Brain management utilities
    m.def("createBrainFromTemplate", [](const std::string& template_type, std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (template_type == "standard") {
            // Apply standard template configuration
            brain->getConfig()->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
            brain->getConfig()->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
            brain->getConfig()->set("connection_probability", 0.1f, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        } else if (template_type == "complex") {
            // Apply complex template with more regions and plasticity
            brain->getConfig()->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
            brain->getConfig()->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
            brain->getConfig()->set("connection_probability", 0.05f, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            brain->getConfig()->set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
        } else if (template_type == "benchmark") {
            // Apply benchmark template optimized for performance testing
            brain->getConfig()->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
            brain->getConfig()->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
            brain->getConfig()->set("connection_probability", 0.2f, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.hebbian.enable", false, ConfigSource::Runtime);
            brain->getConfig()->set("plasticity.stdp.enable", false, ConfigSource::Runtime);
        }
        
        brain->initialize();
        return brain;
    }, py::arg("template_type"), py::arg("config") = nullptr,
       "Create a brain from a predefined template. Templates: 'standard', 'complex', 'benchmark'");

    m.def("mergeBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, 
                          float overlap_threshold) -> std::shared_ptr<Brain> {
        // Create merged brain (simplified implementation)
        auto config = std::make_shared<Config>();
        
        // Use parameters from brain1 with adjusted count for overlap
        int64_t total_neurons = brain1->getTotalNeuronCount() + 
                               static_cast<int64_t>(brain2->getTotalNeuronCount() * overlap_threshold);
        config->set("neuron_count", total_neurons, ConfigSource::Runtime);
        config->set("region_count", brain1->getConfig()->getOr<int64_t>("region_count", 1), ConfigSource::Runtime);
        config->set("connection_probability", brain1->getConfig()->getOr<float>("connection_probability", 0.1f), ConfigSource::Runtime);
        
        auto merged_brain = std::make_shared<Brain>(config);
        merged_brain->initialize();
        
        return merged_brain;
    }, py::arg("brain1"), py::arg("brain2"), py::arg("overlap_threshold") = 0.5f,
       "Merge two brains with specified overlap threshold (0.0-1.0)");

    m.def("analyzeLearningTrajectory", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Analyze learning trajectory over specified steps
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::cout << "Analyzing learning trajectory for " << steps << " steps..." << std::endl;
        
        // Record initial state
        float initial_firing_rate = brain->getAverageFiringRate();
        size_t initial_spikes = brain->getTotalSpikeCount();
        
        std::vector<float> firing_rates;
        std::vector<size_t> spike_counts;
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(brain->getTotalSpikeCount() - initial_spikes);
        }
        
        // Calculate learning metrics
        float final_firing_rate = firing_rates.back();
        float firing_rate_change = final_firing_rate - initial_firing_rate;
        
        float total_spike_increase = spike_counts.back();
        
        // Determine learning status
        std::string learning_status;
        if (firing_rate_change > 0.01f || total_spike_increase > 100) {
            learning_status = "STABLE_LEARNING";
        } else if (firing_rate_change < -0.01f) {
            learning_status = "DECREASING_ACTIVITY";
        } else if (total_spike_increase < 10) {
            learning_status = "LIMITED_LEARNING";
        } else {
            learning_status = "STABLE_STATE";
        }
        
        std::cout << "Learning Trajectory Analysis:" << std::endl;
        std::cout << "  Initial firing rate: " << initial_firing_rate << std::endl;
        std::cout << "  Final firing rate: " << final_firing_rate << std::endl;
        std::cout << "  Firing rate change: " << firing_rate_change << std::endl;
        std::cout << "  Total spike increase: " << total_spike_increase << std::endl;
        std::cout << "  Learning status: " << learning_status << std::endl;
        
        // Return analysis results as Python dict (simplified for now)
        // In a real implementation, you'd return a proper Python object
        py::dict results;
        results["initial_firing_rate"] = initial_firing_rate;
        results["final_firing_rate"] = final_firing_rate;
        results["firing_rate_change"] = firing_rate_change;
        results["total_spike_increase"] = total_spike_increase;
        results["learning_status"] = learning_status;
        results["steps_analyzed"] = steps;
        
        return results;
    }, py::arg("brain"), py::arg("steps"),
       "Analyze learning trajectory over specified steps");

    m.def("benchmarkNetworkPerformance", [](std::shared_ptr<Brain> brain, size_t steps, 
                                          const std::string& benchmark_type) {
        // Run performance benchmark on the brain
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::cout << "Running " << benchmark_type << " benchmark for " << steps << " steps..." << std::endl;
        
        // Reset brain for clean benchmark
        brain->reset();
        brain->initialize();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            
            // Log progress every 10% of steps
            if (i % (steps / 10) == 0 && i > 0) {
                std::cout << "  Progress: " << (i * 100.0 / steps) << "% complete" << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Calculate performance metrics
        double steps_per_second = steps / (duration.count() / 1000.0);
        float average_firing_rate = brain->getAverageFiringRate();
        size_t total_spikes = brain->getTotalSpikeCount();
        
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Steps per second: " << steps_per_second << std::endl;
        std::cout << "  Average firing rate: " << average_firing_rate << std::endl;
        std::cout << "  Total spikes generated: " << total_spikes << std::endl;
        
        // Return benchmark results as Python dict
        py::dict results;
        results["total_time_ms"] = duration.count();
        results["steps_per_second"] = steps_per_second;
        results["average_firing_rate"] = average_firing_rate;
        results["total_spikes"] = total_spikes;
        results["benchmark_type"] = benchmark_type;
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("benchmark_type") = "default",
       "Run performance benchmark on the brain");

    m.def("generateTestPattern", [](std::shared_ptr<Brain> brain, const std::string& pattern_type, 
                                   size_t duration) {
        // Generate specific input patterns for testing
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::cout << "Generating " << pattern_type << " test pattern for " << duration << " steps..." << std::endl;
        
        brain->reset();
        brain->initialize();
        
        std::vector<NeuronId> target_neurons;
        
        if (pattern_type == "alternating") {
            // Get all sensory neurons and alternate stimulation
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory) {
                        target_neurons.push_back(neuron->getId());
                    }
                }
            }
            for (size_t i = 0; i < duration; ++i) {
                if (i % 2 == 0 && !target_neurons.empty()) {
                    brain->injectCurrent(target_neurons[0], 50.0f);
                }
                brain->step(i);
            }
        } else if (pattern_type == "burst") {
            // Generate burst patterns
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory) {
                        target_neurons.push_back(neuron->getId());
                    }
                }
            }
            for (size_t i = 0; i < duration; ++i) {
                if (i % 10 == 0 && !target_neurons.empty()) {
                    for (NeuronId id : target_neurons) {
                        brain->injectCurrent(id, 100.0f);
                    }
                }
                brain->step(i);
            }
        } else if (pattern_type == "random") {
            // Generate random patterns
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory) {
                        target_neurons.push_back(neuron->getId());
                    }
                }
            }
            for (size_t i = 0; i < duration; ++i) {
                if (!target_neurons.empty() && (rand() % 100 < 10)) { // 10% probability
                    NeuronId id = target_neurons[rand() % target_neurons.size()];
                    brain->injectCurrent(id, 75.0f);
                }
                brain->step(i);
            }
        }
        
        std::cout << "Test pattern " << pattern_type << " completed." << std::endl;
        std::cout << "Final statistics:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("pattern_type"), py::arg("duration") = 1000,
       "Generate specific input patterns for testing: 'alternating', 'burst', 'random'");

    m.def("runAdaptiveSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                    std::shared_ptr<AgentBrain> agent, size_t max_steps) {
        // Run adaptive simulation with environment interaction
        if (!brain || !world || !agent) {
            throw std::runtime_error("Required parameters (brain, world, agent) cannot be null");
        }
        
        std::cout << "Starting adaptive simulation for up to " << max_steps << " steps..." << std::endl;
        
        // Initialize world and agent
        world->reset();
        agent->initialize(world);
        
        size_t actual_steps = 0;
        size_t early_stop_step = max_steps * 0.8; // Early stop at 80% if stable
        
        for (actual_steps = 0; actual_steps < max_steps; ++actual_steps) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(actual_steps);
            
            // Get motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Check for stability (early stop condition)
            if (actual_steps >= early_stop_step) {
                float firing_rate = brain->getAverageFiringRate();
                if (firing_rate < 1.0f && firing_rate > 0.0f) {
                    std::cout << "  Adaptive simulation stabilized at step " << actual_steps << ". Stopping early." << std::endl;
                    break;
                }
            }
            
            // Log progress every 10%
            if (actual_steps % (max_steps / 10) == 0) {
                std::cout << "  Progress: " << (actual_steps * 100.0 / max_steps) << "% complete" << std::endl;
            }
        }
        
        std::cout << "Adaptive simulation completed after " << actual_steps << " steps." << std::endl;
        std::cout << "Final statistics:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        
        return actual_steps;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("max_steps") = 10000,
       "Run adaptive simulation with environment interaction");

    m.def("runLearningPhase", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               std::shared_ptr<AgentBrain> agent, size_t episodes) {
        // Run focused learning phase
        if (!brain || !world || !agent) {
            throw std::runtime_error("Required parameters (brain, world, agent) cannot be null");
        }
        
        std::cout << "Starting learning phase for " << episodes << " episodes..." << std::endl;
        
        // Enable all plasticity mechanisms for learning
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        size_t successful_episodes = 0;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            std::cout << "Episode " << episode + 1 << "/" << episodes << "..." << std::endl;
            
            // Reset world and brain for new episode
            world->reset();
            world->setAgentStart(10.0f, 10.0f);
            brain->reset();
            brain->initialize();
            agent->reset();
            agent->initialize(world);
            
            // Run episode
            for (size_t step = 0; step < 100; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Get motor command
                auto action = agent->decodeMotorCommand();
                
                // Apply motor command
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
                agent->applyRewardModulation(reward, 0.0);
                
                // Update development
                agent->updateDevelopment(0.1);
            }
            
            // Check if episode was successful (basic heuristic)
            float final_firing_rate = brain->getAverageFiringRate();
            if (final_firing_rate > 0.5f && final_firing_rate < 5.0f) {
                successful_episodes++;
                std::cout << "  ✓ Successful episode" << std::endl;
            } else {
                std::cout << "  ✗ Unsuccessful episode (final firing rate: " << final_firing_rate << ")" << std::endl;
            }
            
            // Log progress
            if ((episode + 1) % 10 == 0) {
                std::cout << "  Progress: " << ((episode + 1) * 100.0 / episodes) << "% complete" << std::endl;
            }
        }
        
        std::cout << "Learning phase completed." << std::endl;
        std::cout << "Success rate: " << (successful_episodes * 100.0 / episodes) << "% (" 
                  << successful_episodes << "/" << episodes << " episodes)" << std::endl;
        
        return successful_episodes;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("episodes") = 100,
       "Run focused learning phase with environment interaction");

    // ==========================================================
    // ENHANCED CLASSES FOR ADVANCED USERS
    // ==========================================================

    // BrainAnalyzer class for network analysis
    py::class_<BrainAnalyzer>(m, "BrainAnalyzer", R"pbdoc(Advanced brain network analysis class)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("analyzeConnectivity", &BrainAnalyzer::analyzeConnectivity)
        .def("detectCommunities", &BrainAnalyzer::detectCommunities)
        .def("calculateEfficiency", &BrainAnalyzer::calculateEfficiency)
        .def("getNetworkStats", &BrainAnalyzer::getNetworkStats);

    // SimulationManager class for complex simulation control
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(Advanced simulation management class)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("runEpisode", &SimulationManager::runEpisode)
        .def("runBatch", &SimulationManager::runBatch)
        .def("exportData", &SimulationManager::exportData)
        .def("setParameters", &SimulationManager::setParameters);

    // ConfigWizard class for easier configuration
    py::class_<ConfigWizard>(m, "ConfigWizard", R"pbdoc(Advanced configuration wizard for NLM)pbdoc")
        .def(py::init<>())
        .def("createConfig", [](const std::string& use_case) {
            auto config = std::make_shared<Config>();
            if (use_case == "learning") {
                config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
                config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
                config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
                config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
                config->set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
                config->set("neuromodulation.reward.enable", true, ConfigSource::Runtime);
            } else if (use_case == "prediction") {
                config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
                config->set("region_count", static_cast<int64_t>(3), ConfigSource::Runtime);
                config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
                config->set("prediction_system.enabled", true, ConfigSource::Runtime);
            }
            return config;
        }, py::arg("use_case") = "learning", "Create configuration based on use case")
        .def("suggestParameters", &ConfigWizard::suggestParameters)
        .def("validateConfig", &ConfigWizard::validateConfig);

    m.def("createCustomWorld", [](size_t width, size_t height, float object_density) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Add random objects based on density
        size_t num_objects = static_cast<size_t>(width * height * object_density);
        for (size_t i = 0; i < num_objects; ++i) {
            float x = static_cast<float>(rand() % width);
            float y = static_cast<float>(rand() % height);
            world->addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Resource, value=1.0f));
        }
        
        world->setAgentStart(width / 2.0f, height / 2.0f);
        world->reset();
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("object_density") = 0.1f,
       "Create a custom world with specified dimensions and object density");

    m.def("generateEnvironmentScenario", [](const std::string& scenario_type, 
                                           std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw std::runtime_error("World cannot be null");
        }
        
        if (scenario_type == "maze") {
            // Generate maze environment
            world->reset();
            // Add walls to create maze
            for (size_t x = 0; x < world->getWidth(); ++x) {
                world->addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType::Wall, radius=0.5f));
                world->addObject(pynlm.WorldObject(x, world->getHeight() - 1, 
                                                 pynlm.WorldObjectType::Wall, radius=0.5f));
            }
            for (size_t y = 0; y < world->getHeight(); ++y) {
                world->addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType::Wall, radius=0.5f));
                world->addObject(pynlm.WorldObject(world->getWidth() - 1, y, 
                                                 pynlm.WorldObjectType::Wall, radius=0.5f));
            }
        } else if (scenario_type == "forest") {
            // Generate forest environment
            world->reset();
            // Add trees
            for (size_t i = 0; i < 20; ++i) {
                float x = static_cast<float>(rand() % world->getWidth());
                float y = static_cast<float>(rand() % world->getHeight());
                world->addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Hazard, value=0.5f));
            }
        }
        
        world->setAgentStart(world->getWidth() / 2.0f, world->getHeight() / 2.0f);
        
        return world;
    }, py::arg("scenario_type"), py::arg("world") = nullptr,
       "Generate specific environment scenarios: 'maze', 'forest', etc.");

    m.def("visualizeNetworkState", [](std::shared_ptr<Brain> brain, const std::string& output_file) {
        // Simple visualization of network state (placeholder implementation)
        std::cout << "Visualizing network state to file: " << output_file << std::endl;
        
        // In a real implementation, this would use matplotlib, plotly, or similar
        // For now, we'll just create a simple text-based visualization
        std::ofstream file(output_file);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create output file: " + output_file);
        }
        
        file << "NLM Network State Visualization" << std::endl;
        file << "===============================" << std::endl;
        file << std::endl;
        file << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        file << std::endl;
        
        // Create simple neuron firing rate distribution
        file << "Neuron Firing Rate Distribution:" << std::endl;
        // This would be more sophisticated in a real implementation
        for (int i = 0; i < 10; ++i) {
            file << "  Region " << i << ": [placeholder firing rates]" << std::endl;
        }
        
        file << std::endl;
        file << "Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        file.close();
        std::cout << "Visualization saved to: " << output_file << std::endl;
        
        return true;
    }, py::arg("brain"), py::arg("output_file") = "network_state.txt",
       "Create simple visualization of network state (text format)");

    m.def("exportBrainData", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        // Export brain data to file
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        std::cout << "Exporting brain data to: " << filename << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create export file: " + filename);
        }
        
        file << "NLM Brain Data Export" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        
        // Export basic brain statistics
        file << "Brain Statistics:" << std::endl;
        file << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        file << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        file << std::endl;
        
        // Export configuration
        file << "Configuration Summary:" << std::endl;
        auto keys = brain->getConfig()->getKeys();
        for (const auto& key : keys) {
            file << "  " << key << std::endl;
        }
        
        file << std::endl;
        file << "Export completed successfully." << std::endl;
        
        file.close();
        return true;
    }, py::arg("brain"), py::arg("filename") = "brain_data.txt",
       "Export brain data to file for later analysis or sharing");

    m.def("importBrainData", [](const std::string& filename) {
        // Import brain data from file (placeholder implementation)
        std::cout << "Importing brain data from: " << filename << std::endl;
        
        // This would load brain state from file in a real implementation
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open import file: " + filename);
        }
        
        std::string line;
        std::getline(file, line); // Skip header
        
        // Simple parsing (would be more sophisticated in real implementation)
        int64_t neuron_count = 1000; // Default
        int64_t region_count = 1;
        
        while (std::getline(file, line)) {
            if (line.find("Total neurons:") != std::string::npos) {
                neuron_count = std::stoll(line.substr(line.find(":") + 1));
            } else if (line.find("Region count:") != std::string::npos) {
                region_count = std::stoll(line.substr(line.find(":") + 1));
            }
        }
        
        file.close();
        
        // Create brain with imported parameters
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuron_count, ConfigSource::Runtime);
        config->set("region_count", region_count, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Brain data imported successfully." << std::endl;
        std::cout << "Imported " << neuron_count << " neurons and " << region_count << " regions." << std::endl;
        
        return brain;
    }, py::arg("filename") = "brain_data.txt",
       "Import brain data from file for continuing simulations or analysis");

    m.def("benchmarkNetwork", [](size_t neuron_count, size_t steps, const std::string& benchmark_type) {
        // Run benchmark on network with specified parameters
        std::cout << "Running " << benchmark_type << " benchmark with " << neuron_count 
                  << " neurons for " << steps << " steps..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double steps_per_second = steps / (duration.count() / 1000.0);
        
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << "  Neurons: " << neuron_count << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Steps per second: " << steps_per_second << std::endl;
        
        return steps_per_second;
    }, py::arg("neuron_count") = 1000, py::arg("steps") = 1000, 
       py::arg("benchmark_type") = "default", "Run network benchmark with specified parameters");

    m.def("compareBrains", [](std::vector<std::shared_ptr<Brain>> brains, size_t steps) {
        // Compare multiple brains
        if (brains.empty()) {
            throw std::runtime_error("At least one brain must be provided for comparison");
        }
        
        std::cout << "Comparing " << brains.size() << " brains over " << steps << " steps..." << std::endl;
        
        std::vector<double> performance_scores;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            auto brain = brains[i];
            brain->reset();
            brain->initialize();
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            double steps_per_second = steps / (duration.count() / 1000.0);
            float average_firing_rate = brain->getAverageFiringRate();
            size_t total_spikes = brain->getTotalSpikeCount();
            
            double score = steps_per_second * 0.5 + average_firing_rate * 0.3 + (total_spikes / 1000.0) * 0.2;
            
            performance_scores.push_back(score);
            
            std::cout << "  Brain " << i + 1 << ": " << score << " (" << steps_per_second << " steps/s, " 
                      << average_firing_rate << " firing rate, " << total_spikes << " spikes)" << std::endl;
        }
        
        // Find best brain
        size_t best_idx = std::distance(performance_scores.begin(), 
                                        std::max_element(performance_scores.begin(), performance_scores.end()));
        
        std::cout << "Best performing brain: " << best_idx + 1 << " with score " << performance_scores[best_idx] << std::endl;
        
        return best_idx;
    }, py::arg("brains"), py::arg("steps") = 1000, "Compare multiple brains and return index of best performer");

    m.def("getNLMVersion", []() {
        return "NLM Version 2.0 - Enhanced Python API";
    }, "Get the current version of NLM");

    m.def("getAvailableTemplates", []() {
        py::list templates;
        templates.append("standard");
        templates.append("complex");
        templates.append("benchmark");
        return templates;
    }, "Get list of available brain templates");

    m.def("getEnvironmentScenarios", []() {
        py::list scenarios;
        scenarios.append("maze");
        scenarios.append("forest");
        scenarios.append("open_world");
        scenarios.append("urban");
        return scenarios;
    }, "Get list of available environment scenarios");


} // namespace nlm
