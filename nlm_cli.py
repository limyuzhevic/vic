#!/usr/bin/env python3
"""
pynlm CLI - Command Line Interface for NLM (Neural Learning Machine)
Advanced interface for expert users with direct access to C++ APIs
"""

import argparse
import json
import sys
import traceback
from typing import Dict, Any

try:
    import pynlm
    PYNLMI_AVAILABLE = True
except ImportError:
    PYNLMI_AVAILABLE = False
    print("Error: pynlm module not available. Please install it first.")

class NLMCLI:
    """Command line interface for NLM advanced operations"""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.last_error = None
    
    def create_brain(self, config: Dict[str, Any] = None):
        """Create a brain with specified configuration"""
        try:
            self.config = pynlm.createDefaultConfig()
            
            if config:
                for key, value in config.items():
                    self.config.set(key, value)
            
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            print(f"✓ Created brain with {self.brain.getTotalNeuronCount()} neurons")
            return True
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Failed to create brain: {e}")
            return False
    
    def load_config(self, filepath: str):
        """Load configuration from file"""
        try:
            if not self.config:
                self.config = pynlm.createDefaultConfig()
            
            success = self.config.loadFromFile(filepath)
            if success:
                print(f"✓ Loaded configuration from: {filepath}")
                return True
            else:
                print(f"✗ Failed to load configuration from: {filepath}")
                return False
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Error loading config: {e}")
            return False
    
    def run_simulation(self, steps: int, timestep: float = 0.001):
        """Run simulation for specified steps"""
        if not self.brain:
            print("✗ No brain created. Use --create-brain first.")
            return False
        
        try:
            print(f"Running simulation for {steps} steps...")
            
            for step in range(steps):
                self.brain.step(step, step * timestep)
                
                if step % 100 == 0:
                    print(f"  Step {step}: "
                          f"{self.brain.getFiringNeuronCount()} firing, "
                          f"{self.brain.getTotalSpikeCount()} spikes")
            
            print(f"✓ Simulation completed. Final stats:")
            print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f}")
            
            return True
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Simulation failed: {e}")
            return False
    
    def save_brain(self, filepath: str):
        """Save brain state to file"""
        if not self.brain:
            print("✗ No brain created to save.")
            return False
        
        try:
            success = self.brain.save(filepath)
            if success:
                print(f"✓ Saved brain state to: {filepath}")
                return True
            else:
                print(f"✗ Failed to save brain state")
                return False
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Error saving brain: {e}")
            return False
    
    def load_brain(self, filepath: str):
        """Load brain state from file"""
        try:
            if not self.config:
                self.config = pynlm.createDefaultConfig()
            
            self.brain = pynlm.createBrain(self.config)
            success = self.brain.load(filepath)
            
            if success:
                print(f"✓ Loaded brain state from: {filepath}")
                print(f"  Loaded brain has {self.brain.getTotalNeuronCount()} neurons")
                return True
            else:
                print(f"✗ Failed to load brain state")
                return False
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Error loading brain: {e}")
            return False
    
    def analyze_brain(self):
        """Analyze current brain state"""
        if not self.brain:
            print("✗ No brain created for analysis.")
            return False
        
        try:
            print("=== Brain Analysis ===")
            
            # Basic statistics
            print(f"Neurons: {self.brain.getTotalNeuronCount()}")
            print(f"Synapses: {self.brain.getTotalSynapseCount()}")
            print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
            print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
            print(f"Average firing rate: {self.brain.getAverageFiringRate():.3f}")
            print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
            
            # Developmental stage
            dev_stage = self.brain.getDevelopmentalStage()
            print(f"Developmental stage: {dev_stage}")
            
            # Brain status
            self.brain.logStatus()
            
            return True
        except Exception as e:
            self.last_error = str(e)
            print(f"✗ Brain analysis failed: {e}")
            return False
    
    def print_help(self):
        """Print help information"""
        help_text = """
NLM CLI - Neural Learning Machine Command Line Interface

USAGE:
    python nlm_cli.py [OPTIONS] [ARGS...]

BASIC COMMANDS:
    --create-brain          Create a new brain with default configuration
    --config FILE           Load configuration from JSON file
    --run-steps STEPS       Run simulation for specified number of steps
    --save-brain FILE       Save brain state to file
    --load-brain FILE       Load brain state from file
    --analyze               Analyze current brain state

ADVANCED OPTIONS:
    --neuron-count COUNT    Number of neurons (default: 1000)
    --synapse-density DENSITY Synapse density (0.0-1.0, default: 0.1)
    --random-seed SEED      Random seed for reproducibility (default: 42)
    --timestep TIME         Simulation timestep in seconds (default: 0.001)
    --help, -h              Show this help message
    --version               Show version information

EXAMPLES:
    # Create and run a simple simulation
    python nlm_cli.py --create-brain --neuron-count 500 --synapse-density 0.05 --run-steps 1000
    
    # Load existing brain and continue simulation
    python nlm_cli.py --config my_config.json --load-brain brain_state.bin --run-steps 500
    
    # Run analysis on current brain
    python nlm_cli.py --config my_config.json --create-brain --analyze
    
    # Save brain with custom configuration
    python nlm_cli.py --create-brain --neuron-count 2000 --random-seed 123 --save-brain experiment1.bin
"""
        print(help_text)

