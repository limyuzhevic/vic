# NLM Helper Functions and Utilities
# Python helper functions and utilities for advanced NLM usage

import pynlm
from typing import List, Dict, Optional, Callable
from enum import Enum

class NLMActionTypes(Enum):
    """Predefined action type collections for common use cases."""
    
    EXPLORATION = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Look,
        pynlm.ActionType.LookUp,
        pynlm.ActionType.LookDown
    ]
    
    SURVIVAL = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat,
        pynlm.ActionType.Drink,
        pynlm.ActionType.Rest
    ]
    
    CREATIVE = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveLeft,
        pynlm.ActionType.MoveRight,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Custom
    ]

class NLMTaskTypes(Enum):
    """Predefined task types for agent behavior."""
    
    NAVIGATION = "navigation"
    EXPLORATION = "exploration"
    RESOURCE_COLLECTION = "resource_collection"
    PREDATOR_AVOIDANCE = "predator_avoidance"
    SOCIAL_INTERACTION = "social_interaction"
    LEARNING = "learning"
    DEVELOPMENT = "development"

def create_agent_with_behavior(
    config=None,
    world_width=20,
    world_height=20,
    vision_range=5,
    task_type=NLMTaskTypes.NAVIGATION,
    enable_subsystems=None
):
    """
    Create a pre-configured agent with specific behavior characteristics.
    
    Args:
        config: Custom configuration object (optional)
        world_width: Width of the simulation world
        world_height: Height of the simulation world
        vision_range: Visual field range for the agent
        task_type: Type of task the agent should perform
        enable_subsystems: Dictionary of subsystems to enable
    
    Returns:
        tuple: (brain, world, agent) configured for the specified task
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Configure for specific task
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 2)
    config.set("world.width", world_width)
    config.set("world.height", world_height)
    
    # Task-specific configuration
    if task_type == NLMTaskTypes.NAVIGATION:
        config.set("brain.connection_probability", 0.08)
        enable_subsystems = {
            "reward_modulation": True,
            "structural_plasticity": False,
            "development": True,
            "curiosity": False
        }
    
    elif task_type == NLMTaskTypes.EXPLORATION:
        config.set("brain.connection_probability", 0.12)
        enable_subsystems = {
            "reward_modulation": True,
            "structural_plasticity": True,
            "development": True,
            "curiosity": True
        }
    
    elif task_type == NLMTaskTypes.RESOURCE_COLLECTION:
        config.set("brain.connection_probability", 0.06)
        enable_subsystems = {
            "reward_modulation": True,
            "structural_plasticity": False,
            "development": True,
            "curiosity": False
        }
    
    else:
        enable_subsystems = {
            "reward_modulation": True,
            "structural_plasticity": True,
            "development": True,
            "curiosity": True
        }
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_width, height=world_height, 
                   visionWidth=vision_range * 2 + 1, 
                   visionHeight=vision_range * 2 + 1)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems based on configuration
    if enable_subsystems:
        agent.enableRewardModulation(enable_subsystems.get("reward_modulation", False))
        agent.enableStructuralPlasticity(enable_subsystems.get("structural_plasticity", False))
        agent.enableDevelopment(enable_subsystems.get("development", False))
        agent.enableCuriosity(enable_subsystems.get("curiosity", False))
    
    return brain, world, agent

def simulate_episode(
    brain,
    world,
    agent,
    max_steps=1000,
    randomize_start=True,
    verbose=False
):
    """
    Run a complete episode simulation with the given agent.
    
    Args:
        brain: The neural brain
        world: The simulation world
        agent: The agent interface
        max_steps: Maximum number of simulation steps
        randomize_start: Whether to randomize starting position
        verbose: Whether to print step-by-step output
    
    Returns:
        dict: Episode statistics and recorded data
    """
    episode_data = {
        "steps": [],
        "action_distribution": {},
        "curiosity_levels": [],
        "novelty_levels": [],
        "reward_history": [],
        "total_spikes": 0,
        "final_position": (0, 0),
        "success_criteria": False
    }
    
    # Reset and initialize
    brain.reset()
    brain.initialize()
    world.reset()
    agent.reset()
    agent.initialize(world)
    
    # Set random start position
    if randomize_start:
        import random
        start_x = random.uniform(5, world.getWidth() - 5)
        start_y = random.uniform(5, world.getHeight() - 5)
        world.setAgentStart(start_x, start_y)
    
    # Run episode
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record episode data
        step_data = {
            "step": step,
            "simulation_time": world.getSimulationTime(),
            "action": action.getName() if hasattr(action, 'getName') else str(action),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation": agent.getNeuromodulationLevel(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "development_stage": brain.getDevelopmentalStage().value if brain.getDevelopmentalStage() else 0
        }
        
        episode_data["steps"].append(step_data)
        episode_data["curiosity_levels"].append(agent.getCuriosityLevel())
        episode_data["novelty_levels"].append(agent.getNoveltyLevel())
        episode_data["total_spikes"] += brain.getTotalSpikeCount()
        
        # Track action distribution
        action_name = step_data["action"]
        if action_name not in episode_data["action_distribution"]:
            episode_data["action_distribution"][action_name] = 0
        episode_data["action_distribution"][action_name] += 1
        
        # Get position for final recording
        agent_body = world.getAgentBody()
        episode_data["final_position"] = (agent_body.x, agent_body.y)
        
        if verbose and step % 100 == 0:
            print(f"  Step {step}: "
                  f"Pos=({agent_body.x:.1f},{agent_body.y:.1f}), "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Dev={brain.getDevelopmentalStage().value if brain.getDevelopmentalStage() else 0}")
    
    # Calculate success criteria
    episode_data["success_criteria"] = (
        episode_data["total_spikes"] > 0 and
        len(episode_data["curiosity_levels"]) > 10 and
        max(episode_data["curiosity_levels"]) > 0.5
    )
    
    return episode_data

def run_multiple_episodes(
    brain_factory,
    world_factory,
    agent_factory,
    num_episodes=5,
    episode_length=500,
    episode_variations=True
):
    """
    Run multiple episodes with different conditions.
    
    Args:
        brain_factory: Function that returns a brain instance
        world_factory: Function that returns a world instance
        agent_factory: Function that returns an agent instance
        num_episodes: Number of episodes to run
        episode_length: Length of each episode
        episode_variations: Whether to vary episode conditions
    
    Returns:
        list: List of episode results
    """
    all_episodes = []
    
    for episode in range(num_episodes):
        print(f"\n--- Running Episode {episode + 1}/{num_episodes} ---")
        
        # Create fresh components for each episode
        brain = brain_factory()
        world = world_factory()
        agent = agent_factory(brain, world)
        
        # Run episode
        episode_result = simulate_episode(
            brain, world, agent, 
            max_steps=episode_length,
            randomize_start=episode_variations,
            verbose=(episode == 0)  # Only verbose for first episode
        )
        
        all_episodes.append({
            "episode_number": episode + 1,
            "data": episode_result,
            "conditions": {
                "start_position_randomized": episode_variations,
                "episode_length": episode_length,
                "neuron_count": brain.getTotalNeuronCount(),
                "region_count": brain.getRegionCount()
            }
        })
        
        print(f"Episode {episode + 1} completed: "
              f"Spikes={episode_result['total_spikes']}, "
              f"Success={episode_result['success_criteria']}")
    
    return all_episodes

def analyze_episode_results(episodes):
    """
    Analyze and compare results from multiple episodes.
    
    Args:
        episodes: List of episode results from run_multiple_episodes
    
    Returns:
        dict: Analysis summary and statistics
    """
    analysis = {
        "episode_count": len(episodes),
        "overall_stats": {},
        "consolidation": {},
        "performance_trends": {}
    }
    
    # Collect all data
    all_spikes = []
    all_curiosity = []
    all_novelty = []
    success_count = 0
    
    for episode in episodes:
        data = episode["data"]
        all_spikes.append(data["total_spikes"])
        all_curiosity.extend(data["curiosity_levels"])
        all_novelty.extend(data["novelty_levels"])
        
        if data["success_criteria"]:
            success_count += 1
    
    # Calculate statistics
    analysis["overall_stats"] = {
        "total_episodes": len(episodes),
        "successful_episodes": success_count,
        "success_rate": success_count / len(episodes) if episodes else 0,
        "avg_spikes_per_episode": sum(all_spikes) / len(all_spikes) if all_spikes else 0,
        "max_spikes": max(all_spikes) if all_spikes else 0,
        "min_spikes": min(all_spikes) if all_spikes else 0,
        "avg_curiosity": sum(all_curiosity) / len(all_curiosity) if all_curiosity else 0,
        "avg_novelty": sum(all_novelty) / len(all_novelty) if all_novelty else 0
    }
    
    # Analyze trends
    if len(all_spikes) > 1:
        spike_variance = sum((x - analysis["overall_stats"]["avg_spikes_per_episode"]) ** 2 for x in all_spikes) / len(all_spikes)
        analysis["performance_trends"]["stability_score"] = max(0, 100 - spike_variance / 10)
    
    return analysis

def save_agent_session(
    brain,
    world,
    agent,
    session_name="nlm_session",
    custom_metadata=None
):
    """
    Save the current agent session state for later recovery.
    
    Args:
        brain: The neural brain
        world: The simulation world
        agent: The agent interface
        session_name: Name for the saved session
        custom_metadata: Additional metadata to save
    
    Returns:
        dict: Session information including saved state paths
    """
    import os
    from datetime import datetime
    
    session_dir = f"/tmp/nlm_sessions/{session_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
    os.makedirs(session_dir, exist_ok=True)
    
    # Save brain state
    brain_filename = f"{session_dir}/brain_state.bin"
    if brain.save(brain_filename):
        print(f"Brain state saved to {brain_filename}")
    else:
        print(f"Warning: Failed to save brain state")
    
    # Save configuration
    config_filename = f"{session_dir}/config.json"
    # Note: Config doesn't have direct save to JSON in current API
    # Would need to extract config from brain
    print(f"Configuration would be saved to {config_filename}")
    
    # Save session metadata
    metadata = {
        "session_name": session_name,
        "timestamp": datetime.now().isoformat(),
        "brain_neuron_count": brain.getTotalNeuronCount(),
        "brain_synapse_count": brain.getTotalSynapseCount(),
        "development_stage": brain.getDevelopmentalStage().value if brain.getDevelopmentalStage() else 0,
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "simulation_time": world.getSimulationTime()
    }
    
    if custom_metadata:
        metadata.update(custom_metadata)
    
    metadata_filename = f"{session_dir}/metadata.json"
    import json
    with open(metadata_filename, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    print(f"Session metadata saved to {metadata_filename}")
    
    return {
        "session_directory": session_dir,
        "brain_state_file": brain_filename,
        "metadata_file": metadata_filename,
        "metadata": metadata
    }

def load_agent_session(session_path):
    """
    Load a previously saved agent session.
    
    Args:
        session_path: Path to the session directory
    
    Returns:
        tuple: (brain, world, agent) loaded from session
    """
    import json
    import os
    
    # Load metadata
    metadata_file = f"{session_path}/metadata.json"
    with open(metadata_file, 'r') as f:
        metadata = json.load(f)
    
    print(f"Loading session: {metadata['session_name']}")
    print(f"Saved at: {metadata['timestamp']}")
    print(f"Development stage: {metadata['development_stage']}")
    
    # Note: This is a simplified example
    # In a real implementation, you would:
    # 1. Load the brain state from the .bin file
    # 2. Recreate the brain with the same configuration
    # 3. Reinitialize the world and agent
    
    print("Note: Full session loading requires brain state serialization support")
    print("Current implementation provides metadata and structure for loading")
    
    return None, None, None

class NLMProfiler:
    """Profiler for NLM simulations with timing and performance analysis."""
    
    def __init__(self, name="NLM Simulation"):
        self.name = name
        self.timings = {}
        self.memory_usage = []
        self.operation_counts = {}
    
    def time_operation(self, operation_name, func, *args, **kwargs):
        """Time an operation and record the result."""
        import time
        
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        
        duration = end_time - start_time
        self.timings[operation_name] = self.timings.get(operation_name, 0) + duration
        
        if "count" not in self.operation_counts:
            self.operation_counts["count"] = 0
        self.operation_counts["count"] += 1
        
        return result, duration
    
    def record_memory_usage(self, description=""):
        """Record current memory usage (simplified implementation)."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.memory_usage.append({
            "description": description,
            "memory_mb": memory_mb,
            "timestamp": len(self.memory_usage)
        })
    
    def get_summary(self):
        """Get profiling summary."""
        summary = {
            "simulation_name": self.name,
            "total_time": sum(self.timings.values()),
            "operation_count": self.operation_counts.get("count", 0),
            "average_operation_time": (
                sum(self.timings.values()) / len(self.timings) 
                if self.timings else 0
            ),
            "memory_samples": len(self.memory_usage)
        }
        
        if self.memory_usage:
            avg_memory = sum(m["memory_mb"] for m in self.memory_usage) / len(self.memory_usage)
            max_memory = max(m["memory_mb"] for m in self.memory_usage)
            summary["average_memory_mb"] = avg_memory
            summary["peak_memory_mb"] = max_memory
        
        return summary

