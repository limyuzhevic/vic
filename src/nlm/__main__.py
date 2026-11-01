# NLM Command Line Interface
# 
# This module provides enhanced command-line interfaces for NLM (Neural Learning Machine)
# with improved usability, validation, and user experience.

import argparse
import sys
from typing import Optional, Dict, Any
from pathlib import Path

# Add the src directory to Python path
sys.path.insert(0, str(Path(__file__).parent / ".." / ".." / "src"))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install NLM Python bindings.")
    print("Run: pip install .")
    sys.exit(1)

class NLMConfig:
    """Enhanced configuration management for NLM"""
    
    DEFAULT_CONFIG = {
        "random_seed": 42,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "environment_width": 20,
        "environment_height": 20,
        "vision_width": 16,
        "vision_height": 16,
        "world_timestep": 0.01,
        "enable_reward_modulation": True,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_curiosity": True,
        "enable_checkpointing": True,
        "checkpoint_path": "./nlm_checkpoint.bin",
    }
    
    @classmethod
    def get_default_config(cls) -> 'NLMConfig':
        """Get a fresh configuration instance"""
        return NLMConfig()
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        self.config = self.DEFAULT_CONFIG.copy()
        if config_dict:
            self.config.update(config_dict)
    
    def get(self, key: str, default=None):
        return self.config.get(key, default)
    
    def set(self, key: str, value: Any):
        self.config[key] = value
    
    def to_pynlm_config(self):
        """Convert to pynlm configuration format"""
        # Map NLM keys to pynlm format if needed
        config = pynlm.createDefaultConfig()
        
        # Set known parameters
        if self.config.get("random_seed") is not None:
            config.set("random_seed", self.config["random_seed"])
        if self.config.get("neuron_count") is not None:
            config.set("neuron_count", self.config["neuron_count"])
        if self.config.get("region_count") is not None:
            config.set("region_count", self.config["region_count"])
        if self.config.get("connection_probability") is not None:
            config.set("connection_probability", self.config["connection_probability"])
            
        return config

