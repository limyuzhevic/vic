#!/usr/bin/env python3
"""
Advanced NLM (熙然) - Neural Learning Machine
Advanced Usage Example

This comprehensive example demonstrates all the advanced features and improvements
made to the NLM project, showcasing the full power of the neural simulation framework.
"""

import pynlm
import numpy as np
import time
import threading
import json
import os
from pathlib import Path
from typing import Dict, List, Any, Optional
class AdvancedBrainConfigurator:
    """Advanced brain configuration for specialized experiments."""
    
    @staticmethod
    def create_memory_brain() -> pynlm.Config:
        """Create a brain optimized for memory tasks."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 3000)
        config.set("region_count", 8)
        config.set("connection_probability", 0.18)
        config.set("stdp_ltp_weight", 0.025)
        config.set("stdp_ltd_weight", 0.022)
        config.set("synaptogenesis_rate", 0.003)
        config.set("pruning_rate", 0.0005)
        return config
    
    @staticmethod
    def create_learning_brain() -> pynlm.Config:
        """Create a brain optimized for rapid learning."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1500)
        config.set("region_count", 5)
        config.set("connection_probability", 0.25)
        config.set("stdp_ltp_weight", 0.035)
        config.set("stdp_ltd_weight", 0.03)
        config.set("synaptogenesis_rate", 0.005)
        config.set("pruning_rate", 0.001)
        config.set("simulation_timestep", 0.0005)
        return config
    
    @staticmethod
    def create_development_brain() -> pynlm.Config:
        """Create a brain with enhanced developmental features."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 2000)
        config.set("region_count", 6)
        config.set("connection_probability", 0.15)
        config.set("stdp_ltp_weight", 0.02)
        config.set("stdp_ltd_weight", 0.018)
        config.set("synaptogenesis_rate", 0.004)
        config.set("pruning_rate", 0.0003)
        config.set("development_enabled", True)
        return config

class StrategicAgent:
    """Advanced agent with strategic decision making and curiosity."""
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Strategic state
        self.strategic_mode = False
        self.energy_threshold = 0.5
        self.exploration_history = []
        self.exploitation_history = []
    
    def get_strategic_action(self) -> pynlm.MotorCommand:
        """Get action with strategic consideration."""
        base_action = self.agent.decodeMotorCommand()
        
        # Energy management
        if self.agent.getEnergy() < self.energy_threshold:
            if base_action != pynlm.MotorCommand.Rest:
                return pynlm.MotorCommand.Rest
        
        # Curiosity-based exploration
        curiosity = self.agent.getCuriosityLevel()
        if curiosity > 0.7:
            # Balance exploration vs exploitation
            if np.random.random() < 0.3:
                return self.random_action()
        
        return base_action
    
    def random_action(self) -> pynlm.MotorCommand:
        """Get a random action for exploration."""
        actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Look
        ]
        return np.random.choice(actions)
    
    def get_performance_metrics(self) -> Dict[str, Any]:
        """Get comprehensive performance metrics."""
        return {
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'development_stage': self.agent.getDevelopmentalStage(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'energy': self.agent.getEnergy()
        }

class AdvancedRewardSystem:
    """Advanced reward system with multiple reward sources."""
    
    def __init__(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        self.agent = agent
        self.world = world
        self.reward_history = []
        self.prediction_history = []
        self.credit_assignment_window = 20
    
    def calculate_comprehensive_reward(
        self,
        action: pynlm.MotorCommand,
        success: bool,
        immediate_reward: float,
        time_step: float
    ) -> float:
        """Calculate sophisticated reward with multiple factors."""
        
        # Base reward
        reward = immediate_reward
        
        # Action efficiency (cost/benefit analysis)
        if action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward]:
            action_cost = 0.1
        elif action in [pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight]:
            action_cost = 0.2
        else:
            action_cost = 0.05
        
        reward -= action_cost
        
        # Success bonus
        if success:
            reward += 1.5
        
        # Novelty bonus
        novelty = self.agent.getNoveltyLevel()
        if novelty > 0.3:
            reward += novelty * 0.8
        
        # Consistency bonus
        if len(self.reward_history) > 10:
            recent_rewards = self.reward_history[-10:]
            consistency = 1.0 - np.std(recent_rewards) if recent_rewards else 0.0
            reward += consistency * 0.3
        
        # Development stage bonus
        if self.agent.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
            reward += 0.5
        
        self.reward_history.append(reward)
        return reward
    
    def apply_credit_assignment(self, action: pynlm.MotorCommand, reward: float):
        """Apply reward to neural pathways with credit assignment."""
        if len(self.reward_history) > self.credit_assignment_window:
            # Calculate prediction error
            recent_avg = np.mean(self.reward_history[-self.credit_assignment_window:])
            prediction_error = recent_avg - 0.5  # Expected baseline
            
            # Apply to agent's reward modulation
            self.agent.applyRewardModulation(
                prediction_error * 0.7,
                prediction_error * 0.5
            )
class AdvancedSimulationEnvironment:
    """Advanced simulation environment with complex world dynamics."""
    
    def __init__(self, width: int = 100, height: int = 100):
        self.width = width
        self.height = height
        self.world = pynlm.createSimpleWorld()
        self.setup_complex_environment()
        self.agent_body = None
    
    def setup_complex_environment(self):
        """Setup environment with obstacles, resources, and hazards."""
        self.world.configure(
            width=self.width,
            height=self.height,
            visionWidth=16,
            visionHeight=16
        )
        self.world.reset()
        
        # Add agent start position
        agent_start_x = self.width // 2
        agent_start_y = self.height // 2
        self.world.setAgentStart(agent_start_x, agent_start_y)
        
        # Add resources (positive value objects)
        self.add_resources()
        
        # Add hazards (negative value objects)
        self.add_hazards()
        
        # Add static obstacles
        self.add_obstacles()
    
    def add_resources(self, count: int = 20):
        """Add beneficial resources to the environment."""
        import random
        for _ in range(count):
            x = random.randint(10, self.width - 10)
            y = random.randint(10, self.height - 10)
            resource_obj = pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Resource, 1.0
            )
            self.world.addObject(resource_obj)
    
    def add_hazards(self, count: int = 10):
        """Add hazardous objects to the environment."""
        import random
        for _ in range(count):
            x = random.randint(10, self.width - 10)
            y = random.randint(10, self.height - 10)
            hazard_obj = pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Hazard, -1.0
            )
            self.world.addObject(hazard_obj)
    
    def add_obstacles(self):
        """Add static obstacles (walls)."""
        # Create perimeter walls
        for x in range(0, self.width, 5):
            wall = pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, 0.0)
            self.world.addObject(wall)
            wall = pynlm.WorldObject(x, self.height, pynlm.WorldObjectType.Wall, 0.0)
            self.world.addObject(wall)
        
        for y in range(0, self.height, 5):
            wall = pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall, 0.0)
            self.world.addObject(wall)
            wall = pynlm.WorldObject(self.width, y, pynlm.WorldObjectType.Wall, 0.0)
            self.world.addObject(wall)
    
    def get_environment_state(self) -> Dict[str, Any]:
        """Get current state of the environment."""
        return {
            'width': self.width,
            'height': self.height,
            'simulation_time': self.world.getSimulationTime(),
            'agent_position': (self.agent_body.x if self.agent_body else 0,
                             self.agent_body.y if self.agent_body else 0),
            'agent_orientation': self.agent_body.orientation if self.agent_body else 0,
            'simulation_running': True
        }
class AdvancedExperimentRunner:
    """Advanced experiment runner for systematic testing."""
    
    def __init__(self, experiment_name: str, config: pynlm.Config):
        self.name = experiment_name
        self.config = config
        self.results = {}
        self.metrics = []
        self.checkpoints = []
    
    def run_experiment(self, duration_steps: int = 5000) -> Dict[str, Any]:
        """Run a complete experiment."""
        print(f"Starting experiment: {self.name}")
        print(f"Configuration: {self.config.summary()}")
        
        # Setup environment
        environment = AdvancedSimulationEnvironment()
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        agent = StrategicAgent(brain, environment.world)
        reward_system = AdvancedRewardSystem(agent, environment.world)
        
        # Run simulation
        start_time = time.time()
        
        for step in range(duration_steps):
            # Update environment
            environment.world.update(0.1)
            
            # Get sensory input
            percept = environment.world.getSensoryPercept()
            agent.agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step, environment.world.getSimulationTime())
            
            # Agent decides action
            action = agent.get_strategic_action()
            
            # Apply action in environment
            environment.world.applyMotorCommand(action, environment.world.getSimulationTime())
            
            # Calculate reward
            success = self._check_action_success(action, environment)
            immediate_reward = self._calculate_immediate_reward(action, success)
            reward = reward_system.calculate_comprehensive_reward(
                action, success, immediate_reward, environment.world.getSimulationTime()
            )
            
            # Apply reward modulation
            reward_system.apply_credit_assignment(action, reward)
            
            # Record checkpoint
            if step % 500 == 0:
                self.checkpoints.append(self._capture_checkpoint(agent, brain, environment))
            
            # Record metrics
            metrics = agent.get_performance_metrics()
            metrics['step'] = step
            metrics['reward'] = reward
            metrics['action'] = str(action)
            self.metrics.append(metrics)
            
            # Progress output
            if step % 1000 == 0:
                self._print_progress(step, start_time, agent, brain)
        
        end_time = time.time()
        
        # Analyze results
        self.results = self._analyze_experiment_results()
        
        print(f"\nExperiment '{self.name}' completed!")
        print(f"Duration: {end_time - start_time:.2f} seconds")
        print(f"Total steps: {duration_steps}")
        print(f"Results: {self.results}")
        
        return self.results
    
    def _check_action_success(self, action: pynlm.MotorCommand, environment: AdvancedSimulationEnvironment) -> bool:
        """Check if action was successful."""
        # Simple success criteria based on action and environment
        success = True
        
        if action == pynlm.MotorCommand.Interact:
            # Check if there's something to interact with
            pass
        
        return success
    
    def _calculate_immediate_reward(self, action: pynlm.MotorCommand, success: bool) -> float:
        """Calculate immediate reward based on action."""
        base_reward = 0.0
        
        if success:
            base_reward += 1.0
        
        if action == pynlm.MotorCommand.Interact:
            base_reward += 0.5
        elif action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward]:
            base_reward += 0.1
        
        return base_reward
    
    def _capture_checkpoint(self, agent: StrategicAgent, brain: pynlm.Brain, environment: AdvancedSimulationEnvironment) -> Dict[str, Any]:
        """Capture simulation checkpoint."""
        return {
            'step': len(self.metrics),
            'brain_state': {
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            },
            'agent_state': agent.get_performance_metrics(),
            'environment_state': environment.get_environment_state()
        }
    
    def _print_progress(self, step: int, start_time: float, agent: StrategicAgent, brain: pynlm.Brain):
        """Print experiment progress."""
        elapsed = time.time() - start_time
        steps_per_second = step / elapsed if elapsed > 0 else 0
        
        print(f"Step {step}/{5000} ({elapsed:.1f}s, {steps_per_second:.1f} steps/sec)")
        print(f"  Spikes: {brain.getTotalSpikeCount()}, "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Stage: {agent.agent.getDevelopmentalStage()}, "
              f"Curiosity: {agent.agent.getCuriosityLevel():.2f}")
    
    def _analyze_experiment_results(self) -> Dict[str, Any]:
        """Analyze and compile experiment results."""
        if not self.metrics:
            return {}
        
        # Analyze performance trends
        spike_trend = self._calculate_trend(
            [m['spike_count'] for m in self.metrics]
        )
        
        curiosity_trend = self._calculate_trend(
            [m['curiosity_level'] for m in self.metrics]
        )
        
        reward_trend = self._calculate_trend(
            [m['reward'] for m in self.metrics]
        )
        
        # Analyze action patterns
        action_counts = {}
        for m in self.metrics:
            action = m['action']
            action_counts[action] = action_counts.get(action, 0) + 1
        
        return {
            'performance': {
                'final_spike_count': self.metrics[-1]['spike_count'],
                'final_firing_rate': self.metrics[-1]['average_firing_rate'],
                'spike_trend': spike_trend,
                'curiosity_trend': curiosity_trend,
                'reward_trend': reward_trend
            },
            'behavior': {
                'action_distribution': action_counts,
                'total_steps': len(self.metrics),
                'final_development_stage': self.metrics[-1]['development_stage']
            },
            'checkpoints': len(self.checkpoints),
            'runtime': {
                'total_metrics': len(self.metrics),
                'checkpoints_captured': len(self.checkpoints)
            }
        }
    
    def _calculate_trend(self, values: List[float]) -> str:
        """Calculate trend direction."""
        if len(values) < 3:
            return "insufficient_data"
        
        # Simple linear regression
        n = len(values)
        x = list(range(n))
        y = values
        
        x_mean = sum(x) / n
        y_mean = sum(y) / n
        
        numerator = sum((x[i] - x_mean) * (y[i] - y_mean) for i in range(n))
        denominator = sum((x[i] - x_mean) ** 2 for i in range(n))
        
        if denominator == 0:
            return "stable"
        
        slope = numerator / denominator
        
        if slope > 0.01:
            return "increasing"
        elif slope < -0.01:
            return "decreasing"
        else:
            return "stable"
class ExperimentManager:
    """Manager for running multiple experiments and comparing results."""
    
    def __init__(self, results_dir: str = "experiment_results"):
        self.results_dir = Path(results_dir)
        self.results_dir.mkdir(exist_ok=True)
        self.experiments = {}
    
    def run_experiment_set(self, experiment_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Run a set of experiments and compare results."""
        print(f"Running {len(experiment_configs)} experiments...")
        
        results = {}
        
        for i, config in enumerate(experiment_configs):
            print(f"\n--- Experiment {i+1}/{len(experiment_configs)} ---")
            
            experiment_name = config.get('name', f'experiment_{i}')
            brain_config = config.get('config')
            duration = config.get('duration_steps', 3000)
            
            if not brain_config:
                print(f"Warning: No config provided for experiment {experiment_name}")
                continue
            
            # Create and run experiment
            experiment = AdvancedExperimentRunner(experiment_name, brain_config)
            experiment_results = experiment.run_experiment(duration)
            
            results[experiment_name] = {
                'experiment': experiment,
                'results': experiment_results,
                'config': brain_config
            }
        
        # Save all results
        self._save_results(results)
        
        # Generate comparison report
        comparison = self._generate_comparison(results)
        
        return {
            'individual_results': results,
            'comparison': comparison
        }
    
    def _save_results(self, results: Dict[str, Any]):
        """Save experiment results to files."""
        for name, data in results.items():
            filename = self.results_dir / f"{name}_{int(time.time())}.json"
            
            with open(filename, 'w') as f:
                json.dump({
                    'name': name,
                    'config': data['config'].summary() if hasattr(data['config'], 'summary') else str(data['config']),
                    'results': data['results']
                }, f, indent=2)
            
            print(f"Results saved to: {filename}")
    
    def _generate_comparison(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate comparison report across experiments."""
        if len(results) < 2:
            return {'message': 'Insufficient experiments for comparison'}
        
        comparison = {
            'experiments': list(results.keys()),
            'performance_ranking': [],
            'config_comparison': {}
        }
        
        # Performance ranking based on spike count and firing rate
        performance_scores = []
        for name, data in results.items():
            exp_results = data['results']
            perf = exp_results.get('performance', {})
            
            # Calculate composite score
            spike_count = perf.get('final_spike_count', 0)
            firing_rate = perf.get('final_firing_rate', 0)
            
            score = spike_count * 0.7 + firing_rate * 100 * 0.3
            performance_scores.append((name, score, perf))
        
        # Sort by performance score
        performance_scores.sort(key=lambda x: x[1], reverse=True)
        
        comparison['performance_ranking'] = [
            {'name': name, 'score': score, 'details': details}
            for name, score, details in performance_scores
        ]
        
        return comparison
class ConfigurationManager:
    """Manage experiment configurations and save/load."""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
    
    def save_configuration(self, config: pynlm.Config, name: str) -> str:
        """Save configuration to file."""
        filename = self.config_dir / f"{name}.json"
        
        config_dict = {}
        for key in config.getKeys():
            config_dict[key] = config.getOr(key, None)
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        print(f"Configuration saved to: {filename}")
        return str(filename)
    
    def load_configuration(self, name: str) -> pynlm.Config:
        """Load configuration from file."""
        filename = self.config_dir / f"{name}.json"
        
        if not filename.exists():
            raise FileNotFoundError(f"Configuration file not found: {filename}")
        
        with open(filename, 'r') as f:
            config_dict = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                config.set(key, value)
            elif isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
        
        print(f"Configuration loaded from: {filename}")
        return config
def create_example_experiments() -> List[Dict[str, Any]]:
    """Create example experiment configurations."""
    configurator = AdvancedBrainConfigurator()
    
    return [
        {
            'name': 'memory_optimization',
            'config': configurator.create_memory_brain(),
            'duration_steps': 4000,
            'description': 'Brain optimized for memory tasks'
        },
        {
            'name': 'rapid_learning',
            'config': configurator.create_learning_brain(),
            'duration_steps': 3000,
            'description': 'Brain optimized for rapid learning'
        },
        {
            'name': 'developmental_focus',
            'config': configurator.create_development_brain(),
            'duration_steps': 5000,
            'description': 'Brain with enhanced developmental features'
        }
    ]
def main():
    """Main function demonstrating advanced NLM usage."""
    print("=" * 60)
    print("Advanced NLM (熙然) - Neural Learning Machine")
    print("Comprehensive Usage Example")
    print("=" * 60)
    
    # Setup
    config_manager = ConfigurationManager()
    experiment_manager = ExperimentManager()
    
    # Create example experiments
    experiments = create_example_experiments()
    
    print(f"\nCreated {len(experiments)} example experiments:")
    for exp in experiments:
        print(f"  - {exp['name']}: {exp['description']}")
    
    # Save configurations
    print("\nSaving experiment configurations...")
    for exp in experiments:
        try:
            config_manager.save_configuration(exp['config'], exp['name'])
        except Exception as e:
            print(f"Warning: Could not save config for {exp['name']}: {e}")
    
    # Run selected experiment (using first as demo)
    selected_experiment = experiments[0]
    print(f"\n{'='*60}")
    print(f"Running selected experiment: {selected_experiment['name']}")
    print(f"{'='*60}")
    
    try:
        # Run experiment
        experiment = AdvancedExperimentRunner(
            selected_experiment['name'],
            selected_experiment['config']
        )
        
        results = experiment.run_experiment(
            duration_steps=selected_experiment['duration_steps']
        )
        
        print(f"\nExperiment completed successfully!")
        print(f"Final performance: {results.get('performance', {})}")
        
    except KeyboardInterrupt:
        print("\nExperiment interrupted by user")
    except Exception as e:
        print(f"\nError during experiment: {e}")
        import traceback
        traceback.print_exc()
    
    print("\n" + "="*60)
    print("Advanced NLM example completed!")
    print("="*60)
if __name__ == "__main__":
    main()
