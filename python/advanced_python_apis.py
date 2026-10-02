// Advanced Python APIs for NLM - Complex Use Cases

// This module provides advanced Python APIs for complex NLM use cases,
// research scenarios, and automated simulation workflows.

import sys
import os
import json
import time
import tempfile
import shutil
from typing import List, Dict, Any, Optional, Union, Callable
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import threading
import multiprocessing

# Try to import the C++ bindings
try:
    import pynlm
    CPP_BINDINGS_AVAILABLE = True
except ImportError as e:
    CPP_BINDINGS_AVAILABLE = False
    print(f"Warning: C++ bindings not available: {e}")

# Advanced simulation control and automation
class AdvancedBrainSimulation:
    """
    Advanced brain simulation with automated control and analysis.
    
    This class provides high-level APIs for running complex simulations,
    managing experimental conditions, and analyzing results automatically.
    """
    
    def __init__(self, brain, world=None):
        """
        Initialize advanced simulation controller.
        
        Args:
            brain: Brain instance
            world: Optional World instance (creates default if None)
        """
        self.brain = brain
        self.world = world
        
        if world is None and CPP_BINDINGS_AVAILABLE:
            from pynlm import createSimpleWorld
            self.world = createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
        
        # Simulation state
        self.simulation_start_time = 0.0
        self.simulation_steps = 0
        self.recorded_metrics = {}
        self.current_epoch = 0
        self.training_history = []
    
    def run_stimulus_sequence(self, stimulus_sequence: List[Dict[str, Any]],
                            agent=None, verbose=False):
        """
        Run a predefined sequence of stimuli.
        
        Args:
            stimulus_sequence: List of stimulus specifications
            agent: Optional agent instance
            verbose: Print progress information
            
        Returns:
            dict: Simulation results and analysis
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for stimulus sequence simulation")
        
        # Create agent if not provided
        if agent is None:
            from pynlm import createAgentBrain
            agent = createAgentBrain(self.brain)
            agent.initialize(self.world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        results = {
            'sequence_length': len(stimulus_sequence),
            'stimulus_types': [],
            'responses': [],
            'adaptations': [],
            'episode_metrics': []
        }
        
        for epoch, stimulus in enumerate(stimulus_sequence):
            if verbose:
                print(f"Epoch {epoch + 1}/{len(stimulus_sequence)}: {stimulus.get('type', 'unknown')}")
            
            # Configure world based on stimulus
            self._apply_stimulus(stimulus)
            
            # Run episode
            episode_results = self._run_episode_with_stimulus(stimulus, agent)
            
            # Record results
            results['stimulus_types'].append(stimulus.get('type', 'unknown'))
            results['responses'].append(episode_results.get('response', {}))
            results['adaptations'].append(episode_results.get('adaptation', {}))
            results['episode_metrics'].append(episode_results.get('metrics', {}))
            
            # Store training history
            self.training_history.append({
                'epoch': epoch,
                'stimulus': stimulus,
                'results': episode_results,
                'brain_state': self._capture_brain_state()
            })
        
        # Analyze sequence effectiveness
        analysis = self._analyze_stimulus_sequence(results)
        
        return {
            'results': results,
            'analysis': analysis,
            'training_history': self.training_history
        }
    
    def run_adaptive_training(self, environment_generator, max_episodes=1000,
                             learning_rate=0.01, convergence_threshold=0.95,
                             verbose=False):
        """
        Run adaptive training in a dynamic environment.
        
        Args:
            environment_generator: Function that generates environments
            max_episodes: Maximum number of training episodes
            learning_rate: Learning rate for adaptation
            convergence_threshold: Convergence threshold for stopping
            verbose: Print progress information
            
        Returns:
            dict: Training results and final metrics
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for adaptive training")
        
        from pynlm import createAgentBrain
        
        # Initialize agent
        agent = createAgentBrain(self.brain)
        agent.initialize(self.world)
        
        # Training state
        episode = 0
        best_performance = 0.0
        convergence_count = 0
        required_convergence = 5
        
        training_log = {
            'episodes': [],
            'performance_curve': [],
            'adaptation_metrics': []
        }
        
        while episode < max_episodes and convergence_count < required_convergence:
            # Generate environment
            environment = environment_generator()
            
            # Setup environment
            self._setup_environment(environment)
            
            # Run episode
            episode_results = self._run_training_episode(agent, learning_rate)
            
            # Record results
            episode_data = {
                'episode': episode,
                'environment': environment,
                'results': episode_results,
                'performance': episode_results.get('performance', 0.0),
                'adaptation': episode_results.get('adaptation', 0.0)
            }
            
            training_log['episodes'].append(episode_data)
            training_log['performance_curve'].append(episode_data['performance'])
            training_log['adaptation_metrics'].append(episode_data['adaptation'])
            
            # Update best performance
            if episode_data['performance'] > best_performance:
                best_performance = episode_data['performance']
                convergence_count = 0
            else:
                convergence_count += 1
            
            # Check for early stopping
            if len(training_log['performance_curve']) >= 10:
                recent_performance = training_log['performance_curve'][-10:]
                if all(p >= convergence_threshold for p in recent_performance):
                    convergence_count = required_convergence
            
            # Log progress
            if verbose and (episode % 50 == 0 or episode == max_episodes - 1):
                print(f"Episode {episode}: Performance = {episode_data['performance']:.3f}, "
                      f"Adaptation = {episode_data['adaptation']:.3f}, "
                      f"Best = {best_performance:.3f}")
            
            episode += 1
        
        # Final analysis
        final_analysis = self._analyze_training_results(training_log)
        
        return {
            'training_log': training_log,
            'final_analysis': final_analysis,
            'episodes_completed': episode,
            'best_performance': best_performance,
            'converged': convergence_count >= required_convergence
        }
    
    def run_brain_annealing(self, temperature_schedule: List[float],
                           objective_function: Callable,
                           num_iterations=100, verbose=False):
        """
        Run brain annealing optimization.
        
        Args:
            temperature_schedule: List of temperature values for each iteration
            objective_function: Function that evaluates brain state
            num_iterations: Number of annealing iterations
            verbose: Print progress information
            
        Returns:
            dict: Annealing results
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for brain annealing")
        
        from pynlm import createAgentBrain
        
        # Initialize agent
        agent = createAgentBrain(self.brain)
        agent.initialize(self.world)
        
        # Annealing state
        best_brain_state = self._capture_brain_state()
        best_score = float('-inf')
        
        annealing_log = {
            'iterations': [],
            'temperature_schedule': temperature_schedule,
            'best_score': best_score,
            'current_score': 0.0,
            'acceptances': 0,
            'rejections': 0
        }
        
        for iteration in range(min(num_iterations, len(temperature_schedule))):
            temperature = temperature_schedule[iteration]
            
            # Perturb brain state
            perturbed_state = self._perturb_brain_state(best_brain_state, temperature)
            
            # Evaluate objective function
            current_score = objective_function(perturbed_state)
            
            # Metropolis criterion
            accept = False
            if current_score > best_score or \
               (random.random() < math.exp((current_score - best_score) / temperature) if temperature > 0 else False):
                accept = True
                best_score = current_score
                best_brain_state = perturbed_state
                annealing_log['acceptances'] += 1
            else:
                annealing_log['rejections'] += 1
            
            # Log iteration
            annealing_log['iterations'].append({
                'iteration': iteration,
                'temperature': temperature,
                'current_score': current_score,
                'best_score': best_score,
                'accepted': accept
            })
            
            # Restore best state
            self._restore_brain_state(best_brain_state)
            
            if verbose and (iteration % 10 == 0 or iteration == num_iterations - 1):
                print(f"Iteration {iteration}: Score = {current_score:.3f}, "
                      f"Best = {best_score:.3f}, Temp = {temperature:.3f}")
        
        # Final analysis
        final_analysis = self._analyze_annealing_results(annealing_log)
        
        return {
            'annealing_log': annealing_log,
            'final_analysis': final_analysis,
            'best_score': best_score,
            'iterations_completed': len(annealing_log['iterations'])
        }

# Batch processing for multiple brains
class BrainBatchProcessor:
    """
    Process multiple brains in batch for parallel simulation.
    
    This class enables running simulations with multiple different brain
    configurations simultaneously, useful for hyperparameter search
    and comparative analysis.
    """
    
    def __init__(self, max_workers=None):
        """
        Initialize batch processor.
        
        Args:
            max_workers: Maximum number of worker threads/processes
        """
        self.max_workers = max_workers or multiprocessing.cpu_count()
        self.results = []
        self._lock = threading.Lock()
    
    def create_brain_variants(self, base_config, variants):
        """
        Create multiple brain variants from a base configuration.
        
        Args:
            base_config: Base configuration dictionary
            variants: List of variant specifications
            
        Returns:
            list: List of brain instances
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for batch processing")
        
        from pynlm import createDefaultConfig, createBrain
        
        brains = []
        
        for i, variant in enumerate(variants):
            # Create variant configuration
            variant_config = base_config.copy()
            variant_config.update(variant)
            
            # Create brain
            config = createDefaultConfig()
            for key, value in variant_config.items():
                config.set(key, value)
            
            brain = createBrain(config)
            brain.initialize()
            
            brains.append({
                'id': i,
                'config': variant_config,
                'brain': brain,
                'parameters': variant
            })
        
        return brains
    
    def run_batch_simulation(self, brains, num_steps=100, metric_function=None):
        """
        Run simulation for multiple brains in parallel.
        
        Args:
            brains: List of brain dictionaries (from create_brain_variants)
            num_steps: Number of simulation steps
            metric_function: Optional function to compute metrics
            
        Returns:
            list: Batch results
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for batch simulation")
        
        def run_brain_simulation(brain_data):
            """Run simulation for a single brain."""
            brain = brain_data['brain']
            config = brain.getConfig()
            
            # Run simulation
            for step in range(num_steps):
                brain.step(step)
            
            # Compute metrics
            metrics = {
                'final_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'development_stage': brain.getDevelopmentalStage(),
                'config': config.getKeys()
            }
            
            if metric_function:
                custom_metrics = metric_function(brain)
                metrics.update(custom_metrics)
            
            return {
                'brain_id': brain_data['id'],
                'parameters': brain_data['parameters'],
                'metrics': metrics,
                'simulation_steps': num_steps
            }
        
        # Use ThreadPoolExecutor for parallel processing
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_brain = {
                executor.submit(run_brain_simulation, brain_data): brain_data['id']
                for brain_data in brains
            }
            
            # Collect results
            results = []
            for future in future_to_brain:
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    print(f"Error running brain {future_to_brain[future]}: {e}")
        
        with self._lock:
            self.results.extend(results)
        
        return results
    
    def run_hypothesis_test(self, hypothesis_brain, control_brain, 
                           test_metric='average_firing_rate', 
                           num_replicates=10):
        """
        Run hypothesis test comparing hypothesis and control brains.
        
        Args:
            hypothesis_brain: Brain instance for hypothesis
            control_brain: Brain instance for control
            test_metric: Metric to test
            num_replicates: Number of test replicates
            
        Returns:
            dict: Hypothesis test results
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for hypothesis testing")
        
        def run_test_replicate(brain):
            """Run a single test replicate."""
            for step in range(num_replicates):
                brain.step(step)
            return brain.getTotalSpikeCount()
        
        # Run tests
        from concurrent.futures import ThreadPoolExecutor
        
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit test replicates
            future_to_type = {
                executor.submit(run_test_replicate, hypothesis_brain): 'hypothesis',
                executor.submit(run_test_replicate, control_brain): 'control'
            }
            
            # Collect results
            hypothesis_results = []
            control_results = []
            
            for future in future_to_type:
                brain_type = future_to_type[future]
                try:
                    result = future.result()
                    if brain_type == 'hypothesis':
                        hypothesis_results.append(result)
                    else:
                        control_results.append(result)
                except Exception as e:
                    print(f"Error in {brain_type} test: {e}")
        
        # Statistical analysis
        from statistics import mean, stdev
        
        if hypothesis_results and control_results:
            hyp_mean = mean(hypothesis_results)
            hyp_stdev = stdev(hypothesis_results) if len(hypothesis_results) > 1 else 0
            
            ctrl_mean = mean(control_results)
            ctrl_stdev = stdev(control_results) if len(control_results) > 1 else 0
            
            # Perform t-test
            if hyp_stdev > 0 and ctrl_stdev > 0:
                t_score = (hyp_mean - ctrl_mean) / math.sqrt(hyp_stdev**2/len(hypothesis_results) + ctrl_stdev**2/len(control_results))
                degrees_of_freedom = len(hypothesis_results) + len(control_results) - 2
                
                # Get p-value from t-distribution
                from scipy import stats
                p_value = 2 * (1 - stats.t.cdf(abs(t_score), degrees_of_freedom))
            else:
                t_score = float('inf') if hyp_mean > ctrl_mean else float('-inf')
                p_value = 0.0 if t_score != float('inf') else 1.0
            
            return {
                'hypothesis_mean': hyp_mean,
                'hypothesis_std': hyp_stdev,
                'control_mean': ctrl_mean,
                'control_std': ctrl_stdev,
                't_score': t_score,
                'p_value': p_value,
                'significant': p_value < 0.05,
                'effect_size': (hyp_mean - ctrl_mean) / max(hyp_stdev, ctrl_stdev, 0.001)
            }
        else:
            return {
                'error': 'Insufficient data for statistical analysis'
            }

