#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings Examples and Utilities

This file provides comprehensive examples and utility functions for working with
the enhanced NLM (Neural Learning Machine) Python bindings.

It demonstrates:
1. Basic brain operations and simulations
2. Configuration management
3. Advanced brain manipulation
4. Experiment setup and control
5. Visualization and analysis utilities
6. Performance monitoring
7. Machine learning integration utilities
"""

import sys
import os
import json
import time
import statistics
import subprocess
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
import matplotlib.pyplot as plt
import numpy as np

# Add the build directory to the path for importing pynlm
try:
    sys.path.insert(0, '/workspace/app/build/lib*')
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some examples will not run.")

class NLMExampleManager:
    """Manages NLM examples and demonstrations"""
    
    def __init__(self, project_root: str = "/workspace/app"):
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build"
        self.examples_dir = self.project_root / "examples"
        self.examples_dir.mkdir(exist_ok=True)
    
    def run_basic_simulation(self, neurons: int = 100, steps: int = 1000) -> Dict[str, Any]:
        """Run a basic simulation example"""
        print(f"Running basic simulation with {neurons} neurons for {steps} steps...")
        
        if not PYNLM_AVAILABLE:
            return {'success': False, 'error': 'pynlm not available'}
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Set parameters
            config.set("neuron_count", neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.15)
            config.set("random_seed", 42)
            
            # Create brain
            brain = pynlm.createBrain(config)
            
            # Initialize brain
            if not brain.initialize():
                return {'success': False, 'error': 'Brain initialization failed'}
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step, step * 0.001)
                
                # Log progress every 100 steps
                if step % 100 == 0:
                    print(f"  Step {step}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
            
            end_time = time.time()
            
            # Collect results
            result = {
                'success': True,
                'neuron_count': neurons,
                'steps': steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
                'total_time': end_time - start_time,
                'time_per_step': (end_time - start_time) / steps,
                'final_developmental_stage': brain.getDevelopmentalStage()
            }
            
            print(f"✓ Simulation completed in {end_time - start_time:.2f}s")
            print(f"  Total spikes: {result['total_spikes']}")
            print(f"  Average firing rate: {result['avg_firing_rate']:.3f} Hz")
            print(f"  Final developmental stage: {result['final_developmental_stage']}")
            
            return result
            
        except Exception as e:
            return {'success': False, 'error': str(e)}
    
    def run_learning_experiment(self, neurons: int = 500, steps: int = 5000) -> Dict[str, Any]:
        """Run a learning experiment example"""
        print(f"Running learning experiment with {neurons} neurons for {steps} steps...")
        
        if not PYNLM_AVAILABLE:
            return {'success': False, 'error': 'pynlm not available'}
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Set learning-specific parameters
            config.set("neuron_count", neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.15)
            config.set("random_seed", 42)
            config.set("synaptogenesis_rate", 0.0001)
            config.set("pruning_rate", 0.00001)
            config.set("stdp_ltp_weight", 0.02)
            config.set("stdp_ltd_weight", 0.015)
            config.set("stdp_tau", 20.0)
            
            # Create brain
            brain = pynlm.createBrain(config)
            
            # Initialize brain
            if not brain.initialize():
                return {'success': False, 'error': 'Brain initialization failed'}
            
            # Track learning metrics
            initial_weights = {}
            synaptic_changes = []
            firing_rates = []
            
            # Record initial state
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                for neuron_id in region.getNeuronIds():
                    neuron = region.getNeuron(neuron_id)
                    if neuron.getNeuronType() == pynlm.NeuronType.Excitatory:
                        # Record initial weight if connected to other neurons
                        pass
            
            # Run learning simulation
            start_time = time.time()
            for step in range(steps):
                # Inject varied sensory input to drive learning
                input_strength = 0.5 + 0.5 * np.sin(step * 0.01)
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, input_strength * 50.0)
                
                brain.step(step, step * 0.001)
                
                # Record metrics
                if step % 100 == 0:
                    firing_rate = brain.getAverageFiringRate()
                    firing_rates.append(firing_rate)
                    
                    print(f"  Step {step}/{steps}: firing rate = {firing_rate:.3f} Hz")
            
            end_time = time.time()
            
            # Collect results
            result = {
                'success': True,
                'neuron_count': neurons,
                'steps': steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
                'total_time': end_time - start_time,
                'time_per_step': (end_time - start_time) / steps,
                'final_developmental_stage': brain.getDevelopmentalStage(),
                'initial_firing_rate': firing_rates[0] if firing_rates else 0,
                'final_firing_rate': firing_rates[-1] if firing_rates else 0,
                'firing_rate_growth': (firing_rates[-1] - firing_rates[0]) / firing_rates[0] * 100 if firing_rates and firing_rates[0] > 0 else 0,
                'learning_success': True if firing_rates and firing_rates[-1] > firing_rates[0] else False
            }
            
            print(f"✓ Learning experiment completed in {end_time - start_time:.2f}s")
            print(f"  Total spikes: {result['total_spikes']}")
            print(f"  Firing rate growth: {result['firing_rate_growth']:.1f}%")
            print(f"  Learning success: {'Yes' if result['learning_success'] else 'No'}")
            
            return result
            
        except Exception as e:
            return {'success': False, 'error': str(e)}
    
    def run_neuromodulation_experiment(self, neurons: int = 300, steps: int = 3000) -> Dict[str, Any]:
        """Run a neuromodulation experiment example"""
        print(f"Running neuromodulation experiment with {neurons} neurons for {steps} steps...")
        
        if not PYNLM_AVAILABLE:
            return {'success': False, 'error': 'pynlm not available'}
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Set neuromodulation-specific parameters
            config.set("neuron_count", neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.20)
            config.set("random_seed", 42)
            
            # Create brain and agent brain
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent_brain = pynlm.createAgentBrain(brain)
            
            # Initialize world
            world.configure(20, 20, 10, 10)
            world.setAgentStart(10, 10)
            
            # Initialize agent brain
            agent_brain.initialize(world)
            
            # Enable neuromodulation systems
            agent_brain.enableRewardModulation(True)
            agent_brain.enableCuriosity(True)
            agent_brain.enableStructuralPlasticity(True)
            
            # Track neuromodulation metrics
            curiosity_levels = []
            novelty_levels = []
            prediction_errors = []
            neuromodulation_levels = []
            
            # Run neuromodulation simulation
            start_time = time.time()
            for step in range(steps):
                # Get current sensory percept
                percept = world.getSensoryPercept()
                
                # Process input
                agent_brain.processSensoryInput(percept)
                
                # Produce action
                action = agent_brain.decodeMotorCommand()
                
                # Apply reward modulation based on novelty
                novelty = np.random.random() * 0.1
                agent_brain.applyRewardModulation(novelty, novelty * 0.8)
                
                # Update development
                agent_brain.updateDevelopment(0.001)
                
                # Record neuromodulation metrics
                if step % 100 == 0:
                    curiosity = agent_brain.getCuriosityLevel()
                    novelty = agent_brain.getNoveltyLevel()
                    prediction_error = agent_brain.getPredictionError()
                    neuromodulation = agent_brain.getNeuromodulationLevel()
                    
                    curiosity_levels.append(curiosity)
                    novelty_levels.append(novelty)
                    prediction_errors.append(prediction_error)
                    neuromodulation_levels.append(neuromodulation)
                    
                    print(f"  Step {step}/{steps}: curiosity={curiosity:.3f}, novelty={novelty:.3f}, pred_error={prediction_error:.3f}")
                
                # Update world
                world.update(0.001)
                
                # Apply motor command
                world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            
            # Collect results
            result = {
                'success': True,
                'neuron_count': neurons,
                'steps': steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'final_curiosity': curiosity_levels[-1] if curiosity_levels else 0,
                'avg_curiosity': statistics.mean(curiosity_levels) if curiosity_levels else 0,
                'max_curiosity': max(curiosity_levels) if curiosity_levels else 0,
                'min_curiosity': min(curiosity_levels) if curiosity_levels else 0,
                'final_novelty': novelty_levels[-1] if novelty_levels else 0,
                'avg_novelty': statistics.mean(novelty_levels) if novelty_levels else 0,
                'final_prediction_error': prediction_errors[-1] if prediction_errors else 0,
                'avg_prediction_error': statistics.mean(prediction_errors) if prediction_errors else 0,
                'final_neuromodulation': neuromodulation_levels[-1] if neuromodulation_levels else 0,
                'total_time': end_time - start_time,
                'time_per_step': (end_time - start_time) / steps,
                'final_developmental_stage': brain.getDevelopmentalStage()
            }
            
            print(f"✓ Neuromodulation experiment completed in {end_time - start_time:.2f}s")
            print(f"  Total spikes: {result['total_spikes']}")
            print(f"  Average curiosity: {result['avg_curiosity']:.3f}")
            print(f"  Average novelty: {result['avg_novelty']:.3f}")
            print(f"  Average prediction error: {result['avg_prediction_error']:.3f}")
            
            return result
            
        except Exception as e:
            return {'success': False, 'error': str(e)}
    
    def save_example_results(self, experiment_name: str, results: Dict[str, Any]) -> str:
        """Save experiment results to file"""
        timestamp = int(time.time())
        filename = f"{experiment_name}_results_{timestamp}.json"
        filepath = self.examples_dir / filename
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"✓ Results saved to: {filepath}")
        return str(filepath)
    
    def run_all_examples(self) -> List[Dict[str, Any]]:
        """Run all available examples and return results"""
        print("Running all NLM examples...")
        print("=" * 60)
        
        results = []
        
        # Run basic simulation
        print("\n1. Basic Simulation Example")
        print("-" * 40)
        basic_result = self.run_basic_simulation(neurons=100, steps=1000)
        results.append({'type': 'basic_simulation', 'results': basic_result})
        
        # Run learning experiment
        print("\n2. Learning Experiment Example")
        print("-" * 40)
        learning_result = self.run_learning_experiment(neurons=500, steps=5000)
        results.append({'type': 'learning_experiment', 'results': learning_result})
        
        # Run neuromodulation experiment
        print("\n3. Neuromodulation Experiment Example")
        print("-" * 40)
        neuromod_result = self.run_neuromodulation_experiment(neurons=300, steps=3000)
        results.append({'type': 'neuromodulation_experiment', 'results': neuromod_result})
        
        # Save individual result files
        for result_data in results:
            self.save_example_results(result_data['type'], result_data['results'])
        
        # Create summary file
        summary_file = self.examples_dir / "examples_summary.json"
        with open(summary_file, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"\n✓ All examples completed")
        print(f"  Summary saved to: {summary_file}")
        
        return results
    
    def create_example_configuration(self, filename: str, config_data: Dict[str, Any]) -> str:
        """Create an example configuration file"""
        filepath = self.examples_dir / filename
        
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"✓ Example configuration saved to: {filepath}")
        return str(filepath)
    
    def create_example_script(self, name: str, content: str) -> str:
        """Create an example Python script"""
        filepath = self.examples_dir / f"{name}.py"
        
        with open(filepath, 'w') as f:
            f.write(content)
        
        # Make the script executable
        os.chmod(filepath, 0o755)
        
        print(f"✓ Example script saved to: {filepath}")
        return str(filepath)
    
    def create_utilities(self) -> None:
        """Create additional utility scripts and documentation"""
        
        # Create basic configuration template
        config_template = {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "enable_plasticity": True,
            "enable_development": True,
            "enable_reward_modulation": True
        }
        
        self.create_example_configuration("config_template.json", config_template)
        
        # Create basic usage example
        usage_example = '''#!/usr/bin/env python3
"""
Basic NLM Usage Example

