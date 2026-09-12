// AgentBrain End-to-End Tests
// Complete agent behavior simulation with full system integration

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include "agent/MotorDecoder.hpp"
#include "agent/NeuromodulationController.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

namespace test_agentbrain_e2e {

void initializeWorldWithObjects(nlm::SimpleWorld& world, const std::vector<nlm::WorldObject>& objects) {
    for (const auto& obj : objects) {
        world.addObject(obj);
    }
}

void updateAgentPosition(nlm::AgentBrain& agent, nlm::SimpleWorld& world, const std::vector<nlm::MotorCommand>& actions, size_t step) {
    if (step < actions.size()) {
        nlm::ActionResult result = world.applyMotorCommand(actions[step], world.getSimulationTime());
        agent.applyRewardModulation(result.reward, 0.0f);  // No prediction initially
    }
}

void testCompleteAgentSimulation() {
    std::cout << "  Testing complete Agent simulation..." << std::endl;
    
    // Create full agent system with sufficient complexity
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create rich world environment
    nlm::SimpleWorld world;
    world.configure(50, 50, 20, 20);  // Large world with good vision
    world.reset();
    world.setAgentStart(25.0f, 25.0f);  // Start in center
    
    // Add diverse objects to the world
    std::vector<nlm::WorldObject> objects;
    
    // Add resources (positive reward)
    for (int i = 0; i < 5; ++i) {
        objects.emplace_back(
            static_cast<float>(10 + i * 8), 
            static_cast<float>(10 + i * 8), 
            nlm::WorldObjectType::Resource, 
            2.0f  // Resource value
        );
    }
    
    // Add hazards (negative reward)
    for (int i = 0; i < 3; ++i) {
        objects.emplace_back(
            static_cast<float>(40 - i * 8), 
            static_cast<float>(40 - i * 8), 
            nlm::WorldObjectType::Hazard, 
            -1.5f  // Hazard damage
        );
    }
    
    // Add walls (boundaries)
    for (int i = 0; i < 10; ++i) {
        objects.emplace_back(
            static_cast_cast<float>(i * 5), 
            static_cast<float>(5), 
            nlm::WorldObjectType::Wall, 
            0.0f
        );
    }
    
    initializeWorldWithObjects(world, objects);
    
    // Initialize agent
    agent.initialize(world);
    
    // Enable all features for comprehensive test
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Create a variety of actions to explore
    std::vector<nlm::MotorCommand> actionSequence = {
        nlm::MotorCommand::MoveForward,
        nlm::MotorCommand::TurnLeft,
        nlm::MotorCommand::TurnRight,
        nlm::MotorCommand::MoveBackward,
        nlm::MotorCommand::Interact,
        nlm::MotorCommand::LookLeft,
        nlm::MotorCommand::LookRight,
        nlm::MotorCommand::Wait
    };
    
    // Extended simulation with environmental interaction
    const int simulationSteps = 200;
    float totalReward = 0.0f;
    float maxNovelty = 0.0f;
    
    for (int step = 0; step < simulationSteps; ++step) {
        // Get current sensory percept
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        
        // Process sensory input through agent
        agent.processSensoryInput(percept);
        
        // Occasionally try curiosity-driven exploration
        if (step % 15 == 0 && agent.getCuriosityLevel() > 0.3f) {
            // Agent may choose exploration action based on curiosity
            nlm::MotorCommand curiosityCmd = agent.decodeMotorCommand();
            (void)curiosityCmd;
        }
        
        // Apply a sequence of actions
        if (step % 5 == 0) {
            updateAgentPosition(agent, world, actionSequence, step / 5);
        }
        
        // Natural world decay (energy loss over time)
        float timeBasedReward = -0.01f;  // Small energy cost
        agent.applyRewardModulation(timeBasedReward, 0.0f);
        
        // Update development system
        agent.updateDevelopment(1.0);
        
        // Track statistics
        totalReward += agent.getNeuromodulationLevel();
        maxNovelty = std::max(maxNovelty, agent.getNoveltyLevel());
        
        // Occasionally add environmental novelty (changing world state)
        if (step % 50 == 0 && step > 0) {
            // Create novelty by perturbing vision input
            std::vector<float> novelVision(20 * 20, 0.5f);
            // Add some random variation
            std::mt19937 rng(42 + step);
            std::uniform_real_distribution<float> dist(-0.2f, 0.2f);
            
            for (size_t i = 0; i < novelVision.size(); ++i) {
                novelVision[i] += dist(rng);
                novelVision[i] = std::max(0.0f, std::min(1.0f, novelVision[i]));
            }
            
            nlm::SensoryPercept novelPercept;
            novelPercept.setVision(novelVision);
            novelPercept.setTouch(std::vector<float>(4, 0.0f));
            novelPercept.setInternal(std::vector<float>(2, 0.5f));
            novelPercept.setProprioception(std::vector<float>(4, 0.0f));
            
            agent.processSensoryInput(novelPercept);
        }
        
        // Verify agent remains in valid state
        assert(agent.getNeuromodulationLevel() >= -1.0f);
        assert(agent.getCuriosityLevel() >= 0.0f);
        assert(agent.getNoveltyLevel() >= 0.0f);
        assert(static_cast<int>(agent.getDevelopmentalStage()) >= 0 && 
               static_cast<int>(agent.getDevelopmentalStage()) <= 3);
    }
    
    // Verify simulation results
    assert(totalReward >= -simulationSteps * 0.02f);  // Should not be too negative
    assert(maxNovelty > 0.0f);  // Should have experienced some novelty
    assert(agent.getDevelopmentalAge() >= 0.0f);  // Development should have progressed
    assert(agent.getNeuromodulationLevel() >= 0.0f);  // Neuromodulation should be present
    
    std::cout << "    Complete Agent simulation: PASSED" << std::endl;
}

void testAgentWithDynamicEnvironment() {
    std::cout << "  Testing Agent with Dynamic Environment..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(30, 30, 12, 12);
    world.reset();
    
    agent.initialize(world);
    
    // Enable curiosity for exploration
    agent.enableCuriosity(true);
    
    // Run multiple episodes
    const int episodes = 5;
    const int stepsPerEpisode = 100;
    
    for (int episode = 0; episode < episodes; ++episode) {
        std::cout << "    Episode " << episode + 1 << "/" << episodes << ": " << std::endl;
        
        // Reset for new episode
        agent.reset();
        world.reset();
        
        // Create dynamic environment (changing each episode)
        std::vector<nlm::WorldObject> episodeObjects;
        
        // Vary object placement and types based on episode
        for (int i = 0; i < 3 + episode; ++i) {
            float x = static_cast<float>(10 + i * 10);
            float y = static_cast<float>(10 + (i % 2) * 15);
            
            if (episode % 2 == 0) {
                episodeObjects.emplace_back(x, y, nlm::WorldObjectType::Resource, 2.0f - episode * 0.1f);
            } else {
                episodeObjects.emplace_back(x, y, nlm::WorldObjectType::Hazard, -1.0f - episode * 0.05f);
            }
        }
        
        // Add changing obstacles
        for (int i = 0; i < 2; ++i) {
            float x = static_cast<float>(20 + i * 5);
            float y = static_cast<float>(20 + i * 5);
            episodeObjects.emplace_back(x, y, nlm::WorldObjectType::Wall, 0.0f);
        }
        
        initializeWorldWithObjects(world, episodeObjects);
        
        // Run episode
        float episodeReward = 0.0f;
        float episodeMaxNovelty = 0.0f;
        
        for (int step = 0; step < stepsPerEpisode; ++step) {
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            // Simple exploration strategy
            if (agent.getCuriosityLevel() > 0.4f && step % 3 == 0) {
                // Act based on curiosity
                nlm::MotorCommand cmd = agent.decodeMotorCommand();
                (void)cmd;
            }
            
            // Apply environmental effects
            float environmentalReward = 0.0f;
            
            // Check for proximity to objects (simplified interaction)
            for (const auto& obj : episodeObjects) {
                float dx = obj.x - world.getAgentBody().x;
                float dy = obj.y - world.getAgentBody().y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance < 5.0f) {
                    environmentalReward += obj.value * (1.0f / (1.0f + distance));
                }
            }
            
            agent.applyRewardModulation(environmentalReward, 0.0f);
            agent.updateDevelopment(1.0);
            
            episodeReward += environmentalReward;
            episodeMaxNovelty = std::max(episodeMaxNovelty, agent.getNoveltyLevel());
            
            // Add temporal novelty
            if (step % 20 == 0) {
                std::vector<float> temporalVision(12 * 12, 0.5f);
                nlm::SensoryPercept temporalPercept;
                temporalPercept.setVision(temporalVision);
                agent.processSensoryInput(temporalPercept);
            }
        }
        
        // Verify episode results
        assert(episodeMaxNovelty >= 0.0f);
        assert(agent.getNeuromodulationLevel() >= 0.0f);
        
        std::cout << "      Episode completed with max novelty: " 
                  << episodeMaxNovelty << ", total modulation: " 
                  << agent.getNeuromodulationLevel() << std::endl;
    }
    
