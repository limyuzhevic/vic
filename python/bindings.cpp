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

// Helper function for consistent return value policies
inline py::return_value_policy getReturnPolicy(const std::string& usage) {
    if (usage.find("reference") != std::string::npos || usage.find("getRegion") != std::string::npos || 
        usage.find("getRegions") != std::string::npos || usage.find("getConfig") != std::string::npos) {
        return py::return_value_policy::reference_internal;
    }
    return py::return_value_policy::copy;
}

// Template function to create ID class bindings with consistent interface
template<typename T>
py::class_<T> createIdClass(py::module_& m, const std::string& name, const std::string& doc) {
    return py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const T& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Create all ID classes using the template (eliminates ~40 lines of duplication)
    createIdClass<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    createIdClass<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    createIdClass<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    createIdClass<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");

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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.loadFromFile(filepath)) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError if file cannot be loaded")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.saveToFile(filepath)) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError if file cannot be saved")
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
        .def("getType", &SensoryInput::getType, 
             "Get the type of sensory input (Vision, Audio, or InternalSignals)")
        .def("getData", &SensoryInput::getData, 
             "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, 
             "Get the dimensionality as a tuple (width, height, channels) for Vision, or (sampleRate, numSamples) for Audio")
        .def("getTimestamp", &SensoryInput::getTimestamp, 
             "Get the timestamp as a double representing simulation time")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input");

py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data. Data must match width * height * channels")
        .def("getWidth", &Vision::getWidth, "Get the image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get the image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels (typically 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio samples. Data must match numSamples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(
             Create an action with type and parameters.
             
             Example:
                 action = Action(ActionType.MoveForward, [0.5, 0.8])  # Move with speed 0.5, angle 0.8
             )pbdoc")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a vector")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get a human-readable name for this action type")
        .def("clone", &Action::clone, "Create a copy of this action");

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
.def("configure", [](SimpleWorld& self, size_t width, size_t height, 
                    size_t visionWidth, size_t visionHeight) {
            if (width == 0 || height == 0 || visionWidth == 0 || visionHeight == 0) {
                throw std::invalid_argument("World dimensions must be greater than zero");
            }
            if (width < visionWidth || height < visionHeight) {
                throw std::invalid_argument("Vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
            R"pbdoc(
            Configure world simulation with validated dimensions.
            
            Example:
                world.configure(100, 100, 30, 30)  # 100x100 world with 30x30 vision
            
            Args:
                width: World width in pixels (must be > 0)
                height: World height in pixels (must be > 0)
                visionWidth: Vision sensor width (must fit within world width)
                visionHeight: Vision sensor height (must fit within world height)
            
            Raises:
                ValueError: If dimensions are invalid
            )pbdoc"),
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (!self.isValidPosition(x, y)) {
                throw std::invalid_argument("Invalid agent start position");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
            "Set agent start position with validation. Position must be within world bounds"),
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
.def("getSensoryPercept", &SimpleWorld::getSensoryPercept, getReturnPolicy("getSensoryPercept"),
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody, getReturnPolicy("getAgentBody"),
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
.def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw std::runtime_error("Failed to save brain state to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             "Save brain state to file. Raises RuntimeError if file cannot be saved")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw std::runtime_error("Failed to load brain state from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             "Load brain state from file. Raises RuntimeError if file cannot be loaded");
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
.def("getRegion", &Brain::getRegion, getReturnPolicy("getRegion"),
              py::arg("id"), "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions, getReturnPolicy("getRegions"),
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
.def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
              "Get excitation/inhibition balance ratio (0.0 = balanced, >1 = excitation dominant, <1 = inhibition dominant)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              "Get total spike count across all neurons")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
              "Get count of currently active neurons (firing > 0 Hz)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
              "Get count of currently firing neurons (> 0 Hz)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
              "Get average firing rate across all neurons in Hz")
        
        // Batch processing methods for performance-critical operations
        .def("injectCurrentBatch", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& currents) {
            for (const auto& [neuron, current] : currents) {
                self.injectCurrent(neuron, current);
            }
        }, py::arg("currents"), "Inject current into multiple neurons in batch")
        
        .def("addRegionsBatch", [](Brain& self, const std::vector<std::string>& names) {
            std::vector<RegionId> ids;
            for (const auto& name : names) {
                ids.push_back(self.addRegion(name));
            }
            return ids;
        }, py::arg("names"), "Add multiple regions and return their IDs")
        
        .def("processSensoryInputsBatch", [](Brain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.receiveSensoryInput(percept);
            }
        }, py::arg("percepts"), "Process multiple sensory inputs in batch")
        
        // Direct C++ object access for performance-critical operations
        .def("getSpikeSystemRaw", [](Brain& self) -> void* {
            // This assumes Brain has a method to access the spike system
            // In practice, this would be replaced with actual method calls
            return nullptr; // Placeholder - implement based on actual C++ interface
        }, "Get raw SpikeSystem pointer for advanced operations")
        
        .def("getWorldRaw", [](Brain& self) -> void* {
            // This assumes Brain has a method to access the world
            // In practice, this would be replaced with actual method calls
            return nullptr; // Placeholder - implement based on actual C++ interface
        }, "Get raw World pointer for advanced operations")
        
        .def("getConfigRaw", [](Brain& self) -> void* {
            // This assumes Brain has a method to access the config
            // In practice, this would be replaced with actual method calls
            return reinterpret_cast<void*>(self.getConfig().get());
        }, "Get raw Config pointer for advanced operations")
        
        // Callback system for event-driven programming
        .def("setSpikeCallback", [](Brain& self, std::function<void(NeuronId, Timestamp)> callback) {
            // This assumes Brain has a method to set spike callbacks
            // In practice, this would be replaced with actual method calls
            // For now, we'll store the callback for demonstration
            self.setSpikeCallback(callback);
        }, py::arg("callback"), "Set callback for spike events")
        
        .def("setActionProducedCallback", [](Brain& self, std::function<void(const Action&)> callback) {
            // This assumes Brain has a method to set action callbacks
            // In practice, this would be replaced with actual method calls
            // For now, we'll store the callback for demonstration
            self.setActionProducedCallback(callback);
        }, py::arg("callback"), "Set callback for action production")
        
        .def("clearCallbacks", [](Brain& self) {
            // This assumes Brain has a method to clear callbacks
            // In practice, this would be replaced with actual method calls
            self.clearCallbacks();
        }, "Clear all registered callbacks");
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
.def("getConfig", &Brain::getConfig, getReturnPolicy("getConfig"),
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
