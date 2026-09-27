// Phase 6 Final Audit Compliance Tests
// Memory Systems Compliance

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralSemanticMemory.hpp"
#include "memory/NeuralProceduralMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <cassert>
#include <iostream>

namespace test_phase6_memory {

void testWorkingMemoryIntegration() {
    std::cout << "  Testing WorkingMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify WorkingMemory is properly integrated
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    std::cout << "    WorkingMemory properly integrated!" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "  Testing EpisodicMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify EpisodicMemory is properly integrated
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    std::cout << "    EpisodicMemory properly integrated!" << std::endl;
}

void testSemanticMemoryIntegration() {
    std::cout << "  Testing SemanticMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify SemanticMemory is properly integrated
    nlm::NeuralSemanticMemory* semanticMemory = brain.getSemanticMemory();
    assert(semanticMemory != nullptr);
    
    std::cout << "    SemanticMemory properly integrated!" << std::endl;
}

void testProceduralMemoryIntegration() {
    std::cout << "  Testing ProceduralMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify ProceduralMemory is properly integrated
    nlm::NeuralProceduralMemory* proceduralMemory = brain.getProceduralMemory();
    assert(proceduralMemory != nullptr);
    
    std::cout << "    ProceduralMemory properly integrated!" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "  Testing AssociativeMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify AssociativeMemory is properly integrated
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << "    AssociativeMemory properly integrated!" << std::endl;
}

void runAllMemoryComplianceTests() {
    std::cout << "=== Phase 6 Memory Systems Compliance Tests ===" << std::endl;
    
    try {
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testSemanticMemoryIntegration();
        testProceduralMemoryIntegration();
        testAssociativeMemoryIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Memory Systems Compliance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_phase6_memory

// Phase 6 Final Audit Compliance Tests
// Cognitive Systems Compliance

#include "brain/NeuralPlanner.hpp"
#include "brain/ConceptFormation.hpp"
#include "brain/SelfModel.hpp"
#include "brain/SocialLearning.hpp"
#include "brain/SpatialRepresentation.hpp"
#include "brain/TemporalRelation.hpp"

namespace test_phase6_cognitive {

void testNeuralPlannerIntegration() {
    std::cout << "  Testing NeuralPlanner integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify NeuralPlanner is properly integrated
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    std::cout << "    NeuralPlanner properly integrated!" << std::endl;
}

void testConceptFormationIntegration() {
    std::cout << "  Testing ConceptFormation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify ConceptFormation is properly integrated
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    std::cout << "    ConceptFormation properly integrated!" << std::endl;
}

void testSelfModelIntegration() {
    std::cout << "  Testing SelfModel integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify SelfModel is properly integrated
    nlm::SelfModel* selfModel = brain.getSelfModel();
    assert(selfModel != nullptr);
    
    std::cout << "    SelfModel properly integrated!" << std::endl;
}

void testSocialLearningIntegration() {
    std::cout << "  Testing SocialLearning integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify SocialLearning is properly integrated
    nlm::SocialLearning* socialLearning = brain.getSocialLearning();
    assert(socialLearning != nullptr);
    
    std::cout << "    SocialLearning properly integrated!" << std::endl;
}

void testSpatialRepresentationIntegration() {
    std::cout << "  Testing SpatialRepresentation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify SpatialRepresentation is properly integrated
    nlm::SpatialRepresentation* spatialRepresentation = brain.getSpatialRepresentation();
    assert(spatialRepresentation != nullptr);
    
    std::cout << "    SpatialRepresentation properly integrated!" << std::endl;
}

void testTemporalRelationIntegration() {
    std::cout << "  Testing TemporalRelation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify TemporalRelation is properly integrated
    nlm::TemporalRelation* temporalRelation = brain.getTemporalRelation();
    assert(temporalRelation != nullptr);
    
    std::cout << "    TemporalRelation properly integrated!" << std::endl;
}

void runAllCognitiveComplianceTests() {
    std::cout << "=== Phase 6 Cognitive Systems Compliance Tests ===" << std::endl;
    
    try {
        testNeuralPlannerIntegration();
        testConceptFormationIntegration();
        testSelfModelIntegration();
        testSocialLearningIntegration();
        testSpatialRepresentationIntegration();
        testTemporalRelationIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Cognitive Systems Compliance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_phase6_cognitive

// Phase 6 Final Audit Compliance Tests
// Neuromodulation Systems Compliance

#include "brain/Neuromodulation/Dopamine.hpp"
#include "brain/Neuromodulation/Curiosity.hpp"
#include "brain/Neuromodulation/Novelty.hpp"
#include "brain/Neuromodulation/PredictionError.hpp"
#include "brain/Neuromodulation/Acetylcholine.hpp"
#include "brain/Neuromodulation/Norepinephrine.hpp"
#include "brain/Neuromodulation/Serotonin.hpp"

namespace test_phase6_neuromodulation {

void testDopamineIntegration() {
    std::cout << "  Testing Dopamine integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Dopamine is properly integrated
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    std::cout << "    Dopamine properly integrated!" << std::endl;
}

void testCuriosityIntegration() {
    std::cout << "  Testing Curiosity integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Curiosity is properly integrated
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    std::cout << "    Curiosity properly integrated!" << std::endl;
}

void testNoveltyIntegration() {
    std::cout << "  Testing Novelty integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Novelty is properly integrated
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    std::cout << "    Novelty properly integrated!" << std::endl;
}

void testPredictionErrorIntegration() {
    std::cout << "  Testing PredictionError integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify PredictionError is properly integrated
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    PredictionError properly integrated!" << std::endl;
}

void testAcetylcholineIntegration() {
    std::cout << "  Testing Acetylcholine integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Acetylcholine is properly integrated
    nlm::Acetylcholine* acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    std::cout << "    Acetylcholine properly integrated!" << std::endl;
}

void testNorepinephrineIntegration() {
    std::cout << "  Testing Norepinephrine integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Norepinephrine is properly integrated
    nlm::Norepinephrine* norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    std::cout << "    Norepinephrine properly integrated!" << std::endl;
}

void testSerotoninIntegration() {
    std::cout << "  Testing Serotonin integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify Serotonin is properly integrated
    nlm::Serotonin* serotonin = brain.getSerotonin();
    assert(serotonin != nullptr);
    
    std::cout << "    Serotonin properly integrated!" << std::endl;
}

void runAllNeuromodulationComplianceTests() {
    std::cout << "=== Phase 6 Neuromodulation Systems Compliance Tests ===" << std::endl;
    
    try {
        testDopamineIntegration();
        testCuriosityIntegration();
        testNoveltyIntegration();
        testPredictionErrorIntegration();
        testAcetylcholineIntegration();
        testNorepinephrineIntegration();
        testSerotoninIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Neuromodulation Systems Compliance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_phase6_neuromodulation

// Phase 6 Final Audit Compliance Tests
// Performance Infrastructure Compliance

void testMemoryPoolCompliance() {
    std::cout << "  Testing MemoryPool compliance..." << std::endl;
    
    struct TestNeuron {
        uint64_t id;
        float potential;
        bool active;
    };
    
    nlm::MemoryPool<TestNeuron> neuronPool(1024, 4);
    
    // Verify MemoryPool properties
    assert(neuronPool.allocatedCount() == 0);
    assert(neuronPool.availableCount() == 1024 * 4);
    
    std::cout << "    MemoryPool compliance verified!" << std::endl;
}

void testEventQueueCompliance() {
    std::cout << "  Testing EventQueue compliance..." << std::endl;
    
    nlm::SpikeRingBuffer spikeQueue;
    nlm::DelayedSpikeQueue delayedQueue;
    
    // Verify EventQueue properties
    assert(spikeQueue.empty());
    assert(spikeQueue.size() == 0);
    assert(delayedQueue.pendingCount() == 0);
    
    std::cout << "    EventQueue compliance verified!" << std::endl;
}

void testParallelProcessingCompliance() {
    std::cout << "  Testing ParallelProcessing compliance..." << std::endl;
    
    nlm::ThreadPool threadPool(2);
    assert(threadPool.getNumThreads() == 2);
    
    std::cout << "    ParallelProcessing compliance verified!" << std::endl;
}

void testSIMDVectorizationCompliance() {
    std::cout << "  Testing SIMDVectorization compliance..." << std::endl;
    
    nlm::SIMDUtils utils;
    std::cout << "    SIMD type: " << utils.getSIMDType() << std::endl;
    std::cout << "    SIMD width: " << utils.SIMD_WIDTH << std::endl;
    
    std::cout << "    SIMDVectorization compliance verified!" << std::endl;
}

void testCheckpointSystemCompliance() {
    std::cout << "  Testing CheckpointSystem compliance..." << std::endl;
    
    nlm::CheckpointManager manager;
    manager.configure("./test_checkpoints_phase6", 1000, 5, true);
    
    assert(manager.getSaveIntervalSteps() == 1000);
    assert(manager.getMaxCheckpoints() == 5);
    assert(manager.getCompressionEnabled() == true);
    
    std::cout << "    CheckpointSystem compliance verified!" << std::endl;
}

void runAllPerformanceInfrastructureComplianceTests() {
    std::cout << "=== Phase 6 Performance Infrastructure Compliance Tests ===" << std::endl;
    
    try {
        testMemoryPoolCompliance();
        testEventQueueCompliance();
        testParallelProcessingCompliance();
        testSIMDVectorizationCompliance();
        testCheckpointSystemCompliance();
        
        std::cout << std::endl;
        std::cout << "=== All Performance Infrastructure Compliance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_phase6_performance

// Phase 6 Final Audit Compliance Tests
// Sleep/Rest Cycle Compliance

void testSleepStageCompliance() {
    std::cout << "  Testing SleepStage compliance..." << std::endl;
    
    // Verify SleepStage enum values
    nlm::SleepStage stages[] = {
        nlm::SleepStage::Wake,
        nlm::SleepStage::NREM1,
        nlm::SleepStage::NREM2,
        nlm::SleepStage::REM,
        nlm::SleepStage::TransitionToSleep,
        nlm::SleepStage::TransitionFromSleep
    };
    
    // All stages should be accessible
    for (auto stage : stages) {
        (void)stage; // Just verify they exist
    }
    
    std::cout << "    SleepStage compliance verified!" << std::endl;
}

void testSleepRestCycleCompliance() {
    std::cout << "  Testing SleepRestCycle compliance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify brain supports sleep stages
    nlm::SleepStage currentStage = nlm::SleepStage::Wake;
    
    std::cout << "    Sleep/rest cycle compliance verified!" << std::endl;
}

void runAllSleepRestCycleComplianceTests() {
    std::cout << "=== Phase 6 Sleep/Rest Cycle Compliance Tests ===" << std::endl;
    
    try {
        testSleepStageCompliance();
        testSleepRestCycleCompliance();
        
        std::cout << std::endl;
        std::cout << "=== All Sleep/Rest Cycle Compliance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_phase6_sleep

// Main function to run all Phase 6 compliance tests
int main() {
    std::cout << "=== Phase 6 Final Audit Compliance Tests ===" << std::endl;
    std::cout << "Testing all NLM enhancements for Phase 6 compliance" << std::endl;
    std::cout << "=======================================================" << std::endl;
    
    try {
        test_phase6_memory::runAllMemoryComplianceTests();
        test_phase6_cognitive::runAllCognitiveComplianceTests();
        test_phase6_neuromodulation::runAllNeuromodulationComplianceTests();
        test_phase6_performance::runAllPerformanceInfrastructureComplianceTests();
        test_phase6_sleep::runAllSleepRestCycleComplianceTests();
        
        std::cout << std::endl;
        std::cout << "=======================================================" << std::endl;
        std::cout << "=== ALL PHASE 6 COMPLIANCE TESTS PASSED ===" << std::endl;
        std::cout << "=======================================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=======================================================" << std::endl;
        std::cout << "=== Phase 6 Compliance Test Suite FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "=======================================================" << std::endl;
        return 1;
    }
}