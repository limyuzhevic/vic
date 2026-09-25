#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

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

// High-level convenience functions for creating common NLM setups

namespace {
    // Create a simple agent with default configuration
    std::shared_ptr<Brain> createSimpleAgent() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
    // Create a simple agent with world
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createSimpleAgentWorld(int width = 20, int height = 20, 
                          int visionWidth = 8, int visionHeight = 8) {
        auto brain = createSimpleAgent();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run a complete agent simulation loop
    void runAgentSimulation(std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent,
                           int numSteps,
                           bool enableLearning = true,
                           bool enableCuriosity = false,
                           std::function<void(int, float)> callback = nullptr) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("Invalid agent components");
        }
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get action and execute it
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation if learning is enabled
            if (enableLearning) {
                // Simple reward based on action success
                float reward = action->getType() == ActionType::MoveForward ? 0.1f : 0.0f;
                agent->applyRewardModulation(reward, reward * 0.5f);
            }
            
            // Call user callback if provided
            if (callback) {
                float firingRate = brain->getAverageFiringRate();
                callback(step, firingRate);
            }
        }
    }
    
    // Create a complete agent with learning features
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createLearningAgent(int width = 20, int height = 20,
                       int visionWidth = 8, int visionHeight = 8,
                       bool enableStructuralPlasticity = false,
                       bool enableDevelopment = false,
                       bool enableNovelty = false) {
        auto brain = createSimpleAgent();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        if (enableStructuralPlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        if (enableNovelty) {
            agent->enableNovelty(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run simulation with statistics collection
    struct SimulationStats {
        int totalSteps;
        int finalSpikeCount;
        float averageFiringRate;
        float maxFiringRate;
        int totalNeuronCount;
        int activeNeuronCount;
        int regionCount;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "SimulationStats:" << std::endl;
            ss << "  Total Steps: " << totalSteps << std::endl;
            ss << "  Final Spike Count: " << finalSpikeCount << std::endl;
            ss << "  Average Firing Rate: " << averageFiringRate << std::endl;
            ss << "  Max Firing Rate: " << maxFiringRate << std::endl;
            ss << "  Total Neuron Count: " << totalNeuronCount << std::endl;
            ss << "  Active Neuron Count: " << activeNeuronCount << std::endl;
            ss << "  Region Count: " << regionCount << std::endl;
            return ss.str();
        }
    };
    
    // Run simulation and collect statistics
    SimulationStats runSimulationWithStats(std::shared_ptr<Brain> brain,
                                          std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent,
                                          int numSteps,
                                          bool enableLearning = true) {
        SimulationStats stats;
        stats.totalSteps = numSteps;
        stats.maxFiringRate = 0.0f;
        
        runAgentSimulation(brain, world, agent, numSteps, enableLearning,
                          false,  // No curiosity for this run
                          [&stats](int step, float firingRate) {
                              if (firingRate > stats.maxFiringRate) {
                                  stats.maxFiringRate = firingRate;
                              }
                          });
        
        stats.finalSpikeCount = brain->getTotalSpikeCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.totalNeuronCount = brain->getTotalNeuronCount();
        stats.activeNeuronCount = brain->getActiveNeuronCount();
        stats.regionCount = brain->getRegionCount();
        
        return stats;
    }
    
    // Create and save a trained brain
    std::shared_ptr<Brain> trainAndSaveBrain(const std::string& filepath,
                                            int width = 20, int height = 20,
                                            int visionWidth = 8, int visionHeight = 8,
                                            int trainingSteps = 1000) {
        auto [brain, world, agent] = createLearningAgent(width, height, visionWidth, visionHeight);
        
        runSimulationWithStats(brain, world, agent, trainingSteps, true);
        
        // Save the trained brain
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to save trained brain");
        }
        
        return brain;
    }
    
    // Load a trained brain
    std::shared_ptr<Brain> loadTrainedBrain(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain from " + filepath);
        }
        
        return brain;
    }
    
    // Create a simulation with progress tracking
    class SimulationRunner {
    public:
        SimulationRunner(std::shared_ptr<Brain> brain,
                        std::shared_ptr<SimpleWorld> world,
                        std::shared_ptr<AgentBrain> agent)
            : brain_(brain), world_(world), agent_(agent) {}
        
        void run(int numSteps, bool enableLearning = true) {
            runAgentSimulation(brain_, world_, agent_, numSteps, enableLearning);
        }
        
        void runWithProgress(int numSteps, bool enableLearning = true, 
                           std::function<void(float)> progressCallback = nullptr) {
            if (!progressCallback) {
                progressCallback = [](float) {};
            }
            
            runAgentSimulation(brain_, world_, agent_, numSteps, enableLearning,
                              false,  // No curiosity for this run
                              [this, progressCallback](int step, float firingRate) {
                                  float progress = static_cast<float>(step) / numSteps;
                                  progressCallback(progress);
                              });
        }
        
        std::shared_ptr<Brain> getBrain() const { return brain_; }
        std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
        std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };
    
    // Create a simulation runner
    SimulationRunner createSimulationRunner(int width = 20, int height = 20,
                                            int visionWidth = 8, int visionHeight = 8) {
        auto [brain, world, agent] = createLearningAgent(width, height, visionWidth, visionHeight);
        return SimulationRunner(brain, world, agent);
    }
    
    // Create a world with obstacles
    std::shared_ptr<SimpleWorld> createWorldWithObstacles(int width = 20, int height = 20,
                                                         int visionWidth = 8, int visionHeight = 8,
                                                         std::vector<std::pair<float, float>> obstacles = {}) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        
        // Add obstacles
        for (const auto& obstacle : obstacles) {
            auto obj = std::make_unique<WorldObject>();
            obj->x = obstacle.first;
            obj->y = obstacle.second;
            obj->type = WorldObjectType::Wall;
            obj->radius = 0.8f;
            obj->active = true;
            world->addObject(*obj);
        }
        
        return world;
    }
    
    // Create a world with resources
    std::shared_ptr<SimpleWorld> createWorldWithResources(int width = 20, int height = 20,
                                                         int visionWidth = 8, int visionHeight = 8,
                                                         std::vector<std::pair<float, float>> resources = {}) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        
        // Add resources
        for (const auto& resource : resources) {
            auto obj = std::make_unique<WorldObject>();
            obj->x = resource.first;
            obj->y = resource.second;
            obj->type = WorldObjectType::Resource;
            obj->radius = 0.3f;
            obj->value = 1.0f;
            obj->active = true;
            world->addObject(*obj);
        }
        
        return world;
    }
    
    // Simple agent controller for basic behavior
    class SimpleAgentController {
    public:
        SimpleAgentController(std::shared_ptr<Brain> brain,
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent)
            : brain_(brain), world_(world), agent_(agent) {}
        
        void run() {
            // Simple navigation behavior
            for (int step = 0; step < 1000; ++step) {
                world_->update(0.1f);
                
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                
                brain_->step(step);
                
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // Simple logging
                if (step % 100 == 0) {
                    std::cout << "Step " << step << ": " 
                             << brain_->getFiringNeuronCount() << " neurons firing, "
                             << brain_->getTotalSpikeCount() << " total spikes" << std::endl;
                }
            }
        }
        
        float getAverageFiringRate() const {
            return brain_->getAverageFiringRate();
        }
        
        int getTotalSpikeCount() const {
            return brain_->getTotalSpikeCount();
        }
        
        size_t getRegionCount() const {
            return brain_->getRegionCount();
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };
    
    // Create simple agent controller
    SimpleAgentController createSimpleAgentController(int width = 20, int height = 20,
                                                      int visionWidth = 8, int visionHeight = 8) {
        auto [brain, world, agent] = createLearningAgent(width, height, visionWidth, visionHeight);
        return SimpleAgentController(brain, world, agent);
    }
} // anonymous namespace

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Real-time neural simulation with LIF neuron dynamics
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (reward, curiosity, novelty)
        - Sensorimotor integration
        - Developmental learning capabilities
        - Checkpoint save/load for persistent training
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // Type identifiers for Python users
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Basic type definitions
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

    // Enum types
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

    // Configuration system
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

    // Sensory input system
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

    // Action system
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " with " + std::to_string(a.getParameters().size()) + " params>";
        });

    // World object
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + std::to_string(obj.y) + " type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        });

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
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& r) {
            return "<ActionResult: reward=" + std::to_string(r.reward) + ", success=" + std::to_string(r.success) + ">";
        });

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
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by obstacle)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class - the core neural network
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Produce motor/action output based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpoint)")
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

    // Agent brain interface
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
             py::arg("enable"),
             "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions for convenience
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

    // ADVANCED FEATURE: High-level simulation utilities
    
    // Create advanced agent with learning capabilities
    m.def("createLearningAgent", 
        [](int width, int height, int visionWidth, int visionHeight,
           bool enableStructuralPlasticity, bool enableDevelopment, bool enableNovelty) {
            return createLearningAgent(width, height, visionWidth, visionHeight,
                                     enableStructuralPlasticity, enableDevelopment, enableNovelty);
        },
        py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        py::arg("enableStructuralPlasticity") = false,
        py::arg("enableDevelopment") = false,
        py::arg("enableNovelty") = false,
        "Create an agent with advanced learning features");

    // Simple creation without parameters
    m.def("createLearningAgent", []() {
        return createLearningAgent();
    }, "Create an agent with default learning features");

    // Simulation utilities
    m.def("runSimulation",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int numSteps,
           bool enableLearning, bool enableCuriosity) {
            runAgentSimulation(brain, world, agent, numSteps, enableLearning, enableCuriosity);
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("numSteps"), py::arg("enableLearning") = true,
        py::arg("enableCuriosity") = false,
        "Run a complete agent simulation loop");

    // Simulation runner for more control
    py::class_<SimulationRunner>(m, "SimulationRunner",
        R"pbdoc(A high-level simulation controller with progress tracking and statistics.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("run", &SimulationRunner::run,
             py::arg("numSteps"), py::arg("enableLearning") = true,
             "Run simulation for specified number of steps")
        .def("runWithProgress", &SimulationRunner::runWithProgress,
             py::arg("numSteps"), py::arg("enableLearning") = true,
             py::arg("progressCallback") = nullptr,
             "Run simulation with progress callback (0.0 to 1.0)")
        .def("getBrain", &SimulationRunner::getBrain,
             py::return_value_policy::reference_internal,
             "Get the brain reference")
        .def("getWorld", &SimulationRunner::getWorld,
             py::return_value_policy::reference_internal,
             "Get the world reference")
        .def("getAgent", &SimulationRunner::getAgent,
             py::return_value_policy::reference_internal,
             "Get the agent reference");

    m.def("createSimulationRunner",
        [](int width, int height, int visionWidth, int visionHeight) {
            return createSimulationRunner(width, height, visionWidth, visionHeight);
        },
        py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        "Create a simulation runner with progress tracking");

    // World creation utilities
    m.def("createWorldWithObstacles",
        [](int width, int height, int visionWidth, int visionHeight,
           std::vector<std::pair<float, float>> obstacles) {
            return createWorldWithObstacles(width, height, visionWidth, visionHeight, obstacles);
        },
        py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        py::arg("obstacles") = std::vector<std::pair<float, float>>(),
        "Create a world with obstacles at specified positions");

    m.def("createWorldWithResources",
        [](int width, int height, int visionWidth, int visionHeight,
           std::vector<std::pair<float, float>> resources) {
            return createWorldWithResources(width, height, visionWidth, visionHeight, resources);
        },
        py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        py::arg("resources") = std::vector<std::pair<float, float>>(),
        "Create a world with resources at specified positions");

    // Simple agent controller for basic behavior
    py::class_<SimpleAgentController>(m, "SimpleAgentController",
        R"pbdoc(A simple controller for basic agent behavior and navigation.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("run", &SimpleAgentController::run,
             "Run the simple agent controller for basic navigation")
        .def("getAverageFiringRate", &SimpleAgentController::getAverageFiringRate,
             "Get average firing rate from the brain")
        .def("getTotalSpikeCount", &SimpleAgentController::getTotalSpikeCount,
             "Get total spike count from the brain")
        .def("getRegionCount", &SimpleAgentController::getRegionCount,
             "Get number of brain regions");

    m.def("createSimpleAgentController",
        [](int width, int height, int visionWidth, int visionHeight) {
            return createSimpleAgentController(width, height, visionWidth, visionHeight);
        },
        py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        "Create a simple agent controller for basic behavior");

    // Training and checkpoint utilities
    m.def("trainAndSaveBrain",
        [](const std::string& filepath, int width, int height,
           int visionWidth, int visionHeight, int trainingSteps) {
            return trainAndSaveBrain(filepath, width, height, visionWidth, visionHeight, trainingSteps);
        },
        py::arg("filepath"), py::arg("width") = 20,
        py::arg("height") = 20,
        py::arg("visionWidth") = 8,
        py::arg("visionHeight") = 8,
        py::arg("trainingSteps") = 1000,
        "Train a brain and save it to a file");

    m.def("loadTrainedBrain",
        [](const std::string& filepath) {
            return loadTrainedBrain(filepath);
        },
        py::arg("filepath"),
        "Load a trained brain from a file");

    // Statistics collection
    py::class_<SimulationStats>(m, "SimulationStats",
        R"pbdoc(Statistics collected during simulation.)pbdoc")
        .def(py::init<>())
        .def_readonly("totalSteps", &SimulationStats::totalSteps,
                     "Total number of simulation steps")
        .def_readonly("finalSpikeCount", &SimulationStats::finalSpikeCount,
                     "Final spike count")
        .def_readonly("averageFiringRate", &SimulationStats::averageFiringRate,
                     "Average firing rate")
        .def_readonly("maxFiringRate", &SimulationStats::maxFiringRate,
                     "Maximum firing rate observed")
        .def_readonly("totalNeuronCount", &SimulationStats::totalNeuronCount,
                     "Total number of neurons")
        .def_readonly("activeNeuronCount", &SimulationStats::activeNeuronCount,
                     "Number of active neurons")
        .def_readonly("regionCount", &SimulationStats::regionCount,
                     "Number of neural regions")
        .def("toString", &SimulationStats::toString,
             "Get formatted statistics string");

    m.def("runSimulationWithStats",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int numSteps,
           bool enableLearning) {
            return runSimulationWithStats(brain, world, agent, numSteps, enableLearning);
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("numSteps"), py::arg("enableLearning") = true,
        "Run simulation and collect statistics");

    // Debug and diagnostic utilities
    m.def("printBrainStats", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== BRAIN STATISTICS ===" << std::endl;
        std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Pending Spikes: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Excit/Inhib Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Region Count: " << brain->getRegionCount() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        std::cout << "=========================" << std::endl;
    }, "Print detailed brain statistics to console");

    // Utility functions for common patterns
    m.def("createAgentWorldPair", 
        []() {
            auto [brain, world, agent] = createSimpleAgentWorld();
            return std::make_tuple(brain, world, agent);
        },
        "Create a brain, world, and agent pair (simple setup)");

    // Error handling utilities
    m.def("validateAgentSetup", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent) {
            if (!brain) throw py::value_error("Brain is null");
            if (!world) throw py::value_error("World is null");
            if (!agent) throw py::value_error("Agent is null");
            if (!brain->initialize()) throw py::runtime_error("Brain initialization failed");
            agent->initialize(world);
            return true;
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Validate and initialize an agent setup");

    // Additional convenience functions for common scenarios
    m.def("runExplorationSimulation",
        [](int worldWidth, int worldHeight, int steps, bool enableCuriosity = true) {
            auto [brain, world, agent] = createLearningAgent(worldWidth, worldHeight);
            agent->enableCuriosity(enableCuriosity);
            
            for (int i = 0; i < steps; ++i) {
                world->update(0.1f);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            return std::make_tuple(brain, world, agent);
        },
        py::arg("worldWidth") = 20,
        py::arg("worldHeight") = 20,
        py::arg("steps") = 100,
        py::arg("enableCuriosity") = true,
        "Run an exploration simulation with curiosity-driven behavior");

    m.def("createDemoEnvironment",
        []() {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(30, 30, 10, 10);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            
            return std::make_tuple(brain, world, agent);
        },
        "Create a demo environment with all features enabled");

    // Version and information
    m.def("getVersion", []() {
        return "pynlm 0.1.0 - NLM Python Bindings";
    }, "Get version information");

    m.def("getFeatures", []() {
        return std::vector<std::string>{
            "Brain simulation with LIF dynamics",
            "Integrated memory systems",
            "Neuromodulation (reward, curiosity, novelty)",
            "Sensorimotor integration",
            "Developmental learning",
            "Checkpoint save/load",
            "High-level simulation utilities",
            "Debug and diagnostic tools",
            "Statistics collection",
            "Progress tracking"
        };
    }, "Get list of available features");

    m.def("getDocumentationUrl", []() {
        return "https://github.com/nlm-project/nlm/blob/master/README.md";
    }, "Get documentation URL");

    // Example patterns for common use cases
    m.def("getExamplePatterns", []() {
        py::dict examples;
        
        // Example 1: Simple brain simulation
        examples["simple_brain"] = R"pbdoc(
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
)pbdoc";
        
        // Example 2: Complete agent with world
        examples["complete_agent"] = R"pbdoc(
import pynlm

# Setup complete agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
)pbdoc";
        
        // Example 3: Using simulation runner
        examples["simulation_runner"] = R"pbdoc(
import pynlm

# Create simulation runner
runner = pynlm.createSimulationRunner(width=30, height=30)

# Run with progress tracking
def progress_callback(progress):
    if progress % 0.1 < 0.01:  # Every 10%
        print(f"Progress: {progress * 100:.0f}%")

runner.runWithProgress(200, progressCallback=progress_callback)

# Get stats
brain = runner.getBrain()
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
)pbdoc";
        
        return examples;
    }, "Get example patterns for common use cases");
} // PYBIND11_MODULE

} // namespace nlm