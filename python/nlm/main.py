#!/usr/bin/env python3

"""
NLM (Neural Learning Machine) Command Line Interface

This CLI provides enhanced access to the NLM neural simulation framework,
including the enhanced Python bindings with integrated memory systems,
prediction, and cognitive capabilities.
"""

import argparse
import sys
import json
import time
from pathlib import Path
from typing import Optional, Dict, Any

# Add the current directory to Python path for local imports
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm_enhanced as nlm
    from pynlm_enhanced import create_brain_default, create_brain_simulation
    ENHANCED_AVAILABLE = True
except ImportError:
    print("Warning: Enhanced NLM bindings not available. Using basic bindings.")
    import pynlm as nlm
    from pynlm import create_brain_default, create_brain_simulation
    ENHANCED_AVAILABLE = False
class NLMBrain:
    """High-level brain manager for NLM simulations."""
    
    def __init__(self, config_file: Optional[str] = None, steps: int = 100):
        self.config_file = config_file
        self.steps = steps
        self.brain = None
        self.simulation = None
        
    def initialize(self) -> bool:
        """Initialize the NLM brain with enhanced bindings."""
        try:
            if ENHANCED_AVAILABLE:
                # Use enhanced simulation wrapper
                self.simulation = create_brain_simulation(self.steps, self.config_file)
                self.brain = self.simulation.brain_
                print("Enhanced NLM brain initialized with integrated systems.")
            else:
                # Fallback to basic initialization
                self.brain = create_brain_default()
                self.simulation = create_brain_simulation(self.steps, self.config_file)
                self.simulation.brain_ = self.brain
                print("Basic NLM brain initialized.")
            
            return True
        except Exception as e:
            print(f"Error initializing brain: {e}")
            return False
    
    def run(self) -> Dict[str, Any]:
        """Run the brain simulation."""
        if not self.brain:
            print("Brain not initialized. Call initialize() first.")
            return {}
        
        print(f"Running simulation for {self.steps} steps...")
        
        # Run simulation
        self.simulation.run()
        
        # Collect statistics
        stats = self.simulation.getStats()
        memory_stats = self.simulation.getMemoryStats()
        prediction_stats = self.simulation.getPredictionStats()
        cognitive_stats = self.simulation.getCognitiveStats()
        neuromod_stats = self.simulation.getNeuromodulationStats()
        dev_stats = self.simulation.getDevelopmentStats()
        
        results = {
            "statistics": stats,
            "memory": memory_stats,
            "prediction": prediction_stats,
            "cognitive": cognitive_stats,
            "neuromodulation": neuromod_stats,
            "development": dev_stats,
            "learning": self.simulation.runLearningExperiment()
        }
        
        print(f"Simulation completed. Step: {stats.get('step', 'unknown')}")
        return results
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        if not self.simulation:
            print("Simulation not initialized.")
            return False
        
        try:
            success = self.simulation.save(filepath)
            if success:
                print(f"Brain state saved to {filepath}")
            else:
                print(f"Failed to save brain state to {filepath}")
            return success
        except Exception as e:
            print(f"Error saving brain: {e}")
            return False
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        if not self.simulation:
            print("Simulation not initialized.")
            return False
        
        try:
            success = self.simulation.load(filepath)
            if success:
                print(f"Brain state loaded from {filepath}")
            else:
                print(f"Failed to load brain state from {filepath}")
            return success
        except Exception as e:
            print(f"Error loading brain: {e}")
            return False
    
    def get_stats_report(self) -> str:
        """Generate a detailed statistics report."""
        if not self.simulation:
            return "Simulation not initialized."
        
        stats = self.simulation.getStats()
        memory_stats = self.simulation.getMemoryStats()
        prediction_stats = self.simulation.getPredictionStats()
        cognitive_stats = self.simulation.getCognitiveStats()
        neuromod_stats = self.simulation.getNeuromodulationStats()
        dev_stats = self.simulation.getDevelopmentStats()
        
        report = f"NLM Brain Simulation Report\n"
        report += f"{'='*50}\n"
        report += f"Step: {stats.get('step', 'N/A')}\n"
        report += f"Total Neurons: {stats.get('total_neurons', 'N/A')}\n"
        report += f"Total Synapses: {stats.get('total_synapses', 'N/A')}\n"
        report += f"Total Spikes: {stats.get('total_spikes', 'N/A')}\n"
        report += f"Active Neurons: {stats.get('active_neurons', 'N/A')}\n"
        report += f"Firing Neurons: {stats.get('firing_neurons', 'N/A')}\n"
        report += f"Excitatory/Inhibitory Ratio: {stats.get('exc_inhibition_ratio', 'N/A')}\n"
        report += f"Average Firing Rate: {stats.get('avg_firing_rate', 'N/A')}\n"
        report += f"\n"
        report += f"Development Stage: {dev_stats.get('stage', 'N/A')}\n"
        report += f"\n"
        
        if memory_stats:
            report += f"Memory Systems:\n"
            report += f"  Working Memory: {memory_stats.get('working_memory_size', 'N/A')}/"
            report += f"{memory_stats.get('working_memory_capacity', 'N/A')}\n"
            report += f"  Episodic Memory: {memory_stats.get('episodic_memory_count', 'N/A')} episodes\n"
            if memory_stats.get('has_associative_memory', False):
                report += f"  Associative Memory: Available\n"
        
        if prediction_stats:
            report += f"Prediction System: Available\n"
        
        if cognitive_stats:
            report += f"Cognitive Systems:\n"
            if cognitive_stats.get('has_planner', False):
                report += f"  Neural Planner: Available\n"
            if cognitive_stats.get('has_concept_formation', False):
                report += f"  Concept Formation: Available\n"
            if cognitive_stats.get('has_attention', False):
                report += f"  Attention System: Available\n"
        
        if neuromod_stats:
            report += f"Neuromodulation:\n"
            if neuromod_stats.get('has_dopamine', False):
                report += f"  Dopamine: Available\n"
            if neuromod_stats.get('has_curiosity', False):
                report += f"  Curiosity Level: {neuromod_stats.get('curiosity_level', 'N/A')}\n"
            if neuromod_stats.get('has_prediction_error', False):
                report += f"  Prediction Error: Available\n"
        
        return report

