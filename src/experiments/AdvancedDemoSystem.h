#ifndef NLM_ADVANCED_DEMOS_HPP
#define NLM_ADVANCED_DEMOS_HPP

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <functional>
#include <thread>

#include "../core/ExpertConfig.hpp"
#include "../core/LoggingProfiling.hpp"
#include "../core/ErrorHandling.hpp"

namespace nlm {
namespace advanced {

// Advanced demo modes for expert users
class AdvancedDemoSystem {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<Config> config;
    
    enum class DemoMode {
        BASIC_LEARNING,
        ADVANCED_LEARNING,
        TEXT_TEACHING,
        SOCIAL_LEARNING,
        DEVELOPMENTAL,
        PERFORMANCE_TEST,
        DEBUG_MODE,
        CUSTOM
    };
    
    struct DemoSettings {
        DemoMode mode;
        std::string name;
        std::string description;
        uint64_t steps;
        bool enableFeatures;
        std::map<std::string, std::string> customParameters;
    };
    
    std::vector<DemoSettings> availableDemos;
    DemoSettings currentDemo;
    
public:
    AdvancedDemoSystem() {
        initializeAvailableDemos();
    }
    
    void initializeAvailableDemos() {
        availableDemos = {
            {DemoMode::BASIC_LEARNING, "Basic Learning", 
             "Simple world interaction and reward-based learning", 
             500, true},
            {DemoMode::ADVANCED_LEARNING, "Advanced Learning",
             "Complex pattern learning with multiple memory systems",
             1000, true},
            {DemoMode::TEXT_TEACHING, "Text Teaching",
             "Learning through text as a sensory channel",
             800, true},
            {DemoMode::SOCIAL_LEARNING, "Social Learning",
             "Learning by observing and imitating others",
             1200, true},
            {DemoMode::DEVELOPMENTAL, "Developmental Learning",
             "Learning across multiple developmental stages",
             2000, true},
            {DemoMode::PERFORMANCE_TEST, "Performance Test",
             "Stress test with maximum complexity",
             500, true},
            {DemoMode::DEBUG_MODE, "Debug Mode",
             "Detailed tracing and debugging mode",
             100, false},
            {DemoMode::CUSTOM, "Custom Demo",
             "User-defined demo with custom parameters",
             500, true}
        };
    }
    
    void setupDemo(const DemoSettings& demo) {
        currentDemo = demo;
        
        // Create configuration based on demo mode
        config = std::make_shared<Config>();
        
        switch (demo.mode) {
            case DemoMode::BASIC_LEARNING:
                setupBasicLearningConfig();
                break;
            case DemoMode::ADVANCED_LEARNING:
                setupAdvancedLearningConfig();
                break;
            case DemoMode::TEXT_TEACHING:
                setupTextTeachingConfig();
                break;
            case DemoMode::SOCIAL_LEARNING:
                setupSocialLearningConfig();
                break;
            case DemoMode::DEVELOPMENTAL:
                setupDevelopmentalConfig();
                break;
            case DemoMode::PERFORMANCE_TEST:
                setupPerformanceTestConfig();
                break;
            case DemoMode::DEBUG_MODE:
                setupDebugModeConfig();
                break;
            case DemoMode::CUSTOM:
                setupCustomConfig(demo.customParameters);
                break;
        }
        
        // Initialize brain and world
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->reset();
        
        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable features based on demo settings
        if (demo.enableFeatures) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        NLM_LOG_DEBUG("Advanced demo setup completed: " + demo.name);
    }
    
