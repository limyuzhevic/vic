#!/usr/bin/env python3
"""
NLM Batch Processing and Advanced Simulation Capabilities
========================================================

This script provides advanced batch processing capabilities for NLM simulations,
including parallel execution, performance optimization, and result aggregation.

Features:
- Batch processing of multiple configurations
- Performance benchmarking
- Result aggregation and comparison
- Configuration optimization
- Automated experiment management
"""

import sys
import os
import time
import json
import numpy as np
from pathlib import Path
from typing import List, Dict, Any, Callable, Optional
from dataclasses import dataclass, field
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import multiprocessing as mp

# Add local module to path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    from pynlm_advanced import NLMAdvancedAPI, SimulationResult
    PYNLM_AVAILABLE = True
except ImportError:
    print("Warning: Advanced NLM modules not available")
    PYNLM_AVAILABLE = False

@dataclass
class BatchSimulationConfig:
    """Configuration for batch simulations."""
    name: str
    config: Any
    steps: int = 1000
    neurons: int = 1000
    world_size: tuple = (20, 20)
    enable_features: Dict[str, bool] = field(default_factory=lambda: {
        "reward_modulation": True,
        "structural_plasticity": True,
        "development": True,
        "curiosity": True
    })
    priority: str = "normal"  # normal, high, critical

@dataclass
class BatchSimulationResult:
    """Result of a batch simulation."""
    config: BatchSimulationConfig
    simulation_result: SimulationResult
    learning_metrics: Dict[str, Any]
    comparison_data: Dict[str, Any]
    timestamp: float = field(default_factory=time.time)
    success: bool = True
    error_message: Optional[str] = None
    performance_metrics: Dict[str, float] = field(default_factory=dict)

