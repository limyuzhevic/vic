import pynlm
import numpy as np
import random
import time
from typing import Dict, List, Tuple
from collections import defaultdict

def memory_replay_experiment_example():
    """
    Memory replay and consolidation experiment demonstrating sleep-like replay phases,
    episodic memory consolidation, and interference management.
    """
    print("=== Memory Replay and Consolidation Experiment ===\n")
    
    # Configure for memory experiments
    config = pynlm.createDefaultConfig()
    
    # Memory system configuration
    config.set("memory.enable_replay", True)
    config.set("memory.enable_consolidation", True)
    config.set("memory.enable_interference_management", True)
    config.set("memory.replay_frequency", 0.15)  # Replay every ~6-7 steps
    config.set("memory.sleep_phases", True)
    config.set("memory.replay_intensity", 0.8)
    config.set("memory.consolidation_threshold", 0.7)
    
    # Brain configuration for memory experiments
    config.set("brain.region_count", 4)
    config.set("brain.neurons_per_region", 200)
    config.set("memory.working_memory_size", 100)
    config.set("memory.episodic_capacity", 2000)
    config.set("memory.associative_pattern_size", 60)
    
    # Neuromodulation for memory
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.dopamine_gain", 1.5)
    config.set("neuromodulation.enable_prediction_error", True)
    
    # Initialize brain with advanced memory systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create experimental world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15, 15)
    world.setMaxEnergy(150.0)
    world.setEnergyDecayRate(0.01)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Initialize memory systems
    _initialize_memory_systems(brain)
    
    # Run memory experiment with different phases
    experiment_results = run_memory_experiment_phases(brain, world, agent)
    
    # Analyze results
    analyze_memory_experiment_results(experiment_results)
    
    return experiment_results

def _initialize_memory_systems(brain: pynlm.Brain):
    """Initialize and configure memory systems."""
    print("Initializing memory systems...")
    
    # Configure working memory
    if brain.getWorkingMemory():
        brain.getWorkingMemory().setCapacity(100)
        brain.getWorkingMemory().setDecayRate(0.05)
    
    # Configure episodic memory
    if brain.getEpisodicMemory():
        brain.getEpisodicMemory().setCapacity(2000)
        brain.getEpisodicMemory().setReplayFrequency(0.15)
        brain.getEpisodicMemory().setConsolidationThreshold(0.7)
    
    # Configure associative memory
    if brain.getAssociativeMemory():
        brain.getAssociativeMemory().setPatternSize(60)
        brain.getAssociativeMemory().setLearningRate(0.02)
        brain.getAssociativeMemory().setInterferenceThreshold(0.8)
    
    print("Memory systems initialized successfully.")

