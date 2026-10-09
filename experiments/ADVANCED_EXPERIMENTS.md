# Advanced Features for Expert Users

This document covers advanced features for expert users of the NLM system, including multi-agent systems, batch processing, and research tools.

## Multi-Agent Systems

### Multi-Agent Framework

The NLM system provides a comprehensive framework for creating and managing multiple intelligent agents. This enables research into social learning, competition, cooperation, and complex group dynamics.

```python
import pynlm

class AgentTeam:
    def __init__(self, num_agents, agent_types=None):
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        
        # Create agents with different specializations
        if agent_types is None:
            agent_types = ['exploration', 'social', 'learning', 'coordination'] * (num_agents // 4 + 1)
        
        for i, agent_type in enumerate(agent_types[:num_agents]):
            config = pynlm.createResearchConfig()
            config.set("brain.neuron_count", 3000 - i * 200)  # Vary brain sizes
            config.set("brain.synapse_density", 0.15 + i * 0.02)
            
            # Configure agent based on type
            if agent_type == 'exploration':
                config.set("neuromod.curiosity.enable", True)
                config.set("neuromod.novelty.enable", True)
                config.set("neuromod.dopamine.scale", 1.5)
            elif agent_type == 'social':
                config.set("plasticity.structural.enable", True)
                config.set("memory.associative.enable", True)
                config.set("cognition.social_learning.enable", True)
            elif agent_type == 'learning':
                config.set("plasticity.stdp.enable", True)
                config.set("plasticity.hebbian.enable", True)
                config.set("prediction.system.enable", True)
            elif agent_type == 'coordination':
                config.set("cognition.planning.enable", True)
                config.set("cognition.attention.enable", True)
                config.set("development.enable", True)
            
            brain = pynlm.createAdvancedBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            self.agents.append({
                'id': i,
                'type': agent_type,
                'brain': brain,
                'agent': agent,
                'history': []
            })
    
    def run_simulation(self, num_steps, enable_communication=True):
        """Run simulation with all agents"""
        for step in range(num_steps):
            self.world.update(0.1)
            
            # Each agent processes the world
            for agent_data in self.agents:
                agent = agent_data['agent']
                percept = self.world.getSensoryPercept()
                
                # Optional inter-agent communication
                if enable_communication and step > 0:
                    communication = self._get_agent_communication(agent_data, step)
                    percept = self._combine_percepts(percept, communication)
                
                agent.processSensoryInput(percept)
                agent_data['brain'].step(step)
                
                # Different action strategies based on agent type
                action = self._get_agent_action(agent_data, step)
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply neuromodulation with agent-specific learning
                reward = self._calculate_agent_reward(agent_data, result, step)
                predicted = self._get_predicted_reward(agent_data, action)
                agent.applyRewardModulation(reward, predicted)
                
                # Update agent development
                agent.updateDevelopment(0.1)
                
                # Record agent history
                agent_data['history'].append({
                    'step': step,
                    'position': (agent_data['agent'].getBrain().getAgentBody().x,
                               agent_data['agent'].getBrain().getAgentBody().y),
                    'reward': result.reward,
                    'action': action,
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel()
                })
    
    def _get_agent_communication(self, agent_data, step):
        """Generate communication between agents"""
        communication = []
        for other_agent in self.agents:
            if other_agent['id'] != agent_data['id']:
                # Simple proximity-based communication
                distance = self._calculate_distance(agent_data, other_agent)
                if distance < 20.0:  # Communication range
                    # Share observations
                    comm_strength = max(0.0, 1.0 - distance / 20.0)
                    communication.append({
                        'from': other_agent['id'],
                        'type': 'observation',
                        'strength': comm_strength,
                        'data': other_agent['history'][-1] if other_agent['history'] else None
                    })
        return communication
    
    def _combine_percepts(self, original_percept, communications):
        """Combine agent's own percepts with communications from other agents"""
        # This is a simplified example - real implementation would be more sophisticated
        combined_vision = original_percept.getVision()
        
        for comm in communications:
            if comm['type'] == 'observation' and comm['data']:
                # Blend observations based on communication strength
                pass  # Implementation depends on your specific needs
        
        return original_percept  # Simplified
    
    def _calculate_distance(self, agent1, agent2):
        """Calculate distance between two agents"""
        x1, y1 = agent1['agent'].getBrain().getAgentBody().x, agent1['agent'].getBrain().getAgentBody().y
        x2, y2 = agent2['agent'].getBrain().getAgentBody().x, agent2['agent'].getBrain().getAgentBody().y
        return ((x2 - x1) ** 2 + (y2 - y1) ** 2) ** 0.5
    
    def _get_agent_action(self, agent_data, step):
        """Get action for agent based on its type and state"""
        agent_type = agent_data['type']
        history = agent_data['history']
        
        # Different action strategies for different agent types
        if agent_type == 'exploration':
            # Exploration agents seek novel experiences
            return self._get_exploration_action(agent_data)
        elif agent_type == 'social':
            # Social agents imitate successful behaviors
            return self._get_social_action(agent_data)
        elif agent_type == 'learning':
            # Learning agents optimize based on prediction errors
            return self._get_learning_action(agent_data)
        elif agent_type == 'coordination':
            # Coordination agents work together toward shared goals
            return self._get_coordination_action(agent_data)
        else:
            return agent_data['agent'].decodeMotorCommand()
    
    def _get_exploration_action(self, agent_data):
        """Exploration agent seeks novel stimuli"""
        agent = agent_data['agent']
        percept = agent.getBrain().getSensoryPercept()
        
        # Move toward areas with high novelty
        novelty_level = agent.getNoveltyLevel()
        if novelty_level < 0.3:
            # Seek novelty
            return pynlm.MotorCommand.TurnLeft
        elif novelty_level > 0.7:
            # Stay in novel area
            return pynlm.MotorCommand.MoveForward
        else:
            # Moderate curiosity - explore
            return pynlm.MotorCommand.TurnRight
    
    def _get_social_action(self, agent_data):
        """Social agent imitates successful behaviors"""
        # Look at successful actions from nearby agents
        successful_actions = []
        for other_agent in self.agents:
            if other_agent['id'] != agent_data['id']:
                # Find actions that led to positive rewards
                for entry in other_agent['history']:
                    if entry['reward'] > 0.5:
                        successful_actions.append(entry['action'])
        
        if successful_actions:
            # Imitate most successful action
            return successful_actions[0]
        else:
            return agent_data['agent'].decodeMotorCommand()
    
    def _get_learning_action(self, agent_data):
        """Learning agent optimizes based on prediction errors"""
        agent = agent_data['agent']
        curiosity_level = agent.getCuriosityLevel()
        prediction_error = agent.getPredictionError()
        
        if prediction_error > 0.5:
            # High prediction error - try new actions
            return pynlm.MotorCommand.TurnLeft
        elif curiosity_level < 0.3:
            # Low curiosity - exploit known good actions
            return pynlm.MotorCommand.MoveForward
        else:
            # Moderate learning - balanced exploration/exploitation
            return pynlm.MotorCommand.MoveForward
    
    def _get_coordination_action(self, agent_data):
        """Coordination agent works toward group goals"""
        # Simple coordination: move toward center
        center_x, center_y = 50, 50
        agent = agent_data['agent']
        body = agent.getBrain().getAgentBody()
        
        if body.x < center_x:
            return pynlm.MotorCommand.MoveForward
        elif body.y < center_y:
            return pynlm.MotorCommand.TurnRight
        else:
            return pynlm.MotorCommand.MoveForward
    
    def _calculate_agent_reward(self, agent_data, action_result, step):
        """Calculate reward for agent based on action result and agent type"""
        agent_type = agent_data['type']
        
        if agent_type == 'exploration':
            # Exploration reward: novelty + resource discovery
            novelty_bonus = agent_data['agent'].getNoveltyLevel() * 0.5
            return action_result.reward + novelty_bonus
        elif agent_type == 'social':
            # Social reward: cooperation + learning from others
            cooperation_bonus = 0.2  # Fixed cooperation bonus
            return action_result.reward + cooperation_bonus
        elif agent_type == 'learning':
            # Learning reward: prediction error reduction
            pred_error = agent_data['agent'].getPredictionError()
            learning_bonus = (1.0 - pred_error) * 0.3
            return action_result.reward + learning_bonus
        elif agent_type == 'coordination':
            # Coordination reward: group success
            group_success = 1.0 if step % 10 == 0 else 0.0
            return action_result.reward + group_success
        else:
            return action_result.reward
    
    def _get_predicted_reward(self, agent_data, action):
        """Get predicted reward for action"""
        # Use agent's prediction system
        pred_system = agent_data['agent'].getBrain().getPredictionSystem()
        if pred_system:
            # Simplified prediction - in practice would be more sophisticated
            return 0.5
        return 0.0
    
    def get_team_statistics(self):
        """Get statistics for the entire team"""
        stats = {
            'total_agents': len(self.agents),
            'agent_types': list(set(a['type'] for a in self.agents)),
            'total_rewards': [],
            'collaboration_scores': [],
            'learning_rates': []
        }
        
        for agent_data in self.agents:
            history = agent_data['history']
            if history:
                rewards = [entry['reward'] for entry in history]
                stats['total_rewards'].append({
                    'agent_id': agent_data['id'],
                    'type': agent_data['type'],
                    'total_reward': sum(rewards),
                    'avg_reward': sum(rewards) / len(rewards),
                    'max_reward': max(rewards),
                    'min_reward': min(rewards)
                })
                
                # Calculate collaboration score (reward shared with team)
                collaboration_score = sum(1 for entry in history if entry['reward'] > 0.5) / len(history)
                stats['collaboration_scores'].append({
                    'agent_id': agent_data['id'],
                    'score': collaboration_score
                })
        
        return stats
```