    void runDemo() {
        NLM_LOG_DEBUG("Starting advanced demo: " + currentDemo.name);
        NLM_STEP_TRACE(0, "DEMO_START", currentDemo.description);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 1; step <= currentDemo.steps; ++step) {
            NLM_STEP_TRACE(step, "STEP_START", "Processing step " + std::to_string(step));
            
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            const SensoryPercept& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            ActionResult result = world->applyMotorCommand(cmd, step * 0.001);
            
            // Apply reward modulation
            agent->applyRewardModulation(result.reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Log neuron activity if enabled
            if (currentDemo.mode == DemoMode::DEBUG_MODE) {
                logNeuronActivity(step);
            }
            
            // Log performance
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            NLM_PERF_MEASURE(brain->step(step), stepDuration);
            
            NLM_STEP_TRACE(step, "STEP_COMPLETE", 
                          "Step completed with reward " + std::to_string(result.reward) +
                          ", action " + getActionName(cmd));
            
            // Print progress
            if (step % 100 == 0) {
                NLM_LOG_DEBUG("Progress: Step " + std::to_string(step) + 
                             "/" + std::to_string(currentDemo.steps) +
                             ", Reward: " + std::to_string(result.reward) +
                             ", Firing rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalDuration = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_STEP_TRACE(0, "DEMO_COMPLETE", "Demo completed successfully");
        NLM_LOG_PERF("Total Demo Time", totalDuration);
        
        printDemoResults();
        
        // Save debug data if in debug mode
        if (currentDemo.mode == DemoMode::DEBUG_MODE) {
            saveDemoDebugData();
        }
    }
    
    void setupBasicLearningConfig() {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        
        // Expert settings
        config->set("enable_reward_modulation", true, ConfigSource::Expert);
        config->set("enable_structural_plasticity", true, ConfigSource::Expert);
        config->set("enable_development", true, ConfigSource::Expert);
        config->set("enable_curiosity", true, ConfigSource::Expert);
    }
    
    void setupAdvancedLearningConfig() {
        config->set("neuron_count", 1000);
        config->set("region_count", 3);
        config->set("connection_probability", 0.15f);
        config->set("random_seed", static_cast<int64_t>(123));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.015f);
        config->set("stdp_ltd_weight", 0.018f);
        
        // Advanced features
        config->set("enable_reward_modulation", true, ConfigSource::Expert);
        config->set("enable_structural_plasticity", true, ConfigSource::Expert);
        config->set("enable_development", true, ConfigSource::Expert);
        config->set("enable_curiosity", true, ConfigSource::Expert);
        config->set("enable_social_learning", true, ConfigSource::Expert);
        config->set("enable_text_teaching", true, ConfigSource::Expert);
        config->set("enable_replay", true, ConfigSource::Expert);
        config->set("enable_checkpointing", true, ConfigSource::Expert);
    }
    
    void setupTextTeachingConfig() {
        config->set("neuron_count", 800);
        config->set("region_count", 2);
        config->set("connection_probability", 0.12f);
        config->set("random_seed", static_cast<int64_t>(456));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.024f);
        
        // Text teaching specific settings
        config->set("enable_reward_modulation", false, ConfigSource::Expert); // Pure STDP
        config->set("enable_structural_plasticity", false, ConfigSource::Expert);
        config->set("enable_development", false, ConfigSource::Expert);
        config->set("enable_curiosity", false, ConfigSource::Expert);
        config->set("enable_text_teaching", true, ConfigSource::Expert);
    }
    
    void setupSocialLearningConfig() {
        config->set("neuron_count", 1200);
        config->set("region_count", 4);
        config->set("connection_probability", 0.18f);
        config->set("random_seed", static_cast<int64_t>(789));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.025f);
        config->set("stdp_ltd_weight", 0.03f);
        
        // Social learning features
        config->set("enable_reward_modulation", true, ConfigSource::Expert);
        config->set("enable_structural_plasticity", true, ConfigSource::Expert);
        config->set("enable_development", true, ConfigSource::Expert);
        config->set("enable_curiosity", true, ConfigSource::Expert);
        config->set("enable_social_learning", true, ConfigSource::Expert);
        config->set("enable_imitation", true, ConfigSource::Expert);
        config->set("enable_observation_learning", true, ConfigSource::Expert);
    }
    
    void setupDevelopmentalConfig() {
        config->set("neuron_count", 1500);
        config->set("region_count", 3);
        config->set("connection_probability", 0.2f);
        config->set("random_seed", static_cast<int64_t>(999));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.036f);
        
