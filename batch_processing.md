# NLM (Neural Learning Machine) - Batch Processing and Automation

**Advanced batch processing, automation, and workflow management for NLM Phase 6**

## Overview

This module provides comprehensive batch processing, automation, and workflow management capabilities for NLM neural simulations. It includes:

- Batch execution of multiple simulations
- Parameter sweeps and optimization
- Workflow management and scheduling
- Automated testing and validation
- Results aggregation and analysis
- Integration with experiment management systems

## Key Components

### 1. Batch Simulation Engine

```python
import pynlm
import numpy as np
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass
from enum import Enum
import multiprocessing as mp
import threading
import json
import time
from pathlib import Path

class BatchMode(Enum):
    SEQUENTIAL = 1      # Run batches sequentially
    PARALLEL = 2        # Run batches in parallel
    DISTRIBUTED = 3     # Distributed processing (requires additional setup)
    PIPELINE = 4        # Pipeline processing

@dataclass
class BatchResult:
    """Results from a single batch run."""
    batch_id: str
    configuration: Dict
    parameters: Dict
    start_time: float
    end_time: float
    duration: float
    metrics: Dict
    success: bool
    error_message: Optional[str]
    output_files: List[str]

@dataclass
class BatchConfiguration:
    """Configuration for batch processing."""
    batch_id: str
    mode: BatchMode
    num_workers: int
    max_memory_mb: int
    timeout_seconds: int
    checkpoint_interval: int
    continue_on_error: bool
    parallel_strategy: str  # "process", "thread", "hybrid"

class NLMBatchProcessor:
    """Advanced batch processor for NLM simulations."""
    
    def __init__(self, config: BatchConfiguration):
        self.config = config
        self.batches: List[Batch] = []
        self.results: Dict[str, BatchResult] = {}
        self.worker_pool: Optional[mp.Pool] = None
        self.thread_pool: Optional[threading.Thread] = None
        self.monitoring = False
        
    def add_batch(self, batch: 'Batch'):
        """Add a batch to the processor."""
        self.batches.append(batch)
    
    def run_all_batches(self) -> Dict[str, BatchResult]:
        """Run all batches in the processor."""
        print(f"Starting batch processing of {len(self.batches)} batches...")
        
        results = {}
        
        for i, batch in enumerate(self.batches):
            print(f"\n{'='*60}")
            print(f"Running batch {i+1}/{len(self.batches)}: {batch.name}")
            print(f"{'='*60}")
            
            try:
                if self.config.mode == BatchMode.PARALLEL:
                    result = self._run_batch_parallel(batch)
                elif self.config.mode == BatchMode.SEQUENTIAL:
                    result = self._run_batch_sequential(batch)
                elif self.config.mode == BatchMode.PIPELINE:
                    result = self._run_batch_pipeline(batch)
                else:
                    result = self._run_batch_sequential(batch)
                
                results[batch.batch_id] = result
                print(f"✓ Batch {batch.batch_id} completed successfully")
                
            except Exception as e:
                error_msg = f"Batch {batch.batch_id} failed: {str(e)}"
                print(f"✗ {error_msg}")
                
                if not self.config.continue_on_error:
                    raise
                
                # Record failed batch
                results[batch.batch_id] = BatchResult(
                    batch_id=batch.batch_id,
                    configuration=batch.configuration,
                    parameters=batch.parameters,
                    start_time=time.time(),
                    end_time=time.time(),
                    duration=0.0,
                    metrics={},
                    success=False,
                    error_message=error_msg,
                    output_files=[]
                )
        
        self.results = results
        return results
    
    def _run_batch_sequential(self, batch: 'Batch') -> BatchResult:
        """Run a batch sequentially."""
        print(f"Running batch {batch.name} sequentially...")
        
        start_time = time.time()
        batch_results = []
        output_files = []
        
        for i, param_set in enumerate(batch.parameter_sets):
            print(f"  Processing configuration {i+1}/{len(batch.parameter_sets)}")
            
            try:
                # Create configuration from parameter set
                config = self._create_config_from_params(batch.configuration, param_set)
                
                # Run simulation
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Setup world and agent if needed
                world = None
                agent = None
                if batch.use_world:
                    world = pynlm.createSimpleWorld()
                    world.configure(
                        param_set.get('world_width', 20),
                        param_set.get('world_height', 20),
                        param_set.get('vision_width', 8),
                        param_set.get('vision_height', 8)
                    )
                    world.reset()
                
                if world and batch.use_agent:
                    agent = pynlm.createAgentBrain(brain)
                    agent.initialize(world)
                    
                    # Enable features based on parameters
                    agent.enableRewardModulation(
                        param_set.get('enable_reward_modulation', True)
                    )
                    agent.enableCuriosity(
                        param_set.get('enable_curiosity', True)
                    )
                    agent.enableDevelopment(
                        param_set.get('enable_development', True)
                    )
                
                # Run simulation
                simulation_steps = param_set.get('steps', 100)
                for step in range(simulation_steps):
                    if world and agent:
                        world.update(0.1)
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        brain.step(step)
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                    else:
                        brain.step(step)
                
                # Collect metrics
                metrics = self._collect_metrics(brain, agent, world, param_set)
                
                # Save results if requested
                if batch.save_individual_results:
                    output_file = self._save_individual_results(
                        brain, agent, world, batch.batch_id, i, param_set
                    )
                    output_files.append(output_file)
                
                batch_results.append(metrics)
                
            except Exception as e:
                print(f"    ✗ Configuration {i+1} failed: {e}")
                if not batch.continue_on_error:
                    raise
                # Continue with next configuration
        
        end_time = time.time()
        
        # Aggregate results
        aggregated_metrics = self._aggregate_batch_results(batch_results)
        
        return BatchResult(
            batch_id=batch.batch_id,
            configuration=batch.configuration,
            parameters=batch.parameters,
            start_time=start_time,
            end_time=end_time,
            duration=end_time - start_time,
            metrics=aggregated_metrics,
            success=True,
            error_message=None,
            output_files=output_files
        )
    
    def _run_batch_parallel(self, batch: 'Batch') -> BatchResult:
        """Run a batch in parallel using multiprocessing."""
        print(f"Running batch {batch.name} in parallel...")
        
        start_time = time.time()
        output_files = []
        
        # Prepare parameter sets for parallel processing
        param_sets = batch.parameter_sets
        num_workers = min(self.config.num_workers, len(param_sets))
        
        if num_workers <= 1:
            return self._run_batch_sequential(batch)
        
        # Create worker pool
        self.worker_pool = mp.Pool(num_workers)
        
        try:
            # Distribute work
            worker_results = []
            for i in range(0, len(param_sets), num_workers):
                batch_slice = param_sets[i:i + num_workers]
                results = self.worker_pool.starmap(
                    self._process_single_parameter_set,
                    [(batch, ps) for ps in batch_slice]
                )
                worker_results.extend(results)
            
            # Collect results
            batch_results = []
            for result in worker_results:
                if result['success']:
                    batch_results.append(result['metrics'])
                output_files.extend(result.get('output_files', []))
            
            # Aggregate results
            aggregated_metrics = self._aggregate_batch_results(batch_results)
            
        finally:
            if self.worker_pool:
                self.worker_pool.close()
                self.worker_pool.join()
                self.worker_pool = None
        
        end_time = time.time()
        
        return BatchResult(
            batch_id=batch.batch_id,
            configuration=batch.configuration,
            parameters=batch.parameters,
            start_time=start_time,
            end_time=end_time,
            duration=end_time - start_time,
            metrics=aggregated_metrics,
            success=True,
            error_message=None,
            output_files=output_files
        )
    
    def _run_batch_pipeline(self, batch: 'Batch') -> BatchResult:
        """Run a batch using pipeline processing."""
        print(f"Running batch {batch.name} in pipeline mode...")
        
        start_time = time.time()
        output_files = []
        
        # Pipeline stages
        stages = [
            ('setup', self._setup_pipeline_stage),
            ('simulation', self._run_simulation_pipeline_stage),
            ('analysis', self._analyze_pipeline_stage),
            ('output', self._output_pipeline_stage)
        ]
        
        # Initialize data pipeline
        pipeline_data = {'batch_id': batch.batch_id}
        
        # Process through pipeline stages
        for stage_name, stage_func in stages:
            print(f"  Processing stage: {stage_name}")
            pipeline_data = stage_func(batch, pipeline_data)
            
            # Check for completion
            if pipeline_data.get('completed', False):
                break
        
        end_time = time.time()
        
        return BatchResult(
            batch_id=batch.batch_id,
            configuration=batch.configuration,
            parameters=batch.parameters,
            start_time=start_time,
            end_time=end_time,
            duration=end_time - start_time,
            metrics=pipeline_data.get('metrics', {}),
            success=pipeline_data.get('success', True),
            error_message=pipeline_data.get('error_message'),
            output_files=output_files
        )
    
    def _process_single_parameter_set(self, batch: 'Batch', param_set: Dict):
        """Process a single parameter set (for parallel execution)."""
        try:
            # Create configuration
            config = self._create_config_from_params(batch.configuration, param_set)
            
            # Run simulation (simplified for multiprocessing)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Collect metrics
            metrics = self._collect_metrics(brain, None, None, param_set)
            
            return {
                'success': True,
                'metrics': metrics,
                'output_files': []
            }
            
        except Exception as e:
            return {
                'success': False,
                'metrics': {},
                'error': str(e),
                'output_files': []
            }
    
    def _create_config_from_params(self, base_config: Dict, param_set: Dict) -> 'Config':
        """Create configuration from parameter set."""
        # Start with base configuration
        config = pynlm.createDefaultConfig()
        
        # Apply base configuration settings
        for key, value in base_config.items():
            if key == 'neuron_count':
                config.set("brain.neuron_count", value)
            elif key == 'synapse_density':
                config.set("brain.synapse_density", value)
            elif key == 'learning_rate':
                config.set("plasticity.stdp.learning_rate", value)
            elif key == 'curiosity_enabled':
                config.set("neuromod.curiosity.enable", value)
            elif key == 'development_enabled':
                config.set("development.enable", value)
        
        # Apply parameter-specific overrides
        for key, value in param_set.items():
            if key == 'neuron_count':
                config.set("brain.neuron_count", value)
            elif key == 'synapse_density':
                config.set("brain.synapse_density", value)
            elif key == 'learning_rate':
                config.set("plasticity.stdp.learning_rate", value)
            elif key == 'curiosity_enabled':
                config.set("neuromod.curiosity.enable", value)
            elif key == 'development_enabled':
                config.set("development.enable", value)
        
        return config
    
    def _collect_metrics(self, brain, agent, world, param_set: Dict) -> Dict:
        """Collect metrics from a simulation run."""
        metrics = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage().value,
            'simulation_steps': param_set.get('steps', 100)
        }
        
        if agent:
            metrics.update({
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation_level': agent.getNeuromodulationLevel()
            })
        
        if world:
            metrics.update({
                'world_time': world.getSimulationTime(),
                'world_width': world.getWidth(),
                'world_height': world.getHeight()
            })
        
        return metrics
    
    def _aggregate_batch_results(self, batch_results: List[Dict]) -> Dict:
        """Aggregate results from multiple parameter sets."""
        if not batch_results:
            return {}
        
        aggregated = {
            'num_configurations': len(batch_results),
            'successful_configurations': len([r for r in batch_results if r.get('success', True)]),
            'failed_configurations': len([r for r in batch_results if not r.get('success', True)]),
            'statistics': {},
            'performance': {},
            'comparisons': {}
        }
        
        # Collect metrics for analysis
        neuron_counts = [r['neuron_count'] for r in batch_results]
        firing_rates = [r['average_firing_rate'] for r in batch_results]
        total_spikes = [r['total_spikes'] for r in batch_results]
        
        # Calculate statistics
        aggregated['statistics'] = {
            'neuron_count': {
                'mean': np.mean(neuron_counts),
                'std': np.std(neuron_counts),
                'min': np.min(neuron_counts),
                'max': np.max(neuron_counts)
            },
            'firing_rate': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates)
            },
            'total_spikes': {
                'mean': np.mean(total_spikes),
                'std': np.std(total_spikes),
                'min': np.min(total_spikes),
                'max': np.max(total_spikes)
            }
        }
        
        # Calculate performance metrics
        aggregated['performance'] = {
            'average_spike_efficiency': np.mean([r['total_spikes'] / r['neuron_count'] 
                                                for r in batch_results if r['neuron_count'] > 0]),
            'average_firing_rate': np.mean(firing_rates),
            'stability_score': self._calculate_stability_score(firing_rates),
            'convergence_indicator': self._calculate_convergence_indicator(batch_results)
        }
        
        return aggregated
    
    def _calculate_stability_score(self, firing_rates: List[float]) -> float:
        """Calculate stability score based on firing rate variance."""
        if len(firing_rates) < 2:
            return 1.0
        
        cv = np.std(firing_rates) / np.mean(firing_rates)
        return max(0.0, 1.0 - cv)
    
    def _calculate_convergence_indicator(self, batch_results: List[Dict]) -> float:
        """Calculate convergence indicator."""
        if len(batch_results) < 10:
            return 0.5
        
        # Simple convergence indicator based on firing rate consistency
        recent_rates = [r['average_firing_rate'] for r in batch_results[-10:]]
        if len(recent_rates) < 2:
            return 0.5
        
        rate_variance = np.var(recent_rates)
        convergence_score = 1.0 - min(rate_variance, 1.0)
        
        return convergence_score
    
    def _save_individual_results(self, brain, agent, world, batch_id: str, 
                                config_index: int, param_set: Dict) -> str:
        """Save individual results for a configuration."""
        # Create output directory
        output_dir = Path(f"batch_results/{batch_id}/config_{config_index}")
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Save brain configuration
        config_file = output_dir / "config.json"
        with open(config_file, 'w') as f:
            json.dump(param_set, f, indent=2)
        
        # Save brain metrics
        metrics_file = output_dir / "metrics.json"
        metrics = self._collect_metrics(brain, agent, world, param_set)
        with open(metrics_file, 'w') as f:
            json.dump(metrics, f, indent=2)
        
        # Save network data if requested
        if param_set.get('save_network', False):
            network_file = output_dir / "network.json"
            # This would require implementing network serialization
            pass
        
        # Save world state if applicable
        if world:
            world_file = output_dir / "world_state.json"
            world_data = {
                'simulation_time': world.getSimulationTime(),
                'agent_position': {
                    'x': world.getAgentBody().x,
                    'y': world.getAgentBody().y
                } if world.getAgentBody() else None
            }
            with open(world_file, 'w') as f:
                json.dump(world_data, f, indent=2)
        
        return str(output_dir)
    
    def _setup_pipeline_stage(self, batch: 'Batch', pipeline_data: Dict) -> Dict:
        """Pipeline setup stage."""
        print("  Setting up pipeline...")
        
        # Initialize resources
        pipeline_data['setup_complete'] = True
        pipeline_data['resources'] = {
            'brain_pool': [],
            'world_pool': [],
            'agent_pool': []
        }
        
        return pipeline_data
    
    def _run_simulation_pipeline_stage(self, batch: 'Batch', pipeline_data: Dict) -> Dict:
        """Pipeline simulation stage."""
        print("  Running simulation pipeline...")
        
        # Simplified pipeline implementation
        # In practice, this would use proper pipeline processing
        results = self._run_batch_sequential(batch)
        
        pipeline_data['metrics'] = results.metrics
        pipeline_data['output_files'] = results.output_files
        pipeline_data['completed'] = True
        
        return pipeline_data
    
    def _analyze_pipeline_stage(self, batch: 'Batch', pipeline_data: Dict) -> Dict:
        """Pipeline analysis stage."""
        print("  Analyzing pipeline results...")
        
        # Add analysis to pipeline data
        if 'metrics' in pipeline_data:
            pipeline_data['analysis'] = {
                'performance_metrics': pipeline_data['metrics'],
                'optimization_recommendations': self._generate_optimization_recommendations(
                    pipeline_data['metrics']
                )
            }
        
        return pipeline_data
    
    def _output_pipeline_stage(self, batch: 'Batch', pipeline_data: Dict) -> Dict:
        """Pipeline output stage."""
        print("  Processing pipeline outputs...")
        
        # Save final results
        if 'output_files' in pipeline_data:
            for output_file in pipeline_data['output_files']:
                print(f"  Output saved: {output_file}")
        
        pipeline_data['output_complete'] = True
        
        return pipeline_data
    
    def _generate_optimization_recommendations(self, metrics: Dict) -> List[str]:
        """Generate optimization recommendations based on metrics."""
        recommendations = []
        
        # Memory optimization
        if metrics.get('memory_growth_mb', 0) > 100:
            recommendations.extend([
                "Reduce neuron count to decrease memory usage",
                "Implement memory pooling for better memory management",
                "Use checkpointing for large simulations"
            ])
        
        # Performance optimization
        if metrics.get('steps_per_second', 0) < 100:
            recommendations.extend([
                "Optimize synaptic operations",
                "Reduce plasticity computation frequency",
                "Use parallel processing for independent simulations"
            ])
        
        # Stability optimization
        if metrics.get('stability_score', 1.0) < 0.8:
            recommendations.extend([
                "Increase exploration parameters for better stability",
                "Implement adaptive learning rates",
                "Add noise to prevent premature convergence"
            ])
        
        return recommendations
    
    def cleanup(self):
        """Clean up resources."""
        if self.worker_pool:
            self.worker_pool.close()
            self.worker_pool.join()
            self.worker_pool = None
        
        if self.thread_pool:
            self.thread_pool.shutdown(wait=True)
            self.thread_pool = None
class Batch:
    """Represents a batch of simulations with common configuration."""
    
    def __init__(self, batch_id: str, name: str):
        self.batch_id = batch_id
        self.name = name
        self.configuration = {}
        self.parameters = {}
        self.parameter_sets = []
        self.use_world = False
        self.use_agent = False
        self.save_individual_results = False
        self.continue_on_error = True
    
    def add_parameter_set(self, param_set: Dict):
        """Add a parameter set to the batch."""
        self.parameter_sets.append(param_set)
    
    def set_configuration(self, config: Dict):
        """Set batch configuration."""
        self.configuration = config
    
    def set_parameters(self, params: Dict):
        """Set batch parameters."""
        self.parameters = params
    
    def enable_world_simulation(self, use_world: bool = True):
        """Enable/disable world simulation."""
        self.use_world = use_world
    
    def enable_agent_simulation(self, use_agent: bool = True):
        """Enable/disable agent simulation."""
        self.use_agent = use_agent

class ParameterSweep:
    """Manages parameter sweeps for batch processing."""
    
    def __init__(self):
        self.sweep_definitions = []
    
    def add_sweep(self, name: str, parameters: Dict[str, List], 
                  base_config: Optional[Dict] = None):
        """Add a parameter sweep definition."""
        sweep = {
            'name': name,
            'parameters': parameters,
            'base_config': base_config or {},
            'combinations': self._generate_parameter_combinations(parameters)
        }
        self.sweep_definitions.append(sweep)
    
    def _generate_parameter_combinations(self, parameters: Dict[str, List]) -> List[Dict]:
        """Generate all parameter combinations."""
        param_names = list(parameters.keys())
        param_values = list(parameters.values())
        
        combinations = []
        
        def generate_combinations(current_combination, current_index):
            if current_index == len(param_names):
                combinations.append(current_combination.copy())
                return
            
            for value in param_values[current_index]:
                current_combination[param_names[current_index]] = value
                generate_combinations(current_combination, current_index + 1)
        
        generate_combinations({}, 0)
        return combinations
    
    def create_batch_from_sweep(self, batch_id: str, sweep_name: str, 
                               use_world: bool = False, use_agent: bool = False) -> Batch:
        """Create a batch from a parameter sweep."""
        sweep = next((s for s in self.sweep_definitions if s['name'] == sweep_name), None)
        
        if not sweep:
            raise ValueError(f"Sweep definition '{sweep_name}' not found")
        
        batch = Batch(batch_id, f"{sweep_name}_sweep")
        batch.set_configuration(sweep['base_config'])
        batch.enable_world_simulation(use_world)
        batch.enable_agent_simulation(use_agent)
        
        for param_set in sweep['combinations']:
            batch.add_parameter_set(param_set)
        
        return batch

# Usage examples
if __name__ == "__main__":
    # Example 1: Simple batch processing
    print("Example 1: Simple Batch Processing")
    print("=" * 50)
    
    config = BatchConfiguration(
        batch_id="simple_batch",
        mode=BatchMode.SEQUENTIAL,
        num_workers=4,
        max_memory_mb=1024,
        timeout_seconds=300,
        checkpoint_interval=100,
        continue_on_error=True
    )
    
    processor = NLMBatchProcessor(config)
    
    # Create simple batch
    batch = Batch("simple_batch_1", "Simple Learning Test")
    batch.add_parameter_set({
        'steps': 100,
        'neuron_count': 500,
        'synapse_density': 0.1
    })
    batch.add_parameter_set({
        'steps': 200,
        'neuron_count': 1000,
        'synapse_density': 0.05
    })
    
    processor.add_batch(batch)
    
    # Run batches
    results = processor.run_all_batches()
    
    # Print results
    for batch_id, result in results.items():
        print(f"\nBatch {batch_id}:")
        print(f"  Success: {result.success}")
        print(f"  Duration: {result.duration:.2f}s")
        print(f"  Metrics keys: {list(result.metrics.keys())}")
    
    # Example 2: Parameter sweep
    print("\n\nExample 2: Parameter Sweep")
    print("=" * 50)
    
    sweep = ParameterSweep()
    sweep.add_sweep(
        name="learning_efficiency",
        parameters={
            'neuron_count': [500, 1000, 1500],
            'synapse_density': [0.05, 0.1, 0.15],
            'learning_rate': [0.001, 0.01, 0.1]
        },
        base_config={
            'enable_reward_modulation': True,
            'enable_curiosity': True
        }
    )
    
    sweep_batch = sweep.create_batch_from_sweep(
        batch_id="sweep_1",
        sweep_name="learning_efficiency",
        use_world=False,
        use_agent=True
    )
    
    processor2 = NLMBatchProcessor(config)
    processor2.add_batch(sweep_batch)
    
    results2 = processor2.run_all_batches()
    
    # Print sweep results
    total_combinations = len(sweep_batch.parameter_sets)
    successful_combinations = sum(1 for r in results2.values() if r.success)
    
    print(f"\nSweep Results:")
    print(f"  Total combinations: {total_combinations}")
    print(f"  Successful combinations: {successful_combinations}")
    print(f"  Success rate: {successful_combinations/total_combinations*100:.1f}%")
    
    # Example 3: Parallel batch processing
    print("\n\nExample 3: Parallel Batch Processing")
    print("=" * 50)
    
    parallel_config = BatchConfiguration(
        batch_id="parallel_batch",
        mode=BatchMode.PARALLEL,
        num_workers=8,
        max_memory_mb=2048,
        timeout_seconds=600,
        checkpoint_interval=200,
        continue_on_error=True
    )
    
    parallel_processor = NLMBatchProcessor(parallel_config)
    
    # Create parallel batch with multiple parameter sets
    parallel_batch = Batch("parallel_batch_1", "Parallel Learning Test")
    for i in range(20):  # 20 parameter sets
        parallel_batch.add_parameter_set({
            'steps': 50 + i * 10,
            'neuron_count': 300 + i * 50,
            'synapse_density': 0.05 + i * 0.01,
            'learning_rate': 0.001 * (1 + i * 0.2)
        })
    
    parallel_processor.add_batch(parallel_batch)
    
    print("Running parallel batch...")
    parallel_results = parallel_processor.run_all_batches()
    
    # Print parallel results
    total_time = sum(r.duration for r in parallel_results.values())
    print(f"\nParallel Batch Results:")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Successful batches: {len([r for r in parallel_results.values() if r.success])}")
    print(f"  Average time per batch: {total_time/len(parallel_results):.2f}s")
    
    # Example 4: Pipeline processing
    print("\n\nExample 4: Pipeline Processing")
    print("=" * 50)
    
    pipeline_config = BatchConfiguration(
        batch_id="pipeline_batch",
        mode=BatchMode.PIPELINE,
        num_workers=4,
        max_memory_mb=1024,
        timeout_seconds=300,
        checkpoint_interval=100,
        continue_on_error=True
    )
    
    pipeline_processor = NLMBatchProcessor(pipeline_config)
    
    # Create pipeline batch
    pipeline_batch = Batch("pipeline_batch_1", "Pipeline Learning Test")
    for i in range(5):
        pipeline_batch.add_parameter_set({
            'steps': 100,
            'neuron_count': 800 + i * 100,
            'synapse_density': 0.1 - i * 0.02,
            'learning_rate': 0.01 * (1 - i * 0.2)
        })
    
    pipeline_processor.add_batch(pipeline_batch)
    
    print("Running pipeline batch...")
    pipeline_results = pipeline_processor.run_all_batches()
    
    # Generate final summary
    print("\n\n" + "="*80)
    print("FINAL BATCH PROCESSING SUMMARY")
    print("="*80)
    
    all_results = {**results, **results2, **parallel_results, **pipeline_results}
    
    total_batches = len(all_results)
    successful_batches = len([r for r in all_results.values() if r.success])
    failed_batches = total_batches - successful_batches
    
    total_time = sum(r.duration for r in all_results.values())
    
    print(f"Total batches processed: {total_batches}")
    print(f"Successful batches: {successful_batches}")
    print(f"Failed batches: {failed_batches}")
    print(f"Overall success rate: {successful_batches/total_batches*100:.1f}%")
    print(f"Total processing time: {total_time:.2f}s")
    print(f"Average time per batch: {total_time/total_batches:.2f}s")
    
    if failed_batches > 0:
        print(f"\nFailed batches:")
        for batch_id, result in all_results.items():
            if not result.success:
                print(f"  {batch_id}: {result.error_message}")
    
    # Save all results to file
    all_results_data = {
        'summary': {
            'total_batches': total_batches,
            'successful_batches': successful_batches,
            'failed_batches': failed_batches,
            'success_rate': successful_batches/total_batches*100,
            'total_time': total_time,
            'average_time_per_batch': total_time/total_batches
        },
        'results': {}
    }
    
    for batch_id, result in all_results.items():
        all_results_data['results'][batch_id] = {
            'success': result.success,
            'duration': result.duration,
            'parameters': result.parameters,
            'metrics': result.metrics,
            'error_message': result.error_message,
            'output_files': result.output_files
        }
    
    with open("all_batch_results.json", "w") as f:
        json.dump(all_results_data, f, indent=2)
    
    print(f"\nAll results saved to 'all_batch_results.json'")
```

