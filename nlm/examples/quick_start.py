"""
Advanced Quick Start Example for NLM

This example demonstrates comprehensive usage of NLM with advanced features including:
- Multi-agent coordination and competition
- Neuromodulation and development
- Structural plasticity
- Real-time monitoring and analysis
- Integration with different brain regions
- Performance optimization techniques

This example builds upon the basic quick start by demonstrating production-ready code
with proper error handling, logging, and best practices.
"""

import time
import logging
from typing import Dict, List, Optional, Any
import numpy as np

from nlm import create_brain, run_simulation, create_agent, World, create_region

# Configure logging for production use
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_advanced_example.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

def setup_advanced_brain() -> Any:
    """Create and configure an advanced brain with multiple features.
    
    Returns:
        Configured brain instance with advanced features enabled.
    """
    logger.info("Setting up advanced brain configuration...")
    
    brain_config = {
        'learning_rate': 0.001,
        'hidden_size': 512,
        'num_layers': 4,
        'dropout_rate': 0.2,
        'batch_size': 64,
        'optimizer': 'adamw',
        'scheduler': 'cosine',
        'weight_decay': 0.0001,
        'enable_neuromodulation': True,
        'enable_structural_plasticity': True,
        'memory_capacity': 10000,
        'replay_frequency': 0.1,
        'curiosity_lambda': 0.5,
        'prediction_error_threshold': 0.1,
        'development_rate': 0.01,
        'adaptation_window': 1000,
    }
    
    brain = create_brain(**brain_config)
    logger.info(f"Advanced brain created with config: {brain_config}")
    return brain

def create_competitive_agents(brain: Any, num_agents: int = 3) -> List[Any]:
    """Create multiple agents that compete and cooperate.
    
    Args:
        brain: Shared brain instance for agents.
        num_agents: Number of agents to create.
        
    Returns:
        List of configured agent instances.
    """
    logger.info(f"Creating {num_agents} competitive agents...")
    agents = []
    
    agent_strategies = ['exploration', 'exploitation', 'balanced']
    
    for i in range(num_agents):
        agent_config = {
            'name': f'agent_{i+1}',
            'strategy': agent_strategies[i % len(agent_strategies)],
            'epsilon': 0.1 + (i * 0.05),
            'temperature': 1.0 / (i + 1),
            'reward_scale': 1.0 + (i * 0.2),
            'exploration_bonus': 0.1 * (i + 1),
        }
        
        agent = create_agent(**agent_config, brain=brain)
        agents.append(agent)
        logger.info(f"Created agent {agent.name} with strategy: {agent_config['strategy']}")
    
    return agents

def setup_multiregional_brain() -> Any:
    """Create a brain with multiple regions and populations.
    
    Returns:
        Multi-regional brain instance.
    """
    logger.info("Setting up multi-regional brain...")
    
    # Create main brain
    brain = create_brain(
        learning_rate=0.001,
        hidden_size=256,
        enable_structural_plasticity=True
    )
    
    # Create different brain regions
    regions = {
        'visual_cortex': {'size': 128, 'plasticity': 0.8, 'receptive_field': 'local'},
        'hippocampus': {'size': 64, 'plasticity': 0.9, 'replay_frequency': 0.05},
        'prefrontal_cortex': {'size': 96, 'plasticity': 0.7, 'working_memory': True},
        'amygdala': {'size': 48, 'plasticity': 0.6, 'emotional_modulation': True},
    }
    
    # Create regions in the brain
    for region_name, region_config in regions.items():
        region = create_region(
            name=region_name,
            size=region_config['size'],
            plasticity=region_config['plasticity']
        )
        brain.add_region(region)
        logger.info(f"Added region {region_name} with config {region_config}")
    
    return brain

def monitor_performance(metrics: Dict[str, float]) -> Dict[str, float]:
    """Monitor and analyze performance metrics in real-time.
    
    Args:
        metrics: Dictionary of performance metrics.
        
    Returns:
        Processed metrics with additional analysis.
    """
    processed_metrics = metrics.copy()
    
    # Calculate derived metrics
    if 'accuracy' in metrics and 'loss' in metrics:
        processed_metrics['performance_score'] = max(0, metrics['accuracy'] - metrics['loss'])
    
    if 'training_steps' in metrics and 'time_elapsed' in metrics:
        processed_metrics['steps_per_second'] = (
            metrics['training_steps'] / metrics['time_elapsed']
            if metrics['time_elapsed'] > 0 else 0
        )
    
    # Log performance alerts
    if processed_metrics.get('performance_score', 0) < 0.5:
        logger.warning("Low performance score detected!")
    
    if metrics.get('loss', 1.0) > 1.0:
        logger.warning("High loss detected - consider adjusting learning rate")
    
    logger.info(f"Performance metrics: {processed_metrics}")
    return processed_metrics

