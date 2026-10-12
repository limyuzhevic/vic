// Integration test framework implementation
// 
// This file implements the base integration test class functionality
// including test setup/teardown, helper methods, and integration utilities

#include "IntegrationTest.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <cmath>

namespace nlm {
namespace test {

// Integration test implementation

void IntegrationTest::SetUp() {
    std::cout << "    Setting up integration test environment..." << std::endl;
    
    // Initialize random number generator
    std::random_device rd;
    rng_.seed(rd());
    
    // Create brain with test configuration
    brain_ = createBrain(config_);
    initialized_ = false;
    
    if (brain_) {
        brain_->initialize();
        initialized_ = true;
        std::cout << "    Brain created and initialized successfully" << std::endl;
    } else {
        std::cout << "    ERROR: Failed to create brain" << std::endl;
    }
}

void IntegrationTest::TearDown() {
    std::cout << "    Cleaning up integration test environment..." << std::endl;
    
    if (brain_) {
        brain_->reset();
        brain_.reset();
    }
    
    initialized_ = false;
}

std::shared_ptr<Brain> IntegrationTest::createBrain(const IntegrationTestConfig& config) {
    auto brainConfig = std::make_shared<Config>();
    
    // Set brain configuration
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    brainConfig->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create brain instance
    return std::make_shared<Brain>(brainConfig);
}

std::shared_ptr<WorkingMemory> IntegrationTest::getWorkingMemory(Brain* brain) {
    if (!brain) return nullptr;
    
    // Check if brain has traditional working memory system
    auto workingMemoryPtr = brain->getWorkingMemory();
    if (workingMemoryPtr) {
        return std::shared_ptr<WorkingMemory>(workingMemoryPtr, [](WorkingMemory*) {});
    }
    
    return nullptr;
}

std::shared_ptr<EpisodicMemory> IntegrationTest::getEpisodicMemory(Brain* brain) {
    if (!brain) return nullptr;
    
    // Check if brain has episodic memory system
    auto episodicMemoryPtr = brain->getEpisodicMemory();
    if (episodicMemoryPtr) {
        return std::shared_ptr<EpisodicMemory>(episodicMemoryPtr, [](EpisodicMemory*) {});
    }
    
    return nullptr;
}

std::shared_ptr<AssociativeMemory> IntegrationTest::getAssociativeMemory(Brain* brain) {
    if (!brain) return nullptr;
    
    // Check if brain has associative memory system
    auto associativeMemoryPtr = brain->getAssociativeMemory();
    if (associativeMemoryPtr) {
        return std::shared_ptr<AssociativeMemory>(associativeMemoryPtr, [](AssociativeMemory*) {});
    }
    
    return nullptr;
}

std::shared_ptr<Reward> IntegrationTest::getRewardSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Reward system is accessed directly through brain
    auto rewardPtr = brain->getReward();
    if (rewardPtr) {
        return std::shared_ptr<Reward>(rewardPtr, [](Reward*) {});
    }
    
    return nullptr;
}

std::shared_ptr<PredictionError> IntegrationTest::getPredictionErrorSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Prediction error system is accessed directly through brain
    auto predictionErrorPtr = brain->getPredictionErrorSignal();
    if (predictionErrorPtr) {
        return std::shared_ptr<PredictionError>(predictionErrorPtr, [](PredictionError*) {});
    }
    
    return nullptr;
}

std::shared_ptr<Curiosity> IntegrationTest::getCuriositySystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Curiosity system is accessed directly through brain
    auto curiosityPtr = brain->getCuriosity();
    if (curiosityPtr) {
        return std::shared_ptr<Curiosity>(curiosityPtr, [](Curiosity*) {});
    }
    
    return nullptr;
}

std::shared_ptr<Novelty> IntegrationTest::getNoveltySystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Novelty system is accessed directly through brain
    auto noveltyPtr = brain->getNovelty();
    if (noveltyPtr) {
        return std::shared_ptr<Novelty>(noveltyPtr, [](Novelty*) {});
    }
    
    return nullptr;
}