## Advanced Features

### 1. Automated Experiment Design

```python
class AutomatedExperimentDesigner:
    """Automatically design experiments based on objectives."""
    
    def __init__(self):
        self.objectives = []
        self.constraints = {}
    
    def add_objective(self, name: str, target_metric: str, 
                     optimization_direction: str = "maximize"):
        """Add an optimization objective."""
        self.objectives.append({
            'name': name,
            'target_metric': target_metric,
            'optimization_direction': optimization_direction
        })
    
    def add_constraint(self, name: str, constraint_type: str, 
                      value: Any, description: str = ""):
        """Add a constraint."""
        self.constraints[name] = {
            'type': constraint_type,
            'value': value,
            'description': description
        }
    
    def design_experiments(self, search_space: Dict, 
                          num_experiments: int = 10) -> List[Batch]:
        """Design experiments based on objectives and constraints."""
        # This would use optimization algorithms (e.g., Bayesian optimization)
        # to select the best parameter combinations
        
        experiments = []
        
        # For demonstration, use random sampling
        param_names = list(search_space.keys())
        
        for i in range(num_experiments):
            # Randomly select parameter values
            param_set = {}
            for param_name in param_names:
                param_values = search_space[param_name]
                param_set[param_name] = np.random.choice(param_values)
            
            # Create batch
            batch = Batch(f"exp_{i:04d}", f"Experiment {i+1}")
            batch.add_parameter_set(param_set)
            
            experiments.append(batch)
        
        return experiments
    
    def optimize_for_objectives(self, base_config: Dict, 
                               search_space: Dict, 
                               num_trials: int = 20) -> Dict:
        """Optimize parameters for given objectives."""
        # This would use optimization libraries (e.g., scikit-optimize, hyperopt)
        # to find optimal parameter values
        
        print("Optimizing parameters for objectives...")
        
        best_parameters = None
        best_score = float('-inf')
        
        for trial in range(num_trials):
            # Generate random parameters
            params = {}
            for param_name in search_space.keys():
                params[param_name] = np.random.choice(search_space[param_name])
            
            # Calculate objective score (conceptual)
            score = self._calculate_objective_score(params, base_config)
            
            if score > best_score:
                best_score = score
                best_parameters = params
        
        return {
            'best_parameters': best_parameters,
            'best_score': best_score,
            'search_space': search_space
        }
    
    def _calculate_objective_score(self, params: Dict, base_config: Dict) -> float:
        """Calculate objective score for given parameters."""
        # This is a conceptual implementation
        # In practice, this would run simulations and evaluate objectives
        
        # Example: Optimize for learning efficiency
        # score = (learning_speed * 0.4) + (stability * 0.3) + (sample_efficiency * 0.3)
        
        # For demonstration, return a simple metric
        neuron_count = params.get('neuron_count', 1000)
        synapse_density = params.get('synapse_density', 0.1)
        learning_rate = params.get('learning_rate', 0.01)
        
        # Simple heuristic for demonstration
        efficiency = (neuron_count / 2000) * 0.3
        connectivity = (synapse_density / 0.2) * 0.4
        plasticity = min(learning_rate / 0.1, 1.0) * 0.3
        
        score = efficiency + connectivity + plasticity
        
        return score
```

