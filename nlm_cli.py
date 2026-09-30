#!/usr/bin/env python3
"""
NLM (熙然) Command Line Interface

A user-friendly command-line interface for the NLM neural simulation framework.
Provides high-level commands for common operations and easy-to-use features.

Phase 6: Final Integration - User Interface Enhancement

Features:
- Simple brain initialization and configuration
- Easy simulation control
- State management (save/load)
- Performance monitoring
- Experiment execution
- Visualization output

Usage: nlm [command] [options]
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not available. Please build the Python module first.")
    print("Run: python setup.py build_ext --inplace or equivalent")

class NLMCommandLine:
    """Main NLM CLI interface"""
    
    def __init__(self):
        self.brain = None
        self.config = None
        
    def create_brain(self, config_file: Optional[str] = None, neuron_count: int = 1000, 
                     region_count: int = 1, connection_prob: float = 0.1) -> bool:
        """Create and initialize a new brain"""
        try:
            if not NLM_AVAILABLE:
                return False
                
            self.config = nlm.createDefaultConfig()
            
            # Set configuration
            self.config.set("neuron_count", neuron_count)
            self.config.set("region_count", region_count)
            self.config.set("connection_probability", connection_prob)
            self.config.set("random_seed", 42)
            self.config.set("simulation_timestep", 0.001)
            
            # Load from file if specified
            if config_file and Path(config_file).exists():
                if not self.config.loadFromFile(config_file):
                    print(f"Warning: Could not load config from {config_file}")
            
            # Create brain
            self.brain = nlm.createBrain(self.config)
            if self.brain.initialize():
                print(f"✓ Brain initialized successfully")
                print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
                print(f"  Regions: {self.brain.getRegionCount()}")
                print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
                return True
            else:
                print("✗ Failed to initialize brain")
                return False
                
        except Exception as e:
            print(f"✗ Error creating brain: {e}")
            return False
    
    def run_simulation(self, steps: int, agent: bool = False) -> bool:
        """Run a simulation for specified number of steps"""
        if not self.brain:
            print("✗ No brain created. Use 'create' command first.")
            return False
            
        try:
            print(f"Running simulation for {steps} steps...")
            
            if agent:
                # Create agent and world for integrated simulation
                world = nlm.createSimpleWorld()
                world.configure(16, 16, 16, 16)  # width, height, vision_width, vision_height
                agent_brain = nlm.createAgentBrain(self.brain)
                
                for step in range(steps):
                    # Get sensory percept
                    percept = world.getSensoryPercept()
                    
                    # Process with agent
                    agent_brain.processSensoryInput(percept)
                    
                    # Brain step
                    self.brain.step(step, step * 0.001)
                    
                    # Get action
                    action = agent_brain.decodeMotorCommand()
                    
                    # Apply to world
                    world.applyMotorCommand(action, step * 0.001)
                    
                    # Log progress
                    if step % 100 == 0:
                        reward = world.getAgentBody().energy
                        firing_rate = self.brain.getAverageFiringRate()
                        print(f"  Step {step}: Reward={reward:.2f}, Firing Rate={firing_rate:.2f}")
                        
            else:
                # Simple brain simulation
                for step in range(steps):
                    # Inject some input to stimulate activity
                    if step % 50 == 0:
                        self.brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 10.0f)
                    
                    self.brain.step(step, step * 0.001)
                    
                    if step % 100 == 0:
                        firing_rate = self.brain.getAverageFiringRate()
                        spike_count = self.brain.getTotalSpikeCount()
                        print(f"  Step {step}: Firing Rate={firing_rate:.2f}, Spikes={spike_count}")
            
            print(f"✓ Simulation completed successfully")
            return True
            
        except Exception as e:
            print(f"✗ Error during simulation: {e}")
            return False
    
    def save_state(self, filepath: str) -> bool:
        """Save brain state to file"""
        if not self.brain:
            print("✗ No brain created. Use 'create' command first.")
            return False
            
        try:
            if self.brain.save(filepath):
                print(f"✓ Brain state saved to {filepath}")
                return True
            else:
                print(f"✗ Failed to save brain state to {filepath}")
                return False
        except Exception as e:
            print(f"✗ Error saving state: {e}")
            return False
    
    def load_state(self, filepath: str) -> bool:
        """Load brain state from file"""
        if not self.brain:
            print("✗ No brain created. Use 'create' command first.")
            return False
            
        try:
            # Create new brain for loading
            new_brain = nlm.createBrain(self.config)
            if new_brain.initialize() and new_brain.load(filepath):
                self.brain = new_brain
                print(f"✓ Brain state loaded from {filepath}")
                return True
            else:
                print(f"✗ Failed to load brain state from {filepath}")
                return False
        except Exception as e:
            print(f"✗ Error loading state: {e}")
            return False
    
    def run_experiment(self, experiment_type: str, steps: int = 1000) -> bool:
        """Run predefined experiment"""
        if not self.brain:
            print("✗ No brain created. Use 'create' command first.")
            return False
            
        try:
            print(f"Running {experiment_type} experiment for {steps} steps...")
            
            if experiment_type == "plasticity":
                self._run_plasticity_experiment(steps)
            elif experiment_type == "stdp":
                self._run_stdp_experiment(steps)
            elif experiment_type == "integration":
                self._run_integration_experiment(steps)
            else:
                print(f"✗ Unknown experiment type: {experiment_type}")
                return False
                
            print(f"✓ {experiment_type} experiment completed")
            return True
            
        except Exception as e:
            print(f"✗ Error during experiment: {e}")
            return False
    
    def _run_plasticity_experiment(self, steps: int):
        """Run plasticity learning experiment"""
        # Enable plasticity on synapses
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                syn.enablePlasticity(True, True, False)
        
        # Apply repeated input pattern
        for step in range(steps):
            for i in range(20):
                self.brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0f)
            self.brain.step(step, step * 0.001)
            
            if step % 200 == 0:
                firing = self.brain.getAverageFiringRate()
                spikes = self.brain.getTotalSpikeCount()
                print(f"  Step {step}: Firing={firing:.2f}, Spikes={spikes}")
    
    def _run_stdp_experiment(self, steps: int):
        """Run STDP verification experiment"""
        # Enable only STDP plasticity
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                syn.enablePlasticity(False, True, False)
        
        # Create correlated pre-post activity
        region = self.brain.getRegion(nlm.RegionId(1))
        if region:
            neurons = region.getAllNeurons()
            if len(neurons) >= 2:
                pre_neuron = neurons[0]
                post_neuron = neurons[1]
                
                for trial in range(50):
                    pre_neuron.injectCurrent(60.0f)
                    self.brain.step(trial * 2, trial * 2 * 0.001)
                    
                    post_neuron.injectCurrent(60.0f)
                    self.brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
    
    def _run_integration_experiment(self, steps: int):
        """Run Phase 6 integration experiment"""
        # Create world and agent
        world = nlm.createSimpleWorld()
        world.configure(16, 16, 16, 16)
        agent_brain = nlm.createAgentBrain(self.brain)
        
        total_reward = 0.0
        
        for step in range(steps):
            # Observe world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step, step * 0.001)
            
            # Get action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step * 0.001)
            
            # Get reward
            reward = world.getAgentBody().energy
            total_reward += reward
            
            agent_brain.applyRewardModulation(reward, 0.0f)
            
            if step % 200 == 0:
                print(f"  Step {step}: Reward={reward:.2f}, Total={total_reward/(step+1):.2f}")
    
    def show_status(self):
        """Display brain status"""
        if not self.brain:
            print("✗ No brain created.")
            return
            
        self.brain.logStatus()
        
    def help(self):
        """Show help information"""
        help_text = """
