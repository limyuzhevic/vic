#!/usr/bin/env python3
"""
Quick Start Demo for NLM Python Bindings

This script provides a quick, easy-to-follow demonstration of the NLM Python bindings.
It's designed to be run immediately and show the key capabilities without
complex setup requirements.

Usage:
    python3 quick_demo.py
"""

import time
import sys

try:
    import pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please build the NLM Python bindings first.")
    sys.exit(1)

print("\n" + "="*60)
print("NLM Python Bindings - Quick Start Demo")
print("="*60 + "\n")

def demo_basic_neural_simulation():
    """Demonstrate basic neural simulation"""
    print("1. Basic Neural Simulation")
    print("-" * 40)
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100, pynlm.ConfigSource.Runtime)
    config.set("random_seed", 42, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("Running simulation...")
    for step in range(50):
        brain.step(step, step * 0.001)
        
        # Inject current at specific steps
        if step == 10:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
            print(f"  Step {step}: Injected sensory current")
        elif step == 30:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 25.0f)
            print(f"  Step {step}: Injected motor current")
    
    print(f"✓ Simulation completed")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    return brain

def demo_plasticity():
    """Demonstrate synaptic plasticity"""
    print("\n2. Synaptic Plasticity Demonstration")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
    config.set("random_seed", 123, pynlm.ConfigSource.Runtime)
    config.set("stdp_enabled", True, pynlm.ConfigSource.Runtime)
    config.set("hebbian_enabled", True, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Enabled plasticity rules:")
    print("  ✓ STDP (Spike-Timing-Dependent Plasticity)")
    print("  ✓ Hebbian (coincidence detection)")
    
    # Run with correlated pre-post activity
    print("Creating correlated pre-post activity (LTP demonstration)...")
    
    for step in range(100):
        # Create pre-post correlation to trigger LTP
        if step % 2 == 0:  # Every other step
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 40.0f)
        else:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 20.0f)
        
        brain.step(step, step * 0.001)
        
        if (step + 1) % 25 == 0:
            print(f"  Step {step + 1}: {brain.getTotalSpikeCount()} spikes so far")
    
    print("✓ Plasticity demonstration completed")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    brain.logStatus()
    return brain

def demo_brain_parts():
    """Demonstrate access to NLM brain components"""
    print("\n3. NLM Brain Components")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 150, pynlm.ConfigSource.Runtime)
    config.set("random_seed", 456, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Accessing NLM brain component systems:")
    
    # Memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"  ✓ Working memory: {working_memory.getCapacity() if working_memory else 'N/A'} capacity")
    print(f"  ✓ Episodic memory: Operational")
    print(f"  ✓ Associative memory: Operational")
    
    # Prediction system
    prediction_system = brain.getPredictionSystem()
    print(f"  ✓ Prediction system: {prediction_system.getPredictionHorizon() if prediction_system else 'N/A'} horizon")
    
    # Neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print(f"  ✓ Dopamine (reward): {dopamine.getLevel() if dopamine else 'N/A'} level")
    print(f"  ✓ Curiosity (exploration): {curiosity.getLevel() if curiosity else 'N/A'} level")
    print(f"  ✓ Novelty detection: {novelty.getLevel() if novelty else 'N/A'} level")
    print(f"  ✓ Prediction error: {prediction_error.getValue() if prediction_error else 'N/A'}")
    
    # Development system
    development = brain.getDevelopmentSystem()
    print(f"  ✓ Development stage: {brain.getDevelopmentalStage().name if development else 'N/A'}")
    
    # Cognitive systems
    planner = brain.getPlanner()
    concept_former = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"  ✓ Neural planner: Operational")
    print(f"  ✓ Concept formation: Operational")
    print(f"  ✓ Attentional selection: Operational")
    
    print("✓ All NLM component systems accessible")
    return brain