### 2. Results Analysis and Visualization

```python
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

class BatchResultsAnalyzer:
    """Analyze and visualize batch processing results."""
    
    def __init__(self):
        self.results_df = None
    
    def load_results(self, batch_results: Dict):
        """Load batch results into DataFrame."""
        records = []
        
        for batch_id, result in batch_results.items():
            for param_idx, param_set in enumerate(result['parameters'].get('parameter_sets', [])):
                record = {
                    'batch_id': batch_id,
                    'parameter_set': param_idx,
                    'neuron_count': param_set.get('neuron_count', 0),
                    'synapse_density': param_set.get('synapse_density', 0.0),
                    'learning_rate': param_set.get('learning_rate', 0.0),
                    'steps': param_set.get('steps', 0),
                    'success': result.success,
                    'duration': result.duration,
                    'total_spikes': result.metrics.get('total_spikes', 0),
                    'firing_rate': result.metrics.get('average_firing_rate', 0.0),
                    'stability_score': result.metrics.get('stability_score', 0.0),
                    'error_message': result.error_message or ''
                }
                
                records.append(record)
        
        self.results_df = pd.DataFrame(records)
        return self.results_df
    
    def analyze_performance_tradeoffs(self) -> Dict:
        """Analyze performance tradeoffs between parameters."""
        if self.results_df is None:
            return {}
        
        analysis = {}
        
        # Neuron count vs performance
        if 'neuron_count' in self.results_df.columns:
            neuron_groups = self.results_df.groupby('neuron_count')
            neuron_analysis = {}
            
            for neuron_count, group in neuron_groups:
                neuron_analysis[str(neuron_count)] = {
                    'mean_firing_rate': group['firing_rate'].mean(),
                    'mean_total_spikes': group['total_spikes'].mean(),
                    'success_rate': group['success'].mean(),
                    'mean_duration': group['duration'].mean()
                }
            
            analysis['neuron_count_analysis'] = neuron_analysis
        
        # Synapse density vs performance
        if 'synapse_density' in self.results_df.columns:
            density_groups = self.results_df.groupby('synapse_density')
            density_analysis = {}
            
            for density, group in density_groups:
                density_analysis[str(density)] = {
                    'mean_firing_rate': group['firing_rate'].mean(),
                    'mean_total_spikes': group['total_spikes'].mean(),
                    'success_rate': group['success'].mean(),
                    'mean_duration': group['duration'].mean()
                }
            
            analysis['synapse_density_analysis'] = density_analysis
        
        # Learning rate vs performance
        if 'learning_rate' in self.results_df.columns:
            rate_groups = self.results_df.groupby('learning_rate')
            rate_analysis = {}
            
            for rate, group in rate_groups:
                rate_analysis[str(rate)] = {
                    'mean_firing_rate': group['firing_rate'].mean(),
                    'mean_total_spikes': group['total_spikes'].mean(),
                    'success_rate': group['success'].mean(),
                    'mean_duration': group['duration'].mean()
                }
            
            analysis['learning_rate_analysis'] = rate_analysis
        
        return analysis
    
    def create_performance_heatmaps(self, output_dir: str = "analysis_plots"):
        """Create performance heatmaps for parameter optimization."""
        if self.results_df is None:
            print("No results data available")
            return
        
        # Create output directory
        Path(output_dir).mkdir(exist_ok=True)
        
        # Neuron count vs synapse density heatmap
        if 'neuron_count' in self.results_df.columns and 'synapse_density' in self.results_df.columns:
            pivot = self.results_df.pivot_table(
                index='neuron_count',
                columns='synapse_density',
                values='firing_rate',
                aggfunc='mean'
            )
            
            plt.figure(figsize=(12, 8))
            sns.heatmap(pivot, annot=True, fmt=".3f", cmap='RdYlBu_r')
            plt.title('Firing Rate Heatmap: Neuron Count vs Synapse Density')
            plt.tight_layout()
            plt.savefig(f"{output_dir}/neuron_synapse_heatmap.png")
            plt.close()
        
        # Success rate heatmap
        if 'neuron_count' in self.results_df.columns and 'synapse_density' in self.results_df.columns:
            success_pivot = self.results_df.pivot_table(
                index='neuron_count',
                columns='synapse_density',
                values='success',
                aggfunc='mean'
            )
            
            plt.figure(figsize=(12, 8))
            sns.heatmap(success_pivot, annot=True, fmt=".2f", cmap='RdYlGn')
            plt.title('Success Rate Heatmap: Neuron Count vs Synapse Density')
            plt.tight_layout()
            plt.savefig(f"{output_dir}/success_heatmap.png")
            plt.close()
        
        # Performance metrics comparison
        if 'learning_rate' in self.results_df.columns:
            fig, axes = plt.subplots(2, 2, figsize=(15, 12))
            
            # Firing rate vs learning rate
            sns.scatterplot(data=self.results_df, x='learning_rate', y='firing_rate',
                           hue='success', style='neuron_count', ax=axes[0, 0])
            axes[0, 0].set_title('Firing Rate vs Learning Rate')
            axes[0, 0].set_xlabel('Learning Rate')
            axes[0, 0].set_ylabel('Firing Rate')
            
            # Total spikes vs learning rate
            sns.scatterplot(data=self.results_df, x='learning_rate', y='total_spikes',
                           hue='success', style='neuron_count', ax=axes[0, 1])
            axes[0, 1].set_title('Total Spikes vs Learning Rate')
            axes[0, 1].set_xlabel('Learning Rate')
            axes[0, 1].set_ylabel('Total Spikes')
            
            # Duration vs neuron count
            sns.scatterplot(data=self.results_df, x='neuron_count', y='duration',
                           hue='success', style='synapse_density', ax=axes[1, 0])
            axes[1, 0].set_title('Duration vs Neuron Count')
            axes[1, 0].set_xlabel('Neuron Count')
            axes[1, 0].set_ylabel('Duration (seconds)')
            
            # Firing rate vs duration
            sns.scatterplot(data=self.results_df, x='firing_rate', y='duration',
                           hue='success', style='synapse_density', ax=axes[1, 1])
            axes[1, 1].set_title('Duration vs Firing Rate')
            axes[1, 1].set_xlabel('Firing Rate')
            axes[1, 1].set_ylabel('Duration (seconds)')
            
            plt.tight_layout()
            plt.savefig(f"{output_dir}/performance_scatters.png")
            plt.close()
        
        print(f"Analysis plots saved to '{output_dir}'")
    
    def generate_optimization_recommendations(self) -> List[str]:
        """Generate recommendations for parameter optimization."""
        if self.results_df is None:
            return []
        
        recommendations = []
        
        # Find optimal parameter ranges
        for param_name in ['neuron_count', 'synapse_density', 'learning_rate']:
            if param_name not in self.results_df.columns:
                continue
            
            # Group by parameter
            groups = self.results_df.groupby(param_name)
            
            # Find parameter values with highest success rate
            best_param = None
            best_success_rate = 0
            
            for param_value, group in groups:
                success_rate = group['success'].mean()
                if success_rate > best_success_rate:
                    best_success_rate = success_rate
                    best_param = param_value
            
            if best_param is not None and best_success_rate > 0.8:
                recommendations.append(
                    f"Use {param_name} = {best_param} for high success rate ({best_success_rate:.1%})"
                )
        
        # Analyze trade-offs
        if len(self.results_df) >= 10:
            # Find parameter combinations that give good performance
            good_performers = self.results_df[
                (self.results_df['success'] == True) & 
                (self.results_df['firing_rate'] > self.results_df['firing_rate'].quantile(0.75))
            ]
            
            if not good_performers.empty:
                avg_good_params = {
                    'neuron_count': good_performers['neuron_count'].mean(),
                    'synapse_density': good_performers['synapse_density'].mean(),
                    'learning_rate': good_performers['learning_rate'].mean()
                }
                
                recommendations.append(
                    f"Recommended parameter combination: {avg_good_params}"
                )
        
        return recommendations
```

