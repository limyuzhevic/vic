#!/usr/bin/env python3
"""
Advanced NLM Python API - Power User Features

Advanced features for power users of the NLM (Neural Learning Machine).
This includes advanced simulation control, debugging tools, performance analysis,
and integration with scientific computing workflows.
"""

import sys
import os
import json
import time
import threading
import queue
from pathlib import Path
from typing import List, Dict, Any, Optional, Callable

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class AdvancedBrainSimulator:
    """
    Advanced brain simulator with enhanced control, monitoring, and automation.
    """
    
    def __init__(self, config_file: Optional[str] = None, neurons: int = 1000):
        self.config_file = config_file
        self.neurons = neurons
        self.brain = None
        self.world = None
        self.agent = None
        self.step_callback = None
        self.error_callback = None
        self.monitoring_enabled = False
        self.monitoring_data = []
        
    def initialize(self, enable_learning: bool = True):
        """Initialize the advanced simulator with monitoring."""
        print(f"Initializing AdvancedBrainSimulator with {self.neurons} neurons...")
        
        # Create configuration
        if self.config_file:
            config = pynlm.createConfig(self.config_file)
        else:
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=self.neurons)
        
        # Create brain
        self.brain = pynlm.BrainWrapper(config.getConfig())
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(30, 30, 8, 8)
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain.getBrain())
        self.agent.initialize(self.world)
        
        # Enable learning if requested
        if enable_learning:
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
        
        print("AdvancedBrainSimulator initialized successfully")
        return True
    
    def run_with_monitoring(self, num_steps: int, timestep: float = 0.1):
        """Run simulation with detailed monitoring."""
        print(f"Running {num_steps} steps with monitoring...")
        
        start_time = time.time()
        self.monitoring_data.clear()
        
        for step in range(num_steps):
            # Record state before step
            if self.monitoring_enabled:
                self.record_step_state(step, timestep)
            
            # Execute simulation step
            try:
                self.world.update(timestep)
                
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                self.brain.step(step)
                
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Get reward
                internal_signals = percept.getInternal()
                reward = internal_signals[0] if internal_signals else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                self.agent.updateDevelopment(timestep)
                
                # Call step callback if set
                if self.step_callback:
                    self.step_callback(step, self.get_current_state())
                
                # Record state after step
                if self.monitoring_enabled:
                    self.record_step_state(step, timestep, "completed")
                    
            except Exception as e:
                if self.error_callback:
                    self.error_callback(step, e)
                else:
                    print(f"Error at step {step}: {e}")
                    raise
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"Simulation completed in {duration:.2f} seconds")
        return duration
    
    def run_training_episode(self, num_steps: int, exploration: bool = True):
        """Run a training episode with exploration-exploitation balance."""
        print(f"Running training episode with {num_steps} steps...")
        
        # Set exploration factor
        if exploration:
            self.agent.enableCuriosity(True)
            self.agent.enableRewardModulation(True)
        
        start_time = time.time()
        
        for step in range(num_steps):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            self.agent.updateDevelopment(0.1)
            
            if step % 50 == 0:
                print(f"Step {step}: curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"novelty={self.agent.getNoveltyLevel():.3f}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"Training episode completed in {duration:.2f} seconds")
        return duration
    
    def get_current_state(self):
        """Get current simulation state as a dictionary."""
        stats = self.brain.getStatistics()
        state = {
            "step": len(self.monitoring_data),
            "neurons_firing": stats['firing_neurons'],
            "total_spikes": stats['total_spikes'],
            "curiosity_level": self.agent.getCuriosityLevel(),
            "novelty_level": self.agent.getNoveltyLevel(),
            "development_stage": self.agent.getDevelopmentalStage(),
            "world_position": (self.world.getAgentBody().x, self.world.getAgentBody().y)
        }
        return state
    
    def record_step_state(self, step: int, timestep: float, status: str = "running"):
        """Record step state for monitoring."""
        state = self.get_current_state()
        state.update({
            "step": step,
            "timestep": timestep,
            "status": status,
            "timestamp": time.time()
        })
        self.monitoring_data.append(state)
    
    def save_monitoring_data(self, filepath: str):
        """Save monitoring data to file."""
        with open(filepath, 'w') as f:
            json.dump(self.monitoring_data, f, indent=2)
        print(f"Monitoring data saved to {filepath}")
    
    def analyze_monitoring_data(self):
        """Analyze monitoring data and return statistics."""
        if not self.monitoring_data:
            return {}
        
        spikes = [d['total_spikes'] for d in self.monitoring_data]
        curiosity = [d['curiosity_level'] for d in self.monitoring_data]
        novelty = [d['novelty_level'] for d in self.monitoring_data]
        
        analysis = {
            "total_steps": len(self.monitoring_data),
            "final_spikes": spikes[-1] if spikes else 0,
            "avg_spikes_per_step": sum(spikes) / len(spikes) if spikes else 0,
            "max_spikes_per_step": max(spikes) if spikes else 0,
            "avg_curiosity": sum(curiosity) / len(curiosity) if curiosity else 0,
            "avg_novelty": sum(novelty) / len(novelty) if novelty else 0,
            "final_curiosity": curiosity[-1] if curiosity else 0,
            "final_novelty": novelty[-1] if novelty else 0
        }
        
        return analysis