This example demonstrates how to use the NLM Python bindings
for basic brain simulation and learning experiments.
"""

import sys
import os
import numpy as np

# Add the build directory to the path
sys.path.insert(0, '/workspace/app/build/lib*')

try:
    import pynlm
    print("NLM Python bindings loaded successfully")
    
    def main():
        print("=== NLM Basic Usage Example ===")
        
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        print(f"Created default config: {config}")
        
        # Modify some parameters
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        config.set("random_seed", 42)
        
        # Create the brain
        brain = pynlm.createBrain(config)
        print(f"Created brain: {brain}")
        
        # Initialize the brain
        if brain.initialize():
            print("✓ Brain initialized successfully")
            
            # Run a simple simulation
            steps = 100
            for step in range(steps):
                brain.step(step, step * 0.001)
                
                if step % 25 == 0:
                    firing_rate = brain.getAverageFiringRate()
                    print(f"  Step {step}: {firing_rate:.3f} Hz average firing rate")
            
            # Print final statistics
            print(f"\nSimulation Results:")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
            print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
            
        else:
            print("✗ Brain initialization failed")
    
    if __name__ == "__main__":
        main()

except ImportError as e:
    print(f"Error loading NLM bindings: {e}")
    print("Please build the NLM project first:")
    print("  cd /workspace/app")
    print("  mkdir build")
    print("  cd build")
    print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("  make -j4")
'''
        
        self.create_example_script("basic_usage_example.py", usage_example)
        
        # Create advanced usage example
        advanced_example = '''#!/usr/bin/env python3
"""
Advanced NLM Usage Example

