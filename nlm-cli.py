#!/usr/bin/env python3
"""
NLM Command Line Interface Tools

This module provides command-line tools for the NLM (Neural Learning Machine) project.
It includes tools for running simulations, managing configurations, and analyzing results.
"""

import argparse
import sys
import json
import time
from pathlib import Path
from typing import Optional, List

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. CLI tools will be limited.")

class NLMCliTools:
    """Command-line interface tools for NLM."""
    
    def __init__(self):
        self.available = PYNLM_AVAILABLE
    
    def run_simulation(self, config_file: Optional[str] = None, 
                      steps: int = 100, verbose: bool = False) -> bool:
        """Run a basic brain simulation."""
        if not self.available:
            print("Error: pynlm not available")
            return False
        
        try:
            # Create default configuration
            config = pynlm.createDefaultConfig()
            
            # Load from config file if provided
            if config_file and Path(config_file).exists():
                config.loadFromFile(config_file)
            
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
            print(f"Running {steps} simulation steps...")
            
            # Run simulation
            for step in range(steps):
                brain.step(step)
                
                if verbose and step % 10 == 0:
                    firing = brain.getFiringNeuronCount()
                    spikes = brain.getTotalSpikeCount()
                    print(f"Step {step:4d}: {firing:4d} firing, {spikes:6d} total spikes")
            
            # Print final statistics
            print("\nSimulation Complete!")
            print(f"  Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            
            return True
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return False
    
    def run_phase6_demo(self, world_size: int = 20, vision_size: int = 8, 
                       steps: int = 200) -> bool:
        """Run the Phase 6 integration demo with agent and world."""
        if not self.available:
            print("Error: pynlm not available")
            return False
        
        try:
            print("Running Phase 6 Integration Demo")
            print(f"World size: {world_size}x{world_size}, Vision: {vision_size}x{vision_size}")
            print(f"Running {steps} simulation steps...")
            
            # Create brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(width=world_size, height=world_size,
                          visionWidth=vision_size, visionHeight=vision_size)
            world.reset()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable advanced features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            
            # Run demo
            for step in range(steps):
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain thinks
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                if step % 50 == 0:
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    agent.applyRewardModulation(reward, 0.0)
                    agent.updateDevelopment(0.1)
                
                # Print progress
                if step % 50 == 0:
                    firing = brain.getFiringNeuronCount()
                    curiosity = agent.getCuriosityLevel()
                    novelty = agent.getNoveltyLevel()
                    dev_stage = brain.getDevelopmentalStage()
                    
                    print(f"Step {step:3d}: "
                          f"Neurons: {firing:4d}, "
                          f"Curiosity: {curiosity:.2f}, "
                          f"Novelty: {novelty:.2f}, "
                          f"Dev: {dev_stage.name}")
            
            print("\nPhase 6 Demo Complete!")
            return True
            
        except Exception as e:
            print(f"Error during Phase 6 demo: {e}")
            return False
    
    def manage_config(self, action: str, config_file: Optional[str] = None,
                     key: Optional[str] = None, value: Optional[str] = None) -> bool:
        """Manage configuration files."""
        try:
            config = pynlm.createDefaultConfig()
            
            if action == "load" and config_file:
                if config.loadFromFile(config_file):
                    print(f"Loaded configuration from {config_file}")
                else:
                    print(f"Failed to load configuration from {config_file}")
                    return False
            
            elif action == "save" and config_file:
                if config.saveToFile(config_file):
                    print(f"Saved configuration to {config_file}")
                else:
                    print(f"Failed to save configuration to {config_file}")
                    return False
            
            elif action == "show":
                print("Current configuration:")
                print(config.summary())
            
            elif action == "get" and key:
                if config.has(key):
                    print(f"{key}: {config.getKeys()}")
                else:
                    print(f"Key '{key}' not found")
            
            elif action == "set" and key and value:
                # Try to convert value to appropriate type
                try:
                    # Try int
                    int_val = int(value)
                    config.set(key, int_val)
                except ValueError:
                    try:
                        # Try float
                        float_val = float(value)
                        config.set(key, float_val)
                    except ValueError:
                        # Try bool
                        if value.lower() in ("true", "false"):
                            bool_val = value.lower() == "true"
                            config.set(key, bool_val)
                        else:
                            # String
                            config.set(key, value)
                print(f"Set {key} = {value}")
            
            return True
            
        except Exception as e:
            print(f"Error managing configuration: {e}")
            return False
    
    def analyze_brain(self, brain, output_file: Optional[str] = None) -> bool:
        """Analyze and save brain statistics."""
        try:
            analysis = {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": brain.getDevelopmentalStage().name,
                "region_count": brain.getRegionCount(),
                "region_ids": [region.getId().value for region in brain.getRegions()]
            }
            
            if output_file:
                with open(output_file, 'w') as f:
                    json.dump(analysis, f, indent=2)
                print(f"Analysis saved to {output_file}")
            
            print("Brain Analysis:")
            for key, value in analysis.items():
                if isinstance(value, list):
                    print(f"  {key}: {len(value)} items")
                else:
                    print(f"  {key}: {value}")
            
            return True
            
        except Exception as e:
            print(f"Error analyzing brain: {e}")
            return False
