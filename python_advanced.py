"""
NLM Advanced Python Usage Guide
===============================

This guide covers advanced features and patterns for experienced NLM users.
"""

import pynlm
import json
import time
from typing import List, Tuple, Optional

def advanced_brain_construction():
    """Advanced brain construction techniques"""
    print("=== Advanced Brain Construction ===")
    
    # 1. Custom configuration with validation
    def create_brain_with_validation(neuron_count: int, min_neurons: int = 100) -> pynlm.Brain:
        if neuron_count < min_neurons:
            raise ValueError(f"Neuron count must be at least {min_neurons}")
        
        config = pynlm.createConfigWithSettings(
            neuron_count=neuron_count,
            region_count=2,
            synapse_density=0.05 + (neuron_count / 10000.0) * 0.2  # Dynamic density
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    # 2. Multi-brain synchronization
    brains = [create_brain_with_validation(500 + i * 100) for i in range(3)]
    
    print(f"Created {len(brains)} synchronized brains")
    print(f"Average neuron count: {sum(b.getTotalNeuronCount() for b in brains) / len(brains):.0f}")
    print()

def advanced_simulation_control():
    """Advanced simulation control and monitoring"""
    print("=== Advanced Simulation Control ===")
    
    # Create a brain with monitoring
    config = pynlm.createConfigWithSettings(
        neuron_count=2000,
        region_count=3,
        synapse_density=0.08
    )
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Advanced simulation loop with detailed monitoring
    simulation_stats = {
        'steps': 0,
        'total_spikes': 0,
        'max_firing_rate': 0.0,
        'avg_curiosity': 0.0,
        'novelty_events': 0,
        'planner_agreements': 0,
        'concept_discoveries': 0
    }
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action and execution
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        simulation_stats['steps'] += 1
        simulation_stats['total_spikes'] += brain.getFiringNeuronCount()
        simulation_stats['max_firing_rate'] = max(
            simulation_stats['max_firing_rate'],
            brain.getAverageFiringRate()
        )
        simulation_stats['avg_curiosity'] += agent.getCuriosityLevel()
        
        if agent.getNoveltyLevel() > 0.5:
            simulation_stats['novelty_events'] += 1
        
        simulation_stats['planner_agreements'] += agent.getPlannerAgreementCount()
        simulation_stats['concept_discoveries'] += agent.getConceptCount()
        
        # Progress output
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing={brain.getFiringNeuronCount():.0f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Planner Agreement={agent.getPlannerAgreementCount()}")
    
    # Calculate final statistics
    simulation_stats['avg_curiosity'] /= simulation_stats['steps']
    
    print("\nSimulation Statistics:")
    for key, value in simulation_stats.items():
        print(f"  {key}: {value}")
    print()

def advanced_data_analysis():
    """Advanced data analysis and visualization preparation"""
    print("=== Advanced Data Analysis ===")
    
    # Create a brain and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect data over time
    time_series_data = {
        'steps': [],
        'firing_rates': [],
        'spike_counts': [],
        'development_stages': []
    }
    
    for step in range(200):
        brain.step(step)
        
        time_series_data['steps'].append(step)
        time_series_data['firing_rates'].append(brain.getAverageFiringRate())
        time_series_data['spike_counts'].append(brain.getTotalSpikeCount())
        time_series_data['development_stages'].append(brain.getDevelopmentalStage())
    
    # Analyze the data
    avg_firing_rate = sum(time_series_data['firing_rates']) / len(time_series_data['firing_rates'])
    final_development_stage = time_series_data['development_stages'][-1]
    
    print(f"Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"Final development stage: {final_development_stage}")
    print(f"Spike growth: {time_series_data['spike_counts'][-1] - time_series_data['spike_counts'][0]} spikes")
    
    # Convert to JSON for analysis (if needed)
    json_data = {
        'summary': {
            'avg_firing_rate': avg_firing_rate,
            'final_development_stage': final_development_stage,
            'total_spikes': time_series_data['spike_counts'][-1]
        },
        'time_series': time_series_data
    }
    
    print(f"Data collected for {len(time_series_data['steps'])} time steps")
    print()

def advanced_agent_features():
    """Advanced agent brain features and debugging"""
    print("=== Advanced Agent Features ===")
    
    # Create a complete agent setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run a simulation and debug
    debug_info = {
        'steps': [],
        'brain_stats': [],
        'agent_stats': [],
        'thought_traces': []
    }
    
    for step in range(50):
        world.update(0.1)
        
        # Get detailed percept information
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Record detailed brain state
        brain_state = {
            'step': step,
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'average_firing_rate': brain.getAverageFiringRate()
        }
        
        # Record agent state
        agent_state = {
            'step': step,
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'planner_agreement_count': agent.getPlannerAgreementCount(),
            'concept_count': agent.getConceptCount()
        }
        
        # Get thought trace
        thought_trace = agent.getThoughtTrace()
        
        debug_info['steps'].append(step)
        debug_info['brain_stats'].append(brain_state)
        debug_info['agent_stats'].append(agent_state)
        debug_info['thought_traces'].append(thought_trace)
        
        # Execute
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    # Print debug summary
    print("Debug Summary:")
    print(f"Total steps: {len(debug_info['steps'])}")
    print(f"Average curiosity: {sum(d['curiosity_level'] for d in debug_info['agent_stats']) / len(debug_info['agent_stats']):.3f}")
    print(f"Planner agreements: {debug_info['agent_stats'][-1]['planner_agreement_count']}")
    print(f"Concepts discovered: {debug_info['agent_stats'][-1]['concept_count']}")
    
    # Print last thought trace
    print(f"\nLast thought trace:\n{debug_info['thought_traces'][-1]}")
    print()

def advanced_configuration_patterns():
    """Advanced configuration patterns"""
    print("=== Advanced Configuration Patterns ===")
    
    # Pattern 1: Configuration validation and error handling
    def validate_and_create_config(config_dict: dict) -> pynlm.Config:
        # Validate config
        required_keys = ['neuron_count', 'region_count']
        for key in required_keys:
            if key not in config_dict:
                raise ValueError(f"Missing required config key: {key}")
        
        if config_dict['neuron_count'] < 100:
            raise ValueError("Neuron count must be at least 100")
        
        # Create config
        config = pynlm.createConfigWithSettings(
            neuron_count=config_dict['neuron_count'],
            region_count=config_dict['region_count'],
            synapse_density=config_dict.get('synapse_density', 0.1),
            random_seed=config_dict.get('random_seed', 42.0)
        )
        
        # Apply additional settings
        if 'connection_probability' in config_dict:
            # This would normally be set in config, but Config doesn't have direct setter
            # This is just an example pattern
            pass
        
        return config
    
    # Pattern 2: Dynamic configuration based on performance
    def performance_adaptive_config(base_neuron_count: int, performance_factor: float) -> pynlm.Config:
        # Adjust configuration based on target performance
        neuron_count = int(base_neuron_count * performance_factor)
        synapse_density = 0.05 + (performance_factor - 1.0) * 0.1  # More neurons, denser connectivity
        
        return pynlm.createConfigWithSettings(
            neuron_count=neuron_count,
            region_count=max(2, int(performance_factor * 2)),
            synapse_density=synapse_density
        )
    
    # Example usage
    custom_config = validate_and_create_config({
        'neuron_count': 1000,
        'region_count': 3,
        'synapse_density': 0.12,
        'random_seed': 123
    })
    
    brain = pynlm.createBrain(custom_config)
    brain.initialize()
    
    print(f"Created brain with custom configuration: {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print()
    
    # Performance adaptive example
    adaptive_config = performance_adaptive_config(500, 1.5)
    brain2 = pynlm.createBrain(adaptive_config)
    brain2.initialize()
    
    print(f"Performance-adaptive brain: {brain2.getTotalNeuronCount()} neurons")
    print()

def advanced_simulation_orchestration():
    """Advanced simulation orchestration and management"""
    print("=== Advanced Simulation Orchestration ===")
    
    # Create simulation manager class pattern
    class SimulationManager:
        def __init__(self, config_template: Optional[pynlm.Config] = None):
            self.config_template = config_template or pynlm.createDefaultConfig()
            self.simulation_history = []
        
        def run_episode(self, world_config: dict, learning_enabled: bool = False, 
                       steps: int = 100) -> dict:
            """Run a complete simulation episode"""
            # Create brain
            config = self.config_template
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(**world_config)
            world.reset()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            if learning_enabled:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            
            # Run simulation
            episode_data = {
                'brain_stats': [],
                'agent_stats': [],
                'actions': []
            }
            
            for step in range(steps):
                world.update(0.1)
                
                # Process
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Neuromodulation
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.1)
                
                # Collect data
                episode_data['brain_stats'].append({
                    'step': step,
                    'neurons_firing': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate()
                })
                
                episode_data['agent_stats'].append({
                    'step': step,
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'prediction_error': agent.getPredictionError()
                })
                
                episode_data['actions'].append(action)
            
            # Save history
            self.simulation_history.append(episode_data)
            
            # Calculate summary
            final_stats = {
                'total_spikes': brain.getTotalSpikeCount(),
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'action_distribution': {}
            }
            
            for action in episode_data['actions']:
                action_name = pynlm.MotorCommand(action).name if hasattr(pynlm.MotorCommand, 'name') else str(action)
                final_stats['action_distribution'][action_name] = final_stats['action_distribution'].get(action_name, 0) + 1
            
            return final_stats
        
        def get_summary_stats(self) -> dict:
            """Get summary statistics of all simulations"""
            if not self.simulation_history:
                return {'episodes': 0}
            
            total_spikes = sum(
                ep_data['brain_stats'][-1]['total_spikes'] 
                for ep_data in self.simulation_history
            )
            
            return {
                'episodes': len(self.simulation_history),
                'total_spikes': total_spikes,
                'avg_spikes_per_episode': total_spikes / len(self.simulation_history)
            }
    
    # Use the simulation manager
    manager = SimulationManager()
    
    # Run multiple episodes with different configurations
    for i in range(3):
        print(f"Running episode {i + 1}...")
        stats = manager.run_episode(
            world_config={
                'width': 20 + i * 5,
                'height': 20 + i * 5,
                'visionWidth': 8,
                'visionHeight': 8
            },
            learning_enabled=(i % 2 == 0),
            steps=50
        )
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Final curiosity: {stats['final_curiosity']:.3f}")
        print()
    
    # Get overall summary
    summary = manager.get_summary_stats()
    print("Simulation Manager Summary:")
    for key, value in summary.items():
        print(f"  {key}: {value}")
    print()

def main():
    """Run all advanced examples"""
    print("NLM Advanced Python Usage Guide")
    print("=" * 50)
    print()
    
    examples = [
        advanced_brain_construction,
        advanced_simulation_control,
        advanced_data_analysis,
        advanced_agent_features,
        advanced_configuration_patterns,
        advanced_simulation_orchestration
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            print(f"--- Example {i} ---")
            example()
        except Exception as e:
            print(f"Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
        print()
    
    print("Advanced examples completed!")
    print()
    print("Key advanced concepts learned:")
    print("  - Configuration validation and error handling")
    print("  - Performance monitoring and statistics collection")
    print("  - Advanced agent brain features and debugging")
    print("  - Simulation orchestration and management")
    print("  - Data analysis and processing patterns")
    print()
    print("For more advanced features, explore the C++ source code and")
    print("customize the bindings for your specific use case.")

if __name__ == "__main__":
    main()