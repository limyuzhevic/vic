# Advanced Python Examples - NLM Usage Guide

# Advanced NLM Python Examples

This document provides comprehensive examples for advanced usage of the NLM Python API.
These examples demonstrate production-ready patterns, debugging techniques, and advanced configuration strategies.

---

# Advanced Example 1: Learning Environment Manager

A complete class for managing complex learning scenarios with NLM.

```python
import pynlm
import time
from typing import List, Tuple, Optional

class LearningEnvironmentManager:
    """Manage complex learning scenarios with NLM brain.
    
    This class provides a high-level interface for creating, training,
    and managing multiple learning scenarios with different configurations
    and behaviors.
    """
    
    def __init__(self):
        self.experiments = {}
        self.checkpoints = {}
        self.current_experiment = None
    
    def create_experiment(self, name: str, config_type: str = "research"):
        """Create a new learning experiment with specified configuration."""
        
        config_presets = {
            "quick_test": pynlm.createQuickTestConfig(),
            "quick_demo": pynlm.createQuickDemoConfig(),
            "research": pynlm.createResearchConfig(),
            "standard": pynlm.createStandardConfig(),
            "small": pynlm.createSmallConfig(),
            "large": pynlm.createLargeConfig()
        }
        
        if config_type not in config_presets:
            raise ValueError(f"Unknown config_type: {config_type}")
        
        config = config_presets[config_type]
        
        # Create learning environment
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=20,
            visionRange=8,
            enableCuriosity=(config_type in ["research", "standard", "large"]),
            enableRewardModulation=True
        )
        
        # Apply experiment-specific configuration
        if "development" in config_type:
            pynlm.configureForDevelopment(config)
        
        # Store experiment
        self.experiments[name] = {
            "brain": brain,
            "world": world,
            "agent": agent,
            "config": config,
            "start_time": time.time(),
            "steps_taken": 0,
            "total_reward": 0.0
        }
        
        return self.experiments[name]
    
    def run_experiment(self, name: str, steps: int, save_progress: bool = False) -> dict:
        """Run an experiment for specified steps."""
        
        if name not in self.experiments:
            raise ValueError(f"Experiment not found: {name}")
        
        experiment = self.experiments[name]
        brain = experiment["brain"]
        world = experiment["world"]
        agent = experiment["agent"]
        
        print(f"Starting experiment '{name}' for {steps} steps...")
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action and apply it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Update statistics
            experiment["steps_taken"] = step + 1
            experiment["total_reward"] += reward
            
            # Save progress if requested
            if save_progress and (step + 1) % 100 == 0:
                self.save_experiment_progress(name, f"progress_step_{step + 1}.pkl")
                
            # Print progress every 50 steps
            if (step + 1) % 50 == 0:
                self.print_experiment_status(name)
        
        print(f"Experiment '{name}' completed!")
        return self.get_experiment_results(name)
    
    def print_experiment_status(self, name: str):
        """Print current experiment status."""
        
        if name not in self.experiments:
            return
        
        exp = self.experiments[name]
        agent = exp["agent"]
        brain = exp["brain"]
        
        print(f"  Status: Step {exp['steps_taken']}, Total Reward: {exp['total_reward']:.2f}")
        print(f"  Brain: {brain.getFiringNeuronCount()} neurons firing, {brain.getAverageFiringRate():.2f} avg rate")
        print(f"  Agent: Curiosity {agent.getCuriosityLevel():.3f}, Novelty {agent.getNoveltyLevel():.3f}")
    
    def get_experiment_results(self, name: str) -> dict:
        """Get comprehensive experiment results."""
        
        if name not in self.experiments:
            raise ValueError(f"Experiment not found: {name}")
        
        exp = self.experiments[name]
        agent = exp["agent"]
        brain = exp["brain"]
        
        results = {
            "name": name,
            "total_steps": exp["steps_taken"],
            "total_reward": exp["total_reward"],
            "avg_reward_per_step": exp["total_reward"] / max(1, exp["steps_taken"]),
            "final_brain_stats": {
                "neurons": brain.getTotalNeuronCount(),
                "synapses": brain.getTotalSynapseCount(),
                "firing_neuron_count": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "development_stage": brain.getDevelopmentalStage(),
                "learned_preference": brain.getLearnedPreference()
            },
            "final_agent_stats": {
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "neuromodulation_level": agent.getNeuromodulationLevel(),
                "reward_modulation_enabled": agent.isRewardModulationEnabled(),
                "curiosity_enabled": agent.isCuriosityEnabled(),
                "structural_plasticity_enabled": agent.isStructuralPlasticityEnabled(),
                "development_enabled": agent.isDevelopmentEnabled()
            },
            "duration_seconds": time.time() - exp["start_time"]
        }
        
        return results
    
    def save_experiment_progress(self, name: str, filename: str):
        """Save experiment progress to file."""
        
        if name not in self.experiments:
            raise ValueError(f"Experiment not found: {name}")
        
        exp = self.experiments[name]
        brain = exp["brain"]
        pynlm.saveBrainState(brain, filename)
        self.checkpoints[f"{name}_{filename}"] = time.time()
        
        print(f"Progress saved to {filename}")
    
    def load_experiment_progress(self, name: str, filename: str):
        """Load experiment progress from file."""
        
        if name not in self.experiments:
            raise ValueError(f"Experiment not found: {name}")
        
        loaded_brain = pynlm.loadBrainState(filename)
        self.experiments[name]["brain"] = loaded_brain
        
        print(f"Progress loaded from {filename}")

# Usage example
if __name__ == "__main__":
    # Create learning environment manager
    manager = LearningEnvironmentManager()
    
    # Create research experiment
    research_exp = manager.create_experiment("research_training", "research")
    
    # Create quick demo
    demo_exp = manager.create_experiment("demo", "quick_demo")
    
    # Run research experiment
    research_results = manager.run_experiment("research_training", 500, save_progress=True)
    
    # Print results
    print("\n=== Research Experiment Results ===")
    for key, value in research_results.items():
        if key != "name":
            print(f"{key}: {value}")
    
    # Save final results
    manager.save_experiment_progress("research_training", "final_research.pkl")
    
    print("\nLearning environment manager completed successfully!")
```

---

# Advanced Example 2: Parallel Experiment Runner

Run multiple experiments in parallel using threading for efficient resource utilization.

```python
import pynlm
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import Dict, List, Tuple

class ParallelExperimentRunner:
    """Run multiple experiments in parallel for efficient resource utilization."""
    
    def __init__(self, max_workers: int = 4):
        self.max_workers = max_workers
        self.results = {}
    
    def run_single_experiment(self, experiment_id: int, config_type: str, steps: int) -> dict:
        """Run a single experiment with specified configuration."""
        
        print(f"Starting experiment {experiment_id} ({config_type})...")
        
        # Create learning environment
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=15 + (experiment_id % 5),  # Vary world size
            visionRange=6 + (experiment_id % 3),
            enableCuriosity=(experiment_id % 3 != 0),
            enableRewardModulation=True
        )
        
        # Configure based on type
        if config_type == "stable":
            brain.getConfig().set("brain.connection_probability", 0.05)
            brain.getConfig().set("plasticity.stdp.learning_rate", 0.0005)
        elif config_type == "exploratory":
            brain.getConfig().set("brain.connection_probability", 0.25)
            brain.getConfig().set("neuromod.dopamine.scale", 2.0)
        
        # Run experiment
        total_reward = 0.0
        max_firing_rate = 0.0
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            total_reward += reward
            max_firing_rate = max(max_firing_rate, brain.getAverageFiringRate())
            
            if step % 100 == 0:
                print(f"  Experiment {experiment_id}: Step {step}")
        
        result = {
            "experiment_id": experiment_id,
            "config_type": config_type,
            "total_steps": steps,
            "total_reward": total_reward,
            "avg_reward_per_step": total_reward / steps,
            "max_firing_rate": max_firing_rate,
            "final_neurons": brain.getTotalNeuronCount(),
            "final_synapses": brain.getTotalSynapseCount(),
            "completed_successfully": True
        }
        
        print(f"Experiment {experiment_id} completed with reward {total_reward:.2f}")
        return result
    
    def run_parallel_experiments(self, experiment_configs: List[Tuple[int, str, int]]) -> Dict[int, dict]:
        """Run multiple experiments in parallel."""
        
        print(f"Running {len(experiment_configs)} experiments in parallel...")
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all experiments
            future_to_exp = {
                executor.submit(self.run_single_experiment, exp_id, config_type, steps): 
                (exp_id, config_type, steps)
                for exp_id, config_type, steps in experiment_configs
            }
            
            # Collect results
            for future in as_completed(future_to_exp):
                exp_id, config_type, steps = future_to_exp[future]
                try:
                    result = future.result()
                    self.results[exp_id] = result
                except Exception as e:
                    print(f"Experiment {exp_id} failed with error: {e}")
                    self.results[exp_id] = {
                        "experiment_id": exp_id,
                        "config_type": config_type,
                        "total_steps": steps,
                        "completed_successfully": False,
                        "error": str(e)
                    }
        
        total_time = time.time() - start_time
        print(f"All experiments completed in {total_time:.2f} seconds")
        return self.results
    
    def get_summary_statistics(self) -> dict:
        """Get summary statistics of all completed experiments."""
        
        if not self.results:
            return {}
        
        successful_results = [r for r in self.results.values() if r.get("completed_successfully", False)]
        
        if not successful_results:
            return {"error": "No successful experiments to analyze"}
        
        rewards = [r["total_reward"] for r in successful_results]
        firing_rates = [r["max_firing_rate"] for r in successful_results]
        
        return {
            "total_experiments": len(self.results),
            "successful_experiments": len(successful_results),
            "success_rate": len(successful_results) / len(self.results) * 100,
            "reward_statistics": {
                "mean": sum(rewards) / len(rewards),
                "min": min(rewards),
                "max": max(rewards),
                "std": (sum((r - sum(rewards)/len(rewards))**2 for r in rewards) / len(rewards))**0.5
            },
            "firing_rate_statistics": {
                "mean": sum(firing_rates) / len(firing_rates),
                "min": min(firing_rates),
                "max": max(firing_rates)
            }
        }

# Usage example
if __name__ == "__main__":
    # Create parallel experiment runner
    runner = ParallelExperimentRunner(max_workers=3)
    
    # Define experiments to run
    experiments = [
        (1, "stable", 300),
        (2, "exploratory", 300),
        (3, "stable", 400),
        (4, "exploratory", 300),
        (5, "stable", 500),
        (6, "exploratory", 400)
    ]
    
    # Run all experiments
    results = runner.run_parallel_experiments(experiments)
    
    # Print summary statistics
    summary = runner.get_summary_statistics()
    print("\n=== Parallel Experiment Summary ===")
    for key, value in summary.items():
        if isinstance(value, dict):
            print(f"{key}:")
            for sub_key, sub_value in value.items():
                print(f"  {sub_key}: {sub_value}")
        else:
            print(f"{key}: {value}")
```

