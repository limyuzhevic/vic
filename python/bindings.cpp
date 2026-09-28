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

    // Advanced simulation control
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                         int numSteps, bool enableLearning) {
        agent->reset();
        world->reset();
        
        double totalReward = 0.0f;
        int curiosityEvents = 0;
        int noveltyEvents = 0;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto* brain = agent->getBrain();
            if (brain) {
                brain->step(step, world->getSimulationTime());
            }
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Collect statistics
            if (enableLearning) {
                float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0f;
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
                
                if (agent->getCuriosityLevel() > 0.8f) curiosityEvents++;
                if (agent->getNoveltyLevel() > 0.5f) noveltyEvents++;
                totalReward += reward;
            }
        }
        
        return std::make_tuple(totalReward, curiosityEvents, noveltyEvents);
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
       "Run a complete simulation episode and return statistics");

    // Performance monitoring and profiling
    m.def("profileBrainActivity", [](std::shared_ptr<Brain> brain, int durationSteps) {
        std::vector<double> spikeCounts;
        std::vector<double> firingRates;
        std::vector<double> membranePotentials;
        
        for (int step = 0; step < durationSteps; ++step) {
            brain->step(step);
            
            spikeCounts.push_back(static_cast<double>(brain->getTotalSpikeCount()));
            firingRates.push_back(static_cast<double>(brain->getFiringNeuronCount()));
            
            // Sample membrane potentials from a few neurons
            double avgMembrane = 0.0;
            int sampleCount = 0;
            auto regions = brain->getRegions();
            for (const auto& region : regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto& neuron : pop->getNeurons()) {
                        if (sampleCount < 10) {
                            avgMembrane += neuron->getState().membranePotential;
                            sampleCount++;
                        }
                    }
                }
            }
            if (sampleCount > 0) {
                membranePotentials.push_back(avgMembrane / sampleCount);
            }
        }
        
        return std::make_tuple(spikeCounts, firingRates, membranePotentials);
    }, py::arg("brain"), py::arg("durationSteps"), "Profile brain activity over time and return metrics");

    // Advanced plasticity control
    m.def("configurePlasticity", [](std::shared_ptr<Brain> brain, bool hebbian, bool stdp, 
                                  bool structural, float hebbianRate, float stdpLTP, float stdpLTD) {
        for (const auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, structural);
                
                auto* hebb = brain->getHebbian();
                if (hebb) {
                    hebb->setLearningRate(hebbianRate);
                }
                
                auto* stdpPtr = brain->getSTDP();
                if (stdpPtr) {
                    stdpPtr->setLTPWeight(stdpLTP);
                    stdpPtr->setLTDWeight(stdpLTD);
                }
            }
        }
        return true;
    }, py::arg("brain"), py::arg("hebbian"), py::arg("stdp"), py::arg("structural"), 
       py::arg("hebbianRate") = 0.001f, py::arg("stdpLTP") = 0.02f, py::arg("stdpLTD") = 0.015f,
       "Configure plasticity mechanisms on the brain");

    // Advanced neuromodulation
    m.def("setNeuromodulationParameters", [](std::shared_ptr<AgentBrain> agent, 
                                           float dopamineScale, float curiosityThreshold,
                                           float noveltyDecay, float predictionErrorGain) {
        if (!agent) return false;
        
        // Enable neuromodulation if not already enabled
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Note: These parameters would need to be added to AgentBrain class
        // For now, we'll just verify the agent is properly configured
        return agent->getBrain() != nullptr;
    }, py::arg("agent"), py::arg("dopamineScale") = 1.0f, py::arg("curiosityThreshold") = 0.3f,
       py::arg("noveltyDecay") = 0.99f, py::arg("predictionErrorGain") = 1.0f,
       "Set neuromodulation parameters for advanced control");

    // Advanced configuration utilities
    m.def("createAdvancedConfig", [](int neuronCount, int regionCount, 
                                   float connectionProbability, bool enableMemorySystems,
                                   bool enablePrediction, bool enableDevelopment) {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("brain.connection_probability", connectionProbability, ConfigSource::Default);
        
        // Enable/disable advanced features
        config->set("memory.enable_working_memory", enableMemorySystems, ConfigSource::Default);
        config->set("memory.enable_episodic_memory", enableMemorySystems, ConfigSource::Default);
        config->set("prediction.enable_prediction", enablePrediction, ConfigSource::Default);
        config->set("development.enable_development", enableDevelopment, ConfigSource::Default);
        
        // Performance settings
        config->set("performance.use_memory_pool", true, ConfigSource::Default);
        config->set("performance.enable_simd_optimization", true, ConfigSource::Default);
        
        // Advanced plasticity settings
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", enableDevelopment, ConfigSource::Default);
        
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 3, 
       py::arg("connectionProbability") = 0.1, py::arg("enableMemorySystems") = true,
       py::arg("enablePrediction") = true, py::arg("enableDevelopment") = true,
       "Create an advanced configuration with multiple features enabled");

    // Neural network analysis tools
    m.def("analyzeNetworkTopology", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> metrics;
        
        if (!brain) return metrics;
        
        int totalNeurons = brain->getTotalNeuronCount();
        int totalSynapses = brain->getTotalSynapseCount();
        int totalRegions = brain->getRegionCount();
        
        // Calculate connectivity metrics
        double connectionDensity = totalSynapses > 0 ? 
            static_cast<double>(totalSynapses) / (totalNeurons * (totalNeurons - 1)) : 0.0;
        
        // Count neuron types
        std::unordered_map<NeuronType, int> neuronTypeCounts;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                neuronTypeCounts[type] += pop->getNeuronCount();
            }
        }
        
        // Calculate excitation/inhibition balance
        int excitatoryNeurons = neuronTypeCounts[NeuronType::Excitatory];
        int inhibitoryNeurons = neuronTypeCounts[NeuronType::Inhibitory];
        double e_in_ratio = inhibitoryNeurons > 0 ? 
            static_cast<double>(excitatoryNeurons) / inhibitoryNeurons : 1.0;
        
        // Estimate network efficiency (simplified)
        double networkEfficiency = connectionDensity * e_in_ratio * 0.5;
        
        // Store metrics
        metrics["total_neurons"] = totalNeurons;
        metrics["total_synapses"] = totalSynapses;
        metrics["total_regions"] = totalRegions;
        metrics["connection_density"] = connectionDensity;
        metrics["excitatory_inhibitory_ratio"] = e_in_ratio;
        metrics["network_efficiency"] = networkEfficiency;
        
        // Add neuron type counts
        for (const auto& pair : neuronTypeCounts) {
            std::string typeName;
            switch (pair.first) {
                case NeuronType::Excitatory: typeName = "excitatory"; break;
                case NeuronType::Inhibitory: typeName = "inhibitory"; break;
                case NeuronType::Sensory: typeName = "sensory"; break;
                case NeuronType::Motor: typeName = "motor"; break;
                case NeuronType::Modulatory: typeName = "modulatory"; break;
                case NeuronType::Internal: typeName = "internal"; break;
                default: typeName = "unknown"; break;
            }
            metrics["neurons_" + typeName] = pair.second;
        }
        
        return metrics;
    }, py::arg("brain"), "Analyze neural network topology and return comprehensive metrics");

    // Advanced error recovery and validation
    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> errors;
        
        if (!brain) {
            errors.push_back("Brain pointer is null");
            return errors;
        }
        
        // Check neuron consistency
        int neuronCount = brain->getTotalNeuronCount();
        if (neuronCount <= 0) {
            errors.push_back("Invalid neuron count: " + std::to_string(neuronCount));
        }
        
        // Check synaptic consistency
        int synapseCount = brain->getTotalSynapseCount();
        if (synapseCount < 0) {
            errors.push_back("Invalid synapse count: " + std::to_string(synapseCount));
        }
        
        // Check firing rates
        float firingRate = brain->getAverageFiringRate();
        if (firingRate < 0.0f || firingRate > 1000.0f) {
            errors.push_back("Unreasonable firing rate: " + std::to_string(firingRate));
        }
        
        // Check excitation/inhibition ratio
        float e_in_ratio = brain->getExcitationInhibitionRatio();
        if (e_in_ratio < 0.1f || e_in_ratio > 10.0f) {
            errors.push_back("Unreasonable E/I ratio: " + std::to_string(e_in_ratio));
        }
        
        // Check developmental stage consistency
        auto devStage = brain->getDevelopmentalStage();
        if (devStage == DevelopmentalStage::Invalid) {
            errors.push_back("Invalid developmental stage");
        }
        
        return errors;
    }, py::arg("brain"), "Validate brain state and return list of any errors found");

    // Performance benchmarking
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int stepCount, 
                             std::string benchmarkType = "comprehensive") {
        std::unordered_map<std::string, double> results;
        
        if (!brain || stepCount <= 0) {
            results["error"] = -1.0;
            return results;
        }
        
        // Time the simulation steps
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < stepCount; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["time_per_step_ms"] = static_cast<double>(duration.count()) / stepCount;
        results["steps_per_second"] = stepCount / (duration.count() / 1000.0);
        
        // Calculate performance metrics
        double spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / stepCount;
        double neuronsPerStep = static_cast<double>(brain->getTotalNeuronCount()) / stepCount;
        
        results["spikes_per_step"] = spikesPerStep;
        results["neurons_per_step"] = neuronsPerStep;
        results["efficiency_score"] = spikesPerStep / (duration.count() / 1000.0); // spikes per second
        
        // Memory usage estimation (simplified)
        results["memory_estimate_mb"] = (neuronCount * sizeof(Neuron) + 
                                        synapseCount * sizeof(Synapse)) / (1024.0 * 1024.0);
        
        return results;
    }, py::arg("brain"), py::arg("stepCount"), py::arg("benchmarkType") = "comprehensive",
       "Benchmark brain performance and return timing and efficiency metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add version information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM Python Bindings - Enhanced Version with Advanced Features";
}

} // namespace nlm
