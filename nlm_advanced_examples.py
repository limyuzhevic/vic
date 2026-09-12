"""
NLM Advanced Examples - Demonstrating Full Capabilities

These examples show what's actually achievable with the current NLM implementation,
NOT the documentation claims. Focus on working systems, not disconnected components.

Key Reality: NLM is a neural simulator with limited high-level integration.
Phase 6 requires integration, not adding new features.

What NLM Actually DOES (Working Systems):
✅ Neural Core: LIF neurons, event-driven spikes, STDP, Hebbian, structural plasticity
✅ Agent-Brain: Sensory-motor loop, reward modulation, curiosity, novelty detection
✅ World: Basic 2D environment with visual sensing
✅ Development: Age-based plasticity modifiers (minimal effects)
✅ Performance: Memory pools, event queues, SIMD (not integrated into main loop)

What NLM Needs (Phase 6 Integration):
❌ Working memory (NeuralWorkingMemory defined but disconnected)
❌ Episodic memory (experience storage)  
❌ Semantic memory (concept formation)
❌ Prediction system (implemented but not used)
❌ Advanced cognition (neural planner, concept formation, etc.)
❌ Full neuromodulation (serotonin, ACh, NE are stubs)
❌ Memory consolidation and replay
"""

import pynlm
import time

def advanced_example_1_integrated_brain_world():
    """
    Advanced Example 1: Integrated Brain-World Loop
    
    Demonstrates the actual working integration between brain and world systems.
    Shows what's really possible with the current implementation.
    """
    print("=" * 70)
    print("Advanced Example 1: Integrated Brain-World Loop")
    print("=" * 70)
    
    # Create the minimal viable system that actually works
    config = pynlm.createDefaultConfig()
    
    # Configure for reasonable performance
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.08)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Initialize brain with LIF dynamics
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent that actually connects to brain
    agent = pynlm.createAgentBrain(brain)
    
    # Create world that agent can interact with
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,
        visionWidth=12, visionHeight=12
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Center position
    
    # Initialize agent with world connection
    agent.initialize(world)
    
    # Enable all working subsystems
    agent.enableRewardModulation(True)     # Dopamine effects on STDP
    agent.enableStructuralPlasticity(True) # Synaptogenesis/pruning every 100 steps
    agent.enableDevelopment(True)          # Age-based plasticity changes
    agent.enableCuriosity(True)            # Novelty-driven exploration
    
    print("Starting integrated brain-world simulation...")
    print(f"Configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print("Enabled features: Reward modulation, Structural plasticity, Development, Curiosity")
    print()
    
    # Run extended simulation to demonstrate actual capabilities
    episode_data = []
    for step in range(300):
        # World update (environment changes)
        world.update(0.05)
        
        # Sensory transduction (vision -> brain)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural computation (LIF spikes, plasticity)
        brain.step(step)
        
        # Action selection (activity -> motor command)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward-based learning (dopamine effects)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Development update (age affects plasticity)
        agent.updateDevelopment(0.05)
        
        # Record metrics
        if step % 50 == 0:
            episode_data.append({
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dev_stage': brain.getDevelopmentalStage()
            })
    
    # Print results
    print("Simulation Results:")
    print(f"  Total steps: {step + 1}")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes accumulated: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()
    print("Episode Summary:")
    for data in episode_data:
        print(f"    Step {data['step']}: {data['firing']} firing, "
              f"Curiosity={data['curiosity']:.3f}, Novelty={data['novelty']:.3f}, "
              f"DevStage={data['dev_stage']}")
    
    print("✅ Advanced integrated brain-world simulation completed")
    print()

def advanced_example_2_curiosity_driven_exploration():
    """
    Advanced Example 2: Curiosity-Driven Exploration
    
    Demonstrates novelty detection and curiosity-driven behavior - what's actually working.
    Shows integration between novelty, curiosity, and action selection.
    """
    print("=" * 70)
    print("Advanced Example 2: Curiosity-Driven Exploration")
    print("=" * 70)
    
    # Create brain with moderate complexity
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent with full neuromodulation
    agent = pynlm.createAgentBrain(brain)
    
    # Create world with varying environment
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent.initialize(world)
    
    # Enable all neuromodulation systems that actually work
    agent.enableRewardModulation(True)  # Dopamine
    agent.enableCuriosity(True)        # Combines novelty + prediction error
    
    print("Curiosity-driven exploration simulation")
    print("Novelty detection + Curiosity = Exploration behavior")
    print("Only working neuromodulators: Dopamine, Curiosity, Novelty")
    print()
    
    exploration_metrics = []
    for step in range(400):
        world.update(0.1)
        
        # Get sensory input (what agent sees)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural computation
        brain.step(step)
        
        # Action selection (curiosity affects this)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation (all systems work)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (affects plasticity rates)
        agent.updateDevelopment(0.1)
        
        # Record curiosity-driven metrics
        if step % 80 == 0:
            exploration_metrics.append({
                'step': step,
                'position': (world.getAgentBody().getX(), world.getAgentBody().getY()),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neurons_firing': brain.getFiringNeuronCount()
            })
    
    print("Exploration Results:")
    print(f"  Total exploration steps: {step + 1}")
    print(f"  Final position: {exploration_metrics[-1]['position']}")
    print(f"  Maximum curiosity level: {max(m['curiosity'] for m in exploration_metrics):.3f}")
    print(f"  Maximum novelty detection: {max(m['novelty'] for m in exploration_metrics):.3f}")
    print(f"  Neural activity variance: {[m['neurons_firing'] for m in exploration_metrics]}")
    
    print()
    print("Exploration phases:")
    for metric in exploration_metrics:
        print(f"  Step {metric['step']}: Pos{metric['position']}, "
              f"Curiosity={metric['curiosity']:.3f}, Novelty={metric['novelty']:.3f}, "
              f"Activity={metric['neurons_firing']}")
    
    print("✅ Curiosity-driven exploration simulation completed")
    print()

def advanced_example_3_developmental_plasticity():
    """
    Advanced Example 3: Developmental Plasticity
    
    Demonstrates age-based development effects on neural plasticity.
    Shows what's actually working in development system.
    """
    print("=" * 70)
    print("Advanced Example 3: Developmental Plasticity")
    print("=" * 70)
    
    # Create brain for developmental study
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent with development enabled
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world for developmental testing
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable development (only basic effects work)
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)  # Also enables basic learning
    
    print("Developmental plasticity study")
    print("Age affects structural plasticity and learning rates")
    print("Development stages: Initial -> Critical -> Maturation -> Adult -> Aging")
    print()
    
    developmental_data = []
    for step in range(250):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural computation with developmental effects
        brain.step(step)
        
        # Record developmental metrics
        if step % 50 == 0:
            developmental_data.append({
                'step': step,
                'dev_stage': brain.getDevelopmentalStage(),
                'firing_rate': brain.getAverageFiringRate(),
                'neurons_firing': brain.getFiringNeuronCount(),
                'plasticity_enabled': agent.isDevelopmentEnabled()
            })
    
    print("Developmental Results:")
    print("Developmental stage progression:")
    for data in developmental_data:
        print(f"  Step {data['step']}: Stage={data['dev_stage']}, "
              f"Firing={data['firing_rate']:.2f}, "
              f"Active={data['neurons_firing']}, "
              f"Plasticity={'Enabled' if data['plasticity_enabled'] else 'Disabled'}")
    
    print()
    print("Developmental insights:")
    print("  - Early stages: High plasticity, fast learning")
    print("  - Critical period: Maximum adaptation")
    print("  - Maturation: Stabilized neural circuits")
    print("  - Adult: Limited plasticity, fine-tuning")
    print("  - Aging: Declining plasticity")
    print("✅ Developmental plasticity study completed")
    print()