---

# Advanced Example 3: Advanced Configuration Manager

Manage complex configurations with validation, presets, and inheritance.

```python
import pynlm
from typing import Dict, Any, Optional
import json

class AdvancedConfigurationManager:
    """Manage advanced NLM configurations with validation and presets."""
    
    def __init__(self):
        self.presets = {}
        self.validations = {}
        self._initialize_presets()
        self._initialize_validations()
    
    def _initialize_presets(self):
        """Initialize configuration presets."""
        
        self.presets = {
            "minimal": {
                "brain.neuron_count": 100,
                "brain.region_count": 1,
                "brain.connection_probability": 0.05,
                "plasticity.stdp.enable": True,
                "neuromod.curiosity.enable": False,
                "development.enable": False
            },
            "balanced": {
                "brain.neuron_count": 500,
                "brain.region_count": 2,
                "brain.connection_probability": 0.15,
                "plasticity.stdp.enable": True,
                "plasticity.stdp.learning_rate": 0.001,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enable": True,
                "development.synaptogenesis_rate": 0.0001,
                "development.pruning_rate": 0.00001
            },
            "high_performance": {
                "brain.neuron_count": 2000,
                "brain.region_count": 3,
                "brain.connection_probability": 0.20,
                "plasticity.stdp.enable": True,
                "plasticity.stdp.learning_rate": 0.002,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "development.enable": True,
                "development.synaptogenesis_rate": 0.00015,
                "development.pruning_rate": 0.000015,
                "simulation_timestep": 0.0005
            },
            "research_optimized": {
                "brain.neuron_count": 1500,
                "brain.region_count": 4,
                "brain.connection_probability": 0.10,
                "plasticity.stdp.enable": True,
                "plasticity.stdp.learning_rate": 0.001,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "development.enable": True,
                "development.synaptogenesis_rate": 0.00012,
                "development.pruning_rate": 0.000012,
                "simulation_timestep": 0.001,
                "checkpoint_interval": 1000,
                "memory_capacity": 1000
            },
            "production_ready": {
                "brain.neuron_count": 1000,
                "brain.region_count": 2,
                "brain.connection_probability": 0.12,
                "plasticity.stdp.enable": True,
                "plasticity.stdp.learning_rate": 0.0008,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.2,
                "development.enable": True,
                "development.synaptogenesis_rate": 0.00008,
                "development.pruning_rate": 0.000008,
                "simulation_timestep": 0.001,
                "checkpoint_interval": 500,
                "memory_capacity": 500,
                "error_retry_attempts": 3,
                "log_level": "info"
            }
        }
    
    def _initialize_validations(self):
        """Initialize configuration validations."""
        
        self.validations = {
            "brain.neuron_count": lambda x: 10 <= x <= 100000,
            "brain.region_count": lambda x: 1 <= x <= 10,
            "brain.connection_probability": lambda x: 0.0 < x <= 1.0,
            "plasticity.stdp.learning_rate": lambda x: 0.0 < x <= 0.1,
            "neuromod.dopamine.scale": lambda x: 0.0 < x <= 5.0,
            "development.synaptogenesis_rate": lambda x: 0.0 <= x <= 0.01,
            "development.pruning_rate": lambda x: 0.0 <= x <= 0.01,
            "simulation_timestep": lambda x: 0.0 < x <= 0.1
        }
    
    def create_preset_config(self, preset_name: str) -> pynlm.Config:
        """Create a configuration from a preset."""
        
        if preset_name not in self.presets:
            raise ValueError(f"Unknown preset: {preset_name}")
        
        config = pynlm.createDefaultConfig()
        
        for key, value in self.presets[preset_name].items():
            config.set(key, value)
        
        return config
    
    def validate_config(self, config: pynlm.Config) -> Tuple[bool, List[str]]:
        """Validate a configuration."""
        
        errors = []
        
        for key, validator in self.validations.items():
            if config.has(key):
                try:
                    value = config.getOr(key, 0)
                    if not validator(value):
                        errors.append(f"Invalid value for {key}: {value}")
                except Exception as e:
                    errors.append(f"Error validating {key}: {e}")
        
        # Cross-validation checks
        if config.has("brain.neuron_count") and config.has("brain.region_count"):
            neurons = config.getOr("brain.neuron_count", 0)
            regions = config.getOr("brain.region_count", 0)
            if neurons < regions:
                errors.append(f"Not enough neurons ({neurons}) for regions ({regions})")
        
        return len(errors) == 0, errors
    
    def export_config(self, config: pynlm.Config, filename: str):
        """Export configuration to JSON file."""
        
        export_data = {}
        keys = config.getKeys()
        
        for key in keys:
            try:
                value = config.get(key)
                export_data[key] = value
            except:
                # Skip keys that can't be retrieved directly
                pass
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        print(f"Configuration exported to {filename}")
    
    def import_config(self, filename: str) -> pynlm.Config:
        """Import configuration from JSON file."""
        
        with open(filename, 'r') as f:
            import_data = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        for key, value in import_data.items():
            try:
                config.set(key, value)
            except Exception as e:
                print(f"Warning: Could not import key {key}: {e}")
        
        # Validate imported config
        is_valid, errors = self.validate_config(config)
        if not is_valid:
            print(f"Warning: Imported configuration has validation errors: {errors}")
        
        return config
    
    def create_compatible_configs(self, base_preset: str, variations: List[str]) -> Dict[str, pynlm.Config]:
        """Create variations of a base configuration."""
        
        base_config = self.create_preset_config(base_preset)
        
        compatible_configs = {base_preset: base_config}
        
        for variation in variations:
            variation_config = pynlm.createDefaultConfig()
            
            # Copy all settings from base config
            base_keys = base_config.getKeys()
            for key in base_keys:
                try:
                    value = base_config.get(key)
                    variation_config.set(key, value)
                except:
                    pass
            
            # Apply variation-specific modifications
            if variation == "small":
                variation_config.set("brain.neuron_count", base_config.getOr("brain.neuron_count", 0) // 2)
            elif variation == "large":
                variation_config.set("brain.neuron_count", base_config.getOr("brain.neuron_count", 0) * 2)
            elif variation == "stable":
                variation_config.set("brain.connection_probability", 0.05)
            elif variation == "exploratory":
                variation_config.set("neuromod.dopamine.scale", 2.0)
            
            compatible_configs[f"{base_preset}_{variation}"] = variation_config
        
        return compatible_configs

# Backward compatibility function
import pynlm
class LegacyCompatibility:
    """Provide backward compatibility for older code patterns."""
    
    @staticmethod
    def create_basic_brain():
        """Legacy function for creating a basic brain (backward compatibility)."""
        return pynlm.createBrain(pynlm.createDefaultConfig())
    
    @staticmethod
    def create_demo_world():
        """Legacy function for creating a demo world (backward compatibility)."""
        return pynlm.createSimpleWorld()
    
    @staticmethod
    def create_agent(brain):
        """Legacy function for creating an agent (backward compatibility)."""
        return pynlm.createAgentBrain(brain)

# Usage example
if __name__ == "__main__":
    # Create configuration manager
    config_manager = AdvancedConfigurationManager()
    
    # Create preset configurations
    minimal_config = config_manager.create_preset_config("minimal")
    balanced_config = config_manager.create_preset_config("balanced")
    high_perf_config = config_manager.create_preset_config("high_performance")
    
    print("Preset configurations created successfully!")
    
    # Validate configurations
    for config_name, config in [("minimal", minimal_config), ("balanced", balanced_config), ("high_perf", high_perf_config)]:
        is_valid, errors = config_manager.validate_config(config)
        if is_valid:
            print(f"{config_name} configuration is valid")
        else:
            print(f"{config_name} configuration has errors: {errors}")
    
    # Export configuration
    config_manager.export_config(balanced_config, "balanced_config.json")
    
    # Import and validate configuration
    imported_config = config_manager.import_config("balanced_config.json")
    is_valid, errors = config_manager.validate_config(imported_config)
    print(f"Imported configuration valid: {is_valid}, errors: {errors}")
```

