// Python bindings enhancement: Add missing convenience functions and advanced features

// This file contains additions to the Python bindings to provide:
// 1. Missing convenience functions documented in easy_usage.md
// 2. Batch processing capabilities
// 3. Higher-level convenience functions
// 4. Enhanced debugging and monitoring tools

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>

namespace py = pybind11;
namespace nlm {

// Enhanced config convenience functions
void addConfigConvenienceFunctions(py::module_& m) {
    // Batch configuration setting
    m.def("configureBatch", [](std::shared_ptr<Config> config, 
                               const std::map<std::string, std::variant<int, double, bool, std::string>>& settings) {
        for (const auto& [key, value] : settings) {
            std::visit([&config, &key](auto&& val) {
                config->set(key, val, ConfigSource::Runtime);
            }, value);
        }
        return config;
    }, py::arg("config"), py::arg("settings"), 
       "Apply multiple configuration settings at once");
    
    // Configuration validation
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Check for required essential keys
        std::vector<std::string> essentialKeys = {
            "brain.neuron_count",
            "brain.v_thresh",
            "brain.tau_mem"
        };
        
        for (const auto& key : essentialKeys) {
            if (!config->has(key)) {
                errors.push_back("Missing essential configuration key: " + key);
            }
        }
        
        // Validate numeric ranges
        auto neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
        if (neuronCount <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        auto v_thresh = config->getOr<double>("brain.v_thresh", 0.0);
        if (v_thresh > 0) {
            errors.push_back("brain.v_thresh should be negative (mV)");
        }
        
        auto tau_mem = config->getOr<double>("brain.tau_mem", 0.0);
        if (tau_mem <= 0) {
            errors.push_back("brain.tau_mem must be positive");
        }
        
        // Validate plasticity settings
        auto stdpEnable = config->getOr<bool>("plasticity.stdp.enable", false);
        auto hebbianEnable = config->getOr<bool>("plasticity.hebbian.enable", false);
        
        if (!stdpEnable && !hebbianEnable) {
            errors.push_back("At least one plasticity mechanism must be enabled");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");
    
    // Configuration from dictionary
    m.def("configFromDict", [](const std::map<std::string, std::variant<int, double, bool, std::string>>& configDict) {
        auto config = std::make_shared<Config>();
        for (const auto& [key, value] : configDict) {
            std::visit([&config, &key](auto&& val) {
                config->set(key, val, ConfigSource::Runtime);
            }, value);
        }
        return config;
    }, py::arg("configDict"), "Create configuration from dictionary");
    
    // Configuration to dictionary
    m.def("configToDict", [](std::shared_ptr<Config> config) {
        std::map<std::string, std::string> configDict;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            std::string valueStr;
            
            // Convert different types to string representation
            if (config->has(key)) {
                // Try different types in order of preference
                if (auto val = config->get<int64_t>(key)) {
                    valueStr = std::to_string(*val);
                } else if (auto val = config->get<double>(key)) {
                    valueStr = std::to_string(*val);
                } else if (auto val = config->get<bool>(key)) {
                    valueStr = *val ? "true" : "false";
                } else if (auto val = config->get<std::string>(key)) {
                    valueStr = *val;
                }
            }
            
            if (!valueStr.empty()) {
                configDict[key] = valueStr;
            }
        }
        
        return configDict;
    }, py::arg("config"), "Convert configuration to dictionary");
}

// Enhanced brain convenience functions
void addBrainConvenienceFunctions(py::module_& m) {
    // Run multiple steps with progress callback
    m.def("runSteps", [](std::shared_ptr<Brain> brain, 
                       size_t numSteps,
                       std::function<void(size_t, size_t)> progressCallback = nullptr) {
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            if (progressCallback) {
                progressCallback(step, numSteps);
            }
        }
        return brain;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("progressCallback") = nullptr,
       "Run multiple simulation steps with optional progress callback");
    
    // Run simulation with environment interaction
    m.def("runEnvironmentEpisode", [](std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     size_t maxSteps,
                                     std::function<void(size_t, const SensoryPercept&, float)> stepCallback = nullptr) {
        
        world->reset();
        agent->initialize(*world);
        
        for (size_t step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (stepCallback) {
                float reward = 0.0f;
                auto internal = percept.getInternal();
                if (!internal->empty()) {
                    reward = internal->front();
                }
                stepCallback(step, percept, reward);
            }
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("maxSteps"), 
       py::arg("stepCallback") = nullptr,
       "Run a complete episode with environment interaction");
    
    // Create simulated environment with reward
    m.def("createRewardWorld", [](int width, int height, float resourceDensity) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Add resources based on density
        int numResources = static_cast<int>(width * height * resourceDensity);
        for (int i = 0; i < numResources; ++i) {
            float x = static_cast<float>(rand() % width);
            float y = static_cast<float>(rand() % height);
            world->addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Resource, value=1.0));
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("resourceDensity") = 0.1,
       "Create a world with random resources");
    
    // Get brain performance statistics
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics");
}

// Enhanced agent convenience functions
void addAgentConvenienceFunctions(py::module_& m) {
    // Create learning agent with automatic feature detection
    m.def("createLearningAgent", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   bool enableAllFeatures = true) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enableAllFeatures) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableAllFeatures") = true,
       "Create an agent with all learning features enabled");
    
    // Run agent learning episode with detailed logging
    m.def("runLearningEpisode", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  size_t maxSteps,
                                  bool verbose = false) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        std::vector<std::map<std::string, double>> episodeData;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto internal = percept.getInternal();
            float reward = internal && !internal->empty() ? internal->front() : 0.0f;
            agent->applyRewardModulation(reward, 0.0);
            
            auto body = world->getAgentBody();
            
            if (verbose && step % 10 == 0) {
                std::cout << "Step " << step << ": "
                         << "Firing: " << brain->getFiringNeuronCount() << ", "
                         << "Curiosity: " << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << agent->getNoveltyLevel() << ", "
                         << "Reward: " << reward << ", "
                         << "Pos: (" << body->x << ", " << body->y << ")" << std::endl;
            }
            
            std::map<std::string, double> stepData;
            stepData["step"] = static_cast<double>(step);
            stepData["firing_neurons"] = brain->getFiringNeuronCount();
            stepData["curiosity"] = agent->getCuriosityLevel();
            stepData["novelty"] = agent->getNoveltyLevel();
            stepData["reward"] = reward;
            stepData["x"] = body->x;
            stepData["y"] = body->y;
            
            episodeData.push_back(stepData);
        }
        
