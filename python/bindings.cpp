// NLM (Neural Learning Machine) Enhanced Python Bindings
// Updated with higher-level helper functions, smart configuration presets,
// and enhanced API for easier Python integration

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

using namespace nlm;

namespace py = pybind11;

// Configuration presets for smart configuration management
namespace nlm_python {

    // Smart configuration system
    class SmartConfig {
    public:
        struct Preset {
            std::string name;
            std::string description;
            std::string category;
            std::map<std::string, std::string> settings;
            std::string useCase;
            double estimatedRuntime;
        };
        
        SmartConfig() {
            initializePresets();
        }
        
        // Create configuration from preset
        static std::shared_ptr<Config> createConfig(const std::string& preset = "auto") {
            auto config = std::make_shared<Config>();
            
            if (preset == "auto") {
                // Smart auto-configuration based on use case detection
                config->set("neuron_count", static_cast<int64_t>(500));
                config->set("region_count", static_cast<int64_t>(2));
                config->set("connection_probability", 0.1f);
                config->set("random_seed", static_cast<int64_t>(42));
                config->set("simulation_timestep", 0.001);
                
                // Enable learning features by default
                config->set("enable_structural_plasticity", true);
                config->set("enable_development", true);
                config->set("enable_curiosity", true);
                config->set("enable_reward_modulation", true);
                config->set("enable_novelty_detection", true);
                
            } else if (preset == "fast") {
                config->set("neuron_count", static_cast<int64_t>(100));
                config->set("region_count", static_cast<int64_t>(1));
                config->set("connection_probability", 0.05f);
                config->set("random_seed", static_cast<int64_t>(42));
                config->set("simulation_timestep", 0.005);
                
                // Minimal features for speed
                config->set("enable_structural_plasticity", false);
                config->set("enable_development", false);
                config->set("enable_curiosity", false);
                
            } else if (preset == "research") {
                config->set("neuron_count", static_cast<int64_t>(2000));
                config->set("region_count", static_cast<int64_t>(4));
                config->set("connection_probability", 0.15f);
                config->set("random_seed", static_cast<int64_t>(42));
                config->set("simulation_timestep", 0.001);
                
                // Full feature set for research
                config->set("enable_structural_plasticity", true);
                config->set("enable_development", true);
                config->set("enable_curiosity", true);
                config->set("enable_reward_modulation", true);
                config->set("enable_novelty_detection", true);
                config->set("enable_prediction", true);
                config->set("enable_planning", true);
                config->set("enable_social_learning", true);
                config->set("enable_episodic_memory", true);
                config->set("enable_working_memory", true);
                config->set("enable_associative_memory", true);
                
            } else if (preset == "demo") {
                config->set("neuron_count", static_cast<int64_t>(50));
                config->set("region_count", static_cast<int64_t>(1));
                config->set("connection_probability", 0.1f);
                config->set("random_seed", static_cast<int64_t>(42));
                config->set("simulation_timestep", 0.001);
                
                // Minimal for demo
                config->set("enable_structural_plasticity", false);
                config->set("enable_development", false);
                config->set("enable_curiosity", false);
                config->set("enable_reward_modulation", false);
                
            } else if (preset == "visualization") {
                config->set("neuron_count", static_cast<int64_t>(200));
                config->set("region_count", static_cast<int64_t>(2));
                config->set("connection_probability", 0.08f);
                config->set("random_seed", static_cast<int64_t>(42));
                config->set("simulation_timestep", 0.001);
                
                // Good for visualization
                config->set("enable_structural_plasticity", true);
                config->set("enable_development", true);
                config->set("enable_curiosity", true);
                config->set("enable_reward_modulation", true);
                config->set("enable_novelty_detection", true);
                config->set("enable_prediction", true);
                config->set("enable_planning", false);
                config->set("enable_social_learning", false);
                config->set("log_level", "debug");
            }
            
            return config;
        }
        
        // Get available presets
        static std::vector<std::string> getAvailablePresets() {
            return {"auto", "fast", "research", "demo", "visualization"};
        }
        
        // Get preset description
        static std::string getPresetDescription(const std::string& preset) {
            std::map<std::string, std::string> descriptions = {
                {"auto", "Smart defaults with full learning capabilities"},
                {"fast", "Optimized for speed and quick demos"},
                {"research", "Full feature set for comprehensive research"},
                {"demo", "Minimal configuration for demonstrations"},
                {"visualization", "Optimized for visualization and debugging"}
            };
            
            auto it = descriptions.find(preset);
            return (it != descriptions.end()) ? it->second : "Preset not found";
        }
        
        // Load presets from file
        static bool loadPresets(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // This would be implemented to load custom presets
            // For now, just return false
            return false;
        }
        