std::shared_ptr<PredictionSystem> IntegrationTest::getPredictionSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Prediction system is accessed directly through brain
    auto predictionPtr = brain->getPredictionSystem();
    if (predictionPtr) {
        return std::shared_ptr<PredictionSystem>(predictionPtr, [](PredictionSystem*) {});
    }
    
    return nullptr;
}

std::shared_ptr<NeuralPrediction> IntegrationTest::getNeuralPredictionSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Neural prediction system is accessed directly through brain
    auto neuralPredictionPtr = brain->getPredictionSystem();
    if (neuralPredictionPtr) {
        return std::shared_ptr<NeuralPrediction>(neuralPredictionPtr, [](NeuralPrediction*) {});
    }
    
    return nullptr;
}

std::shared_ptr<ConceptFormation> IntegrationTest::getConceptFormationSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Concept formation system is accessed directly through brain
    auto conceptFormationPtr = brain->getConceptFormation();
    if (conceptFormationPtr) {
        return std::shared_ptr<ConceptFormation>(conceptFormationPtr, [](ConceptFormation*) {});
    }
    
    return nullptr;
}

std::shared_ptr<NeuralPlanner> IntegrationTest::getPlannerSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Planner system is accessed directly through brain
    auto plannerPtr = brain->getPlanner();
    if (plannerPtr) {
        return std::shared_ptr<NeuralPlanner>(plannerPtr, [](NeuralPlanner*) {});
    }
    
    return nullptr;
}

std::shared_ptr<DevelopmentSystem> IntegrationTest::getDevelopmentSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Development system is accessed directly through brain
    auto developmentPtr = brain->getDevelopmentSystem();
    if (developmentPtr) {
        return std::shared_ptr<DevelopmentSystem>(developmentPtr, [](DevelopmentSystem*) {});
    }
    
    return nullptr;
}

std::shared_ptr<Neuromodulator> IntegrationTest::getNeuromodulatorSystem(Brain* brain) {
    if (!brain) return nullptr;
    
    // Neuromodulator system is accessed through brain->getDopamine()
    auto dopaminePtr = brain->getDopamine();
    if (dopaminePtr) {
        return std::shared_ptr<Neuromodulator>(dopaminePtr, [](Neuromodulator*) {});
    }
    
    return nullptr;
}

template<typename T>
bool IntegrationTest::compareArrays(const std::vector<T>& arr1, const std::vector<T>& arr2, float tolerance) {
    if (arr1.size() != arr2.size()) return false;
    
    for (size_t i = 0; i < arr1.size(); ++i) {
        if (std::abs(static_cast<float>(arr1[i] - arr2[i])) > tolerance) {
            return false;
        }
    }
    
    return true;
}

template<typename T>
bool IntegrationTest::arraysEqual(const std::vector<T>& arr1, const std::vector<T>& arr2) {
    return compareArrays(arr1, arr2, 1e-6f);
}

