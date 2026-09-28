#!/usr/bin/env python3
"""
NLM Advanced CLI - Command Line Interface for Advanced Users

This script demonstrates advanced usage of the NLM framework through command-line interface,
showcasing performance optimizations, debugging tools, and advanced simulation modes.
"""

import argparse
import json
import sys
import time
import os
from pathlib import Path

# Add the enhanced API to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import py_nlm_enhanced as pynlm
    NLM_AVAILABLE = True
except ImportError:
    print("Error: NLM enhanced API not available")
    print("Please install with: pip install py_nlm_enhanced")
    sys.exit(1)

def create_advanced_simulation(config_file=None, duration=1000, mode="adaptive"):
    """Create an advanced simulation with performance monitoring"""
    print(f"Creating advanced NLM simulation (mode: {mode}, duration: {duration}s)")
    
    # Load configuration or use defaults
    if config_file and Path(config_file).exists():
        with open(config_file, 'r') as f:
            config = json.load(f)
        print(f"Loaded configuration from: {config_file}")
    else:
        config = None
        print("Using default configuration")
    
    # Create enhanced brain with performance optimization
    brain = pynlm.create_advanced_brain(
        config=config,
        name=f"AdvancedCLI_{int(time.time())}"
    )
    
    # Create world with advanced features
    world = pynlm.create_advanced_world(width=100, height=100)
    world.configure(
        width=100, height=100,
        vision_width=15, vision_height=15,
        dynamic_objects_enabled=True,
        resource_map_enabled=True,
        weather_enabled=True
    )
    
    # Create advanced agent
    agent = pynlm.create_advanced_agent(brain, world)
    agent.setGoal("Learn and adapt to environment efficiently")
    
    # Initialize with performance monitoring
    start_time = time.time()
    brain.initialize(
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True,
        enable_reward=True
    )
    
    # Add performance monitoring callbacks
    def performance_monitor(brain_obj, step):
        """Monitor and log performance metrics"""
        if step % 100 == 0:
            current_time = time.time()
            elapsed = current_time - start_time
            steps_per_second = step / elapsed if elapsed > 0 else 0
            
            print(f"  [Performance] Step {step}/{duration}, "
                  f"{steps_per_second:.1f} steps/sec, "
                  f"Neurons: {brain_obj.get_firing_neuron_count()}, "
                  f"Spikes: {brain_obj.get_total_spike_count()}, "
                  f"Curiosity: {agent.get_curiosity_level():.3f}, "
                  f"Novelty: {agent.get_novelty_level():.3f}")
    
    brain.add_step_callback(performance_monitor)
    
    # Add learning event monitoring
    def learning_monitor(brain_obj, step):
        """Monitor learning events"""
        if brain_obj.stats['learning_events'] > 0:
            print(f"  [Learning] Step {step}: {brain_obj.stats['learning_events']} learning events detected")
    
    brain.add_learning_callback(learning_monitor)
    
    # Simulation loop with advanced debugging
    print(f"Starting simulation loop...")
    
    for step in range(duration):
        # Update world with performance tracking
        world_update_start = time.time()
        world.update(world.timestep)
        world_update_time = time.time() - world_update_start
        
        # Get sensory input with debug logging
        percept_start = time.time()
        percept = world.get_sensory_percept()
        percept_time = time.time() - percept_start
        
        # Process with advanced cognitive system
        process_start = time.time()
        agent.processSensoryInput(percept)
        process_time = time.time() - process_start
        
        # Run neural simulation
        simulation_start = time.time()
        brain.step(step)
        simulation_time = time.time() - simulation_start
        
        # Make decision
        decision_start = time.time()
        action = agent.decodeMotorCommand()
        decision_time = time.time() - decision_start
        
        # Apply action
        action_start = time.time()
        world.applyMotorCommand(action, world.getSimulationTime())
        action_time = time.time() - action_start
        
        # Apply reward and development
        reward = percept.get('internal', [0])[0] if percept.get('internal') else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(world.timestep)
        
        # Log performance if step is complete
        if step % 100 == 0:
            total_time = world_update_time + percept_time + process_time + simulation_time + decision_time + action_time
            print(f"  [Step {step}] Total: {total_time:.4f}s, "
                  f"World: {world_update_time:.4f}s, "
                  f"Percept: {percept_time:.4f}s, "
                  f"Process: {process_time:.4f}s, "
                  f"Simulation: {simulation_time:.4f}s, "
                  f"Decision: {decision_time:.4f}s, "
                  f"Action: {action_time:.4f}s")
    
    # Calculate final statistics
    end_time = time.time()
    total_time = end_time - start_time
    steps_per_second = duration / total_time if total_time > 0 else 0
    
    print(f"\nSimulation completed!")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average performance: {steps_per_second:.2f} steps/second")
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.4f}")
    
    # Get final state summary
    final_state = brain.get_state_summary()
    print(f"\nFinal State Summary:")
    for key, value in final_state.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for sub_key, sub_value in value.items():
                print(f"    {sub_key}: {sub_value}")
        else:
            print(f"  {key}: {value}")
    
    # Get agent performance metrics
    agent_metrics = agent.getPerformanceMetrics()
    print(f"\nAgent Performance Metrics:")
    for key, value in agent_metrics.items():
        print(f"  {key}: {value:.4f}")
    
    # Save results
    results = {
        'simulation_config': {
            'duration': duration,
            'mode': mode,
            'config_file': config_file
        },
        'performance': {
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'final_step': duration
        },
        'neural_stats': final_state,
        'agent_metrics': agent_metrics
    }
    
    # Save to file
    output_file = f"nlm_simulation_{int(time.time())}.json"
    pynlm.save_simulation_results(results, output_file)
    
    print(f"\nResults saved to: {output_file}")
    
    return results

