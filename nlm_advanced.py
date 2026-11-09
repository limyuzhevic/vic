#!/usr/bin/env python3
"""
Advanced NLM Command Line Tool

This tool provides advanced functionality for NLM (Neural Learning Machine) users,
including configuration management, simulation control, and research features.
"""

import sys
import os
import json
import argparse
import subprocess
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add the nlm module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install the Python bindings.")
    sys.exit(1)

class NLMCommandLine:
    """Main command line interface for NLM."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.world = None
        self.agent = None
        
    def create_brain(self, config_file: Optional[str] = None, neuron_count: int = 1000, 
                     synapse_density: float = 0.1, debug: bool = False) -> None:
        """Create a brain with specified configuration."""
        print(f"Creating brain with {neuron_count} neurons and synapse density {synapse_density}...")
        
        # Create configuration
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.synapse_density", synapse_density)
        
        # Load advanced configuration if provided
        if config_file and Path(config_file).exists():
            self.load_advanced_config(config_file)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"Brain created successfully!")
        print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  - E/I ratio: {self.brain.getExcitationInhibitionRatio():.2f}")
        
    def load_advanced_config(self, config_file: str) -> None:
        """Load advanced configuration from file."""
        print(f"Loading advanced configuration from {config_file}...")
        
        # For now, just print a message - in a real implementation,
        # this would parse the advanced.cfg file and apply settings
        try:
            with open(config_file, 'r') as f:
                content = f.read()
                if 'advanced_brain' in content:
                    print("  ✓ Advanced brain configuration detected")
                if 'advanced_plasticity' in content:
                    print("  ✓ Advanced plasticity configuration detected")
                if 'advanced_neuromodulation' in content:
                    print("  ✓ Advanced neuromodulation configuration detected")
                if 'research_profile' in content:
                    print("  ✓ Research profile detected")
        except Exception as e:
            print(f"  Warning: Could not parse advanced config: {e}")
        
    def run_simulation(self, steps: int, timestep: float = 0.1, 
                      enable_learning: bool = True, debug_output: bool = False) -> None:
        """Run a simulation with specified parameters."""
        print(f"Running simulation for {steps} steps with timestep {timestep}...")
        
        if not self.brain:
            print("Error: Brain not created. Use --create-brain first.")
            return
        
        # Track statistics
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
            
            if debug_output and step % 100 == 0:
                print(f"  Step {step}: {self.brain.getFiringNeuronCount()} firing neurons, "
                      f"{self.brain.getTotalSpikeCount() - initial_spikes} spikes total")
        
        # Calculate statistics
        final_spikes = self.brain.getTotalSpikeCount()
        final_firing = self.brain.getFiringNeuronCount()
        total_spikes = final_spikes - initial_spikes
        
        print(f"Simulation complete!")
        print(f"  - Total spikes: {total_spikes}")
        print(f"  - Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"  - Final firing neurons: {final_firing}")
        
    def create_research_world(self, width: int = 20, height: int = 20,
                             vision_width: int = 8, vision_height: int = 8,
                             enable_rl: bool = True, enable_curiosity: bool = True) -> None:
        """Create a world for research with RL and curiosity."""
        print(f"Creating research world ({width}x{height}) with vision ({vision_width}x{vision_height})...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable advanced features
        if enable_rl:
            self.agent.enableRewardModulation(True)
            print("  ✓ Reward modulation enabled")
        
        if enable_curiosity:
            self.agent.enableCuriosity(True)
            print("  ✓ Curiosity enabled")
        
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        print(f"Research world created successfully!")
        print(f"  - World size: {width}x{height}")
        print(f"  - Vision range: {vision_width}x{vision_height}")
        
    def run_research_episode(self, steps: int, timestep: float = 0.1,
                           render: bool = False) -> None:
        """Run a research episode in the world."""
        print(f"Running research episode for {steps} steps...")
        
        if not self.world or not self.agent:
            print("Error: World or agent not created. Use --create-research-world first.")
            return
        
        # Track statistics
        initial_curiosity = self.agent.getCuriosityLevel() if self.agent else 0.0
        initial_novelty = self.agent.getNoveltyLevel() if self.agent else 0.0
        initial_dev_stage = self.brain.getDevelopmentalStage() if self.brain else 0
        
        for step in range(steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.get_reward_from_world(perp)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            if render and step % 50 == 0:
                self.render_world(step)
            
            if step % 100 == 0:
                self.print_episode_stats(step)
        
        # Print episode statistics
        print(f"Episode complete!")
        if self.agent:
            print(f"  - Final curiosity: {self.agent.getCuriosityLevel():.3f}")
            print(f"  - Final novelty: {self.agent.getNoveltyLevel():.3f}")
            print(f"  - Final development stage: {self.brain.getDevelopmentalStage()}")
        
    def get_reward_from_world(self, percept) -> float:
        """Extract reward from world percept."""
        # This is a placeholder - in a real implementation,
        # this would extract reward from the world state
        return 0.0
        
    def render_world(self, step: int) -> None:
        """Render the world state."""
        if not self.world:
            return
        
        # This is a placeholder - in a real implementation,
        # this would render the world state
        pass
        
    def print_episode_stats(self, step: int) -> None:
        """Print episode statistics."""
        if not self.brain or not self.agent:
            return
        
        print(f"  Step {step}:")
        print(f"    - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"    - Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"    - Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"    - Dev stage: {self.brain.getDevelopmentalStage()}")
        
    def save_research_data(self, filepath: str) -> None:
        """Save research data to file."""
        print(f"Saving research data to {filepath}...")
        
        data = {
            'brain_stats': {
                'total_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
                'total_synapses': self.brain.getTotalSynapseCount() if self.brain else 0,
                'total_spikes': self.brain.getTotalSpikeCount() if self.brain else 0,
                'firing_rate': self.brain.getAverageFiringRate() if self.brain else 0.0,
                'ei_ratio': self.brain.getExcitationInhibitionRatio() if self.brain else 0.0,
                'dev_stage': self.brain.getDevelopmentalStage() if self.brain else 0
            },
            'agent_stats': {
                'curiosity': self.agent.getCuriosityLevel() if self.agent else 0.0,
                'novelty': self.agent.getNoveltyLevel() if self.agent else 0.0,
                'neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
                'prediction_error': self.agent.getPredictionError() if self.agent else 0.0
            },
            'world_stats': {
                'width': self.world.getWidth() if self.world else 0,
                'height': self.world.getHeight() if self.world else 0,
                'time': self.world.getSimulationTime() if self.world else 0.0
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Research data saved successfully!")
        
    def run_advanced_stdp_test(self, duration: int = 1000) -> None:
        """Run an advanced STDP test with multiple spike patterns."""
        print(f"Running advanced STDP test for {duration} steps...")
        
        if not self.brain:
            print("Error: Brain not created. Use --create-brain first.")
            return
        
        # Get first region
        region = self.brain.getRegion(0)
        if not region:
            print("Error: No brain regions found.")
            return
        
        # Enable STDP on all synapses
        for syn in region.getSynapses():
            syn.enablePlasticity(false, true, false)  # STDP only
        
        # Record initial weights
        initial_weights = []
        for syn in region.getSynapses():
            initial_weights.append(syn.getWeight())
        
        # Apply complex spike patterns
        for step in range(duration):
            # Pattern 1: Regular pre-before-post (LTP)
            if step % 4 == 0:
                neuron_ids = [n.getId() for n in region.getAllNeurons()[:10]]
                for i in range(0, len(neuron_ids), 2):
                    if i + 1 < len(neuron_ids):
                        self.brain.injectCurrent(neuron_ids[i], 60.0f)
                        self.brain.step(step, step * 0.001)
                        self.brain.injectCurrent(neuron_ids[i+1], 60.0f)
                        self.brain.step(step + 1, (step + 1) * 0.001)
            
            # Pattern 2: Anti-correlated (LTD)
            elif step % 4 == 1:
                neuron_ids = [n.getId() for n in region.getAllNeurons()[:10]]
                for i in range(0, len(neuron_ids), 2):
                    if i + 1 < len(neuron_ids):
                        self.brain.injectCurrent(neuron_ids[i+1], 60.0f)
                        self.brain.step(step, step * 0.001)
                        self.brain.injectCurrent(neuron_ids[i], 60.0f)
                        self.brain.step(step + 1, (step + 1) * 0.001)
            
            # Pattern 3: Random activity
            else:
                neuron_ids = [n.getId() for n in region.getAllNeurons()[:5]]
                import random
                for neuron_id in neuron_ids:
                    if random.random() < 0.3:  # 30% chance to fire
                        self.brain.injectCurrent(neuron_id, 30.0f)
                        self.brain.step(step, step * 0.001)
            
            if step % 200 == 0:
                print(f"  Step {step}: spikes = {self.brain.getTotalSpikeCount()}")
        
        # Analyze results
        final_weights = []
        for syn in region.getSynapses():
            final_weights.append(syn.getWeight())
        
        # Calculate statistics
        strengthen_count = 0
        weaken_count = 0
        unchanged_count = 0
        
        min_size = min(len(initial_weights), len(final_weights))
        for i in range(min_size):
            delta = final_weights[i] - initial_weights[i]
            if delta > 0.01:
                strengthen_count += 1
            elif delta < -0.01:
                weaken_count += 1
            else:
                unchanged_count += 1
        
        print(f"Advanced STDP test complete!")
        print(f"  - Strengthened synapses: {strengthen_count}")
        print(f"  - Weakened synapses: {weaken_count}")
        print(f"  - Unchanged synapses: {unchanged_count}")
        print(f"  - Average weight change: {(sum(final_weights) - sum(initial_weights)) / min_size:.4f}")
        
    def create_research_profile(self, profile_type: str = "research") -> None:
        """Create a research profile with specific settings."""
        print(f"Creating {profile_type} profile...")
        
        profiles = {
            'standard': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'enable_advanced_plasticity': False,
                'enable_advanced_neuromodulation': False,
                'enable_large_scale': False,
                'enable_vectorization': False
            },
            'research': {
                'neuron_count': 10000,
                'synapse_density': 0.05,
                'enable_advanced_plasticity': True,
                'enable_advanced_neuromodulation': True,
                'enable_large_scale': True,
                'enable_vectorization': False
            },
            'production': {
                'neuron_count': 100,
                'synapse_density': 0.2,
                'enable_advanced_plasticity': False,
                'enable_advanced_neuromodulation': False,
                'enable_large_scale': False,
                'enable_vectorization': True
            },
            'debug': {
                'neuron_count': 100,
                'synapse_density': 0.1,
                'enable_advanced_plasticity': False,
                'enable_advanced_neuromodulation': False,
                'enable_large_scale': False,
                'enable_vectorization': False
            }
        }
        
        if profile_type not in profiles:
            print(f"Error: Unknown profile '{profile_type}'. Available: {list(profiles.keys())}")
            return
        
        profile = profiles[profile_type]
        
        # Apply profile settings
        self.config.set("brain.neuron_count", profile['neuron_count'])
        self.config.set("brain.synapse_density", profile['synapse_density'])
        
        print(f"  ✓ Applied {profile_type} profile settings")
        print(f"    - Neuron count: {profile['neuron_count']}")
        print(f"    - Synapse density: {profile['synapse_density']}")
        print(f"    - Advanced plasticity: {'Enabled' if profile['enable_advanced_plasticity'] else 'Disabled'}")
        print(f"    - Advanced neuromodulation: {'Enabled' if profile['enable_advanced_neuromodulation'] else 'Disabled'}")
        print(f"    - Large scale: {'Enabled' if profile['enable_large_scale'] else 'Disabled'}")
        print(f"    - Vectorization: {'Enabled' if profile['enable_vectorization'] else 'Disabled'}")
        
        # Recreate brain with new settings
        self.create_brain(neuron_count=profile['neuron_count'], 
                         synapse_density=profile['synapse_density'])
        
    def run(self, args) -> None:
        """Run the command line tool with parsed arguments."""
        if args.command == 'create-brain':
            self.create_brain(args.config_file, args.neuron_count, args.synapse_density, args.debug)
        elif args.command == 'run-simulation':
            self.run_simulation(args.steps, args.timestep, args.learning, args.debug_output)
        elif args.command == 'create-research-world':
            self.create_research_world(args.width, args.height, args.vision_width, 
                                     args.vision_height, args.enable_rl, args.enable_curiosity)
        elif args.command == 'run-episode':
            self.run_research_episode(args.steps, args.timestep, args.render)
        elif args.command == 'save-data':
            self.save_research_data(args.filepath)
        elif args.command == 'run-stdp-test':
            self.run_advanced_stdp_test(args.duration)
        elif args.command == 'create-profile':
            self.create_research_profile(args.profile)
        elif args.command == 'help':
            self.print_help()
        else:
            print(f"Unknown command: {args.command}")
            self.print_help()
            
    def print_help(self) -> None:
        """Print help information."""
        help_text = """
