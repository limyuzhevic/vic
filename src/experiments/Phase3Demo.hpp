#include "Phase3Demo.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

Phase3Demo::Phase3Demo() {}

Phase3Demo::~Phase3Demo() {}

void Phase3Demo::run(int numEpisodes, int stepsPerEpisode) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
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
    
    // Configure neurons with population parameters
    configureNeurons(brain, config);
    
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
    
    // Metrics tracking
    std::vector<ExperimentMetrics> allMetrics;
    
    // Run episodes
    for (int episode = 0; episode < numEpisodes; ++episode) {
        std::cout << "--- Episode " << episode << " started ---\n";
        
        // Reset world
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        // Re-configure neurons after reset if needed
        if (config->getOr<bool>("enable_development", true)) {
            configureNeurons(brain, config);
        }
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // 2. Inject sensory input into brain
            agentBrain.processSensoryInput(percept);
            
            // 3. Simulate brain (multiple steps per action)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep);
            }
            
            // 4. Decode motor command from brain activity
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // 5. Apply motor command to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // 6. Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // 7. Update world
            world.update(0.01);  // 10ms timestep
            
            // 8. Update development
            agentBrain.updateDevelopment(0.01);
            
            // Track metrics
            episodeReward += result.reward;
            metrics.steps++;
            metrics.totalReward += result.reward;
            metrics.totalSpikes += brain->getFiringNeuronCount();
            
            // Count actions
            switch (cmd) {
                case MotorCommand::MoveForward: metrics.forwardCount++; break;
                case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                case MotorCommand::Interact: metrics.interactCount++; break;
                default: metrics.waitCount++; break;
            }
            
            // Print periodic updates
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << motorCommandToString(cmd)
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << ", energy=" << world.getAgentBody().energy
                          << "\n";
            }
        }
        
        // Record final metrics
        metrics.finalNovelty = agentBrain.getNoveltyLevel();
        metrics.finalCuriosity = agentBrain.getCuriosityLevel();
        metrics.finalEnergy = world.getAgentBody().energy;
        metrics.predictionError = agentBrain.getPredictionError();
        metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
        metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
        metrics.totalSynapses = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        
        allMetrics.push_back(metrics);
        
        std::cout << "--- Episode " << episode << " ended ---\n";
        std::cout << "  Steps: " << metrics.steps << "\n";
        std::cout << "  Total Reward: " << metrics.totalReward << "\n";
        std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
        std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
        std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                  << ", bwd=" << metrics.backwardCount
                  << ", left=" << metrics.turnLeftCount
                  << ", right=" << metrics.turnRightCount
                  << ", interact=" << metrics.interactCount
                  << ", wait=" << metrics.waitCount
                  << "\n\n";
    }
    
    // Summary
    std::cout << "===========================================\n";
    std::cout << "SUMMARY\n";
    std::cout << "===========================================\n";
    
    if (allMetrics.size() >= 2) {
        const auto& first = allMetrics.front();
        const auto& last = allMetrics.back();
        
        std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
        std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
        std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
        std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
        
        // Calculate behavior change
        float rewardChange = last.totalReward - first.totalReward;
        float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
        
        std::cout << "\nBehavioral change:\n";
        if (rewardChange > 0) {
            std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
        } else if (rewardChange < -5) {
            std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
        }
        
        if (std::abs(actionChange) > 10) {
            std::cout << "  Forward actions changed by " << actionChange << "\n";
        }
        
        // Synapse change
        if (last.totalSynapses != first.totalSynapses) {
            std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
        }
    }
    
    std::cout << "\nPhase 3 demonstration complete.\n";
    std::cout << "The brain received sensory input, produced actions,\n";
    std::cout << "experienced consequences, and modified its synapses.\n";
}

