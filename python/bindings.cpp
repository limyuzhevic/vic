#include <limits>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"

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

    // Type conversion helpers
    m.def("neuron_id_from_index", [](size_t index) {
        NeuronId id;
        id.value = index;
        return id;
    }, py::arg("index"), "Create a NeuronId from an index");

    m.def("neuron_id_to_index", [](const NeuronId& neuron_id) {
        return neuron_id.value;
    }, py::arg("neuron_id"), "Get index from NeuronId");

    m.def("region_id_from_name", [](const std::string& name) {
        RegionId id;
        std::hash<std::string> hasher;
        id.value = hasher(name) % (std::numeric_limits<uint64_t>::max() / 2);
        return id;
    }, py::arg("name"), "Create a RegionId from a name");

    m.def("action_type_from_string", [](const std::string& name) {
        static const std::unordered_map<std::string, ActionType> action_map = {
            {"MoveForward", ActionType::MoveForward},
            {"MoveBackward", ActionType::MoveBackward},
            {"MoveLeft", ActionType::MoveLeft},
            {"MoveRight", ActionType::MoveRight},
            {"TurnLeft", ActionType::TurnLeft},
            {"TurnRight", ActionType::TurnRight},
            {"Look", ActionType::Look},
            {"LookUp", ActionType::LookUp},
            {"LookDown", ActionType::LookDown},
            {"Interact", ActionType::Interact},
            {"Eat", ActionType::Eat},
            {"Drink", ActionType::Drink},
            {"Rest", ActionType::Rest},
            {"Wait", ActionType::Wait},
            {"Custom", ActionType::Custom}
        };
        auto it = action_map.find(name);
        if (it != action_map.end()) {
            return it->second;
        }
        throw std::runtime_error("Unknown action type: " + name);
    }, py::arg("name"), "Convert string to ActionType");

    // ============ ERROR HANDLING UTILITIES ============

    m.def("check_memory_available", [](size_t required_memory_bytes) {
        // Simple memory check - in a real implementation, this would check system memory
        static const size_t available_memory = 1024 * 1024 * 1024; // 1GB (placeholder)
        return required_memory_bytes <= available_memory;
    }, py::arg("required_memory_bytes"), "Check if sufficient memory is available");

    m.def("check_system_initialized", [](const std::shared_ptr<Brain>& brain) {
        return brain && brain->getConfig();
    }, py::arg("brain"), "Check if a brain system is initialized");

    m.def("safe_step", [](std::shared_ptr<Brain>& brain, SimulationStep step, Timestamp time) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        try {
            if (time > 0) {
                brain->step(step, time);
            } else {
                brain->step(step);
            }
            return true;
        } catch (const std::exception& e) {
            // Log the error and return false
            return false;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"), "Safely execute a brain simulation step with error handling");

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

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)
        Implements working memory using neural dynamics and persistent activity.
        Features:
        - Persistent neural activity patterns for short-term storage
        - Recurrent connections for maintenance
        - Activity-dependent plasticity
        - Neural competition for selective retention
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"),
             "Initialize the working memory with a brain reference")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store a pattern in working memory (pattern: List[float], strength: float = 1.0)")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation directly to a neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as list of activations")
        .def("contains", &NeuralWorkingMemory::contains,
             py::arg("neuron"),
             "Check if a neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"),
             "Update working memory state (dt: timestep duration)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear all working memory content")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get memory trace decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"),
             "Set memory trace decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"),
             "Strengthen memory representations (factor: float)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition to select winning neurons")
        .def("isWinning", &NeuralWorkingMemory::isWinning,
             py::arg("neuron"),
             "Check if a neuron is part of the winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

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
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the neural working memory system");

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

    // Convenience methods for Agent
    m.def("getAllNeuromodulators", [](const std::shared_ptr<AgentBrain>& agent) {
        py::dict result;
        if (agent->getBrain()) {
            if (auto dopamine = agent->getBrain()->getDopamine()) {
                result["dopamine"] = std::shared_ptr<Dopamine>(dopamine,
                    [agent](Dopamine* ptr) {});
            }
            if (auto curiosity = agent->getBrain()->getCuriosity()) {
                result["curiosity"] = std::shared_ptr<Curiosity>(curiosity,
                    [agent](Curiosity* ptr) {});
            }
            if (auto novelty = agent->getBrain()->getNovelty()) {
                result["novelty"] = std::shared_ptr<Novelty>(novelty,
                    [agent](Novelty* ptr) {});
            }
            if (auto pred_err = agent->getBrain()->getPredictionErrorSignal()) {
                result["prediction_error"] = std::shared_ptr<PredictionError>(pred_err,
                    [agent](PredictionError* ptr) {});
            }
        }
        return result;
    }, py::arg("agent"), "Get all neuromodulators from an agent brain");

    m.def("enableAllNeuromodulators", [](std::shared_ptr<AgentBrain>& agent, bool enable) {
        agent->enableRewardModulation(enable);
        agent->enableStructuralPlasticity(enable);
        agent->enableDevelopment(enable);
        agent->enableCuriosity(enable);
    }, py::arg("agent"), py::arg("enable"), "Enable or disable all neuromodulators");

    m.def("getSystemStatus", [](const std::shared_ptr<AgentBrain>& agent) {
        py::dict status;
        status["reward_modulation"] = agent->isRewardModulationEnabled();
        status["structural_plasticity"] = agent->isStructuralPlasticityEnabled();
        status["development"] = agent->isDevelopmentEnabled();
        status["curiosity"] = agent->isCuriosityEnabled();
        status["dopamine_level"] = agent->getNeuromodulationLevel();
        status["curiosity_level"] = agent->getCuriosityLevel();
        status["novelty_level"] = agent->getNoveltyLevel();
        status["prediction_error"] = agent->getPredictionError();
        return status;
    }, py::arg("agent"), "Get system status of an agent brain");

    // ============ UTILITY FUNCTIONS ============

    m.def("createExperimentConfig", [](const std::vector<std::string>& system_configs) {
        auto config = std::make_shared<Config>();
        // Parse system configs and apply them
        for (const auto& sys_config : system_configs) {
            // Simple key-value parsing for demonstration
            size_t pos = sys_config.find('=');
            if (pos != std::string::npos) {
                std::string key = sys_config.substr(0, pos);
                std::string value = sys_config.substr(pos + 1);
                config->set(key, value);
            }
        }
        return config;
    }, py::arg("system_configs"), "Create an experiment configuration from system settings");

    m.def("runIntegratedSimulation", [](std::shared_ptr<Brain>& brain, std::shared_ptr<SimpleWorld>& world, int steps) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world is null");
        }
        brain->initialize();
        world->reset();
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1f);
            brain->step(i, world->getSimulationTime());
            if (i % 10 == 0) {
                brain->logStatus();
            }
        }
        return steps;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), "Run an integrated simulation with brain and world");

    m.def("createBrainWithDefaults", [](const std::unordered_map<std::string, std::string>& config_updates) {
        auto config = std::make_shared<Config>();
        // Apply default config values
        config->set("brain.boundary.random_seed", "42");
        config->set("brain.simulation.dt", "0.1");
        config->set("brain.simulation.max_steps", "1000");
        
        // Apply user updates
        for (const auto& [key, value] : config_updates) {
            config->set(key, value);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_updates"), "Create a brain with default configuration and optional updates");

    m.def("createAgentWithBrain", [](std::shared_ptr<Brain>& brain, std::shared_ptr<SimpleWorld>& world) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world is null");
        }
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent with a brain and world");

    // ============ NEURAL PLANNER ==============
    
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning and decision making)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize the neural planner with a brain reference")
        .def("plan_action", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan the next action given current state and target reward")
        .def("evaluate_sequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence and return expected rewards")
        .def("get_planning_depth", &NeuralPlanner::getPlanningDepth,
             "Get the number of steps to look ahead in planning")
        .def("set_planning_depth", &NeuralPlanner::setPlanningDepth,
             py::arg("depth"), "Set the planning depth")
        .def("get_planning_confidence", &NeuralPlanner::getPlanningConfidence,
             "Get the current planning confidence")
        .def("update_plan_quality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcomes")
        .def("clear_cache", &NeuralPlanner::clearCache,
             "Clear the planning cache")
        .def("set_action_quality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set the quality of an action from experience")
        .def("get_current_goal", &NeuralPlanner::getCurrentGoal,
             "Get the current goal state")
        .def("set_current_goal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set the current goal state")
        .def("was_recent_plan_successful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful")
        .def("get_current_goal", &NeuralPlanner::getCurrentGoal,
             "Get the current goal state (Python)")
        .def("set_current_goal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set the current goal state (Python)");

    // ============ CONCEPT FORMATION ==============
    
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for pattern discovery and category formation)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize the concept formation system with a brain reference")
        .def("learn_concept", &ConceptFormation::presentExperience,
             py::arg("neuralPattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Learn a concept from experience (legacy method)")
        .def("classify", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("threshold") = 0.7f,
             "Classify a pattern into an existing concept")
        .def("get_concept_vector", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"),
             "Get the prototype vector for a concept")
        .def("get_similar_concepts", [](const ConceptFormation& cf, const std::vector<float>& pattern, float threshold) {
             // Get all concepts and filter by similarity
             const auto& concepts = cf.getConcepts();
             std::vector<size_t> similarIds;
             for (size_t i = 0; i < concepts.size(); ++i) {
                 float similarity = cf.computeSimilarity(pattern, concepts[i].prototype);
                 if (similarity >= threshold) {
                     similarIds.push_back(i);
                 }
             }
             return similarIds;
         }, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get IDs of concepts similar to the given pattern")
        .def("get_concept_count", &ConceptFormation::getConceptCount,
             "Get the number of discovered concepts")
        .def("clear_concepts", &ConceptFormation::clear,
             "Clear all concepts");

    // ============ DEVELOPMENT SYSTEM ==============
    
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for developmental stages and plasticity modulation)pbdoc")
        .def(py::init<>())
        .def("get_stage", &DevelopmentSystem::getStage,
             "Get the current developmental stage")
        .def("set_stage", &DevelopmentSystem::setStage,
             py::arg("stage"), "Set the developmental stage")
        .def("advance_stage", &DevelopmentSystem::advanceStage,
             "Advance to the next developmental stage")
        .def("get_stage_name", &DevelopmentSystem::getStageName,
             "Get the name of the current developmental stage")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"),
             "Update development (simple version)")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("get_plasticity_modifier", &DevelopmentSystem::getPlasticityModifier,
             "Get the plasticity modifier for the current stage")
        .def("is_critical_period", &DevelopmentSystem::isCriticalPeriod,
             "Check if we're in a critical period for learning")
        .def("get_critical_period_progress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get the progress through the current critical period")
        .def("get_developmental_age", &DevelopmentSystem::getDevelopmentalAge,
             "Get the developmental age (in simulation time)")
        .def("set_developmental_age", &DevelopmentSystem::setDevelopmentalAge,
             py::arg("age"), "Set the developmental age");

    // ========== SYSTEMS ==========

    // Neural planner factory
    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a default NeuralPlanner instance");

    // Concept formation factory
    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create a default ConceptFormation instance");

    // Attentional selection factory
    m.def("createAttentionalSelection", []() -> std::shared_ptr<AttentionalSelection> {
        return std::make_shared<AttentionalSelection>();
    }, "Create a default AttentionalSelection instance");

    // Dopamine neuromodulator factory
    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create a default Dopamine neuromodulator");

    // Novelty neuromodulator factory
    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create a default Novelty neuromodulator");

    // Prediction error neuromodulator factory
    m.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create a default PredictionError neuromodulator");

    // Curiosity neuromodulator factory
    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create a default Curiosity neuromodulator");

    // Episodic memory factory
    m.def("createEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create a default NeuralEpisodicMemory instance");

    // Associative memory factory
    m.def("createAssociativeMemory", []() -> std::shared_ptr<NeuralAssociativeMemory> {
        return std::make_shared<NeuralAssociativeMemory>();
    }, "Create a default NeuralAssociativeMemory instance");

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

    m.def("getWorkingMemoryFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<NeuralWorkingMemory> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<NeuralWorkingMemory>(brain->getWorkingMemory(), 
            [brain](NeuralWorkingMemory* ptr) {
                // Delete the NeuralWorkingMemory if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the working memory from a brain object");

    m.def("createWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create a default NeuralWorkingMemory instance");

    m.def("createWorkingMemoryConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for working memory
        config->set("working_memory.capacity", 100);
        config->set("working_memory.decay_rate", 0.01f);
        config->set("working_memory.reinforcement_strength", 1.0f);
        config->set("working_memory.competition_threshold", 0.3f);
        return config;
    }, "Create a default working memory configuration");

    // ========== PREDICTION SYSTEM ==========
    
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation
    
    Implements predictive coding and forward models to anticipate future sensory states
    and compute prediction errors for learning and adaptation.
    
    Key features:
    - Predict next state based on current sensory input
    - Update predictions using actual observations
    - Maintain prediction error and confidence metrics
    - Track prediction error history for analysis
    - Support for training the prediction model
    )pbdoc")
        .def(py::init<>())
        .def("predict_next_state", &PredictionSystem::predictNextState,
             py::arg("current_state"), "Predict the next sensory state based on current input")
        .def("update_predictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("get_prediction_error", &PredictionSystem::getPredictionError,
             "Get current prediction error value")
        .def("get_confidence", &PredictionSystem::getConfidence,
             "Get prediction confidence (0.0 to 1.0)")
        .def("get_error_history", &PredictionSystem::getErrorHistory,
             "Get prediction error history as list of floats")
        .def("clear_history", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"), "Train prediction model with observation");

    m.def("getPredictionSystemFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<PredictionSystem> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<PredictionSystem>(brain->getPredictionSystem(),
            [brain](PredictionSystem* ptr) {
                // Delete the PredictionSystem if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the prediction system from a brain object");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create a default PredictionSystem instance");

    m.def("createPredictionSystemConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for prediction system
        config->set("prediction.error_threshold", 0.1f);
        config->set("prediction.confidence_threshold", 0.8f);
        config->set("prediction.history_size", 100);
        config->set("prediction.update_frequency", 1);
        return config;
    }, "Create a default prediction system configuration");

    // ============ HELPER FUNCTIONS FOR BRAIN SYSTEM ACCESS ============
    
    // Neural planner helper
    m.def("getNeuralPlannerFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<NeuralPlanner> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<NeuralPlanner>(brain->getPlanner(),
            [brain](NeuralPlanner* ptr) {
                // Delete the NeuralPlanner if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the neural planner from a brain object");

    // Concept formation helper
    m.def("getConceptFormationFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<ConceptFormation> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<ConceptFormation>(brain->getConceptFormation(),
            [brain](ConceptFormation* ptr) {
                // Delete the ConceptFormation if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the concept formation system from a brain object");

    // Attentional selection helper
    m.def("getAttentionalSelectionFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<AttentionalSelection> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<AttentionalSelection>(brain->getAttention(),
            [brain](AttentionalSelection* ptr) {
                // Delete the AttentionalSelection if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the attentional selection system from a brain object");

    // Development system helper
    m.def("getDevelopmentSystemFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<DevelopmentSystem> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<DevelopmentSystem>(brain->getDevelopmentSystem(),
            [brain](DevelopmentSystem* ptr) {
                // Delete the DevelopmentSystem if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the development system from a brain object");

    // Neuromodulation system helpers
    m.def("getDopamineFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Dopamine> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<Dopamine>(brain->getDopamine(),
            [brain](Dopamine* ptr) {
                // Delete the Dopamine if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the dopamine system from a brain object");

    m.def("getCuriosityFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Curiosity> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<Curiosity>(brain->getCuriosity(),
            [brain](Curiosity* ptr) {
                // Delete the Curiosity if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the curiosity system from a brain object");

    m.def("getNoveltyFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Novelty> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<Novelty>(brain->getNovelty(),
            [brain](Novelty* ptr) {
                // Delete the Novelty if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the novelty system from a brain object");

    m.def("getPredictionErrorFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<PredictionError> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<PredictionError>(brain->getPredictionErrorSignal(),
            [brain](PredictionError* ptr) {
                // Delete the PredictionError if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the prediction error system from a brain object");

    // Episodic memory helper
    m.def("getEpisodicMemoryFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<NeuralEpisodicMemory> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<NeuralEpisodicMemory>(brain->getEpisodicMemory(),
            [brain](NeuralEpisodicMemory* ptr) {
                // Delete the NeuralEpisodicMemory if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the episodic memory system from a brain object");

    // Associative memory helper
    m.def("getAssociativeMemoryFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<NeuralAssociativeMemory> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<NeuralAssociativeMemory>(brain->getAssociativeMemory(),
            [brain](NeuralAssociativeMemory* ptr) {
                // Delete the NeuralAssociativeMemory if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the associative memory system from a brain object");

    // Plasticity system helpers
    m.def("getSTDPFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<STDP> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<STDP>(brain->getSTDP(),
            [brain](STDP* ptr) {
                // Delete the STDP if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the STDP plasticity system from a brain object");

    m.def("getHebbianFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Hebbian> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<Hebbian>(brain->getHebbian(),
            [brain](Hebbian* ptr) {
                // Delete the Hebbian if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the Hebbian plasticity system from a brain object");

    m.def("getStructuralPlasticityFromBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<StructuralPlasticity> {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        return std::shared_ptr<StructuralPlasticity>(brain->getStructuralPlasticity(),
            [brain](StructuralPlasticity* ptr) {
                // Delete the StructuralPlasticity if brain is being destroyed
                // but don't delete brain itself
            });
    }, py::arg("brain"), "Get the structural plasticity system from a brain object");

    // Configuration helpers for each system
    m.def("createNeuralPlannerConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("planner.planning_depth", 5);
        config->set("planner.planning_confidence_threshold", 0.8f);
        config->set("planner.cache_size", 100);
        return config;
    }, "Create a default neural planner configuration");

    m.def("createConceptFormationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("concept.formation_threshold", 0.7f);
        config->set("concept.stability_threshold", 0.8f);
        config->set("concept.stability_window", 10);
        config->set("concept.max_concepts", 100);
        return config;
    }, "Create a default concept formation configuration");

    m.def("createDevelopmentSystemConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("development.critical_period_length", 50);
        config->set("development.plasticity_modifier", 1.0f);
        config->set("development.maturation_rate", 0.01f);
        return config;
    }, "Create a default development system configuration");

    m.def("createDopamineConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("dopamine.baseline_level", 0.1f);
        config->set("dopamine.decay_rate", 0.1f);
        config->set("dopamine.release_rate", 1.0f);
        config->set("dopamine.plasticity_factor", 0.5f);
        return config;
    }, "Create a default dopamine configuration");

    m.def("createNoveltyConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("novelty.decay_rate", 0.1f);
        config->set("novelty.threshold", 0.3f);
        config->set("novelty.history_size", 100);
        return config;
    }, "Create a default novelty configuration");

    m.def("createPredictionErrorConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("prediction_error.error_threshold", 0.1f);
        config->set("prediction_error.magnitude_threshold", 0.5f);
        config->set("prediction_error.history_size", 100);
        return config;
    }, "Create a default prediction error configuration");
}

} // namespace nlm