Advanced NLM Command Line Tool

Commands:

  create-brain [options]
    Create a brain with specified configuration.
    
    Options:
      --config-file FILE      Load advanced configuration from file
      --neuron-count N        Number of neurons (default: 1000)
      --synapse-density D     Synapse density (default: 0.1)
      --debug                 Enable debug output

  run-simulation [options]
    Run a simulation with specified parameters.
    
    Options:
      --steps N               Number of simulation steps (default: 1000)
      --timestep T            Simulation timestep (default: 0.1)
      --no-learning          Disable learning
      --debug-output         Enable debug output during simulation

  create-research-world [options]
    Create a world for research with RL and curiosity.
    
    Options:
      --width W              World width (default: 20)
      --height H             World height (default: 20)
      --vision-width VW      Vision width (default: 8)
      --vision-height VH     Vision height (default: 8)
      --no-rl                Disable reward learning
      --no-curiosity         Disable curiosity

  run-episode [options]
    Run a research episode in the world.
    
    Options:
      --steps N               Number of steps (default: 1000)
      --timestep T            Timestep (default: 0.1)
      --render               Enable rendering

  save-data FILE
    Save research data to file.

  run-stdp-test [options]
    Run an advanced STDP test with multiple spike patterns.
    
    Options:
      --duration N            Test duration (default: 1000)

  create-profile PROFILE
    Create a research profile with specific settings.
    
    Profiles: standard, research, production, debug

  help
    Show this help message.

