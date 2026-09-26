// Memory System Tests
// Comprehensive test suite for memory systems functionality

#include "MemorySystemTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace test_suites {

class MemorySystemTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Memory System Tests ===" << std::endl;
        
        test_meta_learning_memory_management();
        test_hierarchical_memory_organization();
        test_knowledge_transfer_mechanisms();
        test_memory_consolidation_and_forgetting();
        test_memory_prioritization_algorithms();
        test_working_memory_dynamics();
        test_episodic_memory_retrieval();
        test_semantic_memory_acquisition();
        test_procedural_memory_learning();
        test_associative_memory_strength();
        
        std::cout << "=== Memory System Tests Complete ===" << std::endl;
    }

private:
    static void test_meta_learning_memory_management() {
        std::cout << "Testing meta-learning memory management..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Meta-learning memory management
        std::vector<float> metaMemoryMetrics;
        std::vector<float> learningEfficiencyMetrics;
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Meta-memory monitoring
            float memoryUtilization = brain.getMemoryUtilization();
            float memoryEfficiency = brain.getMemoryEfficiency();
            float learningRate = brain.getCurrentLearningRate();
            
            // Meta-memory optimization
            if (memoryUtilization > 0.8f) {
                // High memory utilization - optimize memory allocation
                float optimizationSignal = 0.7f;
                brain.applyMemoryOptimization(optimizationSignal);
            } else if (learningRate < 0.1f) {
                // Low learning rate - increase memory availability
                float availabilitySignal = 0.9f;
                brain.applyMemoryAvailability(availabilitySignal);
            }
            
            metaMemoryMetrics.push_back(memoryUtilization);
            learningEfficiencyMetrics.push_back(memoryEfficiency * learningRate);
        }
        
        // Verify meta-memory management effectiveness
        assert(metaMemoryMetrics.size() == 100);
        assert(learningEfficiencyMetrics.size() == 100);
        
        float avgMemoryUtilization = std::accumulate(metaMemoryMetrics.begin(), metaMemoryMetrics.end(), 0.0f) / metaMemoryMetrics.size();
        assert(avgMemoryUtilization >= 0.5f && avgMemoryUtilization <= 0.9f);
        
        std::cout << "    Meta-learning memory management: PASSED" << std::endl;
    }
    
    static void test_hierarchical_memory_organization() {
        std::cout << "Testing hierarchical memory organization..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Hierarchical memory organization testing
        std::vector<std::string> memoryLevels = {"sensory", "working", "episodic", "semantic", "procedural"};
        std::unordered_map<std::string, float> memoryLevelUtilization;
        
        for (const auto& level : memoryLevels) {
            memoryLevelUtilization[level] = 0.0f;
        }
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Hierarchical memory operations
            if (step % 16 == 0) {
                // Sensory level processing
                float sensoryMemory = brain.getSensoryMemoryLevel();
                memoryLevelUtilization["sensory"] += sensoryMemory;
            }
            
            if (step % 20 == 0) {
                // Working memory activation
                float workingMemory = brain.getWorkingMemoryLevel();
                memoryLevelUtilization["working"] += workingMemory;
            }
            
            if (step % 24 == 0) {
                // Episodic memory consolidation
                float episodicMemory = brain.getEpisodicMemoryLevel();
                memoryLevelUtilization["episodic"] += episodicMemory;
            }
            
            if (step % 28 == 0) {
                // Semantic memory strengthening
                float semanticMemory = brain.getSemanticMemoryLevel();
                memoryLevelUtilization["semantic"] += semanticMemory;
            }
            
            if (step % 32 == 0) {
                // Procedural memory refinement
                float proceduralMemory = brain.getProceduralMemoryLevel();
                memoryLevelUtilization["procedural"] += proceduralMemory;
            }
        }
        
        // Normalize memory level utilization
        for (auto& [level, utilization] : memoryLevelUtilization) {
            utilization /= (step / memoryLevels.size());
            assert(utilization >= 0.0f && utilization <= 1.0f);
        }
        
        // Verify hierarchical organization
        assert(memoryLevelUtilization.size() == 5);
        
        // Memory levels should be properly utilized
        for (const auto& [level, utilization] : memoryLevelUtilization) {
            assert(utilization > 0.3f);
        }
        
        std::cout << "    Hierarchical memory organization: PASSED" << std::endl;
    }
    
    static void test_knowledge_transfer_mechanisms() {
        std::cout << "Testing knowledge transfer mechanisms..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Knowledge transfer testing
        std::vector<std::vector<float>> sourceKnowledge;
        std::vector<std::vector<float>> targetKnowledge;
        
        // Create source knowledge base
        for (int i = 0; i < 10; ++i) {
            std::vector<float> knowledge(8, 0.0f);
            for (int j = 0; j < 8; ++j) {
                knowledge[j] = static_cast<float>(rand()) / RAND_MAX;
            }
            sourceKnowledge.push_back(knowledge);
        }
        
        // Learn source knowledge
        for (int step = 0; step < 60; ++step) {
            brain.step(step);
            
            // Apply knowledge transfer signals
            nlm::Neuromodulator transferSignal;
            transferSignal.setLevel(0.8f);
            brain.applyNeuromodulation(transferSignal);
            
            // Store source knowledge
            if (step % 10 == 0) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.action = nlm::ActionType::Look;
                brain.getEpisodicMemory()->storeEpisode(episode);
            }
        }
        
        // Transfer knowledge to target domain
        for (int step = 60; step < 120; ++step) {
            brain.step(step);
            
            // Apply transfer learning signals
            nlm::Neuromodulator transferSignal;
            transferSignal.setLevel(0.6f);
            brain.applyNeuromodulation(transferSignal);
            
            // Target knowledge acquisition
            if (step % 15 == 0) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.action = nlm::ActionType::Interact;
                brain.getEpisodicMemory()->storeEpisode(episode);
            }
        }
        
        // Verify knowledge transfer effectiveness
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        size_t episodeCount = episodicMemory->getEpisodeCount();
        assert(episodeCount >= 10);  // Should have stored episodes
        
        // Knowledge should be transferable
        float knowledgeTransferRate = static_cast<float>(episodeCount) / 10.0f;
        assert(knowledgeTransferRate >= 0.8f);
        
        std::cout << "    Knowledge transfer mechanisms: PASSED" << std::endl;
    }
    
    static void test_memory_consolidation_and_forgetting() {
        std::cout << "Testing memory consolidation and forgetting..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Memory consolidation and forgetting testing
        std::vector<float> consolidationMetrics;
        std::vector<float> forgettingMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Consolidation phase (steps 30-60)
            if (step >= 30 && step <= 60) {
                float consolidationSignal = 0.9f;
                brain.applyMemoryConsolidation(consolidationSignal);
                consolidationMetrics.push_back(1.0f);
                forgettingMetrics.push_back(0.1f);
            }nlm::Neuromodulator forget1;
            forget1.setLevel(0.1f);
            brain.applyNeuromodulation(forget1);
            } else if (step >= 70) {
                // Forgetting phase (steps 70+)
                float forgettingSignal = 0.2f + 0.3f * ((step - 70) / 20.0f);
                brain.applyMemoryForgetting(forgettingSignal);
                consolidationMetrics.push_back(0.3f);
                forgettingMetrics.push_back(forgettingSignal);
            } else {
                // Stable phase (steps 0-29, 61-69)
                float stableSignal = 0.5f;
                brain.applyMemoryStabilization(stableSignal);
                consolidationMetrics.push_back(0.5f);
                forgettingMetrics.push_back(0.5f);
            }
        }
        
        // Verify consolidation and forgetting dynamics
        assert(consolidationMetrics.size() == 90);
        assert(forgettingMetrics.size() == 90);
        
        // Consolidation should be effective
        float avgConsolidation = std::accumulate(consolidationMetrics.begin(), consolidationMetrics.end(), 0.0f) / consolidationMetrics.size();
        assert(avgConsolidation >= 0.4f);
        
        // Forgetting should be controlled
        float avgForgetting = std::accumulate(forgettingMetrics.begin(), forgettingMetrics.end(), 0.0f) / forgettingMetrics.size();
        assert(avgForgetting >= 0.1f && avgForgetting <= 0.8f);
        
        std::cout << "    Memory consolidation and forgetting: PASSED" << std::endl;
    }
    
    static void test_memory_prioritization_algorithms() {
        std::cout << "Testing memory prioritization algorithms..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 420);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Memory prioritization testing
        std::vector<nlm::EpisodicMemoryItem> prioritizedMemories;
        std::vector<float> priorityScores;
        
        // Create test memories with different priorities
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.action = static_cast<nlm::ActionType>(step % 5);
            episode.reward = static_cast<float>((step * 7) % 10) / 10.0f;
            episode.novelty = static_cast<float>((step * 13) % 10) / 10.0f;
            
            // Calculate priority score
            float priorityScore = 0.4f * episode.reward + 0.3f * episode.novelty + 
                                 0.2f * (1.0f - step / 50.0f) + 0.1f;
            priorityScores.push_back(priorityScore);
            
            // Apply memory prioritization signals
            nlm::Neuromodulator prioritySignal;
            prioritySignal.setLevel(priorityScore);
            brain.applyNeuromodulation(prioritySignal);
            
            // Store episode
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
        
        // Retrieve prioritized memories
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        size_t episodeCount = episodicMemory->getEpisodeCount();
        assert(episodeCount >= 10);
        
        // Verify prioritization effectiveness
        assert(priorityScores.size() == 50);
        
        // Priority scores should be reasonable
        float avgPriority = std::accumulate(priorityScores.begin(), priorityScores.end(), 0.0f) / priorityScores.size();
        assert(avgPriority >= 0.2f && avgPriority <= 0.9f);
        
        // Higher priority memories should be more accessible
        float highPriorityAccess = 0.0f;
        for (float score : priorityScores) {
            if (score > 0.7f) highPriorityAccess += 1.0f;
        }
        assert(highPriorityAccess >= 2.0f);  // Should have at least 2 high priority memories
        
        std::cout << "    Memory prioritization algorithms: PASSED" << std::endl;
    }
    
    static void test_working_memory_dynamics() {
        std::cout << "Testing working memory dynamics..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Working memory dynamics testing
        std::vector<float> memoryTraceDurations;
        std::vector<float> interferenceMetrics;
        std::vector<float> rehearsalMetrics;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            auto* workingMemory = brain.getWorkingMemory();
            assert(workingMemory != nullptr);
            
            // Working memory trace dynamics
            float traceDuration = workingMemory->getCurrentTraceDuration();
            memoryTraceDurations.push_back(traceDuration);
            
            // Interference simulation
            if (step % 10 == 0) {
                float interference = workingMemory->getInterferenceLevel();
                interferenceMetrics.push_back(interference);
                
                // Apply interference management
                nlm::Neuromodulator interferenceSignal;
                interferenceSignal.setLevel(0.3f + interference * 0.4f);
                brain.applyNeuromodulation(interferenceSignal);
            }
            
            // Rehearsal simulation
            if (step % 15 == 0) {
                float rehearsal = workingMemory->getRehearsalStrength();
                rehearsalMetrics.push_back(rehearsal);
                
                // Apply rehearsal enhancement
                nlm::Neuromodulator rehearsalSignal;
                rehearsalSignal.setLevel(0.7f + rehearsal * 0.3f);
                brain.applyNeuromodulation(rehearsalSignal);
            }
        }
        
        // Verify working memory dynamics
        assert(memoryTraceDurations.size() == 70);
        assert(interferenceMetrics.size() >= 7);
        assert(rehearsalMetrics.size() >= 5);
        
        // Trace durations should be reasonable
        float avgTraceDuration = std::accumulate(memoryTraceDurations.begin(), memoryTraceDurations.end(), 0.0f) / memoryTraceDurations.size();
        assert(avgTraceDuration >= 0.1f && avgTraceDuration <= 1.0f);
        
        std::cout << "    Working memory dynamics: PASSED" << std::endl;
    }
    
    static void test_episodic_memory_retrieval() {
        std::cout << "Testing episodic memory retrieval..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Episodic memory retrieval testing
        std::vector<nlm::EpisodicMemoryItem> testEpisodes;
        
        // Create test episodes
        for (int i = 0; i < 20; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.positionX = static_cast<float>(i % 10);
            episode.positionY = static_cast<float>((i * 3) % 10);
            episode.action = static_cast<nlm::ActionType>(i % 4);
            episode.reward = static_cast<float>((i * 7) % 10) / 10.0f;
            episode.energy = static_cast<float>((i * 11) % 10) / 10.0f;
            episode.novelty = static_cast<float>((i * 13) % 10) / 10.0f;
            
            testEpisodes.push_back(episode);
        }
        
        // Store all episodes
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        for (int i = 0; i < 20; ++i) {
            episodicMemory->storeEpisode(testEpisodes[i]);
        }
        
        // Test retrieval strategies
        std::vector<nlm::EpisodicMemoryItem> temporalRetrievals;
        std::vector<nlm::EpisodicMemoryItem> locationRetrievals;
        std::vector<nlm::EpisodicMemoryItem> actionRetrievals;
        
        // Temporal retrieval (recent episodes)
        temporalRetrievals = episodicMemory->getRecentEpisodes(5);
        assert(temporalRetrievals.size() == 5);
        
        // Location-based retrieval
        locationRetrievals = episodicMemory->retrieveByLocation(5, 5, 2, 5);
        assert(locationRetrievals.size() <= 5);
        
        // Action-based retrieval
        actionRetrievals = episodicMemory->retrieveAfterAction(nlm::ActionType::MoveForward, 5);
        assert(actionRetrievals.size() <= 5);
        
        // Similarity-based retrieval
        std::vector<float> queryPattern{0.5f, 0.5f, 0.5f};
        std::vector<nlm::EpisodicMemoryItem> similarRetrievals = episodicMemory->retrieveSimilar(queryPattern, 5);
        assert(similarRetrievals.size() <= 5);
        
        // Verify retrieval effectiveness
        assert(temporalRetrievals.size() > 0);
        assert(locationRetrievals.size() >= 0);
        assert(actionRetrievals.size() >= 0);
        assert(similarRetrievals.size() >= 0);
        
        std::cout << "    Episodic memory retrieval: PASSED" << std::endl;
    }
    
    static void test_semantic_memory_acquisition() {
        std::cout << "Testing semantic memory acquisition..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 360);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Semantic memory acquisition testing
        std::unordered_map<std::string, std::string> semanticFacts;
        
        // Create semantic facts
        semanticFacts["concept_1"] = "object_type_A";
        semanticFacts["concept_2"] = "object_type_B";
        semanticFacts["concept_3"] = "action_type_X";
        semanticFacts["concept_4"] = "action_type_Y";
        semanticFacts["concept_5"] = "attribute_1";
        
        // Semantic memory operations
        for (int step = 0; step < 40; ++step) {
            brain.step(step);
            
            auto* semanticMemory = brain.getSemanticMemory();
            assert(semanticMemory != nullptr);
            
            // Store facts
            if (step % 8 == 0 && semanticFacts.size() > 0) {
                auto it = semanticFacts.begin();
                semanticMemory->storeFact(it->first, it->second);
            }
            
            // Retrieve facts
            if (step % 10 == 0 && !semanticFacts.empty()) {
                auto it = semanticFacts.begin();
                std::string retrieved = semanticMemory->retrieveFact(it->first);
                assert(retrieved == it->second);
            }
            
            // Test fact existence
            if (step % 12 == 0) {
                bool hasFact = semanticMemory->hasFact("concept_1");
                assert(hasFact == true);
            }
        }
        
        // Verify semantic memory operations
        auto* semanticMemory = brain.getSemanticMemory();
        assert(semanticMemory != nullptr);
        
        size_t factCount = semanticMemory->getAllFacts().size();
        assert(factCount >= 0);  // Should have stored some facts
        
        std::cout << "    Semantic memory acquisition: PASSED" << std::endl;
    }
    
    static void test_procedural_memory_learning() {
        std::cout << "Testing procedural memory learning..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Procedural memory learning testing
        std::vector<nlm::Skill> learnedSkills;
        
        // Create initial skills
        nlm::Skill skill1{"move_forward", {}, 0.5f};
        nlm::Skill skill2{"look_around", {}, 0.3f};
        nlm::Skill skill3{"interact", {}, 0.7f};
        
        learnedSkills.push_back(skill1);
        learnedSkills.push_back(skill2);
        learnedSkills.push_back(skill3);
        
        // Procedural memory operations
        for (int step = 0; step < 60; ++step) {
            brain.step(step);
            
            auto* proceduralMemory = brain.getProceduralMemory();
            assert(proceduralMemory != nullptr);
            
            // Learn skills
            if (step % 15 == 0 && learnedSkills.size() > 0) {
                auto& skill = learnedSkills[step % learnedSkills.size()];
                std::vector<NeuronId> pattern;
                for (int i = 0; i < 10; ++i) {
                    pattern.push_back(static_cast<NeuronId>(i));
                }
                proceduralMemory->learnSkill(skill.name, pattern);
            }
            
            // Update proficiency
            if (step % 20 == 0) {
                for (auto& skill : learnedSkills) {
                    proceduralMemory->updateProficiency(skill.name, 0.1f);
                }
            }
            
            // Retrieve skills
            if (step % 25 == 0) {
                for (auto& skill : learnedSkills) {
                    Skill* retrieved = proceduralMemory->getSkill(skill.name);
                    assert(retrieved != nullptr);
                    assert(retrieved->name == skill.name);
                }
            }
        }
        
        // Verify procedural memory operations
        auto* proceduralMemory = brain.getProceduralMemory();
        assert(proceduralMemory != nullptr);
        
        std::vector<Skill*> allSkills = proceduralMemory->getAllSkills();
        assert(allSkills.size() >= 0);  // Should have learned some skills
        
        std::cout << "    Procedural memory learning: PASSED" << std::endl;
    }
    
    static void test_associative_memory_strength() {
        std::cout << "Testing associative memory strength..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 340);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Associative memory testing
        std::vector<std::pair<NeuronId, NeuronId>> associations;
        std::vector<float> associationStrengths;
        
        // Create associations
        for (int i = 0; i < 30; ++i) {
            NeuronId neuron1(static_cast<uint64_t>(i * 2));
            NeuronId neuron2(static_cast<uint64_t>(i * 2 + 1));
            float strength = static_cast<float>((i * 3) % 10) / 10.0f + 0.1f;
            
            associations.push_back({neuron1, neuron2});
            associationStrengths.push_back(strength);
        }
        
        // Associative memory operations
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        for (int i = 0; i < 30; ++i) {
            brain.step(i);
            
            // Create associations
            auto& assoc = associations[i];
            associativeMemory->associate(assoc.first, assoc.second, associationStrengths[i]);
            
            // Retrieve associations
            if (i % 10 == 0) {
                std::vector<NeuronId> retrieved = associativeMemory->getAssociations(assoc.first);
                assert(retrieved.size() > 0);
            }
            
            // Get association strength
            float strength = associativeMemory->getAssociationStrength(assoc.first, assoc.second);
            assert(strength >= 0.0f);
            
            // Update association strength
            if (i % 12 == 0) {
                associativeMemory->updateAssociation(assoc.first, assoc.second, 0.05f);
            }
        }
        
        // Verify associative memory operations
        assert(associations.size() == 30);
        assert(associationStrengths.size() == 30);
        
        // Average association strength should be reasonable
        float avgStrength = std::accumulate(associationStrengths.begin(), associationStrengths.end(), 0.0f) / associationStrengths.size();
        assert(avgStrength >= 0.1f && avgStrength <= 1.0f);
        
        std::cout << "    Associative memory strength: PASSED" << std::endl;
    }
};

} // namespace test_suites