---

# Advanced Example 4: Performance Testing and Benchmarking

Comprehensive performance testing framework for NLM configurations.

```python
import pynlm
import time
import statistics
from typing import List, Dict, Any
import psutil
import os

class NLMBenchmark:
    """Performance testing and benchmarking framework for NLM."""
    
    def __init__(self):
        self.results = {}
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
    
    def benchmark_configuration(self, config_name: str, brain, world, agent, 
                              steps: int, time_step: float = 0.1) -> Dict[str, Any]:
        """Benchmark a single configuration."""
        
        print(f"Starting benchmark for {config_name}...")
        
        start_time = time.time()
        start_memory = self.get_memory_usage()
        
        # Run simulation
        step_times = []
        reward_values = []
        firing_rates = []
        
        for step in range(steps):
            step_start = time.time()
            
            world.update(time_step)
            agent.processSensoryInput(world.getSensoryPer졸)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(time_step)
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            reward_values.append(reward)
            firing_rates.append(brain.getAverageFiringRate())
        
        end_time = time.time()
        end_memory = self.get_memory_usage()
        
        # Calculate statistics
        total_time = end_time - start_time
        sim_time = steps * time_step
        time_per_step = total_time / steps
        
        benchmark_result = {
            "config_name": config_name,
            "total_steps": steps,
            "total_time": total_time,
            "simulated_time": sim_time,
            "real_time_per_step": time_per_step,
            "steps_per_second": steps / total_time,
            "memory_start_mb": start_memory,
            "memory_end_mb": end_memory,
            "memory_peak_mb": max(start_memory, end_memory),
            "memory_delta_mb": end_memory - start_memory,
            "rewards": {
                "mean": statistics.mean(reward_values),
                "median": statistics.median(reward_values),
                "min": min(reward_values),
                "max": max(reward_values),
                "std": statistics.stdev(reward_values) if len(reward_values) > 1 else 0.0
            },
            "firing_rates": {
                "mean": statistics.mean(firing_rates),
                "median": statistics.median(firing_rates),
                "min": min(firing_rates),
                "max": max(firing_rates)
            },
            "step_time_stats": {
                "mean": statistics.mean(step_times),
                "median": statistics.median(step_times),
                "min": min(step_times),
                "max": max(step_times),
                "std": statistics.stdev(step_times) if len(step_times) > 1 else 0.0
            }
        }
        
        print(f"Benchmark for {config_name} completed!")
        print(f"  Performance: {steps/total_time:.1f} steps/second")
        print(f"  Memory delta: {benchmark_result['memory_delta_mb']:.1f} MB")
        print(f"  Average reward: {benchmark_result['rewards']['mean']:.3f}")
        
        return benchmark_result
    
    def benchmark_multiple_configs(self, config_list: List[Dict[str, Any]], 
                                 steps_per_config: int = 1000) -> Dict[str, Any]:
        """Benchmark multiple configurations."""
        
        print(f"Benchmarking {len(config_list)} configurations...")
        
        for i, config_spec in enumerate(config_list):
            print(f"\n{progress bar} Configuration {i+1}/{len(config_list)}: {config_spec.get('name', 'Unnamed')}")
            
            config_name = config_spec.get('name', f"config_{i}")
            
            # Create learning environment
            brain, world, agent = pynlm.createLearningEnvironment(
                worldSize=config_spec.get('world_size', 15),
                visionRange=config_spec.get('vision_range', 8),
                enableCuriosity=config_spec.get('enable_curiosity', True),
                enableRewardModulation=config_spec.get('enable_reward', True)
            )
            
            # Apply custom configuration if specified
            if 'custom_config' in config_spec:
                for key, value in config_spec['custom_config'].items():
                    brain.getConfig().set(key, value)
            
            # Run benchmark
            result = self.benchmark_configuration(
                config_name, brain, world, agent, 
                steps_per_config, config_spec.get('time_step', 0.1)
            )
            
            self.results[config_name] = result
        
        return self.get_summary_statistics()
    
    def get_summary_statistics(self) -> Dict[str, Any]:
        """Get summary statistics across all benchmarks."""
        
        if not self.results:
            return {}
        
        successful_results = [r for r in self.results.values() if 'error' not in r]
        
        if not successful_results:
            return {"error": "No successful benchmarks to analyze"}
        
        all_steps_per_second = [r['steps_per_second'] for r in successful_results]
        all_memory_delta = [r['memory_delta_mb'] for r in successful_results]
        
        summary = {
            "total_benchmarks": len(self.results),
            "successful_benchmarks": len(successful_results),
            "success_rate": len(successful_results) / len(self.results) * 100,
            "performance_comparison": {
                "steps_per_second": {
                    "mean": statistics.mean(all_steps_per_second),
                    "median": statistics.median(all_steps_per_second),
                    "min": min(all_steps_per_second),
                    "max": max(all_steps_per_second),
                    "std": statistics.stdev(all_steps_per_second) if len(all_steps_per_second) > 1 else 0.0
                },
                "memory_delta": {
                    "mean": statistics.mean(all_memory_delta),
                    "median": statistics.median(all_memory_delta),
                    "min": min(all_memory_delta),
                    "max": max(all_memory_delta),
                    "std": statistics.stdev(all_memory_delta) if len(all_memory_delta) > 1 else 0.0
                }
            },
            "best_performers": {
                "steps_per_second": max(successful_results, key=lambda x: x['steps_per_second'])['config_name'],
                "memory_efficiency": min(successful_results, key=lambda x: abs(x['memory_delta_mb']))['config_name']
            }
        }
        
        return summary

# Usage example
if __name__ == "__main__":
    # Create benchmark suite
    benchmark = NLMBenchmark()
    
    # Define configurations to benchmark
    configs = [
        {
            "name": "minimal_config",
            "world_size": 10,
            "vision_range": 6,
            "enable_curiosity": False,
            "enable_reward": True
        },
        {
            "name": "balanced_config",
            "world_size": 15,
            "vision_range": 8,
            "enable_curiosity": True,
            "enable_reward": True
        },
        {
            "name": "high_perf_config",
            "world_size": 20,
            "vision_range": 10,
            "enable_curiosity": True,
            "enable_reward": True
        }
    ]
    
    # Run benchmarks
    results = benchmark.benchmark_multiple_configs(configs, steps_per_config=500)
    
    # Print summary
    print("\n" + "="*50)
    print("BENCHMARK SUMMARY")
    print("="*50)
    
    for key, value in results.items():
        if isinstance(value, dict):
            print(f"{key}:")
            for sub_key, sub_value in value.items():
                print(f"  {sub_key}: {sub_value}")
        else:
            print(f"{key}: {value}")
```

---

# Advanced Example 5: Error Handling and Recovery

Robust error handling with automatic recovery mechanisms.