def main():
    if not PYNLMI_AVAILABLE:
        print("Error: pynlm module not available.")
        print("Please install it with: pip install -e .")
        sys.exit(1)
    
    parser = argparse.ArgumentParser(
        description="NLM CLI - Advanced command line interface for neural simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_cli.py --create-brain --neuron-count 1000 --run-steps 1000
  python nlm_cli.py --config config.json --create-brain --analyze
  python nlm_cli.py --create-brain --save-brain my_brain.bin
        """
    )
    
    # Basic commands
    parser.add_argument('--create-brain', action='store_true',
                       help='Create a new brain with default configuration')
    parser.add_argument('--config', type=str, help='Load configuration from JSON file')
    parser.add_argument('--run-steps', type=int, help='Run simulation for specified number of steps')
    parser.add_argument('--save-brain', type=str, help='Save brain state to file')
    parser.add_argument('--load-brain', type=str, help='Load brain state from file')
    parser.add_argument('--analyze', action='store_true', help='Analyze current brain state')
    
    # Advanced options
    parser.add_argument('--neuron-count', type=int, default=1000,
                       help='Number of neurons (default: 1000)')
    parser.add_argument('--synapse-density', type=float, default=0.1,
                       help='Synapse density (0.0-1.0, default: 0.1)')
    parser.add_argument('--random-seed', type=int, default=42,
                       help='Random seed for reproducibility (default: 42)')
    parser.add_argument('--timestep', type=float, default=0.001,
                       help='Simulation timestep in seconds (default: 0.001)')
    parser.add_argument('--help', '-h', action='store_true', help='Show help message')
    parser.add_argument('--version', action='store_true', help='Show version information')
    
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Build configuration
    config = {}
    if args.create_brain:
        config['brain.neuron_count'] = args.neuron_count
        config['brain.synapse_density'] = args.synapse_density
        config['random_seed'] = args.random_seed
    
    # Execute commands
    success = True
    
    if args.help:
        cli.print_help()
        return
    
    if args.version:
        print("NLM CLI v1.0.0")
        print("Python bindings for NLM (Neural Learning Machine)")
        return
    
    if args.config:
        success &= cli.load_config(args.config)
        if args.create_brain:
            success &= cli.create_brain(config)
    elif args.create_brain:
        success &= cli.create_brain(config)
    
    if args.run_steps:
        success &= cli.run_simulation(args.run_steps, args.timestep)
    
    if args.save_brain:
        success &= cli.save_brain(args.save_brain)
    
    if args.load_brain:
        success &= cli.load_brain(args.load_brain)
    
    if args.analyze:
        success &= cli.analyze_brain()
    
    if not success and cli.last_error:
        print(f"\nLast error: {cli.last_error}")
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()