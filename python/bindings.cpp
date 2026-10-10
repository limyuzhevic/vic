#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python utilities and extensions

// Batch processing utility
class BatchProcessor {
public:
    struct BatchResult {
        std::vector<std::string> logs;
        double totalTime;
        int totalSteps;
        int totalSpikes;
        double avgFiringRate;
        std::map<std::string, double> metrics;
    };
    
    static BatchResult runBatch(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        int numSteps,
        int batchSize = 100
    ) {
        BatchResult result;
        double batchStartTime = py::time();
        int batchSteps = 0;
        int batchSpikes = 0;
        
        for (int step = 0; step < numSteps; step += batchSize) {
            int currentBatchSize = std::min(batchSize, numSteps - step);
            
            for (int i = 0; i < currentBatchSize; ++i) {
                int actualStep = step + i;
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain step
                brain->step(actualStep, actualStep * 0.001);
                
                // Get motor command
                auto action = agent->decodeMotorCommand();
                
                // Apply action to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Record metrics
                batchSteps++;
                batchSpikes += brain->getFiringNeuronCount();
                
                // Log periodically
                if ((actualStep + 1) % 1000 == 0) {
                    result.logs.push_back("Step " + std::to_string(actualStep + 1) + 
                                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
                }
            }
        }
        
        double batchEndTime = py::time();
        result.totalTime = batchEndTime - batchStartTime;
        result.totalSteps = batchSteps;
        result.totalSpikes = batchSpikes;
        result.avgFiringRate = (batchSpikes / batchSteps) * 1000.0; // Convert to Hz
        
        return result;
    }
};

// Advanced configuration utilities
class ConfigUtils {
public:
    static std::map<std::string, std::variant<int, double, bool, std::string>> 
        loadConfigFromFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filepath)) {
            auto keys = config->getKeys();
            std::map<std::string, std::variant<int, double, bool, std::string>> result;
            
            for (const auto& key : keys) {
                auto value = config->get<std::string>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            
            return result;
        }
        
        throw std::runtime_error("Failed to load configuration from: " + filepath);
    }
    
    static bool saveConfigToFile(const std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                                const std::string& filepath) {
        auto nlmConfig = std::make_shared<Config>();
        
        for (const auto& pair : config) {
            const std::string& key = pair.first;
            const auto& value = pair.second;
            
            if (std::holds_alternative<int>(value)) {
                nlmConfig->set(key, std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                nlmConfig->set(key, std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                nlmConfig->set(key, std::get<bool>(value));
            } else if (std::holds_alternative<std::string>(value)) {
                nlmConfig->set(key, std::get<std::string>(value));
            }
        }
        
        return nlmConfig->saveToFile(filepath);
    }
    
    static std::string generateConfigTemplate() {
        std::stringstream ss;
        ss << "# NLM Configuration Template\n";
        ss << "# Random seed for reproducible experiments\n";
        ss << "random_seed = 42\n";
        ss << "\n";
        ss << "# Simulation timestep in seconds\n";
        ss << "simulation_timestep = 0.001\n";
        ss << "\n";
        ss << "# Number of neurons in the brain\n";
        ss << "neuron_count = 1000\n";
        ss << "\n";
        ss << "# Number of brain regions\n";
        ss << "region_count = 2\n";
        ss << "\n";
        ss << "# Connection probability for random connectivity\n";
        ss << "connection_probability = 0.1\n";
        ss << "\n";
        ss << "# Plasticity settings\n";
        ss << "plasticity_learning_rate = 0.01\n";
        ss << "stdp_ltp_weight = 0.01\n";
        ss << "stdp_ltd_weight = 0.012\n";
        ss << "stdp_time_constant = 20.0\n";
        ss << "\n";
        ss << "# Development settings\n";
        ss << "development_synaptogenesis_rate = 0.001\n";
        ss << "development_pruning_rate = 0.0001\n";
        ss << "\n";
        ss << "# Neuromodulation settings\n";
        ss << "dopamine_baseline = 0.1\n";
        ss << "\n";
        ss << "# Reward settings\n";
        ss << "reward_discount_factor = 0.99\n";
        ss << "\n";
        ss << "# Environment settings\n";
        ss << "environment_name = GridWorld\n";
        ss << "environment_width = 10\n";
        ss << "environment_height = 10\n";
        ss << "\n";
        ss << "# Logging settings\n";
        ss << "log_level = INFO\n";
        ss << "log_to_file = false\n";
        ss << "log_filename = nlm.log\n";
        ss << "\n";
        ss << "# Simulation settings\n";
        ss << "max_simulation_steps = 10000\n";
        ss << "simulation_time_limit = 0.0\n";
        
        return ss.str();
    }
};

// Advanced world utilities
class WorldUtils {
public:
    static void configureWorldForLearning(
        std::shared_ptr<SimpleWorld> world,
        int width = 20,
        int height = 20,
        int visionWidth = 8,
        int visionHeight = 8,
        float maxEnergy = 100.0f
    ) {
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        world->setAgentStart(width / 2.0f, height / 2.0f);
    }
    
    static std::vector<std::vector<float>> getVisionGrid(
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent
    ) {
        auto percept = world->getSensoryPercept();
        auto vision = percept->getVision();
        
        std::vector<std::vector<float>> grid(vision->getHeight(), 
                                            std::vector<float>(vision->getWidth()));
        
        auto data = vision->getData();
        for (size_t y = 0; y < vision->getHeight(); ++y) {
            for (size_t x = 0; x < vision->getWidth(); ++x) {
                size_t index = y * vision->getWidth() + x;
                if (index < data.size()) {
                    grid[y][x] = data[index];
                }
            }
        }
        
        return grid;
    }
    
    static std::string visualizeWorld(
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int resolution = 50
    ) {
        auto body = world->getAgentBody();
        auto vision = getVisionGrid(world, agent);
        
        std::stringstream ss;
        ss << "World Visualization (" << resolution << "x" << resolution << "):" << std::endl;
        ss << "Agent Position: (" << body->x << ", " << body->y << ")" << std::endl;
        ss << "Agent Orientation: " << body->orientation << " rad" << std::endl;
        ss << "Agent Energy: " << body->energy << "/" << world->getMaxEnergy() << std::endl;
        ss << std::endl;
        
        ss << "Vision Grid (8x8):" << std::endl;
        for (size_t y = 0; y < vision.size(); ++y) {
            ss << "  [";
            for (size_t x = 0; x < vision[y].size(); ++x) {
                float val = vision[y][x];
                if (val > 0.7f) ss << "#";
                else if (val > 0.4f) ss << "+";
                else if (val > 0.1f) ss << ".";
                else ss << " ";
                ss << " ";
            }
            ss << "]" << std::endl;
        }
        
        return ss.str();
    }
};

// Advanced brain utilities
class BrainUtils {
public:
    static std::map<std::string, double> getBrainMetrics(std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        
        metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        metrics["developmental_stage"] = static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()));
        
        return metrics;
    }
    
    static std::string generateBrainReport(std::shared_ptr<Brain> brain) {
        auto metrics = getBrainMetrics(brain);
        std::stringstream ss;
        ss << "=== NLM Brain Report ===" << std::endl;
        ss << std::endl;
        ss << "Neural Network Statistics:" << std::endl;
        ss << "  Total Neurons: " << metrics["total_neurons"] << std::endl;
        ss << "  Total Synapses: " << metrics["total_synapses"] << std::endl;
        ss << "  Firing Neurons: " << metrics["firing_neurons"] << std::endl;
        ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
           << metrics["average_firing_rate"] << " Hz" << std::endl;
        ss << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(4) 
           << metrics["excitatory_inhibition_ratio"] << std::endl;
        ss << "  Total Spikes: " << metrics["total_spikes"] << std::endl;
        ss << std::endl;
        ss << "Developmental State:" << std::endl;
        ss << "  Stage: ";
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: ss << "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: ss << "CriticalPeriod"; break;
            case DevelopmentalStage::Maturation: ss << "Maturation"; break;
            case DevelopmentalStage::Adult: ss << "Adult"; break;
            case DevelopmentalStage::Aging: ss << "Aging"; break;
        }
        ss << std::endl;
        
        return ss.str();
    }
    
    static void saveBrainWithTimestamp(std::shared_ptr<Brain> brain, const std::string& basePath) {
        std::time_t now = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        
        std::string filepath = basePath + "_brain_" + std::string(timestamp) + ".bin";
        brain->save(filepath);
    }
};