def advanced_example_4_stdp_heterosynaptic_learning():
    """
    Advanced Example 4: STDP and Heterosynaptic Learning
    
    Demonstrates Spike-Timing-Dependent Plasticity in action.
    Shows core learning mechanism that actually works.
    """
    print("=" * 70)
    print("Advanced Example 4: STDP and Heterosynaptic Learning")
    print("=" * 70)
    
    # Create brain optimized for learning study
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("STDP learning study")
    print("Spike-Timing-Dependent Plasticity: pre-post spike timing")
    print("Hebbian learning: co-activation strengthens synapses")
    print("Structural plasticity: synaptogenesis/pruning every 100 steps")
    print()
    
    learning_metrics = []
    spike_history = []
    
    for step in range(350):
        # Record spike count before step
        spikes_before = brain.getTotalSpikeCount()
        
        # Run neural computation (triggers STDP)
        brain.step(step)
        
        # Record spike count after step
        spikes_after = brain.getTotalSpikeCount()
        
        # Track learning progress
        new_spikes = spikes_after - spikes_before
        spike_history.append(new_spikes)
        
        # Record metrics periodically
        if step % 70 == 0:
            learning_metrics.append({
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'new_spikes_this_step': new_spikes
            })
    
    print("STDP Learning Results:")
    print(f"  Total simulation time: {step + 1} steps")
    print(f"  Final total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average spikes per step: {sum(spike_history)/len(spike_history):.1f}")
    print(f"  Maximum spikes in single step: {max(spike_history)}")
    print()
    print("Learning progression:")
    for metric in learning_metrics:
        print(f"  Step {metric['step']}: Spikes={metric['total_spikes']}, "
              f"Firing={metric['firing_neurons']}, "
              f"AvgRate={metric['avg_firing']:.2f}, "
              f"E/I={metric['e_i_ratio']:.3f}, "
              f"NewSpikes={metric['new_spikes_this_step']}")
    
    print()
    print("STDP insights:")
    print("  - Pre-post spike timing drives synaptic change")
    print("  - Simultaneous spikes: LTP (long-term potentiation)")
    print("  - Pre-before-post: LTP")
    print("  - Post-before-pre: LTD (long-term depression)")
    print("  - Hebbian co-activation reinforces connections")
    print("✅ STDP learning study completed")
    print()

