"""
NLM (Neural Learning Machine) Enhanced Python Examples
=====================================================

Comprehensive examples for using the enhanced NLM Python bindings.
This file contains examples for beginners and advanced users.
"""

import pynlm
import time
import json
from pathlib import Path

# Import CLI tool for advanced operations
try:
    import sys
    sys.path.insert(0, 'python')
    from nlm_cli import (
        create_brain_from_config,
        create_complete_system,
        run_simulation,
        analyze_brain,
        benchmark_brain,
        create_test_environment,
        validate_environment,
        save_results,
        load_results
    )
    CLI_AVAILABLE = True
except ImportError:
    CLI_AVAILABLE = False
    print("Note: CLI tools not available. Some examples will be limited.")

def example_1_basic_usage():
    """
    Example 1: Basic NLM Usage
    ==========================
    
    This is the simplest way to create and run a neural simulation.
    """
    print("=== Example 1: Basic NLM Usage ===")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getRegionCount()} regions")
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
    
    # Get final statistics
    stats = brain.getBrainStats()
    print(f"Final firing rate: {stats['firing_rate']:.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()

def example_2_complete_system():
    """
    Example 2: Complete NLM System
    ================================
    
    Create a complete NLM system with brain, world, and agent.
    This is the typical usage pattern for real-world simulations.
    """
    print("=== Example 2: Complete NLM System ===")
    
    # Create complete system
    brain, world, agent = pynlm.createCompleteSystem()
    
    print("Created complete NLM system:")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  - World: {world.getWidth()}x{world.getHeight()}m")
    print(f"  - Agent starting position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Enabled learning systems: Reward, Curiosity, Development")
    
    # Run simulation
    brain, world, agent = pynlm.runSimpleSimulation(brain, world, agent, 500)
    
    # Get final statistics
    brain_stats = brain.getBrainStats()
    agent_stats = {
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'reward': agent.getNeuromodulationLevel(),
        'development': agent.getDevelopmentalStage()
    }
    
    print("\nFinal statistics:")
    print(f"  Brain firing rate: {brain_stats['firing_rate']:.3f}")
    print(f"  Agent curiosity: {agent_stats['curiosity']:.3f}")
    print(f"  Agent novelty: {agent_stats['novelty']:.3f}")
    print(f"  Agent development stage: {agent_stats['development']}")
    print()

def example_3_configuration_management():
    """
    Example 3: Configuration Management
    ===================================
    
    Learn how to work with configurations in NLM.
    """
    print("=== Example 3: Configuration Management ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"Default config: {config.summary()}")
    
    # Modify configuration
    config.setInt("neuron_count", 5000)
    config.setFloat("connection_probability", 0.15f)
    config.setInt("region_count", 3)
    
    print("Modified configuration:")
    print(f"  - Neurons: {config.getInt('neuron_count')}")
    print(f"  - Connection probability: {config.getFloat('connection_probability')}")
    print(f"  - Regions: {config.getInt('region_count')}")
    
    # Validate configuration
    errors, warnings = config.validate()
    if errors:
        print("\nConfiguration errors:")
        for error in errors:
            print(f"  - {error}")
    
    if warnings:
        print("\nConfiguration warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    
    # Create derived configuration
    derived_config = config.createDerived("_production")
    print(f"\nCreated derived config: {derived_config.summary()}")
    
    # Export/import configuration
    config_dict = config.exportToDict()
    print(f"\nExported config keys: {list(config_dict.keys())}")
    
    # Load from dictionary
    new_config = pynlm.createDefaultConfig()
    new_config.importFromDict(config_dict)
    print(f"Imported config neuron count: {new_config.getInt('neuron_count')}")
    print()

def example_4_network_analysis():
    """
    Example 4: Network Analysis
    ==========================
    
    Analyze the structure and properties of the neural network.
    """
    print("=== Example 4: Network Analysis ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get comprehensive brain statistics
    stats = brain.getBrainStats()
    print("Brain statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Get connectivity statistics
    connectivity = brain.getConnectivityStats()
    print("\nConnectivity statistics:")
    for key, value in connectivity.items():
        print(f"  {key}: {value}")
    
    # Get neuron distribution
    distribution = brain.getNeuronDistribution()
    print("\nNeuron distribution:")
    for key, value in distribution.items():
        print(f"  {key}: {value}")
    
    # Get region statistics
    regions = brain.getRegionStats()
    print("\nRegion statistics:")
    for i, region_stats in enumerate(regions):
        print(f"  Region {i}:")
        for key, value in region_stats.items():
            print(f"    {key}: {value}")
    
    # Network activity analysis
    activity = brain.getNetworkActivity()
    print(f"\nNetwork activity: {activity}")
    
    # Learning progress analysis
    learning = brain.getLearningProgress()
    print(f"Learning progress: {learning}")
    print()

def example_5_simulation_benchmark():
    """
    Example 5: Simulation Benchmarking
    ====================================
    
    Benchmark brain performance with different network sizes.
    """
    print("=== Example 5: Simulation Benchmarking ===")
    
    if not CLI_AVAILABLE:
        print("CLI tools not available. Skipping benchmark example.")
        return
    
    # Run benchmarks
    neuron_counts = [1000, 5000, 10000, 20000]
    results = benchmark_brain(neuron_counts, 1000)
    
    print("\nBenchmark results:")
    print("Neurons | Steps | Time (ms) | Steps/sec | Spikes/step")
    print("-" * 70)
    
    for result in results:
        print(f"{result['neurons']:7d} | "
              f"{result['steps']:5d} | "
              f"{result['time_ms']:9.1f} | "
              f"{result['steps_per_second']:10.0f} | "
              f"{result['spikes_per_step']:11.2f}")
    
    # Save results
    save_results(results, "benchmark_results.json")
    print(f"\nResults saved to benchmark_results.json")
    print()

def example_6_advanced_agent():
    """
    Example 6: Advanced Agent Behavior
    ==================================
    
    Create an agent with advanced learning capabilities.
    """
    print("=== Example 6: Advanced Agent Behavior ===")
    
    # Create complete system
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Enable all learning systems
    agent.enableAllSystems()
    
    print("Enabled all learning systems")
    
    # Create custom action
    action = pynlm.Action(pynlm.ActionType.Custom)
    action.addParameter(0.5f)  # Action strength
    action.addParameter(0.3f)  # Action direction
    action.addParameter(0.8f)  # Action confidence
    
    # Normalize action parameters
    action.normalizeParameters(0.0f, 1.0f)
    
    print(f"Created custom action with {action.getParameterCount()} parameters")
    print(f"Action type: {action.getType()}")
    print(f"Normalized parameters: {action.getParameters()}")
    
    # Run simulation with agent learning
    print("\nRunning simulation with agent learning...")
    
    for step in range(1000):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation with curiosity
        reward = 0.5f + (agent.getCuriosityLevel() * 0.5f)
        agent.applyRewardModulation(reward, 0.0f)
        
        agent.updateDevelopment(0.1)
    
    # Get final agent statistics
    agent_stats = agent.getAgentStats()
    print(f"\nFinal agent statistics:")
    for key, value in agent_stats.items():
        print(f"  {key}: {value}")
    
    # Get behavior profile
    behavior = agent.getBehaviorProfile()
    print(f"\nAgent behavior profile: {behavior}")
    print()

def example_7_environment_setup():
    """
    Example 7: Environment Setup
    ============================
    
    Create and manage test environments for NLM experiments.
    """
    print("=== Example 7: Environment Setup ===")
    
    # Create test environment
    world = pynlm.createTestEnvironment(width=30, height=30, resources=8, hazards=5)
    
    print(f"Created test environment: {world.getWidth()}x{world.getHeight()}m")
    
    # Validate environment
    validation = validate_environment(world)
    valid_count = sum(validation.values())
    total_count = len(validation)
    
    print(f"\nEnvironment validation: {valid_count}/{total_count} checks passed")
    
    # Get environment statistics
    env_stats = {
        'world_size': world.getWidth() * world.getHeight(),
        'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
        'agent_energy': world.getAgentBody().energy,
        'agent_health': world.getAgentBody().health
    }
    
    print(f"Environment statistics:")
    for key, value in env_stats.items():
        print(f"  {key}: {value}")
    
    # Save environment configuration
    env_config = {
        'width': world.getWidth(),
        'height': world.getHeight(),
        'vision_width': world.getVisionWidth(),
        'vision_height': world.getVisionHeight(),
        'max_energy': world.getMaxEnergy()
    }
    
    with open('environment_config.json', 'w') as f:
        json.dump(env_config, f, indent=2)
    
    print(f"\nEnvironment configuration saved to environment_config.json")
    print()

def example_8_network_serialization():
    """
    Example 8: Network Serialization
    =================================
    
    Save and load neural networks for later use.
    """
    print("=== Example 8: Network Serialization ===")
    
    # Create and train a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for step in range(500):
        brain.step(step)
    
    # Get initial statistics
    print(f"Initial stats - Neurons: {brain.getTotalNeuronCount()}, "
          f"Spikes: {brain.getTotalSpikeCount()}, "
          f"Firing: {brain.getFiringNeuronCount()}")
    
    # Save brain state
    brain.save("trained_brain.bin")
    print("\nBrain state saved to trained_brain.bin")
    
    # Create new brain and load state
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    
    # Load the saved brain
    if new_brain.load("trained_brain.bin"):
        print("Brain state loaded successfully")
        print(f"Loaded stats - Neurons: {new_brain.getTotalNeuronCount()}, "
              f"Spikes: {new_brain.getTotalSpikeCount()}, "
              f"Firing: {new_brain.getFiringNeuronCount()}")
    else:
        print("Failed to load brain state")
    
    # Create network snapshot
    snapshot = brain.createNetworkSnapshot()
    print(f"\nNetwork snapshot created: {len(snapshot)} items")
    
    # Compare brain states
    if brain.load("trained_brain.bin"):
        print("\nBrain state reloaded - comparing with original...")
        
        # Get network statistics
        original_stats = {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
        
        new_stats = {
            'neurons': new_brain.getTotalNeuronCount(),
            'synapses': new_brain.getTotalSynapseCount(),
            'firing_rate': new_brain.getAverageFiringRate()
        }
        
        print(f"Original stats: {original_stats}")
        print(f"Loaded stats: {new_stats}")
    
    # Clean up
    import os
    if os.path.exists("trained_brain.bin"):
        os.remove("trained_brain.bin")
        print("\nCleaned up temporary files")
    
    print()

def example_9_cli_integration():
    """
    Example 9: CLI Integration
    ==========================
    
    Use the NLM Command Line Interface for advanced operations.
    """
    print("=== Example 9: CLI Integration ===")
    
    if not CLI_AVAILABLE:
        print("CLI tools not available. Install with: pip install pynlm")
        return
    
    print("NLM CLI tools available!")
    print("\nAvailable commands:")
    print("  - nlm create-brain --neuron-count 5000")
    print("  - nlm create-system")
    print("  - nlm run --steps 1000 --verbose")
    print("  - nlm analyze --output analysis.json")
    print("  - nlm benchmark --neurons 1000,5000,10000")
    print("  - nlm create-env --width 50 --height 50")
    print("  - nlm validate")
    
    print(f"\nFor more information, run: python -m pynlm.cli help")
    print()

def example_10_performance_monitoring():
    """
    Example 10: Performance Monitoring
    ==================================
    
    Monitor and analyze simulation performance over time.
    """
    print("=== Example 10: Performance Monitoring ===")
    
    # Create test environment
    world = pynlm.createTestEnvironment(20, 20, 5, 3)
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Enable learning systems
    agent.enableAllSystems()
    
    print("Starting performance monitoring...")
    
    # Collect performance data
    performance_data = []
    start_time = time.time()
    
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record performance metrics
        step_time = time.time() - start_time
        firing_rate = brain.getAverageFiringRate()
        neurons_active = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        performance_data.append({
            'step': step,
            'time_elapsed': step_time,
            'firing_rate': firing_rate,
            'neurons_active': neurons_active,
            'total_spikes': total_spikes,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'reward': agent.getNeuromodulationLevel()
        })
        
        # Apply reward modulation
        reward = (agent.getCuriosityLevel() + agent.getNoveltyLevel()) / 2.0
        agent.applyRewardModulation(reward, 0.0f)
        
        agent.updateDevelopment(0.1)
    
    # Analyze performance data
    avg_firing_rate = sum(d['firing_rate'] for d in performance_data) / len(performance_data)
    avg_time_per_step = sum(d['time_elapsed'] for d in performance_data) / len(performance_data)
    max_firing_rate = max(d['firing_rate'] for d in performance_data)
    min_firing_rate = min(d['firing_rate'] for d in performance_data)
    
    print(f"\nPerformance Analysis:")
    print(f"  Total steps: {len(performance_data)}")
    print(f"  Average firing rate: {avg_firing_rate:.3f}")
    print(f"  Average time per step: {avg_time_per_step:.4f}s")
    print(f"  Max firing rate: {max_firing_rate:.3f}")
    print(f"  Min firing rate: {min_firing_rate:.3f}")
    print(f"  Steps per second: {len(performance_data) / (performance_data[-1]['time_elapsed']):.0f}")
    
    # Save performance data
    with open('performance_data.json', 'w') as f:
        json.dump(performance_data, f, indent=2)
    
    print(f"\nPerformance data saved to performance_data.json")
    
    # Create performance plot (if matplotlib available)
    try:
        import matplotlib.pyplot as plt
        
        steps = [d['step'] for d in performance_data]
        firing_rates = [d['firing_rate'] for d in performance_data]
        curiosities = [d['curiosity'] for d in performance_data]
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
        
        ax1.plot(steps, firing_rates, 'b-', alpha=0.7, label='Firing Rate')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Firing Rate')
        ax1.set_title('NLM Simulation Performance: Firing Rate')
        ax1.grid(True, alpha=0.3)
        ax1.legend()
        
        ax2.plot(steps, curiosities, 'r-', alpha=0.7, label='Curiosity')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Curiosity Level')
        ax2.set_title('NLM Simulation Performance: Curiosity')
        ax2.grid(True, alpha=0.3)
        ax2.legend()
        
        plt.tight_layout()
        plt.savefig('performance_plot.png', dpi=150)
        plt.close()
        
        print("Performance plot saved to performance_plot.png")
    
    except ImportError:
        print("Matplotlib not available - skipping performance plot")
    
    print()

def run_all_examples():
    """Run all examples"""
    print("NLM Enhanced Python Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_complete_system,
        example_3_configuration_management,
        example_4_network_analysis,
        example_5_simulation_benchmark,
        example_6_advanced_agent,
        example_7_environment_setup,
        example_8_network_serialization,
        example_9_cli_integration,
        example_10_performance_monitoring,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
            print("-" * 50)
        except Exception as e:
            print(f"Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
            print("-" * 50)
    
    print("\nAll examples completed!")
    print("\nFor more information and documentation, visit:")
    print("  - https://github.com/nlm-project/nlm")
    print("  - https://nlm.readthedocs.io")

if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--run-all':
        # Run all examples
        run_all_examples()
    else:
        # Print help
        print(__doc__)
        print("\nUsage:")
        print("  python examples.py                    # Show this help")
        print("  python examples.py --run-all          # Run all examples")
        print("\nFor more examples, visit the examples directory.")