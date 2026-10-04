// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Advanced Configuration Tests
void testAdvancedBrainConfiguration() {
    nlm::Config config;
    
    // Test brain configuration methods
    config.setBrainType("Phase6_Integrated");
    config.setNeuronCount(2000);
    config.setRegionCount(4);
    config.setConnectionProbability(0.15f);
    
    assert(config.getOr<std::string>("brain_type", "") == "Phase6_Integrated");
    assert(config.getOr<size_t>("neuron_count", 0) == 2000);
    assert(config.getOr<size_t>("region_count", 0) == 4);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    std::cout << "    testAdvancedBrainConfiguration passed" << std::endl;
}

void testAdvancedMemoryConfiguration() {
    nlm::Config config;
    
    // Test memory configuration methods
    config.setWorkingMemoryCapacity(500);
    config.setWorkingMemoryDecayRate(0.02f);
    config.setEpisodicMemoryMaxEpisodes(5000);
    config.setEpisodicMemoryConsolidationThreshold(0.7f);
    
    assert(config.getOr<size_t>("working_memory_capacity", 0) == 500);
    assert(config.getOr<float>("working_memory_decay_rate", 0.0f) == 0.02f);
    assert(config.getOr<size_t>("episodic_memory_max_episodes", 0) == 5000);
    assert(config.getOr<float>("episodic_memory_consolidation_threshold", 0.0f) == 0.7f);
    
    std::cout << "    testAdvancedMemoryConfiguration passed" << std::endl;
}

void testAdvancedPredictionConfiguration() {
    nlm::Config config;
    
    // Test prediction configuration methods
    config.setPredictionDepth(8);
    config.setPredictionConfidenceThreshold(0.6f);
    config.setPredictionErrorSmoothing(0.9f);
    
    assert(config.getOr<int>("prediction_depth", 0) == 8);
    assert(config.getOr<float>("prediction_confidence_threshold", 0.0f) == 0.6f);
    assert(config.getOr<float>("prediction_error_smoothing", 0.0f) == 0.9f);
    
    std::cout << "    testAdvancedPredictionConfiguration passed" << std::endl;
}

void testAdvancedCognitionConfiguration() {
    nlm::Config config;
    
    // Test cognition configuration methods
    config.setPlannerPlanningDepth(5);
    config.setConceptFormationStabilityThreshold(0.8f);
    config.setAttentionInhibitionStrength(0.7f);
    config.setAttentionExcitationStrength(1.2f);
    
    assert(config.getOr<int>("planner_planning_depth", 0) == 5);
    assert(config.getOr<float>("concept_formation_stability_threshold", 0.0f) == 0.8f);
    assert(config.getOr<float>("attention_inhibition_strength", 0.0f) == 0.7f);
    assert(config.getOr<float>("attention_excitation_strength", 0.0f) == 1.2f);
    
    std::cout << "    testAdvancedCognitionConfiguration passed" << std::endl;
}

void testAdvancedNeuromodulationConfiguration() {
    nlm::Config config;
    
    // Test neuromodulation configuration methods
    config.setDopaminePlasticityFactor(1.2f);
    config.setNoveltyDetectionThreshold(0.3f);
    config.setCuriosityExplorationWeight(0.8f);
    config.setSerotoninMoodInfluence(0.6f);
    config.setNorepinephrineArousalThreshold(0.4f);
    config.setAcetylcholineAttentionGain(1.5f);
    
    assert(config.getOr<float>("dopamine_plasticity_factor", 0.0f) == 1.2f);
    assert(config.getOr<float>("novelty_detection_threshold", 0.0f) == 0.3f);
    assert(config.getOr<float>("curiosity_exploration_weight", 0.0f) == 0.8f);
    assert(config.getOr<float>("serotonin_mood_influence", 0.0f) == 0.6f);
    assert(config.getOr<float>("norepinephrine_arousal_threshold", 0.0f) == 0.4f);
    assert(config.getOr<float>("acetylcholine_attention_gain", 0.0f) == 1.5f);
    
    std::cout << "    testAdvancedNeuromodulationConfiguration passed" << std::endl;
}

void testAdvancedDevelopmentConfiguration() {
    nlm::Config config;
    
    // Test development configuration methods
    config.setDevelopmentStageTransitions(0.8f);
    config.setDevelopmentalPlasticityMultiplier(1.5f);
    config.setMaturationRate(0.3f);
    
    assert(config.getOr<float>("development_stage_transitions", 0.0f) == 0.8f);
    assert(config.getOr<float>("developmental_plasticity_multiplier", 0.0f) == 1.5f);
    assert(config.getOr<float>("maturation_rate", 0.0f) == 0.3f);
    
    std::cout << "    testAdvancedDevelopmentConfiguration passed" << std::endl;
}

