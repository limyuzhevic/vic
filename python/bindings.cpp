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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
.def("loadFromArgs", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"),
              "Load configuration from command line arguments (argc, argv)")
         .def("loadFromArgsPython", [](Config& self, const std::vector<std::string>& args) {
             std::vector<char*> argv;
             argv.reserve(args.size() + 1);
             argv.push_back(nullptr); // argv[0] should be program name
             for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
             int argc = static_cast<int>(argv.size());
             return self.loadFromArgs(argc, argv.data());
         }, py::arg("args"),
            "Load configuration from command line arguments (Python-friendly version)")
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // NEW ADVANCED PYTHON API FOR EXPERT USERS
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get firing rates of internal neurons")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence (0.0-1.0)")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get number of social observations")
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
             "Encode text string as sensory pattern (Phase 6 feature)")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get number of learned concepts")
        .def("getAgentBody", &AgentBrain::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get complete agent body state")
        .def("setAgentStart", &AgentBrain::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position in world")
        .def("getConfig", &AgentBrain::getConfig,
             py::return_value_policy::reference_internal,
             "Get agent configuration")
        .def("printDebugInfo", [](const AgentBrain& self) {
            self.getBrain()->logStatus();
            // Print additional agent-specific debug info
            std::cout << "=== AgentBrain Debug ===" << std::endl;
            std::cout << "  RewardModulation: " << (self.isRewardModulationEnabled() ? "ON" : "OFF") << std::endl;
            std::cout << "  StructuralPlasticity: " << (self.isStructuralPlasticityEnabled() ? "ON" : "OFF") << std::endl;
            std::cout << "  Development: " << (self.isDevelopmentEnabled() ? "ON" : "OFF") << std::endl;
            std::cout << "  Curiosity: " << (self.isCuriosityEnabled() ? "ON" : "OFF") << std::endl;
            std::cout << "  InternalNeurons: " << self.getInternalFiringRates().size() << std::endl;
            std::cout << "  Concepts: " << self.getConceptCount() << std::endl;
        }, "Print comprehensive debug information")
        .def("benchmarkEpisode", [](AgentBrain& self, SimpleWorld& world, int steps) {
            std::vector<float> rewards;
            for (int i = 0; i < steps; ++i) {
                world.update(0.1);
                SensoryPercept percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                // Get underlying brain - need access to brain's parent
                // This is a simplified version
                MotorCommand cmd = self.decodeMotorCommand();
                world.applyMotorCommand(cmd, world.getSimulationTime());
                float reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0f;
                self.applyRewardModulation(reward, 0.0f);
                rewards.push_back(reward);
            }
            float avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
            return std::make_tuple(avgReward, rewards.size());
        }, py::arg("world"), py::arg("steps"),
            "Run a benchmark episode and return (average_reward, steps)")
        .def("exportBrainState", [](AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            return brain->save(filepath);
        }, py::arg("filepath"),
            "Export brain state to checkpoint file (returns success)")
        .def("importBrainState", [](AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            return brain->load(filepath);
        }, py::arg("filepath"),
            "Import brain state from checkpoint file (returns success)");

    // ADVANCED PYTHON API FOR EXPERT USERS
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for a more capable simulation
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 8, ConfigSource::Default);
        config->set("connection_probability", 0.15, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create an advanced configuration with optimized settings for expert users");

    // Helper functions for enhanced configuration management
    m.def("quickCreateConfig", [](int neuronCount = 1000, int regionCount = 4) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, ConfigSource::Default);
        config->set("region_count", regionCount, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 4,
       "Create a quick configuration with specified neuron and region counts");

    // Visualization and debugging utilities
    m.def("createVisualizationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("visualization.enable", true, ConfigSource::Default);
        config->set("visualization.output_freq", 100, ConfigSource::Default);
        config->set("visualization.performance_tracking", true, ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for visualization");

    // Performance and scalability helpers
    m.def("createScalableConfig", [](int numRegions = 8, float connectivity = 0.12f) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 5000, ConfigSource::Default);
        config->set("region_count", numRegions, ConfigSource::Default);
        config->set("connection_probability", connectivity, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        return config;
    }, py::arg("numRegions") = 8, py::arg("connectivity") = 0.12f,
       "Create a scalable configuration for large simulations");

    // Experimental setup helpers
    m.def("createResearchConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 10000, ConfigSource::Default);
        config->set("region_count", 16, ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.008f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.016f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        config->set("development.stage.synaptogenesis.enabled", true, ConfigSource::Default);
        config->set("development.stage.pruning.enabled", true, ConfigSource::Default);
        config->set("development.stage.maturation.enabled", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.2f, ConfigSource::Default);
        return config;
    }, "Create a research-grade configuration for experimental studies");

    // Validation and diagnostic functions
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) errors.push_back("Missing neuron_count");
        else {
            auto countOpt = config->get<int64_t>("neuron_count");
            if (countOpt && *countOpt < 100) errors.push_back("Neuron count too low (< 100)");
            else if (countOpt && *countOpt > 100000) errors.push_back("Neuron count too high (> 100,000)");
        }
        
        if (!config->has("region_count")) errors.push_back("Missing region_count");
        
        if (!config->has("connection_probability")) errors.push_back("Missing connection_probability");
        else {
            auto probOpt = config->get<double>("connection_probability");
            if (probOpt && (*probOpt < 0.0 || *probOpt > 1.0)) 
                errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
        
        return std::make_pair(errors.empty(), errors);
    }, py::arg("config"), "Validate configuration and return (is_valid, error_messages)");

    // Configuration merging and comparison
    m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& override) {
        auto result = std::make_shared<Config>();
        
        // Copy all keys from base config
        auto baseKeys = base->getKeys();
        for (const auto& key : baseKeys) {
            if (auto valueOpt = base->get<ConfigValue>(key)) {
                result->set(key, *valueOpt, ConfigSource::File);
            }
        }
        
        // Override with override config values
        auto overrideKeys = override->getKeys();
        for (const auto& key : overrideKeys) {
            if (auto valueOpt = override->get<ConfigValue>(key)) {
                result->set(key, *valueOpt, ConfigSource::CommandLine);
            }
        }
        
        return result;
    }, py::arg("base"), py::arg("override"), "Merge two configurations, with override taking precedence");

    // Configuration statistics and summary
    m.def("getConfigStats", [](const std::shared_ptr<Config>& config) {
        std::map<std::string, std::string> stats;
        
        stats["total_keys"] = std::to_string(config->getKeys().size());
        
        auto keys = config->getKeys();
        int intCount = 0, floatCount = 0, boolCount = 0, strCount = 0, otherCount = 0;
        
        for (const auto& key : keys) {
            if (auto valOpt = config->get<int64_t>(key)) intCount++;
            else if (auto valOpt = config->get<double>(key)) floatCount++;
            else if (auto valOpt = config->get<bool>(key)) boolCount++;
            else if (auto valOpt = config->get<std::string>(key)) strCount++;
            else otherCount++;
        }
        
        stats["int_values"] = std::to_string(intCount);
        stats["float_values"] = std::to_string(floatCount);
        stats["bool_values"] = std::to_string(boolCount);
        stats["string_values"] = std::to_string(strCount);
        stats["other_values"] = std::to_string(otherCount);
        
        return stats;
    }, py::arg("config"), "Get statistics about configuration contents");

    // Quick setup helpers for different simulation scenarios
    m.def("createExplorationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 6, ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        return config;
    }, "Create configuration optimized for exploration/learning scenarios");

    m.def("createExploitationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 3000, ConfigSource::Default);
        config->set("region_count", 8, ConfigSource::Default);
        config->set("connection_probability", 0.18f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", false, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        return config;
    }, "Create configuration optimized for exploitation/stabilization scenarios");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
