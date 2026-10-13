#!/usr/bin/env python3

# Make the script executable
import os
import stat

# Add executable permission
st = os.stat(__file__)
os.chmod(__file__, st.st_mode | stat.S_IEXEC)
"""
NLM Python CLI - Python interface for NLM (Neural Learning Machine) advanced debugging and analysis.

This script provides a Python-based command line interface for NLM users who prefer
Python over native command line tools. It offers the same advanced debugging,
analysis, and control capabilities as the nlmc command line tool.
"""
"""

import sys
import json
import argparse
from typing import Optional, List, Dict, Any
import numpy as np

# Try to import NLM modules
try:
    from nlm import Config, Brain, NeuronType
    from nlm.core.Types import RegionId, NeuronId
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Warning: NLM Python bindings not available. Some features will be limited.", file=sys.stderr)

class NLMCLI:
    """NLM Command Line Interface in Python."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.current_step = 0
        
    def initialize(self, config_file: str = "configs/default.cfg") -> bool:
        """Initialize NLM brain from configuration file."""
        if not NLM_AVAILABLE:
            print("NLM not available. Cannot initialize brain.", file=sys.stderr)
            return False
            
        try:
            # Create config
            self.config = Config()
            
            # Load configuration
            if config_file and config_file != "none":
                if self.config.loadFromFile(config_file):
                    print(f"Loaded configuration from: {config_file}")
                else:
                    print("Using default configuration.")
            
            # Set default values for Phase 2
            self.config.set("random_seed", 42)
            self.config.set("simulation_timestep", 0.001)
            self.config.set("neuron_count", 500)
            self.config.set("region_count", 1)
            self.config.set("connection_probability", 0.15)
            
            # Initialize brain
            self.brain = Brain(self.config)
            if self.brain.initialize():
                print("NLM brain initialized successfully.")
                print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
                print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
                return True
            else:
                print("Failed to initialize brain!", file=sys.stderr)
                return False
                
        except Exception as e:
            print(f"Error initializing NLM: {e}", file=sys.stderr)
            return False
    
    def show_help(self, command: Optional[str] = None):
        """Show help information for commands."""
        help_text = {
            "debug-neurons": """
Debug Neurons
=============
Shows detailed information about neurons in the brain.
Usage: nlmcli debug-neurons [count]

Arguments:
  count   Number of neurons to display (default: 10)

Examples:
  nlmcli debug-neurons
  nlmcli debug-neurons 20
""",
            
            "debug-synapses": """
Debug Synapses
===============
Shows detailed information about synapses in the brain.
Usage: nlmcli debug-synapses
""",
            
            "stats": """
Statistics
===========
Shows comprehensive statistics about the brain state.
Usage: nlmcli stats
""",
            
            "learning-curve": """
Learning Curve
===============
Analyzes learning progress and synaptic changes.
Usage: nlmcli learning-curve
""",
            
            "network-analysis": """
Network Analysis
================
Analyzes network properties and connectivity patterns.
Usage: nlmcli network-analysis
""",
            
            "save": """
Save Brain State
=================
Saves the current brain state to a file.
Usage: nlmcli save <filename>

Arguments:
  filename  Path to save the brain state
""",
            
            "load": """
Load Brain State
================
Loads a brain state from a file.
Usage: nlmcli load <filename>

Arguments:
  filename  Path to load the brain state from
""",
            
            "reset": """
Reset Brain
===========
Resets the brain to its initial state.
Usage: nlmcli reset
""",
            
            "config": """
Show Configuration
===================
Shows the current configuration settings.
Usage: nlmcli config
""",
            
            "config-set": """
Set Configuration Value
=======================
Sets a configuration value.
Usage: nlmcli config-set <key> <value>

Arguments:
  key    Configuration key
  value  Configuration value
""",
            
            "run-experiment": """
Run Experiment
===============
Runs a predefined experiment.
Usage: nlmcli run-experiment <name>

Arguments:
  name  Experiment name (plasticity, stdp, connectivity)
""",
            
            "export-neurons": """
Export Neurons
===============
Exports neuron data to a file.
Usage: nlmcli export-neurons <filename>

Arguments:
  filename  Path to export neuron data
""",
            
            "export-synapses": """
Export Synapses
===============
Exports synapse data to a file.
Usage: nlmcli export-synapses <filename>

Arguments:
  filename  Path to export synapse data
""",
            
            "heatmap": """
Generate Heatmap
===============
Generates a neural activity heatmap.
Usage: nlmcli heatmap
""",
            
            "help": """
Help
====
Shows help information for commands.
Usage: nlmcli help [command]

Arguments:
  command  Specific command to show help for
""",
            
            "version": """
Version
=======
Shows NLM version information.
Usage: nlmcli version
"""
        }
        
        if command and command in help_text:
            print(help_text[command])
        elif command:
            print(f"Unknown command: {command}", file=sys.stderr)
            self.show_help()
        else:
            print("NLM Python CLI - Advanced Debugging and Analysis")
            print("=" * 50)
            print("Available commands:")
            for cmd in sorted(help_text.keys()):
                print(f"  {cmd}")
            print("\nFor detailed help on a specific command, use: nlmcli help <command>")
    
    def show_version(self):
        """Show NLM version information."""
        print("NLM (熙然) - Neural Learning Machine")
        print("Version: 1.0.0 (Phase 6 - Integrated Artificial Brain)")
        print("Built with C++20, SIMD optimization, and Python bindings")
        print("Advanced neural computation framework with memory, cognition, and neuromodulation")
    
    def debug_neurons(self, count: int = 10):
        """Debug neurons information."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        print(f"=== NLM Neuron Debug Information ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Active neurons: {self.brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        
        regions = self.brain.getRegionIds()
        for i, region_id in enumerate(regions):
            if i >= count:
                break
                
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                if neurons:
                    neuron = neurons[min(i, len(neurons) - 1)]
                    state = neuron.getState()
                    print(f"\nNeuron {i}:")
                    print(f"  Type: {state.firingState}")
                    print(f"  Membrane potential: {state.membranePotential:.2f} mV")
                    print(f"  Threshold: {state.threshold:.2f} mV")
                    print(f"  Refractory remaining: {state.refractoryRemaining}")
                    print(f"  Last spike time: {state.lastSpikeTime:.3f}")
        
        return True
    
    def debug_synapses(self):
        """Debug synapses information."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        print("=== NLM Synapse Debug Information ===")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        
        total_excitatory = 0.0
        total_inhibitory = 0.0
        excitatory_count = 0
        inhibitory_count = 0
        
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                weight = syn.getWeight()
                if weight > 0:
                    total_excitatory += weight
                    excitatory_count += 1
                else:
                    total_inhibitory += abs(weight)
                    inhibitory_count += 1
        
        print(f"Excitatory synapses: {excitatory_count}")
        print(f"Inhibitory synapses: {inhibitory_count}")
        print(f"Total excitatory weight: {total_excitatory:.4f}")
        print(f"Total inhibitory weight: {total_inhibitory:.4f}")
        
        if total_inhibitory > 0:
            ei_ratio = total_excitatory / total_inhibitory
        elif total_excitatory > 0:
            ei_ratio = float('inf')
        else:
            ei_ratio = 0.0
            
        print(f"E/I ratio: {ei_ratio}")
        return True
    
    def show_stats(self):
        """Show comprehensive statistics."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        print("=== NLM Brain Statistics ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"Active neurons: {self.brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio()}")
        print(f"Pending events: {self.brain.getPendingSpikeEventCount()}")
        
        # Memory system stats
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            print(f"Working memory traces: {working_mem.getActiveTraces()}")
            
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
        
        return True
    
    def run_experiment(self, experiment_name: str):
        """Run a predefined experiment."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        print(f"\n=== Running Experiment: {experiment_name} ===")
        
        if experiment_name.lower() == "plasticity":
            print("Running plasticity learning experiment...")
            
            # Enable plasticity on synapses
            for region in self.brain.getRegions():
                for syn in region.getSynapses():
                    syn.enablePlasticity(True, True, False)  # Enable Hebbian and STDP
            
            # Run learning simulation
            for step in range(100):
                self.brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0)
                self.brain.step(step, step * 0.001)
                
            print(f"Experiment complete. Total spikes: {self.brain.getTotalSpikeCount()}")
            
        elif experiment_name.lower() == "stdp":
            print("Running STDP verification experiment...")
            # Enable only STDP
            for region in self.brain.getRegions():
                for syn in region.getSynapses():
                    syn.enablePlasticity(False, True, False)
            
            print("STDP experiment setup complete.")
            
        elif experiment_name.lower() == "connectivity":
            print("Running basic connectivity test...")
            # Inject current to test spike propagation
            for neuron_count in range(min(10, self.brain.getTotalNeuronCount())):
                for region in self.brain.getRegions():
                    neurons = region.getAllNeurons()
                    if neurons:
                        neurons[neuron_count].injectCurrent(50.0)
            
            for step in range(50):
                self.brain.step(step, step * 0.001)
                
            print(f"Connectivity test complete. Total spikes: {self.brain.getTotalSpikeCount()}")
            
        else:
            print(f"Unknown experiment: {experiment_name}", file=sys.stderr)
            return False
            
        return True
    
    def run_command(self, command: str, args: List[str]):
        """Run a specific command."""
        try:
            if command == "help":
                self.show_help(args[0] if args else None)
                
            elif command == "version":
                self.show_version()
                
            elif command == "debug-neurons":
                count = int(args[0]) if args else 10
                self.debug_neurons(count)
                
            elif command == "debug-synapses":
                self.debug_synapses()
                
            elif command == "stats":
                self.show_stats()
                
            elif command == "learning-curve":
                print("=== Learning Curve Analysis ===")
                print("Note: Learning curve analysis requires historical data tracking.")
                print(f"Current spike activity: {self.brain.getTotalSpikeCount()} total spikes")
                
            elif command == "network-analysis":
                print("=== Network Analysis ===")
                total_neurons = self.brain.getTotalNeuronCount()
                total_synapses = self.brain.getTotalSynapseCount()
                
                if total_neurons > 0:
                    connectivity = total_synapses / (total_neurons * total_neurons)
                    print(f"Connectivity density: {connectivity:.6f}")
                
            elif command == "save":
                if args:
                    self.save_brain_state(args[0])
                else:
                    print("Error: Filename required for save command", file=sys.stderr)
                    
            elif command == "load":
                if args:
                    self.load_brain_state(args[0])
                else:
                    print("Error: Filename required for load command", file=sys.stderr)
                    
            elif command == "reset":
                self.reset_brain()
                
            elif command == "config":
                self.show_config()
                
            elif command == "config-set":
                if len(args) >= 2:
                    self.set_config_value(args[0], args[1])
                else:
                    print("Error: Key and value required for config-set command", file=sys.stderr)
                    
            elif command == "run-experiment":
                if args:
                    self.run_experiment(args[0])
                else:
                    print("Error: Experiment name required for run-experiment command", file=sys.stderr)
                    
            elif command == "export-neurons":
                if args:
                    self.export_neurons(args[0])
                else:
                    print("Error: Filename required for export-neurons command", file=sys.stderr)
                    
            elif command == "export-synapses":
                if args:
                    self.export_synapses(args[0])
                else:
                    print("Error: Filename required for export-synapses command", file=sys.stderr)
                    
            elif command == "heatmap":
                print("=== Heatmap Generation ===")
                print("Heatmap generation requires matplotlib and would create a visualization")
                print("of neural activity patterns.")
                
            else:
                print(f"Unknown command: {command}", file=sys.stderr)
                self.show_help()
                
        except Exception as e:
            print(f"Error executing command {command}: {e}", file=sys.stderr)
            return False
            
        return True
    
    def save_brain_state(self, filename: str):
        """Save brain state to file."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        if self.brain.save(filename):
            print(f"Brain state saved to: {filename}")
            return True
        else:
            print(f"Failed to save brain state to {filename}", file=sys.stderr)
            return False
    
    def load_brain_state(self, filename: str):
        """Load brain state from file."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        if self.brain.load(filename):
            print(f"Brain state loaded from: {filename}")
            return True
        else:
            print(f"Failed to load brain state from {filename}", file=sys.stderr)
            return False
    
    def reset_brain(self):
        """Reset brain to initial state."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        self.brain.reset()
        self.brain.initialize()
        print("Brain reset and reinitialized")
        return True
    
    def show_config(self):
        """Show configuration."""
        if not self.config:
            print("Config not available!", file=sys.stderr)
            return False
            
        print("=== NLM Configuration ===")
        print(self.config.summary())
        return True
    
    def set_config_value(self, key: str, value: str):
        """Set configuration value."""
        if not self.config:
            print("Config not available!", file=sys.stderr)
            return False
            
        try:
            if key == "random_seed":
                self.config.set(key, int(value))
            elif key in ["neuron_count", "region_count"]:
                self.config.set(key, int(value))
            elif key in ["connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", 
                        "stdp_tau", "synaptogenesis_rate", "pruning_rate"]:
                self.config.set(key, float(value))
            elif key in ["simulation_timestep", "reward_discount_factor"]:
                self.config.set(key, float(value))
            elif key == "log_level":
                self.config.set(key, value)
            else:
                print(f"Unknown config key: {key}", file=sys.stderr)
                return False
                
            print(f"Config {key} set to {value}")
            return True
            
        except Exception as e:
            print(f"Error setting config {key}: {e}", file=sys.stderr)
            return False
    
    def export_neurons(self, filename: str):
        """Export neurons to file."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        try:
            with open(filename, 'w') as f:
                f.write("Neuron Export\n")
                f.write("==============\n")
                f.write(f"Total neurons: {self.brain.getTotalNeuronCount()}\n\n")
                
                for i in range(min(10, self.brain.getTotalNeuronCount())):
                    f.write(f"Neuron {i}: Data not implemented in current version\n")
                    
            print(f"Neurons exported to {filename}")
            return True
            
        except Exception as e:
            print(f"Error exporting neurons: {e}", file=sys.stderr)
            return False
    
    def export_synapses(self, filename: str):
        """Export synapses to file."""
        if not self.brain:
            print("Brain not initialized!", file=sys.stderr)
            return False
            
        try:
            with open(filename, 'w') as f:
                f.write("Synapse Export\n")
                f.write("==============\n")
                f.write(f"Total synapses: {self.brain.getTotalSynapseCount()}\n\n")
                
                for region in self.brain.getRegions():
                    for syn in region.getSynapses():
                        f.write(f"{syn.getSourceNeuron().index()} -> {syn.getDestinationNeuron().index()} : "
                               f"{syn.getWeight()} (type: {syn.getType()})\n")
            
            print(f"Synapses exported to {filename}")
            return True
            
        except Exception as e:
            print(f"Error exporting synapses: {e}", file=sys.stderr)
            return False

def main():
    """Main function."""
    cli = NLMCLI()
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="NLM Python CLI - Advanced debugging and analysis for Neural Learning Machine",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('command', nargs='?', help='Command to execute')
    parser.add_argument('args', nargs='*', help='Command arguments')
    parser.add_argument('--config', '-c', default='configs/default.cfg', 
                       help='Configuration file path (default: configs/default.cfg)')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Check if user wants help
    if not args.command or args.command in ['help', '--help', '-h']:
        cli.show_help()
        return 0
    
    # Initialize NLM
    if not cli.initialize(args.config):
        return 1
    
    # Run the command
    if not cli.run_command(args.command, args.args):
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
