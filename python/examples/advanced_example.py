#!/usr/bin/env python3
"""
NLM Python Examples: Advanced Usage and Power User Features
===========================================================

This file contains advanced examples demonstrating the power of the
enhanced NLM Python bindings, including batch processing, analysis,
and advanced configuration management.

These examples build upon the basic usage examples and showcase
features for advanced users and power users.
"""

import sys
sys.path.insert(0, '/workspace/app/python')

from nlm import (
    createDefaultConfig,
    createLearningConfig,
    createMinimalConfig,
    createFromTemplate,
    createWorld,
    createBrain,
    createAgentBrain,
    quickSimulation,
    runBasicTest,
    analyzeLearning,
    getBrainStats,
    compareBrains,
    exportBrainState,
    createSensoryPercept,
    createTextPercept,
    encodeText,
    batchTrain,
    validateConfig,
    createConfigFromEnv,
    getVersion,
    listAvailableBrains,
    runBenchmarks,
    enableAdvancedFeatures
)

def example_advanced_batch_training():
    """
    Advanced batch training with different configurations and analysis.
    """
    print("=== Advanced Batch Training ===")
    
    # Create multiple brains with different characteristics
    brain_configs = []
    rewards = [10.0, -5.0, 15.0, 0.0, 8.0, -12.0, 20.0]
    
    # Define different configuration strategies
    strategies = [
        {"neurons": 100, "regions": 1, "learning_rate": 0.01, "name": "conservative"},
        {"neurons": 500, "regions": 2, "learning_rate": 0.05, "name": "balanced"},
        {"neurons": 1000, "regions": 3, "learning_rate": 0.1, "name": "aggressive"},
        {"neurons": 200, "regions": 1, "learning_rate": 0.02, "name": "slow"},
        {"neurons": 750, "regions": 2, "learning_rate": 0.07, "name": "fast"},
        {"neurons": 150, "regions": 1, "learning_rate": 0.03, "name": "medium"},
        {"neurons": 1200, "regions": 4, "learning_rate": 0.15, "name": "massive"},
    ]
    
    # Create brains using different strategies
    brains = []
    for i, strategy in enumerate(strategies):
        config = createLearningConfig(
            learning_rate=strategy["learning_rate"],
            connection_probability=0.15
        )
        config.set("neuron_count", strategy["neurons"])
        config.set("region_count", strategy["regions"])
        
        brain = createBrain(config)
        brains.append(brain)
        print(f"Created {strategy['name']} brain with {strategy['neurons']} neurons")
    
    # Run batch training
    print("\nRunning batch training...")
    batchTrain(brains, rewards, steps=200)
    
    # Analyze results
    print("\nBatch Training Results:")
    labels = [s["name"] for s in strategies]
    comparison = compareBrains(brains, labels)
    
    for label, stats in comparison.items():
        print(f"\n{label}:")
        print(f"  Neurons: {stats['total_neurons']}")
        print(f"  Synapses: {stats['total_synapses']}")
        print(f"  Active neurons: {stats['active_neurons']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")
    
    # Export comprehensive analysis
    comprehensive_analysis = {}
    for i, (label, stats) in enumerate(comparison.items()):
        # JSON export for each brain
        json_data = exportBrainState(brains[i], "json")
        comprehensive_analysis[f"{label}_json"] = len(json_data)
        
        # CSV export for each brain
        csv_data = exportBrainState(brains[i], "csv")
        comprehensive_analysis[f"{label}_csv"] = len(csv_data)
    
    print(f"\nComprehensive analysis exported: {len(comprehensive_analysis)} files")
    
    print("\nAdvanced batch training completed successfully!\n")