```python
import pynlm
import traceback
import time
from typing import Dict, Any, Callable

class ErrorHandler:
    """Robust error handling with automatic recovery for NLM operations."""
    
    def __init__(self, max_retries: int = 3, retry_delay: float = 1.0):
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        self.error_history = []
        self.recovery_handlers = {}
    
    def add_recovery_handler(self, error_type: str, handler: Callable):
        """Add a recovery handler for a specific error type."""
        self.recovery_handlers[error_type] = handler
    
    def execute_with_recovery(self, operation: Callable, *args, **kwargs) -> Any:
        """Execute an operation with automatic recovery."""
        
        last_exception = None
        
        for attempt in range(self.max_retries + 1):
            try:
                result = operation(*args, **kwargs)
                
                if attempt > 0:
                    print(f"Operation succeeded on attempt {attempt + 1}")
                
                return result
                
            except Exception as e:
                last_exception = e
                error_type = type(e).__name__
                
                self.error_history.append({
                    "timestamp": time.time(),
                    "attempt": attempt + 1,
                    "error_type": error_type,
                    "error_message": str(e),
                    "traceback": traceback.format_exc()
                })
                
                print(f"Attempt {attempt + 1} failed: {error_type}: {e}")
                
                if attempt == self.max_retries:
                    print(f"Max retries ({self.max_retries}) exceeded")
                    break
                
                # Apply recovery handler if available
                if error_type in self.recovery_handlers:
                    print(f"Attempting recovery with handler for {error_type}")
                    try:
                        self.recovery_handlers[error_type](*args, **kwargs)
                        time.sleep(self.retry_delay)
                    except Exception as recovery_error:
                        print(f"Recovery handler failed: {recovery_error}")
                else:
                    time.sleep(self.retry_delay)
        
        # If we get here, all retries failed
        raise RuntimeError(f"Operation failed after {self.max_retries + 1} attempts") from last_exception

class RobustNLMInterface:
    """Robust NLM interface with comprehensive error handling."""
    
    def __init__(self):
        self.error_handler = ErrorHandler(max_retries=3, retry_delay=0.5)
        self.setup_recovery_handlers()
    
    def setup_recovery_handlers(self):
        """Setup recovery handlers for common error scenarios."""
        
        # Handler for brain initialization failures
        def brain_initialization_handler(*args, **kwargs):
            print("Recovering from brain initialization failure...")
            # Reset configuration and try simpler initialization
            config = kwargs.get('config') or args[0] if args else None
            if config:
                # Try with minimal configuration
                config.set("brain.neuron_count", 100)
                config.set("brain.region_count", 1)
                config.set("brain.connection_probability", 0.05)
        
        # Handler for save/load failures
        def save_load_handler(*args, **kwargs):
            print("Recovering from save/load failure...")
            # Try with different file format or location
            filename = kwargs.get('filename') or args[1] if len(args) > 1 else "fallback.pkl"
            # Ensure directory exists
            import os
            os.makedirs(os.path.dirname(filename), exist_ok=True)
        
        # Handler for simulation errors
        def simulation_handler(*args, **kwargs):
            print("Recovering from simulation error...")
            # Reduce simulation speed or parameters
            world = kwargs.get('world') or args[1] if len(args) > 1 else None
            if world:
                # Store original settings and apply safer ones
                pass
        
        self.error_handler.add_recovery_handler("BrainInitializationError", brain_initialization_handler)
        self.error_handler.add_recovery_handler("SaveError", save_load_handler)
        self.error_handler.add_recovery_handler("LoadError", save_load_handler)
        self.error_handler.add_recovery_handler("SimulationError", simulation_handler)
    
    def safe_create_brain(self, neuron_count: int = 500, connection_prob: float = 0.15) -> pynlm.Brain:
        """Safely create a brain with error handling and recovery."""
        
        return self.error_handler.execute_with_recovery(
            lambda: self._create_brain_with_validation(neuron_count, connection_prob)
        )
    
    def _create_brain_with_validation(self, neuron_count: int, connection_prob: float) -> pynlm.Brain:
        """Internal brain creation with validation."""
        
        # Validate parameters
        if neuron_count < 10 or neuron_count > 100000:
            raise ValueError(f"Invalid neuron count: {neuron_count}")
        
        if connection_prob <= 0 or connection_prob > 1.0:
            raise ValueError(f"Invalid connection probability: {connection_prob}")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 1)
        config.set("brain.connection_probability", connection_prob)
        
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        return brain
    
    def safe_run_simulation(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                           agent: pynlm.AgentBrain, steps: int, 
                           time_step: float = 0.1) -> Dict[str, Any]:
        """Safely run a simulation with error handling and recovery."""
        
        return self.error_handler.execute_with_recovery(
            self._run_simulation_core, brain, world, agent, steps, time_step
        )
    
    def _run_simulation_core(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                             agent: pynlm.AgentBrain, steps: int, 
                             time_step: float) -> Dict[str, Any]:
        """Core simulation implementation."""
        
        total_reward = 0.0
        steps_completed = 0
        errors = []
        
        for step in range(steps):
            try:
                # Update world
                world.update(time_step)
                
                # Process sensory input
                agent.processSensoryInput(world.getSensoryPercept())
                
                # Brain step
                brain.step(step)
                
                # Get action and apply it
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get reward and apply learning
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(time_step)
                
                total_reward += reward
                steps_completed += 1
                
            except Exception as e:
                errors.append(f"Step {step}: {type(e).__name__}: {e}")
                print(f"Error at step {step}: {e}")
                # Continue simulation despite individual step errors
                continue
        
        return {
            "total_reward": total_reward,
            "steps_completed": steps_completed,
            "errors": errors,
            "success_rate": steps_completed / steps * 100 if steps > 0 else 0.0
        }
    
    def safe_save_brain(self, brain: pynlm.Brain, filename: str) -> bool:
        """Safely save a brain state with error handling and recovery."""
        
        return self.error_handler.execute_with_recovery(
            pynlm.saveBrainState, brain, filename
        )
    
    def safe_load_brain(self, filename: str) -> pynlm.Brain:
        """Safely load a brain state with error handling and recovery."""
        
        return self.error_handler.execute_with_recovery(
            pynlm.loadBrainState, filename
        )

# Usage example
if __name__ == "__main__":
    # Create robust interface
    robust_interface = RobustNLMInterface()
    
    try:
        # Safely create brain
        print("Creating brain with robust error handling...")
        brain = robust_interface.safe_create_brain(neuron_count=500, connection_prob=0.15)
        print(f"Brain created successfully with {brain.getTotalNeuronCount()} neurons")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Safely run simulation
        print("Running simulation with robust error handling...")
        results = robust_interface.safe_run_simulation(brain, world, agent, 200)
        
        print(f"Simulation completed: {results['steps_completed']}/{200} steps")
        print(f"Total reward: {results['total_reward']:.2f}")
        print(f"Success rate: {results['success_rate']:.1f}%")
        if results['errors']:
            print(f"Errors encountered: {len(results['errors'])}")
            for error in results['errors'][-5:]:  # Show last 5 errors
                print(f"  - {error}")
        
        # Safely save results
        print("Saving brain state...")
        if robust_interface.safe_save_brain(brain, "robust_brain.pkl"):
            print("Brain saved successfully!")
            
            # Try to load it back
            print("Loading brain state...")
            loaded_brain = robust_interface.safe_load_brain("robust_brain.pkl")
            print(f"Brain loaded successfully with {loaded_brain.getTotalNeuronCount()} neurons")
        
        print("\nRobust interface completed successfully!")
        
    except Exception as e:
        print(f"Critical error: {type(e).__name__}: {e}")
        print("Error history:")
        for error_entry in robust_interface.error_handler.error_history:
            print(f"  Attempt {error_entry['attempt']}: {error_entry['error_type']}: {error_entry['error_message']}")
```

---

# Advanced Example 6: Real-time Monitoring and Visualization

Real-time monitoring and visualization framework for NLM simulations.

