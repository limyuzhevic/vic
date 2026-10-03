#!/usr/bin/env python3
"""
Demonstration script showing the enhanced NLM Python bindings.

This script demonstrates the new APIs added to the Python bindings,
including direct neuron/synapse access, plasticity systems,
memory operations, and advanced usage patterns.
"""

import pynlm

def demonstrate_core_enhancements():
    """Demonstrate the core enhancements to the Python bindings."""
    
    print("=== NLM Python Bindings Enhancement Demo ===\n")
    
    # Create and initialize brain
    print("1. Creating and initializing brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    
    # Demonstrate direct neuron access (NEW API)
    print("\n2. Direct neuron manipulation (NEW API)...")
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        for neuron_id in region.getNeuronIds():
            neuron = brain.getNeuron(neuron_id)
            print(f"   Neuron {neuron_id}: potential={neuron.getMembranePotential():.1f}mV, type={neuron.getType()}")
            
            # Example: Direct current injection
            if neuron.getMembranePotential() < -60.0:
                neuron.injectCurrent(10.0)
            break  # Just show one neuron per region
        break  # Just show first region
    
    # Demonstrate synapse access (NEW API)
    print("\n3. Direct synapse access (NEW API)...")
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        for synapse_id in region.getSynapseIds():
            synapse = brain.getSynapse(synapse_id)
            print(f"   Synapse {synapse_id}: weight={synapse.getWeight():.3f}, "
                  f"source={synapse.getSourceNeuron()}, dest={synapse.getDestinationNeuron()}")
            
            # Record some spike activity for demonstration
            synapse.recordPreSpike(0)
            synapse.recordPostSpike(10)
            break  # Just show one synapse per region
        break  # Just show first region
    
    # Demonstrate plasticity systems (NEW API)
    print("\n4. Plasticity system configuration (NEW API)...")
    
    # STDP
    stdp = brain.getSTDP()
    print(f"   STDP - LTP weight: {stdp.getLTPWeight():.3f}, LTD weight: {stdp.getLTDWeight():.3f}")
    stdp.configure(ltpWeight=0.02, ltdWeight=0.025, tau=25.0)
    
    # Hebbian
    hebbian = brain.getHebbian()
    print(f"   Hebbian - Learning rate: {hebbian.getLearningRate():.3f}, "
          f"Max weight: {hebbian.getMaxWeight():.3f}")
    hebbian.setLearningRate(0.01)
    
    # Demonstrate memory systems (NEW API)
    print("\n5. Memory system operations (NEW API)...")
    
    # Working memory
    working_mem = brain.getWorkingMemory()
    working_mem.store(pynlm.NeuronId(0), 0.7)
    working_mem.store(pynlm.NeuronId(1), 0.9)
    print(f"   Working memory: {working_mem.getCurrentSize()}/{working_mem.getCapacity()} items")
    
    # Episodic memory
    episodic_mem = brain.getEpisodicMemory()
    item = pynlm.EpisodicMemoryItem()
    item.timestamp = 100
    item.neurons = [pynlm.NeuronId(5), pynlm.NeuronId(10)]
    item.values = [0.8, 0.6]
    episodic_mem.storeEpisode(item)
    print(f"   Episodic memory: {episodic_mem.getEpisodeCount()} episodes")
    
    # Semantic memory
    semantic_mem = brain.getSemanticMemory()
    semantic_mem.storeFact("temperature", "hot")
    semantic_mem.storeFact("water", "available")
    print(f"   Semantic memory: {len(semantic_mem.getAllFacts())} facts stored")
    
    # Demonstrate neuromodulation (NEW API)
    print("\n6. Neuromodulation system control (NEW API)...")
    
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    print(f"   Dopamine: level={dopamine.getLevel():.3f}, "
          f"plasticity_factor={dopamine.getPlasticityFactor():.3f}")
    print(f"   Curiosity: level={curiosity.getLevel():.3f}")
    
    # Demonstrate development control (NEW API)
    print("\n7. Development system control (NEW API)...")
    
    print(f"   Current developmental stage: {brain.getDevelopmentalStage()}")
    
    # Demonstrate world operations
    print("\n8. Enhanced world operations...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Add objects to world
    resource = pynlm.WorldObject(x=5, y=5, type=pynlm.WorldObjectType.Resource, value=1.0)
    hazard = pynlm.WorldObject(x=15, y=15, type=pynlm.WorldObjectType.Hazard, value=0.5)
    world.addObject(resource)
    world.addObject(hazard)
    
    print(f"   World: {world.getWidth()}x{world.getHeight()}, "
          f"simulation_time={world.getSimulationTime():.1f}")
    
    # Demonstrate agent integration
    print("\n9. Agent with enhanced capabilities...")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"   Agent: curiosity={agent.getCuriosityLevel():.3f}, "
          f"novelty={agent.getNoveltyLevel():.3f}")
    
    # Run demonstration simulation
    print("\n10. Running demonstration simulation...")
    
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Sense and act
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print statistics every 5 steps
        if step % 5 == 0:
            print(f"   Step {step}: "
                  f"neurons_firing={brain.getFiringNeuronCount()}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"novelty={agent.getNoveltyLevel():.3f}")
    
    # Print final statistics
    print("\n=== Final Statistics ===")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Agent novelty: {agent.getNoveltyLevel():.3f}")
    
    print("\n=== Demonstration Complete ===")
    print("\nKey enhancements demonstrated:")
    print("  ✓ Direct neuron and synapse access")
    print("  ✓ Plasticity system configuration (STDP, Hebbian)")
    print("  ✓ Memory system operations (working, episodic, semantic)")
    print("  ✓ Neuromodulation control (dopamine, curiosity, novelty)")
    print("  ✓ Development system access")
    print("  ✓ Enhanced world operations")
    print("  ✓ Advanced agent capabilities")
    print("\nThe NLM Python bindings now provide comprehensive access to all")
    print("neural simulation systems for advanced research and development.")

if __name__ == "__main__":
    demonstrate_core_enhancements()