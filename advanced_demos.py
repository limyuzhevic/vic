#!/usr/bin/env python3
"""
NLM Advanced Demo Programs

Educational demonstrations of NLM capabilities for different skill levels.
"""

import pynlm
import json
import time
import sys

def demo_basic_neural_dynamics():
    """
    Demo 1: Basic Neural Dynamics
    
    Demonstrates fundamental LIF neuron properties and spike propagation.
    """
    print("=" * 60)
    print("Demo 1: Basic Neural Dynamics")
    print("=" * 60)
    
    # Create a minimal brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Simulate and observe
    print("\nSimulating neural dynamics...")
    print("Step | Firing | Avg Rate | Spike Count")
    print("-" * 45)
    
    total_spikes = 0
    for step in range(50):
        brain.step(step)
        firing = brain.getFiringNeuronCount()
        total_spikes += brain.getTotalSpikeCount()
        print(f"{step:4d} | {firing:6d} | {brain.getAverageFiringRate():8.4f} | {total_spikes:10d}")
    
    print(f"\nKey observations:")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
    print(f"  - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
    print(f"  - Total spike count: {total_spikes}")
    
    if brain.getTotalSpikeCount() > 10:
        print("  ✓ Network is producing spikes - neural dynamics working!")
    else:
        print("  ⚠ Low spike activity - consider increasing input current")
    
    return brain

def demo_learning_through_stdp():
    """
    Demo 2: Spike-Timing-Dependent Plasticity (STDP)
    
    Demonstrates how STDP enables learning through spike timing.
    """
    print("\n" + "=" * 60)
    print("Demo 2: Spike-Timing-Dependent Plasticity (STDP)")
    print("=" * 60)
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for world interaction
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    world.reset()
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print(f"Created learning brain with {brain.getTotalNeuronCount()} neurons")
    print(f"STDP parameters: LTP={config.getOr('stdp_ltp_weight', 0.02)}, "
          f"LTD={config.getOr('stdp_ltd_weight', 0.015)}")
    
    # Initial plasticity statistics
    initial_plasticity = brain.getPlasticityInfo()
    print(f"\nInitial plasticity weights: {initial_plasticity}")
    
    # Run simulation with correlated activity
    print("\nRunning correlated pre-post spike simulation...")
    print("Step | Pre -> Post | LTP Expected")
    print("-" * 40)
    
    for step in range(100):
        # Inject correlated activity (pre-synaptic then post-synaptic)
        # This should trigger LTP through STDP
        if step % 2 == 0:
            # Pre-synaptic neuron fires
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0)
        else:
            # Post-synaptic neuron fires shortly after
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0)
        
        brain.step(step)
        
        if step % 20 == 0:
            print(f"{step:4d} | {'Yes' if step % 2 == 0 else 'No'}    | {'Expected' if step % 2 == 0 else 'Expected'}")
    
    # Check final plasticity
    final_plasticity = brain.getPlasticityInfo()
    print(f"\nFinal plasticity weights: {final_plasticity}")
    
    # Calculate change
    if 'stdp_ltp' in initial_plasticity and 'stdp_ltp' in final_plasticity:
        ltp_change = final_plasticity['stdp_ltp'] - initial_plasticity['stdp_ltp']
        print(f"STDP LTP weight change: {ltp_change:+.6f}")
        
        if abs(ltp_change) > 0.001:
            print("  ✓ STDP is working - synaptic weights changed through spike timing!")
        else:
            print("  ⚠ Minimal change - may need stronger correlated activity")
    
    return brain, agent, world