class BatchExperimentRunner:
    """
    Run batch experiments with automated parameter sweeps and result aggregation.
    """
    
    def __init__(self, output_dir: str = "batch_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.results = []
        
    def run_parameter_sweep(self, base_config: Dict[str, Any], 
                           parameters: Dict[str, List[Any]],
                           num_steps: int = 100) -> List[Dict[str, Any]]:
        """
        Run parameter sweep experiment.
        
        Args:
            base_config: Base configuration parameters
            parameters: Dictionary of parameters to sweep (key: parameter name, value: list of values)
            num_steps: Number of simulation steps per run
            
        Returns:
            List of results
        """
        print("Running parameter sweep experiment...")
        
        param_names = list(parameters.keys())
        param_values = list(parameters.values())
        
        # Generate all combinations
        from itertools import product
        combinations = list(product(*param_values))
        
        print(f"Testing {len(combinations)} parameter combinations...")
        
        for i, combo in enumerate(combinations):
            print(f"Combination {i+1}/{len(combinations)}: {dict(zip(param_names, combo))}")
            
            # Create configuration
            config = pynlm.ConfigWrapper()
            
            # Apply base configuration
            for key, value in base_config.items():
                if key == "neuron_count":
                    config.setInt("brain.neuron_count", value)
                elif key == "connection_probability":
                    config.setFloat("brain.connection_probability", value)
            
            # Apply parameter combination
            for name, value in zip(param_names, combo):
                if name == "neuron_count":
                    config.setInt("brain.neuron_count", value)
                elif name == "connection_probability":
                    config.setFloat("brain.connection_probability", value)
            
            # Run simulation
            brain = pynlm.BrainWrapper(config.getConfig())
            if brain.initialize():
                brain.runSimulation(num_steps)
                
                stats = brain.getStatistics()
                result = {
                    **dict(zip(param_names, combo)),
                    **stats,
                    "success": True
                }
                
                self.results.append(result)
                
                print(f"  -> Total spikes: {stats['total_spikes']}, "
                      f"avg firing rate: {stats['avg_firing_rate']:.2f} Hz")
                
            else:
                print(f"  -> Failed to initialize brain")
                self.results.append({
                    **dict(zip(param_names, combo)),
                    "success": False,
                    "error": "initialization_failed"
                })
        
        # Save results
        self.save_results("parameter_sweep.json")
        
        # Find best configurations
        successful_results = [r for r in self.results if r.get("success", False)]
        if successful_results:
            best_by_spikes = max(successful_results, key=lambda r: r["total_spikes"])
            best_by_rate = max(successful_results, key=lambda r: r["avg_firing_rate"])
            
            print(f"\nBest by total spikes: {best_by_spikes}")
            print(f"Best by firing rate: {best_by_rate}")
        
        return self.results
    
    def run_lifetime_experiment(self, num_runs: int, neurons: int = 1000, 
                                steps_per_run: int = 200) -> List[Dict[str, Any]]:
        """
        Run multiple lifetime experiments.
        
        Args:
            num_runs: Number of runs
            neurons: Number of neurons per run
            steps_per_run: Steps per run
            
        Returns:
            List of results
        """
        print(f"Running {num_runs} lifetime experiments...")
        
        for run in range(num_runs):
            print(f"Run {run + 1}/{num_runs}")
            
            # Create configuration
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=neurons)
            
            # Create brain
            brain = pynlm.BrainWrapper(config.getConfig())
            if brain.initialize():
                brain.runSimulation(steps_per_run)
                
                stats = brain.getStatistics()
                result = {
                    "run": run + 1,
                    "neurons": neurons,
                    "steps": steps_per_run,
                    **stats,
                    "success": True
                }
                
                self.results.append(result)
                print(f"  -> Completed: {stats['total_spikes']} spikes, "
                      f"{stats['avg_firing_rate']:.2f} Hz average")
                
            else:
                print(f"  -> Failed to initialize")
                self.results.append({
                    "run": run + 1,
                    "neurons": neurons,
                    "success": False,
                    "error": "initialization_failed"
                })
        
        # Save results
        self.save_results("lifetime_experiment.json")
        
        # Calculate statistics
        successful_results = [r for r in self.results if r.get("success", False)]
        if successful_results:
            avg_spikes = sum(r["total_spikes"] for r in successful_results) / len(successful_results)
            avg_rate = sum(r["avg_firing_rate"] for r in successful_results) / len(successful_results)
            
            print(f"\nLifetime experiment results:")
            print(f"  Successful runs: {len(successful_results)}/{num_runs}")
            print(f"  Average spikes per run: {avg_spikes:.1f}")
            print(f"  Average firing rate: {avg_rate:.2f} Hz")
        
        return self.results
    
    def save_results(self, filename: str):
        """Save results to file."""
        filepath = self.output_dir / filename
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"Results saved to {filepath}")
    
    def load_results(self, filename: str) -> List[Dict[str, Any]]:
        """Load results from file."""
        filepath = self.output_dir / filename
        with open(filepath, 'r') as f:
            self.results = json.load(f)
        return self.results