## Integration with Experiment Management

### 1. Experiment Tracking Integration

```python
class ExperimentTracker:
    """Track experiments across different systems."""
    
    def __init__(self, tracking_system: str = "mlflow"):
        self.tracking_system = tracking_system
        self.experiment_log = []
    
    def log_experiment(self, experiment_name: str, parameters: Dict,
                      metrics: Dict, status: str = "completed"):
        """Log experiment results to tracking system."""
        experiment_record = {
            'timestamp': time.time(),
            'name': experiment_name,
            'parameters': parameters,
            'metrics': metrics,
            'status': status,
            'system': self.tracking_system
        }
        
        self.experiment_log.append(experiment_record)
        
        # Log to external system
        if self.tracking_system == "mlflow":
            self._log_to_mlflow(experiment_record)
        elif self.tracking_system == "wandb":
            self._log_to_wandb(experiment_record)
        elif self.tracking_system == "comet":
            self._log_to_comet(experiment_record)
    
    def _log_to_mlflow(self, experiment_record: Dict):
        """Log to MLflow."""
        try:
            import mlflow
            
            with mlflow.start_run():
                mlflow.log_params(experiment_record['parameters'])
                mlflow.log_metrics(experiment_record['metrics'])
                mlflow.set_tag("experiment_name", experiment_record['name'])
                mlflow.set_tag("status", experiment_record['status'])
                
        except ImportError:
            print("MLflow not available - skipping experiment logging")
    
    def _log_to_wandb(self, experiment_record: Dict):
        """Log to Weights & Biases."""
        try:
            import wandb
            
            with wandb.init(project="nlm_experiments") as run:
                wandb.config.update(experiment_record['parameters'])
                wandb.log(experiment_record['metrics'])
                wandb.run.name = experiment_record['name']
                wandb.run.tags = [experiment_record['status']]
                
        except ImportError:
            print("Weights & Biases not available - skipping experiment logging")
    
    def _log_to_comet(self, experiment_record: Dict):
        """Log to Comet ML."""
        try:
            from comet_ml import Experiment
            
            experiment = Experiment()
            experiment.set_name(experiment_record['name'])
            experiment.log_parameters(experiment_record['parameters'])
            experiment.log_metrics(experiment_record['metrics'])
            experiment.set_status(experiment_record['status'])
            
        except ImportError:
            print("Comet ML not available - skipping experiment logging")
    
    def export_experiment_log(self, output_file: str):
        """Export experiment log to file."""
        with open(output_file, 'w') as f:
            json.dump(self.experiment_log, f, indent=2, default=str)
        
        print(f"Experiment log exported to {output_file}")
```