        return std::make_tuple(episodeData, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("maxSteps"), py::arg("verbose") = false,
       "Run a learning episode and return detailed data");
    
    // Batch agent training
    m.def("batchTrainAgents", [](const std::vector<std::shared_ptr<Brain>>& brains,
                               const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                               const std::vector<std::shared_ptr<AgentBrain>>& agents,
                               size_t stepsPerEpisode,
                               size_t numEpisodes) {
        std::vector<std::vector<std::map<std::string, double>>> allEpisodeData;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            std::vector<std::map<std::string, double>> episodeData;
            
            for (size_t i = 0; i < brains.size(); ++i) {
                auto& brain = brains[i];
                auto& world = worlds[i];
                auto& agent = agents[i];
                
                world->reset();
                agent->initialize(*world);
                
                for (size_t step = 0; step < stepsPerEpisode; ++step) {
                    world->update(0.1);
                    
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    brain->step(step);
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    auto internal = percept.getInternal();
                    float reward = internal && !internal->empty() ? internal->front() : 0.0f;
                    agent->applyRewardModulation(reward, 0.0);
                    
                    std::map<std::string, double> stepData;
                    stepData["episode"] = static_cast<double>(episode);
                    stepData["agent"] = static_cast<double>(i);
                    stepData["step"] = static_cast<double>(step);
                    stepData["firing_neurons"] = brain->getFiringNeuronCount();
                    stepData["curiosity"] = agent->getCuriosityLevel();
                    stepData["reward"] = reward;
                    
                    episodeData.push_back(stepData);
                }
            }
            
            allEpisodeData.push_back(episodeData);
        }
        
        return allEpisodeData;
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), 
       py::arg("stepsPerEpisode"), py::arg("numEpisodes"),
       "Train multiple agents in parallel and return performance data");
}