void Phase3Demo::runWithAgent(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                             int numEpisodes, int stepsPerEpisode, bool enableDevelopment) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - AGENT TRAINING DEMO\n";
    std::cout << "===========================================\n\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    configureEnvironment(config);
    configureBrain(config);
    config->set("enable_development", enableDevelopment);
    
    // Create brain
    auto brain = agent->getBrain();
    
    std::cout << "Training agent with configuration:\n";
    std::cout << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 500) << "\n";
    std::cout << "  Episodes: " << numEpisodes << "\n";
    std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
    
    // Initialize world
    world->reset();
    agent->reset();
    brain->reset();
    
    // Metrics tracking
    std::vector<ExperimentMetrics> allMetrics;
    
    // Run episodes
    for (int episode = 0; episode < numEpisodes; ++episode) {
        std::cout << "--- Training Episode " << episode << " ---\n";
        
        // Reset environment
        world->reset();
        agent->reset();
        brain->reset();
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // Get sensory percept from world
            const SensoryPercept& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Simulate brain
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep);
            }
            
            // Decode motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world->update(0.01);
            
            // Update development
            agent->updateDevelopment(0.01);
            
            // Track metrics
            episodeReward += result.reward;
            metrics.steps++;
            metrics.totalReward += result.reward;
            metrics.totalSpikes += brain->getFiringNeuronCount();
        }
        
        // Record metrics
        metrics.finalNovelty = agent->getNoveltyLevel();
        metrics.finalCuriosity = agent->getCuriosityLevel();
        metrics.finalEnergy = world->getAgentBody().energy;
        metrics.predictionError = agent->getPredictionError();
        metrics.totalSynapses = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        
        allMetrics.push_back(metrics);
        
        std::cout << "  Episode " << episode << " completed: "
                  << metrics.totalReward << " total reward, "
                  << brain->getTotalSpikeCount() << " spikes, "
                  << metrics.finalEnergy << " energy\n";
    }
    
    // Training summary
    std::cout << "\n=== TRAINING SUMMARY ===\n";
    if (allMetrics.size() >= 2) {
        const auto& first = allMetrics.front();
        const auto& last = allMetrics.back();
        
        std::cout << "Performance improvement over training:\n";
        std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
        std::cout << "  Final energy: " << first.finalEnergy << " -> " << last.finalEnergy << "\n";
        std::cout << "  Novelty level: " << first.finalNovelty << " -> " << last.finalNovelty << "\n";
        
        float rewardImprovement = last.totalReward - first.totalReward;
        if (rewardImprovement > 0) {
            std::cout << "  ✓ Positive learning observed (reward increased)\n";
        } else if (rewardImprovement < -10) {
            std::cout << "  ! Significant reward decrease (possible exploration or extinction)\n";
        }
    }
    
    std::cout << "\nAgent training complete.\n";
    std::cout << "The agent learned through interaction with the environment.\n";
}

void Phase3Demo::configureEnvironment(std::shared_ptr<Config> config) {
    config->set("environment_width", 20);
    config->set("environment_height", 20);
    config->set("vision_width", 16);
    config->set("vision_height", 16);
}

void Phase3Demo::configureBrain(std::shared_ptr<Config> config) {
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("enable_development", true);
}

void Phase3Demo::configureNeurons(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    // Get neuron configuration
    float restingPotential = config->getOr<float>("resting_potential", -70.0f);
    float threshold = config->getOr<float>("threshold", -55.0f);
    float leakConductance = config->getOr<float>("leak_conductance", 10.0f);
    uint32_t refractoryPeriod = static_cast<uint32_t>(config->getOr<int64_t>("refractory_period", 5));
    float resetPotential = config->getOr<float>("reset_potential", -70.0f);
    NeuronType neuronType = NeuronType::Internal;
    
    // Get all regions and configure neurons
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        std::vector<Neuron*> neuronPtrs;
        
        for (auto& neuron : neurons) {
            neuronPtrs.push_back(neuron.get());
        }
        
        Neuron::configurePopulation(neuronPtrs, NeuronConfig{
            restingPotential, threshold, leakConductance,
            refractoryPeriod, resetPotential, neuronType
        });
    }
}

const char* Phase3Demo::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

const char* Phase3Demo::stageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

} // namespace nlm
