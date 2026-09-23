#!/usr/bin/env python3
"""
NLM CLI - Modern command-line interface for Neural Learning Machine

This tool provides advanced capabilities for:
- Running simulations with various configurations
- Interactive brain exploration and inspection
- Performance benchmarking
- Configuration management
- Visualization and debugging

The CLI is designed to showcase the Phase 6 integrated brain capabilities.
"""

import sys
import os
import json
import time
import argparse
from pathlib import Path
from typing import Optional, Dict, Any, List
import threading
import queue

# Add the current directory to the path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some CLI features will be limited.")

# Color codes for terminal output
class Colors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"

# Global configuration
DEFAULT_CONFIG_FILE = "nlm_config.json"
SESSION_DIR = "nlm_sessions"

class NLMCLI:
    """Main NLM CLI class"""
    
    def __init__(self):
        self.sessions_dir = Path(SESSION_DIR)
        self.sessions_dir.mkdir(exist_ok=True)
        self.current_session = None
        self.session_file = None
        
    def print_banner(self, version="Phase 6.0 - Final Integration"):
        """Print the NLM CLI banner"""
        print(f"{Colors.HEADER}{Colors.BOLD}")
        print("╔═══════════════════════════════════════════════════════════════╗")
        print("║                                                               ║")
        print("║     NLM — 熙然 (Serene Flow)                                ║")
        print("║     Neural Learning Machine                                   ║")
        print("║                                                               ║")
        print("║     Phase 6.0 - Final Integration Demo                        ║")
        print("║                                                               ║")
        print("║     Advanced Brain Simulation with Full Integration          ║")
        print("║                                                               ║")
        print("╚═══════════════════════════════════════════════════════════════╝")
        print(f"{Colors.ENDC}")
        print(f"\n{Colors.OKCYAN}Phase 6 Features Demonstrated:{Colors.ENDC}")
        print(f"  • Memory Systems (Working + Episodic)")
        print(f"  • Neuromodulation (Dopamine + Curiosity + Novelty)")
        print(f"  • Prediction & Cognition Systems")
        print(f"  • Development & Structural Plasticity")
        print(f"  • Checkpointing & Replay")
        print()
        
    def create_default_config(self):
        """Create and save default configuration"""
        config = {
            "brain": {
                "neuron_count": 1000,
                "region_count": 1,
                "connection_probability": 0.1,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            },
            "simulation": {
                "max_steps": 10000,
                "enable_checkpointing": True,
                "enable_replay": True,
                "enable_development": True
            },
            "agent": {
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_structural_plasticity": True,
                "enable_development": True
            }
        }
        
        with open(DEFAULT_CONFIG_FILE, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"{Colors.OKGREEN}Created default configuration: {DEFAULT_CONFIG_FILE}{Colors.ENDC}")
        return config
        
    def load_config(self, config_file: Optional[str] = None) -> Dict[str, Any]:
        """Load configuration from file"""
        if config_file is None:
            config_file = DEFAULT_CONFIG_FILE
            
        if not os.path.exists(config_file):
            print(f"{Colors.WARNING}Config file {config_file} not found. Using defaults.{Colors.ENDC}")
            return self.create_default_config()
        
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            print(f"{Colors.OKGREEN}Loaded configuration from: {config_file}{Colors.ENDC}")
            return config
        except Exception as e:
            print(f"{Colors.FAIL}Error loading config: {e}{Colors.ENDC}")
            return self.create_default_config()
            
    def run_demo(self, steps: int = 1000, config_file: Optional[str] = None):
        """Run the Phase 6 integration demo"""
        print(f"{Colors.OKBLUE}=== Running Phase 6 Integration Demo ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE:
            print(f"{Colors.FAIL}pynlm not available. Cannot run demo.{Colors.ENDC}")
            return
        
        config = self.load_config(config_file)
        
        # Create brain with configuration
        brain_config = config.get("brain", {})
        n_neurons = brain_config.get("neuron_count", 1000)
        
        print(f"Creating brain with {n_neurons} neurons...")
        
        # Create Python bindings configuration
        py_config = pynlm.createDefaultConfig()
        
        # Set configuration values
        for key, value in brain_config.items():
            if isinstance(value, float):
                py_config.set(key, value)
            elif isinstance(value, int):
                py_config.set(key, value)
                
        brain = pynlm.createBrain(py_config)
        brain.initialize()
        
        # Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable features based on config
        agent_config = config.get("agent", {})
        if agent_config.get("enable_reward_modulation", True):
            agent.enableRewardModulation(True)
        if agent_config.get("enable_curiosity", True):
            agent.enableCuriosity(True)
        if agent_config.get("enable_structural_plasticity", True):
            # Note: This might not exist in pynlm, check
            pass
        if agent_config.get("enable_development", True):
            # Note: This might not exist in pynlm, check
            pass
        
        print(f"Starting simulation for {steps} steps...")
        
        # Run simulation
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                firing = brain.getFiringNeuronCount()
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                dopamine = agent.getNeuromodulationLevel()
                dev_stage = brain.getDevelopmentalStage()
                
                print(f"  Step {step:4d}: Firing={firing:4d}, "
                      f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                      f"Dopamine={dopamine:.3f}, Stage={dev_stage.name}")
        
        # Print final results
        print(f"{Colors.OKGREEN}\n=== Demo Complete ==={Colors.ENDC}")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total synapses: {brain.getTotalSynapseCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        if config.get("simulation", {}).get("enable_checkpointing", False):
            checkpoint_path = f"{SESSION_DIR}/demo_checkpoint.bin"
            brain.save(checkpoint_path)
            print(f"Saved checkpoint to: {checkpoint_path}")
            
    def run_interactive_mode(self):
        """Run interactive mode for brain exploration"""
        print(f"{Colors.OKBLUE}=== Interactive Brain Exploration ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE:
            print(f"{Colors.FAIL}pynlm not available. Cannot run interactive mode.{Colors.ENDC}")
            return
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"{Colors.OKGREEN}Brain created with {brain.getTotalNeuronCount()} neurons{Colors.ENDC}")
        print("\nInteractive Commands:")
        print("  step - Take one simulation step")
        print("  stats - Show brain statistics")
        print("  visual - Show brain visualization")
        print("  reward <amount> - Apply reward modulation")
        print("  quit - Exit interactive mode")
        
        while True:
            try:
                command = input(f"\n{nlm}> ").strip().lower()
                
                if command == "step":
                    brain.step(0)
                    print("Step completed.")
                    
                elif command == "stats":
                    self.print_brain_stats(brain)
                    
                elif command == "visual":
                    self.show_brain_visualization(brain)
                    
                elif command.startswith("reward"):
                    try:
                        parts = command.split()
                        if len(parts) > 1:
                            reward = float(parts[1])
                            print(f"Reward {reward} applied (would need agent for this)")
                    except:
                        print("Invalid reward command. Usage: reward <amount>")
                        
                elif command == "quit":
                    print("Exiting interactive mode.")
                    break
                    
                elif command == "help":
                    print("Available commands: step, stats, visual, reward <amount>, quit, help")
                    
                else:
                    print(f"Unknown command: {command}. Type 'help' for available commands.")
                    
            except KeyboardInterrupt:
                print("\nInterrupted. Exiting interactive mode.")
                break
                
    def print_brain_stats(self, brain):
        """Print detailed brain statistics"""
        print(f"{Colors.OKCYAN}\n=== Brain Statistics ==={Colors.ENDC}")
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Synapses: {brain.getTotalSynapseCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        # Check memory systems if available
        if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
            wm = brain.getWorkingMemory()
            print(f"Working memory traces: {wm.getActiveTraces()}")
            
        if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
            em = brain.getEpisodicMemory()
            print(f"Episodic memory episodes: {em.getEpisodeCount()}")
            
        # Check neuromodulation systems
        if hasattr(brain, 'getDopamine') and brain.getDopamine():
            dopamine = brain.getDopamine()
            print(f"Dopamine level: {dopamine.getLevel():.3f}")
            
        # Check developmental stage
        print(f"Developmental stage: {brain.getDevelopmentalStage().name}")
        
    def show_brain_visualization(self, brain):
        """Show simple brain visualization"""
        print(f"{Colors.OKCYAN}\n=== Brain Visualization ==={Colors.ENDC}")
        
        # Create a simple text-based visualization
        firing = brain.getFiringNeuronCount()
        total = brain.getTotalNeuronCount()
        if total > 0:
            firing_percentage = (firing / total) * 100
            
            print(f"Firing neurons: {firing}/{total} ({firing_percentage:.1f}%)")
            
            # Create a simple bar chart
            bar_length = 50
            filled_length = int((firing_percentage / 100) * bar_length)
            
            print(f"Activity level:")
            print(f"  [{'#' * filled_length}{'.' * (bar_length - filled_length)}] {firing_percentage:.1f}%")
            
    def run_benchmark(self, config_file: Optional[str] = None):
        """Run performance benchmark"""
        print(f"{Colors.OKBLUE}=== Performance Benchmark ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE:
            print(f"{Colors.FAIL}pynlm not available. Cannot run benchmark.{Colors.ENDC}")
            return
        
        # Test different brain sizes
        sizes = [100, 500, 1000]
        
        for size in sizes:
            print(f"\nTesting brain size: {size} neurons")
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time initialization
            start_time = time.time()
            brain.initialize()
            init_time = time.time() - start_time
            
            # Time simulation
            start_time = time.time()
            for step in range(100):  # Short simulation
                brain.step(step)
            sim_time = time.time() - start_time
            
            # Time memory operations
            start_time = time.time()
            # Access some memory systems
            if hasattr(brain, 'getWorkingMemory'):
                wm = brain.getWorkingMemory()
                if wm:
                    _ = wm.getActiveTraces()
            _ = brain.getTotalSpikeCount()
            mem_time = time.time() - start_time
            
            print(f"  Initialization: {init_time:.3f}s")
            print(f"  Simulation (100 steps): {sim_time:.3f}s")
            print(f"  Memory ops: {mem_time:.3f}s")
            print(f"  Throughput: {100/sim_time:.1f} steps/second")
            
    def save_session(self, session_name: str, brain, world=None, agent=None):
        """Save current session state"""
        if not self.sessions_dir.exists():
            self.sessions_dir.mkdir()
            
        session_path = self.sessions_dir / f"{session_name}.json"
        
        session_data = {
            "timestamp": time.time(),
            "brain_stats": {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "firing_count": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount()
            }
        }
        
        # Add agent state if available
        if agent:
            session_data["agent_stats"] = {
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "dopamine": agent.getNeuromodulationLevel()
            }
            
        # Add world state if available
        if world:
            session_data["world_stats"] = {
                "time": world.getSimulationTime(),
                "agent_pos": (world.getAgentBody().getPosition().x, 
                            world.getAgentBody().getPosition().y) if hasattr(world, 'getAgentBody') else None
            }
        
        try:
            with open(session_path, 'w') as f:
                json.dump(session_data, f, indent=2)
                
            print(f"{Colors.OKGREEN}Session saved to: {session_path}{Colors.ENDC}")
            return session_path
        except Exception as e:
            print(f"{Colors.FAIL}Error saving session: {e}{Colors.ENDC}")
            return None
            
    def load_session(self, session_name: str):
        """Load session state"""
        session_path = self.sessions_dir / f"{session_name}.json"
        
        if not session_path.exists():
            print(f"{Colors.FAIL}Session not found: {session_name}{Colors.ENDC}")
            return None
            
        try:
            with open(session_path, 'r') as f:
                session_data = json.load(f)
                
            print(f"{Colors.OKGREEN}Session loaded from: {session_path}{Colors.ENDC}")
            return session_data
        except Exception as e:
            print(f"{Colors.FAIL}Error loading session: {e}{Colors.ENDC}")
            return None
            
    def list_sessions(self):
        """List all saved sessions"""
        if not self.sessions_dir.exists():
            print(f"{Colors.WARNING}No sessions directory found.{Colors.ENDC}")
            return
            
        sessions = list(self.sessions_dir.glob("*.json"))
        
        if not sessions:
            print(f"{Colors.WARNING}No sessions found.{Colors.ENDC}")
            return
            
        print(f"{Colors.OKCYAN}\n=== Saved Sessions ==={Colors.ENDC}")
        for session in sessions:
            try:
                with open(session, 'r') as f:
                    data = json.load(f)
                    timestamp = data.get('timestamp', 0)
                    time_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp))
                    neurons = data.get('brain_stats', {}).get('neuron_count', 'unknown')
                    print(f"  {session.stem:<20} ({time_str}) - {neurons} neurons")
            except:
                print(f"  {session.stem:<20} (unknown details)")


def main():
    """Main CLI entry point"""
    cli = NLMCLI()
    
    parser = argparse.ArgumentParser(
        description="NLM CLI - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli demo                    # Run Phase 6 integration demo
  nlm-cli benchmark                # Run performance benchmark
  nlm-cli interactive              # Enter interactive brain exploration mode
  nlm-cli sessions list            # List saved sessions
  nlm-cli create-config            # Create default configuration file
        """
    )
    
    parser.add_argument(
        '--config', '-c',
        help='Configuration file path (default: nlm_config.json)',
        default=None
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of simulation steps (default: 1000)'
    )
    
    parser.add_argument(
        '--version', '-v',
        action='version',
        version='NLM CLI Phase 6.0'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Demo command
    demo_parser = subparsers.add_parser('demo', help='Run Phase 6 integration demo')
    demo_parser.add_argument('--steps', type=int, default=1000, help='Number of steps to simulate')
    demo_parser.add_argument('--config', help='Configuration file')
    
    # Interactive command
    subparsers.add_parser('interactive', help='Enter interactive brain exploration mode')
    
    # Benchmark command
    subparsers.add_parser('benchmark', help='Run performance benchmark')
    
    # Config management commands
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='config_command', help='Config commands')
    
    config_subparsers.add_parser('create', help='Create default configuration file')
    config_subparsers.add_parser('edit', help='Edit configuration file')
    
    # Session management commands
    session_parser = subparsers.add_parser('sessions', help='Session management')
    session_subparsers = session_parser.add_subparsers(dest='session_command', help='Session commands')
    
    session_subparsers.add_parser('list', help='List saved sessions')
    session_save_parser = session_subparsers.add_parser('save', help='Save current state')
    session_save_parser.add_argument('name', help='Session name')
    
    session_load_parser = session_subparsers.add_parser('load', help='Load session state')
    session_load_parser.add_argument('name', help='Session name')
    
    # Debug commands
    debug_parser = subparsers.add_parser('debug', help='Debug and diagnostic commands')
    debug_subparsers = debug_parser.add_subparsers(dest='debug_command', help='Debug commands')
    
    debug_subparsers.add_parser('info', help='Show system and brain information')
    debug_subparsers.add_parser('memory', help='Check memory system status')
    debug_subparsers.add_parser('neuromodulation', help='Check neuromodulation system status')
    
    args = parser.parse_args()
    
    # Print banner
    cli.print_banner()
    
    # Handle commands
    if not args.command:
        parser.print_help()
        return
        
    try:
        if args.command == 'demo':
            cli.run_demo(steps=args.steps, config_file=args.config)
            
        elif args.command == 'interactive':
            cli.run_interactive_mode()
            
        elif args.command == 'benchmark':
            cli.run_benchmark(config_file=args.config)
            
        elif args.command == 'config':
            if not args.config_command:
                print("Config command required. Available: create, edit")
                return
                
            if args.config_command == 'create':
                cli.create_default_config()
            elif args.config_command == 'edit':
                print("Config editing not yet implemented.")
                
        elif args.command == 'sessions':
            if not args.session_command:
                print("Session command required. Available: list, save, load")
                return
                
            if args.session_command == 'list':
                cli.list_sessions()
            elif args.session_command == 'save':
                print("Session saving not yet implemented in interactive mode.")
            elif args.session_command == 'load':
                print("Session loading not yet implemented.")
                
        elif args.command == 'debug':
            if not args.debug_command:
                print("Debug command required. Available: info, memory, neuromodulation")
                return
                
            if args.debug_command == 'info':
                if PYNLM_AVAILABLE:
                    config = pynlm.createDefaultConfig()
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    cli.print_brain_stats(brain)
            elif args.debug_command == 'memory':
                if PYNLM_AVAILABLE:
                    config = pynlm.createDefaultConfig()
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
                        wm = brain.getWorkingMemory()
                        print(f"Working memory: {wm.getActiveTraces()} active traces")
                    if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
                        em = brain.getEpisodicMemory()
                        print(f"Episodic memory: {em.getEpisodeCount()} episodes")
            elif args.debug_command == 'neuromodulation':
                if PYNLM_AVAILABLE:
                    config = pynlm.createDefaultConfig()
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    if hasattr(brain, 'getDopamine') and brain.getDopamine():
                        dopamine = brain.getDopamine()
                        print(f"Dopamine level: {dopamine.getLevel():.3f}")
                        
        else:
            print(f"Unknown command: {args.command}")
            
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Operation interrupted by user.{Colors.ENDC}")
    except Exception as e:
        print(f"{Colors.FAIL}Error: {e}{Colors.ENDC}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()