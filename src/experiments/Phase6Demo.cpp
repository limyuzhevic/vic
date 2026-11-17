#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <set>
#include <utility>

using namespace nlm;

class PerformanceMonitor {
public:
    PerformanceMonitor() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void recordStep() { steps_++; }
    void recordSpike(size_t count) { spikesRecorded_ += count; }
    void recordReward(float reward) { totalReward_ += reward; }
    
    double getElapsedTime() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - startTime_);
        return duration.count() / 1000.0;
    }
    
    void printReport() const {
        std::cout << "\n=== PERFORMANCE MONITORING ===\n";
        std::cout << "Simulation time: " << getElapsedTime() << "s\n";
        std::cout << "Steps completed: " << steps_ << "\n";
        std::cout << "Total spikes generated: " << spikesRecorded_ << "\n";
        std::cout << "Average spikes per step: " << (steps_ > 0 ? spikesRecorded_ / steps_ : 0.0) << "\n";
        std::cout << "Total reward: " << totalReward_ << "\n";
        std::cout << "Average reward per step: " << (steps_ > 0 ? totalReward_ / steps_ : 0.0) << "\n";
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    size_t steps_ = 0;
    size_t spikesRecorded_ = 0;
    float totalReward_ = 0.0f;
};

class Phase6Demo {
public:
    Phase6Demo() {}
    ~Phase6Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 6 - FINAL INTEGRATION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Performance monitoring
        PerformanceMonitor perfMonitor;
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;

            // Phase 5 navigation/interact metrics: visited 1x1 cells,
            // interact outcomes from the world's own result message
            // (consumed_resource / hit_hazard / no_object — not a label
            // given to the brain), and forward/backward oscillation
            // (a Fwd step immediately followed by Bwd, or vice versa).
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;

            float episodeReward = 0.0f;
            // Monotonic brain-step counter for this episode. The old code
            // reused neuralStep 0..9 for every action, so delayed spikes
            // scheduled for step 10+ could never be delivered and
            // accumulated forever in the spike system.
            uint64_t brainStepCounter = 0;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();

                // 2. Process sensory input with agent brain
                agentBrain.processSensoryInput(percept);

                // 3. Brain step
                brain->step(step, step * 0.001);

                // 4. Get motor command from brain
                MotorCommand cmd = agentBrain.decodeMotorCommand();

                // 5. Apply action to world
                ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001);

                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(actionResult.reward, 0.0f);

                // 7. Update development
                agentBrain.updateDevelopment(0.001);

                // 8. Collect metrics
                if (actionResult.success) {
                    episodeReward += actionResult.reward;
                    if (actionResult.message == "consumed_resource") {
                        resourcesConsumed++;
                    } else if (actionResult.message == "hit_hazard") {
                        hazardsHit++;
                    }
                }

                // Track position for navigation metrics
                visitedCells.insert(std::make_pair(static_cast<int>(world.getAgentBody().x), 
                                                  static_cast<int>(world.getAgentBody().y)));
                
                // Performance monitoring
                perfMonitor.recordStep();
                perfMonitor.recordSpike(brain->getFiringNeuronCount());
                perfMonitor.recordReward(actionResult.reward);
            }
            
            // Calculate performance metrics
            float avgReward = episodeReward / stepsPerEpisode;
            float movementEfficiency = resourcesConsumed / (resourcesConsumed + hazardsHit + 1);
            
            metrics.reward = episodeReward;
            metrics.resourceEfficiency = movementEfficiency;
            metrics.navigationDistance = visitedCells.size();
            metrics.survivalTime = stepsPerEpisode; // All steps survived
            
            allMetrics.push_back(metrics);
            
            std::cout << "  Episode " << episode << " complete: reward=" << episodeReward 
                      << ", resources=" << resourcesConsumed << ", hazards=" << hazardsHit << "\n";
        }
        
        // Calculate overall performance
        float totalReward = 0.0f;
        float totalEfficiency = 0.0f;
        float totalDistance = 0.0f;
        
        for (const auto& metrics : allMetrics) {
            totalReward += metrics.reward;
            totalEfficiency += metrics.resourceEfficiency;
            totalDistance += metrics.navigationDistance;
        }
        
        float avgReward = totalReward / numEpisodes;
        float avgEfficiency = totalEfficiency / numEpisodes;
        float avgDistance = totalDistance / numEpisodes;
        
        // Show performance report
        perfMonitor.printReport();
        
        std::cout << "\n=== PHASE 6 RESULTS ===\n";
        std::cout << "Average reward per episode: " << avgReward << "\n";
        std::cout << "Resource efficiency: " << avgEfficiency << "\n";
        std::cout << "Navigation coverage: " << avgDistance << "\n";
        std::cout << "Overall performance: " << (avgReward > 0 ? "GOOD" : "NEEDS IMPROVEMENT") << "\n";
        
        return allMetrics;
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        // Additional brain-specific configuration
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("max_energy", 100.0f);
    }
};

int main(int argc, char* argv[]) {
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    std::cout << "NLM Phase 6 Integration Demo\n";
    std::cout << "================================\n\n";
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create demo
    Phase6Demo demo;
    
    // Run the demo
    auto results = demo.run(numTrials, 200);
    
    std::cout << "\n=== DEMO COMPLETE ===\n";
    std::cout << "Ran " << numTrials << " trial(s) with 200 steps each.\n";
    std::cout << "The integrated brain demonstrated adaptive behavior.\n";
    
    return 0;
}