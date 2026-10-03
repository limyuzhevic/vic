# NLM Advanced Documentation

## Overview

This document provides comprehensive documentation for the NLM Phase 6 integrated artificial brain system. It covers advanced usage patterns, integration examples, CLI documentation, performance guidelines, research methods, and troubleshooting.

## Advanced Python Guide

### 1. Batch Processing with Python

This example demonstrates advanced batch processing capabilities for training multiple agent instances simultaneously:

```python
import pynlm
import numpy as np
from typing import List, Dict, Any

def create_batch_of_agents(agent_configs: List[Dict[str, Any]]) -> List[pynlm.AgentBrain]:
    """Create multiple agent brains with different configurations.
    
    Args:
        agent_configs: List of configuration dictionaries for each agent
        
    Returns:
        List of initialized AgentBrain instances
    """
    agents = []
    
    for i, config in enumerate(agent_configs):
        # Create brain with specific configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize with world
        world = pynlm.createSimulationWorld(100, 100)
        agent.initialize(world)
        
        # Enable all learning modules
        agent.enableAllModules()
        
        agents.append(agent)
    
    return agents

def batch_process_multiple_episodes(
    agents: List[pynlm.AgentBrain],
    episodes_per_agent: int,
    max_steps: int
) -> Dict[str, List[float]]:
    """Process multiple agents in batch, collecting performance metrics.
    
    Args:
        agents: List of agent brains to train
        episodes_per_agent: Number of episodes per agent
        max_steps: Maximum steps per episode
        
    Returns:
        Dictionary of performance metrics per agent
    """
    results = {}
    
    for agent_idx, agent in enumerate(agents):
        agent_rewards = []
        agent_learning_curves = []
        
        for episode in range(episodes_per_agent):
            # Create fresh world for each episode
            world = pynlm.createSimulationWorld(100, 100)
            agent.initialize(world)
            
            episode_reward = 0.0
            learning_curve = []
            
            # Run episode
            for step in range(max_steps):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain = agent.getBrain()
                brain.step(step)
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                
                # Apply action in world
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = result.reward if result else 0.0
                episode_reward += reward
                agent.applyRewardModulation(reward, 0.0)
                
                # Update world
                world.update(0.1)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Record learning metrics
                learning_curve.append({
                    'step': step,
                    'reward': reward,
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel()
                })
            
            agent_rewards.append(episode_reward)
            agent_learning_curves.append(learning_curve)
        
        results[f'agent_{agent_idx}'] = {
            'total_rewards': agent_rewards,
            'avg_reward_per_episode': np.mean(agent_rewards),
            'std_reward_per_episode': np.std(agent_rewards),
            'learning_curve': agent_learning_curves
        }
    
    return results

# Example usage
if __name__ == "__main__":
    # Create different agent configurations
    agent_configs = [
        {  # Small agent
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.05,
            'learning_rate': 0.01
        },
        {  # Medium agent  
            'neuron_count': 2000,
            'region_count': 4,
            'connection_probability': 0.02,
            'learning_rate': 0.005
        },
        {  # Large agent
            'neuron_count': 5000,
            'region_count': 8,
            'connection_probability': 0.01,
            'learning_rate': 0.001
        }
    ]
    
    # Create batch of agents
    agents = create_batch_of_agents(agent_configs)
    print(f"Created {len(agents)} agents for batch training")
    
    # Run batch processing
    results = batch_process_multiple_episodes(
        agents=agents,
        episodes_per_agent=10,
        max_steps=1000
    )
    
    # Analyze results
    print("\n=== Batch Processing Results ===")
    for agent_name, metrics in results.items():
        print(f"\n{agent_name}:")
        print(f"  Average reward per episode: {metrics['avg_reward_per_episode']:.2f}")
        print(f"  Reward std: {metrics['std_reward_per_episode']:.2f}")
        print(f"  Total episodes: {len(metrics['total_rewards'])}")
```

### 2. Serialization and Model Persistence

This example demonstrates advanced serialization features for saving and loading brain states:

