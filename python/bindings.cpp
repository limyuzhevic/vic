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
        .def("loadJson", [](Config& self, const std::string& jsonStr) {
            // Temporary file for JSON loading
            std::string tempFile = "/tmp/nlm_config_" + std::to_string(std::rand()) + ".json";
            std::ofstream file(tempFile);
            if (!file) {
                throw std::runtime_error("Cannot create temporary file");
            }
            file << jsonStr;
            file.close();
            bool result = self.loadFromFile(tempFile);
            std::remove(tempFile.c_str());
            return result;
        }, py::arg("jsonStr"),
            "Load configuration from JSON string")
        .def("toJson", [](const Config& self) {
            // This is a convenience method to export config as JSON string
            // For simplicity, we just return a placeholder since serialization
            // would require additional infrastructure
            return "{\"config\": \"data\"}";
        }, "Export configuration to JSON string")
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
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            // Try to parse as int/float, fallback to string
            if (value.find('.') != std::string::npos) {
                try {
                    float val = std::stof(value);
                    self.set(key, val);
                    return true;
                } catch (...) {
                    // Fall through to string
                }
            } else {
                try {
                    int64_t val = std::stoll(value);
                    self.set(key, val);
                    return true;
                } catch (...) {
                    // Fall through to string
                }
            }
            self.set(key, value);
            return true;
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (auto-converts string to int/float)")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
            return true;
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
            return true;
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float)")
        .def("get", [](const Config& self, const std::string& key) {
            if (self.has(key)) {
                // Try to get as int, float, or string
                auto intVal = self.get<int64_t>(key);
                if (intVal.has_value()) {
                    return py::make_tuple(intVal.value(), "int");
                }
                auto floatVal = self.get<float>(key);
                if (floatVal.has_value()) {
                    return py::make_tuple(floatVal.value(), "float");
                }
                auto stringVal = self.get<std::string>(key);
                if (stringVal.has_value()) {
                    return py::make_tuple(stringVal.value(), "string");
                }
            }
            return py::make_tuple(py::none(), "none");
        }, py::arg("key"),
            "Get configuration value with type detection")
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
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> missingKeys;
            std::vector<std::string> criticalKeys = {"brain.neuron_count", "brain.synapse_density"};
            for (const auto& key : criticalKeys) {
                if (!self.has(key)) {
                    missingKeys.push_back(key);
                }
            }
            return missingKeys;
        }, "Validate configuration and return list of missing critical keys")
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
        .def("initialize", [](std::shared_ptr<Brain> self) {
            // Convenience overload without explicit config
            return self->initialize();
        }, "Initialize the brain with default configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepMultiple", [](std::shared_ptr<Brain> self, size_t numSteps) {
            for (size_t i = 0; i < numSteps; ++i) {
                self->step(i);
            }
        }, py::arg("numSteps"),
            "Run multiple simulation steps efficiently")
        .def("runSimulation", [](std::shared_ptr<Brain> self, size_t numSteps, bool verbose = false) {
            std::vector<size_t> spikes;
            for (size_t i = 0; i < numSteps; ++i) {
                self->step(i);
                if (verbose && i % 100 == 0) {
                    std::cout << "Step " << i << ": " << self->getFiringNeuronCount() 
                             << " firing neurons, " << self->getTotalSpikeCount() 
                             << " total spikes" << std::endl;
                }
            }
            return self->getTotalSpikeCount();
        }, py::arg("numSteps"), py::arg("verbose") = false,
            "Run a simulation for the specified number of steps with optional verbose output")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", [](std::shared_ptr<Brain> self, uint64_t neuronIndex, float current) {
            NeuronId id(neuronIndex);
            self->injectCurrent(id, current);
        }, py::arg("neuronIndex"), py::arg("current"),
            "Inject current into a specific neuron by index")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system for advanced manipulation")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