void testAdvancedCheckpointConfiguration() {
    nlm::Config config;
    
    // Test checkpoint configuration methods
    config.setCheckpointPath("./nlm_checkpoints");
    config.setCheckpointCompressionLevel(6);
    config.setCheckpointMaxFiles(50);
    config.setCheckpointAutoSave(true);
    
    assert(config.getOr<std::string>("checkpoint_path", "") == "./nlm_checkpoints");
    assert(config.getOr<int>("checkpoint_compression_level", 0) == 6);
    assert(config.getOr<size_t>("checkpoint_max_files", 0) == 50);
    assert(config.getOr<bool>("checkpoint_auto_save", false) == true);
    
    std::cout << "    testAdvancedCheckpointConfiguration passed" << std::endl;
}

void testAdvancedPerformanceConfiguration() {
    nlm::Config config;
    
    // Test performance configuration methods
    config.setMemoryPoolSize(1024 * 1024);
    config.setEventQueueCapacity(10000);
    config.setParallelProcessingEnabled(true);
    config.setSIMDVectorizationEnabled(true);
    
    assert(config.getOr<size_t>("memory_pool_size", 0) == 1024 * 1024);
    assert(config.getOr<size_t>("event_queue_capacity", 0) == 10000);
    assert(config.getOr<bool>("parallel_processing_enabled", false) == true);
    assert(config.getOr<bool>("simd_vectorization_enabled", false) == true);
    
    std::cout << "    testAdvancedPerformanceConfiguration passed" << std::endl;
}

void testAdvancedReplayConfiguration() {
    nlm::Config config;
    
    // Test replay configuration methods
    config.setReplayMemoryCount(20);
    config.setReplayConfidenceThreshold(0.7f);
    config.setReplayTimeStepInterval(100);
    
    assert(config.getOr<size_t>("replay_memory_count", 0) == 20);
    assert(config.getOr<float>("replay_confidence_threshold", 0.0f) == 0.7f);
    assert(config.getOr<size_t>("replay_time_step_interval", 0) == 100);
    
    std::cout << "    testAdvancedReplayConfiguration passed" << std::endl;
}

void testAdvancedRestConfiguration() {
    nlm::Config config;
    
    // Test rest configuration methods
    config.setRestPeriodDuration(1000);
    config.setRestNeuromodulatorThresholds(0.7f, 0.8f, 0.3f, 0.4f);
    config.setRestDevelopmentAcceleration(true);
    
    assert(config.getOr<size_t>("rest_period_duration", 0) == 1000);
    assert(config.getOr<float>("rest_serotonin_threshold", 0.0f) == 0.7f);
    assert(config.getOr<float>("rest_dopamine_threshold", 0.0f) == 0.8f);
    assert(config.getOr<float>("rest_norepinephrine_threshold", 0.0f) == 0.3f);
    assert(config.getOr<float>("rest_acetylcholine_threshold", 0.0f) == 0.4f);
    assert(config.getOr<bool>("rest_development_acceleration", false) == true);
    
    std::cout << "    testAdvancedRestConfiguration passed" << std::endl;
}

void testAdvancedEventSystemConfiguration() {
    nlm::Config config;
    
    // Test event system configuration methods
    config.setEventQueueMaxSize(5000);
    config.setEventProcessingInterval(10);
    config.setEventPriorityLevel(3);
    
    assert(config.getOr<size_t>("event_queue_max_size", 0) == 5000);
    assert(config.getOr<size_t>("event_processing_interval", 0) == 10);
    assert(config.getOr<size_t>("event_priority_level", 0) == 3);
    
    std::cout << "    testAdvancedEventSystemConfiguration passed" << std::endl;
}

void testAdvancedMemoryConsolidationConfiguration() {
    nlm::Config config;
    
    // Test memory consolidation configuration methods
    config.setMemoryConsolidationThreshold(0.5f);
    config.setMemoryConsolidationDecay(0.95f);
    config.setMemoryReplayStrength(1.2f);
    
    assert(config.getOr<float>("memory_consolidation_threshold", 0.0f) == 0.5f);
    assert(config.getOr<float>("memory_consolidation_decay", 0.0f) == 0.95f);
    assert(config.getOr<float>("memory_replay_strength", 0.0f) == 1.2f);
    
    std::cout << "    testAdvancedMemoryConsolidationConfiguration passed" << std::endl;
}

void testAdvancedConfigurationSummary() {
    nlm::Config config;
    
    // Set some advanced configuration values
    config.setBrainType("Phase6_Integrated");
    config.setNeuronCount(1000);
    config.setWorkingMemoryCapacity(500);
    config.setPredictionDepth(5);
    config.setParallelProcessingEnabled(true);
    
    // Test summary generation
    std::string summary = config.getAdvancedConfigSummary();
    
    assert(!summary.empty());
    assert(summary.find("Advanced Configuration Summary") != std::string::npos);
    assert(summary.find("Phase6_Integrated") != std::string::npos);
    assert(summary.find("1000") != std::string::npos);
    assert(summary.find("500") != std::string::npos);
    assert(summary.find("5") != std::string::npos);
    
    std::cout << "    testAdvancedConfigurationSummary passed" << std::endl;
}