```python
import pynlm
import time
import threading
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any
from collections import deque

class RealTimeMonitor:
    """Real-time monitoring and visualization for NLM simulations."""
    
    def __init__(self, update_interval: float = 0.1):
        self.update_interval = update_interval
        self.monitoring = False
        self.monitor_thread = None
        
        # Data buffers
        self.step_data = deque(maxlen=1000)
        self.reward_data = deque(maxlen=1000)
        self.firing_rate_data = deque(maxlen=1000)
        self.curiosity_data = deque(maxlen=1000)
        self.memory_data = deque(maxlen=1000)
        
        # Statistics
        self.stats = {
            "total_steps": 0,
            "total_reward": 0.0,
            "max_firing_rate": 0.0,
            "min_firing_rate": 0.0,
            "start_time": 0.0,
            "last_update": 0.0
        }
    
    def start_monitoring(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                        agent: pynlm.AgentBrain) -> bool:
        """Start real-time monitoring."""
        
        if self.monitoring:
            print("Monitoring already started")
            return False
        
        self.monitoring = True
        self.stats["start_time"] = time.time()
        self.stats["last_update"] = time.time()
        
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop, args=(brain, world, agent),
            daemon=True
        )
        self.monitor_thread.start()
        
        print("Real-time monitoring started")
        return True
    
    def stop_monitoring(self) -> Dict[str, Any]:
        """Stop real-time monitoring and return final statistics."""
        
        if not self.monitoring:
            print("Monitoring not started")
            return self.get_statistics()
        
        self.monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        print("Real-time monitoring stopped")
        return self.get_statistics()
    
    def _monitor_loop(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                     agent: pynlm.AgentBrain):
        """Main monitoring loop."""
        
        while self.monitoring:
            try:
                current_time = time.time()
                
                # Collect data
                self.step_data.append(self.stats["total_steps"])
                self.reward_data.append(self.stats["total_reward"])
                self.firing_rate_data.append(brain.getAverageFiringRate())
                self.curiosity_data.append(agent.getCuriosityLevel())
                
                # Update statistics
                self.stats["last_update"] = current_time
                
                # Sleep for update interval
                time.sleep(self.update_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                break
    
    def update_stats(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, 
                    reward: float, steps_completed: int):
        """Update monitoring statistics."""
        
        self.stats["total_steps"] = steps_completed
        self.stats["total_reward"] = reward
        self.stats["max_firing_rate"] = max(self.stats["max_firing_rate"], 
                                           brain.getAverageFiringRate())
        self.stats["min_firing_rate"] = min(self.stats["min_firing_rate"], 
                                           brain.getAverageFiringRate())
    
    def get_statistics(self) -> Dict[str, Any]:n        """Get current monitoring statistics."""
        
        if not self.step_data:
            return {"error": "No data collected"}
        
        return {
            "total_steps": self.stats["total_steps"],
            "total_reward": self.stats["total_reward"],
            "avg_reward_per_step": self.stats["total_reward"] / max(1, self.stats["total_steps"]),
            "duration_seconds": time.time() - self.stats["start_time"],
            "steps_per_second": self.stats["total_steps"] / max(1, time.time() - self.stats["start_time"]),
            "firing_rate": {
                "current": self.firing_rate_data[-1] if self.firing_rate_data else 0.0,
                "mean": np.mean(list(self.firing_rate_data)) if self.firing_rate_data else 0.0,
                "max": np.max(list(self.firing_rate_data)) if self.firing_rate_data else 0.0,
                "min": np.min(list(self.firing_rate_data)) if self.firing_rate_data else 0.0
            },
            "curiosity": {
                "current": self.curiosity_data[-1] if self.curiosity_data else 0.0,
                "mean": np.mean(list(self.curiosity_data)) if self.curiosity_data else 0.0,
                "max": np.max(list(self.curiosity_data)) if self.curiosity_data else 0.0,
                "min": np.min(list(self.curiosity_data)) if self.curiosity_data else 0.0
            },
            "data_points": len(self.step_data)
        }
    
    def plot_realtime(self, save_plots: bool = False):
        """Generate real-time plots."""
        
        if not self.step_data:
            print("No data to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("NLM Real-time Monitoring", fontsize=16)
        
        # Plot 1: Reward over time
        axes[0, 0].plot(self.step_data, self.reward_data, 'g-', linewidth=1, alpha=0.7)
        axes[0, 0].set_xlabel("Step")
        axes[0, 0].set_ylabel("Total Reward")
        axes[0, 0].set_title("Reward Accumulation")
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Firing rate over time
        axes[0, 1].plot(self.step_data, self.firing_rate_data, 'r-', linewidth=1, alpha=0.7)
        axes[0, 1].set_xlabel("Step")
        axes[0, 1].set_ylabel("Firing Rate")
        axes[0, 1].set_title("Average Firing Rate")
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Curiosity over time
        axes[1, 0].plot(self.step_data, self.curiosity_data, 'b-', linewidth=1, alpha=0.7)
        axes[1, 0].set_xlabel("Step")
        axes[1, 0].set_ylabel("Curiosity Level")
        axes[1, 0].set_title("Curiosity Level")
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Step rate (performance)
        if len(self.step_data) > 10:
            x = list(self.step_data)
            y = np.diff(x) / np.diff(self.step_data)  # Steps per unit step
            axes[1, 1].plot(x[1:], y, 'c-', linewidth=1, alpha=0.7)
            axes[1, 1].set_xlabel("Step")
            axes[1, 1].set_ylabel("Steps/Second")
            axes[1, 1].set_title("Simulation Performance")
            axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_plots:
            plt.savefig('nlm_realtime_monitoring.png', dpi=150, bbox_inches='tight')
            print("Plot saved as 'nlm_realtime_monitoring.png'")
        
        plt.show()

# Usage example
if __name__ == "__main__":
    # Create monitoring instance
    monitor = RealTimeMonitor(update_interval=0.05)
    
    try:
        # Create learning environment
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=20,
            visionRange=8,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        
        # Start monitoring
        monitor.start_monitoring(brain, world, agent)
        
        # Run simulation with monitoring
        print("Running simulation with real-time monitoring...")
        
        total_reward = 0.0
        steps_completed = 0
        
        for step in range(500):
            # Update world
            world.update(0.1)
            
            # Get and process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            steps_completed += 1
            
            # Update monitoring
            monitor.update_stats(brain, agent, total_reward, steps_completed)
            
            # Print status every 100 steps
            if (step + 1) % 100 == 0:
                print(f"Step {step + 1}: Reward={total_reward:.2f}, "
                      f"Firing Rate={brain.getAverageFiringRate():.2f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
        
        # Stop monitoring and get statistics
        stats = monitor.stop_monitoring()
        
        print("\n" + "="*50)
        print("MONITORING STATISTICS")
        print("="*50)
        
        for key, value in stats.items():
            if isinstance(value, dict):
                print(f"{key}:")
                for sub_key, sub_value in value.items():
                    print(f"  {sub_key}: {sub_value}")
            else:
                print(f"{key}: {value}")
        
        # Generate plots
        print("\nGenerating plots...")
        monitor.plot_realtime(save_plots=True)
        
        print("\nReal-time monitoring completed successfully!")
        
    except Exception as e:
        print(f"Monitoring error: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
```

---

# Advanced Example 7: Integration with External Systems

Integration framework for connecting NLM with external systems and APIs.

