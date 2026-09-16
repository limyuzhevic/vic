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
        })
        // Convenience methods for common configuration tasks
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value with validation")
        .def("get", py::overload_cast<const std::string&, const T&>(&Config::get, py::const), py::arg("key"), py::arg("default"))
        .def("getAs", [](const Config& self, const std::string& key) {
            auto result = self.getAs<std::string>(key);
            return py::make_tuple(result.first, result.second);
        }, py::arg("key"))
        .def("loadFromConfigString", [](Config& self, const std::string& json) {
            // Create a temporary file and load from it for convenience
            std::string tempPath = "/tmp/nlm_config_" + std::to_string(std::time(nullptr)) + ".json";
            std::ofstream file(tempPath);
            file << json;
            file.close();
            bool success = self.loadFromFile(tempPath);
            std::remove(tempPath.c_str());
            return success;
        }, py::arg("json"), "Load configuration from a JSON string")
        .def("toDict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Get the value - try different types based on what's stored
                bool has_int = self.has(key);
                if (has_int) {
                    result[key.c_str()] = self.getOr<int64_t>(key, 0);
                } else {
                    result[key.c_str()] = "<unknown>";
                }
            }
            return result;
        }, "Export configuration as Python dictionary")
        .def("validate", [](const Config& self, const std::string& pattern) {
            py::list validKeys;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find(pattern) != std::string::npos) {
                    validKeys.append(key);
                }
            }
            return validKeys;
        }, py::arg("pattern"), "Find configuration keys matching pattern");

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

    // Factory functions with presets
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfig", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::str(item.first);
            // Try to parse as different types
            try {
                int64_t intVal = py::cast<int64_t>(item.second);
                config->set(key, intVal, ConfigSource::Python);
            } catch (...) {
                try {
                    double floatVal = py::cast<double>(item.second);
                    config->set(key, floatVal, ConfigSource::Python);
                } catch (...) {
                    try {
                        bool boolVal = py::cast<bool>(item.second);
                        config->set(key, boolVal, ConfigSource::Python);
                    } catch (...) {
                        std::string strVal = py::cast<std::string>(item.second);
                        config->set(key, strVal, ConfigSource::Python);
                    }
                }
            }
        }
        return config;
    }, py::arg("configDict"), "Create a configuration from a Python dictionary");

    m.def("createConfigFromJSON", [](const std::string& json) {
        auto config = std::make_shared<Config>();
        std::string tempPath = "/tmp/nlm_config_" + std::to_string(std::time(nullptr)) + ".json";
        std::ofstream file(tempPath);
        file << json;
        file.close();
        config->loadFromFile(tempPath);
        std::remove(tempPath.c_str());
        return config;
    }, py::arg("json"), "Create a configuration from a JSON string");

    m.def("createOptimizedBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        
        // Apply optimizations based on config
        if (config->has("optimization.enable_vectorization") && 
            config->getOr<bool>("optimization.enable_vectorization", false)) {
            // Enable SIMD optimizations if available
            brain->getConfig()->set("optimization.simd_level", 3, ConfigSource::Default);
        }
        
        if (config->has("optimization.enable_memory_pools") &&
            config->getOr<bool>("optimization.enable_memory_pools", false)) {
            // Enable memory pools for better performance
            brain->getConfig()->set("memory.pool_size", 1024 * 1024, ConfigSource::Default);
        }
        
        return brain;
    }, py::arg("config"), "Create an optimized brain with performance enhancements");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createAgentSystem", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize a complete agent system");

    // Utility and helper functions
    m.def("validateBrainConfig", [](const std::shared_ptr<Config>& config) {
        py::dict issues;
        
        if (!config->has("brain.neuron_count")) {
            issues["missing_neuron_count"] = "brain.neuron_count is required";
        } else {
            int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
            if (neuronCount <= 0 || neuronCount > 1000000) {
                issues["invalid_neuron_count"] = "brain.neuron_count must be between 1 and 1,000,000";
            }
        }
        
        if (!config->has("brain.connection_probability")) {
            issues["missing_connection_probability"] = "brain.connection_probability is recommended";
        }
        
        return issues;
    }, py::arg("config"), "Validate brain configuration and return any issues");

    // Advanced brain creation with development presets
    m.def("createDevelopmentalBrain", [](int stage, int neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount, ConfigSource::Default);
        config->set("development.stage", static_cast<int>(stage), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("stage") = 0, py::arg("neuronCount") = 1000,
       "Create a brain with specified developmental stage");

    // Brain analysis and debugging utilities
    m.def("analyzeBrainDynamics", [](const std::shared_ptr<Brain>& brain) {
        py::dict analysis;
        
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["avg_firing_rate"] = brain->getAverageFiringRate();
        analysis["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        analysis["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Analyze neuron type distribution
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto neurons = region->getAllNeurons();
            int excitatory = 0, inhibitory = 0, sensory = 0, motor = 0, internal = 0;
            
            for (const auto& neuron : neurons) {
                switch (neuron->getType()) {
                    case NeuronType::Excitatory: excitatory++; break;
                    case NeuronType::Inhibitory: inhibitory++; break;
                    case NeuronType::Sensory: sensory++; break;
                    case NeuronType::Motor: motor++; break;
                    case NeuronType::Internal: internal++; break;
                }
            }
            
            analysis["neuron_type_distribution"] = py::make_tuple(
                excitatory, inhibitory, sensory, motor, internal);
        }
        
        // Analyze synaptic connectivity
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto synapses = region->getSynapses();
            int excitatory = 0, inhibitory = 0;
            
            for (const auto& synapse : synapses) {
                switch (synapse->getType()) {
                    case SynapseType::Excitatory: excitatory++; break;
                    case SynapseType::Inhibitory: inhibitory++; break;
                    default: break;
                }
            }
            
            analysis["synapse_type_distribution"] = py::make_tuple(excitatory, inhibitory);
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze brain dynamics and return comprehensive statistics");

    // Simulation utilities
    m.def("runControlledSimulation", [](std::shared_ptr<Brain> brain, int steps, 
                                         std::shared_ptr<SimpleWorld> world = nullptr,
                                         std::shared_ptr<AgentBrain> agent = nullptr) {
        py::list statistics;
        
        for (int step = 0; step < steps; ++step) {
            // Update world if provided
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                
                if (agent) {
                    agent->processSensoryInput(percept);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
            
            // Brain step
            brain->step(step);
            
            // Record statistics every 100 steps
            if (step % 100 == 0) {
                py::dict stats;
                stats["step"] = step;
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["total_spikes"] = brain->getTotalSpikeCount();
                stats["avg_firing_rate"] = brain->getAverageFiringRate();
                stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                statistics.append(stats);
            }
        }
        
        return statistics;
    }, py::arg("brain"), py::arg("steps"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
       "Run a controlled simulation with optional world and agent integration");

    // Configuration templates
    m.def("getDefaultProfiles", []() {
        py::dict profiles;
        
        // Performance profile
        {
            py::dict perfProfile;
            perfProfile["brain.neuron_count"] = 5000;
            perfProfile["brain.connection_probability"] = 0.05;
            perfProfile["optimization.enable_vectorization"] = true;
            perfProfile["optimization.enable_memory_pools"] = true;
            perfProfile["plasticity.stdp.enable"] = true;
            perfProfile["plasticity.hebbian.enable"] = true;
            perfProfiles["performance"] = perfProfile;
        }
        
        // Learning profile
        {
            py::dict learnProfile;
            learnProfile["brain.neuron_count"] = 10000;
            learnProfile["brain.connection_probability"] = 0.1;
            learnProfile["plasticity.stdp.enable"] = true;
            learnProfile["plasticity.hebbian.enable"] = true;
            learnProfile["plasticity.structural.enable"] = true;
            learnProfile["neuromod.dopamine.scale"] = 1.2;
            learnProfile["neuromod.curiosity.enable"] = true;
            learnProfile["neuromod.novelty.enable"] = true;
            perfProfiles["learning"] = learnProfile;
        }
        
        // Development profile
        {
            py::dict devProfile;
            devProfile["brain.neuron_count"] = 8000;
            devProfile["development.stage"] = 2;  // CriticalPeriod
            devProfile["brain.connection_probability"] = 0.08;
            devProfile["plasticity.structural.enable"] = true;
            devProfile["neuromod.curiosity.enable"] = true;
            perfProfiles["development"] = devProfile;
        }
        
        // Exploration profile
        {
            py::dict exploreProfile;
            exploreProfile["brain.neuron_count"] = 3000;
            exploreProfile["brain.connection_probability"] = 0.15;  // Higher connectivity
            exploreProfile["neuromod.curiosity.enable"] = true;
            exploreProfile["neuromod.novelty.enable"] = true;
            exploreProfile["plasticity.stdp.enable"] = true;
            exploreProfile["plasticity.stdp.tau_plus"] = 40.0;
            exploreProfile["plasticity.stdp.tau_minus"] = 40.0;
            perfProfiles["exploration"] = exploreProfile;
        }
        
        return perfProfiles;
    }, "Get predefined configuration profiles for different use cases");

    // Validation and error checking utilities
    m.def("validateSimulationSetup", [](std::shared_ptr<Brain> brain, 
                                         std::shared_ptr<SimpleWorld> world = nullptr,
                                         std::shared_ptr<AgentBrain> agent = nullptr) {
        py::dict validation;
        bool isValid = true;
        
        // Validate brain
        if (!brain->getConfig()) {
            validation["brain_config"] = "Brain has no configuration";
            isValid = false;
        }
        
        auto config = brain->getConfig();
        if (config) {
            int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
            if (neuronCount <= 0 || neuronCount > 1000000) {
                validation["brain_config"] = "Invalid neuron count: " + std::to_string(neuronCount);
                isValid = false;
            }
        }
        
        // Validate world if provided
        if (world) {
            int width = world->getWidth();
            int height = world->getHeight();
            if (width <= 0 || height <= 0) {
                validation["world_config"] = "Invalid world dimensions";
                isValid = false;
            }
        }
        
        // Validate agent if provided
        if (agent) {
            if (!agent->getBrain()) {
                validation["agent_config"] = "Agent has no brain";
                isValid = false;
            }
        }
        
        validation["overall_valid"] = isValid;
        return validation;
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
       "Validate simulation setup and return any issues");

    // Performance benchmarking utilities
    m.def("benchmarkWorldInteraction", [](std::shared_ptr<AgentBrain> agent,
                                          std::shared_ptr<SimpleWorld> world,
                                          int iterations = 100) {
        std::vector<double> times;
        std::vector<int> spikeCounts;
        
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
            times.push_back(elapsed);
            
            if (i % 10 == 0) {
                spikeCounts.push_back(agent->getBrain()->getFiringNeuronCount());
            }
        }
        
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        
        return py::make_tuple(avgTime, minTime, maxTime, spikeCounts);
    }, py::arg("agent"), py::arg("world"), py::arg("iterations") = 100,
       "Benchmark world interaction performance with detailed statistics");

    // Memory management utilities
    m.def("optimizeMemoryUsage", [](std::shared_ptr<Brain> brain) {
        auto config = brain->getConfig();
        
        // Enable memory pooling if available
        config->set("memory.enable_pools", true, ConfigSource::Default);
        config->set("memory.pool_size", 1024 * 1024, ConfigSource::Default);  // 1MB
        
        // Enable garbage collection friendly structures
        config->set("memory.gc_threshold", 0.8, ConfigSource::Default);
        
        // Optimize for specific use case
        config->set("optimization.strategic_gc", true, ConfigSource::Default);
        
        return "Memory optimization enabled. Recommended to restart brain for full effect.";
    }, py::arg("brain"), "Enable memory optimization settings for better performance");

    // Debugging and monitoring utilities
    m.def("enableBrainMonitoring", [](std::shared_ptr<Brain> brain, bool continuous = false) {
        auto config = brain->getConfig();
        
        config->set("debug.enable_monitoring", true, ConfigSource::Default);
        config->set("debug.monitoring_interval", 1.0, ConfigSource::Default);  // 1 second
        
        if (continuous) {
            config->set("debug.continuous_monitoring", true, ConfigSource::Default);
        }
        
        brain->logStatus();
        return "Brain monitoring enabled.";
    }, py::arg("brain"), py::arg("continuous") = false,
       "Enable brain monitoring and logging for debugging and analysis");

    // Export utility functions
    m.def("exportBrainState", [](const std::shared_ptr<Brain>& brain, const std::string& filename) {
        try {
            brain->save(filename);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error saving brain state: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("brain"), py::arg("filename"), "Save brain state to file");

    m.def("importBrainState", [](const std::string& filename) -> std::shared_ptr<Brain> {
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->load(filename);
            return brain;
        } catch (const std::exception& e) {
            std::cerr << "Error loading brain state: " << e.what() << std::endl;
            return nullptr;
        }
    }, py::arg("filename"), "Load brain state from file");

    // Performance optimization utilities
    m.def("optimizeBrainPerformance", [](std::shared_ptr<Brain> brain, const std::string& profile = "auto") {
        auto config = brain->getConfig();
        
        if (profile == "performance") {
            config->set("optimization.enable_vectorization", true, ConfigSource::Default);
            config->set("optimization.enable_memory_pools", true, ConfigSource::Default);
            config->set("optimization.simd_level", 3, ConfigSource::Default);
        } else if (profile == "learning") {
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        } else if (profile == "balanced") {
            config->set("optimization.enable_vectorization", true, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        } else {  // auto
            int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
            if (neuronCount > 5000) {
                config->set("optimization.enable_vectorization", true, ConfigSource::Default);
                config->set("optimization.enable_memory_pools", true, ConfigSource::Default);
            }
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        }
        
        return "Brain performance optimization applied with profile: " + profile;
    }, py::arg("brain"), py::arg("profile") = "auto", "Apply performance optimizations based on profile");