```python
import pynlm
import json
import time
from pathlib import Path

def save_brain_checkpoint(
    brain: pynlm.Brain,
    agent: pynlm.AgentBrain,
    world: pynlm.SimpleWorld,
    base_path: str,
    metadata: Dict[str, Any] = None
) -> str:
    """Save brain, agent, and world state with metadata.
    
    Args:
        brain: Brain instance to save
        agent: AgentBrain instance to save
        world: SimpleWorld instance to save
        base_path: Base directory path
        metadata: Optional metadata to save with brain state
        
    Returns:
        Path to saved checkpoint file
    """
    Path(base_path).mkdir(parents=True, exist_ok=True)
    
    timestamp = int(time.time())
    checkpoint_path = f"{base_path}/checkpoint_{timestamp}"
    
    # Save brain state
    brain_path = f"{checkpoint_path}_brain.bin"
    agent_path = f"{checkpoint_path}_agent.json"
    world_path = f"{checkpoint_path}_world.json"
    metadata_path = f"{checkpoint_path}_metadata.json"
    
    brain.save(brain_path)
    
    # Save agent state (JSON serialization)
    agent_stats = {
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'developmental_stage': agent.getDevelopmentalStage().name,
        'reward_modulation_enabled': agent.isRewardModulationEnabled(),
        'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
        'development_enabled': agent.isDevelopmentEnabled(),
        'curiosity_enabled': agent.isCuriosityEnabled()
    }
    
    with open(agent_path, 'w') as f:
        json.dump(agent_stats, f, indent=2)
    
    # Save world state
    world_stats = {
        'width': world.getWidth(),
        'height': world.getHeight(),
        'simulation_time': world.getSimulationTime(),
        'max_energy': world.getMaxEnergy()
    }
    
    with open(world_path, 'w') as f:
        json.dump(world_stats, f, indent=2)
    
    # Save metadata
    if metadata:
        full_metadata = {
            'timestamp': timestamp,
            'brain_stats': pynlm.getBrainStats(brain),
            'region_stats': pynlm.getRegionStats(brain),
            'experiment_info': metadata
        }
        
        with open(metadata_path, 'w') as f:
            json.dump(full_metadata, f, indent=2)
    
    print(f"Saved checkpoint to: {checkpoint_path}")
    return checkpoint_path

def load_brain_checkpoint(
    checkpoint_path: str,
    brain: pynlm.Brain,
    agent: pynlm.AgentBrain,
    world: pynlm.SimpleWorld
) -> Dict[str, Any]:
    """Load brain, agent, and world state from checkpoint.
    
    Args:
        checkpoint_path: Path to checkpoint directory
        brain: Brain instance to load into
        agent: AgentBrain instance to load into
        world: SimpleWorld instance to load into
        
    Returns:
        Loaded metadata
    """
    # Load brain state
    brain_path = f"{checkpoint_path}_brain.bin"
    agent_path = f"{checkpoint_path}_agent.json"
    world_path = f"{checkpoint_path}_world.json"
    metadata_path = f"{checkpoint_path}_metadata.json"
    
    brain.load(brain_path)
    
    # Load agent state
    with open(agent_path, 'r') as f:
        agent_stats = json.load(f)
    
    # Load world state
    with open(world_path, 'r') as f:
        world_stats = json.load(f)
    
    # Load metadata
    metadata = {}
    if Path(metadata_path).exists():
        with open(metadata_path, 'r') as f:
            metadata = json.load(f)
    
    print(f"Loaded checkpoint from: {checkpoint_path}")
    return metadata

# Advanced serialization example
if __name__ == "__main__":
    # Create and configure a brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimulationWorld(100, 100)
    agent.initialize(world)
    
    # Enable all modules for advanced training
    agent.enableAllModules()
    
    # Run some training to create interesting state
    for step in range(500):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    # Save checkpoint with metadata
    checkpoint = save_brain_checkpoint(
        brain=brain,
        agent=agent,
        world=world,
        base_path="./checkpoints",
        metadata={
            'experiment': 'advanced_training',
            'duration': 500,
            'final_reward': 250.0,
            'agent_type': 'actor_critic'
        }
    )
    
    # Later, load the checkpoint
    loaded_metadata = load_brain_checkpoint(checkpoint, brain, agent, world)
    print(f"Experiment: {loaded_metadata.get('experiment_info', {}).get('experiment', 'unknown')}")
```

### 3. Visualization and Analytics

