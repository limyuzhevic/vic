# Experiment management utilities
class NLMExperimentManager:
    """Advanced experiment manager for NLM simulations"""
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
        self.experiment_templates = {}
        self.parameter_sweep = {}
    
    def create_experiment(self, name: str, config: NLMSimulationConfig, 
                         num_steps: int, experiment_type: str = 'standard') -> Dict:
        """Create and run an experiment"""
        experiment_id = f"{name}_{int(time.time())}"
        
        if experiment_type == 'standard':
            results = self._run_standard_experiment(config, num_steps)
        elif experiment_type == 'comparative':
            results = self._run_comparative_experiment(config, num_steps)
        elif experiment_type == 'developmental':
            results = self._run_developmental_experiment(config, num_steps)
        elif experiment_type == 'parameter_sweep':
            results = self._run_parameter_sweep_experiment(config, num_steps)
        elif experiment_type == 'longitudinal':
            results = self._run_longitudinal_experiment(config, num_steps)
        else:
            raise ValueError(f"Unknown experiment type: {experiment_type}")
        
        self.experiments[experiment_id] = {
            'name': name,
            'config': config,
            'num_steps': num_steps,
            'type': experiment_type,
            'start_time': time.time(),
            'status': 'completed'
        }
        
        self.results[experiment_id] = results
        
        return {
            'experiment_id': experiment_id,
            'name': name,
            'type': experiment_type,
            'num_steps': num_steps,
            'results': results,
            'start_time': self.experiments[experiment_id]['start_time'],
            'status': 'completed'
        }
    
    def create_experiment_template(self, name: str, template_config: NLMSimulationConfig, 
                                   description: str = ""):
        """Create a reusable experiment template"""
        self.experiment_templates[name] = {
            'config': template_config,
            'description': description,
            'created_at': time.time()
        }
    
    def run_experiment_from_template(self, template_name: str, experiment_name: str,
                                   num_steps: int) -> Dict:
        """Run an experiment using a template"""
        if template_name not in self.experiment_templates:
            raise ValueError(f"Template not found: {template_name}")
        
        template = self.experiment_templates[template_name]
        return self.create_experiment(experiment_name, template['config'], num_steps)
    
    def set_parameter_sweep(self, base_config: NLMSimulationConfig, 
                           parameter_ranges: Dict[str, List]) -> bool:
        """Set up a parameter sweep experiment"""
        try:
            self.parameter_sweep = {
                'base_config': base_config,
                'parameter_ranges': parameter_ranges,
                'created_at': time.time()
            }
            return True
        except Exception as e:
            print(f"Error setting up parameter sweep: {e}")
            return False
    
    def run_parameter_sweep(self, num_steps: int) -> Dict:
        """Run parameter sweep experiment"""
        if not self.parameter_sweep:
            raise ValueError("No parameter sweep configured")
        
        parameter_ranges = self.parameter_sweep['parameter_ranges']
        base_config = self.parameter_sweep['base_config']
        
        experiments = []
        experiment_results = []
        
        # Generate all parameter combinations
        import itertools
        param_combinations = list(itertools.product(*parameter_ranges.values()))
        
        for i, param_values in enumerate(param_combinations):
            # Create experiment config with specific parameter values
            experiment_config = self._create_config_from_params(base_config, parameter_ranges, param_values)
            
            experiment_name = f"sweep_{i}"
            result = self.create_experiment(experiment_name, experiment_config, num_steps, 'standard')
            
            experiments.append({
                'name': experiment_name,
                'config': experiment_config,
                'parameters': dict(zip(parameter_ranges.keys(), param_values)),
                'experiment_id': result['experiment_id']
            })
            
            experiment_results.append(result['results'])
        
        sweep_result = {
            'type': 'parameter_sweep',
            'base_config': base_config,
            'parameter_ranges': parameter_ranges,
            'total_combinations': len(param_combinations),
            'experiments': experiments,
            'results': experiment_results,
            'completed_at': time.time()
        }
        
        return sweep_result
    
    def _create_config_from_params(self, base_config: NLMSimulationConfig, 
                                  parameter_ranges: Dict[str, List], 
                                  param_values: tuple) -> NLMSimulationConfig:
        """Create config from parameter values"""
        config = NLMSimulationConfig()
        
        # Copy all base config values
        for attr_name in base_config.__dict__:
            setattr(config, attr_name, getattr(base_config, attr_name))
        
        # Apply parameter values
        param_dict = dict(zip(parameter_ranges.keys(), param_values))
        for param_name, param_value in param_dict.items():
            if hasattr(config, param_name):
                setattr(config, param_name, param_value)
        
        return config
    
    def _run_standard_experiment(self, config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run a standard experiment"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        results = []
        for step in range(num_steps):
            brain.step(step)
            
            results.append({
                'step': step,
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'development_stage': brain.getDevelopmentalStage(),
                'exc_inhibition_ratio': brain.getExcitationInhibitionRatio()
            })
        
        final_result = results[-1] if results else {}
        
        return {
            'final_firing_rate': final_result.get('average_firing_rate', 0),
            'max_firing_rate': max(r['average_firing_rate'] for r in results) if results else 0,
            'total_spikes': sum(r['total_spike_count'] for r in results),
            'development_time': brain.getDevelopmentalStage(),
            'firing_neurons_trend': [r['firing_neuron_count'] for r in results],
            'exc_inhibition_ratio_final': final_result.get('exc_inhibition_ratio', 0),
            'time_per_step_avg': sum(r['step'] for r in results) / len(results) if results else 0
        }
    
    def _run_comparative_experiment(self, config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run comparative experiment with different configurations"""
        configs = [
            config,
            NLMSimulationConfig(brain_neuron_count=config.brain_neuron_count * 2),
            NLMSimulationConfig(brain_synapse_density=config.brain_synapse_density * 0.5),
            NLMSimulationConfig(brain_v_thresh=config.brain_v_thresh - 5.0),
            NLMSimulationConfig(brain_tau_mem=config.brain_tau_mem * 1.5)
        ]
        
        comparative_results = []
        for i, cfg in enumerate(configs):
            brain = self._create_brain_from_config(cfg)
            brain.initialize()
            
            results = []
            for step in range(num_steps):
                brain.step(step)
                results.append({
                    'step': step,
                    'firing_rate': brain.getAverageFiringRate(),
                    'firing_neurons': brain.getFiringNeuronCount()
                })
            
            comparative_results.append({
                'configuration_id': i,
                'neuron_count': cfg.brain_neuron_count,
                'synapse_density': cfg.brain_synapse_density,
                'v_thresh': cfg.brain_v_thresh,
                'tau_mem': cfg.brain_tau_mem,
                'final_firing_rate': results[-1]['firing_rate'] if results else 0,
                'max_firing_rate': max(r['firing_rate'] for r in results) if results else 0,
                'stability_score': self._calculate_stability_score([r['firing_rate'] for r in results])
            })
        
        best_config = max(comparative_results, key=lambda x: x['max_firing_rate'])
        
        return {
            'comparative_results': comparative_results,
            'best_configuration': best_config,
            'performance_variance': self._calculate_performance_variance(comparative_results)
        }
    
    def _run_developmental_experiment(self, config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run developmental experiment tracking brain maturation"""
        brain = self._create_brain_from_config(config)
        brain.initialize()
        
        developmental_data = []
        for step in range(num_steps):
            brain.step(step)
            
            developmental_data.append({
                'step': step,
                'developmental_stage': brain.getDevelopmentalStage(),
                'firing_rate': brain.getAverageFiringRate(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'stage_progress': self._get_stage_progress(brain.getDevelopmentalStage()),
                'spike_count': brain.getTotalSpikeCount()
            })
        
        stage_transitions = self._analyze_stage_transitions(developmental_data)
        maturation_profile = self._analyze_maturation_profile(developmental_data)
        
        return {
            'developmental_data': developmental_data,
            'final_stage': brain.getDevelopmentalStage(),
            'stage_transitions': stage_transitions,
            'maturation_profile': maturation_profile,
            'critical_period_duration': self._identify_critical_period(developmental_data),
            'stability_phase': self._identify_stability_phase(developmental_data)
        }
    
    def _run_parameter_sweep_experiment(self, config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run parameter sweep experiment"""
        return self.run_parameter_sweep(num_steps)
    
    def _run_longitudinal_experiment(self, config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run longitudinal experiment tracking changes over extended period"""
        brains = []
        for i in range(5):  # Run 5 independent simulations
            brain = self._create_brain_from_config(config)
            brain.initialize()
            brains.append(brain)
        
        longitudinal_data = []
        for step in range(num_steps):
            step_data = []
            for brain in brains:
                brain.step(step)
                step_data.append({
                    'firing_rate': brain.getAverageFiringRate(),
                    'spike_count': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'development_stage': brain.getDevelopmentalStage()
                })
            
            longitudinal_data.append({
                'step': step,
                'simulations': step_data,
                'mean_firing_rate': np.mean([s['firing_rate'] for s in step_data]),
                'std_firing_rate': np.std([s['firing_rate'] for s in step_data]),
                'mean_spike_count': np.mean([s['spike_count'] for s in step_data])
            })
        
        convergence_analysis = self._analyze_convergence(longitudinal_data)
        inter_simulation_correlation = self._calculate_inter_simulation_correlation(longitudinal_data)
        
        return {
            'longitudinal_data': longitudinal_data,
            'convergence_analysis': convergence_analysis,
            'inter_simulation_correlation': inter_simulation_correlation,
            'simulation_diversity': self._calculate_simulation_diversity(longitudinal_data)
        }
    
    def _create_brain_from_config(self, config: NLMSimulationConfig) -> Any:
        """Helper to create brain from config"""
        pynlm_config = pynlm.createDefaultConfig()
        pynlm_config.set("brain.neuron_count", config.brain_neuron_count)
        pynlm_config.set("brain.synapse_density", config.brain_synapse_density)
        pynlm_config.set("brain.connection_probability", config.brain_connection_probability)
        pynlm_config.set("brain.initial_weight_mean", config.brain_initial_weight_mean)
        pynlm_config.set("brain.initial_weight_std", config.brain_initial_weight_std)
        pynlm_config.set("brain.v_thresh", config.brain_v_thresh)
        pynlm_config.set("brain.v_rest", config.brain_v_rest)
        pynlm_config.set("brain.v_reset", config.brain_v_reset)
        pynlm_config.set("brain.tau_mem", config.brain_tau_mem)
        pynlm_config.set("brain.tau_ref", config.brain_tau_ref)
        
        return pynlm.createBrain(pynlm_config)
    
    def _get_stage_progress(self, stage) -> float:
        """Calculate progress through developmental stage"""
        stage_mapping = {'Initial': 0, 'CriticalPeriod': 1, 'Maturation': 2, 'Adult': 3, 'Aging': 4}
        return stage_mapping.get(str(stage), 0)
    
    def _calculate_stability_score(self, firing_rates: List[float]) -> float:
        """Calculate stability score based on firing rate variance"""
        if len(firing_rates) < 2:
            return 0.0
        
        mean_rate = np.mean(firing_rates)
        variance = np.var(firing_rates)
        std_rate = np.sqrt(variance)
        
        # Stability is higher when variance is low relative to mean
        if mean_rate > 0:
            return max(0.0, 1.0 - (std_rate / mean_rate))
        else:
            return 0.0
    
    def _analyze_stage_transitions(self, developmental_data: List[Dict]) -> Dict:
        """Analyze transitions between developmental stages"""
        transitions = {}
        previous_stage = None
        
        for data in developmental_data:
            current_stage = data['developmental_stage']
            if previous_stage is not None and current_stage != previous_stage:
                transition_key = f"{previous_stage}_to_{current_stage}"
                if transition_key not in transitions:
                    transitions[transition_key] = 0
                transitions[transition_key] += 1
            previous_stage = current_stage
        
        return transitions
    
    def _analyze_maturation_profile(self, developmental_data: List[Dict]) -> Dict:
        """Analyze maturation profile over time"""
        stage_profile = {}
        
        for data in developmental_data:
            stage = data['developmental_stage']
            if stage not in stage_profile:
                stage_profile[stage] = []
            stage_profile[stage].append({
                'step': data['step'],
                'firing_rate': data['firing_rate'],
                'stage_progress': data['stage_progress']
            })
        
        # Calculate average firing rate for each stage
        stage_averages = {}
        for stage, data in stage_profile.items():
            stage_averages[stage] = np.mean([d['firing_rate'] for d in data])
        
        return {
            'stage_profile': stage_profile,
            'stage_averages': stage_averages,
            'stage_duration': {stage: len(data) for stage, data in stage_profile.items()}
        }
    
    def _identify_critical_period(self, developmental_data: List[Dict]) -> Dict:
        """Identify critical period during development"""
        # Look for periods of high developmental change
        stage_changes = []
        for i in range(1, len(developmental_data)):
            prev_stage = developmental_data[i-1]['developmental_stage']
            curr_stage = developmental_data[i]['developmental_stage']
            if prev_stage != curr_stage:
                stage_changes.append(developmental_data[i]['step'])
        
        if len(stage_changes) >= 2:
            critical_period_start = stage_changes[0]
            critical_period_end = stage_changes[-1]
            duration = critical_period_end - critical_period_start
            
            return {
                'start_step': critical_period_start,
                'end_step': critical_period_end,
                'duration': duration,
                'stage_transitions': len(stage_changes),
                'high_activity_steps': len([d for d in developmental_data if d['firing_rate'] > np.mean([dd['firing_rate'] for dd in developmental_data])])
            }
        
        return {
            'start_step': None,
            'end_step': None,
            'duration': None,
            'stage_transitions': len(stage_changes),
            'high_activity_steps': 0
        }
    
    def _identify_stability_phase(self, developmental_data: List[Dict]) -> Dict:
        """Identify phase of stability in development"""
        firing_rates = [d['firing_rate'] for d in developmental_data]
        
        # Find period with minimal change in firing rate
        min_variance = float('inf')
        stability_phase = None
        
        for i in range(len(developmental_data) - 10):
            window_rates = firing_rates[i:i+10]
            window_variance = np.var(window_rates)
            
            if window_variance < min_variance:
                min_variance = window_variance
                stability_phase = {
                    'start_step': i,
                    'end_step': i+10,
                    'variance': window_variance,
                    'mean_firing_rate': np.mean(window_rates)
                }
        
        return stability_phase or {
            'start_step': None,
            'end_step': None,
            'variance': None,
            'mean_firing_rate': None
        }
    
    def _calculate_performance_variance(self, comparative_results: List[Dict]) -> Dict:
        """Calculate performance variance across configurations"""
        firing_rates = [r['max_firing_rate'] for r in comparative_results]
        
        return {
            'mean': np.mean(firing_rates),
            'std': np.std(firing_rates),
            'cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0,
            'min': np.min(firing_rates),
            'max': np.max(firing_rates),
            'outliers': self._identify_outliers(firing_rates)
        }
    
    def _identify_outliers(self, values: List[float]) -> List[int]:
        """Identify outlier values using IQR method"""
        if len(values) < 4:
            return []
        
        q1 = np.percentile(values, 25)
        q3 = np.percentile(values, 75)
        iqr = q3 - q1
        
        lower_bound = q1 - 1.5 * iqr
        upper_bound = q3 + 1.5 * iqr
        
        outliers = [i for i, val in enumerate(values) if val < lower_bound or val > upper_bound]
        return outliers
    
    def _analyze_convergence(self, longitudinal_data: List[Dict]) -> Dict:
        """Analyze convergence of multiple simulations"""
        if not longitudinal_data:
            return {}
        
        # Analyze convergence over time
        convergence_points = []
        for i in range(1, len(longitudinal_data)):
            prev_mean = longitudinal_data[i-1]['mean_firing_rate']
            curr_mean = longitudinal_data[i]['mean_firing_rate']
            change = abs(curr_mean - prev_mean)
            convergence_points.append(change)
        
        # Find point where changes become small
        threshold = np.mean(convergence_points) * 0.1  # 10% of mean change
        convergence_step = None
        
        for i, change in enumerate(convergence_points):
            if change < threshold:
                convergence_step = i + 1
                break
        
        return {
            'convergence_step': convergence_step,
            'convergence_threshold': threshold,
            'final_variance': np.var([d['std_firing_rate'] for d in longitudinal_data]),
            'stability_reached': convergence_step is not None
        }
    
    def _calculate_inter_simulation_correlation(self, longitudinal_data: List[Dict]) -> Dict:
        """Calculate correlation between simulations"""
        if len(longitudinal_data) < 2:
            return {}
        
        # Extract time series data for each simulation
        simulation_series = []
        for step_data in longitudinal_data:
            simulation_series.append([s['firing_rate'] for s in step_data['simulations']])
        
        # Calculate correlation matrix
        corr_matrix = np.corrcoef(simulation_series)
        
        return {
            'correlation_matrix': corr_matrix.tolist(),
            'mean_correlation': np.mean(np.abs(corr_matrix[np.triu_indices_from(corr_matrix, k=1)])),
            'similarity_index': np.mean(np.abs(corr_matrix))
        }
    
    def _calculate_simulation_diversity(self, longitudinal_data: List[Dict]) -> Dict:
        """Calculate diversity between simulations"""
        if not longitudinal_data:
            return {}
        
        # Calculate pairwise distances
        simulation_series = []
        for step_data in longitudinal_data:
            simulation_series.append([s['firing_rate'] for s in step_data['simulations']])
        
        n_simulations = len(simulation_series)
        pairwise_distances = []
        
        for i in range(n_simulations):
            for j in range(i+1, n_simulations):
                distance = np.sqrt(np.sum((np.array(simulation_series[i]) - np.array(simulation_series[j]))**2))
                pairwise_distances.append(distance)
        
        return {
            'mean_distance': np.mean(pairwise_distances),
            'std_distance': np.std(pairwise_distances),
            'max_distance': np.max(pairwise_distances),
            'diversity_index': np.mean(pairwise_distances) / np.max(pairwise_distances) if np.max(pairwise_distances) > 0 else 0
        }
    
    def get_experiment_results(self, experiment_id: str) -> Optional[Dict]:
        """Get results for a specific experiment"""
        return self.results.get(experiment_id)
    
    def list_experiments(self) -> List[Dict]:
        """List all experiments"""
        return list(self.experiments.values())
    
    def delete_experiment(self, experiment_id: str) -> bool:
        """Delete an experiment and its results"""
        try:
            if experiment_id in self.experiments:
                del self.experiments[experiment_id]
            if experiment_id in self.results:
                del self.results[experiment_id]
            return True
        except Exception as e:
            print(f"Error deleting experiment: {e}")
            return False

# Global experiment manager instance
_experiment_manager = NLMExperimentManager()

def get_experiment_manager() -> NLMExperimentManager:
    """Get global experiment manager instance"""
    return _experiment_manager

# Convenience functions for experiment management
def create_experiment(name: str, config: NLMSimulationConfig, 
                     num_steps: int, experiment_type: str = 'standard') -> Dict:
    """Create and run an experiment"""
    return _experiment_manager.create_experiment(name, config, num_steps, experiment_type)

def create_experiment_template(name: str, template_config: NLMSimulationConfig, 
                               description: str = ""):
    """Create a reusable experiment template"""
    _experiment_manager.create_experiment_template(name, template_config, description)

def run_parameter_sweep(base_config: NLMSimulationConfig, 
                       parameter_ranges: Dict[str, List], 
                       num_steps: int) -> Dict:
    """Run a parameter sweep experiment"""
    _experiment_manager.set_parameter_sweep(base_config, parameter_ranges)
    return _experiment_manager.run_parameter_sweep(num_steps)

# Batch utilities
class BatchProcessor:
    """Process batches of simulations efficiently"""
    
    @staticmethod
    def process_simulations_in_parallel(sim_configs: List[NLMSimulationConfig], 
                                      num_steps: int,
                                      num_workers: int = 4) -> List[Dict]:
        """Process multiple simulations in parallel"""
        import concurrent.futures
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
            futures = []
            for config in sim_configs:
                future = executor.submit(BatchProcessor._run_single_simulation, config, num_steps)
                futures.append(future)
            
            results = []
            for future in concurrent.futures.as_completed(futures):
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    print(f"Error in parallel simulation: {e}")
        
        return results
    
    @staticmethod
    def _run_single_simulation(config: NLMSimulationConfig, num_steps: int) -> Dict:
        """Run a single simulation"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        results = []
        for step in range(num_steps):
            brain.step(step)
            results.append({
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'firing_neurons': brain.getFiringNeuronCount()
            })
        
        return {
            'config': config,
            'results': results,
            'final_firing_rate': results[-1]['firing_rate'] if results else 0,
            'max_firing_rate': max(r['firing_rate'] for r in results) if results else 0
        }

# Update todo list
import json

with open('/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f8ce44ab-b949-46f6-8d57-611579d79bed/.todo.json', 'r') as f:
    todos = json.load(f)

# Mark first todo as completed and next as in_progress
todos[0]['status'] = 'completed'
todos[1]['status'] = 'in_progress'

with open('/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f8ce44ab-b949-46f6-8d57-611579d79bed/.todo.json', 'w') as f:
    json.dump(todos, f, indent=2)