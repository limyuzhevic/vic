#include <cstdint>

// Type definitions for better Python interoperability
using SimulationStep = uint64_t;
using Timestamp = double;
using TimestepDuration = double;
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;
using SynapticWeight = float;
using MembranePotential = float;
using FiringRate = float;
using Delay = uint32_t;

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
        })
        .def("is_valid", [](const NeuronId& id) { return id != INVALID_NEURON_ID; })
        .def_static("invalid", []() { return INVALID_NEURON_ID; });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("is_valid", [](const SynapseId& id) { return id != INVALID_SYNAPSE_ID; })
        .def_static("invalid", []() { return INVALID_SYNAPSE_ID; });

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
        })
        .def("is_valid", [](const RegionId& id) { return id != INVALID_REGION_ID; })
        .def_static("invalid", []() { return INVALID_REGION_ID; });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("is_valid", [](const PopulationId& id) { return id != INVALID_POPULATION_ID; })
        .def_static("invalid", []() { return INVALID_POPULATION_ID; });

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
        
        // ========== ENHANCED BRAIN CONVENIENCE METHODS ==========
        
        // Simulation convenience methods
        .def("stepSim", [](Brain& self, int steps) {
            for (int i = 0; i < steps; ++i) {
                self.step(i);
            }
        }, py::arg("steps"), "Perform multiple simulation steps")
        
        .def("stepWithTime", [](Brain& self, int steps, Timestamp startTime) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, startTime + i);
            }
        }, py::arg("steps"), py::arg("startTime"), "Perform multiple simulation steps with time")
        
        // Memory convenience methods
        .def("getAllMemories", [](Brain& self) {
            auto working = self.getWorkingMemory();
            auto episodic = self.getEpisodicMemory();
            auto associative = self.getAssociativeMemory();
            return std::make_tuple(working, episodic, associative);
        }, "Get all memory systems as tuple")
        
        // Action convenience methods
        .def("produceActionWithRetry", [](Brain& self, int maxRetries) {
            for (int i = 0; i < maxRetries; ++i) {
                auto action = self.produceAction();
                if (action) {
                    return action;
                }
            }
            return std::unique_ptr<Action>(nullptr);
        }, py::arg("maxRetries"), "Produce action with retry logic")
        
        // Developmental convenience methods
        .def("developToStage", &Brain::setDevelopmentalStage,
              py::arg("stage"), "Develop brain to specific stage")
        
        // Configuration convenience methods
        .def("setConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get configuration reference for direct modification")
        
        // Statistics convenience methods
        .def("getSystemStats", [](const Brain& self) {
            return std::make_tuple(
                self.getTotalNeuronCount(),
                self.getTotalSynapseCount(),
                self.getActiveNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getExcitationInhibitionRatio(),
                self.getTotalSpikeCount(),
                self.getPendingSpikeEventCount()
            );
        }, "Get comprehensive system statistics")
        
        .def("getMemoryStats", [](const Brain& self) {
            auto working = self.getWorkingMemory();
            auto episodic = self.getEpisodicMemory();
            auto associative = self.getAssociativeMemory();
            return std::make_tuple(
                working ? working->getMemoryCapacity() : 0,
                episodic ? episodic->getMemoryCapacity() : 0,
                associative ? associative->getMemoryCapacity() : 0
            );
        }, "Get memory system statistics")
        
        .def("getPlasticityStats", [](const Brain& self) {
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            return std::make_tuple(
                stdp ? stdp->getPlasticityRate() : 0.0f,
                hebbian ? hebbian->getPlasticityRate() : 0.0f,
                structural ? structural->getPlasticityRate() : 0.0f
            );
        }, "Get plasticity system statistics")
        
        .def("getNeuromodulationStats", [](const Brain& self) {
            auto dopamine = self.getDopamine();
            auto curiosity = self.getCuriosity();
            auto novelty = self.getNovelty();
            auto predictionError = self.getPredictionErrorSignal();
            return std::make_tuple(
                dopamine ? dopamine->getLevel() : 0.0f,
                curiosity ? curiosity->getLevel() : 0.0f,
                novelty ? novelty->getLevel() : 0.0f,
                predictionError ? predictionError->getError() : 0.0f
            );
        }, "Get neuromodulation system statistics")
        
        .def("getPredictionStats", [](const Brain& self) {
            auto prediction = self.getPredictionSystem();
            auto neuralPrediction = prediction ? prediction->getNeuralPrediction() : nullptr;
            return neuralPrediction ? std::make_tuple(
                neuralPrediction->getPredictionAccuracy(),
                neuralPrediction->getPredictionConfidence()
            ) : std::make_tuple(0.0f, 0.0f);
        }, "Get prediction system statistics")
        
        .def("getDevelopmentStats", [](const Brain& self) {
            auto development = self.getDevelopmentSystem();
            return development ? std::make_tuple(
                development->getDevelopmentalStage(),
                development->getAge(),
                development->getMaturationLevel()
            ) : std::make_tuple(DevelopmentalStage::Initial, 0.0, 0.0f);
        }, "Get development system statistics")
        
        // Convenience methods for world integration
        .def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create agent brain interface from existing brain")
        
        // Backup and restore convenience
        .def("backupAndSave", [](Brain& self, const std::string& filepath) {
            bool success = self.save(filepath);
            if (success) {
                self.logStatus();
            }
            return success;
        }, py::arg("filepath"), "Backup and save brain state")
        
        .def("loadAndValidate", [](Brain& self, const std::string& filepath) {
            bool success = self.load(filepath);
            if (success) {
                self.initialize();
            }
            return success;
        }, py::arg("filepath"), "Load and validate brain state")
        
        // Convenience factory methods
        .def_static("createDefaultConfigWithSettings", [](const std::string& configPath) {
            auto config = std::make_shared<Config>();
            config->loadFromFile(configPath);
            return config;
        }, py::arg("configPath"), "Create config with file loading")
        
        .def_static("createBrainWithConfig", [](const std::string& configPath) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->loadFromFile(configPath);
            return std::make_shared<Brain>(config);
        }, py::arg("configPath"), "Create brain with configuration from file");

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

    // ========== ENHANCED AGENT BRAIN CONVENIENCE METHODS ==========
    
    // Processing convenience methods
    .def("processBatchInputs", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
        for (const auto& percept : percepts) {
            self.processSensoryInput(percept);
        }
    }, py::arg("percepts"), "Process multiple sensory inputs in batch")
    
    .def("processSinglePercept", [](AgentBrain& self, const SensoryPercept& percept) {
        self.processSensoryInput(percept);
        self.decodeMotorCommand();
        return self.decodeMotorCommand();
    }, py::arg("percept"), "Process single percept and return action")
    
    // Neuromodulation convenience methods
    .def("applyRewardWithPrediction", &AgentBrain::applyRewardModulation,
          py::arg("reward"), py::arg("predictedReward"),
          "Apply reward modulation with prediction error")
    
    .def("applyCuriosityDrivenExploration", [](AgentBrain& self, float noveltyThreshold) {
        auto curiosity = self.getCuriosityLevel();
        if (curiosity > noveltyThreshold) {
            self.enableCuriosity(true);
        }
        return self.getCuriosityLevel();
    }, py::arg("noveltyThreshold"), "Enable curiosity-driven exploration")
    
    // Development convenience methods
    .def("advanceDevelopment", [](AgentBrain& self, double timestep) {
        self.updateDevelopment(timestep);
        return self.getDevelopmentalStage();
    }, py::arg("timestep"), "Advance development and return stage")
    
    .def("isDevelopmentReady", [](AgentBrain& self) {
        auto stage = self.getDevelopmentalStage();
        return stage >= DevelopmentalStage::Maturation;
    }, "Check if development is mature")
    
    // System status convenience methods
    .def("getBrainStatus", [](AgentBrain& self) {
        auto brain = self.getBrain();
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getActiveNeuronCount(),
            brain->getDevelopmentalStage()
        );
    }, "Get comprehensive brain status")
    
    .def("getNeuromodulationStatus", [](AgentBrain& self) {
        return std::make_tuple(
            self.getNeuromodulationLevel(),
            self.getCuriosityLevel(),
            self.getNoveltyLevel(),
            self.getPredictionError()
        );
    }, "Get neuromodulation status")
    
    .def("getDevelopmentStatus", [](AgentBrain& self) {
        return std::make_tuple(
            self.getDevelopmentalStage(),
            self.getNeuromodulationLevel(),
            self.getCuriosityLevel()
        );
    }, "Get development status")
    
    // Convenience methods for world interaction
    .def("connectToWorld", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
        self.initialize(*world);
        return self.getBrain();
    }, py::arg("world"), "Connect agent brain to world")
    
    .def("processEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                            const std::vector<SensoryPercept>& observations,
                            const std::vector<float>& rewards,
                            double timestep) {
        self.initialize(*world);
        for (size_t i = 0; i < observations.size(); ++i) {
            self.processSensoryInput(observations[i]);
            if (i < rewards.size()) {
                self.applyRewardModulation(rewards[i], 0.0f);
            }
            self.updateDevelopment(timestep);
        }
        auto action = self.decodeMotorCommand();
        return std::make_tuple(action, self.getPredictionError());
    }, py::arg("world"), py::arg("observations"), py::arg("rewards"), py::arg("timestep"),
         "Process complete episode and return final action and prediction error")
    
    // Convenience factory methods
    .def_static("createAgentWithBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain from existing brain")
    
    .def_static("createAgentWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<AgentBrain> {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("config"), "Create agent brain with configuration")
    
.def_static("createAgentWithWorld", [](std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("world"), "Create agent brain with world");

    // Context manager for brain simulation sessions
    py::class_<BrainSessionManager>(m, "BrainSessionManager", 
              R"pbdoc(Context manager for brain simulation sessions with automatic cleanup)pbdoc")
        .def(py::init<>())
        .def("create_session", [](BrainSessionManager& self, std::shared_ptr<Config> config) {
            auto brain = std::make_shared<Brain>(config);
            auto session = std::make_shared<BrainSession>(brain);
            self.sessions_.push_back(session);
            return session;
        }, py::arg("config"), "Create a new simulation session")
        .def("run_session", [](BrainSessionManager& self, size_t sessionIndex, 
                             const std::vector<SensoryPercept>& inputs,
                             double timestep = 0.1) {
            if (sessionIndex < self.sessions_.size()) {
                auto& session = self.sessions_[sessionIndex];
                session->brain_->initialize();
                for (const auto& input : inputs) {
                    session->brain_->receiveSensoryInput(input);
                    session->brain_->step(session->stepCounter_++);
                }
                return session->brain_->produceAction();
            }
            return std::unique_ptr<Action>(nullptr);
        }, py::arg("sessionIndex"), py::arg("inputs"), py::arg("timestep") = 0.1,
             "Run a simulation session with given inputs")
        .def("cleanup_session", [](BrainSessionManager& self, size_t sessionIndex) {
            if (sessionIndex < self.sessions_.size()) {
                self.sessions_.erase(self.sessions_.begin() + sessionIndex);
                return true;
            }
            return false;
        }, py::arg("sessionIndex"), "Clean up and remove a session")
        .def("get_session_count", [](BrainSessionManager& self) {
            return self.sessions_.size();
        }, "Get number of active sessions")
        .def("cleanup_all_sessions", [](BrainSessionManager& self) {
            self.sessions_.clear();
        }, "Clean up all sessions");

    // Context manager for agent brain sessions
    py::class_<AgentBrainSessionManager>(m, "AgentBrainSessionManager",
              R"pbdoc(Context manager for agent brain sessions with automatic cleanup)pbdoc")
        .def(py::init<>())
        .def("create_session", [](AgentBrainSessionManager& self, std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            auto session = std::make_shared<AgentBrainSession>(agent, world);
            self.sessions_.push_back(session);
            return session;
        }, py::arg("brain"), py::arg("world"), "Create a new agent brain session")
        .def("run_session", [](AgentBrainSessionManager& self, size_t sessionIndex,
                             const std::vector<SensoryPercept>& observations,
                             const std::vector<float>& rewards,
                             double timestep = 0.1) {
            if (sessionIndex < self.sessions_.size()) {
                auto& session = self.sessions_[sessionIndex];
                for (size_t i = 0; i < observations.size(); ++i) {
                    session->agent_->processSensoryInput(observations[i]);
                    if (i < rewards.size()) {
                        session->agent_->applyRewardModulation(rewards[i], 0.0f);
                    }
                    session->agent_->updateDevelopment(timestep);
                }
                return std::make_tuple(session->agent_->decodeMotorCommand(),
                                     session->agent_->getPredictionError());
            }
            return std::make_tuple(MotorCommand::Wait, 0.0f);
        }, py::arg("sessionIndex"), py::arg("observations"), py::arg("rewards"), py::arg("timestep") = 0.1,
             "Run an agent brain session with given observations and rewards")
        .def("cleanup_session", [](AgentBrainSessionManager& self, size_t sessionIndex) {
            if (sessionIndex < self.sessions_.size()) {
                self.sessions_.erase(self.sessions_.begin() + sessionIndex);
                return true;
            }
            return false;
        }, py::arg("sessionIndex"), "Clean up and remove an agent brain session")
        .def("get_session_count", [](AgentBrainSessionManager& self) {
            return self.sessions_.size();
        }, "Get number of active agent brain sessions")
        .def("cleanup_all_sessions", [](AgentBrainSessionManager& self) {
            self.sessions_.clear();
        }, "Clean up all agent brain sessions");

    // Expose invalid ID constants for Python users
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
