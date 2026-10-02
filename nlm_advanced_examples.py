#!/usr/bin/env python3
"""
NLM Advanced Examples

Advanced examples demonstrating NLM's capabilities for power users and researchers.

This file contains examples for:
1. Complex agent behaviors with curiosity-driven learning
2. Memory consolidation and replay
3. Developmental neuroscience simulations
4. Plasticity experiments with STDP and Hebbian learning
5. Multi-region brain architectures
6. Advanced configuration management
7. Performance optimization techniques
8. Scientific experiments and data analysis

To run examples:
    python nlm_advanced_examples.py [example_name]

Available examples:
    agent-learning          Agent learning with curiosity and memory
    development            Brain development and maturation
    plasticity-experiment  Synaptic plasticity experiments
    multi-region          Multi-region brain architecture
    memory-consolidation   Memory consolidation and replay
    world-interaction      Agent-world interaction and reward
    performance            Performance benchmarking
"""

import argparse
import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
def example_agent_learning():
    """Advanced agent learning with curiosity-driven exploration and memory integration."""
    print("=== Example 1: Agent Learning with Curiosity and Memory ===")
    
    # Create a sophisticated agent with multiple subsystems
    config = nlm.createDefaultConfig()
    
    # Advanced configuration for learning agent
    config.set("brain.neuron_count", 2000)  # Larger brain
    config.set("brain.region_count", 3)     # Multiple regions
    config.set("random_seed", 12345)
    config.set("simulation_timestep", 0.001)
    
    # Enable all learning features
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(200.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Created agent with {brain.getTotalNeuronCount()} neurons and {brain.getRegionCount()} regions")
    print(f"Agent features: reward={agent.isRewardModulationEnabled()}, "
          f"plasticity={agent.isStructuralPlasticityEnabled()}, "
          f"development={agent.isDevelopmentEnabled()}, "
          f"curiosity={agent.isCuriosityEnabled()}")
    
    # Run extended training episode
    episodes = 20
    steps_per_episode = 1000
    
    for episode in range(episodes):
        print(f"\n--- Episode {episode + 1}/{episodes} ---")
        
        # Reset for new episode
        world.reset()
        brain.reset()
        agent.reset()
        world.setAgentStart(15.0, 15.0)
        
        episode_steps = 0
        total_reward = 0.0
        exploration_bonus = 0.0
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Agent processes input
            agent.processSensoryInput(percept)
            
            # Brain processes step
            brain.step(step, step * 0.1)
            
            # Agent decides action
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and get curiosity bonus
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            curiosity_bonus = agent.getCuriosityLevel()
            
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            exploration_bonus += curiosity_bonus
            
            episode_steps += 1
            
            # Print progress
            if step % 200 == 0:
                firing = brain.getFiringNeuronCount()
                spikes = brain.getTotalSpikeCount()
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                pred_error = agent.getPredictionError()
                
                print(f"  Step {step:4d}: firing={firing:4d}, spikes={spikes:6d}, "
                      f"curiosity={curiosity:.2f}, novelty={novelty:.2f}, "
                      f"pred_err={pred_error:.2f}, reward={reward:+.2f}")
        
        # Update development periodically
        if episode % 5 == 0:
            agent.updateDevelopment(0.1 * episode)
        
        # Show episode summary
        avg_curiosity = exploration_bonus / steps_per_episode
        print(f"Episode {episode + 1} Summary:")
        print(f"  Total reward: {total_reward:+.2f}")
        print(f"  Average curiosity: {avg_curiosity:.2f}")
        print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
        
        # Save checkpoint periodically
        if episode % 10 == 0:
            checkpoint_file = f"agent_checkpoint_ep{episode:02d}.bin"
            brain.save(checkpoint_file)
            print(f"  Saved checkpoint: {checkpoint_file}")
    
    print("\n=== Agent Learning Example Complete ===")
    
    # Save final trained brain
    brain.save("trained_agent_brain.bin")
    print("Saved final trained brain to: trained_agent_brain.bin")
    
    return brain, agent, world
def example_development():
    """Brain development and maturation simulation."""
    print("\n=== Example 2: Brain Development and Maturation ===")
    
    # Create brain with developmental configuration
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("random_seed", 42)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("Initial brain state:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
    # Simulate development over time
    development_phases = [
        ("Initial", 0, 1000),
        ("Critical Period", 1000, 5000),
        ("Maturation", 5000, 10000),
        ("Adult", 10000, 20000)
    ]
    
    current_phase = 0
    phase_start_step = 0
    
    total_steps = 20000
    
    for step in range(total_steps):
        # Update brain
        brain.step(step, step * 0.001)
        
        # Check for phase transitions
        if current_phase < len(development_phases) - 1:
            phase_name, start_step, end_step = development_phases[current_phase]
            if step >= end_step:
                current_phase += 1
                print(f"\n[{step}] Phase transition: {phase_name} -> {development_phases[current_phase][0]}")
        
        # Update development system
        if step % 100 == 0:
            brain.develop()
            developmental_stage = brain.getDevelopmentalStage()
            
            print(f"Step {step:5d}: {developmental_stage.name:15s} | "
                  f"Neurons: {brain.getTotalNeuronCount():4d} | "
                  f"Synapses: {brain.getTotalSynapseCount():4d} | "
                  f"Firing: {brain.getFiringNeuronCount():4d} | "
                  f"Total spikes: {brain.getTotalSpikeCount():6d}")
    
    # Show final development summary
    print(f"\n=== Development Summary ===")
    print(f"Total simulation steps: {total_steps}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Save developed brain
    brain.save("developed_brain.bin")
    print("Saved developed brain to: developed_brain.bin")
    
    return brain
def example_plasticity_experiment():
    """Advanced synaptic plasticity experiments with STDP and Hebbian learning."""
    print("\n=== Example 3: Synaptic Plasticity Experiment ===")
    
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("random_seed", 12345)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Get plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    print("Plasticity systems initialized:")
    print(f"  STDP: LTP weight={stdp.getLTPWeight()}, LTD weight={stdp.getLTDWeight()}")
    print(f"  Hebbian: learning rate={hebbian.getLearningRate()}, max weight={hebbian.getMaxWeight()}")
    
    # Configure for specific plasticity experiment
    stdp.configure(0.02f, 0.018f, 20.0f)  # Potentiation experiment
    hebbian.configure(0.005f, 0.003f, 100.0f, 10.0f)
    
    # Run correlation experiment
    print("\nRunning correlation experiment...")
    
    # Create correlated activity pattern
    steps = 5000
    spike_pairs = 0
    
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        # Create correlated pre-post activity
        if step % 2 == 0:
            # Fire pre-synaptic neuron
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 50.0f)
        else:
            # Fire post-synaptic neuron  
            brain.injectCurrentToNeurons(nlm.NeuronType.Motor, 50.0f)
        
        # Check for spike pairs
        if step > 10 and step % 10 == 0:
            # Count spike pairs for STDP analysis
            spike_pairs += 1
    
    # Show plasticity results
    print(f"\n=== Plasticity Experiment Results ===")
    print(f"Total steps: {steps}")
    print(f"Estimated spike pairs: {spike_pairs}")
    print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Analyze synapse weights
    total_weight = 0.0
    for region in brain.getRegions():
        for syn in region->getSynapses():
            total_weight += abs(syn->getWeight())
    
    print(f"Total absolute synaptic weight: {total_weight:.3f}")
    
    # Save plasticity experiment results
    brain.save("plasticity_experiment.bin")
    print("Saved experiment results to: plasticity_experiment.bin")
    
    return brain
def example_multi_region():
    """Multi-region brain architecture with inter-region connections."""
    print("\n=== Example 4: Multi-Region Brain Architecture ===")
    
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 4)
    config.set("random_seed", 99999)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created multi-region brain with {brain.getRegionCount()} regions")
    
    # Add inter-region connections
    regions = brain.getRegionIds()
    for i, source_region in enumerate(regions):
        for j, target_region in enumerate(regions):
            if i != j:  # Don't connect region to itself
                # Create random inter-region connections
                if i % 2 == 0 and j % 2 == 1:  # Some pattern
                    delay = 5 + (i + j) % 20  # Random delay between 5-24 steps
                    brain.addInterRegionConnection(source_region, target_region, 0.1f, delay)
    
    print(f"Added {brain.getTotalSynapseCount() - sum(r->getSynapseCount() for r in brain.getRegions())} inter-region connections")
    
    # Run multi-region simulation
    steps = 3000
    print(f"\nRunning multi-region simulation for {steps} steps...")
    
    region_activity = {}
    for region_id in regions:
        region_activity[region_id] = []
    
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        # Record regional activity
        for region_id in regions:
            region = brain.getRegion(region_id)
            if region:
                activity = region->getTotalNeuronCount() * 0.01f  # Simplified activity measure
                region_activity[region_id].append(activity)
        
        if step % 500 == 0:
            print(f"Step {step}: Regional activity summary")
            for region_id in regions:
                avg_activity = sum(region_activity[region_id][-10:]) / min(10, len(region_activity[region_id]))
                print(f"  Region {region_id.index}: {avg_activity:.2f}")
    
    # Analyze regional specialization
    print(f"\n=== Multi-Region Analysis ===")
    for region_id in regions:
        activity = region_activity[region_id]
        if activity:
            avg_activity = sum(activity) / len(activity)
            peak_activity = max(activity)
            print(f"Region {region_id.index}: avg={avg_activity:.2f}, peak={peak_activity:.2f}")
    
    # Save multi-region brain
    brain.save("multi_region_brain.bin")
    print("Saved multi-region brain to: multi_region_brain.bin")
    
    return brain
def example_memory_consolidation():
    """Memory consolidation and replay simulation."""
    print("\n=== Example 5: Memory Consolidation and Replay ===")
    
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("random_seed", 777)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Memory systems initialized:")
    print(f"  Working memory capacity: {working_memory->getCapacity()}")
    print(f"  Episodic memory max episodes: {episodic_memory->getMaxEpisodes()}")
    
    # Run learning and consolidation experiment
    learning_steps = 2000
    replay_interval = 100
    consolidation_interval = 1000
    
    print(f"\nRunning learning experiment ({learning_steps} steps)...")
    
    for step in range(learning_steps):
        brain.step(step, step * 0.001)
        
        # Store episodes periodically (simulating experience)
        if step % 20 == 0:
            # Create a simple episode from current brain state
            nlm.EpisodicMemoryItem episode
            episode.timestamp = step
            episode.reward = 0.0  # Simplified
            
            # Add some neurons to episode
            for region in brain.getRegions():
                for pop in region->getPopulations():
                    for neuron in pop->getNeurons():
                        if neuron->isFiring() or 
                           abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f:
                            episode.activeNeurons.push_back(neuron->getId())
                            episode.neuronActivations.push_back(
                                abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f)
            
            episodic_memory->storeEpisode(episode)
        
        # Replay episodes periodically
        if step % replay_interval == 0 and episodic_memory->getEpisodeCount() > 0:
            print(f"Step {step}: Replaying {episodic_memory->getEpisodeCount()} episodes...")
            
            # Get episodes for replay
            episodes_to_replay = episodic_memory->getEpisodesForReplay(3)
            
            for episode in episodes_to_replay:
                episodic_memory->replayEpisode(episode)
        
        # Consolidate memory periodically
        if step % consolidation_interval == 0 and episodic_memory->getEpisodeCount() > 0:
            print(f"Step {step}: Consolidating memory...")
            episodic_memory->consolidate(0.3f)
        
        if step % 500 == 0:
            print(f"  Step {step}: Episodes stored={episodic_memory->getEpisodeCount()}, "
                  f"working_memory traces={working_memory->getActiveTraces()}")
    
    # Show final memory status
    print(f"\n=== Memory Consolidation Summary ===")
    print(f"Total episodes stored: {episodic_memory->getEpisodeCount()}")
    print(f"Working memory active traces: {working_memory->getActiveTraces()}")
    print(f"Final brain spikes: {brain.getTotalSpikeCount()}")
    
    # Save memory system state
    brain.save("memory_consolidation.bin")
    print("Saved memory system state to: memory_consolidation.bin")
    
    return brain
def example_world_interaction():
    """Advanced agent-world interaction with complex reward and curiosity."""
    print("\n=== Example 6: Agent-World Interaction ===")
    
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("random_seed", 333)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.setAgentStart(10.0, 10.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Created agent-world system")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Complex interaction simulation
    steps = 3000
    
    interaction_stats = {
        'rewards': [],
        'curiosity_levels': [],
        'action_counts': {},
        'state_changes': []
    }
    
    print(f"\nRunning interaction simulation ({steps} steps)...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get current state
        body = world.getAgentBody()
        interaction_stats['state_changes'].append((body.x, body.y, body.orientation))
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Track action statistics
        action_name = str(action.getType())
        interaction_stats['action_counts'][action_name] = interaction_stats['action_counts'].get(action_name, 0) + 1
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with noise
        base_reward = 0.1  # Small base reward
        noise = (hash(step) % 20 - 10) / 100.0  # Some noise
        reward = base_reward + noise
        
        curiosity_bonus = agent.getCuriosityLevel()
        
        agent.applyRewardModulation(reward, 0.0)
        
        interaction_stats['rewards'].append(reward)
        interaction_stats['curiosity_levels'].append(curiosity_bonus)
        
        if step % 500 == 0:
            avg_reward = sum(interaction_stats['rewards']) / len(interaction_stats['rewards'])
            avg_curiosity = sum(interaction_stats['curiosity_levels']) / len(interaction_stats['curiosity_levels'])
            
            print(f"Step {step:4d}: reward={avg_reward:+.3f}, "
                  f"curiosity={avg_curiosity:.2f}, "
                  f"actions={len(interaction_stats['action_counts'])}")
    
    # Analyze interaction patterns
    print(f"\n=== Interaction Analysis ===")
    
    if interaction_stats['rewards']:
        avg_reward = sum(interaction_stats['rewards']) / len(interaction_stats['rewards'])
        max_reward = max(interaction_stats['rewards'])
        min_reward = min(interaction_stats['rewards'])
        print(f"Rewards: avg={avg_reward:+.3f}, max={max_reward:.3f}, min={min_reward:.3f}")
    
    if interaction_stats['curiosity_levels']:
        avg_curiosity = sum(interaction_stats['curiosity_levels']) / len(interaction_stats['curiosity_levels'])
        print(f"Curiosity levels: avg={avg_curiosity:.2f}")
    
    print(f"Action distribution:")
    for action, count in sorted(interaction_stats['action_counts'].items(), key=lambda x: x[1], reverse=True):
        percentage = (count / steps) * 100
        print(f"  {action}: {count} ({percentage:.1f}%)")
    
    print(f"\n=== Interaction Complete ===")
    
    # Save interaction data
    brain.save("world_interaction.bin")
    print("Saved interaction results to: world_interaction.bin")
    
    return brain, agent, world
def example_performance():
    """Performance benchmarking and optimization demonstration."""
    print("\n=== Example 7: Performance Benchmarking ===")
    
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("random_seed", 123)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("Running performance benchmarks...")
    
    # Benchmark different operations
    benchmarks = {
        'step_timing': [],
        'memory_usage': [],
        'plasticity_timing': [],
        'stats_timing': []
    }
    
    steps = 1000
    
    # Benchmark step operation
    print("Benchmarking step operations...")
    start_time = time.time()
    for step in range(steps):
        brain.step(step, step * 0.001)
    step_time = time.time() - start_time
    benchmarks['step_timing'].append(step_time)
    
    print(f"  Step time: {step_time/steps*1000:.3f}ms per step, "
          f"{steps/step_time:.0f} steps/sec")
    
    # Benchmark memory system
    print("Benchmarking memory system operations...")
    working_memory = brain.getWorkingMemory()
    
    start_time = time.time()
    for step in range(min(steps, 500)):
        # Store some traces
        for region in brain.getRegions():
            for neuron in region->getAllNeurons():
                if neuron->isFiring():
                    working_memory->storeToNeuron(neuron->getId(), 0.5f)
        working_memory->update(0.001)
    memory_time = time.time() - start_time
    benchmarks['memory_timing'].append(memory_time)
    
    print(f"  Memory operations time: {memory_time/min(steps,500)*1000:.3f}ms per iteration")
    
    # Benchmark statistics
    print("Benchmarking statistics operations...")
    
    start_time = time.time()
    for step in range(min(steps, 200)):
        brain.getTotalNeuronCount()
        brain.getTotalSynapseCount()
        brain.getTotalSpikeCount()
        brain.getAverageFiringRate()
        brain.getExcitationInhibitionRatio()
    stats_time = time.time() - start_time
    benchmarks['stats_timing'].append(stats_time)
    
    print(f"  Stats operations time: {stats_time/min(steps,200)*1000:.3f}ms per iteration")
    
    # Show summary
    print(f"\n=== Performance Benchmark Summary ===")
    
    print(f"Steps benchmarked: {steps}")
    print(f"Total step time: {benchmarks['step_timing'][0]:.3f}s")
    print(f"Average step rate: {steps/benchmarks['step_timing'][0]:.0f} steps/sec")
    
    if benchmarks['memory_timing']:
        print(f"Memory operations time: {benchmarks['memory_timing'][0]:.3f}s for {min(steps,500)} iterations")
    
    if benchmarks['stats_timing']:
        print(f"Stats operations time: {benchmarks['stats_timing'][0]:.3f}s for {min(steps,200)} iterations")
    
    # Estimate memory usage (simplified)
    print(f"\nEstimated memory usage:")
    print(f"  Brain object: ~{brain.getTotalNeuronCount() * 100} bytes for neuron data")
    print(f"  Synapses: ~{brain.getTotalSynapseCount() * 50} bytes for synapse data")
    print(f"  Total estimated: ~{(brain.getTotalNeuronCount() * 100 + brain.getTotalSynapseCount() * 50) / 1024:.1f} KB")
    
    print(f"\n=== Performance Benchmark Complete ===")
    
    # Save performance test results
    brain.save("performance_test.bin")
    print("Saved performance test results to: performance_test.bin")
    
    return brain
def main():
    parser = argparse.ArgumentParser(
        description="NLM Advanced Examples",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument("example", nargs="?", 
                       choices=["agent-learning", "development", "plasticity-experiment",
                               "multi-region", "memory-consolidation", "world-interaction",
                               "performance", "all"],
                       help="Example to run (default: run all examples)")
    
    parser.add_argument("--steps", type=int, default=1000,
                       help="Default number of steps (default: 1000)")
    
    parser.add_argument("--output-dir", default=".",
                       help="Output directory for saved results (default: current directory)")
    
    args = parser.parse_args()
    
    if not args.example or args.example == "all":
        print("Running ALL advanced examples...")
        print("This will take several minutes.")
        print("Press Ctrl+C to skip examples you don't want to run.")
        print()
        
        examples_to_run = [
            ("agent-learning", example_agent_learning),
            ("development", example_development),
            ("plasticity-experiment", example_plasticity_experiment),
            ("multi-region", example_multi_region),
            ("memory-consolidation", example_memory_consolidation),
            ("world-interaction", example_world_interaction),
            ("performance", example_performance)
        ]
        
        for example_name, example_func in examples_to_run:
            try:
                print(f"\n{'='*60}")
                print(f"Running: {example_name}")
                print(f"{'='*60}")
                
                result = example_func()
                
                print(f"\n✓ {example_name} completed successfully")
                
            except Exception as e:
                print(f"\n✗ {example_name} failed: {e}")
                import traceback
                traceback.print_exc()
                
    else:
        # Run specific example
        examples = {
            "agent-learning": example_agent_learning,
            "development": example_development,
            "plasticity-experiment": example_plasticity_experiment,
            "multi-region": example_multi_region,
            "memory-consolidation": example_memory_consolidation,
            "world-interaction": example_world_interaction,
            "performance": example_performance
        }
        
        if args.example in examples:
            print(f"Running: {args.example}")
            print("=" * 60)
            
            try:
                examples[args.example]()
                print(f"\n✓ {args.example} completed successfully")
            except Exception as e:
                print(f"\n✗ {args.example} failed: {e}")
                import traceback
                traceback.print_exc()
        else:
            print(f"Unknown example: {args.example}")
            sys.exit(1)
    
    print("\n" + "="*60)
    print("NLM Advanced Examples Complete!")
    print("="*60)
    print("\nExamples have generated saved brain files in:")
    print("  - trained_agent_brain.bin")
    print("  - developed_brain.bin")
    print("  - plasticity_experiment.bin")
    print("  - multi_region_brain.bin")
    print("  - memory_consolidation.bin")
    print("  - world_interaction.bin")
    print("  - performance_test.bin")
if __name__ == "__main__":
    main()
