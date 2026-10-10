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

    m.def("createDefaultConfigWithParams", [](size_t neuronCount, size_t regionCount, 
                                              float connectionProbability, float simulationTimestep) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("simulation_timestep", simulationTimestep);
        config->set("random_seed", uint64_t(42));
        
        return config;
    }, py::arg("neuronCount") = 1000UL, py::arg("regionCount") = 1UL, 
       py::arg("connectionProbability") = 0.1f, py::arg("simulationTimestep") = 0.001,
       "Create a configuration with sensible defaults");

    m.def("createDefaultBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a default brain with initialization");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("runSimpleLearningExperiment", [](std::shared_ptr<Brain> brain, 
                                           std::shared_ptr<SimpleWorld> world, 
                                           size_t steps) -> std::string {
        std::ostringstream oss;
        oss << "=== Simple Learning Experiment ===\n";
        oss << "Initial Brain State:\n";
        oss << brain->logStatus();
        oss << "\nStarting experiment with " << steps << " steps...\n";
        
        world->configure(100, 100, 20, 20);
        world->reset();
        world->setAgentStart(50, 50);
        
        world->setRandomSeed(42);
        
        for (size_t step = 0; step < steps; ++step) {
            oss << "\nStep " << step + 1 << ":\n";
            
            // Create a simple sensory input (random noise)
            auto vision = std::make_shared<Vision>(20, 20, 3);
            std::vector<float> data;
            data.reserve(20 * 20 * 3);
            for (size_t i = 0; i < 20 * 20 * 3; ++i) {
                data.push_back((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f);
            }
            vision->setData(data);
            
            // Get sensory percept and inject into brain
            auto percept = world->getSensoryPercept();
            percept->setVision(vision);
            percept->setTimestamp(static_cast<double>(step));
            
            brain->receiveSensoryInput(*percept);
            
            brain->step(step, static_cast<double>(step) * 0.1);
            
            auto action = brain->produceAction();
            world->applyMotorCommand(action->getType(), static_cast<double>(step));
            
            // Add some occasional world changes for learning
            if (step % 50 == 0) {
                world->update(step);
            }
            
            if (step % 100 == 0) {
                oss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
                oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
                oss << "  Firing rate: " << brain->getAverageFiringRate() << "\n";
            }
        }
        
        oss << "\nExperiment completed!\n";
        oss << "Final Brain State:\n";
        oss << brain->logStatus();
        
        return oss.str();
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 1000UL,
       "Run a simple learning experiment with visual output");

    m.def("visualizeBrainState", [](const std::shared_ptr<Brain>& brain, size_t maxRegions = 10) -> std::string {
        std::ostringstream oss;
        oss << "=== Brain State Visualization ===\n\n";
        
        oss << "Basic Statistics:\n";
        oss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        oss << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
        oss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        oss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        oss << "  Pending spike events: " << brain->getPendingSpikeEventCount() << "\n\n";
        
        oss << "Region Information (showing " << std::min(maxRegions, brain->getRegionCount()) 
            << " of " << brain->getRegionCount() << " regions):\n";
        
        auto regionIds = brain->getRegionIds();
        size_t regionsToShow = std::min(maxRegions, regionIds.size());
        for (size_t i = 0; i < regionsToShow; ++i) {
            auto region = brain->getRegion(regionIds[i]);
            if (region) {
                oss << "  Region " << region->getId().index() << 
                    " (\"" << region->getName() << "\"):\n";
                oss << "    Neurons: " << region->getTotalNeuronCount() << ", "
                    << "Synapses: " << region->getSynapseCount() << "\n";
                oss << "    Active: " << region->getActiveNeuronCount() << ", "
                    << "Firing: " << region->getFiringNeuronCount() << "\n";
                oss << "    Avg firing rate: " << region->getAverageFiringRate() << "\n";
                oss << "    Avg synaptic weight: " << region->getAverageSynapticWeight() << "\n";
                oss << "    Synaptic density: " << region->getSynapticDensity() << "\n";
                if (i < regionsToShow - 1) oss << "\n";
            }
        }
        
        if (brain->getRegionCount() > maxRegions) {
            oss << "  ... and " << (brain->getRegionCount() - maxRegions) << " more regions\n";
        }
        
        return oss.str();
    }, py::arg("brain"), py::arg("maxRegions") = 10UL,
       "Simple text visualization of brain activity state");

    m.def("example_basic_usage", []() -> std::string {
        std::ostringstream oss;
        oss << "=== Basic NLM Usage Example ===\n\n";
        
        oss << "1. Creating a default configuration...\n";
        auto config = nlm::createDefaultConfig();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        oss << "   Config created with custom parameters\n\n";
        
        oss << "2. Creating a brain with the configuration...\n";
        auto brain = nlm::createDefaultBrain(config);
        oss << "   Brain initialized successfully\n\n";
        
        oss << "3. Creating a world...\n";
        auto world = nlm::createSimpleWorld();
        world->configure(50, 50, 10, 10);
        world->reset();
        world->setAgentStart(25, 25);
        oss << "   World configured and reset\n\n";
        
        oss << "4. Running a learning experiment (200 steps)...\n";
        auto experimentResult = nlm::runSimpleLearningExperiment(brain, world, 200UL);
        oss << experimentResult << "\n";
        
        oss << "5. Visualizing brain state...\n";
        auto visualization = nlm::visualizeBrainState(brain, 5UL);
        oss << visualization << "\n";
        
        oss << "6. Saving brain state to file...\n";
        bool saved = brain->save("./example_brain_checkpoint");
        oss << "   Brain saved: " << (saved ? "Yes" : "No") << "\n\n";
        
        oss << "=== Example Complete ===\n";
        oss << "This example demonstrates the basic workflow of creating, training, "
            << "and visualizing an NLM brain.\n";
        
        return oss.str();
    }, "Complete working example demonstrating basic NLM usage patterns");

    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (Python-friendly)");

    m.def("create_default_config_with_params", [](size_t neuron_count, size_t region_count, 
                                                  float connection_probability, float simulation_timestep) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", neuron_count);
        config->set("region_count", region_count);
        config->set("connection_probability", connection_probability);
        config->set("simulation_timestep", simulation_timestep);
        config->set("random_seed", uint64_t(42));
        
        return config;
    }, py::arg("neuron_count") = 1000UL, py::arg("region_count") = 1UL, 
       py::arg("connection_probability") = 0.1f, py::arg("simulation_timestep") = 0.001,
       "Create a configuration with sensible defaults (Python-friendly)");

    m.def("create_default_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a brain with default parameters and initialize it (Python-friendly)");

    m.def("run_simple_learning_experiment", [](std::shared_ptr<Brain> brain, 
                                               std::shared_ptr<SimpleWorld> world, 
                                               size_t steps) -> std::string {
        std::ostringstream oss;
        oss << "=== Simple Learning Experiment ===\n";
        oss << "Initial Brain State:\n";
        oss << brain->logStatus();
        oss << "\nStarting experiment with " << steps << " steps...\n";
        
        world->configure(100, 100, 20, 20);
        world->reset();
        world->setAgentStart(50, 50);
        
        world->setRandomSeed(42);
        
        for (size_t step = 0; step < steps; ++step) {
            oss << "\nStep " << step + 1 << ":\n";
            
            // Create a simple sensory input (random noise)
            auto vision = std::make_shared<Vision>(20, 20, 3);
            std::vector<float> data;
            data.reserve(20 * 20 * 3);
            for (size_t i = 0; i < 20 * 20 * 3; ++i) {
                data.push_back((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f);
            }
            vision->setData(data);
            
            // Get sensory percept and inject into brain
            auto percept = world->getSensoryPercept();
            percept->setVision(vision);
            percept->setTimestamp(static_cast<double>(step));
            
            brain->receiveSensoryInput(*percept);
            
            brain->step(step, static_cast<double>(step) * 0.1);
            
            auto action = brain->produceAction();
            world->applyMotorCommand(action->getType(), static_cast<double>(step));
            
            // Add some occasional world changes for learning
            if (step % 50 == 0) {
                world->update(step);
            }
            
            if (step % 100 == 0) {
                oss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
                oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
                oss << "  Firing rate: " << brain->getAverageFiringRate() << "\n";
            }
        }
        
        oss << "\nExperiment completed!\n";
        oss << "Final Brain State:\n";
        oss << brain->logStatus();
        
        return oss.str();
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 1000UL,
       "Run a simple learning experiment with visual output (Python-friendly)");

    m.def("visualize_brain_state", [](const std::shared_ptr<Brain>& brain, size_t max_regions = 10) -> std::string {
        std::ostringstream oss;
        oss << "=== Brain State Visualization ===\n\n";
        
        oss << "Basic Statistics:\n";
        oss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        oss << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
        oss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        oss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        oss << "  Pending spike events: " << brain->getPendingSpikeEventCount() << "\n\n";
        
        oss << "Region Information (showing " << std::min(max_regions, brain->getRegionCount()) 
            << " of " << brain->getRegionCount() << " regions):\n";
        
        auto regionIds = brain->getRegionIds();
        size_t regionsToShow = std::min(max_regions, regionIds.size());
        for (size_t i = 0; i < regionsToShow; ++i) {
            auto region = brain->getRegion(regionIds[i]);
            if (region) {
                oss << "  Region " << region->getId().index() << 
                    " (\"" << region->getName() << "\"):\n";
                oss << "    Neurons: " << region->getTotalNeuronCount() << ", "
                    << "Synapses: " << region->getSynapseCount() << "\n";
                oss << "    Active: " << region->getActiveNeuronCount() << ", "
                    << "Firing: " << region->getFiringNeuronCount() << "\n";
                oss << "    Avg firing rate: " << region->getAverageFiringRate() << "\n";
                oss << "    Avg synaptic weight: " << region->getAverageSynapticWeight() << "\n";
                oss << "    Synaptic density: " << region->getSynapticDensity() << "\n";
                if (i < regionsToShow - 1) oss << "\n";
            }
        }
        
        if (brain->getRegionCount() > max_regions) {
            oss << "  ... and " << (brain->getRegionCount() - max_regions) << " more regions\n";
        }
        
        return oss.str();
    }, py::arg("brain"), py::arg("max_regions") = 10UL,
       "Simple text visualization of brain activity state (Python-friendly)");

    m.def("example_basic_usage", []() -> std::string {
        std::ostringstream oss;
        oss << "=== Basic NLM Usage Example ===\n\n";
        
        oss << "1. Creating a default configuration...\n";
        auto config = create_default_config();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        oss << "   Config created with custom parameters\n\n";
        
        oss << "2. Creating a brain with the configuration...\n";
        auto brain = create_default_brain(config);
        oss << "   Brain initialized successfully\n\n";
        
        oss << "3. Creating a world...\n";
        auto world = create_simple_world();
        world->configure(50, 50, 10, 10);
        world->reset();
        world->setAgentStart(25, 25);
        oss << "   World configured and reset\n\n";
        
        oss << "4. Running a learning experiment (200 steps)...\n";
        auto experimentResult = run_simple_learning_experiment(brain, world, 200UL);
        oss << experimentResult << "\n";
        
        oss << "5. Visualizing brain state...\n";
        auto visualization = visualize_brain_state(brain, 5UL);
        oss << visualization << "\n";
        
        oss << "6. Saving brain state to file...\n";
        bool saved = brain->save("./example_brain_checkpoint");
        oss << "   Brain saved: " << (saved ? "Yes" : "No") << "\n\n";
        
        oss << "=== Example Complete ===\n";
        oss << "This example demonstrates the basic workflow of creating, training, "
            << "and visualizing an NLM brain.\n";
        
        return oss.str();
    }, "Complete working example demonstrating basic NLM usage patterns");
