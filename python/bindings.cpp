#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Python exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error("NLM Error: " + message) {}
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message) : NLMError("Validation Error: " + message) {}
};

class SerializationError : public NLMError {
public:
    SerializationError(const std::string& message) : NLMError("Serialization Error: " + message) {}
};

class CallbackError : public NLMError {
public:
    CallbackError(const std::string& message) : NLMError("Callback Error: " + message) {}
};

// Pythonic utility functions
std::vector<float> perceptToVector(const SensoryPercept& percept) {
    return percept.getAllSignals();
}

SensoryPercept vectorToPercept(const std::vector<float>& data, 
                               size_t visionSize = 256, 
                               size_t touchSize = 8, 
                               size_t internalSize = 4, 
                               size_t proprioSize = 6) {
    SensoryPercept percept;
    
    size_t offset = 0;
    if (data.size() >= offset + visionSize) {
        std::vector<float> vision(data.begin() + offset, data.begin() + offset + visionSize);
        percept.setVision(vision);
        offset += visionSize;
    }
    
    if (data.size() >= offset + touchSize) {
        std::vector<float> touch(data.begin() + offset, data.begin() + offset + touchSize);
        percept.setTouch(touch);
        offset += touchSize;
    }
    
    if (data.size() >= offset + internalSize) {
        std::vector<float> internal(data.begin() + offset, data.begin() + offset + internalSize);
        percept.setInternal(internal);
        offset += internalSize;
    }
    
    if (data.size() >= offset + proprioSize) {
        std::vector<float> proprio(data.begin() + offset, data.begin() + offset + proprioSize);
        percept.setProprioception(proprio);
    }
    
    return percept;
}

