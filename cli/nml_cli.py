#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

A comprehensive CLI tool for NLM neural simulation management.
"""

import argparse
import sys
import os
from pathlib import Path
from typing import List, Optional

# Add the src directory to Python path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent / "src"))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from memory.Memory import WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory, AssociativeMemory
    from brain.Neuron import Neuron
    from brain.NeuronType import NeuronType
    from environment.ActionType import ActionType
    from core.ExperimentStep import SimulationStep
    from experiments.ExperimentRunner import ExperimentRunner
    from experiments.Phase6Demo import Phase6Demo
    from performance.PerformanceMonitor import PerformanceMonitor
    
    CLI_AVAILABLE = True
except ImportError as e:
    CLI_AVAILABLE = False
    print(f"Warning: Could not import NLM modules: {e}")
    print("Some CLI features will not be available.")

class NLMError(Exception):
    """Base exception for NLM CLI errors."""
    pass

class NLMCLI:
    """NLM Command Line Interface."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.performance_monitor = None
        
    def load_config(self, filepath: str) -> bool:
        """Load configuration from file."""
        try:
            self.config = Config()
            if self.config.load_from_file(filepath):
                print(f"Configuration loaded from {filepath}")
                return True
            else:
                print(f"Failed to load configuration from {filepath}")
                return False
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def create_brain(self) -> bool:
        """Create a new brain instance."""
        try:
            self.brain = Brain()
            
            # Configure brain with settings from config
            if self.config:
                max_steps = self.config.get_or("max_steps", 1000)
                self.brain.set_max_steps(max_steps)
                
                time_step = self.config.get_or("time_step", 0.001)
                self.brain.set_time_step(time_step)
                
                leak_conductance = self.config.get_or("leak_conductance", 10.0)
                self.brain.set_leak_conductance(leak_conductance)
            
            print("Brain created successfully")
            return True
        except Exception as e:
            print(f"Error creating brain: {e}")
            return False
    
    def setup_memory_systems(self) -> bool:
        """Setup memory systems in the brain."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            # Configure memory system sizes from config
            working_memory_capacity = self.config.get_or("working_memory_capacity", 100) if self.config else 100
            episodic_memory_capacity = self.config.get_or("episodic_memory_capacity", 1000) if self.config else 1000
            
            # Setup working memory
            working_memory = self.brain.create_working_memory(capacity=working_memory_capacity)
            print(f"Working memory created with capacity {working_memory_capacity}")
            
            # Setup episodic memory
            episodic_memory = self.brain.create_episodic_memory(max_episodes=episodic_memory_capacity)
            print(f"Episodic memory created with capacity {episodic_memory_capacity}")
            
            # Setup other memory systems
            self.brain.create_semantic_memory()
            print("Semantic memory created")
            
            self.brain.create_procedural_memory()
            print("Procedural memory created")
            
            self.brain.create_associative_memory()
            print("Associative memory created")
            
            return True
        except Exception as e:
            print(f"Error setting up memory systems: {e}")
            return False
    
    def setup_neural_network(self) -> bool:
        """Setup neural network structure."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            # Create neural regions from config
            if self.config:
                regions_config = self.config.get("regions", {})
                for region_name, region_config in regions_config.items():
                    neuron_count = region_config.get("neuron_count", 10)
                    neuron_type = self._get_neuron_type(region_config.get("neuron_type", "excitatory"))
                    self.brain.create_region(region_name, neuron_count, neuron_type)
                    print(f"Created region '{region_name}' with {neuron_count} {neuron_type} neurons")
            
            # Create default regions if not configured
            if not self.config or "regions" not in self.config.get_or({}, {}, {}):
                self.brain.create_region("sensory", 10, NeuronType.EXCITATORY)
                self.brain.create_region("motor", 5, NeuronType.EXCITATORY)
                self.brain.create_region("associative", 20, NeuronType.EXCITATORY)
                self.brain.create_region("cognitive", 15, NeuronType.EXCITATORY)
                print("Created default neural regions")
            
            return True
        except Exception as e:
            print(f"Error setting up neural network: {e}")
            return False
    
    def _get_neuron_type(self, neuron_type_str: str) -> NeuronType:
        """Convert string to NeuronType."""
        neuron_type_str = neuron_type_str.lower()
        if neuron_type_str == "excitatory":
            return NeuronType.EXCITATORY
        elif neuron_type_str == "inhibitory":
            return NeuronType.INHIBITORY
        elif neuron_type_str == "interneuron":
            return NeuronType.INTERNEURON
        elif neuron_type_str == "external":
            return NeuronType.EXTERNAL
        else:
            return NeuronType.EXCITATORY  # Default
    
    def add_connections(self) -> bool:
        """Add synaptic connections."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            # Get all neurons
            all_neurons = self.brain.get_all_neurons()
            if len(all_neurons) < 2:
                print("Need at least 2 neurons for connections")
                return False
            
            # Add connections between regions
            regions = self.brain.get_all_regions()
            for region_name, region in regions.items():
                neurons = region.get_all_neurons()
                if neurons:
                    # Connect to sensory region if it exists
                    if region_name != "sensory" and "sensory" in regions:
                        sensory_neurons = regions["sensory"].get_all_neurons()
                        if sensory_neurons and neurons:
                            for sensory_neuron in sensory_neurons:
                                for target_neuron in neurons:
                                    self.brain.create_synapse(
                                        sensory_neuron, 
                                        target_neuron, 
                                        weight=self.config.get_or("default_synaptic_weight", 0.5) if self.config else 0.5
                                    )
                    
                    # Add recurrent connections within region
                    if len(neurons) >= 2:
                        for i in range(min(3, len(neurons))):
                            for j in range(min(3, len(neurons))):
                                if i != j:
                                    self.brain.create_synapse(
                                        neurons[i], 
                                        neurons[j], 
                                        weight=self.config.get_or("recurrent_weight", 0.2) if self.config else 0.2
                                    )
            
            print(f"Added {len(self.brain.get_all_synapses())} synaptic connections")
            return True
        except Exception as e:
            print(f"Error adding connections: {e}")
            return False
    
    def run_simulation(self, steps: int) -> bool:
        """Run the simulation."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            print(f"Running simulation for {steps} steps...")
            
            # Start performance monitoring
            self.performance_monitor = PerformanceMonitor()
            self.performance_monitor.start()
            
            # Store initial statistics
            initial_stats = self.brain.get_statistics()
            
            # Run simulation
            for step in range(steps):
                # Apply time-varying input (sine wave for demonstration)
                import math
                input_value = 0.5 * (1 + math.sin(2 * math.pi * step / 100)) / 2
                
                # Apply input to sensory region
                for region_name in ["sensory", "motor", "associative", "cognitive"]:
                    self.brain.set_region_input(region_name, input_value)
                
                # Step brain simulation
                self.brain.step()
                
                # Print progress every 10% of simulation
                if step % max(1, steps // 10) == 0:
                    current_stats = self.brain.get_statistics()
                    print(f"Step {step}/{steps}, "
                          f"spike rate: {current_stats.average_firing_rate:.2f} Hz, "
                          f"total spikes: {current_stats.total_spikes}")
            
            # Get final statistics
            final_stats = self.brain.get_statistics()
            
            # Stop performance monitoring
            if self.performance_monitor:
                self.performance_monitor.stop()
                perf_stats = self.performance_monitor.get_statistics()
                print(f"\nPerformance Statistics:")
                print(f"  Total time: {perf_stats.total_time:.2f} ms")
                print(f"  Average step time: {perf_stats.average_step_time:.4f} ms")
                print(f"  Min step time: {perf_stats.min_step_time:.4f} ms")
                print(f"  Max step time: {perf_stats.max_step_time:.4f} ms")
                print(f"  Average memory usage: {perf_stats.average_memory_usage:.2f} MB")
            
            print(f"\nSimulation complete!")
            print(f"Total simulation time: {final_stats.simulation_time:.2f} ms")
            print(f"Total spikes: {final_stats.total_spikes}")
            print(f"Average firing rate: {final_stats.average_firing_rate:.2f} Hz")
            print(f"Max firing rate: {final_stats.max_firing_rate:.2f} Hz")
            
            # Store final results
            self.save_results("simulation_results.txt")
            
            return True
        except Exception as e:
            print(f"Error during simulation: {e}")
            return False
    
    def save_results(self, filename: str) -> bool:
        """Save simulation results to file."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            with open(filename, "w") as f:
                f.write("NLM Simulation Results\n")
                f.write("=" * 50 + "\n\n")
                
                # Get statistics
                stats = self.brain.get_statistics()
                mem_stats = self.brain.get_memory_statistics()
                
                f.write(f"Simulation Statistics:\n")
                f.write(f"  Total simulation time: {stats.simulation_time:.2f} ms\n")
                f.write(f"  Total steps: {stats.total_steps}\n")
                f.write(f"  Total spikes: {stats.total_spikes}\n")
                f.write(f"  Average firing rate: {stats.average_firing_rate:.2f} Hz\n")
                f.write(f"  Max firing rate: {stats.max_firing_rate:.2f} Hz\n\n")
                
                f.write(f"Memory Statistics:\n")
                f.write(f"  Working memory usage: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}\n")
                f.write(f"  Episodic memory episodes: {mem_stats.episodic_memory_episodes}\n")
                f.write(f"  Semantic memory facts: {len(self.brain.get_semantic_memory().get_all_facts())}\n")
                f.write(f"  Procedural memory skills: {len(self.brain.get_procedural_memory().get_all_skills())}\n")
                f.write(f"  Associative memory associations: {mem_stats.associative_memory_associations}\n\n")
                
                # Save neuron information
                f.write(f"Neuron Information:\n")
                for region_name, region in self.brain.get_all_regions().items():
                    neurons = region.get_all_neurons()
                    f.write(f"  Region '{region_name}': {len(neurons)} neurons\n")
                    
                    for neuron in neurons:
                        f.write(f"    Neuron {neuron.get_id()}: "
                               f"Type={neuron.get_type()}, "
                               f"Region={neuron.get_region_id()}, "
                               f"Current={neuron.get_state().membranePotential:.2f} mV\n")
            
            print(f"Results saved to {filename}")
            return True
        except Exception as e:
            print(f"Error saving results: {e}")
            return False
    
    def run_experiment(self, experiment_name: str) -> bool:
        """Run a predefined experiment."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            print(f"Running experiment: {experiment_name}")
            
            if experiment_name == "phase6_demo":
                # Run Phase 6 demo
                demo = Phase6Demo()
                demo.run(brain=self.brain)
                print("Phase 6 demo completed")
                
            elif experiment_name == "integration_test":
                # Run integration test
                runner = ExperimentRunner()
                runner.run_integration_test(brain=self.brain)
                print("Integration test completed")
                
            elif experiment_name == "stress_test":
                # Run stress test
                if self.config:
                    steps = self.config.get_or("stress_test_steps", 5000)
                else:
                    steps = 5000
                
                print(f"Running stress test with {steps} steps...")
                for step in range(steps):
                    self.brain.step()
                    if step % 1000 == 0:
                        print(f"  Progress: {step}/{steps} steps")
                
                print("Stress test completed")
                
            else:
                print(f"Unknown experiment: {experiment_name}")
                return False
            
            return True
        except Exception as e:
            print(f"Error running experiment: {e}")
            return False
    
    def show_status(self) -> bool:
        """Show current status of the brain system."""
        try:
            if not self.brain:
                print("Brain not initialized")
                return False
            
            print("\n=== NLM System Status ===\n")
            
            # Get brain statistics
            stats = self.brain.get_statistics()
            mem_stats = self.brain.get_memory_statistics()
            
            print(f"Brain Status:")
            print(f"  Max steps: {stats.max_steps}")
            print(f"  Current step: {stats.total_steps}")
            print(f"  Time step: {stats.time_step:.4f} ms")
            print(f"  Simulation time: {stats.simulation_time:.2f} ms")
            print(f"  Total spikes: {stats.total_spikes}")
            print(f"  Average firing rate: {stats.average_firing_rate:.2f} Hz")
            
            print(f"\nMemory Status:")
            print(f"  Working memory: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity} ({mem_stats.working_memory_usage/mem_stats.working_memory_capacity*100:.1f}%)")
            print(f"  Episodic memory: {mem_stats.episodic_memory_episodes} episodes")
            print(f"  Semantic memory: {len(self.brain.get_semantic_memory().get_all_facts())} facts")
            print(f"  Procedural memory: {len(self.brain.get_procedural_memory().get_all_skills())} skills")
            print(f"  Associative memory: {mem_stats.associative_memory_associations} associations")
            
            print(f"\nNeural Network Status:")
            for region_name, region in self.brain.get_all_regions().items():
                neurons = region.get_all_neurons()
                print(f"  Region '{region_name}': {len(neurons)} neurons")
            
            print(f"  Total synapses: {len(self.brain.get_all_synapses())}")
            
            if self.performance_monitor:
                perf_stats = self.performance_monitor.get_statistics()
                print(f"\nPerformance Monitoring:")
                print(f"  Total time: {perf_stats.total_time:.2f} ms")
                print(f"  Average step time: {perf_stats.average_step_time:.4f} ms")
                print(f"  Peak memory usage: {perf_stats.peak_memory_usage:.2f} MB")
            
            return True
        except Exception as e:
            print(f"Error showing status: {e}")
            return False
    
    def save_config(self, filepath: str) -> bool:
        """Save configuration to file."""
        try:
            if not self.config:
                print("No configuration to save")
                return False
            
            if self.config.save_to_file(filepath):
                print(f"Configuration saved to {filepath}")
                return True
            else:
                print(f"Failed to save configuration to {filepath}")
                return False
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False

def main():
    """Main CLI function."""
    if not CLI_AVAILABLE:
        print("Error: NLM modules not available. Please ensure the C++ library is built and installed.")
        sys.exit(1)
    
    cli = NLMCLI()
    
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s init --config config.yaml
  %(prog)s simulate --steps 1000
  %(prog)s experiment --name phase6_demo
  %(prog)s status
  %(prog)s results simulation_results.txt

Configuration file examples:
  max_steps: 1000
  time_step: 0.001
  leak_conductance: 10.0
  working_memory_capacity: 100
  episodic_memory_capacity: 1000
  default_synaptic_weight: 0.5
  recurrent_weight: 0.2

  regions:
    sensory:
      neuron_count: 10
      neuron_type: excitatory
    motor:
      neuron_count: 5
      neuron_type: excitatory
    associative:
      neuron_count: 20
      neuron_type: excitatory
    cognitive:
      neuron_count: 15
      neuron_type: excitatory
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Init command
    init_parser = subparsers.add_parser("init", help="Initialize NLM system")
    init_parser.add_argument("--config", "-c", help="Configuration file path")
    init_parser.add_argument("--no-memory", action="store_true", help="Skip memory system setup")
    init_parser.add_argument("--no-connections", action="store_true", help="Skip synaptic connection setup")
    
    # Simulate command
    simulate_parser = subparsers.add_parser("simulate", help="Run simulation")
    simulate_parser.add_argument("--steps", "-s", type=int, default=1000, help="Number of simulation steps (default: 1000)")
    simulate_parser.add_argument("--monitor", action="store_true", help="Enable performance monitoring")
    simulate_parser.add_argument("--save-results", "-o", help="Save results to file")
    
    # Experiment command
    experiment_parser = subparsers.add_parser("experiment", help="Run predefined experiment")
    experiment_parser.add_argument("--name", "-n", required=True, 
                                  choices=["phase6_demo", "integration_test", "stress_test"],
                                  help="Experiment name")
    experiment_parser.add_argument("--config", "-c", help="Configuration file path")
    
    # Status command
    subparsers.add_parser("status", help="Show system status")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration commands")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Configuration subcommands")
    
    save_config_parser = config_subparsers.add_parser("save", help="Save configuration")
    save_config_parser.add_argument("filepath", help="Configuration file path")
    
    # Results command
    results_parser = subparsers.add_parser("results", help="Results commands")
    results_subparsers = results_parser.add_subparsers(dest="results_command", help="Results subcommands")
    
    show_results_parser = results_subparsers.add_parser("show", help="Show results")
    show_results_parser.add_argument("filepath", help="Results file path")
    
    # Connection command
    connection_parser = subparsers.add_parser("connection", help="Connection commands")
    connection_subparsers = connection_parser.add_subparsers(dest="connection_command", help="Connection subcommands")
    
    add_connections_parser = connection_subparsers.add_parser("add", help="Add connections")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show help for a specific command")
    help_parser.add_argument("command", help="Command to show help for")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    success = True
    
    if args.command == "init":
        if args.config:
            success = cli.load_config(args.config)
            if not success:
                sys.exit(1)
        
        success = cli.create_brain()
        if not success:
            sys.exit(1)
        
        if not args.no_memory:
            success = cli.setup_memory_systems()
            if not success:
                sys.exit(1)
        
        success = cli.setup_neural_network()
        if not success:
            sys.exit(1)
        
        if not args.no_connections:
            success = cli.add_connections()
            if not success:
                sys.exit(1)
        
        print("NLM system initialized successfully")
        
    elif args.command == "simulate":
        if not cli.brain:
            print("Brain not initialized. Use 'init' command first.")
            sys.exit(1)
        
        success = cli.run_simulation(args.steps)
        if not success:
            sys.exit(1)
        
    elif args.command == "experiment":
        if args.config:
            success = cli.load_config(args.config)
            if not success:
                sys.exit(1)
        
        if not cli.brain:
            success = cli.create_brain()
            if not success:
                sys.exit(1)
            
            success = cli.setup_memory_systems()
            if not success:
                sys.exit(1)
            
            success = cli.setup_neural_network()
            if not success:
                sys.exit(1)
        
        success = cli.run_experiment(args.name)
        if not success:
            sys.exit(1)
        
    elif args.command == "status":
        success = cli.show_status()
        if not success:
            sys.exit(1)
        
    elif args.command == "config":
        if args.config_command == "save":
            if not cli.config:
                print("No configuration to save")
                sys.exit(1)
            
            success = cli.save_config(args.filepath)
            if not success:
                sys.exit(1)
        
    elif args.command == "results":
        if args.results_command == "show":
            if not os.path.exists(args.filepath):
                print(f"Results file not found: {args.filepath}")
                sys.exit(1)
            
            with open(args.filepath, "r") as f:
                print(f.read())
        
    elif args.command == "connection":
        if args.connection_command == "add":
            if not cli.brain:
                print("Brain not initialized. Use 'init' command first.")
                sys.exit(1)
            
            success = cli.add_connections()
            if not success:
                sys.exit(1)
        
    elif args.command == "help":
        # Show help for specific command
        parser.print_help()
        
    else:
        parser.print_help()
        sys.exit(1)
    
    print("\nCommand completed successfully!")

if __name__ == "__main__":
    main()