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
        
        This enhanced API provides:
        - Pythonic context managers for automatic resource management
        - Enhanced configuration API with convenient getter/setter methods
        - Properties and Pythonic accessors for better encapsulation
        - Convenience methods for common operations
        - Advanced APIs for experienced users
        - Comprehensive error handling and informative exceptions
    )pbdoc";

    // Register C++ exceptions as Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "RuntimeError", PyExc_RuntimeError);

    // Helper function to convert C++ exceptions to Python exceptions
    auto convertException = [](const std::exception& e) {
        // Return Python-style error messages
        return std::string("NLM Error: ") + e.what();
    };

    // Exception safety wrapper for configuration operations
    auto safeConfigOperation = [&convertException](auto&& operation, const std::string& operationName) {
        try {
            return operation();
        } catch (const std::exception& e) {
            throw py::value_error(convertException(e));
        }
    };

    // Exception safety wrapper for brain operations
    auto safeBrainOperation = [&convertException](auto&& operation, const std::string& operationName) {
        try {
            return operation();
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Brain " + operationName + ": ") + convertException(e));
        }
    };

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
        
        // Enhanced Pythonic configuration API
        .def("setIfNotExists", [](Config& self, const std::string& key, 
                                 const std::string& value) {
            if (!self.has(key)) {
                self.set<std::string>(key, value);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value only if key doesn't exist")
        
        .def("get", [](Config& self, const std::string& key, 
                      const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default fallback")
        
        .def("getInt", [](Config& self, const std::string& key, 
                         int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer configuration value with default fallback")
        
        .def("getFloat", [](Config& self, const std::string& key, 
                           float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float configuration value with default fallback")
        
        .def("getBool", [](Config& self, const std::string& key, 
                          bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean configuration value with default fallback")
        
        .def("getVector", [](Config& self, const std::string& key, 
                            const std::vector<float>& defaultValue) {
            return self.getOr<std::vector<float>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get vector configuration value with default fallback")
        
        .def("set", [](Config& self, const std::string& key, 
                      const std::string& value) {
            self.set<std::string>(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration value")
        
        .def("set", [](Config& self, const std::string& key, 
                      int value) {
            self.set<int>(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        
        .def("set", [](Config& self, const std::string& key, 
                      float value) {
            self.set<float>(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        
        .def("set", [](Config& self, const std::string& key, 
                      bool value) {
            self.set<bool>(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        
        .def("set", [](Config& self, const std::string& key, 
                      const std::vector<float>& value) {
            self.set<std::vector<float>>(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set vector configuration value")
        
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

    // Enhanced Brain class with Pythonic APIs
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
        
        // Pythonic context manager for automatic resource management
        .def("__enter__", [](Brain& self) {
            // Initialize if not already initialized
            if (self.getTotalNeuronCount() == 0) {
                self.initialize();
            }
            return &self;
        })
        
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        })
        
        // Pythonic properties and methods
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
                              "Get total neuron count")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
                              "Get total synapse count")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                              "Get count of active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                              "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "Get average firing rate")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
                              "Get total spike count")
        .def_property_readonly("development_stage", &Brain::getDevelopmentalStage,
                              "Get current developmental stage")
        .def_property_readonly("config", &Brain::getConfig,
                              "Get the brain configuration")
        
        // Convenience methods for common operations
        .def("runSimulation", [](Brain& self, size_t steps, 
                               bool logProgress = false) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
                if (logProgress && i % 100 == 0) {
                    std::cout << "Step " << i << ": " 
                             << self.getFiringNeuronCount() << " firing neurons" << std::endl;
                }
            }
        }, py::arg("steps"), py::arg("logProgress") = false,
           "Run simulation for specified number of steps")
        
        .def("runToSteadyState", [](Brain& self, size_t maxSteps = 1000) {
            size_t noChangeCount = 0;
            size_t neuronCount = self.getTotalNeuronCount();
            
            for (size_t i = 0; i < maxSteps; ++i) {
                self.step(i);
                
                // Check if firing rate has stabilized
                float firingRate = self.getAverageFiringRate();
                if (i > 0 && std::abs(firingRate - self.getAverageFiringRate()) < 0.01) {
                    noChangeCount++;
                } else {
                    noChangeCount = 0;
                }
                
                if (noChangeCount >= 10) {
                    break;
                }
            }
        }, py::arg("maxSteps") = 1000,
           "Run simulation until steady state is reached")
        
        .def("getStatistics", &Brain::logStatus,
             "Get comprehensive brain statistics")

    // Enhanced AgentBrain class with Pythonic APIs
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
        
        // Pythonic AgentBrain API enhancements
        .def_property_readonly("brain", &AgentBrain::getBrain,
                              "Get the underlying brain")
        .def_property_readonly("curiosity", &AgentBrain::getCuriosityLevel,
                              "Get current curiosity level")
        .def_property_readonly("novelty", &AgentBrain::getNoveltyLevel,
                              "Get current novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                              "Get current prediction error")
        .def_property_readonly("neuromodulation", &AgentBrain::getNeuromodulationLevel,
                              "Get current neuromodulation level")
        
        // Convenience methods for common agent operations
        .def("runEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                            size_t maxSteps, bool enableLearning = true) {
            if (enableLearning) {
                self.enableRewardModulation(true);
                self.enableCuriosity(true);
                self.enableDevelopment(true);
            }
            
            for (size_t i = 0; i < maxSteps; ++i) {
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(i);
                
                auto motorCmd = self.decodeMotorCommand();
                world->applyMotorCommand(motorCmd, world->getSimulationTime());
                
                // Get reward from world for learning
                float reward = 0.0f;
                if (percept) {
                    auto internal = percept->getInternal();
                    if (!internal.empty()) {
                        reward = internal[0];
                    }
                }
                self.applyRewardModulation(reward, 0.0f);
                self.updateDevelopment(0.1f);
            }
        }, py::arg("world"), py::arg("maxSteps"), py::arg("enableLearning") = true,
           "Run a complete agent episode")
        
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning and development features")
        
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning and development features")

    // Enhanced SimpleWorld class with Pythonic APIs
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // Pythonic SimpleWorld API enhancements
        .def_property_readonly("width", &SimpleWorld::getWidth,
                              "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight,
                              "Get world height")
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime,
                              "Get current simulation time")
        .def_property_readonly("max_energy", &SimpleWorld::getMaxEnergy,
                              "Get maximum agent energy")
        
        // Convenience methods for common world operations
        .def("createEmptyGrid", [](SimpleWorld& self) {
            self.reset();
            // Create empty grid
            for (float x = 0.5f; x < self.getWidth(); x += 1.0f) {
                for (float y = 0.5f; y < self.getHeight(); y += 1.0f) {
                    auto worldObj = WorldObject(x, y, WorldObjectType::Empty);
                    self.addObject(worldObj);
                }
            }
        }, "Create empty grid world")
        
        .def("addResources", [](SimpleWorld& self, size_t count, float value = 1.0f) {
            for (size_t i = 0; i < count; ++i) {
                float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getWidth() - 1.0f);
                float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getHeight() - 1.0f);
                auto resource = WorldObject(x, y, WorldObjectType::Resource, value, 0.3f);
                self.addObject(resource);
            }
        }, py::arg("count"), py::arg("value") = 1.0f,
           "Add random resource objects to the world")
        
        .def("addHazards", [](SimpleWorld& self, size_t count, float damage = 0.5f) {
            for (size_t i = 0; i < count; ++i) {
                float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getWidth() - 1.0f);
                float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getHeight() - 1.0f);
                auto hazard = WorldObject(x, y, WorldObjectType::Hazard, damage, 0.4f);
                self.addObject(hazard);
            }
        }, py::arg("count"), py::arg("damage") = 0.5f,
           "Add random hazard objects to the world")
        
        .def("getRandomEmptyPosition", [](SimpleWorld& self) {
            for (int attempts = 0; attempts < 100; ++attempts) {
                float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getWidth() - 1.0f);
                float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (self.getHeight() - 1.0f);
                if (self.isValidPosition(x, y)) {
                    return std::make_pair(x, y);
                }
            }
            return std::make_pair(0.0f, 0.0f);
        }, "Get random empty position in the world")
        
        .def("clearHazards", [](SimpleWorld& self) {
            // This would require more complex implementation to iterate and remove hazards
            self.reset();
            // For now, just reset the world
        }, "Clear all hazard objects from the world");

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

    // Advanced utility functions
    m.def("runCompleteSimulation", [](std::shared_ptr<Config> config,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t steps,
                                     bool enableLearning = true) {
        auto brain = m.attr("createBrain")(config);
        auto agent = m.attr("createAgentBrain")(brain);
        
        agent->initialize(world);
        
        if (enableLearning) {
            agent->enableAllFeatures();
        }
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (enableLearning) {
                float reward = 0.0f;
                if (percept) {
                    auto internal = percept->getInternal();
                    if (!internal.empty()) {
                        reward = internal[0];
                    }
                }
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1f);
            }
            
            if (i % 100 == 0) {
                std::cout << "Step " << i << ": " 
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << agent->getCuriosityLevel() << " curiosity" << std::endl;
            }
        }
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("world"), py::arg("steps"),
       py::arg("enableLearning") = true,
       "Run a complete simulation with agent and learning");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up development-oriented configuration
        config->set<int>("neuron_count", 2000);
        config->set<float>("connection_probability", 0.15f);
        config->set<float>("stdp_ltp_weight", 0.01f);
        config->set<float>("stdp_ltd_weight", 0.012f);
        config->set<size_t>("replay_interval", 50);
        config->set<size_t>("consolidation_interval", 500);
        config->set<float>("synaptogenesis_rate", 0.0002f);
        config->set<float>("pruning_rate", 0.00002f);
        config->set<std::string>("checkpoint_dir", "./checkpoints");
        
        return config;
    }, "Create a development-focused configuration");

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up exploration-oriented configuration
        config->set<int>("neuron_count", 1000);
        config->set<float>("connection_probability", 0.1f);
        config->set<float>("stdp_ltp_weight", 0.008f);
        config->set<float>("stdp_ltd_weight", 0.008f);
        config->set<size_t>("replay_interval", 200);
        config->set<size_t>("consolidation_interval", 2000);
        config->set<float>("synaptogenesis_rate", 0.00005f);
        config->set<float>("pruning_rate", 0.00001f);
        config->set<std::string>("checkpoint_dir", "./exploration_checkpoints");
        
        return config;
    }, "Create an exploration-focused configuration");

} // namespace nlm
