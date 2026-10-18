#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>

// Include enhanced NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralAttention.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/cognition/SelfModel.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations
class EnhancedBrain;
class EnhancedConfig;
class EnhancedAgentBrain;
class EnhancedSimpleWorld;

// Exception classes for Python bindings
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error("NLM Error: " + message) {}
};

class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message) : NLMError("Simulation Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message) : NLMError("Memory Error: " + message) {}
};

// Context manager for simulation management
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<Brain> brain, double timestep = 0.001)
        : brain_(brain), timestep_(timestep) {
        if (!brain_) {
            throw SimulationError("Brain cannot be null");
        }
    }
    
    ~SimulationContext() {
        // Ensure brain is reset on destruction
        brain_->reset();
    }
    
    // Run simulation with automatic reset
    double run(double duration, std::function<bool(double)> callback = nullptr) {
        double elapsed = 0.0;
        brain_->reset();
        brain_->initialize();
        
        for (double time = 0.0; time < duration; time += timestep_) {
            brain_->step(static_cast<SimulationStep>(time / timestep_), time);
            
            if (callback) {
                if (callback(time)) {
                    break;
                }
            }
            
            elapsed = time;
        }
        
        return elapsed;
    }
    
    // Run multiple steps with error handling
    std::vector<double> runSteps(size_t steps, std::function<bool(SimulationStep)> callback = nullptr) {
        std::vector<double> times;
        brain_->reset();
        brain_->initialize();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            double time = step * timestep_;
            try {
                brain_->step(step, time);
                times.push_back(time);
                
                if (callback && !callback(step)) {
                    break;
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Simulation step ") + std::to_string(step) + 
                            std::string(" failed: ") + e.what());
                // Continue with next step
                continue;
            }
        }
        
        return times;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    double getTimestep() const { return timestep_; }
    
private:
    std::shared_ptr<Brain> brain_;
    double timestep_;
};