class DebugTools:
    """
    Debugging tools for NLM development and troubleshooting.
    """
    
    @staticmethod
    def create_debug_brain(config_file: Optional[str] = None):
        """Create a brain with enhanced debugging capabilities."""
        if config_file:
            config = pynlm.createConfig(config_file)
        else:
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=100)
        
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            print("Debug brain initialized successfully")
            return brain
        else:
            print("Failed to initialize debug brain")
            return None
    
    @staticmethod
    def step_by_step_simulation(brain, max_steps: int = 10):
        """Run simulation step by step for debugging."""
        print("Starting step-by-step simulation...")
        
        for step in range(max_steps):
            print(f"\n--- Step {step} ---")
            
            # Get current statistics
            stats = brain.getStatistics()
            print(f"Statistics:")
            print(f"  Neurons: {stats['total_neurons']}")
            print(f"  Firing: {stats['firing_neurons']}")
            print(f"  Spikes: {stats['total_spikes']}")
            print(f"  Firing rate: {stats['avg_firing_rate']:.2f} Hz")
            print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
            
            # Run step
            brain.step(step)
            
            # Ask user if they want to continue
            if step < max_steps - 1:
                input("Press Enter to continue to next step...")
        
        print("\nStep-by-step simulation completed")
    
    @staticmethod
    def validate_configuration(config_file: str):
        """Validate configuration file and report issues."""
        print(f"Validating configuration: {config_file}")
        
        try:
            validation_result = pynlm.validateConfiguration(config_file)
            
            if validation_result['valid']:
                print("Configuration is valid!")
            else:
                print("Configuration is INVALID!")
            
            print(f"Status: {validation_result['valid']}")
            print(f"Has neuron count: {validation_result['has_neuron_count']}")
            print(f"Has connection probability: {validation_result['has_connection_probability']}")
            
            return validation_result['valid']
            
        except Exception as e:
            print(f"Error validating configuration: {e}")
            return False
    
    @staticmethod
    def compare_configurations(config_files: List[str]):
        """Compare multiple configuration files."""
        print("Comparing configurations...")
        
        for config_file in config_files:
            print(f"\n--- {config_file} ---")
            try:
                validation = pynlm.validateConfiguration(config_file)
                print(f"Valid: {validation['valid']}")
            except Exception as e:
                print(f"Error: {e}")

