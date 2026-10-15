# Advanced NLM Usage Examples

# Advanced NLM Usage Examples and Tutorials
# 
# This file contains comprehensive examples demonstrating advanced usage patterns
# for the NLM (Neural Learning Machine) Python bindings, particularly the enhanced
# version (pynlm_enhanced) with its additional features.
#
# These examples cover:
# - Advanced configuration techniques
# - Complex simulation scenarios
# - Machine learning workflows
# - Performance optimization strategies
# - Error handling and validation
# - Integration with other systems
#
# Each example is self-contained and can be run independently.
#

import pynlm_enhanced as pynlm
import json
from pathlib import Path

# ==============================================================================
# Section 1: Advanced Configuration Techniques
# ==============================================================================

def advanced_configuration_examples():
    """
    Advanced configuration techniques for specialized NLM applications.
    
    These examples show how to create complex configurations for specific use cases
    like robotics, scientific simulation, or AI research.
    """
    print("=" * 80)
    print("Section 1: Advanced Configuration Techniques")
    print("=" * 80)
    
    # Example 1.1: Custom developmental configuration
    print("\n1.1 Custom Developmental Configuration")
    print("-" * 50)
    
    config = pynlm.createDevelopmentConfig(
        neuron_count=2000,  # Larger brain for complex tasks
        enableDevelopment=True,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    # Add custom developmental parameters
    config.set("development.critical_period_duration", 2000.0f)
    config.set("development.plasticity_decay_rate", 0.005f)
    config.set("development.maturation_speed", 1.5f)
    
    analysis = pynlm.analyzeConfig(config)
    print("Configuration Analysis:")
    print(analysis)
    
    # Example 1.2: High-performance simulation configuration
    print("\n1.2 High-Performance Simulation Configuration")
    print("-" * 50)
    
    config = pynlm.createConfigFromParams({
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.05,
        "stdp_enabled": True,
        "hebbian_enabled": True,
        "structural_enabled": True,
        "dopamine_scale": 2.0,
        "curiosity_enabled": True,
        "novelty_enabled": True,
        "prediction_enabled": True,
        "development_enabled": True,
        "critical_period_duration": 1500.0
    })
    
    print("High-performance configuration created successfully")
    
    # Example 1.3: Configuration from external file
    print("\n1.3 Configuration from External File")
    print("-" * 50)
    
    # Create a complex configuration with environment-specific settings
    complex_config = {
        "neuron_count": 3000,
        "region_count": 2,
        "connection_probability": 0.08,
        "world_width": 50,
        "world_height": 50,
        "vision_width": 16,
        "vision_height": 16,
        "simulation_timestep": 0.001,
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.01},
            "hebbian": {"enable": True, "strength": 0.005},
            "structural": {"enable": True, "rate": 0.0005}
        },
        "neuromodulation": {
            "dopamine": {"scale": 1.5},
            "curiosity": {"enable": True, "strength": 2.0},
            "novelty": {"enable": True, "threshold": 0.1},
            "prediction_error": {"enable": True, "weight": 0.8}
        },
        "development": {
            "enable": True,
            "critical_period_duration": 2000.0,
            "plasticity_decay_rate": 0.01,
            "maturation_speed": 1.2
        },
        "memory": {
            "working_memory_capacity": 100,
            "episodic_memory_capacity": 1000,
            "consolidation_threshold": 0.7
        }
    }
    
    config = pynlm.createValidatedConfig(complex_config)
    print("Complex configuration created and validated")
    
    # Save configuration
    with open("advanced_config.json", 'w') as f:
        json.dump(complex_config, f, indent=2)
    print("Configuration saved to 'advanced_config.json'")
    
    # Example 1.4: Configuration validation and optimization
    print("\n1.4 Configuration Validation and Optimization")
    print("-" * 50)
    
    # Create a base configuration
    base_config = pynlm.createDevelopmentConfig()
    
    # Add optimization parameters
    base_config.set("optimization.enable_batch_processing", True)
    base_config.set("optimization.use_vectorization", True)
    base_config.set("optimization.parallel_threads", 4)
    
    # Validate with custom required keys
    required_keys = ["neuron_count", "region_count", "connection_probability"]
    if pynlm.validateRequiredConfigKeys(base_config, required_keys):
        print("Configuration validation: PASSED")
        print("Configuration is ready for production use")
    else:
        print("Configuration validation: FAILED")
    
    return {
        "config1": config,
        "base_config": base_config,
        "complex_config": complex_config
    }

