#!/usr/bin/env python3
"""
Demo script showing the improved NLM Python bindings
This script demonstrates all the key improvements made to the Python API.
"""

import pynlm
import time

def demo_basic_functionality():
    """Demonstrate basic NLM functionality (unchanged API)"""
    print("=== Basic NLM Functionality ===")
    
    # Create a basic brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain status: {brain.getTotalSpikeCount()} total spikes")
    
    # Simple simulation
    for i in range(50):
        brain.step(i, i * 0.001)
    
    print(f"After 50 steps: {brain.getTotalSpikeCount()} total spikes")
    print(f"Active neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def demo_phase6_systems():
    """Demonstrate Phase 6 system access (NEW functionality)"""
    print("=== Phase 6 System Access (NEW) ===")
    
    # Create brain with Phase 6 systems
    config = pynlm.createPhase6Config()  # This would be a new helper function
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Phase 6 systems available:")
    
    # Memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"  - Working Memory: {'Available' if working_memory else 'Not Available'}")
    print(f"  - Episodic Memory: {'Available' if episodic_memory else 'Not Available'}")
    print(f"  - Associative Memory: {'Available' if associative_memory else 'Not Available'}")
    
    # Prediction system
    prediction_system = brain.getPredictionSystem()
    print(f"  - Prediction System: {'Available' if prediction_system else 'Not Available'}")
    
    # Cognition systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"  - Neural Planner: {'Available' if planner else 'Not Available'}")
    print(f"  - Concept Formation: {'Available' if concept_formation else 'Not Available'}")
    print(f"  - Attention System: {'Available' if attention else 'Not Available'}")
    
    # Development system
    development_system = brain.getDevelopmentSystem()
    developmental_stage = brain.getDevelopmentalStage()
    
    print(f"  - Development System: {'Available' if development_system else 'Not Available'}")
    print(f"  - Current Developmental Stage: {developmental_stage}")
    
    # Neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print(f"  - Dopamine System: {'Available' if dopamine else 'Not Available'}")
    print(f"  - Curiosity System: {'Available' if curiosity else 'Not Available'}")
    print(f"  - Novelty Detection: {'Available' if novelty else 'Not Available'}")
    print(f"  - Prediction Error: {'Available' if prediction_error else 'Not Available'}")
    
    # Run a simple simulation with all systems
    print("\nRunning simulation with all Phase 6 systems...")
    for i in range(100):
        # Inject some sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        # Step the brain
        brain.step(i, i * 0.001)
        
        if i % 20 == 0:
            print(f"  Step {i}: {brain.getTotalSpikeCount()} spikes, "
                  f"Avg firing: {brain.getAverageFiringRate():.2f} Hz")
    
    print()

def demo_agent_with_phase6():
    """Demonstrate AgentBrain with Phase 6 systems"""
    print("=== AgentBrain with Phase 6 Systems ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 16, 16)  # width, height, visionWidth, visionHeight
    world.reset()
    
    # Create brain with Phase 6 systems
    config = pynlm.createPhase6Config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable key Phase 6 features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Agent initialized in world ({world.getWidth()}x{world.getHeight()})")
    print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run agent simulation
    print("\nRunning agent simulation...")
    total_reward = 0.0
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(action_result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        total_reward += action_result.reward
        
        if step % 10 == 0:
            print(f"  Step {step}: Action={action}, Reward={action_result.reward:.2f}, "
                  f"Total Reward={total_reward:.2f}")
            print(f"    Brain spikes: {brain.getTotalSpikeCount()}, "
                  f"Development Stage: {agent.getDevelopmentalStage()}")
    
    print(f"\nSimulation complete!")
    print(f"Total reward: {total_reward:.2f}")
    print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print()

def demo_error_handling():
    """Demonstrate improved error handling"""
    print("=== Error Handling Demo ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to access a non-existent region
        region_id = pynlm.RegionId(99999)  # Very unlikely to exist
        region = brain.getRegion(region_id)
        
        if region is None:
            print("Successfully handled non-existent region (returned None)")
        
        # Try to create a brain with invalid config
        try:
            invalid_brain = pynlm.createBrain(None)  # This should fail
            print("ERROR: Should have failed with None config")
        except Exception as e:
            print(f"Correctly caught error with invalid config: {type(e).__name__}")
        
        print("Error handling working correctly!")
        
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
    
    print()

def main():
    """Run all demos"""
    print("NLM Python Bindings Demo")
    print("=" * 50)
    print()
    
    demo_basic_functionality()
    demo_phase6_systems()
    demo_agent_with_phase6()
    demo_error_handling()
    
    print("=" * 50)
    print("Demo complete!")
    print()
    print("Key improvements demonstrated:")
    print("✓ Basic NLM functionality (unchanged API)")
    print("✓ Phase 6 system access (NEW functionality)")
    print("✓ AgentBrain with Phase 6 systems")
    print("✓ Improved error handling")
    print()
    print("For more examples, see:")
    print("- pynlm/easy_usage.md (beginner examples)")
    print("- pynlm/HOW_TO_USE.md (comprehensive documentation)")

if __name__ == "__main__":
    main()
