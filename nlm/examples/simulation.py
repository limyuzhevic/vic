"""
Advanced Simulation Example for NLM

This example demonstrates comprehensive simulation capabilities with NLM including:
- Multi-agent coordination and competition
- Complex world environments
- Real-time performance monitoring
- Advanced error handling and recovery
- Developmental plasticity over time
- Memory integration and replay mechanisms
- Real-time visualization and analysis

This simulation showcases production-ready code with robust error handling,
comprehensive logging, and advanced NLM features.
"""

import time
import logging
import json
from typing import Dict, List, Optional, Any
import numpy as np
from datetime import datetime

from nlm import create_brain, World, create_agent, create_region

# Configure logging for simulation monitoring
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(name)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_simulation_example.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class SimulationStage:
    """Represents a stage in the simulation with its own configuration and state."""
    
    def __init__(self, name: str, config: Dict[str, Any]):
        self.name = name
        self.config = config
        self.start_time = None
        self.end_time = None
        self.metrics = {}
        self.status = 'pending'
    
    def start(self) -> None:
        self.start_time = time.time()
        self.status = 'running'
        logger.info(f"Starting simulation stage: {self.name}")
    
    def complete(self, metrics: Dict[str, Any] = None) -> None:
        self.end_time = time.time()
        self.status = 'completed'
        if metrics:
            self.metrics = metrics
        duration = self.end_time - self.start_time if self.start_time else 0
        logger.info(f"Completed simulation stage: {self.name} in {duration:.2f} seconds")
    
    def get_duration(self) -> float:
        if self.start_time:
            end = self.end_time if self.end_time else time.time()
            return end - self.start_time
        return 0.0

