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

    // Advanced Network Analysis Functions
    m.def("analyzeNetworkTopology", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) return std::map<std::string, double>{};
        
        std::map<std::string, double> topology;
        topology["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        topology["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        topology["region_count"] = static_cast<double>(brain->getRegionCount());
        topology["firing_rate"] = brain->getAverageFiringRate();
        topology["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        topology["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return topology;
    }, py::arg("brain"), "Analyze network topology and properties");

    m.def("calculateNetworkDensity", [](const std::shared_ptr<Brain>& brain) {
        if (!brain || brain->getTotalNeuronCount() == 0) return 0.0f;
        
        size_t totalPossibleSynapses = brain->getTotalNeuronCount() * (brain->getTotalNeuronCount() - 1);
        if (totalPossibleSynapses == 0) return 0.0f;
        
        return static_cast<double>(brain->getTotalSynapseCount()) / totalPossibleSynapses;
    }, py::arg("brain"), "Calculate network density (0.0 to 1.0)");

    m.def("getNeuronDistribution", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, size_t> distribution;
        if (!brain) return distribution;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                std::string typeStr;
                switch (type) {
                    case NeuronType::Excitatory: typeStr = "excitatory"; break;
                    case NeuronType::Inhibitory: typeStr = "inhibitory"; break;
                    case NeuronType::Sensory: typeStr = "sensory"; break;
                    case NeuronType::Motor: typeStr = "motor"; break;
                    case NeuronType::Modulatory: typeStr = "modulatory"; break;
                    case NeuronType::Internal: typeStr = "internal"; break;
                    default: typeStr = "unknown"; break;
                }
                distribution[typeStr] += pop->getNeuronCount();
            }
        }
        
        return distribution;
    }, py::arg("brain"), "Get distribution of neuron types");

    m.def("getAverageSynapticWeights", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> weights;
        if (!brain) return weights;
        
        for (const auto& region : brain->getRegions()) {
            float avgWeight = region->getAverageSynapticWeight();
            std::string regionName = region->getName();
            if (regionName.empty()) {
                regionName = "region_" + std::to_string(region->getId().value);
            }
            weights[regionName] = avgWeight;
        }
        
        return weights;
    }, py::arg("brain"), "Get average synaptic weights by region");

    m.def("detectNetworkCommunities", [](const std::shared_ptr<Brain>& brain, double threshold = 0.5) {
        std::vector<std::vector<std::string>> communities;
        if (!brain) return communities;
        
        // Simple community detection based on connection strength
        // This is a placeholder for more sophisticated algorithms
        for (const auto& region : brain->getRegions()) {
            std::vector<std::string> community;
            for (const auto& syn : region->getSynapses()) {
                if (std::abs(syn->getWeight()) > threshold) {
                    community.push_back("synapse_" + std::to_string(syn->getId().value));
                }
            }
            if (!community.empty()) {
                communities.push_back(community);
            }
        }
        
        return communities;
    }, py::arg("brain"), py::arg("threshold"), "Detect communities in the network");

    // Performance Monitoring Functions
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain, uint64_t steps) {
        if (!brain) return std::map<std::string, double>{};
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        
        std::map<std::string, double> profile;
        profile["total_steps"] = static_cast<double>(steps);
        profile["wall_time"] = duration.count();
        profile["steps_per_second"] = steps / duration.count();
        profile["avg_firing_rate"] = brain->getAverageFiringRate();
        profile["neurons_active"] = static_cast<double>(brain->getActiveNeuronCount());
        
        return profile;
    }, py::arg("brain"), py::arg("steps"), "Profile simulation performance");

    m.def("benchmarkPlasticity", [](std::shared_ptr<Brain> brain, uint64_t steps) {
        if (!brain) return std::map<std::string, double>{};
        
        // Reset brain for clean benchmark
        brain->reset();
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Inject activity to trigger plasticity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step);
            
            // Inject reward to trigger reward-modulated learning
            float reward = 1.0f;
            float pred_error = 0.0f;
            
            // This would require access to an AgentBrain for proper neuromodulation
            // For now, just track baseline performance
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        
        std::map<std::string, double> benchmark;
        benchmark["total_steps"] = static_cast<double>(steps);
        benchmark["wall_time"] = duration.count();
        benchmark["steps_per_second"] = steps / duration.count();
        benchmark["final_firing_rate"] = brain->getAverageFiringRate();
        benchmark["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return benchmark;
    }, py::arg("brain"), py::arg("steps"), "Benchmark plasticity performance");

    // Advanced Learning Algorithm Controls
    m.def("configureAdvancedLearning", [](std::shared_ptr<Brain> brain, 
                                          const std::map<std::string, double>& params) {
        if (!brain) return false;
        
        // Apply advanced learning parameters
        auto config = brain->getConfig();
        
        // Hebbian parameters
        if (params.find("hebbian_learning_rate") != params.end()) {
            // This would require access to plasticity modules
            // For now, just store in config
            config->set("plasticity.hebbian.learning_rate", 
                       static_cast<float>(params.at("hebbian_learning_rate")));
        }
        
        // STDP parameters
        if (params.find("stdp_ltp_weight") != params.end()) {
            config->set("plasticity.stdp.ltp_weight",
                       static_cast<float>(params.at("stdp_ltp_weight")));
        }
        if (params.find("stdp_ltd_weight") != params.end()) {
            config->set("plasticity.stdp.ltd_weight",
                       static_cast<float>(params.at("stdp_ltd_weight")));
        }
        
        // Neuromodulation parameters
        if (params.find("dopamine_scale") != params.end()) {
            config->set("neuromod.dopamine.scale",
                       static_cast<float>(params.at("dopamine_scale")));
        }
        
        if (params.find("curiosity_threshold") != params.end()) {
            // This parameter doesn't exist in current config, would need to add
            config->set("neuromod.curiosity.threshold",
                       static_cast<float>(params.at("curiosity_threshold")));
        }
        
        return true;
    }, py::arg("brain"), py::arg("params"), "Configure advanced learning parameters");

    m.def("simulateWithLearningProfile", [](std::shared_ptr<Brain> brain,
                                           const std::string& profile,
                                           uint64_t steps) {
        if (!brain) return std::map<std::string, double>{};
        
        // Apply predefined learning profile
        std::map<std::string, double> defaultProfiles {
            {"conservative", {
                {"hebbian_learning_rate", 0.001},
                {"stdp_ltp_weight", 0.01},
                {"stdp_ltd_weight", 0.012},
                {"dopamine_scale", 0.5}
            }},
            {"aggressive", {
                {"hebbian_learning_rate", 0.01},
                {"stdp_ltp_weight", 0.02},
                {"stdp_ltd_weight", 0.015},
                {"dopamine_scale", 1.5}
            }},
            {"balanced", {
                {"hebbian_learning_rate", 0.005},
                {"stdp_ltp_weight", 0.015},
                {"stdp_ltd_weight", 0.015},
                {"dopamine_scale", 1.0}
            }}
        };
        
        if (defaultProfiles.find(profile) == defaultProfiles.end()) {
            throw std::runtime_error("Unknown learning profile: " + profile);
        }
        
        // Configure with selected profile
        m.def("configureAdvancedLearning", brain, defaultProfiles[profile]);
        
        // Run simulation
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Apply periodic stimulation to maintain learning activity
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        
        std::map<std::string, double> results;
        results["profile"] = profile;
        results["steps"] = static_cast<double>(steps);
        results["time"] = duration.count();
        results["firing_rate"] = brain->getAverageFiringRate();
        results["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return results;
    }, py::arg("brain"), py::arg("profile"), py::arg("steps"), 
        "Run simulation with predefined learning profile");

    // Network Manipulation Functions
    m.def("createCustomNetwork", [](const std::map<std::string, double>& params) {
        auto config = std::make_shared<Config>();
        
        // Set custom network parameters
        if (params.find("neuron_count") != params.end()) {
            config->set("brain.neuron_count", static_cast<int64_t>(params.at("neuron_count")));
        }
        if (params.find("region_count") != params.end()) {
            config->set("brain.region_count", static_cast<int64_t>(params.at("region_count")));
        }
        if (params.find("connection_probability") != params.end()) {
            config->set("brain.connection_probability", static_cast<float>(params.at("connection_probability")));
        }
        if (params.find("initial_weight_mean") != params.end()) {
            config->set("brain.initial_weight_mean", static_cast<float>(params.at("initial_weight_mean")));
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("params"), "Create a custom network with specified parameters");

    m.def("duplicateBrain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) return std::shared_ptr<Brain>{};
        
        auto config = std::make_shared<Config>(*(brain->getConfig()));
        auto duplicate = std::make_shared<Brain>(config);
        
        // This is a simplified duplication - real implementation would need to
        // properly copy all state including neuron positions, weights, etc.
        duplicate->initialize();
        
        return duplicate;
    }, py::arg("brain"), "Create a duplicate of the current brain");

    // Export/Import Utilities
    m.def("exportNetworkToGraphML", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        // Write network data in GraphML format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\">\n";
        file << "  <key id=\"d0\" for=\"default\" attr.name=\"label\" attr.type=\"string\"/>\n";
        file << "  <graph id=\"G\" edgedefault=\"directed\">\n";
        
        // Write nodes (neurons)
        file << "    <node id=\"n0\"/>\n";  // Simplified - would need to iterate over all neurons
        file << "    <node id=\"n1\"/>\n";
        
        // Write edges (synapses)
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                file << "    <edge source=\"" << syn->getSourceNeuron().value
                     << "\" target=\"" << syn->getDestinationNeuron().value
                     << "\"/>\n";
            }
        }
        
        file << "  </graph>\n";
        file << "</graphml>\n";
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Export network to GraphML format");

    m.def("importNetworkFromGraphML", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100); // Default size
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // This is a placeholder - real implementation would parse GraphML file
        // and reconstruct the network topology
        
        return brain;
    }, py::arg("filepath"), "Import network from GraphML format");

    // Batch Processing Utilities
    m.def("runBatchSimulations", [](const std::vector<std::shared_ptr<Config>>& configs,
                                   const std::vector<uint64_t>& steps,
                                   const std::string& outputPrefix = "batch_results") {
        std::vector<std::map<std::string, double>> results;
        
        for (size_t i = 0; i < configs.size() && i < steps.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            brain->initialize();
            
            std::map<std::string, double> result;
            result["config_id"] = static_cast<double>(i);
            result["steps"] = static_cast<double>(steps[i]);
            result["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            result["synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            
            // Run simulation
            for (uint64_t step = 0; step < steps[i]; ++step) {
                brain->step(step);
            }
            
            result["final_firing_rate"] = brain->getAverageFiringRate();
            result["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            
            results.push_back(result);
        }
        
        return results;
    }, py::arg("configs"), py::arg("steps"), py::arg("outputPrefix"), 
        "Run multiple simulations with different configurations");

    m.def("runPopulationStudy", [](uint64_t populationSize, uint64_t stepsPerSimulation,
                                 const std::map<std::string, double>& paramVariation) {
        std::vector<std::map<std::string, double>> populationResults;
        
        for (uint64_t i = 0; i < populationSize; ++i) {
            auto config = std::make_shared<Config>();
            
            // Apply parameter variation for evolutionary study
            if (paramVariation.find("neuron_count") != paramVariation.end()) {
                double variation = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 
                                  paramVariation.at("neuron_count") * 0.2;
                config->set("brain.neuron_count", 
                           static_cast<int64_t>(populationSize + variation));
            }
            
            if (paramVariation.find("connection_probability") != paramVariation.end()) {
                double variation = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 
                                  paramVariation.at("connection_probability") * 0.3;
                config->set("brain.connection_probability", 
                           static_cast<float>(0.1 + variation));
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run simulation
            for (uint64_t step = 0; step < stepsPerSimulation; ++step) {
                brain->step(step);
            }
            
            std::map<std::string, double> result;
            result["individual"] = static_cast<double>(i);
            result["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            result["synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            result["firing_rate"] = brain->getAverageFiringRate();
            result["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
            
            populationResults.push_back(result);
        }
        
        return populationResults;
    }, py::arg("populationSize"), py::arg("stepsPerSimulation"), 
        py::arg("paramVariation"), "Run population study with parameter variations");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