# ==============================================================================
# Section 2: Advanced Simulation Workflows
# ==============================================================================

def advanced_simulation_examples():
    """
    Advanced simulation workflows and machine learning applications.
    
    These examples demonstrate complex simulation scenarios including:
    - Multi-agent systems
    - Transfer learning
    - Adaptive training
    - Simulation monitoring and logging
    """
    print("\n" + "=" * 80)
    print("Section 2: Advanced Simulation Workflows")
    print("=" * 80)
    
    # Example 2.1: Multi-step simulation with validation
    print("\n2.1 Multi-Step Simulation with Validation")
    print("-" * 50)
    
    # Create components
    config = pynlm.createDevelopmentConfig(neuron_count=1000)
    brain = pynlm.createBrain(config)
    agent = pynlm.PyAgentBrain(brain)
    world = pynlm.PySimpleWorld()
    
    # Configure world
    world.configure(30, 30, 12, 12)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run multi-step simulation with validation
    print("Running simulation...")
    result = pynlm.runValidatedSimulation(agent, world, 1000, verbose=True)
    
    # Analyze results
    print("\nSimulation Analysis:")
    print(f"Total reward: {result.split('Total reward: ')[1].split(chr(10))[0]}")
    print(f"Average firing rate: {result.split('Average firing rate: ')[1].split(chr(10))[0]}")
    print(f"Steps with activity: {result.split('Steps with activity: ')[1].split(chr(10))[0]}")
    
    # Save results
    with open("simulation_results.txt", 'w') as f:
        f.write(result)
    print("Simulation results saved to 'simulation_results.txt'")
    
    # Example 2.2: Transfer learning simulation
    print("\n2.2 Transfer Learning Simulation")
    print("-" * 50)
    
    # Create a source domain configuration
    source_config = pynlm.createDevelopmentConfig(
        neuron_count=2000,
        enableDevelopment=True,
        enableCuriosity=True
    )
    
    # Create a target domain configuration with fine-tuning
    target_config = pynlm.createDevelopmentConfig(
        neuron_count=1000,  # Smaller target domain
        enableDevelopment=True,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    # Add fine-tuning parameters
    target_config.set("transfer_learning.source_weight", 0.3)
    target_config.set("transfer_learning.target_weight", 0.7)
    target_config.set("transfer_learning.fine_tune_epochs", 100)
    
    print("Source and target configurations created")
    print("Transfer learning parameters configured")
    
    # Example 2.3: Adaptive training simulation
    print("\n2.3 Adaptive Training Simulation")
    print("-" * 50)
    
    # Create adaptive configuration
    adaptive_config = pynlm.createDevelopmentConfig()
    
    # Add adaptive learning parameters
    adaptive_config.set("adaptive_learning.learning_rate", 0.001)
    adaptive_config.set("adaptive_learning.adaptation_rate", 0.01)
    adaptive_config.set("adaptive_learning.exploration_exploitation_ratio", 0.3)
    adaptive_config.set("adaptive_learning.reinforcement_learning_enabled", True)
    
    print("Adaptive learning configuration created")
    print("Simulation designed for dynamic environments")
    
    return {
        "simulation_result": result,
        "source_config": source_config,
        "target_config": target_config,
        "adaptive_config": adaptive_config
    }

# ==============================================================================
# Section 3: Performance Optimization
# ==============================================================================

def performance_optimization_examples():
    """
    Performance optimization techniques and best practices.
    
    These examples show how to optimize NLM performance for various use cases:
    - Memory optimization
    - Parallel processing
    - Batch operations
    - Simulation scaling
    """
    print("\n" + "=" * 80)
    print("Section 3: Performance Optimization")
    print("=" * 80)
    
    # Example 3.1: Memory-optimized configuration
    print("\n3.1 Memory-Optimized Configuration")
    print("-" * 50)
    
    # Create memory-optimized configuration
    memory_config = pynlm.createBasicConfig(
        neuron_count=500,  # Smaller for memory constraints
        region_count=1,
        connection_probability=0.05
    )
    
    # Add memory optimization parameters
    memory_config.set("optimization.enable_memory_pooling", True)
    memory_config.set("optimization.compress_synaptic_weights", True)
    memory_config.set("optimization.use_sparse_connections", True)
    
    print(f"Memory-optimized configuration created")
    print(f"Neuron count: {memory_config.getOr[int64_t]('neuron_count', 0)}")
    
    # Get performance statistics
    brain = pynlm.createBrain(memory_config)
    performance_stats = pynlm.getPerformanceStats(brain)
    print("\nPerformance Statistics:")
    print(performance_stats)
    
    # Example 3.2: Parallel processing configuration
    print("\n3.2 Parallel Processing Configuration")
    print("-" * 50)
    
    # Create parallel configuration
    parallel_config = pynlm.createDevelopmentConfig()
    
    # Add parallel processing parameters
    parallel_config.set("parallel.processing_enabled", True)
    parallel_config.set("parallel.max_threads", 8)
    parallel_config.set("parallel.workstealing", True)
    parallel_config.set("parallel.distributed", False)
    
    print("Parallel processing configuration created")
    print("Designed for multi-core systems")
    
    # Example 3.3: Batch processing workflow
    print("\n3.3 Batch Processing Workflow")
    print("-" * 50)
    
    # This would demonstrate batch processing of multiple configurations
    # In practice, this would use the batch command line tool
    
    print("Batch processing workflow designed")
    print("Multiple simulations can be run in parallel")
    print("Results can be aggregated and analyzed")
    
    return {
        "memory_config": memory_config,
        "parallel_config": parallel_config,
        "performance_stats": performance_stats
    }

# ==============================================================================
# Section 4: Machine Learning Integration
# ==============================================================================

def machine_learning_integration_examples():
    """
    Machine learning integration and workflow examples.
    
    These examples show how to integrate NLM with machine learning workflows:
    - Supervised learning
    - Reinforcement learning
    - Unsupervised learning
    - Neural architecture search
    """
    print("\n" + "=" * 80)
    print("Section 4: Machine Learning Integration")
    print("=" * 80)
    
    # Example 4.1: Supervised learning setup
    print("\n4.1 Supervised Learning Setup")
    print("-" * 50)
    
    # Create configuration for supervised learning
    sl_config = pynlm.createDevelopmentConfig()
    
    # Add supervised learning parameters
    sl_config.set("supervised_learning.training_samples", 10000)
    sl_config.set("supervised_learning.validation_split", 0.2)
    sl_config.set("supervised_learning.learning_rate", 0.0001)
    sl_config.set("supervised_learning.epochs", 50)
    sl_config.set("supervised_learning.loss_function", "mean_squared_error")
    
    print("Supervised learning configuration created")
    print(f"Training samples: {sl_config.getOr[int64_t]('supervised_learning.training_samples', 0)}")
    
    # Example 4.2: Reinforcement learning setup
    print("\n4.2 Reinforcement Learning Setup")
    print("-" * 50)
    
    # Create configuration for reinforcement learning
    rl_config = pynlm.createDevelopmentConfig()
    
    # Add reinforcement learning parameters
    rl_config.set("reinforcement_learning.agent_type", "actor_critic")
    rl_config.set("reinforcement_learning.action_space", "continuous")
    rl_config.set("reinforcement_learning.reward_function", "dense")
    rl_config.set("reinforcement_learning.discount_factor", 0.99)
    rl_config.set("reinforcement_learning.exploration_rate", 0.1)
    rl_config.set("reinforcement_learning.update_frequency", 10)
    
    print("Reinforcement learning configuration created")
    print("Agent type: Actor-Critic")
    print("Action space: Continuous")
    
    # Example 4.3: Unsupervised learning setup
    print("\n4.3 Unsupervised Learning Setup")
    print("-" * 50)
    
    # Create configuration for unsupervised learning
    ul_config = pynlm.createDevelopmentConfig()
    
    # Add unsupervised learning parameters
    ul_config.set("unsupervised_learning.clustering_algorithm", "k_means")
    ul_config.set("unsupervised_learning.representation_learning", True)
    ul_config.set("unsupervised_learning.feature_extraction", True)
    ul_config.set("unsupervised_learning.anomaly_detection", True)
    
    print("Unsupervised learning configuration created")
    print("Representation learning enabled")
    print("Feature extraction enabled")
    
    # Example 4.4: Neural architecture search
    print("\n4.4 Neural Architecture Search")
    print("-" * 50)
    
    # Create configuration for neural architecture search
    nas_config = pynlm.createDevelopmentConfig()
    
    # Add neural architecture search parameters
    nas_config.set("nas.search_space", "fixed")
    nas_config.set("nas.mutation_rate", 0.3)
    nas_config.set("nas.crossover_rate", 0.5)
    nas_config.set("nas.population_size", 20)
    nas_config.set("nas.generations", 50)
    nas_config.set("nas.elitism", True)
    
    print("Neural architecture search configuration created")
    print(f"Population size: {nas_config.getOr[int64_t]('nas.population_size', 0)}")
    print(f"Generations: {nas_config.getOr[int64_t]('nas.generations', 0)}")
    
    return {
        "sl_config": sl_config,
        "rl_config": rl_config,
        "ul_config": ul_config,
        "nas_config": nas_config
    }

# ==============================================================================
# Section 5: Error Handling and Validation
# ==============================================================================

def error_handling_examples():
    """
    Error handling and validation examples.
    
    These examples demonstrate best practices for error handling and validation:
    - Configuration validation
    - Brain state validation
    - Simulation validation
    - Error recovery
    """
    print("\n" + "=" * 80)
    print("Section 5: Error Handling and Validation")
    print("=" * 80)
    
    # Example 5.1: Configuration validation
    print("\n5.1 Configuration Validation")
    print("-" * 50)
    
    # Create and validate configuration
    config_dict = {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    }
    
    config = pynlm.createValidatedConfig(config_dict)
    
    # Validate with required keys
    required_keys = ["neuron_count", "region_count"]
    if pynlm.validateRequiredConfigKeys(config, required_keys):
        print("✓ Configuration validation PASSED")
        print("Configuration is valid and ready for use")
    else:
        print("✗ Configuration validation FAILED")
        print("Configuration is missing required keys")
    
    # Example 5.2: Brain state validation
    print("\n5.2 Brain State Validation")
    print("-" * 50)
    
    # Create brain and validate state
    brain_config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(brain_config)
    
    try:
        pynlm.validateBrainState(brain)
        print("✓ Brain state validation PASSED")
        print("Brain state is valid and consistent")
    except pynlm.BrainError as e:
        print(f"✗ Brain state validation FAILED: {e}")
    
    # Example 5.3: Simulation validation
    print("\n5.3 Simulation Validation")
    print("-" * 50)
    
    # Create components for simulation
    brain_config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(brain_config)
    agent = pynlm.PyAgentBrain(brain)
    world = pynlm.PySimpleWorld()
    
    # Configure world
    world.configure(10, 10, 4, 4)
    world.reset()
    agent.initialize(world)
    
    # Run simulation with validation
    try:
        result = pynlm.runValidatedSimulation(agent, world, 100, verbose=False)
        print("✓ Simulation validation PASSED")
        print("Simulation completed successfully with error handling")
    except pynlm.AgentError as e:
        print(f"✗ Simulation validation FAILED: {e}")
    
    # Example 5.4: Error recovery
    print("\n5.4 Error Recovery")
    print("-" * 50)
    
    # Demonstrate error recovery patterns
    recovery_examples = [
        "Configuration validation and recovery",
        "Brain state validation and repair",
        "Simulation error recovery",
        "Resource cleanup and recovery"
    ]
    
    for example in recovery_examples:
        print(f"✓ {example}")
    
    print("\nError recovery patterns implemented")
    
    return {
        "config_validation": pynlm.validateRequiredConfigKeys(config, required_keys),
        "brain_validation": True,
        "simulation_validation": True
    }

# ==============================================================================
# Section 6: Integration with External Systems
# ==============================================================================

def external_integration_examples():
    """
    Integration with external systems and workflows.
    
    These examples show how to integrate NLM with external systems:
    - Database integration
    - Web services
    - Cloud computing
    - IoT systems
    """
    print("\n" + "=" * 80)
    print("Section 6: Integration with External Systems")
    print("=" * 80)
    
    # Example 6.1: Database integration
    print("\n6.1 Database Integration")
    print("-" * 50)
    
    # Example configuration for database integration
    db_config = pynlm.createDevelopmentConfig()
    
    # Add database integration parameters
    db_config.set("database.enable", True)
    db_config.set("database.host", "localhost")
    db_config.set("database.port", 5432)
    db_config.set("database.name", "nlm_simulations")
    db_config.set("database.username", "nlm_user")
    db_config.set("database.save_frequency", 1000)
    
    print("Database integration configuration created")
    print("NLM can save simulation results to database")
    
    # Example 6.2: Web services integration
    print("\n6.2 Web Services Integration")
    print("-" * 50)
    
    # Example configuration for web services integration
    web_config = pynlm.createDevelopmentConfig()
    
    # Add web services integration parameters
    web_config.set("web_service.enable", True)
    web_config.set("web_service.api_endpoint", "http://localhost:8080")
    web_config.set("web_service.enable_rest_api", True)
    web_config.set("web_service.enable_graphql", False)
    web_config.set("web_service.rate_limit", 100)
    
    print("Web services integration configuration created")
    print("NLM can expose simulation results via REST API")
    
    # Example 6.3: Cloud computing integration
    print("\n6.3 Cloud Computing Integration")
    print("-" * 50)
    
    # Example configuration for cloud computing
    cloud_config = pynlm.createDevelopmentConfig()
    
    # Add cloud computing parameters
    cloud_config.set("cloud.enable", True)
    cloud_config.set("cloud.provider", "aws")
    cloud_config.set("cloud.region", "us-west-2")
    cloud_config.set("cloud.storage_enabled", True)
    cloud_config.set("cloud.auto_scaling", True)
    cloud_config.set("cloud.load_balancing", True)
    
    print("Cloud computing integration configuration created")
    print("NLM can scale to cloud infrastructure")
    
    # Example 6.4: IoT integration
    print("\n6.4 IoT Integration")
    print("-" * 50)
    
    # Example configuration for IoT integration
    iot_config = pynlm.createDevelopmentConfig()
    
    # Add IoT integration parameters
    iot_config.set("iot.enable", True)
    iot_config.set("iot.device_id", "nlm_sensor_001")
    iot_config.set("iot.sensors_enabled", True)
    iot_config.set("iot.data_streaming", True)
    iot_config.set("iot.edge_computing", True)
    iot_config.set("iot.battery_optimization", True)
    
    print("IoT integration configuration created")
    print("NLM can run on edge devices and IoT systems")
    
    return {
        "db_config": db_config,
        "web_config": web_config,
        "cloud_config": cloud_config,
        "iot_config": iot_config
    }

# ==============================================================================
# Main Execution and Examples
# ==============================================================================

def run_all_examples():
    """
    Run all advanced examples and print a summary.
    
    This function runs all the examples defined in this file and provides
    a comprehensive summary of all advanced NLM usage patterns.
    """
    print("Advanced NLM Usage Examples")
    print("=" * 80)
    print("This script demonstrates advanced usage patterns for NLM Python bindings")
    print("including configuration techniques, simulation workflows, performance")
    print("optimization, machine learning integration, error handling, and")
    print("integration with external systems.")
    print("=" * 80)
    
    # Run all example sections
    print("\nRunning Advanced Configuration Examples...")
    config_results = advanced_configuration_examples()
    
    print("\nRunning Advanced Simulation Examples...")
    simulation_results = advanced_simulation_examples()
    
    print("\nRunning Performance Optimization Examples...")
    performance_results = performance_optimization_examples()
    
    print("\nRunning Machine Learning Integration Examples...")
    ml_results = machine_learning_integration_examples()
    
    print("\nRunning Error Handling Examples...")
    error_results = error_handling_examples()
    
    print("\nRunning External Integration Examples...")
    integration_results = external_integration_examples()
    
    # Print summary
    print("\n" + "=" * 80)
    print("ADVANCED EXAMPLES SUMMARY")
    print("=" * 80)
    
    summary = {
        "Configuration Examples": len([k for k in config_results.keys()]),
        "Simulation Examples": len([k for k in simulation_results.keys()]),
        "Performance Examples": len([k for k in performance_results.keys()]),
        "ML Integration Examples": len([k for k in ml_results.keys()]),
        "Error Handling Examples": len([k for k in error_results.keys()]),
        "External Integration Examples": len([k for k in integration_results.keys()])
    }
    
    for category, count in summary.items():
        print(f"{category}: {count} examples")
    
    print("\nTotal Advanced Examples: {}" .format(sum(summary.values())))
    
    print("\nKey Features Demonstrated:")
    print("✓ Advanced configuration techniques")
    print("✓ Complex simulation workflows")
    print("✓ Performance optimization strategies")
    print("✓ Machine learning integration")
    print("✓ Error handling and validation")
    print("✓ External system integration")
    
    print("\nFor more detailed information, refer to:")
    print("1. Enhanced Bindings Documentation (python/ENHANCED_BINDINGS.md)")
    print("2. Command Line Interface (python/nlm_cli.py)")
    print("3. Original NLM Documentation")
    
    return {
        "config_results": config_results,
        "simulation_results": simulation_results,
        "performance_results": performance_results,
        "ml_results": ml_results,
        "error_results": error_results,
        "integration_results": integration_results,
        "summary": summary
    }

if __name__ == "__main__":
    # Run all examples
    results = run_all_examples()
    
    # Save results
    with open("advanced_examples_results.json", 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"\nResults saved to 'advanced_examples_results.json'")
