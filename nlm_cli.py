#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A comprehensive command-line interface for the Neural Learning Machine (NLM) simulation framework.
Provides easy access to all NLM functionality including Phase 6 integration tests, learning experiments,
and system monitoring.
"""

import argparse
import json
import sys
import os
import time
from typing import Optional, Dict, Any
from pathlib import Path

# Add the current directory to Python path to import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not found. Please install the Python bindings.")
    print("Run: pip install -e .")

class NLMCLI:
    """Main CLI class for NLM operations."""
    
    def __init__(self):
        self.config = {}
        self.brain = None
        self.world = None
        self.agent = None
        
    def create_brain(self, config_file: Optional[str] = None) -> None:
        """Create and initialize a brain."""
        try:
            if PYNLM_AVAILABLE:
                config = pynlm.createDefaultConfig()
                
                if config_file and os.path.exists(config_file):
                    config.loadFromFile(config_file)
                
                self.brain = pynlm.createBrain(config)
                self.brain.initialize()
                
                print(f"✓ Brain created successfully")
                print(f"  - Neurons: {self.brain.getTotalNeuronCount()}")
                print(f"  - Regions: {self.brain.getRegionCount()}")
                print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
            else:
                print("Error: pynlm not available")
                sys.exit(1)
        except Exception as e:
            print(f"Error creating brain: {e}")
            sys.exit(1)
    
    def create_complete_setup(self, neurons: int = 1000, world_size: tuple = (10, 10), 
                             vision_size: tuple = (8, 8), steps: int = 1000) -> None:
        """Create complete brain, world, and agent setup."""
        try:
            if PYNLM_AVAILABLE:
                # Create brain
                config = pynlm.createDefaultConfig()
                config.set("neuron_count", neurons)
                config.set("region_count", 1)
                config.set("connection_probability", 0.15)
                
                self.brain = pynlm.createBrain(config)
                self.brain.initialize()
                
                # Create world
                self.world = pynlm.createSimpleWorld()
                self.world.configure(world_size[0], world_size[1], 
                                   vision_size[0], vision_size[1])
                self.world.reset()
                
                # Create agent
                self.agent = pynlm.createAgentBrain(self.brain)
                self.agent.initialize(self.world)
                
                print(f"✓ Complete setup created")
                print(f"  - Brain: {self.brain.getTotalNeuronCount()} neurons")
                print(f"  - World: {world_size[0]}x{world_size[1]} units")
                print(f"  - Vision: {vision_size[0]}x{vision_size[1]} pixels")
                
                # Enable common features
                self.agent.enableRewardModulation(True)
                self.agent.enableCuriosity(True)
                self.agent.enableDevelopment(True)
                
                print(f"  - Features enabled: reward modulation, curiosity, development")
            else:
                print("Error: pynlm not available")
                sys.exit(1)
        except Exception as e:
            print(f"Error creating complete setup: {e}")
            sys.exit(1)
    
    def run_simulation(self, steps: int, save_checkpoint: Optional[str] = None) -> Dict[str, Any]:
        """Run simulation for specified number of steps."""
        if not self.brain or not self.world or not self.agent:
            print("Error: No setup exists. Use --create-brain, --create-world, or --create-setup first.")
            sys.exit(1)
        
        try:
            print(f"Starting simulation for {steps} steps...")
            
            # Track metrics
            metrics = {
                'steps_completed': 0,
                'total_reward': 0.0,
                'avg_firing_rate': 0.0,
                'max_firing_rate': 0.0,
                'total_spikes': 0,
                'active_neurons': 0,
                'development_stage': 0
            }
            
            start_time = time.time()
            
            for step in range(steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Brain thinks
                self.brain.step(step)
                
                # Get action
                action = self.agent.decodeMotorCommand()
                
                # Apply action in world
                world_time = self.world.getSimulationTime()
                action_result = self.world.applyMotorCommand(action, world_time)
                
                # Apply reward modulation
                self.agent.applyRewardModulation(action_result.reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect metrics
                metrics['total_reward'] += action_result.reward
                metrics['total_spikes'] = self.brain.getTotalSpikeCount()
                metrics['active_neurons'] = self.brain.getFiringNeuronCount()
                metrics['development_stage'] = int(self.agent.getDevelopmentalStage())
                
                current_firing_rate = self.brain.getAverageFiringRate()
                metrics['avg_firing_rate'] = (metrics['avg_firing_rate'] * step + current_firing_rate) / (step + 1)
                metrics['max_firing_rate'] = max(metrics['max_firing_rate'], current_firing_rate)
                
                if step % 100 == 0:
                    print(f"  Step {step:4d}: "
                          f"Firing: {metrics['active_neurons']:3d}, "
                          f"Reward: {action_result.reward:+.3f}, "
                          f"Stage: {metrics['development_stage']}")
                
                # Save checkpoint if requested
                if save_checkpoint and step % 500 == 0:
                    checkpoint_path = save_checkpoint.replace('.bin', f'_{step}.bin')
                    try:
                        if self.brain.save(checkpoint_path):
                            print(f"  Checkpoint saved: {checkpoint_path}")
                    except Exception as e:
                        print(f"  Warning: Failed to save checkpoint: {e}")
            
            end_time = time.time()
            metrics['total_time'] = end_time - start_time
            metrics['steps_completed'] = steps
            metrics['avg_firing_rate'] = metrics['avg_firing_rate'] / steps if steps > 0 else 0.0
            metrics['total_reward_per_step'] = metrics['total_reward'] / steps if steps > 0 else 0.0
            
            print(f"Simulation completed in {metrics['total_time']:.2f} seconds")
            print(f"Performance: {steps / metrics['total_time']:.1f} steps/second")
            
            return metrics
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return {}
    
    def run_brain_test(self, test_type: str, steps: int = 1000) -> Dict[str, Any]:
        """Run specific brain test."""
        try:
            if test_type == "connectivity":
                return self._run_connectivity_test(steps)
            elif test_type == "plasticity":
                return self._run_plasticity_test(steps)
            elif test_type == "stdp":
                return self._run_stdp_test(steps)
            elif test_type == "integration":
                return self._run_integration_test()
            else:
                print(f"Unknown test type: {test_type}")
                return {}
        except Exception as e:
            print(f"Error during brain test: {e}")
            return {}
    
    def _run_connectivity_test(self, steps: int) -> Dict[str, Any]:
        """Test basic neural connectivity."""
        print(f"Running connectivity test for {steps} steps...")
        
        if not self.brain:
            self.create_brain()
        
        # Inject current into sensory neurons
        for i in range(min(20, self.brain.getTotalNeuronCount() // 4)):
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        
        # Run steps
        for step in range(steps):
            self.brain.step(step)
        
        result = {
            'test_type': 'connectivity',
            'steps': steps,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'success': self.brain.getFiringNeuronCount() > 0
        }
        
        print(f"Connectivity test completed:")
        print(f"  - Spikes: {result['total_spikes']}")
        print(f"  - Firing neurons: {result['firing_neurons']}")
        print(f"  - Average firing rate: {result['avg_firing_rate']:.3f}")
        
        return result
    
    def _run_plasticity_test(self, steps: int) -> Dict[str, Any]:
        """Test synaptic plasticity."""
        print(f"Running plasticity test for {steps} steps...")
        
        if not self.brain:
            self.create_brain()
        
        # Record initial weights
        initial_weights = []
        if hasattr(self.brain, 'getRegion') and self.brain.getRegion(0):
            for syn in self.brain.getRegion(0).getSynapses():
                initial_weights.append(syn.getWeight())
        
        # Run simulation with plasticity
        for step in range(steps):
            # Inject sensory input
            for i in range(min(20, self.brain.getTotalNeuronCount() // 4)):
                self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
            
            self.brain.step(step)
        
        # Record final weights
        final_weights = []
        if hasattr(self.brain, 'getRegion') and self.brain.getRegion(0):
            for syn in self.brain.getRegion(0).getSynapses():
                final_weights.append(syn.getWeight())
        
        # Compute statistics
        result = {
            'test_type': 'plasticity',
            'steps': steps,
            'initial_weights_count': len(initial_weights),
            'final_weights_count': len(final_weights),
            'initial_mean_weight': sum(initial_weights) / len(initial_weights) if initial_weights else 0.0,
            'final_mean_weight': sum(final_weights) / len(final_weights) if final_weights else 0.0,
            'weight_change': (sum(final_weights) / len(final_weights) if final_weights else 0.0) - 
                           (sum(initial_weights) / len(initial_weights) if initial_weights else 0.0)
        }
        
        print(f"Plasticity test completed:")
        print(f"  - Initial mean weight: {result['initial_mean_weight']:.4f}")
        print(f"  - Final mean weight: {result['final_mean_weight']:.4f}")
        print(f"  - Weight change: {result['weight_change']:+.4f}")
        print(f"  - {'✓' if abs(result['weight_change']) > 0.001 else '✗'} Learning detected")
        
        return result
    
    def _run_stdp_test(self, steps: int) -> Dict[str, Any]:
        """Test STDP (Spike-Timing Dependent Plasticity)."""
        print(f"Running STDP test for {steps} steps...")
        
        if not self.brain:
            self.create_brain()
        
        # Get synapses for testing
        synapses = []
        if hasattr(self.brain, 'getRegion') and self.brain.getRegion(0):
            synapses = list(self.brain.getRegion(0).getSynapses())[:5]
        
        if len(synapses) < 2:
            print("Error: Not enough synapses for STDP test")
            return {}
        
        # Record initial weights
        initial_weights = [syn.getWeight() for syn in synapses]
        
        # Create correlated activity for LTP
        neurons = []
        if hasattr(self.brain, 'getRegion') and self.brain.getRegion(0):
            neurons = list(self.brain.getRegion(0).getAllNeurons())[:2]
        
        # Run STDP protocol
        for trial in range(steps // 2):
            if len(neurons) >= 2:
                pre = neurons[0]
                post = neurons[1]
                
                # Pre fires first
                pre.injectCurrent(60.0)
                self.brain.step(trial * 2, trial * 2 * 0.001)
                
                # Then post fires
                post.injectCurrent(60.0)
                self.brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
        
        # Record final weights
        final_weights = [syn.getWeight() for syn in synapses]
        
        # Compute delta
        total_delta = sum(final_weights) - sum(initial_weights)
        
        result = {
            'test_type': 'stdp',
            'steps': steps,
            'initial_weights': initial_weights,
            'final_weights': final_weights,
            'total_delta': total_delta,
            'learning_detected': abs(total_delta) > 0.001
        }
        
        print(f"STDP test completed:")
        print(f"  - Total weight change: {total_delta:+.4f}")
        print(f"  - {'✓' if result['learning_detected'] else '✗'} STDP working")
        
        return result
    
    def _run_integration_test(self) -> Dict[str, Any]:
        """Run integration test (requires Phase6 functionality)."""
        print("Running integration test...")
        
        try:
            # Try to import Phase6 functionality
            from src.experiments.Phase6IntegratedExperiment import Phase6IntegratedExperiment, Phase6Config
            
            experiment = Phase6IntegratedExperiment()
            
            config = Phase6Config()
            config.neuronCount = 500
            config.maxSteps = 500
            config.enableCheckpointing = False
            config.enableReplay = False
            config.enableDevelopment = False
            
            result = experiment.run(config)
            
            print("Integration test completed:")
            print(f"  - Working Memory: {'✓' if result.memoryWorkingMemoryIntegrated else '✗'}")
            print(f"  - Episodic Memory: {'✓' if result.memoryEpisodicMemoryIntegrated else '✗'}")
            print(f"  - Neuromodulation: {'✓' if result.neuromodulationIntegrated else '✗'}")
            print(f"  - Prediction: {'✓' if result.predictionIntegrated else '✗'}")
            print(f"  - Development: {'✓' if result.developmentIntegrated else '✗'}")
            
            return {
                'test_type': 'integration',
                'result': result
            }
            
        except ImportError:
            print("Integration test requires Phase6 experiment module")
            return {'test_type': 'integration', 'error': 'Module not available'}
    
    def show_brain_status(self) -> None:
        """Display current brain status and metrics."""
        if not self.brain:
            print("No brain exists. Create one first.")
            return
        
        print("=== Brain Status ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Active neurons: {self.brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"Excitatory/Inhibitory ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"Developmental stage: {int(self.brain.getDevelopmentalStage())}")
        
        if self.agent:
            print("\n=== Agent Status ===")
            print(f"Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"Curiosity level: {self.agent.getCuriosityLevel():.3f}")
            print(f"Novelty level: {self.agent.getNoveltyLevel():.3f}")
            print(f"Prediction error: {self.agent.getPredictionError():.3f}")
            print(f"Development enabled: {self.agent.isDevelopmentEnabled()}")
            print(f"Curiosity enabled: {self.agent.isCuriosityEnabled()}")
    
    def save_config(self, filepath: str) -> None:
        """Save current configuration to file."""
        config_data = {
            'brain': {
                'neuron_count': self.brain.getTotalNeuronCount() if self.brain else 0,
                'synapse_count': self.brain.getTotalSynapseCount() if self.brain else 0,
            },
            'world': {
                'width': self.world.getWidth() if self.world else 0,
                'height': self.world.getHeight() if self.world else 0,
            },
            'agent': {
                'neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
                'curiosity': self.agent.getCuriosityLevel() if self.agent else 0.0,
            }
        }
        
        try:
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            print(f"Configuration saved to {filepath}")
        except Exception as e:
            print(f"Error saving configuration: {e}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Neural Learning Machine",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm --create-brain                    Create and initialize a simple brain
  nlm --create-setup                    Create complete brain+world+agent setup
  nlm --simulate 1000                  Run simulation for 1000 steps
  nlm --test connectivity 500           Test neural connectivity
  nlm --test plasticity 2000            Test synaptic plasticity
  nlm --test stdp 1000                  Test STDP learning
  nlm --show-status                     Show current system status
  nlm --save-config config.json         Save configuration to file

For more help on specific commands, use:
  nlm [command] --help
        """.strip()
    )
    
    # Create subparsers for subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    brain_parser = subparsers.add_parser('create-brain', help='Create and initialize a brain')
    brain_parser.add_argument('config_file', nargs='?', help='Configuration file (optional)')
    
    # Create setup command
    setup_parser = subparsers.add_parser('create-setup', help='Create complete brain+world+agent setup')
    setup_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    setup_parser.add_argument('--world-width', type=int, default=10, help='World width')
    setup_parser.add_argument('--world-height', type=int, default=10, help='World height')
    setup_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
    setup_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
    simulate_parser.add_argument('steps', type=int, help='Number of simulation steps')
    simulate_parser.add_argument('--checkpoint', help='Save checkpoint file path')
    simulate_parser.add_argument('--load', help='Load checkpoint file path')
    
    # Test commands
    test_parser = subparsers.add_parser('test', help='Run brain tests')
    test_subparsers = test_parser.add_subparsers(dest='test_type', help='Test type')
    
    connectivity_test = test_subparsers.add_parser('connectivity', help='Test neural connectivity')
    connectivity_test.add_argument('steps', type=int, nargs='?', default=1000, help='Number of steps')
    
    plasticity_test = test_subparsers.add_parser('plasticity', help='Test synaptic plasticity')
    plasticity_test.add_argument('steps', type=int, nargs='?', default=2000, help='Number of steps')
    
    stdp_test = test_subparsers.add_parser('stdp', help='Test STDP learning')
    stdp_test.add_argument('steps', type=int, nargs='?', default=1000, help='Number of steps')
    
    integration_test = test_subparsers.add_parser('integration', help='Run integration test')
    
    # Status and utility commands
    status_parser = subparsers.add_parser('show-status', help='Show current system status')
    
    config_parser = subparsers.add_parser('save-config', help='Save configuration to file')
    config_parser.add_argument('filepath', help='Configuration file path')
    
    # General options
    parser.add_argument('--version', action='version', version='NLM CLI v0.1.0')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Execute command
    try:
        if args.command == 'create-brain':
            cli.create_brain(args.config_file)
        elif args.command == 'create-setup':
            cli.create_complete_setup(
                neurons=args.neurons,
                world_size=(args.world_width, args.world_height),
                vision_size=(args.vision_width, args.vision_height)
            )
        elif args.command == 'simulate':
            cli.run_simulation(args.steps, args.checkpoint)
        elif args.command == 'test':
            if args.test_type:
                cli.run_brain_test(args.test_type, args.steps if hasattr(args, 'steps') else 1000)
            else:
                parser.print_help()
        elif args.command == 'show-status':
            cli.show_brain_status()
        elif args.command == 'save-config':
            cli.save_config(args.filepath)
        else:
            parser.print_help()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()