def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli run-simulation --steps 1000
  nlm-cli phase6-demo --world-size 30 --steps 500
  nlm-cli config load configs/my_config.cfg
  nlm-cli config save configs/my_config.cfg
  nlm-cli config set brain.neuron_count 2000
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    sim_parser = subparsers.add_parser('run-simulation', help='Run a basic brain simulation')
    sim_parser.add_argument('--config', '-c', help='Configuration file')
    sim_parser.add_argument('--steps', '-s', type=int, default=100, help='Number of simulation steps')
    sim_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    # Phase 6 demo command
    demo_parser = subparsers.add_parser('phase6-demo', help='Run Phase 6 integration demo')
    demo_parser.add_argument('--world-size', type=int, default=20, help='World size (width and height)')
    demo_parser.add_argument('--vision-size', type=int, default=8, help='Vision dimensions')
    demo_parser.add_argument('--steps', '-s', type=int, default=200, help='Number of simulation steps')
    
    # Configuration management commands
    config_parser = subparsers.add_parser('config', help='Manage configuration')
    config_subparsers = config_parser.add_subparsers(dest='action', help='Configuration actions')
    
    load_parser = config_subparsers.add_parser('load', help='Load configuration from file')
    load_parser.add_argument('config_file', help='Configuration file')
    
    save_parser = config_subparsers.add_parser('save', help='Save configuration to file')
    save_parser.add_argument('config_file', help='Configuration file')
    
    show_parser = config_subparsers.add_parser('show', help='Show current configuration')
    
    get_parser = config_subparsers.add_parser('get', help='Get configuration value')
    get_parser.add_argument('key', help='Configuration key')
    
    set_parser = config_subparsers.add_parser('set', help='Set configuration value')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')
    
    # Analysis command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain and save statistics')
    analyze_parser.add_argument('--output', '-o', help='Output file (JSON)')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI tools
    cli = NLMCliTools()
    
    # Execute command
    if not hasattr(args, 'command') or not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == 'run-simulation':
            success = cli.run_simulation(args.config, args.steps, args.verbose)
        elif args.command == 'phase6-demo':
            success = cli.run_phase6_demo(args.world_size, args.vision_size, args.steps)
        elif args.command == 'config':
            if not hasattr(args, 'action') or not args.action:
                config_parser.print_help()
                return 1
            
            if args.action == 'load':
                success = cli.manage_config('load', args.config_file)
            elif args.action == 'save':
                success = cli.manage_config('save', args.config_file)
            elif args.action == 'show':
                success = cli.manage_config('show')
            elif args.action == 'get':
                success = cli.manage_config('get', None, args.key)
            elif args.action == 'set':
                success = cli.manage_config('set', None, args.key, args.value)
            else:
                config_parser.print_help()
                return 1
        elif args.command == 'analyze':
            # This would require a brain object - we'll skip for now
            print("Analyze command requires a brain object. Use with run-simulation or phase6-demo first.")
            return 1
        else:
            parser.print_help()
            return 1
        
        return 0 if success else 1
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())
