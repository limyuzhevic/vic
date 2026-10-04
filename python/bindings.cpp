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

// Custom exception classes for Python bindings
namespace py = pybind11;

class InvalidInputError : public std::runtime_error {
public:
    explicit InvalidInputError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& message) : std::runtime_error(message) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& message) : std::runtime_error(message) {}
};

class FileIOError : public std::runtime_error {
public:
    explicit FileIOError(const std::string& message) : std::runtime_error(message) {}
};

namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<InvalidInputError>(m, "InvalidInputError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<FileIOError>(m, "FileIOError");

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
        .def(py::init<const std::unordered_map<std::string, std::string>&>(), py::arg("config_dict"),
             "Create configuration from a dictionary mapping keys to values")
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
    
    // Convenience factory methods for common configurations
    m.def("create_default_config", []() {
        auto config_map = std::unordered_map<std::string, std::string>{
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"neuron_count", "1000"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"plasticity_learning_rate", "0.01"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"stdp_time_constant", "20.0"},
            {"development_synaptogenesis_rate", "0.001"},
            {"development_pruning_rate", "0.0001"},
            {"dopamine_baseline", "0.1"},
            {"reward_discount_factor", "0.99"},
            {"environment_name", "GridWorld"},
            {"environment_width", "10"},
            {"environment_height", "10"},
            {"max_simulation_steps", "10000"},
            {"log_level", "INFO"}
        };
        return std::make_shared<Config>(config_map);
    }, "Create a configuration with sensible default values for simulation");
    
    m.def("create_minimal_config", []() {
        auto config_map = std::unordered_map<std::string, std::string>{
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"neuron_count", "100"}
        };
        return std::make_shared<Config>(config_map);
    }, "Create a minimal configuration with only essential settings");
    
    m.def("create_research_config", [](int neuron_count, float learning_rate, float connection_prob) {
        auto config_map = std::unordered_map<std::string, std::string>{
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"neuron_count", std::to_string(neuron_count)},
            {"connection_probability", std::to_string(connection_prob)},
            {"plasticity_learning_rate", std::to_string(learning_rate)},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"development_synaptogenesis_rate", "0.001"},
            {"development_pruning_rate", "0.0001"}
        };
        return std::make_shared<Config>(config_map);
    }, py::arg("neuron_count") = 1000, py::arg("learning_rate") = 0.01, 
       py::arg("connection_prob") = 0.1, "Create a research configuration with specified parameters");

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert sensory percept to a Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            SensoryPercept percept;
            if (data.contains("vision")) {
                std::vector<float> vision = data["vision"].cast<std::vector<float>>();
                percept.setVision(vision);
            }
            if (data.contains("touch")) {
                std::vector<float> touch = data["touch"].cast<std::vector<float>>();
                percept.setTouch(touch);
            }
            if (data.contains("internal")) {
                std::vector<float> internal = data["internal"].cast<std::vector<float>>();
                percept.setInternal(internal);
            }
            if (data.contains("proprioception")) {
                std::vector<float> proprioception = data["proprioception"].cast<std::vector<float>>();
                percept.setProprioception(proprioception);
            }
            if (data.contains("audio")) {
                std::vector<float> audio = data["audio"].cast<std::vector<float>>();
                percept.setAudio(audio);
            }
            if (data.contains("timestamp")) {
                double timestamp = data["timestamp"].cast<double>();
                percept.setTimestamp(timestamp);
            }
            return percept;
        }, py::arg("data"), "Create sensory percept from a Python dictionary")
        .def("copy", [](const SensoryPercept& percept) {
            SensoryPercept copy = percept;
            return copy;
        }, "Create a copy of the sensory percept");
    
    // Convenience factory methods for sensory inputs
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromNumpy", [](Vision& self, py::array_t<float> data) {
            auto buf = data.request();
            self.setData(static_cast<float*>(buf.ptr), buf.size);
        }, py::arg("data"), "Set vision data from NumPy array")
        .def("getDataAsNumpy", [](const Vision& self) {
            py::array_t<float> result({self.getWidth(), self.getHeight(), self.getChannels()});
            auto buf = result.request();
            // Note: This assumes setData stores data in row-major order
            size_t idx = 0;
            for (size_t c = 0; c < self.getChannels(); ++c) {
                for (size_t h = 0; h < self.getHeight(); ++h) {
                    for (size_t w = 0; w < self.getWidth(); ++w) {
                        buf.ptr<float>[idx++] = self.getData()[c * self.getHeight() * self.getWidth() + h * self.getWidth() + w];
                    }
                }
            }
            return result;
        }, "Get vision data as NumPy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromNumpy", [](Audio& self, py::array_t<float> data) {
            auto buf = data.request();
            self.setData(static_cast<float*>(buf.ptr), buf.size);
        }, py::arg("data"), "Set audio data from NumPy array")
        .def("getDataAsNumpy", [](const Audio& self) {
            py::array_t<float> result({self.getNumSamples()});
            auto buf = result.request();
            std::copy(self.getData().begin(), self.getData().end(), buf.ptr<float>);
            return result;
        }, "Get audio data as NumPy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float signal : signals) {
                self.addSignal(signal);
            }
        }, py::arg("signals"), "Add multiple signals at once")
        .def("getSignalsAsNumpy", [](const InternalSignals& self) {
            py::array_t<float> result({self.getData().size()});
            auto buf = result.request();
            std::copy(self.getData().begin(), self.getData().end(), buf.ptr<float>);
            return result;
        }, "Get internal signals as NumPy array")
        .def("clearSignals", &InternalSignals::clearSignals);

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
        .def(py::init<>(), "Create brain with default configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initialize", [](Brain& self) {
            self.initialize();
        }, "Initialize the brain with default configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("run_steps", [](Brain& self, int steps, float dt = 0.1) {
            for (int i = 0; i < steps; ++i) {
                self.step(i * dt);
            }
        }, py::arg("steps") = 100, py::arg("dt") = 0.1,
           "Run multiple simulation steps efficiently")
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
        .def("reset", [](Brain& self) {
            self.reset();
        }, "Reset brain state to initial conditions")
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
        .def("processSensoryInputs", [](AgentBrain& agent, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                agent.processSensoryInput(percept);
            }
        }, py::arg("percepts"), "Process multiple sensory percepts at once")
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
        .def("reset", [](AgentBrain& agent) {
            agent.reset();
        }, "Reset agent for new episode")
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
        .def("enableAllFeatures", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all neuromodulation and plasticity features")
        .def("disableAllFeatures", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all neuromodulation and plasticity features")
        .def("runEpisode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, int steps, float dt = 0.1) {
            for (int i = 0; i < steps; ++i) {
                auto percept = world->getSensoryPercept();
                agent.processSensoryInput(percept);
                world->update(dt);
                auto action = agent.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                agent.applyRewardModulation(0.0f, 0.0f); // Simple reward
            }
        }, py::arg("world"), py::arg("steps"), py::arg("dt") = 0.1,
           "Run a complete episode through the world")
        .def("getAgentState", [](const AgentBrain& agent) {
            py::dict state;
            state["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            state["neuromodulation_level"] = agent.getNeuromodulationLevel();
            state["curiosity_level"] = agent.getCuriosityLevel();
            state["novelty_level"] = agent.getNoveltyLevel();
            state["prediction_error"] = agent.getPredictionError();
            return state;
        }, "Get current agent state as a dictionary")
        .def_static("from_dict", [](const py::dict& config) -> std::shared_ptr<AgentBrain> {
            // This is a limited constructor for demonstration
            // In practice, you'd need more sophisticated configuration
            auto config_ptr = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config_ptr);
            auto agent = std::make_shared<AgentBrain>(brain);
            return agent;
        }, py::arg("config"), "Create agent brain from configuration dictionary");

    // Advanced features for power users
    m.def("export_brain_to_json", [](const Brain& brain, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        py::dict brain_dict;
        brain_dict["total_neuron_count"] = brain.getTotalNeuronCount();
        brain_dict["total_synapse_count"] = brain.getTotalSynapseCount();
        brain_dict["active_neuron_count"] = brain.getActiveNeuronCount();
        brain_dict["firing_neuron_count"] = brain.getFiringNeuronCount();
        brain_dict["average_firing_rate"] = brain.getAverageFiringRate();
        brain_dict["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        brain_dict["total_spike_count"] = brain.getTotalSpikeCount();
        brain_dict["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        auto config = brain.getConfig();
        if (config) {
            brain_dict["config"] = py::object(py::cast(*config));
        }
        
        // Use pybind11's JSON serialization
        std::stringstream stream;
        py::print(stream, brain_dict);
        file << stream.str();
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to JSON file");
    
    m.def("import_brain_from_json", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        
        py::dict data;
        // Simple JSON parsing - in practice you'd use a proper JSON library
        // This is a simplified version for demonstration
        std::string line;
        while (std::getline(file, line)) {
            // Very basic JSON parsing - would need a proper JSON library in production
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 1);
                // Remove quotes and whitespace
                if (key.front() == '"' && key.back() == '"') {
                    key = key.substr(1, key.size() - 2);
                }
                if (value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                // Try to parse numeric values
                try {
                    if (value.find('.') != std::string::npos) {
                        double num = std::stod(value);
                        data[key.c_str()] = num;
                    } else {
                        int num = std::stoi(value);
                        data[key.c_str()] = num;
                    }
                } catch (...) {
                    data[key.c_str()] = value;
                }
            }
        }
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("filepath"), "Import brain state from JSON file");
    
    m.def("profile_simulation", [](const Brain& brain, int steps, float dt = 0.1) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain.step(i * dt);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict profile_data;
        profile_data["steps"] = steps;
        profile_data["duration_ms"] = static_cast<double>(duration.count());
        profile_data["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        profile_data["neuron_count"] = brain.getTotalNeuronCount();
        profile_data["final_spike_count"] = brain.getTotalSpikeCount();
        profile_data["average_firing_rate"] = brain.getAverageFiringRate();
        
        return profile_data;
    }, py::arg("brain"), py::arg("steps"), py::arg("dt") = 0.1,
       "Profile simulation performance and return metrics");
    
    m.def("benchmark_neural_dynamics", [](const Brain& brain, int test_steps = 1000) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Run multiple stepping operations
        for (int i = 0; i < test_steps; ++i) {
            brain.step(i * 0.001f);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        py::dict benchmark_results;
        benchmark_results["test_steps"] = test_steps;
        benchmark_results["total_duration_us"] = static_cast<double>(duration.count());
        benchmark_results["average_step_time_us"] = static_cast<double>(duration.count()) / test_steps;
        benchmark_results["steps_per_second"] = static_cast<double>(test_steps * 1000000) / duration.count();
        benchmark_results["neuron_count"] = brain.getTotalNeuronCount();
        
        return benchmark_results;
    }, py::arg("brain"), py::arg("test_steps") = 1000,
       "Benchmark neural dynamics performance");
    
    m.def("create_research_environment", [](int width = 20, int height = 20, float timestep = 0.1f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setRandomSeed(42);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("timestep") = 0.1f,
       "Create a research-ready environment with typical settings");
    
    m.def("create_experimental_setup", [](const std::string& experiment_name) {
        py::dict setup;
        if (experiment_name == "curiosity_driven_learning") {
            setup["neuron_count"] = 500;
            setup["connection_probability"] = 0.05;
            setup["plasticity_learning_rate"] = 0.005;
            setup["enable_curiosity"] = true;
            setup["enable_reward_modulation"] = true;
            setup["enable_development"] = true;
        } else if (experiment_name == "working_memory") {
            setup["neuron_count"] = 1000;
            setup["connection_probability"] = 0.15;
            setup["plasticity_learning_rate"] = 0.02;
            setup["enable_reward_modulation"] = true;
            setup["enable_structural_plasticity"] = true;
        } else {
            throw std::runtime_error("Unknown experiment type: " + experiment_name);
        }
        return setup;
    }, py::arg("experiment_name"), "Create experimental setup configuration for specific research areas");
    
    m.def("create_episode_environment", [](int width = 15, int height = 15, float timestep = 0.1f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setRandomSeed(123);
        
        // Add some resources and hazards for the episode
        for (int x = 0; x < width; x += 5) {
            for (int y = 0; y < height; y += 5) {
                if ((x + y) % 10 != 0) {  // Leave some paths open
                    auto resource = WorldObject(2.0f, 2.0f, WorldObjectType::Resource, 1.0f, 0.5f);
                    resource.x = x;
                    resource.y = y;
                    world->addObject(resource);
                }
            }
        }
        
        return world;
    }, py::arg("width") = 15, py::arg("height") = 15, py::arg("timestep") = 0.1f,
       "Create an episode environment with typical obstacles and resources");
    
    // Type aliases and utility functions
    m.def("get_available_action_types", []() {
        std::vector<std::string> actions;
        actions.push_back("MoveForward");
        actions.push_back("MoveBackward");
        actions.push_back("TurnLeft");
        actions.push_back("TurnRight");
        actions.push_back("Look");
        actions.push_back("Interact");
        actions.push_back("Wait");
        return actions;
    }, "Get list of available action types");
    
    m.def("get_available_sensory_inputs", []() {
        std::vector<std::string> inputs;
        inputs.push_back("Vision");
        inputs.push_back("Audio");
        inputs.push_back("Touch");
        inputs.push_back("InternalSignals");
        inputs.push_back("Proprioception");
        return inputs;
    }, "Get list of available sensory input types");
}

} // namespace nlm
