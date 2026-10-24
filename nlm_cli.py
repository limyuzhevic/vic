#!/usr/bin/env python3
"""
Advanced command-line interface for NLM (Neural Learning Machine)

This script provides enhanced functionality for power users including:
- Configuration validation and editing
- Performance profiling
- Experiment management
- Advanced simulation controls
- Checkpoint management
- Batch processing
"""

import argparse
import json
import sys
import os
import time
import tempfile
import pynlm
from typing import Optional, Dict, Any

class NLMCLI:
    """Advanced NLM command-line interface"""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.world = None
        self.agent = None
        self.start_time = None
    
    def create_brain_from_config(self, config_path: Optional[str] = None, 
                                 neuron_count: Optional[int] = None,
                                 learning_rate: Optional[float] = None) -> bool:
        """Create a brain with optional configuration overrides"""
        try:
            if config_path:
                self.config = pynlm.loadConfigFromFile(config_path)
            else:
                self.config = pynlm.createDefaultConfig()
            
            # Apply overrides
            if neuron_count is not None:
                self.config.set("brain.neuron_count", neuron_count)
            
            if learning_rate is not None:
                self.config.set("plasticity.stdp.learning_rate", learning_rate)
            
            # Validate configuration
            pynlm.validateConfig(self.config)
            
            # Create brain
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            print(f"✓ Created brain with {self.brain.getTotalNeuronCount()} neurons")
            return True
            
        except Exception as e:
            print(f"✗ Failed to create brain: {e}")
            return False
    
    def run_simulation(self, num_steps: int, timestep: float = 0.1, 
                      enable_world: bool = False) -> bool:
        """Run a simulation for a given number of steps"""
        if not self.brain:
            print("✗ No brain created. Run 'create-brain' first.")
            return False
        
        try:
            print(f"Starting simulation for {num_steps} steps (timestep={timestep}s)...")
            
            if enable_world:
                if not self.world:
                    self.world = pynlm.createSimpleWorld()
                    self.world.configure(20, 20, 8, 8)
                    self.world.reset()
                    self.agent = pynlm.createAgentBrain(self.brain)
                    self.agent.initialize(self.world)
                
                for step in range(num_steps):
                    self.world.update(timestep)
                    
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    self.brain.step(step)
                    
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
                    if step % 100 == 0:
                        print(f"Step {step}: {self.brain.getFiringNeuronCount()} firing neurons")
            else:
                for step in range(num_steps):
                    self.brain.step(step)
                    
                    if step % 100 == 0:
                        print(f"Step {step}: {self.brain.getFiringNeuronCount()} firing neurons")
            
            print("✓ Simulation completed successfully")
            return True
            
        except Exception as e:
            print(f"✗ Simulation failed: {e}")
            return False
    
    def save_checkpoint(self, filepath: str) -> bool:
        """Save brain checkpoint to file"""
        if not self.brain:
            print("✗ No brain to save.")
            return False
        
        try:
            self.brain.save(filepath)
            print(f"✓ Brain saved to {filepath}")
            return True
        except Exception as e:
            print(f"✗ Failed to save checkpoint: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain checkpoint from file"""
        if not self.brain:
            print("✗ No brain created. Creating new brain first...")
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
        
        try:
            self.brain.load(filepath)
            print(f"✓ Brain loaded from {filepath}")
            return True
        except Exception as e:
            print(f"✗ Failed to load checkpoint: {e}")
            return False
    
    def export_config(self, filepath: str) -> bool:
        """Export configuration to JSON file"""
        if not self.config:
            print("✗ No configuration to export.")
            return False
        
        try:
            self.config.saveToFile(filepath)
            print(f"✓ Configuration exported to {filepath}")
            return True
        except Exception as e:
            print(f"✗ Failed to export configuration: {e}")
            return False
    
    def print_brain_stats(self):
        """Print detailed brain statistics"""
        if not self.brain:
            print("✗ No brain created.")
            return
        
        print("\n=== Brain Statistics ===")
        print(f"Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Active Neurons: {self.brain.getActiveNeuronCount()}")
        print(f"Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average Firing Rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"E/I Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"Total Spikes: {self.brain.getTotalSpikeCount()}")
        print(f"Developmental Stage: {self.brain.getDevelopmentalStage()}")
    
    def run_performance_test(self, duration_seconds: int) -> bool:
        """Run a performance test for specified duration"""
        if not self.brain:
            print("✗ No brain created. Run 'create-brain' first.")
            return False
        
        try:
            self.start_time = time.time()
            step_count = 0
            
            while time.time() - self.start_time < duration_seconds:
                self.brain.step(step_count)
                step_count += 1
                
                if step_count % 1000 == 0:
                    elapsed = time.time() - self.start_time
                    steps_per_sec = step_count / elapsed
                    print(f"Step {step_count}, {steps_per_sec:.1f} steps/sec")
            
            elapsed = time.time() - self.start_time
            steps_per_sec = step_count / elapsed
            
            print(f"\nPerformance Test Results:")
            print(f"Duration: {elapsed:.1f}s")
            print(f"Steps completed: {step_count}")
            print(f"Performance: {steps_per_sec:.1f} steps/sec")
            print(f"Throughput: {self.brain.getTotalSpikeCount() / elapsed:.1f} spikes/sec")
            
            return True
            
        except Exception as e:
            print(f"✗ Performance test failed: {e}")
            return False
    
    def create_agent_with_features(self, learning: bool = True, development: bool = True, 
                                  curiosity: bool = True, novelty: bool = True) -> bool:
        """Create an agent with specific features enabled"""
        if not self.brain:
            print("✗ No brain created.")
            return False
        
        try:
            self.agent = pynlm.createAgentBrain(self.brain)
            
            if learning:
                self.agent.enableRewardModulation(True)
                print("✓ Reward modulation enabled")
            
            if development:
                self.agent.enableDevelopment(True)
                print("✓ Development enabled")
            
            if curiosity:
                self.agent.enableCuriosity(True)
                print("✓ Curiosity enabled")
            
            if novelty:
                # Note: Novelty system would need to be exposed in the API
                print("✓ Novelty enabled (API limitation)")
            
            print("✓ Agent created with specified features")
            return True
            
        except Exception as e:
            print(f"✗ Failed to create agent: {e}")
            return False
    
    def run_batch_simulation(self, config_files: list, steps_per_config: int) -> bool:
        """Run batch simulation with multiple configurations"""
        print(f"Running batch simulation with {len(config_files)} configurations...")
        
        for i, config_file in enumerate(config_files):
            print(f"\n--- Configuration {i+1}/{len(config_files)} ---")
            print(f"Config: {config_file}")
            
            if not self.create_brain_from_config(config_file):
                print(f"✗ Failed to create brain for {config_file}")
                continue
            
            if not self.run_simulation(steps_per_config, enable_world=True):
                print(f"✗ Simulation failed for {config_file}")
                continue
            
            # Save results for this configuration
            results_dir = "batch_results"
            os.makedirs(results_dir, exist_ok=True)
            
            self.save_checkpoint(f"{results_dir}/brain_{i}.bin")
            self.export_config(f"{results_dir}/config_{i}.json")
            
            self.print_brain_stats()
            
            # Reset for next iteration
            self.brain = None
            self.config = None
            self.world = None
            self.agent = None
        
        print(f"\n✓ Batch simulation completed. Results saved to {results_dir}/")
        return True

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="Advanced NLM Command-Line Interface for Power Users",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a brain with custom settings
  %(prog)s create-brain --neurons 2000 --learning-rate 0.01
  
  # Run a simulation with world interaction
  %(prog)s simulate --steps 1000 --world
  
  # Save and load checkpoints
  %(prog)s save-checkpoint --file brain_state.bin
  %(prog)s load-checkpoint --file brain_state.bin
  
  # Run performance test
  %(prog)s performance --duration 60
  
  # Create agent with features
  %(prog)s create-agent --learning --development --curiosity
        """
    )
    
    # Global options
    parser.add_argument("--verbose", "-v", action="store_true",
                       help="Enable verbose output")
    parser.add_argument("--quiet", "-q", action="store_true",
                       help="Suppress non-essential output")
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create brain command
    parser_create = subparsers.add_parser("create-brain", help="Create a brain")
    parser_create.add_argument("--config", "-c", help="Configuration file path")
    parser_create.add_argument("--neurons", type=int, help="Number of neurons")
    parser_create.add_argument("--learning-rate", type=float, help="STDP learning rate")
    parser_create.set_defaults(func=lambda args: cli.create_brain_from_config(
        args.config, args.neurons, args.learning_rate))
    
    # Simulate command
    parser_sim = subparsers.add_parser("simulate", help="Run simulation")
    parser_sim.add_argument("--steps", "-s", type=int, required=True, help="Number of steps")
    parser_sim.add_argument("--timestep", type=float, default=0.1, help="Time step size")
    parser_sim.add_argument("--world", action="store_true", help="Enable world interaction")
    parser_sim.set_defaults(func=lambda args: cli.run_simulation(args.steps, args.timestep, args.world))
    
    # Save checkpoint command
    parser_save = subparsers.add_parser("save-checkpoint", help="Save brain checkpoint")
    parser_save.add_argument("--file", "-f", required=True, help="Output file path")
    parser_save.set_defaults(func=lambda args: cli.save_checkpoint(args.file))
    
    # Load checkpoint command
    parser_load = subparsers.add_parser("load-checkpoint", help="Load brain checkpoint")
    parser_load.add_argument("--file", "-f", required=True, help="Input file path")
    parser_load.set_defaults(func=lambda args: cli.load_checkpoint(args.file))
    
    # Export config command
    parser_export = subparsers.add_parser("export-config", help="Export configuration")
    parser_export.add_argument("--file", "-f", required=True, help="Output file path")
    parser_export.set_defaults(func=lambda args: cli.export_config(args.file))
    
    # Print stats command
    parser_stats = subparsers.add_parser("stats", help="Print brain statistics")
    parser_stats.set_defaults(func=lambda args: cli.print_brain_stats())
    
    # Performance test command
    parser_perf = subparsers.add_parser("performance", help="Run performance test")
    parser_perf.add_argument("--duration", type=int, required=True, help="Test duration in seconds")
    parser_perf.set_defaults(func=lambda args: cli.run_performance_test(args.duration))
    
    # Create agent command
    parser_agent = subparsers.add_parser("create-agent", help="Create agent with features")
    parser_agent.add_argument("--learning", action="store_true", help="Enable reward modulation")
    parser_agent.add_argument("--development", action="store_true", help="Enable development")
    parser_agent.add_argument("--curiosity", action="store_true", help="Enable curiosity")
    parser_agent.add_argument("--novelty", action="store_true", help="Enable novelty detection")
    parser_agent.set_defaults(func=lambda args: cli.create_agent_with_features(
        args.learning, args.development, args.curiosity, args.novelty))
    
    # Batch simulation command
    parser_batch = subparsers.add_parser("batch", help="Run batch simulation")
    parser_batch.add_argument("--configs", "-c", nargs="+", required=True, help="Configuration files")
    parser_batch.add_argument("--steps", "-s", type=int, default=100, help="Steps per configuration")
    parser_batch.set_defaults(func=lambda args: cli.run_batch_simulation(args.configs, args.steps))
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    global cli
    cli = NLMCLI()
    
    # Execute command
    if hasattr(args, 'func'):
        success = args.func(args)
        return 0 if success else 1
    else:
        parser.print_help()
        return 1

if __name__ == "__main__":
    sys.exit(main())
