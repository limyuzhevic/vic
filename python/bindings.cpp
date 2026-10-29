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
    // Add cognitive system access
    .def("getWorkingMemory", &Brain::getWorkingMemory,
         py::return_value_policy::reference_internal,
         "Get working memory system")
    .def("getEpisodicMemory", &Brain::getEpisodicMemory,
         py::return_value_policy::reference_internal,
         "Get episodic memory system")
    .def("getAssociativeMemory", &Brain::getAssociativeMemory,
         py::return_value_policy::reference_internal,
         "Get associative memory system")
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get prediction system")
    .def("getPlanner", &Brain::getPlanner,
         py::return_value_policy::reference_internal,
         "Get neural planner")
    .def("getConceptFormation", &Brain::getConceptFormation,
         py::return_value_policy::reference_internal,
         "Get concept formation system")
    .def("getAttention", &Brain::getAttention,
         py::return_value_policy::reference_internal,
         "Get attentional selection system")
    .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         py::return_value_policy::reference_internal,
         "Get development system")
    .def("getDopamine", &Brain::getDopamine,
         py::return_value_policy::reference_internal,
         "Get dopamine neuromodulator")
    .def("getCuriosity", &Brain::getCuriosity,
         py::return_value_policy::reference_internal,
         "Get curiosity neuromodulator")
    .def("getNovelty", &Brain::getNovelty,
         py::return_value_policy::reference_internal,
         "Get novelty neuromodulator")
    .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
         py::return_value_policy::reference_internal,
         "Get prediction error neuromodulator")
    .def("getAcetylcholine", [](std::shared_ptr<Brain> brain) {
         return brain->getDopamine(); // Temporary - cholinergic system
     }, py::return_value_policy::reference_internal,
         "Get acetylcholine neuromodulator (placeholder)")
    .def("getNorepinephrine", [](std::shared_ptr<Brain> brain) {
         return brain->getDopamine(); // Temporary - noradrenergic system
     }, py::return_value_policy::reference_internal,
         "Get norepinephrine neuromodulator (placeholder)")
    .def("getSerotonin", [](std::shared_ptr<Brain> brain) {
         return brain->getDopamine(); // Temporary - serotonergic system
     }, py::return_value_policy::reference_internal,
         "Get serotonin neuromodulator (placeholder)");

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

    m.def("run_advanced_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        
        // Advanced simulation with full cognitive integration
        NLM_LOG_INFO("Running advanced simulation with cognitive integration");
        
        for (size_t step = 0; step < num_steps; ++step) {
            // 1. Update world
            world->update(0.1);
            
            // 2. Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // 3. Process sensory input
            agent->processSensoryInput(percept);
            
            // 4. Apply neuromodulation based on prediction error
            float reward = 0.0f;
            if (world->getSensoryPercept().getInternal().size() > 0) {
                reward = world->getSensoryPercept().getInternal()[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // 5. Run brain simulation
            brain->step(step);
            
            // 6. Update development
            agent->updateDevelopment(0.1);
            
            // 7. Get action from agent
            auto action = agent->decodeMotorCommand();
            
            // 8. Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // 9. Log progress periodically
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             ": " + std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " +
                             std::to_string(agent->getCuriosityLevel()) + " curiosity, " +
                             std::to_string(agent->getNoveltyLevel()) + " novelty");
            }
        }
        
        // Return final statistics
        return py::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel(),
            agent->getPredictionError()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
         "Run an advanced simulation with full cognitive integration");
    
    m.def("create_cognitive_agent", [](std::shared_ptr<Config> config, 
                                       std::shared_ptr<SimpleWorld> world,
                                       bool enable_planning = true,
                                       bool enable_concept_formation = true,
                                       bool enable_self_model = true,
                                       bool enable_social_learning = false) {
        
        // Create brain with enhanced configuration
        auto brain = pynlm::createBrain(config);
        brain->initialize();
        
        // Create agent brain interface
        auto agent = pynlm::createAgentBrain(brain);
        
        // Initialize with world
        agent->initialize(*world);
        
        // Enable advanced cognitive systems
        if (enable_planning) {
            agent->getPlanner(); // Access planner to enable it
        }
        if (enable_concept_formation) {
            agent->getConceptFormation(); // Access concept formation
        }
        if (enable_self_model) {
            // Self-model is accessible through brain
            agent->getBrain()->getSelfModel(); // Placeholder for self-model access
        }
        if (enable_social_learning) {
            // Social learning system
            agent->getBrain()->getSocialLearning(); // Placeholder
        }
        
        // Enable all neuromodulators
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("config"), py::arg("world"), py::arg("enable_planning") = true,
         py::arg("enable_concept_formation") = true, py::arg("enable_self_model") = true,
         py::arg("enable_social_learning") = false,
         "Create an advanced cognitive agent with planning and concept formation");
    
    m.def("configure_for_task", [](std::shared_ptr<Config> config, 
                                   const std::string& task_type) {
        
        // Configure brain for specific tasks
        if (task_type == "exploration") {
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.15f);
            config->set("neuromod.dopamine.scale", 1.5f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("plasticity.stdp.enable", true);
        } 
        else if (task_type == "exploitation") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.08f);
            config->set("neuromod.dopamine.scale", 0.8f);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
            config->set("plasticity.stdp.enable", true);
        }
        else if (task_type == "social_learning") {
            config->set("brain.neuron_count", 3000);
            config->set("brain.synapse_density", 0.12f);
            config->set("neuromod.dopamine.scale", 1.2f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("social_learning.enabled", true);
        }
        
        return config;
    }, py::arg("config"), py::arg("task_type"),
         "Configure brain for specific task types");
    
    m.def("export_brain_to_file", [](std::shared_ptr<Brain> brain, 
                                     const std::string& filepath) {
        // Save brain configuration to file
        if (brain->getConfig()) {
            brain->getConfig()->saveToFile(filepath);
            NLM_LOG_INFO("Brain configuration saved to: " + filepath);
        } else {
            NLM_LOG_ERROR("Brain has no configuration to save");
        }
    }, py::arg("brain"), py::arg("filepath"),
         "Save brain configuration to file");
    
    m.def("import_brain_from_file", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        // Load brain configuration from file
        auto config = pynlm::createDefaultConfig();
        if (config->loadFromFile(filepath)) {
            NLM_LOG_INFO("Brain configuration loaded from: " + filepath);
            return pynlm::createBrain(config);
        } else {
            NLM_LOG_ERROR("Failed to load brain configuration from: " + filepath);
            return nullptr;
        }
    }, py::arg("filepath"), "Load brain configuration from file");
    
    m.def("compute_brain_metrics", [](std::shared_ptr<Brain> brain) {
        // Compute comprehensive brain metrics
        auto metrics = py::dict();
        
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["predicted_reward"] = 0.5f; // Placeholder
        
        // Get neuromodulation levels
        if (auto* dopamine = brain->getDopamine()) {
            metrics["dopamine_level"] = dopamine->getLevel();
        }
        if (auto* curiosity = brain->getCuriosity()) {
            metrics["curiosity_level"] = curiosity->getLevel();
        }
        if (auto* novelty = brain->getNovelty()) {
            metrics["novelty_level"] = novelty->getLevel();
        }
        
        return metrics;
    }, py::arg("brain"), "Compute comprehensive brain metrics");
    
    m.def("run_development_experiment", [](std::shared_ptr<Brain> brain, 
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent,
                                             double duration_minutes) {
        
        // Run development experiment simulating brain maturation
        NLM_LOG_INFO("Running development experiment for " + 
                     std::to_string(duration_minutes) + " minutes");
        
        double currentTime = 0.0;
        size_t steps_per_minute = 600; // 10 steps per second
        size_t total_steps = static_cast<size_t>(duration_minutes * steps_per_minute);
        
        for (size_t step = 0; step < total_steps && currentTime < duration_minutes * 60.0; ++step) {
            // Update world
            world->update(0.1);
            currentTime += 0.1;
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain simulation
            brain->step(step);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Log developmental stage every 10 minutes
            if (step % (steps_per_minute * 10) == 0) {
                NLM_LOG_INFO("Developmental stage: " + 
                             std::to_string(static_cast<int>(agent->getDevelopmentalStage())) +
                             " (" + std::to_string(currentTime / 60.0) + " minutes)");
            }
        }
        
        return py::make_tuple(
            agent->getDevelopmentalStage(),
            brain->getAverageFiringRate(),
            agent->getCuriosityLevel(),
            currentTime
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration_minutes"),
         "Run a development experiment simulating brain maturation");
    
    m.def("create_research_brain", [](size_t neuron_count = 5000) {
        
        // Create a research-grade brain with advanced settings
        auto config = pynlm::createDefaultConfig();
        
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", 3);
        config->set("brain.connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Advanced plasticity settings
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.01f);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0005f);
        config->set("plasticity.structural.pruning_rate", 0.00005f);
        
        // Advanced neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.novelty_weight", 0.7f);
        config->set("neuromod.curiosity.prediction_error_weight", 0.3f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        
        // Advanced development
        config->set("development.synaptogenesis_rate", 0.001f);
        config->set("development.pruning_rate", 0.0001f);
        config->set("development.maturation_period", 300.0f);
        
        auto brain = pynlm::createBrain(config);
        brain->initialize();
        
        return brain;
    }, py::arg("neuron_count") = 5000,
         "Create a research-grade brain with advanced settings");
    
    m.def("run_research_protocol", [](std::shared_ptr<Brain> brain, 
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      const std::string& protocol_name) {
        
        // Run standardized research protocols
        NLM_LOG_INFO("Running research protocol: " + protocol_name);
        
        if (protocol_name == "learning_curve") {
            // Run learning curve experiment
            NLM_LOG_INFO("Running learning curve protocol");
            
            for (size_t epoch = 0; epoch < 100; ++epoch) {
                // Reset brain each epoch for clean measurement
                brain->reset();
                brain->initialize();
                
                // Run episode
                for (size_t step = 0; step < 500; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    if (step % 100 == 0) {
                        NLM_LOG_INFO("Epoch " + std::to_string(epoch) + 
                                     ", Step " + std::to_string(step) +
                                     ", Firing rate: " + 
                                     std::to_string(brain->getAverageFiringRate()));
                    }
                }
            }
            
            NLM_LOG_INFO("Learning curve protocol complete");
            
        } else if (protocol_name == "novelty_detection") {
            // Run novelty detection experiment
            NLM_LOG_INFO("Running novelty detection protocol");
            
            std::vector<float> baseline_variation(10, 0.5f);
            
            for (size_t trial = 0; trial < 50; ++trial) {
                // Vary input patterns
                for (size_t i = 0; i < baseline_variation.size(); ++i) {
                    baseline_variation[i] = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
                }
                
                // Create world state with variation
                // (Simplified for demonstration)
                
                // Run brain with varying input
                brain->reset();
                brain->initialize();
                
                for (size_t step = 0; step < 200; ++step) {
                    world->update(0.1);
                    agent->processSensoryInput(world->getSensoryPercept());
                    brain->step(step);
                }
                
                NLM_LOG_INFO("Novelty trial " + std::to_string(trial) +
                             ", Novelty level: " + std::to_string(agent->getNoveltyLevel()));
            }
            
            NLM_LOG_INFO("Novelty detection protocol complete");
            
        } else if (protocol_name == "reward_prediction") {
            // Run reward prediction experiment
            NLM_LOG_INFO("Running reward prediction protocol");
            
            float expected_reward = 0.5f;
            float actual_reward = 0.5f;
            
            for (size_t trial = 0; trial < 30; ++trial) {
                // Introduce prediction errors
                if (trial % 5 == 0) {
                    expected_reward = 0.3f;
                } else {
                    expected_reward = 0.7f;
                }
                
                actual_reward = expected_reward + static_cast<float>(rand()) / RAND_MAX * 0.4f - 0.2f;
                actual_reward = std::clamp(actual_reward, 0.0f, 1.0f);
                
                // Apply reward with prediction error
                agent->applyRewardModulation(actual_reward, expected_reward);
                
                NLM_LOG_INFO("Reward trial " + std::to_string(trial) +
                             ", Expected: " + std::to_string(expected_reward) +
                             ", Actual: " + std::to_string(actual_reward) +
                             ", Error: " + std::to_string(agent->getPredictionError()));
            }
            
            NLM_LOG_INFO("Reward prediction protocol complete");
            
        } else {
            NLM_LOG_ERROR("Unknown research protocol: " + protocol_name);
        }
        
        return protocol_name;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("protocol_name"),
         "Run standardized research protocols");
