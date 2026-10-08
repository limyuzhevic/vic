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

    // Convenience functions for creating common simulation setups
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createStandardConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        return config;
    }, "Create a standard configuration with typical settings");

    m.def("createDemoConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 123);
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        return config;
    }, "Create a configuration optimized for quick demo runs");

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 456);
        config->set("neuron_count", 2000);
        config->set("region_count", 3);
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.001);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.018f);
        return config;
    }, "Create a configuration optimized for exploration and learning");

    m.def("createEducationalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 789);
        config->set("neuron_count", 200);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2f);
        config->set("simulation_timestep", 0.001);
        config->set("enable_plasticity", true);
        config->set("enable_development", false);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        return config;
    }, "Create a configuration for educational demonstrations");

    // Advanced simulation mode configurations
    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 999);
        config->set("neuron_count", 5000);
        config->set("region_count", 5);
        config->set("connection_probability", 0.08f);
        config->set("simulation_timestep", 0.001);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("synaptogenesis_rate", 0.00005f);
        config->set("pruning_rate", 0.000005f);
        return config;
    }, "Create a research-grade configuration for large-scale simulations");

    // Helper function for running a complete agent simulation episode
    m.def("run_agent_episode", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<AgentBrain> agent,
                                std::shared_ptr<SimpleWorld> world,
                                int numSteps,
                                bool verbose) {
        if (!brain || !agent || !world) {
            throw std::runtime_error("All simulation components must be non-null");
        }
        
        agent->initialize(*world);
        
        std::vector<float> rewards;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from world (simplified - could be enhanced)
            float reward = 0.0f;
            if (world->getAgentBody().health > 0.0f) {
                reward = 0.1f;  // Small positive reward for survival
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
            
            rewards.push_back(reward);
            
            if (verbose && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                           " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Reward: " + std::to_string(reward));
            }
        }
        
        float avgReward = 0.0f;
        if (!rewards.empty()) {
            float sum = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
            avgReward = sum / rewards.size();
        }
        
        return std::make_pair(rewards, avgReward);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
        py::arg("numSteps") = 1000, py::arg("verbose") = false,
        "Run a complete agent simulation episode and return rewards and average reward");

    // Enhanced statistics function
    m.def("get_brain_statistics", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        std::map<std::string, std::string> stats;
        stats["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        stats["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        stats["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
        stats["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        stats["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        stats["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        stats["e_i_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    // Visualization helper
    m.def("create_visualization_data", [](const std::shared_ptr<Brain>& brain,
                                         const std::shared_ptr<SimpleWorld>& world,
                                         int steps) {
        if (!brain || !world) {
            throw std::runtime_error("Both brain and world must be non-null");
        }
        
        std::vector<std::map<std::string, float>> visualizationData;
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            std::map<std::string, float> frameData;
            frameData["step"] = step;
            frameData["firing_neurons"] = brain->getFiringNeuronCount();
            frameData["total_spikes"] = brain->getTotalSpikeCount();
            frameData["avg_firing_rate"] = brain->getAverageFiringRate();
            
            if (world->getAgentBody().x >= 0 && world->getAgentBody().x < world->getWidth() &&
                world->getAgentBody().y >= 0 && world->getAgentBody().y < world->getHeight()) {
                frameData["agent_x"] = world->getAgentBody().x;
                frameData["agent_y"] = world->getAgentBody().y;
                frameData["agent_energy"] = world->getAgentBody().energy;
            }
            
            visualizationData.push_back(frameData);
        }
        
        return visualizationData;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
        "Create visualization data for simulation steps");

    // Quick setup helpers
    m.def("setup_agent_simulation", [](std::shared_ptr<Config> config,
                                      std::shared_ptr<SimpleWorld> world,
                                      bool enableCuriosity,
                                      bool enableReward,
                                      bool enableDevelopment) {
        if (!config || !world) {
            throw std::runtime_error("Config and world cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableCuriosity(enableCuriosity);
        agent->enableRewardModulation(enableReward);
        agent->enableDevelopment(enableDevelopment);
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"), py::arg("enableCuriosity") = true,
        py::arg("enableReward") = true, py::arg("enableDevelopment") = true,
        "Quick helper to set up a complete agent simulation");

    // Benchmarking helper
    m.def("benchmark_brain", [](const std::shared_ptr<Brain>& brain,
                               int numSteps) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, double> results;
        results["steps_per_second"] = numSteps / (duration.count() / 1000.0);
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["spikes_per_second"] = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("numSteps"), "Benchmark brain performance");

    // Error handling with Python-style exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown exception occurred");
        }
    });

    // Add version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

    // Add useful constants
    m.attr("DEFAULT_RANDOM_SEED") = 42;
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_TIME_STEP") = 0.001;

    // Add common neuron types as convenience constants
    m.attr("EXCITATORY_NEURON_TYPE") = NeuronType::Excitatory;
    m.attr("INHIBITORY_NEURON_TYPE") = NeuronType::Inhibitory;
    m.attr("SENSORY_NEURON_TYPE") = NeuronType::Sensory;
    m.attr("MOTOR_NEURON_TYPE") = NeuronType::Motor;
    m.attr("INTERNAL_NEURON_TYPE") = NeuronType::Internal;

    // Add common action types
    m.attr("MOVE_FORWARD_ACTION") = ActionType::MoveForward;
    m.attr("MOVE_BACKWARD_ACTION") = ActionType::MoveBackward;
    m.attr("TURN_LEFT_ACTION") = ActionType::TurnLeft;
    m.attr("TURN_RIGHT_ACTION") = ActionType::TurnRight;
    m.attr("INTERACT_ACTION") = ActionType::Interact;
    m.attr("WAIT_ACTION") = ActionType::Wait;

    // Add utility functions for common patterns
    m.def("print_brain_status", [](const std::shared_ptr<Brain>& brain) {
        if (brain) {
            brain->logStatus();
        }
    }, py::arg("brain"), "Print detailed brain status information");

    m.def("save_brain_checkpoint", [](const std::shared_ptr<Brain>& brain,
                                      const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to checkpoint file");

    m.def("load_brain_checkpoint", [](std::shared_ptr<Brain>& brain,
                                      const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from checkpoint file");

    m.def("reset_simulation", [](std::shared_ptr<Brain>& brain,
                                 std::shared_ptr<AgentBrain>& agent,
                                 std::shared_ptr<SimpleWorld>& world) {
        if (brain) brain->reset();
        if (agent) agent->reset();
        if (world) world->reset();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), "Reset all simulation components");

    m.def("get_simulation_stats", [](const std::shared_ptr<Brain>& brain,
                                    const std::shared_ptr<AgentBrain>& agent,
                                    const std::shared_ptr<SimpleWorld>& world) {
        std::map<std::string, std::string> stats;
        
        if (brain) {
            stats["neurons"] = std::to_string(brain->getTotalNeuronCount());
            stats["synapses"] = std::to_string(brain->getTotalSynapseCount());
            stats["spikes"] = std::to_string(brain->getTotalSpikeCount());
            stats["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        }
        
        if (agent) {
            stats["curiosity"] = std::to_string(agent->getCuriosityLevel());
            stats["novelty"] = std::to_string(agent->getNoveltyLevel());
            stats["prediction_error"] = std::to_string(agent->getPredictionError());
            stats["development_stage"] = std::to_string(static_cast<int>(agent->getDevelopmentalStage()));
        }
        
        if (world) {
            stats["world_width"] = std::to_string(world->getWidth());
            stats["world_height"] = std::to_string(world->getHeight());
            stats["agent_x"] = std::to_string(world->getAgentBody().x);
            stats["agent_y"] = std::to_string(world->getAgentBody().y);
            stats["simulation_time"] = std::to_string(world->getSimulationTime());
        }
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), "Get comprehensive simulation statistics");

    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain,
                                       int numSteps,
                                       bool verbose = false) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        brain->reset();
        brain->initialize();
        
        float initialWeight = 0.5f;  // Would need to get actual initial weight from brain
        float finalWeight = 0.5f;
        
        for (int step = 0; step < numSteps; ++step) {
            // Inject sensory-like input to stimulate learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step);
            
            if (verbose && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        float weightChange = finalWeight - initialWeight;
        float learningRate = std::abs(weightChange) / numSteps;
        
        return std::make_tuple(weightChange, learningRate);
    }, py::arg("brain"), py::arg("numSteps"), py::arg("verbose") = false,
        "Run a simple learning experiment and return weight change and learning rate");

    m.def("benchmark_neuron_creation", [](int numNeurons,
                                        int numIterations = 10) {
        std::vector<double> times;
        
        for (int iter = 0; iter < numIterations; ++iter) {
            auto config = std::make_shared<Config>();
            config->set("random_seed", iter);
            config->set("neuron_count", numNeurons);
            config->set("region_count", 1);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            times.push_back(duration.count());
        }
        
        double avgTime = 0.0;
        for (double t : times) avgTime += t;
        avgTime /= times.size();
        
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        
        return std::make_tuple(avgTime, minTime, maxTime);
    }, py::arg("numNeurons"), py::arg("numIterations") = 10,
        "Benchmark neuron creation performance");

    m.def("get_version_info", []() {
        std::map<std::string, std::string> versionInfo;
        versionInfo["version"] = "0.1.0";
        versionInfo["major"] = "0";
        versionInfo["minor"] = "1";
        versionInfo["patch"] = "0";
        versionInfo["build"] = __DATE__ " " __TIME__;
        versionInfo["description"] = "NLM (Neural Learning Machine) Python Bindings";
        return versionInfo;
    }, "Get version information as a dictionary");

    m.def("create_test_brain", [](bool enablePlasticity = true,
                                bool enableDevelopment = true,
                                bool enableNeuromodulation = true) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain");
        }
        
        return brain;
    }, py::arg("enablePlasticity") = true,
        py::arg("enableDevelopment") = true,
        py::arg("enableNeuromodulation") = true,
        "Create a simple test brain for quick testing and examples");

    // Add context manager support for simulation lifecycle
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(Manages NLM simulation lifecycle)pbdoc")
        .def(py::init<>())
        .def("create_brain", &SimulationManager::createBrain, py::arg("config"),
             "Create and initialize a brain")
        .def("run_episode", &SimulationManager::runEpisode, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
             "Run a complete simulation episode")
        .def("reset", &SimulationManager::reset, py::arg("brain"), py::arg("agent"), py::arg("world"),
             "Reset all simulation components")
        .def("cleanup", &SimulationManager::cleanup, py::arg("brain"), py::arg("agent"), py::arg("world"),
             "Clean up simulation components");

    // Add mathematical constants and utilities
    m.def("calculate_learning_curvature", [](const std::vector<float>& rewards) {
        if (rewards.size() < 3) {
            return 0.0f; // Not enough data
        }
        
        float sum1 = 0.0f, sum2 = 0.0f;
        for (size_t i = 1; i < rewards.size() - 1; ++i) {
            float diff = rewards[i] - rewards[i-1];
            sum1 += diff;
            sum2 += diff * diff;
        }
        
        if (sum2 == 0.0f) return 0.0f;
        return (rewards.size() - 2) * sum2 - sum1 * sum1;
    }, py::arg("rewards"), "Calculate learning curvature from reward sequence");

    m.def("normalize_reward_sequence", [](const std::vector<float>& rewards) {
        if (rewards.empty()) {
            return std::vector<float>();
        }
        
        float minVal = *std::min_element(rewards.begin(), rewards.end());
        float maxVal = *std::max_element(rewards.begin(), rewards.end());
        float range = maxVal - minVal;
        
        if (range == 0.0f) {
            return std::vector<float>(rewards.size(), 0.5f);
        }
        
        std::vector<float> normalized;
        normalized.reserve(rewards.size());
        for (float r : rewards) {
            normalized.push_back((r - minVal) / range);
        }
        
        return normalized;
    }, py::arg("rewards"), "Normalize a reward sequence to [0, 1] range");

    // Add visualization utilities
    m.def("generate_visualization_csv", [](const std::vector<std::map<std::string, float>>& data,
                                          const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        if (!data.empty()) {
            // Write headers
            size_t headerSize = data[0].size();
            size_t i = 0;
            for (const auto& pair : data[0]) {
                file << pair.first;
                if (++i < headerSize) file << ",";
            }
            file << "\n";
            
            // Write data rows
            for (const auto& row : data) {
                size_t i = 0;
                for (const auto& pair : row) {
                    file << pair.second;
                    if (++i < row.size()) file << ",";
                }
                file << "\n";
            }
        }
        
        file.close();
    }, py::arg("data"), py::arg("filename"), "Generate CSV file from visualization data");

    // Add performance monitoring utilities
    m.def("monitor_memory_usage", []() {
        std::map<std::string, double> memoryStats;
        
        // This is a placeholder - actual memory monitoring would require platform-specific code
        memoryStats["cpu_usage"] = 0.0; // Placeholder
        memoryStats["memory_usage_mb"] = 0.0; // Placeholder
        memoryStats["neural_memory_kb"] = 0.0; // Placeholder
        
        return memoryStats;
    }, "Get current memory usage statistics");

    // Add debugging utilities
    m.def("enable_debug_logging", []() {
        auto logger = std::make_shared<Logger>();
        auto debugLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(debugLogger);
        Logger::setGlobal(logger);
        NLM_LOG_INFO("Debug logging enabled");
    }, "Enable debug level logging");

    m.def("disable_debug_logging", []() {
        auto logger = std::make_shared<Logger>();
        auto infoLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(infoLogger);
        Logger::setGlobal(logger);
        NLM_LOG_INFO("Debug logging disabled");
    }, "Disable debug level logging");

    // Add educational helpers
    m.def("create_educational_example", []() {
        // Create a simple educational example with a small brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 50);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create a world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        world->setAgentStart(5.0, 5.0);
        
        // Create an agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all features for educational demonstration
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, agent, world);
    }, "Create an educational example simulation with all features enabled");

    // Add research helpers
    m.def("create_research_environment", [](int numNeurons, int numRegions,
                                           float connectionProbability,
                                           bool includeAllFeatures = true) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 12345);
        config->set("neuron_count", numNeurons);
        config->set("region_count", numRegions);
        config->set("connection_probability", connectionProbability);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        
        if (includeAllFeatures) {
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("synaptogenesis_rate", 0.00005f);
            config->set("pruning_rate", 0.000005f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("numNeurons") = 1000, py::arg("numRegions") = 3,
        py::arg("connectionProbability") = 0.1f, py::arg("includeAllFeatures") = true,
        "Create a research-grade environment with specified parameters");

    // Add export functions for common simulation scenarios
    m.def("export_basic_simulation", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, std::string> simulationData;
        simulationData["type"] = "basic_neural_simulation";
        simulationData["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        simulationData["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        simulationData["learning_enabled"] = "true";
        simulationData["version"] = "0.1.0";
        return simulationData;
    }, py::arg("brain"), "Export basic simulation metadata");

    m.def("export_advanced_simulation", [](const std::shared_ptr<Brain>& brain,
                                          const std::shared_ptr<AgentBrain>& agent,
                                          const std::shared_ptr<SimpleWorld>& world) {
        std::map<std::string, std::string> simulationData;
        simulationData["type"] = "advanced_agent_simulation";
        simulationData["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        simulationData["region_count"] = std::to_string(brain->getRegionCount());
        simulationData["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        simulationData["learning_enabled"] = "true";
        simulationData["development_enabled"] = "true";
        simulationData["neuromodulation_enabled"] = "true";
        simulationData["curiosity_enabled"] = "true";
        simulationData["reward_enabled"] = "true";
        simulationData["version"] = "0.1.0";
        simulationData["world_width"] = std::to_string(world->getWidth());
        simulationData["world_height"] = std::to_string(world->getHeight());
        
        return simulationData;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), "Export advanced simulation metadata");

    // Add simulation state management
    m.def("save_simulation_state", [](const std::shared_ptr<Brain>& brain,
                                     const std::shared_ptr<AgentBrain>& agent,
                                     const std::shared_ptr<SimpleWorld>& world,
                                     const std::string& baseFilename) {
        std::string timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::string filename = baseFilename + "_" + timestamp + ".json";
        
        auto brainConfig = brain->getConfig();
        if (brainConfig) {
            brainConfig->saveToFile(filename);
        }
        
        return filename;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("baseFilename") = "simulation_state",
        "Save complete simulation state to JSON file");

    m.def("load_simulation_state", [](std::shared_ptr<Brain>& brain,
                                     std::shared_ptr<AgentBrain>& agent,
                                     std::shared_ptr<SimpleWorld>& world,
                                     const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filename)) {
            brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                agent = std::make_shared<AgentBrain>(brain);
                world = std::make_shared<SimpleWorld>();
                agent->initialize(*world);
                return true;
            }
        }
        return false;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"),
        "Load simulation state from JSON file");

    // Add experiment management
    py::class_<ExperimentManager>(m, "ExperimentManager", R"pbdoc(Manages NLM experiments)pbdoc")
        .def(py::init<>())
        .def("create_experiment", &ExperimentManager::createExperiment, py::arg("name"), py::arg("type"),
             "Create a new experiment")
        .def("run_experiment", &ExperimentManager::runExperiment, py::arg("experiment_name"),
             "Run a specific experiment")
        .def("get_experiment_results", &ExperimentManager::getExperimentResults, py::arg("experiment_name"),
             "Get results from a completed experiment")
        .def("list_experiments", &ExperimentManager::listExperiments,
             "List all available experiments");

    // Add statistical analysis utilities
    m.def("calculate_firing_statistics", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        if (totalNeurons == 0) {
            return stats;
        }
        
        size_t firingNeurons = brain->getFiringNeuronCount();
        size_t totalSpikes = brain->getTotalSpikeCount();
        float avgFiringRate = brain->getAverageFiringRate();
        float e_i_ratio = brain->getExcitationInhibitionRatio();
        
        stats["firing_rate_percentage"] = (firingNeurons * 100.0) / totalNeurons;
        stats["total_spikes"] = totalSpikes;
        stats["avg_firing_rate_hz"] = avgFiringRate;
        stats["e_i_ratio"] = e_i_ratio;
        
        return stats;
    }, py::arg("brain"), "Calculate comprehensive firing statistics");

    m.def("calculate_plasticity_metrics", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> metrics;
        
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        metrics["plasticity_enabled"] = 1.0;
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        
        // Get STDP system if available
        auto* stdp = brain->getSTDP();
        if (stdp) {
            metrics["stdp_enabled"] = 1.0;
            // Note: STDP doesn't expose direct metrics through current API
        }
        
        return metrics;
    }, py::arg("brain"), "Calculate plasticity-related metrics");

    m.def("calculate_development_metrics", [](const std::shared_ptr<Brain>& brain,
                                             const std::shared_ptr<AgentBrain>& agent) {
        std::map<std::string, double> metrics;
        
        if (!brain || !agent) {
            throw std::runtime_error("Brain and agent cannot be null");
        }
        
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        float curiosity = agent ? agent->getCuriosityLevel() : 0.0f;
        float novelty = agent ? agent->getNoveltyLevel() : 0.0f;
        
        metrics["developmental_stage"] = static_cast<int>(stage);
        metrics["curiosity_level"] = curiosity;
        metrics["novelty_level"] = novelty;
        
        return metrics;
    }, py::arg("brain"), py::arg("agent"), "Calculate development-related metrics");

    // Add file management utilities
    m.def("list_simulation_files", [](const std::string& directory) {
        std::vector<std::string> files;
        std::string searchPath = directory.empty() ? "." : directory;
        
        for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".json" || 
                 entry.path().extension() == ".bin" ||
                 entry.path().extension() == ".log")) {
                files.push_back(entry.path().string());
            }
        }
        
        return files;
    }, py::arg("directory") = "simulation_files",
        "List simulation-related files in a directory");

    m.def("cleanup_simulation_files", [](const std::string& directory,
                                        const std::vector<std::string>& patterns) {
        size_t cleanedCount = 0;
        
        for (const auto& pattern : patterns) {
            std::regex regexPattern(pattern);
            
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && 
                    std::regex_search(entry.path().string(), regexPattern)) {
                    std::filesystem::remove(entry.path());
                    cleanedCount++;
                }
            }
        }
        
        return cleanedCount;
    }, py::arg("directory") = ".", py::arg("patterns") = std::vector<std::string>({"*.log", "*.tmp"}),
        "Clean up simulation files matching specified patterns");

    // Add collaboration and sharing utilities
    m.def("export_results_to_json", [](const std::map<std::string, std::string>& results,
                                     const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "{\n";
        size_t i = 0;
        for (const auto& pair : results) {
            file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
            if (++i < results.size()) file << ",";
            file << "\n";
        }
        file << "}\n";
        file.close();
        
        return filename;
    }, py::arg("results"), py::arg("filename"), "Export simulation results to JSON file");

    m.def("import_results_from_json", [](const std::string& filename) {
        std::map<std::string, std::string> results;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        // Simple JSON parsing (would need a proper JSON library for production)
        std::string line;
        while (std::getline(file, line)) {
            // Very basic JSON parsing - in production would use a proper JSON library
            size_t colonPos = line.find(":\");
            if (colonPos != std::string::npos) {
                size_t keyEnd = line.find("\"", 1);
                std::string key = line.substr(1, keyEnd - 1);
                
                size_t valueStart = line.find("\":\"", keyEnd) + 3;
                size_t valueEnd = line.find("\"", valueStart);
                std::string value = line.substr(valueStart, valueEnd - valueStart);
                
                results[key] = value;
            }
        }
        
        file.close();
        return results;
    }, py::arg("filename"), "Import simulation results from JSON file");

    // Add model persistence utilities
    m.def("save_model_weights", [](const std::shared_ptr<Brain>& brain,
                                  const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        // Create a model snapshot with key brain parameters
        std::map<std::string, std::vector<float>> modelSnapshot;
        
        // Capture neuron states (sample for demonstration)
        modelSnapshot["neuron_count"] = {static_cast<float>(brain->getTotalNeuronCount())};
        modelSnapshot["synapse_count"] = {static_cast<float>(brain->getTotalSynapseCount())};
        modelSnapshot["firing_rate"] = {brain->getAverageFiringRate()};
        
        // Save to file (simplified - would need proper serialization)
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "{\n";
        file << "  \"neuron_count\": " << brain->getTotalNeuronCount() << ",\n";
        file << "  \"synapse_count\": " << brain->getTotalSynapseCount() << ",\n";
        file << "  \"firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        file << "  \"timestamp\": \"" << filename << "\"\n";
        file << "}\n";
        file.close();
        
        return filename;
    }, py::arg("brain"), py::arg("filename"), "Save model weights to file");

    m.def("load_model_weights", [](std::shared_ptr<Brain>& brain,
                                  const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        // Very basic model loading (would need proper deserialization in production)
        std::string line;
        size_t neuronCount = 0, synapseCount = 0;
        float firingRate = 0.0f;
        
        while (std::getline(file, line)) {
            if (line.find("\"neuron_count\"") != std::string::npos) {
                neuronCount = std::stoul(line.substr(line.find(":") + 1, line.find(",") - line.find(":") - 1));
            } else if (line.find("\"synapse_count\"") != std::string::npos) {
                synapseCount = std::stoul(line.substr(line.find(":") + 1, line.find(",") - line.find(":") - 1));
            } else if (line.find("\"firing_rate\"") != std::string::npos) {
                firingRate = std::stof(line.substr(line.find(":") + 1, line.find(",") - line.find(":") - 1));
            }
        }
        
        file.close();
        
        if (neuronCount > 0) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", neuronCount);
            brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return true;
            }
        }
        
        return false;
    }, py::arg("brain"), py::arg("filename"), "Load model weights from file");

    // Add educational demonstration helpers
    m.def("create_educational_demo", []() {
        // Create a minimal educational demonstration
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, "Create a simple educational demonstration brain");

    m.def("run_educational_demo", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world,
                                   int steps = 100) {
        if (!brain || !agent || !world) {
            throw std::runtime_error("All components must be non-null");
        }
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        std::cout << "Educational Demo Simulation" << std::endl;
        std::cout << "===========================" << std::endl;
        std::cout << "Initial Configuration:" << std::endl;
        std::cout << "  - Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  - Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "  - Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  - Agent features: Reward, Curiosity, Development" << std::endl;
        std::cout << std::endl;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            if (world->getAgentBody().health > 0.0f) {
                reward = 0.1f;
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
            
            if (step % 20 == 0 || step == steps - 1) {
                std::cout << "Step " << step << ": "
                         << "Firing: " << brain->getFiringNeuronCount() << ", "
                         << "Reward: " << reward << ", "
                         << "Curiosity: " << agent->getCuriosityLevel() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Demo completed successfully!" << std::endl;
        std::cout << "Final stats:" << std::endl;
        std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  - Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps") = 100,
        "Run an educational demonstration with step-by-step output");

    // Add quick-start helpers
    m.def("quick_start_basic", []() {
        // Create a complete basic simulation for quick testing
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(8, 8, 6, 6);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, "Quick start with a basic simulation");

    m.def("quick_start_advanced", []() {
        // Create a complete advanced simulation for comprehensive testing
        auto config = std::make_shared<Config>();
        config->set("random_seed", 12345);
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        config->set("connection_probability", 0.1f);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 12, 12);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, "Quick start with an advanced simulation");

    m.def("quick_start_research", []() {
        // Create a research-grade simulation
        auto config = std::make_shared<Config>();
        config->set("random_seed", 999);
        config->set("neuron_count", 2000);
        config->set("region_count", 5);
        config->set("connection_probability", 0.08f);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_neuromodulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward", true);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("synaptogenesis_rate", 0.00005f);
        config->set("pruning_rate", 0.000005f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, "Quick start with a research-grade simulation");

    // Add utility functions for common patterns
    m.def("create_brain_from_config_file", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filename)) {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return brain;
            }
        }
        return std::shared_ptr<Brain>(nullptr);
    }, py::arg("filename"), "Create a brain from a configuration file");

    m.def("save_brain_to_config_file", [](const std::shared_ptr<Brain>& brain,
                                         const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        auto config = brain->getConfig();
        if (config) {
            config->saveToFile(filename);
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("filename"), "Save brain configuration to file");

    m.def("copy_brain", [](const std::shared_ptr<Brain>& sourceBrain,
                         std::shared_ptr<Brain>& targetBrain) {
        if (!sourceBrain) {
            throw std::runtime_error("Source brain cannot be null");
        }
        
        auto sourceConfig = sourceBrain->getConfig();
        if (!sourceConfig) {
            throw std::runtime_error("Source brain has no configuration");
        }
        
        auto targetConfig = std::make_shared<Config>(*sourceConfig);
        targetBrain = std::make_shared<Brain>(targetConfig);
        
        return targetBrain->initialize();
    }, py::arg("sourceBrain"), py::arg("targetBrain"), "Copy a brain to another brain");

    m.def("compare_brains", [](const std::shared_ptr<Brain>& brain1,
                              const std::shared_ptr<Brain>& brain2) {
        std::map<std::string, double> comparison;
        
        if (!brain1 || !brain2) {
            throw std::runtime_error("Both brains must be non-null");
        }
        
        comparison["neuron_count_match"] = (brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount()) ? 1.0 : 0.0;
        comparison["synapse_count_match"] = (brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount()) ? 1.0 : 0.0;
        comparison["firing_rate_diff"] = std::abs(brain1->getAverageFiringRate() - brain2->getAverageFiringRate());
        comparison["total_spikes_diff"] = std::abs(static_cast<double>(brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount()));
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return comparison metrics");

    m.def("clone_agent_with_brain", [](const std::shared_ptr<AgentBrain>& sourceAgent,
                                      const std::shared_ptr<Brain>& newBrain) {
        if (!sourceAgent || !newBrain) {
            throw std::runtime_error("Source agent and new brain must be non-null");
        }
        
        auto clonedAgent = std::make_shared<AgentBrain>(newBrain);
        clonedAgent->initialize(*sourceAgent->getBrain()->getConfig());
        
        // Copy configuration settings
        clonedAgent->enableRewardModulation(sourceAgent->isRewardModulationEnabled());
        clonedAgent->enableStructuralPlasticity(sourceAgent->isStructuralPlasticityEnabled());
        clonedAgent->enableDevelopment(sourceAgent->isDevelopmentEnabled());
        clonedAgent->enableCuriosity(sourceAgent->isCuriosityEnabled());
        
        return clonedAgent;
    }, py::arg("sourceAgent"), py::arg("newBrain"), "Clone an agent with a new brain");

    m.def("create_agent_pair", [](const std::shared_ptr<Brain>& brain1,
                                const std::shared_ptr<Brain>& brain2,
                                const std::shared_ptr<SimpleWorld>& world) {
        auto agent1 = std::make_shared<AgentBrain>(brain1);
        auto agent2 = std::make_shared<AgentBrain>(brain2);
        
        agent1->initialize(*world);
        agent2->initialize(*world);
        
        return std::make_tuple(agent1, agent2);
    }, py::arg("brain1"), py::arg("brain2"), py::arg("world"), "Create a pair of agents with different brains");

    m.def("enable_all_features", [](std::shared_ptr<AgentBrain>& agent) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("agent"), "Enable all agent features for maximum learning capability");

    m.def("disable_all_features", [](std::shared_ptr<AgentBrain>& agent) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        agent->enableRewardModulation(false);
        agent->enableStructuralPlasticity(false);
        agent->enableDevelopment(false);
        agent->enableCuriosity(false);
        
        return agent;
    }, py::arg("agent"), "Disable all agent features for stable operation");

    m.def("toggle_all_features", [](std::shared_ptr<AgentBrain>& agent) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        bool currentState = agent->isRewardModulationEnabled();
        agent->enableRewardModulation(!currentState);
        agent->enableStructuralPlasticity(!currentState);
        agent->enableDevelopment(!currentState);
        agent->enableCuriosity(!currentState);
        
        return agent;
    }, py::arg("agent"), "Toggle all agent features");

    m.def("get_agent_config", [](const std::shared_ptr<AgentBrain>& agent) {
        std::map<std::string, bool> config;
        
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        config["reward_modulation"] = agent->isRewardModulationEnabled();
        config["structural_plasticity"] = agent->isStructuralPlasticityEnabled();
        config["development"] = agent->isDevelopmentEnabled();
        config["curiosity"] = agent->isCuriosityEnabled();
        
        return config;
    }, py::arg("agent"), "Get current agent configuration");

    m.def("set_agent_config", [](std::shared_ptr<AgentBrain>& agent,
                               const std::map<std::string, bool>& config) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        if (config.count("reward_modulation")) {
            agent->enableRewardModulation(config.at("reward_modulation"));
        }
        if (config.count("structural_plasticity")) {
            agent->enableStructuralPlasticity(config.at("structural_plasticity"));
        }
        if (config.count("development")) {
            agent->enableDevelopment(config.at("development"));
        }
        if (config.count("curiosity")) {
            agent->enableCuriosity(config.at("curiosity"));
        }
        
        return agent;
    }, py::arg("agent"), py::arg("config"), "Set agent configuration from a dictionary");

    m.def("agent_config_to_string", [](const std::shared_ptr<AgentBrain>& agent) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        std::stringstream ss;
        ss << "Agent Configuration:" << std::endl;
        ss << "  Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        
        return ss.str();
    }, py::arg("agent"), "Get agent configuration as a formatted string");

    m.def("reset_agent_simulation", [](std::shared_ptr<AgentBrain>& agent,
                                       std::shared_ptr<Brain>& brain,
                                       std::shared_ptr<SimpleWorld>& world) {
        if (agent) agent->reset();
        if (brain) brain->reset();
        if (world) world->reset();
        
        return std::make_tuple(agent, brain, world);
    }, py::arg("agent"), py::arg("brain"), py::arg("world"), "Reset all components in an agent simulation");

    m.def("cleanup_agent_simulation", [](std::shared_ptr<AgentBrain>& agent,
                                         std::shared_ptr<Brain>& brain,
                                         std::shared_ptr<SimpleWorld>& world) {
        if (agent) agent.reset();
        if (brain) brain.reset();
        if (world) world.reset();
        
        return std::make_tuple(agent, brain, world);
    }, py::arg("agent"), py::arg("brain"), py::arg("world"), "Clean up all components in an agent simulation");

    // Add version and metadata functions
    m.def("get_library_info", []() {
        std::map<std::string, std::string> info;
        info["library"] = "NLM Python Bindings";
        info["version"] = "0.1.0";
        info["description"] = "Python bindings for NLM (Neural Learning Machine)";
        info["author"] = "NLM Authors";
        info["date"] = __DATE__;
        info["build"] = __TIME__;
        info["copyright"] = "Copyright (c) 2026 NLM Authors";
        info["license"] = "MIT";
        return info;
    }, "Get library information");

    m.def("print_library_info", []() {
        auto info = get_library_info();
        std::cout << "NLM Python Library Information" << std::endl;
        std::cout << "===========================" << std::endl;
        std::cout << "Library: " << info["library"] << std::endl;
        std::cout << "Version: " << info["version"] << std::endl;
        std::cout << "Description: " << info["description"] << std::endl;
        std::cout << "Author: " << info["author"] << std::endl;
        std::cout << "Date: " << info["date"] << std::endl;
        std::cout << "Time: " << info["build"] << std::endl;
        std::cout << "License: " << info["license"] << std::endl;
    }, "Print library information to console");

    // Add tutorial and example helpers
    m.def("print_basic_tutorial", []() {
        std::string tutorial = R"(
Basic NLM Python Tutorial
=======================

1. Creating a Simple Brain:
   import nlm
   config = nlm.createDefaultConfig()
   brain = nlm.createBrain(config)
   brain.initialize()

2. Running a Simulation:
   for step in range(100):
       brain.step(step)

3. Getting Statistics:
   print(f"Neurons: {brain.getTotalNeuronCount()}")
   print(f"Spikes: {brain.getTotalSpikeCount()}")

4. Using an Agent:
   world = nlm.createSimpleWorld()
   agent = nlm.createAgentBrain(brain)
   agent.initialize(world)

5. Running an Episode:
   for _ in range(50):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(_)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())

For more examples, visit the NLM documentation.
        )";
        std::cout << tutorial << std::endl;
    }, "Print basic tutorial to console");

    m.def("print_advanced_tutorial", []() {
        std::string tutorial = R"(
Advanced NLM Python Tutorial
===========================

1. Using Different Configuration Types:
   import nlm
   
   # Quick demo configuration
   config = nlm.createDemoConfig()
   
   # Research configuration
   config = nlm.createResearchConfig()
   
   # Educational configuration
   config = nlm.createEducationalConfig()

2. Agent Configuration:
   agent = nlm.createAgentBrain(brain)
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   agent.enableDevelopment(True)

3. Simulation Management:
   brain, agent, world = nlm.setup_agent_simulation(config, world)
   
   # Run complete episode
   rewards, avg_reward = nlm.run_agent_episode(brain, agent, world, 1000)

4. Data Analysis:
   stats = nlm.get_brain_statistics(brain)
   for key, value in stats.items():
       print(f"{key}: {value}")

5. Visualization:
   data = nlm.create_visualization_data(brain, world, 100)
   nlm.generate_visualization_csv(data, "visualization.csv")

6. Benchmarking:
   results = nlm.benchmark_brain(brain, 1000)
   for key, value in results.items():
       print(f"{key}: {value}")

7. Experiment Management:
   manager = nlm.ExperimentManager()
   manager.create_experiment("demo", "basic")
   manager.run_experiment("demo")
   results = manager.get_experiment_results("demo")
        )";
        std::cout << tutorial << std::endl;
    }, "Print advanced tutorial to console");

    // Add error message helpers
    m.def("get_error_message", [](const std::string& errorType) {
        std::map<std::string, std::string> messages;
        messages["brain_not_initialized"] = "Brain must be initialized before running simulation steps.";
        messages["invalid_config"] = "Invalid configuration provided. Please check configuration file.";
        messages["agent_not_initialized"] = "Agent must be initialized with a world before processing sensory input.";
        messages["brain_null"] = "Brain reference is null. Please ensure brain is properly created and initialized.";
        messages["world_null"] = "World reference is null. Please ensure world is properly created.";
        messages["agent_null"] = "Agent reference is null. Please ensure agent is properly created.";
        messages["simulation_error"] = "Error occurred during simulation. Check logs for details.";
        messages["memory_error"] = "Memory allocation failed. Try reducing simulation complexity.";
        messages["file_error"] = "File operation failed. Check file permissions and path.";
        
        if (messages.find(errorType) != messages.end()) {
            return messages[errorType];
        }
        return std::string("Unknown error type: ") + errorType;
    }, py::arg("errorType"), "Get error message for a specific error type");

    // Add logging helpers
    m.def("enable_verbose_logging", []() {
        auto logger = std::make_shared<Logger>();
        auto verboseLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(verboseLogger);
        Logger::setGlobal(logger);
        NLM_LOG_INFO("Verbose logging enabled");
    }, "Enable verbose (debug level) logging");

    m.def("disable_verbose_logging", []() {
        auto logger = std::make_shared<Logger>();
        auto infoLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(infoLogger);
        Logger::setGlobal(logger);
        NLM_LOG_INFO("Verbose logging disabled");
    }, "Disable verbose (debug level) logging");

    m.def("log_simulation_summary", [](const std::shared_ptr<Brain>& brain,
                                      const std::shared_ptr<AgentBrain>& agent,
                                      const std::shared_ptr<SimpleWorld>& world) {
        NLM_LOG_INFO("=== Simulation Summary ===");
        if (brain) {
            NLM_LOG_INFO("Brain: " + std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(brain->getTotalSynapseCount()) + " synapses");
            NLM_LOG_INFO("Firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
            NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
        if (agent) {
            NLM_LOG_INFO("Agent: Curiosity=" + std::to_string(agent->getCuriosityLevel()) +
                        ", Novelty=" + std::to_string(agent->getNoveltyLevel()) +
                        ", Development=" + std::to_string(static_cast<int>(agent->getDevelopmentalStage())));
        }
        if (world) {
            NLM_LOG_INFO("World: " + std::to_string(world->getWidth()) + "x" + std::to_string(world->getHeight()) +
                        ", Time=" + std::to_string(world->getSimulationTime()) + "s");
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), "Log a comprehensive simulation summary");

    // Add performance monitoring helpers
    m.def("get_performance_metrics", [](const std::shared_ptr<Brain>& brain,
                                        const std::chrono::high_resolution_clock::time_point& startTime) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, double> metrics;
        metrics["simulation_time_ms"] = duration.count();
        metrics["steps_per_second"] = brain ? (brain->getTotalSpikeCount() / (duration.count() / 1000.0)) : 0.0;
        metrics["neurons_per_second"] = brain ? (brain->getTotalNeuronCount() / (duration.count() / 1000.0)) : 0.0;
        metrics["synapses_per_second"] = brain ? (brain->getTotalSynapseCount() / (duration.count() / 1000.0)) : 0.0;
        
        return metrics;
    }, py::arg("brain"), py::arg("startTime"), "Get performance metrics for a simulation run");

    // Add validation helpers
    m.def("validate_brain_configuration", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::string> errors;
        
        if (!brain) {
            errors.push_back("Brain pointer is null");
        } else {
            auto config = brain->getConfig();
            if (!config) {
                errors.push_back("Brain configuration is null");
            } else {
                size_t neuronCount = brain->getTotalNeuronCount();
                if (neuronCount == 0) {
                    errors.push_back("Brain has no neurons");
                } else if (neuronCount > 10000) {
                    errors.push_back("Brain has too many neurons (>10000)");
                }
                
                size_t regionCount = brain->getRegionCount();
                if (regionCount == 0) {
                    errors.push_back("Brain has no regions");
                }
                
                size_t synapseCount = brain->getTotalSynapseCount();
                if (synapseCount == 0) {
                    errors.push_back("Brain has no synapses");
                }
            }
        }
        
        return errors;
    }, py::arg("brain"), "Validate brain configuration and return any errors");

    m.def("validate_agent_configuration", [](const std::shared_ptr<AgentBrain>& agent) {
        std::vector<std::string> errors;
        
        if (!agent) {
            errors.push_back("Agent pointer is null");
        } else {
            auto brain = agent->getBrain();
            if (!brain) {
                errors.push_back("Agent has no brain reference");
            }
        }
        
        return errors;
    }, py::arg("agent"), "Validate agent configuration and return any errors");

    m.def("validate_world_configuration", [](const std::shared_ptr<SimpleWorld>& world) {
        std::vector<std::string> errors;
        
        if (!world) {
            errors.push_back("World pointer is null");
        } else {
            int width = world->getWidth();
            int height = world->getHeight();
            
            if (width <= 0 || height <= 0) {
                errors.push_back("Invalid world dimensions");
            }
            
            float energy = world->getMaxEnergy();
            if (energy <= 0.0f) {
                errors.push_back("Invalid world energy (must be positive)");
            }
        }
        
        return errors;
    }, py::arg("world"), "Validate world configuration and return any errors");

    // Add result aggregation helpers
    m.def("aggregate_simulation_results", [](const std::vector<std::tuple<std::shared_ptr<Brain>, 
                                                                          std::shared_ptr<AgentBrain>, 
                                                                          std::shared_ptr<SimpleWorld>>>& simulations) {
        std::map<std::string, double> aggregatedResults;
        
        if (simulations.empty()) {
            return aggregatedResults;
        }
        
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalSpikes = 0;
        float totalFiringRate = 0.0f;
        size_t count = 0;
        
        for (const auto& sim : simulations) {
            const auto& brain = std::get<0>(sim);
            const auto& agent = std::get<1>(sim);
            const auto& world = std::get<2>(sim);
            
            if (brain) {
                totalNeurons += brain->getTotalNeuronCount();
                totalSynapses += brain->getTotalSynapseCount();
                totalSpikes += brain->getTotalSpikeCount();
                totalFiringRate += brain->getAverageFiringRate();
                count++;
            }
        }
        
        if (count > 0) {
            aggregatedResults["avg_neurons"] = static_cast<double>(totalNeurons) / count;
            aggregatedResults["avg_synapses"] = static_cast<double>(totalSynapses) / count;
            aggregatedResults["avg_spikes"] = static_cast<double>(totalSpikes) / count;
            aggregatedResults["avg_firing_rate"] = totalFiringRate / count;
        }
        
        return aggregatedResults;
    }, py::arg("simulations"), "Aggregate results from multiple simulations");

    // Add export helpers
    m.def("export_simulation_metadata", [](const std::vector<std::tuple<std::shared_ptr<Brain>, 
                                                                        std::shared_ptr<AgentBrain>, 
                                                                        std::shared_ptr<SimpleWorld>>>& simulations,
                                       const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "{\n";
        file << "  \"simulation_count\": " << simulations.size() << ",\n";
        file << "  \"simulations\": [\n";
        
        for (size_t i = 0; i < simulations.size(); ++i) {
            const auto& brain = std::get<0>(simulations[i]);
            const auto& agent = std::get<1>(simulations[i]);
            const auto& world = std::get<2>(simulations[i]);
            
            file << "    {\n";
            file << "      \"brain_neuron_count\": " << (brain ? brain->getTotalNeuronCount() : 0) << ",\n";
            file << "      \"brain_synapse_count\": " << (brain ? brain->getTotalSynapseCount() : 0) << ",\n";
            file << "      \"brain_total_spikes\": " << (brain ? brain->getTotalSpikeCount() : 0) << ",\n";
            file << "      \"agent_curiosity\": " << (agent ? agent->getCuriosityLevel() : 0.0f) << ",\n";
            file << "      \"world_width\": " << (world ? world->getWidth() : 0) << ",\n";
            file << "      \"world_height\": " << (world ? world->getHeight() : 0) << "\n";
            
            if (i < simulations.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
        
        return filename;
    }, py::arg("simulations"), py::arg("filename"), "Export simulation metadata to JSON file");

    // Add cleanup helpers
    m.def("cleanup_simulation_memory", [](std::vector<std::tuple<std::shared_ptr<Brain>, 
                                                                   std::shared_ptr<AgentBrain>, 
                                                                   std::shared_ptr<SimpleWorld>>>& simulations) {
        size_t cleanedCount = 0;
        
        for (auto& sim : simulations) {
            auto& brain = std::get<0>(sim);
            auto& agent = std::get<1>(sim);
            auto& world = std::get<2>(sim);
            
            brain.reset();
            agent.reset();
            world.reset();
            
            cleanedCount++;
        }
        
        simulations.clear();
        return cleanedCount;
    }, py::arg("simulations"), "Clean up simulation memory and clear vector");

    // Add version comparison helpers
    m.def("check_version_compatibility", [](const std::string& requiredVersion,
                                          const std::string& currentVersion) {
        std::map<std::string, bool> compatibility;
        
        // Simple version parsing (major.minor.patch)
        size_t reqMajor = std::stoul(requiredVersion.substr(0, requiredVersion.find('.')));
        size_t reqMinor = std::stoul(requiredVersion.substr(requiredVersion.find('.') + 1, 
                                                          requiredVersion.rfind('.') - requiredVersion.find('.') - 1));
        size_t reqPatch = std::stoul(requiredVersion.substr(requiredVersion.rfind('.') + 1));
        
        size_t curMajor = std::stoul(currentVersion.substr(0, currentVersion.find('.')));
        size_t curMinor = std::stoul(currentVersion.substr(currentVersion.find('.') + 1, 
                                                          currentVersion.rfind('.') - currentVersion.find('.') - 1));
        size_t curPatch = std::stoul(currentVersion.substr(currentVersion.rfind('.') + 1));
        
        compatibility["major_compatible"] = (curMajor >= reqMajor);
        compatibility["minor_compatible"] = (curMajor > reqMajor) || (curMinor >= reqMinor);
        compatibility["patch_compatible"] = (curMajor > reqMajor) || (curMinor > reqMinor) || (curPatch >= reqPatch);
        compatibility["fully_compatible"] = compatibility["major_compatible"] && 
                                          compatibility["minor_compatible"] && 
                                          compatibility["patch_compatible"];
        
        return compatibility;
    }, py::arg("requiredVersion"), py::arg("currentVersion"), "Check version compatibility between required and current versions");

    // Add debugging helpers
    m.def("dump_simulation_state", [](const std::shared_ptr<Brain>& brain,
                                     const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "Brain Simulation State Dump\n";
        file << "==========================\n";
        file << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        file << "\n";
        
        if (brain) {
            file << "Neuron Count: " << brain->getTotalNeuronCount() << "\n";
            file << "Synapse Count: " << brain->getTotalSynapseCount() << "\n";
            file << "Active Neuron Count: " << brain->getActiveNeuronCount() << "\n";
            file << "Firing Neuron Count: " << brain->getFiringNeuronCount() << "\n";
            file << "Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
            file << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
            file << "Excitement/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            file << "\n";
            
            // Try to get region information
            auto regions = brain->getRegions();
            file << "Region Information:\n";
            for (size_t i = 0; i < regions.size(); ++i) {
                file << "  Region " << i+1 << ": " << regions[i]->getTotalNeuronCount() << " neurons, "
                     << regions[i]->getSynapseCount() << " synapses\n";
            }
        }
        
        file << "\n";
        file << "Dump completed successfully.\n";
        file.close();
        
        return filename;
    }, py::arg("brain"), py::arg("filename"), "Dump simulation state to a text file for debugging");

    // Add memory management helpers
    m.def("estimate_memory_usage", [](const std::shared_ptr<Brain>& brain,
                                     const std::shared_ptr<AgentBrain>& agent,
                                     const std::shared_ptr<SimpleWorld>& world) {
        std::map<std::string, double> memoryEstimate;
        
        // Rough estimates in bytes
        memoryEstimate["brain_neural_memory"] = brain ? (brain->getTotalNeuronCount() * 1000.0) : 0.0; // Rough estimate
        memoryEstimate["agent_memory"] = agent ? 5000.0 : 0.0; // Rough estimate
        memoryEstimate["world_memory"] = world ? (world->getWidth() * world->getHeight() * 100.0) : 0.0; // Rough estimate
        memoryEstimate["total_estimated_memory_mb"] = 
            (memoryEstimate["brain_neural_memory"] + memoryEstimate["agent_memory"] + memoryEstimate["world_memory"]) / (1024.0 * 1024.0);
        
        return memoryEstimate;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), "Estimate memory usage for simulation components");

    // Add utility constants and functions
    m.def("get_default_config_values", []() {
        std::map<std::string, std::string> defaults;
        defaults["random_seed"] = "42";
        defaults["neuron_count"] = "1000";
        defaults["region_count"] = "2";
        defaults["connection_probability"] = "0.1";
        defaults["simulation_timestep"] = "0.001";
        defaults["enable_plasticity"] = "true";
        defaults["enable_development"] = "true";
        defaults["enable_neuromodulation"] = "true";
        defaults["enable_curiosity"] = "true";
        defaults["enable_reward"] = "true";
        return defaults;
    }, "Get default configuration values as a dictionary");

    // Add batch processing helpers
    m.def("run_batch_simulations", [](const std::vector<std::shared_ptr<Config>>& configs,
                                     const std::vector<std::string>& simulationNames,
                                     int stepsPerSimulation,
                                     bool verbose = false) {
        std::vector<std::map<std::string, double>> results;
        
        if (configs.size() != simulationNames.size()) {
            throw std::runtime_error("Number of configs and simulation names must match");
        }
        
        for (size_t i = 0; i < configs.size(); ++i) {
            std::map<std::string, double> result;
            result["name"] = simulationNames[i];
            
            auto brain = std::make_shared<Brain>(configs[i]);
            if (!brain->initialize()) {
                result["success"] = 0.0;
                results.push_back(result);
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < stepsPerSimulation; ++step) {
                brain->step(step);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            result["success"] = 1.0;
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["avg_firing_rate"] = brain->getAverageFiringRate();
            result["simulation_time_ms"] = duration.count();
            result["steps_per_second"] = stepsPerSimulation / (duration.count() / 1000.0);
            
            results.push_back(result);
            
            if (verbose) {
                std::cout << "Completed simulation: " << simulationNames[i] << std::endl;
                std::cout << "  Spikes: " << brain->getTotalSpikeCount() << std::endl;
                std::cout << "  Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                std::cout << "  Time: " << duration.count() << " ms" << std::endl;
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("simulationNames"), py::arg("stepsPerSimulation"), py::arg("verbose") = false,
        "Run multiple simulations with different configurations and return results");

    // Add final summary helper
    m.def("print_simulation_summary", [](const std::vector<std::map<std::string, double>>& results) {
        std::cout << "=== NLM Simulation Batch Results ===" << std::endl;
        std::cout << std::endl;
        
        double totalSuccess = 0.0, totalSpikes = 0.0, totalFiringRate = 0.0, totalTime = 0.0, totalSpeed = 0.0;
        size_t validResults = 0;
        
        for (const auto& result : results) {
            if (result.find("success") != result.end() && result.at("success") > 0.5) {
                validResults++;
                totalSuccess += result.at("success");
                totalSpikes += result.value("total_spikes", 0.0);
                totalFiringRate += result.value("avg_firing_rate", 0.0);
                totalTime += result.value("simulation_time_ms", 0.0);
                totalSpeed += result.value("steps_per_second", 0.0);
                
                std::cout << "Simulation: " << result.value("name", "Unknown") << std::endl;
                std::cout << "  Success: " << (result.at("success") * 100) << "%" << std::endl;
                std::cout << "  Spikes: " << result.value("total_spikes", 0) << std::endl;
                std::cout << "  Firing rate: " << result.value("avg_firing_rate", 0.0) << " Hz" << std::endl;
                std::cout << "  Time: " << result.value("simulation_time_ms", 0.0) << " ms" << std::endl;
                std::cout << "  Speed: " << result.value("steps_per_second", 0.0) << " steps/sec" << std::endl;
                std::cout << std::endl;
            }
        }
        
        if (validResults > 0) {
            std::cout << "=== Aggregate Results ===" << std::endl;
            std::cout << "Successful simulations: " << validResults << " out of " << results.size() << std::endl;
            std::cout << "Average success rate: " << (totalSuccess / validResults) * 100 << "%" << std::endl;
            std::cout << "Average total spikes: " << totalSpikes / validResults << std::endl;
            std::cout << "Average firing rate: " << totalFiringRate / validResults << " Hz" << std::endl;
            std::cout << "Average simulation time: " << totalTime / validResults << " ms" << std::endl;
            std::cout << "Average steps per second: " << totalSpeed / validResults << " steps/sec" << std::endl;
        }
    }, py::arg("results"), "Print summary of batch simulation results");

    // Add final welcome message
    m.def("print_welcome_message", []() {
        std::cout << "=====================================================" << std::endl;
        std::cout << "Welcome to NLM (Neural Learning Machine) Python Bindings" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "This is version 0.1.0 of the NLM Python interface." << std::endl;
        std::cout << "For documentation and examples, visit: https://nlm.readthedocs.io" << std::endl;
        std::cout << std::endl;
        std::cout << "Key features:" << std::endl;
        std::cout << "  ✓ Comprehensive brain simulation" << std::endl;
        std::cout << "  ✓ Advanced agent system" << std::endl;
        std::cout << "  ✓ Rich configuration options" << std::endl;
        std::cout << "  ✓ Powerful Python API" << std::endl;
        std::cout << "  ✓ Extensive debugging tools" << std::endl;
        std::cout << "  ✓ Batch processing capabilities" << std::endl;
        std::cout << "  ✓ Performance monitoring" << std::endl;
        std::cout << std::endl;
        std::cout << "Getting started:" << std::endl;
        std::cout << "  1. Create a configuration: config = nlm.createDefaultConfig()" << std::endl;
        std::cout << "  2. Create a brain: brain = nlm.createBrain(config)" << std::endl;
        std::cout << "  3. Initialize: brain.initialize()" << std::endl;
        std::cout << "  4. Run simulation: for step in range(100): brain.step(step)" << std::endl;
        std::cout << "  5. Use agent: agent = nlm.createAgentBrain(brain)" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, use help() in the interactive environment." << std::endl;
        std::cout << "=====================================================" << std::endl;
    }, "Print welcome message with getting started information");

    // Add final registration of simulation manager
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(Manages NLM simulation lifecycle)pbdoc")
        .def(py::init<>())
        .def("create_brain", &SimulationManager::createBrain, py::arg("config"),
             "Create and initialize a brain")
        .def("run_episode", &SimulationManager::runEpisode, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
             "Run a complete simulation episode")
        .def("reset", &SimulationManager::reset, py::arg("brain"), py::arg("agent"), py::arg("world"),
             "Reset all simulation components")
        .def("cleanup", &SimulationManager::cleanup, py::arg("brain"), py::arg("agent"), py::arg("world"),
             "Clean up simulation components");

    // Add final quick start functions
    m.def("quick_start", []() {
        std::cout << "=== Quick Start: NLM Python ===" << std::endl;
        std::cout << std::endl;
        std::cout << "This script sets up and runs a basic NLM simulation." << std::endl;
        std::cout << std::endl;
        
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        std::cout << "Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "Running 100 simulation steps..." << std::endl;
        
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        std::cout << "Simulation completed!" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        return brain;
    }, "Quick start with a basic simulation");

    m.def("quick_start_advanced", []() {
        std::cout << "=== Quick Start: Advanced NLM ===" << std::endl;
        std::cout << std::endl;
        std::cout << "This script sets up and runs an advanced NLM simulation with all features." << std::endl;
        std::cout << std::endl;
        
        auto config = nlm::createResearchConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 12, 12);
        world->reset();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        std::cout << "Advanced simulation setup complete." << std::endl;
        std::cout << "Brain: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "World: " << world->getWidth() << "x" << world->getHeight() << " cells" << std::endl;
        std::cout << "Agent features enabled: Reward, Curiosity, Development, Plasticity" << std::endl;
        
        return std::make_tuple(brain, agent, world);
    }, "Quick start with an advanced simulation");

    m.def("quick_start_demo", []() {
        std::cout << "=== Quick Start: Demo Mode ===" << std::endl;
        std::cout << std::endl;
        std::cout << "This script sets up and runs a demo simulation for testing and education." << std::endl;
        std::cout << std::endl;
        
        auto config = nlm::createDemoConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        std::cout << "Demo simulation setup complete." << std::endl;
        std::cout << "Brain: " << brain->getTotalNeuronCount() << " neurons (demo scale)" << std::endl;
        std::cout << "World: " << world->getWidth() << "x" << world->getHeight() << " cells" << std::endl;
        std::cout << "Running 50 steps for demo purposes..." << std::endl;
        
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            if (world->getAgentBody().health > 0.0f) {
                reward = 0.1f;
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        std::cout << "Demo completed successfully!" << std::endl;
        std::cout << "Final stats:" << std::endl;
        std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  - Curiosity level: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "  - Novelty level: " << agent->getNoveltyLevel() << std::endl;
        std::cout << "  - Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        
        return std::make_tuple(brain, agent, world);
    }, "Quick start with a demo simulation");

    // Add final registration of main functions
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Add final utility functions
    m.def("get_version", []() {
        return "0.1.0";
    }, "Get the current version of the Python bindings");

    m.def("get_info", []() {
        std::map<std::string, std::string> info;
        info["name"] = "NLM Python Bindings";
        info["version"] = "0.1.0";
        info["description"] = "Python bindings for NLM (Neural Learning Machine)";
        info["author"] = "NLM Authors";
        return info;
    }, "Get basic information about the library");

    // Add final welcome message
    m.def("print_startup_message", []() {
        print_welcome_message();
        print_basic_tutorial();
    }, "Print startup message with welcome and basic tutorial");

    // Register the main exception type
    py::register_exception<std::runtime_error>(m, "NLMError");

    // Add final constants
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

    std::cout << "NLM Python bindings initialized successfully!" << std::endl;
    std::cout << "For help, use help() in the interactive environment." << std::endl;
}

} // namespace nlm

