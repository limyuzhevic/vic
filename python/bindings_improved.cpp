#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>

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

// High-level configuration presets
inline std::shared_ptr<Config> createMinimalConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    return config;
}

inline std::shared_ptr<Config> createBrainConfig(size_t neuronCount = 1000,
                                                  size_t regionCount = 1,
                                                  float connectionProb = 0.1f) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(regionCount));
    config->set("connection_probability", connectionProb);
    return config;
}

inline std::shared_ptr<Config> createSimulationConfig(size_t steps = 1000,
                                                      double timestep = 0.001,
                                                      const std::string& checkpointDir = "./checkpoints") {
    auto config = std::make_shared<Config>();
    config->set("simulation_timestep", timestep);
    config->set("checkpoint_dir", checkpointDir);
    config->set("replay_interval", static_cast<int64_t>(steps / 10));
    config->set("consolidation_interval", static_cast<int64_t>(steps));
    return config;
}

// High-level brain wrapper with common operations
class BrainSimulator {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    bool initialized_ = false;
    bool tracking_ = false;
    
public:
    BrainSimulator() : brain_(nullptr), agent_(nullptr), world_(nullptr) {}
    
    BrainSimulator(std::shared_ptr<Config> config) {
        brain_ = std::make_shared<Brain>(config);
    }
    
    // Initialize with configuration
    bool initialize(std::shared_ptr<Config> config) {
        brain_ = std::make_shared<Brain>(config);
        return initialize();
    }
    
    bool initialize() {
        if (!brain_) {
            return false;
        }
        initialized_ = brain_->initialize();
        return initialized_;
    }
    
    // Set up world and agent for simulation
    void setupWorld(double width = 20.0, double height = 20.0,
                   size_t visionWidth = 8, size_t visionHeight = 8,
                   double agentX = 10.0, double agentY = 10.0) {
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(width, height, visionWidth, visionHeight);
        world_->reset();
        world_->setAgentStart(agentX, agentY);
        
        agent_ = std::make_shared<AgentBrain>(brain_);
        agent_->initialize(*world_);
    }
    
    // Enable learning subsystems
    void enableLearning(bool rewardModulation = true,
                       bool structuralPlasticity = true,
                       bool development = true,
                       bool curiosity = true) {
        if (agent_) {
            agent_->enableRewardModulation(rewardModulation);
            agent_->enableStructuralPlasticity(structuralPlasticity);
            agent_->enableDevelopment(development);
            agent_->enableCuriosity(curiosity);
        }
    }
    
    // Run a single simulation step
    void step(size_t stepNumber) {
        if (!brain_ || !initialized_) return;
        
        // Update world
        if (world_) {
            world_->update(0.1);
        }
        
        // Process sensory input
        if (agent_) {
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
        }
        
        // Brain computation
        brain_->step(stepNumber);
        
        // Decode action if using agent
        if (agent_) {
            auto action = agent_->decodeMotorCommand();
            if (world_) {
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
        }
        
        // Apply neuromodulation if learning is enabled
        if (agent_) {
            double reward = 0.0;
            if (world_) {
                auto percept = world_->getSensoryPercept();
                auto internal = percept.getInternal();
                if (!internal.empty()) {
                    reward = internal[0];
                }
            }
            if (reward != 0.0) {
                agent_->applyRewardModulation(reward, 0.0);
            }
        }
    }
    
    // Run simulation for specified number of steps
    void run(size_t numSteps, bool logProgress = false) {
        for (size_t step = 0; step < numSteps; ++step) {
            step(step);
            if (logProgress && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           ", Firing neurons: " + std::to_string(brain_->getFiringNeuronCount()));
            }
        }
    }
    