def run_debug_mode(brain, world, agent, steps=100):
    """Run simulation with detailed debugging output"""
    print("\n" + "="*60)
    print("DEBUG MODE - Detailed Simulation Analysis")
    print("="*60)
    
    for step in range(steps):
        print(f"\n--- Step {step} ---")
        
        # Update world
        world.update(world.timestep)
        
        # Get perception
        percept = world.get_sensory_percept()
        print(f"World State: Position {agent.world.agent_position}, "
              f"Vision size: {len(percept.get('vision', []))}")
        print(f"World Properties: Weather={agent.world.weather_conditions}, "
              f"Time={agent.world.time_of_day}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        print(f"Agent State: Curiosity={agent.get_curiosity_level():.3f}, "
              f"Novelty={agent.get_novelty_level():.3f}, "
              f"PredictionError={agent.get_prediction_error():.3f}")
        
        # Brain step
        brain.step(step)
        print(f"Brain Activity: Neurons firing={brain.get_firing_neuron_count()}, "
              f"Total spikes={brain.get_total_spike_count()}, "
              f"Firing rate={brain.get_average_firing_rate():.3f}")
        
        # Decision
        action = agent.decodeMotorCommand()
        print(f"Decision: Action={action}, "
              f"Development Stage={agent.get_developmental_stage().name}")
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.update_development(world.timestep)
        
        # Debug information
        if step % 10 == 0:
            print(f"  Debug: Plasticity modifier={brain.plasticity_modifier:.3f}, "
                  f"Neuromodulation={agent.get_neuromodulation_level():.3f}")
    
    print("\nDebug mode completed")