def run_memory_experiment_phases(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                                 agent: pynlm.AgentBrain) -> Dict:
    """Run memory experiment with different experimental phases."""
    
    # Experimental phases
    phases = {
        "encoding": {"duration": 600, "replay_interval": 50, "phase_goals": "experience_collection"},
        "replay": {"duration": 200, "replay_intensity": 0.8, "phase_goals": "memory_reactivation"},
        "consolidation": {"duration": 400, "consolidation_rate": 0.3, "phase_goals": "memory_strengthening"},
        "testing": {"duration": 300, "access_probability": 0.25, "phase_goals": "retrieval_assessment"}
    }
    
    # Memory tracking
    memory_stats = {
        "encoding_sessions": [],
        "replay_sessions": [],
        "consolidation_sessions": [],
        "testing_sessions": [],
        "memory_strength": [],
        "retrieval_success": [],
        "interference_effects": [],
        "replay_events": [],
        "consolidation_events": []
    }
    
    current_phase = "encoding"
    phase_step = 0
    encoding_start_time = 0
    
    # Main experimental loop
    total_steps = sum(phase["duration"] for phase in phases.values())
    
    for step in range(total_steps):
        # Phase management
        if step >= phases[current_phase]["duration"] + encoding_start_time:
            # Move to next phase
            current_phase = get_next_memory_phase(current_phase)
            phase_step = 0
            encoding_start_time = step - phases["encoding"]["duration"]
            
            print(f"Phase transition: {current_phase.upper()}")
            
            if current_phase == "replay":
                if brain.getEpisodicMemory():
                    brain.getEpisodicMemory().initiateReplay()
                memory_stats["replay_sessions"].append(step)
                
            elif current_phase == "consolidation":
                if brain.getEpisodicMemory():
                    brain.getEpisodicMemory().startConsolidation()
                memory_stats["consolidation_sessions"].append(step)
        
        # Phase-specific behavior
        if current_phase == "encoding":
            # Normal learning and experience collection
            world.update(0.1)
            
            # Collect experience for potential replay
            if step % phases[current_phase]["replay_interval"] == 0:
                if brain.getEpisodicMemory():
                    percept = world.getSensoryPercept()
                    brain.getEpisodicMemory().recordExperience(percept, step)
                    memory_stats["encoding_sessions"].append({
                        "step": step,
                        "percept_hash": hash(str(percept.getAllSignals())),
                        "memory_id": f"enc_{step}"
                    })
            
            # Standard agent processing
            _process_agent_step(agent, world, step, memory_stats)
                
        elif current_phase == "replay":
            # Memory replay phase
            world.update(0.1)
            
            # Spontaneous replay events
            if random.random() < phases[current_phase]["replay_intensity"]:
                if brain.getEpisodicMemory():
                    replay_event = brain.getEpisodicMemory().triggerReplay()
                    memory_stats["replay_events"].append({
                        "step": step,
                        "replay_type": replay_event.get("type", "unknown"),
                        "strength": replay_event.get("strength", 0.0)
                    })
            
            # Background world interaction
            _process_agent_step(agent, world, step, memory_stats)
                
        elif current_phase == "consolidation":
            # Consolidation phase with systematic replay
            world.update(0.1)
            
            # Systematic memory consolidation
            if brain.getEpisodicMemory() and brain.getEpisodicMemory().shouldConsolidate():
                consolidation_event = brain.getEpisodicMemory().consolidateRecentMemories(20)
                memory_stats["consolidation_events"].append({
                    "step": step,
                    "consolidated_count": consolidation_event.get("count", 0),
                    "strength_gain": consolidation_event.get("strength_gain", 0.0)
                })
            
            # Background processing
            _process_agent_step(agent, world, step, memory_stats)
                
        elif current_phase == "testing":
            # Testing phase - memory retrieval without new encoding
            world.update(0.1)
            
            # Track memory access patterns
            if random.random() < phases[current_phase]["access_probability"]:
                if brain.getEpisodicMemory():
                    test_percept = world.getSensoryPercept()
                    retrieval_success = brain.getEpisodicMemory().attemptRetrieval(test_percept)
                    memory_stats["testing_sessions"].append({
                        "step": step,
                        "success": retrieval_success,
                        "percept_hash": hash(str(test_percept.getAllSignals()))
                    })
                    memory_stats["retrieval_success"].append(retrieval_success)
            
            # Background processing
            _process_agent_step(agent, world, step, memory_stats)
        
        # Collect memory statistics periodically
        if step % 50 == 0:
            # Memory strength tracking
            if brain.getEpisodicMemory():
                strength = brain.getEpisodicMemory().getAverageMemoryStrength()
                memory_stats["memory_strength"].append({
                    "step": step,
                    "strength": strength,
                    "phase": current_phase
                })
            
            # Working memory usage
            if brain.getWorkingMemory():
                active_count = brain.getWorkingMemory().getActiveMemoryCount()
                memory_stats["working_memory_usage"].append({
                    "step": step,
                    "active_count": active_count,
                    "phase": current_phase
                })
            
            # Interference tracking
            if brain.getAssociativeMemory():
                interference = brain.getAssociativeMemory().getInterferenceLevel()
                memory_stats["interference_effects"].append({
                    "step": step,
                    "interference_level": interference,
                    "phase": current_phase
                })
    
    return memory_stats