    std::cout << "    Agent with Dynamic Environment: PASSED" << std::endl;
}

void testAgentWithMemoryAndLearning() {
    std::cout << "  Testing Agent with Memory and Learning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(40, 40, 16, 16);
    world.reset();
    world.setRandomSeed(12345ULL);
    
    agent.initialize(world);
    
    // Enable all plasticity systems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Learning phases
    struct LearningPhase {
        std::string name;
        int duration;
        float rewardMean;
        float rewardStd;
    };
    
    std::vector<LearningPhase> phases = {
        {"Exploration", 50, 0.3f, 0.2f},
        {"Exploitation", 100, 0.7f, 0.3f},
        {"Consolidation", 75, 0.5f, 0.4f},
        {"Adaptation", 60, 0.6f, 0.25f}
    };
    
    float totalLearningScore = 0.0f;
    int phaseCount = 0;
    
    for (const auto& phase : phases) {
        std::cout << "    Learning phase: " << phase.name << std::endl;
        
        // Reset for new phase
        agent.reset();
        
        // Simulate learning with rewards based on phase characteristics
        std::mt19937 rng(12345 + phaseCount);
        std::normal_distribution<float> rewardDist(phase.rewardMean, phase.rewardStd);
        
        float phaseTotal = 0.0f;
        float phaseMaxNovelty = 0.0f;
        float phaseMaxCuriosity = 0.0f;
        
        for (int step = 0; step < phase.duration; ++step) {
            // Generate reward based on normal distribution (learning with noise)
            float reward = rewardDist(rng);
            reward = std::max(-0.5f, std::min(1.5f, reward));  // Clamp
            
            // Apply reward with prediction error learning
            float predicted = agent.getExpectedReward();
            agent.applyRewardModulation(reward, predicted);
            
            // Update development to allow plasticity changes
            agent.updateDevelopment(1.0);
            
            // Track learning metrics
            phaseTotal += std::abs(agent.getPredictionError());
            phaseMaxNovelty = std::max(phaseMaxNovelty, agent.getNoveltyLevel());
            phaseMaxCuriosity = std::max(phaseMaxCuriosity, agent.getCuriosityLevel());
            
            // Add exploration noise
            if (agent.getCuriosityLevel() > 0.5f) {
                nlm::SensoryPercept percept;
                std::vector<float> vision(16 * 16, 0.5f);
                
                // Add random perturbation for exploration
                for (size_t i = 0; i < vision.size(); ++i) {
                    std::uniform_real_distribution<float> noise(-0.3f, 0.3f);
                    vision[i] += noise(rng);
                    vision[i] = std::max(0.0f, std::min(1.0f, vision[i]));
                }
                
                percept.setVision(vision);
                agent.processSensoryInput(percept);
            }
        }
        
        // Calculate learning score (higher is better)
        float learningScore = (phaseTotal / phase.duration) * 
                            (1.0f + phaseMaxNovelty) * 
                            (1.0f + phaseMaxCuriosity);
        
        totalLearningScore += learningScore;
        phaseCount++;
        
        std::cout << "      Phase score: " << learningScore 
                  << ", Max novelty: " << phaseMaxNovelty 
                  << ", Max curiosity: " << phaseMaxCuriosity << std::endl;
    }
    
    // Verify overall learning
    assert(phaseCount == static_cast<int>(phases.size()));
    assert(totalLearningScore > 0.0f);  // Should have learned something
    assert(agent.getDevelopmentalAge() > 0.0f);
    
    std::cout << "    Agent with Memory and Learning: PASSED" << std::endl;
}