def create_config_file(config_path):
    """Create example configuration file"""
    config = {
        "brain": {
            "neuron_count": 2000,
            "region_count": 2,
            "synapse_density": 0.08,
            "connection_probability": 0.06,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 20.0
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.001
            },
            "structural": {
                "enable": True,
                "synaptogenesis_rate": 0.00005,
                "pruning_rate": 0.000005
            }
        }
    }
    
    with open(config_path, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Configuration file created: {config_path}")
    return config

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced CLI - Advanced Neural Simulation Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s run --duration 1000 --mode adaptive
  %(prog)s run --config my_config.json --duration 500
  %(prog)s debug --steps 200
  %(prog)s create-config my_config.json
  %(prog)s --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run command
    parser_run = subparsers.add_parser('run', help='Run advanced simulation')
    parser_run.add_argument('--duration', type=int, default=1000,
                           help='Simulation duration in steps (default: 1000)')
    parser_run.add_argument('--mode', choices=['basic', 'learning', 'exploration', 'adaptive', 'continual', 'competitive'],
                           default='adaptive', help='Simulation mode (default: adaptive)')
    parser_run.add_argument('--config', help='Configuration file path (JSON)')
    parser_run.add_argument('--output', help='Output file for results (default: auto-generated)')
    parser_run.add_argument('--debug', action='store_true', help='Enable debug output')
    
    # Debug command
    parser_debug = subparsers.add_parser('debug', help='Run simulation with detailed debugging')
    parser_debug.add_argument('--steps', type=int, default=100, help='Number of debug steps (default: 100)')
    
    # Create config command
    parser_create_config = subparsers.add_parser('create-config', help='Create example configuration file')
    parser_create_config.add_argument('output', help='Output configuration file path')
    
    # Version command
    parser_version = subparsers.add_parser('version', help='Show version information')
    
    # Help command
    parser_help = subparsers.add_parser('help', help='Show detailed help information')
    
    args = parser.parse_args()
    
    # Handle different commands
    if args.command == 'run':
        if args.debug:
            print("Debug mode enabled - detailed output will be shown")
        
        results = create_advanced_simulation(
            config_file=args.config,
            duration=args.duration,
            mode=args.mode
        )
        
        # Save to custom output file if specified
        if args.output:
            pynlm.save_simulation_results(results, args.output)
            print(f"Results also saved to: {args.output}")
        
    elif args.command == 'debug':
        # For debug mode, we need to create a minimal simulation
        print("Debug mode - creating minimal simulation for debugging")
        
        brain = pynlm.create_advanced_brain()
        world = pynlm.create_advanced_world()
        agent = pynlm.create_advanced_agent(brain, world)
        
        brain.initialize(
            enable_learning=True,
            enable_curiosity=True,
            enable_development=True
        )
        
        run_debug_mode(brain, world, agent, args.steps)
        
    elif args.command == 'create-config':
        create_config_file(args.output)
        
    elif args.command == 'version':
        print(f"NLM Advanced CLI Version 2.0.0")
        print(f"Enhanced NLM Python API: {pynlm.__version__}")
        print(f"Description: {pynlm.__description__}")
        print(f"Author: {pynlm.__author__}")
        
    elif args.command == 'help':
        print("NLM Advanced CLI - Advanced Neural Simulation Framework")
        print("=" * 60)
        print("\nAvailable Commands:")
        print("  run      - Run advanced simulation with various modes")
        print("  debug    - Run simulation with detailed debugging output")
        print("  create-config - Create example configuration file")
        print("  version  - Show version information")
        print("  help     - Show this help message")
        print("\nSimulation Modes:")
        print("  basic    - Simple neural simulation")
        print("  learning - Enable online learning")
        print("  exploration - Enable curiosity-driven exploration")
        print("  adaptive - Enable development, learning, and curiosity")
        print("  continual - Continuous adaptation")
        print("  competitive - Competitive learning")
        print("\nPerformance Features:")
        print("  - Advanced memory systems (short-term, long-term, episodic)")
        print("  - Planning and decision systems")
        print("  - Reinforcement learning integration")
        print("  - Cognitive processing engine")
        print("  - Motivation system")
        print("  - Performance monitoring and optimization")
        
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