### Batch Processing System

The NLM system supports batch processing for running multiple simulations efficiently, which is essential for research and parameter tuning.

```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import time

class BatchProcessor:
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
        self.results = []
    
    def run_batch_simulation(self, sim_configs, sim_function, num_iterations=1):
        """Run multiple simulations in batch"""
        self.results = []
        
        # Prepare simulation tasks
        tasks = []
        for i, config in enumerate(sim_configs):
            for iter_num in range(num_iterations):
                task = {
                    'task_id': i * num_iterations + iter_num,
                    'config': config,
                    'iteration': iter_num,
                    'timestamp': time.time()
                }
                tasks.append(task)
        
        # Run simulations in parallel
        with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
            futures = [executor.submit(self._run_single_simulation, task) 
                      for task in tasks]
            
            for future in futures:
                result = future.result()
                self.results.append(result)
        
        return self.results
    
    def run_batch_parallel(self, sim_configs, sim_function, num_iterations=1, 
                          parallelization='processes'):
        """Run simulations with different parallelization strategies"""
        if parallelization == 'processes':
            return self._run_process_parallel(sim_configs, sim_function, num_iterations)
        elif parallelization == 'threads':
            return self._run_thread_parallel(sim_configs, sim_function, num_iterations)
        elif parallelization == 'vectorized':
            return self._run_vectorized_parallel(sim_configs, sim_function, num_iterations)
        else:
            raise ValueError(f"Unknown parallelization method: {parallelization}")
    
    def _run_single_simulation(self, task):
        """Run a single simulation with given task configuration"""
        try:
            # Import here to avoid pickling issues in multiprocessing
            import pynlm
            
            config = task['config']
            simulation_function = task.get('sim_function')
            
            # Create brain and run simulation
            brain = pynlm.createAdvancedBrain(config)
            
            if simulation_function:
                result = simulation_function(brain, task)
            else:
                # Default simulation
                result = self._default_simulation(brain, task)
            
            return {
                'task_id': task['task_id'],
                'success': True,
                'result': result,
                'config_used': config,
                'execution_time': time.time() - task['timestamp']
            }
            
        except Exception as e:
            return {
                'task_id': task['task_id'],
                'success': False,
                'error': str(e),
                'config_used': task.get('config'),
                'execution_time': time.time() - task.get('timestamp', 0)
            }
    
    def _default_simulation(self, brain, task):
        """Default simulation function"""
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        num_steps = task.get('steps', 1000)
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        return {
            'final_reward': result.reward,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
    
    def _run_process_parallel(self, sim_configs, sim_function, num_iterations):
        """Run simulations using process-based parallelization"""
        tasks = []
        for i, config in enumerate(sim_configs):
            for iter_num in range(num_iterations):
                task = {
                    'config': config,
                    'sim_function': sim_function,
                    'steps': 500 + iter_num * 100,
                    'timestamp': time.time()
                }
                tasks.append(task)
        
        with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
            futures = [executor.submit(self._run_single_simulation, task) for task in tasks]
            results = []
            
            for future in futures:
                results.append(future.result())
        
        return results
    
    def _run_thread_parallel(self, sim_configs, sim_function, num_iterations):
        """Run simulations using thread-based parallelization"""
        tasks = []
        for i, config in enumerate(sim_configs):
            for iter_num in range(num_iterations):
                task = {
                    'config': config,
                    'sim_function': sim_function,
                    'steps': 500 + iter_num * 100,
                    'timestamp': time.time()
                }
                tasks.append(task)
        
        with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
            futures = [executor.submit(self._run_single_simulation, task) for task in tasks]
            results = []
            
            for future in futures:
                results.append(future.result())
        
        return results
    
    def _run_vectorized_parallel(self, sim_configs, sim_function, num_iterations):
        """Run simulations with vectorized operations"""
        # Vectorized simulation - optimized for running many simulations
        # with similar configurations simultaneously
        results = []
        
        start_time = time.time()
        
        # Group similar configurations for efficiency
        config_groups = self._group_configs_by_similarity(sim_configs)
        
        for group_idx, config_group in enumerate(config_groups):
            # Run group of simulations
            group_results = []
            for config in config_group:
                for iter_num in range(num_iterations):
                    task = {
                        'config': config,
                        'sim_function': sim_function,
                        'steps': 500 + iter_num * 100,
                        'timestamp': time.time()
                    }
                    
                    result = self._run_single_simulation(task)
                    group_results.append(result)
            
            # Process group results
            processed_group = self._process_simulation_group(group_results)
            results.extend(processed_group)
        
        total_time = time.time() - start_time
        
        return {
            'results': results,
            'total_time': total_time,
            'simulations_per_second': len(results) / total_time if total_time > 0 else 0
        }
    
    def _group_configs_by_similarity(self, sim_configs):
        """Group configurations by similarity for vectorized processing"""
        # Simple grouping based on neuron count and plasticity settings
        groups = {}
        
        for config in sim_configs:
            # Extract key features for grouping
            key_features = (
                config.get('brain.neuron_count', 1000),
                config.get('plasticity.stdp.enable', False),
                config.get('plasticity.hebbian.enable', False),
                config.get('neuromod.curiosity.enable', False),
                config.get('development.enable', False)
            )
            
            if key_features not in groups:
                groups[key_features] = []
            
            groups[key_features].append(config)
        
        return list(groups.values())
    
    def _process_simulation_group(self, group_results):
        """Process a group of simulation results"""
        processed = []
        
        for result in group_results:
            if result['success']:
                processed.append({
                    'task_id': result['task_id'],
                    'success': True,
                    'performance_metrics': self._calculate_performance_metrics(result['result']),
                    'config_used': result['config_used'],
                    'execution_time': result['execution_time']
                })
            else:
                processed.append(result)
        
        return processed
    
    def _calculate_performance_metrics(self, simulation_result):
        """Calculate performance metrics for a simulation"""
        metrics = {
            'efficiency_score': 0.0,
            'learning_efficiency': 0.0,
            'exploration_exploitation_ratio': 0.0,
            'stability_score': 0.0,
            'novelty_diversity': 0.0
        }
        
        # Calculate efficiency based on reward per simulation step
        total_reward = simulation_result.get('final_reward', 0)
        total_steps = 1000  # Default
        metrics['efficiency_score'] = total_reward / total_steps if total_steps > 0 else 0
        
        # Learning efficiency based on developmental stage
        stage = simulation_result.get('final_stage')
        stage_values = {
            nlm.DevelopmentalStage.Initial: 0.0,
            nlm.DevelopmentalStage.CriticalPeriod: 0.3,
            nlm.DevelopmentalStage.Maturation: 0.6,
            nlm.DevelopmentalStage.Adult: 0.8,
            nlm.DevelopmentalStage.Aging: 0.9
        }
        metrics['learning_efficiency'] = stage_values.get(stage, 0.0)
        
        # Exploration/exploitation ratio
        curiosity = simulation_result.get('curiosity_level', 0.5)
        novelty = simulation_result.get('novelty_level', 0.5)
        metrics['exploration_exploitation_ratio'] = curiosity / (curiosity + novelty) if (curiosity + novelty) > 0 else 0.5
        
        # Stability based on prediction error
        pred_error = simulation_result.get('prediction_error', 1.0)
        metrics['stability_score'] = 1.0 - pred_error
        
        # Novelty diversity based on curiosity and novelty
        metrics['novelty_diversity'] = (curiosity + novelty) / 2.0
        
        return metrics
    
    def get_batch_statistics(self):
        """Get statistics for batch processing run"""
        if not self.results:
            return {
                'total_simulations': 0,
                'successful_simulations': 0,
                'failed_simulations': 0,
                'success_rate': 0.0,
                'avg_execution_time': 0.0,
                'total_time': 0.0
            }
        
        successful = [r for r in self.results if r['success']]
        failed = [r for r in self.results if not r['success']]
        
        execution_times = [r['execution_time'] for r in self.results if r['success']]
        
        return {
            'total_simulations': len(self.results),
            'successful_simulations': len(successful),
            'failed_simulations': len(failed),
            'success_rate': len(successful) / len(self.results) if self.results else 0,
            'avg_execution_time': sum(execution_times) / len(execution_times) if execution_times else 0,
            'total_time': sum(execution_times),
            'simulations_per_minute': len(self.results) / (sum(execution_times) / 60) if execution_times else 0
        }

# Utility functions for research

def run_parameter_sweep(param_grid, sim_function=None, num_iterations=5):
    """Run parameter sweep across simulation configurations"""
    sim_configs = []
    
    # Generate all parameter combinations
    param_names = list(param_grid.keys())
    param_values = list(param_grid.values())
    
    # Generate combinations (simplified - in practice use itertools.product)
    for i in range(len(param_values[0])):
        config = {}
        for j, param_name in enumerate(param_names):
            config[param_name] = param_values[j][i]
        sim_configs.append(config)
    
    # Create batch processor
    processor = BatchProcessor()
    
    # Run parameter sweep
    results = processor.run_batch_simulation(
        sim_configs, sim_function, num_iterations
    )
    
    return processor.get_batch_statistics(), results

def run_optimization_study(objective_function, param_ranges, iterations=100):
    """Run optimization study using NLM simulations"""
    # Generate parameter combinations for optimization
    sim_configs = generate_optimization_parameters(param_ranges, iterations)
    
    # Define simulation function for optimization
    def optimization_simulation(config, task):
        brain = pynlm.createAdvancedBrain(config)
        
        # Run quick simulation
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Quick simulation
        for step in range(200):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, 0.0)
        
        # Return objective value
        return objective_function(brain, agent, result)
    
    # Run optimization
    processor = BatchProcessor()
    results = processor.run_batch_simulation(
        sim_configs, optimization_simulation, num_iterations=1
    )
    
    # Extract best parameters
    best_result = max([r for r in results if r['success']], 
                     key=lambda x: x['result'], default=None)
    
    return {
        'best_parameters': best_result['config_used'] if best_result else None,
        'best_score': best_result['result'] if best_result else None,
        'statistics': processor.get_batch_statistics(),
        'all_results': results
    }

def generate_optimization_parameters(param_ranges, num_combinations):
    """Generate parameter combinations for optimization"""
    import random
    
    param_names = list(param_ranges.keys())
    combinations = []
    
    for _ in range(num_combinations):
        config = {}
        for param_name in param_names:
            param_range = param_ranges[param_name]
            if isinstance(param_range, tuple) and len(param_range) == 2:
                # Continuous parameter
                config[param_name] = random.uniform(param_range[0], param_range[1])
            elif isinstance(param_range, list):
                # Discrete parameter
                config[param_name] = random.choice(param_range)
            else:
                # Fixed parameter
                config[param_name] = param_range
        
        combinations.append(config)
    
    return combinations

# High-performance simulation utilities

class SimulationProfiler:
    def __init__(self):
        self.profiling_data = []
        self.start_time = None
        self.end_time = None
    
    def start_profiling(self):
        """Start profiling a simulation"""
        import time
        self.start_time = time.time()
        self.profiling_data = {
            'start_time': self.start_time,
            'steps_completed': 0,
            'current_firing_rate': 0,
            'memory_usage': {},
            'cpu_time': 0
        }
    
    def update_metrics(self, brain, world, step):
        """Update profiling metrics during simulation"""
        import time
        
        if self.profiling_data:
            self.profiling_data['steps_completed'] = step
            self.profiling_data['current_firing_rate'] = brain.getFiringNeuronCount()
            
            # Simple memory monitoring (platform-specific)
            import psutil
            process = psutil.Process()
            self.profiling_data['memory_usage'] = {
                'rss': process.memory_info().rss,
                'vms': process.memory_info().vms
            }
            
            self.profiling_data['cpu_time'] = time.time() - self.start_time
    
    def stop_profiling(self):
        """Stop profiling and return results"""
        if self.start_time:
            import time
            self.end_time = time.time()
            
            self.profiling_data['end_time'] = self.end_time
            self.profiling_data['total_time'] = self.end_time - self.start_time
            
            if self.profiling_data['steps_completed'] > 0:
                self.profiling_data['steps_per_second'] = (
                    self.profiling_data['steps_completed'] / 
                    self.profiling_data['total_time']
                )
                self.profiling_data['neurons_per_second'] = (
                    self.profiling_data['current_firing_rate'] / 
                    self.profiling_data['total_time']
                )
            
            return self.profiling_data
        
        return None
    
    def get_performance_summary(self, profiling_data):
        """Generate performance summary"""
        if not profiling_data:
            return None
        
        summary = {
            'total_time': profiling_data['total_time'],
            'steps_completed': profiling_data['steps_completed'],
            'steps_per_second': profiling_data.get('steps_per_second', 0),
            'peak_firing_rate': profiling_data['current_firing_rate'],
            'memory_usage_mb': profiling_data['memory_usage']['rss'] / (1024 * 1024),
            'cpu_time': profiling_data['cpu_time']
        }
        
        return summary

# Export utilities

def export_simulation_results(results, filename, format='json'):
    """Export simulation results to file"""
    import json
    import pickle
    import csv
    
    if format == 'json':
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
    elif format == 'pickle':
        with open(filename, 'wb') as f:
            pickle.dump(results, f)
    elif format == 'csv':
        # Flatten results for CSV export
        flattened = []
        for result in results:
            if isinstance(result, dict):
                flattened.append(result)
        
        if flattened:
            keys = flattened[0].keys()
            with open(filename, 'w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=keys)
                writer.writeheader()
                writer.writerows(flattened)
    
    return filename

def import_simulation_results(filename, format='json'):
    """Import simulation results from file"""
    import json
    import pickle
    import csv
    
    if format == 'json':
        with open(filename, 'r') as f:
            return json.load(f)
    elif format == 'pickle':
        with open(filename, 'rb') as f:
            return pickle.load(f)
    elif format == 'csv':
        with open(filename, 'r') as f:
            reader = csv.DictReader(f)
            return list(reader)
    
    return None