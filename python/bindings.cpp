#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <map>
#include <filesystem>

// Core system headers
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"

// Configuration and core headers
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

// Agent and interface headers
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// World and environment headers
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"

// Sensory headers
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Motor headers
#include "../src/motor/Action.hpp"

// Memory system headers
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"

// Development and neuromodulation headers
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"

// Plasticity headers
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

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

// Helper function to convert std::vector to Python list of float
static py::list float_vector_to_list(const std::vector<float>& data) {
    py::list result;
    for (float val : data) {
        result.append(val);
    }
    return result;
}

// Helper function to convert float array to Python list
static py::list float_array_to_list(const std::array<float, 4>& data) {
    py::list result;
    for (float val : data) {
        result.append(val);
    }
    return result;
}

// Helper function to convert Python list to std::vector
static std::vector<float> list_to_float_vector(const py::list& lst) {
    std::vector<float> result;
    result.reserve(lst.size());
    for (auto item : lst) {
        result.push_back(item.cast<float>());
    }
    return result;
}

// Helper function for action type conversion
static ActionType string_to_action_type(const std::string& str) {
    if (str == "MoveForward") return ActionType::MoveForward;
    if (str == "MoveBackward") return ActionType::MoveBackward;
    if (str == "TurnLeft") return ActionType::TurnLeft;
    if (str == "TurnRight") return ActionType::TurnRight;
    if (str == "LookLeft") return ActionType::LookLeft;
    if (str == "LookRight") return ActionType::LookRight;
    if (str == "Interact") return ActionType::Interact;
    if (str == "Wait") return ActionType::Wait;
    return ActionType::Wait;
}

