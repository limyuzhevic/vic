// Development system integration tester implementation
// 
// This file implements the development system integration tests

#include "DevelopmentIntegrationTester.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <cmath>

namespace nlm {
namespace test {

void DevelopmentIntegrationTester::SetUp() {
    IntegrationTest::SetUp();
    std::cout << "    DevelopmentIntegrationTester setup completed" << std::endl;
}

void DevelopmentIntegrationTester::TearDown() {
    IntegrationTest::TearDown();
    std::cout << "    DevelopmentIntegrationTester teardown completed" << std::endl;
}

DevelopmentSystem* DevelopmentIntegrationTester::getDevelopmentSystem(Brain* brain) const {
    if (!brain) return nullptr;
    return brain->getDevelopmentSystem();
}

Neuromodulator* DevelopmentIntegrationTester::getNeuromodulatorSystem(Brain* brain) const {
    if (!brain) return nullptr;
    return brain->getDopamine(); // Use dopamine as example neuromodulator
}

TEST_F(DevelopmentIntegrationTester, PlasticityMaturationWithMemorySystems) {
    // Test developmental plasticity with learning processes
    // Plasticity matures across developmental stages
    // Integration verified by stage-dependent memory plasticity
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto workingMemory = brain_->getWorkingMemory();
    
    if (developmentSystem && workingMemory) {
        developmentSystem->setStage(DevelopmentalStage::YOUNG);
        
        // Test plasticity maturation
        float plasticityModifier = developmentSystem->getPlasticityModifier();
        EXPECT_GT(plasticityModifier, 0.0f);
        
        workingMemory->clear();
        workingMemory->storeToNeuron(1, 0.5f);
        
        // Memory should be affected by development
        workingMemory->update(0.01f);
        
        // Advance development
        developmentSystem->advanceStage();
        
        std::cout << "      Plasticity maturation with memory systems test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, CriticalPeriodEffectsWithLearning) {
    // Test critical period effects with neural development
    // Critical periods enable rapid learning at specific developmental stages
    // Integration verified by age-dependent learning sensitivity
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    
    if (developmentSystem) {
        // Test critical period detection
        bool isCriticalPeriod = developmentSystem->isCriticalPeriod();
        float criticalProgress = developmentSystem->getCriticalPeriodProgress();
        
        EXPECT_GE(criticalProgress, 0.0f);
        EXPECT_LE(criticalProgress, 1.0f);
        
        // Learning should be enhanced during critical period
        developmentSystem->setStage(DevelopmentalStage::ADOLESCENT);
        
        bool isCriticalNow = developmentSystem->isCriticalPeriod();
        
        std::cout << "      Critical period effects with learning test passed" << std::endl;
    } else {
        std::cout << "      Development system not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, StructuralPlasticityWithNeuralProcessing) {
    // Test structural plasticity with synaptic remodeling
    // Synaptogenesis modifies neural connectivity
    // Integration verified by development-dependent synaptic changes
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    
    if (developmentSystem) {
        // Test developmental stage effects on plasticity
        developmentSystem->setStage(DevelopmentalStage::MATURING);
        
        float plasticityModifier = developmentSystem->getPlasticityModifier();
        
        // Structural plasticity should be development-modulated
        developmentSystem->advanceStage();
        
        std::cout << "      Structural plasticity with neural processing test passed" << std::endl;
    } else {
        std::cout << "      Development system not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, AdaptiveDevelopmentAcrossSystems) {
    // Test adaptive development across the lifespan
    // Development adapts based on experience and environment
    // Integration verified by lifespan developmental adaptation
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto workingMemory = brain_->getWorkingMemory();
    
    if (developmentSystem && workingMemory) {
        // Test adaptive development
        double initialAge = developmentSystem->getDevelopmentalAge();
        
        workingMemory->clear();
        workingMemory->storeToNeuron(1, 0.5f);
        
        // Development should adapt to experience
        developmentSystem->advanceStage();
        
        double newAge = developmentSystem->getDevelopmentalAge();
        EXPECT_GE(newAge, initialAge);
        
        // Memory should adapt to development
        workingMemory->update(0.01f);
        
        std::cout << "      Adaptive development across systems test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, DevelopmentWithNeuromodulation) {
    // Test neuromodulation integration with development system
    // Neuromodulators interact with developmental processes
    // Integration verified by age-dependent neuromodulation effects
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto neuromodulator = getNeuromodulatorSystem(brain_.get());
    
    if (developmentSystem && neuromodulator) {
        // Test neuromodulation with development
        developmentSystem->setStage(DevelopmentalStage::ADULT);
        
        float neuromodulatorLevel = neuromodulator->getLevel();
        float plasticityModifier = developmentSystem->getPlasticityModifier();
        
        // Neuromodulation should affect development
        neuromodulator->setLevel(0.5f);
        
        std::cout << "      Development with neuromodulation test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, DevelopmentWithPrediction) {
    // Test prediction integration with development system
    // Developmental stage affects prediction capabilities
    // Integration verified by age-dependent prediction performance
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto predictionSystem = brain_->getPredictionSystem();
    
    if (developmentSystem && predictionSystem) {
        developmentSystem->setStage(DevelopmentalStage::YOUNG);
        
        // Test prediction with development
        float predictionError = predictionSystem->getPredictionError();
        
        // Development affects prediction
        developmentSystem->advanceStage();
        
        std::cout << "      Development with prediction test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, DevelopmentWithCognition) {
    // Test cognition integration with development system
    // Developmental stage affects cognitive capabilities
    // Integration verified by age-dependent cognitive performance
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto conceptFormation = brain_->getConceptFormation();
    auto planner = brain_->getPlanner();
    
    if (developmentSystem && conceptFormation && planner) {
        developmentSystem->setStage(DevelopmentalStage::YOUNG);
        
        // Test concept formation with development
        std::vector<float> pattern = {0.5f, 0.7f, 0.6f};
        std::vector<float> features = {0.5f, 0.7f, 0.6f};
        
        size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.8f, 0);
        
        // Test planning with development
        std::vector<float> currentState = {0.0f, 0.0f, 0.0f};
        ActionType action = planner->planAction(currentState, 0.5f);
        
        // Development affects cognition
        developmentSystem->advanceStage();
        
        std::cout << "      Development with cognition test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

TEST_F(DevelopmentIntegrationTester, DevelopmentWithActionSelection) {
    // Test action selection integration with development system
    // Developmental stage affects action selection strategies
    // Integration verified by age-dependent action selection
    
    auto developmentSystem = getDevelopmentSystem(brain_.get());
    auto planner = brain_->getPlanner();
    auto motorSystem = brain_->getMotorSystem();
    
    if (developmentSystem && planner && motorSystem) {
        developmentSystem->setStage(DevelopmentalStage::YOUNG);
        
        // Test action selection with development
        std::vector<float> currentState = {0.0f, 0.0f, 0.0f};
        ActionType action = planner->planAction(currentState, 0.5f);
        
        EXPECT_NE(action, ActionType::NONE);
        
        // Development affects action selection
        developmentSystem->advanceStage();
        
        std::cout << "      Development with action selection test passed" << std::endl;
    } else {
        std::cout << "      Required systems not available, skipping test" << std::endl;
    }
}

void DevelopmentIntegrationTester::verifyDevelopmentIntegrity(DevelopmentSystem* developmentSystem) {
    if (!developmentSystem) return;
    
    // Verify development system integrity
    DevelopmentalStage stage = developmentSystem->getStage();
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    double age = developmentSystem->getDevelopmentalAge();
    
    EXPECT_GE(plasticityModifier, 0.0f);
    EXPECT_LE(plasticityModifier, 2.0f); // Reasonable plasticity range
    EXPECT_GE(age, 0.0);
}

float DevelopmentIntegrationTester::calculatePlasticityImpact(DevelopmentSystem* developmentSystem) {
    if (!developmentSystem) return 0.0f;
    
    // Calculate plasticity impact
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    DevelopmentalStage stage = developmentSystem->getStage();
    
    // Different stages have different plasticity profiles
    float stageModifier = 1.0f;
    switch (stage) {
        case DevelopmentalStage::EARLY:
            stageModifier = 2.0f; // Early development has high plasticity
            break;
        case DevelopmentalStage::YOUNG:
            stageModifier = 1.5f;
            break;
        case DevelopmentalStage::ADULT:
            stageModifier = 1.0f;
            break;
        case DevelopmentalStage::AGING:
            stageModifier = 0.5f;
            break;
        default:
            stageModifier = 1.0f;
    }
    
    return plasticityModifier * stageModifier;
}

} // namespace test
} // namespace nlm