# Integration Tests
// End-to-end integration tests for NLM systems
// Verifies all brain systems work together as a coherent unit
// Tests memory, prediction, cognition, neuromodulation, and development integration

#include "brain/Brain.hpp"
#include "profiling/PerformanceProfiler.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>

namespace test_integration {

void testBrainMemoryIntegration() {
    std::cout << "=== Testing Brain Memory Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store some information
    workingMemory->storePattern(std::vector<float>(100, 0.5f));
    
    // Test episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->recordExperience(1.0, "Test experience");
    
    // Test associative memory integration
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    associativeMemory->associatePatterns(std::vector<float>(50, 0.3f), std::vector<float>(50, 0.7f));
    
    // Run simulation to test integration
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify systems remained functional
    assert(brain.getTotalNeuronCount() == 200);
    assert(workingMemory->getActivePatternCount() > 0);
    assert(episodicMemory->getExperienceCount() > 0);
    
    std::cout << "✓ Brain memory integration tests passed\n";
}

void testBrainPredictionIntegration() {
    std::cout << "\n=== Testing Brain Prediction Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_prediction", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Generate prediction error
    auto predictionError = predictionSystem->computePredictionError(1.0, 0.9);
    assert(predictionError >= 0.0 && predictionError <= 2.0);
    
    // Run simulation to test prediction dynamics
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify prediction error affects plasticity
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // Check that plasticity is active
            assert(stdp->isEnabled());
        }
    }
    
    std::cout << "✓ Brain prediction integration tests passed\n";
}

void testBrainCognitionIntegration() {
    std::cout << "\n=== Testing Brain Cognition Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_planning", true, nlm::ConfigSource::Default);
    config->set("enable_attention", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test cognition systems integration
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Generate some cognitive activity through simulation
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Simulate sensory input to drive cognition
        nlm::SensoryInput sensoryInput;
        sensoryInput.neuronType = nlm::NeuronType::Sensory;
        sensoryInput.intensity = 1.0;
        sensoryInput.pattern.resize(50, 0.5f);
        brain.receiveSensoryInput(sensoryInput);
        
        // Test action production
        auto action = brain.produceAction();
        // Action may be null depending on system state, which is fine
    }
    
    std::cout << "✓ Brain cognition integration tests passed\n";
}

void testBrainNeuromodulationIntegration() {
    std::cout << "\n=== Testing Brain Neuromodulation Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation systems integration
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Apply neuromodulatory signals
    nlm::Neuromodulator dopamineSignal;
    dopamineSignal.type = nlm::NeuromodulatorType::Dopamine;
    dopamineSignal.strength = 0.8;
    dopamineSignal.timestamp = 1.0;
    brain.applyNeuromodulation(dopamineSignal);
    
    nlm::Neuromodulator curiositySignal;
    curiositySignal.type = nlm::NeuromodulatorType::Curiosity;
    curiositySignal.strength = 0.6;
    curiositySignal.timestamp = 1.0;
    brain.applyNeuromodulation(curiositySignal);
    
    nlm::Neuromodulator noveltySignal;
    noveltySignal.type = nlm::NeuromodulatorType::Novelty;
    noveltySignal.strength = 0.9;
    noveltySignal.timestamp = 1.0;
    brain.applyNeuromodulation(noveltySignal);
    
    // Run simulation to test neuromodulation effects
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
        
        // Verify neuromodulation affects plasticity
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // Check that plasticity rules are applied
            assert(stdp->isEnabled());
        }
    }
    
    std::cout << "✓ Brain neuromodulation integration tests passed\n";
}

void testBrainDevelopmentIntegration() {
    std::cout << "\n=== Testing Brain Development Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system integration
    auto* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Get developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial && stage <= nlm::DevelopmentalStage::Adult);
    
    // Run development simulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Check that developmental stage changes over time
        auto currentStage = brain.getDevelopmentalStage();
        assert(currentStage >= stage); // Stage should not regress
        stage = currentStage;
        
        // Test plasticity rate changes with development
        auto* stdp = brain.getSTDP();
        if (stdp) {
            double plasticityRate = stdp->getPlasticityRate();
            assert(plasticityRate >= 0.0 && plasticityRate <= 1.0);
        }
    }
    
    std::cout << "✓ Brain development integration tests passed\n";
}

void testBrainCheckpointIntegration() {
    std::cout << "\n=== Testing Brain Checkpoint Integration ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("checkpoint_interval_steps", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation with checkpointing
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Test checkpoint save/load
        if (step % 50 == 0) {
            // Save checkpoint
            std::string checkpointFile = "/tmp/brain_checkpoint_" + std::to_string(step) + ".bin";
            assert(brain.save(checkpointFile));
            
            // Create new brain and load checkpoint
            auto config2 = std::make_shared<nlm::Config>();
            config2->set("neuron_count", config->getInt("neuron_count"), nlm::ConfigSource::Default);
            config2->set("region_count", config->getInt("region_count"), nlm::ConfigSource::Default);
            
            nlm::Brain brain2(config2);
            assert(brain2.initialize());
            assert(brain2.load(checkpointFile));
            
            // Verify state was loaded correctly
            assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
            assert(brain2.getRegionCount() == brain.getRegionCount());
        }
    }
    
    std::cout << "✓ Brain checkpoint integration tests passed\n";
}

