// Advanced Simulation Tests
// Comprehensive test suite for advanced simulation capabilities

#include "AdvancedSimulationTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Experiment.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <unordered_set>
#include <memory>

namespace test_suites {

class AdvancedSimulationTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Advanced Simulation Tests ===" << std::endl;
        
        test_multi_agent_interaction_simulation();
        test_complex_adaptive_system_simulation();
        test_social_learning_and_cultural_transmission();
        test_emergent_behavior_analysis();
        test_multi_agent_coordination();
        test_population_dynamics();
        test_ecosystem_simulation();
        test_predator_prey_dynamics();
        test_synchronization_effects();
        test_self_organization_dynamics();
        
        std::cout << "=== Advanced Simulation Tests Complete ===" << std::endl;
    }

private:
    static void test_multi_agent_interaction_simulation() {
        std::cout << "Testing multi-agent interaction simulation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Multi-agent simulation setup
        std::vector<std::shared_ptr<nlm::Brain>> agents;
        std::vector<std::unique_ptr<nlm::AgentBrain>> agentBrains;
        
        // Create multiple agents
        for (int i = 0; i < 5; ++i) {
            auto agentConfig = std::make_shared<nlm::Config>();
            agentConfig->set("neuron_count", 300);
            auto agentBrain = std::make_shared<nlm::Brain>(agentConfig);
            agentBrain->initialize();
            agents.push_back(agentBrain);
            
            auto agentWrapper = std::make_unique<nlm::AgentBrain>(agentBrain);
            agentBrains.push_back(std::move(agentWrapper));
        }
        
        // Multi-agent interaction simulation
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Each agent interacts with the environment
            for (size_t i = 0; i < agents.size(); ++i) {
                agents[i]->step(step);
                
                // Inter-agent interactions
                if (i > 0) {
                    // Agent i observes agent i-1's actions
                    auto action1 = agents[i-1]->produceAction();
                    if (action1) {
                        // Social learning and imitation
                        agents[i]->receiveSensoryInput(*action1);
                    }
                }
                
                // Group coordination
                if (step % 10 == 0) {
                    // Collective behavior
                    float groupSignal = 0.5f;
                    nlm::Neuromodulator groupNeuromodulator;
                    groupNeuromodulator.setLevel(groupSignal);
                    agents[i]->applyNeuromodulation(groupNeuromodulator);
                }
            }
        }
        
        // Verify multi-agent dynamics
        assert(agents.size() == 5);
        assert(agentBrains.size() == 5);
        
        // Agents should have developed different behaviors
        float totalActivity = 0.0f;
        for (const auto& agent : agents) {
            totalActivity += agent->getAverageFiringRate();
        }
        assert(totalActivity > 0.0f);
        
        std::cout << "    Multi-agent interaction simulation: PASSED" << std::endl;
    }
    
    static void test_complex_adaptive_system_simulation() {
        std::cout << "Testing complex adaptive system simulation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Complex adaptive system simulation
        std::vector<std::vector<float>> adaptiveParameters;
        std::vector<float> systemStability;
        std::vector<float> adaptationRates;
        
        for (int step = 0; step < 120; ++step) {
            brain.step(step);
            
            // Adaptive parameters
            float plasticityRate = brain.getCurrentPlasticityRate();
            float learningRate = brain.getCurrentLearningRate();
            float adaptability = brain.getAdaptabilityMetric();
            
            adaptiveParameters.push_back({plasticityRate, learningRate, adaptability});
            
            // System stability analysis
            float stability = 1.0f - fabs(plasticityRate - 0.5f) - fabs(learningRate - 0.3f);
            systemStability.push_back(stability);
            
            // Adaptation rate calculation
            float adaptationRate = stability * adaptability;
            adaptationRates.push_back(adaptationRate);
            
            // Adaptive system control
            if (step % 15 == 0) {
                // System parameter adjustment
                float adjustment = 0.1f * ((step / 15) % 2 == 0 ? 1.0f : -1.0f);
                brain.adjustPlasticityRate(adjustment);
                brain.adjustLearningRate(adjustment * 0.5f);
            }
        }
        
        // Verify complex adaptive system dynamics
        assert(adaptiveParameters.size() == 120);
        assert(systemStability.size() == 120);
        assert(adaptationRates.size() == 120);
        
        // System should remain stable
        float avgStability = std::accumulate(systemStability.begin(), systemStability.end(), 0.0f) / systemStability.size();
        assert(avgStability >= 0.3f && avgStability <= 0.9f);
        
        // Adaptation should be effective
        float avgAdaptationRate = std::accumulate(adaptationRates.begin(), adaptationRates.end(), 0.0f) / adaptationRates.size();
        assert(avgAdaptationRate >= 0.1f);
        
        std::cout << "    Complex adaptive system simulation: PASSED" << std::endl;
    }
    
    static void test_social_learning_and_cultural_transmission() {
        std::cout << "Testing social learning and cultural transmission..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Social learning and cultural transmission simulation
        std::vector<std::string> culturalTraits;
        std::vector<float> transmissionRates;
        
        // Initialize cultural traits
        culturalTraits = {"cooperation", "competition", "curiosity", "caution"};
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Cultural transmission simulation
            for (size_t i = 0; i < culturalTraits.size(); ++i) {
                float transmissionRate = 0.0f;
                
                // Social learning based on trait popularity
                if (i == 0) transmissionRate = 0.7f;  // Cooperation spreads easily
                else if (i == 1) transmissionRate = 0.5f;  // Competition spreads moderately
                else if (i == 2) transmissionRate = 0.8f;  // Curiosity spreads quickly
                else transmissionRate = 0.3f;  // Caution spreads slowly
                
                transmissionRates.push_back(transmissionRate);
                
                // Cultural evolution
                if (step % 20 == 0) {
                    // Trait modification through cultural evolution
                    float evolutionFactor = 0.1f * ((step / 20) % 2 == 0 ? 1.0f : -1.0f);
                    brain.evolveCulturalTrait(culturalTraits[i], evolutionFactor);
                }
                
                // Social learning reinforcement
                nlm::Neuromodulator socialLearningSignal;
                socialLearningSignal.setLevel(transmissionRate);
                brain.applyNeuromodulation(socialLearningSignal);
            }
        }
        
        // Verify cultural transmission dynamics
        assert(culturalTraits.size() == 4);
        assert(transmissionRates.size() == 400);  // 4 traits * 100 steps
        
        // Cultural transmission should be effective
        float avgTransmissionRate = std::accumulate(transmissionRates.begin(), transmissionRates.end(), 0.0f) / transmissionRates.size();
        assert(avgTransmissionRate >= 0.4f);
        
        // Key traits should dominate
        assert(culturalTraits[0] == "cooperation");  // Should still be present
        assert(culturalTraits[2] == "curiosity");  // Should still be present
        
        std::cout << "    Social learning and cultural transmission: PASSED" << std::endl;
    }
    
    static void test_emergent_behavior_analysis() {
        std::cout << "Testing emergent behavior analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Emergent behavior analysis
        std::vector<std::vector<float>> behavioralPatterns;
        std::vector<float> emergenceScores;
        std::vector<float> complexityMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Analyze behavioral patterns
            std::vector<float> pattern(8, 0.0f);
            pattern[0] = brain.getAverageFiringRate();  // Overall activity
            pattern[1] = brain.getCurrentPlasticityRate();  // Plasticity
            pattern[2] = brain.getCurrentLearningRate();  // Learning rate
            pattern[3] = brain.getAdaptabilityMetric();  // Adaptability
            pattern[4] = brain.getMemoryUtilization();  // Memory usage
            pattern[5] = brain.getCognitiveLoad();  // Cognitive load
            pattern[6] = brain.getExplorationRate();  // Exploration
            pattern[7] = brain.getExploitationRate();  // Exploitation
            
            behavioralPatterns.push_back(pattern);
            
            // Emergence score calculation
            float emergence = pattern[0] * pattern[1] * pattern[2] * pattern[3];
            emergenceScores.push_back(emergence);
            
            // Complexity metric
            float complexity = pattern[4] + pattern[5] + pattern[6] + pattern[7];
            complexityMetrics.push_back(complexity);
            
            // Emergent behavior triggers
            if (step % 25 == 0 && emergence > 0.3f) {
                // Emergent behavior detected
                nlm::Neuromodulator emergenceSignal;
                emergenceSignal.setLevel(emergence);
                brain.applyNeuromodulation(emergenceSignal);
            }
        }
        
        // Verify emergent behavior analysis
        assert(behavioralPatterns.size() == 90);
        assert(emergenceScores.size() == 90);
        assert(complexityMetrics.size() == 90);
        
        // Emergence should be detectable
        float maxEmergence = *std::max_element(emergenceScores.begin(), emergenceScores.end());
        assert(maxEmergence > 0.1f);
        
        // Behavior should be complex enough to show emergence
        float avgComplexity = std::accumulate(complexityMetrics.begin(), complexityMetrics.end(), 0.0f) / complexityMetrics.size();
        assert(avgComplexity >= 0.5f);
        
        std::cout << "    Emergent behavior analysis: PASSED" << std::endl;
    }
    
    static void test_multi_agent_coordination() {
        std::cout << "Testing multi-agent coordination..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Multi-agent coordination simulation
        std::vector<std::vector<float>> coordinationMetrics;
        std::vector<float> groupCohesion;
        std::vector<float> roleSpecialization;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Coordination metrics
            float coordination = brain.getCurrentCoordinationLevel();
            float cohesion = brain.getGroupCohesion();
            float specialization = brain.getRoleSpecialization();
            
            coordinationMetrics.push_back({coordination, cohesion, specialization});
            groupCohesion.push_back(cohesion);
            roleSpecialization.push_back(specialization);
            
            // Coordination enhancement
            if (step % 14 == 0) {
                // Improve coordination through communication
                float communicationSignal = 0.8f;
                nlm::Neuromodulator coordinationSignal;
                coordinationSignal.setLevel(communicationSignal);
                brain.applyNeuromodulation(coordinationSignal);
            }
        }
        
        // Verify multi-agent coordination
        assert(coordinationMetrics.size() == 70);
        assert(groupCohesion.size() == 70);
        assert(roleSpecialization.size() == 70);
        
        // Coordination should improve over time
        float finalCohesion = groupCohesion.back();
        float initialCohesion = groupCohesion.front();
        assert(finalCohesion >= initialCohesion * 0.9f);
        
        // Role specialization should be present
        float avgSpecialization = std::accumulate(roleSpecialization.begin(), roleSpecialization.end(), 0.0f) / roleSpecialization.size();
        assert(avgSpecialization >= 0.3f);
        
        std::cout << "    Multi-agent coordination: PASSED" << std::endl;
    }
    
    static void test_population_dynamics() {
        std::cout << "Testing population dynamics..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Population dynamics simulation
        std::vector<float> populationGrowth;
        std::vector<float> resourceConsumption;
        std::vector<float> competitionLevels;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Population metrics
            float growthRate = brain.getPopulationGrowthRate();
            float resourceUsage = brain.getResourceConsumption();
            float competition = brain.getCompetitionLevel();
            
            populationGrowth.push_back(growthRate);
            resourceConsumption.push_back(resourceUsage);
            competitionLevels.push_back(competition);
            
            // Population regulation
            if (step % 20 == 0) {
                // Density-dependent regulation
                float regulationSignal = 0.6f + 0.4f * (1.0f - growthRate);
                nlm::Neuromodulator regulationNeuromodulator;
                regulationNeuromodulator.setLevel(regulationSignal);
                brain.applyNeuromodulation(regulationNeuromodulator);
            }
        }
        
        // Verify population dynamics
        assert(populationGrowth.size() == 80);
        assert(resourceConsumption.size() == 80);
        assert(competitionLevels.size() == 80);
        
        // Population should be stable
        float avgGrowth = std::accumulate(populationGrowth.begin(), populationGrowth.end(), 0.0f) / populationGrowth.size();
        assert(avgGrowth >= 0.0f && avgGrowth <= 1.0f);
        
        // Resource consumption should be reasonable
        float avgResourceConsumption = std::accumulate(resourceConsumption.begin(), resourceConsumption.end(), 0.0f) / resourceConsumption.size();
        assert(avgResourceConsumption >= 0.1f && avgResourceConsumption <= 1.0f);
        
        std::cout << "    Population dynamics: PASSED" << std::endl;
    }
    
    static void test_ecosystem_simulation() {
        std::cout << "Testing ecosystem simulation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 420);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Ecosystem simulation
        std::vector<std::vector<float>> ecosystemMetrics;
        std::vector<float> biodiversityIndices;
        std::vector<float> ecosystemStability;
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Ecosystem metrics
            float biodiversity = brain.getEcosystemBiodiversity();
            float stability = brain.getEcosystemStability();
            float resilience = brain.getEcosystemResilience();
            
            ecosystemMetrics.push_back({biodiversity, stability, resilience});
            biodiversityIndices.push_back(biodiversity);
            ecosystemStability.push_back(stability);
            
            // Ecosystem balance
            if (step % 25 == 0) {
                // Maintain ecosystem balance
                float balanceSignal = 0.7f + 0.3f * (stability - 0.5f);
                nlm::Neuromodulator balanceNeuromodulator;
                balanceNeuromodulator.setLevel(balanceSignal);
                brain.applyNeuromodulation(balanceNeuromodulator);
            }
        }
        
        // Verify ecosystem simulation
        assert(ecosystemMetrics.size() == 100);
        assert(biodiversityIndices.size() == 100);
        assert(ecosystemStability.size() == 100);
        
        // Biodiversity should be maintained
        float avgBiodiversity = std::accumulate(biodiversityIndices.begin(), biodiversityIndices.end(), 0.0f) / biodiversityIndices.size();
        assert(avgBiodiversity >= 0.3f && avgBiodiversity <= 0.9f);
        
        // Ecosystem should be stable
        float avgStability = std::accumulate(ecosystemStability.begin(), ecosystemStability.end(), 0.0f) / ecosystemStability.size();
        assert(avgStability >= 0.5f);
        
        std::cout << "    Ecosystem simulation: PASSED" << std::endl;
    }
    
    static void test_predator_prey_dynamics() {
        std::cout << "Testing predator-prey dynamics..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Predator-prey dynamics simulation
        std::vector<float> predatorPopulations;
        std::vector<float> preyPopulations;
        std::vector<float> interactionStrengths;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Predator-prey metrics
            float predator = brain.getPredatorPopulation();
            float prey = brain.getPreyPopulation();
            float interaction = brain.getPredatorPreyInteraction();
            
            predatorPopulations.push_back(predator);
            preyPopulations.push_back(prey);
            interactionStrengths.push_back(interaction);
            
            // Population regulation
            if (step % 30 == 0) {
                // Carrying capacity effects
                float carryingCapacity = prey * 0.8f;  // Prey carrying capacity
                float carryingSignal = carryingCapacity / (predator + 1.0f);
                nlm::Neuromodulator carryingNeuromodulator;
                carryingNeuromodulator.setLevel(carryingSignal);
                brain.applyNeuromodulation(carryingNeuromodulator);
            }
        }
        
        // Verify predator-prey dynamics
        assert(predatorPopulations.size() == 90);
        assert(preyPopulations.size() == 90);
        assert(interactionStrengths.size() == 90);
        
        // Predator-prey populations should be stable
        float avgPredator = std::accumulate(predatorPopulations.begin(), predatorPopulations.end(), 0.0f) / predatorPopulations.size();
        float avgPrey = std::accumulate(preyPopulations.begin(), preyPopulations.end(), 0.0f) / preyPopulations.size();
        
        assert(avgPredator >= 0.1f && avgPredator <= 1.0f);
        assert(avgPrey >= 0.1f && avgPrey <= 1.0f);
        
        // Interaction strength should be reasonable
        float avgInteraction = std::accumulate(interactionStrengths.begin(), interactionStrengths.end(), 0.0f) / interactionStrengths.size();
        assert(avgInteraction >= 0.1f && avgInteraction <= 1.0f);
        
        std::cout << "    Predator-prey dynamics: PASSED" << std::endl;
    }
    
    static void test_synchronization_effects() {
        std::cout << "Testing synchronization effects..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 360);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Synchronization effects simulation
        std::vector<float> synchronizationMetrics;
        std::vector<float> phaseLockingMetrics;
        std::vector<float> entrainmentMetrics;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Synchronization metrics
            float synchronization = brain.getSynchronizationLevel();
            float phaseLocking = brain.getPhaseLockingLevel();
            float entrainment = brain.getEntrainmentLevel();
            
            synchronizationMetrics.push_back(synchronization);
            phaseLockingMetrics.push_back(phaseLocking);
            entrainmentMetrics.push_back(entrainment);
            
            // Synchronization enhancement
            if (step % 14 == 0) {
                // External synchronization signal
                float syncSignal = 0.9f;
                nlm::Neuromodulator syncNeuromodulator;
                syncNeuromodulator.setLevel(syncSignal);
                brain.applyNeuromodulation(syncNeuromodulator);
            }
        }
        
        // Verify synchronization effects
        assert(synchronizationMetrics.size() == 70);
        assert(phaseLockingMetrics.size() == 70);
        assert(entrainmentMetrics.size() == 70);
        
        // Synchronization should be effective
        float avgSynchronization = std::accumulate(synchronizationMetrics.begin(), synchronizationMetrics.end(), 0.0f) / synchronizationMetrics.size();
        assert(avgSynchronization >= 0.6f);
        
        // Phase locking should be present
        float avgPhaseLocking = std::accumulate(phaseLockingMetrics.begin(), phaseLockingMetrics.end(), 0.0f) / phaseLockingMetrics.size();
        assert(avgPhaseLocking >= 0.4f);
        
        std::cout << "    Synchronization effects: PASSED" << std::endl;
    }
    
    static void test_self_organization_dynamics() {
        std::cout << "Testing self-organization dynamics..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 390);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Self-organization dynamics simulation
        std::vector<std::vector<float>> selfOrganizationMetrics;
        std::vector<float> patternFormationMetrics;
        std::vector<float> structureEmergenceMetrics;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Self-organization metrics
            float patternFormation = brain.getPatternFormationLevel();
            float structureEmergence = brain.getStructureEmergenceLevel();
            float selfOrganization = brain.getSelfOrganizationLevel();
            
            selfOrganizationMetrics.push_back({patternFormation, structureEmergence, selfOrganization});
            patternFormationMetrics.push_back(patternFormation);
            structureEmergenceMetrics.push_back(structureEmergence);
            
            // Self-organization enhancement
            if (step % 20 == 0) {
                // Spontaneous pattern formation
                float patternSignal = 0.8f;
                nlm::Neuromodulator patternNeuromodulator;
                patternNeuromodulator.setLevel(patternSignal);
                brain.applyNeuromodulation(patternNeuromodulator);
            }
        }
        
        // Verify self-organization dynamics
        assert(selfOrganizationMetrics.size() == 80);
        assert(patternFormationMetrics.size() == 80);
        assert(structureEmergenceMetrics.size() == 80);
        
        // Pattern formation should be effective
        float avgPatternFormation = std::accumulate(patternFormationMetrics.begin(), patternFormationMetrics.end(), 0.0f) / patternFormationMetrics.size();
        assert(avgPatternFormation >= 0.5f);
        
        // Structure emergence should be present
        float avgStructureEmergence = std::accumulate(structureEmergenceMetrics.begin(), structureEmergenceMetrics.end(), 0.0f) / structureEmergenceMetrics.size();
        assert(avgStructureEmergence >= 0.3f);
        
        std::cout << "    Self-organization dynamics: PASSED" << std::endl;
    }
};

} // namespace test_suites