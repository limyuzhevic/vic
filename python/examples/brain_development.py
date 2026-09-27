#!/usr/bin/env python3
"""
Brain Development and Adaptation Example

This example demonstrates how brain systems develop and adapt over time
through learning, memory consolidation, and neuromodulation.
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
import numpy as np

def example_development_stages():
    """Demonstrate brain development through different stages."""
    print("=== Brain Development Example ===")
    
    # Create brain with development
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent_brain = nlm.createAgentBrain(brain)
    
    print("✓ Created developing brain")
    
    # Set different developmental stages
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableRewardModulation(True)
    
    print("✓ Enabled development and neuromodulation")
    
    # Simulate development over time
    for stage in range(10):
        # Set developmental stage
        stage_name = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"][stage % 5]
        brain.setDevelopmentalStage(stage_name)
        
        print(f"  Stage {stage + 1}: {stage_name}")
        
        # Get developmental stage
        current_stage = brain.getDevelopmentalStage()
        print(f"    Current: {current_stage}")
        
        # Update development
        agent_brain.updateDevelopment(1.0)
        
        # Get neuromodulation level
        neuromodulation_level = brain.getNeuromodulationLevel()
        curiosity_level = brain.getCuriosityLevel()
        
        print(f"    Neuromodulation: {neuromodulation_level:.3f}")
        print(f"    Curiosity: {curiosity_level:.3f}")
        
        # Run a simple cognitive step
        if stage % 2 == 0:
            working_memory = brain.get_cognitive_systems().get_neural_working_memory()
            pattern = np.random.rand(5).astype(float)
            working_memory.store(pattern, 1.0)
            working_memory.runCompetition()

def example_memory_replay():
    """Demonstrate memory replay and consolidation."""
    print("\n=== Memory Replay Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    episodic_memory = brain.get_cognitive_systems().get_episodic_memory()
    
    print("✓ Created episodic memory system")
    
    # Create some episodic memories
    memories = []
    for i in range(5):
        memory = nlm.EpisodicMemoryItem()
        memory.sensoryState = np.random.rand(10).astype(float)
        memory.positionX = i * 0.5
        memory.positionY = i * 0.3
        memory.orientation = i * 0.2
        memory.action = nlm.ActionType.MoveForward if i % 2 == 0 else nlm.ActionType.TurnLeft
        memory.reward = np.random.rand()
        memory.energy = 1.0 - i * 0.1
        memory.novelty = np.random.rand()
        memory.resultingSensoryState = memory.sensoryState * 0.8 + np.random.rand(10) * 0.1
        memory.resultingReward = memory.reward
        memory.age = i
        
        memories.append(memory)
        episodic_memory.storeEpisode(memory)
        print(f"  Stored memory {i+1}: reward={memory.reward:.3f}, novelty={memory.novelty:.3f}")
    
    # Enable replay
    episodic_memory.enableReplay(True)
    print("✓ Enabled replay")
    
    # Get memories for replay
    episodes_to_replay = episodic_memory.getEpisodesForReplay(3)
    print(f"✓ Got {len(episodes_to_replay)} episodes for replay")
    
    # Replay some episodes
    if len(episodes_to_replay) >= 2:
        episodic_memory.replayEpisode(episodes_to_replay[0])
        print("✓ Replayed most relevant episode")
    
    # Consolidate memory
    episodic_memory.consolidate(0.5)
    print("✓ Consolidated memory (threshold=0.5)")
    
    # Get statistics
    avg_reward = episodic_memory.getAverageReward()
    recent = episodic_memory.getRecentEpisodes(3)
    print(f"✓ Average reward: {avg_reward:.3f}")
    print(f"✓ Recent episodes: {len(recent)}")

def example_attention_and_focus():
    """Demonstrate attentional selection and focus."""
    print("\n=== Attention Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    attentional_selection = brain.get_cognitive_systems().get_attentional_selection()
    
    print("✓ Created attentional selection system")
    
    # Set attention parameters
    attentional_selection.setInhibitionStrength(0.5)
    attentional_selection.setExcitationStrength(1.5)
    attentional_selection.setCompetitionThreshold(0.3)
    
    print(f"✓ Set inhibition strength: 0.5")
    print(f"✓ Set excitation strength: 1.5")
    print(f"✓ Set competition threshold: 0.3")
    
    # Create test neurons
    test_neurons = [nlm.NeuronId(i) for i in range(10)]
    
    # Apply top-down and bottom-up attention
    for i, neuron in enumerate(test_neurons[:5]):
        attentional_selection.applyTopDownBias(neuron, 0.3 * (i + 1))
        attentional_selection.applyBottomUpSalience(neuron, 0.4 * (5 - i))
    
    print("✓ Applied attentional biases")
    
    # Test competition
    winners = attentional_selection.processCompetition(test_neurons, 0.5)
    print(f"✓ Competition winners: {len(winners)} neurons")
    
    # Check attention status
    for neuron in test_neurons[:3]:
        is_attended = attentional_selection.isAttended(neuron)
        inhibition = attentional_selection.getInhibitionFor(neuron)
        excitation = attentional_selection.getExcitationFor(neuron)
        print(f"  Neuron {neuron.value}: attended={is_attended}, inhibition={inhibition:.3f}, excitation={excitation:.3f}")
    
    # Focus on a specific region
    attentional_selection.focusOnRegion(nlm.RegionId(100))
    attended_regions = attentional_selection.getAttendedRegions()
    print(f"✓ Focused on region: {attended_regions}")

def example_neuromodulation_integration():
    """Demonstrate neuromodulation integration with plasticity."""
    print("\n=== Neuromodulation Integration Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent_brain = nlm.createAgentBrain(brain)
    
    print("✓ Created integrated neuromodulation system")
    
    # Enable all neuromodulation types
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    print("✓ Enabled reward, curiosity, and development")
    
    # Create test scenario
    world = nlm.createSimpleWorld()
    
    # Simulate learning episodes
    for episode in range(8):
        # Generate random experience
        sensory_input = nlm.Vision(10, 10)  # 10x10 vision field
        percept = nlm.SensoryPercept()
        percept.setVision(sensory_input)
        
        # Process through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Decode action
        action = agent_brain.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(action, 0.0)
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(result.reward, 0.5 * result.reward)
        
        # Get neuromodulation levels
        neuromodulation_level = brain.getNeuromodulationLevel()
        curiosity_level = brain.getCuriosityLevel()
        novelty_level = brain.getNoveltyLevel()
        prediction_error = brain.getPredictionError()
        
        print(f"  Episode {episode + 1}: reward={result.reward:.3f}, "
              f"neuromod={neuromodulation_level:.3f}, "
              f"curiosity={curiosity_level:.3f}, "
              f"novelty={novelty_level:.3f}, "
              f"pred_error={prediction_error:.3f}")
        
        # Update development
        agent_brain.updateDevelopment(1.0)

def main():
    """Run all development examples."""
    print("NLM Brain Development and Adaptation Examples")
    print("=" * 60)
    
    try:
        example_development_stages()
        example_memory_replay()
        example_attention_and_focus()
        example_neuromodulation_integration()
        
        print("\n" + "=" * 60)
        print("All development examples completed successfully!")
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
