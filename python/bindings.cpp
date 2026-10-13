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
        
        // Pythonic property access
        .def("__contains__", &Config::has,
             "Check if key exists (supports 'in' operator)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        
        // Safe get methods with defaults
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            return std::make_tuple(true, value);
        }, py::arg("key"),
           "Get value for key (returns tuple of success and value)")
        .def("get_or", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get value or default if key doesn't exist")
        .def("get_int", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            return std::make_tuple(true, value);
        }, py::arg("key"),
           "Get integer value for key (returns tuple of success and value)")
        .def("get_int_or", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer value or default if key doesn't exist")
        .def("get_float", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            return std::make_tuple(true, value);
        }, py::arg("key"),
           "Get float value for key (returns tuple of success and value)")
        .def("get_float_or", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float value or default if key doesn't exist")
        .def("get_bool", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            return std::make_tuple(true, value);
        }, py::arg("key"),
           "Get boolean value for key (returns tuple of success and value)")
        .def("get_bool_or", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean value or default if key doesn't exist")
        
        // Configuration setters with validation
        .def("set_int", &Config::set, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            return self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            return self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        
        // Configuration validation and info
        .def("validate", [](Config& self) {
            std::string errors;
            bool valid = true;
            
            // Validate common configuration keys
            auto neuronCount = self.getOr<size_t>("neuron_count", 0);
            if (neuronCount == 0) {
                valid = false;
                errors += "neuron_count must be > 0; ";
            } else if (neuronCount > 100000) {
                valid = false;
                errors += "neuron_count must be <= 100000; ";
            }
            
            auto regionCount = self.getOr<size_t>("region_count", 0);
            if (regionCount == 0) {
                valid = false;
                errors += "region_count must be > 0; ";
            } else if (regionCount > 100) {
                valid = false;
                errors += "region_count must be <= 100; ";
            }
            
            auto connProb = self.getOr<float>("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                valid = false;
                errors += "connection_probability must be between 0.0 and 1.0; ";
            }
            
            return std::make_tuple(valid, errors.empty() ? "Configuration is valid" : errors);
        }, "Validate configuration and return (is_valid, error_message)")
        .def("get_config_info", [](Config& self) {
            std::stringstream ss;
            ss << "Configuration Info:" << std::endl;
            ss << "Keys: " << self.getKeys().size() << std::endl;
            ss << "Summary: " << self.summary() << std::endl;
            
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                ss << "  " << key << std::endl;
            }
            
            return ss.str();
        }, "Get detailed configuration information");

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
        
        // Pythonic property access
        .def("region_count", &Brain::getRegionCount,
             "Get the number of regions (Pythonic property access)")
        .def("total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count (Pythonic property access)")
        .def("total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count (Pythonic property access)")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate (Pythonic property access)")
        
        // Convenience methods for common operations
        .def("add_sensory_region", &Brain::addRegion, py::arg("name") = "Sensory",
             "Add a sensory region (convenience method)")
        .def("add_motor_region", &Brain::addRegion, py::arg("name") = "Motor",
             "Add a motor region (convenience method)")
        .def("add_internal_region", &Brain::addRegion, py::arg("name") = "Internal",
             "Add an internal region (convenience method)")
        
        // Batch operations
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of specific type (alias for injectCurrentToNeurons)")
        .def("get_region_by_index", [](const Brain& brain, size_t index) {
            auto ids = brain.getRegionIds();
            if (index < ids.size()) {
                return brain.getRegion(ids[index]);
            }
            return (const NeuralRegion*)nullptr;
        }, py::arg("index"),
             "Get region by index (Pythonic access)")
        .def("get_region_count", &Brain::getRegionCount,
             "Get region count (alias for getRegionCount)")
        .def("get_total_spikes", &Brain::getTotalSpikeCount,
             "Get total spike count (alias for getTotalSpikeCount)")
        
        // Training helpers
        .def("enable_learning", [](Brain& brain, bool enable) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
        }, py::arg("enable") = true,
             "Enable synaptic learning across all synapses")
        .def("set_learning_rate", [](Brain& brain, float rate) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    float currentWeight = syn->getWeight();
                    syn->setWeight(currentWeight * rate);
                }
            }
        }, py::arg("rate"),
             "Scale all synaptic weights by learning rate")
        
        // Simulation helpers
        .def("run_simulation_step", &Brain::step,
             py::arg("currentStep"),
             "Run a single simulation step (convenience method)")
        .def("run_simulation_steps", [](Brain& brain, size_t numSteps) {
            for (size_t i = 0; i < numSteps; ++i) {
                brain.step(i, i * 0.001);
            }
        }, py::arg("numSteps"),
             "Run multiple simulation steps (convenience method)")
        
        // Context manager support for resource management
        .def("__enter__", [](Brain& brain) { return &brain; })
        .def("__exit__", [](Brain& brain, py::args) {
            brain.reset();
        });

    // Pythonic property access for AgentBrain
    .def("get_brain", &AgentBrain::getBrain,
         py::return_value_policy::reference_internal,
         "Get the underlying brain (Pythonic property access)")
    
    // Enable/disable systems with convenience methods
    .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
         py::arg("enable") = true,
         "Enable/disable reward modulation")
    .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable") = true,
         "Enable/disable structural plasticity")
    .def("enable_development", &AgentBrain::enableDevelopment,
         py::arg("enable") = true,
         "Enable/disable development")
    .def("enable_curiosity", &AgentBrain::enableCuriosity,
         py::arg("enable") = true,
         "Enable/disable curiosity system")
    
    // Neuromodulation level queries
    .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
         "Get current neuromodulation level")
    .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
         "Get current curiosity level")
    .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
         "Get current novelty level")
    .def("get_prediction_error", &AgentBrain::getPredictionError,
         "Get current prediction error")
    
    // System state queries
    .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
         "Check if reward modulation is enabled")
    .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
         "Check if curiosity is enabled")
    
    // Convenience methods for common operations
    .def("apply_reward", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predicted_reward"),
         "Apply reward-based neuromodulation (convenience method)")
    .def("reset_agent", &AgentBrain::reset,
         "Reset agent for new episode (convenience method)")
    
    // Training helpers
    .def("train_step", [](AgentBrain& agent, const SensoryPercept& percept) {
        agent.processSensoryInput(percept);
        return agent.getBrain()->produceAction();
    }, py::arg("percept"),
         "Perform a single training step (convenience method)")
    
    .def("episode_train", [](AgentBrain& agent, const std::vector<SensoryPercept>& percepts) {
        agent.reset();
        std::vector<std::unique_ptr<Action>> actions;
        
        for (const auto& percept : percepts) {
            agent.processSensoryInput(percept);
            auto action = agent.decodeMotorCommand();
            if (action) {
                actions.push_back(std::move(action));
            }
        }
        
        return actions;
    }, py::arg("percepts"),
         "Train over an entire episode (convenience method)")
    
    // Context manager support
    .def("__enter__", [](AgentBrain& agent) { return &agent; })
    .def("__exit__", [](AgentBrain& agent, py::args) {
        agent.reset();
    });

    // Convenience functions for creating common brain configurations
    m.def("create_default_brain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");
    
    m.def("create_small_brain", [](size_t neuronCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 100,
       "Create a small brain with specified neuron count");
    
    m.def("create_experimental_brain", [](size_t neuronCount, size_t regionCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 500, py::arg("regionCount") = 1,
       "Create an experimental brain with specified neuron count and regions");
    
    m.def("create_demo_brain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Enable all learning systems for demonstration
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a demo brain with all learning systems enabled");
    
    // Error handling and validation helpers
    m.def("validate_brain_config", [](std::shared_ptr<Config> config) {
        bool valid = true;
        std::string errors;
        
        auto neuronCount = config->getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0) {
            valid = false;
            errors += "neuron_count must be > 0; ";
        } else if (neuronCount > 100000) {
            valid = false;
            errors += "neuron_count must be <= 100000; ";
        }
        
        auto regionCount = config->getOr<size_t>("region_count", 0);
        if (regionCount == 0) {
            valid = false;
            errors += "region_count must be > 0; ";
        } else if (regionCount > 100) {
            valid = false;
            errors += "region_count must be <= 100; ";
        }
        
        auto connProb = config->getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            valid = false;
            errors += "connection_probability must be between 0.0 and 1.0; ";
        }
        
        return std::make_tuple(valid, errors.empty() ? "Configuration is valid" : errors);
    }, py::arg("config"),
       "Validate brain configuration and return validation result");
    
    // Simulation utilities
    m.def("run_brain_simulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
       "Run simulation for specified number of steps and return spike count");
    
    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for benchmarking");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        return std::make_tuple(
            totalSpikes,
            static_cast<double>(steps) / duration.count() * 1000.0,
            duration.count()
        );
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain simulation performance");
    
    // Convenience functions for getting brain statistics
    m.def("get_brain_summary", [](std::shared_ptr<Brain> brain) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for summary");
        }
        
        std::stringstream ss;
        ss << "Brain Summary:" << std::endl;
        ss << "  Regions: " << brain->getRegionCount() << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        return ss.str();
    }, py::arg("brain"),
       "Get comprehensive summary of brain state");
    
    // Utility functions
    m.def("compare_brains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, size_t steps) {
        if (!brain1->initialize() || !brain2->initialize()) {
            throw std::runtime_error("Failed to initialize brains for comparison");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            brain1->step(i, i * 0.001);
            brain2->step(i, i * 0.001);
        }
        
        double spikeRatio = static_cast<double>(brain1->getTotalSpikeCount()) / 
                           static_cast<double>(brain2->getTotalSpikeCount());
        
        return std::make_tuple(
            brain1->getTotalSpikeCount(),
            brain2->getTotalSpikeCount(),
            spikeRatio,
            brain1->getAverageFiringRate() / brain2->getAverageFiringRate()
        );
    }, py::arg("brain1"), py::arg("brain2"), py::arg("steps"),
       "Compare two brains over specified steps");
}

} // namespace nlm
