#!/usr/bin/env python3

"""
NLM (Neural Learning Machine) Command Line Interface

A comprehensive CLI for NLM simulations, training, and experimentation.
"""

import argparse
import sys
import json
import os
from typing import Optional, List
from pathlib import Path

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some commands will not work.")


class NLMCLI:
    """NLM Command Line Interface"""

    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None

    def create_brain(self, config_file: Optional[str] = None) -> None:
        """Create and initialize a brain"""
        print("Creating NLM brain...")
        
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm module not available")
            return
            
        # Create config
        if config_file and os.path.exists(config_file):
            config = pynlm.Config()
            config.loadFromFile(config_file)
            print(f"  Loaded configuration from: {config_file}")
        else:
            config = pynlm.createDefaultConfig()
            print("  Using default configuration")
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        
        # Initialize brain
        if self.brain.initialize():
            print("  Brain initialized successfully")
            print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
        else:
            print("  ERROR: Failed to initialize brain")
            self.brain = None

    def create_world(self, width: int = 10, height: int = 10, 
                     vision_width: int = 8, vision_height: int = 8) -> None:
        """Create a simple world"""
        print("Creating simple world...")
        
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm module not available")
            return
            
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        
        print(f"  World configured: {width}x{height}")
        print(f"  Vision: {vision_width}x{vision_height}")
        print("  World reset and ready")

    def create_agent(self) -> None:
        """Create agent brain interface"""
        if not self.brain:
            print("ERROR: No brain available. Create a brain first.")
            return
            
        print("Creating agent brain interface...")
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        print("  Agent initialized with world")

    def run_simulation(self, steps: int, enable_features: dict = None) -> None:
        """Run a complete simulation"""
        if not (self.brain and self.world and self.agent):
            print("ERROR: Missing components. Create brain, world, and agent first.")
            return
            
        if enable_features is None:
            enable_features = {}
            
        print(f"Running simulation for {steps} steps...")
        
        # Enable requested features
        if enable_features.get('reward_modulation', False):
            self.agent.enableRewardModulation(True)
            print("  Reward modulation enabled")
        
        if enable_features.get('structural_plasticity', False):
            self.agent.enableStructuralPlasticity(True)
            print("  Structural plasticity enabled")
            
        if enable_features.get('development', False):
            self.agent.enableDevelopment(True)
            print("  Development enabled")
            
        if enable_features.get('curiosity', False):
            self.agent.enableCuriosity(True)
            print("  Curiosity enabled")

        # Main simulation loop
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept and process it
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Agent decides action
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"  Step {step}: {self.brain.getFiringNeuronCount()} neurons firing, "
                      f"{self.brain.getTotalSpikeCount()} total spikes")

        print(f"Simulation completed after {steps} steps")
        print(f"Final statistics:")
        print(f"  - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  - Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  - Developmental stage: {self.agent.getDevelopmentalStage()}")

    def get_brain_stats(self) -> None:
        """Display brain statistics"""
        if not self.brain:
            print("ERROR: No brain available")
            return
            
        print("Brain Statistics:")
        print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  - Active neurons: {self.brain.getActiveNeuronCount()}")
        print(f"  - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  - Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  - Excitation/Inhibition ratio: {self.brain.getExcitationInhibitionRatio():.2f}")
        print(f"  - Total spike count: {self.brain.getTotalSpikeCount()}")
        print(f"  - Developmental stage: {self.brain.getDevelopmentalStage()}")
        
        if self.agent:
            print(f"  - Neuromodulation level: {self.agent.getNeuromodulationLevel():.2f}")
            print(f"  - Curiosity level: {self.agent.getCuriosityLevel():.2f}")
            print(f"  - Novelty level: {self.agent.getNoveltyLevel():.2f}")

    def save_brain(self, filepath: str) -> None:
        """Save brain state to file"""
        if not self.brain:
            print("ERROR: No brain available")
            return
            
        print(f"Saving brain state to: {filepath}")
        if self.brain.save(filepath):
            print("  Brain saved successfully")
        else:
            print("  ERROR: Failed to save brain")

    def load_brain(self, filepath: str) -> None:
        """Load brain state from file"""
        if not self.brain:
            print("ERROR: No brain instance available. Create a new brain first.")
            return
            
        print(f"Loading brain state from: {filepath}")
        if self.brain.load(filepath):
            print("  Brain loaded successfully")
        else:
            print("  ERROR: Failed to load brain")

    def run_demo(self, demo_type: str = "simple") -> None:
        """Run predefined demos"""
        print(f"Running {demo_type} demo...")
        
        if demo_type == "simple":
            self.create_brain()
            if self.brain:
                for i in range(10):
                    self.brain.step(i)
                print("Simple brain demo completed!")
                
        elif demo_type == "agent":
            self.create_brain()
            self.create_world(20, 20, 8, 8)
            self.create_agent()
            self.run_simulation(100, {
                'reward_modulation': True,
                'curiosity': True
            })
            
        elif demo_type == "complete":
            self.create_brain()
            self.create_world(15, 15, 8, 8)
            self.create_agent()
            self.run_simulation(100, {
                'reward_modulation': True,
                'structural_plasticity': True,
                'development': True,
                'curiosity': True
            })
        else:
            print(f"Unknown demo type: {demo_type}")