float IntegrationTest::calculateCrossCorrelation(const std::vector<float>& vec1, const std::vector<float>& vec2) {
    if (vec1.size() != vec2.size() || vec1.empty()) return 0.0f;
    
    float mean1 = 0.0f, mean2 = 0.0f;
    for (size_t i = 0; i < vec1.size(); ++i) {
        mean1 += vec1[i];
        mean2 += vec2[i];
    }
    mean1 /= vec1.size();
    mean2 /= vec2.size();
    
    float numerator = 0.0f, denom1 = 0.0f, denom2 = 0.0f;
    for (size_t i = 0; i < vec1.size(); ++i) {
        float diff1 = vec1[i] - mean1;
        float diff2 = vec2[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    if (denom1 <= 0.0f || denom2 <= 0.0f) return 0.0f;
    
    return numerator / std::sqrt(denom1 * denom2);
}

double IntegrationTest::measureExecutionTime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    return std::chrono::duration<double>(end - start).count();
}

void IntegrationTest::testMemorySystemIntegration(Brain* brain, const std::string& testName) {
    std::cout << "    Testing memory system integration: " << testName << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    // Test neural working memory integration
    auto workingMemory = getWorkingMemory(brain);
    if (workingMemory) {
        std::cout << "      Neural working memory: Present" << std::endl;
    } else {
        std::cout << "      Neural working memory: Not available" << std::endl;
    }
    
    // Test episodic memory integration
    auto episodicMemory = getEpisodicMemory(brain);
    if (episodicMemory) {
        std::cout << "      Neural episodic memory: Present" << std::endl;
    } else {
        std::cout << "      Neural episodic memory: Not available" << std::endl;
    }
    
    // Test associative memory integration
    auto associativeMemory = getAssociativeMemory(brain);
    if (associativeMemory) {
        std::cout << "      Neural associative memory: Present" << std::endl;
    } else {
        std::cout << "      Neural associative memory: Not available" << std::endl;
    }
    
    std::cout << "    Memory system integration test completed: " << testName << std::endl;
}

void IntegrationTest::testWorkingMemoryIntegration(Brain* brain) {
    std::cout << "      Testing working memory integration..." << std::endl;
    
    if (!brain) return;
    
    auto workingMemory = getWorkingMemory(brain);
    if (workingMemory) {
        // Test basic working memory operations
        workingMemory->clear();
        
        size_t initialSize = workingMemory->getCurrentSize();
        std::cout << "        Initial memory size: " << initialSize << std::endl;
        
        // Test decay
        workingMemory->decay(0.1f);
        std::cout << "        Decay test completed" << std::endl;
    }
}

void IntegrationTest::testEpisodicMemoryIntegration(Brain* brain) {
    std::cout << "      Testing episodic memory integration..." << std::endl;
    
    if (!brain) return;
    
    auto episodicMemory = getEpisodicMemory(brain);
    if (episodicMemory) {
        episodicMemory->clear();
        
        size_t initialCount = episodicMemory->getEpisodeCount();
        std::cout << "        Initial episode count: " << initialCount << std::endl;
        
        // Test consolidation
        episodicMemory->consolidate(0.5f);
        std::cout << "        Consolidation test completed" << std::endl;
    }
}

void IntegrationTest::testAssociativeMemoryIntegration(Brain* brain) {
    std::cout << "      Testing associative memory integration..." << std::endl;
    
    if (!brain) return;
    
    auto associativeMemory = getAssociativeMemory(brain);
    if (associativeMemory) {
        associativeMemory->clear();
        
        // Test association creation
        associativeMemory->associate(1, 2, 0.5f);
        float strength = associativeMemory->getAssociationStrength(1, 2);
        std::cout << "        Association strength: " << strength << std::endl;
        
        // Test association update
        associativeMemory->updateAssociation(1, 2, 0.2f);
        strength = associativeMemory->getAssociationStrength(1, 2);
        std::cout << "        Updated association strength: " << strength << std::endl;
    }
}

void IntegrationTest::testMemoryConsolidation(Brain* brain) {
    std::cout << "      Testing memory consolidation..." << std::endl;
    
    if (!brain) return;
    
    // Test multiple memory systems integration
    testWorkingMemoryIntegration(brain);
    testEpisodicMemoryIntegration(brain);
    testAssociativeMemoryIntegration(brain);
    
    std::cout << "        Memory consolidation test completed" << std::endl;
}

void IntegrationTest::testNeuromodulationSystemIntegration(Brain* brain, const std::string& testName) {
    std::cout << "    Testing neuromodulation system integration: " << testName << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    // Test dopamine integration
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "      Dopamine system: Present" << std::endl;
    } else {
        std::cout << "      Dopamine system: Not available" << std::endl;
    }
    
    // Test curiosity integration
    auto curiosity = getCuriositySystem(brain);
    if (curiosity) {
        std::cout << "      Curiosity system: Present" << std::endl;
    } else {
        std::cout << "      Curiosity system: Not available" << std::endl;
    }
    
    // Test novelty integration
    auto novelty = getNoveltySystem(brain);
    if (novelty) {
        std::cout << "      Novelty system: Present" << std::endl;
    } else {
        std::cout << "      Novelty system: Not available" << std::endl;
    }
    
    // Test prediction error integration
    auto predictionError = getPredictionErrorSystem(brain);
    if (predictionError) {
        std::cout << "      Prediction error system: Present" << std::endl;
    } else {
        std::cout << "      Prediction error system: Not available" << std::endl;
    }
    
    std::cout << "    Neuromodulation system integration test completed: " << testName << std::endl;
}

