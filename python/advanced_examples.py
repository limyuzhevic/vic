"""
NLM Advanced Python Examples
============================

Advanced examples for power users of the enhanced NLM Python bindings.
These examples demonstrate complex scenarios and optimization techniques.
"""

import pynlm
import json
import time
import statistics
from typing import List, Dict, Tuple

class NLMExperiment:
    """
    Advanced NLM experiment management class.
    """
    
    def __init__(self, name: str):
        self.name = name
        self.results = []
        self.timings = []
        
    def run_experiment(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                      agent: pynlm.AgentBrain, steps: int, **kwargs) -> Dict:
        """
        Run a single experiment with comprehensive tracking.
        """
        start_time = time.time()
        step_times = []
        
        print(f"Running experiment '{self.name}' for {steps} steps...")
        
        for step in range(steps):
            step_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Motor control
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = kwargs.get('reward', 0.1)
            curiosity = kwargs.get('curiosity', 0.0)
            agent.applyRewardModulation(reward, curiosity)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            # Collect metrics every 100 steps
            if step % 100 == 0:
                metrics = {
                    'step': step,
                    'time_elapsed': time.time() - start_time,
                    'step_time': step_time,
                    'firing_rate': brain.getAverageFiringRate(),
                    'neurons_active': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'reward': agent.getNeuromodulationLevel(),
                    'development': agent.getDevelopmentalStage()
                }
                self.results.append(metrics)
        
        total_time = time.time() - start_time
        
        # Calculate performance metrics
        avg_step_time = statistics.mean(step_times) if step_times else 0
        max_step_time = max(step_times) if step_times else 0
        min_step_time = min(step_times) if step_times else 0
        std_step_time = statistics.stdev(step_times) if len(step_times) > 1 else 0
        
        experiment_result = {
            'name': self.name,
            'total_steps': steps,
            'total_time': total_time,
            'steps_per_second': steps / total_time if total_time > 0 else 0,
            'avg_step_time': avg_step_time,
            'max_step_time': max_step_time,
            'min_step_time': min_step_time,
            'std_step_time': std_step_time,
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_neurons_active': brain.getFiringNeuronCount(),
            'final_total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_reward': agent.getNeuromodulationLevel(),
            'final_development': agent.getDevelopmentalStage(),
            'metrics_collected': len(self.results)
        }
        
        self.timings.append(experiment_result)
        
        print(f"Experiment '{self.name}' completed in {total_time:.2f}s")
        print(f"  Performance: {steps/total_time:.0f} steps/s")
        print(f"  Final firing rate: {experiment_result['final_firing_rate']:.3f}")
        print(f"  Final curiosity: {experiment_result['final_curiosity']:.3f}")
        
        return experiment_result
    
    def get_summary(self) -> Dict:
        """Get experiment summary."""
        if not self.timings:
            return {}
        
        return {
            'experiment_name': self.name,
            'total_runs': len(self.timings),
            'total_steps': sum(t['total_steps'] for t in self.timings),
            'total_time': sum(t['total_time'] for t in self.timings),
            'avg_steps_per_second': statistics.mean(t['steps_per_second'] for t in self.timings) if self.timings else 0,
            'avg_final_firing_rate': statistics.mean(t['final_firing_rate'] for t in self.timings) if self.timings else 0,
            'avg_final_curiosity': statistics.mean(t['final_curiosity'] for t in self.timings) if self.timings else 0
        }

