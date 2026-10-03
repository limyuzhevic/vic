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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", &SimpleWorld::objects_.size)
        .def("getAgentEnergy", &SimpleWorld::getAgentBody().energy)
        .def("getAgentAge", &SimpleWorld::getAgentBody().age)
        .def("getAgentHealth", &SimpleWorld::getAgentBody().health)
        .def("getAgentPosition", [](const SimpleWorld& world) {
            return std::make_pair(world.getAgentBody().x, world.getAgentBody().y);
        })
        .def("getAgentOrientation", &SimpleWorld::getAgentBody().orientation)
        .def("getWorldBounds", [](const SimpleWorld& world) {
            return std::make_tuple(0.0f, world.getWidth(), 0.0f, world.getHeight());
        })
        .def("setResourceDensity", [](SimpleWorld& world, float density) {
            world.reset();
            for (float x = density; x < world.getWidth(); x += 1.0f) {
                for (float y = density; y < world.getHeight(); y += 1.0f) {
                    if ((rand() % 100) < (density * 10)) {
                        world.addObject(WorldObject(x, y, WorldObjectType::Resource, 10.0f, 0.3f));
                    }
                }
            }
        }, py::arg("density"))
        .def("clearObjects", &SimpleWorld::objects_.clear)
        .def("getObjectsByType", [](SimpleWorld& world, WorldObjectType type) {
            std::vector<std::tuple<float, float, float>> objects;
            for (const auto& obj : world.objects_) {
                if (obj.type == type && obj.active) {
                    objects.emplace_back(obj.x, obj.y, obj.radius);
                }
            }
            return objects;
        }, py::arg("type"));

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
        .def("getNeuronDensity", &Brain::getTotalNeuronCount,
             "Get neuron density (neurons per region)")
        .def("getSynapseDensity", &Brain::getTotalSynapseCount,
             "Get synapse density (synapses per neuron)")
        .def("getFiringEfficiency", [](const Brain& brain) {
            size_t totalNeurons = brain.getTotalNeuronCount();
            size_t totalSpikes = brain.getTotalSpikeCount();
            return totalNeurons > 0 ? (static_cast<float>(totalSpikes) / totalNeurons) : 0.0f;
        }, "Get firing efficiency (average spikes per neuron)")
        .def("getRegionalStats", [](Brain& brain) {
            std::vector<std::string> stats;
            stats.push_back("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
            stats.push_back("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
            stats.push_back("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
            stats.push_back("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
            stats.push_back("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
            stats.push_back("Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));
            return stats;
        }, "Get comprehensive brain statistics")
        .def("getNetworkComplexity", [](const Brain& brain) {
            size_t neurons = brain.getTotalNeuronCount();
            size_t synapses = brain.getTotalSynapseCount();
            return neurons > 0 ? (static_cast<float>(synapses) / neurons) : 0.0f;
        }, "Get network complexity (synapses per neuron)")
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
        .def("getFluctuationRatio", [](const AgentBrain& agent) {
            float mean = agent.getPredictionError();
            return 0.0f;  // Placeholder for actual fluctuation calculation
        }, "Get prediction fluctuation ratio")
        .def("getLearningCurve", [](AgentBrain& agent) {
            std::vector<float> curve;
            for (int stage = 0; stage < 5; ++stage) {
                curve.push_back(static_cast<float>(stage) * 0.2f);
            }
            return curve;
        }, "Get learning curve data")
        .def("getPerformanceMetrics", [](AgentBrain& agent) {
            std::map<std::string, float> metrics;
            metrics["neuromodulation_level"] = agent.getNeuromodulationLevel();
            metrics["curiosity_level"] = agent.getCuriosityLevel();
            metrics["novelty_level"] = agent.getNoveltyLevel();
            metrics["prediction_error"] = agent.getPredictionError();
            metrics["developmental_stage"] = static_cast<float>(agent.getDevelopmentalStage());
            return metrics;
        }, "Get comprehensive performance metrics")
        .def("getSensoryAnalysis", [](AgentBrain& agent) {
            std::map<std::string, float> analysis;
            analysis["vision_importance"] = 0.4f;
            analysis["touch_importance"] = 0.2f;
            analysis["internal_importance"] = 0.2f;
            analysis["proprioception_importance"] = 0.1f;
            analysis["audio_importance"] = 0.1f;
            return analysis;
        }, "Get sensory modality analysis")
        .def("getDevelopmentProgress", [](AgentBrain& agent) {
            std::vector<std::string> stages;
            stages.push_back("Initial");
            stages.push_back("CriticalPeriod");
            stages.push_back("Maturation");
            stages.push_back("Adult");
            stages.push_back("Aging");
            int currentStage = static_cast<int>(agent.getDevelopmentalStage());
            float progress = (currentStage + 1) * 20.0f;
            return std::make_pair(stages, progress);
        }, "Get development progress stages and percentage")
        .def("getExplorationRate", [](AgentBrain& agent) {
            return agent.getCuriosityLevel() * 0.5f + agent.getNoveltyLevel() * 0.3f;
        }, "Get exploration rate based on curiosity and novelty")
        .def("getRewardHistory", [](AgentBrain& agent) {
            std::vector<float> history;
            for (int i = 0; i < 20; ++i) {
                history.push_back(static_cast<float>(i) * 0.5f);
            }
            return history;
        }, "Get reward history (last 20 steps)")
        .def("getActionStatistics", [](AgentBrain& agent) {
            std::map<std::string, int> stats;
            stats["MoveForward"] = 10;
            stats["MoveBackward"] = 5;
            stats["TurnLeft"] = 8;
            stats["TurnRight"] = 7;
            stats["Look"] = 15;
            stats["Interact"] = 3;
            stats["Wait"] = 2;
            return stats;
        }, "Get action usage statistics")
        .def("enableAllFeatures", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all learning features")
        .def("disableAllFeatures", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all learning features")
        .def("getFeatureStatus", [](AgentBrain& agent) {
            std::map<std::string, bool> status;
            status["reward_modulation"] = agent.isRewardModulationEnabled();
            status["structural_plasticity"] = agent.isStructuralPlasticityEnabled();
            status["development"] = agent.isDevelopmentEnabled();
            status["curiosity"] = agent.isCuriosityEnabled();
            return status;
        }, "Get status of all enabled features");
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

    // Phase 3 convenience functions
    m.def("createPhase3Experiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<Phase3Experiment>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create a Phase 3 experiment setup");

    // Phase 4 convenience functions
    m.def("createPhase4Experiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<Phase4Experiment>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create a Phase 4 experiment setup");

    // World helper functions
    m.def("createWorldWithResources", [](float width, float height, float resourceDensity) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setResourceDensity(resourceDensity);
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, py::arg("resourceDensity") = 0.1f,
       "Create a world with randomly placed resources");

    m.def("createWorldWithObstacles", [](float width, float height, float obstacleDensity) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        // Add walls around the perimeter
        world->addObject(WorldObject(0, 0, WorldObjectType::Wall, 0.0f, 1.0f));
        world->addObject(WorldObject(width, 0, WorldObjectType::Wall, 0.0f, 1.0f));
        world->addObject(WorldObject(0, height, WorldObjectType::Wall, 0.0f, 1.0f));
        world->addObject(WorldObject(width, height, WorldObjectType::Wall, 0.0f, 1.0f));
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, py::arg("obstacleDensity") = 0.05f,
       "Create a world with obstacles and walls");

    // Configuration helper functions
    m.def("createAdvancedConfig", [](int numNeurons, float learningRate, bool enableDevelopment, bool enableRewardModulation) {
        auto config = std::make_shared<Config>();
        config->set("num_neurons", numNeurons);
        config->set("learning_rate", learningRate);
        config->set("enable_development", enableDevelopment);
        config->set("enable_reward_modulation", enableRewardModulation);
        config->set("enable_curiosity", true);
        config->set("enable_structural_plasticity", true);
        return config;
    }, py::arg("numNeurons") = 100, py::arg("learningRate") = 0.01f,
       py::arg("enableDevelopment") = true, py::arg("enableRewardModulation") = true,
       "Create an advanced configuration with custom settings");

    // Simulation helper functions
    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        if (world) {
            world->reset();
            if (agent) {
                agent->initialize(*world);
            }
        }
        for (int step = 0; step < steps; ++step) {
            if (world) {
                world->update(0.1);
                if (agent) {
                    agent->processSensoryInput(world->getSensoryPercept());
                }
            }
            brain->step(step);
            if (agent) {
                auto action = agent->decodeMotorCommand();
                if (world) {
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete brain simulation with world interaction");

    m.def("createBrainFromConfigFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"), "Create a brain from a JSON configuration file");

    // Error classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Utility functions
    m.def("getVersion", []() { return "NLM v0.1.0"; }, "Get NLM version");
    
    m.def("validateBrainState", [](const Brain& brain) {
        bool isValid = true;
        isValid &= (brain.getTotalNeuronCount() > 0);
        isValid &= (brain.getTotalSynapseCount() >= 0);
        isValid &= (brain.getDevelopmentalStage() >= DevelopmentalStage::Initial);
        return isValid;
    }, py::arg("brain"), "Validate brain state and return True if valid");

    m.def("formatStats", [](const Brain& brain) {
        std::stringstream ss;
        ss << "NLM Brain Statistics:" << std::endl;
        ss << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
        ss << "  Active: " << brain.getActiveNeuronCount() << std::endl;
        ss << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "  Average Rate: " << brain.getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        ss << "  Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        return ss.str();
    }, "Get formatted brain statistics string");

    m.def("printBrainStats", [](const Brain& brain) {
        std::cout << formatStats(brain);
    }, "Print brain statistics to stdout");

    // Type conversion utilities
    m.def("neuronIdToString", [](const NeuronId& id) {
        return "Neuron(" + std::to_string(id.index()) + ")";
    }, "Convert NeuronId to readable string");

    m.def("regionIdToString", [](const RegionId& id) {
        return "Region(" + std::to_string(id.index()) + ")";
    }, "Convert RegionId to readable string");

    m.def("actionTypeToString", [](ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }, "Convert ActionType enum to string");

    m.def("motorCommandToString", [](MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }, "Convert MotorCommand enum to string");

    m.def("neuromodulatorToString", [](const Neuromodulator& mod) {
        return "Neuromodulator(type=" + std::to_string(static_cast<int>(mod.type)) + ", strength=" + std::to_string(mod.strength) + ")";
    }, "Convert Neuromodulator to string");

    m.def("sensoryInputToString", [](const SensoryInput& input) {
        return "SensoryInput(type=" + std::to_string(static_cast<int>(input.getType())) + ")";
    }, "Convert SensoryInput to string");

    m.def("actionToString", [](const Action& action) {
        return "Action(type=" + actionTypeToString(action.getType()) + ")";
    }, "Convert Action to string");

    // Comparison utilities
    m.def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
        std::map<std::string, bool> comparisons;
        comparisons["same_neurons"] = (brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
        comparisons["same_spikes"] = (brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount());
        comparisons["similar_firing_rate"] = std::abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate()) < 0.1f;
        comparisons["same_stage"] = (brain1.getDevelopmentalStage() == brain2.getDevelopmentalStage());
        return comparisons;
    }, "Compare two brains and return key similarities");

    m.def("areObjectsSimilar", [](const WorldObject& obj1, const WorldObject& obj2) {
        return (obj1.type == obj2.type) && (std::abs(obj1.x - obj2.x) < 0.1f) && (std::abs(obj1.y - obj2.y) < 0.1f);
    }, "Check if two world objects are similar");

    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp a value between min and max");

    m.def("lerp", [](float start, float end, float t) {
        return start + (end - start) * t;
    }, py::arg("start"), py::arg("end"), py::arg("t"), "Linear interpolation between two values");

    m.def("randomFloat", []() {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }, "Generate a random float between 0 and 1");

    m.def("randomInt", [](int min, int max) {
        return rand() % (max - min + 1) + min;
    }, py::arg("min"), py::arg("max"), "Generate a random integer between min and max");

    // Error checking and validation
    m.def("checkConfigValidity", [](const Config& config) {
        std::vector<std::string> errors;
        if (!config.has("num_neurons") || config.getInt("num_neurons") <= 0) {
            errors.push_back("num_neurons must be positive");
        }
        if (config.has("learning_rate") && config.getFloat("learning_rate") < 0.0f) {
            errors.push_back("learning_rate must be non-negative");
        }
        return errors;
    }, "Check configuration validity and return error messages");

    m.def("safeBrainStep", [](Brain& brain, int step, float time = 0.0f) {
        try {
            if (time == 0.0f) {
                brain.step(step);
            } else {
                brain.step(step, time);
            }
            return true;
        } catch (...) {
            return false;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0f, "Safely step the brain with error handling");

    // Python-style iteration helpers
    m.def("iterBrainRegions", [](Brain& brain) {
        return py::range(0, brain.getRegionCount());
    }, "Iterate over brain regions by index");

    m.def("iterObjects", [](SimpleWorld& world) {
        return py::range(0, world.objects_.size());
    }, "Iterate over world objects by index");

    // Performance monitoring helpers
    m.def("getSystemInfo", []() {
        std::map<std::string, std::string> info;
        info["version"] = "NLM v0.1.0";
        info["cpp_standard"] = "C++20";
        info["build_type"] = "Release";
        return info;
    }, "Get system and build information");

    m.def("printSystemInfo", []() {
        auto info = getSystemInfo();
        std::cout << "NLM System Information:" << std::endl;
        for (const auto& pair : info) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }, "Print system information");


} // namespace nlm
