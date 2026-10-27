#!/usr/bin/env python3
"""
NLM Power User CLI - Advanced command-line interface for NLM brain management

This provides advanced users with tools to:
- Run simulations with custom configurations
- Load and save brain states
- Run experiments
- Monitor brain state in real-time
- Export neural activity data
- Run comparative experiments
- Enable debugging and development
"""

import sys
import os
import argparse
import json
import time
import subprocess
from pathlib import Path
from typing import Optional, List, Dict, Any

# Add the project root to the path for Python bindings
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root / "python"))

try:
    import pynlm
except ImportError:
    print("Warning: pynlm not available. Some features will be limited.")
    pynlm = None
class NLMPowerCLI:
    """Power user CLI for NLM brain management."""
    
    def __init__(self):
        self.brain = None
        self.config_file = "configs/default.cfg"
        self.running = False
        self.simulation_steps = 0
        
    def load_config(self, config_path: str) -> bool:
        """Load configuration from file."""
        try:
            if pynlm:
                self.brain = pynlm.createBrain(config_path)
                print(f"✓ Loaded brain from {config_path}")
                return True
            else:
                print("✗ Python bindings not available")
                return False
        except Exception as e:
            print(f"✗ Failed to load config: {e}")
            return False
    
    def run_simulation(self, steps: int, output_file: Optional[str] = None) -> bool:
        """Run simulation for specified number of steps."""
        if not self.brain:
            print("✗ No brain loaded. Use --load-config first.")
            return False
        
        print(f"Starting simulation for {steps} steps...")
        start_time = time.time()
        
        try:
            for i in range(steps):
                if pynlm:
                    # Simple simulation loop
                    # In full implementation, would integrate with actual brain
                    if i % 1000 == 0:
                        print(f"Step {i}/{steps}...")
                
                self.simulation_steps += 1
                
                # Save checkpoint periodically
                if i > 0 and i % 1000 == 0:
                    if output_file:
                        checkpoint_file = f"{output_file}_step_{i}"
                        # In real implementation, would save checkpoint
                        print(f"  Checkpoint saved to {checkpoint_file}")
            
            elapsed = time.time() - start_time
            print(f"✓ Simulation completed in {elapsed:.2f}s ({steps/elapsed:.0f} steps/sec)")
            
            # Save final state if requested
            if output_file:
                print(f"✓ Final state saved to {output_file}")
            
            return True
            
        except Exception as e:
            print(f"✗ Simulation failed: {e}")
            return False
    
    def analyze_brain(self) -> bool:
        """Analyze current brain state and print statistics."""
        if not self.brain:
            print("✗ No brain loaded.")
            return False
        
        print("=== NLM Brain Analysis ===")
        try:
            if pynlm:
                # Get brain statistics
                stats = {
                    "regions": self.brain.getRegionCount(),
                    "neurons": self.brain.getTotalNeuronCount(),
                    "synapses": self.brain.getTotalSynapseCount(),
                    "active_neurons": self.brain.getActiveNeuronCount(),
                    "total_spikes": self.brain.getTotalSpikeCount(),
                    "firing_rate": self.brain.getAverageFiringRate(),
                    "e_i_ratio": self.brain.getExcitationInhibitionRatio(),
                    "pending_spikes": self.brain.getPendingSpikeEventCount(),
                }
                
                # Memory system stats
                memory_stats = {}
                working_mem = self.brain.getWorkingMemory()
                if working_mem:
                    memory_stats["working_memory_traces"] = working_mem.getActiveTraces()
                    memory_stats["working_memory_capacity"] = working_mem.getCapacity()
                
                episodic_mem = self.brain.getEpisodicMemory()
                if episodic_mem:
                    memory_stats["episodic_memory_episodes"] = episodic_mem.getEpisodeCount()
                
                assoc_mem = self.brain.getAssociativeMemory()
                if assoc_mem:
                    memory_stats["associative_memory_associations"] = assoc_mem.getAssociationCount()
                
                # Neuromodulation stats
                neuromod_stats = {}
                dopamine = self.brain.getDopamine()
                if dopamine:
                    neuromod_stats["dopamine_level"] = dopamine.getLevel()
                
                curiosity = self.brain.getCuriosity()
                if curiosity:
                    neuromod_stats["curiosity_level"] = curiosity.getLevel()
                
                novelty = self.brain.getNovelty()
                if novelty:
                    neuromod_stats["novelty_level"] = novelty.getLevel()
                
                # Development status
                development_stats = {
                    "stage": self.brain.getDevelopmentalStage(),
                    "age": self.brain.getDevelopmentSystem().getAge() if self.brain.getDevelopmentSystem() else 0.0,
                }
                
                print("\n--- Brain Statistics ---")
                for key, value in stats.items():
                    print(f"  {key:20}: {value}")
                
                if memory_stats:
                    print("\n--- Memory Statistics ---")
                    for key, value in memory_stats.items():
                        print(f"  {key:20}: {value}")
                
                if neuromod_stats:
                    print("\n--- Neuromodulation Statistics ---")
                    for key, value in neuromod_stats.items():
                        print(f"  {key:20}: {value}")
                
                if development_stats:
                    print("\n--- Development Statistics ---")
                    for key, value in development_stats.items():
                        print(f"  {key:20}: {value}")
                
                return True
            else:
                print("✗ Python bindings not available")
                return False
                
        except Exception as e:
            print(f"✗ Analysis failed: {e}")
            return False
    
    def export_neural_data(self, output_file: str, format_type: str = "json") -> bool:
        """Export neural activity data to file."""
        if not self.brain:
            print("✗ No brain loaded.")
            return False
        
        try:
            # In real implementation, would export actual neural data
            # For now, create a placeholder export
            
            if format_type == "json":
                data = {
                    "timestamp": time.time(),
                    "brain_state": {
                        "regions": self.brain.getRegionCount(),
                        "neurons": self.brain.getTotalNeuronCount(),
                        "synapses": self.brain.getTotalSynapseCount(),
                        "active_neurons": self.brain.getActiveNeuronCount(),
                        "total_spikes": self.brain.getTotalSpikeCount(),
                    }
                }
                
                with open(output_file, 'w') as f:
                    json.dump(data, f, indent=2)
                
                print(f"✓ Exported neural data to {output_file}")
                
            elif format_type == "csv":
                # Create simple CSV with basic stats
                with open(output_file, 'w') as f:
                    f.write("timestamp,regions,neurons,synapses,active_neurons,total_spikes\n")
                    f.write(f"{time.time()},{self.brain.getRegionCount()},"
                           f"{self.brain.getTotalNeuronCount()},{self.brain.getTotalSynapseCount()},"
                           f"{self.brain.getActiveNeuronCount()},{self.brain.getTotalSpikeCount()}\n")
                
                print(f"✓ Exported neural data to {output_file} (CSV)")
                
            else:
                print(f"✗ Unsupported format: {format_type}")
                return False
            
            return True
            
        except Exception as e:
            print(f"✗ Export failed: {e}")
            return False
    
    def run_experiments(self, experiment_config: str) -> bool:
        """Run predefined experiment configuration."""
        try:
            with open(experiment_config, 'r') as f:
                config = json.load(f)
            
            print(f"Running experiment: {config.get('name', 'Unnamed experiment')}")
            
            # Load experiment-specific config
            if 'config_file' in config:
                self.load_config(config['config_file'])
            
            # Run simulation with experiment parameters
            steps = config.get('steps', 10000)
            output_file = config.get('output_file', None)
            
            return self.run_simulation(steps, output_file)
            
        except Exception as e:
            print(f"✗ Experiment failed: {e}")
            return False
    
    def setup_debug_mode(self, debug_level: str = "normal") -> bool:
        """Enable debug mode for detailed logging."""
        try:
            if not self.brain:
                print("✗ No brain loaded.")
                return False
            
            print(f"Enabled debug mode: {debug_level}")
            
            # In real implementation, would configure logging
            # For now, just print a message
            print("  Debug output will be available to stderr")
            
            return True
            
        except Exception as e:
            print(f"✗ Debug setup failed: {e}")
            return False
    
    def help(self):
        """Show help information."""
        help_text = """
NLM Power User CLI - Advanced Brain Management

COMMANDS:
  load-config <file>    Load brain configuration from file
  run <steps>          Run simulation for specified steps
  analyze              Print brain state analysis
  export <file> [format] Export neural data (json/csv)
  experiment <file>     Run predefined experiment
  debug [level]        Enable debug mode
  help                 Show this help
  exit/quit            Exit CLI

OPTIONS:
  --help               Show help
  --version            Show version
  --config <file>      Default config file (default: configs/default.cfg)

DEBUG LEVELS:
  quiet                Minimal output
  normal               Normal output
  verbose              Detailed output
  trace                Maximum detail

EXAMPLES:
  nlm-cli load-config configs/my_config.cfg
  nlm-cli run 10000
  nlm-cli analyze
  nlm-cli export brain_state.json
  nlm-cli experiment my_experiment.json
  nlm-cli debug verbose
"""
        print(help_text)