def _process_agent_step(agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                       step: int, stats: Dict):
    """Process a single agent step and update statistics."""
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through agent brain
    agent.processSensoryInput(percept)
    
    # Get brain reference (assuming we have access to the brain)
    # This is a simplification - in real usage, you'd need to pass the brain
    brain = agent.getBrain()
    
    # Brain processes
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate reward
    reward = _calculate_memory_reward(world, agent, step)
    
    # Apply reward modulation
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # Update development periodically
    if step % 100 == 0:
        agent.updateDevelopment(step * 0.1)

def _calculate_memory_reward(world: pynlm.SimpleWorld, agent: pynlm.AgentBrain, step: int) -> float:
    """Calculate reward based on memory task goals."""
    reward = 0.0
    agent_body = world.getAgentBody()
    
    # Reward for exploration and novel experiences
    novelty_bonus = min(agent.getNoveltyLevel() * 2.0, 3.0)
    reward += novelty_bonus
    
    # Reward for efficient memory usage
    if agent.getPredictionError() < 0.1:
        reward += 2.0  # Good prediction
    
    # Reward for curiosity-driven exploration
    if agent.getCuriosityLevel() > 0.5:
        reward += 1.0
    
    # Resource collection reward
    vision = world.getSensoryPercept().getVision()
    for y in range(len(vision)):
        for x in range(len(vision[0])):
            if vision[y][x] == 5:  # Resource
                reward += 3.0
            elif vision[y][x] == 6:  # Hazard
                reward -= 1.0
            elif vision[y][x] == 7:  # Marker (goal)
                reward += 5.0
    
    # Energy cost
    reward -= agent_body.energy * 0.01
    
    # Health penalty
    if agent_body.health < 50:
        reward -= 1.0
    
    # Time-based efficiency bonus
    if step < 200:
        reward += 3.0
    
    return reward

def get_next_memory_phase(current_phase: str) -> str:
    """Get the next experimental memory phase."""
    phases = ["encoding", "replay", "consolidation", "testing"]
    current_index = phases.index(current_phase)
    return phases[(current_index + 1) % len(phases)]

