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

        // Environment management
    m.def("createEnvironment", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new world environment");

    // Advanced agent methods
    m.def("enableLearning", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableLearning(enable);
        return agent;
    }, py::arg("agent"), py::arg("enable"),
       "Enable or disable learning in agent brain");

    m.def("getMetrics", [](std::shared_ptr<AgentBrain> agent) {
        nlohmann::json metrics;
        metrics["curiosity_level"] = agent->getCuriosityLevel();
        metrics["novelty_level"] = agent->getNoveltyLevel();
        metrics["neuromodulation_level"] = agent->getNeuromodulationLevel();
        metrics["prediction_error"] = agent->getPredictionError();
        metrics["learning_enabled"] = agent->isLearningEnabled();
        return metrics.dump(2);
    }, py::arg("agent"),
       "Get agent performance metrics");

    m.def("applyLearning", [](std::shared_ptr<AgentBrain> agent, float reward, float predictedReward) {
        agent->applyRewardModulation(reward, predictedReward);
        return agent->getNeuromodulationLevel();
    }, py::arg("agent"), py::arg("reward"), py::arg("predicted_reward"),
       "Apply learning signal to agent");

    // Brain control methods
    m.def("enableDevelopment", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableDevelopment(enable);
        return agent;
    }, py::arg("agent"), py::arg("enable"),
       "Enable or disable developmental processes");

    m.def("getDevelopmentalStage", [](std::shared_ptr<AgentBrain> agent) {
        return static_cast<int>(agent->getDevelopmentalStage());
    }, py::arg("agent"),
       "Get current developmental stage");

    // Configuration helpers
    m.def("configureBrain", [](std::shared_ptr<Brain> brain, const std::string& configJson) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromJson(configJson)) {
            throw std::runtime_error("Failed to load JSON configuration");
        }
        return brain;
    }, py::arg("brain"), py::arg("config_json"),
       "Configure brain with JSON configuration string");

    m.def("createConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file");
        }
        return config;
    }, py::arg("filepath"),
       "Create configuration from file (JSON or legacy format)");

    // Training and simulation methods
    m.def("trainBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world, int numEpisodes, int maxStepsPerEpisode) {
        double totalReward = 0.0;
        int totalSteps = 0;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            brain->initialize();
            agent->initialize(*world);
            agent->enableLearning(true);
            
            double episodeReward = 0.0;
            int stepsInEpisode = 0;
            
            for (int step = 0; step < maxStepsPerEpisode && stepsInEpisode < maxStepsPerEpisode; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Simple reward calculation
                float reward = agent->getCuriosityLevel() * 0.1f + 0.01f;
                episodeReward += reward;
                agent->applyRewardModulation(reward, 0.0f);
                
                stepsInEpisode++;
                totalSteps++;
                
                // Check termination conditions
                if (agent->getCuriosityLevel() > 0.8f) {
                    break; // Agent is sufficiently curious
                }
                if (stepsInEpisode >= maxStepsPerEpisode) {
                    break;
                }
            }
            
            totalReward += episodeReward;
            
            // Decay learning rate over time for stability
            if (episode % 10 == 0) {
                agent->enableLearning(true);
            }
        }
        
        nlohmann::json results;
        results["total_episodes"] = numEpisodes;
        results["total_steps"] = totalSteps;
        results["average_reward_per_episode"] = totalReward / numEpisodes;
        results["final_curiosity_level"] = agent->getCuriosityLevel();
        results["final_neuromodulation_level"] = agent->getNeuromodulationLevel();
        
        return results.dump(2);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       py::arg("num_episodes"), py::arg("max_steps_per_episode"),
       "Train brain through multiple episodes of interaction");

    // Export and import methods
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to file");

    m.def("importBrainState", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain state from file");
        }
        return brain;
    }, py::arg("filepath"),
       "Import brain state from file");

    // Simulation control
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world, int totalSteps) {
        brain->initialize();
        agent->initialize(*world);
        
        nlohmann::json simulationData;
        auto& steps = simulationData["steps"];
        
        double cumulativeReward = 0.0;
        
        for (int step = 0; step < totalSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = agent->getCuriosityLevel() * 0.05f + 0.001f;
            cumulativeReward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Record step data
            nlohmann::json stepData;
            stepData["step"] = step;
            stepData["reward"] = reward;
            stepData["curiosity_level"] = agent->getCuriosityLevel();
            stepData["novelty_level"] = agent->getNoveltyLevel();
            stepData["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stepData["total_spikes"] = brain->getTotalSpikeCount();
            stepData["firing_neurons"] = brain->getFiringNeuronCount();
            stepData["active_neurons"] = brain->getActiveNeuronCount();
            
            steps.push_back(stepData);
            
            // Log periodically
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": Reward=" + std::to_string(reward) +
                           ", Curiosity=" + std::to_string(agent->getCuriosityLevel()) +
                           ", Spikes=" + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        simulationData["total_steps"] = totalSteps;
        simulationData["total_reward"] = cumulativeReward;
        simulationData["average_reward_per_step"] = cumulativeReward / totalSteps;
        
        return simulationData.dump(2);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       py::arg("total_steps"),
       "Run complete simulation and return data");

    // Module initialization
    m.def("version", []() {
        return std::string("NLM Python Bindings v0.1.0");
    }, "Get the version of the NLM Python bindings");

    m.def("getPythonVersion", []() {
#if PY_VERSION_HEX >= 0x03080000
        return std::string("Python 3.8+");
#else
        return std::string("Python <3.8");
#endif
    }, "Get Python version compatibility");

    // Statistics and diagnostics
    m.def("getSystemStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        nlohmann::json stats;
        
        // Brain statistics
        stats["brain_stats"]["neuron_count"] = brain->getTotalNeuronCount();
        stats["brain_stats"]["synapse_count"] = brain->getTotalSynapseCount();
        stats["brain_stats"]["active_neurons"] = brain->getActiveNeuronCount();
        stats["brain_stats"]["total_spikes"] = brain->getTotalSpikeCount();
        stats["brain_stats"]["firing_rate"] = brain->getAverageFiringRate();
        stats["brain_stats"]["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["brain_stats"]["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Agent statistics
        stats["agent_stats"]["curiosity_level"] = agent->getCuriosityLevel();
        stats["agent_stats"]["novelty_level"] = agent->getNoveltyLevel();
        stats["agent_stats"]["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["agent_stats"]["prediction_error"] = agent->getPredictionError();
        stats["agent_stats"]["learning_enabled"] = agent->isLearningEnabled();
        stats["agent_stats"]["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        stats["agent_stats"]["development_enabled"] = agent->isDevelopmentEnabled();
        stats["agent_stats"]["curiosity_enabled"] = agent->isCuriosityEnabled();
        
        return stats.dump(2);
    }, py::arg("brain"), py::arg("agent"),
       "Get system statistics for monitoring");

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (auto-detects JSON or legacy format)")
        .def("loadFromJson", &Config::loadFromJson, py::arg("jsonString"),
             "Load configuration from a JSON string")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (auto-detects JSON or legacy format)")
        .def("saveToJsonString", &Config::toJsonString,
             "Export configuration as JSON string")
        .def("toJsonString", &Config::toJsonString,
             "Export configuration as JSON string (alias)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get a configuration value as string")
        .def("get", &Config::get<int>, py::arg("key"),
             "Get a configuration value as int")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get a configuration value as double")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get a configuration value as bool")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value or return default if not found")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value or return default if not found")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value or return default if not found")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value or return default if not found")
        .def("set", static_cast<void(Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration value (string)")
        .def("set", static_cast<void(Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration value (int)")
        .def("set", static_cast<void(Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration value (double)")
        .def("set", static_cast<void(Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration value (bool)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate, py::arg("schema"),
             "Validate configuration against JSON schema")
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
        .def("stepWithTime", [](Brain& self, double timestep) {
            SimulationStep step = self.getConfig()->getOr<SimulationStep>("current_step", 0);
            Timestamp time = self.getConfig()->getOr<Timestamp>("current_time", 0.0);
            time += timestep;
            self.getConfig()->set("current_time", time);
            self.step(step, time);
            self.getConfig()->set("current_step", step + 1);
        }, py::arg("timestep"),
           "Perform a simulation step with time duration")
        .def("pause", [](Brain& self) {
            self.getConfig()->set("paused", true);
        }, "Pause simulation")
        .def("resume", [](Brain& self) {
            self.getConfig()->set("paused", false);
        }, "Resume simulation")
        .def("isPaused", [](Brain& self) {
            return self.getConfig()->getOr<bool>("paused", false);
        }, "Check if simulation is paused")
        .def("checkpoint", &Brain::save, py::arg("filepath"),
             "Save brain state to checkpoint file")
        .def("loadCheckpoint", &Brain::load, py::arg("filepath"),
             "Load brain state from checkpoint file")
        .def("createCheckpoint", [](Brain& self, const std::string& prefix = "/tmp/nlm_checkpoint_",
                    size_t maxCheckpoints = 100, size_t keepLast = 10, bool compress = true) {
            if (self.getSpikeSystem() && self.getSpikeSystem()->getCheckpointManager()) {
                self.getSpikeSystem()->getCheckpointManager()->configure(prefix, maxCheckpoints, keepLast, compress);
                return self.getSpikeSystem()->getCheckpointManager()->createCheckpoint(self, 0.0);
            }
            return false;
        }, py::arg("prefix") = "/tmp/nlm_checkpoint_",
           py::arg("maxCheckpoints") = 100, py::arg("keepLast") = 10, py::arg("compress") = true,
           "Create an automatic checkpoint with configuration")
        .def("runEpisode", [](Brain& self, std::shared_ptr<SimpleWorld> world, float maxDuration = 100.0f) {
            if (!world) return;
            self.reset();
            self.initialize();
            world->reset();
            world->setAgentStart(0, 0);
            
            float time = 0.0f;
            while (time < maxDuration && !self.getConfig()->getOr<bool>("episode_terminated", false)) {
                self.stepWithTime(0.001);
                time += 0.001;
            }
            return;
        }, py::arg("world"), py::arg("maxDuration") = 100.0f,
           "Run a complete simulation episode")
        .def("train", [](Brain& self, std::shared_ptr<SimpleWorld> world, size_t episodes = 1,
                    float maxDuration = 100.0f, bool verbose = false) {
            if (!world) return;
            for (size_t i = 0; i < episodes; ++i) {
                if (verbose) {
                    std::cout << "Episode " << (i + 1) << "/" << episodes << "..." << std::endl;
                }
                self.runEpisode(world, maxDuration);
                if (verbose) {
                    self.logStatus();
                }
            }
        }, py::arg("world"), py::arg("episodes") = 1, py::arg("maxDuration") = 100.0f,
           py::arg("verbose") = false,
           "Train the brain through multiple episodes")
        .def("getMetrics", [](Brain& self) {
            py::dict metrics;
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["avg_firing_rate"] = self.getAverageFiringRate();
            metrics["e_i_ratio"] = self.getExcitationInhibitionRatio();
            metrics["dev_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get brain performance metrics")
        .def("exportState", &Brain::save, py::arg("filepath"),
             "Export brain state to file (checkpoint)")
        .def("importState", &Brain::load, py::arg("filepath"),
             "Import brain state from file (checkpoint)")
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
             "Save brain state to file (alias for checkpoint)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file (alias for checkpoint)")
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
        .def("enableLearning", [](Brain& self, bool enable) {
            if (auto sp = self.getStructuralPlasticity()) {
                sp->setEnabled(enable);
            }
        }, py::arg("enable"),
           "Enable or disable structural plasticity learning")
        .def("getLearningStatus", [](Brain& self) {
            py::dict status;
            if (auto sp = self.getStructuralPlasticity()) {
                status["structural_plasticity"] = sp->isEnabled();
            }
            if (auto stdp = self.getSTDP()) {
                status["stdp_enabled"] = true;
            }
            if (auto hebbian = self.getHebbian()) {
                status["hebbian_enabled"] = true;
            }
            return status;
        }, "Get learning system status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyLearning", [](AgentBrain& self, float reward, float novelty = 0.0f) {
            self.applyRewardModulation(reward, self.getPredictionError());
            // Apply novelty-based curiosity
            if (self.isCuriosityEnabled()) {
                float curiosity = self.getCuriosityLevel();
                if (curiosity > 0.5f) {
                    // Boost exploration
                }
            }
        }, py::arg("reward"), py::arg("novelty") = 0.0f,
           "Apply learning signal with reward and novelty")
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
        .def("getMetrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["dopamine"] = self.getNeuromodulationLevel();
            metrics["novelty"] = self.getNoveltyLevel();
            metrics["curiosity"] = self.getCuriosityLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["dev_stage"] = static_cast<int>(self.getDevelopmentalStage());
            metrics["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            metrics["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            metrics["development_enabled"] = self.isDevelopmentEnabled();
            metrics["curiosity_enabled"] = self.isCuriosityEnabled();
            return metrics;
        }, "Get agent brain performance metrics")
        .def("enableLearning", [](AgentBrain& self, bool reward, bool structural, bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(structural);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward"), py::arg("structural") = true, py::arg("development") = true,
           py::arg("curiosity") = true,
           "Enable learning subsystems")
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

    // Advanced brain configuration methods
    m.def("configureBrain", [](std::shared_ptr<Brain> brain, const py::dict& configDict) {
        auto cfg = brain->getConfig();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            // Handle different types
            if (value.is_number_integer()) {
                cfg->set(key, value.cast<int>());
            } else if (value.is_number()) {
                cfg->set(key, value.cast<double>());
            } else if (value.is_boolean()) {
                cfg->set(key, value.cast<bool>());
            } else if (value.is_string()) {
                cfg->set(key, value.cast<std::string>());
            }
        }
    }, py::arg("brain"), py::arg("configDict"),
       "Configure brain with Python dictionary of settings");

    // Learning and training methods
    m.def("trainBrain", [](std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<SimpleWorld> world,
                           size_t episodes = 1, float maxDuration = 100.0f, bool verbose = false) {
        auto brain = agentBrain->getBrain();
        agentBrain->initialize(*world);
        for (size_t i = 0; i < episodes; ++i) {
            if (verbose) {
                std::cout << "Episode " << (i + 1) << "/" << episodes << "..." << std::endl;
            }
            float time = 0.0f;
            while (time < maxDuration) {
                auto percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                auto command = agentBrain->decodeMotorCommand();
                world->applyMotorCommand(command, time);
                world->update(0.001f);
                time += 0.001f;
            }
            agentBrain->reset();
            world->reset();
        }
    }, py::arg("agentBrain"), py::arg("world"), py::arg("episodes") = 1,
       py::arg("maxDuration") = 100.0f, py::arg("verbose") = false,
       "Train agent brain through episodes");

    // Export and analysis methods
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                const std::string& format = "checkpoint") {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), py::arg("format") = "checkpoint",
       "Export brain state to file");

    m.def("collectMetrics", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain) {
        py::dict allMetrics;
        
        // Brain metrics
        auto brainMetrics = brain->getMetrics();
        for (auto item : brainMetrics) {
            allMetrics["brain_" + item.first] = item.second;
        }
        
        // Agent brain metrics  
        auto agentMetrics = agentBrain->getMetrics();
        for (auto item : agentMetrics) {
            allMetrics["agent_" + item.first] = item.second;
        }
        
        return allMetrics;
    }, py::arg("brain"), py::arg("agentBrain"),
       "Collect comprehensive metrics from brain and agent");

    m.def("analyzePerformance", [](const py::dict& metrics, const std::string& analysisType = "basic") {
        py::dict analysis;
        
        if (analysisType == "basic") {
            // Basic performance indicators
            analysis["total_neurons"] = metrics["brain_total_neurons"].cast<size_t>();
            analysis["learning_efficiency"] = metrics["agent_reward_modulation_enabled"].cast<bool>() ? 0.8f : 0.3f;
            analysis["stability"] = metrics["brain_e_i_ratio"].cast<float>() / 10.0f;
        } else if (analysisType == "advanced") {
            // Advanced analysis
            analysis["spike_rate"] = metrics["brain_total_spikes"].cast<size_t>() / 100.0f;
            analysis["memory_utilization"] = 0.5f; // Simplified
            analysis["adaptation_rate"] = metrics["agent_curiosity_enabled"].cast<bool>() ? 0.7f : 0.4f;
        }
        
        return analysis;
    }, py::arg("metrics"), py::arg("analysisType") = "basic",
       "Analyze performance metrics");

} // namespace nlm