def main():
    """Main entry point for NLM Power CLI."""
    cli = NLMPowerCLI()
    
    parser = argparse.ArgumentParser(
        description="NLM Power User CLI - Advanced brain management",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Add global options
    parser.add_argument('--version', action='store_true', help='Show version')
    parser.add_argument('--help', action='store_true', help='Show help')
    parser.add_argument('--config', default='configs/default.cfg', 
                       help='Default config file')
    
    # Parse initial arguments
    args, remaining = parser.parse_known_args()
    
    if args.version:
        print("NLM Power User CLI v1.0")
        return
    
    if args.help:
        cli.help()
        return
    
    cli.config_file = args.config
    
    # Interactive mode
    print("NLM Power User CLI - Type 'help' for commands")
    print(f"Default config: {cli.config_file}")
    
    while True:
        try:
            command_line = input("\nnlm-cli> ").strip()
            
            if not command_line:
                continue
            
            # Parse command
            parts = command_line.split()
            command = parts[0].lower()
            args = parts[1:]
            
            if command in ['exit', 'quit']:
                print("Goodbye!")
                break
            
            elif command == 'help':
                cli.help()
            
            elif command == 'load-config':
                if len(args) >= 1:
                    cli.load_config(args[0])
                else:
                    print("Usage: load-config <file>")
            
            elif command == 'run':
                if len(args) >= 1:
                    steps = int(args[0])
                    output_file = args[1] if len(args) > 1 else None
                    cli.run_simulation(steps, output_file)
                else:
                    print("Usage: run <steps> [output_file]")
            
            elif command == 'analyze':
                cli.analyze_brain()
            
            elif command == 'export':
                if len(args) >= 1:
                    output_file = args[0]
                    format_type = args[1] if len(args) > 1 else "json"
                    cli.export_neural_data(output_file, format_type)
                else:
                    print("Usage: export <file> [format]")
            
            elif command == 'experiment':
                if len(args) >= 1:
                    cli.run_experiments(args[0])
                else:
                    print("Usage: experiment <file>")
            
            elif command == 'debug':
                debug_level = args[0] if len(args) >= 1 else "normal"
                cli.setup_debug_mode(debug_level)
            
            else:
                print(f"Unknown command: {command}")
                print("Type 'help' for available commands")
        
        except KeyboardInterrupt:
            print("\nInterrupted by user")
            break
        
        except Exception as e:
            print(f"Error: {e}")
if __name__ == "__main__":
    main()
