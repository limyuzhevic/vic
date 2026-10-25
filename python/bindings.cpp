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
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validate", &SensoryInput::validate, "Validate the sensory input")
        .def("getError", &SensoryInput::getError, "Get validation error message");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get vision channels")
        .def("isValid", &Vision::isValid, "Check if vision data is valid")
        .def("resize", [](Vision& self, size_t width, size_t height, size_t channels) {
            self.resize(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Resize vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("isValid", &Audio::isValid, "Check if audio data is valid");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            if (std::isinf(value) || std::isnan(value)) {
                throw py::value_error("Signal value cannot be NaN or infinite");
            }
            self.addSignal(value);
        }, py::arg("value"), "Add internal signal with validation")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of internal signals")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"), "Get signal at index")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals")
        .def("isValid", &InternalSignals::isValid, "Check if internal signals are valid");

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

    // Enhanced Python convenience functions with comprehensive error handling and validation
    m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, uint64_t steps) {
        // Validate inputs
        if (!brain) {
            throw py::value_error("Brain must not be null");
        }
        if (!world) {
            throw py::value_error("World must not be null");
        }
        if (steps == 0) {
            throw py::value_error("Number of steps must be greater than 0");
        }
        
        // Initialize agent with world
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all subsystems for complete functionality
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run simulation with validation
        for (uint64_t step = 0; step < steps; ++step) {
            try {
                // Get observation from world
                const auto& percept = world->getSensoryPercept();
                
                // Process sensory input through agent
                agent->processSensoryInput(percept);
                
                // Brain computational step
                brain->step(step, step * 0.001);
                
                // Get motor command from agent's brain processing
                MotorCommand cmd = agent->decodeMotorCommand();
                
                // Apply action to world and get reward
                world->applyAction(brain.get(), cmd, step * 0.001);
                
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Simulation error at step ") + std::to_string(step) + ": " + std::string(e.what()));
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run a complete agent simulation with brain and world. Validates inputs and provides detailed error messages for debugging.")

    m.def("runSimpleBrainSimulation", [](std::shared_ptr<Brain> brain, uint64_t steps) {
        // Validate inputs
        if (!brain) {
            throw py::value_error("Brain must not be null");
        }
        if (steps == 0) {
            throw py::value_error("Number of steps must be greater than 0");
        }
        
        // Run brain simulation with validation
        for (uint64_t step = 0; step < steps; ++step) {
            try {
                brain->step(step, step * 0.001);
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain simulation error at step ") + std::to_string(step) + ": " + std::string(e.what()));
            }
        }
    }, py::arg("brain"), py::arg("steps"),
       "Run a simple brain simulation. Validates inputs and provides detailed error messages for debugging.")

    m.def("saveBrainToFile", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Validate inputs
        if (!brain) {
            throw py::value_error("Brain must not be null");
        }
        if (filepath.empty()) {
            throw py::value_error("Filepath cannot be empty");
        }
        
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to save brain to file ") + filepath + ": " + std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file with comprehensive error handling. Validates inputs and provides detailed error messages for troubleshooting.")

    m.def("loadBrainFromFile", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Validate inputs
        if (!brain) {
            throw py::value_error("Brain must not be null");
        }
        if (filepath.empty()) {
            throw py::value_error("Filepath cannot be empty");
        }
        
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to load brain from file ") + filepath + ": " + std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file with comprehensive error handling. Validates inputs and provides detailed error messages for troubleshooting.")

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            auto config = std::make_shared<Config>();
            
            // Set sensible defaults for a working brain simulation
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            
            // STDP parameters for learning
            config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
            config->set("stdp_tau", 20.0f, ConfigSource::Default);
            
            // Plasticity parameters
            config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config->set("pruning_rate", 0.00001f, ConfigSource::Default);
            
            return config;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to create default configuration: ") + std::string(e.what()));
        }
    }, "Create a default configuration with sensible defaults for a working brain simulation. Automatically initializes with optimal parameters for learning and development.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        // Validate configuration
        if (!config) {
            throw py::value_error("Configuration must not be null");
        }
        
        try {
            // Create brain with configuration
            auto brain = std::make_shared<Brain>(config);
            
            // Initialize brain - this is critical for the brain to function
            if (!brain->initialize()) {
                throw py::runtime_error("Failed to initialize brain. Check configuration parameters such as neuron_count, region_count, and connection_probability.");
            }
            
            return brain;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to create brain: ") + std::string(e.what()));
        }
    }, py::arg("config"), "Create a new brain with configuration. Automatically initializes the brain and validates configuration parameters. Throws detailed error messages if initialization fails.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            auto world = std::make_shared<SimpleWorld>();
            
            // Configure with optimal settings for agent simulation
            world->configure(20.0f, 20.0f, 16, 16);  // 20x20 world with 16x16 vision
            world->setRandomSeed(42);  // For reproducible results
            
            return world;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to create simple world: ") + std::string(e.what()));
        }
    }, "Create a new simple world with optimal default settings. Configures a 20x20 world with 16x16 vision and reproducible random seed for consistent agent simulations.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        // Validate brain parameter
        if (!brain) {
            throw py::value_error("Brain must not be null when creating agent brain");
        }
        
        try {
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to create agent brain: ") + std::string(e.what()));
        }
    }, py::arg("brain"), "Create a new agent brain interface. Validates that brain is not null and provides detailed error messages for troubleshooting.")

    // Comprehensive validation and utility functions
    m.def("validateBrain", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain must not be null for validation");
        }
        
        try {
            // Check if brain is properly initialized
            auto config = brain->getConfig();
            if (!config) {
                throw py::runtime_error("Brain configuration is missing - brain may not be properly initialized");
            }
            
            // Validate configuration parameters
            auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                throw py::runtime_error("Brain configuration has invalid neuron_count: " + std::to_string(neuronCount));
            }
            
            // Check basic brain functionality
            size_t totalNeurons = brain->getTotalNeuronCount();
            if (totalNeurons == 0) {
                throw py::runtime_error("Brain has no neurons despite configuration specifying " + std::to_string(neuronCount));
            }
            
            // Verify connectivity
            size_t totalSynapses = brain->getTotalSynapseCount();
            if (totalSynapses == 0) {
                throw py::runtime_error("Brain has no synapses - check connection_probability in configuration");
            }
            
            return true;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Brain validation failed: ") + std::string(e.what()));
        }
    }, py::arg("brain"), "Validate brain configuration and state. Performs comprehensive validation including configuration checks, neuron/synapse counts, and connectivity. Throws detailed exception if validation fails.")

    m.def("validateWorld", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw py::value_error("World must not be null for validation");
        }
        
        try {
            // Check world dimensions
            float width = world->getWidth();
            float height = world->getHeight();
            
            if (width <= 0) {
                throw py::runtime_error("World width must be positive, got: " + std::to_string(width));
            }
            if (height <= 0) {
                throw py::runtime_error("World height must be positive, got: " + std::to_string(height));
            }
            
            // Check agent state
            const auto& agent = world->getAgentBody();
            
            if (agent.energy < 0 || agent.energy > world->getMaxEnergy()) {
                throw py::runtime_error("Agent energy out of valid range [0, " + std::to_string(world->getMaxEnergy()) + "], got: " + std::to_string(agent.energy));
            }
            
            if (agent.health <= 0) {
                throw py::runtime_error("Agent health must be positive, got: " + std::to_string(agent.health));
            }
            
            // Check agent position is within world bounds
            if (agent.x < 0 || agent.x >= width || agent.y < 0 || agent.y >= height) {
                throw py::runtime_error("Agent position out of bounds: (" + std::to_string(agent.x) + ", " + std::to_string(agent.y) + ") in world [0, " + std::to_string(width) + ") x [0, " + std::to_string(height) + ")");
            }
            
            return true;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("World validation failed: ") + std::string(e.what()));
        }
    }, py::arg("world"), "Validate world configuration and state. Performs comprehensive validation including dimensions, agent state, and position bounds. Throws detailed exception if validation fails.")

    m.def("resetBrain", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain must not be null for reset operation");
        }
        
        try {
            brain->reset();
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to reset brain: ") + std::string(e.what()));
        }
    }, py::arg("brain"), "Reset brain state to initial configuration. Validates input and provides detailed error messages for troubleshooting. Useful for restarting simulations.")

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain must not be null for statistics");
        }
        
        try {
            py::dict stats;
            stats["neuron_count"] = static_cast<int>(brain->getTotalNeuronCount());
            stats["synapse_count"] = static_cast<int>(brain->getTotalSynapseCount());
            stats["firing_neuron_count"] = static_cast<int>(brain->getFiringNeuronCount());
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["total_spikes"] = static_cast<int>(brain->getTotalSpikeCount());
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            return stats;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to get brain statistics: ") + std::string(e.what()));
        }
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary. Includes neuron counts, firing rates, spike statistics, and developmental stage. Validates input and handles all potential errors.")

    m.def("getWorldStats", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw py::value_error("World must not be null for statistics");
        }
        
        try {
            py::dict stats;
            stats["width"] = world->getWidth();
            stats["height"] = world->getHeight();
            stats["simulation_time"] = world->getSimulationTime();
            stats["max_energy"] = world->getMaxEnergy();
            stats["energy_decay_rate"] = world->getEnergyDecayRate();
            
            const auto& agent = world->getAgentBody();
            stats["agent_x"] = agent.x;
            stats["agent_y"] = agent.y;
            stats["agent_orientation"] = agent.orientation;
            stats["agent_energy"] = agent.energy;
            stats["agent_health"] = agent.health;
            stats["agent_age"] = agent.age;
            stats["agent_is_moving"] = agent.isMoving;
            stats["agent_is_turning"] = agent.isTurning;
            
            return stats;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to get world statistics: ") + std::string(e.what()));
        }
    }, py::arg("world"), "Get comprehensive world statistics as a dictionary. Includes world dimensions, simulation time, agent state (position, energy, health, age, movement), and configuration. Validates input and handles all potential errors.")

    m.def("resetAgent", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw py::value_error("Agent must not be null for reset operation");
        }
        
        try {
            agent->reset();
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to reset agent: ") + std::string(e.what()));
        }
    }, py::arg("agent"), "Reset agent state to initial configuration. Validates input and provides detailed error messages for troubleshooting. Useful for restarting agent simulations.")

    m.def("getAgentStats", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw py::value_error("Agent must not be null for statistics");
        }
        
        try {
            py::dict stats;
            stats["curiosity_level"] = agent->getCuriosityLevel();
            stats["novelty_level"] = agent->getNoveltyLevel();
            stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
            stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent->isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent->isCuriosityEnabled();
            
            return stats;
            
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to get agent statistics: ") + std::string(e.what()));
        }
    }, py::arg("agent"), "Get comprehensive agent statistics as a dictionary. Includes neuromodulation levels, curiosity/novelty, developmental stage, and subsystem enable flags. Validates input and handles all potential errors.")

    m.def("enableAllSubsystems", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw py::value_error("Agent must not be null for subsystem configuration");
        }
        
        try {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to enable all subsystems: ") + std::string(e.what()));
        }
    }, py::arg("agent"), "Enable all agent subsystems (reward modulation, structural plasticity, development, curiosity). Validates input and provides detailed error messages for troubleshooting.")

    m.def("disableAllSubsystems", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw py::value_error("Agent must not be null for subsystem configuration");
        }
        
        try {
            agent->enableRewardModulation(false);
            agent->enableStructuralPlasticity(false);
            agent->enableDevelopment(false);
            agent->enableCuriosity(false);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Failed to disable all subsystems: ") + std::string(e.what()));
        }
    }, py::arg("agent"), "Disable all agent subsystems (reward modulation, structural plasticity, development, curiosity). Validates input and provides detailed error messages for troubleshooting.")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