void testGetAllAdvancedKeys() {
    nlm::Config config;
    
    // Get all advanced keys
    auto advancedKeys = config.getAllAdvancedKeys();
    
    // Check that essential advanced keys are present
    assert(!advancedKeys.empty());
    
    // Check some specific advanced keys
    bool hasBrainKeys = false;
    bool hasMemoryKeys = false;
    bool hasPredictionKeys = false;
    
    for (const auto& key : advancedKeys) {
        if (key == "brain_type" || key == "neuron_count" || key == "region_count") {
            hasBrainKeys = true;
        }
        if (key == "working_memory_capacity" || key == "working_memory_decay_rate") {
            hasMemoryKeys = true;
        }
        if (key == "prediction_depth" || key == "prediction_confidence_threshold") {
            hasPredictionKeys = true;
        }
    }
    
    assert(hasBrainKeys);
    assert(hasMemoryKeys);
    assert(hasPredictionKeys);
    
    std::cout << "    testGetAllAdvancedKeys passed" << std::endl;
}

void testLoadAdvancedConfigFromFile() {
    nlm::Config config;
    
    // Create a temporary file with advanced configuration
    std::string tempFile = "test_advanced_config.txt";
    std::ofstream file(tempFile);
    
    file << "# Advanced NLM Configuration\n";
    file << "brain_type = \"Phase6_Advanced\"\n";
    file << "neuron_count = 5000\n";
    file << "working_memory_capacity = 1000\n";
    file << "prediction_depth = 10\n";
    file << "parallel_processing_enabled = true\n";
    
    file.close();
    
    // Load the advanced configuration
    config.loadAdvancedConfigFromFile(tempFile);
    
    // Verify the values were loaded
    assert(config.getOr<std::string>("brain_type", "") == "Phase6_Advanced");
    assert(config.getOr<size_t>("neuron_count", 0) == 5000);
    assert(config.getOr<size_t>("working_memory_capacity", 0) == 1000);
    assert(config.getOr<int>("prediction_depth", 0) == 10);
    assert(config.getOr<bool>("parallel_processing_enabled", false) == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testLoadAdvancedConfigFromFile passed" << std::endl;
}

void testSaveAdvancedConfigToFile() {
    nlm::Config config;
    
    // Set some advanced configuration values
    config.setBrainType("Phase6_Advanced");
    config.setNeuronCount(3000);
    config.setWorkingMemoryCapacity(800);
    config.setPredictionDepth(7);
    config.setParallelProcessingEnabled(true);
    
    // Save to file
    std::string tempFile = "test_saved_advanced_config.txt";
    config.saveAdvancedConfigToFile(tempFile);
    
    // Verify the file was created
    assert(std::filesystem::exists(tempFile));
    
    // Read back and verify values
    std::ifstream file(tempFile);
    std::string line;
    bool foundBrainType = false;
    bool foundNeuronCount = false;
    
    while (std::getline(file, line)) {
        if (line.find("brain_type") != std::string::npos && line.find("Phase6_Advanced") != std::string::npos) {
            foundBrainType = true;
        }
        if (line.find("neuron_count") != std::string::npos && line.find("3000") != std::string::npos) {
            foundNeuronCount = true;
        }
    }
    
    file.close();
    assert(foundBrainType);
    assert(foundNeuronCount);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testSaveAdvancedConfigToFile passed" << std::endl;
}

void testAdvancedConfigurationSetAdvanced() {
    nlm::Config config;
    
    // Test the advanced setAdvanced method
    config.setAdvanced("brain_type", "Phase6_Advanced");
    config.setAdvanced("neuron_count", 5000u);
    config.setAdvanced("working_memory_capacity", 1000u);
    config.setAdvanced("prediction_depth", 10);
    
    assert(config.getOr<std::string>("brain_type", "") == "Phase6_Advanced");
    assert(config.getOr<size_t>("neuron_count", 0) == 5000u);
    assert(config.getOr<size_t>("working_memory_capacity", 0) == 1000u);
    assert(config.getOr<int>("prediction_depth", 0) == 10);
    
    std::cout << "    testAdvancedConfigurationSetAdvanced passed" << std::endl;
}

void testAdvancedConfigurationValidation() {
    nlm::Config config;
    
    // Test validation for string values
    bool validString = config.validateAdvancedSetting("brain_type", "Phase6_Test");
    assert(validString);
    
    // Test validation for size_t values
    bool validNeuronCount = config.validateAdvancedSetting("neuron_count", 1000u);
    assert(validNeuronCount);
    
    // Test validation for float values
    bool validProbability = config.validateAdvancedSetting("connection_probability", 0.5f);
    assert(validProbability);
    
    // Test validation for bool values
    bool validBool = config.validateAdvancedSetting("parallel_processing_enabled", true);
    assert(validBool);
    
    std::cout << "    testAdvancedConfigurationValidation passed" << std::endl;
}

void testPhase6IntegrationExample() {
    nlm::Config config;
    
    // Phase 6 Integration Example - test all the example configurations from the spec
    config.setBrainType("Phase6_Integrated");
    config.setNeuronCount(2000);
    config.setRegionCount(4);
    config.setConnectionProbability(0.15f);
    
    config.setWorkingMemoryCapacity(500);
    config.setWorkingMemoryDecayRate(0.02f);
    config.setEpisodicMemoryMaxEpisodes(5000);
    config.setEpisodicMemoryConsolidationThreshold(0.7f);
    
    config.setPredictionDepth(8);
    config.setPredictionConfidenceThreshold(0.6f);
    config.setPredictionErrorSmoothing(0.9f);
    
    config.setDopaminePlasticityFactor(1.2f);
    config.setNoveltyDetectionThreshold(0.3f);
    config.setCuriosityExplorationWeight(0.8f);
    config.setSerotoninMoodInfluence(0.6f);
    config.setNorepinephrineArousalThreshold(0.4f);
    config.setAcetylcholineAttentionGain(1.5f);
    
    config.setDevelopmentStageTransitions(0.8f);
    config.setDevelopmentalPlasticityMultiplier(1.5f);
    config.setMaturationRate(0.3f);
    
    config.setCheckpointPath("./nlm_checkpoints");
    config.setCheckpointCompressionLevel(6);
    config.setCheckpointMaxFiles(50);
    config.setCheckpointAutoSave(true);
    
    config.setMemoryPoolSize(1024 * 1024);
    config.setEventQueueCapacity(10000);
    config.setParallelProcessingEnabled(true);
    config.setSIMDVectorizationEnabled(true);
    
    config.setReplayMemoryCount(20);
    config.setReplayConfidenceThreshold(0.7f);
    config.setReplayTimeStepInterval(100);
    
    config.setRestPeriodDuration(1000);
    config.setRestNeuromodulatorThresholds(0.7f, 0.8f, 0.3f, 0.4f);
    config.setRestDevelopmentAcceleration(true);
    
    config.setEventQueueMaxSize(5000);
    config.setEventProcessingInterval(10);
    config.setEventPriorityLevel(3);
    
    config.setMemoryConsolidationThreshold(0.5f);
    config.setMemoryConsolidationDecay(0.95f);
    config.setMemoryReplayStrength(1.2f);
    
    // Verify all configurations were set correctly
    assert(config.getOr<std::string>("brain_type", "") == "Phase6_Integrated");
    assert(config.getOr<size_t>("neuron_count", 0) == 2000);
    assert(config.getOr<size_t>("region_count", 0) == 4);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    assert(config.getOr<size_t>("working_memory_capacity", 0) == 500);
    assert(config.getOr<size_t>("episodic_memory_max_episodes", 0) == 5000);
    
    assert(config.getOr<int>("prediction_depth", 0) == 8);
    assert(config.getOr<float>("prediction_confidence_threshold", 0.0f) == 0.6f);
    
    assert(config.getOr<float>("dopamine_plasticity_factor", 0.0f) == 1.2f);
    assert(config.getOr<float>("acetylcholine_attention_gain", 0.0f) == 1.5f);
    
    std::cout << "    testPhase6IntegrationExample passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Advanced configuration tests
    testAdvancedBrainConfiguration();
    testAdvancedMemoryConfiguration();
    testAdvancedPredictionConfiguration();
    testAdvancedCognitionConfiguration();
    testAdvancedNeuromodulationConfiguration();
    testAdvancedDevelopmentConfiguration();
    testAdvancedCheckpointConfiguration();
    testAdvancedPerformanceConfiguration();
    testAdvancedReplayConfiguration();
    testAdvancedRestConfiguration();
    testAdvancedEventSystemConfiguration();
    testAdvancedMemoryConsolidationConfiguration();
    
    testAdvancedConfigurationSummary();
    testGetAllAdvancedKeys();
    testLoadAdvancedConfigFromFile();
    testSaveAdvancedConfigToFile();
    testAdvancedConfigurationSetAdvanced();
    testAdvancedConfigurationValidation();
    testPhase6IntegrationExample();
}

} // namespace test_config

int main() {
    test_config::runAll();
    return 0;
}