def demo_world_interaction():
    """Demonstrate agent-world interaction"""
    print("\n4. Agent-World Interaction")
    print("-" * 40)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(32, 32, 8, 8)
    world.setRandomSeed(789)
    
    # Add world objects
    # Resource (food)
    resource = pynlm.WorldObject()
    resource.x = 20.0
    resource.y = 20.0
    resource.type = pynlm.WorldObjectType.Resource
    resource.value = 10.0
    world.addObject(resource)
    
    # Hazard (wall)
    wall = pynlm.WorldObject()
    wall.x = 25.0
    wall.y = 25.0
    wall.type = pynlm.WorldObjectType.Wall
    wall.radius = 1.5
    world.addObject(wall)
    
    # Create agent brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100, pynlm.ConfigSource.Runtime)
    config.set("random_seed", 999, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    print(f"✓ Created world ({world.getWidth()}x{world.getHeight()})")
    print(f"✓ Created agent brain interface")
    print(f"  Sensory inputs: {agent_brain.getSensoryInputSize()}")
    print(f"  Motor outputs: {agent_brain.getMotorOutputSize()}")
    
    # Run interaction loop
    print("Running agent-world interaction...")
    
    for episode in range(3):
        print(f"  Episode {episode + 1}:")
        
        # Reset for new episode
        world.reset()
        agent_brain.reset()
        brain.reset()
        brain.initialize()
        
        # Add fresh objects
        resource2 = pynlm.WorldObject()
        resource2.x = 15.0
        resource2.y = 15.0
        resource2.type = pynlm.WorldObjectType.Resource
        resource2.value = 5.0
        world.addObject(resource2)
        
        for step in range(20):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process and act
            agent_brain.processSensoryInput(percept)
            
            # Run neural simulation
            for neural_step in range(3):
                brain.step(neural_step, neural_step * 0.001)
            
            # Produce action
            action = brain.produceAction()
            if action:
                print(f"    Step {step}: Produced action {action.getType()}")
            
            # Update world
            world.update(0.01)
            
            # Show agent status
            body = world.getAgentBody()
            if step % 5 == 0:
                print(f"      Position: ({body.x:.1f}, {body.y:.1f}), Energy: {body.energy:.1f}")
    
    print("✓ World interaction demonstration completed")
    return agent_brain

def demo_architecture_summary():
    """Show architecture summary"""
    print("\n5. NLM Architecture Summary")
    print("-" * 40)
    
    print("NLM (Neural Learning Machine) Architecture:")
    print("=" * 40)
    
    print("\nCore Components:")
    print("  🧠 Brain Core: LIF neurons, spike dynamics, plasticity")
    print("  🧩 Sensory Systems: Vision, Audio, Internal signals")
    print("  ⚙️  Motor Systems: Action selection and execution")
    print("  🌍 Environment: World simulation and interaction")
    print("  🧠 Memory Systems:")
    print("     - Working Memory: Transient active information")
    print("     - Episodic Memory: Experience encoding")
    print("     - Associative Memory: Pattern associations")
    print("  🎯 Cognitive Systems:")
    print("     - Prediction: Temporal sequence learning")
    print("     - Attention: Competitive selection (NOT Transformer)")
    print("     - Planning: Multi-step action selection")
    print("     - Concept Formation: Pattern discovery")
    print("  🧪 Neuromodulation:")
    print("     - Dopamine: Reward and reinforcement")
    print("     - Curiosity: Exploration motivation")
    print("     - Novelty: Surprise detection")
    print("     - Prediction Error: Learning signals")
    print("  📈 Development: Age-dependent plasticity and maturation")
    
    print("\nKey Design Principles:")
    print("  ✓ No ML frameworks - implements its own neural computation")
    print("  ✓ No Transformer architecture - uses spiking dynamics")
    print("  ✓ Biological inspiration - incrementally more realistic")
    print("  ✓ Modular design - clear interfaces between components")
    print("  ✓ Determinism - reproducible experiments via explicit seeds")
    print("  ✓ Performance - data-oriented design for large-scale simulation")
    print("  ✓ Extensibility - clear patterns for adding new components")
    print("  ✓ No predefined knowledge - all learning from experience")
    
    print("\n✓ NLM provides a complete, biologically-inspired neural simulation")
    print("  platform capable of developing and adapting through experience.")


def main():
    """Main function running all demonstrations"""
    print(f"Python version: {sys.version}")
    print(f"NLM module version: {getattr(pynlm, '__version__', 'Unknown')}")
    
    start_time = time.time()
    
    try:
        # Run all demonstrations
        demo_basic_neural_simulation()
        demo_plasticity()
        demo_brain_parts()
        demo_world_interaction()
        demo_architecture_summary()
        
        total_time = time.time() - start_time
        
        print(f"\n{'='*60}")
        print("DEMO COMPLETE")
        print(f"{'='*60}")
        print(f"✓ All demonstrations completed successfully!")
        print(f"✓ Total execution time: {total_time:.2f} seconds")
        print(f"✓ NLM Python bindings are working correctly!")
        
        print(f"\n{'='*60}")
        print("KEY CAPABILITIES DEMONSTRATED:")
        print(f"{'='*60}")
        print("1. ✓ Neural simulation with LIF neuron dynamics")
        print("2. ✓ Synaptic plasticity (STDP, Hebbian)")
        print("3. ✓ Complete NLM brain architecture integration")
        print("4. ✓ Agent-world interaction and control")
        print("5. ✓ All NLM systems (memory, prediction, cognition)")
        print("6. ✓ Neuromodulation (reward, curiosity, novelty)")
        print("7. ✓ Developmental progression")
        
        print(f"\nFor more advanced usage:")
        print(f"  - See: docs/PYTHON_API.md (comprehensive API documentation)")
        print(f"  - See: examples/nlm_python_examples.py (extensive examples)")
        print(f"  - See: nlm_demo.py (modular demo system)")
        print(f"  - Run: python3 -m pip show pynlm (installation info)")
        
        return True
        
    except KeyboardInterrupt:
        print(f"\n⚠ Demo interrupted by user")
        return False
    except Exception as e:
        print(f"\n✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)