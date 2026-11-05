#!/usr/bin/env python3
"""
Enhanced pynlm Python API Demonstration

This script demonstrates the enhanced Phase 6 API for the NLM (Neural Learning Machine)
neural simulation framework. It shows how the new bindings provide access to
the complete neural, cognitive, and memory systems implemented in Phases 2-6.

The enhanced API includes:
- Full Phase 6 cognitive and social learning systems
- Complete neuromodulation control (dopamine, curiosity, novelty)
- Advanced memory systems (working, episodic, associative)
- Rich configuration API with parameter setting and retrieval
- Comprehensive diagnostic counters and statistics
- Text processing and language understanding
- Social learning and multi-agent studies
"""

import pynlm

def main():
    print("=== Enhanced NLM (Neural Learning Machine) API Demo ===\n")
    
    # 1. Create a configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    print(f"   Configuration created: {config.summary()}")
    
    # 2. Create and configure the brain
    print("\n2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # 3. Enable all advanced systems
    print("\n3. Enabling all advanced Phase 6 systems...")
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all subsystems for comprehensive simulation
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # 4. Run a simulation with advanced features
    print("\n4. Running simulation with Phase 6 systems...")
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"\n   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Total spikes: {brain.getTotalSpikeCount()}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Prediction error: {agent.getPredictionError():.3f}")
            print(f"     Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"     Dev stage: {brain.getDevelopmentalStage()}")
    
    # 5. Demonstrate Phase 6 advanced features
    print("\n5. Demonstrating Phase 6 advanced features...")
    
    # Phase 6 Text Processing
    print("\n   a) Phase 6 Text Processing:")
    text_pattern = agent.encodeText("hello world")
    print(f"      Encoded text pattern: {text_pattern}")
    
    # Working Memory
    print("\n   b) Working Memory System:")
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"      Active traces: {working_memory.getActiveTraces()}")
        print(f"      Memory neurons: {working_memory.getMemoryNeuronCount()}")
        print(f"      Memory activity: {working_memory.getMemoryActivity():.3f}")
    
    # Episodic Memory
    print("\n   c) Episodic Memory System:")
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        episode_count = episodic_memory.getEpisodeCount()
        print(f"      Total episodes: {episode_count}")
        avg_reward = episodic_memory.getAverageReward()
        print(f"      Average reward: {avg_reward:.3f}")
        recent_count = episodic_memory.getRecentEpisodes(3).size()
        print(f"      Recent episodes: {recent_count}")
    
    # Associative Memory
    print("\n   d) Associative Memory System:")
    assoc_memory = brain.getAssociativeMemory()
    if assoc_memory:
        assoc_count = assoc_memory.getAssociationCount()
        print(f"      Total associations: {assoc_count}")
    
    # Cognitive Systems
    print("\n   e) Cognitive Systems:")
    if brain.getPredictionSystem():
        print("      Prediction System: Enabled")
    if brain.getPlanner():
        print(f"      Planner: Enabled (depth: {brain.getPlanner().getPlanningDepth()})")
        print(f"      Current goal: {brain.getPlanner().getCurrentGoal()}")
    if brain.getConceptFormation():
        print(f"      Concept formation: {brain.getConceptFormation().getConceptCount()} concepts")
    if brain.getAttention():
        print("      Attentional selection: Enabled")
    
    # Neuromodulation
    print("\n   f) Neuromodulation Systems:")
    if brain.getDopamine():
        print("      Dopamine: Enabled")
    if brain.getCuriosity():
        print(f"      Curiosity level: {agent.getCuriosityLevel():.3f}")
    if brain.getNovelty():
        print(f"      Novelty level: {agent.getNoveltyLevel():.3f}")
    if brain.getPredictionErrorSignal():
        print(f"      Prediction error: {agent.getPredictionError():.3f}")
    
    # Social Learning
    print("\n   g) Social Learning:")
    observations = agent.getSocialObservationCount()
    print(f"      Social observations: {observations}")
    if observations > 0:
        imitation_action = agent.getImitationActionAndSimilarity(
            percept.getVision() if hasattr(percept, 'getVision') else []
        )
        print(f"      Best imitation action: {imitation_action.first}")
        print(f"      Similarity: {imitation_action.second:.3f}")
    
    # Advanced Statistics
    print("\n   h) Advanced Statistics:")
    print(f"      Total spike count: {brain.getTotalSpikeCount()}")
    print(f"      Pending spike events: {brain.getPendingSpikeEventCount()}")
    print(f"      Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"      E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Self-model and Thought Trace
    print("\n   i) Cognitive Transparency:")
    thought_trace = agent.getThoughtTrace()
    print(f"      Thought trace: {thought_trace}")
    
    if agent.getLearnedPreference() != 0.0:
        print(f"      Learned preference: {agent.getLearnedPreference():.3f}")
    
    # Development and Plasticity
    print("\n   j) Development and Plasticity:")
    print(f"      Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"      Self-model confidence: {agent.getSelfModelConfidence():.3f}")
    print(f"      Concept count: {agent.getConceptCount()}")
    
    # Plasticity counters
    print("\n   k) Plasticity Diagnostics:")
    print(f"      Reflex bonus events: {agent.getReflexBonusCount()}")
    print(f"      Reflex suppression events: {agent.getReflexSuppressCount()}")
    print(f"      Winner-take-all pulses: {agent.getWtaPulseCount()}")
    print(f"      Planner overrides: {agent.getPlannerOverrideCount()}")
    print(f"      Planner agreements: {agent.getPlannerAgreementCount()}")
    
    print("\n=== Enhanced API Demo Complete ===")
    print("\nThe enhanced pynlm API now provides full access to:")
    print("✓ All Phase 6 cognitive and social learning systems")
    print("✓ Complete neuromodulation control")
    print("✓ Advanced memory systems (working, episodic, associative)")
    print("✓ Rich configuration API with parameter setting")
    print("✓ Comprehensive diagnostic counters and statistics")
    print("✓ Text processing and language understanding")
    print("✓ Social learning and multi-agent studies")
    print("✓ Full checkpoint and development system support")

if __name__ == "__main__":
    main()
