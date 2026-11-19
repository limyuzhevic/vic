#!/usr/bin/env python3
"""
NLM - Advanced Neural Learning Machine CLI Tools

Sophisticated command-line interface for NLM with advanced features:
- Brain configuration management
- Simulation control and monitoring
- Checkpoint management
- Performance profiling
- Advanced analysis tools
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any
import time

# Add the parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not available. Please install NLM Python bindings.")

class NLMCLI:
    """Advanced NLM command-line interface"""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
    
    def create_brain(self, neuron_count: int = 1000, region_count: int = 2, 
                    connection_probability: float = 0.1) -> bool:
        """Create a new brain with specified configuration"""
        try:
            self.config = pynlm.createDefaultConfig()
            self.config.set("neuron_count", neuron_count)
            self.config.set("region_count", region_count)
            self.config.set("connection_probability", connection_probability)
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            print(f"✓ Created brain with {neuron_count} neurons, {region_count} regions")
            print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"  - Firing rate: {self.brain.getAverageFiringRate():.2f}")
            return True
        except Exception as e:
            print(f"✗ Failed to create brain: {e}")
            return False
    
    def simulate(self, steps: int, verbose: bool = False) -> bool:
        """Run simulation for specified number of steps"""
        if not self.brain:
            print("✗ No brain created. Use 'create' command first.")
            return False
        
        try:
            print(f"Running simulation for {steps} steps...")
            start_time = time.time()
            
            for step in range(steps):
                self.brain.step(step)
                
                if verbose and step % 100 == 0:
                    print(f"  Step {step}: {self.brain.getFiringNeuronCount()} firing, "
                          f"{self.brain.getTotalSpikeCount()} total spikes")
            
            end_time = time.time()
            duration = end_time - start_time
            
            print(f"✓ Simulation completed in {duration:.2f} seconds")
            print(f"  - Final firing rate: {self.brain.getAverageFiringRate():.2f}")
            print(f"  - Total spikes: {self.brain.getTotalSpikeCount()}")
            print(f"  - Active neurons: {self.brain.getFiringNeuronCount()}")
            print(f"  - Average step time: {duration/steps*1000:.2f} ms")
            return True
        except Exception as e:
            print(f"✗ Simulation failed: {e}")
            return False
    
    def save_state(self, filepath: str) -> bool:
        """Save brain state to checkpoint file"""
        if not self.brain:
            print("✗ No brain to save.")
            return False
        
        try:
            if self.brain.save(filepath):
                print(f"✓ Brain state saved to {filepath}")
                return True
            else:
                print(f"✗ Failed to save brain state to {filepath}")
                return False
        except Exception as e:
            print(f"✗ Save failed: {e}")
            return False
    
    def load_state(self, filepath: str) -> bool:
        """Load brain state from checkpoint file"""
        if not self.config:
            print("✗ No config available for loading.")
            return False
        
        try:
            # Create new brain with same config
            old_brain = self.brain
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            if self.brain.load(filepath):
                print(f"✓ Brain state loaded from {filepath}")
                print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
                print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
                return True
            else:
                print(f"✗ Failed to load brain state from {filepath}")
                # Restore old brain if load failed
                self.brain = old_brain
                return False
        except Exception as e:
            print(f"✗ Load failed: {e}")
            return False
    
    def analyze(self) -> bool:
        """Analyze brain state and provide insights"""
        if not self.brain:
            print("✗ No brain to analyze.")
            return False
        
        try:
            print("=== BRAIN ANALYSIS ===")
            
            # Basic statistics
            total_neurons = self.brain.getTotalNeuronCount()
            total_synapses = self.brain.getTotalSynapseCount()
            firing_rate = self.brain.getAverageFiringRate()
            eir_ratio = self.brain.getExcitationInhibitionRatio()
            total_spikes = self.brain.getTotalSpikeCount()
            
            print(f"Network Statistics:")
            print(f"  - Neurons: {total_neurons}")
            print(f"  - Synapses: {total_synapses}")
            print(f"  - Connectivity: {total_synapses/total_neurons:.4f} synapses/neuron")
            print(f"  - Firing Rate: {firing_rate:.2f} Hz")
            print(f"  - Total Spikes: {total_spikes}")
            print(f"  - E/I Ratio: {eir_ratio:.3f}")
            
            # Connectivity analysis
            if total_neurons > 0 and total_synapses > 0:
                print(f"\nConnectivity Analysis:")
                print(f"  - Average firing per neuron: {firing_rate/total_neurons:.4f}")
                print(f"  - Network activity: {'HIGH' if firing_rate > 10 else 'MEDIUM' if firing_rate > 5 else 'LOW'}")
                
                if eir_ratio > 1.5:
                    print(f"  - Imbalance: EXCITATORY dominance detected")
                elif eir_ratio < 0.5:
                    print(f"  - Imbalance: INHIBITORY dominance detected")
                else:
                    print(f"  - Balance: Healthy E/I ratio")
            
            # Learning indicators
            print(f"\nLearning Indicators:")
            if firing_rate > 0 and total_spikes > 100:
                print(f"  - Learning Active: ✓ (significant spike activity)")
            else:
                print(f"  - Learning Active: ✗ (low activity)")
                
            if abs(eir_ratio - 1.0) < 0.2:
                print(f"  - Stability: ✓ (balanced E/I)")
            else:
                print(f"  - Stability: ✗ (imbalanced)")
                
            return True
        except Exception as e:
            print(f"✗ Analysis failed: {e}")
            return False
    
    def benchmark(self, steps: int = 1000) -> bool:
        """Benchmark brain performance"""
        if not self.brain:
            print("✗ No brain to benchmark.")
            return False
        
        try:
            print(f"Benchmarking brain for {steps} steps...")
            
            start_time = time.time()
            for step in range(steps):
                self.brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            steps_per_second = steps / duration
            avg_step_time = (duration / steps) * 1000  # ms
            
            print(f"Performance Results:")
            print(f"  - Total time: {duration:.3f} seconds")
            print(f"  - Steps per second: {steps_per_second:.1f}")
            print(f"  - Average step time: {avg_step_time:.2f} ms")
            print(f"  - Throughput: {(steps_per_second * 1000):.1f} steps/sec")
            
            return True
        except Exception as e:
            print(f"✗ Benchmark failed: {e}")
            return False
    
    def export_config(self, filepath: str) -> bool:
        """Export current configuration to file"""
        if not self.config:
            print("✗ No configuration to export.")
            return False
        
        try:
            self.config.saveToFile(filepath)
            print(f"✓ Configuration exported to {filepath}")
            return True
        except Exception as e:
            print(f"✗ Export failed: {e}")
            return False
    
    def import_config(self, filepath: str) -> bool:
        """Import configuration from file"""
        try:
            self.config = pynlm.createDefaultConfig()
            if self.config.loadFromFile(filepath):
                print(f"✓ Configuration imported from {filepath}")
                return True
            else:
                print(f"✗ Failed to import configuration from {filepath}")
                return False
        except Exception as e:
            print(f"✗ Import failed: {e}")
            return False
    
    def setup_full_environment(self, world_width: int = 20, world_height: int = 20,
                              vision_width: int = 8, vision_height: int = 8) -> bool:
        """Set up complete brain-world-agent environment"""
        if not PYNLM_AVAILABLE:
            print("✗ Python bindings not available.")
            return False
        
        try:
            # Create components
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(world_width, world_height, vision_width, vision_height)
            self.world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            print("✓ Complete environment setup:")
            print(f"  - Brain: {self.brain.getTotalNeuronCount()} neurons")
            print(f"  - World: {world_width}x{world_height} grid")
            print(f"  - Vision: {vision_width}x{vision_height} pixels")
            print(f"  - Agent: Connected to brain and world")
            
            return True
        except Exception as e:
            print(f"✗ Environment setup failed: {e}")
            return False
    
    def run_full_episode(self, steps: int) -> bool:
        """Run complete episode with world interaction"""
        if not (self.brain and self.world and self.agent):
            print("✗ Incomplete environment. Use 'setup' command first.")
            return False
        
        try:
            print(f"Running full episode for {steps} steps...")
            
            total_reward = 0.0
            steps_without_reward = 0
            
            for step in range(steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process in brain
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                
                # Get action
                action = self.agent.decodeMotorCommand()
                
                # Apply action
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                self.agent.applyRewardModulation(result.reward, 0.0)
                
                total_reward += result.reward
                
                if verbose and step % 50 == 0:
                    print(f"  Step {step}: Reward {result.reward:.2f}, "
                          f"Firing: {self.brain.getFiringNeuronCount()}, "
                          f"Curiosity: {self.agent.getCuriosityLevel():.2f}")
            
            print(f"✓ Episode completed:")
            print(f"  - Total reward: {total_reward:.2f}")
            print(f"  - Avg reward/step: {total_reward/steps:.2f}")
            print(f"  - Final curiosity: {self.agent.getCuriosityLevel():.2f}")
            print(f"  - Final novelty: {self.agent.getNoveltyLevel():.2f}")
            print(f"  - Final prediction error: {self.agent.getPredictionError():.2f}")
            print(f"  - Development stage: {self.brain.getDevelopmentalStage()}")
            
            return True
        except Exception as e:
            print(f"✗ Episode failed: {e}")
            return False

def main():
    """Main CLI entry point"""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module not available. Please install NLM Python bindings.")
        print("Run: pip install . or pip install -e .")
        sys.exit(1)
    
    cli = NLMCLI()
    
    parser = argparse.ArgumentParser(
        description="NLM Advanced Neural Learning Machine CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s create --neurons 500 --regions 2
  %(prog)s simulate --steps 1000 --verbose
  %(prog)s setup --world 30x30 --vision 12x12
  %(prog)s analyze
  %(prog)s benchmark --steps 500
  %(prog)s save --file brain_state.bin
  %(prog)s load --file brain_state.bin

For more help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a new brain')
    create_parser.add_argument('--neurons', '-n', type=int, default=1000,
                               help='Number of neurons (default: 1000)')
    create_parser.add_argument('--regions', '-r', type=int, default=2,
                               help='Number of regions (default: 2)')
    create_parser.add_argument('--conn', '-c', type=float, default=0.1,
                               help='Connection probability (default: 0.1)')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
    simulate_parser.add_argument('--steps', '-s', type=int, default=1000,
                                help='Number of simulation steps (default: 1000)')
    simulate_parser.add_argument('--verbose', '-v', action='store_true',
                                help='Enable verbose output')
    
    # Save command
    save_parser = subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('--file', '-f', type=str, default='brain_state.bin',
                           help='Output file (default: brain_state.bin)')
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('--file', '-f', type=str, default='brain_state.bin',
                           help='Input file (default: brain_state.bin)')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain state')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark performance')
    benchmark_parser.add_argument('--steps', '-s', type=int, default=1000,
                                 help='Number of steps for benchmark (default: 1000)')
    
    # Setup environment command
    setup_parser = subparsers.add_parser('setup', help='Setup complete brain-world-agent environment')
    setup_parser.add_argument('--world-width', type=int, default=20,
                             help='World width (default: 20)')
    setup_parser.add_argument('--world-height', type=int, default=20,
                             help='World height (default: 20)')
    setup_parser.add_argument('--vision-width', type=int, default=8,
                             help='Vision width (default: 8)')
    setup_parser.add_argument('--vision-height', type=int, default=8,
                             help='Vision height (default: 8)')
    
    # Export config command
    export_parser = subparsers.add_parser('export-config', help='Export configuration to file')
    export_parser.add_argument('--file', '-f', type=str, default='config.json',
                              help='Output file (default: config.json)')
    
    # Import config command
    import_parser = subparsers.add_parser('import-config', help='Import configuration from file')
    import_parser.add_argument('--file', '-f', type=str, required=True,
                              help='Input configuration file')
    
    # Run episode command
    episode_parser = subparsers.add_parser('episode', help='Run complete episode')
    episode_parser.add_argument('--steps', '-s', type=int, default=1000,
                               help='Number of episode steps (default: 1000)')
    episode_parser.add_argument('--verbose', '-v', action='store_true',
                               help='Enable verbose output')
    
    # Status command
    status_parser = subparsers.add_parser('status', help='Show current status')
    
    # Help command
    help_parser = subparsers.add_parser('help', help='Show help information')
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)
    
    args = parser.parse_args()
    
    # Command handlers
    if args.command == 'create':
        if cli.create_brain(args.neurons, args.regions, args.conn):
            print("✓ Brain created successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'simulate':
        if cli.simulate(args.steps, args.verbose):
            print("✓ Simulation completed successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'save':
        if cli.save_state(args.file):
            print("✓ State saved successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'load':
        if cli.load_state(args.file):
            print("✓ State loaded successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'analyze':
        if cli.analyze():
            print("✓ Analysis completed successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'benchmark':
        if cli.benchmark(args.steps):
            print("✓ Benchmark completed successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'setup':
        if cli.setup_full_environment(args.world_width, args.world_height,
                                    args.vision_width, args.vision_height):
            print("✓ Environment setup completed successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'export-config':
        if cli.export_config(args.file):
            print("✓ Configuration exported successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'import-config':
        if cli.import_config(args.file):
            print("✓ Configuration imported successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'episode':
        if cli.run_full_episode(args.steps):
            print("✓ Episode completed successfully")
        else:
            sys.exit(1)
    
    elif args.command == 'status':
        print("=== CURRENT STATUS ===")
        if cli.brain:
            print(f"Brain: {'✓' if cli.brain else '✗'}")
            print(f"  - Neurons: {cli.brain.getTotalNeuronCount()}")
            print(f"  - Synapses: {cli.brain.getTotalSynapseCount()}")
            print(f"  - Firing rate: {cli.brain.getAverageFiringRate():.2f} Hz")
        
        if cli.world:
            print(f"World: {'✓' if cli.world else '✗'}")
            print(f"  - Size: {cli.world.getWidth()}x{cli.world.getHeight()}")
        
        if cli.agent:
            print(f"Agent: {'✓' if cli.agent else '✗'}")
            print(f"  - Curiosity: {cli.agent.getCuriosityLevel():.2f}")
            print(f"  - Novelty: {cli.agent.getNoveltyLevel():.2f}")
        
        if not (cli.brain and cli.world and cli.agent):
            print("  - Incomplete setup")
    
    elif args.command == 'help':
        parser.print_help()
    
    else:
        print(f"Unknown command: {args.command}")
        sys.exit(1


if __name__ == "__main__":
    main()