def interactive_mode(args):
    """Run in interactive mode."""
    print("NLM Interactive Mode")
    print("===================")
    
    brain_manager = NLMBrain(args.config, args.steps)
    
    if not brain_manager.initialize():
        print("Failed to initialize NLM brain.")
        return 1
    
    print("\nCommands:")
    print("  run [steps]    - Run simulation")
    print("  stats          - Show current statistics")
    print("  learn          - Run learning experiment")
    print("  save <file>    - Save brain state")
    print("  load <file>    - Load brain state")
    print("  help           - Show this help")
    print("  exit           - Exit interactive mode")
    
    while True:
        try:
            cmd = input("\n> ").strip()
            if not cmd:
                continue
            
            parts = cmd.split()
            command = parts[0].lower()
            
            if command == "exit":
                break
            elif command == "help":
                print("\nCommands:")
                print("  run [steps]    - Run simulation")
                print("  stats          - Show current statistics")
                print("  learn          - Run learning experiment")
                print("  save <file>    - Save brain state")
                print("  load <file>    - Load brain state")
                print("  help           - Show this help")
                print("  exit           - Exit interactive mode")
            
            elif command == "stats":
                print("\n" + brain_manager.get_stats_report())
            
            elif command == "learn":
                print("\nRunning learning experiment...")
                if args.enhanced and ENHANCED_AVAILABLE:
                    # Use enhanced learning
                    learning_results = brain_manager.simulation.runLearningExperiment()
                    print(f"Learning Factor: {learning_results.get('learning_factor', 'N/A')}")
                    print(f"Learning Detected: {learning_results.get('learning_detected', 'N/A')}")
                else:
                    print("Basic learning experiment completed.")
            
            elif command == "save":
                if len(parts) < 2:
                    print("Usage: save <filepath>")
                else:
                    brain_manager.save(parts[1])
            
            elif command == "load":
                if len(parts) < 2:
                    print("Usage: load <filepath>")
                else:
                    brain_manager.load(parts[1])
            
            elif command == "run":
                steps = args.steps
                if len(parts) > 1:
                    try:
                        steps = int(parts[1])
                        brain_manager.steps = steps
                    except ValueError:
                        print("Invalid number of steps.")
                
                results = brain_manager.run()
                print("\n" + brain_manager.get_stats_report())
            
            else:
                print(f"Unknown command: {command}")
                print("Type 'help' for available commands.")
                
        except KeyboardInterrupt:
            print("\nInterrupted by user.")
            break
        except Exception as e:
            print(f"Error: {e}")
    
    print("Exiting NLM interactive mode.")
    return 0