def analyze_memory_experiment_results(results: Dict):
    """Analyze and report memory experiment results."""
    print("\n=== Memory Experiment Analysis ===\n")
    
    # Basic statistics
    total_steps = sum(len(phase_data) for phase_data in results.values() if isinstance(phase_data, list))
    
    print("Phase Summary:")
    for phase, data in results.items():
        if isinstance(data, list):
            print(f"  {phase.capitalize()}: {len(data)} events")
        elif isinstance(data, dict) and "step" in data[0] if data else False:
            print(f"  {phase.capitalize()}: {len(data)} sessions")
    
    print(f"\nTotal experimental steps: {total_steps}")
    
    # Memory strength analysis
    if results["memory_strength"]:
        strengths = [event["strength"] for event in results["memory_strength"]]
        print(f"\nMemory Strength:")
        print(f"  Mean strength: {np.mean(strengths):.3f}")
        print(f"  Strength range: {min(strengths):.3f} - {max(strengths):.3f}")
        print(f"  Strength trend: {'Increasing' if np.mean(strengths[-10:]) > np.mean(strengths[:10]) else 'Decreasing' if np.mean(strengths[-10:]) < np.mean(strengths[:10]) else 'Stable'}")
    
    # Retrieval success analysis
    if results["retrieval_success"]:
        success_rate = np.mean(results["retrieval_success"]) * 100
        print(f"\nRetrieval Performance:")
        print(f"  Success rate: {success_rate:.1f}%")
        print(f"  Total retrievals: {len(results['retrieval_success'])}")
    
    # Interference analysis
    if results["interference_effects"]:
        interference_levels = [event["interference_level"] for event in results["interference_effects"]]
        print(f"\nInterference Management:")
        print(f"  Mean interference: {np.mean(interference_levels):.3f}")
        print(f"  Interference stability: {1.0 / (1.0 + np.std(interference_levels)):.3f}")
    
    # Replay events analysis
    if results["replay_events"]:
        replay_types = defaultdict(int)
        for event in results["replay_events"]:
            replay_types[event["replay_type"]] += 1
        
        print(f"\nReplay Events:")
        print(f"  Total replay events: {len(results['replay_events'])}")
        print(f"  Replay types: {dict(replay_types)}")
    
    # Consolidation events analysis
    if results["consolidation_events"]:
        total_consolidated = sum(event["consolidated_count"] for event in results["consolidation_events"])
        total_strength_gain = sum(event["strength_gain"] for event in results["consolidation_events"])
        
        print(f"\nConsolidation Events:")
        print(f"  Total memories consolidated: {total_consolidated}")
        print(f"  Total strength gain: {total_strength_gain:.3f}")
        print(f"  Consolidation efficiency: {total_strength_gain / max(total_consolidated, 1):.3f}")
    
    # Phase performance comparison
    print(f"\nPhase Performance:")
    for phase in ["encoding", "replay", "consolidation", "testing"]:
        if phase in results:
            phase_data = results[phase]
            if isinstance(phase_data, list):
                print(f"  {phase.capitalize()}: {len(phase_data)} sessions recorded")
    
    # Key findings
    print(f"\n=== Key Findings ===")
    
    # Memory consolidation effectiveness
    if results["consolidation_events"] and results["memory_strength"]:
        initial_strength = results["memory_strength"][0]["strength"] if results["memory_strength"] else 0
        final_strength = results["memory_strength"][-1]["strength"] if results["memory_strength"] else 0
        consolidation_effectiveness = (final_strength - initial_strength) / max(initial_strength, 1)
        print(f"Memory consolidation effectiveness: {consolidation_effectiveness:.2%}")
    
    # Replay effectiveness
    if results["replay_events"] and results["retrieval_success"]:
        replay_effectiveness = np.mean(results["retrieval_success"]) * 100
        print(f"Replay-assisted retrieval: {replay_effectiveness:.1f}% success rate")
    
    # Interference management
    if results["interference_effects"]:
        interference_stability = 1.0 / (1.0 + np.std(interference_levels))
        if interference_stability > 0.5:
            print(f"Effective interference management: stability = {interference_stability:.3f}")
        else:
            print(f"Interference management needs improvement: stability = {interference_stability:.3f}")
    
    # Overall experiment success
    success_criteria_met = []
    
    if results["memory_strength"] and np.mean([s["strength"] for s in results["memory_strength"]]) > 0.5:
        success_criteria_met.append("Memory strength above baseline")
    
    if results["retrieval_success"] and np.mean(results["retrieval_success"]) > 0.6:
        success_criteria_met.append("Retrieval success above threshold")
    
    if results["consolidation_events"] and len(results["consolidation_events"]) > 0:
        success_criteria_met.append("Consolidation events detected")
    
    if results["replay_events"] and len(results["replay_events"]) > 0:
        success_criteria_met.append("Replay events detected")
    
    print(f"\nExperiment Success Criteria Met: {len(success_criteria_met)}/{min(4, len(success_criteria_met))}")
    for criterion in success_criteria_met:
        print(f"  ✓ {criterion}")

if __name__ == "__main__":
    print("Starting Memory Replay and Consolidation Experiment...\n")
    
    start_time = time.time()
    results = memory_replay_experiment_example()
    end_time = time.time()
    
    print(f"\n=== Example Complete ===")
    print(f"Total execution time: {end_time - start_time:.2f} seconds")
    print(f"Memory experiment completed successfully!")
    print(f"Example demonstrates advanced memory replay, consolidation, and interference management.")
