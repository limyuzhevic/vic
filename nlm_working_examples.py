"""
NLM Working Examples - Demonstrating What's Actually Functional

This file demonstrates working examples based on the actual NLM capabilities,
NOT the documentation claims. Key reality: NLM is a neural simulator, not a complete AI.

What's Working:
- ✅ Basic neural network (1000 neurons, LIF spiking)
- ✅ STDP and Hebbian plasticity learning
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Sensory-motor loop
- ✅ Basic reward modulation
- ✅ Novelty detection and curiosity
- ✅ Development stages (minimal effects)
- ✅ Basic configurable world

What's NOT Working (Phase 6 integration needed):
- ❌ Working memory (disconnected)
- ❌ Episodic memory (experience storage)
- ❌ Semantic memory (concept formation)
- ❌ Prediction system
- ❌ Advanced cognition (planning, attention)
- ❌ Full neuromodulation (serotonin, ACh, NE are stubs)
- ❌ Memory consolidation and replay
"""

import pynlm

def example_1_silent_brain():
    """Example 1: Silent Brain (Just Neurons)
    
    This demonstrates the basic neural core - neurons firing and learning.
    What's working: LIF neurons, STDP, Hebbian learning, structural plasticity.
    What's NOT working: Memory systems, prediction, advanced cognition.
    """
    print("=" * 60)
    print("Example 1: Silent Brain - Just Neurons")
    print("=" * 60)
    
    # Create brain with default settings
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, {brain.getTotalSpikeCount()} total spikes")
    
    print(f"Final stats - Firing rate: {brain.getAverageFiringRate():.2f}, E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print("✅ Silent brain test completed successfully")
    print()

def example_2_brain_learning():
    """Example 2: Brain with Basic Learning
    
    This demonstrates STDP and Hebbian learning in action.
    What's working: STDP, Hebbian plasticity, structural plasticity.
    """
    print("=" * 60)
    print("Example 2: Brain with Basic Learning")
    print("=" * 60)
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Running simulation to demonstrate learning mechanisms...")
    print("(STDP, Hebbian, structural plasticity every 100 steps)")
    
    # Run longer simulation to see learning
    for step in range(250):
        brain.step(step)
        
        if step == 99 or step == 149 or step == 199 or step == 249:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")
    print("✅ Learning simulation completed successfully")
    print()

def example_3_simple_agent():
    """Example 3: Simple Agent in World
    
    This demonstrates the sensory-motor loop with basic learning.
    What's working: Vision, motor decoding, reward modulation, curiosity.
    What's NOT working: Memory storage, prediction, advanced planning.
    """
    print("=" * 60)
    print("Example 3: Simple Agent in World")
    print("=" * 60)
    
    # Setup complete agent system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)  # Center of world
    
    agent.initialize(world)
    
    # Enable working subsystems
    agent.enableRewardModulation(True)  # Basic reward modulation
    agent.enableCuriosity(True)        # Basic novelty detection
    # Note: Development is minimal, structural plasticity works but limited
    
    print("Starting agent simulation...")
    print("(Sensory-motor loop with basic learning)")
    
    # Run agent episode
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print(f"Episode completed! Final state:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print("✅ Agent simulation completed successfully")
    print()

def example_4_advanced_learning():
    """Example 4: Advanced Learning Simulation
    
    This demonstrates curiosity-driven exploration and reward-based learning.
    What's working: Full agent loop with neuromodulation.
    What's limited: No memory storage, no prediction.
    """
    print("=" * 60)
    print("Example 4: Advanced Learning Simulation")
    print("=" * 60)
    
    def run_learning_simulation(brain, agent, world, duration=500):
        """Run a learning simulation with curiosity and rewards."""
        
        for step in range(duration):
            # Update world environment
            world.update(0.1)
            
            # Get observations and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Neural computation
            brain.step(step)
            
            # Generate and execute action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward-based learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress periodically
            if step % 100 == 0:
                print(f"  Step {step}:")
                print(f"    Neural activity: {brain.getFiringNeuronCount()} firing neurons")
                print(f"    Learning signals: Dopamine={agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
                print(f"    Cognitive state: DevStage={brain.getDevelopmentalStage()}")
    
    # Setup learning environment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable all working subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting advanced learning simulation...")
    print("(Observation -> Learning -> Action -> Reward -> Adaptation)")
    print("Note: No memory storage, continual learning limited")
    
    run_learning_simulation(brain, agent, world, 500)
    
    print(f"Learning completed!")
    print(f"Final neural statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print("✅ Advanced learning simulation completed successfully")
    print()

def example_5_neural_network_basics():
    """Example 5: Neural Network Basics
    
    Simple demonstration of what's actually possible with NLM.
    """
    print("=" * 60)
    print("Example 5: Neural Network Basics")
    print("=" * 60)
    
    # Create simple neural network
    config = pynlm.createDefaultConfig()
    
    # Configure basic parameters
    config.set("brain.neuron_count", 500)  # Half of default
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Created neural network with:")
    print(f"  {brain.getTotalNeuronCount()} neurons")
    print(f"  {brain.getTotalSynapseCount()} synapses")
    print(f"  STDP: Enabled")
    print(f"  Hebbian: Enabled")
    print(f"  Structural plasticity: Enabled (every 100 steps)")
    
    # Demonstrate neural dynamics
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Neural network simulation completed!")
    print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print("✅ Neural network basics demonstration completed")
    print()

def main():
    """Run all working examples."""
    print("NLM Working Examples - Demonstrating What's Actually Functional")
    print("=" * 80)
    print()
    print("IMPORTANT: These examples show what's currently working in NLM.")
    print("Many documented features are not yet integrated (Phase 6 required).")
    print()
    
    example_1_silent_brain()
    example_2_brain_learning()
    example_3_simple_agent()
    example_4_advanced_learning()
    example_5_neural_network_basics()
    
    print("=" * 80)
    print("SUMMARY: All examples demonstrate working NLM functionality")
    print("=" * 80)
    print()
    print("What's actually working in NLM:")
    print("✅ LIF neuron dynamics and spike-based computation")
    print("✅ STDP and Hebbian plasticity learning rules")
    print("✅ Structural plasticity (synaptogenesis/pruning)")
    print("✅ Sensory input and motor output systems")
    print("✅ Basic reward modulation and curiosity")
    print("✅ Novelty detection")
    print("✅ Development stages (minimal effects)")
    print("✅ Configurable world simulation")
    print()
    print("What's NOT yet functional (Phase 6 integration needed):")
    print("❌ Working memory (NeuralWorkingMemory exists but disconnected)")
    print("❌ Episodic memory (experience storage)")
    print("❌ Semantic memory (concept formation)")
    print("❌ Prediction system")
    print("❌ Advanced cognition (planning, attention)")
    print("❌ Full neuromodulation (serotonin, ACh, NE are stubs)")
    print("❌ Memory consolidation and replay")
    print()
    print("NLM is a neural simulator, not a complete AI system.")
    print("Phase 6 must focus on integration, not new features.")

if __name__ == "__main__":
    main()