This example demonstrates advanced NLM features including:
- Agent-based simulation with world interaction
- Neuromodulation systems
- Memory and prediction systems
- Advanced configuration management
"""

import sys
import os
import numpy as np
import time

sys.path.insert(0, '/workspace/app/build/lib*')

try:
    import pynlm
    print("NLM Python bindings loaded successfully")
    
    class NLMExperiment:
        def __init__(self):
            # Create configuration
            self.config = pynlm.createDefaultConfig()
            
            # Configure for advanced experiments
            self.config.set("neuron_count", 500)
            self.config.set("region_count", 1)
            self.config.set("connection_probability", 0.15)
            self.config.set("random_seed", 42)
            self.config.set("enable_plasticity", True)
            self.config.set("enable_development", True)
            
            # Create brain
            self.brain = pynlm.createBrain(self.config)
            
            # Create world
            self.world = pynlm.createSimpleWorld()
            
            # Create agent brain
            self.agent_brain = pynlm.createAgentBrain(self.brain)
            
            # Initialize
            self.world.configure(20, 20, 10, 10)
            self.world.setAgentStart(10, 10)
            self.agent_brain.initialize(self.world)
            
            # Enable advanced features
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableCuriosity(True)
            self.agent_brain.enableStructuralPlasticity(True)
            
            print("✓ Advanced experiment environment initialized")
        
        def run_experiment(self, steps: int) -> Dict[str, Any]:
            """Run the experiment for specified number of steps"""
            print(f"Running advanced experiment for {steps} steps...")
            
            start_time = time.time()
            
            for step in range(steps):
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process input and get brain activity
                self.agent_brain.processSensoryInput(percept)
                
                # Generate action
                action = self.agent_brain.decodeMotorCommand()
                
                # Apply neuromodulation based on novelty
                novelty = np.random.random() * 0.1
                reward = novelty * 0.8
                self.agent_brain.applyRewardModulation(reward, reward * 0.9)
                
                # Update development
                self.agent_brain.updateDevelopment(0.001)
                
                # Execute action in world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Update world
                self.world.update(0.001)
                
                # Log progress
                if step % 100 == 0:
                    curiosity = self.agent_brain.getCuriosityLevel()
                    novelty = self.agent_brain.getNoveltyLevel()
                    print(f"  Step {step}: curiosity={curiosity:.3f}, novelty={novelty:.3f}")
            
            end_time = time.time()
            
            # Collect results
            result = {
                'total_steps': steps,
                'total_time': end_time - start_time,
                'avg_time_per_step': (end_time - start_time) / steps,
                'total_spikes': self.brain.getTotalSpikeCount(),
                'final_curiosity': self.agent_brain.getCuriosityLevel(),
                'final_novelty': self.agent_brain.getNoveltyLevel(),
                'final_prediction_error': self.agent_brain.getPredictionError(),
                'final_neuromodulation': self.agent_brain.getNeuromodulationLevel(),
                'final_developmental_stage': self.brain.getDevelopmentalStage(),
                'final_firing_rate': self.brain.getAverageFiringRate()
            }
            
            print(f"✓ Experiment completed in {end_time - start_time:.2f}s")
            return result
        
        def save_results(self, filepath: str, results: Dict[str, Any]):
            """Save experiment results to file"""
            import json
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2, default=str)
            print(f"✓ Results saved to: {filepath}")
    
    if __name__ == "__main__":
        # Run the experiment
        experiment = NLMExperiment()
        results = experiment.run_experiment(1000)
        
        # Save results
        experiment.save_results("advanced_experiment_results.json", results)
        
        print("\n=== Advanced Experiment Complete ===")
        print(f"Total spikes: {results['total_spikes']}")
        print(f"Average time per step: {results['avg_time_per_step']*1000:.2f}ms")
        print(f"Final developmental stage: {results['final_developmental_stage']}")

except ImportError as e:
    print(f"Error loading NLM bindings: {e}")
    print("Please build the NLM project first.")
'''
        
        self.create_example_script("advanced_usage_example.py", advanced_example)
        
        # Create comparison example
        comparison_example = '''#!/usr/bin/env python3
"""
NLM Experiment Comparison Example