```python
import pynlm
import json
import requests
import time
from typing import Dict, Any, Optional
from abc import ABC, abstractmethod

class ExternalSystemInterface(ABC):
    """Abstract base class for external system interfaces."""
    
    @abstractmethod
    def connect(self) -> bool:
        """Connect to the external system."""
        pass
    
    @abstractmethod
    def disconnect(self) -> bool:
        """Disconnect from the external system."""
        pass
    
    @abstractmethod
    def send_data(self, data: Dict[str, Any]) -> bool:
        """Send data to the external system."""
        pass
    
    @abstractmethod
    def receive_data(self) -> Optional[Dict[str, Any]]:
        """Receive data from the external system."""
        pass

class APISystemInterface(ExternalSystemInterface):
    """Interface for REST API external systems."""
    
    def __init__(self, base_url: str, api_key: str = None):
        self.base_url = base_url.rstrip('/')
        self.api_key = api_key
        self.session = requests.Session()
        
        if api_key:
            self.session.headers.update({'Authorization': f'Bearer {api_key}'})
    
    def connect(self) -> bool:
        """Test connection to API."""
        try:
            response = self.session.get(f"{self.base_url}/health")
            return response.status_code == 200
        except Exception:
            return False
    
    def disconnect(self) -> bool:
        """Disconnect from API."""
        self.session.close()
        return True
    
    def send_data(self, data: Dict[str, Any]) -> bool:
        """Send data to API endpoint."""
        try:
            response = self.session.post(
                f"{self.base_url}/data",
                json=data,
                timeout=30
            )
            return response.status_code in [200, 201, 202]
        except Exception:
            return False
    
    def receive_data(self) -> Optional[Dict[str, Any]]:
        """Receive data from API endpoint."""
        try:
            response = self.session.get(
                f"{self.base_url}/data/latest",
                timeout=30
            )
            if response.status_code == 200:
                return response.json()
        except Exception:
            pass
        return None

class DatabaseSystemInterface(ExternalSystemInterface):
    """Interface for database external systems."""
    
    def __init__(self, connection_string: str):
        self.connection_string = connection_string
        self.connection = None
    
    def connect(self) -> bool:
        """Connect to database."""
        try:
            # Implementation would depend on database type
            # This is a placeholder
            self.connection = "connected"
            return True
        except Exception:
            return False
    
    def disconnect(self) -> bool:
        """Disconnect from database."""
        try:
            if self.connection:
                # Close connection
                self.connection = None
            return True
        except Exception:
            return False
    
    def send_data(self, data: Dict[str, Any]) -> bool:
        """Insert data into database."""
        try:
            # Implementation would depend on database type
            # This is a placeholder
            return True
        except Exception:
            return False
    
    def receive_data(self) -> Optional[Dict[str, Any]]:
        """Retrieve data from database."""
        try:
            # Implementation would depend on database type
            # This is a placeholder
            return {"status": "ok", "data": []}
        except Exception:
            return None

class NLMExternalIntegration:
    """Integration framework for connecting NLM with external systems."""
    
    def __init__(self):
        self.external_systems = {}
        self.integration_config = {}
        self.data_buffer = {}
        self.last_sync_time = {}
    
    def register_system(self, name: str, system: ExternalSystemInterface, 
                       config: Dict[str, Any] = None):
        """Register an external system for integration."""
        
        if not system.connect():
            raise RuntimeError(f"Failed to connect to external system: {name}")
        
        self.external_systems[name] = system
        self.integration_config[name] = config or {}
        self.data_buffer[name] = {}
        self.last_sync_time[name] = time.time()
        
        print(f"Registered external system: {name}")
    
    def unregister_system(self, name: str) -> bool:
        """Unregister an external system."""
        
        if name not in self.external_systems:
            return False
        
        if not self.external_systems[name].disconnect():
            print(f"Warning: Failed to cleanly disconnect system: {name}")
        
        del self.external_systems[name]
        del self.integration_config[name]
        del self.data_buffer[name]
        del self.last_sync_time[name]
        
        print(f"Unregistered external system: {name}")
        return True
    
    def collect_nlm_data(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, 
                        world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """Collect NLM data for external integration."""
        
        collected_data = {
            "timestamp": time.time(),
            "brain_state": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "firing_neuron_count": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "development_stage": brain.getDevelopmentalStage(),
                "learned_preference": brain.getLearnedPreference()
            },
            "agent_state": {
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "neuromodulation_level": agent.getNeuromodulationLevel(),
                "reward_modulation_enabled": agent.isRewardModulationEnabled(),
                "curiosity_enabled": agent.isCuriosityEnabled()
            },
            "world_state": {
                "simulation_time": world.getSimulationTime(),
                "agent_position": {
                    "x": world.getAgentBody().x,
                    "y": world.getAgentBody().y,
                    "orientation": world.getAgentBody().orientation
                },
                "agent_energy": world.getAgentBody().energy,
                "agent_health": world.getAgentBody().health,
                "agent_age": world.getAgentBody().age
            }
        }
        
        return collected_data
    
    def send_to_external_systems(self, data: Dict[str, Any]) -> Dict[str, bool]:
        """Send collected data to all registered external systems."""
        
        results = {}
        
        for system_name, system in self.external_systems.items():
            try:
                # Prepare data for this system
                system_data = self.prepare_data_for_system(system_name, data)
                
                # Send to external system
                success = system.send_data(system_data)
                
                results[system_name] = success
                
                if success:
                    self.last_sync_time[system_name] = time.time()
                    print(f"Data sent successfully to {system_name}")
                else:
                    print(f"Failed to send data to {system_name}")
                    
            except Exception as e:
                print(f"Error sending data to {system_name}: {e}")
                results[system_name] = False
        
        return results
    
    def prepare_data_for_system(self, system_name: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """Prepare data for a specific external system."""
        
        # Get system-specific configuration
        config = self.integration_config.get(system_name, {})
        
        # Create system-specific data payload
        system_data = {
            "source": "nlm",
            "timestamp": data["timestamp"],
            "system": system_name,
            "configuration": config
        }
        
        # Include only specified data fields based on configuration
        if system_name == "api_system":
            system_data.update({
                "brain_metrics": data["brain_state"],
                "agent_metrics": data["agent_state"],
                "world_metrics": data["world_state"]
            })
        
        elif system_name == "database_system":
            system_data.update({
                "simulation_id": f"sim_{int(data['timestamp'])}",
                "brain_metrics": data["brain_state"]["avg_firing_rate"],
                "agent_metrics": data["agent_state"]["curiosity_level"],
                "reward_accumulated": data.get("reward_accumulated", 0.0)
            })
        
        return system_data
    
    def start_data_collection(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, 
                             world: pynlm.SimpleWorld, interval: float = 10.0,
                             auto_send: bool = True):
        """Start continuous data collection and integration."""
        
        print("Starting NLM external integration...")
        
        def data_collection_loop():
            while True:
                try:
                    # Collect current NLM data
                    data = self.collect_nlm_data(brain, agent, world)
                    
                    # Store in buffer
                    for system_name in self.external_systems:
                        self.data_buffer[system_name].update(data)
                    
                    # Send to external systems if auto-send is enabled
                    if auto_send:
                        results = self.send_to_external_systems(data)
                        print(f"Data sent to {sum(results.values())}/{len(results)} systems")
                    
                    # Sleep for collection interval
                    time.sleep(interval)
                    
                except Exception as e:
                    print(f"Data collection error: {e}")
                    break
        
        # Start collection thread
        collection_thread = threading.Thread(target=data_collection_loop, daemon=True)
        collection_thread.start()
        
        return collection_thread
    
    def stop_data_collection(self, thread: threading.Thread) -> Dict[str, Any]:
        """Stop data collection and return integration statistics."""
        
        if thread:
            thread.join(timeout=5.0)
        
        # Generate integration statistics
        statistics = {
            "total_systems": len(self.external_systems),
            "active_systems": sum(1 for system in self.external_systems.values() if True),  # Would check connection status
            "data_points_collected": sum(len(buffer) for buffer in self.data_buffer.values()),
            "last_sync_times": self.last_sync_time
        }
        
        print("NLM external integration stopped")
        return statistics

# Usage example
if __name__ == "__main__":
    # Create NLM integration framework
    nlm_integration = NLMExternalIntegration()
    
    try:
        # Create learning environment
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=20,
            visionRange=8,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        
        # Create and register external systems
        # API system example
        api_system = APISystemInterface(
            base_url="https://api.example.com",
            api_key="your-api-key-here"
        )
        nlm_integration.register_system("api_system", api_system, {
            "data_fields": ["brain_metrics", "agent_metrics", "world_metrics"],
            "send_interval": 15.0
        })
        
        # Database system example
        db_system = DatabaseSystemInterface("your-connection-string-here")
        nlm_integration.register_system("database_system", db_system, {
            "table": "nlm_simulation_data",
            "batch_size": 100,
            "compression": True
        })
        
        # Start data collection and integration
        collection_thread = nlm_integration.start_data_collection(
            brain, world, agent, interval=5.0, auto_send=True
        )
        
        # Run simulation with external integration
        print("Running simulation with external system integration...")
        
        total_reward = 0.0
        
        for step in range(300):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            
            # Print status every 50 steps
            if (step + 1) % 50 == 0:
                print(f"Step {step + 1}: Reward={total_reward:.2f}, "
                      f"Brain Neurons={brain.getTotalNeuronCount()}, "
                      f"Agent Curiosity={agent.getCuriosityLevel():.3f}")
        
        # Stop integration and get statistics
        print("Stopping data collection...")
        stats = nlm_integration.stop_data_collection(collection_thread)
        
        print("\n" + "="*50)
        print("INTEGRATION STATISTICS")
        print("="*50)
        
        for key, value in stats.items():
            if isinstance(value, dict):
                print(f"{key}:")
                for sub_key, sub_value in value.items():
                    print(f"  {sub_key}: {sub_value}")
            else:
                print(f"{key}: {value}")
        
        print("\nExternal integration completed successfully!")
        
    except Exception as e:
        print(f"Integration error: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
    
    finally:
        # Cleanup
        for system_name in list(nlm_integration.external_systems.keys()):
            nlm_integration.unregister_system(system_name)
```

---

# Advanced Example 8: Machine Learning Model Training

Framework for training external machine learning models using NLM data.

