#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace nlm {

// Custom Python exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    explicit NLMError(const char* message) : std::runtime_error(message) {}
};

// Convert NLMError to Python exception
inline void register_nlm_exception(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
}
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
        // Convenience methods for common setup patterns
        .def("setup_vision", [](Config& self, size_t width, size_t height, 
                               size_t channels = 3) {
            self.set("brain.vision.width", static_cast<uint64_t>(width));
            self.set("brain.vision.height", static_cast<uint64_t>(height));
            self.set("brain.vision.channels", static_cast<uint64_t>(channels));
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           R"pbdoc(Setup vision configuration parameters)pbdoc")
        .def("setup_audio", [](Config& self, size_t sampleRate, size_t numSamples) {
            self.set("brain.audio.sampleRate", static_cast<uint64_t>(sampleRate));
            self.set("brain.audio.numSamples", static_cast<uint64_t>(numSamples));
        }, py::arg("sampleRate"), py::arg("numSamples"),
           R"pbdoc(Setup audio configuration parameters)pbdoc")
        .def("setup_brain", [](Config& self, size_t neuronCount = 100, 
                              size_t regionCount = 1) {
            self.set("brain.neuron.count", static_cast<uint64_t>(neuronCount));
            self.set("brain.region.count", static_cast<uint64_t>(regionCount));
        }, py::arg("neuronCount") = 100, py::arg("regionCount") = 1,
           R"pbdoc(Setup basic brain configuration)pbdoc")
        .def("setup_performance", [](Config& self, bool enableSIMD = true, 
                                     bool enableParallel = false) {
            self.set("performance.simd.enabled", enableSIMD);
            self.set("performance.parallel.enabled", enableParallel);
        }, py::arg("enableSIMD") = true, py::arg("enableParallel") = false,
           R"pbdoc(Setup performance optimization settings)pbdoc")
        .def("validate", &Config::summary,
             "Validate configuration and return summary")
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
        .def("getChannels", &Vision::getChannels)
        // Convenience methods
        .def("setImageData", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& image) {
            std::vector<float> flatData;
            for (const auto& row : image) {
                for (const auto& pixel : row) {
                    flatData.insert(flatData.end(), pixel.begin(), pixel.end());
                }
            }
            self.setData(flatData);
        }, py::arg("image"), R"pbdoc(Set vision data from 3D list of RGB values)pbdoc")
        .def("resize", &Vision::resize, py::arg("width"), py::arg("height"),
             R"pbdoc(Resize vision input to specified dimensions)pbdoc")
        .def("normalize", &Vision::normalize, R"pbdoc(Normalize vision data to [0,1] range)pbdoc");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Convenience methods
        .def("setAudioData", &Audio::setData, py::arg("data"), R"pbdoc(Set audio data directly)pbdoc")
        .def("addNoise", [](Audio& self, float amount) {
            auto data = self.getData();
            for (auto& sample : data) {
                sample += (rand() / (float)RAND_MAX - 0.5f) * amount * 2.0f;
            }
            self.setData(data);
        }, py::arg("amount"), R"pbdoc(Add random noise to audio data)pbdoc");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Convenience methods
        .def("addNeuronActivity", &InternalSignals::addSignal, py::arg("activity"),
             R"pbdoc(Add neuron activity as internal signal)pbdoc")
        .def("addMemoryContent", &InternalSignals::addSignal, py::arg("content"),
             R"pbdoc(Add memory content as internal signal)pbdoc");

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
        // Convenience methods
        .def("create_move", &Action::create_move, py::arg("direction"), py::arg("speed") = 1.0f,
             R"pbdoc(Create a movement action)pbdoc")
        .def("create_look", &Action::create_look, py::arg("direction"),
             R"pbdoc(Create a look action)pbdoc")
        .def("create_interact", &Action::create_interact, py::arg("target"),
             R"pbdoc(Create an interaction action)pbdoc")
        .def("is_movement", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight;
        }, R"pbdoc(Check if action is a movement action)pbdoc")
        .def("is_sensory", [](const Action& self) {
            return self.getType() == ActionType::Look ||
                   self.getType() == ActionType::LookUp ||
                   self.getType() == ActionType::LookDown ||
                   self.getType() == ActionType::Interact;
        }, R"pbdoc(Check if action is a sensory/action)pbdoc")
        .def("__repr__", [](const Action& act) {
            return "<Action: " + act.getName() + ">";
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
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, R"pbdoc(Check if action was successful)pbdoc")
        .def("was_rewarded", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, R"pbdoc(Check if action received positive reward)pbdoc")
        .def("get_satisfied", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, R"pbdoc(Check if action was both successful and rewarded)pbdoc")
        .def("create_success", &ActionResult::create_success,
             py::arg("reward") = 1.0f, py::arg("message") = "",
             R"pbdoc(Create a successful action result)pbdoc")
        .def("create_failure", &ActionResult::create_failure,
             py::arg("reward") = -1.0f, py::arg("message") = "",
             R"pbdoc(Create a failed action result)pbdoc")
        .def("__repr__", [](const ActionResult& res) {
            return "<ActionResult: reward=" + std::to_string(res.reward) +
                   ", success=" + std::to_string(res.success) + ">";
        });

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
        // Convenience methods
        .def("update_vision", [](SensoryPercept& self, const std::vector<std::vector<std::vector<float>>>& image) {
            std::vector<float> flatData;
            for (const auto& row : image) {
                for (const auto& pixel : row) {
                    flatData.insert(flatData.end(), pixel.begin(), pixel.end());
                }
            }
            Vision vision;
            vision.setData(flatData);
            self.setVision(vision);
        }, py::arg("image"), R"pbdoc(Update vision percept from 3D list)pbdoc")
        .def("update_internal_signals", [](SensoryPercept& self, const std::vector<float>& signals) {
            InternalSignals internal;
            for (float sig : signals) {
                internal.addSignal(sig);
            }
            self.setInternal(internal);
        }, py::arg("signals"), R"pbdoc(Update internal signals from list)pbdoc")
        .def("is_vision_available", [](const SensoryPercept& self) {
            return self.getVision().getWidth() > 0;
        }, R"pbdoc(Check if vision data is available)pbdoc")
        .def("is_audio_available", [](const SensoryPercept& self) {
            return self.getAudio().getNumSamples() > 0;
        }, R"pbdoc(Check if audio data is available)pbdoc")
        .def("get_all_data", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory data as concatenated vector)pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) +
                   "x" + std::to_string(percept.getVisionHeight()) + ", " +
                   "audio_samples=" + std::to_string(percept.getAudio().getNumSamples()) + ">";
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions and sensory field of view)pbdoc")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state by timestep duration)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to world)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add world object)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid (not inside obstacles))pbdoc")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum energy for agent)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for reproducible simulation)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Convenience methods
        .def("create_world", &SimpleWorld::configure,
             py::arg("width") = 100, py::arg("height") = 100,
             py::arg("visionWidth") = 64, py::arg("visionHeight") = 64,
             R"pbdoc(Create world with default dimensions)pbdoc")
        .def("add_resource", [](SimpleWorld& self, float x, float y, 
                               float radius = 0.3f, float value = 10.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.3f, py::arg("value") = 10.0f,
           R"pbdoc(Add resource object at position)pbdoc")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, 
                             float radius = 0.3f, float value = -5.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.3f, py::arg("value") = -5.0f,
           R"pbdoc(Add hazard object at position)pbdoc")
        .def("get_agent_state", &SimpleWorld::getAgentBody,
             R"pbdoc(Get current agent state)pbdoc")
        .def("get_environment_state", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time)pbdoc");

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
        // Convenience methods for brain setup and control
        .def("create_region", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region)pbdoc")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID)pbdoc")
        .def("get_region_count", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions)pbdoc")
        .def("get_all_region_ids", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs)pbdoc")
        .def("add_inter_region_connection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f,
             py::arg("delay") = 1,
             R"pbdoc(Add connection between regions)pbdoc")
        .def("remove_inter_region_connection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             R"pbdoc(Remove connection between regions)pbdoc")
        // Statistics and monitoring
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons)pbdoc")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count)pbdoc")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)pbdoc")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage)pbdoc")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration)pbdoc")
        .def("log_status", &Brain::logStatus,
             R"pbdoc(Log brain status)pbdoc")
        // Advanced features for expert users
        .def("get_spike_system", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get spike system access)pbdoc")
        .def("get_stdp", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get STDP plasticity system)pbdoc")
        .def("get_hebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get Hebbian plasticity system)pbdoc")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get structural plasticity system)pbdoc")
        .def("get_working_memory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get working memory system)pbdoc")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get episodic memory system)pbdoc")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get associative memory system)pbdoc")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get prediction system)pbdoc")
        .def("get_planner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get neural planner system)pbdoc")
        .def("get_concept_formation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get concept formation system)pbdoc")
        .def("get_attention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get attentional selection system)pbdoc")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get development system)pbdoc")
        .def("get_dopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get dopamine neuromodulation system)pbdoc")
        .def("get_curiosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get curiosity neuromodulation system)pbdoc")
        .def("get_novelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get novelty detection system)pbdoc")
        .def("get_prediction_error", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get prediction error system)pbdoc")
        .def("get_random_generator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get random number generator)pbdoc")
        // Simulation control methods
        .def("run_steps", [](Brain& self, size_t steps, double timestep = 0.01) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, timestep * i);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.01,
           R"pbdoc(Run multiple simulation steps efficiently)pbdoc")
        .def("simulate_episode", [](Brain& self, const std::vector<class SensoryInput>& inputs, 
                                   double timestep = 0.01) {
            for (size_t i = 0; i < inputs.size(); ++i) {
                self.receiveSensoryInput(inputs[i]);
                self.step(i, timestep * i);
            }
        }, py::arg("inputs"), py::arg("timestep") = 0.01,
           R"pbdoc(Simulate an episode with given sensory inputs)pbdoc")
        .def("export_brain_state", &Brain::save, py::arg("filepath"),
             R"pbdoc(Export brain state to file)pbdoc")
        .def("import_brain_state", &Brain::load, py::arg("filepath"),
             R"pbdoc(Import brain state from file)pbdoc");

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
        // Configuration and system control
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods for common agent behaviors
        .def("create_and_initialize", [](std::shared_ptr<Brain> brain, const SimpleWorld& world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            return agent;
        }, py::arg("brain"), py::arg("world"),
           R"pbdoc(Create and initialize agent brain in one call)pbdoc")
        .def("run_episode", [](AgentBrain& self, SimpleWorld& world, 
                              const std::vector<class SensoryPercept>& percepts,
                              double timestep = 0.01) {
            bool episodeComplete = false;
            for (size_t i = 0; i < percepts.size() && !episodeComplete; ++i) {
                self.processSensoryInput(percepts[i]);
                world.update(timestep * i);
                
                if (world.getAgentBody().health <= 0.0f || 
                    self.getDevelopmentalStage() == DevelopmentalStage::Aging) {
                    episodeComplete = true;
                }
            }
            if (episodeComplete) {
                self.reset();
            }
        }, py::arg("world"), py::arg("percepts"), py::arg("timestep") = 0.01,
           R"pbdoc(Run complete episode with given percepts)pbdoc")
        .def("get_agent_stats", [](const AgentBrain& self) {
            return py::dict(
                "curiosity_level"_a = self.getCuriosityLevel(),
                "novelty_level"_a = self.getNoveltyLevel(),
                "neuromodulation_level"_a = self.getNeuromodulationLevel(),
                "prediction_error"_a = self.getPredictionError(),
                "development_stage"_a = static_cast<int>(self.getDevelopmentalStage())
            );
        }, R"pbdoc(Get agent statistics as dictionary)pbdoc")
        .def("simulate_action", [](AgentBrain& self, const class Action& action) {
            ActionResult result;
            result.set_success(true);
            result.set_reward(1.0f);
            result.set_message("Action simulated successfully");
            self.applyRewardModulation(1.0f, 0.5f);
            return result;
        }, py::arg("action"), R"pbdoc(Simulate action and return result)pbdoc");

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