def example_performance_analysis():
    """
    Performance analysis and benchmarking of different brain configurations.
    """
    print("=== Performance Analysis ===")
    
    # Create brains with varying sizes for performance testing
    brain_sizes = [100, 250, 500, 1000, 1500]
    brains = []
    
    print("Creating brains with different sizes...")
    for size in brain_sizes:
        config = createMinimalConfig(size)
        brain = createBrain(config)
        brains.append(brain)
        print(f"  Created brain with {size} neurons")
    
    # Run benchmarks
    print("\nRunning performance benchmarks...")
    benchmarks = runBenchmarks()
    
    print("Benchmark Results:")
    for metric, value in benchmarks.items():
        if isinstance(value, float) and value > 1.0:
            print(f"  {metric}: {value:.3f}")
        else:
            print(f"  {metric}: {value}")
    
    # Performance test with different simulation steps
    print("\nPerformance testing with varying simulation steps:")
    test_steps = [10, 50, 100, 200]
    
    for size, brain in zip(brain_sizes, brains):
        for steps in test_steps:
            quickSimulation(brain, steps, 10.0)
            stats = getBrainStats(brain)
            time_per_step = 1.0 / benchmarks["simulation_speed"] * steps
            
            print(f"  {size} neurons, {steps} steps: "
                  f"{stats['total_spikes']} spikes, "
                  f"{time_per_step:.3f}s simulation time")
    
    print("\nPerformance analysis completed successfully!\n")

def example_learning_curves():
    """
    Advanced learning curve analysis and progress tracking.
    """
    print("=== Learning Curves Analysis ===")
    
    # Create multiple learning experiments
    experiments = []
    learning_rates = [0.01, 0.05, 0.1]
    
    for lr in learning_rates:
        config = createLearningConfig(lr, 0.15)
        brain = createBrain(config)
        experiments.append(brain)
        print(f"Created learning experiment with rate {lr}")
    
    # Run learning experiments
    print("\nRunning learning experiments...")
    all_metrics = []
    
    for i, brain in enumerate(experiments):
        print(f"  Experiment {i+1}: Running...")
        metrics = analyzeLearning(brain, steps=500)
        all_metrics.append(metrics)
        learning, change, init_syn, final_syn = metrics
        print(f"    Learning: {learning}, Weight change: {change:.4f}")
    
    # Analyze learning progress over time
    print("\nLearning Progress Analysis:")
    
    # Create synthetic learning curves
    time_points = list(range(0, 501, 50))
    learning_curves = {}
    
    for i, (lr, brain) in enumerate(zip(learning_rates, experiments)):
        curve = []
        for step in time_points:
            if step == 0:
                curve.append(0.0)
            else:
                # Simulate learning progress
                progress = min(1.0, (step / 500.0) * (0.5 + lr))
                curve.append(progress)
        
        learning_curves[f"LR_{lr}"] = curve
        print(f"  LR_{lr}: Final learning progress: {curve[-1]:.3f}")
    
    # Calculate average learning rate
    print("\nAverage Learning Rates:")
    for name, curve in learning_curves.items():
        avg_rate = sum(curve[i] - curve[i-1] for i in range(1, len(curve))) / (len(curve) - 1)
        print(f"  {name}: {avg_rate:.4f} per 50 steps")
    
    print("\nLearning curves analysis completed successfully!\n")

def example_text_associations():
    """
    Advanced text processing and association learning (Phase 6 features).
    """
    print("=== Text Associations Learning ===")
    
    # Create brain for text learning
    config = createLearningConfig(0.03, 0.1)
    brain = createBrain(config)
    
    # Create agent for text processing
    world = createWorld(20, 20, 16, 16)
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Text learning experiment
    print("Creating text learning experiment...")
    
    # Word pairs for association learning
    word_pairs = [
        ("apple", "fruit"),
        ("car", "vehicle"),
        ("book", "knowledge"),
        ("water", "drink"),
        ("fire", "danger")
    ]
    
    # Encode and process each word pair
    for word1, word2 in word_pairs:
        # Create percepts with different words
        percept1 = createTextPercept(word1)
        percept2 = createTextPercept(word2)
        
        # Process both percepts
        agent.processSensoryInput(percept1)
        action1 = agent.decodeMotorCommand()
        
        agent.processSensoryInput(percept2)
        action2 = agent.decodeMotorCommand()
        
        print(f"  Processed '{word1}' -> action: {action1}")
        print(f"  Processed '{word2}' -> action: {action2}")
        
        # Apply reward modulation to learn associations
        reward = 10.0 if word1 == "apple" or word2 == "car" else 0.0
        agent.applyRewardModulation(reward, reward * 0.5)
    
    # Check concept formation
    print("\nConcept formation results:")
    print(f"  Concept count: {agent.getConceptCount()}")
    print(f"  Self model confidence: {agent.getSelfModelConfidence():.3f}")
    print(f"  Social observation count: {agent.getSocialObservationCount()}")
    
    # Show learned preferences
    print(f"  Learned preference: {brain.getLearnedPreference():.3f}")
    
    print("\nText associations learning completed successfully!\n")

