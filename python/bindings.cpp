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
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Advanced convenience methods
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            // Get object at position
            auto objects = self.getObjects(); // Assuming this method exists
            for (const auto& obj : objects) {
                if (std::abs(obj.x - x) < 0.5f && std::abs(obj.y - y) < 0.5f) {
                    return obj;
                }
            }
            return WorldObject(); // Return empty object if not found
        })
        .def("hasObjectAt", &SimpleWorld::isValidPosition)
        .def("stepWorld", [](SimpleWorld& self, float timestep, 
                           const std::function<void(float, float)>& on_action) {
            // Convenience method for running world steps with action callbacks
            for (float t = 0; t < timestep; t += 0.1f) {
                self.update(t);
                auto percept = self.getSensoryPercept();
                auto body = self.getAgentBody();
                // In a real implementation, you would process this
                on_action(body.x, body.y);
            }
        });

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
             "Log brain status")
        // Advanced Brain convenience methods
        .def("runSteps", [](Brain& self, size_t num_steps, double timestep = 0.001,
                          const std::function<void(size_t)> callback = nullptr) {
            """Run multiple simulation steps efficiently"""
            for (size_t step = 0; step < num_steps; ++step) {
                self.step(step, step * timestep);
                if (callback) {
                    callback(step);
                }
            }
        })
        .def("runPlasticityExperiment", [](Brain& self, size_t num_steps,
                                          const std::vector<NeuronId>& pattern_neurons,
                                          float current_strength = 30.0f) {
            """Run a structured plasticity experiment"""
            // Enable plasticity
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            // Apply pattern
            for (size_t step = 0; step < num_steps; ++step) {
                for (const auto& neuron_id : pattern_neurons) {
                    self.injectCurrent(neuron_id, current_strength);
                }
                self.step(step, step * 0.001);
            }
        })
        .def("getConnectivityStats", [](const Brain& self) {
            """Get detailed connectivity statistics"""
            auto stats = py::dict();
            
            size_t total_neurons = self.getTotalNeuronCount();
            size_t total_synapses = self.getTotalSynapseCount();
            size_t firing_neurons = self.getFiringNeuronCount();
            float ei_ratio = self.getExcitationInhibitionRatio();
            
            stats["total_neurons"] = total_neurons;
            stats["total_synapses"] = total_synapses;
            stats["firing_neurons"] = firing_neurons;
            stats["excitation_inhibition_ratio"] = ei_ratio;
            stats["synaptic_density"] = total_neurons > 0 ? 
                static_cast<float>(total_synapses) / total_neurons : 0.0f;
            
            return stats;
        })
        .def("simulateToTarget", [](Brain& self, size_t target_spikes,
                                   double timestep = 0.001) {
            """Simulate until target spike count is reached"""
            size_t current_spikes = self.getTotalSpikeCount();
            size_t steps = 0;
            
            while (current_spikes < target_spikes && steps < 1000000) {
                self.step(steps, steps * timestep);
                current_spikes = self.getTotalSpikeCount();
                steps++;
            }
            
            return steps;
        });

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
        // Advanced AgentBrain convenience methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, 
                           std::shared_ptr<py::dict> episode_data = nullptr,
                           int max_steps = 1000) {
            """Run a complete agent episode with optional data collection"""
            int steps = 0;
            bool episode_complete = false;
            
            while (!episode_complete && steps < max_steps) {
                world.update(0.1f); // Standard timestep
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto brain = self.getBrain();
                brain->step(steps, world.getSimulationTime());
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Collect episode data if requested
                if (episode_data) {
                    auto step_data = py::dict(
                        "step", steps,
                        "brain_spikes", brain->getTotalSpikeCount(),
                        "firing_rate", brain->getAverageFiringRate(),
                        "action", action,
                        "reward", world.getSensoryPercept().getInternal()[0]
                    );
                    (*episode_data)[std::to_string(steps)] = step_data;
                }
                
                steps++;
                
                // Check for episode termination conditions
                auto brain = self.getBrain();
                if (brain->getFiringNeuronCount() < 1) {  // Too quiet
                    episode_complete = true;
                }
                // Add more termination conditions here (e.g., reward threshold)
            }
            
            return steps;
        })
        .def("simulateUntil", [](AgentBrain& self, SimpleWorld& world,
                                const std::function<bool(const std::shared_ptr<Brain>&)>& condition,
                                int max_steps = 10000) {
            """Simulate until a condition is met or max steps reached"""
            for (int step = 0; step < max_steps; ++step) {
                world.update(0.1f);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto brain = self.getBrain();
                brain->step(step, world.getSimulationTime());
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                if (condition(brain)) {
                    return step;
                }
            }
            return max_steps;
        })

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

    // Advanced factory functions and convenience methods
    m.def("createBrainWithDefaultConfig", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a configuration object");

    m.def("createWorldWithDimensions", [](size_t width, size_t height,
                                         size_t visionWidth = 8, size_t visionHeight = 8) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"),
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Create and configure a simple world");

    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent brain and initialize it with a world");

    // Performance monitoring utilities
    m.def("simulateBrainEfficiently", [](std::shared_ptr<Brain> brain, size_t steps,
                                       const std::function<bool(const std::shared_ptr<Brain>&)>& stop_condition = nullptr) {
        """Run a brain simulation with optional early termination"""
        size_t current_step = 0;
        bool should_stop = false;
        
        while (current_step < steps && !should_stop) {
            brain->step(current_step, current_step * 0.001);
            current_step++;
            
            if (stop_condition && stop_condition(brain)) {
                should_stop = true;
            }
        }
        
        return current_step;
    }, py::arg("brain"), py::arg("steps"),
        py::arg("stop_condition") = nullptr,
        "Run brain simulation efficiently with early termination");

    m.def("createLearningEnvironment", [](const std::string& config_file = "configs/default.cfg") -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        // Create a complete learning environment
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable key subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config_file") = "configs/default.cfg",
        "Create a complete learning environment (brain, world, agent)");

    m.def("createConfigPresets", []() {
        """Get predefined configuration presets"""
        auto presets = py::dict();
        
        // Create a simple config
        auto simple_config = std::make_shared<Config>();
        simple_config->set("brain.neuron_count", 500);
        simple_config->set("brain.synapse_density", 0.1);
        simple_config->set("plasticity.stdp.enable", true);
        
        // Create an advanced config
        auto advanced_config = std::make_shared<Config>();
        advanced_config->set("brain.neuron_count", 2000);
        advanced_config->set("brain.synapse_density", 0.15);
        advanced_config->set("plasticity.stdp.enable", true);
        advanced_config->set("plasticity.hebbian.enable", true);
        advanced_config->set("plasticity.structural.enable", true);
        advanced_config->set("neuromod.dopamine.scale", 2.0f);
        
        // Create a minimal config
        auto minimal_config = std::make_shared<Config>();
        minimal_config->set("brain.neuron_count", 100);
        minimal_config->set("brain.synapse_density", 0.05);
        
        presets["simple"] = simple_config;
        presets["advanced"] = advanced_config;
        presets["minimal"] = minimal_config;
        
        return presets;
    }, "Get predefined configuration presets");

    m.def("createExperimentSetup", [](const std::string& experiment_type = "plasticity",
                                    const std::vector<std::string>& custom_config = {}) -> py::dict {
        """Create a complete experiment setup with predefined parameters"""
        auto setup = py::dict();
        
        if (experiment_type == "plasticity") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(30, 30, 10, 10);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            
            setup["brain"] = brain;
            setup["world"] = world;
            setup["agent"] = agent;
            setup["config"] = config;
            
        } else if (experiment_type == "development") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.08);
            config->set("development.enabled", true);
            config->set("neuromod.curiosity.enable", true);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(15, 15, 6, 6);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            
            setup["brain"] = brain;
            setup["world"] = world;
            setup["agent"] = agent;
            setup["config"] = config;
        }
        
        return setup;
    }, py::arg("experiment_type") = "plasticity",
        py::arg("custom_config") = std::vector<std::string>(),
        "Create a complete experiment setup");

    // Utility functions
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        """Benchmark brain performance"""
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double steps_per_second = (steps * 1000.0) / duration.count();
        
        return py::make_tuple(steps, duration.count(), steps_per_second);
    }, py::arg("brain"), py::arg("steps"),
        "Benchmark brain performance and return stats");

    m.def("profileBrainActivity", [](std::shared_ptr<Brain> brain, size_t steps) {
        """Profile brain activity and collect statistics"""
        auto stats = py::dict();
        
        size_t max_firing = 0;
        size_t total_spikes = 0;
        float total_energy = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            size_t firing = brain->getFiringNeuronCount();
            max_firing = std::max(max_firing, firing);
            total_spikes = brain->getTotalSpikeCount();
            
            // Note: Brain doesn't expose energy consumption directly
            // This would require access to internal systems
        }
        
        stats["max_firing_neurons"] = max_firing;
        stats["total_spikes"] = total_spikes;
        stats["average_firing_rate"] = total_spikes / static_cast<float>(steps);
        stats["final_spike_count"] = brain->getTotalSpikeCount();
        
        return stats;
    }, py::arg("brain"), py::arg("steps"),
        "Profile brain activity and collect statistics");

    // Advanced simulation helpers
    m.def("createControlledSimulation", [](std::shared_ptr<Brain> brain,
                                          const std::function<void(size_t)> pre_step,
                                          const std::function<void(size_t)> post_step,
                                          size_t steps) {
        """Run a simulation with pre and post step callbacks"""
        for (size_t step = 0; step < steps; ++step) {
            pre_step(step);
            brain->step(step, step * 0.001);
            post_step(step);
        }
    }, py::arg("brain"), py::arg("pre_step"), py::arg("post_step"), py::arg("steps"),
        "Run a simulation with custom callbacks");

    m.def("createDataLogger", [](std::shared_ptr<Brain> brain,
                               const std::string& log_file,
                               size_t log_interval = 100) {
        """Create a data logger for brain simulation"""
        auto logger = [brain, log_file, log_interval](size_t current_step) {
            if (current_step % log_interval == 0) {
                auto stats = py::dict();
                stats["step"] = current_step;
                stats["spike_count"] = brain->getTotalSpikeCount();
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["avg_firing_rate"] = brain->getAverageFiringRate();
                
                // In a real implementation, you would write this to a file
                // std::ofstream file(log_file, std::ios::app);
                // file << "Step: " << stats["step"].cast<size_t>() 
                //      << ", Spikes: " << stats["spike_count"].cast<size_t>()
                //      << ", Firing: " << stats["firing_neurons"].cast<size_t>() << std::endl;
            }
        };
        
        return logger;
    }, py::arg("brain"), py::arg("log_file"), py::arg("log_interval") = 100,
        "Create a data logger for brain simulation");

    // Special high-level constructors for advanced users
    m.def("createResearchGradeBrain", [](const std::string& config_file = "configs/default.cfg") -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<Config>> {
        """Create a research-grade brain with all systems enabled"""
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        
        // Enable all advanced systems
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enabled", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return std::make_tuple(brain, config);
    }, py::arg("config_file") = "configs/default.cfg",
        "Create a research-grade brain with all advanced systems enabled");

    m.def("createProductionBrain", [](size_t neuron_count = 5000) -> std::shared_ptr<Brain> {
        """Create a production-ready brain with optimized settings"""
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", 0.12);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.002f);
        config->set("plasticity.hebbian.enable", false); // For stability
        config->set("plasticity.structural.enable", true);
        config->set("simulation_timestep", 0.001f);
        config->set("checkpoint.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("neuron_count") = 5000,
        "Create a production-ready brain with optimized settings");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