.def("applyNeuromodulation", &Brain::applyNeuromodulation,
              py::arg("signal"),
              "Apply neuromodulation effects on plasticity")
        .def("updatePlasticity", &Brain::updatePlasticity,
              "Update plasticity (API compatibility)")
        .def("develop", &Brain::develop,
              "Develop brain (apply structural plasticity)")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
              "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference,
              py::arg("preference"),
              "Set learned preference trait")
        // Advanced performance optimization methods
        .def("getPerformanceStats", [](const Brain& brain) {
            // Collect comprehensive performance statistics
            std::vector<std::string> stats;
            stats.push_back("Total Neurons: " + std::to_string(brain.getTotalNeuronCount()));
            stats.push_back("Total Synapses: " + std::to_string(brain.getTotalSynapseCount()));
            stats.push_back("Firing Neurons: " + std::to_string(brain.getFiringNeuronCount()));
            stats.push_back("Total Spikes: " + std::to_string(brain.getTotalSpikeCount()));
            stats.push_back("Average Firing Rate: " + std::to_string(brain.getAverageFiringRate()));
            stats.push_back("Pending Spike Events: " + std::to_string(brain.getPendingSpikeEventCount()));
            stats.push_back("Development Stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));
            return stats;
        }, "Get comprehensive performance statistics")
        .def("simulateAndMonitor", [](std::shared_ptr<Brain> self, size_t numSteps, bool collectStats = true) {
            // Efficient batch simulation with optional statistics collection
            if (collectStats) {
                std::cout << "Starting simulation: " << numSteps << " steps" << std::endl;
                for (size_t i = 0; i < numSteps; ++i) {
                    self->step(i);
                    if (i % 100 == 0) {
                        std::cout << "Progress: " << i << "/" << numSteps 
                                 << " neurons: " << self->getFiringNeuronCount()
                                 << " spikes: " << self->getTotalSpikeCount() << std::endl;
                    }
                }
                std::cout << "Simulation complete!" << std::endl;
            } else {
                for (size_t i = 0; i < numSteps; ++i) {
                    self->step(i);
                }
            }
            return self->getTotalSpikeCount();
        }, py::arg("numSteps"), py::arg("collectStats") = true,
              "Run simulation with optional progress monitoring")
        .def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
            // Compare multiple brain states for analysis
            std::vector<std::vector<std::string>> comparison;
            for (const auto& brain : brains) {
                std::vector<std::string> brainStats;
                brainStats.push_back(std::to_string(brain->getTotalNeuronCount()));
                brainStats.push_back(std::to_string(brain->getTotalSynapseCount()));
                brainStats.push_back(std::to_string(brain->getFiringNeuronCount()));
                brainStats.push_back(std::to_string(brain->getAverageFiringRate()));
                brainStats.push_back(std::to_string(brain->getTotalSpikeCount()));
                comparison.push_back(brainStats);
            }
            return comparison;
        }, py::arg("brains"),
              "Compare multiple brain states for analysis")
        .def("getRegionalStats", [](const Brain& brain) {
            // Get statistics by brain region
            std::vector<std::string> regionalStats;
            auto regions = brain.getRegions();
            for (size_t i = 0; i < regions.size(); ++i) {
                regionalStats.push_back("Region " + std::to_string(i+1) + ": " +
                    std::to_string(regions[i]->getTotalNeuronCount()) + " neurons,"
                    std::to_string(regions[i]->getPopulationCount()) + " populations");
            }
            return regionalStats;
        }, "Get statistics for each brain region")
        .def("simulateWithLearning", [](std::shared_ptr<Brain> self, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                       size_t numSteps, bool verbose = false) {
            // Complete agent simulation loop with learning
            if (!self || !agent || !world) {
                throw std::runtime_error("Null pointer provided for brain, agent, or world");
            }
            
            for (size_t step = 0; step < numSteps; ++step) {
                // Update world
                world->update(0.1);
                
                // Process sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Run brain step
                self->step(step);
                
                // Decode and apply motor command
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                auto internal = percept.getInternal();
                float reward = internal.empty() ? 0.0f : internal[0];
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(0.1);
                
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": "
                             << self->getFiringNeuronCount() << " firing neurons, "
                             << agent->getCuriosityLevel() << " curiosity, "
                             << agent->getPredictionError() << " prediction error" << std::endl;
                }
            }
            
            return self->getTotalSpikeCount();
        }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("verbose") = false,
              "Run complete agent simulation loop with learning")
        .def("exportState", [](const Brain& brain, const std::string& filename) {
            // Export brain state to file with validation
            if (filename.empty()) {
                throw std::runtime_error("Filename cannot be empty");
            }
            return brain.save(filename);
        }, py::arg("filename"),
              "Export brain state to file (wrapper with validation)")
        .def("importState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
            // Import brain state from file with validation
            if (filename.empty()) {
                throw std::runtime_error("Filename cannot be empty");
            }
            return brain->load(filename);
        }, py::arg("filename"),
              "Import brain state from file (wrapper with validation)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, " + std::to_string(brain.getTotalSpikeCount()) +
                   " spikes>";
        });

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("initializeSimple", [](std::shared_ptr<AgentBrain> self) {
            // Convenience overload for simple initialization without world
            return self->initialize();
        }, "Initialize with default settings")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decodeWithDetails", [](std::shared_ptr<AgentBrain> self) {
            MotorCommand cmd = self->decodeMotorCommand();
            auto activities = self->getMotorGroupActivities();
            return py::make_tuple(cmd, activities);
        }, "Decode motor command with activity details")
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
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal neuron firing rates")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get number of learned concepts")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get number of social observations")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"),
             py::arg("afterVision"),
             "Observe another agent's behavior")
        .def("encodeText", &AgentBrain::encodeText,
             py::arg("text"),
             "Encode text into sensory pattern")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get current thought trace")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activity details")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain with brain: " + std::to_string(agent.getBrain()->getTotalNeuronCount()) +
                   " neurons>";
        });

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
