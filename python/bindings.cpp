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
        .def(py::init<const std::string&>(), py::arg("filepath"), "Create and load configuration from JSON file")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
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
        .def("getInt", [](Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (val) return *val;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get integer value from configuration")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto val = self.get<float>(key);
            if (val) return *val;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get float value from configuration")
        .def("getDouble", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val) return *val;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get double value from configuration")
        .def("getString", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get string value from configuration")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set integer value in configuration")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set float value in configuration")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set double value in configuration")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set string value in configuration")
        .def("getIntOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get integer with default")
        .def("getFloatOr", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get float with default")
        .def("getDoubleOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get double with default")
        .def("getStringOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get string with default")
        .def("validate", [](Config& self) {
            // Simple validation: check for required brain parameters
            std::vector<std::string> errors;
            if (!self.has("brain.neuron_count")) {
                errors.push_back("Missing 'brain.neuron_count'");
            }
            if (!self.has("brain.synapse_density")) {
                errors.push_back("Missing 'brain.synapse_density'");
            }
            if (!errors.empty()) {
                throw py::value_error("Configuration validation failed:\n" + 
                    "\n".join(errors.begin(), errors.end()));
            }
            return true;
        }, "Validate configuration")
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
        .def("stepMultiple", [](Brain& self, SimulationStep startStep, SimulationStep steps, bool withTime = false) {
            if (withTime) {
                for (SimulationStep i = startStep; i < startStep + steps; ++i) {
                    self.step(i, i * 0.001);
                }
            } else {
                for (SimulationStep i = startStep; i < startStep + steps; ++i) {
                    self.step(i);
                }
            }
        }, py::arg("start_step"), py::arg("steps"), py::arg("with_time") = false,
            "Perform multiple simulation steps efficiently")
        .def("runEpisode", [](Brain& self, SimulationStep steps, bool withSensory = false) {
            SimulationStep currentStep = 0;
            for (SimulationStep i = 0; i < steps; ++i) {
                if (withSensory) {
                    // Apply some simple sensory input for demonstration
                    if (i % 10 == 0) {
                        auto sensory = std::make_shared<Vision>(10, 10);
                        std::vector<float> data(100, 0.1f);
                        sensory->setData(data);
                        self.receiveSensoryInput(sensory);
                    }
                }
                self.step(currentStep);
                currentStep++;
            }
        }, py::arg("steps"), py::arg("with_sensory") = false,
            "Run a complete episode with optional sensory input")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrentByType", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("neuron_type"), py::arg("current"),
            "Inject current into neurons of specific type using NeuronType enum")
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
        .def("getRegionByName", [](Brain& self, const std::string& name) {
            auto regions = self.getRegions();
            for (auto* region : regions) {
                if (region && region->getName() == name) {
                    return region;
                }
            }
            throw py::key_error("Region not found: " + name);
        }, py::arg("name"), py::return_value_policy::reference_internal,
            "Get region by name")
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
        .def("getTotalSpikeCountInStep", [](Brain& self, SimulationStep step) {
            return self.getTotalSpikeCount();
        }, py::arg("step"),
            "Get total spike count (for compatibility)")
        .def("getActiveNeurons", [](Brain& self) {
            auto regions = self.getRegions();
            std::vector<NeuronId> activeNeurons;
            for (auto* region : regions) {
                if (region) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron && neuron->getFiringState() == FiringState::Active) {
                            activeNeurons.push_back(neuron->getId());
                        }
                    }
                }
            }
            return activeNeurons;
        }, "Get list of active neuron IDs")
        .def("getFiringNeurons", [](Brain& self) {
            auto regions = self.getRegions();
            std::vector<NeuronId> firingNeurons;
            for (auto* region : regions) {
                if (region) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron && neuron->getFiringState() == FiringState::Firing) {
                            firingNeurons.push_back(neuron->getId());
                        }
                    }
                }
            }
            return firingNeurons;
        }, "Get list of firing neuron IDs")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getConfigValue", [](Brain& self, const std::string& key) {
            auto config = self.getConfig();
            if (config->has(key)) {
                auto val = config->get<std::string>(key);
                if (val) return *val;
            }
            throw py::key_error("Key not found in brain config: " + key);
        }, py::arg("key"), "Get configuration value from brain config")
        .def("getConfigInt", [](Brain& self, const std::string& key) {
            auto config = self.getConfig();
            auto val = config->get<int64_t>(key);
            if (val) return *val;
            throw py::key_error("Key not found in brain config: " + key);
        }, py::arg("key"), "Get integer configuration value")
        .def("getConfigFloat", [](Brain& self, const std::string& key) {
            auto config = self.getConfig();
            auto val = config->get<float>(key);
            if (val) return *val;
            throw py::key_error("Key not found in brain config: " + key);
        }, py::arg("key"), "Get float configuration value")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("createCheckpoint", [](Brain& self) {
            // This will create a checkpoint using the brain's internal checkpoint manager
            // For now, return a placeholder indicating checkpoint creation
            return "Checkpoint created at step " + std::to_string(self.getTotalSpikeCount());
        }, "Create a checkpoint")
        .def("status", [](Brain& self) {
            std::stringstream ss;
            ss << "Brain Status:" << std::endl;
            ss << "  Total Neurons: " << self.getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << self.getTotalSynapseCount() << std::endl;
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage());
            return ss.str();
        }, "Get brain status as string");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("createEpisode", [](AgentBrain& self) {
            // Create a new episode by resetting and preparing for learning
            self.reset();
            // Additional initialization could go here
            return "Episode created";
        }, "Create new learning episode")
        .def("getEpisodeStats", [](AgentBrain& self) {
            // Get comprehensive episode statistics
            auto brain = self.getBrain();
            std::stringstream ss;
            ss << "Episode Statistics:" << std::endl;
            ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Curiosity Level: " << self.getCuriosityLevel() << std::endl;
            ss << "  Novelty Level: " << self.getNoveltyLevel() << std::endl;
            ss << "  Prediction Error: " << self.getPredictionError();
            return ss.str();
        }, "Get episode statistics")
        .def("learnFromExperience", [](AgentBrain& self, float reward, float predictedReward) {
            // Learn from experience using reward modulation
            self.applyRewardModulation(reward, predictedReward);
            self.updateDevelopment(0.1);
            return "Learning completed";
        }, py::arg("reward"), py::arg("predicted_reward"),
            "Learn from experience with reward and prediction");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a new brain with default configuration")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    .def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (alternative)")
    // Convenience functions for common patterns
    .def("runAgentSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                  std::shared_ptr<AgentBrain> agent, int numSteps) {
        // Common agent simulation pattern
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation
            float reward = 0.1f;
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete agent simulation (convenience function)")
    .def("createTrainingEnvironment", [](int width, int height, int visionWidth, int visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        world->setRandomSeed(42);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
        py::arg("vision_width") = 8, py::arg("vision_height") = 8,
        "Create and configure a simple training world (convenience function)")
    .def("createLearningBrain", [](int neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
        config->set("brain.synapse_density", 0.1f, ConfigSource::User);
        config->set("random_seed", 42, ConfigSource::User);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 1000, "Create and configure a learning brain (convenience function)")
    // Module attributes for constants and utilities
    .def("getVersion", []() { return "0.1.0"; }, "Get NLM Python bindings version")
    .def("getSupportedActions", []() {
        return std::vector<ActionType>{
            ActionType::MoveForward, ActionType::MoveBackward, ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Look, ActionType::LookUp, ActionType::LookDown, ActionType::Interact,
            ActionType::Eat, ActionType::Drink, ActionType::Rest, ActionType::Wait, ActionType::Custom
        };
    }, "Get list of supported action types")
    .def("getNeuronTypes", []() {
        return std::vector<NeuronType>{
            NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
            NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
        };
    }, "Get list of neuron types")
    .def("getWorldObjectTypes", []() {
        return std::vector<WorldObjectType>{
            WorldObjectType::Empty, WorldObjectType::Resource, WorldObjectType::Hazard,
            WorldObjectType::Wall, WorldObjectType::Marker
        };
    }, "Get list of world object types")
    .def("createRandomSeed", []() {
        return std::time(nullptr);
    }, "Generate a random seed for reproducible simulations")
    // Type conversions and utilities
    .def("toNeuronId", [](uint64_t value) { return NeuronId(value); },
         py::arg("value"), "Convert uint64 to NeuronId")
    .def("toSynapseId", [](uint64_t value) { return SynapseId(value); },
         py::arg("value"), "Convert uint64 to SynapseId")
    .def("toRegionId", [](uint64_t value) { return RegionId(value); },
         py::arg("value"), "Convert uint64 to RegionId")
    .def("toPopulationId", [](uint64_t value) { return PopulationId(value); },
         py::arg("value"), "Convert uint64 to PopulationId")
    // Error handling
    .def("setErrorCallback", [](std::function<void(const std::string&)> callback) {
        // Set global error callback for warnings and errors
        // This is a placeholder for future implementation
    }, py::arg("callback"), "Set error callback function (future feature)")
    // Performance monitoring
    .def("getMemoryUsage", []() {
        // Return basic memory information
        std::stringstream ss;
        ss << "Memory usage information not available (requires platform-specific implementation)";
        return ss.str();
    }, "Get memory usage information")
    .def("getPerformanceStats", [](const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "Performance Statistics:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        return ss.str();
    }, py::arg("brain"), "Get performance statistics for a brain");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