    // Get simulation statistics
    py::dict getStats() const {
        py::dict stats;
        if (brain_) {
            stats["neurons_total"] = brain_->getTotalNeuronCount();
            stats["neurons_firing"] = brain_->getFiringNeuronCount();
            stats["spikes_total"] = brain_->getTotalSpikeCount();
            stats["synapses_total"] = brain_->getTotalSynapseCount();
            stats["average_firing_rate"] = brain_->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
            
            if (agent_) {
                stats["curiosity_level"] = agent_->getCuriosityLevel();
                stats["novelty_level"] = agent_->getNoveltyLevel();
                stats["prediction_error"] = agent_->getPredictionError();
                stats["neuromodulation_level"] = agent_->getNeuromodulationLevel();
            }
        }
        if (world_) {
            stats["world_width"] = world_->getWidth();
            stats["world_height"] = world_->getHeight();
            stats["simulation_time"] = world_->getSimulationTime();
            auto body = world_->getAgentBody();
            stats["agent_x"] = body.x;
            stats["agent_y"] = body.y;
            stats["agent_energy"] = body.energy;
            stats["agent_health"] = body.health;
        }
        return stats;
    }
    
    // Save brain state to file
    bool save(const std::string& filepath) const {
        return brain_ ? brain_->save(filepath) : false;
    }
    
    // Load brain state from file
    bool load(const std::string& filepath) {
        if (!brain_) return false;
        bool success = brain_->load(filepath);
        if (success) {
            initialized_ = true;
        }
        return success;
    }
    
    // Check if simulator is initialized
    bool isInitialized() const { return initialized_; }
    
    // Reset simulator
    void reset() {
        if (brain_) brain_->reset();
        initialized_ = false;
    }
    
    // Get underlying brain (advanced usage)
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get underlying world (advanced usage)
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    // Get underlying agent (advanced usage)
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
    
    // Context manager support
    BrainSimulator* operator->() { return this; }
};

// Simple simulation helpers
class SimpleSimulation {
public:
    // Run a simple brain-only simulation
    static void runBrainOnly(size_t steps, size_t neuronCount = 1000) {
        auto config = createBrainConfig(neuronCount);
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
                if (step % 100 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                               ", Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
        }
    }
    
    // Run a simple agent-world simulation
    static void runAgentWorld(size_t steps, 
                             size_t worldWidth = 20, size_t worldHeight = 20,
                             double timestep = 0.1) {
        auto brainConfig = createBrainConfig(1000);
        auto simConfig = createSimulationConfig(steps, timestep);
        
        auto brain = std::make_shared<Brain>(brainConfig);
        if (!brain->initialize()) return;
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        agent->enableLearning(true, true, true, true);
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                           ", Action: " + std::to_string(static_cast<int>(action)) +
                           ", Curiosity: " + std::to_string(agent->getCuriosityLevel()));
            }
        }
    }
    
    // Run learning experiment
    static void runLearningExperiment(size_t steps,
                                     const std::string& description = "Learning experiment") {
        NLM_LOG_INFO("=== " + description + " ===");
        
        auto config = createBrainConfig(500); // Smaller brain for faster experiments
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for learning experiment");
            return;
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        world->setAgentStart(7.5, 7.5);
        
        agent->initialize(*world);
        agent->enableLearning(true, true, true, true);
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from world (simplified)
            double reward = 0.0;
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                           ", Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                           ", Action: " + std::to_string(static_cast<int>(action)));
            }
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        NLM_LOG_INFO("Learning experiment completed:");
        NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(finalSpikes - initialSpikes));
        NLM_LOG_INFO("  Final curiosity: " + std::to_string(agent->getCuriosityLevel()));
        NLM_LOG_INFO("  Final novelty: " + std::to_string(agent->getNoveltyLevel()));
    }
};

// Context manager wrapper for Python
template<typename T>
class PyContextManager {
public:
    PyContextManager(T* obj) : obj_(obj) {}
    
    T* operator->() { return obj_; }
    
    T& operator*() { return *obj_; }
    
    ~PyContextManager() {
        delete obj_;
    }
    
private:
    T* obj_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python API
        --------------------------------------------------
        
