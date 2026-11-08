#!/usr/bin/env python3
"""
Advanced NLM Python Examples

This file contains advanced examples and performance optimization techniques
for the NLM Python API, targeting experienced users.
"""

import pynlm
import time
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import numpy as np

class AdvancedNLMExamples:
    """Advanced examples for NLM Python API usage."""
    
    @staticmethod
    def parallel_simulation_example():
        """
        Example 1: Parallel simulation execution
        
        Demonstrates running multiple simulations in parallel using threading.
        Useful for hyperparameter testing and ensemble methods.
        """
        print("=== Parallel Simulation Example ===")
        
        def run_single_simulation(config_settings, simulation_id):
            """Run a single simulation with given configuration."""
            config = pynlm.createDefaultConfig()
            for key, value in config_settings.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Add some initial activity
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(1000):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
            
            return {
                "simulation_id": simulation_id,
                "config": config_settings,
                "final_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity_level": agent.getCuriosityLevel()
            }
        
        # Define different configurations to test
        configs = [
            {"brain.neuron_count": 500, "plasticity.stdp.enable": True},
            {"brain.neuron_count": 1000, "plasticity.stdp.enable": False},
            {"brain.neuron_count": 2000, "plasticity.stdp.enable": True, "plasticity.hebbian.enable": True},
            {"brain.neuron_count": 3000, "plasticity.stdp.enable": True, "plasticity.hebbian.enable": True, "plasticity.structural.enable": True}
        ]
        
        # Run simulations in parallel
        results = []
        with ThreadPoolExecutor(max_workers=4) as executor:
            futures = []
            for i, config in enumerate(configs):
                future = executor.submit(run_single_simulation, config, i)
                futures.append(future)
            
            for future in futures:
                results.append(future.result())
        
        # Print results summary
        print(f"Completed {len(results)} simulations in parallel")
        for result in results:
            print(f"  Simulation {result['simulation_id']}: {result['final_spikes']} spikes, "
                  f"{result['firing_rate']:.2f} Hz average firing rate")
        
        return results
    
    @staticmethod
    def hyperparameter_tuning_example():
        """
        Example 2: Hyperparameter tuning
        
        Demonstrates systematic hyperparameter testing with optimization.
        """
        print("\n=== Hyperparameter Tuning Example ===")
        
        def evaluate_config(hyperparams, simulation_id):
            """Evaluate a single hyperparameter set."""
            config = pynlm.createDefaultConfig()
            
            # Apply hyperparameters
            config.set("brain.neuron_count", hyperparams["neuron_count"])
            config.set("brain.synapse_density", hyperparams["synapse_density"])
            config.set("plasticity.stdp.learning_rate", hyperparams["stdp_lr"])
            config.set("plasticity.hebbian.learning_rate", hyperparams["hebbian_lr"])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Simple performance metric
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation and collect metrics
            total_spikes = 0
            for step in range(500):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
            
            total_spikes = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            # Calculate performance score
            # Higher is better: combine spikes and firing rate
            performance_score = (total_spikes * 0.7) + (firing_rate * 0.3)
            
            return {
                "simulation_id": simulation_id,
                "hyperparameters": hyperparams,
                "total_spikes": total_spikes,
                "firing_rate": firing_rate,
                "performance_score": performance_score
            }
        
        # Define hyperparameter search space
        hyperparam_sets = []
        
        # Neuron count variations
        for neuron_count in [500, 1000, 1500, 2000]:
            # Synapse density variations
            for synapse_density in [0.05, 0.1, 0.15]:
                # STDP learning rates
                for stdp_lr in [0.001, 0.01, 0.1]:
                    # Hebbian learning rates
                    for hebbian_lr in [0.001, 0.01, 0.1]:
                        hyperparam_sets.append({
                            "neuron_count": neuron_count,
                            "synapse_density": synapse_density,
                            "stdp_lr": stdp_lr,
                            "hebbian_lr": hebbian_lr
                        })
        
        # Evaluate hyperparameters (using parallel processing for speed)
        results = []
        with ThreadPoolExecutor(max_workers=8) as executor:
            futures = []
            for i, hyperparams in enumerate(hyperparam_sets):
                future = executor.submit(evaluate_config, hyperparams, i)
                futures.append(future)
            
            for future in futures:
                results.append(future.result())
        
        # Find best hyperparameters
        best_result = max(results, key=lambda x: x["performance_score"])
        
        print(f"Evaluated {len(results)} hyperparameter sets")
        print(f"Best configuration found:")
        print(f"  Neuron count: {best_result['hyperparameters']['neuron_count']}")
        print(f"  Synapse density: {best_result['hyperparameters']['synapse_density']}")
        print(f"  STDP learning rate: {best_result['hyperparameters']['stdp_lr']}")
        print(f"  Hebbian learning rate: {best_result['hyperparameters']['hebbian_lr']}")
        print(f"  Performance score: {best_result['performance_score']:.2f}")
        print(f"  Total spikes: {best_result['total_spikes']}")
        print(f"  Firing rate: {best_result['firing_rate']:.2f} Hz")
        
        return results, best_result
    
    @staticmethod
    def batch_processing_example():
        """
        Example 3: Batch processing and memory management
        
        Demonstrates efficient batch processing with memory management.
        """
        print("\n=== Batch Processing Example ===")
        
        class BatchProcessor:
            def __init__(self, batch_size=10, max_memory_gb=2.0):
                self.batch_size = batch_size
                self.max_memory_gb = max_memory_gb
                self.processed_count = 0
            
            def process_simulation_batch(self, config_batch):
                """Process a batch of simulations."""
                results = []
                
                for i, config_settings in enumerate(config_batch):
                    try:
                        print(f"Processing batch item {i+1}/{len(config_batch)}")
                        
                        config = pynlm.createDefaultConfig()
                        for key, value in config_settings.items():
                            config.set(key, value)
                        
                        brain = pynlm.createBrain(config)
                        brain.initialize()
                        
                        world = pynlm.createSimpleWorld()
                        world.configure(10, 10, 8, 8)
                        world.reset()
                        
                        agent = pynlm.createAgentBrain(brain)
                        agent.initialize(world)
                        
                        agent.enableRewardModulation(True)
                        
                        # Run simulation
                        for step in range(200):
                            world.update(0.1)
                            agent.processSensoryInput(world.getSensoryPercept())
                            brain.step(step)
                        
                        result = {
                            "config": config_settings,
                            "spikes": brain.getTotalSpikeCount(),
                            "firing_rate": brain.getAverageFiringRate(),
                            "success": True
                        }
                        
                        results.append(result)
                        
                        # Explicit cleanup to free memory
                        del brain
                        del world
                        del agent
                        
                    except Exception as e:
                        print(f"Error processing config {config_settings}: {e}")
                        result = {
                            "config": config_settings,
                            "error": str(e),
                            "success": False
                        }
                        results.append(result)
                
                self.processed_count += len(config_batch)
                return results
        
        # Create batch of different configurations
        config_batch = []
        base_config = {"brain.synapse_density": 0.1, "plasticity.stdp.enable": True}
        
        for i in range(20):  # 20 different configurations
            config_variant = base_config.copy()
            config_variant["brain.neuron_count"] = 500 + (i * 100)
            config_variant["plasticity.hebbian.enable"] = (i % 2 == 0)
            config_batch.append(config_variant)
        
        # Process batch
        processor = BatchProcessor(batch_size=20)
        batch_results = processor.process_simulation_batch(config_batch)
        
        # Analyze results
        successful = [r for r in batch_results if r["success"]]
        failed = [r for r in batch_results if not r["success"]]
        
        print(f"\nBatch processing results:")
        print(f"  Total processed: {processor.processed_count}")
        print(f"  Successful: {len(successful)}")
        print(f"  Failed: {len(failed)}")
        
        if successful:
            avg_spikes = np.mean([r["spikes"] for r in successful])
            avg_firing_rate = np.mean([r["firing_rate"] for r in successful])
            print(f"  Average spikes: {avg_spikes:.1f}")
            print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
        
        return batch_results
    
    @staticmethod
    def memory_efficiency_example():
        """
        Example 4: Memory-efficient simulation
        
        Demonstrates techniques for reducing memory usage.
        """
        print("\n=== Memory Efficiency Example ===")
        
        class MemoryEfficientSimulator:
            def __init__(self):
                self.checkpoint_interval = 50
                self.memory_usage_samples = []
            
            def simulate_with_memory_monitoring(self, config, max_steps=500):
                """Run simulation with memory monitoring."""
                import psutil
                import os
                
                config_obj = pynlm.createDefaultConfig()
                for key, value in config.items():
                    config_obj.set(key, value)
                
                brain = pynlm.createBrain(config_obj)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(8, 8, 6, 6)
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                agent.enableRewardModulation(True)
                
                # Monitor memory usage
                process = psutil.Process(os.getpid())
                
                for step in range(max_steps):
                    # Record memory usage
                    if step % 10 == 0:
                        memory_mb = process.memory_info().rss / 1024 / 1024
                        self.memory_usage_samples.append(memory_mb)
                    
                    # Update world
                    world.update(0.1)
                    
                    # Process sensory input
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    
                    # Brain step
                    brain.step(step)
                    
                    # Get action
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Checkpoint memory
                    if step % self.checkpoint_interval == 0 and step > 0:
                        print(f"Step {step}: Memory usage: {self.memory_usage_samples[-1]:.1f} MB")
                
                return {
                    "final_spikes": brain.getTotalSpikeCount(),
                    "final_firing_rate": brain.getAverageFiringRate(),
                    "memory_samples": self.memory_usage_samples,
                    "peak_memory_mb": max(self.memory_usage_samples) if self.memory_usage_samples else 0
                }
        
        # Test with different brain sizes to see memory impact
        configs = [
            {"brain.neuron_count": 500},
            {"brain.neuron_count": 1000},
            {"brain.neuron_count": 1500},
            {"brain.neuron_count": 2000},
        ]
        
        memory_efficiency_sim = MemoryEfficientSimulator()
        memory_results = []
        
        for i, config in enumerate(configs):
            print(f"\nTesting configuration {i+1}: {config['brain.neuron_count']} neurons")
            result = memory_efficiency_sim.simulate_with_memory_monitoring(config, max_steps=300)
            memory_results.append(result)
            
            print(f"  Final spikes: {result['final_spikes']}")
            print(f"  Peak memory: {result['peak_memory_mb']:.1f} MB")
        
        # Analyze memory efficiency
        print("\n=== Memory Efficiency Analysis ===")
        for i, result in enumerate(memory_results):
            neurons = configs[i]["brain.neuron_count"]
            memory_per_neuron = result["peak_memory_mb"] / neurons
            spikes_per_memory = result["final_spikes"] / result["peak_memory_mb"] if result["peak_memory_mb"] > 0 else 0
            
            print(f"Config {i+1} ({neurons} neurons):")
            print(f"  Memory per neuron: {memory_per_neuron:.6f} MB")
            print(f"  Spikes per MB memory: {spikes_per_memory:.1f}")
        
        return memory_results
    
    @staticmethod
    def real_time_processing_example():
        """
        Example 5: Real-time processing and streaming
        
        Demonstrates real-time data processing capabilities.
        """
        print("\n=== Real-Time Processing Example ===")
        
        class RealTimeSimulator:
            def __init__(self, target_fps=10):
                self.target_fps = target_fps
                self.step_time = 1.0 / target_fps
                self.current_time = 0.0
                self.simulation_active = False
            
            def run_real_time_simulation(self, config):
                """Run simulation in real-time mode."""
                config_obj = pynlm.createDefaultConfig()
                for key, value in config.items():
                    config_obj.set(key, value)
                
                brain = pynlm.createBrain(config_obj)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(12, 12, 10, 10)
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                
                self.simulation_active = True
                step_count = 0
                
                print("Starting real-time simulation (press Ctrl+C to stop)...")
                
                try:
                    while self.simulation_active:
                        start_time = time.time()
                        
                        # Update world
                        world.update(self.step_time)
                        
                        # Get and process percept
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        
                        # Brain step with timing
                        brain.step(step_count, self.current_time)
                        
                        # Get action
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, self.current_time)
                        
                        # Log statistics
                        if step_count % 10 == 0:
                            print(f"Step {step_count}: Time={self.current_time:.2f}s, "
                                  f"Firing={brain.getFiringNeuronCount()}, "
                                  f"Spikes={brain.getTotalSpikeCount()}")
                        
                        step_count += 1
                        self.current_time += self.step_time
                        
                        # Control frame rate
                        elapsed = time.time() - start_time
                        if elapsed < self.step_time:
                            time.sleep(self.step_time - elapsed)
                        
                except KeyboardInterrupt:
                    print("\nReal-time simulation stopped by user")
                
                self.simulation_active = False
                
                return {
                    "total_steps": step_count,
                    "final_time": self.current_time,
                    "final_spikes": brain.getTotalSpikeCount(),
                    "final_firing_rate": brain.getAverageFiringRate()
                }
        
        # Create real-time simulator
        real_time_sim = RealTimeSimulator(target_fps=10)
        
        # Define simulation configuration
        config = {
            "brain.neuron_count": 800,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True
        }
        
        print("Real-time simulation configured with:")
        for key, value in config.items():
            print(f"  {key}: {value}")
        
        # Run in background thread (commented out to avoid blocking)
        # result = real_time_sim.run_real_time_simulation(config)
        # print(f"\nReal-time simulation completed: {result}")
        
        return config
    
    @staticmethod
    def continuous_learning_example():
        """
        Example 6: Continuous learning and adaptation
        
        Demonstrates continuous learning with changing environments.
        """
        print("\n=== Continuous Learning Example ===")
        
        class ContinuousLearningSystem:
            def __init__(self):
                self.agent_history = []
                self.environment_history = []
                self.adaptation_enabled = True
            
            def run_continuous_learning(self, num_phases=5):
                """Run continuous learning across multiple phases."""
                print("Starting continuous learning system...")
                
                for phase in range(num_phases):
                    print(f"\n--- Phase {phase + 1}/{num_phases} ---")
                    
                    # Create new configuration for this phase
                    config = pynlm.createDefaultConfig()
                    config.set("brain.neuron_count", 1000)
                    config.set("brain.synapse_density", 0.1 + (phase * 0.02))
                    
                    # Create brain and agent
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    
                    world = pynlm.createSimpleWorld()
                    world.configure(10, 10, 8, 8)
                    
                    agent = pynlm.createAgentBrain(brain)
                    agent.initialize(world)
                    
                    # Enable different subsystems based on phase
                    if phase == 0:
                        # Phase 1: Basic learning
                        agent.enableRewardModulation(True)
                        agent.enableCuriosity(False)
                    elif phase == 1:
                        # Phase 2: Add curiosity
                        agent.enableRewardModulation(True)
                        agent.enableCuriosity(True)
                        agent.enableDevelopment(True)
                    else:
                        # Phase 3+: Full learning
                        agent.enableRewardModulation(True)
                        agent.enableCuriosity(True)
                        agent.enableDevelopment(True)
                        agent.enableStructuralPlasticity(True)
                    
                    # Run phase simulation
                    phase_spikes = 0
                    for step in range(200):
                        world.update(0.1)
                        agent.processSensoryInput(world.getSensoryPercept())
                        brain.step(step)
                        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                    
                    phase_spikes = brain.getTotalSpikeCount()
                    phase_firing_rate = brain.getAverageFiringRate()
                    phase_curiosity = agent.getCuriosityLevel()
                    phase_novelty = agent.getNoveltyLevel()
                    
                    # Store phase results
                    phase_result = {
                        "phase": phase + 1,
                        "spikes": phase_spikes,
                        "firing_rate": phase_firing_rate,
                        "curiosity": phase_curiosity,
                        "novelty": phase_novelty,
                        "development_stage": brain.getDevelopmentalStage().name
                    }
                    
                    self.agent_history.append(agent)
                    self.environment_history.append(world)
                    
                    print(f"  Total spikes: {phase_spikes}")
                    print(f"  Firing rate: {phase_firing_rate:.2f} Hz")
                    print(f"  Curiosity level: {phase_curiosity:.3f}")
                    print(f"  Development stage: {phase_result['development_stage']}")
                    
                    # Brief pause between phases
                    print(f"  Waiting for next phase...")
                    time.sleep(1)
                
                # Analyze adaptation
                print(f"\n=== Continuous Learning Analysis ===")
                spikes_by_phase = [r["spikes"] for r in self.agent_history]
                firing_rates_by_phase = [r["firing_rate"] for r in self.agent_history]
                curiosity_levels_by_phase = [r["curiosity"] for r in self.agent_history]
                
                print(f"Spikes by phase: {spikes_by_phase}")
                print(f"Firing rates by phase: {[f'{fr:.2f}' for fr in firing_rates_by_phase]}")
                print(f"Curiosity levels by phase: {[f'{c:.3f}' for c in curiosity_levels_by_phase]}")
                
                # Calculate adaptation metrics
                spike_improvement = (spikes_by_phase[-1] - spikes_by_phase[0]) / spikes_by_phase[0] * 100
                firing_rate_change = firing_rates_by_phase[-1] - firing_rates_by_phase[0]
                curiosity_growth = curiosity_levels_by_phase[-1] - curiosity_levels_by_phase[0]
                
                print(f"\nAdaptation Metrics:")
                print(f"  Spike count change: {spike_improvement:+.1f}%")
                print(f"  Firing rate change: {firing_rate_change:+.2f} Hz")
                print(f"  Curiosity growth: {curiosity_growth:+.3f}")
                
                return self.agent_history, self.environment_history
        
        # Run continuous learning
        learning_system = ContinuousLearningSystem()
        history = learning_system.run_continuous_learning()
        
        return history
    
    @classmethod
    def run_all_examples(cls):
        """Run all advanced examples."""
        print("=" * 80)
        print("ADVANCED NLM PYTHON EXAMPLES")
        print("=" * 80)
        
        examples = [
            ("Parallel Simulation", cls.parallel_simulation_example),
            ("Hyperparameter Tuning", cls.hyperparameter_tuning_example),
            ("Batch Processing", cls.batch_processing_example),
            ("Memory Efficiency", cls.memory_efficiency_example),
            ("Real-Time Processing", cls.real_time_processing_example),
            ("Continuous Learning", cls.continuous_learning_example)
        ]
        
        results = {}
        
        for name, example_func in examples:
            try:
                print(f"\n{'='*80}")
                print(f"Running: {name}")
                print(f"{'='*80}")
                
                result = example_func()
                results[name] = {"success": True, "result": result}
                print(f"✓ {name} completed successfully")
                
            except Exception as e:
                print(f"✗ {name} failed: {e}")
                results[name] = {"success": False, "error": str(e)}
        
        # Print summary
        print(f"\n{'='*80}")
        print("ADVANCED EXAMPLES SUMMARY")
        print(f"{'='*80}")
        
        for name, result in results.items():
            status = "✓ PASS" if result["success"] else "✗ FAIL"
            print(f"{status}: {name}")
        
        return results

if __name__ == "__main__":
    # Run all advanced examples
    AdvancedNLMExamples.run_all_examples()
    
    print(f"\n{'='*80}")
    print("ALL ADVANCED EXAMPLES COMPLETED")
    print(f"{'='*80}")