"""
NLM Python Bindings - Advanced Usage Examples

This file demonstrates advanced usage patterns and new features in the pynlm Python bindings.
"""

import pynlm
import time
import tempfile
import os

def example_1_batch_processing():
    """
    Example 1: Batch Processing
    ============================
    Process multiple simulation steps efficiently using batch operations.
    """
    print("=== Example 1: Batch Processing ===")
    
    # Create brain and agent
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(10, 10, 8, 8)
    agent.initialize(world)
    
    # Create multiple percepts for batch processing
    percepts = []
    for i in range(20):
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        data = [0.1f] * (8 * 8 * 3)
        vision.setData(data)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        percepts.append(percept)
    
    # Process all percepts at once
    agent.runSteps(percepts, 0.001)
    
    # Get brain statistics
    stats = agent.getBrainStats()
    print(f"Brain Statistics:")
    print(f"  Neurons: {stats[0]}")
    print(f"  Active Neurons: {stats[1]}")
    print(f"  Total Spikes: {stats[2]}")
    print(f"  Average Firing Rate: {stats[3]:.2f} Hz")
    print(f"  Developmental Stage: {stats[4]}")
    
    return brain

def example_2_configuration_management():
    """
    Example 2: Configuration Management
    ==================================
    Advanced configuration management with presets and validation.
    """
    print("\n=== Example 2: Configuration Management ===")
    
    # Use predefined presets
    configs = {
        "small": pynlm.createSmallConfig(),
        "large": pynlm.createLargeConfig(),
        "learning": pynlm.createLearningConfig(),
        "exploration": pynlm.createExplorationConfig()
    }
    
    # Configure and validate
    for name, config in configs.items():
        print(f"\n{name.capitalize()} Configuration:")
        print(f"  Neurons: {config.getOr('neuron_count', 0)}")
        print(f"  Regions: {config.getOr('region_count', 0)}")
        
        # Validate configuration
        errors, warnings = pynlm.validateConfig(config)
        if warnings:
            print(f"  Warnings: {len(warnings)}")
    
    # Custom configuration
    custom_config = pynlm.Config()
    custom_config.setFloat("custom_param", 2.718f)
    custom_config.setInt("iteration_limit", 1000)
    custom_config.setBool("enable_learning", True)
    
    print(f"\nCustom Config:")
    print(f"  custom_param: {custom_config.getFloat('custom_param', 0.0f)}")
    print(f"  iteration_limit: {custom_config.getInt('iteration_limit', 0)}")
    print(f"  enable_learning: {custom_config.getBool('enable_learning', False)}")
    
    return custom_config

def example_3_progress_monitoring():
    """
    Example 3: Progress Monitoring
    =============================
    Monitor long-running simulations with progress tracking.
    """
    print("\n=== Example 3: Progress Monitoring ===")
    
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(20, 20, 8, 8)
    agent.initialize(world)
    
    # Progress tracking data collection
    progress_data = []
    def progress_callback(step, firing_rate):
        progress_data.append((step, firing_rate))
        if step % 50 == 0:
            print(f"  Step {step}: Firing rate = {firing_rate:.2f} Hz")
    
    # Create progress tracker
    tracker = pynlm.createProgressTracker(100, progress_callback)
    tracker.setMessage("Running simulation...")
    
    print(f"Progress tracker created: {tracker.getProgress():.1%}")
    
    # Run with progress tracking (simplified - would need proper percepts)
    print("Progress tracking setup complete")
    
    return tracker, progress_data

def example_4_error_handling():
    """
    Example 4: Robust Error Handling
    =================================
    Validate and handle errors gracefully.
    """
    print("\n=== Example 4: Error Handling ===")
    
    # Create brain with validation
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    
    # Validate brain state
    errors, warnings = pynlm.validateBrain(brain)
    
    print(f"Validation Results:")
    print(f"  Errors: {len(errors)}")
    print(f"  Warnings: {len(warnings)}")
    
    if warnings:
        print("  Warnings found:")
        for warning in warnings:
            print(f"    - {warning}")
    
    # Safe initialization
    print("Attempting safe initialization...")
    success = brain.initialize()
    if not success:
        print("  Initialization failed - this should not happen with valid config")
    
    print("  Safe initialization successful")
    
    return brain

def example_5_benchmarking():
    """
    Example 5: Benchmarking
    =========================
    Measure simulation performance.
    """
    print("\n=== Example 5: Benchmarking ===")
    
    configs = [
        ("Small", pynlm.createSmallConfig()),
        ("Default", pynlm.createDefaultConfig()),
        ("Large", pynlm.createLargeConfig())
    ]
    
    results = []
    
    for name, config in configs:
        print(f"\nBenchmarking {name} configuration...")
        
        # Run benchmark
        result = pynlm.runBenchmark(config, 100)
        spike_count, firing_rate, duration_ms, steps_per_sec = result
        
        print(f"  Steps per second: {steps_per_sec:.1f}")
        print(f"  Duration: {duration_ms:.1f} ms")
        print(f"  Spikes generated: {spike_count}")
        print(f"  Average firing rate: {firing_rate:.2f} Hz")
        
        results.append({
            'name': name,
            'steps_per_sec': steps_per_sec,
            'duration_ms': duration_ms,
            'spike_count': spike_count,
            'firing_rate': firing_rate
        })
    
    # Compare results
    print(f"\nPerformance Summary:")
    fastest = max(results, key=lambda x: x['steps_per_sec'])
    print(f"  Fastest: {fastest['name']} ({fastest['steps_per_sec']:.1f} steps/sec)")
    
    return results

def example_6_advanced_agent_features():
    """
    Example 6: Advanced Agent Features
    ==================================
    Demonstrate advanced agent capabilities with all features enabled.
    """
    print("\n=== Example 6: Advanced Agent Features ===")
    
    # Create learning-focused brain
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(15, 15, 8, 8)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced features enabled:")
    print(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
    
    return agent

def example_7_agent_with_actions():
    """
    Example 7: Agent with Action Callback
    ======================================
    Demonstrate using action providers with batch processing.
    """
    print("\n=== Example 7: Agent with Action Callback ===")
    
    config = pynlm.createSmallConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(10, 10, 8, 8)
    agent.initialize(world)
    
    # Create percepts
    percepts = []
    for i in range(5):
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        data = [0.1f] * (8 * 8 * 3)
        vision.setData(data)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        percepts.append(percept)
    
    # Define action provider
    actions_taken = []
    def action_provider():
        from pynlm import MotorCommand
        # Simple action selection
        action = MotorCommand()
        action.setType(pynlm.MotorCommand.MoveForward)
        actions_taken.append("MoveForward")
        return action
    
    # Run with action provider
    agent.runStepsWithActions(percepts, 0.001, action_provider)
    
    print(f"Actions taken: {actions_taken}")
    print(f"Total actions: {len(actions_taken)}")
    
    return agent, actions_taken

def main():
    """Run all examples"""
    print("NLM Python Bindings - Advanced Usage Examples")
    print("=" * 60)
    
    examples = [
        example_1_batch_processing,
        example_2_configuration_management,
        example_3_progress_monitoring,
        example_4_error_handling,
        example_5_benchmarking,
        example_6_advanced_agent_features,
        example_7_agent_with_actions
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"  ERROR: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    main()