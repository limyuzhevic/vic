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
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Pythonic property access and convenience methods
        .def_property_readonly("sensory_input_size", &Brain::getSensoryInputSize,
                              "Get expected sensory input size")
        .def_property_readonly("motor_output_size", &Brain::getMotorOutputSize,
                              "Get expected motor output size")
        .def_property_readonly("spike_system", &Brain::getSpikeSystem,
                              py::return_value_policy::reference_internal,
                              "Get the spike system")
        .def_property_readonly("stdp", &Brain::getSTDP,
                              py::return_value_policy::reference_internal,
                              "Get the STDP plasticity rule")
        .def_property_readonly("hebbian", &Brain::getHebbian,
                              py::return_value_policy::reference_internal,
                              "Get the Hebbian plasticity rule")
        .def_property_readonly("structural_plasticity", &Brain::getStructuralPlasticity,
                              py::return_value_policy::reference_internal,
                              "Get the structural plasticity rule")
        .def_property_readonly("working_memory", &Brain::getWorkingMemory,
                              py::return_value_policy::reference_internal,
                              "Get the working memory system")
        .def_property_readonly("episodic_memory", &Brain::getEpisodicMemory,
                              py::return_value_policy::reference_internal,
                              "Get the episodic memory system")
        .def_property_readonly("associative_memory", &Brain::getAssociativeMemory,
                              py::return_value_policy::reference_internal,
                              "Get the associative memory system")
        .def_property_readonly("prediction_system", &Brain::getPredictionSystem,
                              py::return_value_policy::reference_internal,
                              "Get the prediction system")
        .def_property_readonly("planner", &Brain::getPlanner,
                              py::return_value_policy::reference_internal,
                              "Get the neural planner")
        .def_property_readonly("concept_formation", &Brain::getConceptFormation,
                              py::return_value_policy::reference_internal,
                              "Get the concept formation system")
        .def_property_readonly("attention", &Brain::getAttention,
                              py::return_value_policy::reference_internal,
                              "Get the attentional selection system")
        .def_property_readonly("development_system", &Brain::getDevelopmentSystem,
                              py::return_value_policy::reference_internal,
                              "Get the development system")
        .def_property_readonly("dopamine", &Brain::getDopamine,
                              py::return_value_policy::reference_internal,
                              "Get the dopamine neuromodulator")
        .def_property_readonly("curiosity", &Brain::getCuriosity,
                              py::return_value_policy::reference_internal,
                              "Get the curiosity neuromodulator")
        .def_property_readonly("novelty", &Brain::getNovelty,
                              py::return_value_policy::reference_internal,
                              "Get the novelty neuromodulator")
        .def_property_readonly("prediction_error", &Brain::getPredictionErrorSignal,
                              py::return_value_policy::reference_internal,
                              "Get the prediction error signal")
        .def_property_readonly("random_generator", &Brain::getRandomGenerator,
                              py::return_value_policy::reference_internal,
                              "Get the random number generator")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                              "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                              "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                              "Get novelty level")
        .def_property_readonly("prediction_error_level", &AgentBrain::getPredictionError,
                              "Get prediction error")
        // Convenience methods for common operations
        .def("simulate", [](Brain& self, int steps) {
            for (int i = 0; i < steps; ++i) {
                self.step(i);
            }
        }, py::arg("steps"), "Simulate for a number of steps")
        .def("simulate_with_time", [](Brain& self, int steps, Timestamp timeStep) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timeStep);
            }
        }, py::arg("steps"), py::arg("time_step"), "Simulate for steps with time step")
        .def("reset_and_initialize", [](Brain& self) {
            self.reset();
            self.initialize();
        }, "Reset and reinitialize the brain")
        .def("create_sensory_percept", [](Brain& self) {
            return SensoryPercept();
        }, "Create an empty sensory percept")
        .def("create_vision_input", [](Brain& self, size_t width, size_t height, size_t channels = 3) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a vision input with specified dimensions")
        .def("create_audio_input", [](Brain& self, size_t sampleRate, size_t numSamples) {
            return Audio(sampleRate, numSamples);
        }, py::arg("sample_rate"), py::arg("num_samples"), "Create an audio input")
        .def("create_action", [](Brain& self, ActionType type) {
            return Action(type);
        }, py::arg("type"), "Create an action of specified type")
        .def("create_action_with_params", [](Brain& self, ActionType type, const std::vector<float>& params) {
            return Action(type, params);
        }, py::arg("type"), py::arg("parameters"), "Create an action with type and parameters")
        .def("create_world_object", [](Brain& self, float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            return WorldObject(x, y, type, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, "Create a world object")
        .def("create_agent_body", [](Brain& self) {
            return AgentBody();
        }, "Create an agent body state")
        .def("create_action_result", [](Brain& self, float reward, bool success, const std::string& message = "") {
            return ActionResult(reward, success, message);
        }, py::arg("reward"), py::arg("success"), py::arg("message") = "", "Create an action result")
        // Advanced convenience methods
        .def("run_episode", [](Brain& self, const std::vector<float>& sensoryData, int steps, float timestep = 1.0f) {
            self.reset();
            self.initialize();
            Vision vision;
            vision.setData(sensoryData);
            auto percept = SensoryPercept();
            percept.setVision(vision);
            self.receive_sensory_input(percept);
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
            return self.produce_action();
        }, py::arg("sensory_data"), py::arg("steps"), py::arg("timestep") = 1.0f, "Run a complete episode with sensory input")
        .def("run_episode_with_vision", [](Brain& self, size_t width, size_t height, size_t channels, 
                                          const std::vector<float>& data, int steps, float timestep = 1.0f) {
            self.reset();
            self.initialize();
            Vision vision(width, height, channels);
            vision.setData(data);
            auto percept = SensoryPercept();
            percept.setVision(vision);
            self.receive_sensory_input(percept);
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
            return self.produce_action();
        }, py::arg("width"), py::arg("height"), py::arg("channels"), 
                  py::arg("data"), py::arg("steps"), py::arg("timestep") = 1.0f, 
                  "Run episode with vision input")
        .def("create_simple_world", []() {
            auto world = std::make_shared<SimpleWorld>();
            return world;
        }, "Create a new simple world")
        .def("create_default_config", []() {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("create_brain", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        .def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create an agent brain interface");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("prediction_error_level", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
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
        // Convenience properties for common neuromodulation levels
        .def_property_readonly("reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
                              "Check if reward modulation is enabled")
        .def_property_readonly("structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
                              "Check if structural plasticity is enabled")
        .def_property_readonly("development_enabled", &AgentBrain::isDevelopmentEnabled,
                              "Check if development is enabled")
        .def_property_readonly("curiosity_enabled", &AgentBrain::isCuriosityEnabled,
                              "Check if curiosity is enabled")
        // Convenience methods for common operations
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all adaptive systems (reward, plasticity, development, curiosity)")
        .def("disable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all adaptive systems")
        .def("apply_modulation", [](AgentBrain& self, float reward, float curiosity, float novelty, float prediction_error) {
            self.applyRewardModulation(reward, prediction_error);
        }, py::arg("reward"), py::arg("curiosity"), py::arg("novelty"), py::arg("prediction_error"),
           "Apply all neuromodulation signals at once")
        .def("get_neuromodulation_summary", [](AgentBrain& self) {
            return "Neuromodulation: " +
                   std::string(self.isRewardModulationEnabled() ? "R+" : "R-") +
                   std::string(self.isStructuralPlasticityEnabled() ? "P+" : "P-") +
                   std::string(self.isDevelopmentEnabled() ? "D+" : "D-") +
                   std::string(self.isCuriosityEnabled() ? "C+" : "C-") +
                   " | " +
                   std::to_string(self.getNeuromodulationLevel()) + "/10";
        }, "Get a summary of current neuromodulation state")
        .def("reset_systems", [](AgentBrain& self) {
            self.reset();
            self.enableAllSystems();
        }, "Reset agent and re-enable all systems");

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