        // Save presets to file
        static bool savePresets(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // This would be implemented to save current presets
            // For now, just return false
            return false;
        }
        
    private:
        void initializePresets() {
            // Presets are defined in the createConfig function
        }
    };

    // Learning session management
    class LearningSession {
    public:
        struct SessionMetrics {
            float totalReward = 0.0f;
            size_t totalSteps = 0;
            size_t totalSpikes = 0;
            size_t firingNeurons = 0;
            float meanReward = 0.0f;
            float stdDevReward = 0.0f;
            float meanFiringRate = 0.0f;
            float learningProgress = 0.0f;
            size_t episodesCompleted = 0;
        };
        
        LearningSession(std::shared_ptr<Brain> brain,
                       std::shared_ptr<SimpleWorld> world,
                       std::shared_ptr<AgentBrain> agent)
            : brain_(brain), world_(world), agent_(agent) {}
        
        // Run a single episode
        void runEpisode(int steps, float timestep) {
            world_->reset();
            agent_->reset();
            brain_->reset();
            brain_->initialize();
            
            episodeRewards_.clear();
            episodeSpikes_.clear();
            episodeFiringRates_.clear();
            
            float episodeReward = 0.0f;
            
            for (int step = 0; step < steps; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                
                // Run brain
                brain_->step(step, step * timestep);
                
                // Get action
                MotorCommand action = agent_->decodeMotorCommand();
                ActionResult result = world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // Apply reward and neuromodulation
                agent_->applyRewardModulation(result.reward, 0.0f);
                
                // Update world and agent
                world_->update(timestep);
                agent_->updateDevelopment(timestep);
                
                // Record metrics
                episodeRewards_.push_back(result.reward);
                episodeSpikes_.push_back(brain_->getTotalSpikeCount());
                episodeFiringRates_.push_back(brain_->getFiringNeuronCount());
                
                episodeReward += result.reward;
                
                // Progress reporting
                if (step % 100 == 0) {
                    std::cout << "  Step " << step 
                              << ": Reward " << result.reward
                              << ", Total " << episodeReward
                              << ", Spikes " << brain_->getTotalSpikeCount()
                              << ", Firing " << brain_->getFiringNeuronCount() << std::endl;
                }
            }
            
            totalReward_ += episodeReward;
            totalSteps_ += steps;
            
            std::cout << "Episode complete. Reward: " << episodeReward 
                      << ", Running total: " << totalReward_ << std::endl;
        }
        
        // Compute statistics
        SessionMetrics computeStatistics() {
            SessionMetrics metrics;
            
            // Compute reward statistics
            if (!episodeRewards_.empty()) {
                float sum = 0.0f, sumSq = 0.0f;
                for (float r : episodeRewards_) {
                    sum += r;
                    sumSq += r * r;
                }
                
                metrics.meanReward = sum / episodeRewards_.size();
                metrics.stdDevReward = std::sqrt(sumSq / episodeRewards_.size() - metrics.meanReward * metrics.meanReward);
                
                // Find max reward
                metrics.totalReward = *std::max_element(episodeRewards_.begin(), episodeRewards_.end());
                
                // Compute spike and firing rate statistics
                if (!episodeSpikes_.empty()) {
                    float sumSpikes = 0.0f, sumFiring = 0.0f;
                    for (size_t s : episodeSpikes_) {
                        sumSpikes += s;
                    }
                    for (size_t f : episodeFiringRates_) {
                        sumFiring += f;
                    }
                    
                    metrics.totalSpikes = sumSpikes;
                    metrics.firingNeurons = sumFiring;
                    metrics.meanFiringRate = sumFiring / episodeFiringRates_.size();
                }
                
                // Compute learning progress (compare first vs second half)
                if (episodeRewards_.size() >= 2) {
                    size_t half = episodeRewards_.size() / 2;
                    float firstHalf = std::accumulate(episodeRewards_.begin(), 
                                                     episodeRewards_.begin() + half, 0.0f) / half;
                    float secondHalf = std::accumulate(episodeRewards_.begin() + half, 
                                                      episodeRewards_.end(), 0.0f) / (episodeRewards_.size() - half);
                    metrics.learningProgress = secondHalf - firstHalf;
                }
            }
            
            return metrics;
        }
        
