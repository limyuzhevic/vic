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
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with Pythonic factory methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        // Simplified constructors with default parameters
        .def(py::init<>(), "Create empty vision input")
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        
        // Pythonic factory methods
        .def_static("from_image", [](size_t width, size_t height, size_t channels = 3) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
            "Create vision input from image dimensions (Pythonic factory)")
        .def_static("from_data", [](const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            vision.setData(data);
            return vision;
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
            "Create vision input from image data (Pythonic factory)")
        .def_static("grayscale", [](size_t width, size_t height) {
            return Vision(width, height, 1);
        }, py::arg("width"), py::arg("height"), "Create grayscale vision input")
        .def_static("rgb", [](size_t width, size_t height) {
            return Vision(width, height, 3);
        }, py::arg("width"), py::arg("height"), "Create RGB vision input")
        .def_static("rgba", [](size_t width, size_t height) {
            return Vision(width, height, 4);
        }, py::arg("width"), py::arg("height"), "Create RGBA vision input")
        
        // Enhanced property access with validation
        .def("get_width", &Vision::getWidth, "Get image width")
        .def("get_height", &Vision::getHeight, "Get image height")
        .def("get_channels", &Vision::getChannels, "Get number of color channels")
        .def("is_grayscale", [](const Vision& self) {
            return self.getChannels() == 1;
        }, "Check if image is grayscale")
        .def("is_rgb", [](const Vision& self) {
            return self.getChannels() == 3;
        }, "Check if image is RGB")
        .def("is_rgba", [](const Vision& self) {
            return self.getChannels() == 4;
        }, "Check if image is RGBA")
        
        // Data management with validation
        .def("set_data", &Vision::setData, py::arg("data"),
             "Set image data with validation")
        .def("set_data_bytes", [](Vision& self, const py::bytes& data, size_t width, size_t height, size_t channels) {
            auto buffer = py::buffer(data);
            auto ptr = buffer.request();
            if (ptr.size != width * height * channels * sizeof(float)) {
                throw py::value_error("Data size mismatch for specified dimensions");
            }
            std::vector<float> vec(static_cast<const float*>(ptr.ptr), 
                                 static_cast<const float*>(ptr.ptr) + ptr.size / sizeof(float));
            self.setData(vec);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels"),
           "Set image data from bytes with validation")
        .def("get_data", &Vision::getData, "Get image data as vector")
        .def("clone", &Vision::clone, "Clone the vision input");

    // Enhanced Audio class with Pythonic factory methods
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        // Simplified constructors with default parameters
        .def(py::init<>(), "Create empty audio input")
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and duration")
        
        // Pythonic factory methods
        .def_static("from_samples", [](size_t sampleRate, size_t numSamples) {
            return Audio(sampleRate, numSamples);
        }, py::arg("sampleRate"), py::arg("numSamples"),
            "Create audio input from sample rate and number of samples (Pythonic factory)")
        .def_static("from_samples_with_data", [](size_t sampleRate, size_t numSamples, const std::vector<float>& data) {
            Audio audio(sampleRate, numSamples);
            if (data.size() != numSamples) {
                throw py::value_error("Data size mismatch for specified number of samples");
            }
            audio.setData(data);
            return audio;
        }, py::arg("sampleRate"), py::arg("numSamples"), py::arg("data"),
            "Create audio input from sample rate, duration, and audio data (Pythonic factory)")
        .def_static("create_default_audio", [](double duration, double sampleRate = 44100.0) {
            size_t numSamples = static_cast<size_t>(duration * sampleRate);
            return Audio(static_cast<size_t>(sampleRate), numSamples);
        }, py::arg("duration"), py::arg("sampleRate") = 44100.0,
            "Create audio input with default sample rate for given duration")
        
        // Enhanced property access
        .def("get_sample_rate", &Audio::getSampleRate, "Get audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of audio samples")
        .def("get_duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("get_data", &Audio::getData, "Get audio data as vector")
        .def("set_data", &Audio::setData, py::arg("data"),
             "Set audio data with validation")
        .def("clone", &Audio::clone, "Clone the audio input");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

        // Builder pattern fluent interface with validation
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("with_parameters", [](Action& self, const std::vector<float>& params) {
            self.setParameters(params);
            return self;
        }, py::arg("params"), "Set parameters and return self for chaining")
        .def("move_forward", []() {
            Action action;
            action.setType(ActionType::MoveForward);
            return action;
        }, "Create move forward action (alternative)")
        .def("turn_left", []() {
            Action action;
            action.setType(ActionType::TurnLeft);
            return action;
        }, "Create turn left action (alternative)")
        
        // Properties with validation
        .def("get_type", &Action::getType, "Get action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("get_name", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone the action")
        .def("__repr__", [](const Action& a) {
            return "<Action type: " + a.getName() + ">";
        });

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
        .def("getRegion", [](const Brain& self, const RegionId& id) {
            auto region = self.getRegion(id);
            return region;
        }, py::arg("id"),
             "Get a region by ID (returns smart pointer)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (returns shared_ptr)")
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
        .def("getBrain", [](const AgentBrain& self) {
            return self.getBrain();
        }, py::return_value_policy::reference_internal,
             "Get the underlying brain (returns smart pointer)")
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
    }, "Create a default configuration")

    // Enhanced factory methods with validation and error handling
    m.def("createBrainFromConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain from configuration object")

    m.def("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to create brain from file: " + filepath + " - file not found or invalid format");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"), "Create a brain from a configuration file")

    m.def("createBrainFromArgs", [](int argc, char** argv) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromArgs(argc, argv)) {
            throw std::runtime_error("Failed to create brain from arguments: invalid configuration");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("argc"), py::arg("argv"), "Create a brain from command line arguments")

    m.def("createAgentBrainFromConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<AgentBrain> {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null for agent brain creation");
        }
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("config"), "Create an agent brain from configuration (Pythonic factory)")

    m.def("createAgentBrainFromBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null for agent brain creation");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain from brain object (Pythonic factory)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createSimpleWorldWithSize", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create a simple world with specified dimensions and vision configuration")

    m.def("createSimpleWorldWithVision", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        return createSimpleWorldWithSize(width, height, visionWidth, visionHeight);
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create a simple world with vision configuration (Pythonic factory)")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Context managers for resource cleanup
    py::class_<py::module_::None> mNone(m, "None");
    
    // Enhanced Brain context manager
    py::class_<Brain, std::shared_ptr<Brain>>(m, "BrainContext", R"pbdoc(Context manager for Brain resources)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](Brain& self) {
            return &self;
        }, "Enter the context")
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        }, "Exit the context and clean up resources")
        .def("step", &Brain::step, py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("currentStep"), py::arg("currentTime"),
           "Perform a simulation step with timestamp")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file");

    // Enhanced World context manager
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "WorldContext", R"pbdoc(Context manager for World resources)pbdoc")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Enter the context")
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
        }, "Exit the context and clean up resources")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("reset", &SimpleWorld::reset,
             "Reset world state")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept");

    // Type conversion utilities
    m.def("to_string", [](const Action& action) {
        return action.getName();
    }, "Convert Action to string representation");
    
    m.def("to_vector", [](const Vision& vision) {
        return vision.getData();
    }, "Convert Vision to data vector");
    
    m.def("to_vector", [](const Audio& audio) {
        return audio.getData();
    }, "Convert Audio to data vector");

    // Serialization support
    m.def("serialize_brain", [](const Brain& brain, const std::string& filepath) {
        brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
        "Serialize brain to file");

    m.def("deserialize_brain", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath + ".config");
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"),
        "Deserialize brain from file");

    // Utility functions
    m.def("compare_actions", [](const Action& a1, const Action& a2) {
        return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
    }, py::arg("action1"), py::arg("action2"),
        "Compare two actions for equality");

    m.def("get_action_type_names", []() {
        std::vector<std::string> names;
        for (int i = 0; i < static_cast<int>(ActionType::Custom); ++i) {
            names.push_back(ActionType(i));
        }
        return names;
    }, "Get all action type names");

    m.def("calculate_brain_stats", [](const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        return stats;
    }, py::arg("brain"),
        "Calculate comprehensive brain statistics");

    // Enhanced error reporting
    m.def("get_last_error", []() {
        return std::string("No error available - implement error tracking system");
    }, "Get last error message");

    m.def("has_error", []() {
        return false;
    }, "Check if there was an error");
}
