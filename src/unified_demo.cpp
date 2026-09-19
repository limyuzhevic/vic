#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "environment/Environment.hpp"
#include "sensory/SensoryInput.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

// Phase 6: Unified Demo - Complete Brain Integration
// Demonstrates the full capabilities of the integrated artificial brain

namespace nlm {

class UnifiedDemo {
public:
    UnifiedDemo() : 
        simulationSteps_(0),
        totalSimTime_(0.0),
        demoPhase_(DemoPhase::Initialization),
        simulationRunning_(false) {}
    
    bool initialize() {
        try {
            std::cout << "=== NLM Phase 6: Unified Demo Application ===" << std::endl;
            std::cout << "Initializing complete integrated artificial brain..." << std::endl;
            
            // Create configuration
            config_ = std::make_shared<Config>();
            setupConfiguration();
            
            // Create brain
            brain_ = std::make_unique<Brain>(config_);
            
            // Initialize brain with all integrated systems
            if (!brain_->initialize()) {
                throw std::runtime_error("Brain initialization failed");
            }
            
            // Initialize environment
            environment_ = std::make_unique<Environment>();
            environment_->initialize(brain_.get());
            
            // Initialize visualization (optional)
            visualization_ = std::make_unique<VisualizationInterface>();
            visualization_->initialize(brain_.get());
            
            demoPhase_ = DemoPhase::Running;
            simulationRunning_ = true;
            
            std::cout << "✓ Brain initialized with all integrated systems" << std::endl;
            std::cout << "✓ Environment configured" << std::endl;
            std::cout << "✓ Visualization system ready" << std::endl;
            std::cout << "\n=== Demo Started ===" << std::endl;
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during initialization: " << e.what() << std::endl;
            return false;
        }
    }
    
    void setupConfiguration() {
        // Enhanced configuration for demo
        config_->set("random_seed", 42);
        config_->set("simulation_timestep", 0.001);
        
        // Larger brain for demonstration
        config_->set("neuron_count", 2000);
        config_->set("region_count", 4);
        config_->set("connection_probability", 0.12);
        
        // Memory system settings
        config_->set("working_memory_capacity", 500);
        config_->set("episodic_memory_max_episodes", 2000);
        
        // Plasticity settings
        config_->set("plasticity_learning_rate", 0.015);
        config_->set("stdp_ltp_weight", 0.012);
        config_->set("stdp_ltd_weight", 0.013);
        
        // Neuromodulation settings
        config_->set("dopamine_baseline", 0.1);
        config_->set("curiosity_novelty_weight", 0.6);
        config_->set("curiosity_prediction_error_weight", 0.4);
        
        // Development settings
        config_->set("synaptogenesis_rate", 0.0005);
        config_->set("pruning_rate", 0.00005);
        
        // Simulation settings
        config_->set("max_simulation_steps", 5000);
        config_->set("visualization_enabled", true);
        config_->set("checkpoint_dir", "./demo_checkpoints");
        config_->set("checkpoint_interval", 1000);
        
        std::cout << "✓ Configuration loaded" << std::endl;
    }
    
    void run() {
        if (!simulationRunning_) {
            std::cout << "Demo not initialized or already completed" << std::endl;
            return;
        }
        
        std::cout << "Starting simulation loop..." << std::endl;
        std::cout << "Press Ctrl+C to stop early\n" << std::endl;
        
        try {
            while (simulationRunning_ && simulationSteps_ < config_->getOr<size_t>("max_simulation_steps", 5000)) {
                // Update environment
                environment_->update(0.001); // simulation_timestep
                
                // Get sensory input from environment
                SensoryInput sensoryInput = environment_->getObservation();
                
                // Process sensory input
                if (!sensoryInput.getData().empty()) {
                    brain_->receiveSensoryInput(sensoryInput);
                    std::cout << "[Step " << simulationSteps_ << "] " 
                             << "Received sensory input: " 
                             << formatSensoryInput(sensoryInput) << std::endl;
                }
                
                // Main brain computation step
                brain_->step(simulationSteps_, totalSimTime_);
                simulationSteps_++;
                totalSimTime_ += 0.001;
                
                // Generate and apply action
                auto action = brain_->produceAction();
                environment_->applyAction(action);
                
                // Update visualization
                if (visualization_) {
                    visualization_->update(simulationSteps_);
                }
                
                // Log progress periodically
                if (simulationSteps_ % 100 == 0) {
                    logProgress();
                }
                
                // Check for demo completion conditions
                checkDemoConditions();
                
                // Small delay for demo readability
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error during simulation: " << e.what() << std::endl;
            simulationRunning_ = false;
        }
        
        // Final status
        completeDemo();
    }
    
    void logProgress() {
        std::cout << "\n=== Progress Report (Step " << simulationSteps_ << ") ===" << std::endl;
        
        // Brain statistics
        std::cout << "Neural Computation:" << std::endl;
        std::cout << "  Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  E/I ratio: " << std::fixed << std::setprecision(2) 
                  << brain_->getExcitationInhibitionRatio() << std::endl;
        
        // Memory systems
        std::cout << "\nMemory Systems:" << std::endl;
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            std::cout << "  Working memory: " << wm->getActiveTraces() << " traces, " 
                      << wm->getMemoryActivity() << " avg activation" << std::endl;
        }
        
        auto* em = brain_->getEpisodicMemory();
        if (em) {
            std::cout << "  Episodic memory: " << em->getEpisodeCount() << " episodes" << std::endl;
        }
        
        auto* am = brain_->getAssociativeMemory();
        if (am) {
            // No direct count method, show it's available
            std::cout << "  Associative memory: Active" << std::endl;
        }
        
        // Neuromodulation
        std::cout << "\nNeuromodulation:" << std::endl;
        auto* dm = brain_->getDopamine();
        if (dm) {
            std::cout << "  Dopamine level: " << std::fixed << std::setprecision(3) 
                      << dm->getLevel() << std::endl;
        }
        
        auto* cm = brain_->getCuriosity();
        if (cm) {
            std::cout << "  Curiosity level: " << std::fixed << std::setprecision(3) 
                      << cm->getExplorationDrive() << std::endl;
        }
        
        auto* nm = brain_->getNovelty();
        if (nm) {
            std::cout << "  Novelty level: " << std::fixed << std::setprecision(3) 
                      << nm->getLevel() << std::endl;
        }
        
        auto* pem = brain_->getPredictionErrorSignal();
        if (pem) {
            std::cout << "  Prediction error: " << std::fixed << std::setprecision(3) 
                      << pem->getLevel() << std::endl;
        }
        
        // Environment state
        std::cout << "\nEnvironment State:" << std::endl;
        auto envState = environment_->getStateDescription();
        std::cout << "  " << envState << std::endl;
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
    }
    