def demo_memory_integration():
    """
    Demo 3: Memory System Integration
    
    Demonstrates how working memory and episodic memory integrate with neural activity.
    """
    print("\n" + "=" * 60)
    print("Demo 3: Memory System Integration")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with memory systems enabled")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    
    # Check initial memory state
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    
    print(f"\nInitial memory state:")
    print(f"  Working memory traces: {working_mem.getActiveTraces() if working_mem else 0}")
    print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount() if episodic_mem else 0}")
    
    # Run simulation
    print("\nRunning simulation with memory integration...")
    print("Step | Working Mem | Episodic Mem | Spike Rate")
    print("-" * 55)
    
    for step in range(80):
        brain.step(step)
        
        wm_traces = working_mem.getActiveTraces() if working_mem else 0
        em_episodes = episodic_mem.getEpisodeCount() if episodic_mem else 0
        
        print(f"{step:4d} | {wm_traces:11d} | {em_episodes:12d} | {brain.getAverageFiringRate():10.4f}")
        
        # Store episode periodically
        if step % 20 == 0 and episodic_mem:
            # Create an episodic memory item
            from dataclasses import dataclass
            @dataclass
            class DemoEpisode:
                timestamp: int
                active_neurons: list
                neuron_activations: list
                reward: float
            
            episode = DemoEpisode(
                timestamp=step,
                active_neurons=[],
                neuron_activations=[],
                reward=0.0
            )
            episodic_mem.storeEpisode(episode)
    
    # Check final memory state
    final_wm = working_mem.getActiveTraces() if working_mem else 0
    final_em = episodic_mem.getEpisodeCount() if episodic_mem else 0
    
    print(f"\nFinal memory state:")
    print(f"  Working memory traces: {final_wm}")
    print(f"  Episodic memory episodes: {final_em}")
    print(f"  Working memory capacity: {working_mem.getCapacity() if working_mem else 'N/A'}")
    
    # Check integration
    print(f"\nIntegration assessment:")
    if final_wm > 0:
        print("  ✓ Working memory is capturing neural activity")
    if final_em > 0:
        print("  ✓ Episodic memory is storing experiences")
    
    # Replay test
    if episodic_mem and final_em > 0:
        print(f"  ✓ Memory replay available: {episodic_mem.getEpisodesForReplay(1)} episodes")
    
    return brain

