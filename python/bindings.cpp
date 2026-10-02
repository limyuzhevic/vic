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
              "Log brain status")
        
        // Convenience methods for Brain class
        .def("run_simulation", [](Brain& self, size_t steps, bool logProgress = false) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * 0.001);
                if (logProgress && i % 100 == 0) {
                    NLM_LOG_INFO("Simulation step: " + std::to_string(i) + "/" + std::to_string(steps));
                }
            }
            return self.getTotalSpikeCount();
        }, py::arg("steps"), py::arg("logProgress") = false,
              "Run a simulation for the given number of steps and return total spike count")
        
        .def("analyze_network", [](Brain& self) {
            std::map<std::string, std::string> analysis;
            analysis["total_neurons"] = std::to_string(self.getTotalNeuronCount());
            analysis["total_synapses"] = std::to_string(self.getTotalSynapseCount());
            analysis["active_neurons"] = std::to_string(self.getActiveNeuronCount());
            analysis["firing_neurons"] = std::to_string(self.getFiringNeuronCount());
            analysis["avg_firing_rate"] = std::to_string(self.getAverageFiringRate());
            analysis["excitatory_inhibitory_ratio"] = std::to_string(self.getExcitationInhibitionRatio());
            analysis["total_spikes"] = std::to_string(self.getTotalSpikeCount());
            return analysis;
        }, "Analyze network statistics and return them as a dictionary")
        
        .def("create_simple_test", [](Brain& self) {
            // Create a simple test scenario with random connections
            auto config = self.getConfig();
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::CommandLine);
            config->set("connection_probability", 0.2f, ConfigSource::CommandLine);
            
            auto newBrain = std::make_shared<Brain>(config);
            if (newBrain->initialize()) {
                // Inject some random input to stimulate activity
                for (size_t i = 0; i < 10 && i < newBrain->getTotalNeuronCount(); ++i) {
                    newBrain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
                }
                
                for (size_t step = 0; step < 200; ++step) {
                    newBrain->step(step, step * 0.001);
                }
                
                return newBrain;
            }
            return std::shared_ptr<Brain>();
        }, "Create a simple test brain with basic connectivity for testing purposes")
        
        .def("compare_with", [](const Brain& self, const Brain& other) {
            std::map<std::string, double> comparison;
            comparison["neuron_count_diff"] = static_cast<double>(self.getTotalNeuronCount() - other.getTotalNeuronCount());
            comparison["synapse_count_diff"] = static_cast<double>(self.getTotalSynapseCount() - other.getTotalSynapseCount());
            comparison["spike_count_diff"] = static_cast<double>(self.getTotalSpikeCount() - other.getTotalSpikeCount());
            comparison["firing_rate_diff"] = self.getAverageFiringRate() - other.getAverageFiringRate();
            comparison["e_i_ratio_diff"] = self.getExcitationInhibitionRatio() - other.getExcitationInhibitionRatio();
            return comparison;
        }, py::arg("other"),
              "Compare this brain with another and return differences as a dictionary")

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Convenience methods for AgentBrain class
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                             const std::vector<float>& initialPercept, size_t maxSteps = 1000) {
            self.initialize(world);
            
            if (!initialPercept.empty() && initialPercept.size() == self.getSensoryInputSize()) {
                // Convert vector to SensoryPercept
                auto percept = std::make_shared<SensoryPercept>();
                
                // Inject initial percept
                self.processSensoryInput(percept);
            }
            
            float totalReward = 0.0f;
            for (size_t step = 0; step < maxSteps; ++step) {
                // In a real implementation, you would:
                // 1. Get percept from world
                // 2. Process it through the brain
                // 3. Get action from brain
                // 4. Apply action to world
                // 5. Get reward
                
                // For now, just process existing percept if available
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Get action
                auto action = self.decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, step * 0.001f);
                
                // Get reward (simplified)
                float reward = action.getParameters().empty() ? 0.0f : action.getParameters()[0];
                totalReward += reward;
                
                if (step % 100 == 0) {
                    NLM_LOG_INFO("Episode step " + std::to_string(step) + ", total reward: " + std::to_string(totalReward));
                }
            }
            
            self.reset();
            return totalReward;
        }, py::arg("world"), py::arg("initialPercept"), py::arg("maxSteps") = 1000,
              "Run an episode and return total accumulated reward")
        
        .def("create_learning_scenario", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                            const std::string& scenarioType) {
            world->reset();
            
            if (scenarioType == "avoid_obstacle") {
                // Create scenario where agent must avoid an obstacle
                world->setAgentStart(1.0f, 1.0f);
                // Add obstacle at position (5.0, 5.0)
                auto obstacle = std::make_shared<WorldObject>(5.0f, 5.0f, WorldObjectType::Hazard, 1.0f, 0.5f);
                world->addObject(obstacle);
                
                // Inject percept about obstacle
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
            } else if (scenarioType == "seek_resource") {
                // Create scenario where agent must seek a resource
                world->setAgentStart(1.0f, 1.0f);
                // Add resource at position (8.0, 8.0)
                auto resource = std::make_shared<WorldObject>(8.0f, 8.0f, WorldObjectType::Resource, 10.0f, 0.3f);
                world->addObject(resource);
                
                // Inject percept about resource
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
            } else if (scenarioType == "navigate_maze") {
                // Create a simple maze scenario
                world->setAgentStart(1.0f, 1.0f);
                // Add walls to create maze boundaries
                for (float x = 0; x < 10; x += 1.0f) {
                    auto wall = std::make_shared<WorldObject>(x, 0.0f, WorldObjectType::Wall, 1.0f, 0.2f);
                    world->addObject(wall);
                    wall = std::make_shared<WorldObject>(x, 10.0f, WorldObjectType::Wall, 1.0f, 0.2f);
                    world->addObject(wall);
                }
                for (float y = 0; y < 10; y += 1.0f) {
                    auto wall = std::make_shared<WorldObject>(0.0f, y, WorldObjectType::Wall, 1.0f, 0.2f);
                    world->addObject(wall);
                    wall = std::make_shared<WorldObject>(10.0f, y, WorldObjectType::Wall, 1.0f, 0.2f);
                    world->addObject(wall);
                }
                
                // Inject percept about maze
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
            }
            
            return world;
        }, py::arg("world"), py::arg("scenarioType"),
              "Create a learning scenario in the world and return the modified world")

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

    m.def("create_test_scenario", [](std::shared_ptr<SimpleWorld> world, const std::string& scenarioType) {
        return createAgentBrain_scenario(world, scenarioType);
    }, py::arg("world"), py::arg("scenarioType"),
         "Create a test scenario for an agent brain (convenience function)");

    m.def("run_advanced_test", [](std::shared_ptr<Brain> brain, size_t steps) {
        return run_brain_advanced_test(brain, steps);
    }, py::arg("brain"), py::arg("steps"),
         "Run an advanced test on a brain and return performance metrics");

    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors = validate_config_parameters(config);
        return errors;
    }, py::arg("config"),
         "Validate configuration parameters and return any errors");

    m.def("export_brain_state", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return export_brain_to_file(brain, filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Export brain state to file and return success status");

    m.def("import_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return import_brain_from_file(brain, filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Import brain state from file and return success status");

    m.def("calculate_network_metrics", [](const std::shared_ptr<Brain>& brain) {
        return calculate_brain_metrics(brain);
    }, py::arg("brain"),
         "Calculate comprehensive network metrics for a brain");

    // Invalid IDs for users
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Constants for user convenience
    m.attr("DEFAULT_BRAIN_NEURON_COUNT") = 500;
    m.attr("DEFAULT_BRAIN_REGION_COUNT") = 1;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_RANDOM_SEED") = 42;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.15f;

    // Python module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        Provides classes for:
        
        - Brain: Central neural simulation brain class
        - Config: Configuration system for NLM parameters
        - AgentBrain: Interface connecting NLM brain to world
        - SimpleWorld: Simple 2D world for NLM simulation
        - Vision: Vision sensory input
        - Audio: Audio sensory input
        - InternalSignals: Internal signals sensory input
        - Action: Action representation for motor output
        - WorldObject: World object representation
        - AgentBody: Agent body state
        - ActionResult: Action result from world
        - SensoryPercept: Sensory percept data
        
        Convenience methods and utilities are provided for:
        - Simulation control and monitoring
        - Network analysis and comparison
        - Scenario creation and testing
        - Configuration validation
        - State export/import
        - Performance benchmarking
        
        Usage examples:
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.createBrain()
        
        # Initialize the brain
        brain.initialize()
        
        # Run a simulation
        spikes = brain.run_simulation(1000, logProgress=True)
        
        # Analyze the network
        metrics = brain.analyze_network()
        
        # Create a test scenario
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        world = agent_brain.create_learning_scenario("avoid_obstacle")
        
        # Run an episode
        total_reward = agent_brain.run_episode(world, maxSteps=500)
        ```
        
        For more information, visit: https://nlm.readthedocs.io
    )pbdoc";

} // namespace nlm

// Helper functions implementation

std::shared_ptr<AgentBrain> createAgentBrain_scenario(std::shared_ptr<SimpleWorld> world, const std::string& scenarioType) {
    // Create a dummy brain for testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        return nullptr;
    }
    
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    agentBrain->initialize(world);
    
    return agentBrain;
}

std::map<std::string, double> run_brain_advanced_test(std::shared_ptr<Brain> brain, size_t steps) {
    std::map<std::string, double> results;
    
    // Run advanced test with different scenarios
    brain->reset();
    brain->initialize();
    
    // Scenario 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Scenario 2: Plasticity learning
    runPlasticityExperiment(brain);
    
    // Scenario 3: STDP verification
    runStdpVerification(brain);
    
    // Collect results
    results["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
    results["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
    results["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
    results["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
    results["avg_firing_rate"] = brain->getAverageFiringRate();
    results["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
    results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
    results["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
    
    return results;
}

std::vector<std::string> validate_config_parameters(const std::shared_ptr<Config>& config) {
    std::vector<std::string> errors;
    
    // Basic validation rules
    if (config->getOr<int64_t>("neuron_count", 0) <= 0) {
        errors.push_back("neuron_count must be positive");
    }
    
    if (config->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        errors.push_back("simulation_timestep must be positive");
    }
    
    if (config->getOr<float>("connection_probability", -1.0f) < 0.0f || 
        config->getOr<float>("connection_probability", 2.0f) > 1.0f) {
        errors.push_back("connection_probability must be between 0.0 and 1.0");
    }
    
    // Check for required parameters
    if (!config->has("random_seed")) {
        errors.push_back("random_seed parameter is required");
    }
    
    return errors;
}

bool export_brain_to_file(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    try {
        brain->save(filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export brain to file: " + std::string(e.what()));
        return false;
    }
}

bool import_brain_from_file(std::shared_ptr<Brain> brain, const std::string& filepath) {
    try {
        brain->load(filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import brain from file: " + std::string(e.what()));
        return false;
    }
}

std::map<std::string, double> calculate_brain_metrics(const std::shared_ptr<Brain>& brain) {
    std::map<std::string, double> metrics;
    
    metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
    metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
    metrics["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
    metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
    metrics["avg_firing_rate"] = brain->getAverageFiringRate();
    metrics["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
    metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
    metrics["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
    metrics["plasticity_enabled"] = 1.0; // Simplified
    metrics["novelty_level"] = 0.5; // Simplified
    metrics["prediction_error"] = 0.0; // Simplified
    
    // Calculate additional metrics
    if (metrics["total_neurons"] > 0) {
        metrics["activity_rate"] = metrics["active_neurons"] / metrics["total_neurons"];
        metrics["firing_rate_normalized"] = metrics["firing_neurons"] / metrics["total_neurons"];
    } else {
        metrics["activity_rate"] = 0.0;
        metrics["firing_rate_normalized"] = 0.0;
    }
    
    return metrics;
}