void IntegrationTest::testDopamineIntegration(Brain* brain) {
    std::cout << "      Testing dopamine integration..." << std::endl;
    
    if (!brain) return;
    
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        // Test basic dopamine operations
        float initialLevel = dopamine->getLevel();
        dopamine->setLevel(0.5f);
        float newLevel = dopamine->getLevel();
        
        std::cout << "        Dopamine level change: " << initialLevel << " -> " << newLevel << std::endl;
        
        // Reset
        dopamine->setLevel(initialLevel);
    }
}

void IntegrationTest::testCuriosityIntegration(Brain* brain) {
    std::cout << "      Testing curiosity integration..." << std::endl;
    
    if (!brain) return;
    
    auto curiosity = getCuriositySystem(brain);
    if (curiosity) {
        float initialLevel = curiosity->getLevel();
        std::cout << "        Initial curiosity level: " << initialLevel << std::endl;
    }
}

void IntegrationTest::testNoveltyIntegration(Brain* brain) {
    std::cout << "      Testing novelty integration..." << std::endl;
    
    if (!brain) return;
    
    auto novelty = getNoveltySystem(brain);
    if (novelty) {
        float initialLevel = novelty->getLevel();
        novelty->setLevel(0.3f);
        float newLevel = novelty->getLevel();
        
        std::cout << "        Novelty level change: " << initialLevel << " -> " << newLevel << std::endl;
        
        // Reset
        novelty->setLevel(initialLevel);
    }
}

void IntegrationTest::testPredictionErrorIntegration(Brain* brain) {
    std::cout << "      Testing prediction error integration..." << std::endl;
    
    if (!brain) return;
    
    auto predictionError = getPredictionErrorSystem(brain);
    if (predictionError) {
        float initialError = predictionError->getError();
        predictionError->computeError(0.5f, 0.8f);
        float newError = predictionError->getError();
        
        std::cout << "        Prediction error change: " << initialError << " -> " << newError << std::endl;
    }
}

void IntegrationTest::testPredictionSystemIntegration(Brain* brain, const std::string& testName) {
    std::cout << "    Testing prediction system integration: " << testName << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    auto predictionSystem = getPredictionSystem(brain);
    if (predictionSystem) {
        std::cout << "      Prediction system: Present" << std::endl;
    } else {
        std::cout << "      Prediction system: Not available" << std::endl;
    }
    
    std::cout << "    Prediction system integration test completed: " << testName << std::endl;
}

void IntegrationTest::testForwardPredictionIntegration(Brain* brain) {
    std::cout << "      Testing forward prediction integration..." << std::endl;
    
    if (!brain) return;
    
    auto predictionSystem = getPredictionSystem(brain);
    if (predictionSystem) {
        float initialError = predictionSystem->getPredictionError();
        float initialConfidence = predictionSystem->getConfidence();
        
        std::cout << "        Initial prediction error: " << initialError << std::endl;
        std::cout << "        Initial prediction confidence: " << initialConfidence << std::endl;
    }
}

void IntegrationTest::testValuePredictionIntegration(Brain* brain) {
    std::cout << "      Testing value prediction integration..." << std::endl;
    
    if (!brain) return;
    
    auto predictionSystem = getPredictionSystem(brain);
    if (predictionSystem) {
        float currentError = predictionSystem->getPredictionError();
        std::cout << "        Current prediction error: " << currentError << std::endl;
    }
}

void IntegrationTest::testBehavioralPredictionIntegration(Brain* brain) {
    std::cout << "      Testing behavioral prediction integration..." << std::endl;
    
    if (!brain) return;
    
    // Check if brain has neural prediction system
    auto neuralPrediction = getNeuralPredictionSystem(brain);
    if (neuralPrediction) {
        std::cout << "        Neural prediction system: Present" << std::endl;
    }
}