        // Developmental settings
        config->set("enable_reward_modulation", true, ConfigSource::Expert);
        config->set("enable_structural_plasticity", true, ConfigSource::Expert);
        config->set("enable_development", true, ConfigSource::Expert);
        config->set("enable_curiosity", true, ConfigSource::Expert);
        config->set("enable_replay", true, ConfigSource::Expert);
        config->set("enable_checkpointing", true, ConfigSource::Expert);
        config->set("critical_period_enabled", true, ConfigSource::Expert);
    }
    
    void setupPerformanceTestConfig() {
        config->set("neuron_count", 2000);
        config->set("region_count", 5);
        config->set("connection_probability", 0.25f);
        config->set("random_seed", static_cast<int64_t>(111));
        config->set("simulation_timestep", 0.0005); // Faster simulation
        config->set("stdp_ltp_weight", 0.04f);
        config->set("stdp_ltd_weight", 0.048f);
        
        // Performance features
        config->set("enable_reward_modulation", true, ConfigSource::Expert);
        config->set("enable_structural_plasticity", true, ConfigSource::Expert);
        config->set("enable_development", true, ConfigSource::Expert);
        config->set("enable_curiosity", true, ConfigSource::Expert);
        config->set("enable_social_learning", true, ConfigSource::Expert);
        config->set("enable_text_teaching", true, ConfigSource::Expert);
        config->set("enable_replay", true, ConfigSource::Expert);
        config->set("enable_checkpointing", true, ConfigSource::Expert);
        config->set("enable_performance_profiling", true, ConfigSource::Expert);
        config->set("enable_multithreading", true, ConfigSource::Expert);
        config->set("thread_count", 8, ConfigSource::Expert);
    }
    
    void setupDebugModeConfig() {
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.5f); // Dense connectivity for debugging
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.05f);
        config->set("stdp_ltd_weight", 0.06f);
        