This example demonstrates advanced visualization and analytics capabilities:

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def create_advanced_visualization(
    brain: pynlm.Brain,
    agent: pynlm.AgentBrain,
    world: pynlm.SimpleWorld,
    output_dir: str = "./visualizations"
):
    """Create comprehensive visualizations of brain and agent state.
    
    Args:
        brain: Brain instance to visualize
        agent: AgentBrain instance for additional metrics
        world: SimpleWorld instance
        output_dir: Directory for output files
    """
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    
    # Get brain statistics
    brain_stats = pynlm.getBrainStats(brain)
    region_stats = pynlm.getRegionStats(brain)
    
    # Create comprehensive visualization
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('NLM Brain State Visualization', fontsize=16)
    
    # 1. Neural population activity
    ax1 = axes[0, 0]
    populations = []
    firing_rates = []
    for region in region_stats:
        populations.append(region['id'])
        firing_rates.append(float(region['synapse_count']) / max(1, float(region['neuron_count'])))
    
    bars = ax1.bar(range(len(populations)), firing_rates, color='skyblue', alpha=0.7)
    ax1.set_xlabel('Region ID')
    ax1.set_ylabel('Synaptic Density')
    ax1.set_title('Neural Population Activity')
    ax1.set_xticks(range(len(populations)))
    ax1.set_xticklabels(populations)
    
    # 2. Learning metrics over time
    ax2 = axes[0, 1]
    # Generate synthetic learning curve
    steps = np.arange(0, 1000, 10)
    learning_curve = 50 * (1 - np.exp(-steps/200)) + np.random.normal(0, 5, len(steps))
    ax2.plot(steps, learning_curve, 'b-', linewidth=2, label='Learning Progress')
    ax2.set_xlabel('Training Steps')
    ax2.set_ylabel('Performance Score')
    ax2.set_title('Learning Curve')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 3. Neuromodulation levels
    ax3 = axes[0, 2]
    modules = ['Curiosity', 'Novelty', 'Prediction Error', 'Development']
    levels = [
        agent.getCuriosityLevel(),
        agent.getNoveltyLevel(),
        agent.getPredictionError(),
        0.5  # Placeholder for development level
    ]
    colors = ['red', 'orange', 'yellow', 'green']
    bars = ax3.bar(modules, levels, color=colors, alpha=0.7)
    ax3.set_ylabel('Level')
    ax3.set_title('Neuromodulation Levels')
    ax3.set_ylim(0, 1)
    for bar, level in zip(bars, levels):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                f'{level:.2f}', ha='center', va='bottom')
    
    # 4. Developmental stages
    ax4 = axes[1, 0]
    developmental_stages = [
        'Initial', 'Critical', 'Maturation', 'Adult', 'Aging'
    ]
    # Simulate developmental progression
    stage_values = [0.2, 0.5, 0.8, 1.0, 0.6]
    bars = ax4.barh(developmental_stages, stage_values, color='steelblue', alpha=0.7)
    ax4.set_xlabel('Development Level')
    ax4.set_title('Developmental Progression')
    ax4.set_xlim(0, 1.2)
    
    # 5. Memory integration visualization
    ax5 = axes[1, 1]
    memory_types = ['Working Memory', 'Episodic Memory', 'Semantic Memory']
    memory_values = [0.7, 0.5, 0.3]
    colors = ['gold', 'lime', 'cyan']
    wedges, texts, autotexts = ax5.pie(memory_values, labels=memory_types, 
                                     colors=colors, autopct='%1.1f%%',
                                     startangle=90, explode=(0.05, 0.05, 0.05))
    ax5.set_title('Memory System Integration')
    
    # 6. Performance metrics
    ax6 = axes[1, 2]
    metrics = ['Neurons', 'Synapses', 'Active Neurons', 'Firing Rate', 'Spike Count']
    values = [
        brain_stats.get('total_neurons', 0),
        brain_stats.get('total_synapses', 0),
        brain_stats.get('active_neurons', 0),
        float(brain_stats.get('average_firing_rate', 0)),
        brain_stats.get('total_spikes', 0)
    ]
    
    # Normalize for better visualization
    max_val = max(values) if values else 1
    normalized_values = [v/max_val * 100 for v in values]
    
    bars = ax6.barh(metrics, normalized_values, color='purple', alpha=0.7)
    ax6.set_xlabel('Normalized Value (%)')
    ax6.set_title('System Performance Metrics')
    ax6.set_xlim(0, 100)
    
    # Add value labels
    for i, (bar, value) in enumerate(zip(bars, values)):
        width = bar.get_width()
        ax6.text(width + 1, bar.get_y() + bar.get_height()/2.,
                f'{value}', ha='left', va='center', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(f"{output_dir}/brain_state_visualization.png", 
                dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Saved visualization to: {output_dir}/brain_state_visualization.png")
    
    # Generate statistical summary
    return {
        'brain_stats': brain_stats,
        'region_count': len(region_stats),
        'avg_curiosity': agent.getCuriosityLevel(),
        'avg_novelty': agent.getNoveltyLevel(),
        'avg_prediction_error': agent.getPredictionError(),
        'visualization_saved': True
    }

# Advanced visualization example
if __name__ == "__main__":
    # Create and train an agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimulationWorld(100, 100)
    agent.initialize(world)
    
    agent.enableAllModules()
    
    # Train for a while
    for step in range(1000):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    # Create visualization
    metrics = create_advanced_visualization(brain, agent, world)
    print(f"Visualization created with {metrics['region_count']} regions")
```

### 4. Hybrid Python/C++ Integration

This example demonstrates hybrid Python/C++ integration for complex workflows:

```python
import pynlm
import numpy as np
import json
from pathlib import Path
import subprocess
import sys

class NLPHybridWorkflow:
    """Hybrid Python/C++ workflow manager for advanced NLM operations."""
    
    def __init__(self, config_path: str = None):
        """Initialize hybrid workflow.
        
        Args:
            config_path: Optional path to configuration file
        """
        self.config_path = config_path
        self.workflow_history = []
        
        if config_path:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(config_path)
            self.config = config
        else:
            self.config = pynlm.createDefaultConfig()
    
    def load_c_plus_plus_module(self, module_path: str) -> Any:
        """Load C++ extension module if available.
        
        Args:
            module_path: Path to C++ module
            
        Returns:
            Module object or None if not available
        """
        try:
            # Try to import custom C++ module
            import importlib.util
            spec = importlib.util.spec_from_file_location("nlm_module", module_path)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            return module
        except ImportError:
            print(f"Warning: Could not load C++ module from {module_path}")
            return None
    
    def run_c_plus_plus_benchmark(self, benchmark_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run C++ benchmark using subprocess.
        
        Args:
            benchmark_config: Benchmark configuration
            
        Returns:
            Benchmark results
        """
        cmd = [
            'python', '-m', 'pip', 'install', '-e', '.', '--no-build-isolation'
        ]
        
        try:
            # Install package in development mode
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            
            if result.returncode != 0:
                print(f"Installation failed: {result.stderr}")
                return {'success': False, 'error': result.stderr}
            
            # Run benchmark script
            benchmark_script = f"{benchmark_config.get('script', 'benchmark.py')}"
            
            if Path(benchmark_script).exists():
                cmd = [sys.executable, benchmark_script]
                cmd.extend(['--neurons', str(benchmark_config.get('neurons', 1000))])
                cmd.extend(['--steps', str(benchmark_config.get('steps', 1000))])
                
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
                
                if result.returncode == 0:
                    # Parse benchmark output
                    output_lines = result.stdout.strip().split('\n')
                    benchmark_results = {}
                    
                    for line in output_lines:
                        if ':' in line and any(metric in line.lower() for metric in 
                                            ['time', 'memory', 'neurons', 'spikes']):
                            parts = line.split(':')
                            if len(parts) == 2:
                                key = parts[0].strip().lower().replace(' ', '_')
                                value = parts[1].strip()
                                benchmark_results[key] = value
                    
                    return {'success': True, 'results': benchmark_results, 'output': result.stdout}
                else:
                    return {'success': False, 'error': result.stderr}
            
            return {'success': False, 'error': 'Benchmark script not found'}
            
        except subprocess.TimeoutExpired:
            return {'success': False, 'error': 'Benchmark timed out'}
        except Exception as e:
            return {'success': False, 'error': str(e)}
    
    def create_agent_from_template(
        self,
        template_name: str,
        parameters: Dict[str, Any]
    ) -> pynlm.AgentBrain:
        """Create agent from predefined template.
        
        Args:
            template_name: Name of template (actor_critic, ddpg, a2c, etc.)
            parameters: Template parameters
            
        Returns:
            Configured AgentBrain instance
        """
        templates = {
            'actor_critic': {
                'neuron_count': parameters.get('neurons', 1000),
                'learning_rate': parameters.get('learning_rate', 0.001),
                'gamma': parameters.get('gamma', 0.99),
                'epsilon': parameters.get('epsilon', 0.9)
            },
            'ddpg': {
                'neuron_count': parameters.get('neurons', 2000),
                'learning_rate': parameters.get('learning_rate', 0.0001),
                'target_update': parameters.get('target_update', 0.001),
                'buffer_size': parameters.get('buffer_size', 10000)
            },
            'a2c': {
                'neuron_count': parameters.get('neurons', 1500),
                'learning_rate': parameters.get('learning_rate', 0.0005),
                'gae_lambda': parameters.get('gae_lambda', 0.95),
                'entropy_coef': parameters.get('entropy_coef', 0.01)
            }
        }
        
        if template_name not in templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        template_params = templates[template_name]
        template_params.update(parameters)
        
        # Create brain with template configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", template_params['neuron_count'])
        config.set("brain.region_count", template_params.get('region_count', 4))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Apply template-specific settings
        if template_name == 'actor_critic':
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        elif template_name == 'ddpg':
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
        elif template_name == 'a2c':
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
        
        return agent
    
    def run_complete_workflow(self, workflow_config: Dict[str, Any]) -> Dict[str, Any]:
        """Execute complete hybrid workflow.
        
        Args:
            workflow_config: Complete workflow configuration
            
        Returns:
            Workflow execution results
        """
        workflow_id = f"workflow_{len(self.workflow_history)}"
        start_time = time.time()
        
        try:
            # Record workflow start
            workflow_step = {
                'id': workflow_id,
                'name': workflow_config.get('name', 'unnamed_workflow'),
                'start_time': start_time,
                'status': 'running',
                'steps': []
            }
            
            # Step 1: Setup
            setup_step = {
                'name': 'setup',
                'status': 'completed',
                'details': f"Initialized {workflow_config.get('agent_type', 'agent')} agent"
            }
            workflow_step['steps'].append(setup_step)
            
            # Step 2: Create agent
            agent = self.create_agent_from_template(
                workflow_config['agent_type'],
                workflow_config.get('parameters', {})
            )
            
            # Step 3: Create environment
            if workflow_config.get('use_world', True):
                world = pynlm.createSimulationWorld(
                    workflow_config.get('world_width', 100),
                    workflow_config.get('world_height', 100)
                )
                agent.initialize(world)
            
            setup_step['details'] = f"Created {workflow_config['agent_type']} agent with {agent.getBrain().getTotalNeuronCount()} neurons"
            
            # Step 4: Training
            training_step = {
                'name': 'training',
                'status': 'running'
            }
            workflow_step['steps'].append(training_step)
            
            episodes = workflow_config.get('episodes', 100)
            max_steps = workflow_config.get('max_steps', 1000)
            
            training_metrics = []
            
            for episode in range(episodes):
                episode_metrics = {
                    'episode': episode,
                    'reward': 0.0,
                    'curiosity': 0.0,
                    'novelty': 0.0
                }
                
                # Reset for new episode
                if workflow_config.get('use_world', True):
                    agent.reset()
                
                for step in range(max_steps):
                    if workflow_config.get('use_world', True):
                        # Get sensory input
                        percept = world.getSensoryPercept()
                        
                        # Process sensory input
                        agent.processSensoryInput(percept)
                        
                        # Run brain step
                        brain = agent.getBrain()
                        brain.step(step)
                        
                        # Decode motor command
                        action = agent.decodeMotorCommand()
                        
                        # Apply action
                        result = world.applyMotorCommand(action, world.getSimulationTime())
                        reward = result.reward if result else 0.0
                        
                        # Apply reward modulation
                        agent.applyRewardModulation(reward, 0.0)
                        
                        # Update world and development
                        world.update(0.1)
                        agent.updateDevelopment(0.1)
                        
                        # Record metrics
                        episode_metrics['reward'] += reward
                        episode_metrics['curiosity'] = agent.getCuriosityLevel()
                        episode_metrics['novelty'] = agent.getNoveltyLevel()
                    
                    training_metrics.append(episode_metrics.copy())
            
            training_step.update({
                'status': 'completed',
                'episodes_completed': episodes,
                'total_steps': episodes * max_steps,
                'avg_reward': np.mean([m['reward'] for m in training_metrics]),
                'max_reward': max([m['reward'] for m in training_metrics])
            })
            
            # Step 5: Evaluation
            evaluation_step = {
                'name': 'evaluation',
                'status': 'running'
            }
            workflow_step['steps'].append(evaluation_step)
            
            # Get final metrics
            final_metrics = {
                'brain_stats': pynlm.getBrainStats(agent.getBrain()),
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'final_prediction_error': agent.getPredictionError(),
                'training_summary': training_step
            }
            
            evaluation_step.update({
                'status': 'completed',
                'metrics': final_metrics
            })
            
            # Step 6: Save results
            save_step = {
                'name': 'save_results',
                'status': 'running'
            }
            workflow_step['steps'].append(save_step)
            
            # Save model if requested
            if workflow_config.get('save_model', False):
                output_dir = workflow_config.get('output_dir', './workflows')
                Path(output_dir).mkdir(parents=True, exist_ok=True)
                
                # Save brain state
                brain_path = f"{output_dir}/{workflow_id}_brain.bin"
                agent.getBrain().save(brain_path)
                
                # Save metrics
                metrics_path = f"{output_dir}/{workflow_id}_metrics.json"
                with open(metrics_path, 'w') as f:
                    json.dump(final_metrics, f, indent=2)
                
                save_step['details'] = f"Saved model to {brain_path}"
            
            save_step['status'] = 'completed'
            
            # Complete workflow record
            end_time = time.time()
            workflow_step['end_time'] = end_time
            workflow_step['duration'] = end_time - start_time
            workflow_step['status'] = 'completed'
            
            self.workflow_history.append(workflow_step)
            
            return {
                'success': True,
                'workflow_id': workflow_id,
                'results': final_metrics,
                'workflow': workflow_step
            }
            
        except Exception as e:
            # Record workflow failure
            workflow_step.update({
                'end_time': time.time(),
                'status': 'failed',
                'error': str(e)
            })
            
            self.workflow_history.append(workflow_step)
            
            return {
                'success': False,
                'workflow_id': workflow_id,
                'error': str(e)
            }

# Advanced hybrid workflow example
if __name__ == "__main__":
    # Initialize hybrid workflow
    workflow = NLPHybridWorkflow()
    
    # Configure complete workflow
    workflow_config = {
        'name': 'Advanced Actor-Critic Training',
        'agent_type': 'actor_critic',
        'parameters': {
            'neurons': 2000,
            'learning_rate': 0.001,
            'gamma': 0.99,
            'epsilon': 0.9
        },
        'episodes': 50,
        'max_steps': 1000,
        'world_width': 100,
        'world_height': 100,
        'use_world': True,
        'save_model': True,
        'output_dir': './advanced_workflows'
    }
    
    # Run complete workflow
    results = workflow.run_complete_workflow(workflow_config)
    
    if results['success']:
        print(f"Workflow completed successfully!")
        print(f"Workflow ID: {results['workflow_id']}")
        print(f"Duration: {results['results']['training_summary']['duration']:.2f}s")
        print(f"Average reward: {results['results']['training_summary']['avg_reward']:.2f}")
        
        # Print brain statistics
        brain_stats = results['results']['brain_stats']
        print(f"\nBrain Statistics:")
        print(f"  Total neurons: {brain_stats.get('total_neurons', 0)}")
        print(f"  Total synapses: {brain_stats.get('total_synapses', 0)}")
        print(f"  Active neurons: {brain_stats.get('active_neurons', 0)}")
        print(f"  Firing rate: {brain_stats.get('average_firing_rate', 0):.3f}")
        print(f"  Total spikes: {brain_stats.get('total_spikes', 0)}")
    else:
        print(f"Workflow failed: {results['error']}")
```