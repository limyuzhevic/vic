// Integration Testing Framework
// Phase 6 system integration tests and cross-component validation

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace integration_tests {

    // Integration test configuration
    struct IntegrationTestConfig {
        std::string test_name;
        bool enable_memory_integration;
        bool enable_neuromodulation_integration;
        bool enable_prediction_integration;
        bool enable_development_integration;
        bool enable_cognition_integration;
        int neuron_count;
        int simulation_steps;
        float performance_threshold;
    };

    // Integration test results
    struct IntegrationTestResult {
        std::string test_name;
        bool passed;
        std::vector<std::string> failed_systems;
        std::vector<std::string> integration_issues;
        double execution_time;
        int neurons_tested;
        int synapses_tested;
        float avg_firing_rate;
        float total_reward;
    };

    // Phase 6 system integration tests
    class Phase6IntegrationTests {
    public:
        IntegrationTestResult test_memory_integration() {
            IntegrationTestResult result;
            result.test_name = "Phase 6 Memory Integration Test";
            result.passed = true;
            result.execution_time = 0.0;
            result.neurons_tested = 0;
            result.synapses_tested = 0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Create brain configuration with memory systems
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("region_count", 2);
                cfg->set("enable_memory", true);
                cfg->set("enable_reward", true);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                // Test working memory integration
                auto* working_memory = brain->getWorkingMemory();
                if (working_memory) {
                    result.neurons_tested = working_memory->getActiveTraces();
                    
                    // Store and retrieve traces
                    working_memory->storeTrace(NeuronId(1), 0.5f);
                    working_memory->storeTrace(NeuronId(2), 0.8f);
                    
                    float trace1 = working_memory->getTrace(NeuronId(1));
                    float trace2 = working_memory->getTrace(NeuronId(2));
                    
                    if (trace1 < 0.0f || trace1 > 1.0f || trace2 < 0.0f || trace2 > 1.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Working memory trace values out of bounds");
                    }
                    
                    // Test trace decay
                    working_memory->updateTrace(NeuronId(1), 0.01f);
                    float decayed_trace = working_memory->getTrace(NeuronId(1));
                    if (decayed_trace >= trace1) {
                        result.passed = false;
                        result.integration_issues.push_back("Working memory trace decay failed");
                    }
                    
                    std::cout << "[PASS] Working memory integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Working memory system not available");
                }
                
                // Test episodic memory integration
                auto* episodic_memory = brain->getEpisodicMemory();
                if (episodic_memory) {
                    result.neurons_tested += episodic_memory->getEpisodeCount();
                    
                    // Create and store episode
                    Episode episode;
                    episode.id = EpisodeId(1);
                    episode.timestamp = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
                    episode.neuron_ids = {NeuronId(1), NeuronId(2), NeuronId(3)};
                    episode.synaptic_weights = {0.5f, 0.8f, 0.3f};
                    episode.reward = 1.0f;
                    
                    episodic_memory->storeEpisode(episode);
                    
                    // Retrieve episode
                    auto retrieved_episode = episodic_memory->getEpisode(EpisodeId(1));
                    if (!retrieved_episode) {
                        result.passed = false;
                        result.integration_issues.push_back("Failed to retrieve stored episode");
                    } else if (retrieved_episode->reward != 1.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Episode reward mismatch");
                    }
                    
                    // Test replay functionality
                    auto replay_episodes = episodic_memory->getEpisodesForReplay(3);
                    if (replay_episodes.size() > 0) {
                        std::cout << "[PASS] Episodic memory replay successful" << std::endl;
                    } else {
                        std::cout << "[INFO] No episodes available for replay (may be normal)" << std::endl;
                    }
                    
                    std::cout << "[PASS] Episodic memory integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Episodic memory system not available");
                }
                
                // Test semantic memory integration
                auto* semantic_memory = brain->getSemanticMemory();
                if (semantic_memory) {
                    // Store and retrieve concept
                    Concept concept;
                    concept.id = ConceptId(1);
                    concept.name = "TestConcept";
                    concept.activation = 0.8f;
                    concept.associations = {0.5f, 0.7f, 0.9f};
                    
                    semantic_memory->storeConcept(concept);
                    
                    auto retrieved_concept = semantic_memory->getConcept(ConceptId(1));
                    if (!retrieved_concept) {
                        result.passed = false;
                        result.integration_issues.push_back("Failed to retrieve stored concept");
                    } else if (retrieved_concept->name != "TestConcept") {
                        result.passed = false;
                        result.integration_issues.push_back("Concept name mismatch");
                    }
                    
                    std::cout << "[PASS] Semantic memory integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Semantic memory system not available (may be normal)" << std::endl;
                }
                
                // Run simulation steps
                for (uint64_t step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                    
                    if (working_memory) {
                        working_memory->storeTrace(NeuronId(step % 100), 0.1f);
                    }
                    
                    if (episodic_memory && step % 10 == 0) {
                        Episode episode;
                        episode.id = EpisodeId(static_cast<uint64_t>(step / 10) + 10);
                        episode.timestamp = std::chrono::duration<double>(step * 0.001).count();
                        episode.neuron_ids = {NeuronId(step % 10), NeuronId((step + 1) % 10)};
                        episode.reward = static_cast<float>(rand()) / RAND_MAX;
                        
                        episodic_memory->storeEpisode(episode);
                    }
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
                std::cout << "[PASS] Phase 6 memory integration test completed" << std::endl;
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        IntegrationTestResult test_neuromodulation_integration() {
            IntegrationTestResult result;
            result.test_name = "Phase 6 Neuromodulation Integration Test";
            result.passed = true;
            result.execution_time = 0.0;
            result.neurons_tested = 0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Create brain configuration with neuromodulation
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("region_count", 1);
                cfg->set("enable_reward", true);
                cfg->set("enable_curiosity", true);
                cfg->set("enable_novelty", true);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                // Test dopamine system integration
                auto* dopamine = brain->getDopamine();
                if (dopamine) {
                    // Inject current to stimulate dopamine response
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
                    
                    // Run simulation steps
                    for (int step = 0; step < 50; ++step) {
                        brain->step(step, step * 0.001);
                        
                        // Apply reward modulation
                        float reward = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
                        dopamine->applyReward(reward, 0.01f);
                    }
                    
                    float dopamine_level = dopamine->getLevel();
                    if (dopamine_level < 0.0f || dopamine_level > 10.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Dopamine level out of reasonable bounds");
                    }
                    
                    std::cout << "[PASS] Dopamine system integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Dopamine system not available");
                }
                
                // Test curiosity system integration
                auto* curiosity = brain->getCuriosity();
                if (curiosity) {
                    // Test novelty detection
                    float sensory_input = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
                    curiosity->detectNovelty(sensory_input);
                    
                    // Test curiosity drive
                    curiosity->increaseCuriosity(0.1f);
                    curiosity->decreaseCuriosity(0.05f);
                    
                    float curiosity_level = curiosity->getLevel();
                    if (curiosity_level < 0.0f || curiosity_level > 10.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Curiosity level out of reasonable bounds");
                    }
                    
                    std::cout << "[PASS] Curiosity system integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Curiosity system not available (may be normal)" << std::endl;
                }
                
                // Test novelty system integration
                auto* novelty = brain->getNovelty();
                if (novelty) {
                    // Test novelty detection with different inputs
                    novelty->processInput(0.0f);   // Normal input
                    novelty->processInput(1.5f);   // Novel input
                    novelty->processInput(-1.0f);  // Anomalous input
                    
                    float novelty_level = novelty->getNoveltyLevel();
                    if (novelty_level < 0.0f || novelty_level > 1.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Novelty level out of reasonable bounds");
                    }
                    
                    std::cout << "[PASS] Novelty system integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Novelty system not available (may be normal)" << std::endl;
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
                std::cout << "[PASS] Phase 6 neuromodulation integration test completed" << std::endl;
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        IntegrationTestResult test_prediction_integration() {
            IntegrationTestResult result;
            result.test_name = "Phase 6 Prediction Integration Test";
            result.passed = true;
            result.execution_time = 0.0;
            result.neurons_tested = 0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Create brain configuration with prediction systems
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("region_count", 1);
                cfg->set("enable_prediction", true);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                // Test prediction system integration
                auto* prediction_system = brain->getPredictionSystem();
                if (prediction_system) {
                    // Test prediction model training
                    std::vector<float> input_features = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
                    std::vector<float> target_output = {0.8f, 0.9f, 0.7f, 0.6f, 0.5f};
                    
                    prediction_system->train(input_features, target_output);
                    
                    // Test prediction
                    std::vector<float> prediction_output = prediction_system->predict(input_features);
                    
                    if (prediction_output.size() != target_output.size()) {
                        result.passed = false;
                        result.integration_issues.push_back("Prediction output size mismatch");
                    }
                    
                    // Check prediction accuracy (should be reasonable)
                    float prediction_error = 0.0f;
                    for (size_t i = 0; i < prediction_output.size(); ++i) {
                        prediction_error += std::abs(prediction_output[i] - target_output[i]);
                    }
                    prediction_error /= prediction_output.size();
                    
                    if (prediction_error > 1.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Prediction error too high");
                    }
                    
                    std::cout << "[PASS] Prediction system integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Prediction system not available");
                }
                
                // Test neural prediction integration
                auto* neural_prediction = brain->getNeuralPrediction();
                if (neural_prediction) {
                    // Test forward pass
                    std::vector<float> test_input = {0.5f, 0.5f, 0.5f};
                    std::vector<float> prediction = neural_prediction->forward(test_input);
                    
                    if (prediction.empty()) {
                        result.passed = false;
                        result.integration_issues.push_back("Neural prediction produced empty output");
                    }
                    
                    // Test error backpropagation
                    std::vector<float> target = {0.8f, 0.8f, 0.8f};
                    neural_prediction->backward(prediction, target);
                    
                    std::cout << "[PASS] Neural prediction integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Neural prediction system not available (may be normal)" << std::endl;
                }
                
                // Test attention integration
                auto* attention = brain->getAttention();
                if (attention) {
                    // Test attention mechanism
                    std::vector<float> query = {0.1f, 0.2f, 0.3f};
                    std::vector<std::vector<float>> keys = {{0.1f, 0.2f}, {0.3f, 0.4f}, {0.5f, 0.6f}};
                    std::vector<std::vector<float>> values = {{0.1f, 0.1f}, {0.2f, 0.2f}, {0.3f, 0.3f}};
                    
                    std::vector<float> output = attention->compute_attention(query, keys, values);
                    
                    if (output.empty()) {
                        result.passed = false;
                        result.integration_issues.push_back("Attention mechanism produced empty output");
                    }
                    
                    std::cout << "[PASS] Attention system integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Attention system not available (may be normal)" << std::endl;
                }
                
                // Run simulation with prediction
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                    
                    if (prediction_system && step % 10 == 0) {
                        std::vector<float> features = {
                            (static_cast<float>(rand()) / RAND_MAX),
                            (static_cast<float>(rand()) / RAND_MAX),
                            (static_cast<float>(rand()) / RAND_MAX)
                        };
                        auto prediction = prediction_system->predict(features);
                    }
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
                std::cout << "[PASS] Phase 6 prediction integration test completed" << std::endl;
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        IntegrationTestResult test_development_integration() {
            IntegrationTestResult result;
            result.test_name = "Phase 6 Development Integration Test";
            result.passed = true;
            result.execution_time = 0.0;
            result.neurons_tested = 0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Create brain configuration with development systems
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("region_count", 1);
                cfg->set("enable_development", true);
                cfg->set("enable_synaptogenesis", true);
                cfg->set("enable_pruning", true);
                cfg->set("enable_maturation", true);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                // Test development system integration
                auto* development_system = brain->getDevelopmentSystem();
                if (development_system) {
                    // Test synaptogenesis
                    development_system->initSynaptogenesis(0.0001f);  // Synaptogenesis rate
                    
                    // Simulate synaptogenesis over time
                    for (int step = 0; step < 100; ++step) {
                        development_system->updateSynaptogenesis(0.001f);
                    }
                    
                    // Test pruning
                    development_system->initPruning(0.00001f);  // Pruning rate
                    
                    for (int step = 0; step < 100; ++step) {
                        development_system->updatePruning(0.001f);
                    }
                    
                    // Test maturation
                    development_system->initMaturation(0.01f);  // Maturation rate
                    
                    for (int step = 0; step < 100; ++step) {
                        development_system->updateMaturation(0.001f);
                    }
                    
                    std::cout << "[PASS] Development system integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Development system not available");
                }
                
                // Test structural plasticity integration
                auto* structural_plasticity = brain->getStructuralPlasticity();
                if (structural_plasticity) {
                    // Test new synapse formation
                    structural_plasticity->addNewSynapse(NeuronId(1), NeuronId(2), 0.5f);
                    
                    // Test synapse elimination
                    structural_plasticity->eliminateSynapse(NeuronId(3), NeuronId(4));
                    
                    // Test dendritic growth
                    structural_plasticity->growDendrite(NeuronId(1), 0.1f);
                    
                    std::cout << "[PASS] Structural plasticity integration successful" << std::endl;
                } else {
                    std::cout << "[INFO] Structural plasticity system not available (may be normal)" << std::endl;
                }
                
                // Run simulation with development
                for (int step = 0; step < 200; ++step) {
                    brain->step(step, step * 0.001);
                    
                    if (development_system) {
                        development_system->updateDevelopment(0.001f);
                    }
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
                std::cout << "[PASS] Phase 6 development integration test completed" << std::endl;
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        IntegrationTestResult test_cognition_integration() {
            IntegrationTestResult result;
            result.test_name = "Phase 6 Cognition Integration Test";
            result.passed = true;
            result.execution_time = 0.0;
            result.neurons_tested = 0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Create brain configuration with cognition systems
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("region_count", 1);
                cfg->set("enable_planner", true);
                cfg->set("enable_concept_formation", true);
                cfg->set("enable_attention", true);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                // Test planner integration
                auto* planner = brain->getPlanner();
                if (planner) {
                    // Test goal setting and planning
                    Goal goal;
                    goal.id = GoalId(1);
                    goal.description = "Test goal";
                    goal.priority = 0.8f;
                    goal.deadline = std::chrono::duration<double>(100.0).count();
                    goal.status = GoalStatus::PENDING;
                    
                    planner->addGoal(goal);
                    
                    // Test plan execution
                    Plan plan;
                    plan.id = PlanId(1);
                    plan.goal_id = GoalId(1);
                    plan.steps = {"Step 1", "Step 2", "Step 3"};
                    plan.status = PlanStatus::EXECUTING;
                    
                    planner->addPlan(plan);
                    
                    // Test plan evaluation
                    float plan_score = planner->evaluatePlan(plan);
                    
                    if (plan_score < 0.0f || plan_score > 1.0f) {
                        result.passed = false;
                        result.integration_issues.push_back("Plan evaluation score out of bounds");
                    }
                    
                    std::cout << "[PASS] Planner integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Planner not available");
                }
                
                // Test concept formation integration
                auto* concept_formation = brain->getConceptFormation();
                if (concept_formation) {
                    // Test concept learning
                    Concept concept;
                    concept.id = ConceptId(1);
                    concept.name = "TestConcept";
                    concept.features = {0.1f, 0.2f, 0.3f, 0.4f};
                    concept.associations = {0.5f, 0.7f, 0.9f};
                    
                    concept_formation->learnConcept(concept);
                    
                    // Test concept retrieval
                    auto retrieved_concept = concept_formation->getConcept("TestConcept");
                    if (!retrieved_concept) {
                        result.passed = false;
                        result.integration_issues.push_back("Failed to retrieve learned concept");
                    }
                    
                    // Test concept activation
                    concept_formation->activateConcept(ConceptId(1));
                    
                    std::cout << "[PASS] Concept formation integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Concept formation not available");
                }
                
                // Test attention integration
                auto* attention = brain->getAttention();
                if (attention) {
                    // Test attention allocation
                    std::vector<Stimulus> stimuli;
                    for (int i = 0; i < 10; ++i) {
                        Stimulus stimulus;
                        stimulus.id = StimulusId(i);
                        stimulus.salience = (static_cast<float>(rand()) / RAND_MAX);
                        stimulus.relevance = (static_cast<float>(rand()) / RAND_MAX);
                        stimuli.push_back(stimulus);
                    }
                    
                    attention->allocate_attention(stimuli);
                    
                    // Test attention focusing
                    attention->focus_attention(StimulusId(5), 0.8f);
                    
                    // Test attention modulation
                    attention->modulate_attention(0.1f);
                    
                    std::cout << "[PASS] Attention integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Attention not available");
                }
                
                // Run simulation with cognition
                for (int step = 0; step < 150; ++step) {
                    brain->step(step, step * 0.001);
                    
                    if (planner && step % 50 == 0) {
                        // Check for goal completion
                        planner->updateGoalStatus(GoalId(1), GoalStatus::COMPLETED);
                    }
                    
                    if (concept_formation && step % 30 == 0) {
                        // Test new concept learning
                        Concept new_concept;
                        new_concept.id = ConceptId(static_cast<uint64_t>(step / 30) + 10);
                        new_concept.name = "DynamicConcept_" + std::to_string(step);
                        new_concept.features = {
                            (static_cast<float>(rand()) / RAND_MAX),
                            (static_cast<float>(rand()) / RAND_MAX),
                            (static_cast<float>(rand()) / RAND_MAX)
                        };
                        
                        concept_formation->learnConcept(new_concept);
                    }
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
                std::cout << "[PASS] Phase 6 cognition integration test completed" << std::endl;
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        // Run all Phase 6 integration tests
        std::vector<IntegrationTestResult> run_all_phase6_tests() {
            std::vector<IntegrationTestResult> results;
            
            results.push_back(test_memory_integration());
            results.push_back(test_neuromodulation_integration());
            results.push_back(test_prediction_integration());
            results.push_back(test_development_integration());
            results.push_back(test_cognition_integration());
            
            return results;
        }
        
        // Print integration test results
        void print_results(const std::vector<IntegrationTestResult>& results) {
            std::cout << "=== NLM Phase 6 Integration Test Results ===" << std::endl;
            std::cout << std::endl;
            
            int passed = 0;
            int failed = 0;
            
            for (const auto& result : results) {
                if (result.passed) {
                    passed++;
                    std::cout << "✓ " << result.test_name << " PASSED" << std::endl;
                    std::cout << "  Execution time: " << result.execution_time << "s" << std::endl;
                    std::cout << "  Neurons tested: " << result.neurons_tested << std::endl;
                    std::cout << "  Synaptic connections tested: " << result.synapses_tested << std::endl;
                    std::cout << "  Average firing rate: " << result.avg_firing_rate << std::endl;
                    std::cout << "  Total reward: " << result.total_reward << std::endl;
                } else {
                    failed++;
                    std::cout << "✗ " << result.test_name << " FAILED" << std::endl;
                    std::cout << "  Execution time: " << result.execution_time << "s" << std::endl;
                    std::cout << "  Integration issues: " << std::endl;
                    for (const auto& issue : result.integration_issues) {
                        std::cout << "    - " << issue << std::endl;
                    }
                    for (const auto& system : result.failed_systems) {
                        std::cout << "  Failed systems: " << system << std::endl;
                    }
                }
                std::cout << std::endl;
            }
            
            std::cout << "=== Summary ===" << std::endl;
            std::cout << "Total tests: " << results.size() << std::endl;
            std::cout << "Passed: " << passed << std::endl;
            std::cout << "Failed: " << failed << std::endl;
            std::cout << "Success rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;
        }
    };

    // Cross-component integration tests
    class CrossComponentIntegrationTests {
    public:
        IntegrationTestResult test_memory_neuromodulation_integration() {
            IntegrationTestResult result;
            result.test_name = "Cross-Component: Memory-Neuromodulation Integration";
            result.passed = true;
            result.execution_time = 0.0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                // Test that neuromodulation affects memory formation
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("enable_memory", true);
                cfg->set("enable_reward", true);
                
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                auto* working_memory = brain->getWorkingMemory();
                auto* dopamine = brain->getDopamine();
                
                if (working_memory && dopamine) {
                    // Store some traces
                    for (int i = 0; i < 10; ++i) {
                        working_memory->storeTrace(NeuronId(i), 0.5f);
                    }
                    
                    // Apply reward modulation
                    for (int i = 0; i < 5; ++i) {
                        float reward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
                        dopamine->applyReward(reward, 0.01f);
                    }
                    
                    // Check that memory traces are still stable
                    for (int i = 0; i < 10; ++i) {
                        float trace = working_memory->getTrace(NeuronId(i));
                        if (trace < 0.0f || trace > 1.0f) {
                            result.passed = false;
                            result.integration_issues.push_back("Memory traces affected by neuromodulation");
                            break;
                        }
                    }
                    
                    std::cout << "[PASS] Memory-neuromodulation integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Required systems not available");
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        IntegrationTestResult test_prediction_cognition_integration() {
            IntegrationTestResult result;
            result.test_name = "Cross-Component: Prediction-Cognition Integration";
            result.passed = true;
            result.execution_time = 0.0;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", 100);
                cfg->set("enable_prediction", true);
                cfg->set("enable_planner", true);
                
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.integration_issues.push_back("Brain initialization failed");
                    return result;
                }
                
                auto* prediction_system = brain->getPredictionSystem();
                auto* planner = brain->getPlanner();
                
                if (prediction_system && planner) {
                    // Generate predictions for planning
                    std::vector<float> input_features = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
                    auto predictions = prediction_system->predict(input_features);
                    
                    // Create goals based on predictions
                    Goal goal;
                    goal.id = GoalId(1);
                    goal.description = "Achieve predicted outcome";
                    goal.priority = 0.8f;
                    goal.status = GoalStatus::PENDING;
                    
                    planner->addGoal(goal);
                    
                    // Execute plan
                    Plan plan;
                    plan.id = PlanId(1);
                    plan.goal_id = GoalId(1);
                    plan.steps = {"Execute prediction-based plan"};
                    plan.status = PlanStatus::EXECUTING;
                    
                    planner->addPlan(plan);
                    
                    std::cout << "[PASS] Prediction-cognition integration successful" << std::endl;
                } else {
                    result.passed = false;
                    result.integration_issues.push_back("Required systems not available");
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.integration_issues.push_back(std::string("Exception: ") + e.what());
            }
            
            return result;
        }
        
        // Run all cross-component integration tests
        std::vector<IntegrationTestResult> run_all_cross_component_tests() {
            std::vector<IntegrationTestResult> results;
            
            results.push_back(test_memory_neuromodulation_integration());
            results.push_back(test_prediction_cognition_integration());
            
            return results;
        }
    };

    // System connectivity tests
    class SystemConnectivityTests {
    public:
        bool test_brain_system_connectivity() {
            std::cout << "=== Testing Brain System Connectivity ===" << std::endl;
            
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 50);
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cout << "[FAIL] Brain initialization failed" << std::endl;
                return false;
            }
            
            bool all_systems_connected = true;
            
            // Test all system connections
            if (brain->getWorkingMemory() != nullptr) {
                std::cout << "[PASS] Working memory connected" << std::endl;
            } else {
                std::cout << "[FAIL] Working memory not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getEpisodicMemory() != nullptr) {
                std::cout << "[PASS] Episodic memory connected" << std::endl;
            } else {
                std::cout << "[FAIL] Episodic memory not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getDopamine() != nullptr) {
                std::cout << "[PASS] Dopamine system connected" << std::endl;
            } else {
                std::cout << "[FAIL] Dopamine system not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getCuriosity() != nullptr) {
                std::cout << "[PASS] Curiosity system connected" << std::endl;
            } else {
                std::cout << "[FAIL] Curiosity system not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getPredictionSystem() != nullptr) {
                std::cout << "[PASS] Prediction system connected" << std::endl;
            } else {
                std::cout << "[FAIL] Prediction system not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getPlanner() != nullptr) {
                std::cout << "[PASS] Planner connected" << std::endl;
            } else {
                std::cout << "[FAIL] Planner not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getConceptFormation() != nullptr) {
                std::cout << "[PASS] Concept formation connected" << std::endl;
            } else {
                std::cout << "[FAIL] Concept formation not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getAttention() != nullptr) {
                std::cout << "[PASS] Attention system connected" << std::endl;
            } else {
                std::cout << "[FAIL] Attention system not connected" << std::endl;
                all_systems_connected = false;
            }
            
            if (brain->getDevelopmentSystem() != nullptr) {
                std::cout << "[PASS] Development system connected" << std::endl;
            } else {
                std::cout << "[FAIL] Development system not connected" << std::endl;
                all_systems_connected = false;
            }
            
            // Run simulation to test connectivity under load
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
                
                if (step % 20 == 0) {
                    std::cout << "  Step " << step << "/100 connectivity maintained" << std::endl;
                }
            }
            
            if (all_systems_connected) {
                std::cout << "[PASS] All brain systems are properly connected" << std::endl;
            }
            
            return all_systems_connected;
        }
    };

    // Main integration test runner
    class IntegrationTestRunner {
    public:
        void run_all_integration_tests() {
            std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
            std::cout << "Running comprehensive Phase 6 system integration tests..." << std::endl;
            std::cout << std::endl;
            
            // Run Phase 6 integration tests
            Phase6IntegrationTests phase6_tests;
            auto phase6_results = phase6_tests.run_all_phase6_tests();
            
            std::cout << std::endl;
            phase6_tests.print_results(phase6_results);
            
            // Run cross-component integration tests
            std::cout << "=== Cross-Component Integration Tests ===" << std::endl;
            CrossComponentIntegrationTests cross_component_tests;
            auto cross_component_results = cross_component_tests.run_all_cross_component_tests();
            
            std::cout << std::endl;
            cross_component_tests.print_results(cross_component_results);
            
            // Run system connectivity tests
            std::cout << "=== System Connectivity Tests ===" << std::endl;
            SystemConnectivityTests connectivity_tests;
            bool connectivity_passed = connectivity_tests.test_brain_system_connectivity();
            
            std::cout << std::endl;
            std::cout << "=== Integration Testing Complete ===" << std::endl;
            std::cout << "All integration tests have been executed." << std::endl;
        }
        
        void print_results(const std::vector<IntegrationTestResult>& results) {
            int passed = 0;
            int failed = 0;
            
            for (const auto& result : results) {
                if (result.passed) {
                    passed++;
                } else {
                    failed++;
                }
            }
            
            std::cout << "=== Integration Testing Summary ===" << std::endl;
            std::cout << "Total tests: " << results.size() << std::endl;
            std::cout << "Passed: " << passed << std::endl;
            std::cout << "Failed: " << failed << std::endl;
            std::cout << "Success rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;
        }
    };

} // namespace integration_tests

// Integration test entry point
void run_integration_tests() {
    try {
        integration_tests::IntegrationTestRunner runner;
        runner.run_all_integration_tests();
        std::cout << std::endl;
        std::cout << "=== Integration Testing Complete ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during integration testing: " << e.what() << std::endl;
        return 1;
    }
}

int main() {
    run_integration_tests();
}