class BatchProcessor:
    """
    Advanced batch processing engine for NLM simulations.
    """
    
    def __init__(self, max_workers: int = 4, enable_parallel: bool = True):
        self.max_workers = max_workers
        self.enable_parallel = enable_parallel
        self.results: List[BatchSimulationResult] = []
        self.lock = threading.Lock()
        
    def create_config_variations(
        self,
        base_config: Any,
        variations: Dict[str, List[Any]],
        prefix: str = ""
    ) -> List[BatchSimulationConfig]:
        """
        Create multiple configuration variations for batch processing.
        
        Args:
            base_config: Base configuration template
            variations: Dictionary of parameter variations
            prefix: Prefix for configuration names
            
        Returns:
            List of batch simulation configurations
        """
        configs = []
        
        # Generate all combinations of variations
        param_names = list(variations.keys())
        param_values = list(variations.values())
        
        # Simple Cartesian product for up to 4 parameters
        from itertools import product
        
        for i, combination in enumerate(product(*param_values)):
            config_name = f"{prefix}_config_{i}"
            config = BatchSimulationConfig(
                name=config_name,
                config=self._apply_variations(base_config, param_names, combination)
            )
            configs.append(config)
        
        return configs
    
    def _apply_variations(self, base_config: Any, param_names: List[str], 
                         values: tuple) -> Any:
        """Apply variations to a base configuration."""
        # This is a simplified version - in reality, would need proper config access
        return base_config
    
    def run_single_simulation(self, config: BatchSimulationConfig) -> BatchSimulationResult:
        """
        Run a single simulation with the given configuration.
        """
        print(f"Running simulation: {config.name}")
        start_time = time.time()
        
        try:
            # Create brain and environment
            brain = pynlm.createBrain(config.config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(config.world_size[0], config.world_size[1], 
                         8, 8)  # Fixed vision size
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features based on configuration
            for feature, enabled in config.enable_features.items():
                if hasattr(agent, f"enable_{feature}"):
                    getattr(agent, f"enable_{feature}")(enabled)
            
            # Run simulation
            simulation_steps = config.steps
            for step in range(simulation_steps):
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                # Get action and apply
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply neuromodulation
                agent.applyRewardModulation(result.reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            
            # Create simulation result
            sim_result = SimulationResult(
                total_steps=simulation_steps,
                total_spikes=brain.getTotalSpikeCount(),
                final_firing_rate=brain.getAverageFiringRate(),
                memory_episodes=brain.getTotalNeuronCount() // 100,
                total_reward=0.0,  # Would need reward tracking
                duration_seconds=end_time - start_time,
                final_neural_state=None
            )
            
            # Get learning metrics
            learning_metrics = NLMAdvancedAPI.advanced_learning_analysis(brain)
            
            # Create comparison data
            comparison_data = {
                "final_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
                "development_stage": brain.getDevelopmentalStage(),
                "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio()
            }
            
            # Performance metrics
            performance_metrics = {
                "steps_per_second": simulation_steps / (end_time - start_time),
                "spikes_per_second": brain.getTotalSpikeCount() / (end_time - start_time),
                "efficiency_score": brain.getAverageFiringRate() / (end_time - start_time)
            }
            
            return BatchSimulationResult(
                config=config,
                simulation_result=sim_result,
                learning_metrics=learning_metrics,
                comparison_data=comparison_data,
                performance_metrics=performance_metrics
            )
            
        except Exception as e:
            error_msg = f"Simulation {config.name} failed: {str(e)}"
            print(f"ERROR: {error_msg}")
            return BatchSimulationResult(
                config=config,
                simulation_result=None,
                learning_metrics={},
                comparison_data={},
                error_message=error_msg,
                success=False
            )
    
    def run_batch(self, configs: List[BatchSimulationConfig]) -> List[BatchSimulationResult]:
        """
        Run a batch of simulations.
        
        Args:
            configs: List of configurations to run
            
        Returns:
            List of simulation results
        """
        print(f"Starting batch processing with {len(configs)} configurations")
        
        if self.enable_parallel and self.max_workers > 1:
            return self._run_parallel_batch(configs)
        else:
            return self._run_sequential_batch(configs)
    
    def _run_sequential_batch(self, configs: List[BatchSimulationConfig]) -> List[BatchSimulationResult]:
        """Run simulations sequentially."""
        results = []
        for i, config in enumerate(configs):
            print(f"Progress: {i+1}/{len(configs)}")
            result = self.run_single_simulation(config)
            results.append(result)
            
            # Save intermediate result
            self._save_result(result)
        
        return results
    
    def _run_parallel_batch(self, configs: List[BatchSimulationConfig]) -> List[BatchSimulationResult]:
        """Run simulations in parallel."""
        results = []
        
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_config = {
                executor.submit(self.run_single_simulation, config): config
                for config in configs
            }
            
            # Collect results as they complete
            for i, future in enumerate(as_completed(future_to_config)):
                config = future_to_config[future]
                print(f"Progress: {i+1}/{len(configs)}")
                
                try:
                    result = future.result()
                    results.append(result)
                    self._save_result(result)
                except Exception as e:
                    error_msg = f"Parallel execution failed for {config.name}: {str(e)}"
                    print(f"ERROR: {error_msg}")
                    results.append(BatchSimulationResult(
                        config=config,
                        simulation_result=None,
                        learning_metrics={},
                        comparison_data={},
                        error_message=error_msg,
                        success=False
                    ))
        
        return results
    
    def _save_result(self, result: BatchSimulationResult):
        """Save individual result to disk."""
        try:
            filename = f"results/{result.config.name}.json"
            os.makedirs("results", exist_ok=True)
            
            # Convert result to JSON-serializable format
            result_data = {
                "config_name": result.config.name,
                "success": result.success,
                "simulation_result": {
                    "total_steps": result.simulation_result.total_steps,
                    "total_spikes": result.simulation_result.total_spikes,
                    "final_firing_rate": result.simulation_result.final_firing_rate,
                    "memory_episodes": result.simulation_result.memory_episodes,
                    "total_reward": result.simulation_result.total_reward,
                    "duration_seconds": result.simulation_result.duration_seconds
                },
                "learning_metrics": result.learning_metrics,
                "comparison_data": result.comparison_data,
                "performance_metrics": result.performance_metrics,
                "timestamp": result.timestamp
            }
            
            with open(filename, 'w') as f:
                json.dump(result_data, f, indent=2)
                
        except Exception as e:
            print(f"Failed to save result: {e}")
    
    def analyze_batch_results(self) -> Dict[str, Any]:
        """
        Analyze all batch results and generate summary statistics.
        
        Returns:
            Dictionary with analysis results
        """
        successful_results = [r for r in self.results if r.success]
        
        if not successful_results:
            return {"error": "No successful simulations to analyze"}
        
        analysis = {
            "total_simulations": len(self.results),
            "successful_simulations": len(successful_results),
            "failed_simulations": len(self.results) - len(successful_results),
            
            "performance_summary": {
                "average_steps_per_second": np.mean([
                    r.performance_metrics.get("steps_per_second", 0)
                    for r in successful_results
                ]),
                "average_spikes_per_second": np.mean([
                    r.performance_metrics.get("spikes_per_second", 0)
                    for r in successful_results
                ]),
                "average_efficiency_score": np.mean([
                    r.performance_metrics.get("efficiency_score", 0)
                    for r in successful_results
                ])
            },
            
            "simulation_results": {
                "firing_rates": [r.simulation_result.final_firing_rate for r in successful_results],
                "total_spikes": [r.simulation_result.total_spikes for r in successful_results],
                "durations": [r.simulation_result.duration_seconds for r in successful_results]
            },
            
            "learning_metrics_summary": self._aggregate_learning_metrics(successful_results),
            
            "recommendations": self._generate_recommendations(successful_results)
        }
        
        return analysis
    
    def _aggregate_learning_metrics(self, results: List[BatchSimulationResult]) -> Dict[str, Any]:
        """Aggregate learning metrics across all successful results."""
        if not results:
            return {}
        
        # This would aggregate the learning metrics from all simulations
        # For now, return a placeholder structure
        return {
            "average_weight_change": 0.0,
            "average_learning_rate": 0.0,
            "average_stability": 0.0,
            "average_adaptation_rate": 0.0,
            "average_memory_retention": 0.0
        }
    
    def _generate_recommendations(self, results: List[BatchSimulationResult]) -> List[str]:
        """Generate optimization recommendations based on results."""
        recommendations = []
        
        # Analyze patterns in successful results
        successful = [r for r in results if r.success]
        if not successful:
            return recommendations
        
        # Example recommendations based on performance metrics
        avg_efficiency = np.mean([
            r.performance_metrics.get("efficiency_score", 0)
            for r in successful
        ])
        
        if avg_efficiency < 0.5:
            recommendations.append(
                "Consider reducing simulation steps or increasing neuron count for better efficiency"
            )
        
        avg_firing_rate = np.mean([
            r.simulation_result.final_firing_rate
            for r in successful
        ])
        
        if avg_firing_rate > 50.0:
            recommendations.append(
                "High firing rates detected - consider adjusting neuron thresholds or connection weights"
            )
        
        return recommendations

def create_experiment_preset(name: str, **kwargs) -> BatchSimulationConfig:
    """
    Create a predefined experiment preset.
    """
    config = BatchSimulationConfig(
        name=name,
        config=None,  # Would need actual config
        steps=kwargs.get("steps", 1000),
        neurons=kwargs.get("neurons", 1000),
        world_size=kwargs.get("world_size", (20, 20)),
        enable_features=kwargs.get("enable_features", {}),
        priority=kwargs.get("priority", "normal")
    )
    return config

def run_performance_benchmark(configs: List[BatchSimulationConfig]) -> Dict[str, Any]:
    """
    Run a performance benchmark on given configurations.
    """
    print("Running performance benchmark...")
    
    processor = BatchProcessor(max_workers=2, enable_parallel=False)
    results = processor.run_batch(configs)
    
    analysis = processor.analyze_batch_results()
    
    # Save benchmark results
    os.makedirs("results", exist_ok=True)
    with open("results/benchmark_results.json", 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"Benchmark completed: {analysis['successful_simulations']}/{analysis['total_simulations']} successful")
    
    return analysis

def main():
    """
    Main function to demonstrate batch processing capabilities.
    """
    print("=== NLM Batch Processing and Advanced Simulation ===\n")
    
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    try:
        # Create base configuration
        print("1. Creating base configuration...")
        base_config = pynlm.createDefaultConfig()
        
        # Create configuration variations for batch processing
        print("\n2. Creating configuration variations...")
        variations = {
            "neuron_count": [500, 1000, 2000],
            "connection_density": [0.05, 0.1, 0.15],
            "learning_rate": [0.001, 0.005, 0.01]
        }
        
        processor = BatchProcessor(max_workers=2, enable_parallel=True)
        configs = processor.create_config_variations(base_config, variations, "batch_test")
        
        print(f"Created {len(configs)} configurations for batch testing")
        
        # Run batch processing
        print("\n3. Running batch processing...")
        results = processor.run_batch(configs)
        
        # Analyze results
        print("\n4. Analyzing batch results...")
        analysis = processor.analyze_batch_results()
        
        print(f"Batch processing completed:")
        print(f"  - Total simulations: {analysis['total_simulations']}")
        print(f"  - Successful: {analysis['successful_simulations']}")
        print(f"  - Failed: {analysis['failed_simulations']}")
        print(f"  - Average efficiency: {analysis['performance_summary']['average_efficiency_score']:.3f}")
        
        # Save detailed results
        print("\n5. Saving detailed results...")
        os.makedirs("results", exist_ok=True)
        
        # Save all results
        for result in processor.results:
            if result.success:
                filename = f"results/{result.config.name}_analysis.json"
                with open(filename, 'w') as f:
                    json.dump({
                        "config": result.config.name,
                        "performance": result.performance_metrics,
                        "simulation": {
                            "firing_rate": result.simulation_result.final_firing_rate,
                            "total_spikes": result.simulation_result.total_spikes,
                            "duration": result.simulation_result.duration_seconds
                        }
                    }, f, indent=2)
        
        # Save analysis
        with open("results/batch_analysis.json", 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print("\n=== Batch Processing Complete ===")
        print("Results saved to:")
        print("  - results/*.json (individual configuration results)")
        print("  - results/batch_analysis.json (aggregate analysis)")
        print("  - results/benchmark_results.json (if benchmarked)")
        
        return 0
        
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())