### 2. Results Comparison and Analysis

```python
class ResultsComparator:
    """Compare results across different experiments and configurations."""
    
    def __init__(self):
        self.experiments = {}
    
    def add_experiment(self, experiment_name: str, batch_results: Dict):
        """Add experiment results for comparison."""
        self.experiments[experiment_name] = {
            'batch_results': batch_results,
            'aggregated_metrics': self._aggregate_experiment_results(batch_results)
        }
    
    def _aggregate_experiment_results(self, batch_results: Dict) -> Dict:
        """Aggregate results from a single experiment."""
        aggregated = {
            'total_batches': len(batch_results),
            'successful_batches': len([r for r in batch_results.values() if r.success]),
            'failed_batches': len([r for r in batch_results.values() if not r.success]),
            'average_duration': 0.0,
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'parameter_distributions': {}
        }
        
        # Calculate aggregates
        durations = [r.duration for r in batch_results.values()]
        spikes = [r.metrics.get('total_spikes', 0) for r in batch_results.values()]
        firing_rates = [r.metrics.get('average_firing_rate', 0.0) for r in batch_results.values()]
        
        aggregated['average_duration'] = sum(durations) / len(durations) if durations else 0
        aggregated['total_spikes'] = sum(spikes)
        aggregated['average_firing_rate'] = sum(firing_rates) / len(firing_rates) if firing_rates else 0
        
        # Collect parameter distributions
        for batch_id, result in batch_results.items():
            if 'parameters' in result and 'parameter_sets' in result['parameters']:
                for param_set in result['parameters']['parameter_sets']:
                    for key, value in param_set.items():
                        if key not in aggregated['parameter_distributions']:
                            aggregated['parameter_distributions'][key] = []
                        aggregated['parameter_distributions'][key].append(value)
        
        return aggregated
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict:
        """Compare multiple experiments."""
        if len(experiment_names) < 2:
            return {}
        
        comparison = {}
        
        # Get metrics for each experiment
        experiment_metrics = {}
        for exp_name in experiment_names:
            if exp_name in self.experiments:
                metrics = self.experiments[exp_name]['aggregated_metrics']
                experiment_metrics[exp_name] = metrics
        
        if len(experiment_metrics) < 2:
            return {}
        
        # Compare key metrics
        metrics_to_compare = [
            'total_spikes', 'average_firing_rate', 'average_duration',
            'successful_batches', 'failed_batches'
        ]
        
        for metric in metrics_to_compare:
            values = {}
            for exp_name, metrics in experiment_metrics.items():
                values[exp_name] = metrics.get(metric, 0)
            
            comparison[f'{metric}_comparison'] = values
        
        # Calculate performance rankings
        performance_ranking = self._calculate_performance_ranking(experiment_metrics)
        comparison['performance_ranking'] = performance_ranking
        
        # Identify best performing configurations
        best_configurations = self._identify_best_configurations(experiment_metrics)
        comparison['best_configurations'] = best_configurations
        
        return comparison
    
    def _calculate_performance_ranking(self, experiment_metrics: Dict) -> List[str]:
        """Calculate performance ranking of experiments."""
        # Simple ranking based on total spikes (higher is better)
        performance_scores = {}
        
        for exp_name, metrics in experiment_metrics.items():
            # Calculate composite score
            score = (
                metrics.get('total_spikes', 0) * 0.4 +
                metrics.get('average_firing_rate', 0) * 0.3 +
                (metrics.get('successful_batches', 0) - metrics.get('failed_batches', 0)) * 0.3
            )
            performance_scores[exp_name] = score
        
        # Sort by performance
        sorted_experiments = sorted(performance_scores.items(), key=lambda x: x[1], reverse=True)
        
        return [exp_name for exp_name, score in sorted_experiments]
    
    def _identify_best_configurations(self, experiment_metrics: Dict) -> Dict:
        """Identify best performing parameter configurations."""
        best_configs = {}
        
        for exp_name, metrics in experiment_metrics.items():
            # Find parameter combinations that gave best results
            param_distributions = metrics.get('parameter_distributions', {})
            
            if param_distributions:
                # Calculate parameter effectiveness
                param_effectiveness = {}
                
                for param_name, values in param_distributions.items():
                    # Find value with best performance
                    # This would require more detailed analysis in practice
                    param_effectiveness[param_name] = {
                        'most_effective_value': max(set(values), key=values.count),
                        'frequency': values.count(max(set(values), key=values.count))
                    }
                
                best_configs[exp_name] = param_effectiveness
        
        return best_configs
    
    def generate_comparison_report(self, comparison: Dict) -> str:
        """Generate comparison report."""
        report = "=== EXPERIMENT COMPARISON REPORT ===\n\n"
        
        if 'performance_ranking' in comparison:
            report += "Performance Ranking:\n"
            for i, exp_name in enumerate(comparison['performance_ranking'], 1):
                report += f"  {i}. {exp_name}\n"
            report += "\n"
        
        if 'best_configurations' in comparison:
            report += "Best Performing Configurations:\n"
            for exp_name, configs in comparison['best_configurations'].items():
                report += f"\n  {exp_name}:\n"
                for param_name, effectiveness in configs.items():
                    report += f"    {param_name}: {effectiveness['most_effective_value']} "
                    report += f"(frequency: {effectiveness['frequency']})\n"
        
        return report
```

