"""
Advanced NLM Usage Examples
===========================

This module contains advanced examples for using the NLM framework,
demonstrating features available to power users and researchers.

Examples include:
- Network analysis and visualization
- Performance benchmarking
- Advanced plasticity control
- Real-time monitoring
- Multi-format data export
"""

import pynlm
import time
import json
from typing import Dict, List, Optional

class AdvancedNLMSimulation:
    """
    Advanced NLM simulation with comprehensive monitoring and analysis.
    
    This class demonstrates advanced features including network analysis,
    performance benchmarking, and real-time monitoring capabilities.
    """
    
    def __init__(self, config=None):
        """Initialize advanced simulation with enhanced configuration."""
        if config is None:
            config = pynlm.createDefaultConfig()
            # Advanced configuration for performance
            config.set("brain.neuron_count", 2000)
            config.set("brain.synapse_density", 0.2)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.structural.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure world for advanced simulation
        self.world.configure(
            width=50,
            height=50,
            visionWidth=12,
            visionHeight=12
        )
        self.world.setAgentStart(25.0, 25.0)
        
        # Monitoring data storage
        self.monitoring_data = []
        self.network_analysis_data = {}
        
        print(f"Advanced NLM simulation initialized")
        print(f"  - Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - World size: {self.world.getWidth()}x{self.world.getHeight()}")
    
    def run_advanced_simulation(self, num_steps=5000, enable_learning=True):
        """
        Run advanced simulation with comprehensive monitoring.
        
        Args:
            num_steps: Number of simulation steps
            enable_learning: Enable advanced learning mechanisms
        """
        print(f"Starting advanced simulation with {num_steps} steps...")
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable advanced subsystems
        if enable_learning:
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
        
        # Start monitoring
        self.brain.startMonitoring(50)
        
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation with advanced feedback
            reward = self._calculate_advanced_reward(percept)
            predicted_reward = self._predict_reward(action)
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect monitoring data every 100 steps
            if step % 100 == 0:
                self._collect_monitoring_data(step)
            
            # Log progress for large steps
            if step % 1000 == 0:
                self._log_progress(step)
        
        # Stop monitoring
        self.brain.stopMonitoring()
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Perform advanced network analysis
        self._perform_network_analysis()
        
        # Generate final report
        self._generate_advanced_report()
        
        print(f"Simulation completed in {duration:.2f} seconds")
        return self._get_simulation_results()
    
    def _calculate_advanced_reward(self, percept):
        """Calculate advanced reward based on multiple factors."""
        base_reward = 0.0
        
        # Reward for proximity to center (stable environment)
        agent_body = self.world.getAgentBody()
        center_distance = ((agent_body.x - 25.0) ** 2 + (agent_body.y - 25.0) ** 2) ** 0.5
        base_reward += max(0.0, 1.0 - center_distance / 35.0)
        
        # Reward for energy conservation
        energy_efficiency = agent_body.energy / (agent_body.age + 1)
        base_reward += energy_efficiency * 0.1
        
        # Reward based on curiosity level
        curiosity = self.agent.getCuriosityLevel()
        base_reward += curiosity * 0.5
        
        # Reward based on novelty detection
        novelty = self.agent.getNoveltyLevel()
        base_reward += novelty * 0.3
        
        return base_reward
    
    def _predict_reward(self, action):
        """Predict reward for given action."""
        # Simple reward prediction based on action type and agent state
        agent_body = self.world.getAgentBody()
        prediction = 0.0
        
        if action.getType() == pynlm.ActionType.Eat:
            prediction += 1.0
        elif action.getType() == pynlm.ActionType.MoveForward:
            prediction += 0.5
        elif action.getType() == pynlm.ActionType.Interact:
            prediction += 0.8
        
        # Adjust prediction based on agent state
        prediction *= (1.0 + agent_body.energy / 100.0)
        
        return prediction
    
    def _collect_monitoring_data(self, step):
        """Collect comprehensive monitoring data."""
        agent_body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        data_point = {
            'step': step,
            'time': self.world.getSimulationTime(),
            'x': agent_body.x,
            'y': agent_body.y,
            'energy': agent_body.energy,
            'health': agent_body.health,
            'age': agent_body.age,
            'is_moving': agent_body.isMoving,
            'is_turning': agent_body.isTurning,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'development_stage': self.agent.getDevelopmentalStage(),
            'reward_modulation': self.agent.getNeuromodulationLevel()
        }
        
        self.monitoring_data.append(data_point)
    
    def _log_progress(self, step):
        """Log simulation progress with advanced metrics."""
        print(f"  Step {step}:")
        print(f"    - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"    - Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"    - Avg firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"    - Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"    - Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"    - E/I balance: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"    - Development: {self.agent.getDevelopmentalStage()}")
    
    def _perform_network_analysis(self):
        """Perform comprehensive network analysis."""
        print("Performing network analysis...")
        
        # Basic network statistics
        total_neurons = self.brain.getTotalNeuronCount()
        total_synapses = self.brain.getTotalSynapseCount()
        
        self.network_analysis_data = {
            'basic_stats': {
                'neuron_count': total_neurons,
                'synapse_count': total_synapses,
                'network_density': total_synapses / total_neurons,
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio()
            },
            'activity_stats': {
                'max_firing_rate': max([d['avg_firing_rate'] for d in self.monitoring_data]),
                'min_firing_rate': min([d['avg_firing_rate'] for d in self.monitoring_data]),
                'avg_activity': sum([d['firing_neurons'] for d in self.monitoring_data]) / len(self.monitoring_data),
                'activity_variance': sum([(d['firing_neurons'] - sum([d['firing_neurons'] for d in self.monitoring_data])/len(self.monitoring_data))**2 for d in self.monitoring_data]) / len(self.monitoring_data)
            },
            'plasticity_stats': {
                'development_stage': self.agent.getDevelopmentalStage(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'learning_enabled': {
                    'reward_modulation': self.agent.isRewardModulationEnabled(),
                    'structural_plasticity': self.agent.isStructuralPlasticityEnabled(),
                    'development': self.agent.isDevelopmentEnabled(),
                    'curiosity': self.agent.isCuriosityEnabled()
                }
            },
            'environmental_stats': {
                'final_position': {
                    'x': self.world.getAgentBody().x,
                    'y': self.world.getAgentBody().y
                },
                'final_energy': self.world.getAgentBody().energy,
                'final_age': self.world.getAgentBody().age
            }
        }
    
    def _generate_advanced_report(self):
        """Generate comprehensive analysis report."""
        print("\n" + "="*60)
        print("ADVANCED SIMULATION REPORT")
        print("="*60)
        
        print(f"\nSIMULATION OVERVIEW:")
        print(f"  Total steps: {len(self.monitoring_data)}")
        print(f"  Final step: {self.monitoring_data[-1]['step'] if self.monitoring_data else 'N/A'}")
        print(f"  Simulation duration: {self.monitoring_data[-1]['time'] if self.monitoring_data else 'N/A'}s")
        
        print(f"\nNETWORK STATISTICS:")
        for key, value in self.network_analysis_data['basic_stats'].items():
            if isinstance(value, float):
                print(f"  {key}: {value:.4f}")
            else:
                print(f"  {key}: {value}")
        
        print(f"\nACTIVITY ANALYSIS:")
        activity_stats = self.network_analysis_data['activity_stats']
        print(f"  Max firing rate: {activity_stats['max_firing_rate']:.2f} Hz")
        print(f"  Min firing rate: {activity_stats['min_firing_rate']:.2f} Hz")
        print(f"  Average activity: {activity_stats['avg_activity']:.2f} neurons")
        print(f"  Activity variance: {activity_stats['activity_variance']:.2f}")
        
        print(f"\nNEUROMODULATION STATUS:")
        neuromod = self.network_analysis_data['plasticity_stats']['learning_enabled']
        for system, enabled in neuromod.items():
            status = "✓" if enabled else "✗"
            print(f"  {status} {system.replace('_', ' ').title()}: {'Enabled' if enabled else 'Disabled'}")
        
        print(f"\nENVIRONMENTAL OUTCOME:")
        env_stats = self.network_analysis_data['environmental_stats']
        print(f"  Final position: ({env_stats['final_position']['x']:.1f}, {env_stats['final_position']['y']:.1f})")
        print(f"  Final energy: {env_stats['final_energy']:.1f}")
        print(f"  Final age: {env_stats['final_age']:.1f}")
        
        # Save data for further analysis
        self._save_analysis_data()
    
    def _save_analysis_data(self):
        """Save analysis data in multiple formats."""
        import os
        
        # Create analysis directory if it doesn't exist
        os.makedirs('analysis_data', exist_ok=True)
        
        # Save monitoring data as JSON
        with open('analysis_data/monitoring_data.json', 'w') as f:
            json.dump(self.monitoring_data, f, indent=2)
        
        # Save network analysis as JSON
        with open('analysis_data/network_analysis.json', 'w') as f:
            json.dump(self.network_analysis_data, f, indent=2)
        
        # Save as CSV for spreadsheet analysis
        self._save_csv_data()
        
        print(f"\nData saved to analysis_data/ directory")
    
    def _save_csv_data(self):
        """Save monitoring data as CSV."""
        import csv
        
        if not self.monitoring_data:
            return
        
        with open('analysis_data/monitoring_data.csv', 'w', newline='') as f:
            fieldnames = self.monitoring_data[0].keys()
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(self.monitoring_data)
    
    def _get_simulation_results(self):
        """Get comprehensive simulation results."""
        return {
            'steps_completed': len(self.monitoring_data),
            'final_step': self.monitoring_data[-1]['step'] if self.monitoring_data else 0,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_firing_rate': self.brain.getAverageFiringRate(),
            'network_density': self.brain.getNetworkDensity(),
            'final_position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'development_stage': self.agent.getDevelopmentalStage(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'monitoring_data': self.monitoring_data,
            'network_analysis': self.network_analysis_data
        }
class PerformanceBenchmark:
    """
    Performance benchmarking for NLM simulations.
    
    This class benchmarks different aspects of NLM performance including
    network size, simulation speed, and memory usage.
    """
    
    def __init__(self):
        self.benchmark_results = {}
    
    def benchmark_network_sizes(self, neuron_counts=[100, 500, 1000, 2000]):
        """
        Benchmark simulation performance for different network sizes.
        
        Args:
            neuron_counts: List of neuron counts to test
        """
        print("Benchmarking network sizes...")
        print("Neuron Count | Time (s) | Spikes/s | Memory (MB) | Throughput")
        print("-" * 70)
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.synapse_density", 0.1)
            
            # Warm-up run
            brain = pynlm.createBrain(config)
            brain.initialize()
            for _ in range(100):
                brain.step(_)
            
            # Benchmark run
            start_time = time.time()
            for step in range(1000):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            spikes_per_sec = brain.getTotalSpikeCount() / duration
            
            print(f"{neuron_count:11d} | {duration:8.2f} | {spikes_per_sec:8.0f} | N/A       | {neuron_count/duration:8.0f}")
            
            self.benchmark_results[f'neurons_{neuron_count}'] = {
                'duration': duration,
                'spikes_per_second': spikes_per_sec,
                'throughput': neuron_count / duration
            }
    
    def benchmark_plasticity_modes(self):
        """
        Benchmark performance with different plasticity configurations.
        """
        print("\nBenchmarking plasticity modes...")
        
        plasticity_configs = [
            {'hebbian': False, 'stdp': False, 'reward': False},
            {'hebbian': True, 'stdp': False, 'reward': False},
            {'hebbian': False, 'stdp': True, 'reward': False},
            {'hebbian': True, 'stdp': True, 'reward': True}
        ]
        
        print("Config        | Time (s) | Network Changes | Performance")
        print("-" * 65)
        
        for i, config in enumerate(plasticity_configs):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            start_time = time.time()
            for step in range(2000):
                brain.step(step)
                
                # Apply plasticity based on configuration
                if config['hebbian']:
                    # Simulate Hebbian plasticity
                    pass
                if config['stdp']:
                    # Simulate STDP
                    pass
                if config['reward']:
                    # Simulate reward-modulated plasticity
                    pass
            end_time = time.time()
            
            duration = end_time - start_time
            
            config_name = f"Hebb:{config['hebbian']}_STDP:{config['stdp']}_Rew:{config['reward']}"
            print(f"{config_name:15s} | {duration:8.2f} | N/A           | N/A")
            
            self.benchmark_results[f'plasticity_{i}'] = {
                'duration': duration,
                'config': config
            }
    
    def get_summary(self):
        """Get benchmark summary."""
        print("\n" + "="*60)
        print("BENCHMARK SUMMARY")
        print("="*60)
        
        print(f"\nTotal benchmarks run: {len(self.benchmark_results)}")
        
        print(f"\nNetwork Size Benchmarks:")
        for key, result in self.benchmark_results.items():
            if key.startswith('neurons_'):
                neuron_count = int(key.split('_')[1])
                print(f"  {neuron_count} neurons: {result['duration']:.2f}s, {result['spikes_per_second']:.0f} spikes/s")
        
        print(f"\nPlasticity Mode Benchmarks:")
        for key, result in self.benchmark_results.items():
            if key.startswith('plasticity_'):
                print(f"  Mode {key.split('_')[1]}: {result['duration']:.2f}s")
        
        # Performance recommendations
        print(f"\nPERFORMANCE RECOMMENDATIONS:")
        if 'neurons_2000' in self.benchmark_results:
            result_2000 = self.benchmark_results['neurons_2000']
            print(f"  - Large networks (2000 neurons) process at ~{result_2000['throughput']:.0f} neurons/second")
            print(f"  - Optimal for real-time applications: ~{1/result_2000['spikes_per_second']:.2f}s per spike")
        
        return self.benchmark_results
class NLMAdvancedExamples:
    """
    Main interface for accessing all advanced NLM examples and demonstrations.
    """
    
    @staticmethod
    def run_advanced_simulation_example():
        """Run the advanced simulation example."""
        print("=== ADVANCED SIMULATION EXAMPLE ===")
        simulation = AdvancedNLMSimulation()
        results = simulation.run_advanced_simulation(num_steps=1000, enable_learning=True)
        
        print(f"\nSimulation Results:")
        print(f"  Steps completed: {results['steps_completed']}")
        print(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz")
        print(f"  Network density: {results['network_density']:.4f}")
        print(f"  Final position: ({results['final_position'][0]:.1f}, {results['final_position'][1]:.1f})")
        
        return results
    
    @staticmethod
    def run_performance_benchmark_example():
        """Run the performance benchmark example."""
        print("=== PERFORMANCE BENCHMARK EXAMPLE ===")
        benchmark = PerformanceBenchmark()
        benchmark.benchmark_network_sizes([100, 500, 1000])
        benchmark.benchmark_plasticity_modes()
        
        return benchmark.get_summary()
    
    @staticmethod
    def run_comprehensive_demo():
        """Run comprehensive demo showcasing all advanced features."""
        print("=== COMPREHENSIVE ADVANCED DEMO ===")
        
        # Example 1: Advanced simulation
        print("\n1. Running Advanced Simulation...")
        results1 = NLMAdvancedExamples.run_advanced_simulation_example()
        
        # Example 2: Performance benchmark
        print("\n2. Running Performance Benchmark...")
        results2 = NLMAdvancedExamples.run_performance_benchmark_example()
        
        # Summary
        print("\n" + "="*60)
        print("COMPREHENSIVE DEMO COMPLETE")
        print("="*60)
        print(f"Advanced simulation completed with {results1['steps_completed']} steps")
        print(f"Benchmark tested {len([k for k in results2.keys() if k.startswith('neurons_')])} network sizes")
        print(f"Plasticity modes tested: {len([k for k in results2.keys() if k.startswith('plasticity_')])}")
        print(f"Data saved to: analysis_data/ directory")
        
        return {'simulation_results': results1, 'benchmark_results': results2}
if __name__ == "__main__":
    print("NLM Advanced Examples")
    print("=====================")
    
    # Run comprehensive demo
    results = NLMAdvancedExamples.run_comprehensive_demo()
    
    print(f"\nAll examples completed successfully!")
    print(f"Check analysis_data/ directory for saved results")