def example_error_recovery():
    """
    Error handling and recovery mechanisms.
    """
    print("=== Error Recovery ===")
    
    # Create test scenarios that demonstrate error handling
    error_tests = []
    
    # Test 1: Invalid configuration
    print("Test 1: Invalid configuration handling...")
    try:
        invalid_config = createDefaultConfig()
        # Remove required keys
        invalid_config.set("neuron_count", 0)  # Invalid: 0 neurons
        
        brain = createBrain(invalid_config)
        print("  ERROR: Should have failed with 0 neurons")
    except Exception as e:
        print(f"  Successfully caught error: {type(e).__name__}")
    
    # Test 2: Missing configuration
    print("\nTest 2: Missing configuration handling...")
    try:
        incomplete_config = createLearningConfig(0.05, 0.15)
        # Create brain with incomplete config
        brain = createBrain(incomplete_config)
        
        # This should work with defaults
        quickSimulation(brain, 10, 10.0)
        print("  Successfully handled incomplete configuration with defaults")
    except Exception as e:
        print(f"  Handled error: {type(e).__name__}")
    
    # Test 3: Invalid export format
    print("\nTest 3: Invalid export format handling...")
    try:
        config = createDefaultConfig()
        brain = createBrain(config)
        quickSimulation(brain, 5, 10.0)
        
        # Try invalid export format
        exportBrainState(brain, "invalid_format_xyz")
        print("  ERROR: Should have failed with invalid format")
    except Exception as e:
        print(f"  Successfully caught export error: {type(e).__name__}")
    
    # Test 4: Recovery from failed operations
    print("\nTest 4: Recovery from failed operations...")
    try:
        # Create a valid brain
        config = createDefaultConfig()
        brain = createBrain(config)
        
        # Run successful simulation
        quickSimulation(brain, 10, 10.0)
        print("  First simulation successful")
        
        # Try invalid operation
        try:
            exportBrainState(brain, "invalid")
        except:
            print("  Caught invalid export error")
        
        # Recovery: create new brain and continue
        new_config = createLearningConfig(0.02, 0.1)
        new_brain = createBrain(new_config)
        quickSimulation(new_brain, 20, 5.0)
        print("  Successfully recovered and created new brain")
        
    except Exception as e:
        print(f"  Recovery error: {type(e).__name__}")
    
    print("\nError recovery tests completed successfully!\n")

def example_environment_integration():
    """
    Environment integration and configuration management.
    """
    print("=== Environment Integration ===")
    
    # Test environment variable integration
    print("Testing environment variable integration...")
    
    # Save original environment variables
    original_neuron_count = __import__('os').environ.get("NLM_NEURON_COUNT")
    original_region_count = __import__('os').environ.get("NLM_REGION_COUNT")
    original_seed = __import__('os').environ.get("NLM_RANDOM_SEED")
    original_log_level = __import__('os').environ.get("NLM_LOG_LEVEL")
    
    # Set environment variables
    __import__('os').environ["NLM_NEURON_COUNT"] = "800"
    __import__('os').environ["NLM_REGION_COUNT"] = "4"
    __import__('os').environ["NLM_RANDOM_SEED"] = "98765"
    __import__('os').environ["NLM_LOG_LEVEL"] = "INFO"
    
    try:
        # Create config from environment
        env_config = createConfigFromEnv()
        print("Created config from environment variables")
        
        # Verify environment variables were read
        neuron_count = env_config.getOr("neuron_count", 0)
        region_count = env_config.getOr("region_count", 0)
        
        print(f"  Neuron count from env: {neuron_count}")
        print(f"  Region count from env: {region_count}")
        
        # Create brain with environment config
        brain = createBrain(env_config)
        quickSimulation(brain, 30, 15.0)
        
        print("Successfully ran simulation with environment configuration")
        
    finally:
        # Restore original environment variables
        if original_neuron_count:
            __import__('os').environ["NLM_NEURON_COUNT"] = original_neuron_count
        else:
            __import__('os').environ.pop("NLM_NEURON_COUNT", None)
            
        if original_region_count:
            __import__('os').environ["NLM_REGION_COUNT"] = original_region_count
        else:
            __import__('os').environ.pop("NLM_REGION_COUNT", None)
            
        if original_seed:
            __import__('os').environ["NLM_RANDOM_SEED"] = original_seed
        else:
            __import__('os').environ.pop("NLM_RANDOM_SEED", None)
            
        if original_log_level:
            __import__('os').environ["NLM_LOG_LEVEL"] = original_log_level
        else:
            __import__('os').environ.pop("NLM_LOG_LEVEL", None)
    
    # Test configuration validation
    print("\nTesting configuration validation...")
    
    # Valid configuration
    valid_config = createLearningConfig(0.05, 0.2)
    validation_result = validateConfig(valid_config)
    print(f"Valid config: {validation_result}")
    
    # Invalid configuration
    invalid_config = createLearningConfig(0.05, 0.2)
    invalid_config.set("neuron_count", 0)  # Invalid
    
    try:
        validateConfig(invalid_config)
        print("ERROR: Should have failed validation")
    except Exception as e:
        print(f"Successfully caught invalid config error: {type(e).__name__}")
    
    print("\nEnvironment integration completed successfully!\n")

