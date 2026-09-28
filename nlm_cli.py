#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

Central command-line interface for NLM Phase 6 operations.
Provides unified access to brain creation, execution, inspection, and management.
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Optional, Dict, Any
import subprocess

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Install with: pip install pynlm")
    sys.exit(1)

class NLMCLI:
    """Main NLM CLI interface."""
    
    def __init__(self):
        self.brain = None
        self.config = None
    
    def create_brain(self, args):
        """Create a new brain with specified configuration."""
        print(f"Creating NLM brain with {args.neurons} neurons...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply custom settings if provided
        if hasattr(args, 'connection_probability'):
            config.set("brain.connection_probability", args.connection_probability)
        if hasattr(args, 'region_count'):
            config.set("brain.region_count", args.region_count)
        if hasattr(args, 'plasticity_enabled'):
            config.set("plasticity.enable", args.plasticity_enabled)
        if hasattr(args, 'neuromod_curiosity'):
            config.set("neuromod.curiosity.enable", args.neurom_curiosity)
        if hasattr(args, 'neuromod_novelty'):
            config.set("neuromod.novelty.enable", args.neurom_novelty)
        if hasattr(args, 'memory_episodic'):
            config.set("memory.episodic.enable", args.memory_episodic)
        if hasattr(args, 'memory_working'):
            config.set("memory.working.enable", args.memory_working)
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.config = config
        
        # Initialize brain
        if not self.brain.initialize():
            print("Error: Failed to initialize brain")
            return False
        
        # Save if requested
        if args.output:
            self.brain.save(args.output)
            print(f"Brain saved to: {args.output}")
        
        print(f"Brain created successfully with {self.brain.getTotalNeuronCount()} neurons")
        return True
    
    def run_brain(self, args):
        """Run the brain simulation."""
        if not self.brain:
            print("Error: No brain loaded. Use 'nlm-cli create' first.")
            return False
        
        print(f"Running brain simulation for {args.steps} steps...")
        
        # Run simulation
        for step in range(args.steps):
            self.brain.step(step)
            
            # Print progress periodically
            if step % 100 == 0:
                print(f"Step {step}/{args.steps}: "
                      f"{self.brain.getFiringNeuronCount()} firing neurons, "
                      f"{self.brain.getAverageFiringRate():.2f} Hz")
        
        print(f"Simulation completed. Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        return True
    
    def run_experiment(self, args):
        """Run a specific experiment."""
        if not self.brain:
            print("Error: No brain loaded. Use 'nlm-cli create' first.")
            return False
        
        experiment_type = args.experiment
        
        if experiment_type == "phase6":
            print("Running Phase 6 integration experiment...")
            # Run Phase 6 integration
            result = self.brain.runPhase6Integration(args.steps, args.neurons)
            
            print("\n=== Experiment Results ===")
            print(f"Total Reward: {result.get('totalReward', 0):.3f}")
            print(f"Avg Firing Rate: {result.get('avgFiringRate', 0):.3f} Hz")
            print(f"Memory Episodes: {result.get('memoryEpisodesStored', 0)}")
            print(f"Integration Status:")
            print(f"  Working Memory: {'✅' if result.get('memoryWorkingMemoryIntegrated', False) else '❌'}")
            print(f"  Episodic Memory: {'✅' if result.get('memoryEpisodicMemoryIntegrated', False) else '❌'}")
            print(f"  Neuromodulation: {'✅' if result.get('neuromodulationIntegrated', False) else '❌'}")
            print(f"  Prediction: {'✅' if result.get('predictionIntegrated', False) else '❌'}")
            print(f"  Development: {'✅' if result.get('developmentIntegrated', False) else '❌'}")
            
            return True
        
        print(f"Unknown experiment type: {experiment_type}")
        return False
    
    def inspect_brain(self, args):
        """Inspect brain state and components."""
        if not self.brain:
            print("Error: No brain loaded. Use 'nlm-cli create' first.")
            return False
        
        print("=== Brain Inspection ===")
        
        # Basic statistics
        print(f"\nBasic Statistics:")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Average Firing Rate: {self.brain.getAverageFiringRate():.3f} Hz")
        print(f"  Total Spike Count: {self.brain.getTotalSpikeCount()}")
        print(f"  Developmental Stage: {self.brain.getDevelopmentalStage()}")
        
        # Memory systems
        if args.memory:
            print(f"\nMemory Systems:")
            working_memory = self.brain.getWorkingMemory()
            episodic_memory = self.brain.getEpisodicMemory()
            
            if working_memory:
                print(f"  Working Memory: Active traces = {working_memory.getActiveTraces()}")
            else:
                print(f"  Working Memory: Not available")
            
            if episodic_memory:
                print(f"  Episodic Memory: Episodes = {episodic_memory.getEpisodeCount()}")
            else:
                print(f"  Episodic Memory: Not available")
        
        # Neuromodulation systems
        if args.neuromodulation:
            print(f"\nNeuromodulation Systems:")
            dopamine = self.brain.getDopamine()
            curiosity = self.brain.getCuriosity()
            novelty = self.brain.getNovelty()
            
            if dopamine:
                print(f"  Dopamine: Available")
            else:
                print(f"  Dopamine: Not available")
            
            if curiosity:
                print(f"  Curiosity: Available")
            else:
                print(f"  Curiosity: Not available")
            
            if novelty:
                print(f"  Novelty: Available")
            else:
                print(f"  Novelty: Not available")
        
        # Cognition systems
        if args.cognition:
            print(f"\nCognition Systems:")
            planner = self.brain.getPlanner()
            concept_formation = self.brain.getConceptFormation()
            attention = self.brain.getAttention()
            
            if planner:
                print(f"  Planner: Available")
            else:
                print(f"  Planner: Not available")
            
            if concept_formation:
                print(f"  Concept Formation: Available")
            else:
                print(f"  Concept Formation: Not available")
            
            if attention:
                print(f"  Attention: Available")
            else:
                print(f"  Attention: Not available")
        
        # Prediction system
        if args.prediction:
            print(f"\nPrediction System:")
            prediction_system = self.brain.getPredictionSystem()
            
            if prediction_system:
                print(f"  Prediction System: Available")
            else:
                print(f"  Prediction System: Not available")
        
        # Development system
        if args.development:
            print(f"\nDevelopment System:")
            development_system = self.brain.getDevelopmentSystem()
            
            if development_system:
                print(f"  Development System: Available")
                print(f"    Current Stage: {self.brain.getDevelopmentalStage()}")
            else:
                print(f"  Development System: Not available")
        
        return True
    
    def checkpoint_brain(self, args):
        """Save or load brain checkpoint."""
        if not self.brain:
            print("Error: No brain loaded.")
            return False
        
        if args.save and args.output:
            print(f"Saving brain checkpoint to: {args.output}")
            if self.brain.save(args.output):
                print("Checkpoint saved successfully")
                return True
            else:
                print("Error: Failed to save checkpoint")
                return False
        
        elif args.load and args.input:
            print(f"Loading brain checkpoint from: {args.input}")
            
            # Create new brain to load into
            temp_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            if temp_brain.load(args.input):
                self.brain = temp_brain
                print("Checkpoint loaded successfully")
                return True
            else:
                print("Error: Failed to load checkpoint")
                return False
        
        print("Error: Specify --save --output or --load --input")
        return False
    
    def list_sessions(self, args):
        """List saved brain sessions."""
        print("=== Saved Brain Sessions ===")
        
        # Look for brain files in current directory
        brain_files = list(Path(".").glob("*.bin"))
        
        if not brain_files:
            print("No saved brain files found.")
            return True
        
        for bf in brain_files:
            size = bf.stat().st_size
            print(f"  {bf.name} ({size:,} bytes)")
        
        return True

def main():
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface for Phase 6 Operations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli create --neurons 1000 --output brain.bin
  nlm-cli run --steps 1000 brain.bin
  nlm-cli inspect brain.bin --memory --neuromodulation
  nlm-cli checkpoint --brain brain.bin --save checkpoint.bin
  nlm-cli experiment phase6 --steps 2000
        """
    )
    
    # Create subparsers
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    parser_create = subparsers.add_parser("create", help="Create a new brain")
    parser_create.add_argument("--neurons", type=int, default=1000, help="Number of neurons (default: 1000)")
    parser_create.add_argument("--connection-probability", type=float, default=0.1, help="Connection probability (default: 0.1)")
    parser_create.add_argument("--region-count", type=int, default=1, help="Number of neural regions (default: 1)")
    parser_create.add_argument("--plasticity-enabled", action="store_true", help="Enable plasticity")
    parser_create.add_argument("--neuromod-curiosity", action="store_true", help="Enable curiosity")
    parser_create.add_argument("--neuromod-novelty", action="store_true", help="Enable novelty detection")
    parser_create.add_argument("--memory-episodic", action="store_true", help="Enable episodic memory")
    parser_create.add_argument("--memory-working", action="store_true", help="Enable working memory")
    parser_create.add_argument("--output", help="Save brain to file")
    
    # Run command
    parser_run = subparsers.add_parser("run", help="Run brain simulation")
    parser_run.add_argument("brain", help="Brain file to run")
    parser_run.add_argument("--steps", type=int, default=1000, help="Number of simulation steps (default: 1000)")
    
    # Inspect command
    parser_inspect = subparsers.add_parser("inspect", help="Inspect brain state")
    parser_inspect.add_argument("brain", help="Brain file to inspect")
    parser_inspect.add_argument("--memory", action="store_true", help="Inspect memory systems")
    parser_inspect.add_argument("--neuromodulation", action="store_true", help="Inspect neuromodulation systems")
    parser_inspect.add_argument("--cognition", action="store_true", help="Inspect cognition systems")
    parser_inspect.add_argument("--prediction", action="store_true", help="Inspect prediction system")
    parser_inspect.add_argument("--development", action="store_true", help="Inspect development system")
    
    # Checkpoint command
    parser_checkpoint = subparsers.add_parser("checkpoint", help="Save/load brain checkpoints")
    parser_checkpoint.add_argument("--brain", help="Brain file to checkpoint")
    parser_checkpoint.add_argument("--save", action="store_true", help="Save checkpoint")
    parser_checkpoint.add_argument("--load", action="store_true", help="Load checkpoint")
    parser_checkpoint.add_argument("--output", help="Checkpoint output file")
    parser_checkpoint.add_argument("--input", help="Checkpoint input file")
    
    # Experiment command
    parser_experiment = subparsers.add_parser("experiment", help="Run specific experiment")
    parser_experiment.add_argument("experiment", help="Experiment type (phase6)")
    parser_experiment.add_argument("--steps", type=int, default=2000, help="Number of simulation steps (default: 2000)")
    parser_experiment.add_argument("--neurons", type=int, default=500, help="Number of neurons (default: 500)")
    
    # List sessions command
    parser_list = subparsers.add_parser("list", help="List saved sessions")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Execute command
    if args.command == "create":
        success = cli.create_brain(args)
    elif args.command == "run":
        cli.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        cli.brain.load(args.brain)
        success = cli.run_brain(args)
    elif args.command == "inspect":
        cli.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        cli.brain.load(args.brain)
        success = cli.inspect_brain(args)
    elif args.command == "checkpoint":
        cli.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        cli.brain.load(args.brain)
        success = cli.checkpoint_brain(args)
    elif args.command == "experiment":
        cli.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        cli.brain.load(args.brain)
        success = cli.run_experiment(args)
    elif args.command == "list":
        success = cli.list_sessions(args)
    else:
        parser.print_help()
        success = False
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()