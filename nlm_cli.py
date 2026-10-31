#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

Advanced CLI for the NLM (Neural Learning Machine) brain simulation project.
Provides unified interface to all phases and functionality.
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional

# Add src to path for imports
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from core.Config.Config import Config
from core.Logger.Logger import Logger
from brain.Brain import Brain

class NLMCLI:
    def __init__(self):
        self.logger = Logger()
        self.setup_logging()
    
    def setup_logging(self):
        """Setup logging configuration"""
        console_logger = self.logger.addLogger("console")
        console_logger.setLevel("INFO")
    
    def create_parser(self):
        """Create and return the argument parser"""
        parser = argparse.ArgumentParser(
            description="NLM - Neural Learning Machine: An Experimental Artificial Developmental Brain",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  # Run Phase 2 demo (basic neural computation)
  nlm --phase 2 --neuron-count 1000 --steps 1000

  # Run Phase 6 integration demo (full brain integration)
  nlm --phase 6 --neuron-count 500 --max-steps 2000

  # Run performance benchmark
  nlm --benchmark --neuron-counts "100,500,1000,2000"

  # Custom configuration
  nlm --config my_config.json --phase 4

  # Detailed logging
  nlm --phase 2 --log-level DEBUG --verbose
            """
        )
        
        # Main operation mode
        parser.add_argument(
            '--phase', '-p',
            type=str,
            choices=['1', '2', '3', '4', '5', '6'],
            default='2',
            help='Phase to run (default: 2)'
        )
        
        # Configuration options
        parser.add_argument(
            '--config', '-c',
            type=str,
            help='Configuration file path (JSON or YAML format)'
        )
        
        parser.add_argument(
            '--set', '-s',
            action='append',
            help='Set configuration value (format: key=value)'
        )
        
        # Network parameters
        parser.add_argument(
            '--neuron-count', '-n',
            type=int,
            default=500,
            help='Number of neurons (default: 500)'
        )
        
        parser.add_argument(
            '--region-count',
            type=int,
            default=1,
            help='Number of brain regions (default: 1)'
        )
        
        parser.add_argument(
            '--connection-prob',
            type=float,
            default=0.1,
            help='Connection probability for random connectivity (default: 0.1)'
        )
        
        # Simulation parameters
        parser.add_argument(
            '--steps', '-st',
            type=int,
            help='Number of simulation steps'
        )
        
        parser.add_argument(
            '--timestep',
            type=float,
            default=0.001,
            help='Simulation timestep in seconds (default: 0.001)'
        )
        
        # Performance and benchmarking
        parser.add_argument(
            '--benchmark',
            action='store_true',
            help='Run performance benchmark instead of simulation'
        )
        
        parser.add_argument(
            '--neuron-counts',
            type=str,
            help='Comma-separated list of neuron counts for benchmarking'
        )
        
        # Output and logging
        parser.add_argument(
            '--verbose', '-v',
            action='store_true',
            help='Enable verbose output'
        )
        
        parser.add_argument(
            '--quiet', '-q',
            action='store_true',
            help='Suppress non-essential output'
        )
        
        parser.add_argument(
            '--log-level',
            type=str,
            choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL'],
            default='INFO',
            help='Log level (default: INFO)'
        )
        
        parser.add_argument(
            '--output-file',
            type=str,
            help='Output file for results and logs'
        )
        
        # Advanced options
        parser.add_argument(
            '--enable-plasticity',
            action='store_true',
            default=True,
            help='Enable neural plasticity (default: enabled)'
        )
        
        parser.add_argument(
            '--stdp-enabled',
            action='store_true',
            default=True,
            help='Enable Spike-Timing Dependent Plasticity (STDP) (default: enabled)'
        )
        
        parser.add_argument(
            '--learning-rate',
            type=float,
            default=0.01,
            help='Learning rate for plasticity (default: 0.01)'
        )
        
        parser.add_argument(
            '--checkpoint-enabled',
            action='store_true',
            help='Enable checkpoint saving'
        )
        
        parser.add_argument(
            '--checkpoint-path',
            type=str,
            help='Path for checkpoint files'
        )
        
        # Help and version
        parser.add_argument(
            '--version',
            action='version',
            version='NLM %(prog)s v0.1.0 (Phase 6 Final Integration)',
            help='Show version information'
        )
        
        return parser
    
    def parse_cli_args(self):
        """Parse command line arguments"""
        parser = self.create_parser()
        return parser.parse_args()
    
    def load_configuration(self, args):
        """Load configuration from file, CLI args, and defaults"""
        config = Config()
        
        # Load from config file if specified
        if args.config:
            if not config.loadFromFile(args.config):
                print(f"Warning: Could not load config file: {args.config}")
                print("Using defaults and command-line arguments only.")
        
        # Set defaults based on phase
        phase = args.phase
        if phase == '2':
            config.set("random_seed", 42)
            config.set("simulation_timestep", 0.001)
            config.set("neuron_count", args.neuron_count)
            config.set("region_count", args.region_count)
            config.set("connection_probability", args.connection_prob)
            
            # Phase 2 specific settings
            config.set("stdp_ltp_weight", 0.02f)
            config.set("stdp_ltd_weight", 0.015f)
            config.set("stdp_tau", 20.0f)
            config.set("synaptogenesis_rate", 0.0001f)
            config.set("pruning_rate", 0.00001f)
        
        elif phase == '6':
            config.set("random_seed", 42)
            config.set("simulation_timestep", 0.001)
            config.set("neuron_count", args.neuron_count)
            config.set("region_count", 1)
            config.set("connection_probability", 0.1f)
            config.set("stdp_ltp_weight", 0.01f)
            config.set("stdp_ltd_weight", 0.012f)
            config.set("synaptogenesis_rate", 0.0001f)
            config.set("pruning_rate", 0.00001f)
            
            # Phase 6 integration settings
            config.set("enable_checkpointing", True)
            config.set("checkpoint_dir", args.checkpoint_path if args.checkpoint_path else "./checkpoints")
            config.set("replay_interval", 100)
            config.set("consolidation_interval", 1000)
        
        # Override with command line arguments
        if args.steps:
            config.set("max_steps", args.steps)
        
        config.set("simulation_timestep", args.timestep)
        
        # Apply additional settings from --set arguments
        if args.set:
            for setting in args.set:
                if '=' in setting:
                    key, value = setting.split('=', 1)
                    # Try to convert value to appropriate type
                    config.set(key.strip(), value.strip())
        
        return config
    
    def run_phase2_demo(self, config):
        """Run Phase 2 demo (basic neural computation)"""
        print("\n" + "="*60)
        print("NLM Phase 2: Real Neural Computation")
        print("="*60 + "\n")
        
        # Initialize brain
        brain = Brain(config)
        if not brain.initialize():
            print("ERROR: Failed to initialize brain!")
            return False
        
        brain.logStatus()
        
        # Run basic connectivity test
        self.run_basic_connectivity_test(brain)
        
        # Run plasticity experiment
        self.run_plasticity_experiment(brain)
        
        # Run STDP verification
        self.run_stdp_verification(brain)
        
        # Final status
        print("\n" + "="*60)
        print("Phase 2 Complete")
        print("="*60)
        brain.logStatus()
        
        return True
    
    def run_basic_connectivity_test(self, brain):
        """Test basic neural connectivity"""
        print("\n--- Test 1: Basic Neural Connectivity ---")
        
        region = brain.getRegion(1)
        if not region:
            print("  No neurons found!")
            return
        
        neurons = region.getAllNeurons()
        if neurons.empty():
            print("  No neurons found!")
            return
        
        # Get initial spike count
        size_t initialSpikes = brain.getTotalSpikeCount();
        
        # Inject current into first 10 neurons
        print("  Injecting current into 10 neurons...")
        for i in range(min(10, len(neurons))):
            neurons[i]->injectCurrent(50.0f)
        
        # Run a few steps
        for step in range(50):
            brain.step(step, step * 0.001)
        
        size_t spikes = brain.getTotalSpikeCount() - initialSpikes
        print(f"  Spikes generated: {spikes}")
        
        if spikes > 0:
            print("  ✓ Spikes propagate through network")
        else:
            print("  ! No spikes - checking neuron parameters...")
            for i in range(min(3, len(neurons))):
                print(f"    Neuron {i} V={neurons[i]->getMembranePotential()} "
                      f"thresh={neurons[i]->getThreshold()}")
    
    def run_plasticity_experiment(self, brain):
        """Run plasticity learning experiment"""
        print("\n--- Test 2: Plasticity Learning Experiment ---")
        
        # Enable plasticity on synapses
        region = brain.getRegion(1)
        if region:
            for syn in region.getSynapses():
                syn->enablePlasticity(true, true, false)
        
        print("Applying repeated input patterns (1000 steps)...")
        
        for step in range(1000):
            # Inject random input
            for i in range(20):
                brain.injectCurrentToNeurons("Sensory", 30.0f)
            
            brain.step(step, step * 0.001)
            
            if step % 100 == 0:
                print(f"  Step {step} | Spikes: {brain.getTotalSpikeCount()} "
                      f"| Firing: {brain.getFiringNeuronCount()}")
        
        print("  ✓ Plasticity experiment completed")
    
    def run_stdp_verification(self, brain):
        """Run STDP verification"""
        print("\n--- Test 3: STDP Verification ---")
        
        region = brain.getRegion(1)
        if not region or region.getSynapses().size() < 5:
            print("  Not enough synapses for STDP test")
            return
        
        auto& synapses = region.getSynapses();
        print("  Testing STDP on 5 synapses:")
        
        # Record initial weights
        vector<float> beforeWeights;
        for i in range(5):
            beforeWeights.push_back(synapses[i]->getWeight())
            synapses[i]->enablePlasticity(false, true, false)
            print(f"    Synapse {i} before: {beforeWeights[i]}")
        
        print("  Creating correlated pre->post activity (potentiation)...")
        
        for trial in range(50):
            auto neurons = region.getAllNeurons();
            if neurons.size() >= 2:
                # Pre fires first
                neurons[0]->injectCurrent(60.0f)
                brain.step(trial * 2, trial * 2 * 0.001)
                
                # Then post fires
                neurons[1]->injectCurrent(60.0f)
                brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
        
        print("  After correlated activity:")
        float totalDelta = 0.0f;
        for i in range(5):
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            print(f"    Synapse {i} after: {synapses[i]->getWeight()} "
                  f"(Δ={delta})")
        
        if totalDelta > 0.001f:
            print("  ✓ STDP WORKING: Pre-before-post produced potentiation")
        else:
            print("  ! No significant change detected")
    
    def run_benchmark(self, config, neuron_counts_str):
        """Run performance benchmark"""
        print("\n" + "="*60)
        print("NLM Performance Benchmark")
        print("="*60 + "\n")
        
        # Parse neuron counts
        if neuron_counts_str:
            try:
                neuron_counts = [int(n.strip()) for n in neuron_counts_str.split(',')]
            except ValueError:
                print(f"Error: Invalid neuron-counts format: {neuron_counts_str}")
                print("Using default benchmark configuration")
                neuron_counts = [100, 500, 1000, 2000]
        else:
            neuron_counts = [100, 500, 1000, 2000]
        
        print(f"Benchmarking with neuron counts: {neuron_counts}")
        
        # TODO: Implement actual benchmarking
        print("Benchmarking functionality to be implemented...")
        
        return True
    
    def run(self):
        """Main CLI entry point"""
        args = self.parse_cli_args()
        
        # Setup logging level
        self.logger.setLevel(args.log_level)
        
        # Load configuration
        config = self.load_configuration(args)
        
        # Log configuration summary
        print(config.summary())
        
        # Run the requested phase or benchmark
        if args.benchmark:
            success = self.run_benchmark(config, args.neuron_counts_str if hasattr(args, 'neuron_counts_str') else None)
        elif args.phase == '2':
            success = self.run_phase2_demo(config)
        elif args.phase == '6':
            success = self.run_phase6_demo(config)
        else:
            print(f"Phase {args.phase} not yet implemented")
            success = False
        
        return success

def main():
    """Entry point for the CLI"""
    cli = NLMCLI()
    
    try:
        success = cli.run()
        return 0 if success else 1
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    main()
