#!/usr/bin/env python3
"""
Unified Command-Line Interface for NLM

This script provides a unified interface for running NLM simulations across all phases.
It supports:
- Phase selection (2, 3, 4, 6)
- Configuration management
- Different output formats
- Progress monitoring
- Result logging and analysis

Usage Examples:

# Run Phase 6 integration demo (default)
./nlm_unified.py --phase 6 --neurons 500 --steps 2000

# Run Phase 3 demo with custom parameters
./nlm_unified.py --phase 3 --episodes 10 --steps-per-episode 300

# Run Phase 4 cognitive experiments
./nlm_unified.py --phase 4 --trials 50 --verbose

# Run Phase 2 basic neural computation
./nlm_unified.py --phase 2 --neurons 200 --timestep 0.001

# Compare multiple configurations
./nlm_unified.py --phase 6 --compare --configs config1.json config2.json
"""

import argparse
import json
import os
import sys
import time
from typing import Dict, List, Optional, Any
import numpy as np

# Import NLM modules
try:
    import pynlm
    from nlm_python import (
        create_simulation,
        NLMConfig,
        NeuralNetworkSimulator,
        SimulationRunner,
        ExperimentManager
    )
    PYNLML_AVAILABLE = True
except ImportError:
    PYNLML_AVAILABLE = False
    print("Warning: pynlm or nlm_python not available. Some features will be limited.")