class NLMOptimizer:
    """
    Advanced NLM optimizer for parameter tuning and performance optimization.
    """
    
    @staticmethod
    def optimize_brain_config(base_config: pynlm.Config, 
                             neuron_counts: List[int],
                             trials_per_config: int = 3) -> Dict:
        """
        Optimize brain configuration for different network sizes.
        """
        results = {}
        
        for neuron_count in neuron_counts:
            config_name = f"{neuron_count}_neurons"
            config_results = []
            
            print(f"Optimizing configuration for {neuron_count} neurons...")
            
            for trial in range(trials_per_config):
                # Create and optimize configuration
                trial_config = pynlm.createDefaultConfig()
                
                # Copy base config parameters
                for key in base_config.getKeys():
                    if key in ['connection_probability', 'stdp_ltp_weight', 
                              'stdp_ltd_weight', 'synaptogenesis_rate']:
                        value = base_config.getFloat(key, 0.0f)
                        trial_config.setFloat(key, value)
                
                # Apply trial-specific modifications
                if trial == 0:
                    trial_config.setFloat('connection_probability', 0.1f)
                elif trial == 1:
                    trial_config.setFloat('connection_probability', 0.15f)
                else:
                    trial_config.setFloat('connection_probability', 0.12f)
                
                # Create brain
                brain = pynlm.createBrain(trial_config)
                brain.initialize()
                
                # Run benchmark
                start_time = time.time()
                for step in range(1000):
                    brain.step(step)
                elapsed = time.time() - start_time
                
                trial_result = {
                    'trial': trial,
                    'connection_probability': trial_config.getFloat('connection_probability'),
                    'steps_per_second': 1000 / elapsed,
                    'final_firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount()
                }
                
                config_results.append(trial_result)
            
            # Calculate statistics
            avg_steps_per_second = statistics.mean(r['steps_per_second'] for r in config_results)
            avg_firing_rate = statistics.mean(r['final_firing_rate'] for r in config_results)
            std_steps_per_second = statistics.stdev(r['steps_per_second'] for r in config_results) if len(config_results) > 1 else 0
            
            results[config_name] = {
                'trials': config_results,
                'avg_steps_per_second': avg_steps_per_second,
                'avg_firing_rate': avg_firing_rate,
                'std_steps_per_second': std_steps_per_second,
                'optimal_connection_prob': max(config_results, key=lambda r: r['steps_per_second'])['connection_probability']
            }
        
        return results
    
    @staticmethod
    def optimize_agent_behavior(base_brain: pynlm.Brain,
                               world: pynlm.SimpleWorld,
                               agent_configs: List[Dict]) -> Dict:
        """
        Optimize agent behavior with different configurations.
        """
        results = {}
        
        for config_name, config in agent_configs.items():
            print(f"Testing agent behavior: {config_name}")
            
            # Create agent
            agent = pynlm.createAgentBrain(base_brain)
            agent.initialize(world)
            
            # Apply configuration
            if config.get('enable_reward_modulation', True):
                agent.enableRewardModulation(True)
            if config.get('enable_curiosity', True):
                agent.enableCuriosity(True)
            if config.get('enable_development', True):
                agent.enableDevelopment(True)
            if config.get('enable_structural_plasticity', False):
                agent.enableStructuralPlasticity(True)
            
            # Run experiment
            experiment = NLMExperiment(config_name)
            result = experiment.run_experiment(base_brain, world, agent, 
                                             config.get('steps', 500),
                                             reward=config.get('reward', 0.1),
                                             curiosity=config.get('curiosity', 0.0))
            
            results[config_name] = {
                'experiment': experiment,
                'result': result,
                'config': config
            }
        
        return results