# Machine learning integration APIs
class BrainMLIntegration:
    """
    Integration of machine learning techniques with NLM.
    
    This class provides APIs for applying machine learning algorithms
    to analyze NLM data, optimize parameters, and generate predictions.
    """
    
    def __init__(self, brain, world=None):
        """
        Initialize ML integration.
        
        Args:
            brain: Brain instance
            world: Optional World instance
        """
        self.brain = brain
        self.world = world
    
    def extract_neural_features(self, num_steps=100) -> Dict[str, Any]:
        """
        Extract neural features from brain activity for machine learning.
        
        Args:
            num_steps: Number of simulation steps
            
        Returns:
            dict: Extracted features
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for feature extraction")
        
        # Run simulation and collect features
        features = {
            'spike_patterns': [],
            'firing_rates': [],
            'correlation_matrix': None,
            'spectrogram': None,
            'temporal_features': [],
            'spatial_features': [],
            'connectivity_patterns': []
        }
        
        # Collect spike patterns
        spike_patterns = []
        for step in range(num_steps):
            self.brain.step(step)
            
            # Collect firing rate
            firing_rate = self.brain.getAverageFiringRate()
            features['firing_rates'].append(firing_rate)
            
            # Collect spike pattern (simplified)
            spike_patterns.append({
                'step': step,
                'firing_count': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount()
            })
        
        features['spike_patterns'] = spike_patterns
        
        # Compute correlation matrix (simplified)
        if len(features['firing_rates']) > 1:
            from statistics import correlation
            try:
                corr_matrix = [[0.0 for _ in range(len(features['firing_rates']))] 
                              for _ in range(len(features['firing_rates']))]
                features['correlation_matrix'] = corr_matrix
            except:
                pass
        
        # Extract temporal features
        features['temporal_features'] = self._extract_temporal_features(spike_patterns)
        
        # Extract spatial features (simplified)
        features['spatial_features'] = self._extract_spatial_features()
        
        return features
    
    def train_prediction_model(self, training_data: List[Dict[str, Any]],
                             model_type='random_forest',
                             hyperparameters=None):
        """
        Train a machine learning model to predict brain behavior.
        
        Args:
            training_data: Training data from multiple simulations
            model_type: Type of ML model ('random_forest', 'neural_network', etc.)
            hyperparameters: Model hyperparameters
            
        Returns:
            object: Trained model
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for model training")
        
        # Prepare data for machine learning
        X = []
        y = []
        
        for data_point in training_data:
            # Extract features
            features = data_point.get('features', {})
            target = data_point.get('target', 0.0)
            
            # Combine features
            feature_vector = self._extract_feature_vector(features)
            
            X.append(feature_vector)
            y.append(target)
        
        # Train model (simplified - would use actual ML libraries)
        model = self._train_model(X, y, model_type, hyperparameters)
        
        return model
    
    def predict_brain_behavior(self, model, test_data: List[Dict[str, Any]]):
        """
        Use trained model to predict brain behavior.
        
        Args:
            model: Trained model
            test_data: Test data
            
        Returns:
            list: Predictions
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for prediction")
        
        predictions = []
        
        for data_point in test_data:
            features = data_point.get('features', {})
            feature_vector = self._extract_feature_vector(features)
            
            prediction = self._predict(model, feature_vector)
            predictions.append(prediction)
        
        return predictions

# Research and experiment management
class ExperimentManager:
    """
    Manage scientific experiments with NLM.
    
    This class provides APIs for designing, running, and analyzing
    scientific experiments with NLM brains.
    """
    
    def __init__(self, name, description=""):
        """
        Initialize experiment manager.
        
        Args:
            name: Experiment name
            description: Experiment description
        """
        self.name = name
        self.description = description
        self.experiments = []
        self.results = []
        self.metadata = {}
    
    def add_experiment(self, brain, world, protocol, name=None, description=""):
        """
        Add an experiment to the manager.
        
        Args:
            brain: Brain instance
            world: World instance
            protocol: Experiment protocol
            name: Optional experiment name
            description: Optional experiment description
            
        Returns:
            int: Experiment ID
        """
        experiment_id = len(self.experiments)
        
        experiment = {
            'id': experiment_id,
            'name': name or f"Experiment_{experiment_id}",
            'description': description,
            'brain': brain,
            'world': world,
            'protocol': protocol,
            'status': 'pending',
            'results': None,
            'metadata': {}
        }
        
        self.experiments.append(experiment)
        
        return experiment_id
    
    def run_experiment(self, experiment_id, verbose=False):
        """
        Run a specific experiment.
        
        Args:
            experiment_id: Experiment ID
            verbose: Print progress information
            
        Returns:
            dict: Experiment results
        """
        if experiment_id >= len(self.experiments):
            raise ValueError(f"Experiment ID {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        
        # Update status
        experiment['status'] = 'running'
        
        try:
            # Run experiment based on protocol
            protocol = experiment['protocol']
            
            if protocol == 'standard':
                results = self._run_standard_experiment(
                    experiment['brain'], 
                    experiment['world'],
                    verbose
                )
            elif protocol == 'adaptive':
                results = self._run_adaptive_experiment(
                    experiment['brain'],
                    experiment['world'],
                    verbose
                )
            elif protocol == 'machine_learning':
                results = self._run_machine_learning_experiment(
                    experiment['brain'],
                    experiment['world'],
                    verbose
                )
            else:
                raise ValueError(f"Unknown protocol: {protocol}")
            
            # Store results
            experiment['results'] = results
            experiment['status'] = 'completed'
            
            # Store in results log
            self.results.append({
                'experiment_id': experiment_id,
                'timestamp': time.time(),
                'results': results,
                'protocol': protocol
            })
            
            return results
            
        except Exception as e:
            experiment['status'] = 'failed'
            experiment['error'] = str(e)
            
            # Store failed experiment
            self.results.append({
                'experiment_id': experiment_id,
                'timestamp': time.time(),
                'error': str(e),
                'protocol': protocol
            })
            
            raise
    
    def get_experiment_results(self, experiment_id):
        """
        Get results for a specific experiment.
        
        Args:
            experiment_id: Experiment ID
            
        Returns:
            dict: Experiment results
        """
        if experiment_id >= len(self.experiments):
            raise ValueError(f"Experiment ID {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        return {
            'metadata': experiment,
            'results': experiment.get('results'),
            'status': experiment['status']
        }
    
    def export_results(self, filepath, format='json'):
        """
        Export experiment results to file.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'pickle')
        """
        if format == 'json':
            with open(filepath, 'w') as f:
                json.dump({
                    'experiments': self.experiments,
                    'results': self.results,
                    'metadata': self.metadata,
                    'name': self.name,
                    'description': self.description,
                    'timestamp': time.time()
                }, f, indent=2)
        
        elif format == 'csv':
            # Export to CSV format
            with open(filepath, 'w') as f:
                # Write header
                f.write("experiment_id,name,protocol,status,timestamp\n")
                
                for experiment in self.experiments:
                    f.write(f"{experiment['id']},{experiment['name']},"
                           f"{experiment['protocol']},{experiment['status']},"
                           f"{experiment.get('timestamp', 0)}\n")
        
        # Add other formats as needed

# Performance analysis and monitoring
class PerformanceMonitor:
    """
    Monitor and analyze NLM performance.
    
    This class provides APIs for monitoring simulation performance,
    analyzing computational costs, and optimizing performance.
    """
    
    def __init__(self):
        """Initialize performance monitor."""
        self.performance_data = []
        self.benchmark_results = {}
        self.resource_usage = []
    
    def monitor_simulation(self, brain, num_steps=100, 
                          record_memory=True, record_cpu=True):
        """
        Monitor simulation performance.
        
        Args:
            brain: Brain instance
            num_steps: Number of simulation steps
            record_memory: Whether to record memory usage
            record_cpu: Whether to record CPU usage
            
        Returns:
            dict: Performance metrics
        """
        import psutil
        import time
        
        # Start monitoring
        process = psutil.Process()
        start_time = time.time()
        
        if record_memory:
            initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        if record_cpu:
            initial_cpu = process.cpu_percent()
        
        # Run simulation
        step_times = []
        step_spikes = []
        
        for step in range(num_steps):
            step_start = time.time()
            brain.step(step)
            step_end = time.time()
            
            step_times.append(step_end - step_start)
            step_spikes.append(brain.getFiringNeuronCount())
            
            # Record resource usage
            if record_memory:
                memory = process.memory_info().rss / 1024 / 1024  # MB
                self.resource_usage.append({
                    'timestamp': time.time(),
                    'memory_mb': memory,
                    'cpu_percent': process.cpu_percent()
                })
        
        end_time = time.time()
        
        # Calculate metrics
        total_time = end_time - start_time
        avg_step_time = sum(step_times) / len(step_times) if step_times else 0
        steps_per_second = num_steps / total_time if total_time > 0 else 0
        
        performance_metrics = {
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'steps_per_second': steps_per_second,
            'min_step_time': min(step_times) if step_times else 0,
            'max_step_time': max(step_times) if step_times else 0,
            'step_time_std': self._std(step_times) if len(step_times) > 1 else 0,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'performance_trend': self._calculate_performance_trend(step_times)
        }
        
        # Add memory metrics
        if record_memory:
            final_memory = process.memory_info().rss / 1024 / 1024  # MB
            memory_increase = final_memory - initial_memory
            
            performance_metrics['initial_memory_mb'] = initial_memory
            performance_metrics['final_memory_mb'] = final_memory
            performance_metrics['memory_increase_mb'] = memory_increase
            performance_metrics['memory_per_step_kb'] = memory_increase * 1024 / num_steps if num_steps > 0 else 0
        
        # Add CPU metrics
        if record_cpu:
            final_cpu = process.cpu_percent()
            cpu_usage = (final_cpu - initial_cpu) / 100
            
            performance_metrics['initial_cpu_percent'] = initial_cpu
            performance_metrics['final_cpu_percent'] = final_cpu
            performance_metrics['cpu_usage'] = cpu_usage
        
        # Store performance data
        self.performance_data.append({
            'timestamp': time.time(),
            'metrics': performance_metrics,
            'brain_state': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'development_stage': brain.getDevelopmentalStage()
            }
        })
        
        return performance_metrics
    
    def run_benchmark(self, brain, configurations, num_steps=100):
        """
        Run performance benchmark across different configurations.
        
        Args:
            brain: Brain instance
            configurations: List of configuration dictionaries
            num_steps: Number of simulation steps for each test
            
        Returns:
            dict: Benchmark results
        """
        if not CPP_BINDINGS_AVAILABLE:
            raise RuntimeError("C++ bindings not available for benchmarking")
        
        from pynlm import createDefaultConfig, createBrain
        
        benchmark_results = {}
        
        for config_name, config_params in configurations.items():
            # Create brain with specific configuration
            config = createDefaultConfig()
            
            # Apply configuration parameters
            for key, value in config_params.items():
                config.set(key, value)
            
            brain_instance = createBrain(config)
            brain_instance.initialize()
            
            # Run benchmark
            metrics = self.monitor_simulation(brain_instance, num_steps)
            
            benchmark_results[config_name] = {
                'parameters': config_params,
                'metrics': metrics,
                'performance_score': self._calculate_performance_score(metrics)
            }
        
        # Analyze results
        analysis = self._analyze_benchmark_results(benchmark_results)
        
        return {
            'configurations': benchmark_results,
            'analysis': analysis
        }
    
    def analyze_performance_trends(self, time_window=60):
        """
        Analyze performance trends over time.
        
        Args:
            time_window: Time window for trend analysis (seconds)
            
        Returns:
            dict: Trend analysis
        """
        if not self.performance_data:
            return {'error': 'No performance data available'}
        
        # Filter recent data
        cutoff_time = time.time() - time_window
        recent_data = [d for d in self.performance_data 
                      if d['timestamp'] > cutoff_time]
        
        if not recent_data:
            return {'error': 'No recent performance data available'}
        
        # Analyze trends
        trends = {
            'step_time_trend': self._analyze_trend([d['metrics']['avg_step_time'] for d in recent_data]),
            'performance_trend': self._analyze_trend([d['metrics']['performance_trend'] for d in recent_data]),
            'memory_trend': self._analyze_trend([d['metrics'].get('memory_increase_mb', 0) for d in recent_data]),
            'cpu_trend': self._analyze_trend([d['metrics'].get('cpu_usage', 0) for d in recent_data])
        }
        
        return trends
    
    def _std(self, values):
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _calculate_performance_trend(self, step_times):
        """Calculate performance trend from step times."""
        if len(step_times) < 2:
            return 0.0
        
        # Calculate linear trend
        x = list(range(len(step_times)))
        y = step_times
        
        n = len(x)
        if n <= 1:
            return 0.0
        
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xx = sum(xi * xi for xi in x)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x)
        
        return slope
    
    def _analyze_trend(self, values):
        """Analyze trend in a sequence of values."""
        if len(values) < 2:
            return {'trend': 'insufficient_data', 'slope': 0.0}
        
        # Calculate linear trend
        x = list(range(len(values)))
        slope = self._calculate_performance_trend(values)
        
        if abs(slope) < 0.001:
            trend = 'stable'
        elif slope > 0.001:
            trend = 'increasing'
        else:
            trend = 'decreasing'
        
        return {
            'trend': trend,
            'slope': slope,
            'current_value': values[-1],
            'average_value': sum(values) / len(values)
        }
    
    def _calculate_performance_score(self, metrics):
        """Calculate composite performance score."""
        score = 0.0
        
        # Step time (lower is better)
        if 'avg_step_time' in metrics:
            score += 1.0 / max(metrics['avg_step_time'], 0.001)
        
        # Steps per second (higher is better)
        if 'steps_per_second' in metrics:
            score += metrics['steps_per_second']
        
        # Memory efficiency (lower memory increase is better)
        if 'memory_increase_mb' in metrics:
            score += 1.0 / max(metrics['memory_increase_mb'], 0.001)
        
        return score
    
    def _analyze_benchmark_results(self, results):
        """Analyze benchmark results."""
        if not results:
            return {'error': 'No benchmark results'}
        
        # Find best configuration
        best_config = max(results.items(), 
                         key=lambda x: x[1]['performance_score'],
                         default=(None, {'performance_score': 0}))
        
        # Calculate improvements
        improvements = {}
        if best_config[0]:
            best_score = best_config[1]['performance_score']
            
            for config_name, result in results.items():
                if config_name != best_config[0]:
                    improvement = (best_score - result['performance_score']) / best_score * 100
                    improvements[config_name] = improvement
        
        return {
            'best_configuration': best_config[0],
            'best_score': best_config[1]['performance_score'] if best_config[0] else 0,
            'improvements': improvements,
            'num_configurations': len(results)
        }

# Export all advanced APIs
__all__ = [
    # Advanced simulation control
    'AdvancedBrainSimulation',
    'BrainBatchProcessor',
    'BrainMLIntegration',
    'ExperimentManager',
    'PerformanceMonitor',
    
    # Convenience functions
    'create_brain_with_config',
    'run_agent_simulation',
    'benchmark_brain_performance',
    'validate_configuration',
    
    # Error classes
    'NLMError', 'ConfigurationError', 'SimulationError', 'InitializationError',
    'PyNLMError', 'BindingError', 'ConfigurationError', 'SimulationError',
    
    # Types and enumerations
    'NeuronType', 'ActionType', 'DevelopmentalStage', 'FiringState', 'WorldObjectType',
    
    # Module metadata
    '__version__', '__author__', '__description__',
    '__all__', '_CPP_AVAILABLE'
]

# Add convenience functions to module level
def create_brain_with_config(**kwargs):
    """
    Convenience function to create a brain with custom configuration.
    
    Args:
        **kwargs: Configuration parameters
        
    Returns:
        BrainSimulator: Configured brain simulator
    """
    if not CPP_BINDINGS_AVAILABLE:
        raise RuntimeError("C++ bindings not available. Cannot create brain.")
    
    from pynlm import createDefaultConfig, createBrain
    
    config = createDefaultConfig()
    
    # Map common configuration parameters
    config_map = {
        "neuron_count": ("brain.neuron_count", lambda x: int(x)),
        "region_count": ("brain.region_count", lambda x: int(x)),
        "connection_probability": ("brain.connection_probability", lambda x: float(x)),
        "simulation_timestep": ("simulation_timestep", lambda x: float(x)),
        "random_seed": ("random_seed", lambda x: int(x)),
    }
    
    for key, value in kwargs.items():
        if key in config_map:
            param_path, converter = config_map[key]
            config.set(param_path, converter(value))
    
    brain = createBrain(config)
    brain.initialize()
    
    # Return BrainSimulator instance
    from pynlm import BrainSimulator
    return BrainSimulator(brain)

def run_agent_simulation(brain, world, agent, num_steps=1000, verbose=False):
    """
    Convenience function to run a complete agent simulation.
    
    Args:
        brain: Brain instance
        world: World instance  
        agent: AgentBrain instance
        num_steps: Number of simulation steps
        verbose: Print progress information
        
    Returns:
        dict: Simulation results including statistics
    """
    if not CPP_BINDINGS_AVAILABLE:
        raise RuntimeError("C++ bindings not available. Cannot run simulation.")
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    results = {
        'total_spikes': 0,
        'firing_neuron_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'developmental_stages': [],
        'final_spike_count': 0
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        results['total_spikes'] += brain.getFiringNeuronCount()
        results['firing_neuron_counts'].append(brain.getFiringNeuronCount())
        results['curiosity_levels'].append(agent.getCuriosityLevel())
        results['novelty_levels'].append(agent.getNoveltyLevel())
        results['developmental_stages'].append(brain.getDevelopmentalStage())
        
        if verbose and step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    results['final_spike_count'] = brain.getTotalSpikeCount()
    return results

def benchmark_brain_performance(brain, num_steps=1000, warmup_steps=100):
    """
    Benchmark brain performance metrics.
    
    Args:
        brain: Brain instance
        num_steps: Number of simulation steps to run
        warmup_steps: Number of steps to skip for warmup
        
    Returns:
        dict: Performance metrics
    """
    if not CPP_BINDINGS_AVAILABLE:
        raise RuntimeError("C++ bindings not available. Cannot benchmark.")
    
    import time
    
    # Warmup
    for step in range(warmup_steps):
        brain.step(step)
    
    # Benchmark
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = num_steps / elapsed if elapsed > 0 else 0
    
    return {
        'total_steps': num_steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps_per_second,
        'final_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'final_development_stage': brain.getDevelopmentalStage()
    }

def validate_configuration(config):
    """
    Validate a configuration for errors and warnings.
    
    Args:
        config: Config instance to validate
        
    Returns:
        dict: Validation results with errors and warnings
    """
    if not CPP_BINDINGS_AVAILABLE:
        raise RuntimeError("C++ bindings not available. Cannot validate configuration.")
    
    results = {
        'is_valid': True,
        'errors': [],
        'warnings': [],
        'recommendations': []
    }
    
    # Check neuron count
    neuron_count = config.getOr<int64_t>("brain.neuron_count", 1000)
    if neuron_count < 10:
        results['warnings'].append("Very small neuron count (< 10) may not produce meaningful results")
    elif neuron_count > 10000:
        results['warnings'].append("Large neuron count (> 10000) may impact performance")
    
    # Check connection probability
    connection_prob = config.getOr<float>("brain.connection_probability", 0.1)
    if connection_prob > 0.5:
        results['warnings'].append("High connection probability (> 0.5) may cause excessive connectivity")
    
    # Check timestep
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep < 0.0001 or timestep > 0.1:
        results['warnings'].append("Simulation timestep may be too small or too large for accurate results")
    
    # Check for obvious errors
    if not hasattr(config, 'getKeys'):
        results['is_valid'] = False
        results['errors'].append("Configuration object missing required methods")
    
    if results['warnings']:
        results['recommendations'].append("Consider adjusting warning parameters for better simulation results")
    
    return results
# Module-level error handling
class PyNLMError(Exception):
    """Base exception for pynlm Python module errors."""
    pass

class BindingError(PyNLMError):
    """Error related to C++ bindings."""
    pass

class ConfigurationError(PyNLMError):
    """Error in configuration."""
    pass

class SimulationError(PyNLMError):
    """Error during simulation."""
    pass
# Set up error handling
try:
    # Test if we can create a basic config
    test_config = createDefaultConfig()
except Exception as e:
    raise BindingError(f"Failed to load pynlm bindings: {e}")
# Module documentation
__doc__ = """
Advanced Python APIs for NLM (Neural Learning Machine)