// Enhanced configuration with validation
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    EnhancedConfig(const std::string& filepath) : config_(std::make_shared<Config>()) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
    }
    
    // Template-based configuration access
    template<typename T>
    EnhancedConfig& set(const std::string& key, T value) {
        if constexpr (std::is_same_v<T, std::string>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, int>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, double>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, bool>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            config_->set(key, value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            config_->set(key, value);
        } else {
            throw ConfigurationError("Unsupported type for key: " + key);
        }
        return *this;
    }
    
    // Convenience methods
    EnhancedConfig& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    // Get configuration values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Chained configuration methods
    EnhancedConfig& brain(const std::string& neurons, const std::string& connections) {
        config_->set("brain.neuron_count", neurons);
        config_->set("brain.connection_probability", connections);
        return *this;
    }
    
    EnhancedConfig& plasticity(bool enable, double learningRate = 0.001) {
        config_->set("plasticity.enable", enable);
        config_->set("plasticity.learning_rate", learningRate);
        return *this;
    }
    
    EnhancedConfig& neuromodulation(bool enable, double scale = 1.0) {
        config_->set("neuromodulation.enable", enable);
        config_->set("neuromodulation.scale", scale);
        return *this;
    }
    
    EnhancedConfig& memory(size_t capacity, double decayRate = 0.01) {
        config_->set("memory.capacity", static_cast<int64_t>(capacity));
        config_->set("memory.decay_rate", decayRate);
        return *this;
    }
    
    // Export configuration
    std::string toJSON() const {
        std::ostringstream oss;
        oss << "{";
        for (const auto& key : config_->getKeys()) {
            oss << "\"" << key << "\": ";
            auto value = config_->get<std::string>(key);
            if (value) {
                oss << "\"" << *value << "\"";
            }
            oss << ", ";
        }
        oss << "}";
        return oss.str();
    }
    
    bool save(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool load(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    std::shared_ptr<Config> getInternalConfig() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced AgentBrain with advanced features
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}
    
    // Context manager for agent operations
    class Episode {
    public:
        Episode(EnhancedAgentBrain& agent, std::shared_ptr<SimpleWorld> world)
            : agent_(agent), world_(world), total_reward_(0.0) {
            if (!world_) {
                throw SimulationError("World cannot be null");
            }
            world_->reset();
            agent_.initialize(*world_);
        }
        
        ~Episode() {
            agent_.reset();
        }
        
        // Run episode with comprehensive tracking
        std::vector<double> runEpisode(double duration = 10.0) {
            std::vector<double> rewards;
            double elapsed = 0.0;
            
            for (double time = 0.0; time < duration; time += 0.1) {
                // Get sensory input
                auto percept = world_->getSensoryPercept();
                
                // Process sensory input
                agent_.processSensoryInput(percept);
                
                // Run brain step
                auto step = static_cast<SimulationStep>(time * 10.0);
                brain_->step(step, time);
                
                // Get motor command
                auto action = agent_.decodeMotorCommand();
                
                // Apply action to world
                world_->applyMotorCommand(action, time);
                
                // Get reward
                auto reward = world_->computeReward(brain_.get());
                total_reward_ += reward;
                rewards.push_back(reward);
                
                // Apply reward modulation
                agent_.applyRewardModulation(reward, 0.0);
                
                // Update development
                agent_.updateDevelopment(0.1);
                
                elapsed = time;
            }
            
            return rewards;
        }
        
        // Run episode with custom logic
        std::vector<double> runEpisodeCustom(std::function<double(double, const SensoryPercept&)> customLogic) {
            std::vector<double> rewards;
            brain_->reset();
            brain_->initialize();
            agent_.reset();
            
            double time = 0.0;
            auto percept = world_->getSensoryPercept();
            
            for (int step = 0; step < 1000; ++step) {
                double reward = customLogic(time, percept);
                
                agent_.processSensoryInput(percept);
                brain_->step(static_cast<SimulationStep>(step), time);
                auto action = agent_.decodeMotorCommand();
                world_->applyMotorCommand(action, time);
                
                total_reward_ += reward;
                rewards.push_back(reward);
                
                time += 0.01;
                if (step % 10 == 0) {
                    percept = world_->getSensoryPercept();
                }
            }
            
            return rewards;
        }
        
        double getTotalReward() const { return total_reward_; }
        
        std::shared_ptr<Brain> getBrain() const { return agent_->getBrain(); }
        std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
        
        // Enable/disable subsystems
        void enableRewardModulation(bool enable) { agent_->enableRewardModulation(enable); }
        void enableStructuralPlasticity(bool enable) { agent_->enableStructuralPlasticity(enable); }
        void enableDevelopment(bool enable) { agent_->enableDevelopment(enable); }
        void enableCuriosity(bool enable) { agent_->enableCuriosity(enable); }
        
        // Get subsystem states
        bool isRewardModulationEnabled() const { return agent_->isRewardModulationEnabled(); }
        bool isStructuralPlasticityEnabled() const { return agent_->isStructuralPlasticityEnabled(); }
        bool isDevelopmentEnabled() const { return agent_->getBrain()->getDevelopmentalStage() != DevelopmentalStage::Initial; }
        
        // Get statistics
        std::map<std::string, double> getStatistics() const {
            std::map<std::string, double> stats;
            auto brain = getBrain();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain->getAverageFiringRate();
            stats["curiosity"] = agent_->getCuriosityLevel();
            stats["novelty"] = agent_->getNoveltyLevel();
            stats["prediction_error"] = agent_->getPredictionError();
            stats["development_stage"] = static_cast<double>(brain->getDevelopmentalStage());
            stats["total_reward"] = total_reward_;
            return stats;
        }
        
    private:
        EnhancedAgentBrain& agent_;
        std::shared_ptr<SimpleWorld> world_;
        double total_reward_;
        
        friend class EnhancedAgentBrain;
    };
    
    // Create episode
    Episode createEpisode(std::shared_ptr<SimpleWorld> world) {
        return Episode(*this, world);
    }
    
    // Run simulation with comprehensive tracking
    std::vector<std::map<std::string, double>> runSimulation(
        std::shared_ptr<SimpleWorld> world,
        double duration,
        std::function<void(double, const Episode&)> callback = nullptr) {
        
        std::vector<std::map<std::string, double>> episode_stats;
        auto episode = createEpisode(world);
        
        double start_time = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        auto rewards = episode.runEpisode(duration);
        
        double end_time = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        auto stats = episode.getStatistics();
        stats["duration"] = end_time - start_time;
        stats["reward_count"] = rewards.size();
        stats["total_reward"] = episode.getTotalReward();
        
        if (callback) {
            callback(end_time, episode);
        }
        
        episode_stats.push_back(stats);
        return episode_stats;
    }
    
    std::shared_ptr<Brain> getBrain() const { return agent_->getBrain(); }
    std::shared_ptr<AgentBrain> getInternalAgent() const { return agent_; }
    
    // Enhanced agent methods
    void enableAllSubsystems() {
        agent_->enableRewardModulation(true);
        agent_->enableStructuralPlasticity(true);
        agent_->enableDevelopment(true);
        agent_->enableCuriosity(true);
    }
    
    void disableAllSubsystems() {
        agent_->enableRewardModulation(false);
        agent_->enableStructuralPlasticity(false);
        agent_->enableDevelopment(false);
        agent_->enableCuriosity(false);
    }
    
    std::string getStatusReport() const {
        std::ostringstream oss;
        oss << "=== Agent Brain Status Report ===\n";
        oss << "Brain neurons: " << getBrain()->getTotalNeuronCount() << "\n";
        oss << "Brain synapses: " << getBrain()->getTotalSynapseCount() << "\n";
        oss << "Firing neurons: " << getBrain()->getFiringNeuronCount() << "\n";
        oss << "Total spikes: " << getBrain()->getTotalSpikeCount() << "\n";
        oss << "Average firing rate: " << getBrain()->getAverageFiringRate() << "\n";
        oss << "Reward modulation: " << (agent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        oss << "Structural plasticity: " << (agent_->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << "\n";
        oss << "Development enabled: " << (getBrain()->getDevelopmentalStage() != DevelopmentalStage::Initial ? "Yes" : "No") << "\n";
        oss << "Curiosity level: " << agent_->getCuriosityLevel() << "\n";
        oss << "Novelty level: " << agent_->getNoveltyLevel() << "\n";
        oss << "Prediction error: " << agent_->getPredictionError() << "\n";
        oss << "Developmental stage: " << static_cast<int>(getBrain()->getDevelopmentalStage()) << "\n";
        return oss.str();
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};

// Enhanced World with advanced features
class EnhancedSimpleWorld {
public:
    EnhancedSimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Context manager for world management
    class WorldSession {
    public:
        WorldSession(EnhancedSimpleWorld& world, float width = 20.0f, float height = 20.0f,
                   float visionWidth = 8.0f, float visionHeight = 8.0f)
            : world_(world), width_(width), height_(height) {
            world_.world_->configure(width, height, visionWidth, visionHeight);
            world_.world_->reset();
        }
        
        ~WorldSession() {
            // Cleanup handled by EnhancedSimpleWorld
        }
        
        // Run world simulation
        std::vector<std::map<std::string, double>> runSimulation(double duration) {
            std::vector<std::map<std::string, double>> states;
            double timestep = 0.1;
            double time = 0.0;
            
            for (int step = 0; step < static_cast<int>(duration / timestep); ++step) {
                world_.world_->update(timestep);
                
                auto state = collectState(time);
                states.push_back(state);
                
                time += timestep;
            }
            
            return states;
        }
        
        // Run world with custom objects
        std::vector<std::map<std::string, double>> runSimulationWithObjects(
            const std::vector<WorldObject>& objects,
            double duration) {
            
            // Add objects to world
            for (const auto& obj : objects) {
                world_.world_->addObject(obj);
            }
            
            std::vector<std::map<std::string, double>> states;
            double timestep = 0.1;
            double time = 0.0;
            
            for (int step = 0; step < static_cast<int>(duration / timestep); ++step) {
                world_.world_->update(timestep);
                
                auto state = collectState(time);
                state["object_count"] = objects.size();
                state["total_value"] = calculateTotalValue(objects);
                states.push_back(state);
                
                time += timestep;
            }
            
            // Cleanup objects
            for (const auto& obj : objects) {
                world_.world_->removeObject(obj.x, obj.y);
            }
            
            return states;
        }
        
        std::map<std::string, double> collectState(double time) const {
            std::map<std::string, double> state;
            state["time"] = time;
            state["width"] = world_.world_->getWidth();
            state["height"] = world_.world_->getHeight();
            state["simulation_time"] = world_.world_->getSimulationTime();
            state["max_energy"] = world_.world_->getMaxEnergy();
            
            auto body = world_.world_->getAgentBody();
            state["agent_x"] = body.x;
            state["agent_y"] = body.y;
            state["agent_orientation"] = body.orientation;
            state["agent_energy"] = body.energy;
            state["agent_health"] = body.health;
            state["agent_age"] = body.age;
            
            return state;
        }
        
        double calculateTotalValue(const std::vector<WorldObject>& objects) const {
            double total = 0.0;
            for (const auto& obj : objects) {
                total += obj.value;
            }
            return total;
        }
        
    private:
        EnhancedSimpleWorld& world_;
        float width_;
        float height_;
        
        friend class EnhancedSimpleWorld;
    };
    
    WorldSession createSession(float width = 20.0f, float height = 20.0f,
                               float visionWidth = 8.0f, float visionHeight = 8.0f) {
        return WorldSession(*this, width, height, visionWidth, visionHeight);
    }
    
    std::shared_ptr<SimpleWorld> getInternalWorld() const { return world_; }
    
    // Enhanced world methods
    void configure(float width, float height, float visionWidth = 8.0f, float visionHeight = 8.0f) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    std::string getStatusReport() const {
        std::ostringstream oss;
        oss << "=== World Status Report ===\n";
        oss << "Width: " << world_->getWidth() << "\n";
        oss << "Height: " << world_->getHeight() << "\n";
        oss << "Vision: " << world_->getVisionWidth() << "x" << world_->getVisionHeight() << "\n";
        oss << "Max energy: " << world_->getMaxEnergy() << "\n";
        oss << "Energy decay rate: " << world_->getEnergyDecayRate() << "\n";
        oss << "Simulation time: " << world_->getSimulationTime() << "\n";
        oss << "Random seed: " << world_->getRandomSeed() << "\n";
        return oss.str();
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Robust error handling with comprehensive exception management
        - Enhanced configuration support with JSON/YAML format detection
        - Context managers for simulation and episode management
        - Advanced statistics and monitoring
        - Performance optimization and error recovery
        - Enhanced Python API with better documentation and examples
        
        Features:
        - SimulationContext: Context manager for simulation control
        - EnhancedConfig: Advanced configuration management with validation
        - EnhancedAgentBrain: Agent brain with advanced features
        - EnhancedSimpleWorld: World simulation with enhanced management
        - Episode: Context manager for agent episodes
        - WorldSession: Context manager for world sessions
        
        Example usage:
        ```python
        import pynlm
        
        # Create enhanced configuration
        config = pynlm.EnhancedConfig()\
            .set("brain.neuron_count", 1000)\
            .set("plasticity.enable", True)\
            .set("neuromodulation.curiosity.enable", True)
        
        # Create brain and agent
        brain = pynlm.Brain(config)
        agent = pynlm.EnhancedAgentBrain(brain)
        
        # Create world and run episode
        world = pynlm.EnhancedSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.setAgentStart(10.0, 10.0)
        
        # Run episode using context manager
        with agent.createEpisode(world) as episode:
            rewards = episode.runEpisode(10.0)
            print(f"Total reward: {episode.getTotalReward()}")
        
        # Run simulation with enhanced tracking
        stats = agent.runSimulation(world, 60.0)
        print(f"Episode statistics: {stats}")
        
        # Get status report
        print(agent.getStatusReport())
        ```
    )pbdoc";
    
    // Register exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    
    // Register basic types
    py::class_<NeuronId>(m, "NeuronId")
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
    
    // Register enhanced configuration
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(
        Enhanced configuration manager with validation and format detection.
        Supports JSON, YAML, and simple key=value formats with automatic error recovery.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        .def("set", &EnhancedConfig::set)
        .def("get", &EnhancedConfig::get)
        .def("getOr", &EnhancedConfig::getOr)
        .def("brain", &EnhancedConfig::brain)
        .def("plasticity", &EnhancedConfig::plasticity)
        .def("neuromodulation", &EnhancedConfig::neuromodulation)
        .def("memory", &EnhancedConfig::memory)
        .def("toJSON", &EnhancedConfig::toJSON)
        .def("save", &EnhancedConfig::save)
        .def("load", &EnhancedConfig::load);
    
    // Register enhanced agent brain
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(
        Enhanced agent brain interface with advanced features.
        Includes context managers for episode and simulation control.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("createEpisode", &EnhancedAgentBrain::createEpisode)
        .def("runSimulation", &EnhancedAgentBrain::runSimulation)
        .def("enableAllSubsystems", &EnhancedAgentBrain::enableAllSubsystems)
        .def("disableAllSubsystems", &EnhancedAgentBrain::disableAllSubsystems)
        .def("getStatusReport", &EnhancedAgentBrain::getStatusReport);
    
    // Register enhanced world
    py::class_<EnhancedSimpleWorld>(m, "EnhancedSimpleWorld", R"pbdoc(
        Enhanced world simulation with advanced management features.
        Includes context managers for world sessions.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &EnhancedSimpleWorld::configure)
        .def("createSession", &EnhancedSimpleWorld::createSession)
        .def("setAgentStart", &EnhancedSimpleWorld::setAgentStart)
        .def("getStatusReport", &EnhancedSimpleWorld::getStatusReport);
    
    // Register simulation context
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(
        Context manager for simulation control with automatic cleanup.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, double>(), py::arg("brain"), py::arg("timestep") = 0.001)
        .def("run", &SimulationContext::run)
        .def("runSteps", &SimulationContext::runSteps)
        .def("getBrain", &SimulationContext::getBrain)
        .def("getTimestep", &SimulationContext::getTimestep);
    
    // Register enhanced types
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Register basic classes
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>())
        .def("getType", &Action::getType)
        .def("setType", &Action::setType)
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters)
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &AgentBrain::updateDevelopment)
        .def("reset", &AgentBrain::reset);
    
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart)
        .def("update", &SimpleWorld::update)
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &SimpleWorld::getAgentBody)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight);
    
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>())
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);
    
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch)
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal)
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio);
    
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput)
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate);
    
    // Factory functions
    m.def("createEnhancedConfig", []() { return EnhancedConfig(); },
          "Create enhanced configuration with defaults");
    
    m.def("createEnhancedConfigFromFile", [](const std::string& filepath) { 
        return EnhancedConfig(filepath); 
    }, "Create enhanced configuration from file", py::arg("filepath"));
    
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, "Create enhanced brain with configuration");
    
    m.def("createEnhancedAgent", [](std::shared_ptr<Brain> brain) {
        return EnhancedAgentBrain(brain);
    }, "Create enhanced agent brain");
    
    m.def("createEnhancedWorld", []() {
        return EnhancedSimpleWorld();
    }, "Create enhanced world");
    
    // Utility functions
    m.def("runSimulationWithContext", [](EnhancedAgentBrain& agent, EnhancedSimpleWorld& world, double duration) {
        return agent.runSimulation(world, duration);
    }, "Run simulation with enhanced tracking", py::arg("agent"), py::arg("world"), py::arg("duration"));
    
    m.def("createEpisodeWithContext", [](EnhancedAgentBrain& agent, EnhancedSimpleWorld& world) {
        return agent.createEpisode(world);
    }, "Create episode with enhanced context", py::arg("agent"), py::arg("world"));
    
    // Version information
    m.attr("VERSION") = "3.0.0";
    m.attr("BUILD_DATE") = __DATE__ " " __TIME__;
    m.attr("CXX_STANDARD") = "C++20";
    
    // Documentation strings
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        
        Version 3.0.0 - Enhanced with robust error handling, 
        comprehensive configuration management, and advanced Python features.
        
        Key improvements:
        - Enhanced error handling with comprehensive exception management
        - Multi-format configuration support (JSON, YAML, simple key=value)
        - Context managers for automatic resource management
        - Advanced statistics and monitoring
        - Performance optimization and error recovery
        - Enhanced Python API with comprehensive documentation
        
        Installation:
        pip install pynlm
        
        Quick start:
        import pynlm
        
        config = pynlm.createEnhancedConfig()\
            .set("brain.neuron_count", 1000)\
            .set("plasticity.enable", True)
        
        brain = pynlm.createEnhancedBrain(config)
        agent = pynlm.createEnhancedAgent(brain)
        world = pynlm.createEnhancedWorld()
        
        with agent.createEpisode(world) as episode:
            rewards = episode.runEpisode(10.0)
            print(f"Total reward: {episode.getTotalReward()}")
        
        See ENHANCED_DOCUMENTATION.md for comprehensive documentation.
    )pbdoc";
    
    // Add examples
    m.def("example_basic_usage", []() {
        return R"pbdoc(
            Basic usage example:
            
            import pynlm
            
            # Create enhanced configuration
            config = pynlm.createEnhancedConfig()\
                .set("brain.neuron_count", 1000)\
                .set("plasticity.enable", True)\
                .set("neuromodulation.curiosity.enable", True)
            
            # Create brain and agent
            brain = pynlm.createEnhancedBrain(config)
            agent = pynlm.createEnhancedAgent(brain)
            
            # Create world
            world = pynlm.createEnhancedWorld()
            world.configure(20, 20, 8, 8)
            world.setAgentStart(10.0, 10.0)
            
            # Run episode
            with agent.createEpisode(world) as episode:
                rewards = episode.runEpisode(10.0)
                print(f"Total reward: {episode.getTotalReward()}")
                print(f"Statistics: {episode.getStatistics()}")
            
            # Show status report
            print(agent.getStatusReport())
        )pbdoc";
    });
    
    m.def("example_advanced_usage", []() {
        return R"pbdoc(
            Advanced usage example:
            
            import pynlm
            
            # Create configuration with all features enabled
            config = pynlm.createEnhancedConfig()\
                .brain("2000", "0.05")\
                .plasticity(True, 0.001)\
                .neuromodulation(True, 1.0)\
                .memory(5000, 0.01)
            
            # Create brain and run complex simulation
            brain = pynlm.createEnhancedBrain(config)
            agent = pynlm.createEnhancedAgent(brain)
            world = pynlm.createEnhancedWorld()
            
            # Enable all subsystems
            agent.enableAllSubsystems()
            
            # Run long simulation with monitoring
            stats = agent.runSimulation(world, 300.0)
            
            # Analyze results
            for stat in stats:
                print(f"Episode stats: {stat}")
            
            # Create custom episode with advanced logic
            with agent.createEpisode(world) as episode:
                # Custom episode logic
                rewards = episode.runEpisodeCustom(lambda t, percept: 1.0 if t > 5.0 else 0.0)
                print(f"Custom episode reward: {episode.getTotalReward()}")
        )pbdoc";
    });
    
    m.def("example_configuration_examples", []() {
        return R"pbdoc(
            Configuration examples:
            
            # JSON configuration
            {
                "brain": {
                    "neuron_count": 1000,
                    "region_count": 1,
                    "connection_probability": 0.1
                },
                "plasticity": {
                    "stdp": {"enable": true, "learning_rate": 0.001},
                    "hebbian": {"enable": true, "learning_rate": 0.01},
                    "structural": {"enable": true, "synaptogenesis_rate": 0.0001}
                },
                "neuromodulation": {
                    "dopamine": {"scale": 1.0},
                    "curiosity": {"enable": true, "scale": 1.0},
                    "novelty": {"enable": true, "scale": 1.0}
                }
            }
            
            # YAML configuration
            brain:
              neuron_count: 1000
              region_count: 1
              connection_probability: 0.1
            
            plasticity:
              stdp:
                enable: true
                learning_rate: 0.001
              hebbian:
                enable: true
                learning_rate: 0.01
            
            neuromodulation:
              curiosity:
                enable: true
                scale: 1.0
        )pbdoc";
    });
    
    m.def("example_error_handling", []() {
        return R"pbdoc(
            Error handling examples:
            
            import pynlm
            
            try:
                config = pynlm.createEnhancedConfig("missing.json")
            except pynlm.ConfigurationError as e:
                print(f"Configuration error: {e}")
                # Create config with defaults
                config = pynlm.createEnhancedConfig()\
                    .set("brain.neuron_count", 1000)
            
            try:
                brain = pynlm.createEnhancedBrain(config)
                brain.initialize()
                brain.step(0)
            except pynlm.SimulationError as e:
                print(f"Simulation error: {e}")
                # Handle simulation error
            
            try:
                world = pynlm.createEnhancedWorld()
                world.configure(20, 20, 8, 8)
            except pynlm.NLMError as e:
                print(f"World configuration error: {e}")
        )pbdoc";
    });
}

} // namespace nlm