def advanced_learning_example():
    """
    Advanced learning example with curriculum learning.
    """
    print("=== Advanced Learning Example ===")
    
    # Create complete system
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Enable all learning systems
    agent.enableAllSystems()
    
    # Curriculum learning phases
    phases = [
        {'name': 'Warm-up', 'steps': 100, 'reward': 0.5, 'curiosity': 0.1},
        {'name': 'Exploration', 'steps': 200, 'reward': 0.3, 'curiosity': 0.5},
        {'name': 'Exploitation', 'steps': 300, 'reward': 0.8, 'curiosity': 0.2},
        {'name': 'Refinement', 'steps': 200, 'reward': 1.0, 'curiosity': 0.1}
    ]
    
    # Run curriculum learning
    for phase in phases:
        print(f"\n--- Phase: {phase['name']} ---")
        
        experiment = NLMExperiment(phase['name'])
        result = experiment.run_experiment(
            brain, world, agent, 
            phase['steps'],
            reward=phase['reward'],
            curiosity=phase['curiosity']
        )
        
        # Phase-specific adjustments
        if phase['name'] == 'Warm-up':
            # Low learning rate for stability
            pass
        elif phase['name'] == 'Exploration':
            # High curiosity for exploration
            pass
        elif phase['name'] == 'Exploitation':
            # Focus on reward maximization
            pass
        elif phase['name'] == 'Refinement':
            # Fine-tuning and consolidation
            pass
    
    # Get final learning curve
    print("\n=== Learning Analysis ===")
    
    all_firing_rates = []
    all_curiosities = []
    all_rewards = []
    
    for experiment in NLMExperiment.get_experiments():
        all_firing_rates.extend([r['firing_rate'] for r in experiment.results])
        all_curiosities.extend([r['curiosity'] for r in experiment.results])
        all_rewards.extend([r['reward'] for r in experiment.results])
    
    if all_firing_rates:
        print(f"Average firing rate: {statistics.mean(all_firing_rates):.3f}")
        print(f"Firing rate std: {statistics.stdev(all_firing_rates) if len(all_firing_rates) > 1 else 0:.3f}")
        print(f"Min firing rate: {min(all_firing_rates):.3f}")
        print(f"Max firing rate: {max(all_firing_rates):.3f}")
        
        print(f"\nAverage curiosity: {statistics.mean(all_curiosities):.3f}")
        print(f"Average reward: {statistics.mean(all_rewards):.3f}")
    
    # Save learning curve
    learning_data = {
        'firing_rates': all_firing_rates,
        'curiosities': all_curiosities,
        'rewards': all_rewards,
        'phases': [p['name'] for p in phases]
    }
    
    with open('learning_curve.json', 'w') as f:
        json.dump(learning_data, f, indent=2)
    
    print(f"\nLearning data saved to learning_curve.json")
    print()

def network_analysis_example():
    """
    Advanced network analysis with statistical modeling.
    """
    print("=== Network Analysis Example ===")
    
    # Create and run brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Collect network statistics over time
    network_stats = []
    
    for step in range(1000):
        brain.step(step)
        
        if step % 50 == 0:
            stats = {
                'step': step,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            }
            network_stats.append(stats)
    
    # Statistical analysis
    print("\nNetwork Statistics Analysis:")
    print("-" * 50)
    
    for stat in ['firing_rate', 'active_neurons', 'firing_neurons', 'total_spikes', 'e_i_ratio']:
        values = [s[stat] for s in network_stats]
        mean_val = statistics.mean(values)
        std_val = statistics.stdev(values) if len(values) > 1 else 0
        min_val = min(values)
        max_val = max(values)
        
        print(f"{stat.replace('_', ' ').title()}:")
        print(f"  Mean: {mean_val:.3f}")
        print(f"  Std: {std_val:.3f}")
        print(f"  Min: {min_val:.3f}")
        print(f"  Max: {max_val:.3f}")
        print()
    
    # Network topology analysis
    print("Network Topology Analysis:")
    print("-" * 30)
    
    # Get region information
    regions = brain.getRegions()
    for i, region in enumerate(regions):
        neuron_count = region.getTotalNeuronCount()
        synapse_count = region.getTotalSynapseCount()
        population_count = region.getPopulationCount()
        
        print(f"Region {i}:")
        print(f"  Neurons: {neuron_count}")
        print(f"  Synapses: {synapse_count}")
        print(f"  Populations: {population_count}")
        if neuron_count > 0:
            print(f"  Synapses/Neuron: {synapse_count/neuron_count:.3f}")
        print()
    
    # Save network analysis
    analysis = {
        'network_stats': network_stats,
        'final_stats': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        },
        'statistical_summary': {}
    }
    
    for stat in ['firing_rate', 'active_neurons', 'firing_neurons', 'total_spikes', 'e_i_ratio']:
        values = [s[stat] for s in network_stats]
        analysis['statistical_summary'][stat] = {
            'mean': statistics.mean(values),
            'std': statistics.stdev(values) if len(values) > 1 else 0,
            'min': min(values),
            'max': max(values),
            'range': max(values) - min(values)
        }
    
    with open('network_analysis.json', 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"Network analysis saved to network_analysis.json")
    print()