def run_batch_mode(args):
    """Run in batch mode (non-interactive)."""
    brain_manager = NLMBrain(args.config, args.steps)
    
    if not brain_manager.initialize():
        print("Failed to initialize NLM brain.")
        return 1
    
    print(f"Running batch simulation for {args.steps} steps...")
    start_time = time.time()
    
    results = brain_manager.run()
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds.")
    
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")
    else:
        # Print summary
        stats = results.get("statistics", {})
        print(f"\nSummary:")
        print(f"  Total Spikes: {stats.get('total_spikes', 'N/A')}")
        print(f"  Final Step: {stats.get('step', 'N/A')}")
        
        learning = results.get("learning", {})
        if learning:
            print(f"  Learning Factor: {learning.get('learning_factor', 'N/A')}")
            print(f"  Learning Detected: {learning.get('learning_detected', 'N/A')}")

def demo_mode(args):
    """Run demonstration mode."""
    print("NLM Enhanced Demo")
    print("=================")
    
    brain_manager = NLMBrain(None, args.steps)
    
    if not brain_manager.initialize():
        print("Failed to initialize NLM brain.")
        return 1
    
    print(f"\nRunning enhanced demo simulation...")
    print(f"This demo shows integration of memory systems, prediction,")
    print(f"and cognitive functions in NLM.")
    
    results = brain_manager.run()
    
    print(f"\nDemo completed successfully!")
    print(f"Key features demonstrated:")
    
    memory_stats = results.get("memory", {})
    if memory_stats:
        print(f"  ✓ Memory Systems: Working ({memory_stats.get('working_memory_size', 'N/A')}/"
              f"{memory_stats.get('working_memory_capacity', 'N/A')})")
    
    cognitive_stats = results.get("cognitive", {})
    if cognitive_stats:
        systems = []
        if cognitive_stats.get('has_planner', False):
            systems.append("Neural Planner")
        if cognitive_stats.get('has_concept_formation', False):
            systems.append("Concept Formation")
        if cognitive_stats.get('has_attention', False):
            systems.append("Attention System")
        if systems:
            print(f"  ✓ Cognitive Systems: {', '.join(systems)}")
    
    neuromod_stats = results.get("neuromodulation", {})
    if neuromod_stats:
        systems = []
        if neuromod_stats.get('has_dopamine', False):
            systems.append("Dopamine")
        if neuromod_stats.get('has_curiosity', False):
            systems.append(f"Curiosity ({neuromod_stats.get('curiosity_level', 'N/A')})")
        if neuromod_stats.get('has_prediction_error', False):
            systems.append("Prediction Error")
        if systems:
            print(f"  ✓ Neuromodulation: {', '.join(systems)}")
    
    learning = results.get("learning", {})
    if learning:
        print(f"  ✓ Learning: Factor {learning.get('learning_factor', 'N/A')}")
    
    return 0
def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Enhanced CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm run --steps 100                    Run simulation with 100 steps
  nlm demo --steps 50                    Run demonstration
  nlm interactive --config myconfig.cfg   Start interactive mode
  nlm batch --steps 200 --output results.json  Run and save results

The enhanced version provides integrated memory systems, prediction,
and cognitive functions. Use --enhanced flag for enhanced features.
        ""
    )
    
    parser.add_argument(
        "mode",
        choices=["run", "demo", "interactive", "batch"],
        help="Operation mode"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps (default: 100)"
    )
    
    parser.add_argument(
        "--config", "-c",
        type=str,
        help="Configuration file path"
    )
    
    parser.add_argument(
        "--output", "-o",
        type=str,
        help="Output file for batch mode results (JSON)"
    )
    
    parser.add_argument(
        "--enhanced",
        action="store_true",
        help="Use enhanced NLM bindings with integrated systems"
    )
    
    parser.add_argument(
        "--quiet",
        action="store_true",
        help="Suppress status messages"
    )
    
    args = parser.parse_args()
    
    if not args.quiet:
        print(f"NLM Enhanced CLI - Mode: {args.mode}")
        if args.config:
            print(f"Config: {args.config}")
        print(f"Steps: {args.steps}")
        if args.enhanced:
            print("Enhanced bindings: Enabled")
        print()
    
    try:
        if args.mode == "interactive":
            return interactive_mode(args)
        elif args.mode == "batch":
            return run_batch_mode(args)
        elif args.mode == "demo":
            return demo_mode(args)
        else:  # run mode
            brain_manager = NLMBrain(args.config, args.steps)
            if not brain_manager.initialize():
                return 1
            return run_batch_mode(args)
    except KeyboardInterrupt:
        print("\nInterrupted by user.")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1
if __name__ == "__main__":
    sys.exit(main())