def example_advanced_features():
    """
    Advanced features and power user capabilities.
    """
    print("=== Advanced Features ===")
    
    # Enable advanced features
    print("Enabling advanced features...")
    features_status = enableAdvancedFeatures()
    print(f"Advanced features status: {features_status}")
    
    # Get available brain types
    print("\nAvailable brain types:")
    brain_types = listAvailableBrains()
    for brain_type in brain_types:
        print(f"  - {brain_type}")
    
    # Show version information
    print(f"\nNLM Python Bindings Version: {getVersion()}")
    
    # Create advanced brain configuration
    print("\nCreating advanced brain configuration...")
    
    # Complex configuration with multiple settings
    config = createLearningConfig(learning_rate=0.08, connection_probability=0.25)
    config.set("neuron_count", 2000)
    config.set("region_count", 5)
    config.set("plasticity_learning_rate", 0.05)
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("development_pruning_rate", 0.0002)
    config.set("reward_discount_factor", 0.95)
    config.set("environment_name", "AdvancedWorld")
    
    print("Created complex configuration with advanced settings")
    
    # Create and test advanced brain
    brain = createBrain(config)
    quickSimulation(brain, 100, 20.0)
    
    # Get detailed statistics
    stats = getBrainStats(brain)
    print("\nAdvanced Brain Statistics:")
    for key, value in stats.items():
        if isinstance(value, float) and value > 100:
            print(f"  {key}: {value:.1f}")
        else:
            print(f"  {key}: {value}")
    
    # Export in multiple formats
    print("\nExporting advanced brain state...")
    json_export = exportBrainState(brain, "json")
    csv_export = exportBrainState(brain, "csv")
    
    print(f"  JSON export size: {len(json_export)} characters")
    print(f"  CSV export size: {len(csv_export)} characters")
    
    print("\nAdvanced features demonstration completed successfully!\n")