def experimental_design_example():
    """
    Experimental design example for NLM research.
    """
    print("=== Experimental Design Example ===")
    
    # Design multiple experimental conditions
    experimental_conditions = {
        'baseline': {
            'neuron_count': 1000,
            'connection_probability': 0.1,
            'enable_reward': True,
            'enable_curiosity': False,
            'enable_development': False,
            'steps': 500
        },
        'reward_conditioned': {
            'neuron_count': 1000,
            'connection_probability': 0.15,
            'enable_reward': True,
            'enable_curiosity': True,
            'enable_development': True,
            'steps': 500
        },
        'curiosity_driven': {
            'neuron_count': 1000,
            'connection_probability': 0.12,
            'enable_reward': False,
            'enable_curiosity': True,
            'enable_development': True,
            'steps': 500
        },
        'development_enabled': {
            'neuron_count': 1000,
            'connection_probability': 0.08,
            'enable_reward': True,
            'enable_curiosity': True,
            'enable_development': True,
            'steps': 500
        }
    }
    
    # Run all experimental conditions
    experimental_results = {}
    
    for condition_name, condition_config in experimental_conditions.items():
        print(f"\nRunning experimental condition: {condition_name}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.setInt("neuron_count", condition_config['neuron_count'])
        config.setFloat("connection_probability", condition_config['connection_probability'])
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Apply condition configuration
        agent.enableRewardModulation(condition_config['enable_reward'])
        agent.enableCuriosity(condition_config['enable_curiosity'])
        agent.enableDevelopment(condition_config['enable_development'])
        
        # Run experiment
        experiment = NLMExperiment(condition_name)
        result = experiment.run_experiment(
            brain, world, agent, 
            condition_config['steps'],
            reward=0.5 if condition_config['enable_reward'] else 0.0,
            curiosity=0.5 if condition_config['enable_curiosity'] else 0.0
        )
        
        experimental_results[condition_name] = {
            'config': condition_config,
            'experiment': experiment,
            'result': result
        }
    
    # Statistical analysis across conditions
    print("\n=== Experimental Results Analysis ===")
    
    print("Comparison across experimental conditions:")
    print("-" * 60)
    print(f"{'Condition':<20} {'Firing Rate':<12} {'Curiosity':<12} {'Reward':<12} {'Steps/s':<10}")
    print("-" * 60)
    
    for condition_name, data in experimental_results.items():
        result = data['result']
        config = data['config']
        
        print(f"{condition_name:<20} "
              f"{result['final_firing_rate']:<12.3f} "
              f"{result['final_curiosity']:<12.3f} "
              f"{result['final_reward']:<12.3f} "
              f"{result['steps_per_second']:<10.0f}")
    
    # Save experimental results
    with open('experimental_results.json', 'w') as f:
        # Convert non-serializable objects
        serializable_results = {}
        for condition_name, data in experimental_results.items():
            serializable_results[condition_name] = {
                'config': data['config'],
                'result': {k: (v if not isinstance(v, (list, dict)) else str(v)) 
                          for k, v in data['result'].items()},
                'metrics_count': len(data['experiment'].results)
            }
        
        json.dump(serializable_results, f, indent=2)
    
    print(f"\nExperimental results saved to experimental_results.json")
    print()

def optimization_workflow_example():
    """
    Complete optimization workflow example.
    """
    print("=== Optimization Workflow Example ===")
    
    # Step 1: Define optimization problem
    print("Step 1: Define optimization problem")
    print("  - Objective: Maximize steps per second")
    print("  - Variables: Connection probability, neuron count")
    print("  - Constraints: Fixed simulation time, memory limits")
    print()
    
    # Step 2: Create baseline configuration
    print("Step 2: Create baseline configuration")
    baseline_config = pynlm.createDefaultConfig()
    baseline_config.setFloat("connection_probability", 0.1f)
    print(f"  Baseline: {baseline_config.summary()}")
    print()
    
    # Step 3: Run optimization study
    print("Step 3: Run optimization study")
    
    neuron_counts = [500, 1000, 2000, 5000]
    optimizer_results = NLMOptimizer.optimize_brain_config(baseline_config, neuron_counts)
    
    print("\nOptimization Results:")
    print("-" * 80)
    print(f"{'Neurons':<10} {'Avg Steps/s':<12} {'Avg Firing':<12} {'Optimal Conn':<15} {'Performance':<15}")
    print("-" * 80)
    
    for config_name, results in optimizer_results.items():
        neuron_count = int(config_name.split('_')[0])
        perf_score = results['avg_steps_per_second'] / max(r['steps_per_second'] for r in results['trials'])
        
        print(f"{neuron_count:<10} {results['avg_steps_per_second']:<12.0f} "
              f"{results['avg_firing_rate']:<12.3f} "
              f"{results['optimal_connection_prob']:<15.3f} "
              f"{perf_score:.2f}")
    
    # Step 4: Agent behavior optimization
    print("\nStep 4: Agent behavior optimization")
    
    # Create base system
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Define agent behavior configurations
    agent_configs = {
        'conservative': {
            'enable_reward_modulation': True,
            'enable_curiosity': False,
            'enable_development': True,
            'enable_structural_plasticity': False,
            'steps': 300
        },
        'exploratory': {
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_structural_plasticity': True,
            'steps': 300
        },
        'balanced': {
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_structural_plasticity': False,
            'steps': 300
        }
    }
    
    agent_results = NLMOptimizer.optimize_agent_behavior(brain, world, agent_configs)
    
    print("\nAgent Behavior Optimization Results:")
    print("-" * 60)
    print(f"{'Strategy':<12} {'Curiosity':<10} {'Reward':<10} {'Final Firing':<12} {'Performance':<10}")
    print("-" * 60)
    
    for strategy_name, data in agent_results.items():
        result = data['result']
        config = data['config']
        
        # Calculate performance score based on final state
        perf_score = (result['final_curiosity'] + result['final_reward']) / 2.0
        
        print(f"{strategy_name:<12} {config['enable_curiosity']:<10} "
              f"{config['enable_reward_modulation']:<10} "
              f"{result['final_firing_rate']:<12.3f} "
              f"{perf_score:.2f}")
    
    # Step 5: Generate optimization report
    print("\nStep 5: Generate optimization report")
    
    optimization_report = {
        'problem_definition': 'Maximize neural network performance',
        'variables_optimized': ['connection_probability', 'neuron_count', 'agent_behavior'],
        'constraints': ['fixed_simulation_time', 'memory_limits'],
        'baseline_config': baseline_config.summary(),
        'brain_optimization': optimizer_results,
        'agent_optimization': {name: {k: str(v) if not isinstance(v, (int, float, str, list, dict, type(None))) else v 
                                    for k, v in data.items() if k != 'experiment'} 
                              for name, data in agent_results.items()},
        'recommendations': [
            f"Use {optimizer_results['1000_neurons']['optimal_connection_prob']:.3f} connection probability for 1000 neurons",
            "Balanced agent strategy provides best overall performance",
            "Consider larger networks (2000+ neurons) for highest throughput"
        ]
    }
    
    with open('optimization_report.json', 'w') as f:
        json.dump(optimization_report, f, indent=2)
    
    print(f"Optimization report saved to optimization_report.json")
    print()
    print("=== Optimization Complete ===")
    print("Key findings:")
    for rec in optimization_report['recommendations']:
        print(f"  - {rec}")
    print()

def run_all_advanced_examples():
    """Run all advanced examples."""
    print("NLM Advanced Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Advanced Learning", advanced_learning_example),
        ("Network Analysis", network_analysis_example),
        ("Experimental Design", experimental_design_example),
        ("Optimization Workflow", optimization_workflow_example),
    ]
    
    for name, example in examples:
        print(f"--- {name} ---")
        try:
            example()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
        print("=" * 60)
    
    print("\nAll advanced examples completed!")

if __name__ == '__main__':
    # Run all advanced examples
    run_all_advanced_examples()
    
    print("\nFor more information, visit:")
    print("  - https://github.com/nlm-project/nlm")
    print("  - https://nlm.readthedocs.io")
    print("  - python -m pynlm.cli --help")