"""
Advanced Learning and Memory Systems Example

This script demonstrates advanced usage of NLM's memory systems
and learning capabilities, including working memory, episodic memory,
and associative memory integration.
"""

import pynlm
import time
from nlm_error_handling import (
    NLMErrorHandler, run_safe_simulation, validate_brain, 
    validate_agent, validate_world, safe_simulation_step
)
def create_advanced_learning_config():
    """Create an advanced configuration optimized for learning"""
    config = pynlm.createDefaultConfig()
    
    # Increase neuron count for more complex learning
    config.set("neuron_count", 3000)
    config.set("region_count", 3)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("plasticity.structural.enable", True)
    
    # Advanced neuromodulation settings
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    
    # Advanced development settings
    config.set("development.initial_plasticity", 1.5)
    config.set("development.synaptogenesis_rate", 0.00002)
    config.set("development.pruning_rate", 0.000005)
    
    return config
def run_memory_intensive_learning():
    """Run a comprehensive learning session with memory system integration"""
    print("=== Advanced Learning and Memory Systems Example ===\n")
    
    # 1. Create advanced configuration
    print("Step 1: Creating advanced configuration...")
    config = create_advanced_learning_config()
    
    # 2. Create brain with enhanced learning capabilities
    print("Step 2: Initializing brain with advanced learning systems...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Access and configure memory systems
    print("Step 3: Configuring memory systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    if working_memory:
        working_memory.setCapacity(500)
        working_memory.enableLateralInhibition(True)
        
    if episodic_memory:
        episodic_memory.setMaxEpisodes(1000)
        episodic_memory.enableReplayDuringRest(True)
        episodic_memory.setReplayFrequency(50)  # Replay every 50 steps
        
    if associative_memory:
        associative_memory.setAssociationStrength(0.8)
        associative_memory.enablePatternCompletion(True)
    
    # 4. Create agent with advanced learning capabilities
    print("Step 4: Creating advanced agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 5. Create complex world for learning
    print("Step 5: Creating learning environment...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,  # Larger world for more exploration
        height=30,
        visionWidth=12,  # Better vision range
        visionHeight=12
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Center of world
    
    # Configure world with resources and hazards
    for i in range(8):  # Place resources
        x = 5 + i * 3
        y = 5 + i * 2
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0))
    
    agent.initialize(world)
    
    # 6. Run learning episodes with comprehensive monitoring
    print("Step 6: Starting learning episodes...")
    episodes_completed = 0
    learning_metrics = {
        'total_spikes': 0,
        'memory_traces': 0,
        'episodes_stored': 0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': []
    }
    
    # Use error handling for safe simulation
    with NLMErrorHandler(brain, agent, world) as error_handler:
        try:
            for episode in range(5):  # Run 5 episodes
                print(f"\n--- Episode {episode + 1} ---")
                
                episode_spikes = 0
                episode_time = 0
                
                # Run episode with step-by-step monitoring
                for step in range(300):  # Each episode has 300 steps
                    step_start = time.time()
                    
                    # Update world
                    world.update(0.1)
                    episode_time += 0.1
                    
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Process through agent with advanced features
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Apply advanced neuromodulation
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    agent.applyRewardModulation(reward, 0.0)
                    agent.updateDevelopment(0.1)
                    
                    # Collect metrics
                    episode_spikes += brain.getTotalSpikeCount() - learning_metrics['total_spikes']
                    
                    # Monitor agent state
                    curiosity = agent.getCuriosityLevel()
                    novelty = agent.getNoveltyLevel()
                    development_stage = agent.getDevelopmentalStage()
                    
                    learning_metrics['curiosity_levels'].append(curiosity)
                    learning_metrics['novelty_levels'].append(novelty)
                    learning_metrics['development_stages'].append(development_stage)
                    
                    # Advanced memory system operations
                    if working_memory:
                        active_traces = working_memory.getActiveTraces()
                        learning_metrics['memory_traces'] = max(learning_metrics['memory_traces'], active_traces)
                    
                    if episodic_memory:
                        episodes_stored = episodic_memory.getEpisodeCount()
                        learning_metrics['episodes_stored'] = max(learning_metrics['episodes_stored'], episodes_stored)
                    
                    # Print progress
                    if step % 50 == 0:
                        print(f"  Step {step}: "
                              f"Firing={brain.getFiringNeuronCount()}, "
                              f"Curiosity={curiosity:.3f}, "
                              f"Novelty={novelty:.3f}, "
                              f"Dev={development_stage}, "
                              f"Action={action}")
                    
                    # Check for development stage transitions
                    if step % 100 == 0:
                        print(f"  Development Stage: {agent.getDevelopmentalStage()}")
                        print(f"  Plasticity Modifier: {agent.getBrain().getDevelopmentSystem().getPlasticityModifier():.3f}")
                
                episodes_completed += 1
                learning_metrics['total_spikes'] = brain.getTotalSpikeCount()
                
                print(f"Episode {episode + 1} complete: {episode_spikes} spikes, {episode_time:.1f}s")
                
        except Exception as e:
            error_handler.add_error(e)
            print(f"Error during learning: {e}")
    
    # 7. Analyze and display learning results
    print("\n=== Learning Analysis ===")
    print(f"Episodes completed: {episodes_completed}")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Peak memory traces: {learning_metrics['memory_traces']}")
    print(f"Peak episodes stored: {learning_metrics['episodes_stored']}")
    
    # Calculate learning metrics
    if learning_metrics['curiosity_levels']:
        avg_curiosity = sum(learning_metrics['curiosity_levels']) / len(learning_metrics['curiosity_levels'])
        avg_novelty = sum(learning_metrics['novelty_levels']) / len(learning_metrics['novelty_levels'])
        
        print(f"Average curiosity: {avg_curiosity:.3f}")
        print(f"Average novelty: {avg_novelty:.3f}")
        print(f"Curiosity stability: {max(learning_metrics['curiosity_levels']) - min(learning_metrics['curiosity_levels']):.3f}")
    
    # Show development progression
    print(f"\nDevelopment Stage Progression: {set(learning_metrics['development_stages'])}")
    
    # 8. Save learned brain state
    print("\n=== Saving Learned Brain State ===")
    brain.save("advanced_learning_brain.bin")
    print("Brain state saved to advanced_learning_brain.bin")
    
    # 9. Export learning metrics
    print("\n=== Learning Metrics Summary ===")
    metrics_dict = {
        'episodes_completed': episodes_completed,
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'final_development_stage': brain.getDevelopmentalStage(),
        'peak_memory_traces': learning_metrics['memory_traces'],
        'peak_episodes_stored': learning_metrics['episodes_stored'],
        'average_curiosity': avg_curiosity if learning_metrics['curiosity_levels'] else 0,
        'average_novelty': avg_novelty if learning_metrics['novelty_levels'] else 0,
    }
    
    import json
    with open("learning_metrics.json", "w") as f:
        json.dump(metrics_dict, f, indent=2, default=str)
    
    print("Learning metrics saved to learning_metrics.json")
    
    return metrics_dict, brain, agent, world
def demonstrate_advanced_memory_functions():
    """Demonstrate advanced memory system functions"""
    print("\n=== Advanced Memory Systems Demonstration ===\n")
    
    # Create brain for memory demonstration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if not working_memory or not episodic_memory:
        print("Memory systems not available for demonstration")
        return
    
    # 1. Working Memory Demonstration
    print("1. Working Memory Demonstration:")
    print("   - Storing neural activation patterns...")
    
    # Simulate storing different activation patterns
    for i in range(5):
        working_memory.storeToNeuron(i, 0.5 + (i * 0.1))
        time.sleep(0.01)  # Small delay for demonstration
    
    active_traces = working_memory.getActiveTraces()
    memory_neuron = working_memory.getMemoryNeurons()
    
    print(f"   - Active memory traces: {active_traces}")
    print(f"   - Memory neurons: {len(memory_neuron) if memory_neuron else 0}")
    
    # 2. Working Memory Competition
    print("\n2. Working Memory Competition:")
    working_memory.setInhibitionStrength(0.7)
    working_memory.setExcitationStrength(1.3)
    
    for i in range(3):
        working_memory.storeToNeuron(10 + i, 0.8 - (i * 0.2))
    
    print("   - Stored multiple competing patterns")
    print("   - Inhibition strength: 0.7, Excitation strength: 1.3")
    
    # 3. Episodic Memory Demonstration
    print("\n3. Episodic Memory Demonstration:")
    print("   - Encoding experiences as episodic memories...")
    
    # Create episodic memory items
    for i in range(3):
        episodic_memory_item = pynlm.EpisodicMemoryItem()
        episodic_memory_item.timestamp = i
        episodic_memory_item.reward = 1.0 - (i * 0.2)
        episodic_memory_item.activeNeurons = [i * 10, i * 10 + 1, i * 10 + 2]
        episodic_memory_item.neuronActivations = [0.5, 0.7, 0.6]
        
        episodic_memory.storeEpisode(episodic_memory_item)
    
    print(f"   - Episodes stored: {episodic_memory.getEpisodeCount()}")
    
    # 4. Episodic Memory Replay
    print("\n4. Episodic Memory Replay:")
    episodic_memory.enableReplayDuringRest(True)
    episodic_memory.setReplayFrequency(20)
    
    print("   - Replay enabled with frequency: 20 steps")
    print("   - Important episodes will be replayed during rest periods")
    
    # 5. Memory Consolidation
    print("\n5. Memory Consolidation:")
    episodic_memory.consolidate(0.3)
    print("   - Consolidated weak memories with threshold: 0.3")
    print("   - Strong memories retained, weak memories pruned")
    
    print("\n=== Memory Systems Demonstration Complete ===")
    return brain
def main():
    """Main function to run all demonstrations"""
    print("NLM Advanced Learning and Memory Systems Example")
    print("=" * 60)
    
    try:
        # Run comprehensive learning example
        metrics, brain, agent, world = run_memory_intensive_learning()
        
        # Demonstrate advanced memory functions
        memory_brain = demonstrate_advanced_memory_functions()
        
        # Summary
        print("\n" + "=" * 60)
        print("EXAMPLE SUMMARY")
        print("=" * 60)
        print("✓ Advanced learning configuration created")
        print("✓ Comprehensive memory systems integrated")
        print("✓ 5 learning episodes completed")
        print("✓ Advanced error handling implemented")
        print("✓ Memory systems demonstrated")
        print("✓ Brain state saved")
        print("✓ Learning metrics exported")
        print("\nThis example demonstrates the full power of NLM's")
        print("advanced learning and memory systems, including:")
        print("• Enhanced plasticity mechanisms")
        print("• Working memory with competitive dynamics")
        print("• Episodic memory with replay and consolidation")
        print("• Neuromodulation for adaptive learning")
        print("• Development stages and maturation")
        print("• Advanced error handling and recovery")
        print("• Comprehensive monitoring and metrics")
        
    except Exception as e:
        print(f"Example failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    exit(main())