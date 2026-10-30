// Comprehensive Test Suite for Enhanced NLM Python Bindings
// File: tests/test_enhanced_bindings.cpp

#include "python/bindings.cpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace test_enhanced_bindings {

void test_error_handling() {
    std::cout << "Testing enhanced error handling..." << std::endl;
    
    try {
        // Test invalid configuration validation
        auto config = std::make_shared<Config>();
        std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
        
        // This should not throw since we don't validate on creation
        ConfigValidator::validateRequiredKeys(*config, requiredKeys);
        std::cout << "  test_error_handling passed" << std::endl;
    } catch (const Exception& e) {
        std::cout << "  test_error_handling failed: " << e.what() << std::endl;
        throw;
    }
}

void test_config_validation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.1);
    
    // Test required keys validation
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    ConfigValidator::validateRequiredKeys(*config, requiredKeys);
    
    // Test missing keys validation (should throw)
    try {
        std::vector<std::string> missingKeys = {"neuron_count", "region_count", "missing_key"};
        ConfigValidator::validateRequiredKeys(*config, missingKeys);
        assert(false && "Should have thrown ConfigValidationError");
    } catch (const ConfigValidationError& e) {
        std::cout << "  ConfigValidationError correctly thrown: " << e.what() << std::endl;
    }
    
    std::cout << "  test_config_validation passed" << std::endl;
}

void test_preset_configs() {
    std::cout << "Testing preset configurations..." << std::endl;
    
    // Test different preset configurations
    auto minimal_config = ConfigValidator::createPresetConfig("minimal");
    assert(minimal_config->has("neuron_count"));
    assert(minimal_config->has("region_count"));
    assert(minimal_config->has("connection_probability"));
    
    auto standard_config = ConfigValidator::createPresetConfig("standard");
    assert(standard_config->has("stdp_ltp_weight"));
    assert(standard_config->has("stdp_ltd_weight"));
    
    auto advanced_config = ConfigValidator::createPresetConfig("advanced");
    assert(advanced_config->has("synaptogenesis_rate"));
    assert(advanced_config->has("pruning_rate"));
    
    auto research_config = ConfigValidator::createPresetConfig("research");
    assert(research_config->has("neuromod.dopamine.scale"));
    assert(research_config->has("neuromod.curiosity.enable"));
    
    std::cout << "  test_preset_configs passed" << std::endl;
}

void test_config_merge() {
    std::cout << "Testing configuration merge..." << std::endl;
    
    auto config1 = std::make_shared<Config>();
    config1->set("neuron_count", 1000);
    config1->set("region_count", 4);
    
    auto config2 = std::make_shared<Config>();
    config2->set("connection_probability", 0.1);
    config2->set("stdp_ltp_weight", 0.001);
    
    std::vector<std::shared_ptr<Config>> configs = {config1, config2};
    auto merged = ConfigValidator::mergeConfigs(configs);
    
    // Both configs should be present in merged config
    assert(merged->has("neuron_count"));
    assert(merged->has("region_count"));
    assert(merged->has("connection_probability"));
    assert(merged->has("stdp_ltp_weight"));
    
    // Test config diff
    std::string diff = ConfigValidator::diffConfigs(*config1, *config2);
    assert(!diff.empty());
    
    std::cout << "  test_config_merge passed" << std::endl;
}

void test_performance_profiler() {
    std::cout << "Testing performance profiler..." << std::endl;
    
    PerformanceProfiler profiler("test_profiler");
    
    // Test basic profiler operations
    assert(profiler.getName() == "test_profiler");
    assert(!profiler.isRunning());
    assert(profiler.getCallCount() == 0);
    
    // Start and stop profiler
    profiler.start();
    assert(profiler.isRunning());
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    profiler.stop();
    assert(!profiler.isRunning());
    
    // Check profiling results
    assert(profiler.getCallCount() == 1);
    assert(profiler.getElapsedMs() > 0);
    assert(profiler.getAverageMs() > 0);
    assert(profiler.getMinMs() >= 0);
    assert(profiler.getMaxMs() >= profiler.getAverageMs());
    
    // Test report generation
    std::string report = profiler.getReport();
    assert(!report.empty());
    assert(report.find("Profiler: test_profiler") != std::string::npos);
    
    std::cout << "  test_performance_profiler passed" << std::endl;
}