def run_learning_with_feedback(brain: Any, agents: List[Any], steps: int = 1000) -> Dict[str, Any]:
    """Run learning simulation with comprehensive feedback and analysis.
    
    Args:
        brain: Brain instance for learning.
        agents: List of agents participating in learning.
        steps: Number of learning steps to run.
        
    Returns:
        Dictionary containing learning results and analysis.
    """
    logger.info(f"Starting advanced learning with {len(agents)} agents for {steps} steps...")
    
    results = {
        'total_steps': steps,
        'agent_results': {},
        'world_metrics': {},
        'learning_curve': [],
        'final_parameters': None,
        'convergence_step': None,
        'performance_analysis': {}
    }
    
    # Initialize tracking variables
    start_time = time.time()
    best_accuracy = 0.0
    convergence_threshold = 0.95
    learning_rate_history = []
    
    # Create world for learning
    world = World("advanced_learning_world")
    world.add_brain(brain)
    
    # Add agents to world
    for agent in agents:
        world.add_agent(agent)
    
    # Training loop with feedback
    for step in range(steps):
        try:
            # Record current parameters before update
            current_params = brain.get_parameters()
            learning_rate_history.append(current_params.get('learning_rate', 0.001))
            
            # Each agent performs thinking and action
            step_results = {}
            for agent in agents:
                # Create complex observation with multiple features
                observation = {
                    'step': step,
                    'timestamp': time.time(),
                    'agent': agent.name,
                    'world_state': {
                        'brains': len(world.get_brains()),
                        'agents': len(world.get_agents()),
                        'step_completed': step
                    }
                }
                
                # Agent thinks (decision making)
                decision = agent.think(observation)
                
                # Agent acts with complex action
                action_result = agent.act('learn', {
                    'observation': observation,
                    'decision': decision,
                    'reward': decision.get('reward', 0.0),
                    'prediction_error': decision.get('prediction_error', 0.0),
                    'curiosity_bonus': decision.get('curiosity_bonus', 0.0)
                })
                
                step_results[agent.name] = {
                    'decision': decision,
                    'action_result': action_result,
                    'reward': action_result.get('reward', 0.0)
                }
            
            # Simulate world update
            if step % 100 == 0:
                world_result = world.run_simulation(100)
                results['world_metrics'][step] = world_result
            
            # Record learning metrics
            learning_metrics = {
                'step': step + 1,
                'learning_rate': learning_rate_history[-1],
                'total_reward': sum(r['reward'] for r in step_results.values()),
                'agents_active': len([r for r in step_results.values() if r['reward'] > 0]),
                'avg_reward': np.mean([r['reward'] for r in step_results.values()]),
            }
            
            results['learning_curve'].append(learning_metrics)
            
            # Check for convergence
            current_accuracy = np.mean([r['reward'] for r in step_results.values()]) / 100
            if current_accuracy > best_accuracy:
                best_accuracy = current_accuracy
                results['convergence_step'] = step + 1
            
            # Update brain parameters with simulated learning
            if step % 50 == 0:
                brain.update_parameters(learning_rate=learning_rate_history[-1] * 0.9)
            
            # Log progress periodically
            if (step + 1) % 200 == 0:
                logger.info(f"Step {step + 1}/{steps} completed")
                monitor_performance({
                    'accuracy': best_accuracy,
                    'loss': 1.0 - best_accuracy,
                    'training_steps': step + 1,
                    'time_elapsed': time.time() - start_time,
                })
        
        except Exception as e:
            logger.error(f"Error at step {step}: {str(e)}")
            results['error'] = str(e)
            break
    
    # Finalize results
    end_time = time.time()
    results['final_parameters'] = brain.get_parameters()
    results['total_time'] = end_time - start_time
    results['best_accuracy'] = best_accuracy
    results['convergence_threshold'] = convergence_threshold
    
    # Performance analysis
    results['performance_analysis'] = monitor_performance({
        'accuracy': best_accuracy,
        'loss': 1.0 - best_accuracy,
        'training_steps': steps,
        'time_elapsed': results['total_time'],
        'steps_per_second': steps / results['total_time'] if results['total_time'] > 0 else 0,
        'learning_rate_stability': np.std(learning_rate_history) if learning_rate_history else 0,
    })
    
    logger.info(f"Learning completed in {results['total_time']:.2f} seconds")
    logger.info(f"Best accuracy: {best_accuracy:.4f}")
    
    return results