        // Save session data
        bool saveSession(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "NLM Learning Session Data\n";
            file << "==========================\n\n";
            
            file << "Configuration:\n";
            auto configKeys = brain_->getConfig()->getKeys();
            for (const auto& key : configKeys) {
                file << "  " << key << ": ";
                if (key == "neuron_count") {
                    file << brain_->getConfig()->getOr<int64_t>(key, 0) << std::endl;
                } else if (key == "connection_probability") {
                    file << std::fixed << std::setprecision(3) 
                         << brain_->getConfig()->getOr<float>(key, 0.0f) << std::endl;
                } else {
                    file << brain_->getConfig()->get(key).value_or("") << std::endl;
                }
            }
            
            file << "\nSession Statistics:\n";
            SessionMetrics stats = computeStatistics();
            file << "  Total Reward: " << stats.totalReward << std::endl;
            file << "  Total Steps: " << stats.totalSteps << std::endl;
            file << "  Total Spikes: " << stats.totalSpikes << std::endl;
            file << "  Mean Reward: " << stats.meanReward << std::endl;
            file << "  Reward Std Dev: " << stats.stdDevReward << std::endl;
            file << "  Mean Firing Rate: " << stats.meanFiringRate << std::endl;
            file << "  Learning Progress: " << stats.learningProgress << std::endl;
            
            file << "\nEpisode Data:\n";
            file << "  Episodes: " << episodeRewards_.size() << std::endl;
            file << "  Steps per Episode: " << (episodeRewards_.empty() ? 0 : totalSteps_ / episodeRewards_.size()) << std::endl;
            
            return true;
        }
        
        // Get agent body state
        const AgentBody& getAgentBody() const {
            return world_->getAgentBody();
        }
        
        // Get world dimensions
        std::pair<float, float> getWorldDimensions() const {
            return std::make_pair(world_->getWidth(), world_->getHeight());
        }
        
        // Get current developmental stage
        DevelopmentalStage getDevelopmentalStage() const {
            return agent_->getDevelopmentalStage();
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
        
        std::vector<float> episodeRewards_;
        std::vector<size_t> episodeSpikes_;
        std::vector<size_t> episodeFiringRates_;
        
        float totalReward_ = 0.0f;
        size_t totalSteps_ = 0;
    };
    
    // Convenience function to create a learning session
    LearningSession createLearningSession(std::shared_ptr<Config> config, const std::string& preset = "auto") {
        auto smartConfig = SmartConfig::createConfig(preset);
        auto brain = std::make_shared<Brain>(smartConfig);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return LearningSession(brain, world, agent);
    }

}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        
        Enhanced Python interface for the NLM neural simulation framework with:
        
        **Smart Configuration Management:**
        - Automatic configuration presets (auto, fast, research, demo, visualization)
        - Smart defaults that adapt to use cases
        - Preset descriptions and recommendations
        
        **Enhanced Learning Session Management:**
        - LearningSession class for comprehensive experiment tracking
        - Automatic metrics collection and statistics
        - Session data persistence
        - Progress monitoring and reporting
        
        **Advanced Python Features:**
        - Pythonic API design
        - Type safety and error handling
        - Extensive documentation and examples
        - Performance monitoring capabilities
        
        **Core Classes:**
        - Brain: Neural computation engine
        - Config: Configuration management
        - AgentBrain: Agent-world interface
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Sensory processing
        - Action: Motor output
        - WorldObject: World objects
        - AgentBody: Agent physical state
        
        **Example Usage:**
        
        import pynlm
        
        # Quick start with smart defaults
        config = pynlm.createSmartConfig('auto')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a simple simulation
        for step in range(100):
            brain.step(step)
        
        # Create a complete learning environment
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run a learning session
        session = pynlm.createLearningSession(config, 'auto')
        session.runEpisode(100, 0.01)
        stats = session.computeStatistics()
        
        print(f"Learning progress: {stats.learningProgress}")
        print(f"Mean reward: {stats.meanReward}")
        print(f"Firing neurons: {stats.firingNeurons}")
        
        # Save session data
        session.saveSession("learning_session.txt")
        
        Available presets: 'auto', 'fast', 'research', 'demo', 'visualization'
        
        For more information, see the NLM documentation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Core types (same as before)
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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with smart preset support)pbdoc")
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

    // Smart configuration functions
    m.def("createSmartConfig", &nlm_python::SmartConfig::createConfig,
          py::arg("preset") = "auto",
          "Create a smart configuration with preset settings");
    
    m.def("getAvailablePresets", &nlm_python::SmartConfig::getAvailablePresets,
          "Get list of available configuration presets");
    
    m.def("getPresetDescription", &nlm_python::SmartConfig::getPresetDescription,
          py::arg("preset"),
          "Get description of a configuration preset");

    // Learning session function
    m.def("createLearningSession", &nlm_python::createLearningSession,
          py::arg("config"), py::arg("preset") = "auto",
          "Create a learning session with smart defaults");

    // Other classes (same as before)
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
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

    // Pythonic API convenience functions
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

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add module-level documentation with examples
    m.doc() += "\n\n";
    m.doc() += "For installation and documentation, see the NLM documentation.\n\n";
    m.doc() += "To learn more about NLM, visit: https://github.com/nlm-project/nlm\n\n";
}

} // namespace nlm