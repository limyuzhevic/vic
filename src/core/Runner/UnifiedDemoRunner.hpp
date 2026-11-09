#pragma once

#include "../core/CLI/CLIFramework.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>

namespace nlm {

class UnifiedDemoRunner {
public:
    UnifiedDemoRunner() = default;
    ~UnifiedDemoRunner() = default;
    
    // Run Phase 2 demo (basic neural computation)
    static int runPhase2Demo(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        std::cout << "=== NLM Phase 2: Advanced Neural Computation ===\n";
        
        // Setup brain configuration
        auto cfg = std::make_shared<Config>();
        if (nlmConfig) {
            // Copy settings
            for (const auto& key : nlmConfig->getKeys()) {
                cfg->set(key, nlmConfig->get(key).value_or(ConfigValue{}));
            }
        }
        
        // Apply Phase 2 specific settings
        cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
        cfg->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        cfg->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return 1;
        }
        
        // Log brain status
        std::cout << "Brain initialized successfully:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        
        // Performance monitoring
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run basic connectivity test
        std::cout << "\n--- Test 1: Basic Neural Connectivity ---\n";
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                std::cout << "  No neurons found!\n";
            } else {
                size_t initialSpikes = brain->getTotalSpikeCount();
                
                // Inject current into first 10 neurons
                std::cout << "  Injecting current into 10 neurons...\n";
                for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(50.0f);
                }
                
                // Run simulation
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
                std::cout << "  Spikes generated: " << spikes << "\n";
                
