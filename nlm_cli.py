#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

This CLI provides command-line access to the NLM neural simulation framework.
It supports running simulations, managing brain states, and experimenting with
artificial neural networks.
"""

import argparse
import json
import sys
import os
from typing import Optional

# Try to import NLM, with helpful error message
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Error: Could not import pynlm: {e}")
    print("Please install the NLM Python bindings using:")
    print("  pip install pynlm")
    sys.exit(1)


def create_brain_from_config(config_file: Optional[str] = None) -> pynlm.Brain:
    """Create a brain with specified configuration."""
    if config_file:
        print(f"Loading configuration from {config_file}")
        # For now, we'll create a default config
        # In a full implementation, this would parse the config file
        pass
    
    config = pynlm.createDefaultConfig()
    
    # Standard configuration for CLI
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1f)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


def run_basic_simulation(brain: pynlm.Brain, steps: int, verbose: bool = False):
    """Run a basic simulation with the given brain."""
    print(f"Running basic simulation for {steps} steps...")
    
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    initial_synapses = brain.getTotalSynapseCount()
    
    for step in range(steps):
        brain.step(step)
        
        if verbose and (step % 100 == 0 or step == steps - 1):
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            spikes = brain.getTotalSpikeCount() - initial_spikes
            print(f"  Step {step:4d}: {firing:3d} firing neurons, "
                  f"{rate:6.2f} avg rate, {spikes:4d} total spikes")
    
    final_spikes = brain.getTotalSpikeCount() - initial_spikes
    final_firing = brain.getFiringNeuronCount()
    final_rate = brain.getAverageFiringRate()
    
    print(f"\nSimulation Results:")
    print(f"  Total neurons: {initial_neurons}")
    print(f"  Total synapses: {initial_synapses}")
    print(f"  Total steps: {steps}")
    print(f"  Final firing neurons: {final_firing}")
    print(f"  Average firing rate: {final_rate:.2f}")
    print(f"  Total spikes generated: {final_spikes}")
    print(f"  Spikes per step: {final_spikes / steps:.2f}")


def run_agent_simulation(config_file: Optional[str] = None, steps: int = 1000):
    """Run a complete agent simulation with brain, world, and agent."""
    print(f"Running agent simulation for {steps} steps...")
    
    # Create components
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"  Brain initialized: {brain.getTotalNeuronCount()} neurons")
    print(f"  World configured: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Agent created with capabilities: reward, curiosity, development")
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward based on position (stay in bounds)
        body = world.getAgentBody()
        if body and body.x >= 0 and body.x < world.getWidth() and \
           body.y >= 0 and body.y < world.getHeight():
            reward = 1.0
        else:
            reward = -1.0
            
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            firing = brain.getFiringNeuronCount()
            print(f"  Step {step:4d}: curiosity={curiosity:.2f}, novelty={novelty:.2f}, "
                  f"firing neurons={firing}")
    
    print(f"\nAgent Simulation Results:")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Total neurons used: {brain.getTotalNeuronCount()}")
    print(f"  Active synapses: {brain.getTotalSynapseCount()}")


def save_brain_state(brain: pynlm.Brain, filepath: str):
    """Save brain state to file."""
    print(f"Saving brain state to {filepath}...")
    
    try:
        brain.save(filepath)
        print(f"  Successfully saved brain state to {filepath}")
        return True
    except Exception as e:
        print(f"  Error saving brain state: {e}")
        return False


def load_brain_state(filepath: str, config: Optional[pynlm.Config] = None):
    """Load brain state from file."""
    print(f"Loading brain state from {filepath}...")
    
    try:
        if config:
            brain = pynlm.createBrain(config)
            brain.initialize()
        else:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
        success = brain.load(filepath)
        
        if success:
            print(f"  Successfully loaded brain state from {filepath}")
            print(f"  Loaded {brain.getTotalNeuronCount()} neurons")
            print(f"  Loaded {brain.getTotalSynapseCount()} synapses")
            return brain
        else:
            print(f"  Failed to load brain state from {filepath}")
            return None
            
    except Exception as e:
        print(f"  Error loading brain state: {e}")
        return None


def print_brain_info(brain: pynlm.Brain):
    """Print detailed information about the brain."""
    print("=== NLM Brain Information ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"Total spike count: {brain.getTotalSpikeCount()}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Number of regions: {brain.getRegionCount()}")
    
    regions = brain.getRegionIds()
    if regions:
        print(f"Region IDs: {[str(r) for r in regions]}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s basic --steps 1000 --verbose
  %(prog)s agent --steps 500
  %(prog)s save brain_state.bin
  %(prog)s load brain_state.bin
  %(prog)s info
  %(prog)s experiment --steps 2000

For more detailed help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    # Top-level subparsers
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Basic simulation command
    parser_basic = subparsers.add_parser('basic', help='Run basic brain simulation')
    parser_basic.add_argument('--steps', type=int, default=1000,
                             help='Number of simulation steps (default: 1000)')
    parser_basic.add_argument('--verbose', action='store_true',
                            help='Print verbose progress information')
    
    # Agent simulation command
    parser_agent = subparsers.add_parser('agent', help='Run agent simulation with world interaction')
    parser_agent.add_argument('--steps', type=int, default=1000,
                             help='Number of simulation steps (default: 1000)')
    
    # Save brain state command
    parser_save = subparsers.add_parser('save', help='Save brain state to file')
    parser_save.add_argument('filepath', help='Path to save brain state file')
    
    # Load brain state command
    parser_load = subparsers.add_parser('load', help='Load brain state from file')
    parser_load.add_argument('filepath', help='Path to brain state file')
    
    # Info command
    parser_info = subparsers.add_parser('info', help='Display brain information')
    
    # Experiment command
    parser_experiment = subparsers.add_parser('experiment', help='Run experiment with enhanced logging')
    parser_experiment.add_argument('--steps', type=int, default=2000,
                                  help='Number of simulation steps (default: 2000)')
    parser_experiment.add_argument('--experiment-type', choices=['basic', 'development', 'learning', 'comprehensive'], 
                                  default='comprehensive',
                                  help='Type of experiment to run (default: comprehensive)')
    
    # List available experiments
    parser_list = subparsers.add_parser('list', help='List available experiments and features')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        sys.exit(1)
    
    try:
        if args.command == 'basic':
            brain = create_brain_from_config()
            run_basic_simulation(brain, args.steps, args.verbose)
            
        elif args.command == 'agent':
            run_agent_simulation(None, args.steps)
            
        elif args.command == 'save':
            brain = create_brain_from_config()
            run_basic_simulation(brain, 100)  # Brief simulation to have something to save
            save_brain_state(brain, args.filepath)
            
        elif args.command == 'load':
            config = pynlm.createDefaultConfig()
            brain = load_brain_state(args.filepath, config)
            if brain:
                print_brain_info(brain)
                
        elif args.command == 'info':
            brain = create_brain_from_config()
            run_basic_simulation(brain, 10)  # Brief run for demo
            print_brain_info(brain)
            
        elif args.command == 'experiment':
            run_agent_simulation(None, args.steps)
            print("\nExperiment completed. Check logs for detailed metrics.")
            
        elif args.command == 'list':
            print("=== Available NLM Features ===")
            print("Basic simulation:")
            print("  - Basic neural computation")
            print("  - Synaptic plasticity (STDP, Hebbian)")
            print("  - Real-time statistics monitoring")
            print()
            print("Agent simulation:")
            print("  - Integration with world environment")
            print("  - Sensory perception and motor control")
            print("  - Reward-based learning")
            print("  - Neuromodulation (curiosity, novelty)")
            print()
            print("Brain management:")
            print("  - Save/load brain states")
            print("  - Checkpoint restoration")
            print("  - Configuration management")
            print()
            print("Visualization and monitoring:")
            print("  - Real-time firing rate statistics")
            print("  - Development stage tracking")
            print("  - Neuromodulation levels")
            print()
            print("For complete documentation, visit: https://nlm.readthedocs.io")
            
        else:
            parser.print_help()
            
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