This example demonstrates how to compare different experimental
configurations and their effects on brain performance.
"""

import sys
import os
import json
import matplotlib.pyplot as plt
import numpy as np

sys.path.insert(0, '/workspace/app/build/lib*')

try:
    import pynlm
    print("NLM Python bindings loaded successfully")
    
    class ExperimentComparator:
        def __init__(self):
            self.results = {}
        
        def run_experiment_with_config(self, config_name: str, config: Dict[str, Any], 
                                       neurons: int = 300, steps: int = 1000) -> Dict[str, Any]:
            """Run an experiment with specific configuration"""
            print(f"\nRunning {config_name} experiment...")
            print(f"  Neurons: {neurons}, Steps: {steps}")
            
            # Create configuration object
            config_obj = pynlm.createDefaultConfig()
            
            # Set parameters
            config_obj.set("neuron_count", neurons)
            config_obj.set("region_count", 1)
            config_obj.set("random_seed", config.get("random_seed", 42))
            config_obj.set("connection_probability", config.get("connection_probability", 0.15))
            
            # Set plasticity parameters
            config_obj.set("stdp_ltp_weight", config.get("stdp_ltp_weight", 0.02))
            config_obj.set("stdp_ltd_weight", config.get("stdp_ltd_weight", 0.015))
            config_obj.set("stdp_tau", config.get("stdp_tau", 20.0))
            
            # Create brain
            brain = pynlm.createBrain(config_obj)
            
            if not brain.initialize():
                raise Exception("Brain initialization failed")
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step, step * 0.001)
            
            end_time = time.time()
            
            # Collect results
            result = {
                'config_name': config_name,
                'neuron_count': neurons,
                'steps': steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
                'total_time': end_time - start_time,
                'time_per_step': (end_time - start_time) / steps,
                'final_developmental_stage': brain.getDevelopmentalStage()
            }
            
            print(f"  ✓ Completed in {result['total_time']:.2f}s")
            print(f"    Total spikes: {result['total_spikes']}")
            print(f"    Avg firing rate: {result['avg_firing_rate']:.3f} Hz")
            
            return result
        
        def run_all_comparisons(self):
            """Run multiple experiments with different configurations"""
            # Define different experiment configurations
            configs = {
                'conservative': {
                    'random_seed': 42,
                    'connection_probability': 0.10,
                    'stdp_ltp_weight': 0.01,
                    'stdp_ltd_weight': 0.005,
                    'stdp_tau': 10.0
                },
                'balanced': {
                    'random_seed': 123,
                    'connection_probability': 0.15,
                    'stdp_ltp_weight': 0.02,
                    'stdp_ltd_weight': 0.015,
                    'stdp_tau': 20.0
                },
                'aggressive': {
                    'random_seed': 456,
                    'connection_probability': 0.20,
                    'stdp_ltp_weight': 0.05,
                    'stdp_ltd_weight': 0.03,
                    'stdp_tau': 30.0
                }
            }
            
            # Run experiments
            for config_name, config in configs.items():
                result = self.run_experiment_with_config(config_name, config)
                self.results[config_name] = result
            
            # Generate comparison report
            self.generate_comparison_report()
        
        def generate_comparison_report(self):
            """Generate comparison report and plots"""
            print("\n" + "="*60)
            print("EXPERIMENT COMPARISON REPORT")
            print("="*60)
            
            if not self.results:
                print("No results to compare")
                return
            
            # Print summary table
            print("\nResults Summary:")
            print("-" * 80)
            print(f"{'Config':<12} {'Neurons':<8} {'Steps':<8} {'Total Spikes':<15} {'Avg Firing':<12} {'Time (s)':<10} {'Dev Stage':<10}")
            print("-" * 80)
            
            for config_name, result in self.results.items():
                print(f"{config_name:<12} {result['neuron_count']:<8} {result['steps']:<8} "
                      f"{result['total_spikes']:<15} {result['avg_firing_rate']:<12.3f} "
                      f"{result['total_time']:<10.3f} {result['final_developmental_stage']:<10}")
            
            # Create performance comparison plot
            self.create_performance_plot()
            
            # Save comparison results
            with open('/workspace/app/examples/comparison_results.json', 'w') as f:
                json.dump(self.results, f, indent=2)
            
            print(f"\n✓ Comparison results saved to: /workspace/app/examples/comparison_results.json")
        
        def create_performance_plot(self):
            """Create performance comparison plot"""
            try:
                # Create figure with subplots
                fig, axes = plt.subplots(2, 2, figsize=(12, 10))
                fig.suptitle('NLM Experiment Configuration Comparison', fontsize=16)
                
                config_names = list(self.results.keys())
                
                # Plot 1: Total Spikes
                ax1 = axes[0, 0]
                spikes = [self.results[config]['total_spikes'] for config in config_names]
                bars1 = ax1.bar(config_names, spikes, color='skyblue', alpha=0.7)
                ax1.set_title('Total Spikes Generated')
                ax1.set_ylabel('Number of Spikes')
                ax1.grid(True, alpha=0.3)
                
                # Add value labels on bars
                for bar, spike in zip(bars1, spikes):
                    ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                            f'{spike:,}', ha='center', va='bottom')
                
                # Plot 2: Average Firing Rate
                ax2 = axes[0, 1]
                firing_rates = [self.results[config]['avg_firing_rate'] for config in config_names]
                bars2 = ax2.bar(config_names, firing_rates, color='lightgreen', alpha=0.7)
                ax2.set_title('Average Firing Rate (Hz)')
                ax2.set_ylabel('Firing Rate (Hz)')
                ax2.grid(True, alpha=0.3)
                
                # Add value labels on bars
                for bar, rate in zip(bars2, firing_rates):
                    ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                            f'{rate:.2f}', ha='center', va='bottom')
                
                # Plot 3: Execution Time
                ax3 = axes[1, 0]
                times = [self.results[config]['total_time'] for config in config_names]
                bars3 = ax3.bar(config_names, times, color='salmon', alpha=0.7)
                ax3.set_title('Execution Time (seconds)')
                ax3.set_ylabel('Time (seconds)')
                ax3.grid(True, alpha=0.3)
                
                # Add value labels on bars
                for bar, time_val in zip(bars3, times):
                    ax3.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                            f'{time_val:.3f}', ha='center', va='bottom')
                
                # Plot 4: Developmental Stage
                ax4 = axes[1, 1]
                dev_stages = [self.results[config]['final_developmental_stage'] for config in config_names]
                bars4 = ax4.bar(config_names, dev_stages, color='gold', alpha=0.7)
                ax4.set_title('Final Developmental Stage')
                ax4.set_ylabel('Developmental Stage')
                ax4.set_ylim(0, 6)  # Assume stages go from 0 to 5
                ax4.grid(True, alpha=0.3)
                
                # Add value labels on bars
                for bar, stage in zip(bars4, dev_stages):
                    ax4.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                            f'{stage}', ha='center', va='bottom')
                
                # Adjust layout
                plt.tight_layout()
                
                # Save plot
                plt.savefig('/workspace/app/examples/experiment_comparison.png', 
                           dpi=300, bbox_inches='tight')
                print(f"✓ Performance plot saved to: /workspace/app/examples/experiment_comparison.png")
                
                # Close plot
                plt.close()
                
            except Exception as e:
                print(f"Warning: Could not create performance plot: {e}")
    
    if __name__ == "__main__":
        print("=== NLM Experiment Comparison Example ===")
        
        comparator = ExperimentComparator()
        comparator.run_all_comparisons()
        
        print("\n=== Experiment Comparison Complete ===")

except ImportError as e:
    print(f"Error loading NLM bindings: {e}")
    print("Please build the NLM project first.")
'''
        
        self.create_example_script("comparison_example.py", comparison_example)
        
        print("✓ Created additional utility examples")

if __name__ == '__main__':
    manager = NLMExampleManager()
    manager.create_utilities()
    
    print("\n=== NLM Python Bindings Examples Created ===")
    print("Example files created in:")
    print("  - /workspace/app/examples/")
    print("\nKey example files:")
    print("  - basic_usage_example.py - Basic NLM usage")
    print("  - advanced_usage_example.py - Advanced features")
    print("  - comparison_example.py - Configuration comparison")
    print("  - config_template.json - Configuration template")
    print("\nTo run examples:")
    print("  python examples/basic_usage_example.py")
    print("  python examples/advanced_usage_example.py")
    print("  python examples/comparison_example.py")

else:
    print("Error: Could not import pynlm")
    print("Please build the NLM project first.")
    sys.exit(1)