NLM (熙然) Neural Learning Machine - Command Line Interface

Commands:

  create [options]           Create and initialize a new brain
    --neuron-count N          Number of neurons (default: 1000)
    --region-count R          Number of regions (default: 1)
    --connection-prob P       Connection probability (default: 0.1)
    --config-file FILE         Load configuration from file

  run [steps] [--agent]      Run simulation for specified steps
    steps                     Number of simulation steps
    --agent                   Use integrated agent/world simulation

  save FILE                  Save brain state to file

  load FILE                  Load brain state from file

  experiment TYPE [steps]    Run predefined experiment
    TYPE: plasticity, stdp, integration
    steps                     Number of steps (default: 1000)

  status                     Show current brain status

  help                       Show this help message

  exit/quit                 Exit the program

Examples:

  nlm create --neuron-count 500 --region-count 2
      Create a brain with 500 neurons in 2 regions

  nlm run 1000
      Run a simple brain simulation for 1000 steps

  nlm run 500 --agent
      Run integrated agent-world simulation for 500 steps

  nlm experiment plasticity 2000
      Run plasticity learning experiment for 2000 steps

  nlm save brain_state.bin
      Save current brain state to file

  nlm load brain_state.bin
      Load brain state from file

For more information about NLM, visit: https://github.com/nlm-project/nlm
"""
        print(help_text)

def main():
    cli = NLMCommandLine()
    
    # If no arguments provided, show help
    if len(sys.argv) < 2:
        cli.help()
        return
    
    command = sys.argv[1].lower()
    
    if command == "help":
        cli.help()
        return
        
    elif command == "exit" or command == "quit":
        print("Goodbye!")
        return
        
    elif command == "create":
        # Parse create command arguments
        parser = argparse.ArgumentParser(description="Create and initialize NLM brain")
        parser.add_argument("--neuron-count", type=int, default=1000, help="Number of neurons")
        parser.add_argument("--region-count", type=int, default=1, help="Number of regions")
        parser.add_argument("--connection-prob", type=float, default=0.1, help="Connection probability")
        parser.add_argument("--config-file", help="Load configuration from file")
        
        try:
            args = parser.parse_args(sys.argv[2:])
            cli.create_brain(args.config_file, args.neuron_count, args.region_count, args.connection_prob)
        except SystemExit:
            print("\nUsage: nlm create [options]")
            print("Use --help for more information")
            
    elif command == "run":
        # Parse run command arguments
        parser = argparse.ArgumentParser(description="Run simulation")
        parser.add_argument("steps", type=int, help="Number of simulation steps")
        parser.add_argument("--agent", action="store_true", help="Use integrated agent/world simulation")
        
        try:
            args = parser.parse_args(sys.argv[2:])
            cli.run_simulation(args.steps, args.agent)
        except SystemExit:
            print("\nUsage: nlm run steps [--agent]")
            print("Use --help for more information")
            
    elif command == "save":
        if len(sys.argv) < 3:
            print("✗ Missing filename for save command")
            print("Usage: nlm save FILE")
        else:
            cli.save_state(sys.argv[2])
            
    elif command == "load":
        if len(sys.argv) < 3:
            print("✗ Missing filename for load command")
            print("Usage: nlm load FILE")
        else:
            cli.load_state(sys.argv[2])
            
    elif command == "experiment":
        # Parse experiment command arguments
        parser = argparse.ArgumentParser(description="Run experiment")
        parser.add_argument("type", help="Experiment type (plasticity, stdp, integration)")
        parser.add_argument("steps", nargs="?", type=int, default=1000, help="Number of steps")
        
        try:
            args = parser.parse_args(sys.argv[2:])
            cli.run_experiment(args.type, args.steps)
        except SystemExit:
            print("\nUsage: nlm experiment TYPE [steps]")
            print("Use --help for more information")
            
    elif command == "status":
        cli.show_status()
        
    else:
        print(f"✗ Unknown command: {command}")
        print("Use 'help' to see available commands")

if __name__ == "__main__":
    main()