// Enhanced world convenience functions
void addWorldConvenienceFunctions(py::module_& m) {
    // Create structured world with obstacles
    m.def("createStructuredWorld", [](int width, int height, 
                                     float obstacleDensity = 0.1,
                                     float resourceDensity = 0.05) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Add obstacles
        int numObstacles = static_cast<int>(width * height * obstacleDensity);
        for (int i = 0; i < numObstacles; ++i) {
            float x, y;
            do {
                x = static_cast<float>(rand() % width);
                y = static_cast<float>(rand() % height);
            } while (!world->isValidPosition(x, y)); // Ensure not overlapping
            
            world->addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Wall));
        }
        
        // Add resources
        int numResources = static_cast<int>(width * height * resourceDensity);
        for (int i = 0; i < numResources; ++i) {
            float x, y;
            bool valid = false;
            do {
                x = static_cast<float>(rand() % width);
                y = static_cast<float>(rand() % height);
                valid = world->isValidPosition(x, y);
            } while (!valid);
            
            world->addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Resource, value=1.0));
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, 
       py::arg("obstacleDensity") = 0.1, py::arg("resourceDensity") = 0.05,
       "Create a world with obstacles and resources");
    
    // World analysis
    m.def("analyzeWorld", [](std::shared_ptr<SimpleWorld> world) {
        std::map<std::string, double> analysis;
        
        auto body = world->getAgentBody();
        analysis["agent_x"] = body->x;
        analysis["agent_y"] = body->y;
        analysis["agent_energy"] = body->energy;
        analysis["agent_age"] = body->age;
        
        // Count world objects by type
        auto percept = world->getSensoryPercept();
        auto vision = percept.getVision();
        // Note: This is simplified analysis
        
        return analysis;
    }, py::arg("world"), "Analyze world state and return statistics");
}

// Enhanced debugging and monitoring
void addMonitoringFunctions(py::module_& m) {
    // Create debug logger
    m.def("createDebugLogger", []() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, "Create and configure debug logger");
    
    // Get system performance metrics
    m.def("getSystemMetrics", []() {
        std::map<std::string, double> metrics;
        
        // This would require system-specific instrumentation
        // For now, return placeholder metrics
        metrics["cpu_usage"] = 0.0;  // Would require actual measurement
        metrics["memory_usage_mb"] = 0.0;  // Would require actual measurement
        metrics["simulation_time_s"] = 0.0;  // Would require timing
        
        return metrics;
    }, "Get system performance metrics");
    
    // Performance benchmark
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, 
                              size_t numSteps,
                              size_t warmUpSteps = 100) {
        // Warm up
        for (size_t step = 0; step < warmUpSteps; ++step) {
            brain->step(step);
        }
        
        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double stepsPerSecond = (numSteps / (duration.count() / 1000.0));
        
        std::map<std::string, double> results;
        results["steps_per_second"] = stepsPerSecond;
        results["total_time_ms"] = duration.count();
        results["steps_benchmarked"] = static_cast<double>(numSteps);
        
        return results;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("warmUpSteps") = 100,
       "Benchmark brain performance");
}

// Main function to add all enhanced Python bindings
void addEnhancedPythonBindings(py::module_& m) {
    addConfigConvenienceFunctions(m);
    addBrainConvenienceFunctions(m);
    addAgentConvenienceFunctions(m);
    addWorldConvenienceFunctions(m);
    addMonitoringFunctions(m);
    
    // Add version information
    m.attr("__version__") = "2.0.0-enhanced";
    
    // Add convenience aliases for common operations
    m.def("quickTest", [](size_t numSteps = 100) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("numSteps") = 100,
       "Quick test: create brain and run steps, return spike count");
    
    // Simulation factory
    m.def("createSimulation", [](const std::string& configFile = "",
                                 bool enableLearning = false) {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("configFile") = "", py::arg("enableLearning") = false,
       "Factory function to create complete simulation environment");
}