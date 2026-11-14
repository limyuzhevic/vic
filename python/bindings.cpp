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

    m.def("createAdvancedConfig", [](std::map<std::string, py::object> config_dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Helper function to convert py::object to ConfigValue
        auto convert_to_config_value = [](py::object obj) -> ConfigValue {
            if (obj.is_none()) {
                return ConfigValue{}; // Empty variant
            } else if (obj.is<float>()) {
                return obj.cast<float>();
            } else if (obj.is<double>()) {
                return obj.cast<double>();
            } else if (obj.is<int>()) {
                return obj.cast<int>();
            } else if (obj.is<int64_t>()) {
                return obj.cast<int64_t>();
            } else if (obj.is<bool>()) {
                return obj.cast<bool>();
            } else if (obj.is<std::string>()) {
                return obj.cast<std::string>();
            } else if (obj.is<py::list>()) {
                auto lst = obj.cast<py::list>();
                std::vector<int> int_vec;
                std::vector<double> double_vec;
                std::vector<std::string> string_vec;
                
                for (size_t i = 0; i < lst.size(); ++i) {
                    auto item = lst[i];
                    if (item.is<float>()) {
                        double_vec.push_back(item.cast<double>());
                    } else if (item.is<double>()) {
                        double_vec.push_back(item.cast<double>());
                    } else if (item.is<int>()) {
                        int_vec.push_back(item.cast<int>());
                    } else if (item.is<std::string>()) {
                        string_vec.push_back(item.cast<std::string>());
                    }
                }
                
                if (!int_vec.empty()) {
                    return ConfigValue{int_vec};
                } else if (!double_vec.empty()) {
                    return ConfigValue{double_vec};
                } else if (!string_vec.empty()) {
                    return ConfigValue{string_vec};
                }
            }
            
            return ConfigValue{}; // Default empty
        };
        
        // Set configuration values from dictionary
        for (const auto& item : config_dict) {
            std::string key = item.first;
            py::object value = item.second;
            
            ConfigValue config_value = convert_to_config_value(value);
            config->set(key, config_value, ConfigSource::Runtime);
        }
        
        return config;
    }, py::arg("config_dict"), "Create an advanced configuration from a dictionary");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainWithSystem", [](std::shared_ptr<Config> config, bool enable_memory, bool enable_neuromodulation, bool enable_plasticity, bool enable_development) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        
        // Configure brain systems based on parameters
        if (enable_memory) {
            // Enable memory systems if requested
            config->set("memory.working_memory.enabled", true, ConfigSource::Runtime);
            config->set("memory.episodic_memory.enabled", true, ConfigSource::Runtime);
            config->set("memory.associative_memory.enabled", true, ConfigSource::Runtime);
        }
        
        if (enable_neuromodulation) {
            // Enable neuromodulation systems if requested
            config->set("neuromodulation.dopamine.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.curiosity.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.novelty.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.prediction_error.enabled", true, ConfigSource::Runtime);
        }
        
        if (enable_plasticity) {
            // Enable plasticity systems if requested
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
        }
        
        if (enable_development) {
            // Enable development system if requested
            config->set("development.enabled", true, ConfigSource::Runtime);
        }
        
        brain->initialize();
        return brain;
    }, py::arg("config"), py::arg("enable_memory") = true, py::arg("enable_neuromodulation") = true, 
       py::arg("enable_plasticity") = true, py::arg("enable_development") = true, 
       "Create a brain with advanced system configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createWorldWithEnvironment", [](float width, float height, size_t vision_width, size_t vision_height, float max_energy) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setMaxEnergy(max_energy);
        world->reset();
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, py::arg("vision_width") = 16, 
       py::arg("vision_height") = 16, py::arg("max_energy") = 100.0f, 
       "Create a simple world with custom environment configuration");

    m.def("createAdvancedWorld", [](std::map<std::string, py::object> world_dict) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        
        // Set world configuration from dictionary
        for (const auto& item : world_dict) {
            std::string key = item.first;
            py::object value = item.second;
            
            if (key == "width" && value.is<float>()) {
                world->configure(value.cast<float>(), world->getHeight(), 
                               world->getVisionWidth(), world->getVisionHeight());
            } else if (key == "height" && value.is<float>()) {
                world->configure(world->getWidth(), value.cast<float>(),
                               world->getVisionWidth(), world->getVisionHeight());
            } else if (key == "vision_width" && value.is<size_t>()) {
                world->configure(world->getWidth(), world->getHeight(),
                               value.cast<size_t>(), world->getVisionHeight());
            } else if (key == "vision_height" && value.is<size_t>()) {
                world->configure(world->getWidth(), world->getHeight(),
                               world->getVisionWidth(), value.cast<size_t>());
            } else if (key == "max_energy" && value.is<float>()) {
                world->setMaxEnergy(value.cast<float>());
            } else if (key == "energy_decay_rate" && value.is<float>()) {
                world->setEnergyDecayRate(value.cast<float>());
            }
        }
        
        world->reset();
        return world;
    }, py::arg("world_dict"), "Create a simple world with advanced configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createAgentWithConfiguration", [](std::shared_ptr<Brain> brain, std::map<std::string, py::object> agent_config) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure agent based on settings
        for (const auto& item : agent_config) {
            std::string key = item.first;
            py::object value = item.second;
            
            if (key == "enable_reward_modulation" && value.is<bool>()) {
                agent->enableRewardModulation(value.cast<bool>());
            } else if (key == "enable_structural_plasticity" && value.is<bool>()) {
                agent->enableStructuralPlasticity(value.cast<bool>());
            } else if (key == "enable_development" && value.is<bool>()) {
                agent->enableDevelopment(value.cast<bool>());
            } else if (key == "enable_curiosity" && value.is<bool>()) {
                agent->enableCuriosity(value.cast<bool>());
            }
        }
        
        return agent;
    }, py::arg("brain"), py::arg("agent_config"), "Create an agent brain with advanced configuration");

    // Advanced Python API functions
    m.def("configure_brain_systems", [](std::shared_ptr<Brain> brain, 
                                       bool enable_memory = true,
                                       bool enable_neuromodulation = true,
                                       bool enable_plasticity = true,
                                       bool enable_development = true) {
        auto config = brain->getConfig();
        
        if (enable_memory) {
            config->set("memory.working_memory.enabled", true, ConfigSource::Runtime);
            config->set("memory.episodic_memory.enabled", true, ConfigSource::Runtime);
            config->set("memory.associative_memory.enabled", true, ConfigSource::Runtime);
        }
        
        if (enable_neuromodulation) {
            config->set("neuromodulation.dopamine.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.curiosity.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.novelty.enabled", true, ConfigSource::Runtime);
            config->set("neuromodulation.prediction_error.enabled", true, ConfigSource::Runtime);
        }
        
        if (enable_plasticity) {
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
        }
        
        if (enable_development) {
            config->set("development.enabled", true, ConfigSource::Runtime);
        }
        
        // Re-initialize brain with new configuration
        brain->reset();
        brain->initialize();
    }, py::arg("brain"), py::arg("enable_memory") = true, py::arg("enable_neuromodulation") = true,
       py::arg("enable_plasticity") = true, py::arg("enable_development") = true,
       "Configure brain systems with advanced settings");

    m.def("run_advanced_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                                       std::shared_ptr<SimpleWorld> world, 
                                       size_t num_steps, double timestep = 0.1,
                                       bool enable_checkpointing = false) {
        // Run advanced simulation with monitoring
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get motor command
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(motor_cmd, step * timestep);
            
            // Apply reward modulation
            auto reward = world->getSensoryPercept().getInternal()[0];
            if (!world->getSensoryPercept().getInternal().empty()) {
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Update development
            agent->updateDevelopment(timestep);
            
            // Enable checkpointing if requested
            if (enable_checkpointing && step % 100 == 0) {
                std::string checkpoint_file = "checkpoint_step_" + std::to_string(step) + ".bin";
                brain->save(checkpoint_file);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple(num_steps, duration.count());
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("num_steps"), 
       py::arg("timestep") = 0.1, py::arg("enable_checkpointing") = false,
       "Run advanced simulation with comprehensive monitoring");

    m.def("analyze_simulation_performance", [](std::shared_ptr<Brain> brain, 
                                              std::shared_ptr<AgentBrain> agent,
                                              std::shared_ptr<SimpleWorld> world,
                                              size_t steps_to_analyze) {
        // Analyze simulation performance
        std::map<std::string, double> performance_metrics;
        
        // Neural performance metrics
        performance_metrics["total_neurons"] = brain->getTotalNeuronCount();
        performance_metrics["total_synapses"] = brain->getTotalSynapseCount();
        performance_metrics["firing_rate"] = brain->getAverageFiringRate();
        performance_metrics["activity_level"] = brain->getFiringNeuronCount() / 
                                               std::max(1.0, brain->getTotalNeuronCount());
        
        // Plasticity metrics
        auto* stdp = brain->getSTDP();
        if (stdp) {
            performance_metrics["stdp_weight_change"] = stdp->getTotalWeightChange();
            performance_metrics["stdp_plasticity_level"] = stdp->getPlasticityLevel();
        }
        
        auto* hebbian = brain->getHebbian();
        if (hebbian) {
            performance_metrics["hebbian_weight_change"] = hebbian->getTotalWeightChange();
            performance_metrics["hebbian_plasticity_level"] = hebbian->getPlasticityLevel();
        }
        
        // Memory performance metrics
        performance_metrics["working_memory_usage"] = agent->getWorkingMemoryUsage();
        performance_metrics["episodic_memory_usage"] = agent->getEpisodicMemoryUsage();
        performance_metrics["concept_count"] = agent->getConceptCount();
        
        // Neuromodulation metrics
        performance_metrics["dopamine_level"] = agent->getNeuromodulationLevel();
        performance_metrics["curiosity_level"] = agent->getCuriosityLevel();
        performance_metrics["novelty_level"] = agent->getNoveltyLevel();
        performance_metrics["prediction_error"] = agent->getPredictionError();
        
        // Development metrics
        performance_metrics["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        performance_metrics["developmental_age"] = agent->getDevelopmentalAge();
        performance_metrics["plasticity_modifier"] = agent->getPlasticityModifier();
        
        // World performance metrics
        performance_metrics["world_simulation_time"] = world->getSimulationTime();
        performance_metrics["world_energy_level"] = world->getAgentBody().energy;
        performance_metrics["world_health_level"] = world->getAgentBody().health;
        
        return performance_metrics;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps_to_analyze") = 1000,
       "Analyze simulation performance with comprehensive metrics");

    m.def("export_research_data", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world,
                                   const std::string& filename,
                                   const std::string& format = "json") {
        // Export research data in specified format
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filename);
            }
            
            if (format == "json") {
                // Export as JSON
                nlohmann::json data;
                
                // Brain data
                data["brain"]["total_neurons"] = brain->getTotalNeuronCount();
                data["brain"]["total_synapses"] = brain->getTotalSynapseCount();
                data["brain"]["firing_rate"] = brain->getAverageFiringRate();
                data["brain"]["spike_count"] = brain->getTotalSpikeCount();
                
                // Agent data
                data["agent"]["curiosity_level"] = agent->getCuriosityLevel();
                data["agent"]["novelty_level"] = agent->getNoveltyLevel();
                data["agent"]["dopamine_level"] = agent->getNeuromodulationLevel();
                data["agent"]["prediction_error"] = agent->getPredictionError();
                data["agent"]["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
                data["agent"]["concept_count"] = agent->getConceptCount();
                
                // World data
                data["world"]["simulation_time"] = world->getSimulationTime();
                data["world"]["agent_energy"] = world->getAgentBody().energy;
                data["world"]["agent_health"] = world->getAgentBody().health;
                data["world"]["agent_x"] = world->getAgentBody().x;
                data["world"]["agent_y"] = world->getAgentBody().y;
                data["world"]["agent_orientation"] = world->getAgentBody().orientation;
                
                // Write JSON data
                file << data.dump(2);
                
            } else if (format == "csv") {
                // Export as CSV
                file << "Metric,Value\n";
                file << "brain.total_neurons," << brain->getTotalNeuronCount() << "\n";
                file << "brain.total_synapses," << brain->getTotalSynapseCount() << "\n";
                file << "brain.firing_rate," << brain->getAverageFiringRate() << "\n";
                file << "brain.spike_count," << brain->getTotalSpikeCount() << "\n";
                file << "agent.curiosity_level," << agent->getCuriosityLevel() << "\n";
                file << "agent.novelty_level," << agent->getNoveltyLevel() << "\n";
                file << "agent.dopamine_level," << agent->getNeuromodulationLevel() << "\n";
                file << "agent.prediction_error," << agent->getPredictionError() << "\n";
                file << "agent.developmental_stage," << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
                file << "agent.concept_count," << agent->getConceptCount() << "\n";
                file << "world.simulation_time," << world->getSimulationTime() << "\n";
                file << "world.agent_energy," << world->getAgentBody().energy << "\n";
                file << "world.agent_health," << world->getAgentBody().health << "\n";
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Error exporting research data: ") + e.what());
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"), 
       py::arg("format") = "json", "Export research data in specified format");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