```python
import pynlm
import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, Any, List, Tuple
import threading
import time
import json

class NLMDataCollector:
    """Collect and prepare NLM data for machine learning training."""
    
    def __init__(self, simulation_steps: int = 1000):
        self.simulation_steps = simulation_steps
        self.collected_data = []
        self.feature_columns = [
            'total_neurons', 'total_synapses', 'firing_neuron_count',
            'total_spikes', 'avg_firing_rate', 'e_i_ratio', 'development_stage',
            'learned_preference', 'curiosity_level', 'novelty_level',
            'prediction_error', 'neuromodulation_level', 'agent_energy',
            'agent_health', 'agent_age', 'simulation_time'
        ]
        self.target_columns = ['total_reward', 'learning_progress']
    
    def run_simulation_and_collect(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                                  agent: pynlm.AgentBrain) -> List[Dict[str, Any]]:
        """Run simulation and collect data for ML training."""
        
        simulation_data = []
        
        for step in range(self.simulation_steps):
            # Update world
            world.update(0.1)
            
            # Get current state for data collection
            current_state = self._get_current_state(brain, agent, world)
            
            # Store state before action
            simulation_data.append({
                'step': step,
                'timestamp': time.time(),
                'state': current_state,
                'action_taken': None,
                'reward_received': 0.0,
                'learning_applied': False
            })
            
            # Process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update state with action and reward
            if simulation_data:
                simulation_data[-1].update({
                    'action_taken': action.getType() if hasattr(action, 'getType') else str(action),
                    'reward_received': reward,
                    'learning_applied': True
                })
        
        return simulation_data
    
    def _get_current_state(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, 
                          world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """Get current NLM state as dictionary."""
        
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'learned_preference': brain.getLearnedPreference(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health,
            'agent_age': world.getAgentBody().age,
            'simulation_time': world.getSimulationTime()
        }
    
    def calculate_targets(self, data: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Calculate ML training targets from simulation data."""
        
        # Calculate reward accumulation
        reward_accumulation = []
        cumulative_reward = 0.0
        
        for entry in data:
            cumulative_reward += entry.get('reward_received', 0.0)
            reward_accumulation.append(cumulative_reward)
        
        # Calculate learning progress (change in learned preference)
        learning_progress = []
        prev_preference = None
        
        for entry in data:
            current_preference = entry['state']['learned_preference']
            if prev_preference is not None:
                progress = abs(current_preference - prev_preference)
                learning_progress.append(progress)
            else:
                learning_progress.append(0.0)
            prev_preference = current_preference
        
        # Add targets to data
        for i, entry in enumerate(data):
            entry['total_reward'] = reward_accumulation[i]
            entry['learning_progress'] = learning_progress[i]
        
        return data

class NMLModelTrainer:
    """Train machine learning models using NLM data."""
    
    def __init__(self):
        self.models = {}
        self.scalers = {}
        self.feature_importance = {}
        self.training_history = {}
    
    def create_feature_matrix(self, data: List[Dict[str, Any]], 
                             feature_columns: List[str]) -> Tuple[np.ndarray, np.ndarray]:
        """Create feature matrix and target vector from data."""
        
        # Convert to DataFrame
        df = pd.DataFrame(data)
        
        # Select features and targets
        X = df[feature_columns].values
        y_reward = df['total_reward'].values
        y_progress = df['learning_progress'].values
        
        return X, y_reward, y_progress
    
    def train_regression_models(self, X_train: np.ndarray, y_reward_train: np.ndarray, 
                               y_progress_train: np.ndarray) -> Dict[str, Any]:
        """Train regression models for NLM data prediction."""
        
        models = {}
        training_history = {}
        
        # Create and train Random Forest Regressor for reward prediction
        reward_model = Pipeline([
            ('scaler', StandardScaler()),
            ('regressor', RandomForestRegressor(n_estimators=100, random_state=42))
        ])
        
        reward_model.fit(X_train, y_reward_train)
        
        # Create and train Random Forest Regressor for learning progress prediction
        progress_model = Pipeline([
            ('scaler', StandardScaler()),
            ('regressor', RandomForestRegressor(n_estimators=100, random_state=42))
        ])
        
        progress_model.fit(X_train, y_progress_train)
        
        models['reward_prediction'] = reward_model
        models['learning_progress_prediction'] = progress_model
        
        # Store feature importance
        feature_importance = {
            'reward_prediction': reward_model.named_steps['regressor'].feature_importances_,
            'learning_progress_prediction': progress_model.named_steps['regressor'].feature_importances_
        }
        
        # Store training history
        training_history['reward_model_score'] = reward_model.score(X_train, y_reward_train)
        training_history['progress_model_score'] = progress_model.score(X_train, y_progress_train)
        
        self.models = models
        self.feature_importance = feature_importance
        self.training_history = training_history
        
        return {
            'models': models,
            'feature_importance': feature_importance,
            'training_history': training_history
        }
    
    def predict_brain_behavior(self, X_test: np.ndarray) -> Dict[str, Any]:
        """Predict brain behavior based on NLM state."""
        
        if not self.models:
            raise RuntimeError("No models trained. Call train_regression_models first.")
        
        predictions = {}
        
        for model_name, model in self.models.items():
            predictions[model_name] = model.predict(X_test)
        
        return {
            'predictions': predictions,
            'feature_importance': self.feature_importance
        }
    
    def evaluate_model_performance(self, X_test: np.ndarray, y_reward_test: np.ndarray, 
                                  y_progress_test: np.ndarray) -> Dict[str, Any]:
        """Evaluate model performance on test data."""
        
        if not self.models:
            raise RuntimeError("No models trained. Call train_regression_models first.")
        
        # Get predictions
        predictions = self.predict_brain_behavior(X_test)
        
        # Calculate performance metrics
        reward_predictions = predictions['predictions']['reward_prediction']
        progress_predictions = predictions['predictions']['learning_progress_prediction']
        
        from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
        
        reward_metrics = {
            'mse': mean_squared_error(y_reward_test, reward_predictions),
            'mae': mean_absolute_error(y_reward_test, reward_predictions),
            'r2': r2_score(y_reward_test, reward_predictions)
        }
        
        progress_metrics = {
            'mse': mean_squared_error(y_progress_test, progress_predictions),
            'mae': mean_absolute_error(y_progress_test, progress_predictions),
            'r2': r2_score(y_progress_test, progress_predictions)
        }
        
        return {
            'reward_prediction_metrics': reward_metrics,
            'learning_progress_metrics': progress_metrics,
            'overall_performance': {
                'avg_mse': (reward_metrics['mse'] + progress_metrics['mse']) / 2,
                'avg_mae': (reward_metrics['mae'] + progress_metrics['mae']) / 2,
                'avg_r2': (reward_metrics['r2'] + progress_metrics['r2']) / 2
            }
        }

class NLMModelDeployment:
    """Deploy trained NLM models for real-time prediction."""
    
    def __init__(self):
        self.trained_models = {}
        self.deployment_active = False
        self.deployment_thread = None
        self.prediction_history = []
    
    def deploy_model(self, model_name: str, model_data: Dict[str, Any]):
        """Deploy a trained NLM model for real-time use."""
        
        self.trained_models[model_name] = model_data
        print(f"Model '{model_name}' deployed successfully")
    
    def start_real_time_prediction(self, brain: pynlm.Brain, agent: pynlm.AgentBrain,
                                  world: pynlm.SimpleWorld, interval: float = 5.0):
        """Start real-time model prediction based on NLM state."""
        
        if not self.trained_models:
            raise RuntimeError("No models deployed. Call deploy_model first.")
        
        self.deployment_active = True
        
        def prediction_loop():
            while self.deployment_active:
                try:
                    # Get current NLM state
                    current_state = self._get_brain_state(brain, agent, world)
                    
                    # Convert to feature vector
                    feature_vector = self._prepare_features(current_state)
                    
                    # Get predictions from all deployed models
                    predictions = self._get_model_predictions(feature_vector)
                    
                    # Store predictions
                    self.prediction_history.append({
                        'timestamp': time.time(),
                        'state': current_state,
                        'predictions': predictions
                    })
                    
                    # Log predictions
                    self._log_predictions(predictions)
                    
                    # Sleep for prediction interval
                    time.sleep(interval)
                    
                except Exception as e:
                    print(f"Prediction error: {e}")
                    break
        
        # Start prediction thread
        self.deployment_thread = threading.Thread(target=prediction_loop, daemon=True)
        self.deployment_thread.start()
        
        print("Real-time model deployment started")
    
    def _get_brain_state(self, brain: pynlm.Brain, agent: pynlm.AgentBrain,
                        world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """Get current brain state for prediction."""
        
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'learned_preference': brain.getLearnedPreference(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health,
            'agent_age': world.getAgentBody().age,
            'simulation_time': world.getSimulationTime()
        }
    
    def _prepare_features(self, state: Dict[str, Any]) -> np.ndarray:
        """Prepare state features for model prediction."""
        
        # Extract features in the correct order
        feature_names = [
            'total_neurons', 'total_synapses', 'firing_neuron_count',
            'total_spikes', 'avg_firing_rate', 'e_i_ratio', 'development_stage',
            'learned_preference', 'curiosity_level', 'novelty_level',
            'prediction_error', 'neuromodulation_level', 'agent_energy',
            'agent_health', 'agent_age', 'simulation_time'
        ]
        
        features = [state[name] for name in feature_names]
        return np.array(features).reshape(1, -1)
    
    def _get_model_predictions(self, feature_vector: np.ndarray) -> Dict[str, Any]:
        """Get predictions from all deployed models."""
        
        predictions = {}
        
        for model_name, model_data in self.trained_models.items():
            try:
                if 'model' in model_data:
                    model = model_data['model']
                    prediction = model.predict(feature_vector)[0]
                    predictions[model_name] = {
                        'predicted_value': prediction,
                        'confidence': self._calculate_prediction_confidence(model, feature_vector),
                        'timestamp': time.time()
                    }
            except Exception as e:
                print(f"Error predicting with model {model_name}: {e}")
        
        return predictions
    
    def _calculate_prediction_confidence(self, model, feature_vector: np.ndarray) -> float:
        """Calculate confidence score for model prediction."""
        
        # Use feature importance to calculate confidence
        if hasattr(model, 'named_steps'):
            if 'regressor' in model.named_steps:
                regressor = model.named_steps['regressor']
                if hasattr(regressor, 'feature_importances_'):
                    importance = regressor.feature_importances_
                    # Higher feature importance variance indicates higher confidence
                    confidence = np.std(importance) / (np.mean(importance) + 1e-8)
                    return min(confidence, 1.0)  # Cap at 1.0
        
        return 0.5  # Default confidence
    
    def _log_predictions(self, predictions: Dict[str, Any]):
        """Log predictions for monitoring and analysis."""
        
        timestamp = time.time()
        log_entry = {
            'timestamp': timestamp,
            'predictions': predictions
        }
        
        self.prediction_history.append(log_entry)
        
        # Print summary for high-confidence predictions
        for model_name, pred_info in predictions.items():
            confidence = pred_info.get('confidence', 0)
            predicted_value = pred_info.get('predicted_value', 0)
            
            if confidence > 0.7:
                print(f"High-confidence prediction from {model_name}: {predicted_value:.3f}")
    
    def stop_real_time_prediction(self) -> Dict[str, Any]:
        """Stop real-time model prediction and return statistics."""
        
        if not self.deployment_active:
            print("Deployment not active")
            return self.get_deployment_statistics()
        
        self.deployment_active = False
        
        if self.deployment_thread:
            self.deployment_thread.join(timeout=5.0)
            self.deployment_thread = None
        
        print("Real-time model deployment stopped")
        return self.get_deployment_statistics()
    
    def get_deployment_statistics(self) -> Dict[str, Any]:
        """Get deployment statistics and performance metrics."""
        
        if not self.prediction_history:
            return {"error": "No predictions recorded"}
        
        all_predictions = []
        for entry in self.prediction_history:
            all_predictions.extend(entry['predictions'].values())
        
        if not all_predictions:
            return {"error": "No prediction data available"}
        
        prediction_values = [p['predicted_value'] for p in all_predictions]
        confidences = [p.get('confidence', 0) for p in all_predictions]
        
        return {
            "total_predictions": len(self.prediction_history),
            "deployment_duration": time.time() - self.prediction_history[0]['timestamp'] if self.prediction_history else 0,
            "average_prediction": np.mean(prediction_values) if prediction_values else 0,
            "prediction_std": np.std(prediction_values) if prediction_values else 0,
            "average_confidence": np.mean(confidences) if confidences else 0,
            "high_confidence_predictions": sum(1 for c in confidences if c > 0.7),
            "deployed_models": list(self.trained_models.keys())
        }

# Usage example
if __name__ == "__main__":
    print("=== NLM Machine Learning Model Training Demo ===\n")
    
    # Initialize components
    data_collector = NLMDataCollector(simulation_steps=500)
    model_trainer = NMLModelTrainer()
    model_deployment = NLMModelDeployment()
    
    try:
        # Create learning environment
        print("1. Creating learning environment...")
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=20,
            visionRange=8,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        
        # Run simulation and collect data
        print("2. Running simulation and collecting data...")
        raw_data = data_collector.run_simulation_and_collect(brain, world, agent)
        
        # Calculate training targets
        print("3. Calculating training targets...")
        training_data = data_collector.calculate_targets(raw_data)
        
        # Prepare feature matrix
        print("4. Preparing feature matrix...")
        X, y_reward, y_progress = model_trainer.create_feature_matrix(
            training_data, data_collector.feature_columns
        )
        
        # Split data into train and test sets
        X_train, X_test, y_reward_train, y_reward_test, y_progress_train, y_progress_test = train_test_split(
            X, y_reward, y_progress, test_size=0.2, random_state=42
        )
        
        # Train models
        print("5. Training machine learning models...")
        training_results = model_trainer.train_regression_models(X_train, y_reward_train, y_progress_train)
        
        print(f"   Reward prediction R² score: {training_results['training_history']['reward_model_score']:.3f}")
        print(f"   Learning progress R² score: {training_results['training_history']['progress_model_score']:.3f}")
        
        # Evaluate models
        print("6. Evaluating model performance...")
        evaluation_results = model_trainer.evaluate_model_performance(
            X_test, y_reward_test, y_progress_test
        )
        
        print(f"   Average R² score: {evaluation_results['overall_performance']['avg_r2']:.3f}")
        print(f"   Average MSE: {evaluation_results['overall_performance']['avg_mse']:.3f}")
        
        # Deploy model for real-time prediction
        print("7. Deploying model for real-time prediction...")
        
        # Create deployment model
        deployment_model = {
            'model': model_trainer.models['reward_prediction'],
            'feature_columns': data_collector.feature_columns,
            'model_type': 'RandomForestRegressor',
            'training_date': time.time(),
            'performance_metrics': evaluation_results['reward_prediction_metrics']
        }
        
        model_deployment.deploy_model('reward_prediction_model', deployment_model)
        
        # Start real-time prediction
        print("8. Starting real-time prediction...")
        
        # Continue simulation for real-time testing
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Get predictions from deployed model
        current_state = model_deployment._get_brain_state(brain, agent, world)
        feature_vector = model_deployment._prepare_features(current_state)
        predictions = model_deployment._get_model_predictions(feature_vector)
        
        print(f"   Real-time predictions: {predictions}")
        
        # Stop real-time prediction
        print("9. Stopping real-time prediction...")
        deployment_stats = model_deployment.stop_real_time_prediction()
        
        print(f"   Total predictions made: {deployment_stats['total_predictions']}")
        print(f"   Average prediction confidence: {deployment_stats['average_confidence']:.3f}")
        
        print("\n=== Machine Learning Model Training Complete ===")
        
    except Exception as e:
        print(f"\nError during ML training: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
```