def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a simple brain and run basic simulation
  nlm-cli brain simple --steps 1000

  # Create complete agent setup
  nlm-cli agent create --world-width 20 --world-height 20
  nlm-cli agent run --steps 500 --features reward_modulation curiosity

  # Save and load brain states
  nlm-cli brain save my_brain.nlm
  nlm-cli brain load my_brain.nlm

  # Run built-in demos
  nlm-cli demo simple
  nlm-cli demo agent
  nlm-cli demo complete
        ""
    )

    # Create main command parser
    subparsers = parser.add_subparsers(dest='command', help='Available commands')

    # Brain commands
    brain_parser = subparsers.add_parser('brain', help='Brain management commands')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_command', help='Brain operations')

    brain_subparsers.add_parser('create', help='Create a brain')
    brain_create_parser = brain_subparsers.add_parser('create', help='Create a brain')
    brain_create_parser.add_argument('--config', '-c', help='Configuration file path')

    brain_subparsers.add_parser('stats', help='Display brain statistics')
    brain_subparsers.add_parser('reset', help='Reset brain state')

    save_parser = brain_subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('filepath', help='Output file path')

    load_parser = brain_subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('filepath', help='Input file path')

    # World commands
    world_parser = subparsers.add_parser('world', help='World management commands')
    world_subparsers = world_parser.add_subparsers(dest='world_command', help='World operations')

    world_create_parser = world_subparsers.add_parser('create', help='Create a world')
    world_create_parser.add_argument('--width', type=int, default=10, help='World width')
    world_create_parser.add_argument('--height', type=int, default=10, help='World height')
    world_create_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
    world_create_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')

    world_subparsers.add_parser('reset', help='Reset world')

    # Agent commands
    agent_parser = subparsers.add_parser('agent', help='Agent management commands')
    agent_subparsers = agent_parser.add_subparsers(dest='agent_command', help='Agent operations')

    agent_subparsers.add_parser('create', help='Create agent brain interface')
    agent_subparsers.add_parser('stats', help='Display agent statistics')

    run_parser = agent_subparsers.add_parser('run', help='Run simulation')
    run_parser.add_argument('--steps', type=int, required=True, help='Number of simulation steps')
    run_parser.add_argument('--features', nargs='+', help='Features to enable')

    # Demo commands
    demo_parser = subparsers.add_parser('demo', help='Run built-in demonstrations')
    demo_parser.add_argument('type', choices=['simple', 'agent', 'complete'], help='Demo type')

    # Direct execution commands (create both brain and agent automatically)
    exec_parser = subparsers.add_parser('exec', help='Execute simulation with all components')
    exec_parser.add_argument('--steps', type=int, required=True, help='Number of simulation steps')
    exec_parser.add_argument('--config', help='Configuration file path')
    exec_parser.add_argument('--world-width', type=int, default=10, help='World width')
    exec_parser.add_argument('--world-height', type=int, default=10, help='World height')
    exec_parser.add_argument('--enable-features', nargs='+', help='Features to enable')

    # Global options
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')

    # Parse arguments
    args = parser.parse_args()

    # Create CLI instance
    cli = NLMCLI()

    try:
        if args.command == 'brain':
            if args.brain_command == 'create':
                cli.create_brain(args.config)
            elif args.brain_command == 'stats':
                cli.get_brain_stats()
            elif args.brain_command == 'save':
                cli.save_brain(args.filepath)
            elif args.brain_command == 'load':
                cli.load_brain(args.filepath)

        elif args.command == 'world':
            if args.world_command == 'create':
                cli.create_world(args.width, args.height, args.vision_width, args.vision_height)
            elif args.world_command == 'reset':
                if cli.world:
                    cli.world.reset()
                    print("World reset")
                else:
                    print("ERROR: No world available")

        elif args.command == 'agent':
            if args.agent_command == 'create':
                cli.create_agent()
            elif args.agent_command == 'stats':
                if cli.agent:
                    print("Agent Statistics:")
                    print(f"  - Sensory input size: {cli.agent.getSensoryInputSize()}")
                    print(f"  - Motor output size: {cli.agent.getMotorOutputSize()}")
                    print(f"  - Developmental stage: {cli.agent.getDevelopmentalStage()}")
                    print(f"  - Neuromodulation level: {cli.agent.getNeuromodulationLevel():.2f}")
                    print(f"  - Curiosity level: {cli.agent.getCuriosityLevel():.2f}")
                    print(f"  - Novelty level: {cli.agent.getNoveltyLevel():.2f}")
                    print(f"  - Prediction error: {cli.agent.getPredictionError():.2f}")
                else:
                    print("ERROR: No agent available")

            elif args.agent_command == 'run':
                features = {}
                if args.features:
                    feature_map = {
                        'reward_modulation': 'reward_modulation',
                        'structural_plasticity': 'structural_plasticity',
                        'development': 'development',
                        'curiosity': 'curiosity'
                    }
                    for f in args.features:
                        if f in feature_map:
                            features[feature_map[f]] = True

                cli.create_brain()
                cli.create_world()
                cli.create_agent()
                cli.run_simulation(args.steps, features)

        elif args.command == 'demo':
            cli.run_demo(args.type)

        elif args.command == 'exec':
            cli.create_brain(args.config)
            cli.create_world(args.world_width, args.world_height)
            cli.create_agent()

            features = {}
            if args.enable_features:
                feature_map = {
                    'reward_modulation': 'reward_modulation',
                    'structural_plasticity': 'structural_plasticity',
                    'development': 'development',
                    'curiosity': 'curiosity'
                }
                for f in args.enable_features:
                    if f in feature_map:
                        features[feature_map[f]] = True

            cli.run_simulation(args.steps, features)

        else:
            parser.print_help()

    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    except Exception as e:
        print(f"ERROR: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()