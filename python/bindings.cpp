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
        .def("get", [](const Config& self, const std::string& key) {
            return self.getOr<std::string>(key, "");
        }, py::arg("key"),
             "Get a configuration value as string")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.getOr<int64_t>(key, 0);
        }, py::arg("key"),
             "Get a configuration value as integer")
        .def("getFloat", [](const Config& self, const std::string& key) {
            return self.getOr<double>(key, 0.0);
        }, py::arg("key"),
             "Get a configuration value as float")
        .def("getBool", [](const Config& self, const std::string& key) {
            // Convert to string first, then check if it's "true", "1", "yes", etc.
            std::string val = self.getOr<std::string>(key, "");
            return val == "true" || val == "1" || val == "yes";
        }, py::arg("key"),
             "Get a configuration value as boolean")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (string)")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (integer)")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (float)")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value ? "true" : "false", ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (boolean)")
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
        
        // Convenience methods for common operations
        .def("runExperiment", [](Brain& self, int steps, double timestep) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"),
             "Run a simulation experiment for a given number of steps")
        
        .def("getNeuronStats", [](const Brain& self) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["firing_rate"] = self.getAverageFiringRate();
            stats["spike_count"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive statistics about the brain")
        
        .def("resetAndInitialize", [](Brain& self, std::shared_ptr<Config> config) {
            self.reset();
            auto newBrain = std::make_shared<Brain>(config);
            if (newBrain->initialize()) {
                // Swap the internal implementation
                self.pImpl = newBrain->pImpl;
                newBrain->pImpl = nullptr;
                return true;
            }
            return false;
        }, py::arg("config"),
             "Reset and reinitialize the brain with a new configuration")
        
        .def("saveWithTimestamp", [](Brain& self, const std::string& basePath) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            std::string timestamp = ss.str();
            std::string filepath = basePath + "_" + timestamp + ".nlm";
            return self.save(filepath);
        }, py::arg("basePath"),
             "Save brain state with timestamp in filename")
        
        .def("getSystemInfo", [](const Brain& self) {
            std::map<std::string, std::string> info;
            info["version"] = "NLM 0.1.0";
            info["brain_type"] = "Artificial Neural Learning Machine";
            info["neuron_model"] = "LIF (Leaky Integrate-and-Fire)";
            info["plasticity"] = "STDP + Hebbian + Structural";
            info["memory_systems"] = "Working + Episodic + Associative";
            info[" neuromodulation"] = "Dopamine + Curiosity + Novelty";
            info["cognition"] = "Prediction + Planning + Concept Formation";
            return info;
        }, "Get system information and capabilities");

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
        
        // Convenience methods for agent brain operations
        .def("runEpisode", [](AgentBrain& self, const SensoryPercept& initialPercept, 
                             double maxTime, double timestep) {
            auto startTime = std::chrono::system_clock::now();
            double currentTime = 0.0;
            self.processSensoryInput(initialPercept);
            
            while (currentTime < maxTime) {
                self.updateDevelopment(timestep);
                auto action = self.decodeMotorCommand();
                // In a real implementation, we'd apply the action here
                currentTime += timestep;
                
                auto now = std::chrono::system_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime);
                if (elapsed.count() >= 1.0) {  // Log every second
                    break;
                }
            }
            self.reset();
        }, py::arg("initialPercept"), py::arg("maxTime"), py::arg("timestep"),
             "Run an agent episode with given initial percept")
        
        .def("getAgentStats", [](const AgentBrain& self) {
            std::map<std::string, double> stats;
            auto brain = self.getBrain();
            if (brain) {
                stats["neuromodulation_level"] = self.getNeuromodulationLevel();
                stats["curiosity_level"] = self.getCuriosityLevel();
                stats["novelty_level"] = self.getNoveltyLevel();
                stats["prediction_error"] = self.getPredictionError();
                stats["brain_spikes"] = brain->getTotalSpikeCount();
                stats["brain_firing_rate"] = brain->getAverageFiringRate();
            }
            return stats;
        }, "Get comprehensive statistics about the agent")
        
        .def("getLearningStatus", [](const AgentBrain& self) {
            std::map<std::string, bool> status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get current learning system status")
        
        .def("trainEpisode", [](AgentBrain& self, SimpleWorld& world, 
                               const SensoryPercept& initialPercept,
                               double maxTime = 10.0, double timestep = 0.01) {
            self.initialize(world);
            auto startTime = std::chrono::system_clock::now();
            double currentTime = 0.0;
            SensoryPercept currentPercept = initialPercept;
            
            while (currentTime < maxTime) {
                self.updateDevelopment(timestep);
                auto action = self.decodeMotorCommand();
                // Apply action in world
                if (action) {
                    world.applyMotorCommand(*action, currentTime);
                }
                
                currentPercept = world.getSensoryPercept();
                self.processSensoryInput(currentPercept);
                
                currentTime += timestep;
                auto now = std::chrono::system_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime);
                if (elapsed.count() >= maxTime) {
                    break;
                }
            }
            self.reset();
        }, py::arg("world"), py::arg("initialPercept"), 
             py::arg("maxTime") = 10.0, py::arg("timestep") = 0.01,
             "Run a complete training episode with world interaction")
        
        .def("getActionStats", [](const AgentBrain& self) {
            std::map<std::string, double> stats;
            // This would require access to action decoding statistics
            // Placeholder for future implementation
            stats["action_count"] = 0.0;
            stats["action_frequency"] = 0.0;
            return stats;
        }, "Get statistics about agent actions")
        
        .def("getPerformanceMetrics", [](const AgentBrain& self) {
            std::map<std::string, double> metrics;
            auto brain = self.getBrain();
            if (brain) {
                metrics["exploration_efficiency"] = self.getCuriosityLevel();
                metrics["prediction_accuracy"] = 1.0 - self.getPredictionError();
                metrics["adaptation_rate"] = self.getNeuromodulationLevel();
                metrics["learning_speed"] = brain->getAverageFiringRate() * 0.1;
            }
            return metrics;
        }, "Get performance metrics for the agent");

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
    
    // Advanced factory methods for common setups
    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a brain with default configuration");

    m.def("createAgentBrainWithWorld", [](std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("world"), "Create an agent brain with initialized world");

    m.def("createStandardWorld", [](size_t width, size_t height, 
                                   size_t visionWidth = 5, 
                                   size_t visionHeight = 5) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setAgentStart(width/2, height/2);
        return world;
    }, py::arg("width"), py::arg("height"), 
       py::arg("visionWidth") = 5, py::arg("visionHeight") = 5,
       "Create a standard world with given dimensions");
    
    // Convenience functions for experiment setup
    m.def("setupExperimentWorld", [](std::shared_ptr<SimpleWorld> world) {
        // Add some resources and hazards to the world
        auto agentBody = world->getAgentBody();
        
        // Add resources (food, water, etc.)
        for (int i = 0; i < 3; ++i) {
            float x = static_cast<float>(rand() % world->getWidth());
            float y = static_cast<float>(rand() % world->getHeight());
            WorldObject resource;
            resource.x = x;
            resource.y = y;
            resource.type = WorldObjectType::Resource;
            resource.value = 10.0f + static_cast<float>(rand() % 20);
            resource.radius = 0.3f;
            world->addObject(resource);
        }
        
        // Add some hazards
        for (int i = 0; i < 2; ++i) {
            float x = static_cast<float>(rand() % world->getWidth());
            float y = static_cast<float>(rand() % world->getHeight());
            WorldObject hazard;
            hazard.x = x;
            hazard.y = y;
            hazard.type = WorldObjectType::Hazard;
            hazard.value = -20.0f;
            hazard.radius = 0.4f;
            world->addObject(hazard);
        }
        
        // Add a marker
        WorldObject marker;
        marker.x = 10.0f;
        marker.y = 10.0f;
        marker.type = WorldObjectType::Marker;
        marker.value = 0.0f;
        marker.radius = 0.2f;
        world->addObject(marker);
        
        return world;
    }, py::arg("world"), "Setup a standard experiment world with resources, hazards, and markers");
    
    // Advanced utilities and helpers
    m.def("createSensoryPercept", [](const Vision& vision, 
                                   const InternalSignals& internal,
                                   const Audio& audio) {
        SensoryPercept percept;
        percept.setVision(vision);
        percept.setInternal(internal);
        percept.setAudio(audio);
        return percept;
    }, py::arg("vision"), py::arg("internal"), py::arg("audio"),
       "Create a sensory percept from all input types");
    
    m.def("createAction", [](ActionType type, std::vector<float> parameters) {
        return Action(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create an action with type and parameters");
    
    m.def("getDefaultActionParameters", [](ActionType type) {
        std::map<std::string, double> params;
        switch (type) {
            case ActionType::MoveForward:
                params["speed"] = 1.0;
                break;
            case ActionType::TurnLeft:
                params["angle"] = 0.5;
                break;
            case ActionType::Look:
                params["distance"] = 5.0;
                break;
            default:
                params["value"] = 1.0;
        }
        return params;
    }, py::arg("type"), "Get default parameters for an action type");
    
    // Performance and debugging utilities
    m.def("benchmarkSimulation", [](Brain& brain, int steps, double timestep) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain.step(i, i * timestep);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::map<std::string, double> results;
        results["steps"] = static_cast<double>(steps);
        results["timestep"] = timestep;
        results["duration_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = steps / (duration.count() / 1000.0);
        results["spikes_per_step"] = brain.getTotalSpikeCount() / static_cast<double>(steps);
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
       "Benchmark simulation performance and return metrics");
    
    m.def("saveBrainCheckpoint", [](Brain& brain, const std::string& prefix) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        std::string timestamp = ss.str();
        return brain.save(prefix + "_checkpoint_" + timestamp + ".nlm");
    }, py::arg("brain"), py::arg("prefix"), "Save a brain checkpoint with timestamp");
    
    m.def("loadBrainCheckpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        }
        return nullptr;
    }, py::arg("filepath"), "Load a brain from checkpoint file");
    
    // Educational and demonstration utilities
    m.def("runLearningDemo", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning experiment
        for (int step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto stats = brain->getNeuronStats();
        stats["learning_phases_completed"] = 1;
        return stats;
    }, "Run a learning demonstration and return statistics");
    
    m.def("createDevelopmentDemoConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0005f, ConfigSource::Default);
        config->set("pruning_rate", 0.00005f, ConfigSource::Default);
        config->set("critical_period_start", 10, ConfigSource::Default);
        config->set("critical_period_end", 100, ConfigSource::Default);
        return config;
    }, "Create a configuration suitable for development demonstrations");

} // namespace nlm