MotorCommand indexToMotorCommand(size_t index) {
    if (index >= 7) return MotorCommand::Wait; // Safety check
    
    switch (index) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

std::vector<MotorCommand> indicesToMotorCommands(const std::vector<size_t>& indices) {
    std::vector<MotorCommand> commands;
    commands.reserve(indices.size());
    
    for (size_t index : indices) {
        commands.push_back(indexToMotorCommand(index));
    }
    
    return commands;
}

std::vector<size_t> motorCommandsToIndices(const std::vector<MotorCommand>& commands) {
    std::vector<size_t> indices;
    indices.reserve(commands.size());
    
    for (MotorCommand cmd : commands) {
        switch (cmd) {
            case MotorCommand::MoveForward: indices.push_back(0); break;
            case MotorCommand::MoveBackward: indices.push_back(1); break;
            case MotorCommand::TurnLeft: indices.push_back(2); break;
            case MotorCommand::TurnRight: indices.push_back(3); break;
            case MotorCommand::LookLeft: indices.push_back(4); break;
            case MotorCommand::LookRight: indices.push_back(5); break;
            case MotorCommand::Interact: indices.push_back(6); break;
            default: indices.push_back(7); // Wait or invalid
        }
    }
    
    return indices;
}

// High-level convenience functions
void runEpisodeSimulation(std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world,
                         const std::vector<SensoryPercept>& initialPercepts,
                         size_t maxSteps,
                         float episodeThreshold,
                         bool saveEpisodeState = false,
                         const std::string& episodeFile = "episode_state.json") {
    
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    if (!world) {
        throw ValidationError("SimpleWorld pointer is null");
    }
    
    // Initialize agent with world
    agent->initialize(*world);
    
    // Start episode
    size_t episodeId = agent->getCurrentEpisode();
    agent->startEpisode(episodeId, "simulation");
    
    std::vector<float> rewards;
    std::vector<float> predictedRewards;
    
    // Process percepts and run simulation
    for (size_t step = 0; step < maxSteps; ++step) {
        // Process current percept
        if (step < initialPercepts.size()) {
            agent->processSensoryInput(initialPercepts[step], false);
        }
        
        // Get motor command
        MotorCommand action = agent->decodeMotorCommand();
        
        // Record action
        float reward = 0.0f; // Simulated reward
        agent->recordEpisodeAction(action, reward);
        rewards.push_back(reward);
        predictedRewards.push_back(0.0f); // Placeholder for predicted reward
        
        // Apply reward shaping if enabled
        agent->applyRewardShaping(reward);
        
        // End episode if threshold reached
        if (agent->getEpisodeScore() >= episodeThreshold) {
            agent->endEpisode(reward);
            break;
        }
    }
    
    // Save episode state if requested
    if (saveEpisodeState) {
        if (!agent->saveState(episodeFile)) {
            throw SerializationError("Failed to save episode state to " + episodeFile);
        }
    }
    
    // Get final metrics
    agent->collectMetrics();
}

void batchProcessPercepts(std::shared_ptr<AgentBrain> agent,
                         const std::vector<std::vector<float>>& batchPercepts,
                         const std::vector<float>& rewards,
                         const std::vector<float>& predictedRewards,
                         bool validateInputs = true) {
    
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    std::vector<SensoryPercept> percepts;
    percepts.reserve(batchPercepts.size());
    
    // Convert vectors to percepts
    for (const auto& data : batchPercepts) {
        SensoryPercept percept = vectorToPercept(data);
        if (validateInputs && !agent->validateSensoryPercept(percept)) {
            throw ValidationError("Invalid sensory percept in batch processing");
        }
        percepts.push_back(percept);
    }
    
    // Process batch of percepts
    agent->processPercepts(percepts);
    
    // Apply batch of rewards if same size
    if (rewards.size() == percepts.size()) {
        for (size_t i = 0; i < rewards.size(); ++i) {
            if (validateInputs && !agent->validateRewardValue(rewards[i])) {
                throw ValidationError("Invalid reward value in batch processing");
            }
            if (i < predictedRewards.size()) {
                agent->applyRewardModulation(rewards[i], predictedRewards[i]);
            }
        }
    }
}

void checkpointAndSave(std::shared_ptr<AgentBrain> agent,
                      const std::string& checkpointName,
                      const std::string& stateFile = "agent_state.json") {
    
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    // Create checkpoint
    agent->setCheckpoint(0.5f);
    
    // Save state
    if (!agent->saveState(stateFile)) {
        throw SerializationError("Failed to save agent state to " + stateFile);
    }
}

std::map<std::string, double> getPerformanceMetrics(std::shared_ptr<AgentBrain> agent) {
    std::map<std::string, double> metrics;
    
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    auto perf = agent->profilePerformance();
    metrics["efficiency"] = perf.processingEfficiency;
    metrics["fps"] = perf.stepsPerSecond;
    metrics["memory_mb"] = perf.memoryUsageBytes / (1024.0 * 1024.0);
    metrics["session_uptime"] = perf.sessionDurationSeconds;
    metrics["average_step_time_ms"] = perf.averageStepTime * 1000.0;
    
    return metrics;
}

std::string formatMetricsSummary(std::shared_ptr<AgentBrain> agent,
                                 const std::string& format = "text") {
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    std::string summary = agent->getMetricsSummary();
    
    if (format == "json") {
        auto metrics = agent->collectMetrics();
        std::map<std::string, double> metricMap;
        metricMap["total_steps"] = metrics.totalSteps;
        metricMap["total_reward"] = metrics.avgReward;
        metricMap["avg_prediction_error"] = metrics.avgPredictionError;
        metricMap["avg_curiosity"] = metrics.avgCuriosity;
        metricMap["avg_novelty"] = metrics.avgNovelty;
        
        // Convert to JSON string
        json j = metricMap;
        return j.dump(2);
    }
    
    return summary;
}

void runLearningSession(std::shared_ptr<AgentBrain> agent,
                       std::shared_ptr<SimpleWorld> world,
                       size_t episodeCount = 10,
                       size_t maxStepsPerEpisode = 100,
                       float learningRate = 0.01f,
                       float explorationRate = 0.1f) {
    
    if (!agent) {
        throw ValidationError("AgentBrain pointer is null");
    }
    
    if (!world) {
        throw ValidationError("SimpleWorld pointer is null");
    }
    
    // Configure learning parameters
    agent->enableLearning(true);
    agent->setLearningRate(learningRate);
    agent->setExplorationRate(explorationRate);
    
    // Run learning episodes
    for (size_t episode = 0; episode < episodeCount; ++episode) {
        agent->initialize(*world);
        agent->startEpisode(episode, "learning");
        
        // Simulate episode
        for (size_t step = 0; step < maxStepsPerEpisode; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept, false);
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward
            float reward = 0.0f; // Simplified reward
            agent->applyRewardModulation(reward, 0.0f);
            
            // Record action
            agent->recordEpisodeAction(action, reward);
            
            // Update development
            agent->updateDevelopment(1.0);
            
            // End episode if triggered
            if (agent->isEpisodeActive()) {
                // Simulate episode ending based on conditions
                if (step % 20 == 19) { // End every 20 steps for demo
                    agent->endEpisode(reward);
                    break;
                }
            }
        }
        
        // Trigger learning epoch periodically
        if (episode % 3 == 2) {
            agent->triggerLearningEpoch();
        }
        
        // Collect metrics
        agent->collectMetrics();
    }
}

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

    py::class_<AgentBrain> agentBrainClass(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc");
    
    // Define the bindings for AgentBrain
    agentBrainClass
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
        
        // === CONVENIENCE METHODS ===
        
        // Batch processing methods for multiple steps
        .def("processPercepts", &AgentBrain::processPercepts,
             py::arg("percepts"),
             "Process multiple sensory percepts")
        .def("decodeCommands", &AgentBrain::decodeCommands,
             py::arg("steps"),
             "Decode motor commands for multiple steps")
        .def("stepMultiple", &AgentBrain::stepMultiple,
             py::arg("percepts"), py::arg("rewards"), py::arg("predictedRewards"),
             py::arg("maxSteps") = 100,
             "Process multiple steps with batch processing")
        
        // State checkpoint/save/load utilities
        .def("saveState", &AgentBrain::saveState,
             py::arg("filepath"),
             "Save brain state to file")
        .def("loadState", &AgentBrain::loadState,
             py::arg("filepath"),
             "Load brain state from file")
        .def("setCheckpoint", &AgentBrain::setCheckpoint,
             py::arg("threshold") = 0.5f,
             "Set a checkpoint with given threshold")
        .def("loadCheckpoint", &AgentBrain::loadCheckpoint,
             py::arg("name"),
             "Load checkpoint by name")
        .def("getAvailableCheckpoints", &AgentBrain::getAvailableCheckpoints,
             "Get list of available checkpoints")
        
        // Simulation statistics and metrics collection
        .def("collectMetrics", &AgentBrain::collectMetrics,
             "Collect simulation metrics")
        .def("resetMetrics", &AgentBrain::resetMetrics,
             "Reset metrics collection")
        .def("getMetricsSummary", &AgentBrain::getMetricsSummary,
             "Get metrics summary as string")
        
        // High-level learning control methods
        .def("enableLearning", &AgentBrain::enableLearning,
             py::arg("enable"),
             "Enable or disable learning")
        .def("setLearningRate", &AgentBrain::setLearningRate,
             py::arg("rate"),
             "Set learning rate")
        .def("setExplorationRate", &AgentBrain::setExplorationRate,
             py::arg("rate"),
             "Set exploration rate")
        .def("setRewardShaping", &AgentBrain::setRewardShaping,
             py::arg("shapingFactor"),
             "Set reward shaping factor")
        .def("setCuriosityExploration", &AgentBrain::setCuriosityExploration,
             py::arg("exploration"),
             "Set curiosity-based exploration level")
        .def("triggerLearningEpoch", &AgentBrain::triggerLearningEpoch,
             "Trigger learning epoch")
        .def("triggerCriticalPeriod", &AgentBrain::triggerCriticalPeriod,
             "Trigger critical period development")
        .def("triggerAdultStage", &AgentBrain::triggerAdultStage,
             "Trigger adult development stage")
        
        // === ADVANCED FEATURES ===
        
        // Reward shaping and penalty systems
        .def("applyRewardShaping", &AgentBrain::applyRewardShaping,
             py::arg("baseReward"), py::arg("shapingFactor") = 1.0f,
             "Apply reward shaping")
        .def("addPenalty", &AgentBrain::addPenalty,
             py::arg("penalty"), py::arg("reason") = "",
             "Add a penalty with optional reason")
        .def("clearPenalties", &AgentBrain::clearPenalties,
             "Clear all penalties")
        .def("getPenalties", &AgentBrain::getPenalties,
             "Get list of penalties")
        .def("getTotalPenalty", &AgentBrain::getTotalPenalty,
             "Get total penalty value")
        
        // Episode management and reset utilities
        .def("isEpisodeActive", &AgentBrain::isEpisodeActive,
             "Check if episode is active")
        .def("startEpisode", &AgentBrain::startEpisode,
             py::arg("episodeId") = 0, py::arg("episodeType") = "default",
             "Start a new episode")
        .def("endEpisode", &AgentBrain::endEpisode,
             py::arg("finalReward"),
             "End current episode with final reward")
        .def("forceReset", &AgentBrain::forceReset,
             "Force reset (equivalent to reset but for episodes)")
        .def("getCurrentEpisode", &AgentBrain::getCurrentEpisode,
             "Get current episode ID")
        .def("getEpisodeType", &AgentBrain::getEpisodeType,
             "Get episode type")
        .def("getEpisodeScore", &AgentBrain::getEpisodeScore,
             "Get current episode score")
        .def("recordEpisodeAction", &AgentBrain::recordEpisodeAction,
             py::arg("action"), py::arg("reward"),
             "Record action and reward for episode")
        
        // Memory introspection and debugging tools
        .def("introspectMemory", &AgentBrain::introspectMemory,
             "Get memory statistics and introspection data")
        .def("getNeuronActivityProfile", &AgentBrain::getNeuronActivityProfile,
             "Get neuron activity profile")
        .def("getActiveNeurons", &AgentBrain::getActiveNeurons,
             "Get list of active neurons with activity levels")
        .def("getSynapticStrengths", &AgentBrain::getSynapticStrengths,
             "Get synaptic strength measurements")
        
        // Performance profiling utilities
        .def("profilePerformance", &AgentBrain::profilePerformance,
             "Get performance statistics")
        .def("getProcessingEfficiency", &AgentBrain::getProcessingEfficiency,
             "Get processing efficiency ratio")
        .def("getFPS", &AgentBrain::getFPS,
             "Get frames per second (steps per second)")
        .def("getMemoryUsage", &AgentBrain::getMemoryUsage,
             "Get current memory usage in bytes")
        .def("getSessionUptime", &AgentBrain::getSessionUptime,
             "Get session uptime in seconds")
        
        // === ERROR HANDLING AND VALIDATION ===
        
        // Input validation methods
        .def("validateSensoryPercept", &AgentBrain::validateSensoryPercept,
             py::arg("percept"),
             "Validate sensory percept input")
        .def("validateRewardValue", &AgentBrain::validateRewardValue,
             py::arg("reward"),
             "Validate reward value range")
        .def("validateMotorCommand", &AgentBrain::validateMotorCommand,
             py::arg("cmd"),
             "Validate motor command")
        .def("validateWorldState", &AgentBrain::validateWorldState,
             py::arg("world"),
             "Validate world state")
        
        // State validation utilities
        .def("validateBrainState", &AgentBrain::validateBrainState,
             "Validate brain state consistency")
        .def("validateNeuromodulationState", &AgentBrain::validateNeuromodulationState,
             "Validate neuromodulation state")
        .def("validateDevelopmentState", &AgentBrain::validateDevelopmentState,
             "Validate development state")
        .def("validateMemoryState", &AgentBrain::validateMemoryState,
             "Validate memory state")
        
        // === PYTHON-SPECIFIC FEATURES ===
        
        // Type conversion utilities for Python interop
        .def("perceptToPythonVector", &AgentBrain::perceptToPythonVector,
             py::arg("percept"),
             "Convert percept to Python vector for interop")
        .def("pythonVectorToPercept", &AgentBrain::pythonVectorToPercept,
             py::arg("data"),
             "Convert Python vector to percept")
        .def("getMotorCommandFromIndex", &AgentBrain::getMotorCommandFromIndex,
             py::arg("index"),
             "Get motor command from index")
        .def("getMotorCommandIndex", &AgentBrain::getMotorCommandIndex,
             py::arg("cmd"),
             "Get motor command index")
        .def("pythonListToMotorCommands", &AgentBrain::pythonListToMotorCommands,
             py::arg("indices"),
             "Convert Python list of indices to motor commands")
        .def("motorCommandsToPythonList", &AgentBrain::motorCommandsToPythonList,
             py::arg("commands"),
             "Convert motor commands to Python list of indices")
        
        // Serialization methods for Python objects
        .def("serializeToJSON", &AgentBrain::serializeToJSON,
             "Serialize agent brain state to JSON")
        .def("serializeToMsgPack", &AgentBrain::serializeToMsgPack,
             "Serialize agent brain state to MessagePack")
        .def("deserializeFromJSON", &AgentBrain::deserializeFromJSON,
             py::arg("jsonStr"),
             "Deserialize agent brain state from JSON")
        .def("deserializeFromMsgPack", &AgentBrain::deserializeFromMsgPack,
             py::arg("msgpackStr"),
             "Deserialize agent brain state from MessagePack")
        
        // Callback mechanism support
        .def("addRewardCallback", &AgentBrain::addRewardCallback,
             py::arg("callback"),
             "Add reward callback function")
        .def("addActionCallback", &AgentBrain::addActionCallback,
             py::arg("callback"),
             "Add action callback function")
        .def("addLearningCallback", &AgentBrain::addLearningCallback,
             py::arg("callback"),
             "Add learning callback function")
        .def("removeRewardCallback", &AgentBrain::removeRewardCallback,
             py::arg("id"),
             "Remove reward callback by ID")
        .def("removeActionCallback", &AgentBrain::removeActionCallback,
             py::arg("id"),
             "Remove action callback by ID")
        .def("removeLearningCallback", &AgentBrain::removeLearningCallback,
             py::arg("id"),
             "Remove learning callback by ID")
        .def("clearAllCallbacks", &AgentBrain::clearAllCallbacks,
             "Clear all callbacks")
        
        // Debug and introspection utilities
        .def("getStateSummary", &AgentBrain::getStateSummary,
             "Get state summary as string")
        .def("debugInfo", &AgentBrain::debugInfo,
             "Get debug information")
        .def("enableDebugMode", &AgentBrain::enableDebugMode,
             py::arg("enable"),
             "Enable or disable debug mode")
        .def("isDebugModeEnabled", &AgentBrain::isDebugModeEnabled,
             "Check if debug mode is enabled")
        .def("logMetrics", &AgentBrain::logMetrics,
             "Log metrics to output")
        
        // Utility methods for Python
        .def("normalizeReward", &AgentBrain::normalizeReward,
             py::arg("reward"), py::arg("min") = -1.0f, py::arg("max") = 1.0f,
             "Normalize reward to range [min, max]")
        .def("denormalizeReward", &AgentBrain::denormalizeReward,
             py::arg("normalizedReward"), py::arg("min") = -1.0f, py::arg("max") = 1.0f,
             "Denormalize reward from range [min, max]")
                .def("selectActionWithProbability", &AgentBrain::selectActionWithProbability,
             py::arg("actions"), py::arg("probabilities"),
             "Select action based on probabilities")
        .def("computeDistance", &AgentBrain::computeDistance,
             py::arg("vec1"), py::arg("vec2"),
             "Compute distance between two vectors")
        
        // === ADVANCED FEATURES ===
        
        // === ADVANCED LEARNING ALGORITHMS ===
        
        // Advanced learning algorithms
        .def("enableCurriculumLearning", &AgentBrain::enableCurriculumLearning,
             py::arg("enable"), py::arg("strategy") = "difficulty_adaptive",
             "Enable curriculum learning with adaptive difficulty")
        .def("setTransferLearningTarget", &AgentBrain::setTransferLearningTarget,
             py::arg("sourceBrain"),
             "Set transfer learning target from source brain")
        .def("enableMetaLearning", &AgentBrain::enableMetaLearning,
             py::arg("enable"), py::arg("metaLearningRate") = 0.01f,
             "Enable meta-learning with specified learning rate")
        .def("setCustomPlasticityRule", &AgentBrain::setCustomPlasticityRule,
             py::arg("ruleName"), py::arg("parameters"),
             "Set custom plasticity rule with parameters")
        .def("applyAdvancedRewardShaping", &AgentBrain::applyAdvancedRewardShaping,
             py::arg("baseReward"), py::arg("shapingFactor") = 1.0f, py::arg("strategy") = "exponential",
             "Apply advanced reward shaping with specified strategy")
        .def("triggerMetaLearningCycle", &AgentBrain::triggerMetaLearningCycle,
             "Trigger meta-learning cycle")
        
        // === MEMORY MANAGEMENT ===
        
        // Enhanced memory management
        .def("enableMemoryPooling", &AgentBrain::enableMemoryPooling,
             py::arg("enable"), py::arg("poolSize") = 1024,
             "Enable memory pooling with specified pool size")
        .def("setCustomMemoryAllocator", &AgentBrain::setCustomMemoryAllocator,
             py::arg("allocator"), py::arg("deallocator"),
             "Set custom memory allocator and deallocator functions")
        .def("enableMemoryEfficientCheckpointing", &AgentBrain::enableMemoryEfficientCheckpointing,
             py::arg("enable"), py::arg("compression") = "gzip",
             "Enable memory-efficient checkpointing with compression")
        .def("enablePersistentMemory", &AgentBrain::enablePersistentMemory,
             py::arg("persistentPath"),
             "Enable persistent memory with specified path")
        .def("optimizeMemoryUsage", &AgentBrain::optimizeMemoryUsage,
             "Optimize memory usage efficiency")
        .def("getMemoryFootprint", &AgentBrain::getMemoryFootprint,
             "Get total memory footprint in bytes")
        .def("monitorMemoryUsage", &AgentBrain::monitorMemoryUsage,
             py::arg("thresholdMB") = 1000.0f,
             "Monitor memory usage and alert if threshold exceeded")
        
        // === ADVANCED NEURAL DYNAMICS ===
        
        // Advanced neural dynamics
        .def("registerCustomNeuronModel", &AgentBrain::registerCustomNeuronModel,
             py::arg("modelName"), py::arg("activationFunc"), py::arg("derivativeFunc"),
             "Register custom neuron model with activation and derivative functions")
        .def("setNetworkTopology", &AgentBrain::setNetworkTopology,
             py::arg("connectivityMatrix"),
             "Set network topology with connectivity matrix")
        .def("enableSparseConnectivity", &AgentBrain::enableSparseConnectivity,
             py::arg("sparsityLevel") = 0.95f,
             "Enable sparse connectivity with specified sparsity level")
        .def("optimizeNetworkTopology", &AgentBrain::optimizeNetworkTopology,
             "Optimize network topology for better performance")
        .def("setStructuralPlasticityControl", &AgentBrain::setStructuralPlasticityControl,
             py::arg("structuralLearningRate") = 0.01f, py::arg("structuralDecayRate") = 0.001f,
             "Set structural plasticity control parameters")
        .def("configureNeuromodulation", &AgentBrain::configureNeuromodulation,
             py::arg("dopamineGain") = 1.0f, py::arg("curiosityGain") = 1.0f, py::arg("noveltyGain") = 1.0f,
             "Configure neuromodulation with specified gains")
        
        // === PERFORMANCE OPTIMIZATION ===
        
        // Performance optimization
        .def("enableParallelProcessing", &AgentBrain::enableParallelProcessing,
             py::arg("enable"), py::arg("numThreads") = std::thread::hardware_concurrency(),
             "Enable or disable parallel processing with specified thread count")
        .def("enableVectorization", &AgentBrain::enableVectorization,
             py::arg("enable"),
             "Enable or disable vectorization optimizations")
        .def("enableGPUEnabled", &AgentBrain::enableGPUEnabled,
             py::arg("enable"),
             "Enable or disable GPU acceleration")
        .def("setCustomCompilationFlags", &AgentBrain::setCustomCompilationFlags,
             py::arg("flags"),
             "Set custom compilation flags for optimization")
        .def("enablePerformanceProfiling", &AgentBrain::enablePerformanceProfiling,
             py::arg("enable"), py::arg("samplingInterval") = 0.1f,
             "Enable or disable performance profiling with specified sampling interval")
        .def("optimizeForLatency", &AgentBrain::optimizeForLatency,
             "Optimize system for minimum latency")
        .def("optimizeForThroughput", &AgentBrain::optimizeForThroughput,
             "Optimize system for maximum throughput")
        .def("getLatency", &AgentBrain::getLatency,
             "Get current system latency in seconds")
        .def("getThroughput", &AgentBrain::getThroughput,
             "Get current system throughput in operations per second")
        
        // === ADVANCED SIMULATION FEATURES ===
        
        // Advanced simulation features
        .def("enableMultiAgentCoordination", &AgentBrain::enableMultiAgentCoordination,
             py::arg("enable"),
             "Enable or disable multi-agent coordination")
        .def("addAgentToTeam", &AgentBrain::addAgentToTeam,
             py::arg("agent"), py::arg("teamId") = "default",
             "Add agent brain to specified team for coordinated learning")
        .def("enableTeamLearning", &AgentBrain::enableTeamLearning,
             py::arg("teamId"), py::arg("learningStrategy") = "collaborative",
             "Enable team learning with specified strategy")
        .def("enableTeamCompetition", &AgentBrain::enableTeamCompetition,
             py::arg("competitionType") = "cooperative",
             "Enable team competition with specified competition type")
        .def("configureCommunicationProtocol", &AgentBrain::configureCommunicationProtocol,
             py::arg("protocol"), py::arg("parameters"),
             "Configure communication protocol with parameters")
        .def("enableSocialLearning", &AgentBrain::enableSocialLearning,
             py::arg("enable"), py::arg("socialLearningRate") = 0.1f,
             "Enable or disable social learning with specified rate")
        .def("analyzeEmergentBehaviors", &AgentBrain::analyzeEmergentBehaviors,
             "Analyze emergent behaviors in multi-agent system")
        .def("getEmergentBehaviorMetrics", &AgentBrain::getEmergentBehaviorMetrics,
             "Get metrics describing emergent behaviors")
        
        // === EXPERT TOOLS AND UTILITIES ===
        
        // Expert tools and utilities
        .def("createVisualizationSnapshot", &AgentBrain::createVisualizationSnapshot,
             py::arg("outputPath") = "visualization.png",
             "Create visualization snapshot of current brain state")
        .def("enableExperimentTracking", &AgentBrain::enableExperimentTracking,
             py::arg("trackingId") = "default",
             "Enable experiment tracking with specified ID")
        .def("configureHyperparameterTuning", &AgentBrain::configureHyperparameterTuning,
             py::arg("searchSpace"), py::arg("optimizationAlgorithm") = "bayesian",
             "Configure hyperparameter tuning with search space and algorithm")
        .def("triggerHyperparameterOptimization", &AgentBrain::triggerHyperparameterOptimization,
             "Trigger hyperparameter optimization process")
        .def("getHyperparameterImportance", &AgentBrain::getHyperparameterImportance,
             "Get importance scores for hyperparameters")
        .def("enableModelInterpretability", &AgentBrain::enableModelInterpretability,
             py::arg("enable"),
             "Enable or disable model interpretability features")
        .def("generateModelExplanation", &AgentBrain::generateModelExplanation,
             "Generate human-readable explanation of model behavior")
        .def("runPerformanceBenchmark", &AgentBrain::runPerformanceBenchmark,
             py::arg("benchmarkSuite") = "default",
             "Run performance benchmark suite")
        .def("getBenchmarkResults", &AgentBrain::getBenchmarkResults,
             "Get results from performance benchmark runs")
        
        // === EXISTING METHODS (for compatibility) ===
        
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), py::arg("computeNovelty"),
             "Process sensory input with novelty computation")
        .def("applyAdvancedModulation", &AgentBrain::applyAdvancedModulation,
             py::arg("reward"), py::arg("predictedReward"), py::arg("shapingFactor") = 1.0f,
             "Apply advanced reward modulation")
        .def("updateDevelopmentalStage", &AgentBrain::updateDevelopmentalStage,
             py::arg("timestep"),
             "Update developmental stage");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === PYTHON EXCEPTION DEFINITIONS ===
    
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());
    py::register_exception<SerializationError>(m, "SerializationError", PyExc_IOError.ptr());
    py::register_exception<CallbackError>(m, "CallbackError", PyExc_RuntimeError.ptr());
}

} // namespace nlm

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === PYTHON EXCEPTION DEFINITIONS ===
    
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());
    py::register_exception<SerializationError>(m, "SerializationError", PyExc_IOError.ptr());
    py::register_exception<CallbackError>(m, "CallbackError", PyExc_RuntimeError.ptr());
}

} // namespace nlm
