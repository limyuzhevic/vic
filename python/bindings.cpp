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
        .property("value", &NeuronId::value)
        .property("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .property("value", &SynapseId::value)
        .property("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .property("value", &RegionId::value)
        .property("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .property("value", &PopulationId::value)
        .property("index", &PopulationId::index)
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
        // Pythonic property-based access
        .def("get_int", &Config::get<int>, py::arg("key"),
             "Get integer value from config")
        .def("get_float", &Config::get<double>, py::arg("key"),
             "Get float value from config")
        .def("get_string", &Config::get<std::string>, py::arg("key"),
             "Get string value from config")
        .def("get_bool", &Config::get<bool>, py::arg("key"),
             "Get boolean value from config")
        .def("get_optional", [](const Config& self, const std::string& key) {
            return py::none();
        }, py::arg("key"),
             "Get optional value (returns None if key doesn't exist)")
        // Pythonic set methods with type checking
        .def("set_int", &Config::set<int>, py::arg("key"), py::arg("value"),
             "Set integer value in config")
        .def("set_float", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set float value in config")
        .def("set_string", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set string value in config")
        .def("set_bool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set boolean value in config")
        // Convenience methods
        .def("get_or", [](Config& self, const std::string& key, int default_val) {
            return self.get_or<int>(key, default_val);
        }, py::arg("key"), py::arg("default_val"),
             "Get value or return default if key doesn't exist")
        .def("get_or", [](Config& self, const std::string& key, double default_val) {
            return self.get_or<double>(key, default_val);
        }, py::arg("key"), py::arg("default_val"),
             "Get value or return default if key doesn't exist")
        .def("get_or", [](Config& self, const std::string& key, const std::string& default_val) {
            return self.get_or<std::string>(key, default_val);
        }, py::arg("key"), py::arg("default_val"),
             "Get value or return default if key doesn't exist")
        .def("validate", [](Config& self) {
            // Basic validation - can be extended
            return true;
        }, "Validate configuration");

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
        .property("width", &Vision::getWidth)
        .property("height", &Vision::getHeight)
        .property("channels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .property("sampleRate", &Audio::getSampleRate)
        .property("numSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .property("type", &Action::getType, &Action::setType)
        .property("parameters", &Action::getParameters, &Action::setParameters)
        .def("getName", &Action::getName)
        .def("getParameterCount", &Action::getParameterCount)
        .def("getParameter", &Action::getParameter, py::arg("index"))
        .def("setParameter", &Action::setParameter, py::arg("index"), py::arg("value"))
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .property("x", &WorldObject::x)
        .property("y", &WorldObject::y)
        .property("radius", &WorldObject::radius)
        .property("type", &WorldObject::type)
        .property("value", &WorldObject::value)
        .property("active", &WorldObject::active)
        .def("getDistance", &WorldObject::getDistance, py::arg("other"),
             "Get distance to another world object");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .property("x", &AgentBody::x)
        .property("y", &AgentBody::y)
        .property("orientation", &AgentBody::orientation)
        .property("velocity_x", &AgentBody::velocityX)
        .property("velocity_y", &AgentBody::velocityY)
        .property("angular_velocity", &AgentBody::angularVelocity)
        .property("energy", &AgentBody::energy)
        .property("health", &AgentBody::health)
        .property("age", &AgentBody::age)
        .property("is_moving", &AgentBody::isMoving)
        .property("is_turning", &AgentBody::isTurning)
        .property("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("getSpeed", &AgentBody::getSpeed, "Get current speed")
        .def("getDirection", &AgentBody::getDirection, "Get current direction")
        .def("move", &AgentBody::move, py::arg("dx"), py::arg("dy"),
             "Move agent by given offsets");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .property("reward", &ActionResult::reward)
        .property("success", &ActionResult::success)
        .property("message", &ActionResult::message)
        .def("is_successful", &ActionResult::success, "Check if action was successful");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .property("vision_width", &SensoryPercept::getVisionWidth)
        .property("vision_height", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp);

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
        .property("width", &SimpleWorld::getWidth)
        .property("height", &SimpleWorld::getHeight)
        .property("simulation_time", &SimpleWorld::getSimulationTime)
        .property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy)
        .property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate)
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"));

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<const Brain&>(), py::arg("other"))
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
        // Context manager support
        .def("__enter__", [](Brain& self) -> Brain* { return &self; })
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        });

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

    // ===== IMPROVED HIGH-LEVEL UTILITY FUNCTIONS =====
    
    // Safe brain loading with better error handling
    m.def("safe_load_brain", [](const std::string& filepath, std::shared_ptr<Config> config = nullptr) {
        auto brain = createBrain(config ? config : createDefaultConfig());
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain from " + filepath);
        }
        return brain;
    }, py::arg("filepath"), py::arg("config") = nullptr,
       "Safely load brain with error handling. Returns the loaded brain or throws on failure.");
    
    // Save brain with validation
    m.def("save_brain_safe", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain safely with validation. Returns True on success, False on failure.");
    
    // Enhanced configuration utilities
    m.def("create_config_from_dict", [](const py::dict& config_dict) -> std::shared_ptr<Config> {
        auto config = createDefaultConfig();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else {
                throw std::runtime_error("Unsupported config type for key: " + key);
            }
        }
        return config;
    }, py::arg("config_dict"), "Create a config from a Python dictionary");
    
    // Run a complete simulation episode with progress tracking
    m.def("run_simulation_episode", [](std::shared_ptr<Brain> brain,
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      int max_steps,
                                      std::function<double()> reward_fn) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Brain, world, and agent must not be null");
        }
        
        agent->initialize(world);
        double total_reward = 0.0;
        
        for (int step = 0; step < max_steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = reward_fn ? reward_fn() : 0.0;
            total_reward += reward;
            agent->applyRewardModulation(reward, total_reward * 0.9);
            
            agent->updateDevelopment(0.1);
        }
        
        return total_reward;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
       py::arg("max_steps") = 1000, py::arg("reward_fn") = nullptr,
       "Run a complete simulation episode. Returns total reward accumulated.");
    
    // Convenience function for creating complete agent setup
    m.def("create_agent_system", [](std::shared_ptr<Config> config,
                                   std::shared_ptr<SimpleWorld> world,
                                   bool enable_learning = true,
                                   bool enable_development = true,
                                   bool enable_curiosity = false) {
        auto brain = createBrain(config);
        brain->initialize();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
        }
        if (enable_development) {
            agent->enableDevelopment(true);
        }
        if (enable_curiosity) {
            agent->enableCuriosity(true);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"), py::arg("enable_learning") = true,
       py::arg("enable_development") = true, py::arg("enable_curiosity") = false,
       "Create a complete agent system with brain, agent, and world. Returns tuple (brain, agent, world).");
    
    // Batch operations for performance
    m.def("batch_brain_steps", [](std::shared_ptr<Brain> brain, int num_steps) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
        }
    }, py::arg("brain"), py::arg("num_steps"),
       "Execute multiple brain steps efficiently.");
    
    // Configuration validation and schema
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Config is null");
        }
        
        // Basic validation checks
        bool is_valid = true;
        std::string errors;
        
        if (config->has("brain.neuron_count")) {
            auto neuron_count = config->get<size_t>("brain.neuron_count");
            if (neuron_count < 100) {
                errors += "Neuron count should be >= 100. ";
                is_valid = false;
            }
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->get<double>("brain.synapse_density");
            if (density <= 0.0 || density >= 1.0) {
                errors += "Synapse density must be between 0 and 1. ";
                is_valid = false;
            }
        }
        
        return std::make_pair(is_valid, errors);
    }, py::arg("config"),
       "Validate configuration. Returns (is_valid, error_messages).");

