#!/usr/bin/env python3
"""
NLM Python Bindings - Enhanced API

This script provides comprehensive Python bindings for the NLM (Neural Learning Machine) framework,
enhancing the basic API with advanced functionality for research and development.

Key Features:
- Full access to all NLM brain systems and components
- Advanced memory management and cognitive modeling capabilities
- Powerful exploration and experimentation tools
- Comprehensive documentation and examples

Usage Examples:
    import pynlm

    # Create and initialize a brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Create agent with all subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)

    # Run simulation
    for step in range(1000):
        brain.step(step)
        action = agent.decodeMotorCommand()

    # Access brain statistics and metrics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    print(f"Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")

Advanced Usage:

1. Multi-system Experimentation
    def run_advanced_experiment(neuron_count=500, enable_cognition=True):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllSystems()  # Enable all cognitive and memory systems
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        for step in range(2000):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            if step % 200 == 0:
                print(f"Step {step}: Firing rate={brain.getAverageFiringRate():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        return agent.getPerformanceMetrics()

2. Memory System Exploration
    def analyze_memory_retention(agent, num_steps=1000):
        initial_memories = {
            'working_memory': agent.getBrain().getWorkingMemory().getActiveTraces(),
            'episodic_episodes': agent.getBrain().getEpisodicMemory().getEpisodeCount(),
            'concept_count': agent.getBrain().getConceptFormation().getConceptCount() if agent.getBrain().getConceptFormation() else 0
        }
        
        for step in range(num_steps):
            # Simulate agent-environment interaction
            pass
            
        final_memories = {
            'working_memory': agent.getBrain().getWorkingMemory().getActiveTraces(),
            'episodic_episodes': agent.getBrain().getEpisodicMemory().getEpisodeCount(),
            'concept_count': agent.getBrain().getConceptFormation().getConceptCount() if agent.getBrain().getConceptFormation() else 0
        }
        
        retention_rate = {}
        for key in initial_memories:
            if initial_memories[key] > 0:
                retention_rate[key] = (final_memories[key] - initial_memories[key]) / initial_memories[key]
            else:
                retention_rate[key] = 0.0
                
        return retention_rate

3. Development and Learning Analysis
    def study_development(agent, time_span=300.0):
        developmental_stages = []
        metrics_over_time = {
            'plasticity': [],
            'curiosity': [],
            'novelty': [],
            'firing_rate': [],
            'memory_capacity': []
        }
        
        timestep = time_span / 1000
        for step in range(1000):
            # Simulate development over time
            developmental_stages.append(agent.getDevelopmentalStage())
            metrics_over_time['plasticity'].append(agent.getNeuromodulationLevel())
            metrics_over_time['curiosity'].append(agent.getCuriosityLevel())
            metrics_over_time['novelty'].append(agent.getNoveltyLevel())
            metrics_over_time['firing_rate'].append(agent.getBrain().getAverageFiringRate())
            metrics_over_time['memory_capacity'].append(agent.getBrain().getWorkingMemory().getCapacity())
            
            if step % 100 == 0:
                agent.updateDevelopment(timestep * 100)
                
        return developmental_stages, metrics_over_time

4. Checkpoint and State Management
    def save_brain_state(brain, filename, metadata=None):
        """Save brain state with optional metadata"""
        timestamp = datetime.now().isoformat()
        
        if not brain.save(filename):
            raise RuntimeError(f"Failed to save brain state to {filename}")
            
        if metadata:
            with open(filename + ".meta", "w") as f:
                json.dump({
                    'timestamp': timestamp,
                    'metadata': metadata,
                    'stats': {
                        'neuron_count': brain.getTotalNeuronCount(),
                        'synapse_count': brain.getTotalSynapseCount(),
                        'total_spikes': brain.getTotalSpikeCount(),
                        'firing_rate': brain.getAverageFiringRate()
                    }
                }, f, indent=2)
        
        return filename

5. Configuration and Experiment Management
    def create_experimental_config(base_config, modifications):
        """Create and modify experimental configurations"""
        config = pynlm.createDefaultConfig()
        
        # Apply base configuration
        for key, value in base_config.items():
            if isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
            elif isinstance(value, bool):
                config.set(key, value)
        
        # Apply experimental modifications
        for key, value in modifications.items():
            if isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
            elif isinstance(value, bool):
                config.set(key, value)
        
        return config

    # Example experimental configurations
    EXPLORATION_CONFIG = {
        'brain.neuron_count': 2000,
        'brain.synapse_density': 0.15,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': True,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    }
    
    STABILITY_CONFIG = {
        'brain.neuron_count': 500,
        'brain.synapse_density': 0.05,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': False,
        'neuromod.curiosity.enable': False,
        'neuromod.novelty.enable': False
    }
    
    CREATIVITY_CONFIG = {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.12,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': True,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'neuromod.dopamine.scale': 1.5
    }

# Advanced Command Line Interface Integration
if __name__ == "__main__":
    import argparse
    import json
    import time
    from datetime import datetime
    
    def main():
        parser = argparse.ArgumentParser(
            description="NLM Advanced Python Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Advanced usage examples:
  %(prog)s experiment --config creative --steps 5000
  %(prog)s explore --neurons 1000 --plasticity full
  %(prog)s analyze --load brain_state.bin --output metrics.json
            """
        )
        
        parser.add_argument('--config', choices=['exploration', 'stability', 'creativity'],
                          default='exploration', help='Experimental configuration')
        parser.add_argument('--steps', type=int, default=1000,
                          help='Number of simulation steps')
        parser.add_argument('--neurons', type=int, default=1000,
                          help='Number of neurons')
        parser.add_argument('--plasticity', choices=['full', 'stdp', 'hebbian', 'none'],
                          default='full', help='Plasticity mechanism')
        parser.add_argument('--world', type=int, nargs=2, metavar=('WIDTH', 'HEIGHT'),
                          default=[20, 20], help='World dimensions')
        parser.add_argument('--save', type=str, help='Save brain state to file')
        parser.add_argument('--load', type=str, help='Load brain state from file')
        parser.add_argument('--output', type=str, help='Output metrics to JSON file')
        
        args = parser.parse_args()
        
        print(f"NLM Advanced Interface - Configuration: {args.config}")
        print(f"Steps: {args.steps}, Neurons: {args.neurons}")
        print(f"World: {args.world[0]}x{args.world[1]}")
        print(f"Plasticity: {args.plasticity}")
        
        # Create configuration based on args
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neurons)
        config.set("brain.synapse_density", 0.1)
        
        # Apply plasticity settings
        if args.plasticity == 'none':
            config.set("plasticity.stdp.enable", False)
            config.set("plasticity.hebbian.enable", False)
        elif args.plasticity == 'stdp':
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", False)
        elif args.plasticity == 'hebbian':
            config.set("plasticity.stdp.enable", False)
            config.set("plasticity.hebbian.enable", True)
        else:  # full
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
        
        # Create and run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        if args.load:
            print(f"Loading brain state from {args.load}")
            brain.load(args.load)
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllSystems()
        
        world = pynlm.createSimpleWorld()
        world.configure(args.world[0], args.world[1], 8, 8)
        world.reset()
        agent.initialize(world)
        
        start_time = time.time()
        for step in range(args.steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            if step % 500 == 0:
                print(f"Step {step}/{args.steps} completed")
        
        elapsed = time.time() - start_time
        print(f"Simulation completed in {elapsed:.2f} seconds")
        
        # Save state if requested
        if args.save:
            brain.save(args.save)
            print(f"Brain state saved to {args.save}")
        
        # Output metrics if requested
        if args.output:
            metrics = {
                'config': args.config,
                'steps': args.steps,
                'neurons': args.neurons,
                'world_dimensions': args.world,
                'plasticity': args.plasticity,
                'total_spikes': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'memory_traces': brain.getWorkingMemory().getActiveTraces(),
                'episodic_episodes': brain.getEpisodicMemory().getEpisodeCount(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'developmental_stage': str(agent.getDevelopmentalStage()),
                'elapsed_time': elapsed
            }
            
            with open(args.output, 'w') as f:
                json.dump(metrics, f, indent=2)
            
            print(f"Metrics saved to {args.output}")
        
        # Print final status
        print("\n=== Final Status ===")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total synapses: {brain.getTotalSynapseCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Currently firing: {brain.getFiringNeuronCount()}")
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        print(f"Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {agent.getPredictionError():.3f}")
        print(f"Developmental stage: {agent.getDevelopmentalStage()}")
        print(f"Performance score: {(brain.getAverageFiringRate() + agent.getCuriosityLevel() + (1.0 - agent.getPredictionError())) / 3:.3f}")

    if __name__ == "__main__":
        main()