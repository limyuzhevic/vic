// Phase 6 Experiment Implementation
// Integrates all brain systems and demonstrates complete functionality

#include "experiments/Phase6Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

Phase6Experiment::Phase6Experiment()
    : Experiment("Phase 6: Final Integration", "Integration of all brain systems")
{
    // Initialize experiment metrics
    auto& metrics = getMetrics();
    metrics.recordMetric("memory_systems", 2.0f);
    metrics.recordMetric("neuromodulation_systems", 5.0f);
    metrics.recordMetric("cognition_systems", 2.0f);
    metrics.recordMetric("prediction_system", 1.0f);
    metrics.recordMetric("development_system", 1.0f);
}

Phase6Experiment::~Phase6Experiment() = default;

bool Phase6Experiment::initialize() {
    NLM_LOG_INFO("Initializing Phase 6 Integration Experiment");
    NLM_LOG_INFO("This experiment demonstrates the complete integrated brain.");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Systems to Test ===");
    NLM_LOG_INFO("1. Memory Systems (Working + Episodic + Associative)");
    NLM_LOG_INFO("2. Neuromodulation Systems (Dopamine, Curiosity, Novelty, etc.)");
    NLM_LOG_INFO("3. Cognition Systems (Planning, Concept Formation, Attention)");
    NLM_LOG_INFO("4. Prediction System");
    NLM_LOG_INFO("5. Development System");
    NLM_LOG_INFO("6. Integration (all systems working together)");
    
    // Create experiment runner
    experimentRunner_ = std::make_unique<Phase6IntegratedExperiment>();
    
    return true;
}

Phase6IntegrationResult Phase6Experiment::run(const Phase6Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Max Steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Replay: " + std::string(config.enableReplay ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "enabled" : "disabled"));
    
    return experimentRunner_->run(config);
}

Phase6IntegrationResult Phase6Experiment::run(const ExperimentConfig& config) {
    // Convert ExperimentConfig to Phase6Config
    Phase6Config phase6Config;
    
    // Extract parameters from ExperimentConfig
    if (config.hasParameter("neuronCount")) {
        phase6Config.neuronCount = config.getParameter<size_t>("neuronCount");
    }
    
    if (config.hasParameter("maxSteps")) {
        phase6Config.maxSteps = config.getParameter<uint64_t>("maxSteps");
    }
    
    phase6Config.enableCheckpointing = config.getParameter<bool>("enableCheckpointing", true);
    phase6Config.enableReplay = config.getParameter<bool>("enableReplay", true);
    phase6Config.enableDevelopment = config.getParameter<bool>("enableDevelopment", true);
    
    return run(phase6Config);
}

bool Phase6Experiment::runTests() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Phase 6 Integration Tests ===");
    
    if (!experimentRunner_) {
        NLM_LOG_ERROR("Experiment runner not initialized");
        return false;
    }
    
    // Run integration verification
    NLM_LOG_INFO("Running integration verification...");
    bool integrationOK = experimentRunner_->verifyIntegration();
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        return false;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    
    // Run system integration tests
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Running System Integration Tests ---");
    
    // Test memory integration
    NLM_LOG_INFO("Testing memory integration...");
    bool memoryOK = experimentRunner_->testMemoryIntegration();
    if (!memoryOK) {
        NLM_LOG_ERROR("Memory integration test failed!");
        return false;
    }
    NLM_LOG_INFO("Memory integration test passed!");
    
    // Test neuromodulation integration
    NLM_LOG_INFO("Testing neuromodulation integration...");
    bool neuromodulationOK = experimentRunner_->testNeuromodulationIntegration();
    if (!neuromodulationOK) {
        NLM_LOG_ERROR("Neuromodulation integration test failed!");
        return false;
    }
    NLM_LOG_INFO("Neuromodulation integration test passed!");
    
    // Test checkpointing
    NLM_LOG_INFO("Testing checkpointing...");
    bool checkpointingOK = experimentRunner_->testCheckpointing();
    if (!checkpointingOK) {
        NLM_LOG_ERROR("Checkpointing test failed!");
        return false;
    }
    NLM_LOG_INFO("Checkpointing test passed!");
    
    // Test replay
    NLM_LOG_INFO("Testing replay system...");
    bool replayOK = experimentRunner_->testReplay();
    if (!replayOK) {
        NLM_LOG_ERROR("Replay test failed!");
        return false;
    }
    NLM_LOG_INFO("Replay test passed!");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Phase 6 Integration Tests Passed! ===");
    
    return true;
}

std::string Phase6Experiment::getDescription() const {
    return "Phase 6: Final Integration of All Brain Systems\n\n"
           "This experiment tests the complete integrated NLM brain architecture.\n"
           "It verifies that all brain systems (memory, neuromodulation, cognition, prediction, development)\n"
           "are properly connected and functioning together as a unified artificial brain.\n\n"
           "Key achievements demonstrated:\n"
           "- Memory systems integrated with neural processing\n"
           "- Neuromodulation affects plasticity and neural dynamics\n"
           "- Development affects plasticity rates over time\n"
           "- Prediction system integrated with perception\n"
           "- Replay and consolidation functional\n"
           "- Checkpoint save/load working\n"
           "- Complete brain loop functions coherently";
}

} // namespace nlm