// ===== CUSTOM NLM EXCEPTION CLASSES =====
    
    // Base NLM exception
    py::class_<std::runtime_error>(m, "NLMError", py::module_local(),
        "Base exception for NLM errors")
        .def(py::init<const std::string&>());
    
    // Configuration-specific exception
    py::class_<std::runtime_error>(m, "ConfigError", py::module_local(),
        "Configuration error")
        .def(py::init<const std::string&>());
    
    // Brain-specific exception
    py::class_<std::runtime_error>(m, "BrainError", py::module_local(),
        "Brain operation error")
        .def(py::init<const std::string&>());
    
    // World-specific exception
    py::class_<std::runtime_error>(m, "WorldError", py::module_local(),
        "World operation error")
        .def(py::init<const std::string&>());
    
    // Simulation-specific exception
    py::class_<std::runtime_error>(m, "SimulationError", py::module_local(),
        "Simulation operation error")
        .def(py::init<const std::string&>());

    // ===== ENHANCED ERROR HANDLING UTILITIES =====
    
    // Safe configuration creation with validation
    m.def("create_config_safe", [](py::kwargs kwargs) {
        auto config = createDefaultConfig();
        
        for (auto& item : kwargs) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else {
                throw ConfigError("Unsupported config type for key: " + key);
            }
        }
        
        return config;
    }, "Create configuration with validation from keyword arguments");
    
    // Safe brain operations with error checking
    m.def("safe_initialize", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw BrainError("Brain is null");
        }
        if (!brain->initialize()) {
            throw BrainError("Brain initialization failed");
        }
        return true;
    }, py::arg("brain"), "Safely initialize brain. Returns True on success, throws on failure.");
    
    // Safe world operations with validation
    m.def("safe_update_world", [](std::shared_ptr<SimpleWorld> world, double timestep) {
        if (!world) {
            throw WorldError("World is null");
        }
        if (timestep <= 0.0) {
            throw WorldError("Timestep must be positive, got " + std::to_string(timestep));
        }
        world->update(timestep);
        return true;
    }, py::arg("world"), py::arg("timestep"), "Safely update world. Returns True on success, throws on failure.");
    
    // Enhanced simulation with error recovery
    m.def("run_simulation_with_recovery", [](std::shared_ptr<Brain> brain,
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent,
                                             int max_steps,
                                             std::function<double()> reward_fn,
                                             bool continue_on_error = true,
                                             std::function<void(const std::string&)> error_handler = nullptr) {
        if (!brain || !world || !agent) {
            throw SimulationError("Brain, world, and agent must not be null");
        }
        
        agent->initialize(world);
        double total_reward = 0.0;
        int error_count = 0;
        
        for (int step = 0; step < max_steps; ++step) {
            try {
                world->update(0.1);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                double reward = reward_fn ? reward_fn() : 0.0;
                total_reward += reward;
                agent->applyRewardModulation(reward, total_reward * 0.9);
                agent->updateDevelopment(0.1);
                
            } catch (const std::exception& e) {
                std::string error_msg = std::string("Step " + std::to_string(step) + ": " + e.what());
                error_count++;
                
                if (error_handler) {
                    error_handler(error_msg);
                }
                
                if (!continue_on_error) {
                    throw SimulationError("Simulation failed and error recovery is disabled");
                }
                
                // Continue simulation despite errors
                continue;
            }
        }
        
        return std::make_tuple(total_reward, error_count);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("max_steps") = 1000, py::arg("reward_fn") = nullptr,
       py::arg("continue_on_error") = true, py::arg("error_handler") = nullptr,
       "Run simulation with error recovery. Returns (total_reward, error_count).");
    
    // State backup and recovery utilities
    m.def("backup_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw BrainError("Brain is null");
        }
        
        bool success = brain->save(filepath);
        if (!success) {
            throw BrainError("Failed to save brain state to " + filepath);
        }
        
        return filepath;
    }, py::arg("brain"), py::arg("filepath"), "Backup brain state. Returns filepath on success, throws on failure.");
    
    // Enhanced configuration validation
    m.def("validate_config_strict", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw ConfigError("Config is null");
        }
        
        std::vector<std::string> errors;
        
        // Validate required parameters
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing required parameter: brain.neuron_count");
        } else {
            auto neuron_count = config->get<size_t>("brain.neuron_count");
            if (neuron_count < 100) {
                errors.push_back("Neuron count must be >= 100, got " + std::to_string(neuron_count));
            }
        }
        
        if (!config->has("brain.synapse_density")) {
            errors.push_back("Missing required parameter: brain.synapse_density");
        } else {
            auto density = config->get<double>("brain.synapse_density");
            if (density <= 0.0 || density >= 1.0) {
                errors.push_back("Synapse density must be between 0 and 1, got " + std::to_string(density));
            }
        }
        
        return errors;
    }, py::arg("config"), "Strictly validate configuration. Returns list of errors.");
    
    // Configuration validation with exception
    m.def("validate_config_or_throw", [](std::shared_ptr<Config> config) {
        auto errors = validate_config_strict(config);
        if (!errors.empty()) {
            std::string error_msg = "Configuration validation failed: ";
            for (const auto& error : errors) {
                error_msg += error + "; ";
            }
            throw ConfigError(error_msg);
        }
        return true;
    }, py::arg("config"), "Validate configuration and throw ConfigError on invalid configuration.");
    
    // Batch operations with error handling
    m.def("batch_brain_steps_safe", [](std::shared_ptr<Brain> brain, int num_steps,
                                     std::function<void(const std::string&)> error_handler = nullptr) {
        if (!brain) {
            throw BrainError("Brain is null");
        }
        
        for (int i = 0; i < num_steps; ++i) {
            try {
                brain->step(i);
            } catch (const std::exception& e) {
                std::string error_msg = std::string("Step " + std::to_string(i) + ": " + e.what());
                if (error_handler) {
                    error_handler(error_msg);
                } else {
                    throw;
                }
            }
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("error_handler") = nullptr,
       "Execute multiple brain steps with error handling.");
    
    // Environment setup utility with error handling
    m.def("setup_agent_environment", [](std::shared_ptr<Config> config,
                                       std::shared_ptr<SimpleWorld> world,
                                       bool setup_learning = true,
                                       bool setup_development = true,
                                       bool setup_curiosity = false) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        if (!brain->initialize()) {
            throw BrainError("Failed to initialize brain");
        }
        
        agent->initialize(world);
        
        if (setup_learning) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
        }
        if (setup_development) {
            agent->enableDevelopment(true);
        }
        if (setup_curiosity) {
            agent->enableCuriosity(true);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"), py::arg("setup_learning") = true,
       py::arg("setup_development") = true, py::arg("setup_curiosity") = false,
       "Set up complete agent environment. Returns tuple (brain, agent, world).");

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

    // ===== EXPORT CONSTANT VALUES =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