    void checkDemoConditions() {
        // Learning phase: Check if brain is actively learning
        if (simulationSteps_ < 500) {
            demoPhase_ = DemoPhase::Learning;
            std::cout << "[Demo Phase: Learning] " << "Brain exploring environment..." << std::endl;
        }
        
        // Consolidation phase: Memory consolidation
        else if (simulationSteps_ < 2000) {
            demoPhase_ = DemoPhase::Consolidation;
            // Trigger episodic memory consolidation
            auto* em = brain_->getEpisodicMemory();
            if (em) {
                em->consolidate(0.5f);
            }
            std::cout << "[Demo Phase: Consolidation] " << "Memory consolidation in progress..." << std::endl;
        }
        
        // Optimization phase: Fine-tuning and adaptation
        else if (simulationSteps_ < 4000) {
            demoPhase_ = DemoPhase::Optimization;
            std::cout << "[Demo Phase: Optimization] " << "Brain optimizing behavior..." << std::endl;
        }
        
        // Stabilization phase: Mature behavior
        else {
            demoPhase_ = DemoPhase::Stabilization;
            std::cout << "[Demo Phase: Stabilization] " << "Brain behavior stabilized..." << std::endl;
        }
    }
    
    void completeDemo() {
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "Simulation finished after " << simulationSteps_ << " steps" << std::endl;
        std::cout << "Total simulated time: " << std::fixed << std::setprecision(3) 
                  << totalSimTime_ << " seconds" << std::endl;
        
        // Final statistics
        std::cout << "\n=== Final Brain State ===" << std::endl;
        brain_->logStatus();
        
        // Save checkpoint
        std::string checkpointFile = "./final_demo_checkpoint";
        if (brain_->save(checkpointFile)) {
            std::cout << "✓ Final brain state saved to: " << checkpointFile << std::endl;
        } else {
            std::cout << "✗ Failed to save final checkpoint" << std::endl;
        }
        
        simulationRunning_ = false;
    }
    
    void stop() {
        std::cout << "\nStopping demo..." << std::endl;
        simulationRunning_ = false;
    }
    
    std::string formatSensoryInput(const SensoryInput& input) const {
        const auto& data = input.getData();
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            oss << std::fixed << std::setprecision(3) << data[i];
            if (i < data.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }
    
    bool isRunning() const { return simulationRunning_; }
    
private:
    enum class DemoPhase {
        Initialization,
        Learning,
        Consolidation,
        Optimization,
        Stabilization,
        Completed
    };
    
    std::shared_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<Environment> environment_;
    std::unique_ptr<VisualizationInterface> visualization_;
    
    size_t simulationSteps_;
    double totalSimTime_;
    DemoPhase demoPhase_;
    bool simulationRunning_;
};

// Main demo entry point
int main() {
    try {
        std::cout << "NLM Unified Demo Application" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << "This demo showcases the complete integration of all" << std::endl;
        std::cout << "brain systems: neural computation, memory, prediction," << std::endl;
        std::cout << "cognition, neuromodulation, and development." << std::endl;
        std::cout << "" << std::endl;
        
        // Create and initialize demo
        UnifiedDemo demo;
        
        if (!demo.initialize()) {
            std::cerr << "Failed to initialize demo" << std::endl;
            return 1;
        }
        
        // Run demo
        demo.run();
        
        std::cout << "\nDemo completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

} // namespace nlm

// Entry point for the demo executable
int main() {
    return nlm::main();
}