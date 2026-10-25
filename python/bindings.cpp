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
        })
        // Advanced configuration features
        .def("getInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("default"),
             "Get integer value with default")
        .def("getFloat", &Config::getOr<double>, py::arg("key"), py::arg("default"),
             "Get float value with default")
        .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("default"),
             "Get string value with default")
        .def("setInt", &Config::set<int64_t>, py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set float value")
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set string value")
        .def("configureForPhase", [](Config& config, int phase, int scale = 1) {
            // Phase-based configuration presets
            if (phase == 2) {
                config.set("neuron_count", 500 * scale, ConfigSource::Default);
                config.set("region_count", 1, ConfigSource::Default);
                config.set("connection_probability", 0.15f, ConfigSource::Default);
                config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                config.set("simulation_timestep", 0.001, ConfigSource::Default);
            } else if (phase == 4) {
                config.set("neuron_count", 1000 * scale, ConfigSource::Default);
                config.set("region_count", 4, ConfigSource::Default);
                config.set("connection_probability", 0.1f, ConfigSource::Default);
                config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                config.set("simulation_timestep", 0.001, ConfigSource::Default);
            } else if (phase == 6) {
                config.set("neuron_count", 2000 * scale, ConfigSource::Default);
                config.set("region_count", 8, ConfigSource::Default);
                config.set("connection_probability", 0.05f, ConfigSource::Default);
                config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                config.set("simulation_timestep", 0.001, ConfigSource::Default);
            }
        }, py::arg("phase"), py::arg("scale") = 1,
             "Configure system for a specific phase with scaling")
        .def("clone", [](const Config& config) {
            auto cloned = std::make_shared<Config>();
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                if (config.has(key)) {
                    // Note: Config doesn't have a direct get method for all types
                    // This is a limitation - in practice you need to know the type
                }
            }
            return cloned;
        }, "Create a copy of this configuration")
        .def("validate", [](Config& config) {
            bool valid = true;
            auto keys = config.getKeys();
            
            // Basic validation checks
            if (config.has("neuron_count")) {
                int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
                if (neuronCount <= 0) {
                    std::cout << "Warning: neuron_count should be positive" << std::endl;
                    valid = false;
                }
            }
            
            if (config.has("simulation_timestep")) {
                double timestep = config.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0) {
                    std::cout << "Warning: simulation_timestep should be positive" << std::endl;
                    valid = false;
                }
            }
            
            return valid;
        }, "Validate configuration parameters")
        .def("getPhaseSpecificConfig", [](int phase) {
            auto config = std::make_shared<Config>();
            
            switch (phase) {
                case 2: // Basic neural computation
                    config->set("neuron_count", 500, ConfigSource::Default);
                    config->set("region_count", 1, ConfigSource::Default);
                    config->set("connection_probability", 0.15f, ConfigSource::Default);
                    break;
                case 3: // World interaction
                    config->set("neuron_count", 1000, ConfigSource::Default);
                    config->set("region_count", 2, ConfigSource::Default);
                    config->set("connection_probability", 0.1f, ConfigSource::Default);
                    config->set("enable_reward_modulation", true, ConfigSource::Default);
                    break;
                case 4: // Emerging cognition
                    config->set("neuron_count", 2000, ConfigSource::Default);
                    config->set("region_count", 4, ConfigSource::Default);
                    config->set("connection_probability", 0.08f, ConfigSource::Default);
                    config->set("enable_reward_modulation", true, ConfigSource::Default);
                    config->set("enable_structural_plasticity", true, ConfigSource::Default);
                    break;
                case 6: // Final integration
                    config->set("neuron_count", 5000, ConfigSource::Default);
                    config->set("region_count", 8, ConfigSource::Default);
                    config->set("connection_probability", 0.05f, ConfigSource::Default);
                    config->set("enable_reward_modulation", true, ConfigSource::Default);
                    config->set("enable_structural_plasticity", true, ConfigSource::Default);
                    config->set("enable_development", true, ConfigSource::Default);
                    config->set("enable_curiosity", true, ConfigSource::Default);
                    break;
                default:
                    std::cout << "Warning: Unknown phase " << phase << ". Using phase 2 defaults." << std::endl;
                    config->set("neuron_count", 100, ConfigSource::Default);
                    config->set("region_count", 1, ConfigSource::Default);
            }
            
            return config;
        }, py::arg("phase"), "Get phase-specific default configuration")
        .def("merge", [](Config& config, const Config& other) {
            // Merge other config into this one
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                if (other.has(key)) {
                    // Note: This is a simplified merge - actual implementation
                    // would need to handle type conversion
                }
            }
        }, py::arg("other"), "Merge another configuration into this one");

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
        .def("stepMultiple", [](Brain& brain, std::vector<SimulationStep> steps, double timestep = 0.001) {
            for (size_t i = 0; i < steps.size(); ++i) {
                Timestamp time = static_cast<Timestamp>(i) * timestep;
                brain.step(steps[i], time);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
             "Perform multiple simulation steps efficiently")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrentToAll", [](Brain& brain, MembranePotential current) {
            for (NeuronType type : {NeuronType::Excitatory, NeuronType::Inhibitory, 
                                    NeuronType::Sensory, NeuronType::Motor, 
                                    NeuronType::Internal, NeuronType::Modulatory}) {
                brain.injectCurrentToNeurons(type, current);
            }
        }, py::arg("current"),
             "Inject current into all neuron types")
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
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator for reproducible experiments")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("simulateEpisode", [](Brain& brain, double duration, double timestep = 0.001) {
            size_t steps = static_cast<size_t>(duration / timestep);
            for (size_t step = 0; step < steps; ++step) {
                brain.step(step, step * timestep);
            }
        }, py::arg("duration"), py::arg("timestep") = 0.001,
             "Run simulation for a specified duration")
        .def("getPerformanceMetrics", [](const Brain& brain) {
            py::dict metrics;
            metrics["total_neurons"] = brain.getTotalNeuronCount();
            metrics["total_synapses"] = brain.getTotalSynapseCount();
            metrics["active_neurons"] = brain.getActiveNeuronCount();
            metrics["firing_neurons"] = brain.getFiringNeuronCount();
            metrics["average_firing_rate"] = brain.getAverageFiringRate();
            metrics["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            metrics["total_spikes"] = brain.getTotalSpikeCount();
            metrics["pending_spike_events"] = brain.getPendingSpikeEventCount();
            metrics["development_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return metrics;
        }, "Get comprehensive performance metrics")
        .def("enableAllPlasticity", [](Brain& brain) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
        }, "Enable all plasticity mechanisms (Hebbian, STDP, structural)")
        .def("disableAllPlasticity", [](Brain& brain) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, false, false);
                }
            }
        }, "Disable all plasticity mechanisms")
        .def("enableAllNeuromodulation", [](Brain& brain) {
            if (auto* dopamine = brain.getDopamine()) dopamine->enable(true);
            if (auto* curiosity = brain.getCuriosity()) curiosity->enable(true);
            if (auto* novelty = brain.getNovelty()) novelty->enable(true);
            if (auto* predictionError = brain.getPredictionErrorSignal()) predictionError->enable(true);
        }, "Enable all neuromodulation systems")
        .def("disableAllNeuromodulation", [](Brain& brain) {
            if (auto* dopamine = brain.getDopamine()) dopamine->enable(false);
            if (auto* curiosity = brain.getCuriosity()) curiosity->enable(false);
            if (auto* novelty = brain.getNovelty()) novelty->enable(false);
            if (auto* predictionError = brain.getPredictionErrorSignal()) predictionError->enable(false);
        }, "Disable all neuromodulation systems")
        .def("getMemoryStats", [](const Brain& brain) {
            py::dict stats;
            if (auto* wm = brain.getWorkingMemory()) {
                stats["working_memory"] = py::dict(
                    "active_traces", wm->getActiveTraces(),
                    "capacity", wm->getCapacity(),
                    "active_traces_percentage", 
                        static_cast<float>(wm->getActiveTraces()) / wm->getCapacity()
                );
            }
            if (auto* em = brain.getEpisodicMemory()) {
                stats["episodic_memory"] = py::dict(
                    "episode_count", em->getEpisodeCount(),
                    "max_capacity", em->getMaxCapacity(),
                    "episode_percentage",
                        static_cast<float>(em->getEpisodeCount()) / em->getMaxCapacity()
                );
            }
            if (auto* am = brain.getAssociativeMemory()) {
                stats["associative_memory"] = py::dict(
                    "association_count", am->getAssociationCount(),
                    "max_capacity", am->getMaxCapacity()
                );
            }
            return stats;
        }, "Get memory system statistics")
        .def("getLearningStats", [](const Brain& brain) {
            py::dict stats;
            if (auto* stdp = brain.getSTDP()) {
                stats["stdp"] = py::dict(
                    "learning_rate", stdp->getLearningRate(),
                    "potentiation_strength", stdp->getPotentiationStrength(),
                    "depression_strength", stdp->getDepressionStrength(),
                    "tau_plus", stdp->getTimeConstantPlus(),
                    "tau_minus", stdp->getTimeConstantMinus()
                );
            }
            if (auto* hebbian = brain.getHebbian()) {
                stats["hebbian"] = py::dict(
                    "learning_rate", hebbian->getLearningRate(),
                    "weight_change_max", hebbian->getMaxWeightChange()
                );
            }
            if (auto* structural = brain.getStructuralPlasticity()) {
                stats["structural_plasticity"] = py::dict(
                    "synaptogenesis_rate", structural->getSynaptogenesisRate(),
                    "pruning_rate", structural->getPruningRate(),
                    "max_synapses", structural->getMaxSynapses()
                );
            }
            return stats;
        }, "Get learning mechanism statistics")

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
        // Advanced AgentBrain features
        .def("runEpisode", [](AgentBrain& agent, SimpleWorld& world, int numSteps) {
            for (int step = 0; step < numSteps; ++step) {
                // Update world
                world.update(0.1);
                
                // Process sensory input
                agent.processSensoryInput(world.getSensoryPercept());
                
                // Get action from brain (simulate brain step)
                // Note: Brain step is called separately by the user
                auto action = agent.decodeMotorCommand();
                
                // Apply action to world
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("world"), py::arg("numSteps"),
             "Run a complete episode with agent and world interaction")
        .def("runControlledEpisode", [](AgentBrain& agent, SimpleWorld& world, 
                                       std::function<bool(int)> condition,
                                       std::function<void(int)> callback,
                                       double timestep = 0.1) {
            int step = 0;
            while (condition(step)) {
                world.update(timestep);
                agent.processSensoryInput(world.getSensoryPercept());
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                callback(step);
                step++;
            }
        }, py::arg("world"), py::arg("condition"), py::arg("callback"), py::arg("timestep") = 0.1,
             "Run episode with custom control logic")
        .def("enableAllSubsystems", &AgentBrain::enableRewardModulation, 
             py::arg("true"), py::arg("true"), py::arg("true"), py::arg("true"),
             "Enable all learning and adaptive systems")
        .def("disableAllSubsystems", &AgentBrain::enableRewardModulation,
             py::arg("false"), py::arg("false"), py::arg("false"), py::arg("false"),
             "Disable all learning and adaptive systems")
        .def("getComprehensiveStats", [](const AgentBrain& agent) {
            py::dict stats;
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["is_reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["is_structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["is_development_enabled"] = agent.isDevelopmentEnabled();
            stats["is_curiosity_enabled"] = agent.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics")
        .def("updateAllSystems", [](AgentBrain& agent, SimpleWorld& world, double timestep) {
            agent.updateDevelopment(timestep);
            // Note: Reward modulation is typically applied by the user
            // after receiving world feedback
        }, py::arg("world"), py::arg("timestep"),
             "Update all agent systems")
        .def("setDevelopmentParameters", [](AgentBrain& agent, double age, double plasticityModifier) {
            // These are internal parameters that affect development
            // In a real implementation, these would be exposed through proper setters
        }, py::arg("age"), py::arg("plasticity_modifier"),
             "Set development parameters")
        .def("getEpisodeStats", [](const AgentBrain& agent, const SimpleWorld& world, int episodeSteps) {
            py::dict stats;
            stats["steps_completed"] = episodeSteps;
            stats["simulation_time"] = world.getSimulationTime();
            stats["average_firing_rate"] = agent.getBrain()->getAverageFiringRate();
            stats["curiosity_trajectory"] = "simulated"; // Would need to track over time
            return stats;
        }, py::arg("world"), py::arg("episodeSteps"),
             "Get statistics about an episode")

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainFromPhase", [](int phase, int scale = 1) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        if (phase == 2) {
            config->set("neuron_count", 500 * scale, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
        } else if (phase == 4) {
            config->set("neuron_count", 1000 * scale, ConfigSource::Default);
            config->set("region_count", 4, ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
        } else if (phase == 6) {
            config->set("neuron_count", 2000 * scale, ConfigSource::Default);
            config->set("region_count", 8, ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("phase"), py::arg("scale") = 1,
       "Create a brain configured for a specific development phase");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("runPhase2Experiment", [](int numSteps, int scale = 1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500 * scale, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run basic connectivity test
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        return spikes;
    }, py::arg("numSteps"), py::arg("scale") = 1,
       "Run a Phase 2 basic neural computation experiment and return spike count");

    m.def("runPhase6IntegrationDemo", []() {
        Phase6IntegratedExperiment experiment;
        
        if (!experiment.verifyIntegration()) {
            throw std::runtime_error("Integration verification failed");
        }
        
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        py::dict demoResult;
        demoResult["totalReward"] = result.totalReward;
        demoResult["avgFiringRate"] = result.avgFiringRate;
        demoResult["memoryEpisodesStored"] = result.memoryEpisodesStored;
        demoResult["dopamineLevel"] = result.dopamineLevel;
        demoResult["workingMemoryIntegrated"] = result.memoryWorkingMemoryIntegrated;
        demoResult["episodicMemoryIntegrated"] = result.memoryEpisodicMemoryIntegrated;
        demoResult["neuromodulationIntegrated"] = result.neuromodulationIntegrated;
        demoResult["predictionIntegrated"] = result.predictionIntegrated;
        demoResult["developmentIntegrated"] = result.developmentIntegrated;
        demoResult["checkpointingWorks"] = result.checkpointingWorks;
        
        return demoResult;
    }, "Run Phase 6 integration demo and return results");

    m.def("getVersionInfo", []() {
        py::dict versionInfo;
        versionInfo["major"] = 0;
        versionInfo["minor"] = 1;
        versionInfo["patch"] = 0;
        versionInfo["phase"] = "6 - Final Integration";
        versionInfo["description"] = "Neural Learning Machine - Artificial Developmental Brain";
        return versionInfo;
    }, "Get version information about the NLM system");

    m.def("getSystemCapabilities", []() {
        py::dict capabilities;
        capabilities["phases_supported"] = std::vector<int>{2, 3, 4, 6};
        capabilities["memory_systems"] = std::vector<std::string>{
            "working_memory", "episodic_memory", "associative_memory"
        };
        capabilities["neuromodulation_systems"] = std::vector<std::string>{
            "dopamine", "curiosity", "novelty", "prediction_error"
        };
        capabilities["plasticity_mechanisms"] = std::vector<std::string>{
            "stdp", "hebbian", "structural"
        };
        capabilities["cognition_systems"] = std::vector<std::string>{
            "planner", "concept_formation", "attention"
        };
        capabilities["sensory_systems"] = std::vector<std::string>{
            "vision", "audio", "internal_signals", "proprioception"
        };
        capabilities["motor_systems"] = std::vector<std::string>{
            "action_selection", "reward_modulation"
        };
        capabilities["development_stages"] = std::vector<std::string>{
            "initial", "critical_period", "maturation", "adult", "aging"
        };
        return capabilities;
    }, "Get information about supported system capabilities");

    // Exception definitions for Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Configuration for exception translation
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::logic_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

} // namespace nlm