                if (spikes > 0) {
                    std::cout << "  ✓ Spikes propagate through network\n";
                }
            }
        }
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Run plasticity experiment
        std::cout << "\n--- Test 2: Plasticity Learning Experiment ---\n";
        
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
            
            std::cout << "Applying repeated input patterns (1000 steps)...\n";
            
            // Get sensory neurons for pattern input
            std::vector<NeuronId> patternNeurons;
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
            
            for (SimulationStep step = 0; step < 1000; ++step) {
                for (NeuronId id : patternNeurons) {
                    brain->injectCurrent(id, 30.0f);
                }
                brain->step(step, step * 0.001);
                
                if (step % 200 == 0) {
                    std::cout << "  Step " << step << ": "
                              << "Spikes: " << brain->getTotalSpikeCount() << ", "
                              << "Firing: " << brain->getFiringNeuronCount() << "\n";
                }
            }
            
            // Calculate plasticity results
            size_t strengthened = 0, weakened = 0, unchanged = 0;
            for (size_t i = 0; i < std::min<size_t>(10, region->getSynapses().size()); ++i) {
                float weight = region->getSynapses()[i]->getWeight();
                if (weight > 0.5f) strengthened++;
                else if (weight < 0.3f) weakened++;
                else unchanged++;
            }
            
            std::cout << "\nPlasticity Results:\n";
            std::cout << "  Strengthened: " << strengthened << "\n";
            std::cout << "  Weakened: " << weakened << "\n";
            std::cout << "  Unchanged: " << unchanged << "\n";
            std::cout << "  Total spikes generated: " << brain->getTotalSpikeCount() << "\n";
        }
        
        // Run STDP verification
        std::cout << "\n--- Test 3: STDP Verification ---\n";
        
        if (region && !region->getSynapses().empty()) {
            Synapse* syn = region->getSynapses()[0].get();
            syn->enablePlasticity(false, true, false);
            float beforeWeight = syn->getWeight();
            
            std::cout << "Testing STDP on synapse with weight: " << beforeWeight << "\n";
            
            for (int trial = 0; trial < 50; ++trial) {
                brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float afterWeight = syn->getWeight();
            float delta = afterWeight - beforeWeight;
            
            std::cout << "After correlated activity:\n";
            std::cout << "  Weight: " << afterWeight << " (Δ=" << delta << ")\n";
            
            if (delta > 0.001f) {
                std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation\n";
            } else if (delta < -0.001f) {
                std::cout << "  ! STDP reversed: Check parameters\n";
            } else {
                std::cout << "  ! No change: STDP may not be triggering\n";
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Phase 2 Complete ===\n";
        std::cout << "Simulation time: " << elapsed << " seconds\n";
        std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << "\n";
        
        return 0;
    }
    
    // Run Phase 3 demo (world interaction)
    static int runPhase3Demo(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        std::cout << "=== NLM Phase 3: World Interaction Demo ===\n";
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        if (nlmConfig) {
            for (const auto& key : nlmConfig->getKeys()) {
                cfg->set(key, nlmConfig->get(key).value_or(ConfigValue{}));
            }
        }
        
        // Configure for Phase 3
        cfg->set("neuron_count", 500);
        cfg->set("region_count", 2);
        cfg->set("connection_probability", 0.1f);
        cfg->set("random_seed", static_cast<int64_t>(42));
        cfg->set("simulation_timestep", 0.001);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("stdp_tau", 20.0f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Create brain and agent
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agent.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agent.getMotorOutputSize() << "\n\n";
        
        // Run demo
        return runPhase3DemoInternal(config, brain, agent, world, 5, 200);
    }
    
    // Run Phase 3 demo internal implementation
    static int runPhase3DemoInternal(const CLIConfig& config, const std::shared_ptr<Brain>& brain,
                                    AgentBrain& agent, SimpleWorld& world,
                                    int numEpisodes, int stepsPerEpisode) {
        int totalReward = 0;
        int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
        std::set<std::pair<int, int>> visitedCells;
        int tooSoonCount = 0;
        int oscillations = 0;
        MotorCommand prevMove = MotorCommand::Wait;
        bool hasPrevMove = false;
        uint64_t brainStepCounter = 0;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            world.reset();
            agent.reset();
            brain->reset();
            brain->initialize();
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agent.processSensoryInput(percept);
                
                // 3. Simulate brain
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(brainStepCounter++);
                }
                
                // 4. Decode motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // 5. Apply action to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agent.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.1);
                
                // 8. Update development
                agent.updateDevelopment(0.1);
                
                // Track metrics
                totalReward += result.reward;
                if (result.message == "too soon") ++tooSoonCount;
                if (cmd == MotorCommand::Interact) {
                    if (result.message == "consumed_resource") ++resourcesConsumed;
                    else if (result.message == "hit_hazard") ++hazardsHit;
                    else ++emptyInteracts;
                }
                if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                    if (hasPrevMove && prevMove != cmd) ++oscillations;
                    prevMove = cmd;
                    hasPrevMove = true;
                }
                
                // Record visited cells
                const auto& body = world.getAgentBody();
                visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                    static_cast<int>(std::floor(body.y))});
                
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agent.getNeuromodulationLevel()
                              << ", novelty=" << agent.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << "\n";
                }
            }
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Total Reward: " << totalReward << "\n";
            std::cout << "  Resources consumed: " << resourcesConsumed << "\n";
            std::cout << "  Hazards hit: " << hazardsHit << "\n";
            std::cout << "  Empty interacts: " << emptyInteracts << "\n";
            std::cout << "  Cells visited: " << visitedCells.size() << "\n";
        }
        
        std::cout << "\n=== Phase 3 Demo Complete ===\n";
        std::cout << "Total Reward: " << totalReward << "\n";
        std::cout << "Resources consumed: " << resourcesConsumed << "\n";
        std::cout << "Hazards hit: " << hazardsHit << "\n";
        std::cout << "Cells visited: " << visitedCells.size() << "\n";
        
        return 0;
    }
    
    // Helper function for motor command to string conversion
    static const char* motorCommandToString(MotorCommand cmd) {
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
    
    // Run Phase 6 integration demo
    static int runPhase6Demo(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        std::cout << "=== NLM Phase 6: Integration Demo ===\n";
        
        // Create minimal configuration for Phase 6
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        cfg->set("connection_probability", 0.1f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Test basic integration
        std::cout << "Testing brain integration...\n";
        
        // Test 1: Check memory systems
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        
        std::cout << "Memory Systems:\n";
        std::cout << "  Working Memory: " << (workingMemory ? "Available" : "Not Available") << "\n";
        std::cout << "  Episodic Memory: " << (episodicMemory ? "Available" : "Not Available") << "\n";
        
        std::cout << "Neuromodulation Systems:\n";
        std::cout << "  Dopamine: " << (dopamine ? "Available" : "Not Available") << "\n";
        std::cout << "  Curiosity: " << (curiosity ? "Available" : "Not Available") << "\n";
        std::cout << "  Novelty: " << (novelty ? "Available" : "Not Available") << "\n";
        
        // Run some simulation steps
        std::cout << "Running simulation for " << config.maxSteps << " steps...\n";
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 1000 == 0) {
                std::cout << "  Step " << step << ": "
                          << "Spikes: " << brain->getTotalSpikeCount() << ", "
                          << "Firing: " << brain->getFiringNeuronCount() << "\n";
            }
        }
        
        std::cout << "\n=== Phase 6 Integration Results ===\n";
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << "\n";
        
        if (workingMemory) {
            std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << "\n";
        }
        if (episodicMemory) {
            std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << "\n";
        }
        
        return 0;
    }
    
    // Run all demos with unified framework
    static int runAllDemos(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        std::cout << "=== Running All NLM Demos ===\n";
        
        int result = 0;
        
        std::cout << "1. Running Phase 2 Demo...\n";
        result = runPhase2Demo(config, nlmConfig);
        if (result != 0) return result;
        
        std::cout << "2. Running Phase 3 Demo...\n";
        result = runPhase3Demo(config, nlmConfig);
        if (result != 0) return result;
        
        std::cout << "3. Running Phase 6 Integration Demo...\n";
        result = runPhase6Demo(config, nlmConfig);
        if (result != 0) return result;
        
        std::cout << "\n=== All Demos Completed Successfully ===\n";
        return 0;
    }
};

} // namespace nlm