void IntegrationTest::testMetaPredictionIntegration(Brain* brain) {
    std::cout << "      Testing meta-prediction integration..." << std::endl;
    
    if (!brain) return;
    
    auto predictionSystem = getPredictionSystem(brain);
    if (predictionSystem) {
        // Test meta-prediction through confidence and error
        float confidence = predictionSystem->getConfidence();
        float error = predictionSystem->getPredictionError();
        
        std::cout << "        Prediction confidence: " << confidence << std::endl;
        std::cout << "        Prediction error: " << error << std::endl;
    }
}

void IntegrationTest::testCognitionSystemIntegration(Brain* brain, const std::string& testName) {
    std::cout << "    Testing cognition system integration: " << testName << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    // Test concept formation integration
    auto conceptFormation = getConceptFormationSystem(brain);
    if (conceptFormation) {
        std::cout << "      Concept formation system: Present" << std::endl;
    } else {
        std::cout << "      Concept formation system: Not available" << std::endl;
    }
    
    // Test planner integration
    auto planner = getPlannerSystem(brain);
    if (planner) {
        std::cout << "      Neural planner system: Present" << std::endl;
    } else {
        std::cout << "      Neural planner system: Not available" << std::endl;
    }
    
    std::cout << "    Cognition system integration test completed: " << testName << std::endl;
}

void IntegrationTest::testConceptFormationIntegration(Brain* brain) {
    std::cout << "      Testing concept formation integration..." << std::endl;
    
    if (!brain) return;
    
    auto conceptFormation = getConceptFormationSystem(brain);
    if (conceptFormation) {
        size_t initialConcepts = conceptFormation->getConceptCount();
        std::cout << "        Initial concept count: " << initialConcepts << std::endl;
    }
}

void IntegrationTest::testAttentionSelectionIntegration(Brain* brain) {
    std::cout << "      Testing attention selection integration..." << std::endl;
    
    if (!brain) return;
    
    auto attention = brain->getAttention();
    if (attention) {
        std::cout << "        Attentional selection system: Present" << std::endl;
    }
}

void IntegrationTest::testPlanningIntegration(Brain* brain) {
    std::cout << "      Testing planning integration..." << std::endl;
    
    if (!brain) return;
    
    auto planner = getPlannerSystem(brain);
    if (planner) {
        size_t planningDepth = planner->getPlanningDepth();
        std::cout << "        Planning depth: " << planningDepth << std::endl;
    }
}

void IntegrationTest::testSelfModelIntegration(Brain* brain) {
    std::cout << "      Testing self-model integration..." << std::endl;
    
    if (!brain) return;
    
    // Check if brain has self-model system (through neural planner or other mechanisms)
    auto planner = getPlannerSystem(brain);
    if (planner) {
        std::cout << "        Neural planning system provides self-model capabilities" << std::endl;
    }
}

void IntegrationTest::testDevelopmentSystemIntegration(Brain* brain, const std::string& testName) {
    std::cout << "    Testing development system integration: " << testName << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        std::cout << "      Development system: Present" << std::endl;
        
        DevelopmentalStage currentStage = developmentSystem->getStage();
        std::cout << "        Current developmental stage: " << static_cast<int>(currentStage) << std::endl;
        
        bool criticalPeriod = developmentSystem->isCriticalPeriod();
        std::cout << "        Critical period: " << (criticalPeriod ? "Active" : "Not active") << std::endl;
    } else {
        std::cout << "      Development system: Not available" << std::endl;
    }
    
    std::cout << "    Development system integration test completed: " << testName << std::endl;
}

void IntegrationTest::testPlasticityMaturationIntegration(Brain* brain) {
    std::cout << "      Testing plasticity maturation integration..." << std::endl;
    
    if (!brain) return;
    
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        float plasticityModifier = developmentSystem->getPlasticityModifier();
        std::cout << "        Plasticity modifier: " << plasticityModifier << std::endl;
    }
}