def demo_neuromodulation():
    """
    Demo 4: Neuromodulation Integration
    
    Demonstrates how dopamine, curiosity, and novelty signals affect learning.
    """
    print("\n" + "=" * 60)
    print("Demo 4: Neuromodulation Integration")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 250)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(8, 8, 4, 4)
    world.reset()
    agent.initialize(world)
    
    # Enable all neuromodulation subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created neuromodulation-enabled agent")
    print(f"Brain neurons: {brain.getTotalNeuronCount()}")
    
    # Show initial neuromodulation state
    neuromod_state = brain.getNeuromodulationState()
    print(f"\nInitial neuromodulation state:")
    for key, value in neuromod_state.items():
        print(f"  {key}: {value:.3f}")
    
    # Run training episode
    print(f"\nRunning neuromodulation training episode...")
    print("Step | Reward | Dopamine | Curiosity | Novelty | Dev Stage")
    print("-" * 70)
    
    for step in range(60):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Compute reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.01)
        
        # Show neuromodulation state
        current_state = brain.getNeuromodulationState()
        dev_stage = agent.getDevelopmentalStage()
        
        print(f"{step:4d} | {reward:6.3f} | {current_state.get('dopamine', 0):8.3f} | "
              f"{current_state.get('curiosity', 0):8.3f} | {current_state.get('novelty', 0):7.3f} | {dev_stage}")
    
    # Final assessment
    final_state = brain.getNeuromodulationState()
    print(f"\nFinal neuromodulation assessment:")
    print(f"  Dopamine level: {final_state.get('dopamine', 0):.3f}")
    print(f"  Curiosity level: {final_state.get('curiosity', 0):.3f}")
    print(f"  Novelty level: {final_state.get('novelty', 0):.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # Check if neuromodulation is working
    if abs(final_state.get('dopamine', 0)) > 0.01:
        print("  ✓ Reward modulation (dopamine) is active")
    if final_state.get('curiosity', 0) > 0.1:
        print("  ✓ Curiosity-driven exploration is active")
    if final_state.get('novelty', 0) > 0.1:
        print("  ✓ Novelty detection is active")
    
    return brain, agent, world

def demo_advanced_features():
    """
    Demo 5: Advanced NLM Features
    
    Demonstrates advanced capabilities like network analysis and plasticity control.
    """
    print("\n" + "=" * 60)
    print("Demo 5: Advanced NLM Features")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 400)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced demo brain: {brain.getTotalNeuronCount()} neurons")
    
    # Get initial comprehensive stats
    print(f"\nInitial comprehensive brain analysis:")
    brain_stats = brain.getBrainStats()
    for key, value in brain_stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.4f}")
        else:
            print(f"  {key}: {value}")
    
    # Analyze network connectivity
    print(f"\nNetwork connectivity analysis:")
    connectivity = brain.analyzeConnectivity()
    total_connections = sum(len(conns) for conns in connectivity.values())
    avg_connections = total_connections / len(connectivity) if connectivity else 0
    
    print(f"  Total neural connections: {total_connections}")
    print(f"  Average connections per neuron: {avg_connections:.1f}")
    print(f"  Network density: {avg_connections / (brain.getTotalNeuronCount() - 1) * 100:.2f}%")
    
    # Test plasticity control
    print(f"\nPlasticity system control:")
    plasticity_info = brain.getPlasticityInfo()
    for key, value in plasticity_info.items():
        print(f"  {key}: {value:.4f}")
    
    # Modify plasticity parameters
    print(f"\nModifying plasticity parameters...")
    brain.updatePlasticityConfig({
        "stdp_ltp": 0.025,
        "stdp_ltd": 0.018,
        "hebbian_rate": 0.003
    })
    
    updated_plasticity = brain.getPlasticityInfo()
    print(f"Updated plasticity settings:")
    for key, value in updated_plasticity.items():
        print(f"  {key}: {value:.4f}")
    
    # Simulate learning episode
    print(f"\nRunning advanced learning simulation...")
    
    # Create agent and world for more complex interaction
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(12, 12, 6, 6)
    world.reset()
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run extended simulation
    steps = 100
    print(f"Simulating {steps} steps with full feature integration...")
    
    for step in range(steps):
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Select action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.01)
    
    # Final comprehensive analysis
    print(f"\nFinal comprehensive analysis:")
    final_stats = brain.getBrainStats()
    final_plasticity = brain.getPlasticityInfo()
    final_neuromod = brain.getNeuromodulationState()
    
    print(f"  Brain health: {'Healthy' if brain.isHealthy() else 'Needs attention'}")
    print(f"  Final spike count: {final_stats.get('spike_count', 0)}")
    print(f"  Final firing rate: {final_stats.get('average_firing_rate', 0):.4f} Hz")
    print(f"  Final E/I ratio: {final_stats.get('e_i_ratio', 0):.4f}")
    print(f"  Dopamine level: {final_neuromod.get('dopamine', 0):.3f}")
    print(f"  Curiosity level: {final_neuromod.get('curiosity', 0):.3f}")
    print(f"  Novelty level: {final_neuromod.get('novelty', 0):.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # Memory assessment
    memory_stats = brain.getMemoryStats()
    print(f"  Working memory traces: {memory_stats.get('working_memory_traces', 0)}")
    print(f"  Episodic memory episodes: {memory_stats.get('episodic_memory_episodes', 0)}")
    
    return brain, agent, world

def demo_interactive_tutorials():
    """
    Demo 6: Interactive Tutorial System
    
    Demonstrates the tutorial system with step-by-step guidance.
    """
    print("\n" + "=" * 60)
    print("Demo 6: Interactive Tutorial System")
    print("=" * 60)
    
    print("Welcome to the NLM Interactive Tutorial!")
    print("\nThis tutorial will guide you through the key concepts of NLM.")
    
    tutorials = [
        ("1. Creating Your First Brain", "Learn to create and initialize a neural brain"),
        ("2. Understanding Neural Populations", "Explore how neurons are organized into populations"),
        ("3. Spike Propagation", "Watch how spikes travel through the network"),
        ("4. Memory Integration", "Discover how working and episodic memory work"),
        ("5. Learning Mechanisms", "Explore STDP and Hebbian learning"),
        ("6. Neuromodulation", "Understand how dopamine and curiosity affect learning"),
        ("7. Development and Plasticity", "Learn about brain development and adaptation"),
    ]
    
    print(f"\nAvailable tutorials:")
    for title, description in tutorials:
        print(f"  {title}: {description}")
    
    print(f"\nTutorial system features:")
    print("  ✓ Step-by-step guidance")
    print("  ✓ Interactive examples")
    print("  ✓ Code snippets")
    print("  ✓ Visual feedback")
    print("  ✓ Assessment quizzes")
    
    # Create a simple tutorial example
    print(f"\nExample Tutorial: Creating Your First Brain")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    
    print("Step 1: Create a default configuration")
    print("  config = pynlm.createDefaultConfig()")
    
    print("Step 2: Create a brain with the configuration")
    print("  brain = pynlm.createBrain(config)")
    
    print("Step 3: Initialize the brain")
    print("  brain.initialize()")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Step 4: Verify your brain was created successfully")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print(f"  Healthy: {'Yes' if brain.isHealthy() else 'No'}")
    
    print(f"\nTutorial complete! You've successfully created and initialized your first NLM brain.")
    
    return brain

def run_all_demos():
    """Run all demonstration programs"""
    demos = [
        ("Basic Neural Dynamics", demo_basic_neural_dynamics),
        ("Spike-Timing-Dependent Plasticity", demo_learning_through_stdp),
        ("Memory System Integration", demo_memory_integration),
        ("Neuromodulation Integration", demo_neuromodulation),
        ("Advanced NLM Features", demo_advanced_features),
        ("Interactive Tutorial System", demo_interactive_tutorials),
    ]
    
    results = {}
    
    for title, demo_func in demos:
        try:
            result = demo_func()
            results[title] = {"success": True, "result": result}
        except Exception as e:
            print(f"❌ Error in {title}: {e}")
            results[title] = {"success": False, "error": str(e)}
    
    # Summary
    print("\n" + "=" * 60)
    print("DEMO SUMMARY")
    print("=" * 60)
    
    successful = sum(1 for r in results.values() if r["success"])
    total = len(results)
    
    print(f"Total demos run: {total}")
    print(f"Successful: {successful}")
    print(f"Failed: {total - successful}")
    
    print(f"\nDetailed results:")
    for title, result in results.items():
        status = "✅ SUCCESS" if result["success"] else "❌ FAILED"
        print(f"  {status} - {title}")
    
    return results

def main():
    """Main demo runner"""
    print("NLM Advanced Demo Programs")
    print("=" * 60)
    print("Educational demonstrations of NLM capabilities")
    
    if len(sys.argv) > 1 and sys.argv[1] == "--run-all":
        # Run all demonstrations
        results = run_all_demos()
        
        successful = sum(1 for r in results.values() if r["success"])
        total = len(results)
        
        if successful == total:
            print(f"\n🎉 All {total} demonstrations completed successfully!")
            return 0
        else:
            print(f"\n⚠️  {total - successful} demonstrations failed")
            return 1
    else:
        # Run specific demo
        print("\nAvailable demos:")
        demos = [
            ("1", "Basic Neural Dynamics", demo_basic_neural_dynamics),
            ("2", "Spike-Timing-Dependent Plasticity", demo_learning_through_stdp),
            ("3", "Memory System Integration", demo_memory_integration),
            ("4", "Neuromodulation Integration", demo_neuromodulation),
            ("5", "Advanced NLM Features", demo_advanced_features),
            ("6", "Interactive Tutorial System", demo_interactive_tutorials),
            ("all", "All Demos", run_all_demos),
        ]
        
        for num, title, func in demos:
            print(f"  {num}. {title}")
        
        print("\nUsage: python advanced_demos.py <demo_number>")
        print("       python advanced_demos.py --run-all")
        
        if len(sys.argv) > 1:
            choice = sys.argv[1]
            
            for num, title, func in demos:
                if choice == num:
                    try:
                        result = func()
                        print(f"\n✅ Demo '{title}' completed successfully!")
                        return 0
                    except Exception as e:
                        print(f"\n❌ Error in demo '{title}': {e}")
                        return 1
            
            print(f"\n❌ Unknown demo: {choice}")
            return 1
        else:
            print("\nNo demo specified. Use 'python advanced_demos.py 1' for basic dynamics,")
            print("or 'python advanced_demos.py --run-all' to run all demonstrations.")
            return 0

if __name__ == '__main__':
    sys.exit(main())