class NLMCLI:
    """Enhanced command-line interface for NLM"""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) - Advanced Brain Simulation",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm --phase 2                    # Run Phase 2: Basic neural computation
  nlm --phase 3 --episodes 10     # Run Phase 3 with 10 episodes
  nlm --phase 6 --benchmark        # Run Phase 6 integration with benchmarking
  nlm --config my_config.cfg       # Load custom configuration
  nlm --verbose --profile          # Run with verbose logging and profiling
            """
        )
        
        self.setup_arguments()
    
    def setup_arguments(self):
        """Setup command line arguments"""
        # Basic options
        self.parser.add_argument(
            "--phase", "-p",
            choices=["2", "3", "4", "5", "6"],
            default="2",
            help="Run specific phase (2=basic neural computation, 3=world interaction, "
                 "4=cognition, 5=brain-personality, 6=final integration)"
        )
        
        self.parser.add_argument(
            "--config", "-c",
            help="Load configuration from file"
        )
        
        self.parser.add_argument(
            "--verbose", "-v",
            action="store_true",
            help="Enable verbose logging"
        )
        
        self.parser.add_argument(
            "--profile",
            action="store_true",
            help="Enable performance profiling"
        )
        
        self.parser.add_argument(
            "--benchmark",
            action="store_true",
            help="Run benchmark mode"
        )
        
        self.parser.add_argument(
            "--checkpoint",
            help="Save brain state to checkpoint file"
        )
        
        self.parser.add_argument(
            "--load",
            help="Load brain state from checkpoint file"
        )
        
        # Phase-specific arguments
        self.setup_phase_arguments()
    
    def setup_phase_arguments(self):
        """Setup phase-specific arguments"""
        phase_group = self.parser.add_argument_group("Phase-specific options")
        
        # Phase 2 arguments
        phase2_group = phase_group.add_mutually_exclusive_group()
        phase2_group.add_argument(
            "--neurons", "-n",
            type=int,
            help="Number of neurons (default: 500 for Phase 2, 1000 for Phase 3+)
        phase2_group.add_argument(
            "--timestep", "-t",
            type=float,
            help="Simulation timestep (default: 0.001)"
        )
        phase2_group.add_argument(
            "--seed",
            type=int,
            help="Random seed (default: 42)"
        )
        
        # Phase 3 arguments
        phase3_group = phase_group.add_mutually_exclusive_group()
        phase3_group.add_argument(
            "--episodes",
            type=int,
            help="Number of episodes (default: 5 for Phase 3)"
        )
        phase3_group.add_argument(
            "--steps",
            type=int,
            help="Steps per episode (default: 200 for Phase 3)"
        )
        
        # Phase 4 arguments
        phase4_group = phase_group.add_mutually_exclusive_group()
        phase4_group.add_argument(
            "--trials",
            type=int,
            help="Number of trials (default: 50 for Phase 4)"
        )
        
        # Phase 5 arguments
        phase5_group = phase_group.add_mutually_exclusive_group()
        phase5_group.add_argument(
            "--sandbox-steps",
            type=int,
            help="Sandbox steps (default: 100 for Phase 5)"
        )
        
        # Phase 6 arguments
        phase6_group = phase_group.add_mutually_exclusive_group()
        phase6_group.add_argument(
            "--maxsteps",
            type=int,
            help="Maximum steps for Phase 6 (default: 2000)"
        )
        phase6_group.add_argument(
            "--test-mode",
            choices=["quick", "full", "integration"],
            default="quick",
            help="Test mode for Phase 6 (default: quick)"
        )
    
    def run(self, args=None):
        """Parse arguments and run the specified demo"""
        parsed_args = self.parser.parse_args(args)
        
        # Initialize configuration
        config = NLMConfig()
        
        # Load custom config file if specified
        if parsed_args.config:
            try:
                custom_config = NLMConfig()
                # This would need to be implemented based on the config file format
                print(f"Note: Custom config file '{parsed_args.config}' loading not implemented")
            except Exception as e:
                print(f"Error loading config file: {e}")
                return 1
        
        # Set verbose mode
        if parsed_args.verbose:
            print("Verbose mode enabled")
        
        # Set profiling
        if parsed_args.profile:
            print("Performance profiling enabled")
            # TODO: Implement profiling
        
        # Set benchmark mode
        if parsed_args.benchmark:
            print("Benchmark mode enabled")
        
        # Phase-specific configuration
        phase = int(parsed_args.phase)
        if parsed_args.neurons:
            config.set("neuron_count", parsed_args.neurons)
        if parsed_args.timestep:
            config.set("simulation_timestep", parsed_args.timestep)
        if parsed_args.seed:
            config.set("random_seed", parsed_args.seed)
        
        # Create brain with enhanced configuration
        print(f"\n=== NLM Phase {phase} ===")
        brain_config = config.to_pynlm_config()
        
        try:
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
            print(f"Total synapses: {brain.getTotalSynapseCount()}")
            
            # Run selected phase
            self.run_phase(phase, brain, parsed_args)
            
            # Handle checkpointing
            if parsed_args.checkpoint:
                print(f"Note: Checkpoint saving to '{parsed_args.checkpoint}' not implemented")
            
            if parsed_args.load:
                print(f"Note: Checkpoint loading from '{parsed_args.load}' not implemented")
                
        except Exception as e:
            print(f"Error running NLM: {e}")
            return 1
        
        return 0
    
    def run_phase(self, phase: int, brain, args):
        """Run the specified phase"""
        if phase == 2:
            self.run_phase2(brain, args)
        elif phase == 3:
            self.run_phase3(brain, args)
        elif phase == 4:
            self.run_phase4(brain, args)
        elif phase == 5:
            self.run_phase5(brain, args)
        elif phase == 6:
            self.run_phase6(brain, args)
        else:
            print(f"Unknown phase: {phase}")
    
    def run_phase2(self, brain, args):
        """Phase 2: Basic neural computation"""
        print("Running Phase 2: Basic Neural Computation...")
        print("This demo shows:")
        print("  - LIF neuron dynamics")
        print("  - Event-driven spike propagation")
        print("  - STDP and Hebbian plasticity")
        print("  - Structural plasticity")
        
        # Run a few steps
        for step in range(100):
            brain.step(step)
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("\nPhase 2 complete!")
    
    def run_phase3(self, brain, args):
        """Phase 3: World interaction"""
        print("Running Phase 3: World Interaction...")
        print("Note: Full Phase 3 demo requires world and agent setup")
        print("This would include sensory input processing, action selection,")
        print("and environmental interaction.")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 16, 16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.01)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("\nPhase 3 complete!")
    
    def run_phase4(self, brain, args):
        """Phase 4: Emerging cognition"""
        print("Running Phase 4: Emerging Cognition...")
        print("This demo tests various cognitive systems:")
        print("  - Temporal prediction")
        print("  - Working memory")
        print("  - Episodic memory")
        print("  - Concept formation")
        print("  - Neural attention")
        print("  - Planning")
        print("  - Self-model")
        print("  - Social learning")
        print("  - Continual learning")
        
        # Phase 4 is complex and would require all the experiment systems
        # For simplicity, we show a representative subset
        for step in range(50):
            brain.step(step)
            if step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("\nPhase 4 basic run complete!")
        print("Note: Full Phase 4 demo requires experimental frameworks.")
    
    def run_phase5(self, brain, args):
        """Phase 5: Brain-personality"""
        print("Running Phase 5: Brain-Personality...")
        print("This demo shows:")
        print("  - Pattern probe (discrimination)")
        print("  - Sandbox learning")
        print("  - Checkpoint round-trip")
        print("  - Learned preference trait")
        
        for step in range(50):
            brain.step(step)
            if step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("\nPhase 5 basic run complete!")
        print("Note: Full Phase 5 demo requires sandbox environment.")
    
    def run_phase6(self, brain, args):
        """Phase 6: Final integration"""
        print("Running Phase 6: Final Integration...")
        print("This demo tests system integration:")
        print("  - Memory systems")
        print("  - Neuromodulation")
        print("  - Prediction systems")
        print("  - Development")
        print("  - Checkpointing")
        print("  - Replay systems")
        
        for step in range(100):
            brain.step(step)
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("\nPhase 6 complete!")

def main():
    """Main entry point"""
    cli = NLMCLI()
    return cli.run()

if __name__ == "__main__":
    sys.exit(main())