---

# Summary of Advanced Python Features

This document demonstrates advanced Python programming capabilities with NLM:

## 1. Learning Environment Management
- `LearningEnvironmentManager`: Comprehensive experiment management
- Support for multiple experiment configurations
- Progress saving and loading
- Detailed result analysis

## 2. Parallel Experiment Execution
- `ParallelExperimentRunner`: Multi-threaded experiment execution
- Configurable worker pools
- Performance comparison across configurations
- Summary statistics generation

## 3. Advanced Configuration Management
- `AdvancedConfigurationManager`: Preset-based configuration management
- Configuration validation and export/import
- Flexible configuration inheritance
- Preset compatibility checking

## 4. Robust Error Handling
- `ErrorHandler`: Automatic recovery mechanisms
- Comprehensive error tracking
- Configurable retry policies
- Recovery handler system

## 5. Real-time Monitoring
- `RealTimeMonitor`: Live monitoring and visualization
- Thread-safe data collection
- Comprehensive statistics tracking
- Automatic plotting and report generation

## 6. External System Integration
- `NLMExternalIntegration`: Integration with external APIs and databases
- Abstract interface for external systems
- Automatic data collection and synchronization
- Error handling for external communications

## 7. Machine Learning Model Training
- `NLMDataCollector`: Automated data collection for ML training
- `NMLModelTrainer`: Comprehensive ML model training and evaluation
- `NLMModelDeployment`: Real-time model deployment and prediction
- Feature engineering and preprocessing pipelines

## Key Benefits

1. **Scalability**: Run multiple experiments in parallel for efficient resource utilization
2. **Robustness**: Comprehensive error handling with automatic recovery mechanisms
3. **Flexibility**: Extensive configuration options and preset management
4. **Monitoring**: Real-time monitoring and visualization of simulation performance
5. **Integration**: Seamless integration with external systems and APIs
6. **Machine Learning**: Automated ML model training for behavior prediction

These advanced Python features transform NLM from a simple simulation tool into a comprehensive neural learning platform suitable for research, production, and machine learning applications.

---

# Next Steps for Advanced Users

1. **Start with `LearningEnvironmentManager`** for complex experiment management
2. **Use `ParallelExperimentRunner`** to speed up experimentation with multiple configurations
3. **Implement external system integration** for real-world applications
4. **Deploy ML models** for behavior prediction and optimization
5. **Utilize real-time monitoring** for continuous performance assessment

The enhanced NLM Python API now provides everything needed for production-grade neural learning systems!