void test_sensory_input_validation() {
    std::cout << "Testing sensory input validation..." << std::endl;
    
    // Test Vision validation
    Vision vision(10, 10, 3);
    std::vector<float> data(100, 0.5f);
    vision.setData(data);
    assert(vision.getWidth() == 10);
    assert(vision.getHeight() == 10);
    assert(vision.getChannels() == 3);
    
    // Test Vision dimension validation
    assert(vision.validate_dimensions(10, 10));
    assert(!vision.validate_dimensions(5, 5));
    
    // Test Audio validation
    Audio audio(44100, 1024);
    std::vector<float> audio_data(1024, 0.0f);
    audio.setData(audio_data);
    assert(audio.getSampleRate() == 44100);
    assert(audio.getNumSamples() == 1024);
    
    // Test InternalSignals batch operations
    InternalSignals internal;
    std::vector<float> signals = {0.1f, 0.2f, 0.3f, 0.4f};
    internal.addSignalBatch(signals);
    
    std::cout << "  test_sensory_input_validation passed" << std::endl;
}

void test_world_object_operations() {
    std::cout << "Testing world object operations..." << std::endl;
    
    WorldObject obj1(1.0f, 2.0f, WorldObjectType::Resource, 10.0f, 0.5f);
    assert(obj1.x == 1.0f);
    assert(obj1.y == 2.0f);
    assert(obj1.type == WorldObjectType::Resource);
    assert(obj1.value == 10.0f);
    assert(obj1.radius == 0.5f);
    
    // Test position validation
    assert(obj1.is_valid_position(0.0f, 10.0f, 0.0f, 10.0f));
    assert(!obj1.is_valid_position(20.0f, 20.0f, 0.0f, 10.0f));
    
    // Test distance calculation
    WorldObject obj2(4.0f, 6.0f, WorldObjectType::Resource, 5.0f, 0.5f);
    float distance = obj1.distance_to(obj2);
    assert(distance > 0);
    
    // Test position update
    obj1.update_position(5.0f, 7.0f);
    assert(obj1.x == 5.0f);
    assert(obj1.y == 7.0f);
    
    std::cout << "  test_world_object_operations passed" << std::endl;
}

void test_agent_body_operations() {
    std::cout << "Testing agent body operations..." << std::endl;
    
    AgentBody body;
    body.x = 10.0f;
    body.y = 15.0f;
    body.orientation = 45.0f;
    body.velocityX = 2.0f;
    body.velocityY = 1.0f;
    body.energy = 0.8f;
    body.health = 0.9f;
    body.age = 100.0f;
    body.isMoving = true;
    body.isTurning = false;
    
    // Test state validation
    assert(body.validate_state());
    
    // Test velocity calculation
    float velocity_magnitude = body.get_velocity_magnitude();
    assert(velocity_magnitude > 0);
    
    // Test position update
    body.update_position(5.0f, 3.0f);
    assert(body.x == 15.0f);
    assert(body.y == 18.0f);
    
    std::cout << "  test_agent_body_operations passed" << std::endl;
}