class PerformanceProfiler:
    """
    Performance profiling tools for NLM simulations.
    """
    
    @staticmethod
    def profile_simulation(config, num_steps: int):
        """Profile simulation performance."""
        print(f"Profiling simulation with {config.getConfig().getInt('brain.neuron_count')} neurons...")
        
        brain = pynlm.BrainWrapper(config.getConfig())
        
        # Time initialization
        init_start = time.time()
        brain.initialize()
        init_time = time.time() - init_start
        
        # Time simulation
        sim_start = time.time()
        brain.runSimulation(num_steps)
        sim_time = time.time() - sim_start
        
        # Get statistics
        stats = brain.getStatistics()
        
        print(f"\nPerformance Profile:")
        print(f"  Initialization time: {init_time:.3f} seconds")
        print(f"  Simulation time: {sim_time:.3f} seconds")
        print(f"  Steps per second: {num_steps / sim_time:.1f}")
        print(f"  Neurons per second: {config.getConfig().getInt('brain.neuron_count') / sim_time:.0f}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        
        profile = {
            "initialization_time": init_time,
            "simulation_time": sim_time,
            "steps_per_second": num_steps / sim_time,
            "neurons_per_second": config.getConfig().getInt('brain.neuron_count') / sim_time,
            "total_spikes": stats['total_spikes'],
            "avg_firing_rate": stats['avg_firing_rate'],
            "steps": num_steps,
            "neurons": config.getConfig().getInt('brain.neuron_count')
        }
        
        return profile
    
    @staticmethod
    def benchmark_neuron_counts(neurons_list: List[int], steps: int = 100):
        """Benchmark performance across different neuron counts."""
        print("Benchmarking performance across neuron counts...")
        
        results = []
        
        for neurons in neurons_list:
            print(f"\nBenchmarking with {neurons} neurons...")
            
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=neurons)
            
            profile = PerformanceProfiler.profile_simulation(config, steps)
            results.append(profile)
        
        # Save results
        with open("neuron_benchmark.json", 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\nBenchmark results saved to neuron_benchmark.json")
        
        # Find optimal neuron count
        best_result = max(results, key=lambda r: r["steps_per_second"])
        print(f"\nBest performance: {best_result['neurons']} neurons "
              f"({best_result['steps_per_second']:.1f} steps/second)")
        
        return results

# Convenience functions for quick usage
def quick_test():
    """Run a quick system test."""
    print("Running quick system test...")
    return pynlm.runQuickTest()

def create_demo_brain(neurons: int = 500, steps: int = 50):
    """Create and run a demo brain quickly."""
    print(f"Creating demo brain with {neurons} neurons for {steps} steps...")
    
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=neurons)
    
    brain = pynlm.BrainWrapper(config.getConfig())
    if brain.initialize():
        brain.runSimulation(steps)
        
        stats = brain.getStatistics()
        print(f"Demo completed: {stats['total_spikes']} total spikes, "
              f"{stats['avg_firing_rate']:.2f} Hz average firing rate")
        
        return brain, stats
    else:
        print("Failed to create demo brain")
        return None, None

def create_learning_agent_demo(steps: int = 200):
    """Create and run a learning agent demo."""
    print(f"Creating learning agent demo with {steps} steps...")
    
    brain, world, agent = pynlm.createReadyAgent("configs/default.cfg", steps)
    
    print(f"Learning agent demo completed:")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent

# Main function for direct execution
if __name__ == "__main__":
    print("NLM Advanced Python API - Power User Features")
    print("==============================================")
    print()
    
    # Example usage
    print("Examples of advanced usage:")
    print("1. AdvancedBrainSimulator - Enhanced simulation with monitoring")
    print("2. BatchExperimentRunner - Automated parameter sweeps")
    print("3. DebugTools - Debugging and validation utilities")
    print("4. PerformanceProfiler - Performance benchmarking")
    print()
    
    # Quick demo
    print("Running quick demo...")
    quick_test()
    print()
    
    # Create demo brain
    create_demo_brain(100, 20)
    print()
    
    # Create learning agent demo
    create_learning_agent_demo(50)
    
    print("\nAdvanced API features loaded successfully!")