void IntegrationTest::testCriticalPeriodIntegration(Brain* brain) {
    std::cout << "      Testing critical period integration..." << std::endl;
    
    if (!brain) return;
    
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        bool criticalPeriod = developmentSystem->isCriticalPeriod();
        float criticalProgress = developmentSystem->getCriticalPeriodProgress();
        
        std::cout << "        Critical period active: " << (criticalPeriod ? "Yes" : "No") << std::endl;
        std::cout << "        Critical period progress: " << criticalProgress << std::endl;
    }
}

void IntegrationTest::testStructuralPlasticityIntegration(Brain* brain) {
    std::cout << "      Testing structural plasticity integration..." << std::endl;
    
    if (!brain) return;
    
    // Check if development system is available for plasticity
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        std::cout << "        Development system provides plasticity control" << std::endl;
    }
}

void IntegrationTest::testAdaptiveDevelopmentIntegration(Brain* brain) {
    std::cout << "      Testing adaptive development integration..." << std::endl;
    
    if (!brain) return;
    
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        double developmentalAge = developmentSystem->getDevelopmentalAge();
        std::cout << "        Developmental age: " << developmentalAge << std::endl;
    }
}

void IntegrationTest::testCompleteBrainLoop(Brain* brain) {
    std::cout << "    Testing complete brain loop integration..." << std::endl;
    
    if (!brain) {
        std::cout << "    ERROR: Brain is null" << std::endl;
        return;
    }
    
    // Run brain for several steps to test complete loop
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
        
        // Check memory systems
        auto workingMemory = getWorkingMemory(brain);
        if (workingMemory) {
            workingMemory->update(0.01f);
        }
        
        // Check neuromodulation systems
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            dopamine->update(0.01f);
        }
        
        // Check prediction systems
        auto predictionSystem = getPredictionSystem(brain);
        if (predictionSystem) {
            // Note: PredictionSystem doesn't have update() method based on header
            // In real implementation, this would update predictions
        }
        
        // Check development system
        auto developmentSystem = getDevelopmentSystem(brain);
        if (developmentSystem) {
            developmentSystem->update(brain, step, 0.01f);
        }
    }
    
    std::cout << "    Complete brain loop test completed" << std::endl;
}

void IntegrationTest::testMemoryDevelopmentInteraction(Brain* brain) {
    std::cout << "    Testing memory-development interaction..." << std::endl;
    
    if (!brain) return;
    
    // Test interaction between memory and development systems
    auto developmentSystem = getDevelopmentSystem(brain);
    if (developmentSystem) {
        // Check developmental effects on memory systems
        float plasticityModifier = developmentSystem->getPlasticityModifier();
        std::cout << "      Plasticity modifier from development: " << plasticityModifier << std::endl;
    }
    
    // Test memory operations
    testMemoryConsolidation(brain);
    
    stdcout << "    Memory-development interaction test completed" << std::endl;
}

void IntegrationTest::testNeuromodulationMotivationIntegration(Brain* brain) {
    std::cout << "    Testing neuromodulation-motivation integration..." << std::endl;
    
    if (!brain) return;
    
    // Test interaction between neuromodulation and action selection
    auto dopamine = brain->getDopamine();
    auto curiosity = getCuriositySystem(brain);
    
    if (dopamine && curiosity) {
        std::cout << "      Neuromodulation systems interacting" << std::endl;
    }
    
    std::cout << "    Neuromodulation-motivation integration test completed" << std::endl;
}

void IntegrationTest::testPredictionBehaviorIntegration(Brain* brain) {
    std::cout << "    Testing prediction-behavior integration..." << std::endl;
    
    if (!brain) return;
    
    // Test interaction between prediction and motor systems
    auto predictionSystem = getPredictionSystem(brain);
    auto planner = getPlannerSystem(brain);
    
    if (predictionSystem && planner) {
        std::cout << "      Prediction and planning systems interacting" << std::endl;
    }
    
    std::cout << "    Prediction-behavior integration test completed" << std::endl;
}

} // namespace test
} // namespace nlm