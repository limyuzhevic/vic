#!/usr/bin/env python3
"""
NLM Python CLI Tool
===================

Command-line interface for NLM (Neural Learning Machine) operations.

This tool provides convenient access to common NLM operations from the command line.
"""

import argparse
import json
import sys
import os
from pathlib import Path
import pynlm

class NLMCLI:
    """NLM Command Line Interface."""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="NLM Python CLI Tool - Neural Learning Machine operations",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  # Create a simple brain
  %(prog)s create --neurons 1000
  
  # Run a simulation
  %(prog)s simulate --steps 500 --config config.json
  
  # Create an agent and run episode
  %(prog)s agent --world-width 20 --world-height 20 --steps 300
  
  # Save brain state
  %(prog)s save --file brain_state.nlm
  
  # Load brain state
  %(prog)s load --file brain_state.nlm
  
  # Show statistics
  %(prog)s stats
"""
        )
        
        # Main subcommands
        subparsers = self.parser.add_subparsers(dest='command', help='Command to execute')
        
        # Create brain command
        self._add_create_command(subparsers)
        
        # Simulate command
        self._add_simulate_command(subparsers)
        
        # Agent command
        self._add_agent_command(subparsers)
        
        # Stats command
        self._add_stats_command(subparsers)
        
        # Save command
        self._add_save_command(subparsers)
        
        # Load command
        self._add_load_command(subparsers)
        
        # Config command
        self._add_config_command(subparsers)
        
        # Example command
        self._add_example_command(subparsers)
        
        # State command
        self._add_state_command(subparsers)
        
        self.brain = None
        self.config = None
    
    def _add_create_command(self, subparsers):
        """Add create brain command."""
        parser = subparsers.add_parser('create', help='Create a brain with configuration')
        parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
        parser.add_argument('--regions', type=int, default=1, help='Number of brain regions')
        parser.add_argument('--connection-prob', type=float, default=0.1, help='Connection probability')
        parser.add_argument('--stdp-ltp', type=float, default=0.01, help='STDP LTP weight')
        parser.add_argument('--stdp-ltd', type=float, default=0.012, help='STDP LTD weight')
        parser.add_argument('--stdp-tau', type=float, default=20.0, help='STDP time constant')
        parser.add_argument('--name', type=str, default='brain', help='Brain name')
        parser.add_argument('--output', type=str, help='Save configuration to file')
        
        parser.set_defaults(func=self._create_brain)
    
    def _add_simulate_command(self, subparsers):
        """Add simulate command."""
        parser = subparsers.add_parser('simulate', help='Run simulation')
        parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
        parser.add_argument('--brain-file', type=str, help='Load brain state from file')
        parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
        parser.add_argument('--verbose', action='store_true', help='Verbose output')
        parser.add_argument('--output', type=str, help='Save brain state to file')
        
        parser.set_defaults(func=self._simulate)
    
    def _add_agent_command(self, subparsers):
        """Add agent command."""
        parser = subparsers.add_parser('agent', help='Run agent simulation')
        parser.add_argument('--world-width', type=int, default=20, help='World width')
        parser.add_argument('--world-height', type=int, default=20, help='World height')
        parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
        parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
        parser.add_argument('--steps', type=int, default=200, help='Number of steps')
        parser.add_argument('--start-x', type=float, default=10.0, help='Agent start X position')
        parser.add_argument('--start-y', type=float, default=10.0, help='Agent start Y position')
        parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
        parser.add_argument('--output', type=str, help='Save results to file')
        
        parser.set_defaults(func=self._run_agent)
    
    def _add_stats_command(self, subparsers):
        """Add stats command."""
        parser = subparsers.add_parser('stats', help='Show brain statistics')
        parser.add_argument('--json', action='store_true', help='Output in JSON format')
        
        parser.set_defaults(func=self._show_stats)
    
    def _add_save_command(self, subparsers):
        """Add save command."""
        parser = subparsers.add_parser('save', help='Save brain state to file')
        parser.add_argument('--file', type=str, required=True, help='Output file path')
        
        parser.set_defaults(func=self._save_brain)
    
    def _add_load_command(selfsubparsers):
        """Add load command."""
        parser = subparsers.add_parser('load', help='Load brain state from file')
        parser.add_argument('--file', type=str, required=True, help='Input file path')
        
        parser.set_defaults(func=self._load_brain)
    
    def _add_config_command(self, subparsers):
        """Add config command."""
        parser = subparsers.add_parser('config', help='Configuration management')
        
        subparsers_config = parser.add_subparsers(dest='config_action', help='Config action')
        
        # Show config command
        parser_show = subparsers_config.add_parser('show', help='Show current configuration')
        parser_show.add_argument('--json', action='store_true', help='Output in JSON format')
        parser_show.set_defaults(func=self._show_config)
        
        # Create config command
        parser_create = subparsers_config.add_parser('create', help='Create configuration')
        parser_create.add_argument('--file', type=str, help='Save configuration to file')
        parser_create.set_defaults(func=self._create_config)
        
        # Load config command
        parser_load = subparsers_config.add_parser('load', help='Load configuration')
        parser_load.add_argument('--file', type=str, required=True, help='Configuration file')
        parser_load.set_defaults(func=self._load_config)
    
    def _add_example_command(self, subparsers):
        """Add example command."""
        parser = subparsers.add_parser('example', help='Run example simulation')
        parser.add_argument('--type', choices=['simple', 'agent', 'advanced'], 
                           default='simple', help='Type of example to run')
        parser.add_argument('--steps', type=int, default=100, help='Number of steps')
        parser.add_argument('--output', type=str, help='Save results to file')
        
        parser.set_defaults(func=self._run_example)
    
    def _add_state_command(self, subparsers):
        """Add state command."""
        parser = subparsers.add_parser('state', help='State management')
        
        subparsers_state = parser.add_subparsers(dest='state_action', help='State action')
        
        # Reset command
        parser_reset = subparsers_state.add_parser('reset', help='Reset brain state')
        parser_reset.set_defaults(func=self._reset_state)
        
        # Initialize command
        parser_init = subparsers_state.add_parser('init', help='Initialize brain')
        parser_init.set_defaults(func=self._initialize_brain)
        
        # Status command
        parser_status = subparsers_state.add_parser('status', help='Show brain status')
        parser_status.add_argument('--json', action='store_true', help='Output in JSON format')
        parser_status.set_defaults(func=self._show_status)
    
    def _create_brain(self, args):
        """Create a brain with the given configuration."""
        print(f"Creating brain with {args.neurons} neurons...")
        
        # Create configuration
        self.config = (pynlm.ConfigBuilder()
                      .setNeuronCount(args.neurons)
                      .setRegionCount(args.regions)
                      .setConnectionProbability(args.connection_prob)
                      .setSTDPParameters(args.stdp_ltp, args.stdp_ltd, args.stdp_tau)
                      .build())
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"Brain created successfully!")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        
        if args.output:
            self.config.saveToFile(args.output)
            print(f"Configuration saved to: {args.output}")
        
        return True
    
    def _simulate(self, args):
        """Run simulation."""
        if not self.brain:
            print("Error: No brain created. Use 'create' command first.")
            return False
        
        print(f"Running simulation for {args.steps} steps...")
        
        if args.brain_file:
            print(f"Loading brain state from: {args.brain_file}")
            self.brain.load(args.brain_file)
        
        for step in range(args.steps):
            self.brain.step(step)
            
            if args.verbose and step % 50 == 0:
                stats = self.brain.getStatistics()
                print(f"Step {step}: {stats['firing_neurons']} firing neurons")
        
        print("Simulation completed!")
        
        if args.output:
            self.brain.save(args.output)
            print(f"Brain state saved to: {args.output}")
        
        return True
    
    def _run_agent(self, args):
        """Run agent simulation."""
        print(f"Creating agent simulation in {args.world_width}x{args.world_height} world...")
        
        # Create configuration
        config = (pynlm.ConfigBuilder()
                  .setNeuronCount(args.neurons)
                  .setRegionCount(1)
                  .setConnectionProbability(0.1)
                  .setSTDPParameters(0.01, 0.012, 20.0)
                  .build())
        
        # Create brain, world, and agent
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(args.world_width, args.world_height, 
                       args.vision_width, args.vision_height)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run episode
        for step in range(args.steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            if args.verbose and step % 50 == 0:
                print(f"Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        
        # Get results
        results = agent.getEpisodeResults()
        print(f"\nAgent episode completed!")
        print(f"Steps: {results['total_steps']}")
        print(f"Final curiosity: {results['final_curiosity']:.3f}")
        print(f"Final novelty: {results['final_novelty']:.3f}")
        print(f"Final dopamine: {results['final_dopamine']:.3f}")
        
        # Get world state
        world_state = agent.getWorldState()
        print(f"\nFinal agent state:")
        print(f"  Position: ({world_state['x']:.1f}, {world_state['y']:.1f})")
        print(f"  Orientation: {world_state['orientation']:.1f}°")
        print(f"  Energy: {world_state['energy']:.1f}")
        
        if args.output:
            # Save results to JSON
            output_data = {
                'steps': results['total_steps'],
                'final_curiosity': results['final_curiosity'],
                'final_novelty': results['final_novelty'],
                'final_dopamine': results['final_dopamine'],
                'world_state': world_state,
                'final_stats': brain.getStatistics()
            }
            
            with open(args.output, 'w') as f:
                json.dump(output_data, f, indent=2)
            
            print(f"Results saved to: {args.output}")
        
        return True
    
    def _show_stats(self, args):
        """Show brain statistics."""
        if not self.brain:
            print("Error: No brain created. Use 'create' command first.")
            return False
        
        stats = self.brain.getStatistics()
        
        if args.json:
            print(json.dumps(stats, indent=2))
        else:
            print("Brain Statistics:")
            print("-" * 40)
            print(f"Total neurons: {stats['neuron_count']}")
            print(f"Total synapses: {stats['synapse_count']}")
            print(f"Active neurons: {stats['active_neurons']}")
            print(f"Firing neurons: {stats['firing_neurons']}")
            print(f"Total spikes: {stats['total_spikes']}")
            print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
            print(f"E/I ratio: {stats['e_i_ratio']:.2f}")
        
        return True
    
    def _save_brain(self, args):
        """Save brain state to file."""
        if not self.brain:
            print("Error: No brain created. Use 'create' command first.")
            return False
        
        print(f"Saving brain state to: {args.file}")
        self.brain.save(args.file)
        print("Brain state saved successfully!")
        
        return True
    
    def _load_brain(self, args):
        """Load brain state from file."""
        if not self.brain:
            print("Error: No brain created. Use 'create' command first.")
            return False
        
        print(f"Loading brain state from: {args.file}")
        success = self.brain.load(args.file)
        
        if success:
            print("Brain state loaded successfully!")
        else:
            print("Error: Failed to load brain state.")
            return False
        
        return True
    
    def _show_config(self, args):
        """Show current configuration."""
        if not self.config:
            print("Error: No configuration created.")
            return False
        
        if args.json:
            # Get config as dictionary
            config_dict = {}
            for key in self.config.getKeys():
                config_dict[key] = self.config.get(key)
            print(json.dumps(config_dict, indent=2))
        else:
            print("Current Configuration:")
            print("-" * 40)
            for key in self.config.getKeys():
                value = self.config.get(key)
                print(f"{key}: {value}")
        
        return True
    
    def _create_config(self, args):
        """Create configuration and optionally save it."""
        self.config = pynlm.createDefaultConfig()
        
        print("Default configuration created.")
        
        if args.file:
            self.config.saveToFile(args.file)
            print(f"Configuration saved to: {args.file}")
        
        return True
    
    def _load_config(self, args):
        """Load configuration from file."""
        if not os.path.exists(args.file):
            print(f"Error: Configuration file not found: {args.file}")
            return False
        
        self.config = pynlm.createDefaultConfig()
        success = self.config.loadFromFile(args.file)
        
        if success:
            print(f"Configuration loaded from: {args.file}")
        else:
            print("Error: Failed to load configuration.")
            return False
        
        return True
    
    def _run_example(self, args):
        """Run an example simulation."""
        print(f"Running {args.type} example with {args.steps} steps...")
        
        if args.type == 'simple':
            # Simple brain simulation
            config = (pynlm.ConfigBuilder()
                      .setNeuronCount(500)
                      .setRegionCount(1)
                      .build())
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(args.steps):
                brain.step(step)
            
            stats = brain.getStatistics()
            print(f"Simple example completed: {stats['total_spikes']} spikes in {args.steps} steps")
            
        elif args.type == 'agent':
            # Agent simulation
            config = pynlm.ConfigBuilder().setNeuronCount(1000).build()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            for step in range(args.steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            results = agent.getEpisodeResults()
            print(f"Agent example completed: {results['total_steps']} steps")
            
        elif args.type == 'advanced':
            # Advanced example with all features
            config = (pynlm.ConfigBuilder()
                      .setNeuronCount(2000)
                      .setRegionCount(2)
                      .setConnectionProbability(0.1)
                      .setSTDPParameters(0.01, 0.012, 20.0)
                      .setStructuralPlasticity(0.0001, 0.00001)
                      .setMemoryCapacity(500, 200)
                      .enableNeuromodulation(True)
                      .enableDevelopment(True)
                      .setSimulation(0.001, 1000)
                      .build())
            
            sim = pynlm.SimulationManager(config)
            sim.runSteps(args.steps)
            
            stats = sim.getStatistics()
            print(f"Advanced example completed: {stats['total_spikes']} spikes in {args.steps} steps")
        
        if args.output:
            # Save example output
            output_data = {
                'example_type': args.type,
                'steps': args.steps,
                'timestamp': "TODO: Add timestamp",
                'config': "TODO: Add config"
            }
            
            with open(args.output, 'w') as f:
                json.dump(output_data, f, indent=2)
            
            print(f"Example output saved to: {args.output}")
        
        return True
    
    def _reset_state(self, args):
        """Reset brain state."""
        if not self.brain:
            print("Error: No brain created.")
            return False
        
        self.brain.reset()
        print("Brain state reset successfully!")
        return True
    
    def _initialize_brain(self, args):
        """Initialize brain."""
        if not self.brain:
            print("Error: No brain created.")
            return False
        
        self.brain.initialize()
        print("Brain initialized successfully!")
        return True
    
    def _show_status(self, args):
        """Show brain status."""
        if not self.brain:
            print("Status: No brain created")
            return False
        
        status = {
            'has_brain': True,
            'initialized': True,  # Assume initialized if brain exists
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'has_config': self.config is not None
        }
        
        if args.json:
            print(json.dumps(status, indent=2))
        else:
            print("Brain Status:")
            print("-" * 40)
            print(f"Has brain: {status['has_brain']}")
            print(f"Initialized: {status['initialized']}")
            print(f"Neuron count: {status['neuron_count']}")
            print(f"Synapse count: {status['synapse_count']}")
            print(f"Firing neurons: {status['firing_neurons']}")
            print(f"Has config: {status['has_config']}")
        
        return True
    
    def run(self, args):
        """Run the CLI with given arguments."""
        return args.func(args)
    
    def main(self):
        """Main entry point."""
        try:
            args = self.parser.parse_args()
            
            if not args.command:
                self.parser.print_help()
                return False
            
            return self.run(args)
            
        except KeyboardInterrupt:
            print("\nInterrupted by user.")
            return False
        except Exception as e:
            print(f"Error: {str(e)}")
            return False

if __name__ == "__main__":
    cli = NLMCLI()
    success = cli.main()
    sys.exit(0 if success else 1)