        This is an enhanced version of the NLM Python bindings with:
        - High-level BrainSimulator class for easy simulation management
        - Simple simulation helper functions
        - Better error handling and context managers
        - Configuration presets for common use cases
        
        The API is designed to be intuitive while maintaining access to
        the full power of the C++ neural simulation framework.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register basic types (keep existing bindings)
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

    // Configuration class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
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

    // Sensory input classes
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

    // Action and world object classes
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

    // Agent body
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

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory percept
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

    // Simple world
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class
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
             "Log brain status");

    // AgentBrain class
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Configuration factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        return createMinimalConfig();
    }, "Create a minimal configuration for basic testing");

    m.def("createBrainConfig", [](size_t neuronCount, size_t regionCount, float connectionProb) -> std::shared_ptr<Config> {
        return createBrainConfig(neuronCount, regionCount, connectionProb);
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1, py::arg("connectionProb") = 0.1f,
       "Create a brain configuration with specified parameters");

    m.def("createSimulationConfig", [](size_t steps, double timestep, const std::string& checkpointDir) -> std::shared_ptr<Config> {
        return createSimulationConfig(steps, timestep, checkpointDir);
    }, py::arg("steps") = 1000, py::arg("timestep") = 0.001, py::arg("checkpointDir") = "./checkpoints",
       "Create a simulation configuration with checkpointing");

    // High-level BrainSimulator class
    py::class_<BrainSimulator>(m, "BrainSimulator", R"pbdoc(High-level brain simulation wrapper for easy use)pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &BrainSimulator::initialize,
             py::arg("config") = nullptr,
             "Initialize the brain simulator with configuration")
        .def("setupWorld", &BrainSimulator::setupWorld,
             py::arg("width") = 20.0, py::arg("height") = 20.0,
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             py::arg("agentX") = 10.0, py::arg("agentY") = 10.0,
             "Set up the simulation world and agent")
        .def("enableLearning", &BrainSimulator::enableLearning,
             py::arg("rewardModulation") = true,
             py::arg("structuralPlasticity") = true,
             py::arg("development") = true,
             py::arg("curiosity") = true,
             "Enable learning subsystems")
        .def("step", &BrainSimulator::step,
             py::arg("stepNumber"),
             "Run a single simulation step")
        .def("run", &BrainSimulator::run,
             py::arg("numSteps"), py::arg("logProgress") = false,
             "Run simulation for specified number of steps")
        .def("getStats", &BrainSimulator::getStats,
             "Get simulation statistics as a dictionary")
        .def("save", &BrainSimulator::save,
             py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainSimulator::load,
             py::arg("filepath"),
             "Load brain state from file")
        .def("isInitialized", &BrainSimulator::isInitialized,
             "Check if simulator is initialized")
        .def("reset", &BrainSimulator::reset,
             "Reset simulator to initial state")
        .def("getBrain", &BrainSimulator::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain (advanced)")
        .def("getWorld", &BrainSimulator::getWorld,
             py::return_value_policy::reference_internal,
             "Get the simulation world (advanced)")
        .def("getAgent", &BrainSimulator::getAgent,
             py::return_value_policy::reference_internal,
             "Get the agent brain interface (advanced)");

    // Simple simulation helpers
    m.def("runBrainOnly", &SimpleSimulation::runBrainOnly,
          py::arg("steps") = 1000, py::arg("neuronCount") = 1000,
          "Run a simple brain-only simulation");

    m.def("runAgentWorld", &SimpleSimulation::runAgentWorld,
          py::arg("steps") = 1000, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
          py::arg("timestep") = 0.1,
          "Run a simple agent-world simulation");

    m.def("runLearningExperiment", &SimpleSimulation::runLearningExperiment,
          py::arg("steps") = 1000, py::arg("description") = "Learning experiment",
          "Run a learning experiment with the agent");

    // Common constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;

    // Version
    m.attr("__version__") = "0.1.0-enhanced";

} // namespace nlm