// Convenience functions for common workflows
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes advanced utilities for batch processing, configuration management, and visualization.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Original bindings (existing functionality)
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

    // Original class bindings (simplified for brevity)
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
             "Get a summary string of the configuration");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Add enhanced utilities
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

    // Enhanced utilities
    py::class_<BatchProcessor>(m, "BatchProcessor")
        .def_static("run_batch", &BatchProcessor::runBatch,
                   py::arg("brain"), py::arg("agent"), py::arg("world"),
                   py::arg("num_steps"), py::arg("batch_size") = 100,
                   "Run batch processing on brain simulation");

    py::class_<ConfigUtils>(m, "ConfigUtils")
        .def_static("load_config_from_file", &ConfigUtils::loadConfigFromFile,
                   py::arg("filepath"), "Load configuration from file")
        .def_static("save_config_to_file", &ConfigUtils::saveConfigToFile,
                   py::arg("config"), py::arg("filepath"), "Save configuration to file")
        .def_static("generate_config_template", &ConfigUtils::generateConfigTemplate,
                   "Generate configuration file template");

    py::class_<WorldUtils>(m, "WorldUtils")
        .def_static("configure_world_for_learning", &WorldUtils::configureWorldForLearning,
                   py::arg("world"), py::arg("width") = 20, py::arg("height") = 20,
                   py::arg("vision_width") = 8, py::arg("vision_height") = 8,
                   py::arg("max_energy") = 100.0f,
                   "Configure world for learning scenarios")
        .def_static("get_vision_grid", &WorldUtils::getVisionGrid,
                   py::arg("world"), py::arg("agent"),
                   "Get vision data as 2D grid")
        .def_static("visualize_world", &WorldUtils::visualizeWorld,
                   py::arg("world"), py::arg("agent"), py::arg("resolution") = 50,
                   "Create text visualization of world state");

    py::class_<BrainUtils>(m, "BrainUtils")
        .def_static("get_brain_metrics", &BrainUtils::getBrainMetrics,
                   py::arg("brain"), "Get brain performance metrics")
        .def_static("generate_brain_report", &BrainUtils::generateBrainReport,
                   py::arg("brain"), "Generate detailed brain report")
        .def_static("save_brain_with_timestamp", &BrainUtils::saveBrainWithTimestamp,
                   py::arg("brain"), py::arg("base_path"), "Save brain with timestamp");

    // Add constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add example function
    m.def("example_usage", []() {
        std::cout << "=== NLM Python Usage Examples ===" << std::endl << std::endl;
        std::cout << "# Basic brain creation and simulation" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "# Create a brain with default configuration" << std::endl;
        std::cout << "brain = pynlm.createBrain(pynlm.createDefaultConfig())" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "# Initialize and run a simulation" << std::endl;
        std::cout << "brain.initialize()" << std::endl;
        std::cout << "for i in range(100):" << std::endl;
        std::cout << "    brain.step(i)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "# Get statistics" << std::endl;
        std::cout << "print(f\"Neurons: {brain.getTotalNeuronCount()}\")" << std::endl;
        std::cout << "print(f\"Firing: {brain.getFiringNeuronCount()}\")" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "# Advanced batch processing" << std::endl;
        std::cout << "batch_result = pynlm.BatchProcessor.run_batch(brain, agent, world, 1000)" << std::endl;
        std::cout << "print(f\"Average firing rate: {batch_result.avgFiringRate:.2f} Hz\")" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "# Configuration management" << std::endl;
        std::cout << "config = pynlm.createDefaultConfig()" << std::endl;
        std::cout << "config.set('brain.neuron_count', 2000)" << std::endl;
        std::cout << "# Save to file" << std::endl;
        std::cout << "pynlm.ConfigUtils.save_config_to_file(config.to_dict(), 'my_config.json')" << std::endl;
    }, "Show usage examples");

} // namespace nlm