def example_comprehensive_pipeline():
    """
    Comprehensive pipeline example combining multiple advanced features.
    """
    print("=== Comprehensive Pipeline Example ===")
    
    print("Starting comprehensive pipeline...")
    
    # Step 1: Environment-aware configuration
    print("\nStep 1: Environment-aware configuration")
    env_config = createConfigFromEnv()
    print(f"  Created config with {env_config.getOr('neuron_count', 0)} neurons")
    
    # Step 2: Advanced brain creation
    print("\nStep 2: Advanced brain creation")
    config = createLearningConfig(learning_rate=0.06, connection_probability=0.18)
    config.set("neuron_count", 1500)
    config.set("region_count", 3)
    config.set("plasticity_learning_rate", 0.04)
    
    brain = createBrain(config)
    print("  Created advanced brain with custom configuration")
    
    # Step 3: Performance benchmarking
    print("\nStep 3: Performance benchmarking")
    benchmarks = runBenchmarks()
    print(f"  Config creation time: {benchmarks['config_creation_time']:.3f}s")
    print(f"  Brain initialization time: {benchmarks['brain_initialization_time']:.3f}s")
    print(f"  Simulation speed: {benchmarks['simulation_speed']:.0f} steps/s")
    
    # Step 4: Advanced simulation
    print("\nStep 4: Advanced simulation")
    quickSimulation(brain, 200, 25.0)
    stats = getBrainStats(brain)
    print(f"  Simulation completed with {stats['total_spikes']} spikes")
    
    # Step 5: Learning analysis
    print("\nStep 5: Learning analysis")
    learning_result = analyzeLearning(brain, 500)
    learning, change, init_syn, final_syn = learning_result
    print(f"  Learning occurred: {learning}")
    print(f"  Weight change: {change:.4f}")
    print(f"  Synaptic evolution: {init_syn} -> {final_syn} synapses")
    
    # Step 6: Batch processing with multiple configurations
    print("\nStep 6: Batch processing")
    brains = []
    configs = []
    
    # Create multiple configurations for batch testing
    for i in range(3):
        batch_config = createLearningConfig(
            learning_rate=0.01 * (i + 1),
            connection_probability=0.1 + 0.05 * i
        )
        batch_config.set("neuron_count", 500 + i * 250)
        batch_config.set("region_count", 1 + i)
        
        batch_brain = createBrain(batch_config)
        brains.append(batch_brain)
        configs.append(batch_config)
        print(f"  Created batch configuration {i+1}: {batch_config.getOr('neuron_count', 0)} neurons")
    
    # Batch train
    rewards = [10.0, 5.0, 15.0]
    batchTrain(brains, rewards, steps=100)
    print("  Completed batch training")
    
    # Step 7: Comparative analysis
    print("\nStep 7: Comparative analysis")
    labels = [f"Batch_{i+1}" for i in range(len(brains))]
    comparison = compareBrains(brains, labels)
    
    print("  Comparative results:")
    for label, brain_stats in comparison.items():
        avg_rate = brain_stats['average_firing_rate']
        total_spikes = brain_stats['total_spikes']
        print(f"    {label}: {avg_rate:.2f} Hz avg, {total_spikes} total spikes")
    
    # Step 8: Export comprehensive results
    print("\nStep 8: Export comprehensive results")
    
    # Export main brain
    json_main = exportBrainState(brain, "json")
    csv_main = exportBrainState(brain, "csv")
    
    print(f"  Main brain exported: {len(json_main)} chars (JSON), {len(csv_main)} chars (CSV)")
    
    # Export batch results
    batch_analysis = {}
    for i, (label, brain_stats) in enumerate(comparison.items()):
        json_export = exportBrainState(brains[i], "json")
        csv_export = exportBrainState(brains[i], "csv")
        batch_analysis[f"{label}_json"] = len(json_export)
        batch_analysis[f"{label}_csv"] = len(csv_export)
    
    print(f"  Batch results exported: {len(batch_analysis)} files")
    
    # Step 9: Final validation
    print("\nStep 9: Final validation")
    validation_result = validateConfig(config)
    print(f"  Configuration validation: {validation_result}")
    
    # Step 10: Summary report
    print("\n=== Pipeline Summary ===")
    print(f"  Total brains created: {1 + len(brains)}")
    print(f"  Total simulations run: 1 (main) + {len(brains)} (batch) = {1 + len(brains)}")
    print(f"  Total files exported: 2 (main) + {len(batch_analysis)} (batch) = {2 + len(batch_analysis)}")
    print(f"  Total data processed: {sum(batch_analysis.values()) + len(json_main) + len(csv_main)} characters")
    
    print("\nComprehensive pipeline completed successfully!\n")

def main():
    """Run all advanced examples"""
    print("NLM Python Advanced Examples")
    print("=============================")
    print()
    
    examples = [
        example_advanced_batch_training,
        example_performance_analysis,
        example_learning_curves,
        example_text_associations,
        example_error_recovery,
        example_environment_integration,
        example_advanced_features,
        example_comprehensive_pipeline
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"ERROR in Example {i}: {e}")
            import traceback
            traceback.print_exc()
            print(f"\nExample {i} failed. Continuing with next example...\n")
    
    print("All advanced examples completed!")

if __name__ == "__main__":
    main()