        // Debug features
        config->set("enable_reward_modulation", false, ConfigSource::Expert);
        config->set("enable_structural_plasticity", false, ConfigSource::Expert);
        config->set("enable_development", false, ConfigSource::Expert);
        config->set("enable_curiosity", false, ConfigSource::Expert);
        config->set("enable_debug_output", true, ConfigSource::Expert);
        config->set("enable_step_tracing", true, ConfigSource::Expert);
        config->set("enable_neuron_activity_logging", true, ConfigSource::Expert);
        config->set("enable_performance_profiling", true, ConfigSource::Expert);
        config->set("simulation_timestep", 0.01, ConfigSource::Expert); // Slower for debugging
    }
    
    void setupCustomConfig(const std::map<std::string, std::string>& parameters) {
        // Set defaults
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        
        // Apply custom parameters
        for (const auto& param : parameters) {
            if (param.first == "neuron_count") {
                config->set("neuron_count", std::stoi(param.second));
            } else if (param.first == "region_count") {
                config->set("region_count", std::stoi(param.second));
            } else if (param.first == "connection_probability") {
                config->set("connection_probability", std::stof(param.second));
            } else if (param.first == "simulation_timestep") {
                config->set("simulation_timestep", std::stof(param.second));
            }
        }
    }
    
    void logNeuronActivity(uint64_t step) {
        // Log detailed neuron information
        auto neurons = brain->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            auto* neuron = neurons[i];
            if (neuron) {
                double firingRate = neuron->getFiringRate();
                double voltage = neuron->getMembranePotential();
                NLM_LOG_NEURON(step, neuron->getId().index(), firingRate, voltage);
            }
        }
    }
    
    void printDemoResults() {
        NLM_LOG_DEBUG("=== DEMO RESULTS ===");
        NLM_LOG_DEBUG("Demo: " + currentDemo.name);
        NLM_LOG_DEBUG("Mode: " + std::to_string(static_cast<int>(currentDemo.mode)));
        NLM_LOG_DEBUG("Steps completed: " + std::to_string(currentDemo.steps));
        NLM_LOG_DEBUG("Final firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_DEBUG("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_DEBUG("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_DEBUG("Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        
        if (agent) {
            NLM_LOG_DEBUG("Neuromodulation level: " + std::to_string(agent->getNeuromodulationLevel()));
            NLM_LOG_DEBUG("Curiosity level: " + std::to_string(agent->getCuriosityLevel()));
            NLM_LOG_DEBUG("Prediction error: " + std::to_string(agent->getPredictionError()));
        }
        
        if (world) {
            const auto& body = world->getAgentBody();
            NLM_LOG_DEBUG("Agent position: (" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")");
            NLM_LOG_DEBUG("Agent energy: " + std::to_string(body.energy));
            NLM_LOG_DEBUG("Agent age: " + std::to_string(body.age));
        }
    }
    
    void saveDemoDebugData() {
        // Save debug data for analysis
        std::string filename = "demo_debug_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".txt";
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Advanced Demo Debug Data" << std::endl;
            file << "=====================" << std::endl;
            file << "Demo: " << currentDemo.name << std::endl;
            file << "Time: " << getTimestamp() << std::endl;
            
            // Save configuration
            file << "\nConfiguration:" << std::endl;
            file << "Neurons: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
            file << "Regions: " << config->getOr<int64_t>("region_count", 2) << std::endl;
            file << "Steps: " << currentDemo.steps << std::endl;
            
            file.close();
            NLM_LOG_DEBUG("Debug data saved to: " + filename);
        }
    }
    
    std::string getActionName(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            default: return "Wait";
        }
    }
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void printDemoMenu() {
        std::cout << "=== ADVANCED DEMO SYSTEM ===" << std::endl;
        std::cout << "Available demos:" << std::endl;
        
        for (size_t i = 0; i < availableDemos.size(); ++i) {
            std::cout << i + 1 << ". " << availableDemos[i].name << std::endl;
            std::cout << "   " << availableDemos[i].description << std::endl;
            std::cout << "   Steps: " << availableDemos[i].steps << std::endl;
            std::cout << "   Features: " << (availableDemos[i].enableFeatures ? "Enabled" : "Disabled") << std::endl;
        }
        
        std::cout << availableDemos.size() + 1 << ". Custom demo" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    void runInteractiveMode() {
        int choice;
        while (true) {
            printDemoMenu();
            std::cin >> choice;
            
            if (choice == 0) {
                break;
            } else if (choice >= 1 && choice <= (int)availableDemos.size()) {
                DemoSettings demo = availableDemos[choice - 1];
                setupDemo(demo);
                runDemo();
            } else if (choice == (int)availableDemos.size() + 1) {
                runCustomDemo();
            } else {
                std::cout << "Invalid choice! Please try again." << std::endl;
            }
        }
    }
    
    void runCustomDemo() {
        std::cout << "Enter custom demo parameters:" << std::endl;
        std::cout << "Steps: ";
        uint64_t steps;
        std::cin >> steps;
        
        DemoSettings demo;
        demo.mode = DemoMode::CUSTOM;
        demo.name = "Custom Demo";
        demo.description = "User-defined demo";
        demo.steps = steps;
        demo.enableFeatures = true;
        
        std::cout << "Enter custom parameters (format: key=value):" << std::endl;
        std::string line;
        std::getline(std::cin, line); // Clear buffer
        while (std::getline(std::cin, line) && line != "") {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                demo.customParameters[key] = value;
            }
        }
        
        setupDemo(demo);
        runDemo();
    }
}; // class AdvancedDemoSystem

// Global advanced demo system instance
extern AdvancedDemoSystem globalAdvancedDemoSystem;

// Convenience functions for advanced demo usage
void runAdvancedDemo(const std::string& modeName, uint64_t steps = 1000);
void runPerformanceBenchmark();
void runDevelopmentStageDemo();

} // namespace advanced
} // namespace nlm

#endif // NLM_ADVANCED_DEMOS_HPP