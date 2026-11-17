#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

/**
 * Enhanced Python API with advanced functionality
 */

// Helper functions for advanced operations
namespace pyapi {
    
    // Configuration utilities
    py::dict getConfigSummary(std::shared_ptr<Config> config) {
        py::dict summary;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            if (config->has(key)) {
                // Try to get value as string representation
                summary[key] = config->getOr<std::string>(key, "");
            }
        }
        
        return summary;
    }
    
    // Brain analysis functions
    py::dict analyzeBrainState(std::shared_ptr<Brain> brain) {
        py::dict analysis;
        
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        analysis["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        analysis["region_count"] = brain->getRegionCount();
        
        // Calculate additional metrics
        if (brain->getTotalNeuronCount() > 0) {
            float firingPercentage = (brain->getFiringNeuronCount() * 100.0f) / brain->getTotalNeuronCount();
            analysis["firing_percentage"] = firingPercentage;
            
            if (brain->getExcitationInhibitionRatio() > 0) {
                float logRatio = std::log10(brain->getExcitationInhibitionRatio());
                analysis["log_ei_ratio"] = logRatio;
            }
        }
        
        return analysis;
    }
    
    // Pattern generation for testing
    std::vector<float> generateSensoryPattern(const std::string& patternType, size_t size = 16) {
        std::vector<float> pattern(size, 0.0f);
        
        if (patternType == "random") {
            for (size_t i = 0; i < size; ++i) {
                pattern[i] = static_cast<float>(rand()) / RAND_MAX;
            }
        } else if (patternType == "empty") {
            std::fill(pattern.begin(), pattern.end(), 1.0f);
        } else if (patternType == "bright") {
            for (size_t i = 5; i <= 10 && i < size; ++i) {
                pattern[i] = 0.7f;
            }
            std::fill(pattern.begin(), pattern.end(), 1.0f);
        } else if (patternType == "dark") {
            for (size_t i = 5; i <= 10 && i < size; ++i) {
                pattern[i] = 0.25f;
            }
            std::fill(pattern.begin(), pattern.end(), 1.0f);
        } else if (patternType == "stripes") {
            for (size_t i = 0; i < size; ++i) {
                pattern[i] = ((i / 2) % 2 == 0) ? 0.8f : 0.2f;
            }
        }
        
        return pattern;
    }
    
    // Simulation utilities
    py::dict runSimulationEpisode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t numSteps,
        bool verbose = false
    ) {
        py::dict episodeData;
        std::vector<float> rewards;
        std::vector<float> novelties;
        std::vector<float> curiosities;
        
        if (agent) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        float totalReward = 0.0f;
        size_t stepsWithReward = 0;
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process in brain
            if (agent) {
                agent->processSensoryInput(percept);
                brain->step(step);
                
                // Decode action
                MotorCommand action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                float reward = percept.getInternal().empty() ? 0.0f : percept.getInternal()[0];
                if (std::abs(reward) > 0.01f) {
                    agent->applyRewardModulation(reward, 0.0f);
                    totalReward += reward;
                    stepsWithReward++;
                }
                
                // Update development
                agent->updateDevelopment(0.1);
                
                // Record metrics
                rewards.push_back(reward);
                if (agent->getCuriosityLevel() > 0) {
                    novelties.push_back(agent->getCuriosityLevel());
                    curiosities.push_back(agent->getNoveltyLevel());
                }
                
                if (verbose && step % 25 == 0) {
                    std::cout << "Step " << step << ": Reward=" << reward 
                              << " Total=" << totalReward
                              << " Novelty=" << agent->getNoveltyLevel()
                              << " Curiosity=" << agent->getCuriosityLevel() << std::endl;
                }
            } else {
                brain->step(step);
            }
        }
        
        // Calculate statistics
        episodeData["total_reward"] = totalReward;
        episodeData["steps_with_reward"] = stepsWithReward;
        episodeData["num_steps"] = numSteps;
        
        if (!rewards.empty()) {
            float avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
            episodeData["average_reward"] = avgReward;
            
            // Reward variance
            float variance = 0.0f;
            for (float r : rewards) {
                variance += (r - avgReward) * (r - avgReward);
            }
            variance /= rewards.size();
            episodeData["reward_variance"] = variance;
        }
        
        if (!novelties.empty()) {
            float avgNovelty = std::accumulate(novelties.begin(), novelties.end(), 0.0f) / novelties.size();
            episodeData["average_novelty"] = avgNovelty;
            
            float avgCuriosity = std::accumulate(curiosities.begin(), curiosities.end(), 0.0f) / curiosities.size();
            episodeData["average_curiosity"] = avgCuriosity;
        }
        
        return episodeData;
    }
    
    // Advanced configuration templates
    py::dict getAdvancedConfig(const std::string& configType) {
        py::dict config;
        
        if (configType == "cognitive") {
            // Enhanced cognitive processing
            config["neuron_count"] = 2000;
            config["region_count"] = 3;
            config["connection_probability"] = 0.15f;
            config["stdp_ltp_weight"] = 0.02f;
            config["stdp_ltd_weight"] = 0.015f;
            config["stdp_tau"] = 25.0f;
            config["synaptogenesis_rate"] = 0.0002f;
            config["pruning_rate"] = 0.00002f;
            config["enable_development"] = true;
            config["development_stage_start"] = 0.0;
            config["development_stage_end"] = 1000.0;
        } else if (configType == "social") {
            // Social learning focused
            config["neuron_count"] = 1500;
            config["region_count"] = 2;
            config["connection_probability"] = 0.12f;
            config["stdp_ltp_weight"] = 0.015f;
            config["stdp_ltd_weight"] = 0.012f;
            config["curiosity_enabled"] = true;
            config["novelty_enabled"] = true;
            config["social_learning"] = true;
            config["imitation_rate"] = 0.3f;
        } else if (configType == "creative") {
            // Creative exploration
            config["neuron_count"] = 3000;
            config["region_count"] = 4;
            config["connection_probability"] = 0.18f;
            config["stdp_ltp_weight"] = 0.025f;
            config["stdp_ltd_weight"] = 0.02f;
            config["concept_formation"] = true;
            config["planning_depth"] = 8;
            config["creative_exploration"] = true;
        } else if (configType == "balanced") {
            // Balanced all systems
            config["neuron_count"] = 1500;
            config["region_count"] = 3;
            config["connection_probability"] = 0.1f;
            config["stdp_ltp_weight"] = 0.018f;
            config["stdp_ltd_weight"] = 0.017f;
            config["enable_reward_modulation"] = true;
            config["enable_structural_plasticity"] = true;
            config["enable_development"] = true;
            config["enable_curiosity"] = true;
        }
        
        return config;
    }
    
    // Performance monitoring
    py::dict getPerformanceMetrics(std::shared_ptr<Brain> brain) {
        py::dict metrics;
        
        // Spike statistics
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        
        // Network properties
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["region_count"] = brain->getRegionCount();
        
        // Calculate derived metrics
        if (brain->getTotalNeuronCount() > 0) {
            metrics["spike_per_neuron"] = static_cast<float>(brain->getTotalSpikeCount()) / brain->getTotalNeuronCount();
            metrics["synapse_per_neuron"] = static_cast<float>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
        }
        
        // Developmental metrics
        metrics["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics;
    }
    
    // Memory management
    bool checkMemoryUsage(std::shared_ptr<Brain> brain) {
        // Simple memory check - in real implementation would use system-specific tools
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        // Conservative estimate: ~1KB per neuron, ~100 bytes per synapse
        size_t estimatedMemory = totalNeurons * 1024 + totalSynapses * 100;
        
        // Warn if memory usage exceeds 500MB
        if (estimatedMemory > 500 * 1024 * 1024) {
            std::cout << "[WARN] High memory usage estimated: " 
                      << (estimatedMemory / 1024 / 1024) << " MB" << std::endl;
            return false;
        }
        
        return true;
    }
    
}  // namespace pyapi

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        Enhanced Python API for the NLM C++ neural simulation framework.
        
        This enhanced API provides:
        - Advanced configuration management
        - Simulation utilities and monitoring
        - Pattern generation for testing
        - Performance analysis tools
        - Memory management utilities
        - Experiment management
        - Configuration templates
        
        Basic functionality remains unchanged for backward compatibility.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic classes (same as before for backward compatibility)
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

    // Basic classes (same as original bindings)
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Core simulation classes
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

    // Convenience functions
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced API functions
    m.def("get_config_summary", &pyapi::getConfigSummary,
          "Get summary of configuration parameters", py::arg("config"));

    m.def("analyze_brain_state", &pyapi::analyzeBrainState,
          "Analyze brain state and return metrics", py::arg("brain"));

    m.def("generate_sensory_pattern", &pyapi::generateSensoryPattern,
          "Generate sensory input pattern for testing", 
          py::arg("pattern_type"), py::arg("size") = 16);

    m.def("run_simulation_episode", &pyapi::runSimulationEpisode,
          "Run simulation episode with world interaction",
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("num_steps"), py::arg("verbose") = false);

    m.def("get_advanced_config", &pyapi::getAdvancedConfig,
          "Get advanced configuration template", py::arg("config_type"));

    m.def("get_performance_metrics", &pyapi::getPerformanceMetrics,
          "Get performance metrics from brain", py::arg("brain"));

    m.def("check_memory_usage", &pyapi::checkMemoryUsage,
          "Check estimated memory usage", py::arg("brain"));

    // Phase 6 integration experiment
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment",
        R"pbdoc(Phase 6 integration experiment runner)pbdoc")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"),
             "Run the complete integration test")
        .def("verify_integration", &Phase6IntegratedExperiment::verifyIntegration,
             "Run a simple integration verification")
        .def("test_memory_integration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory integration")
        .def("test_neuromodulation_integration", 
             &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation integration")
        .def("test_checkpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load")
        .def("test_replay", &Phase6IntegratedExperiment::testReplay,
             "Test replay system");

    // Configuration templates
    m.attr("COGNITIVE_CONFIG", py::dict({
        {"neuron_count", 2000},
        {"region_count", 3},
        {"connection_probability", 0.15f},
        {"stdp_ltp_weight", 0.02f},
        {"stdp_ltd_weight", 0.015f},
        {"stdp_tau", 25.0f},
        {"synaptogenesis_rate", 0.0002f},
        {"pruning_rate", 0.00002f},
        {"enable_development", true},
        {"development_stage_start", 0.0},
        {"development_stage_end", 1000.0}
    }));

    m.attr("SOCIAL_CONFIG", py::dict({
        {"neuron_count", 1500},
        {"region_count", 2},
        {"connection_probability", 0.12f},
        {"stdp_ltp_weight", 0.015f},
        {"stdp_ltd_weight", 0.012f},
        {"curiosity_enabled", true},
        {"novelty_enabled", true},
        {"social_learning", true},
        {"imitation_rate", 0.3f}
    }));

    m.attr("CREATIVE_CONFIG", py::dict({
        {"neuron_count", 3000},
        {"region_count", 4},
        {"connection_probability", 0.18f},
        {"stdp_ltp_weight", 0.025f},
        {"stdp_ltd_weight", 0.02f},
        {"concept_formation", true},
        {"planning_depth", 8},
        {"creative_exploration", true}
    }));

    m.attr("BALANCED_CONFIG", py::dict({
        {"neuron_count", 1500},
        {"region_count", 3},
        {"connection_probability", 0.1f},
        {"stdp_ltp_weight", 0.018f},
        {"stdp_ltd_weight", 0.017f},
        {"enable_reward_modulation", true},
        {"enable_structural_plasticity", true},
        {"enable_development", true},
        {"enable_curiosity", true}
    }));

    // Utility functions
    m.def("print_banner", &printBanner,
          "Print NLM banner with title", py::arg("title") = "NLM - Neural Learning Machine");

    m.def("get_timestamp", &getTimestamp,
          "Get current timestamp");

    m.def("format_float", &formatFloat,
          "Format float value", py::arg("value"), py::arg("precision") = 3);

    m.def("format_percentage", &formatPercentage,
          "Format value as percentage", py::arg("value"), py::arg("max") = 1.0f);
}

}  // namespace nlm