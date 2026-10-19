#!/usr/bin/env python3
"""
NLM Python CLI Script
=======================

A command-line interface for running NLM (Neural Learning Machine) simulations
and experiments. This script provides a Python wrapper around the C++ NLM library
with enhanced functionality for easier use and better user experience.

Features:
- Run demos (Phase 3, 4, 6 integration tests)
- Run simulations with various configurations
- Performance benchmarking
- Configuration management
- Result analysis and visualization
"""

import sys
import os
import json
import argparse
import subprocess
import time
from typing import Dict, Any, Optional

# Add the current directory to Python path to import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class NLMCLI:
    """Command-line interface for NLM operations."""
    
    def __init__(self):
        self.version = "0.1.0"
        self.project_dir = os.path.dirname(os.path.abspath(__file__))
        
    def run_demo(self, demo_type: str, steps: int = 1000, verbose: bool = False, 
                 config_file: Optional[str] = None) -> bool:
        """Run a specific demo.
        
        Args:
            demo_type: Type of demo ('phase3', 'phase4', 'phase6', 'benchmark')
            steps: Number of simulation steps
            verbose: Enable verbose output
            config_file: Custom configuration file
            
        Returns:
            bool: True if demo completed successfully
        """
        print(f"=== NLM {demo_type.upper()} Demo ===")
        print(f"Running {demo_type} demo with {steps} steps...")
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Override with custom config file if provided
        if config_file and os.path.exists(config_file):
            try:
                config.loadFromFile(config_file)
                print(f"Loaded configuration from: {config_file}")
            except Exception as e:
                print(f"Warning: Could not load config file {config_file}: {e}")
        
        # Set demo-specific configuration
        config.set("brain.neuron_count", min(1000, max(100, steps // 10)), 
                  pynlm.ConfigSource.Runtime)
        
        try:
            # Create brain and initialize
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            if verbose:
                print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
                
                if verbose and step % 100 == 0:
                    print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            # Print results
            print(f"\n=== Demo Results ===")
            print(f"Total steps: {steps}")
            print(f"Total time: {elapsed:.2f} seconds")
            print(f"Steps per second: {steps / elapsed:.1f}")
            print(f"Total neurons: {brain.getTotalNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"Development stage: {brain.getDevelopmentalStage()}")
            
            # Test integration for phase6
            if demo_type == "phase6":
                self._test_phase6_integration(brain, verbose)
            
            return True
            
        except Exception as e:
            print(f"Error running demo: {e}")
            return False
    
    def _test_phase6_integration(self, brain, verbose: bool):
        """Test Phase 6 integration features.
        
        Args:
            brain: The NLM brain instance
            verbose: Enable verbose output
        """
        print("\n=== Phase 6 Integration Test ===")
        
        # Check memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        print(f"Working Memory: {'ACTIVE' if working_memory else 'INACTIVE'}")
        print(f"Episodic Memory: {'ACTIVE' if episodic_memory else 'INACTIVE'}")
        print(f"Associative Memory: {'ACTIVE' if associative_memory else 'INACTIVE'}")
        
        # Check neuromodulation
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionError()
        
        print(f"Dopamine System: {'ACTIVE' if dopamine else 'INACTIVE'}")
        print(f"Curiosity System: {'ACTIVE' if curiosity else 'INACTIVE'}")
        print(f"Novelty System: {'ACTIVE' if novelty else 'INACTIVE'}")
        print(f"Prediction Error System: {'ACTIVE' if prediction_error else 'INACTIVE'}")
        
        # Check prediction system
        prediction_system = brain.getPredictionSystem()
        print(f"Prediction System: {'ACTIVE' if prediction_system else 'INACTIVE'}")
        
        # Check cognitive systems
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        print(f"Planner: {'ACTIVE' if planner else 'INACTIVE'}")
        print(f"Concept Formation: {'ACTIVE' if concept_formation else 'INACTIVE'}")
        print(f"Attention: {'ACTIVE' if attention else 'INACTIVE'}")
        
        # Check development system
        development_system = brain.getDevelopmentSystem()
        print(f"Development System: {'ACTIVE' if development_system else 'INACTIVE'}")
        
        if verbose:
            print("\n=== Detailed Integration Results ===")
            
            # Test each system's functionality
            if working_memory:
                print(f"Working Memory Active Traces: {working_memory.getActiveTraces()}")
            
            if episodic_memory:
                print(f"Episodic Memory Episode Count: {episodic_memory.getEpisodeCount()}")
            
            if dopamine:
                print(f"Dopamine Level: {brain.getNeuromodulationLevel():.3f}")
            
            if curiosity:
                print(f"Curiosity Level: {brain.getCuriosityLevel():.3f}")
            
            if novelty:
                print(f"Novelty Level: {brain.getNoveltyLevel():.3f}")
            
            if prediction_error:
                print(f"Prediction Error: {brain.getPredictionError():.3f}")
    
    def run_batch(self, demo_type: str, steps: int, config_file: Optional[str] = None,
                  output_file: Optional[str] = None) -> bool:
        """Run multiple simulations in batch.
        
        Args:
            demo_type: Type of demo to run
            steps: Number of simulation steps
            config_file: Custom configuration file
            output_file: File to save results
            
        Returns:
            bool: True if batch completed successfully
        """
        print(f"=== NLM Batch Mode ({demo_type}) ===")
        print(f"Running {steps} simulations...")
        
        results = []
        
        for i in range(steps):
            if i % 10 == 0:
                print(f"  Completed {i}/{steps} simulations...")
            
            success = self.run_demo(demo_type, 100, verbose=False, 
                                  config_file=config_file)
            if not success:
                print(f"Simulation {i} failed!")
                return False
            
            results.append({
                'simulation': i,
                'status': 'success'
            })
        
        print(f"\n=== Batch Results ===")
        print(f"Total simulations: {len(results)}")
        print(f"Successful: {len(results)}")
        print(f"Failed: {0}")
        
        # Save results if output file specified
        if output_file:
            self._save_results(results, output_file)
        
        return True
    
    def _save_results(self, results: list, output_file: str):
        """Save results to file.
        
        Args:
            results: List of result dictionaries
            output_file: File to save results to
        """
        try:
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Results saved to: {output_file}")
        except Exception as e:
            print(f"Warning: Could not save results to {output_file}: {e}")
    
    def run_benchmark(self, benchmark_type: str, steps: int, 
                     config_file: Optional[str] = None) -> bool:
        """Run performance benchmark.
        
        Args:
            benchmark_type: Type of benchmark ('spiking', 'plasticity', 'memory')
            steps: Number of simulation steps
            config_file: Custom configuration file
            
        Returns:
            bool: True if benchmark completed successfully
        """
        print(f"=== NLM Performance Benchmark ({benchmark_type}) ===")
        print(f"Running benchmark with {steps} steps...")
        
        # Create benchmark configuration
        config = pynlm.createDefaultConfig()
        
        if benchmark_type == "spiking":
            config.set("brain.neuron_count", 1000, pynlm.ConfigSource.Default)
        elif benchmark_type == "plasticity":
            config.set("brain.neuron_count", 500, pynlm.ConfigSource.Default)
        elif benchmark_type == "memory":
            config.set("brain.neuron_count", 300, pynlm.ConfigSource.Default)
        
        try:
            # Create brain and initialize
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Warm up
            for _ in range(100):
                brain.step(0)
            
            # Run benchmark
            start_time = time.time()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            # Calculate metrics
            steps_per_second = steps / elapsed
            neurons_per_second = brain.getTotalNeuronCount() * steps_per_second
            spikes_per_second = brain.getTotalSpikeCount() / elapsed
            
            print(f"\n=== Benchmark Results ===")
            print(f"Benchmark type: {benchmark_type}")
            print(f"Total steps: {steps}")
            print(f"Total time: {elapsed:.2f} seconds")
            print(f"Steps per second: {steps_per_second:.1f}")
            print(f"Neurons per second: {neurons_per_second:.0f}")
            print(f"Spikes per second: {spikes_per_second:.0f}")
            print(f"Total neurons: {brain.getTotalNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
            
            # Save benchmark results
            benchmark_results = {
                'benchmark_type': benchmark_type,
                'steps': steps,
                'total_time': elapsed,
                'steps_per_second': steps_per_second,
                'neurons_per_second': neurons_per_second,
                'spikes_per_second': spikes_per_second,
                'total_neurons': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'timestamp': time.time()
            }
            
            with open(f"/tmp/nlm_benchmark_{benchmark_type}_{int(time.time())}.json", 'w') as f:
                json.dump(benchmark_results, f, indent=2)
            
            print(f"Benchmark results saved to: /tmp/nlm_benchmark_{benchmark_type}_*.json")
            
            return True
            
        except Exception as e:
            print(f"Error running benchmark: {e}")
            return False
    
    def _handle_demo_command(self) -> bool:
        """Handle the demo command."""
        if len(sys.argv) < 3:
            print("Error: Missing demo type. Use 'help demo' for more information.")
            return 1
        
        demo_type = sys.argv[2]
        steps = 1000
        verbose = False
        config_file = None
        output_file = None
        
        # Parse options
        i = 3
        while i < len(sys.argv):
            arg = sys.argv[i]
            if arg == "--steps" and i + 1 < len(sys.argv):
                steps = int(sys.argv[i + 1])
                i += 2
            elif arg == "--verbose" or arg == "-v":
                verbose = True
                i += 1
            elif arg == "--config" and i + 1 < len(sys.argv):
                config_file = sys.argv[i + 1]
                i += 2
            elif arg == "--output" and i + 1 < len(sys.argv):
                output_file = sys.argv[i + 1]
                i += 2
            else:
                i += 1
        
        return self.run_demo(demo_type, steps, verbose, config_file)
    
    def _handle_batch_command(self) -> bool:
        """Handle the batch command."""
        if len(sys.argv) < 4:
            print("Error: Missing arguments for batch command. Use 'help batch' for more information.")
            return 1
        
        demo_type = sys.argv[2]
        try:
            num_simulations = int(sys.argv[3])
        except ValueError:
            print(f"Error: Invalid number of simulations: {sys.argv[3]}")
            return 1
        
        steps = 1000
        config_file = None
        output_file = None
        
        # Parse options
        i = 4
        while i < len(sys.argv):
            arg = sys.argv[i]
            if arg == "--steps" and i + 1 < len(sys.argv):
                steps = int(sys.argv[i + 1])
                i += 2
            elif arg == "--config" and i + 1 < len(sys.argv):
                config_file = sys.argv[i + 1]
                i += 2
            elif arg == "--output" and i + 1 < len(sys.argv):
                output_file = sys.argv[i + 1]
                i += 2
            else:
                i += 1
        
        return self.run_batch(demo_type, num_simulations, config_file, output_file)
    
    def _handle_benchmark_command(self) -> bool:
        """Handle the benchmark command."""
        if len(sys.argv) < 3:
            print("Error: Missing benchmark type. Use 'help benchmark' for more information.")
            return 1
        
        benchmark_type = sys.argv[2]
        steps = 1000
        config_file = None
        
        # Parse options
        i = 3
        while i < len(sys.argv):
            arg = sys.argv[i]
            if arg == "--steps" and i + 1 < len(sys.argv):
                steps = int(sys.argv[i + 1])
                i += 2
            elif arg == "--config" and i + 1 < len(sys.argv):
                config_file = sys.argv[i + 1]
                i += 2
            else:
                i += 1
        
        return self.run_benchmark(benchmark_type, steps, config_file)
    
    def _handle_config_command(self) -> bool:
        """Handle the config command."""
        print("Config management is not yet implemented in the CLI.")
        print("For configuration options, use the Python API:")
        print("  import pynlm")
        print("  config = pynlm.createDefaultConfig()")
        print("  config.set('brain.neuron_count', 1000)")
        print("  config.saveToFile('my_config.json')")
        return 0

def main():
    """Main entry point for NLM CLI."""
    cli = NLMCLI()
    
    if len(sys.argv) < 2:
        cli.show_help()
        return 1
    
    command = sys.argv[1].lower()
    
    if command == "help":
        if len(sys.argv) > 2:
            # Help for specific command
            cmd_help = sys.argv[2].lower()
            if cmd_help == "demo":
                print("Demo command help:")
                print("  python nlm_cli.py demo <type> [options]")
                print("  type: phase3, phase4, phase6, benchmark")
                print("  options: --steps N, --verbose, --config FILE, --output FILE")
            elif cmd_help == "batch":
                print("Batch command help:")
                print("  python nlm_cli.py batch <type> steps [options]")
                print("  type: phase3, phase4, phase6, benchmark")
                print("  steps: Number of simulations")
            elif cmd_help == "benchmark":
                print("Benchmark command help:")
                print("  python nlm_cli.py benchmark <type> [options]")
                print("  type: spiking, plasticity, memory")
                print("  options: --steps N, --config FILE")
            elif cmd_help == "config":
                print("Config command help:")
                print("  python nlm_cli.py config [options]")
                print("  options: --show, --set KEY VALUE, --save FILE")
            else:
                print(f"Unknown command: {cmd_help}")
        else:
            cli.show_help()
        return 0
    
    elif command == "version":
        print(f"NLM Python CLI Version {cli.version}")
        return 0
    
    elif command == "demo":
        return cli._handle_demo_command()
    
    elif command == "batch":
        return cli._handle_batch_command()
    
    elif command == "benchmark":
        return cli._handle_benchmark_command()
    
    elif command == "config":
        return cli._handle_config_command()
    
    else:
        print(f"Unknown command: {command}")
        cli.show_help()
        return 1

if __name__ == "__main__":
    sys.exit(main())