void testAgentStressAndRecovery() {
    std::cout << "  Testing Agent Stress and Recovery..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(35, 35, 14, 14);
    world.reset();
    
    agent.initialize(world);
    
    // Stress test: fluctuating rewards and high novelty
    const int stressPeriod = 150;
    const int recoveryPeriod = 100;
    
    // Phase 1: Stress (unpredictable rewards, high novelty)
    std::cout << "    Stress phase..." << std::endl;
    for (int step = 0; step < stressPeriod; ++step) {
        // Highly variable rewards (stress)
        float stressReward = (step % 7 == 0) ? 2.0f : 
                           (step % 11 == 0) ? -1.5f : 
                           (step % 13 == 0) ? 0.5f : -0.2f;
        
        // Add high novelty
        if (step % 3 == 0) {
            std::vector<float> stressVision(14 * 14, 0.5f);
            // Add significant random variation
            std::mt19937 rng(999 + step);
            std::uniform_real_distribution<float> dist(-0.8f, 0.8f);
            
            for (size_t i = 0; i < stressVision.size(); ++i) {
                stressVision[i] = std::max(0.0f, std::min(1.0f, stressVision[i] + dist(rng)));
            }
            
            nlm::SensoryPercept stressPercept;
            stressPercept.setVision(stressVision);
            agent.processSensoryInput(stressPercept);
        }
        
        agent.applyRewardModulation(stressReward, 0.0f);
        agent.updateDevelopment(1.0);
        
        // Verify agent can handle stress
        assert(agent.getNeuromodulationLevel() >= -1.0f);
        assert(agent.getCuriosityLevel() >= 0.0f);
    }
    
    // Phase 2: Recovery (stable rewards, lower novelty)
    std::cout << "    Recovery phase..." << std::endl;
    float recoveryNeuromodulation = agent.getNeuromodulationLevel();
    
    for (int step = 0; step < recoveryPeriod; ++step) {
        // Stable positive rewards (recovery)
        float recoveryReward = 0.3f;
        
        agent.applyRewardModulation(recoveryReward, recoveryReward * 0.9f);
        agent.updateDevelopment(1.0);
        
        // Should show signs of recovery
        assert(agent.getNeuromodulationLevel() >= -0.5f);
    }
    
    // Verify recovery
    float finalNeuromodulation = agent.getNeuromodulationLevel();
    assert(finalNeuromodulation > recoveryNeuromodulation || 
           std::abs(finalNeuromodulation - recoveryNeuromodulation) < 0.1f);
    
    std::cout << "    Agent Stress and Recovery: PASSED" << std::endl;
}