Examples:

  # Create a standard research brain
  nlm-advanced create-brain --neuron-count 5000

  # Run a simulation
  nlm-advanced run-simulation --steps 5000 --timestep 0.05

  # Create a research world and run an episode
  nlm-advanced create-research-world
  nlm-advanced run-episode --steps 2000 --render

  # Run advanced STDP analysis
  nlm-advanced create-brain --neuron-count 1000
  nlm-advanced run-stdp-test

  # Use a research profile
  nlm-advanced create-brad --neuron-count 1000

  # Save research data
  nlm-advanced create-brad --neuron-count 1000
  nlm-advanced run-simulation --steps 1000
  nlm-advanced save-data research_data.json
"""
        print(help_text)

def main():
    parser = argparse.ArgumentParser(description="Advanced NLM Command Line Tool")
    parser.add_argument('command', help="Command to execute")
    
    # Global options
    parser.add_argument('--config-file', help="Advanced configuration file")
    parser.add_argument('--debug', action='store_true', help="Enable debug output")
    
    # Create-brain options
    parser.add_argument('--neuron-count', type=int, default=1000, help="Number of neurons")
    parser.add_argument('--synapse-density', type=float, default=0.1, help="Synapse density")
    
    # Run-simulation options
    parser.add_argument('--steps', type=int, default=1000, help="Number of simulation steps")
    parser.add_argument('--timestep', type=float, default=0.1, help="Simulation timestep")
    parser.add_argument('--no-learning', dest='learning', action='store_false', help="Disable learning")
    parser.add_argument('--debug-output', action='store_true', help="Enable debug output")
    
    # Create-research-world options
    parser.add_argument('--width', type=int, default=20, help="World width")
    parser.add_argument('--height', type=int, default=20, help="World height")
    parser.add_argument('--vision-width', type=int, default=8, help="Vision width")
    parser.add_argument('--vision-height', type=int, default=8, help="Vision height")
    parser.add_argument('--no-rl', dest='enable_rl', action='store_false', help="Disable reward learning")
    parser.add_argument('--no-curiosity', dest='enable_curiosity', action='store_false', help="Disable curiosity")
    
    # Run-episode options
    parser.add_argument('--render', action='store_true', help="Enable rendering")
    
    # Run-stdp-test options
    parser.add_argument('--duration', type=int, default=1000, help="Test duration")
    
    # Create-profile options
    parser.add_argument('--profile', choices=['standard', 'research', 'production', 'debug'], 
                       default='research', help="Research profile")
    
    # Save-data options
    parser.add_argument('--filepath', help="Filepath for research data")
    
    args = parser.parse_args()
    
    # Create CLI tool and run command
    cli = NLMCommandLine()
    cli.run(args)

if __name__ == '__main__':
    main()
