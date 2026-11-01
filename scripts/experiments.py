"""
NLM Experiment Framework - Advanced experiment management for NLM brain simulations
"""

import json
import time
import numpy as np
import pandas as pd
from pathlib import Path
from typing import Dict, Any, List, Optional, Callable
from dataclasses import dataclass, field
from datetime import datetime, timedelta
import threading
import queue

@dataclass
class ExperimentConfig:
    """Configuration for NLM experiments"""
    name: str
    description: str = ""
    brain_config: Dict[str, Any] = field(default_factory=dict)
    world_config: Dict[str, Any] = field(default_factory=dict)
    duration: int = 100
    steps_per_iteration: int = 10
    save_checkpoints: bool = True
    checkpoint_interval: int = 50
    metrics_to_record: List[str] = field(default_factory=list)
    parallel_runs: int = 1
    max_memory_usage: float = 1024.0  # MB

@dataclass
class ExperimentResult:
    """Results from an NLM experiment"""
    experiment_name: str
    start_time: datetime
    end_time: datetime
    metrics: Dict[str, Any] = field(default_factory=dict)
    checkpoints: List[str] = field(default_factory=list)
    success: bool = True
    error_message: str = ""
    performance_stats: Dict[str, Any] = field(default_factory=dict)

class ExperimentRunner:
    """Manages and runs NLM experiments with advanced features"""
    
    def __init__(self, output_dir: str = "experiment_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.experiments = {}
        self.active_runs = {}
        self.results_history = []
        
    def create_experiment(self, config: ExperimentConfig) -> bool:
        """Create and register an experiment"""
        try:
            if config.name in self.experiments:
                print(f"Experiment '{config.name}' already exists")
                return False
            
            # Validate configuration
            self._validate_config(config)
            
            # Create experiment directory
            experiment_dir = self.output_dir / config.name
            experiment_dir.mkdir(exist_ok=True)
            
            # Save configuration
            config_path = experiment_dir / "config.json"
            with open(config_path, 'w') as f:
                json.dump(config.__dict__, f, indent=2, default=str)
            
            self.experiments[config.name] = config
            print(f"Created experiment '{config.name}' in {experiment_dir}")
            return True
            
        except Exception as e:
            print(f"Error creating experiment: {e}")
            return False
    
    def run_experiment(self, name: str, use_gpu: bool = False, 
                      monitoring: bool = True) -> ExperimentResult:
        """Run a single experiment"""
        try:
            if name not in self.experiments:
                raise ValueError(f"Experiment '{name}' not found")
            
            config = self.experiments[name]
            print(f"Starting experiment '{name}' for {config.duration} iterations")
            
            result = ExperimentResult(
                experiment_name=name,
                start_time=datetime.now()
            )
            
            # Initialize experiment state
            experiment_dir = self.output_dir / name
            metrics_file = experiment_dir / "metrics.csv"
            
            # Create metrics dataframe
            metrics_data = []
            
            # Run simulation iterations
            for iteration in range(config.duration):
                iteration_start = time.time()
                
                # Simulate brain operation
                iteration_metrics = self._run_iteration(config, iteration)
                iteration_metrics['iteration'] = iteration
                iteration_metrics['start_time'] = time.time()
                iteration_metrics['end_time'] = iteration_start
                iteration_metrics['duration'] = iteration_metrics['end_time'] - iteration_metrics['start_time']
                
                metrics_data.append(iteration_metrics)
                
                # Save checkpoint if requested
                if config.save_checkpoints and iteration % config.checkpoint_interval == 0:
                    checkpoint_file = experiment_dir / f"checkpoint_iter_{iteration}.json"
                    self._save_checkpoint(config, iteration, checkpoint_file)
                    result.checkpoints.append(str(checkpoint_file))
                
                # Record metrics
                for metric_name, metric_value in iteration_metrics.items():
                    if metric_name not in config.metrics_to_record:
                        config.metrics_to_record.append(metric_name)
                
                # Monitor performance if enabled
                if monitoring:
                    self._monitor_performance(config, iteration)
            
            # Finalize results
            result.end_time = datetime.now()
            result.metrics = self._aggregate_metrics(metrics_data)
            result.performance_stats = self._calculate_performance_stats(metrics_data)
            
            # Save metrics to CSV
            df = pd.DataFrame(metrics_data)
            df.to_csv(metrics_file, index=False)
            
            result.success = True
            
            # Save experiment result
            result_file = experiment_dir / "result.json"
            with open(result_file, 'w') as f:
                json.dump(result.__dict__, f, indent=2, default=str)
            
            self.results_history.append(result)
            self.active_runs[name] = result
            
            print(f"Experiment '{name}' completed successfully in {result.end_time - result.start_time}")
            return result
            
        except Exception as e:
            print(f"Error running experiment '{name}': {e}")
            result = ExperimentResult(
                experiment_name=name,
                start_time=datetime.now(),
                end_time=datetime.now(),
                success=False,
                error_message=str(e)
            )
            return result
    
    def run_multiple_experiments(self, experiment_names: List[str], 
                               parallel: bool = True) -> List[ExperimentResult]:
        """Run multiple experiments in parallel or sequentially"""
        if parallel:
            from concurrent.futures import ThreadPoolExecutor, as_completed
            
            results = []
            with ThreadPoolExecutor(max_workers=min(len(experiment_names), 4)) as executor:
                # Submit all experiments
                future_to_name = {
                    executor.submit(self.run_experiment, name): name
                    for name in experiment_names
                }
                
                # Collect results as they complete
                for future in as_completed(future_to_name):
                    name = future_to_name[future]
                    try:
                        result = future.result()
                        results.append(result)
                        print(f"Completed experiment: {name}")
                    except Exception as e:
                        print(f"Experiment {name} generated an exception: {e}")
                        results.append(ExperimentResult(
                            experiment_name=name,
                            start_time=datetime.now(),
                            end_time=datetime.now(),
                            success=False,
                            error_message=str(e)
                        ))
            
            return results
        else:
            # Sequential execution
            results = []
            for name in experiment_names:
                result = self.run_experiment(name)
                results.append(result)
            
            return results
    
    def analyze_results(self, experiment_name: str, analysis_type: str = "comprehensive") -> Dict[str, Any]:
        """Analyze experiment results"""
        try:
            if experiment_name not in self.experiments:
                raise ValueError(f"Experiment '{experiment_name}' not found")
            
            result = None
            for r in self.results_history:
                if r.experiment_name == experiment_name:
                    result = r
                    break
            
            if not result:
                raise ValueError(f"No results found for experiment '{experiment_name}'")
            
            analysis = {}
            
            if analysis_type == "comprehensive" or analysis_type == "basic":
                # Basic analysis
                analysis['experiment_name'] = experiment_name
                analysis['duration_seconds'] = (result.end_time - result.start_time).total_seconds()
                analysis['success_rate'] = 100.0 if result.success else 0.0
                analysis['num_checkpoints'] = len(result.checkpoints)
                analysis['final_metrics'] = result.metrics
            
            if analysis_type == "comprehensive" or analysis_type == "performance":
                # Performance analysis
                analysis['performance'] = result.performance_stats
                if 'metrics' in result.metrics:
                    analysis['avg_metrics'] = {
                        k: np.mean(v) if isinstance(v, list) else v
                        for k, v in result.metrics['metrics'].items()
                    }
            
            if analysis_type == "comprehensive" or analysis_type == "trends":
                # Trend analysis would require time-series data
                analysis['trend_analysis'] = "Would require time-series data"
            
            # Save analysis
            analysis_file = self.output_dir / experiment_name / "analysis.json"
            with open(analysis_file, 'w') as f:
                json.dump(analysis, f, indent=2, default=str)
            
            return analysis
            
        except Exception as e:
            print(f"Error analyzing results: {e}")
            raise
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict[str, Any]:
        """Compare multiple experiments"""
        try:
            comparison = {}
            
            for name in experiment_names:
                if name not in self.experiments:
                    continue
                
                # Get results
                result = None
                for r in self.results_history:
                    if r.experiment_name == name:
                        result = r
                        break
                
                if result:
                    comparison[name] = {
                        'success': result.success,
                        'duration': (result.end_time - result.start_time).total_seconds(),
                        'num_checkpoints': len(result.checkpoints),
                        'final_metrics': result.metrics
                    }
            
            # Save comparison
            comparison_file = self.output_dir / "experiment_comparison.json"
            with open(comparison_file, 'w') as f:
                json.dump(comparison, f, indent=2, default=str)
            
            return comparison
            
        except Exception as e:
            print(f"Error comparing experiments: {e}")
            raise
    
    def get_experiment_summary(self, name: str) -> Dict[str, Any]:
        """Get summary of an experiment"""
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        result = None
        for r in self.results_history:
            if r.experiment_name == name:
                result = r
                break
        
        if not result:
            return {'experiment_name': name, 'status': 'no_results'}
        
        return {
            'experiment_name': name,
            'config': self.experiments[name].__dict__,
            'result': result.__dict__
        }
    
    def _validate_config(self, config: ExperimentConfig):
        """Validate experiment configuration"""
        if not config.name:
            raise ValueError("Experiment name cannot be empty")
        
        if config.duration <= 0:
            raise ValueError("Duration must be positive")
        
        if config.steps_per_iteration <= 0:
            raise ValueError("Steps per iteration must be positive")
        
        if config.checkpoint_interval <= 0:
            raise ValueError("Checkpoint interval must be positive")
        
        if config.max_memory_usage <= 0:
            raise ValueError("Max memory usage must be positive")
    
    def _run_iteration(self, config: ExperimentConfig, iteration: int) -> Dict[str, Any]:
        """Run a single iteration of the experiment"""
        # Simulate brain operation and collect metrics
        metrics = {
            'iteration': iteration,
            'neurons_processed': config.brain_config.get('neuron_count', 500),
            'regions_active': config.brain_config.get('region_count', 2),
            'activity_level': np.random.random() * 0.8 + 0.1,
            'plasticity_change': np.random.random() * 0.05 - 0.025,
            'memory_utilization': np.random.random() * 0.6 + 0.2,
            'reward_signal': np.random.random() * 0.3,
            'novelty_detected': np.random.random() < 0.2,
            'energy_consumption': np.random.random() * 10 + 5,
            'computation_time': np.random.random() * 0.1 + 0.05
        }
        
        return metrics
    
    def _save_checkpoint(self, config: ExperimentConfig, iteration: int, filepath: Path):
        """Save experiment checkpoint"""
        checkpoint_data = {
            'iteration': iteration,
            'timestamp': datetime.now().isoformat(),
            'brain_config': config.brain_config,
            'world_config': config.world_config,
            'metrics': self._run_iteration(config, iteration)
        }
        
        with open(filepath, 'w') as f:
            json.dump(checkpoint_data, f, indent=2, default=str)
    
    def _monitor_performance(self, config: ExperimentConfig, iteration: int):
        """Monitor performance during experiment"""
        # Simulate performance monitoring
        pass
    
    def _aggregate_metrics(self, metrics_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Aggregate metrics from all iterations"""
        if not metrics_data:
            return {}
        
        aggregated = {}
        for key in metrics_data[0].keys():
            values = [m[key] for m in metrics_data if key in m]
            
            if all(isinstance(v, (int, float)) for v in values):
                aggregated[key] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'median': np.median(values)
                }
            else:
                aggregated[key] = values
        
        return {'metrics': aggregated}
    
    def _calculate_performance_stats(self, metrics_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate performance statistics"""
        if not metrics_data:
            return {}
        
        stats = {
            'total_iterations': len(metrics_data),
            'avg_computation_time': np.mean([m.get('computation_time', 0) for m in metrics_data]),
            'total_energy_consumption': np.sum([m.get('energy_consumption', 0) for m in metrics_data]),
            'activity_range': {
                'min': np.min([m.get('activity_level', 0) for m in metrics_data]),
                'max': np.max([m.get('activity_level', 0) for m in metrics_data])
            }
        }
        
        return stats

class Phase6Experiment(ExperimentRunner):
    """Specialized experiment runner for Phase 6 integration tests"""
    
    def __init__(self, output_dir: str = "phase6_experiments"):
        super().__init__(output_dir)
    
    def create_integration_experiment(self, config: Dict[str, Any]) -> bool:
        """Create a Phase 6 integration experiment"""
        experiment_config = ExperimentConfig(
            name="phase6_integration",
            description="Phase 6 integration test for NLM brain systems",
            brain_config=config.get('brain_config', {}),
            world_config=config.get('world_config', {}),
            duration=config.get('duration', 200),
            steps_per_iteration=config.get('steps_per_iteration', 5),
            save_checkpoints=config.get('save_checkpoints', True),
            checkpoint_interval=config.get('checkpoint_interval', 50),
            metrics_to_record=config.get('metrics_to_record', [
                'activity_level', 'plasticity_change', 'memory_utilization',
                'reward_signal', 'novelty_detected', 'energy_consumption'
            ]),
            parallel_runs=config.get('parallel_runs', 1),
            max_memory_usage=config.get('max_memory_usage', 2048.0)
        )
        
        return self.create_experiment(experiment_config)
    
    def run_integration_test(self) -> ExperimentResult:
        """Run Phase 6 integration test"""
        config = ExperimentConfig(
            name="phase6_integration_test",
            description="Comprehensive integration test of all NLM systems",
            brain_config={
                'neuron_count': 1000,
                'region_count': 4,
                'excitatory_ratio': 0.85,
                'plasticity_enabled': True,
                'development_enabled': True,
                'novelty_enabled': True,
                'curiosity_enabled': True,
                'memory_enabled': True,
                'prediction_enabled': True,
                'cognition_enabled': True
            },
            world_config={
                'width': 200.0,
                'height': 200.0,
                'max_energy': 200.0,
                'energy_decay_rate': 0.002,
                'agent_start_x': 100.0,
                'agent_start_y': 100.0
            },
            duration=200,
            steps_per_iteration=5,
            save_checkpoints=True,
            checkpoint_interval=50,
            metrics_to_record=[
                'activity_level', 'plasticity_change', 'memory_utilization',
                'reward_signal', 'novelty_detected', 'energy_consumption',
                'prediction_accuracy', 'attention_focus', 'planning_efficiency'
            ]
        )
        
        # Add special metrics for Phase 6
        config.metrics_to_record.extend([
            'working_memory_usage',
            'episodic_memory_recall',
            'associative_memory_strength',
            'dopamine_level',
            'curiosity_level',
            'novelty_detection_rate',
            'prediction_error',
            'planning_confidence',
            'concept_clarity'
        ])
        
        self.create_experiment(config)
        return self.run_experiment("phase6_integration_test")

# Export important classes
__all__ = [
    'ExperimentConfig',
    'ExperimentResult', 
    'ExperimentRunner',
    'Phase6Experiment'
]