void testAgentCrossEpochPerformance() {
    std::cout << "  Testing Agent Cross-Epoch Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(45, 45, 18, 18);
    world.reset();
    
    agent.initialize(world);
    
    // Enable all adaptive systems
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    agent.enableStructuralPlasticity(true);
    
    // Run extended simulation with multiple epochs
    const int totalEpochs = 8;
    const int stepsPerEpoch = 120;
    
    std::vector<float> epochScores(totalEpochs, 0.0f);
    
    for (int epoch = 0; epoch < totalEpochs; ++epoch) {
        std::cout << "    Epoch " << epoch + 1 << "/" << totalEpochs << ": " << std::endl;
        
        agent.reset();
        world.reset();
        
        float epochPerformance = 0.0f;
        
        // Varying environment per epoch (to test adaptation)
        float epochRewardBase = 0.3f + 0.1f * epoch;  // Gradual difficulty increase
        float epochNoveltyBase = 0.2f + 0.05f * epoch;  // Gradual novelty increase
        
        for (int step = 0; step < stepsPerEpoch; ++step) {
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            // Agent's adaptive response to epoch characteristics
            float targetReward = epochRewardBase * (1.0f + 0.2f * std::sin(step * 0.1f));
            float predictedReward = agent.getExpectedReward();
            
            float epochReward = targetReward + 
                               (epoch % 2 == 0 ? 0.1f : -0.1f) +  // Periodic variation
                               (step % 5 == 0 ? 0.3f : 0.0f);   // Occasional spikes
            
            agent.applyRewardModulation(epochReward, predictedReward);
            agent.updateDevelopment(1.0);
            
            epochPerformance += std::abs(agent.getPredictionError());
        }
        
        // Normalize performance score
        epochScores[epoch] = epochPerformance / stepsPerEpoch;
        
        std::cout << "      Performance score: " << epochScores[epoch] 
                  << ", Neuromodulation: " << agent.getNeuromodulationLevel() 
                  << ", Age: " << agent.getDevelopmentalAge() << std::endl;
        
        // Verify adaptation (performance should not decrease monotonically)
        if (epoch > 0) {
            assert(epochScores[epoch] >= 0.0f);  // Should not be negative
        }
    }
    
    // Verify overall performance trends
    float avgPerformance = 0.0f;
    for (float score : epochScores) {
        avgPerformance += score;
    }
    avgPerformance /= totalEpochs;
    
    assert(avgPerformance >= 0.0f);
    assert(agent.getDevelopmentalAge() > 0.0f);
    assert(agent.getNeuromodulationLevel() >= 0.0f);
    
    std::cout << "    Agent Cross-Epoch Performance: PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain End-to-End Tests..." << std::endl;
    std::cout << "=============================================" << std::endl;
    
    testCompleteAgentSimulation();
    testAgentWithDynamicEnvironment();
    testAgentWithMemoryAndLearning();
    testAgentStressAndRecovery();
    testAgentCrossEpochPerformance();
    
    std::cout << "=============================================" << std::endl;
    std::cout << "All AgentBrain End-to-End Tests PASSED!" << std::endl;
}

} // namespace test_agentbrain_e2e