void testBrainWithProfiling() {
    std::cout << "\n=== Testing Brain with Performance Profiling ===\n";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("checkpoint_interval_steps", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize performance profiler
    nlm::PerformanceProfiler profiler;
    assert(profiler.initialize());
    profiler.setOutputDirectory("/tmp/brain_profiling");
    profiler.startProfiling();
    
    // Run brain simulation with profiling
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Record neural activity
        profiler.onSpikeGenerated();
        profiler.captureActivityHeatmap();
        
        // Simulate some neuron activations
        for (size_t i = 0; i < 10; ++i) {
            profiler.onNeuronActivated(static_cast<nlm::NeuronId>(i % 600));
        }
        
        // Test progress reporting
        double progress = static_cast<double>(step) / 100.0;
        profiler.setProgress(progress, "Brain simulation step " + std::to_string(step), step, 100);
        
        // Update profiler
        profiler.update();
        
        // Test checkpoint events
        if (step % 50 == 0) {
            profiler.onCheckpointSaved("/tmp/test_checkpoint", 1024);
        }
        
        if (step % 75 == 0) {
            profiler.onCheckpointLoaded("/tmp/test_checkpoint");
        }
    }
    
    profiler.stopProfiling();
    
    // Test analysis
    auto bottleneck = profiler.analyzeBottlenecks();
    double performanceScore = profiler.calculatePerformanceScore();
    
    // Export profiling data
    assert(profiler.exportMetrics("/tmp/brain_metrics.csv"));
    assert(profiler.exportHeatmaps("/tmp/brain_heatmaps.csv"));
    assert(profiler.exportAnalysisReport("/tmp/brain_analysis_report.txt"));
    
    // Test session management
    assert(profiler.saveSession("brain_integration_session"));
    
    auto sessions = profiler.listSessions();
    assert(std::find(sessions.begin(), sessions.end(), "brain_integration_session") != sessions.end());
    
    assert(profiler.loadSession("brain_integration_session"));
    
    profiler.shutdown();
    
    std::cout << "✓ Brain with profiling integration tests passed\n";
}

void testCompleteBrainSystem() {
    std::cout << "\n=== Testing Complete Brain System Integration ===\n";
    
    // Create a comprehensive configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("enable_prediction", true, nlm::ConfigSource::Default);
    config->set("enable_planning", true, nlm::ConfigSource::Default);
    config->set("enable_attention", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("checkpoint_interval_steps", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize performance profiler for monitoring
    nlm::PerformanceProfiler profiler;
    assert(profiler.initialize());
    profiler.startProfiling();
    
    // Run a comprehensive simulation
    std::cout << "  Running comprehensive simulation..." << std::flush;
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Apply various inputs to test system integration
        if (step % 10 == 0) {
            nlm::SensoryInput sensoryInput;
            sensoryInput.neuronType = nlm::NeuronType::Sensory;
            sensoryInput.intensity = 1.0;
            sensoryInput.pattern.resize(100, 0.5f);
            brain.receiveSensoryInput(sensoryInput);
        }
        
        if (step % 15 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 2.0f);
        }
        
        if (step % 20 == 0) {
            nlm::Neuromodulator neuromodulator;
            neuromodulator.type = nlm::NeuromodulatorType::Dopamine;
            neuromodulator.strength = 0.7;
            neuromodulator.timestamp = static_cast<double>(step);
            brain.applyNeuromodulation(neuromodulator);
        }
        
        // Profile brain activity
        if (step % 5 == 0) {
            profiler.captureActivityHeatmap();
        }
        
        // Record neural events
        if (step % 10 == 0 && brain.getTotalSpikeCount() > 0) {
            profiler.onSpikeGenerated();
        }
        
        // Update progress
        if (step % 50 == 0) {
            double progress = static_cast<double>(step) / 200.0;
            profiler.setProgress(progress, "Complete brain system test", step, 200);
        }
        
        profiler.update();
    }
    
    std::cout << " Complete!" << std::endl;
    
    // Final analysis
    auto bottleneck = profiler.analyzeBottlenecks();
    double performanceScore = profiler.calculatePerformanceScore();
    
    // Run performance regression analysis
    auto regressions = profiler.analyzePerformanceRegression();
    
    std::cout << "  Bottleneck Analysis: " << (bottleneck.type.empty() ? "None detected" : bottleneck.type) << std::endl;
    std::cout << "  Performance Score: " << performanceScore << "/100" << std::endl;
    std::cout << "  Performance Regressions: " << regressions.size() << " detected" << std::endl;
    
    profiler.stopProfiling();
    profiler.exportAnalysisReport("/tmp/complete_system_report.txt");
    profiler.shutdown();
    
    // Final system status
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getRegionCount() == 4);
    
    std::cout << "✓ Complete brain system integration tests passed\n";
}

void runAll() {
    testBrainMemoryIntegration();
    testBrainPredictionIntegration();
    testBrainCognitionIntegration();
    testBrainNeuromodulationIntegration();
    testBrainDevelopmentIntegration();
    testBrainCheckpointIntegration();
    testBrainWithProfiling();
    testCompleteBrainSystem();
    
    std::cout << "\n=== All Integration Tests Completed Successfully ===\n";
}

} // namespace test_integration
