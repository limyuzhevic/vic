#!/usr/bin/env python3
"""
Demonstration script showcasing the enhanced NLM Python API.

This script demonstrates the new features and capabilities of the
enhanced NLM Python interface, including presets, parallel execution,
performance monitoring, and advanced error handling.
"""

import sys
import time
import json
import argparse
from pathlib import Path

# Add the pynlm module to the path
sys.path.insert(0, str(Path(__file__).parent))

import pynlm
from pynlm.commands import ExecutionMode
from pynlm.errors import get_logger, NLMConfigurationError
def print_header(title: str):
    """Print a formatted header."""
    print("\n" + "="*60)
    print(f"{title}")
    print("="*60)


def demo_basic_usage():
    """Demonstrate basic usage of the enhanced NLM API."""
    print_header("1. Basic Usage with Enhanced API")
    
    print("Creating brain with preset configuration...")
    brain = pynlm.create_brain_from_preset('quick_test')
    
    print("Initializing brain...")
    brain.initialize()
    
    print("Running simulation (100 steps)...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    elapsed = time.time() - start_time
    print(f"\nSimulation completed in {elapsed:.2f}s")
    print(f"Average performance: {100/elapsed:.1f} steps/second")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Performance rating: {pynlm.get_performance_rating(brain, None)}")
    
    # Show brain summary
    print("\nBrain Summary:")
    print(brain.get_brain_summary())


def demo_presets():
    """Demonstrate different simulation presets."""
    print_header("2. Simulation Presets")
    
    presets = ['quick_test', 'learning_experiment', 'performance_test', 'development_study', 'curiosity_driven']
    
    for preset in presets:
        print(f"\n--- {preset.upper()} Preset ---")
        
        # Create brain with preset
        brain = pynlm.create_brain_from_preset(preset)
        brain.initialize()
        
        # Run short simulation
        for step in range(50):
            brain.step(step)
        
        # Show configuration summary
        config = brain.getConfig()
        neuron_count = config.getOr[int64_t]("brain.neuron_count", 0)
        learning_enabled = config.getOr[bool]("plasticity.stdp.enable", False)
        curiosity_enabled = config.getOr[bool]("neuromod.curiosity.enable", False)
        
        print(f"  Neurons: {neuron_count}")
        print(f"  STDP Learning: {'Enabled' if learning_enabled else 'Disabled'}")
        print(f"  Curiosity: {'Enabled' if curiosity_enabled else 'Disabled'}")
        print(f"  Performance: {brain.getAverageFiringRate():.2f} Hz avg firing rate")


def demo_advanced_setup():
    """Demonstrate advanced setup with world and agent."""
    print_header("3. Advanced Setup with World and Agent")
    
    # Create brain with research preset
    print("Creating research-grade brain...")
    brain = pynlm.create_brain_from_preset('research')
    
    # Create world and agent using utility function
    print("Creating agent-environment setup...")
    world, agent = pynlm.create_agent_environment(
        brain, 
        width=30, 
        height=30, 
        vision_width=10, 
        vision_height=10
    )
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation with monitoring
    print("Running simulation with learning enabled...")
    steps_completed = pynlm.run_simulation_episode(
        world, agent, brain, 200, enable_learning=True
    )
    
    print(f"\nSimulation completed in {steps_completed} steps")
    
    # Show agent statistics
    stats = agent.getStats()
    print("\nAgent Statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    # Show world statistics
    world_body = world.getAgentBody()
    print(f"\nWorld Statistics:")
    print(f"  World size: {world.getWidth()} x {world.getHeight()}")
    print(f"  Agent position: ({world_body.x:.1f}, {world_body.y:.1f})")
    print(f"  Agent energy: {world_body.energy:.1f} / {world.getMaxEnergy():.1f}")
    print(f"  Agent health: {world_body.health:.1f}%")
    print(f"  Simulation time: {world.getSimulationTime():.2f}s")


def demo_parallel_simulation():
    """Demonstrate parallel simulation execution."""
    print_header("4. Parallel Simulation Execution")
    
    print("Setting up parallel simulation...")
    
    # Base configuration
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", 200)
    
    world_config = {
        'width': 20,
        'height': 20,
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'enable_development': True,
    }
    
    # Run parallel simulations
    print("Running 4 parallel simulations...")
    results = pynlm.run_parallel_simulations(
        brain_config, world_config,
        num_simulations=4,
        steps=100,
        mode=ExecutionMode.PARALLEL
    )
    
    # Analyze results
    analysis = pynlm.analyze_simulation_results(results)
    
    print(f"\nParallel Simulation Results:")
    print(f"  Total simulations: {analysis['total_simulations']}")
    print(f"  Successful simulations: {analysis['successful_simulations']}")
    print(f"  Average performance: {analysis['average_performance']}")
    
    if analysis['best_performing_config']:
        print(f"  Best configuration found")
    
    # Show learning analysis
    if analysis['learning_analyses']:
        avg_learning_rate = sum(la['learning_rate'] for la in analysis['learning_analyses']) / len(analysis['learning_analyses'])
        print(f"  Average learning rate: {avg_learning_rate:.3f}")


def demo_error_handling():
    """Demonstrate error handling and validation."""
    print_header("5. Error Handling and Validation")
    
    # Create logger
    logger = pynlm.get_logger('demo', log_level='INFO', enable_file_logging=False)
    error_handler = pynlm.get_error_handler(logger)
    
    # Create configuration with potential issues
    print("Creating configuration with validation...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -100)  # Invalid negative value
    config.set("plasticity.stdp.learning_rate", 1.5)  # Invalid > 1.0
    
    # Validate configuration
    warnings, errors = pynlm.validate_configuration(config)
    
    print(f"\nValidation Results:")
    print(f"  Warnings: {len(warnings)}")
    for warning in warnings:
        print(f"    - {warning}")
    
    print(f"  Errors: {len(errors)}")
    for error in errors:
        print(f"    - {error}")
    
    # Show error handling capabilities
    print("\nError handling features:")
    print("  - Configuration validation with detailed error messages")
    print("  - Simulation error handling with context")
    print("  - Progress tracking with error recovery")
    print("  - Comprehensive logging with multiple levels")


def demo_performance_monitoring():
    """Demonstrate performance monitoring and benchmarking."""
    print_header("6. Performance Monitoring and Benchmarking")
    
    # Create brain with performance preset
    print("Creating performance test brain...")
    brain = pynlm.create_brain_from_preset('performance_test')
    brain.initialize()
    
    # Run benchmark
    print("Running performance benchmark (500 steps)...")
    start_time = time.time()
    
    for step in range(500):
        brain.step(step)
    
    elapsed = time.time() - start_time
    
    # Calculate performance metrics
    performance = {
        'steps_per_second': 500 / elapsed,
        'total_time': elapsed,
        'steps_completed': 500,
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'total_spike_count': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
    }
    
    print(f"\nPerformance Metrics:")
    for key, value in performance.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    # Show performance rating
    print(f"\nPerformance Rating: {pynlm.get_performance_rating(brain, None)}")
    
    # Validate configuration
    warnings = pynlm.validate_configuration(brain.getConfig())
    if warnings:
        print(f"\nConfiguration warnings: {warnings}")


def demo_result_export():
    """Demonstrate result export and analysis."""
    print_header("7. Result Export and Analysis")
    
    # Create and run simulation
    print("Creating and running simulation...")
    brain = pynlm.create_brain_from_preset('learning_experiment')
    
    # Create world and agent
    world, agent = pynlm.create_agent_environment(brain, width=25, height=25)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if agent.isRewardModulationEnabled():
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    # Create result object (simplified)
    print("Exporting simulation results...")
    
    # Export to JSON
    export_data = {
        'brain_stats': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'performance_rating': pynlm.get_performance_rating(brain, None),
        },
        'agent_stats': agent.getStats(),
        'simulation_stats': {
            'steps_completed': 100,
            'world_size': (world.getWidth(), world.getHeight()),
            'learning_enabled': agent.isRewardModulationEnabled(),
        },
        'timestamp': time.time(),
    }
    
    # Save to file
    with open('simulation_results_demo.json', 'w') as f:
        json.dump(export_data, f, indent=2)
    
    print(f"Results saved to 'simulation_results_demo.json'")
    print(f"File size: {len(json.dumps(export_data)):,} characters")
    
    # Load and analyze
    with open('simulation_results_demo.json', 'r') as f:
        loaded_data = json.load(f)
    
    print(f"\nLoaded and analyzed successfully:")
    print(f"  Brain neurons: {loaded_data['brain_stats']['neuron_count']}")
    print(f"  Agent steps: {loaded_data['agent_stats']['step']}")
    print(f"  Performance rating: {loaded_data['brain_stats']['performance_rating']}")


def demo_cli_example():
    """Demonstrate command-line interface usage."""
    print_header("8. Command-Line Interface Example")
    
    print("CLI commands available:")
    print("  python nlm_demo.py --help")
    print("    Display available commands and options")
    
    print("\nExample usage:")
    print("  python nlm_demo.py run --preset learning_experiment")
    print("    Run simulation with learning experiment preset")
    
    print("\n  python nlm_demo.py parallel --num-simulations 4")
    print("    Run 4 parallel simulations")
    
    print("\n  python nlm_demo.py analyze simulation_results.json")
    print("    Analyze simulation results from JSON file")
    
    print("\nFor actual CLI usage, run the script with the desired command.")


def main():
    """Main function to run all demonstrations."""
    parser = argparse.ArgumentParser(
        description="NLM Enhanced Python API Demonstration",
        epilog="Run with --help to see available options."
    )
    
    parser.add_argument(
        '--demo',
        choices=[1, 2, 3, 4, 5, 6, 7, 8],
        default=None,
        help="Run specific demonstration (1-8). If not specified, run all demonstrations."
    )
    
    parser.add_argument(
        '--output',
        help="Output file for results (JSON format)."
    )
    
    args = parser.parse_args()
    
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Presets", demo_presets),
        ("Advanced Setup", demo_advanced_setup),
        ("Parallel Simulation", demo_parallel_simulation),
        ("Error Handling", demo_error_handling),
        ("Performance Monitoring", demo_performance_monitoring),
        ("Result Export", demo_result_export),
        ("CLI Example", demo_cli_example),
    ]
    
    if args.demo:
        demo_name, demo_func = demos[int(args.demo) - 1]
        print(f"\nRunning: {demo_name}")
        print("-" * 60)
        demo_func()
    else:
        print("Running all NLM demonstrations...")
        print("This may take a few minutes. Press Ctrl+C to stop.")
        print()
        
        for demo_name, demo_func in demos:
            try:
                demo_func()
                print(f"\n✓ {demo_name} completed successfully!")
            except Exception as e:
                print(f"\n✗ {demo_name} failed: {e}")
                import traceback
                traceback.print_exc()
            
            input("\nPress Enter to continue to the next demonstration...")
    
    print("\n" + "="*60)
    print("NLM Enhanced Python API Demonstration Complete!")
    print("="*60)
    print("\nFor more information:")
    print("  - Read the Enhanced_API_Summary.md file in the pynlm directory")
    print("  - Check the documentation in the HOW_TO_USE.md file")
    print("  - Visit the NLM project documentation for more details")
if __name__ == "__main__":
    main()