## Conclusion

The enhanced NLM batch processing and automation system provides:

### Core Capabilities

1. **Flexible Batch Processing**: Sequential, parallel, and pipeline processing modes
2. **Parameter Sweeps**: Systematic exploration of parameter combinations
3. **Workflow Management**: Structured workflows for complex experiment designs
4. **Results Aggregation**: Comprehensive analysis and aggregation of batch results
5. **Automation**: Automated experiment design and execution
6. **Integration**: Seamless integration with experiment tracking systems
7. **Analysis Tools**: Advanced results analysis and visualization
8. **Optimization**: Parameter optimization and recommendation generation

### Key Benefits

- **Efficiency**: Parallel execution for faster experimentation
- **Reproducibility**: Structured workflows ensure consistent results
- **Scalability**: Handle large numbers of parameter combinations
- **Analysis**: Comprehensive analysis of parameter effects
- **Automation**: Reduce manual effort in experiment management
- **Integration**: Connect with external experiment tracking systems
- **Visualization**: Intuitive visualizations of experiment results
- **Optimization**: Data-driven recommendations for parameter optimization

### Usage Examples

```python
# Simple batch processing
batch_processor = NLMBatchProcessor(config)
batch = Batch("exp_1", "Learning Efficiency Test")
batch.add_parameter_set({'steps': 100, 'neuron_count': 500})
batch.add_parameter_set({'steps': 200, 'neuron_count': 1000})
processor.add_batch(batch)
results = processor.run_all_batches()

# Parameter sweep
parameter_sweep = ParameterSweep()
parameter_sweep.add_sweep(
    name="hyperparameter_search",
    parameters={'learning_rate': [0.001, 0.01, 0.1], 'neuron_count': [500, 1000]}
)

# Automated experiment design
experiment_designer = AutomatedExperimentDesigner()
experiments = experiment_designer.design_experiments(search_space, num_experiments=20)

# Results analysis
analyzer = BatchResultsAnalyzer()
analyzer.load_results(results)
comparison = analyzer.analyze_performance_tradeoffs()
analyzer.create_performance_heatmaps("analysis_plots")
```

These tools enable systematic, efficient, and reproducible NLM experimentation with comprehensive analysis and automation capabilities. 🚀