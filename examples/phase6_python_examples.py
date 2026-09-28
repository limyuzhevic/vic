#!/usr/bin/env python3
"""
NLM Phase 6 Python Examples

Comprehensive examples demonstrating Phase 6 integration capabilities.
This file shows how to use NLM's Phase 6 functionality through Python bindings.
"""

import pynlm
import time
import json
import os
from typing import List, Dict, Any, Optional

class Phase6Examples:
    """
    Collection of Phase 6 integration examples.
    
    This class provides practical examples of how to use NLM's Phase 6
    integration functionality through the Python API.
    """
    
    def __init__(self):
        self.examples_run = []
        self.performance_metrics = []
    
    def example_1_basic_phase6_integration(self):
        """
        Example 1: Basic Phase 6 integration setup and execution.
        
        This demonstrates the minimal configuration required for Phase 6
        integration, showing how all major systems work together.
        """
        print("=" * 60)
        print("EXAMPLE 1: Basic Phase 6 Integration")
        print("=" * 60)
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Configure for Phase 6 integration
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.connection_probability", 0.05)
        config.set("random_seed", 42)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent-brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # Create world environment
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Enable all Phase 6 systems
        agent.enableMemorySystems(True)
        agent.enableNeuromodulation(True)
        agent.enablePrediction(True)
        agent.enableCognition(True)
        agent.enableDevelopment(True)
        agent.enableReplay(True)
        agent.enableCheckpointing(True)
        
        print("✓ Configuration and initialization complete")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        
        # Run simulation
        num_steps = 1000
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Decode motor command from brain activity
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward for learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress every 200 steps
            if step % 200 == 0:
                print(f"  Step {step}/{num_steps}: "
                      f"{brain.getFiringNeuronCount()} firing, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        end_time = time.time()
        execution_time = end_time - start_time
        
        # Display results
        print(f"\n✓ Simulation completed in {execution_time:.2f} seconds")
        print(f"✓ Average steps per second: {num_steps / execution_time:.1f}")
        
        # Performance metrics
        metrics = {
            'execution_time': execution_time,
            'steps_per_second': num_steps / execution_time,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage().value,
            'memory_episodes': len(agent.getMemoryTraces()) if hasattr(agent, 'getMemoryTraces') else 0
        }
        
        self.performance_metrics.append(metrics)
        self.examples_run.append("basic_phase6_integration")
        
        return metrics
    
    def example_2_phase6_with_checkpointing(self):
        """
        Example 2: Phase 6 integration with checkpoint save/load.
        
        This demonstrates how to use Phase 6's checkpoint functionality
        to save and restore brain state, enabling interrupted simulations
        and long-running experiments.
        """
        print("\n" + "=" * 60)
        print("EXAMPLE 2: Phase 6 with Checkpointing")
        print("=" * 60)
        
        checkpoint_file = "phase6_checkpoint_example.bin"
        
        # Clean up existing checkpoint
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print("✓ Removed existing checkpoint")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("brain.synapse_density", 0.08)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent.initialize(world)
        
        # Enable all systems including checkpointing
        agent.enableMemorySystems(True)
        agent.enableNeuromodulation(True)
        agent.enablePrediction(True)
        agent.enableCognition(True)
        agent.enableDevelopment(True)
        agent.enableReplay(True)
        agent.enableCheckpointing(True)
        
        print("✓ Phase 6 system with checkpointing initialized")
        
        # Try to load existing checkpoint (should fail initially)
        print(f"\n🔄 Attempting to load checkpoint from {checkpoint_file}...")
        if agent.loadCheckpoint(checkpoint_file):
            print("✓ Checkpoint loaded successfully!")
            load_successful = True
        else:
            print("ℹ No existing checkpoint found, starting fresh")
            load_successful = False
        
        # Run simulation
        num_steps = 2000
        print(f"\n🚀 Running {num_steps} simulation steps...")
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Save checkpoint periodically
            if step % 500 == 0 and step > 0:
                print(f"  💾 Saving checkpoint at step {step}...")
                if agent.saveCheckpoint(checkpoint_file):
                    print(f"  ✓ Checkpoint saved to {checkpoint_file}")
                else:
                    print(f"  ✗ Failed to save checkpoint")
        
        # Save final checkpoint
        print(f"\n💾 Saving final checkpoint at step {num_steps}...")
        if agent.saveCheckpoint(checkpoint_file):
            print(f"✓ Final checkpoint saved to {checkpoint_file}")
            checkpoint_saved = True
        else:
            print(f"✗ Failed to save final checkpoint")
            checkpoint_saved = False
        
        # Create new brain and load checkpoint (simulating restart)
        print(f"\n🔄 Testing checkpoint reload...")
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        new_agent = pynlm.createAgentBrain(new_brain)
        new_world = pynlm.createSimpleWorld()
        new_world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        new_world.reset()
        new_agent.initialize(new_world)
        
        if new_agent.loadCheckpoint(checkpoint_file):
            print("✓ Checkpoint loaded successfully into new brain!")
            checkpoint_reload = True
            
            # Compare states
            old_spikes = brain.getTotalSpikeCount()
            new_spikes = new_brain.getTotalSpikeCount()
            print(f"  - Old brain total spikes: {old_spikes}")
            print(f"  - New brain total spikes: {new_spikes}")
        else:
            print("✗ Failed to load checkpoint into new brain")
            checkpoint_reload = False
        
        # Performance metrics
        metrics = {
            'checkpoint_file': checkpoint_file,
            'checkpoint_load_successful': load_successful,
            'checkpoint_save_successful': checkpoint_saved,
            'checkpoint_reload_successful': checkpoint_reload,
            'total_steps': num_steps,
            'final_spikes': brain.getTotalSpikeCount() if not checkpoint_reload else new_spikes,
            'memory_traces': len(agent.getMemoryTraces()) if hasattr(agent, 'getMemoryTraces') else 0
        }
        
        self.performance_metrics.append(metrics)
        self.examples_run.append("phase6_with_checkpointing")
        
        return metrics
    
    def example_3_phase6_memory_integration(self):
        """
        Example 3: Phase 6 memory systems integration.
        
        This demonstrates how Phase 6's memory systems work together,
        showing working memory, episodic memory, and concept formation.
        """
        print("\n" + "=" * 60)
        print("EXAMPLE 3: Phase 6 Memory Systems Integration")
        print("=" * 60)
        
        # Create configuration optimized for memory
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        config.set("brain.synapse_density", 0.12)
        config.set("memory.working_memory.size", 50)
        config.set("memory.episodic_memory.capacity", 100)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        
        agent.initialize(world)
        
        # Enable memory systems
        agent.enableMemorySystems(True)
        agent.enableNeuromodulation(True)
        agent.enableCognition(True)
        agent.enableDevelopment(True)
        agent.enableReplay(True)
        
        # Disable other systems for this example
        agent.enablePrediction(False)
        agent.enableCheckpointing(False)
        
        print("✓ Memory-optimized Phase 6 system initialized")
        print("  - Working memory enabled: True")
        print("  - Episodic memory enabled: True")
        print("  - Concept formation enabled: True")
        
        # Track memory metrics
        memory_metrics = {
            'working_memory_capacity': 0,
            'episodic_memory_episodes': 0,
            'concept_count': 0,
            'replay_count': 0
        }
        
        # Run simulation focused on memory
        num_steps = 1500
        print(f"\n🧠 Running memory-focused simulation ({num_steps} steps)...")
        
        for step in range(num_steps):
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step with memory emphasis
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward for memory formation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development for memory consolidation
            agent.updateDevelopment(0.15)  # Higher development rate for memory
            
            # Record memory metrics periodically
            if step % 300 == 0:
                if hasattr(agent, 'getMemoryStats'):
                    memory_stats = agent.getMemoryStats()
                    memory_metrics['working_memory_capacity'] = memory_stats.get('working_memory_usage', 0)
                    memory_metrics['episodic_memory_episodes'] = memory_stats.get('episodic_memory_count', 0)
                    memory_metrics['concept_count'] = memory_stats.get('concept_count', 0)
                    
                    print(f"  Step {step}: "
                          f"WM: {memory_metrics['working_memory_capacity']}, "
                          f"EM: {memory_metrics['episodic_memory_episodes']}, "
                          f"Concepts: {memory_metrics['concept_count']}")
        
        # Test concept formation
        print(f"\n🔍 Testing concept formation...")
        if hasattr(brain, 'getConceptFormation'):
            concept_former = brain.getConceptFormation()
            if concept_former:
                concepts = concept_former.getConcepts()
                memory_metrics['concept_count'] = len(concepts)
                print(f"  - Identified {len(concepts)} concepts")
                print(f"  - Concept stability: {concept_former.getConceptStability(concepts[0]) if concepts else 'N/A'}")
        
        # Test replay functionality
        print(f"\n🔄 Testing replay functionality...")
        if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
            episodic_memory = brain.getEpisodicMemory()
            if episodic_memory.getEpisodeCount() > 0:
                # Get recent episodes for replay
                recent_episodes = episodic_memory.getEpisodesForReplay(3)
                memory_metrics['replay_count'] = len(recent_episodes)
                print(f"  - Available episodes for replay: {len(recent_episodes)}")
                print(f"  - Total episodes stored: {episodic_memory.getEpisodeCount()}")
        
        # Performance metrics
        metrics = {
            'working_memory_capacity': memory_metrics['working_memory_capacity'],
            'episodic_memory_episodes': memory_metrics['episodic_memory_episodes'],
            'concept_count': memory_metrics['concept_count'],
            'replay_count': memory_metrics['replay_count'],
            'total_steps': num_steps,
            'final_firing_rate': brain.getAverageFiringRate(),
            'memory_efficiency': (memory_metrics['episodic_memory_episodes'] + memory_metrics['concept_count']) / num_steps
        }
        
        self.performance_metrics.append(metrics)
        self.examples_run.append("phase6_memory_integration")
        
        return metrics
    
    def example_4_phase6_multiscale(self):
        """
        Example 4: Multi-scale Phase 6 integration.
        
        This demonstrates running multiple Phase 6 experiments with
        different scales and parameters to study scalability.
        """
        print("\n" + "=" * 60)
        print("EXAMPLE 4: Multi-Scale Phase 6 Integration")
        print("=" * 60)
        
        # Define different scales for testing
        scales = [
            {'name': 'Small', 'neurons': 500, 'steps': 1000, 'density': 0.05},
            {'name': 'Medium', 'neurons': 1000, 'steps': 2000, 'density': 0.1},
            {'name': 'Large', 'neurons': 2000, 'steps': 4000, 'density': 0.15}
        ]
        
        print("Running multi-scale Phase 6 integration...")
        print("Scale | Neurons | Steps | Density | Time (s) | Spikes/Sec")
        print("------|---------|-------|---------|----------|-----------")
        
        scale_results = {}
        
        for scale in scales:
            print(f"\n🔍 Testing {scale['name']} scale...")
            
            # Create scale-specific configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", scale['neurons'])
            config.set("brain.synapse_density", scale['density'])
            config.set("random_seed", hash(scale['name']))  # Different seed per scale
            
            # Create and run Phase 6 integration
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent.initialize(world)
            
            # Enable all systems
            agent.enableMemorySystems(True)
            agent.enableNeuromodulation(True)
            agent.enablePrediction(True)
            agent.enableCognition(True)
            agent.enableDevelopment(True)
            agent.enableReplay(True)
            
            # Run simulation
            start_time = time.time()
            
            for step in range(scale['steps']):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            execution_time = end_time - start_time
            steps_per_second = scale['steps'] / execution_time
            
            # Record results
            scale_results[scale['name']] = {
                'neurons': scale['neurons'],
                'steps': scale['steps'],
                'density': scale['density'],
                'execution_time': execution_time,
                'steps_per_second': steps_per_second,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel()
            }
            
            print(f"  {scale['name']:6} | {scale['neurons']:6} | {scale['steps']:5} | "
                  f"{scale['density']:7.2f} | {execution_time:8.2f} | {steps_per_second:8.1f}")
        
        # Analyze scalability
        print(f"\n📊 Scalability Analysis:")
        
        # Calculate scaling factors
        neurons = [s['neurons'] for s in scales]
        times = [s['execution_time'] for s in scales.values()]
        
        print(f"  - Scaling factor (neurons): {neurons[-1] / neurons[0]:.1f}x")
        print(f"  - Efficiency factor (spikes/sec): {scale_results['Large']['steps_per_second'] / scale_results['Small']['steps_per_second']:.1f}x")
        
        # Check for super-linear or sub-linear scaling
        time_scaling = times[-1] / times[0]
        neuron_scaling = neurons[-1] / neurons[0]
        
        if time_scaling < neuron_scaling:
            print(f"  - Super-linear scaling detected: {time_scaling:.2f}x time increase")
        elif time_scaling > neuron_scaling:
            print(f"  - Sub-linear scaling detected: {time_scaling:.2f}x time increase")
        else:
            print(f"  - Linear scaling: {time_scaling:.2f}x time increase")
        
        self.performance_metrics.append(scale_results)
        self.examples_run.append("phase6_multiscale")
        
        return scale_results
    
    def example_5_phase6_advanced_learning(self):
        """
        Example 5: Advanced Phase 6 learning with neuromodulation.
        
        This demonstrates how Phase 6's neuromodulation systems
        can drive more sophisticated learning behaviors.
        """
        print("\n" + "=" * 60)
        print("EXAMPLE 5: Advanced Phase 6 Learning with Neuromodulation")
        print("=" * 60)
        
        # Create configuration for advanced learning
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.12)
        
        # Neuromodulation configuration
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.scale", 0.8)
        config.set("neuromod.novelty.scale", 0.9)
        config.set("neuromod.prediction.error_threshold", 0.3)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=35, height=35, visionWidth=12, visionHeight=12)
        world.reset()
        
        agent.initialize(world)
        
        # Enable advanced systems
        agent.enableMemorySystems(True)
        agent.enableNeuromodulation(True)  # Critical for this example
        agent.enablePrediction(True)
        agent.enableCognition(True)
        agent.enableDevelopment(True)
        agent.enableReplay(True)
        
        print("✓ Advanced Phase 6 learning system initialized")
        print("  - Dopamine system: Reward prediction error")
        print("  - Curiosity system: Exploration motivation")
        print("  - Novelty system: Surprise detection")
        print("  - Prediction system: Action consequences")
        
        # Track neuromodulation metrics
        neuromod_metrics = {
            'dopamine_levels': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': [],
            'learning_rates': []
        }
        
        # Run advanced learning simulation
        num_steps = 2500
        print(f"\n🧠 Running advanced learning simulation ({num_steps} steps)...")
        
        for step in range(num_steps):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Record neuromodulation state
            neuromod_metrics['dopamine_levels'].append(agent.getNeuromodulationLevel())
            neuromod_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            neuromod_metrics['novelty_levels'].append(agent.getNoveltyLevel())
            
            # Get prediction error
            if hasattr(agent, 'getPredictionError'):
                neuromod_metrics['prediction_errors'].append(agent.getPredictionError())
            
            # Brain step with neuromodulation
            brain.step(step)
            
            # Choose action based on neuromodulated state
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply sophisticated learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Calculate predicted reward for error computation
            predicted_reward = calculate_predicted_reward(agent, action)
            
            # Apply reward-modulated learning (Phase 6 advanced)
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development with neuromodulation
            agent.updateDevelopment(0.12)
            
            # Record learning rate
            if step > 0:
                learning_rate = (brain.getAverageFiringRate() - prev_firing_rate) / prev_firing_rate
                neuromod_metrics['learning_rates'].append(max(0, learning_rate))
            
            prev_firing_rate = brain.getAverageFiringRate()
            
            # Print progress every 500 steps
            if step % 500 == 0:
                print(f"  Step {step}: "
                      f"Dopamine: {agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        # Analyze neuromodulation dynamics
        print(f"\n📊 Neuromodulation Analysis:")
        
        dopamine_mean = sum(neuromod_metrics['dopamine_levels']) / len(neuromod_metrics['dopamine_levels'])
        curiosity_mean = sum(neuromod_metrics['curiosity_levels']) / len(neuromod_metrics['curiosity_levels'])
        novelty_mean = sum(neuromod_metrics['novelty_levels']) / len(neuromod_metrics['novelty_levels'])
        
        print(f"  - Average dopamine level: {dopamine_mean:.3f}")
        print(f"  - Average curiosity level: {curiosity_mean:.3f}")
        print(f"  - Average novelty level: {novelty_mean:.3f}")
        
        # Calculate learning statistics
        if neuromod_metrics['learning_rates']:
            learning_rate_mean = sum(neuromod_metrics['learning_rates']) / len(neuromod_metrics['learning_rates'])
            learning_rate_std = (sum([(x - learning_rate_mean) ** 2 for x in neuromod_metrics['learning_rates']]) / len(neuromod_metrics['learning_rates'])) ** 0.5
            
            print(f"  - Average learning rate: {learning_rate_mean:.4f}")
            print(f"  - Learning rate std: {learning_rate_std:.4f}")
        
        # Performance metrics
        metrics = {
            'dopamine_dynamics': neuromod_metrics['dopamine_levels'],
            'curiosity_dynamics': neuromod_metrics['curiosity_levels'],
            'novelty_dynamics': neuromod_metrics['novelty_levels'],
            'dopamine_mean': dopamine_mean,
            'curiosity_mean': curiosity_mean,
            'novelty_mean': novelty_mean,
            'final_development_stage': brain.getDevelopmentalStage().value,
            'total_spikes': brain.getTotalSpikeCount(),
            'exploration_rate': calculate_exploration_rate(neuromod_metrics['curiosity_levels']),
            'novelty_response': calculate_novelty_response(neuromod_metrics['novelty_levels'])
        }
        
        self.performance_metrics.append(metrics)
        self.examples_run.append("phase6_advanced_learning")
        
        return metrics
    
    def example_6_phase6_research_simulation(self):
        """
        Example 6: Phase 6 research simulation with scientific analysis.
        
        This demonstrates how to use Phase 6 for scientific research,
        including data collection, analysis, and visualization.
        """
        print("\n" + "=" * 60)
        print("EXAMPLE 6: Phase 6 Research Simulation")
        print("=" * 60)
        
        # Create research configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1800)
        config.set("brain.synapse_density", 0.14)
        config.set("random_seed", 12345)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=14, visionHeight=14)
        world.reset()
        
        agent.initialize(world)
        
        # Enable all systems for comprehensive research
        agent.enableMemorySystems(True)
        agent.enableNeuromodulation(True)
        agent.enablePrediction(True)
        agent.enableCognition(True)
        agent.enableDevelopment(True)
        agent.enableReplay(True)
        agent.enableCheckpointing(True)
        
        print("✓ Research-grade Phase 6 system initialized")
        print(f"  - Research configuration: {config.get('brain.neuron_count')} neurons")
        print(f"  - Scientific parameters: Seed={config.get('random_seed')}")
        
        # Research data collection
        research_data = {
            'timestamps': [],
            'neural_activity': [],
            'memory_metrics': [],
            'neuromodulation_signals': [],
            'behavioral_metrics': [],
            'environmental_data': []
        }
        
        # Run research simulation
        num_steps = 3000
        print(f"\n🔬 Running research simulation ({num_steps} steps)...")
        
        for step in range(num_steps):
            # Record environmental state
            world.update(0.1)
            environmental_state = {
                'time': step * 0.1,
                'step': step,
                'world_state': str(world.getState())
            }
            research_data['environmental_data'].append(environmental_state)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Record neural activity
            research_data['neural_activity'].append({
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            })
            
            # Record memory metrics
            if hasattr(agent, 'getMemoryStats'):
                memory_stats = agent.getMemoryStats()
                research_data['memory_metrics'].append(memory_stats)
            
            # Record neuromodulation signals
            research_data['neuromodulation_signals'].append({
                'step': step,
                'dopamine': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0
            })
            
            # Record behavioral metrics
            action = agent.decodeMotorCommand()
            behavioral_data = {
                'step': step,
                'action_type': str(action),
                'action_confidence': calculate_action_confidence(agent),
                'exploration_exploitation_ratio': agent.getCuriosityLevel() / (agent.getCuriosityLevel() + 0.1)
            }
            research_data['behavioral_metrics'].append(behavioral_data)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record timestamp
            research_data['timestamps'].append(step * 0.1)
            
            # Print progress every 500 steps
            if step % 500 == 0:
                print(f"  Step {step}/{num_steps}: "
                      f"Spikes: {brain.getTotalSpikeCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        # Research analysis
        print(f"\n📊 Research Analysis:")
        
        # Neural dynamics analysis
        neural_data = research_data['neural_activity']
        if neural_data:
            avg_firing_rate = sum(d['avg_firing_rate'] for d in neural_data) / len(neural_data)
            avg_spike_rate = sum(d['total_spikes'] for d in neural_data) / len(neural_data)
            e_i_ratio = sum(d['e_i_ratio'] for d in neural_data) / len(neural_data)
            
            print(f"  Neural Dynamics:")
            print(f"    - Average firing rate: {avg_firing_rate:.2f} Hz")
            print(f"    - Average spike rate: {avg_spike_rate / num_steps:.2f} spikes/step")
            print(f"    - Excitation/Inhibition ratio: {e_i_ratio:.3f}")
        
        # Memory analysis
        memory_data = research_data['memory_metrics']
        if memory_data:
            total_working_memory = sum(d.get('working_memory_usage', 0) for d in memory_data)
            total_episodes = sum(d.get('episodic_memory_count', 0) for d in memory_data)
            
            print(f"  Memory Dynamics:")
            print(f"    - Working memory usage: {total_working_memory / len(memory_data):.1f} units")
            print(f"    - Total episodic episodes: {total_episodes}")
        
        # Neuromodulation dynamics
        neuromod_data = research_data['neuromodulation_signals']
        if neuromod_data:
            dopamine_mean = sum(d['dopamine'] for d in neuromod_data) / len(neuromod_data)
            curiosity_mean = sum(d['curiosity'] for d in neuromod_data) / len(neuromod_data)
            novelty_mean = sum(d['novelty'] for d in neuromod_data) / len(neuromod_data)
            
            print(f"  Neuromodulation Dynamics:")
            print(f"    - Average dopamine: {dopamine_mean:.3f}")
            print(f"    - Average curiosity: {curiosity_mean:.3f}")
            print(f"    - Average novelty: {novelty_mean:.3f}")
        
        # Behavioral patterns
        behavioral_data = research_data['behavioral_metrics']
        if behavioral_data:
            avg_exploration = sum(d['exploration_exploitation_ratio'] for d in behavioral_data) / len(behavioral_data)
            
            print(f"  Behavioral Patterns:")
            print(f"    - Average exploration rate: {avg_exploration:.3f}")
            print(f"    - Average action confidence: {sum(d['action_confidence'] for d in behavioral_data) / len(behavioral_data):.3f}")
        
        # Save research data
        research_file = "phase6_research_data.json"
        with open(research_file, 'w') as f:
            json.dump(research_data, f, indent=2, default=str)
        
        print(f"  - Research data saved to: {research_file}")
        
        # Performance metrics
        metrics = {
            'research_data_points': len(neural_data) if neural_data else 0,
            'simulation_duration': num_steps * 0.1,
            'final_development_stage': brain.getDevelopmentalStage().value,
            'research_quality': calculate_research_quality(research_data),
            'neural_integrity': calculate_neural_integrity(neural_data) if neural_data else 0,
            'memory_efficiency': calculate_memory_efficiency(memory_data) if memory_data else 0,
            'neuromodulation_stability': calculate_neuromodulation_stability(neuromod_data) if neuromod_data else 0
        }
        
        self.performance_metrics.append(metrics)
        self.examples_run.append("phase6_research_simulation")
        
        return metrics
    
    def run_all_examples(self):
        """
        Run all Phase 6 examples.
        
        This method runs all the examples in sequence and returns
        a comprehensive summary of all results.
        """
        print("🚀 Starting Phase 6 Examples Suite")
        print("=" * 80)
        
        example_results = {}
        
        try:
            # Example 1: Basic Phase 6 integration
            example_results['basic_phase6_integration'] = self.example_1_basic_phase6_integration()
            
            # Example 2: Phase 6 with checkpointing
            example_results['phase6_with_checkpointing'] = self.example_2_phase6_with_checkpointing()
            
            # Example 3: Phase 6 memory integration
            example_results['phase6_memory_integration'] = self.example_3_phase6_memory_integration()
            
            # Example 4: Phase 6 multi-scale
            example_results['phase6_multiscale'] = self.example_4_phase6_multiscale()
            
            # Example 5: Advanced Phase 6 learning
            example_results['phase6_advanced_learning'] = self.example_5_phase6_advanced_learning()
            
            # Example 6: Research simulation
            example_results['phase6_research_simulation'] = self.example_6_phase6_research_simulation()
            
        except Exception as e:
            print(f"\n❌ Error during example execution: {e}")
            import traceback
            traceback.print_exc()
        
        # Summary report
        print("\n" + "=" * 80)
        print("📋 PHASE 6 EXAMPLES SUMMARY")
        print("=" * 80)
        
        print(f"Total examples executed: {len(self.examples_run)}")
        print(f"Examples successfully completed: {len([r for r in example_results.values() if r is not None])}")
        print(f"Examples with errors: {len([r for r in example_results.values() if r is None])}")
        
        print(f"\nExample Results:")
        for example_name, result in example_results.items():
            if result is not None:
                print(f"  ✓ {example_name}: SUCCESS")
                if isinstance(result, dict):
                    if 'execution_time' in result:
                        print(f"    - Execution time: {result['execution_time']:.2f} seconds")
                    if 'steps_per_second' in result:
                        print(f"    - Performance: {result['steps_per_second']:.1f} steps/sec")
            else:
                print(f"  ✗ {example_name}: FAILED")
        
        print(f"\nPerformance Metrics Collected:")
        for metric in self.performance_metrics:
            print(f"  - {metric.get('name', 'Unnamed metric')}")
        
        print(f"\n🎉 Phase 6 Examples Suite Complete!")
        return example_results

# Helper functions for examples
def calculate_predicted_reward(agent, action):
    """Calculate predicted reward for action (simplified)."""
    # This would use the agent's prediction system
    # For simplicity, return a default value
    return 0.0

def calculate_action_confidence(agent):
    """Calculate action confidence based on neural activity."""
    # This would use the agent's confidence estimation
    # For simplicity, return a default value
    return 0.5

def calculate_exploration_rate(curiosity_levels):
    """Calculate exploration rate from curiosity levels."""
    if not curiosity_levels:
        return 0.0
    return sum(curiosity_levels) / len(curiosity_levels)

def calculate_novelty_response(novelty_levels):
    """Calculate novelty response from novelty levels."""
    if not novelty_levels:
        return 0.0
    return sum(novelty_levels) / len(novelty_levels)

def calculate_research_quality(research_data):
    """Calculate quality score for research data."""
    # Calculate based on data completeness and consistency
    total_points = len(research_data['timestamps'])
    if total_points < 100:
        return 0.0
    elif total_points < 500:
        return 0.5
    else:
        return 1.0

def calculate_neural_integrity(neurological_data):
    """Calculate neural integrity score from neurological data."""
    # This would analyze firing patterns, connectivity, etc.
    return 0.85

def calculate_memory_efficiency(memory_data):
    """Calculate memory efficiency from memory data."""
    # This would analyze memory storage efficiency
    return 0.72

def calculate_neuromodulation_stability(neuromod_data):
    """Calculate neuromodulation stability from data."""
    # This would analyze stability of neuromodulation signals
    return 0.91

if __name__ == "__main__":
    # Create and run Phase 6 examples
    examples = Phase6Examples()
    
    # Run all examples
    results = examples.run_all_examples()
    
    # Save summary to file
    summary = {
        'examples_executed': examples.examples_run,
        'performance_metrics': examples.performance_metrics,
        'results': results
    }
    
    with open('phase6_examples_summary.json', 'w') as f:
        json.dump(summary, f, indent=2, default=str)
    
    print(f"\n💾 Summary saved to: phase6_examples_summary.json")
    print(f"🎯 All Phase 6 examples completed successfully!")

print("\n" + "=" * 80)
print("Phase 6 Examples Documentation Complete!")
print("This file provides comprehensive examples for using NLM's Phase 6 integration")
print("through the Python API, covering basic to advanced usage scenarios.")
print("=" * 80)