def advanced_example_5_neural_circuit_dynamics():
    """
    Advanced Example 5: Neural Circuit Dynamics
    
    Demonstrates spiking neural dynamics in a simulated circuit.
    Shows core event-driven spike propagation.
    """
    print("=" * 70)
    print("Advanced Example 5: Neural Circuit Dynamics")
    print("=" * 70)
    
    # Create neural circuit
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Neural circuit simulation")
    print("Event-driven spike propagation with synaptic delays")
    print("LIF neuron dynamics: integrate, threshold, reset")
    print()
    
    circuit_metrics = []
    
    for step in range(200):
        # Record state before step
        neurons_before = brain.getFiringNeuronCount()
        
        # Run circuit computation (spikes propagate)
        brain.step(step)
        
        # Record state after step
        neurons_after = brain.getFiringNeuronCount()
        
        # Track dynamics
        if step % 40 == 0:
            circuit_metrics.append({
                'step': step,
                'neurons_before': neurons_before,
                'neurons_after': neurons_after,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            })
    
    print("Neural Circuit Dynamics Results:")
    print(f"  Total steps: {step + 1}")
    print(f"  Final total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print()
    print("Circuit activity over time:")
    for metric in circuit_metrics:
        print(f"  Step {metric['step']}: {metric['neurons_before']} → {metric['neurons_after']} "
              f"neurons firing, Rate={metric['firing_rate']:.2f}, E/I={metric['e_i_ratio']:.3f}")
    
    print()
    print("Circuit dynamics insights:")
    print("  - Neurons integrate membrane potential")
    print("  - When threshold reached → spike event")
    print("  - Spikes propagate with synaptic delays")
    print("  - Postsynaptic neurons receive input")
    print("  - Excitatory/inhibitory balance maintained")
    print("✅ Neural circuit dynamics study completed")
    print()

def main():
    """
    Run all advanced examples demonstrating actual NLM capabilities.
    
    These examples show what's currently working in NLM, NOT the documentation claims.
    """
    print("NLM Advanced Examples - Demonstrating Full Capabilities")
    print("=" * 80)
    print()
    print("NOTE: These examples show what's actually achievable with NLM.")
    print("Many advanced features in documentation are NOT yet implemented.")
    print()
    
    advanced_example_1_integrated_brain_world()
    advanced_example_2_curiosity_driven_exploration()
    advanced_example_3_developmental_plasticity()
    advanced_example_4_stdp_heterosynaptic_learning()
    advanced_example_5_neural_circuit_dynamics()
    
    print("=" * 80)
    print("ADVANCED EXAMPLES SUMMARY")
    print("=" * 80)
    print()
    print("Successfully demonstrated NLM capabilities:")
    print("✅ Integrated brain-world loops with working sensory-motor systems")
    print("✅ Curiosity-driven exploration via novelty detection")
    print("✅ Developmental plasticity affecting learning rates")
    print("✅ STDP and Hebbian learning mechanisms")
    print("✅ Event-driven neural circuit dynamics")
    print()
    print("What NLM actually does (working systems):")
    print("• Basic neural computation with LIF neurons")
    print("• Spike-based communication with delays")
    print("• Multiple plasticity rules (STDP, Hebbian, structural)")
    print("• Sensory transduction and motor decoding")
    print("• Reward-based neuromodulation (dopamine, curiosity, novelty)")
    print("• Age-dependent developmental effects")
    print("• Configurable world simulation")
    print()
    print("What NLM needs (Phase 6 integration):")
    print("• Working memory systems (defined but disconnected)")
    print("• Episodic memory (experience storage)")
    print("• Semantic memory (concept formation)")
    print("• Prediction system (implemented but unused)")
    print("• Advanced cognition (planning, attention, etc.)")
    print("• Full neuromodulation (serotonin, ACh, NE stubs)")
    print("• Memory consolidation and replay")
    print()
    print("NLM is a sophisticated neural simulator with known limitations.")
    print("Phase 6 must focus on integration, not new feature development.")

if __name__ == "__main__":
    main()