void test_sensory_percept_operations() {
    std::cout << "Testing sensory percept operations..." << std::endl;
    
    SensoryPercept percept;
    
    // Test validation
    assert(percept.validate());
    
    // Test signal normalization
    std::vector<float> signals = {0.1f, 0.5f, 0.9f, 1.0f};
    percept.setInternal(signals);
    
    std::vector<float> normalized = percept.normalize_signals();
    assert(normalized.size() == signals.size());
    
    // Check that normalized values are in [0, 1] range
    for (float val : normalized) {
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "  test_sensory_percept_operations passed" << stdendl;
}

void test_world_operations() {
    std::cout << "Testing world operations..." << std::endl;
    
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    
    // Test agent position validation
    assert(world.validate_agent_position(10.0f, 10.0f));
    assert(!world.validate_agent_position(30.0f, 30.0f));
    
    // Test batch object operations
    std::vector<WorldObject> objects;
    objects.push_back(WorldObject(1.0f, 1.0f, WorldObjectType::Resource, 5.0f));
    objects.push_back(WorldObject(2.0f, 2.0f, WorldObjectType::Hazard, 3.0f));
    
    world.add_objects_batch(objects);
    
    std::cout << "  test_world_operations passed" << std::endl;
}

void test_brain_comprehensive_stats() {
    std::cout << "Testing brain comprehensive statistics..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    
    Brain brain(config);
    brain.initialize();
    
    // Run simulation steps
    for (uint64_t step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Get comprehensive stats
    std::map<std::string, std::string> stats = brain.getComprehensiveStats();
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_synapses") != stats.end());
    assert(stats.find("firing_neurons") != stats.end());
    assert(stats.find("avg_firing_rate") != stats.end());
    
    // Test activity summary
    std::string summary = brain.getActivitySummary();
    assert(!summary.empty());
    assert(summary.find("Brain Activity Summary") != std::string::npos);
    
    std::cout << "  test_brain_comprehensive_stats passed" << std::endl;
}

void test_agent_comprehensive_stats() {
    std::cout << "Testing agent comprehensive statistics..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(brain);
    
    // Get comprehensive stats
    std::map<std::string, std::string> stats = agent.get_agent_stats();
    assert(stats.find("neuromodulation_level") != stats.end());
    assert(stats.find("curiosity_level") != stats.end());
    assert(stats.find("novelty_level") != stats.end());
    assert(stats.find("prediction_error") != stats.end());
    
    // Test system status
    std::map<std::string, bool> system_status = agent.get_system_status();
    assert(system_status.find("reward_modulation") != system_status.end());
    assert(system_status.find("structural_plasticity") != system_status.end());
    assert(system_status.find("development") != system_status.end());
    assert(system_status.find("curiosity") != system_status.end());
    
    // Test enable/disable all systems
    agent.enable_all_systems();
    agent.disable_all_systems();
    
    std::cout << "  test_agent_comprehensive_stats passed" << std::endl;
}

void test_batch_operations() {
    std::cout << "Testing batch operations..." << std::endl;
    
    // Test batch brain initialization
    auto config1 = std::make_shared<Config>();
    config1->set("neuron_count", 500);
    config1->set("region_count", 2);
    
    auto config2 = std::make_shared<Config>();
    config2->set("neuron_count", 1000);
    config2->set("region_count", 4);
    
    std::vector<std::shared_ptr<Config>> configs = {config1, config2};
    std::vector<std::shared_ptr<Brain>> brains = BatchOperations::batch_initialize_brains(configs);
    
    assert(brains.size() == 2);
    for (const auto& brain : brains) {
        assert(brain->initialize());
    }
    
    // Test batch agent reset
    std::vector<std::shared_ptr<AgentBrain>> agents;
    for (const auto& brain : brains) {
        agents.push_back(std::make_shared<AgentBrain>(brain));
    }
    
    BatchOperations::batch_reset_agents(agents);
    
    // Test batch percept processing
    std::vector<SensoryPercept> percepts(2);
    BatchOperations::batch_process_percepts(agents, percepts);
    
    std::cout << "  test_batch_operations passed" << std::endl;
}

void test_benchmark_suite() {
    std::cout << "Testing benchmark suite..." << std::endl;
    
    PerformanceBenchmarkSuite benchmark_suite;
    
    // Define test scenarios
    std::vector<std::pair<std::string, std::map<std::string, int64_t>>> scenarios = {
        ("small_brain", {{ "neuron_count", 100 }, { "region_count", 2 }}),
        ("medium_brain", {{ "neuron_count", 1000 }, { "region_count", 4 }}),
        ("large_brain", {{ "neuron_count", 10000 }, { "region_count", 8 }})
    };
    
    // Run benchmarks for each scenario
    for (const auto& scenario : scenarios) {
        benchmark_suite.run_scenario(
            scenario.first,
            scenario.second.at("neuron_count"),
            scenario.second.at("region_count"),
            100
        );
    }
    
    // Get benchmark results
    auto results = benchmark_suite.get_benchmark_results();
    assert(results.size() == 3);
    
    for (const auto& result : results) {
        assert(result.second.find("avg_step_time_ms") != result.second.end());
        assert(result.second.find("neurons_per_second") != result.second.end());
        assert(result.second.find("memory_efficiency") != result.second.end());
        
        // Verify results are reasonable
        assert(result.second.at("avg_step_time_ms") > 0);
        assert(result.second.at("neurons_per_second") > 0);
        assert(result.second.at("memory_efficiency") > 0 && result.second.at("memory_efficiency") <= 1.0);
    }
    
    std::cout << "  test_benchmark_suite passed" << std::endl;
}

void test_neural_analyzer() {
    std::cout << "Testing neural activity analyzer..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    Brain brain(config);
    brain.initialize();
    
    NeuralActivityAnalyzer analyzer(brain);
    
    // Analyze different aspects
    auto firing_patterns = analyzer.analyze_firing_patterns();
    assert(firing_patterns.find("firing_rate_distribution") != firing_patterns.end());
    assert(firing_patterns.find("firing_rate_mean") != firing_patterns.end());
    assert(firing_patterns.find("firing_rate_std") != firing_patterns.end());
    
    auto spike_train_stats = analyzer.analyze_spike_trains();
    assert(spike_train_stats.find("interspike_intervals") != spike_train_stats.end());
    assert(spike_train_stats.find("coefficient_of_variation") != spike_train_stats.end());
    
    auto connectivity_analysis = analyzer.analyze_connectivity();
    assert(connectivity_analysis.find("weight_distribution") != connectivity_analysis.end());
    assert(connectivity_analysis.find("average_weight") != connectivity_analysis.end());
    
    auto plasticity_metrics = analyzer.analyze_plasticity();
    assert(plasticity_metrics.find("learning_rate") != plasticity_metrics.end());
    assert(plasticity_metrics.find("weight_change_variance") != plasticity_metrics.end());
    
    std::cout << "  test_neural_analyzer passed" << std::endl;
}

void test_error_analyzer() {
    std::cout << "Testing error analyzer..." << std::endl;
    
    ErrorAnalyzer error_analyzer;
    error_analyzer.enable_pattern_detection(true);
    error_analyzer.enable_temporal_analysis(true);
    error_analyzer.set_error_threshold(0.01);
    
    // Simulate some errors (this is a simplified test)
    std::vector<ErrorEvent> error_events;
    error_events.push_back(ErrorEvent(0.1, "spike_mismatch", "type_mismatch"));
    error_events.push_back(ErrorEvent(0.2, "weight_change", "nan_value"));
    error_events.push_back(ErrorEvent(0.05, "connectivity", "disconnected"));
    
    // Analyze errors
    auto error_classification = error_analyzer.classify_errors(error_events);
    assert(error_classification.find("types") != error_classification.end());
    assert(error_classification.find("most_frequent") != error_classification.end());
    
    // Generate error report
    std::string error_report = error_analyzer.generate_error_report(error_events);
    assert(!error_report.empty());
    assert(error_report.find("Error Analysis Report") != std::string::npos);
    
    std::cout << "  test_error_analyzer passed" << std::endl;
}

void test_integration() {
    std::cout << "Testing complete integration..." << std::endl;
    
    // Create a comprehensive test using all enhanced features
    auto config = ConfigValidator::createPresetConfig("advanced");
    
    Brain brain(config);
    bool brain_init = brain.initialize_safe();
    assert(brain_init);
    
    // Test configuration validation
    std::vector<std::string> required_keys = {"neuron_count", "region_count"};
    config->validateRequiredKeys(required_keys);
    
    // Test performance profiling
    PerformanceProfiler profiler("integration_test");
    profiler.start();
    
    // Run brain steps with profiling
    for (uint64_t step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    profiler.stop();
    assert(profiler.getCallCount() == 1);
    
    // Get comprehensive stats
    std::map<std::string, std::string> brain_stats = brain.getComprehensiveStats();
    assert(brain_stats.find("total_neurons") != brain_stats.end());
    
    // Test neural activity tracking
    NeuralActivityTracker tracker;
    tracker.start();
    
    for (uint64_t step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        tracker.capture_snapshot(step);
    }
    
    tracker.stop();
    std::map<std::string, std::string> neural_analysis = tracker.get_neural_analysis();
    assert(neural_analysis.find("activity_metrics") != neural_analysis.end());
    
    std::cout << "  test_integration passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== Running Enhanced Bindings Tests ===" << std::endl;
    
    test_error_handling();
    test_config_validation();
    test_preset_configs();
    test_config_merge();
    test_performance_profiler();
    test_sensory_input_validation();
    test_world_object_operations();
    test_agent_body_operations();
    test_sensory_percept_operations();
    test_world_operations();
    test_brain_comprehensive_stats();
    test_agent_comprehensive_stats();
    test_batch_operations();
    test_benchmark_suite();
    test_neural_analyzer();
    test_error_analyzer();
    test_integration();
    
    std::cout << "=== All Enhanced Bindings Tests Passed! ===" << std::endl;
}

// Main function for standalone execution
int main() {
    try {
        run_all_tests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}