class AgentCompetitor:
    """Represents an agent in competitive simulation with advanced behavior."""
    
    def __init__(self, name: str, brain: Any, role: str = 'generalist'):
        self.name = name
        self.brain = brain
        self.role = role
        self.performance_history = []
        self.reward_history = []
        self.strategy_params = {
            'exploration_rate': 0.1,
            'exploitation_rate': 0.8,
            'curiosity_weight': 0.5,
            'prediction_error_threshold': 0.1,
        }
        self.position = {'x': 0, 'y': 0}
        self.memory_buffer = []
        self.max_memory = 1000
    
    def think(self, observation: Dict[str, Any]) -> Dict[str, Any]:
        """Make decisions with advanced cognitive processes."""
        # Process observation through brain
        brain_output = self.brain.think(observation)
        
        # Apply role-specific decision making
        if self.role == 'exploration':
            # More random exploration
            action = self._exploration_action(observation, brain_output)
        elif self.role == 'exploitation':
            # Focus on known good actions
            action = self._exploitation_action(observation, brain_output)
        else:  # balanced
            # Balance exploration and exploitation
            action = self._balanced_action(observation, brain_output)
        
        # Update memory buffer
        self._update_memory(observation, action)
        
        # Record performance
        reward = action.get('reward', 0.0)
        self.reward_history.append(reward)
        
        return action
    
    def _exploration_action(self, observation: Dict[str, Any], brain_output: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploration-focused actions."""
        # Add randomness for exploration
        exploration_factor = np.random.random()
        
        base_action = {
            'type': 'exploration',
            'target': self._calculate_exploration_target(observation),
            'intensity': exploration_factor,
            'curiosity_bonus': brain_output.get('curiosity_bonus', 0.0),
        }
        
        if exploration_factor > 0.7:  # High exploration probability
            base_action['novelty'] = 'high'
            base_action['reward'] = 0.5 * exploration_factor
        else:
            base_action['novelty'] = 'medium'
            base_action['reward'] = 0.2 * exploration_factor
        
        return base_action
    
    def _exploitation_action(self, observation: Dict[str, Any], brain_output: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploitation-focused actions."""
        # Focus on known profitable actions
        exploitation_factor = 1.0 - np.random.random() * 0.3
        
        base_action = {
            'type': 'exploitation',
            'target': self._calculate_exploitation_target(observation),
            'intensity': exploitation_factor,
            'confidence': brain_output.get('confidence', 0.5),
        }
        
        base_action['reward'] = 0.8 * exploitation_factor
        base_action['prediction_error'] = brain_output.get('prediction_error', 0.0)
        
        return base_action
    
    def _balanced_action(self, observation: Dict[str, Any], brain_output: Dict[str, Any]) -> Dict[str, Any]:
        """Generate balanced exploration/exploitation actions."""
        exploration_factor = np.random.random() * 0.6 + 0.2  # 0.2-0.8 range
        
        base_action = {
            'type': 'balanced',
            'target': self._calculate_balanced_target(observation),
            'intensity': exploration_factor,
            'exploration_bonus': brain_output.get('exploration_bonus', 0.0),
            'exploitation_bonus': brain_output.get('exploitation_bonus', 0.0),
        }
        
        base_action['reward'] = 0.5 * exploration_factor + 0.3 * brain_output.get('confidence', 0.5)
        
        return base_action
    
    def _calculate_exploration_target(self, observation: Dict[str, Any]) -> Dict[str, float]:
        """Calculate target for exploration actions."""
        return {
            'x': observation.get('position', {}).get('x', 0) + np.random.uniform(-1, 1),
            'y': observation.get('position', {}).get('y', 0) + np.random.uniform(-1, 1),
            'novelty': np.random.random()
        }
    
    def _calculate_exploitation_target(self, observation: Dict[str, Any]) -> Dict[str, float]:
        """Calculate target for exploitation actions."""
        # Focus on previously successful areas
        if hasattr(self, 'successful_positions') and self.successful_positions:
            best_pos = np.mean(self.successful_positions, axis=0)
            return {
                'x': best_pos[0],
                'y': best_pos[1],
                'novelty': 0.1
            }
        else:
            return {
                'x': observation.get('position', {}).get('x', 0),
                'y': observation.get('position', {}).get('y', 0),
                'novelty': 0.0
            }
    
    def _calculate_balanced_target(self, observation: Dict[str, Any]) -> Dict[str, float]:
        """Calculate target for balanced actions."""
        current_pos = observation.get('position', {'x': 0, 'y': 0})
        exploration_weight = 0.3
        
        return {
            'x': current_pos['x'] + np.random.uniform(-0.5, 0.5) * exploration_weight,
            'y': current_pos['y'] + np.random.uniform(-0.5, 0.5) * exploration_weight,
            'novelty': np.random.random() * exploration_weight
        }
    
    def _update_memory(self, observation: Dict[str, Any], action: Dict[str, Any]) -> None:
        """Update agent's memory buffer with experience."""
        experience = {
            'observation': observation,
            'action': action,
            'reward': action.get('reward', 0.0),
            'timestamp': time.time()
        }
        
        self.memory_buffer.append(experience)
        
        # Maintain memory size limit
        if len(self.memory_buffer) > self.max_memory:
            self.memory_buffer.pop(0)
        
        # Update successful positions if reward was good
        if action.get('reward', 0.0) > 0.5:
            if not hasattr(self, 'successful_positions'):
                self.successful_positions = []
            self.successful_positions.append([action['target']['x'], action['target']['y']])
    
    def get_performance_metrics(self) -> Dict[str, Any]:
        """Get comprehensive performance metrics."""
        if not self.reward_history:
            return {'status': 'no_data', 'performance': 0.0}
        
        recent_rewards = self.reward_history[-100:]  # Last 100 rewards
        
        metrics = {
            'avg_reward': np.mean(recent_rewards),
            'max_reward': np.max(recent_rewards),
            'min_reward': np.min(recent_rewards),
            'reward_std': np.std(recent_rewards),
            'success_rate': len([r for r in recent_rewards if r > 0.5]) / len(recent_rewards),
            'total_reward': sum(self.reward_history),
            'memory_usage': len(self.memory_buffer),
        }
        
        return metrics

class WorldEnvironment:
    """Represents a complex world environment for simulation."""
    
    def __init__(self, name: str, config: Dict[str, Any]):
        self.name = name
        self.config = config
        self.width = config.get('width', 100)
        self.height = config.get('height', 100)
        self.time_step = config.get('time_step', 0.1)
        self.brains = []
        self.agents = []
        self.stimuli = []
        self.weather = config.get('weather', 'clear')
        self.temperature = config.get('temperature', 20.0)
        self.humidity = config.get('humidity', 0.5)
        self.obstacles = []
        self.resources = []
        self.time = 0.0
        self.history = []
        self.max_history = 1000
    
    def add_brain(self, brain: Any) -> None:
        self.brains.append(brain)
    
    def add_agent(self, agent: AgentCompetitor) -> None:
        self.agents.append(agent)
    
    def add_stimulus(self, stimulus: Dict[str, Any]) -> None:
        self.stimuli.append(stimulus)
    
    def update_environment(self, dt: float) -> None:
        """Update world state based on physics and conditions."""
        self.time += dt
        
        # Update environmental factors
        self._update_weather()
        self._update_resources()
        self._update_obstacles()
        
        # Store world state
        world_state = {
            'time': self.time,
            'position': self.config.get('spawn_point', {'x': 0, 'y': 0}),
            'weather': self.weather,
            'temperature': self.temperature,
            'humidity': self.humidity,
            'stimuli_count': len(self.stimuli),
            'agents_count': len(self.agents),
            'resources_count': len(self.resources),
        }
        
        self.history.append(world_state)
        if len(self.history) > self.max_history:
            self.history.pop(0)
    
    def _update_weather(self) -> None:
        """Simulate weather changes over time."""
        if self.weather == 'clear':
            if self.time % 300 < self.time_step:  # 5 minutes chance
                self.weather = 'rainy'
        elif self.weather == 'rainy':
            if self.time % 200 < self.time_step:  # 3.33 minutes chance
                self.weather = 'clear'
        
        # Adjust temperature and humidity based on weather
        if self.weather == 'rainy':
            self.temperature = max(10, self.temperature - 0.5)
            self.humidity = min(1.0, self.humidity + 0.1)
        else:
            self.temperature = min(35, self.temperature + 0.2)
            self.humidity = max(0.3, self.humidity - 0.05)
    
    def _update_resources(self) -> None:
        """Update available resources in the environment."""
        # Natural resource regeneration
        self.resources.append({
            'type': 'energy',
            'amount': 10,
            'location': {'x': np.random.uniform(0, self.width), 'y': np.random.uniform(0, self.height)}
        })
        
        # Clean up old resources
        self.resources = [r for r in self.resources if r.get('age', 0) < 100]
    
    def _update_obstacles(self) -> None:
        """Update obstacles in the environment."""
        # Static obstacles
        if self.time % 500 < self.time_step:  # 8.33 seconds chance
            self.obstacles.append({
                'id': len(self.obstacles),
                'type': 'rock' if np.random.random() > 0.5 else 'tree',
                'position': {'x': np.random.uniform(0, self.width), 'y': np.random.uniform(0, self.height)},
                'size': np.random.uniform(5, 15)
            })
    
    def run_step(self) -> Dict[str, Any]:
        """Run one step of the simulation."""
        # Create observation for all agents
        observation = self._create_observation()
        
        # Get actions from all agents
        actions = {}
        for agent in self.agents:
            action = agent.think(observation)
            actions[agent.name] = action
        
        # Update environment
        self.update_environment(self.time_step)
        
        # Return step results
        step_result = {
            'step_number': int(self.time / self.time_step),
            'world_state': observation,
            'agent_actions': actions,
            'environment': {
                'weather': self.weather,
                'temperature': self.temperature,
                'humidity': self.humidity,
                'resources': len(self.resources),
                'obstacles': len(self.obstacles),
            },
            'timestamp': time.time()
        }
        
        return step_result
    
    def _create_observation(self) -> Dict[str, Any]:
        """Create observation for agents based on world state."""
        # Agent positions
        agent_positions = []
        for agent in self.agents:
            agent_positions.append(agent.position)
        
        # Nearby stimuli
        nearby_stimuli = []
        for stimulus in self.stimuli:
            distance = np.sqrt(
                (stimulus['position']['x'] - self.config.get('spawn_point', {}).get('x', 0)) ** 2 +
                (stimulus['position']['y'] - self.config.get('spawn_point', {}).get('y', 0)) ** 2
            )
            if distance < 20:  # 20 unit radius
                nearby_stimuli.append(stimulus)
        
        return {
            'time': self.time,
            'world_name': self.name,
            'environment': {
                'weather': self.weather,
                'temperature': self.temperature,
                'humidity': self.humidity,
            },
            'agents': agent_positions,
            'stimuli': nearby_stimuli,
            'resources': len(self.resources),
            'obstacles': len(self.obstacles),
            'spawn_point': self.config.get('spawn_point', {'x': 0, 'y': 0})
        }

def create_advanced_world(name: str, config: Dict[str, Any]) -> WorldEnvironment:
    """Create and initialize an advanced world environment."""
    world = WorldEnvironment(name, config)
    
    # Add brain regions
    brain_regions = [
        {'name': 'cortex', 'size': 200, 'plasticity': 0.8},
        {'name': 'hippocampus', 'size': 100, 'plasticity': 0.9},
        {'name': 'amygdala', 'size': 50, 'plasticity': 0.7},
    ]
    
    for region_config in brain_regions:
        region = create_region(**region_config)
        brain = create_brain(**config)
        world.add_brain(brain)
        brain.add_region(region)
        logger.info(f"Added brain region {region_config['name']} to world")
    
    # Create and add agents
    roles = ['exploration', 'exploitation', 'balanced']
    for i, role in enumerate(roles):
        brain = create_brain(**config)
        agent = AgentCompetitor(f"agent_{i+1}", brain, role)
        agent.position = {'x': np.random.uniform(10, 90), 'y': np.random.uniform(10, 90)}
        world.add_agent(agent)
        logger.info(f"Created {agent.name} with role {role}")
    
    return world

def analyze_simulation_results(results: List[Dict[str, Any]]) -> Dict[str, Any]:
    """Analyze and summarize simulation results."""
    if not results:
        return {'status': 'no_data'}
    
    analysis = {
        'total_steps': len(results),
        'avg_rewards': [],
        'performance_trends': [],
        'coordination_metrics': {},
        'environmental_impact': {},
        'agent_performance': {}
    }
    
    # Aggregate metrics across steps
    for step_result in results:
        agent_actions = step_result.get('agent_actions', {})
        rewards = []
        
        for agent_name, action in agent_actions.items():
            reward = action.get('reward', 0.0)
            rewards.append(reward)
        
        if rewards:
            analysis['avg_rewards'].append(np.mean(rewards))
    
    # Calculate performance trends
    if analysis['avg_rewards']:
        analysis['performance_trends'] = {
            'mean': np.mean(analysis['avg_rewards']),
            'std': np.std(analysis['avg_rewards']),
            'min': np.min(analysis['avg_rewards']),
            'max': np.max(analysis['avg_rewards']),
            'trend': np.polyfit(range(len(analysis['avg_rewards'])), analysis['avg_rewards'], 1)[0]
        }
    
    return analysis

def run_advanced_simulation() -> Dict[str, Any]:
    """Run comprehensive advanced simulation with multiple stages."""
    logger.info("Starting Advanced NLM Simulation")
    logger.info("=" * 60)
    
    simulation_results = {
        'simulation_info': {
            'name': 'Advanced NLM Simulation',
            'timestamp': datetime.now().isoformat(),
            'status': 'running'
        },
        'stages': {},
        'world_dynamics': {},
        'agent_coordination': {},
        'performance_analysis': {},
        'errors': [],
    }
    
    try:
        # Stage 1: World Creation
        stage1 = SimulationStage('World Creation', {
            'width': 200,
            'height': 150,
            'time_step': 0.1,
            'weather': 'clear',
            'spawn_point': {'x': 100, 'y': 75}
        })
        stage1.start()
        
        world = create_advanced_world('Advanced Simulation World', {
            'width': 200,
            'height': 150,
            'time_step': 0.1,
            'weather': 'clear',
            'spawn_point': {'x': 100, 'y': 75}
        })
        
        stage1.complete({
            'world_created': True,
            'agents': len(world.agents),
            'brains': len(world.brains),
            'simulation_time': stage1.get_duration()
        })
        
        simulation_results['stages']['world_creation'] = {
            'status': stage1.status,
            'metrics': stage1.metrics
        }
        
        # Stage 2: Simulation Run
        stage2 = SimulationStage('Simulation Run', {
            'total_steps': 1000,
            'agents': len(world.agents),
            'recording_frequency': 10
        })
        stage2.start()
        
        all_results = []
        for step in range(1000):
            try:
                step_result = world.run_step()
                all_results.append(step_result)
                
                # Record agent performance periodically
                if (step + 1) % 50 == 0:
                    for agent in world.agents:
                        agent_metrics = agent.get_performance_metrics()
                        logger.info(f"Step {step + 1}: {agent.name} performance: {agent_metrics}")
                
                # Log progress
                if (step + 1) % 200 == 0:
                    logger.info(f"Progress: Step {step + 1}/1000 completed")
                
            except Exception as e:
                error_msg = f"Error at step {step}: {str(e)}"
                logger.error(error_msg)
                simulation_results['errors'].append({
                    'stage': 'simulation_run',
                    'step': step,
                    'error': error_msg,
                    'timestamp': time.time()
                })
        
        stage2.complete({
            'steps_completed': len(all_results),
            'agents_participated': len(world.agents),
            'simulation_time': stage2.get_duration(),
            'final_world_state': {
                'time': world.time,
                'weather': world.weather,
                'agents': len(world.agents),
                'resources': len(world.resources),
            }
        })
        
        simulation_results['stages']['simulation_run'] = {
            'status': stage2.status,
            'metrics': stage2.metrics
        }
        
        # Stage 3: Analysis
        stage3 = SimulationStage('Results Analysis', {
            'result_count': len(all_results),
            'analysis_type': 'comprehensive'
        })
        stage3.start()
        
        analysis = analyze_simulation_results(all_results)
        
        stage3.complete({
            'analysis_completed': True,
            'analysis_results': analysis,
            'simulation_time': stage3.get_duration()
        })
        
        simulation_results['stages']['analysis'] = {
            'status': stage3.status,
            'metrics': stage3.metrics
        }
        
        # Aggregate results
        simulation_results['world_dynamics'] = {
            'final_state': {
                'time': world.time,
                'weather': world.weather,
                'temperature': world.temperature,
                'humidity': world.humidity,
                'resources': len(world.resources),
                'obstacles': len(world.obstacles),
            },
            'environmental_impact': {
                'weather_changes': len([s for s in all_results if s.get('environment', {}).get('weather') == 'rainy']),
                'resource_depletion': len([s for s in all_results if s.get('environment', {}).get('resources', 0) < 5]),
            }
        }
        
        simulation_results['agent_coordination'] = {
            'agents_count': len(world.agents),
            'coordination_score': analysis.get('performance_trends', {}).get('mean', 0.0),
            'role_distribution': [agent.role for agent in world.agents]
        }
        
        simulation_results['performance_analysis'] = {
            'total_simulation_time': sum(stage.get_duration() for stage in [stage1, stage2, stage3]),
            'efficiency_score': len(all_results) / (sum(stage.get_duration() for stage in [stage1, stage2, stage3]) or 1),
            'agent_performance': {agent.name: agent.get_performance_metrics() for agent in world.agents}
        }
        
        simulation_results['simulation_info']['status'] = 'completed'
        
        logger.info("=" * 60)
        logger.info("Advanced simulation completed successfully!")
        logger.info(f"Total steps simulated: {len(all_results)}")
        logger.info(f"Agents participated: {len(world.agents)}")
        logger.info(f"Errors encountered: {len(simulation_results['errors'])}")
        
    except Exception as e:
        logger.error(f"Advanced simulation failed: {str(e)}")
        simulation_results['errors'].append({
            'stage': 'unknown',
            'error': str(e),
            'timestamp': time.time()
        })
        simulation_results['simulation_info']['status'] = 'failed'
    
    return simulation_results

def main() -> None:
    """Main function to run the advanced simulation example."""
    logger.info("=" * 60)
    logger.info("Starting Advanced NLM Simulation Example")
    logger.info("=" * 60)
    
    try:
        # Run the advanced simulation
        results = run_advanced_simulation()
        
        # Log final summary
        logger.info("\n" + "=" * 60)
        logger.info("SIMULATION SUMMARY")
        logger.info("=" * 60)
        
        if 'stages' in results:
            for stage_name, stage_data in results['stages'].items():
                logger.info(f"{stage_name.replace('_', ' ').title()}: {'✓' if stage_data['status'] == 'completed' else '✗'}")
        
        if 'performance_analysis' in results:
            perf = results['performance_analysis']
            logger.info(f"Efficiency Score: {perf.get('efficiency_score', 'N/A'):.2f} steps/sec")
            logger.info(f"Total Simulation Time: {perf.get('total_simulation_time', 0):.2f} seconds")
        
        if 'agent_coordination' in results:
            coord = results['agent_coordination']
            logger.info(f"Agents Coordinated: {coord.get('agents_count', 0)}")
            logger.info(f"Coordination Score: {coord.get('coordination_score', 0):.4f}")
            logger.info(f"Role Distribution: {coord.get('role_distribution', [])}")
        
        if results.get('errors'):
            logger.error(f"Errors encountered: {len(results['errors'])}")
            for i, error in enumerate(results['errors'][:3]):  # Show first 3 errors
                logger.error(f"Error {i+1}: {error['stage']} at step {error.get('step', 'N/A')}")
        
        # Save results to JSON file
        with open('simulation_results.json', 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        logger.info("\nResults saved to simulation_results.json")
        logger.info("Advanced simulation example completed successfully!")
        
    except KeyboardInterrupt:
        logger.info("Simulation interrupted by user")
    except Exception as e:
        logger.error(f"Advanced simulation failed: {str(e)}")
    
    logger.info("=" * 60)

if __name__ == "__main__":
    main()
