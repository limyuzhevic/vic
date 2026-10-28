#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Error/Error.hpp"

// Simple demonstration of advanced C++ API usage
class NLMAdvancedCppExample {
public:
    NLMAdvancedCppExample() : brain(nullptr), world(nullptr), agent(nullptr) {}
    
    ~NLMAdvancedCppExample() {
        cleanup();
    }
    
    void setupLogger() {
        // Initialize logger
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
    }
    
    void setupAdvancedConfiguration() {
        std::cout << "=== Setting Up Advanced Configuration (C++) ===" << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<nlm::Config>();
        
        // Advanced brain configuration
        cfg->set("brain.neuron_count", static_cast<int64_t>(1000));
        cfg->set("brain.synapse_density", 0.15f);
        cfg->set("brain.connection_probability", 0.1f);
        cfg->set("brain.initial_weight_mean", 0.5f);
        cfg->set("brain.initial_weight_std", 0.1f);
        
        // Advanced plasticity configuration
        cfg->set("plasticity.stdp.enable", true);
        cfg->set("plasticity.stdp.learning_rate", 0.005f);
        cfg->set("plasticity.stdp.tau_plus", 20.0f);
        cfg->set("plasticity.stdp.tau_minus", 20.0f);
        cfg->set("plasticity.hebbian.enable", true);
        cfg->set("plasticity.hebbian.learning_rate", 0.01f);
        cfg->set("plasticity.structural.enable", true);
        cfg->set("plasticity.synaptogenesis_rate", 0.0001f);
        cfg->set("plasticity.pruning_rate", 0.00001f);
        
        // Advanced neuromodulation configuration
        cfg->set("neuromod.dopamine.scale", 2.0f);
        cfg->set("neuromod.curiosity.enable", true);
        cfg->set("neuromod.curiosity.threshold", 0.3f);
        cfg->set("neuromod.novelty.enable", true);
        cfg->set("neuromod.novelty.threshold", 0.2f);
        cfg->set("neuromod.prediction_error.enable", true);
        cfg->set("neuromod.prediction_error.gain", 1.5f);
        
        // Advanced memory configuration
        cfg->set("working_memory.capacity", 200);
        cfg->set("working_memory.decay_rate", 0.05f);
        cfg->set("episodic_memory.max_episodes", 5000);
        cfg->set("episodic_memory.replay_interval", 200);
        cfg->set("associative_memory.pattern_threshold", 0.7f);
        cfg->set("associative_memory.pattern_capacity", 1000);
        
        // Advanced cognitive configuration
        cfg->set("cognition.planning_depth", 8);
        cfg->set("cognition.attention.gain", 2.0f);
        cfg->set("cognition.attention.inhibition_strength", 0.6f);
        cfg->set("cognition.attention.excitation_strength", 2.0f);
        cfg->set("cognition.concept_threshold", 0.6f);
        
        // Advanced development configuration
        cfg->set("development.critical_period_start", 50);
        cfg->set("development.critical_period_end", 500);
        cfg->set("development.plasticity_modulation_rate", 0.02f);
        
        // Store configuration
        this->config = cfg;
        
        std::cout << "Advanced configuration setup completed" << std::endl;
        std::cout << "  Neurons: " << cfg->getOr<int64_t>("brain.neuron_count", 1000) << std::endl;
        std::cout << "  STDP enabled: " << (cfg->getOr<bool>("plasticity.stdp.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "  Hebbian enabled: " << (cfg->getOr<bool>("plasticity.hebbian.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "  Neuromodulation enabled: " << (cfg->getOr<bool>("neuromod.dopamine.scale", 0.0f) > 0.0f ? "Yes" : "No") << std::endl;
    }
    
    void createAdvancedBrain() {
        std::cout << "\n=== Creating Advanced Brain with Performance Monitoring ===" << std::endl;
        
        try {
            this->brain = std::make_shared<nlm::Brain>(this->config);
            
            if (!this->brain->initialize()) {
                auto error = std::make_shared<nlm::IntegrationException>("Failed to initialize advanced brain");
                nlm::ErrorHandler::getInstance().addError(error);
                throw *error;
            }
            
            // Enable performance monitoring
            this->brain->enablePerformanceMonitoring(true);
            
            // Set up advanced plasticity
            this->brain->setSTDPParameters(0.01f, 0.012f, 15.0f);
            this->brain->setHebbianParameters(0.005f, 0.001f);
            this->brain->setStructuralPlasticityParameters(0.0002f, 0.00005f);
            
            // Set up neuromodulation
            this->brain->setNeuromodulationParameters(2.0f, 0.2f, 0.3f, 2.0f);
            
            // Set up memory systems
            this->brain->setWorkingMemoryParameters(500, 0.02f);
            this->brain->setEpisodicMemoryParameters(3000, 1000);
            
            // Set up cognitive systems
            this->brain->setCognitiveParameters(10, 2.5f, 0.5f, 1.8f);
            
            // Set up development system
            this->brain->setDevelopmentParameters(100, 1000, 0.03f);
            
            // Enable all subsystems
            this->brain->enableSubsystem("working_memory", true);
            this->brain->enableSubsystem("episodic_memory", true);
            this->brain->enableSubsystem("associative_memory", true);
            this->brain->enableSubsystem("planner", true);
            this->brain->enableSubsystem("concept_formation", true);
            this->brain->enableSubsystem("attention", true);
            this->brain->enableSubsystem("dopamine", true);
            this->brain->enableSubsystem("curiosity", true);
            this->brain->enableSubsystem("novelty", true);
            this->brain->enableSubsystem("prediction_error", true);
            
            std::cout << "Advanced brain created successfully" << std::endl;
            std::cout << "  Neurons: " << this->brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Synapses: " << this->brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Performance monitoring: " << (this->brain->isPerformanceMonitoringEnabled() ? "Enabled" : "Disabled") << std::endl;
            
        } catch (const nlm::IntegrationException& e) {
            std::cerr << "Integration error: " << e.what() << std::endl;
            throw;
        } catch (const nlm::MemoryException& e) {
            std::cerr << "Memory error: " << e.what() << std::endl;
            throw;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            throw;
        }
    }
    
    void setupWorldAndAgent() {
        std::cout << "\n=== Setting Up World and Agent ===" << std::endl;
        
        try {
            // Create world
            this->world = std::make_shared<nlm::SimpleWorld>();
            this->world->configure(30, 30, 12, 12);
            this->world->reset();
            this->world->setAgentStart(15.0, 15.0);
            
            // Create agent
            this->agent = std::make_shared<nlm::AgentBrain>(this->brain);
            this->agent->initialize(*this->world);
            
            // Enable agent subsystems
            this->agent->enableRewardModulation(true);
            this->agent->enableStructuralPlasticity(true);
            this->agent->enableDevelopment(true);
            this->agent->enableCuriosity(true);
            
            std::cout << "World and agent setup completed" << std::endl;
            std::cout << "  World size: " << this->world->getWidth() << "x" << this->world->getHeight() << std::endl;
            std::cout << "  Vision range: " << this->world->getVisionWidth() << "x" << this->world->getVisionHeight() << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error setting up world and agent: " << e.what() << std::endl;
            throw;
        }
    }
    
    void runAdvancedSimulation(int max_steps = 2000) {
        std::cout << "\n=== Running Advanced C++ Simulation ===" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<double> firing_rates;
        std::vector<double> curiosity_levels;
        std::vector<double> rewards;
        
        for (int step = 0; step < max_steps; ++step) {
            // Update world
            this->world->update(0.1);
            
            // Get sensory input
            auto percept = this->world->getSensoryPercept();
            
            // Process sensory input
            this->agent->processSensoryInput(percept);
            
            // Run brain step
            this->brain->step(step);
            
            // Decode motor command
            auto motor_cmd = this->agent->decodeMotorCommand();
            
            // Apply motor command
            this->world->applyMotorCommand(motor_cmd, this->world->getSimulationTime());
            
            // Apply reward modulation
            double reward = 0.0;
            if (percept.getInternal() && !percept.getInternal()->empty()) {
                reward = percept.getInternal()->at(0);
            }
            this->agent->applyRewardModulation(reward, 0.0);
            
            // Update development
            this->agent->updateDevelopment(0.1);
            
            // Collect data
            firing_rates.push_back(this->brain->getFiringNeuronCount());
            curiosity_levels.push_back(this->agent->getCuriosityLevel());
            rewards.push_back(reward);
            
            // Log progress
            if (step % 200 == 0) {
                std::cout << "Step " << step << "/" << max_steps << std::endl;
                std::cout << "  Firing neurons: " << this->brain->getFiringNeuronCount() << std::endl;
                std::cout << "  Total spikes: " << this->brain->getTotalSpikeCount() << std::endl;
                std::cout << "  Curiosity: " << this->agent->getCuriosityLevel() << std::endl;
                std::cout << "  Reward: " << reward << std::endl;
                
                // Check for exploration triggers
                if (this->agent->getCuriosityLevel() > 0.7) {
                    std::cout << "  HIGH CURIOSITY - Triggering exploration behavior" << std::endl;
                }
                
                if (this->agent->getNoveltyLevel() > 0.7) {
                    std::cout << "  HIGH NOVELTY - Triggering novelty-seeking behavior" << std::endl;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nAdvanced C++ simulation completed in " << duration.count() << " ms" << std::endl;
        std::cout << "Average step time: " << duration.count() / (double)max_steps << " ms" << std::endl;
        
        // Calculate statistics
        double avg_firing = 0.0, avg_curiosity = 0.0, avg_reward = 0.0;
        double max_firing = 0.0, min_firing = std::numeric_limits<double>::max();
        
        for (size_t i = 0; i < firing_rates.size(); ++i) {
            avg_firing += firing_rates[i];
            avg_curiosity += curiosity_levels[i];
            avg_reward += rewards[i];
            max_firing = std::max(max_firing, firing_rates[i]);
            min_firing = std::min(min_firing, firing_rates[i]);
        }
        
        avg_firing /= firing_rates.size();
        avg_curiosity /= curiosity_levels.size();
        avg_reward /= rewards.size();
        
        // Print results
        std::cout << "\nC++ Simulation Results:" << std::endl;
        std::cout << "  Total steps: " << max_steps << std::endl;
        std::cout << "  Average firing rate: " << avg_firing << " Hz" << std::endl;
        std::cout << "  Max firing neurons: " << max_firing << std::endl;
        std::cout << "  Min firing neurons: " << min_firing << std::endl;
        std::cout << "  Average curiosity: " << avg_curiosity << std::endl;
        std::cout << "  Average reward: " << avg_reward << std::endl;
        std::cout << "  Total spikes: " << this->brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Network efficiency: " << this->brain->getTotalSpikeCount() / (max_steps * this->brain->getTotalNeuronCount()) * 100 << "%" << std::endl;
        
    }
    
    void exportResults(const std::string& output_dir = "./nlm_results_cpp") {
        std::cout << "\n=== Exporting C++ Results ===" << std::endl;
        
        // Create output directory
        std::system(("mkdir -p " + output_dir).c_str());
        
        // Export simulation data to CSV
        std::string csv_file = output_dir + "/simulation_data.csv";
        std::ofstream csv_out(csv_file);
        
        if (csv_out.is_open()) {
            csv_out << "Step,Time,FiringNeurons,TotalSpikes,Curiosity,Novelty,PredictionError,Reward" << std::endl;
            
            for (size_t i = 0; i < firing_rates.size(); ++i) {
                csv_out << i << "," 
                       << i * 0.1 << "," 
                       << firing_rates[i] << "," 
                       << (i == firing_rates.size() - 1 ? this->brain->getTotalSpikeCount() : 0) << "," 
                       << curiosity_levels[i] << "," 
                       << (this->agent->getNoveltyLevel()) << "," 
                       << (this->agent->getPredictionError()) << "," 
                       << rewards[i] << std::endl;
            }
            
            csv_out.close();
            std::cout << "  Simulation data exported to " << csv_file << std::endl;
        }
        
        // Export brain state
        std::string brain_file = output_dir + "/brain_state.checkpoint";
        if (this->brain->save(brain_file)) {
            std::cout << "  Brain state exported to " << brain_file << std::endl;
        } else {
            std::cout << "  Failed to export brain state" << std::endl;
        }
        
        // Export configuration
        std::string config_file = output_dir + "/configuration.txt";
        std::ofstream config_out(config_file);
        
        if (config_out.is_open()) {
            config_out << "NLM Advanced Configuration (C++)" << std::endl;
            config_out << "================================" << std::endl;
            config_out << "Brain neurons: " << this->brain->getTotalNeuronCount() << std::endl;
            config_out << "Brain synapses: " << this->brain->getTotalSynapseCount() << std::endl;
            config_out << "Developmental stage: " << static_cast<int>(this->brain->getDevelopmentalStage()) << std::endl;
            config_out << "Performance monitoring: " << (this->brain->isPerformanceMonitoringEnabled() ? "Enabled" : "Disabled") << std::endl;
            config_out << "Simulation steps: " << firing_rates.size() << std::endl;
            config_out << "Average firing rate: " << avg_firing << std::endl;
            
            config_out.close();
            std::cout << "  Configuration exported to " << config_file << std::endl;
        }
        
        std::cout << "\nC++ results exported to " << output_dir << std::endl;
    }
    
    void runAdvancedExample() {
        std::cout << "NLM Advanced C++ Example - Phase 6 Deep Dive" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        try {
            // Setup logging
            setupLogger();
            
            // Setup advanced configuration
            setupAdvancedConfiguration();
            
            // Create advanced brain
            createAdvancedBrain();
            
            // Setup world and agent
            setupWorldAndAgent();
            
            // Run advanced simulation
            runAdvancedSimulation(2000);
            
            // Export results
            exportResults();
            
            std::cout << "\n" << std::string(60, '=') << std::endl;
            std::cout << "Advanced C++ example completed successfully!" << std::endl;
            std::cout << "\nKey C++ achievements:" << std::endl;
            std::cout << "  ✓ Advanced configuration management" << std::endl;
            std::cout << "  ✓ Enhanced plasticity control" << std::endl;
            std::cout << "  ✓ Performance monitoring integration" << std::endl;
            std::cout << "  ✓ Agent-world interaction system" << std::endl;
            std::cout << "  ✓ Error handling and recovery" << std::endl;
            std::cout << "  ✓ Result export and documentation" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "\nError in advanced C++ example: " << e.what() << std::endl;
            return;
        }
    }
    
    void cleanup() {
        std::cout << "\nCleaning up resources..." << std::endl;
        this->brain.reset();
        this->agent.reset();
        this->world.reset();
        this->config.reset();
        std::cout << "Cleanup completed." << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::AgentBrain> agent;
    std::shared_ptr<nlm::SimpleWorld> world;
    std::vector<double> firing_rates;
    std::vector<double> curiosity_levels;
    std::vector<double> rewards;
    double avg_firing = 0.0;
};

int main() {
    std::cout << "NLM Advanced C++ Example - Phase 6 Features" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    try {
        NLMAdvancedCppExample example;
        example.runAdvancedExample();
        
        std::cout << "\nAdvanced C++ example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nFatal error: " << e.what() << std::endl;
        return 1;
    }
}
