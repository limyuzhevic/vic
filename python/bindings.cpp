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

    // Advanced utility functions for power users

    // Configuration utilities
    m.def("createAdvancedConfig", [](const std::string& json) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromJSON(json)) {
            throw std::runtime_error("Failed to parse JSON configuration");
        }
        return config;
    }, py::arg("json"), "Create configuration from JSON string with advanced parsing");

    m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& override) {
        auto merged = std::make_shared<Config>(*base);
        // Merge override into base
        for (const auto& entry : override->pImpl->entries) {
            if (!entry.key.empty()) {
                merged->set(entry.key, entry.value, entry.source);
            }
        }
        return merged;
    }, py::arg("base"), py::arg("override"), 
        "Merge two configurations, with override taking precedence");

    // Simulation utilities
    m.def("runSimulationBatch", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                   std::shared_ptr<AgentBrain> agent,
                                   int steps, bool verbose = false) {
        std::vector<std::map<std::string, float>> results;
        results.reserve(steps);
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action and apply to world
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Collect statistics
            if (verbose || step % 100 == 0) {
                std::map<std::string, float> stats;
                stats["step"] = static_cast<float>(step);
                stats["spikes"] = static_cast<float>(brain->getTotalSpikeCount());
                stats["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
                stats["reward"] = agent->getNeuromodulationLevel();
                stats["curiosity"] = agent->getCuriosityLevel();
                stats["novelty"] = agent->getNoveltyLevel();
                results.push_back(stats);
            }
        }
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("steps"), py::arg("verbose") = false,
        "Run a complete simulation batch and return statistics");

    // Performance monitoring
    m.def("monitorBrainPerformance", [](std::shared_ptr<Brain> brain, 
                                         const std::vector<int>& steps) {
        std::map<std::string, std::vector<float>> performance;
        
        for (int step : steps) {
            // Record various metrics
            performance["spike_rate"].push_back(
                static_cast<float>(brain->getTotalSpikeCount()) / (step + 1));
            performance["firing_ratio"].push_back(
                static_cast<float>(brain->getFiringNeuronCount()) / 
                std::max(1.0f, static_cast<float>(brain->getTotalNeuronCount())));
            performance["e_i_ratio"].push_back(
                brain->getExcitationInhibitionRatio());
        }
        
        return performance;
    }, py::arg("brain"), py::arg("steps"), 
        "Monitor brain performance over time");

    // Statistical analysis
    m.def("computeStatistics", [](const std::vector<float>& data) {
        std::map<std::string, float> stats;
        
        if (data.empty()) {
            return stats;
        }
        
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        float mean = sum / data.size();
        
        float variance = 0.0f;
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= data.size();
        float stddev = std::sqrt(variance);
        
        float min = *std::min_element(data.begin(), data.end());
        float max = *std::max_element(data.begin(), data.end());
        
        stats["mean"] = mean;
        stats["stddev"] = stddev;
        stats["min"] = min;
        stats["max"] = max;
        
        return stats;
    }, py::arg("data"), 
        "Compute basic statistics for a dataset");

    // Brain analysis utilities
    m.def("analyzeBrainConnectivity", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, float> analysis;
        
        size_t total_neurons = brain->getTotalNeuronCount();
        size_t total_synapses = brain->getTotalSynapseCount();
        
        if (total_neurons > 0) {
            analysis["avg_synapses_per_neuron"] = static_cast<float>(total_synapses) / total_neurons;
            analysis["connection_density"] = std::min(1.0f, static_cast<float>(total_synapses) / (total_neurons * total_neurons));
        }
        
        analysis["total_neurons"] = static_cast<float>(total_neurons);
        analysis["total_synapses"] = static_cast<float>(total_synapses);
        analysis["firing_ratio"] = static_cast<float>(brain->getFiringNeuronCount()) / std::max(1.0f, static_cast<float>(total_neurons));
        
        return analysis;
    }, py::arg("brain"), 
        "Analyze brain connectivity and structure");

    // Advanced simulation control
    m.def("controlledStep", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int step, double time,
                              bool logActivity = false) {
        // Record pre-step state
        auto preState = std::make_map<std::string, float>{
            {"step", static_cast<float>(step)},
            {"time", static_cast<float>(time)},
            {"spikes", static_cast<float>(brain->getTotalSpikeCount())},
            {"firing", static_cast<float>(brain->getFiringNeuronCount())},
            {"e_i_ratio", brain->getExcitationInhibitionRatio()}
        };
        
        // Run simulation step
        world->update(time);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step, time);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, time);
        agent->applyRewardModulation(agent->getNeuromodulationLevel(), 0.0f);
        
        // Record post-step state
        auto postState = std::make_map<std::string, float>{
            {"step", static_cast<float>(step)},
            {"time", static_cast<float>(time)},
            {"spikes", static_cast<float>(brain->getTotalSpikeCount())},
            {"firing", static_cast<float>(brain->getFiringNeuronCount())},
            {"e_i_ratio", brain->getExcitationInhibitionRatio()},
            {"reward", agent->getNeuromodulationLevel()},
            {"curiosity", agent->getCuriosityLevel()}
        };
        
        if (logActivity) {
            std::cout << "Step " << step << ": " << std::endl;
            std::cout << "  Pre: " << preState["spikes"] << " spikes, " << preState["firing"] << " firing" << std::endl;
            std::cout << "  Post: " << postState["spikes"] << " spikes, " << postState["firing"] << " firing" << std::endl;
            std::cout << "  Action: " << (int)action << std::endl;
        }
        
        return std::make_pair(preState, postState);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("step"), py::arg("time"), py::arg("logActivity") = false,
        "Perform a controlled simulation step with detailed logging");

    // Validation utilities
    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> issues;
        
        if (brain->getTotalNeuronCount() == 0) {
            issues.push_back("No neurons in brain");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            issues.push_back("No synapses in brain");
        }
        
        float e_i_ratio = brain->getExcitationInhibitionRatio();
        if (std::isinf(e_i_ratio) && e_i_ratio < 0) {
            issues.push_back("Invalid excitation/inhibition ratio");
        }
        
        if (brain->getTotalSpikeCount() > 1000000) {
            issues.push_back("Extremely high spike count - possible runaway excitation");
        }
        
        return issues;
    }, py::arg("brain"), 
        "Validate brain state and return any issues found");

    // Learning and training utilities
    m.def("trainBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent,
                          const std::vector<std::pair<float, float>>& reward_schedule,
                          int steps_per_epoch, int num_epochs) {
        std::vector<std::map<std::string, float>> training_history;
        
        for (int epoch = 0; epoch < num_epochs; ++epoch) {
            float epoch_reward = 0.0f;
            int epoch_spikes = 0;
            
            for (int step = 0; step < steps_per_epoch; ++step) {
                // Get current reward target
                float target_reward = reward_schedule[epoch % reward_schedule.size()].first;
                float reward_weight = reward_schedule[epoch % reward_schedule.size()].second;
                
                // Update world
                world->update(0.1);
                
                // Process sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply motor command with reward modulation
                world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(target_reward * reward_weight, 0.0f);
                
                // Collect statistics
                epoch_reward += agent->getNeuromodulationLevel();
                epoch_spikes += brain->getTotalSpikeCount();
            }
            
            std::map<std::string, float> epoch_stats;
            epoch_stats["epoch"] = static_cast<float>(epoch);
            epoch_stats["reward"] = epoch_reward / steps_per_epoch;
            epoch_stats["spikes"] = static_cast<float>(epoch_spikes);
            epoch_stats["spike_rate"] = static_cast<float>(epoch_spikes) / steps_per_epoch;
            
            training_history.push_back(epoch_stats);
            
            std::cout << "Epoch " << epoch << ": Reward = " << epoch_stats["reward"] 
                     << ", Spike Rate = " << epoch_stats["spike_rate"] << std::endl;
        }
        
        return training_history;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("reward_schedule"), py::arg("steps_per_epoch"), py::arg("num_epochs"),
        "Train brain using a reward schedule");

    // Export/import utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Import brain state from file");

    // Utility functions
    m.def("benchmarkPerformance", [](std::shared_ptr<Brain> brain, 
                                     const std::vector<int>& steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step : steps) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::map<std::string, float> results;
        results["total_steps"] = static_cast<float>(steps.size());
        results["time_ms"] = static_cast<float>(duration.count());
        results["steps_per_second"] = static_cast<float>(steps.size()) / 
            std::max(1.0f, duration.count() / 1000.0f);
        
        return results;
    }, py::arg("brain"), py::arg("steps"), 
        "Benchmark brain performance");

    // Advanced configuration utilities
    m.def("getConfigStats", [](const std::shared_ptr<Config>& config) {
        std::map<std::string, float> stats;
        
        auto keys = config->getKeys();
        stats["key_count"] = static_cast<float>(keys.size());
        
        if (!keys.empty()) {
            // Count different value types
            int int_count = 0, float_count = 0, bool_count = 0, string_count = 0, 
                vector_count = 0, other_count = 0;
            
            for (const auto& key : keys) {
                auto val = config->get<std::string>(key);
                if (val.has_value()) {
                    // Try to determine type (simplified)
                    if (val->find('.') != std::string::npos) {
                        float_count++;
                    } else if (val->find('"') != std::string::npos) {
                        string_count++;
                    } else {
                        int_count++;
                    }
                }
            }
            
            stats["int_values"] = static_cast<float>(int_count);
            stats["float_values"] = static_cast<float>(float_count);
            stats["string_values"] = static_cast<float>(string_count);
            stats["bool_values"] = static_cast<float>(bool_count);
            stats["vector_values"] = static_cast<float>(vector_count);
            stats["other_values"] = static_cast<float>(other_count);
        }
        
        return stats;
    }, py::arg("config"), "Get statistics about configuration values");

    // Initialize constants
    m.attr("DEFAULT_BRAIN_NEURONS") = 1000;
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_CHECKPOINT_INTERVAL") = 1000;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
