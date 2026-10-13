#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine Simple CLI

A beginner-friendly command-line interface for quickly running NLM simulations
with minimal configuration and maximum ease of use.
"""

import argparse
import json
import logging
import os
import sys
import time
from pathlib import Path

# Add the project root to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the Python bindings.")
    print("Run: pip install -e .")
    sys.exit(1)

# Configure logging for quiet operation by default
logging.basicConfig(
    level=logging.WARNING,
    format='%(message)s'
)
logger = logging.getLogger(__name__)

class SimpleNLMCLI:
    """Simple CLI handler for NLM operations."""
    
    def __init__(self):
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.output_file = "simulation_results.json"
        
    def quick_run(self, steps: int = 100, neurons: int = 500) -> bool:
        """Quick run with minimal configuration."""
        try:
            print(f"Creating NLM brain with {neurons} neurons...")
            
            # Create brain with minimal configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neurons)
            config.set("brain.region_count", 1)
            config.set("random_seed", 42)
            
            self.current_brain = pynlm.createBrain(config)
            self.current_brain.initialize()
            
            print(f"Running simulation for {steps} steps...")
            
            # Create world and agent
            self.current_world = pynlm.createSimpleWorld()
            self.current_world.configure(10, 10, 6, 6)
            self.current_world.reset()
            self.current_world.setAgentStart(5.0, 5.0)
            
            self.current_agent = pynlm.createAgentBrain(self.current_brain)
            self.current_agent.initialize(self.current_world)
            
            # Enable all features for better learning
            self.current_agent.enableRewardModulation(True)
            self.current_agent.enableCuriosity(True)
            self.current_agent.enableDevelopment(True)
            self.current_agent.enableStructuralPlasticity(True)
            
            # Run simulation
            for step in range(steps):
                # Update world
                self.current_world.update(0.1)
                
                # Get percept and process
                percept = self.current_world.getSensoryPercept()
                self.current_agent.processSensoryInput(percept)
                self.current_brain.receiveSensoryInput(percept)
                
                # Step brain
                self.current_brain.step(step)
                
                # Get action and apply
                action = self.current_agent.decodeMotorCommand()
                self.current_world.applyMotorCommand(action, step * 0.1)
                
                # Progress indicator
                if step % 20 == 0:
                    progress = (step / steps) * 100
                    print(f"Progress: {progress:.1f}% - "
                          f"Firing: {self.current_brain.getFiringNeuronCount()}, "
                          f"Spikes: {self.current_brain.getTotalSpikeCount()}")
            
            print("\n=== Simulation Complete ===")
            print(f"Total Steps: {steps}")
            print(f"Firing Neurons: {self.current_brain.getFiringNeuronCount()}")
            print(f"Total Spikes: {self.current_brain.getTotalSpikeCount()}")
            print(f"Average Firing Rate: {self.current_brain.getAverageFiringRate():.2f} Hz")
            print(f"Neuromodulation Level: {self.current_agent.getNeuromodulationLevel():.3f}")
            print(f"Curiosity Level: {self.current_agent.getCuriosityLevel():.3f}")
            print(f"Novelty Level: {self.current_agent.getNoveltyLevel():.3f}")
            print(f"Development Stage: {self.current_agent.getDevelopmentalStage().name}")
            
            # Save results
            self._save_results()
            
            return True
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return False
    
    def quick_demo(self, steps: int = 50, neurons: int = 200) -> bool:
        """Quick demo with fewer steps for demonstration."""
        try:
            print("NLM Quick Demo - Neural Learning Machine")
            print("=" * 50)
            
            # Create brain
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neurons)
            config.set("brain.region_count", 1)
            
            self.current_brain = pynlm.createBrain(config)
            self.current_brain.initialize()
            
            # Create world and agent
            self.current_world = pynlm.createSimpleWorld()
            self.current_world.configure(8, 8, 4, 4)
            self.current_world.reset()
            self.current_world.setAgentStart(4.0, 4.0)
            
            self.current_agent = pynlm.createAgentBrain(self.current_brain)
            self.current_agent.initialize(self.current_world)
            self.current_agent.enableCuriosity(True)
            self.current_agent.enableRewardModulation(True)
            
            # Run demo
            for step in range(steps):
                self.current_world.update(0.05)
                
                percept = self.current_world.getSensoryPercept()
                self.current_agent.processSensoryInput(percept)
                self.current_brain.receiveSensoryInput(percept)
                
                self.current_brain.step(step)
                
                action = self.current_agent.decodeMotorCommand()
                self.current_world.applyMotorCommand(action, step * 0.05)
                
                if step % 10 == 0:
                    print(f"Step {step}: Agent curiosity={self.current_agent.getCuriosityLevel():.2f}, "
                          f"novelty={self.current_agent.getNoveltyLevel():.2f}")
            
            print("\nDemo completed successfully!")
            return True
            
        except Exception as e:
            print(f"Demo error: {e}")
            return False
    
    def _save_results(self):
        """Save simulation results to file."""
        try:
            results = {
                'timestamp': time.strftime("%Y-%m-%d %H:%M:%S"),
                'metadata': {
                    'brain_type': 'NLM - Neural Learning Machine',
                    'version': 'Phase 6.0',
                    'simulation_steps': self.current_brain.getTotalSpikeCount() if self.current_brain else 0
                },
                'statistics': {
                    'neuron_count': self.current_brain.getTotalNeuronCount() if self.current_brain else 0,
                    'synapse_count': self.current_brain.getTotalSynapseCount() if self.current_brain else 0,
                    'firing_count': self.current_brain.getFiringNeuronCount() if self.current_brain else 0,
                    'total_spikes': self.current_brain.getTotalSpikeCount() if self.current_brain else 0,
                    'avg_firing_rate': self.current_brain.getAverageFiringRate() if self.current_brain else 0,
                    'neuromodulation_level': self.current_agent.getNeuromodulationLevel() if self.current_agent else 0,
                    'curiosity_level': self.current_agent.getCuriosityLevel() if self.current_agent else 0,
                    'novelty_level': self.current_agent.getNoveltyLevel() if self.current_agent else 0,
                    'development_stage': self.current_agent.getDevelopmentalStage().name if self.current_agent else 'Unknown'
                }
            }
            
            with open(self.output_file, 'w') as f:
                json.dump(results, f, indent=2)
                
            print(f"Results saved to {self.output_file}")
            
        except Exception as e:
            print(f"Warning: Could not save results: {e}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (熙然) - Neural Learning Machine Simple CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '--quick-demo',
        action='store_true',
        help='Run a quick demo (50 steps, 200 neurons)'
    )
    
    parser.add_argument(
        '--steps', 
        type=int, 
        default=100,
        help='Number of simulation steps (default: 100)'
    )
    
    parser.add_argument(
        '--neurons',
        type=int,
        default=500,
        help='Number of neurons (default: 500)'
    )
    
    parser.add_argument(
        '--output',
        default='simulation_results.json',
        help='Output file for results (default: simulation_results.json)'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    
    # Set up logging level
    if args.verbose:
        logging.basicConfig(level=logging.INFO, format='%(message)s')
    
    cli = SimpleNLMCLI()
    cli.output_file = args.output
    
    if args.quick_demo:
        success = cli.quick_demo(args.steps, args.neurons)
    else:
        success = cli.quick_run(args.steps, args.neurons)
    
    if success:
        print("\n✅ Simulation completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Simulation failed!")
        sys.exit(1)

if __name__ == '__main__':
    main()