def create_profiling_wrapper(
    brain_factory,
    world_factory,
    agent_factory,
    profile_operations=True
):
    """
    Create a simulation wrapper with profiling capabilities.
    
    Args:
        brain_factory: Function that returns a brain instance
        world_factory: Function that returns a world instance
        agent_factory: Function that returns an agent instance
        profile_operations: Whether to enable detailed profiling
    
    Returns:
        dict: Simulation with profiling capabilities
    """
    profiler = NLMProfiler("NLM Simulation with Profiling")
    
    def run_profiled_simulation(duration=1000):
        """Run simulation with profiling."""
        brain = brain_factory()
        world = world_factory()
        agent = agent_factory(brain, world)
        
        if profile_operations:
            profiler.record_memory_usage("start_simulation")
        
        simulation_data = []
        
        for step in range(duration):
            if profile_operations:
                # Time each major operation
                world, time_update = profiler.time_operation("world_update", world.update, 0.1)
                
                percept, time_sensory = profiler.time_operation("sensory_input", agent.getSensoryInputSize)
                
                brain, time_brain = profiler.time_operation("brain_step", brain.step, step)
                
                action, time_action = profiler.time_operation("action_decode", agent.decodeMotorCommand)
                
                world, time_apply = profiler.time_operation("action_apply", world.applyMotorCommand, action, world.getSimulationTime())
                
                # Record data
                simulation_data.append({
                    "step": step,
                    "timing": {
                        "update": time_update,
                        "sensory": time_sensory,
                        "brain": time_brain,
                        "action": time_action,
                        "apply": time_apply
                    },
                    "metrics": {
                        "firing_neurons": brain.getFiringNeuronCount(),
                        "total_spikes": brain.getTotalSpikeCount(),
                        "curiosity": agent.getCuriosityLevel(),
                        "novelty": agent.getNoveltyLevel()
                    }
                })
            
            else:
                # Run without profiling
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                simulation_data.append({
                    "step": step,
                    "metrics": {
                        "firing_neurons": brain.getFiringNeuronCount(),
                        "total_spikes": brain.getTotalSpikeCount(),
                        "curiosity": agent.getCuriosityLevel(),
                        "novelty": agent.getNoveltyLevel()
                    }
                })
        
        if profile_operations:
            profiler.record_memory_usage("end_simulation")
        
        results = {
            "simulation_data": simulation_data,
            "brain": brain,
            "world": world,
            "agent": agent,
            "profiler": profiler,
            "profile_enabled": profile_operations
        }
        
        return results
    
    return run_profiled_simulation, profiler

# Convenience function for creating specialized agent types
def create_explorer_agent(world_width=30, world_height=30):
    """Create an agent specialized for exploration tasks."""
    return create_agent_with_behavior(
        task_type=NLMTaskTypes.EXPLORATION,
        world_width=world_width,
        world_height=world_height
    )

def create_survival_agent(world_width=20, world_height=20):
    """Create an agent specialized for survival tasks."""
    return create_agent_with_behavior(
        task_type=NLMTaskTypes.RESOURCE_COLLECTION,
        world_width=world_width,
        world_height=world_height
    )

def create_developer_agent(world_width=25, world_height=25):
    """Create an agent specialized for development and learning."""
    return create_agent_with_behavior(
        task_type=NLMTaskTypes.DEVELOPMENT,
        world_width=world_width,
        world_height=world_height
    )