// Helper function for world object type conversion
static WorldObjectType string_to_world_object_type(const std::string& str) {
    if (str == "Resource") return WorldObjectType::Resource;
    if (str == "Hazard") return WorldObjectType::Hazard;
    if (str == "Wall") return WorldObjectType::Wall;
    if (str == "Marker") return WorldObjectType::Marker;
    return WorldObjectType::Empty;
}

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

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Vision& self, const py::array_t<float>& array) {
            if (array.ndim() != 2) {
                throw std::runtime_error("Vision data must be 2D array");
            }
            std::vector<float> data;
            data.reserve(array.size());
            for (ssize_t i = 0; i < array.shape(0); ++i) {
                for (ssize_t j = 0; j < array.shape(1); ++j) {
                    data.push_back(array(i, j));
                }
            }
            self.setData(data);
        }, py::arg("array"), "Set vision data from 2D Python array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataAsList", [](const Vision& self) {
            return float_vector_to_list(self.getData());
        }, "Get vision data as Python list")
        .def("normalize", [](Vision& self, float min_val = 0.0f, float max_val = 1.0f) {
            auto& data = self.getData();
            for (float& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
        }, py::arg("min_val") = 0.0f, py::arg("max_val") = 1.0f, "Normalize vision data to [0,1] range")
        .def("enhance", [](Vision& self, float factor = 1.5f) {
            auto& data = self.getData();
            for (float& val : data) {
                val = std::min(1.0f, val * factor);
            }
        }, py::arg("factor") = 1.5f, "Enhance vision data by multiplicative factor");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Audio& self, const py::array_t<float>& array) {
            if (array.ndim() != 1) {
                throw std::runtime_error("Audio data must be 1D array");
            }
            std::vector<float> data;
            data.reserve(array.size());
            for (ssize_t i = 0; i < array.shape(0); ++i) {
                data.push_back(array[i]);
            }
            self.setData(data);
        }, py::arg("array"), "Set audio data from 1D Python array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDataAsList", [](const Audio& self) {
            return float_vector_to_list(self.getData());
        }, "Get audio data as Python list")
        .def("normalize", [](Audio& self, float min_val = 0.0f, float max_val = 1.0f) {
            auto& data = self.getData();
            for (float& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
        }, py::arg("min_val") = 0.0f, py::arg("max_val") = 1.0f, "Normalize audio data to [0,1] range")
        .def("applyGain", [](Audio& self, float gain) {
            auto& data = self.getData();
            for (float& val : data) {
                val *= gain;
            }
        }, py::arg("gain"), "Apply gain to audio data")
        .def("filterLowPass", [](Audio& self, float cutoff, float sampleRate) {
            auto& data = self.getData();
            if (cutoff >= sampleRate / 2.0f) {
                throw std::runtime_error("Cutoff frequency must be less than Nyquist frequency");
            }
            // Simple first-order low-pass filter
            float alpha = 1.0f / (2.0f * M_PI * cutoff * 1.0f / sampleRate);
            float prev_output = 0.0f;
            for (size_t i = 0; i < data.size(); ++i) {
                float output = alpha * data[i] + (1.0f - alpha) * prev_output;
                prev_output = output;
                data[i] = output;
            }
        }, py::arg("cutoff"), py::arg("sampleRate"), "Apply simple low-pass filter");

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
        .def("getParameter", [](const Action& a, size_t index) -> std::optional<float> {
            const auto& params = a.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return std::nullopt;
        }, py::arg("index"), "Get parameter by index")
        .def("setParameter", [](Action& a, size_t index, float value) {
            auto& params = a.getParameters();
            if (index < params.size()) {
                params[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set parameter by index")
        .def("hasParameter", [](const Action& a, float value) {
            return std::find(a.getParameters().begin(), a.getParameters().end(), value) != a.getParameters().end();
        }, py::arg("value"), "Check if action has specific parameter value")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            std::string params_str = "[";
            const auto& params = a.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                params_str += std::to_string(params[i]);
                if (i < params.size() - 1) params_str += ", ";
            }
            params_str += "]";
            return "<Action: " + a.getName() + " (type=" + std::to_string(static_cast<int>(a.getType())) + ") params=" + params_str + ">";
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("step", &SimpleWorld::update, py::arg("timestep"), "Alias for update method")
        .def("get_agent_body", [](SimpleWorld& world) {
            return world.getAgentBody();
        }, py::return_value_policy::reference_internal, "Get agent body")
        .def("get_sensory_percept", [](SimpleWorld& world) {
            return world.getSensoryPercept();
        }, py::return_value_policy::reference_internal, "Get sensory percept")
        .def("simulate_step", [](SimpleWorld& world, float timestep) {
            world.update(timestep);
            return world.getSensoryPercept();
        }, py::arg("timestep"), "Simulate one step and return percept")
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add resource object to world")
        .def("add_hazard", [](SimpleWorld& world, float x, float y, float hazard_level) {
            WorldObject obj(x, y, WorldObjectType::Hazard, hazard_level, 0.4f);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("hazard_level") = 0.5f, "Add hazard object to world")
        .def("get_objects_near", [](SimpleWorld& world, float x, float y, float radius) {
            py::list result;
            // Note: This is a simplified implementation - real implementation would query world objects
            // For now, we'll just return empty list
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects near position")
        .def("get_stats", [](const SimpleWorld& world) {
            py::dict stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["max_energy"] = world.getMaxEnergy();
            stats["energy_decay_rate"] = world.getEnergyDecayRate();
            stats["simulation_time"] = world.getSimulationTime();
            return stats;
        }, "Get world statistics as dictionary");

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
        .def("step_with_time", [](Brain& brain, SimulationStep step, Timestamp time) {
            brain.step(step, time);
        }, py::arg("step"), py::arg("time"), "Step the brain with explicit timestamp")
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
        // Convenience methods
        .def("simulate_step", [](Brain& brain, SimulationStep step) {
            brain.step(step);
        }, py::arg("step"), "Simulate one step (alias for step())")
        .def("get_stats", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["region_count"] = brain.getRegionCount();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("get_neuron_by_id", [](Brain& brain, NeuronId id) {
            // This requires accessing neurons through regions - simplified version
            py::dict neuron_info;
            neuron_info["id"] = id.value;
            neuron_info["found"] = false; // Would need to search through regions
            return neuron_info;
        }, py::arg("id"), "Get neuron information by ID (placeholder)")
        .def("create_region", [](Brain& brain, const std::string& name) {
            return brain.addRegion(name);
        }, py::arg("name") = "", "Create and return a new region")
        .def("save_checkpoint", [](Brain& brain, const std::string& filepath) {
            return brain.save(filepath);
        }, py::arg("filepath"), "Save brain state as checkpoint")
        .def("load_checkpoint", [](Brain& brain, const std::string& filepath) {
            return brain.load(filepath);
        }, py::arg("filepath"), "Load brain state from checkpoint")
        .def("enable_all_systems", [](Brain& brain) {
            // Enable all plasticity and neuromodulation systems
            auto* stdp = brain.getSTDP();
            auto* hebbian = brain.getHebbian();
            auto* plasticity = brain.getStructuralPlasticity();
            auto* neuromod = brain.getDopamine();
            if (stdp) stdp->setLTPWeight(0.01f);
            if (hebbian) hebbian->setLearningRate(0.001f);
            if (plasticity) {
                plasticity->setSynaptogenesisRate(0.0001f);
                plasticity->setPruningRate(0.00001f);
            }
            if (neuromod) neuromod->setStrength(1.0f);
        }, "Enable all plasticity and neuromodulation systems")
        .def("disable_all_systems", [](Brain& brain) {
            // Disable all plasticity and neuromodulation systems
            auto* stdp = brain.getSTDP();
            auto* hebbian = brain.getHebbian();
            auto* plasticity = brain.getStructuralPlasticity();
            auto* neuromod = brain.getDopamine();
            if (stdp) stdp->setLTPWeight(0.0f);
            if (hebbian) hebbian->setLearningRate(0.0f);
            if (plasticity) {
                plasticity->setSynaptogenesisRate(0.0f);
                plasticity->setPruningRate(0.0f);
            }
            if (neuromod) neuromod->setStrength(0.0f);
        }, "Disable all plasticity and neuromodulation systems")

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
        // Configuration methods
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
        // Convenience methods
        .def("simulate_step", [](AgentBrain& agent, SimpleWorld& world, float timestep) {
            world.update(timestep);
            agent.updateDevelopment(timestep);
            return agent.decodeMotorCommand();
        }, py::arg("world"), py::arg("timestep"), "Simulate one step and return motor command")
        .def("process_observation", [](AgentBrain& agent, const py::dict& observation_dict) {
            // Convert Python dict to SensoryPercept
            SensoryPercept percept;
            
            // Extract vision data
            if (observation_dict.contains("vision")) {
                auto vision_data = observation_dict["vision"].cast<std::vector<float>>();
                percept.setVision(vision_data);
            }
            
            // Extract touch data
            if (observation_dict.contains("touch")) {
                auto touch_data = observation_dict["touch"].cast<std::vector<float>>();
                percept.setTouch(touch_data);
            }
            
            // Extract internal data
            if (observation_dict.contains("internal")) {
                auto internal_data = observation_dict["internal"].cast<std::vector<float>>();
                percept.setInternal(internal_data);
            }
            
            // Extract proprioception data
            if (observation_dict.contains("proprioception")) {
                auto proprio_data = observation_dict["proprioception"].cast<std::vector<float>>();
                percept.setProprioception(proprio_data);
            }
            
            // Extract audio data
            if (observation_dict.contains("audio")) {
                auto audio_data = observation_dict["audio"].cast<std::vector<float>>();
                percept.setAudio(audio_data);
            }
            
            agent.processSensoryInput(percept);
        }, py::arg("observation_dict"), "Process observation from Python dictionary")
        .def("take_action", [](AgentBrain& agent, SimpleWorld& world, float timestep) {
            world.update(timestep);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, timestep);
            return action;
        }, py::arg("world"), py::arg("timestep"), "Take action and return command")
        .def("get_agent_stats", [](AgentBrain& agent) {
            py::dict stats;
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            return stats;
        }, "Get agent statistics as dictionary")
        .def("update_environment", [](AgentBrain& agent, SimpleWorld& world, float timestep, float reward = 0.0f) {
            world.update(timestep);
            
            // Process sensory input
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            // Apply reward modulation
            if (reward != 0.0f) {
                agent.applyRewardModulation(reward, agent.getPredictionError());
            }
            
            // Update development
            agent.updateDevelopment(timestep);
            
            // Get action
            auto action = agent.decodeMotorCommand();
            
            // Apply action
            world.applyMotorCommand(action, timestep);
            
            return action;
        }, py::arg("world"), py::arg("timestep"), py::arg("reward") = 0.0f, "Update environment and return action")
        .def("reset_episode", [](AgentBrain& agent, SimpleWorld& world) {
            agent.reset();
            world.reset();
        }, py::arg("world"), "Reset agent and world for new episode")
        .def("enable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all agent systems")
        .def("disable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all agent systems")

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

    // Enhanced factory functions
    m.def("create_brain_with_config", [](py::dict config_dict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Load configuration from Python dict
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            // Note: Config class would need to be enhanced to support dictionary input
            // For now, this is a placeholder
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), "Create a brain with configuration from Python dictionary");

    m.def("create_demo_scenario", []() {
        py::dict result;
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(100, 100, 16, 16);
        world->setAgentStart(50, 50);
        agent->initialize(*world);
        
        result["brain"] = brain;
        result["world"] = world;
        result["agent"] = agent;
        return result;
    }, "Create a complete demo scenario with brain, world, and agent");

    m.def("simulate_episode", [](std::shared_ptr<AgentBrain> agent, 
                               std::shared_ptr<SimpleWorld> world, 
                               int max_steps, 
                               float reward_threshold = 0.0f) {
        if (!agent || !world) {
            throw std::runtime_error("Agent or world is null");
        }
        
        agent->reset();
        world->reset();
        
        py::list episode_data;
        float total_reward = 0.0f;
        
        for (int step = 0; step < max_steps; ++step) {
            py::dict step_data;
            
            // Update world
            world->update(0.1f);
            
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, 0.1f);
            
            // Get reward from world (simplified)
            float reward = std::rand() % 100 / 100.0f;
            total_reward += reward;
            
            // Store step data
            py::dict step_info;
            step_info["step"] = step;
            step_info["action"] = static_cast<int>(action);
            step_info["reward"] = reward;
            step_info["total_reward"] = total_reward;
            
            // Get vision data
            auto vision = percept.getVision();
            step_info["vision"] = float_vector_to_list(vision);
            
            // Get agent stats
            auto stats = py::cast<py::dict>(agent->get_agent_stats());
            step_info["stats"] = stats;
            
            episode_data.append(step_info);
            
            // Check if episode should end
            if (total_reward >= reward_threshold && reward_threshold > 0.0f) {
                break;
            }
        }
        
        py::dict result;
        result["episode_data"] = episode_data;
        result["total_reward"] = total_reward;
        result["steps_taken"] = episode_data.size();
        return result;
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps"), py::arg("reward_threshold") = 0.0f,
       "Simulate an episode and return detailed data");

    m.def("run_simulation_loop", [](std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  int num_episodes,
                                  int max_steps_per_episode = 100) {
        py::list all_episodes;
        float total_cumulative_reward = 0.0f;
        
        for (int episode = 0; episode < num_episodes; ++episode) {
            auto episode_result = simulate_episode(agent, world, max_steps_per_episode);
            all_episodes.append(episode_result);
            
            auto episode_dict = episode_result.cast<py::dict>();
            total_cumulative_reward += episode_dict["total_reward"].cast<float>();
        }
        
        py::dict summary;
        summary["total_episodes"] = num_episodes;
        summary["average_reward_per_episode"] = total_cumulative_reward / num_episodes;
        summary["all_episodes"] = all_episodes;
        
        return summary;
    }, py::arg("agent"), py::arg("world"), py::arg("num_episodes"), py::arg("max_steps_per_episode") = 100,
       "Run multiple simulation episodes and return summary");

    // Error and exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Constants and configuration
    m.attr("DEFAULT_VISION_SIZE") = py::make_tuple(16, 16, 3);
    m.attr("DEFAULT_WORLD_SIZE") = py::make_tuple(100, 100);
    m.attr("MAX_EPISODE_STEPS") = 1000;
    m.attr("DEFAULT_REWARD_THRESHOLD") = 10.0f;

    // Utility functions
    m.def("validate_observation", [](const py::dict& obs) {
        bool valid = true;
        std::vector<std::string> errors;
        
        if (obs.contains("vision")) {
            auto vision = obs["vision"].cast<py::list>();
            if (vision.size() != 256) {  // 16x16
                errors.push_back("Vision data should have 256 elements (16x16)");
            }
        }
        
        if (obs.contains("touch")) {
            auto touch = obs["touch"].cast<py::list>();
            if (touch.size() != 8) {
                errors.push_back("Touch data should have 8 elements");
            }
        }
        
        if (obs.contains("internal")) {
            auto internal = obs["internal"].cast<py::list>();
            if (internal.size() != 4) {
                errors.push_back("Internal data should have 4 elements");
            }
        }
        
        if (obs.contains("proprioception")) {
            auto proprio = obs["proprioception"].cast<py::list>();
            if (proprio.size() != 6) {
                errors.push_back("Proprioception data should have 6 elements");
            }
        }
        
        py::dict result;
        result["valid"] = valid;
        result["errors"] = errors;
        return result;
    }, py::arg("obs"), "Validate observation data structure");

    m.def("create_action_from_dict", [](const py::dict& action_dict) {
        ActionType type = string_to_action_type(action_dict["type"].cast<std::string>());
        std::vector<float> params;
        
        if (action_dict.contains("parameters")) {
            auto params_list = action_dict["parameters"].cast<py::list>();
            params = list_to_float_vector(params_list);
        }
        
        return Action(type, params);
    }, py::arg("action_dict"), "Create action from Python dictionary");

    m.def("get_action_type_names", []() {
        py::list names;
        names.append("MoveForward");
        names.append("MoveBackward");
        names.append("TurnLeft");
        names.append("TurnRight");
        names.append("LookLeft");
        names.append("LookRight");
        names.append("Interact");
        names.append("Wait");
        return names;
    }, "Get list of available action type names");

    m.def("get_world_object_type_names", []() {
        py::list names;
        names.append("Empty");
        names.append("Resource");
        names.append("Hazard");
        names.append("Wall");
        names.append("Marker");
        return names;
    }, "Get list of available world object type names");

} // namespace nlm