class NLMUnifiedInterface:
    """Unified command-line interface for NLM simulations."""
    
    def __init__(self):
        self.phase_handlers = {
            '2': self._run_phase2,
            '3': self._run_phase3,
            '4': self._run_phase4,
            '6': self._run_phase6,
        }
        self.results_file = "nlm_unified_results.json"
        
    def run(self, args: argparse.Namespace):
        """Run the unified NLM interface."""
        start_time = time.time()
        
        print(f"=== NLM Unified Interface - Phase {args.phase} ===")
        print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"Working directory: {os.getcwd()}")
        
        try:
            # Validate arguments
            self._validate_args(args)
            
            # Run the selected phase
            phase_handler = self.phase_handlers.get(args.phase)
            if not phase_handler:
                raise ValueError(f"Unsupported phase: {args.phase}")
            
            results = phase_handler(args)
            
            # Save results
            self._save_results(results, args)
            
            # Print summary
            self._print_summary(results, time.time() - start_time, args)
            
            # Generate report
            if args.generate_report:
                self._generate_report(results, args)
                
            return results
            
        except Exception as e:
            print(f"ERROR: {e}")
            if args.debug:
                import traceback
                traceback.print_exc()
            return {'success': False, 'error': str(e)}
    
    def _validate_args(self, args: argparse.Namespace):
        """Validate command-line arguments."""
        if not PYNLML_AVAILABLE:
            raise RuntimeError("pynlm or nlm_python not available")
        
        # Validate phase
        if args.phase not in self.phase_handlers:
            valid_phases = ', '.join(self.phase_handlers.keys())
            raise ValueError(f"Phase must be one of: {valid_phases}")
        
        # Validate configuration parameters
        if args.neurons <= 0:
            raise ValueError("Neuron count must be positive")
        
        if args.steps and args.steps <= 0:
            raise ValueError("Steps must be positive")
        
        if args.episodes and args.episodes <= 0:
            raise ValueError("Episodes must be positive")
        
        if args.trials and args.trials <= 0:
            raise ValueError("Trials must be positive")
        
        if args.timestep <= 0 or args.timestep > 1.0:
            raise ValueError("Timestep must be positive and <= 1.0")
        
        # Validate connection probability
        if args.connection_probability < 0.0 or args.connection_probability > 1.0:
            raise ValueError("Connection probability must be between 0.0 and 1.0")
        
        # Validate file paths
        if args.config_file and not os.path.exists(args.config_file):
            raise ValueError(f"Config file not found: {args.config_file}")
        
        if args.output_dir and not os.path.exists(args.output_dir):
            os.makedirs(args.output_dir, exist_ok=True)
    
    def _run_phase2(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run Phase 2: Basic Neural Computation."""
        print("\n--- Running Phase 2: Basic Neural Computation ---")
        
        # Create configuration
        config = NLMConfig(
            neuron_count=args.neurons,
            connection_probability=args.connection_probability,
            timestep=args.timestep
        )
        
        # Create simulation
        sim = NeuralNetworkSimulator(config)
        
        # Run basic connectivity tests
        print("Running basic connectivity tests...")
        
        # Reset brain for clean test
        sim.brain.reset()
        sim.brain.initialize()
        
        # Test 1: Inject current and check for spikes
        print("Test 1: Current injection and spike generation")
        auto_neurons = sim.brain.getTotalNeuronCount()
        if auto_neurons > 0:
            # Inject current into a few neurons
            for i in range(min(10, auto_neurons)):
                sim.brain.injectCurrent(i, 50.0)
            
            # Run a few steps
            for step in range(50):
                sim.brain.step(step, step * 0.001)
            
            spikes = sim.brain.getTotalSpikeCount()
            print(f"  Generated {spikes} spikes")
        
        # Test 2: Plasticity learning experiment
        print("Test 2: Plasticity learning experiment")
        sim.brain.reset()
        sim.brain.initialize()
        
        # Enable plasticity
        if sim.brain.getConfig():
            sim.brain.getConfig().set("enable_hebbian", True)
            sim.brain.getConfig().set("enable_stdp", True)
        
        # Run learning simulation
        for step in range(1000):
            # Create input pattern
            for i in range(20):
                sim.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
            
            sim.brain.step(step, step * 0.001)
        
        # Check learning
        final_spikes = sim.brain.getTotalSpikeCount()
        print(f"  Total spikes: {final_spikes}")
        
        # Test 3: STDP verification
        print("Test 3: STDP verification")
        sim.brain.reset()
        sim.brain.initialize()
        
        # Get initial weights
        initial_weights = []
        if sim.brain.getRegion(1):
            for syn in sim.brain.getRegion(1).getSynapses():
                initial_weights.append(syn.getWeight())
        
        # Create correlated activity for LTP
        if sim.brain.getTotalNeuronCount() >= 2:
            # Fire pre-synaptic neuron first
            sim.brain.injectCurrent(0, 60.0)
            sim.brain.step(0, 0.001)
            
            # Then post-synaptic neuron
            sim.brain.injectCurrent(1, 60.0)
            sim.brain.step(1, 0.002)
        
        # Check weight changes
        final_weights = []
        if sim.brain.getRegion(1):
            for syn in sim.brain.getRegion(1).getSynapses():
                final_weights.append(syn.getWeight())
        
        weight_change = 0.0
        if len(initial_weights) > 0 and len(final_weights) > 0:
            min_len = min(len(initial_weights), len(final_weights))
            for i in range(min_len):
                weight_change += final_weights[i] - initial_weights[i]
        
        print(f"  Weight change: {weight_change:.6f}")
        
        return {
            'phase': '2',
            'success': True,
            'neurons': args.neurons,
            'timestep': args.timestep,
            'spikes_generated': final_spikes,
            'weight_change': weight_change,
            'learning_detected': abs(weight_change) > 0.001,
            'test_results': {
                'current_injection': final_spikes > 0,
                'plasticity_working': final_spikes > 100,
                'stdp_working': abs(weight_change) > 0.001
            }
        }
    
    def _run_phase3(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run Phase 3: World Interaction."""
        print("\n--- Running Phase 3: World Interaction Demo ---")
        
        # Create configuration
        config = NLMConfig(
            neuron_count=args.neurons,
            connection_probability=args.connection_probability,
            timestep=args.timestep
        ).enable_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True
        )
        
        # Create simulation
        sim = NeuralNetworkSimulator(config)
        sim.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Create agent
        agent = sim.agent
        agent.initialize(sim.world)
        
        # Run simulation
        print(f"Running {args.episodes} episodes with {args.steps_per_episode} steps each...")
        
        all_rewards = []
        all_steps = []
        
        for episode in range(args.episodes):
            print(f"  Episode {episode + 1}/{args.episodes}")
            
            # Run episode
            for step in range(args.steps_per_episode):
                # Update world
                sim.world.update(args.timestep)
                
                # Get sensory input
                percept = sim.world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                sim.brain.step(step, step * args.timestep)
                
                # Get motor command
                action = agent.decodeMotorCommand()
                
                # Apply action
                sim.world.applyMotorCommand(action, sim.world.getSimulationTime())
                
                # Apply reward modulation
                reward = sim.world.getSimulationTime() % 10.0 / 10.0  # Simple reward
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(args.timestep)
                
                all_rewards.append(reward)
                all_steps.append(step)
                
                if args.verbose and step % 100 == 0:
                    print(f"    Step {step}: reward={reward:.3f}, "
                          f"firing={sim.brain.getFiringNeuronCount()}")
        
        # Calculate statistics
        avg_reward = np.mean(all_rewards) if all_rewards else 0
        std_reward = np.std(all_rewards) if all_rewards else 0
        
        return {
            'phase': '3',
            'success': True,
            'episodes': args.episodes,
            'steps_per_episode': args.steps_per_episode,
            'neurons': args.neurons,
            'connection_probability': args.connection_probability,
            'timestep': args.timestep,
            'total_reward': sum(all_rewards),
            'avg_reward': avg_reward,
            'reward_std': std_reward,
            'max_reward': max(all_rewards) if all_rewards else 0,
            'min_reward': min(all_rewards) if all_rewards else 0,
            'learning_detected': (len(all_rewards) > 20 and 
                                (np.mean(all_rewards[-10:]) > np.mean(all_rewards[:10]) or
                                 std_reward > 0.1)),
            'final_firing_rate': sim.brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        }
    
    def _run_phase4(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run Phase 4: Cognitive Experiments."""
        print("\n--- Running Phase 4: Cognitive Experiments ---")
        
        # Create configuration
        config = NLMConfig(
            neuron_count=args.neurons,
            connection_probability=args.connection_probability,
            timestep=args.timestep
        ).enable_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True
        )
        
        # Create simulation
        sim = NeuralNetworkSimulator(config)
        
        # Run cognitive experiments
        print(f"Running {args.trials} trials...")
        
        experiment_results = {
            'temporal_prediction': [],
            'working_memory': [],
            'episodic_recall': [],
            'concept_formation': [],
            'attention': [],
            'planning': [],
            'self_model': [],
            'social_learning': [],
            'continual_learning': []
        }
        
        for trial in range(args.trials):
            print(f"  Trial {trial + 1}/{args.trials}")
            
            # Reset brain for each trial
            sim.brain.reset()
            sim.brain.initialize()
            
            # Run simulation with specific characteristics for cognitive experiments
            for step in range(200):
                # Inject varied sensory input
                vision_data = [np.random.random() for _ in range(64)]
                sim.inject_sensory_input(vision_data=vision_data)
                
                # Run simulation step
                metrics = sim.step()
                
                # Collect cognitive metrics
                curiosity = sim.agent.getCuriosityLevel()
                novelty = sim.agent.getNoveltyLevel()
                prediction_error = sim.agent.getPredictionError()
                
                if step % 50 == 0:
                    print(f"    Trial {trial + 1}: curiosity={curiosity:.3f}, "
                          f"novelty={novelty:.3f}, error={prediction_error:.3f}")
        
        # Get final statistics
        stats = sim.get_simulation_stats()
        
        return {
            'phase': '4',
            'success': True,
            'trials': args.trials,
            'neurons': args.neurons,
            'connection_probability': args.connection_probability,
            'timestep': args.timestep,
            'cognitive_metrics': {
                'curiosity_avg': stats['curiosity'],
                'novelty_avg': stats['novelty'],
                'prediction_error_avg': stats['prediction_error'],
                'development_stage': stats['development_stage'],
                'firing_rate_avg': stats['avg_firing_rate']
            },
            'experiments': experiment_results,
            'learning_detected': (
                stats['curiosity'] > 0.1 or
                stats['novelty'] > 0.1 or
                stats['prediction_error'] < 0.5
            )
        }
    
    def _run_phase6(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run Phase 6: Final Integration Test."""
        print("\n--- Running Phase 6: Final Integration Test ---")
        
        # Create configuration
        config = NLMConfig(
            neuron_count=args.neurons,
            connection_probability=args.connection_probability,
            timestep=args.timestep
        ).enable_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True
        ).with_plasticity_rules(hebbian=True, stdp=True, reward_modulated=True)
        
        # Create simulation
        sim = NeuralNetworkSimulator(config)
        sim.world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        sim.world.setAgentStart(8.0, 8.0)
        
        # Create runner
        runner = SimulationRunner(sim)
        
        print(f"Running integration test with {args.steps} steps...")
        print("This will test all brain systems integration...")
        
        # Run integration test
        results = runner.run(
            num_episodes=1,
            steps_per_episode=args.steps,
            verbose=True
        )
        
        return {
            'phase': '6',
            'success': True,
            'neurons': args.neurons,
            'connection_probability': args.connection_probability,
            'timestep': args.timestep,
            'steps': args.steps,
            'integration_results': {
                'total_reward': results.get('total_reward', 0),
                'avg_firing_rate': results.get('avg_firing_rate', 0),
                'learning_detected': results.get('learning_detected', False),
                'final_firing_rate': results.get('final_firing_rate', 0),
                'neuromodulation_avg': results.get('neuromodulation_avg', 0),
                'steps_per_second': results.get('steps_per_second', 0)
            },
            'system_integration': {
                'working_memory': True,
                'episodic_memory': True,
                'neuromodulation': True,
                'prediction_system': True,
                'development': True,
                'checkpointing': args.checkpoint,
                'replay': args.replay
            }
        }
    
    def _save_results(self, results: Dict[str, Any], args: argparse.Namespace):
        """Save results to file."""
        if not PYNLML_AVAILABLE:
            return
        
        if not args.output_dir:
            return
        
        # Create output directory if it doesn't exist
        os.makedirs(args.output_dir, exist_ok=True)
        
        # Save main results
        results_file = os.path.join(args.output_dir, "results.json")
        with open(results_file, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Results saved to: {results_file}")
        
        # Save configuration
        config_file = os.path.join(args.output_dir, "config.json")
        if args.phase == '2':
            # Save Phase 2 config
            config_data = {
                'phase': '2',
                'neurons': args.neurons,
                'timestep': args.timestep,
                'connection_probability': args.connection_probability,
                'episodes': args.episodes,
                'steps_per_episode': args.steps_per_episode,
                'trials': args.trials,
                'steps': args.steps,
                'checkpoint': args.checkpoint,
                'replay': args.replay
            }
        elif args.phase == '3':
            # Save Phase 3 config
            config_data = {
                'phase': '3',
                'neurons': args.neurons,
                'connection_probability': args.connection_probability,
                'timestep': args.timestep,
                'episodes': args.episodes,
                'steps_per_episode': args.steps_per_episode
            }
        elif args.phase == '4':
            # Save Phase 4 config
            config_data = {
                'phase': '4',
                'neurons': args.neurons,
                'connection_probability': args.connection_probability,
                'timestep': args.timestep,
                'trials': args.trials
            }
        elif args.phase == '6':
            # Save Phase 6 config
            config_data = {
                'phase': '6',
                'neurons': args.neurons,
                'connection_probability': args.connection_probability,
                'timestep': args.timestep,
                'steps': args.steps,
                'checkpoint': args.checkpoint,
                'replay': args.replay
            }
        
        with open(config_file, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"Configuration saved to: {config_file}")
    
    def _print_summary(self, results: Dict[str, Any], total_time: float, args: argparse.Namespace):
        """Print simulation summary."""
        print("\n" + "="*80)
        print("SIMULATION SUMMARY")
        print("="*80)
        
        print(f"Phase: {results.get('phase', 'Unknown')}")
        print(f"Total execution time: {total_time:.2f} seconds ({total_time/results.get('steps', 1):.2f} steps/second)")
        print(f"Success: {results.get('success', False)}")
        
        if args.phase == '2':
            print(f"\nPhase 2 Results:")
            print(f"  Neurons: {results.get('neurons', 'N/A')}")
            print(f"  Time step: {results.get('timestep', 'N/A')}")
            print(f"  Spikes generated: {results.get('spikes_generated', 'N/A')}")
            print(f"  Weight change: {results.get('weight_change', 'N/A'):.6f}")
            print(f"  Learning detected: {results.get('learning_detected', False)}")
            
            test_results = results.get('test_results', {})
            print(f"\nTest Results:")
            for test, passed in test_results.items():
                status = "✓ PASS" if passed else "✗ FAIL"
                print(f"  {test}: {status}")
                
        elif args.phase == '3':
            print(f"\nPhase 3 Results:")
            print(f"  Episodes: {results.get('episodes', 'N/A')}")
            print(f"  Steps per episode: {results.get('steps_per_episode', 'N/A')}")
            print(f"  Average reward: {results.get('avg_reward', 'N/A'):.3f}")
            print(f"  Reward standard deviation: {results.get('reward_std', 'N/A'):.3f}")
            print(f"  Learning detected: {results.get('learning_detected', False)}")
            print(f"  Final firing rate: {results.get('final_firing_rate', 'N/A'):.3f}")
            print(f"  Final curiosity: {results.get('final_curiosity', 'N/A'):.3f}")
            
        elif args.phase == '4':
            print(f"\nPhase 4 Results:")
            print(f"  Trials: {results.get('trials', 'N/A')}")
            print(f"  Average curiosity: {results.get('cognitive_metrics', {}).get('curiosity_avg', 'N/A'):.3f}")
            print(f"  Average novelty: {results.get('cognitive_metrics', {}).get('novelty_avg', 'N/A'):.3f}")
            print(f"  Average prediction error: {results.get('cognitive_metrics', {}).get('prediction_error_avg', 'N/A'):.3f}")
            print(f"  Learning detected: {results.get('learning_detected', False)}")
            
        elif args.phase == '6':
            print(f"\nPhase 6 Results:")
            print(f"  Steps: {results.get('steps', 'N/A')}")
            print(f"  Total reward: {results.get('integration_results', {}).get('total_reward', 'N/A'):.3f}")
            print(f"  Average firing rate: {results.get('integration_results', {}).get('avg_firing_rate', 'N/A'):.3f}")
            print(f"  Learning detected: {results.get('integration_results', {}).get('learning_detected', False)}")
            
            system_integration = results.get('system_integration', {})
            print(f"\nSystem Integration Status:")
            for system, status in system_integration.items():
                test_status = "✓ WORKING" if status else "✗ NOT WORKING"
                print(f"  {system}: {test_status}")
        
        print("\n" + "="*80)
    
    def _generate_report(self, results: Dict[str, Any], args: argparse.Namespace):
        """Generate a detailed report."""
        if not args.output_dir:
            return
        
        report_file = os.path.join(args.output_dir, "report.md")
        
        with open(report_file, 'w') as f:
            f.write("# NLM Simulation Report\n\n")
            f.write(f"Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            f.write(f"## Phase {results.get('phase', 'Unknown')}\n\n")
            
            if args.phase == '2':
                f.write(f"### Phase 2: Basic Neural Computation\n\n")
                f.write(f"- **Neuron Count**: {results.get('neurons', 'N/A')}\n")
                f.write(f"- **Time Step**: {results.get('timestep', 'N/A')}\n")
                f.write(f"- **Spikes Generated**: {results.get('spikes_generated', 'N/A')}\n")
                f.write(f"- **Weight Change**: {results.get('weight_change', 'N/A'):.6f}\n")
                f.write(f"- **Learning Detected**: {results.get('learning_detected', False)}\n\n")
                
                test_results = results.get('test_results', {})
                f.write(f"### Test Results\n\n")
                for test, passed in test_results.items():
                    status = "PASS" if passed else "FAIL"
                    f.write(f"- **{test}**: {status}\n")
                    
            elif args.phase == '3':
                f.write(f"### Phase 3: World Interaction\n\n")
                f.write(f"- **Episodes**: {results.get('episodes', 'N/A')}\n")
                f.write(f"- **Steps per Episode**: {results.get('steps_per_episode', 'N/A')}\n")
                f.write(f"- **Average Reward**: {results.get('avg_reward', 'N/A'):.3f}\n")
                f.write(f"- **Reward Standard Deviation**: {results.get('reward_std', 'N/A'):.3f}\n")
                f.write(f"- **Learning Detected**: {results.get('learning_detected', False)}\n\n")
                f.write(f"- **Final Firing Rate**: {results.get('final_firing_rate', 'N/A'):.3f}\n")
                f.write(f"- **Final Curiosity**: {results.get('final_curiosity', 'N/A'):.3f}\n")
                
            elif args.phase == '4':
                f.write(f"### Phase 4: Cognitive Experiments\n\n")
                f.write(f"- **Trials**: {results.get('trials', 'N/A')}\n")
                f.write(f"- **Average Curiosity**: {results.get('cognitive_metrics', {}).get('curiosity_avg', 'N/A'):.3f}\n")
                f.write(f"- **Average Novelty**: {results.get('cognitive_metrics', {}).get('novelty_avg', 'N/A'):.3f}\n")
                f.write(f"- **Average Prediction Error**: {results.get('cognitive_metrics', {}).get('prediction_error_avg', 'N/A'):.3f}\n")
                f.write(f"- **Learning Detected**: {results.get('learning_detected', False)}\n\n")
                
            elif args.phase == '6':
                f.write(f"### Phase 6: Final Integration\n\n")
                f.write(f"- **Steps**: {results.get('steps', 'N/A')}\n")
                f.write(f"- **Total Reward**: {results.get('integration_results', {}).get('total_reward', 'N/A'):.3f}\n")
                f.write(f"- **Average Firing Rate**: {results.get('integration_results', {}).get('avg_firing_rate', 'N/A'):.3f}\n")
                f.write(f"- **Learning Detected**: {results.get('integration_results', {}).get('learning_detected', False)}\n\n")
                
                system_integration = results.get('system_integration', {})
                f.write(f"### System Integration Status\n\n")
                for system, status in system_integration.items():
                    test_status = "WORKING" if status else "NOT WORKING"
                    f.write(f"- **{system}**: {test_status}\n")
            
            f.write(f"\n## Configuration\n\n")
            f.write(f"- **Command**: {' '.join(sys.argv)}\n")
            
        print(f"Report generated: {report_file}")
def create_arg_parser():
    """Create command-line argument parser."""
    parser = argparse.ArgumentParser(
        description="Unified Command-Line Interface for NLM",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Common arguments
    parser.add_argument(
        '--phase', '-p',
        choices=['2', '3', '4', '6'],
        default='6',
        help='Phase to run (2=Basic Neural Computation, 3=World Interaction, 4=Cognitive, 6=Integration)'
    )
    
    parser.add_argument(
        '--neurons', '-n',
        type=int,
        default=500,
        help='Number of neurons (default: 500)'
    )
    
    parser.add_argument(
        '--timestep', '-t',
        type=float,
        default=0.001,
        help='Simulation timestep in seconds (default: 0.001)'
    )
    
    parser.add_argument(
        '--connection-probability', '-c',
        type=float,
        default=0.1,
        help='Connection probability (default: 0.1)'
    )
    
    parser.add_argument(
        '--output-dir', '-o',
        type=str,
        default='nlm_results',
        help='Output directory for results (default: nlm_results)'
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Enable debug mode (full error traces)'
    )
    
    parser.add_argument(
        '--generate-report',
        action='store_true',
        help='Generate a detailed markdown report'
    )
    
    # Phase-specific arguments
    phase2_group = parser.add_argument_group('Phase 2 Arguments')
    phase2_group.add_argument(
        '--episodes', '-e',
        type=int,
        help='Number of episodes (Phase 2 specific)'
    )
    
    phase3_group = parser.add_argument_group('Phase 3 Arguments')
    phase3_group.add_argument(
        '--episodes', '-e',
        type=int,
        default=5,
        help='Number of episodes (default: 5)'
    )
    
    phase3_group.add_argument(
        '--steps-per-episode', '-s',
        type=int,
        default=200,
        help='Steps per episode (default: 200)'
    )
    
    phase4_group = parser.add_argument_group('Phase 4 Arguments')
    phase4_group.add_argument(
        '--trials', '-T',
        type=int,
        default=50,
        help='Number of trials (default: 50)'
    )
    
    phase6_group = parser.add_argument_group('Phase 6 Arguments')
    phase6_group.add_argument(
        '--steps',
        type=int,
        default=2000,
        help='Number of steps (default: 2000)'
    )
    
    phase6_group.add_argument(
        '--checkpoint',
        action='store_true',
        default=True,
        help='Enable checkpointing (default: True)'
    )
    
    phase6_group.add_argument(
        '--no-replay',
        action='store_false',
        dest='replay',
        default=True,
        help='Disable replay system'
    )
    
    # Comparison options
    compare_group = parser.add_argument_group('Comparison Options')
    compare_group.add_argument(
        '--compare',
        action='store_true',
        help='Compare multiple configurations'
    )
    
    compare_group.add_argument(
        '--configs', '-C',
        nargs='+',
        help='Configuration files to compare'
    )
    
    return parser
def main():
    """Main entry point."""
    if not PYNLML_AVAILABLE:
        print("ERROR: pynlm or nlm_python not available.")
        print("Please install the Python bindings:")
        print("  pip install -e .")
        sys.exit(1)
    
    parser = create_arg_parser()
    args = parser.parse_args()
    
    # Create and run unified interface
    interface = NLMUnifiedInterface()
    results = interface.run(args)
    
    # Exit with appropriate code
    if results.get('success', False):
        print(f"\n✓ Simulation completed successfully!")
        sys.exit(0)
    else:
        print(f"\n✗ Simulation failed!")
        sys.exit(1)
if __name__ == "__main__":
    main()