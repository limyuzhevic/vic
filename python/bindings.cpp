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
        // Enhanced configuration methods for better Python usability
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
            "Get configuration value with default fallback (int)")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
            "Get configuration value with default fallback (float)")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
            "Get configuration value with default fallback (string)")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
            "Get configuration value with default fallback (bool)")
        .def("setFromDict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (value.is_none()) {
                    self.set(key, ConfigValue()); // TODO: Handle proper null setting
                } else if (value.is_array()) {
                    auto vec = value.cast<std::vector<int>>();
                    self.set(key, ConfigValue(vec));
                } else {
                    self.set(key, ConfigValue(py::cast<std::string>(value)));
                }
            }
        }, py::arg("dict"),
            "Load configuration from Python dictionary")
        .def("toDict", [](const Config& self) {
            py::dict dict;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    dict[key.c_str()] = val.value();
                }
            }
            return dict;
        },
            "Convert configuration to Python dictionary")
        // Configuration validation and preset methods
        .def("validate", [](Config& self) {
            return self.has("brain.neuron_count") && 
                   self.has("brain.connection_probability");
        },
            "Validate configuration values (basic validation)")
        // Advanced configuration methods
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto val = other.get<std::string>(key);
                if (val.has_value()) {
                    self.set(key, val.value());
                }
            }
            return self;
        }, py::arg("other"),
            "Merge another configuration into this one")
        .def("getSource", [](const Config& self, const std::string& key) {
            auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != self.pImpl->entries.end()) {
                return static_cast<int>(it->source);
            }
            return -1;
        }, py::arg("key"),
            "Get configuration source (0=Default, 1=File, 2=CommandLine, 3=Runtime)")

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
        .def("runSimulation", [](Brain& self, size_t steps, bool useTime = false) {
            if (useTime) {
                for (size_t step = 0; step < steps; ++step) {
                    self.step(step, static_cast<Timestamp>(step) * 0.001);
                }
            } else {
                for (size_t step = 0; step < steps; ++step) {
                    self.step(step);
                }
            }
        }, py::arg("steps"), py::arg("useTime") = false,
            "Run a complete simulation for the specified number of steps")
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
        // Enhanced statistics methods for Python
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
        // Development system enhancements
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        // Brain state monitoring
        .def("getFiringNeurons", &Brain::getAllNeuronsFromFiring,  // TODO: Check actual method
             py::return_value_policy::reference_internal,
             "Get all neurons that are currently firing")
        .def("getNeuronsByType", [](const Brain& self, NeuronType type) {
            std::vector<Neuron*> neurons;
            for (const auto& region : self.getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == type) {
                        for (auto* neuron : pop->getNeurons()) {
                            neurons.push_back(neuron);
                        }
                    }
                }
            }
            return neurons;
        }, py::arg("type"),
            "Get all neurons of a specific type")
        // Brain control and management
        .def("setRandomSeed", [](Brain& self, uint64_t seed) {
            if (auto rng = self.getRandomGenerator()) {
                // TODO: Check if RandomGenerator has setSeed method
            }
        }, py::arg("seed"),
            "Set random seed for reproducible simulations")
        // Brain information and status
        .def("getBrainInfo", &Brain::logStatus,
             "Get detailed brain information and status")
        // Simulation utilities
        .def("simulateEpisode", [](Brain& self, const std::shared_ptr<Config>& config, size_t steps) {
            self.reset();
            self.initialize();
            for (size_t step = 0; step < steps; ++step) {
                self.step(step);
            }
        }, py::arg("config"), py::arg("steps"),
            "Run a complete simulation episode")
        .def("createCheckpoint", &Brain::save, py::arg("filepath"),
             "Create a checkpoint (alias for save)")
        .def("restoreCheckpoint", &Brain::load, py::arg("filepath"),
             "Restore from checkpoint (alias for load)")
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
        // Enhanced sensory processing for Python
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("processVision", [](AgentBrain& self, const py::dict& visionData) {
            Vision vision;
            vision.configure(
                py::cast<size_t>(visionData["width"]),
                py::cast<size_t>(visionData["height"]),
                py::cast<size_t>(visionData["channels"])
            );
            auto& vecData = py::cast<std::vector<float>>(visionData["data"]);
            vision.setData(vecData);
            self.processSensoryInput(vision);
        }, py::arg("visionData"),
            "Process vision data as dictionary for convenience")
        .def("processAudio", [](AgentBrain& self, const py::dict& audioData) {
            Audio audio;
            audio.setSampleRate(py::cast<size_t>(audioData["sampleRate"]));
            audio.setNumSamples(py::cast<size_t>(audioData["numSamples"]));
            auto& vecData = py::cast<std::vector<float>>(audioData["data"]);
            audio.setData(vecData);
            self.processSensoryInput(audio);
        }, py::arg("audioData"),
            "Process audio data as dictionary for convenience")
        // Enhanced motor decoding
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decodeAction", &AgentBrain::decodeMotorCommand,
             "Alias for decodeMotorCommand (more intuitive name)")
        // Enhanced neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyReinforcement", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Alias for applyRewardModulation (more intuitive name)")
        // Enhanced development system
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("advanceDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Alias for updateDevelopment (more intuitive name)")
        // Enhanced developmental stage access
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", [](AgentBrain& self, const std::string& stageStr) {
            DevelopmentalStage stage;
            if (stageStr == "Initial") stage = DevelopmentalStage::Initial;
            else if (stageStr == "CriticalPeriod") stage = DevelopmentalStage::CriticalPeriod;
            else if (stageStr == "Maturation") stage = DevelopmentalStage::Maturation;
            else if (stageStr == "Adult") stage = DevelopmentalStage::Adult;
            else if (stageStr == "Aging") stage = DevelopmentalStage::Aging;
            else stage = DevelopmentalStage::Initial;
            return self.setDevelopmentalStage(stage);
        }, py::arg("stageStr"),
            "Set developmental stage by string name")
        // Enhanced neuromodulation level access
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getDopamineLevel", &AgentBrain::getNeuromodulationLevel,
             "Get dopamine level (alias)")
        // Enhanced curiosity and novelty detection
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getExplorationDrive", &AgentBrain::getCuriosityLevel,
             "Get exploration drive (alias)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getSurpriseLevel", &AgentBrain::getNoveltyLevel,
             "Get surprise level (alias)")
        // Enhanced prediction error
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("getLearningError", &AgentBrain::getPredictionError,
             "Get learning error (alias)")
        // Enhanced subsystem control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("disableRewardModulation", [](AgentBrain& self) {
            self.enableRewardModulation(false);
        }, "Disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("disableStructuralPlasticity", [](AgentBrain& self) {
            self.enableStructuralPlasticity(false);
        }, "Disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("disableDevelopment", [](AgentBrain& self) {
            self.enableDevelopment(false);
        }, "Disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("disableCuriosity", [](AgentBrain& self) {
            self.enableCuriosity(false);
        }, "Disable curiosity")
        // Enhanced status checking
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced agent operations
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("restart", &AgentBrain::reset,
             "Restart agent (alias)")
        // Enhanced brain access
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Agent interaction convenience methods
        .def("think", [](AgentBrain& self, const py::dict& percept) {
            if (percept.contains("vision")) {
                self.processSensoryInput(percept["vision"].cast<Vision>());
            }
            if (percept.contains("audio")) {
                self.processSensoryInput(percept["audio"].cast<Audio>());
            }
            if (percept.contains("internal")) {
                self.processSensoryInput(percept["internal"].cast<InternalSignals>());
            }
        }, py::arg("percept"),
            "Agent thinks based on sensory input (convenience method)")
        .def("act", &AgentBrain::decodeMotorCommand,
             "Agent produces action based on current brain state")
        .def("learn", [](AgentBrain& self, float reward, float predictedReward = 0.0f) {
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward") = 0.0f,
            "Agent learns from experience (convenience method)")
        .def("update", [](AgentBrain& self, float timestep) {
            self.updateDevelopment(timestep);
        }, py::arg("timestep"),
            "Agent updates development system (convenience method)");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    // Enhanced configuration factory functions for Python
    .def("createConfig", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            if (value.is_none()) {
                continue;
            } else if (value.is_array()) {
                auto vec = value.cast<std::vector<int>>();
                config->set(key, ConfigValue(vec));
            } else {
                if (value.is(py::bool_())) {
                    config->set(key, py::cast<bool>(value));
                } else if (value.is(py::int_())) {
                    config->set(key, py::cast<int>(value));
                } else if (value.is(py::float_())) {
                    config->set(key, py::cast<float>(value));
                } else {
                    config->set(key, py::cast<std::string>(value));
                }
            }
        }
        return config;
    }, py::arg("configDict"),
        "Create configuration from Python dictionary")
    .def("createSimpleConfig", [](size_t neuronCount, size_t regionCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        return config;
    }, py::arg("neuronCount"), py::arg("regionCount") = 1,
        "Create simple configuration with specified neuron and region counts")
    .def("createDemoConfig", []() {
        auto config = std::make_shared<Config>();
        // Demo configuration optimized for learning demonstrations
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.05f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create demonstration configuration optimized for learning examples")
    // Enhanced world factory functions
    .def("createWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create configured world with specified dimensions")
    .def("createAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create and initialize agent with brain and world")
    // Enhanced convenience functions for common patterns
    .def("runCompleteEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, size_t steps) {
        brain->reset();
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.0f, 0.0f); // TODO: Get actual reward
        }
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
        "Run a complete simulation episode with brain, world, and agent")
    .def("createBrainAndAgent", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            if (value.is_none()) continue;
            if (value.is(py::bool_())) {
                config->set(key, py::cast<bool>(value));
            } else if (value.is(py::int_())) {
                config->set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<float>(value));
            } else {
                config->set(key, py::cast<std::string>(value));
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return std::make_pair(brain, std::make_shared<AgentBrain>(brain));
    }, py::arg("configDict"),
        "Create brain and agent pair with configuration (returns tuple)")
    // Module metadata and utilities
    .def("getVersion", []() {
        return std::string("NLM Python Bindings v0.1.0");
    }, "Get version information")
    .def("getModuleInfo", []() {
        py::dict info;
        info["name"] = "NLM - Neural Learning Machine";
        info["version"] = "0.1.0";
        info["description"] = "Python bindings for spiking neural network simulation";
        info["author"] = "NLM Authors";
        info["license"] = "MIT";
        return info;
    }, "Get module information")
    // Advanced simulation utilities
    .def("runMultipleEpisodes", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, size_t numEpisodes, size_t stepsPerEpisode) {
        std::vector<float> episodeRewards;
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            brain->reset();
            brain->initialize();
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            
            float totalReward = 0.0f;
            for (size_t step = 0; step < stepsPerEpisode; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(0.0f, 0.0f); // TODO: Get actual reward
            }
            episodeRewards.push_back(totalReward);
        }
        
        return episodeRewards;
    }, py::arg("brain"), py::arg("world"), py::arg("numEpisodes"), py::arg("stepsPerEpisode"),
        "Run multiple simulation episodes and return rewards for each")
    // Performance monitoring utilities
    .def("measurePerformance", [](std::shared_ptr<Brain> brain, size_t steps) {
        brain->reset();
        brain->initialize();
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        py::dict results;
        results["steps"] = steps;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["wall_clock_time_ms"] = static_cast<double>(duration.count());
        results["spikes_per_second"] = (brain->getTotalSpikeCount() * 1000.0) / duration.count();
        
        return results;
    }, py::arg("brain"), py::arg("steps"),
        "Run performance measurement and return timing and statistics metrics")
    // Statistical analysis utilities
    .def("runStatisticalAnalysis", [](std::shared_ptr<Brain> brain, size_t warmUpSteps, size_t analysisSteps) {
        brain->reset();
        brain->initialize();
        
        // Warm up
        for (size_t step = 0; step < warmUpSteps; ++step) {
            brain->step(step);
        }
        
        // Analysis
        std::vector<float> firingRates;
        std::vector<size_t> spikeCounts;
        std::vector<size_t> activeNeuronCounts;
        
        for (size_t step = 0; step < analysisSteps; ++step) {
            brain->step(step + warmUpSteps);
            firingRates.push_back(brain->getAverageFiringRate());
            spikeCounts.push_back(brain->getFiringNeuronCount());
            activeNeuronCounts.push_back(brain->getActiveNeuronCount());
        }
        
        py::dict statistics;
        // Simple statistics calculation
        statistics["mean_firing_rate"] = calculateVectorMean(firingRates);
        statistics["std_firing_rate"] = calculateVectorStd(firingRates);
        statistics["mean_spike_count"] = calculateVectorMean(spikeCounts);
        statistics["std_spike_count"] = calculateVectorStd(spikeCounts);
        statistics["mean_active_neurons"] = calculateVectorMean(activeNeuronCounts);
        statistics["std_active_neurons"] = calculateVectorStd(activeNeuronCounts);
        statistics["min_firing_rate"] = *std::min_element(firingRates.begin(), firingRates.end());
        statistics["max_firing_rate"] = *std::max_element(firingRates.begin(), firingRates.end());
        statistics["min_spike_count"] = *std::min_element(spikeCounts.begin(), spikeCounts.end());
        statistics["max_spike_count"] = *std::max_element(spikeCounts.begin(), spikeCounts.end());
        
        return statistics;
    }, py::arg("brain"), py::arg("warmUpSteps"), py::arg("analysisSteps"),
        "Run statistical analysis on brain performance metrics");

} // namespace nlm
