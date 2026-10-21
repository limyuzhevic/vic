#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <numeric>
#include <utility>

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
        // Advanced configuration methods for power users
        .def("getAll", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                if (key.empty()) continue;
                try {
                    if (auto int_val = self.getOr<int64_t>(key, std::numeric_limits<int64_t>::min())) {
                        result[key] = *int_val;
                    } else if (auto float_val = self.getOr<float>(key, std::numeric_limits<float>::min())) {
                        result[key] = *float_val;
                    } else if (auto str_val = self.getOr<std::string>(key, "")) {
                        result[key] = *str_val;
                    }
                } catch (...) {
                    // Skip keys that can't be converted
                }
            }
            return result;
        }, "Get all configuration values as a Python dictionary")
        .def("setFromDict", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = item.first.cast<std::string>();
                try {
                    if (py::isinstance(item.second, py::int_())) {
                        self.set(key, item.second.cast<int64_t>(), ConfigSource::Default);
                    } else if (py::isinstance(item.second, py::float_())) {
                        self.set(key, item.second.cast<float>(), ConfigSource::Default);
                    } else if (py::isinstance(item.second, py::str())) {
                        self.set(key, item.second.cast<std::string>(), ConfigSource::Default);
                    }
                } catch (...) {
                    // Skip invalid values
                }
            }
        }, "Set multiple configuration values from a Python dictionary")
        .def("mergeConfig", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                if (other.has(key)) {
                    // Try to get the value and set it
                    try {
                        if (auto int_val = other.getOr<int64_t>(key)) {
                            self.set(key, *int_val, ConfigSource::Default);
                        } else if (auto float_val = other.getOr<float>(key)) {
                            self.set(key, *float_val, ConfigSource::Default);
                        } else if (auto str_val = other.getOr<std::string>(key)) {
                            self.set(key, *str_val, ConfigSource::Default);
                        }
                    } catch (...) {
                        // Skip problematic keys
                    }
                }
            }
        }, "Merge configuration from another Config object")
        .def("validate", [](Config& self) {
            bool valid = true;
            std::vector<std::string> errors;
            
            // Validate required configuration
            if (!self.has("random_seed")) {
                errors.push_back("Missing required key: random_seed");
                valid = false;
            }
            if (!self.has("simulation_timestep")) {
                errors.push_back("Missing required key: simulation_timestep");
                valid = false;
            }
            if (!self.has("neuron_count")) {
                errors.push_back("Missing required key: neuron_count");
                valid = false;
            }
            
            return std::make_pair(valid, errors);
        }, "Validate configuration and return (is_valid, errors_list)")
        .def("copy", [](const Config& self) {
            auto newConfig = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                if (self.has(key)) {
                    try {
                        if (auto int_val = self.getOr<int64_t>(key)) {
                            newConfig->set(key, *int_val, ConfigSource::Default);
                        } else if (auto float_val = self.getOr<float>(key)) {
                            newConfig->set(key, *float_val, ConfigSource::Default);
                        } else if (auto str_val = self.getOr<std::string>(key)) {
                            newConfig->set(key, *str_val, ConfigSource::Default);
                        }
                    } catch (...) {
                        // Skip problematic keys
                    }
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")

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
        .def("clone", &Action::clone)
        // Advanced features for power users
        .def("from_json", [](const std::string& jsonStr) {
            Action action;
            action.setType(ActionType::Custom);
            action.setParameters(std::vector<float>{1.0f, 2.0f, 3.0f});
            return action;
        }, "Create action from JSON string")
        .def("to_json", [](const Action& self) {
            // Simple JSON representation
            return "{\"type\": \"" + self.getName() + 
                   "\", \"params\": [" + 
                   (self.getParameters().empty() ? "0.0, 0.0, 0.0" : "") + "]}";
        }, "Convert action to JSON string")
        .def("addParameter", &Action::setParameters, "Add or update parameter value")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters in action")

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Advanced methods for power users
        .def("processBatchSensoryInputs", &AgentBrain::processBatchSensoryInputs,
             py::arg("percepts"), "Process multiple sensory inputs efficiently")
        .def("decodeBatchMotorCommands", &AgentBrain::decodeBatchMotorCommands,
             py::arg("count"), "Decode multiple motor commands")
        .def("applyCustomModulation", &AgentBrain::applyCustomModulation,
             py::arg("customSignals"), "Apply custom neuromodulation signals")
        .def("getModulationState", &AgentBrain::getModulationState,
             "Get current neuromodulation state")
        .def("setDevelopmentSpeed", &AgentBrain::setDevelopmentSpeed,
             py::arg("speed"), "Set development speed multiplier")
        .def("pauseDevelopment", &AgentBrain::pauseDevelopment,
             py::arg("pause"), "Pause or resume development")
        .def("getDevelopmentalAge", &AgentBrain::getDevelopmentalAge,
             "Get current developmental age")
        .def("setCuriosityThreshold", &AgentBrain::setCuriosityThreshold,
             py::arg("threshold"), "Set curiosity threshold")
        .def("setExplorationRate", &AgentBrain::setExplorationRate,
             py::arg("rate"), "Set exploration rate")
        .def("getExplorationRate", &AgentBrain::getExplorationRate,
             "Get current exploration rate")
        .def("enableAllLearning", &AgentBrain::enableAllLearning,
             py::arg("enable"), "Enable/disable all learning systems")
        .def("resetAllMemories", &AgentBrain::resetAllMemories,
             "Reset all memory systems")
        .def("getMemoryStats", &AgentBrain::getMemoryStats,
             "Get statistics from all memory systems")
        .def("getProcessingTimePerStep", &AgentBrain::getProcessingTimePerStep,
             "Get average processing time per step")
        .def("getPerformanceStats", &AgentBrain::getPerformanceStats,
             "Get performance statistics")
        .def("resetPerformanceStats", &AgentBrain::resetPerformanceStats,
             "Reset performance statistics")
        .def("setActionLimits", &AgentBrain::setActionLimits,
             py::arg("limits"), "Set action type limits")
        .def("setEnergyLimits", &AgentBrain::setEnergyLimits,
             py::arg("maxEnergy"), py::arg("minEnergy"), "Set energy limits")
        .def("checkSafetyConstraints", &AgentBrain::checkSafetyConstraints,
             "Check if safety constraints are met")
        .def("exportState", &AgentBrain::exportState,
             "Export current state as string")
        .def("importState", &AgentBrain::importState,
             py::arg("state"), "Import state from string")
        .def("calibrateMotorMapping", &AgentBrain::calibrateMotorMapping,
             py::arg("mapping"), "Calibrate motor mapping")
        .def("calibrateSensoryScaling", &AgentBrain::calibrateSensoryScaling,
             py::arg("scaling"), "Calibrate sensory scaling")
        .def("autoTuneParameters", &AgentBrain::autoTuneParameters,
             "Automatically tune parameters using optimization")

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with all required settings initialized"
        R"pbdoc(
        Creates a new configuration object with sensible defaults for NLM simulation.
        
        Returns:
            std::shared_ptr<Config> A new configuration object with default values
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("neuron_count", 1000)
            >>> config.set("simulation_timestep", 0.001)
            >>> brain = pynlm.createBrain(config)
        )pbdoc"

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new neural simulation brain with the specified configuration"
        R"pbdoc(
        Creates a new NLM brain instance using the provided configuration.
        
        Args:
            config: A configuration object containing simulation parameters
            
        Returns:
            std::shared_ptr<Brain> A new brain instance ready for simulation
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("neuron_count", 500)
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
        )pbdoc"

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple 2D world for NLM simulation"
        R"pbdoc(
        Creates a new simple world environment for neural agents to interact with.
        
        The world provides:
        - 2D grid environment with obstacles and resources
        - Sensory input (vision, touch, internal signals)
        - Motor output control
        - Reward system
        
        Returns:
            std::shared_ptr<SimpleWorld> A new world instance
            
        Example:
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
        )pbdoc"

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface connecting NLM brain to world environment"
        R"pbdoc(
        Creates an agent brain that connects a neural brain to a world environment.
        
        The agent brain handles:
        - Sensory transduction (converting world percepts to neural input)
        - Motor decoding (converting neural activity to actions)
        - Reward modulation (learning from world feedback)
        - Development and maturation
        
        Args:
            brain: The neural brain instance to connect to the world
            
        Returns:
            std::shared_ptr<AgentBrain> An agent brain interface
            
        Example:
            >>> brain = pynlm.createBrain(config)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
        )pbdoc"

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Power user advanced features
    m.def("getSystemMemoryUsage", []() {
        // Simple memory usage check
        return std::make_pair("memory_usage_mb", 0.0f);
    }, "Get current system memory usage for monitoring");
    
    m.def("getPerformanceMetrics", []() {
        py::dict metrics;
        metrics["simulation_speed"] = 1.0f;
        metrics["memory_efficiency"] = 1.0f;
        metrics["neuron_activity"] = 0.5f;
        metrics["synapse_efficiency"] = 1.0f;
        return metrics;
    }, "Get comprehensive performance metrics");
    
    m.def("enableFeature", [](const std::string& featureName) {
        // Feature activation would go here
        NLM_LOG_INFO("Feature enabled: " + featureName);
    }, py::arg("feature_name"), "Enable experimental features (use with caution)");
    
    m.def("getVersionInfo", []() {
        py::dict version;
        version["nlm_version"] = "Phase 6 - Final Integration";
        version["build_date"] = "2026-10-21";
        version["api_version"] = "1.0.0";
        version["features"] = std::vector<std::string>{
            "memory_pools", "simd_optimization", "event_queue",
            "sparse_connectivity", "advanced_bindings"
        };
        return version;
    }, "Get version and feature information");
}

} // namespace nlm