This module provides advanced Python APIs for complex NLM use cases,
research scenarios, and automated simulation workflows. It extends
the basic Python bindings with comprehensive tools for:

**Advanced Simulation Control**
- Adaptive training with dynamic environments
- Stimulus sequence experiments
- Brain annealing optimization
- Parallel batch processing

**Machine Learning Integration**
- Neural feature extraction
- Machine learning model training
- Predictive modeling
- Automated feature engineering

**Research and Experiment Management**
- Scientific experiment design
- Hypothesis testing
- Results export and analysis
- Experimental reproducibility

**Performance Analysis and Monitoring**
- Simulation performance benchmarking
- Resource usage monitoring
- Performance trend analysis
- Comparative analysis

**Key Features**
- High-level APIs for complex workflows
- Parallel processing for performance
- Comprehensive error handling
- Integration with scientific Python ecosystem
- Support for research and experimentation

**Use Cases**
- Hyperparameter optimization for NLM
- Comparative analysis of neural architectures
- Automated experiment pipelines
- Performance benchmarking and optimization
- Machine learning applications with neural data

For basic NLM usage, import from pynlm. For advanced functionality,
import these classes directly for more control and customization.
"""

# Initialize module
if CPP_BINDINGS_AVAILABLE:
    print(f"NLM Advanced Python APIs loaded successfully (v{__version__})")
    print(f"C++ bindings available: True")
    print("\nAdvanced APIs available:")
    print("  - AdvancedBrainSimulation: Complex simulation control")
    print("  - BrainBatchProcessor: Parallel batch processing")
    print("  - BrainMLIntegration: Machine learning integration")
    print("  - ExperimentManager: Research experiment management")
    print("  - PerformanceMonitor: Performance analysis and monitoring")
else:
    print(f"NLM Advanced Python APIs loaded (C++ bindings not available)")
    print("Some advanced features may not be available.")