def run_advanced_simulation() -> Dict[str, Any]:
    """Run comprehensive advanced simulation with all features enabled.
    
    Returns:
        Dictionary containing all simulation results.
    """
    logger.info("Starting advanced NLM simulation...")
    
    simulation_results = {
        'timestamp': time.time(),
        'simulation_config': {},
        'stage_results': {},
        'performance_metrics': {},
        'errors': [],
    }
    
    try:
        # Stage 1: Setup advanced brain
        logger.info("Stage 1: Setting up advanced brain...")
        advanced_brain = setup_advanced_brain()
        simulation_results['stage_results']['brain_setup'] = {
            'status': 'completed',
            'brain_parameters': advanced_brain.get_parameters()
        }
        
        # Stage 2: Create competitive agents
        logger.info("Stage 2: Creating competitive agents...")
        agents = create_competitive_agents(advanced_brain, num_agents=3)
        simulation_results['stage_results']['agent_creation'] = {
            'status': 'completed',
            'agent_count': len(agents),
            'agents': [a.name for a in agents]
        }
        
        # Stage 3: Multi-regional setup
        logger.info("Stage 3: Setting up multi-regional brain...")
        regional_brain = setup_multiregional_brain()
        simulation_results['stage_results']['regional_setup'] = {
            'status': 'completed',
            'regions': regional_brain.get_regions()
        }
        
        # Stage 4: Learning with feedback
        logger.info("Stage 4: Running learning simulation...")
        learning_results = run_learning_with_feedback(advanced_brain, agents, steps=500)
        simulation_results['stage_results']['learning'] = learning_results
        
        # Stage 5: Performance analysis
        logger.info("Stage 5: Performing performance analysis...")
        simulation_results['performance_metrics'] = monitor_performance({
            'final_accuracy': learning_results.get('best_accuracy', 0.0),
            'total_time': learning_results.get('total_time', 0.0),
            'agents_participated': len(agents),
            'regions_configured': len(regional_brain.get_regions()) if regional_brain else 0,
        })
        
        simulation_results['simulation_config'] = {
            'brain_config': {
                'learning_rate': 0.001,
                'hidden_size': 512,
                'num_layers': 4,
            },
            'agents_configured': len(agents),
            'regions_configured': len(regional_brain.get_regions()) if regional_brain else 0,
            'total_steps': 500,
        }
        
        logger.info("Advanced simulation completed successfully!")
        
    except Exception as e:
        logger.error(f"Simulation failed: {str(e)}")
        simulation_results['errors'].append({
            'stage': 'unknown',
            'error': str(e),
            'timestamp': time.time()
        })
    
    return simulation_results

def main() -> None:
    """Main function to run the advanced example."""
    logger.info("=" * 60)
    logger.info("Starting Advanced NLM Example - Quick Start Enhanced")
    logger.info("=" * 60)
    
    try:
        # Run the advanced simulation
        results = run_advanced_simulation()
        
        # Log final summary
        logger.info("\n" + "=" * 60)
        logger.info("ADVANCED SIMULATION SUMMARY")
        logger.info("=" * 60)
        
        if 'stage_results' in results:
            logger.info(f"Brain Setup: {'✓' if results['stage_results']['brain_setup']['status'] == 'completed' else '✗'}")
            logger.info(f"Agent Creation: {'✓' if results['stage_results']['agent_creation']['status'] == 'completed' else '✗'}")
            logger.info(f"Regional Setup: {'✓' if results['stage_results']['regional_setup']['status'] == 'completed' else '✗'}")
            logger.info(f"Learning: {'✓' if 'best_accuracy' in results['stage_results']['learning'] else '✗'}")
        
        if 'performance_metrics' in results:
            perf = results['performance_metrics']
            logger.info(f"Final Performance Score: {perf.get('performance_score', 'N/A'):.4f}")
            logger.info(f"Steps per Second: {perf.get('steps_per_second', 'N/A'):.2f}")
            logger.info(f"Learning Rate Stability: {perf.get('learning_rate_stability', 'N/A'):.6f}")
        
        if results.get('errors'):
            logger.error(f"Errors encountered: {len(results['errors'])}")
        
        logger.info("Advanced example completed successfully!")
        
    except KeyboardInterrupt:
        logger.info("Simulation interrupted by user")
    except Exception as e:
        logger.error(f"Advanced example failed: {str(e)}")
    
    logger.info("=" * 60)

if __name__ == "__main__":
    main()
