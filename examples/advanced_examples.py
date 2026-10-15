#!/usr/bin/env python3
"""
NLM Advanced Examples
=====================

Advanced usage examples for experienced NLM users.
These examples demonstrate complex neural architectures, custom integrations,
and performance optimization techniques.
"""

import pynlm
import time
import math
import json
from pathlib import Path

def complex_brain_architecture_example():
    """
    Example: Complex multi-region brain architecture with specialized regions.
    
    Demonstrates how to create and manage different brain regions for specialized
    cognitive functions like perception, memory, planning, and motor control.
    """
    print("=== Complex Brain Architecture Example ===\n")
    
    # Step 1: Create base configuration
    print("1. Creating base configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.05)
    config.set("random_seed", 42)
    
    # Step 2: Create brain and regions
    print("2. Creating brain and specialized regions...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create specialized regions
    regions = {
        "sensory": brain.addRegion("sensory"),
        "working_memory": brain.addRegion("working_memory"),
        "episodic_memory": brain.addRegion("episodic_memory"),
        "concept": brain.addRegion("concept"),
        "planner": brain.addRegion("planner"),
        "motor": brain.addRegion("motor")
    }
    
    print("   Created regions:")
    for name, region_id in regions.items():
        print(f"     - {name}: {region_id}")
    
    # Step 3: Create agent and world
    print("\n3. Setting up agent and environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 4: Create visualization
    print("\n4. Initializing visualization...")
    # This would integrate with NLM's visualization system when available
    # For now, we'll just set up the visualization interface
    visualization_config = {
        "window_size": (1920, 1080),
        "neuron_radius": 2.0,
        "show_connections": True,
        "show_regions": True,
        "color_scheme": "default"
    }
    
    # Step 5: Run complex simulation
    print("\n5. Running complex multi-region simulation...")
    
    simulation_stats = {
        "steps": 0,
        "region_activity": {name: [] for name in regions.keys()},
        "total_spikes": 0,
        "learning_events": 0,
        "memory_updates": 0
    }
    
    # Main simulation loop with complex interactions
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent (which processes through all regions)
        agent.processSensoryInput(percept)
        
        # Run brain step (updates all regions)
        brain.step(step)
        
        # Record region-specific statistics
        for region_name, region_id in regions.items():
            region = brain.getRegion(region_id)
            if region:
                simulation_stats["region_activity"][region_name].append(
                    brain.getFiringNeuronCount()
                )
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning and development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Track statistics
        simulation_stats["steps"] += 1
        simulation_stats["total_spikes"] += brain.getTotalSpikeCount()
        
        # Print progress every 200 steps
        if step % 200 == 0 and step > 0:
            print(f"   Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Development={brain.getDevelopmentalStage()}")
    
    # Step 6: Analyze and visualize results
    print("\n6. Analyzing simulation results...")
    
    # Calculate statistics per region
    print("\n   Region Activity Summary:")
    for region_name, activities in simulation_stats["region_activity"].items():
        if activities:
            avg_activity = sum(activities) / len(activities)
            max_activity = max(activities)
            print(f"     {region_name:20}: Avg={avg_activity:6.1f}, Max={max_activity:6.1f}")
    
    # Overall statistics
    print(f"\n   Overall Statistics:")
    print(f"     Total steps: {simulation_stats['steps']}")
    print(f"     Total spikes: {simulation_stats['total_spikes']}")
    print(f"     Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"     Final development stage: {brain.getDevelopmentalStage()}")
    print(f"     Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"     Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Save results
    results_file = "complex_brain_simulation_results.json"
    with open(results_file, 'w') as f:
        json.dump({
            "final_stats": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "final_development_stage": brain.getDevelopmentalStage().value
            },
            "simulation_summary": simulation_stats
        }, f, indent=2)
    
    print(f"\n   Results saved to: {results_file}")
    
    return brain, world, agent, simulation_stats

def custom_neuromodulation_example():
    """
    Example: Custom neuromodulation system with multiple interacting signals.
    
    Demonstrates how to create complex neuromodulation systems that coordinate
    different learning and behavioral systems.
    """
    print("\n\n=== Custom Neuromodulation Example ===\n")
    
    # Step 1: Create configuration with custom neuromodulation settings
    print("1. Configuring custom neuromodulation system...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("random_seed", 12345)
    
    # Custom neuromodulation configuration
    custom_neuromod_config = {
        "dopamine": {
            "baseline": 0.0,
            "learning_rate": 0.01,
            "prediction_error_weight": 0.5,
            "novelty_interaction": True
        },
        "curiosity": {
            "exploration_rate": 0.3,
            "novelty_sensitivity": 2.0,
            "reward_threshold": 0.1
        },
        "novelty": {
            "sensitivity": 1.5,
            "prediction_horizon": 5,
            "decay_rate": 0.95
        },
        "prediction_error": {
            "precision": 0.9,
            "adaptation_rate": 0.1,
            "error_threshold": 0.01
        }
    }
    
    # Step 2: Create brain and agent
    print("2. Creating neural system...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=15, visionHeight=15)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 3: Custom neuromodulation simulation
    print("3. Running custom neuromodulation simulation...")
    
    neuromod_stats = {
        "steps": 0,
        "dopamine_levels": [],
        "curiosity_levels": [],
        "novelty_levels": [],
        "prediction_errors": [],
        "behavior_modifications": 0,
        "learning_accelerations": 0
    }
    
    # Custom neuromodulation loop
    for step in range(800):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Custom neuromodulation logic
        current_time = world.getSimulationTime()
        
        # 1. Custom dopamine calculation based on prediction error
        prediction_error = agent.getPredictionError()
        baseline_dopamine = custom_neuromod_config["dopamine"]["baseline"]
        learning_weight = custom_neuromod_config["dopamine"]["learning_rate"]
        
        # Enhanced dopamine signal based on error
        enhanced_dopamine = baseline_dopamine + prediction_error * learning_weight
        if custom_neuromod_config["dopamine"]["novelty_interaction"]:
            enhanced_dopamine *= (1.0 + agent.getNoveltyLevel())
        
        neuromod_stats["dopamine_levels"].append(enhanced_dopamine)
        
        # 2. Custom curiosity modulation
        novelty = agent.getNoveltyLevel()
        curiosity_sensitivity = custom_neuromod_config["curiosity"]["novelty_sensitivity"]
        exploration_rate = custom_neuromod_config["curiosity"]["exploration_rate"]
        
        # Curiosity affected by novelty and prediction error
        dynamic_curiosity = novelty * curiosity_sensitivity
        if prediction_error > custom_neuromod_config["curiosity"]["reward_threshold"]:
            dynamic_curiosity *= 1.5  # Boost exploration after large errors
        
        neuromod_stats["curiosity_levels"].append(dynamic_curiosity)
        
        # 3. Custom novelty detection
        novelty_detection = novelty * custom_neuromod_config["novelty"]["sensitivity"]
        prediction_horizon = custom_neuromod_config["novelty"]["prediction_horizon"]
        
        # Look back for novel patterns
        historical_novelty = sum(neuromod_stats["novelty_levels"][max(0, step - prediction_horizon):step]) / max(1, step)
        enhanced_novelty = novelty_detection * (1.0 + historical_novelty)
        neuromod_stats["novelty_levels"].append(enhanced_novelty)
        
        # 4. Custom prediction error tracking
        error_magnitude = abs(prediction_error)
        error_precision = custom_neuromod_config["prediction_error"]["precision"]
        adaptive_rate = custom_neuromod_config["prediction_error"]["adaptation_rate"]
        
        # Adaptive error tracking
        if error_magnitude > custom_neuromod_config["prediction_error"]["error_threshold"]:
            neuromod_stats["learning_accelerations"] += 1
            adaptive_error = error_magnitude * error_precision * adaptive_rate
        else:
            adaptive_error = error_magnitude * error_precision
        
        neuromod_stats["prediction_errors"].append(adaptive_error)
        
        # 5. Behavior modification based on neuromodulators
        if (enhanced_dopamine > 0.5 and dynamic_curiosity > 0.3 and 
            enhanced_novelty > 1.0):
            neuromod_stats["behavior_modifications"] += 1
            # This would trigger exploration behavior in a real system
        
        # Standard neuromodulation (for compatibility)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        neuromod_stats["steps"] += 1
        
        # Print progress
        if step % 200 == 0 and step > 0:
            print(f"   Step {step}: "
                  f"Dopamine={enhanced_dopamine:.3f}, "
                  f"Curiosity={dynamic_curiosity:.3f}, "
                  f"Novelty={enhanced_novelty:.3f}, "
                  f"Error={adaptive_error:.3f}")
    
    # Step 4: Analyze neuromodulation results
    print("\n4. Neuromodulation Analysis:")
    
    if neuromod_stats["dopamine_levels"]:
        avg_dopamine = sum(neuromod_stats["dopamine_levels"]) / len(neuromod_stats["dopamine_levels"])
        print(f"   Average dopamine level: {avg_dopamine:.3f}")
    
    if neuromod_stats["curiosity_levels"]:
        avg_curiosity = sum(neuromod_stats["curiosity_levels"]) / len(neuromod_stats["curiosity_levels"])
        print(f"   Average curiosity level: {avg_curiosity:.3f}")
    
    if neuromod_stats["novelty_levels"]:
        avg_novelty = sum(neuromod_stats["novelty_levels"]) / len(neuromod_stats["novelty_levels"])
        print(f"   Average novelty level: {avg_novelty:.3f}")
    
    if neuromod_stats["prediction_errors"]:
        avg_error = sum(neuromod_stats["prediction_errors"]) / len(neuromod_stats["prediction_errors"])
        print(f"   Average prediction error: {avg_error:.3f}")
    
    print(f"   Behavior modifications: {neuromod_stats['behavior_modifications']}")
    print(f"   Learning accelerations: {neuromod_stats['learning_accelerations']}")
    
    # Save detailed neuromodulation data
    neuromod_file = "neuromodulation_analysis.json"
    with open(neuromod_file, 'w') as f:
        json.dump({
            "configuration": custom_neuromod_config,
            "final_stats": {
                "final_dopamine": enhanced_dopamine,
                "final_curiosity": dynamic_curiosity,
                "final_novelty": enhanced_novelty,
                "final_error": adaptive_error,
                "total_steps": neuromod_stats["steps"]
            },
            "time_series": neuromod_stats
        }, f, indent=2)
    
    print(f"\n   Neuromodulation data saved to: {neuromod_file}")
    
    return brain, world, agent, neuromod_stats

def memory_integration_example():
    """
    Example: Advanced memory integration and episodic memory system.
    
    Demonstrates how to work with NLM's advanced memory systems including
    working memory, episodic memory, and associative memory.
    """
    print("\n\n=== Memory Integration Example ===\n")
    
    # Step 1: Create memory-optimized configuration
    print("1. Creating memory-optimized configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 8000)
    config.set("random_seed", 999)
    
    # Configure memory systems
    config.set("memory.working_memory_capacity", 1000)
    config.set("memory.episodic_max_episodes", 2000)
    config.set("memory.associative_strength_decay", 0.99)
    config.set("replay.interval", 50)
    config.set("consolidation.interval", 500)
    
    # Step 2: Create neural system
    print("2. Creating neural system with memory capabilities...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(17.5, 17.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 3: Access memory systems
    print("3. Accessing memory systems...")
    
    # Get memory system pointers (these would be exposed in the Python API)
    # For now, we'll simulate the memory system access
    memory_systems = {
        "working_memory": {
            "capacity": brain.getTotalNeuronCount() // 8,  # Approximate
            "current_activity": 0
        },
        "episodic_memory": {
            "max_episodes": 2000,
            "current_episodes": 0,
            "replay_frequency": 10
        },
        "associative_memory": {
            "pattern_count": 0,
            "association_strength": 0.0
        }
    }
    
    # Step 4: Memory-intensive simulation
    print("\n4. Running memory-intensive simulation...")
    
    memory_stats = {
        "steps": 0,
        "working_memory_usage": [],
        "episodic_memory_usage": [],
        "pattern_encodings": 0,
        "consolidations": 0,
        "replay_sessions": 0,
        "memory_efficiency": 0.0
    }
    
    # Special simulation to test memory systems
    for step in range(1200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input (should update working memory)
        agent.processSensoryInput(percept)
        
        # Run brain step (updates all systems)
        brain.step(step)
        
        # Simulate episodic memory updates (every 20 steps)
        if step % 20 == 0:
            memory_systems["episodic_memory"]["current_episodes"] += 1
            memory_stats["pattern_encodings"] += 1
            
            # Simulate memory consolidation (every 500 steps)
            if step % 500 == 0 and step > 0:
                memory_stats["consolidations"] += 1
                print(f"   Consolidation event at step {step}")
            
            # Simulate replay (every 100 steps)
            if step % 100 == 0 and step > 0:
                memory_stats["replay_sessions"] += 1
                print(f"   Replay session at step {step}")
        
        # Track memory usage
        memory_systems["working_memory"]["current_activity"] = brain.getFiringNeuronCount()
        memory_systems["working_memory"]["usage_ratio"] = (
            memory_systems["working_memory"]["current_activity"] / 
            memory_systems["working_memory"]["capacity"]
        )
        
        memory_stats["working_memory_usage"].append(
            memory_systems["working_memory"]["usage_ratio"]
        )
        
        memory_stats["episodic_memory_usage"].append(
            memory_systems["episodic_memory"]["current_episodes"] / 
            memory_systems["episodic_memory"]["max_episodes"]
        )
        
        # Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        memory_stats["steps"] += 1
        
        # Print progress
        if step % 300 == 0 and step > 0:
            usage_ratio = sum(memory_stats["working_memory_usage"]) / len(memory_stats["working_memory_usage"])
            memory_efficiency = 1.0 - usage_ratio  # Higher ratio = more efficient (less unused capacity)
            memory_stats["memory_efficiency"] = memory_efficiency
            print(f"   Step {step}: "
                  f"WM usage={usage_ratio:.2%}, "
                  f"EM loaded={memory_systems['episodic_memory']['current_episodes']}, "
                  f"Patterns={memory_stats['pattern_encodings']}")
    
    # Step 5: Memory system analysis
    print("\n5. Memory System Analysis:")
    
    if memory_stats["working_memory_usage"]:
        avg_wm_usage = sum(memory_stats["working_memory_usage"]) / len(memory_stats["working_memory_usage"])
        max_wm_usage = max(memory_stats["working_memory_usage"])
        print(f"   Working Memory:")
        print(f"     Average usage: {avg_wm_usage:.2%}")
        print(f"     Maximum usage: {max_wm_usage:.2%}")
        print(f"     Efficiency: {memory_stats['memory_efficiency']:.2%}")
    
    if memory_stats["episodic_memory_usage"]:
        avg_em_usage = sum(memory_stats["episodic_memory_usage"]) / len(memory_stats["episodic_memory_usage"])
        max_em_usage = max(memory_stats["episodic_memory_usage"])
        print(f"   Episodic Memory:")
        print(f"     Average utilization: {avg_em_usage:.2%}")
        print(f"     Maximum utilization: {max_em_usage:.2%}")
    
    print(f"   Total pattern encodings: {memory_stats['pattern_encodings']}")
    print(f"   Memory consolidations: {memory_stats['consolidations']}")
    print(f"   Replay sessions: {memory_stats['replay_sessions']}")
    
    # Calculate memory statistics
    total_memory_stats = {
        "working_memory": {
            "average_usage": avg_wm_usage if 'avg_wm_usage' in locals() else 0.0,
            "efficiency": memory_stats['memory_efficiency']
        },
        "episodic_memory": {
            "total_episodes": memory_systems["episodic_memory"]["current_episodes"],
            "max_episodes": memory_systems["episodic_memory"]["max_episodes"],
            "utilization_rate": (memory_systems["episodic_memory"]["current_episodes"] / 
                                memory_systems["episodic_memory"]["max_episodes"]) if memory_systems["episodic_memory"]["max_episodes"] > 0 else 0.0
        },
        "pattern_formation": {
            "total_patterns": memory_stats["pattern_encodings"],
            "consolidation_events": memory_stats["consolidations"],
            "replay_frequency": memory_stats["replay_sessions"] / max(1, memory_stats["steps"] / 100)
        }
    }
    
    # Save memory analysis
    memory_file = "memory_system_analysis.json"
    with open(memory_file, 'w') as f:
        json.dump({
            "final_memory_systems": memory_systems,
            "simulation_results": memory_stats,
            "performance_metrics": total_memory_stats,
            "recommendations": {
                "working_memory": "Consider increasing capacity" if avg_wm_usage > 0.8 else "Memory usage optimal",
                "episodic_memory": "Current capacity sufficient" if avg_em_usage < 0.7 else "Consider increasing capacity",
                "pattern_formation": "Good pattern formation" if memory_stats['pattern_encodings'] > 100 else "Limited pattern diversity"
            }
        }, f, indent=2)
    
    print(f"\n   Memory analysis saved to: {memory_file}")
    
    return brain, world, agent, memory_stats

def advanced_architecture_example():
    """
    Example: Advanced architectural patterns and optimization strategies.
    
    Demonstrates advanced architectural patterns including modular design,
    pipeline processing, and performance optimization techniques.
    """
    print("\n\n=== Advanced Architecture Example ===\n")
    
    # Step 1: Create advanced configuration with optimization parameters
    print("1. Creating advanced architecture configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 15000)
    config.set("random_seed", 777)
    
    # Advanced optimization settings
    advanced_config = {
        "architecture": {
            "pipeline_stages": 4,
            "modular_regions": True,
            "specialized_neurons": True
        },
        "performance": {
            "memory_pools": True,
            "simd_optimization": True,
            "thread_pool_size": 4,
            "event_driven": True
        },
        "learning": {
            "plasticity_rules": "mixed",
            "neuromodulation": "full",
            "development_stages": 5
        },
        "cognition": {
            "prediction_system": True,
            "working_memory": True,
            "planning": True,
            "attention": True
        }
    }
    
    # Apply advanced configuration
    config.set("performance.memory_pools", True)
    config.set("performance.simd_optimization", True)
    config.set("performance.thread_pool_size", 4)
    config.set("performance.event_driven", True)
    config.set("learning.plasticity_rules", "mixed")
    config.set("learning.development_stages", 5)
    config.set("cognition.prediction_system", True)
    config.set("cognition.planning", True)
    
    # Step 2: Create brain with advanced features
    print("2. Creating brain with advanced architecture...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=20, visionHeight=20)
    world.reset()
    world.setAgentStart(30.0, 30.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 3: Create visualization and monitoring
    print("3. Setting up advanced monitoring and visualization...")
    
    monitoring_config = {
        "real_time_monitoring": True,
        "performance_profiling": True,
        "neural_activity_tracking": True,
        "memory_monitoring": True,
        "neuromodulation_tracking": True,
        "data_export_interval": 50,
        "metrics_window": 1000
    }
    
    # Step 4: Advanced simulation with pipeline processing
    print("\n4. Running advanced pipeline simulation...")
    
    # Create pipeline stages for complex processing
    pipeline_stages = {
        "perception": [],
        "working_memory": [],
        "cognition": [],
        "action": []
    }
    
    advanced_stats = {
        "steps": 0,
        "pipeline_efficiency": 0.0,
        "memory_usage": {},
        "performance_metrics": {},
        "neural_patterns": {},
        "optimization_indicators": {}
    }
    
    # Advanced simulation with modular processing
    for step in range(2000):
        # Pipeline stage 1: Perception (sensory processing)
        start_time = time.time()
        percept = world.getSensoryPercept()
        perception_time = time.time() - start_time
        pipeline_stages["perception"].append(perception_time)
        
        # Pipeline stage 2: Working Memory (integration with working memory systems)
        start_time = time.time()
        agent.processSensoryInput(percept)
        working_memory_time = time.time() - start_time
        pipeline_stages["working_memory"].append(working_memory_time)
        
        # Pipeline stage 3: Cognition (processing through all cognitive systems)
        start_time = time.time()
        brain.step(step)
        cognition_time = time.time() - start_time
        pipeline_stages["cognition"].append(cognition_time)
        
        # Pipeline stage 4: Action (motor output and execution)
        start_time = time.time()
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        action_time = time.time() - start_time
        pipeline_stages["action"].append(action_time)
        
        # Learning and development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        agent.updateDevelopment(0.1)
        
        # Track performance metrics
        step_time = perception_time + working_memory_time + cognition_time + action_time
        advanced_stats["performance_metrics"][step] = {
            "total_step_time": step_time,
            "perception_time": perception_time,
            "working_memory_time": working_memory_time,
            "cognition_time": cognition_time,
            "action_time": action_time,
            "neural_activity": brain.getFiringNeuronCount(),
            "curiosity_level": agent.getCuriosityLevel(),
            "memory_efficiency": 1.0 - (brain.getFiringNeuronCount() / brain.getTotalNeuronCount())
        }
        
        # Track neural patterns
        if step % 100 == 0 and step > 0:
            # Detect pattern changes
            recent_activity = sum(pipeline_stages["cognition"]) / len(pipeline_stages["cognition"])
            advanced_stats["neural_patterns"][step] = {
                "average_cognition_time": recent_activity,
                "activity_level": brain.getAverageFiringRate(),
                "development_progress": brain.getDevelopmentalStage().value
            }
        
        # Optimization monitoring
        if step % 500 == 0 and step > 0:
            # Calculate pipeline efficiency
            avg_total_time = sum(pipeline_stages["perception"] + pipeline_stages["working_memory"] + 
                               pipeline_stages["cognition"] + pipeline_stages["action"]) / (step + 1)
            
            pipeline_efficiency = 1.0 / avg_total_time if avg_total_time > 0 else 0.0
            advanced_stats["pipeline_efficiency"] = pipeline_efficiency
            
            # Memory usage monitoring
            memory_usage = {
                "total_neurons": brain.getTotalNeuronCount(),
                "active_neurons": brain.getFiringNeuronCount(),
                "memory_efficiency": 1.0 - (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()),
                "development_stage": brain.getDevelopmentalStage().value
            }
            advanced_stats["memory_usage"][step] = memory_usage
            
            print(f"   Step {step}: "
                  f"Pipeline eff={pipeline_efficiency:.3f}, "
                  f"Activity={brain.getAverageFiringRate():.2f}, "
                  f"Dev={brain.getDevelopmentalStage()}")
        
        advanced_stats["steps"] += 1
    
    # Step 5: Advanced analysis and optimization
    print("\n5. Advanced Performance Analysis:")
    
    # Pipeline efficiency analysis
    if pipeline_stages["perception"]:
        avg_perception = sum(pipeline_stages["perception"]) / len(pipeline_stages["perception"])
        avg_working_memory = sum(pipeline_stages["working_memory"]) / len(pipeline_stages["working_memory"])
        avg_cognition = sum(pipeline_stages["cognition"]) / len(pipeline_stages["cognition"])
        avg_action = sum(pipeline_stages["action"]) / len(pipeline_stages["action"])
        
        print(f"   Pipeline Performance:")
        print(f"     Perception:    {avg_perception*1000:.1f}ms avg")
        print(f"     Working Memory: {avg_working_memory*1000:.1f}ms avg")
        print(f"     Cognition:     {avg_cognition*1000:.1f}ms avg")
        print(f"     Action:        {avg_action*1000:.1f}ms avg")
        print(f"     Total avg:     {(avg_perception+avg_working_memory+avg_cognition+avg_action)*1000:.1f}ms avg")
    
    # Neural activity analysis
    if advanced_stats["performance_metrics"]:
        avg_activity = sum(m["neural_activity"] for m in advanced_stats["performance_metrics"].values()) / len(advanced_stats["performance_metrics"])
        max_activity = max(m["neural_activity"] for m in advanced_stats["performance_metrics"].values())
        print(f"\n   Neural Activity Analysis:")
        print(f"     Average active neurons: {avg_activity:.0f}")
        print(f"     Maximum active neurons: {max_activity:.0f}")
        print(f"     Activity consistency: {1.0 - (max_activity - avg_activity) / max(avg_activity, 1):.3f}")
    
    # Development progress
    print(f"\n   Development Progress:")
    print(f"     Final stage: {brain.getDevelopmentalStage().value}")
    print(f"     Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"     Final novelty: {agent.getNoveltyLevel():.3f}")
    
    # Performance optimization indicators
    optimization_indicators = {
        "pipeline_efficiency": advanced_stats.get("pipeline_efficiency", 0.0),
        "memory_efficiency": sum(1.0 - (m["neural_activity"] / brain.getTotalNeuronCount()) 
                                for m in advanced_stats["performance_metrics"].values()) / len(advanced_stats["performance_metrics"]) if advanced_stats["performance_metrics"] else 0.0,
        "learning_rate": (brain.getTotalSpikeCount() / advanced_stats["steps"]) if advanced_stats["steps"] > 0 else 0.0,
        "stability": 1.0 - max(abs(m["neural_activity"] - avg_activity) for m in advanced_stats["performance_metrics"].values()) / max(avg_activity, 1) if advanced_stats["performance_metrics"] else 0.0
    }
    
    print(f"\n   Optimization Indicators:")
    for indicator, value in optimization_indicators.items():
        print(f"     {indicator.replace('_', ' ').title()}: {value:.3f}")
    
    # Save comprehensive results
    results_file = "advanced_architecture_results.json"
    with open(results_file, 'w') as f:
        json.dump({
            "configuration": advanced_config,
            "monitoring_config": monitoring_config,
            "final_metrics": {
                "total_steps": advanced_stats["steps"],
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "final_development_stage": brain.getDevelopmentalStage().value,
                "final_curiosity": agent.getCuriosityLevel(),
                "final_novelty": agent.getNoveltyLevel()
            },
            "performance_analysis": advanced_stats,
            "pipeline_efficiency": pipeline_efficiency if 'pipeline_efficiency' in locals() else 0.0,
            "optimization_recommendations": {
                "pipeline_optimization": "Consider parallelizing perception and action stages" if optimization_indicators["pipeline_efficiency"] < 0.5 else "Pipeline optimized",
                "memory_optimization": "Memory usage efficient" if optimization_indicators["memory_efficiency"] > 0.7 else "Consider memory optimization",
                "learning_optimization": "Learning active" if optimization_indicators["learning_rate"] > 10 else "Consider increasing learning rate",
                "stability_optimization": "System stable" if optimization_indicators["stability"] > 0.8 else "Improve system stability"
            }
        }, f, indent=2)
    
    print(f"\n   Comprehensive results saved to: {results_file}")
    
    return brain, world, agent, advanced_stats

def main():
    """Run all advanced examples."""
    print("NLM Advanced Examples Suite")
    print("=" * 60)
    
    try:
        # Example 1: Complex brain architecture
        brain1, world1, agent1, stats1 = complex_brain_architecture_example()
        
        # Example 2: Custom neuromodulation
        brain2, world2, agent2, stats2 = custom_neuromodulation_example()
        
        # Example 3: Memory integration
        brain3, world3, agent3, stats3 = memory_integration_example()
        
        # Example 4: Advanced architecture
        brain4, world4, agent4, stats4 = advanced_architecture_example()
        
        print("\n\n" + "=" * 60)
        print("All advanced examples completed successfully!")
        print("=" * 60)
        print("\nGenerated files:")
        print("  - complex_brain_simulation_results.json")
        print("  - neuromodulation_analysis.json")
        print("  - memory_system_analysis.json")
        print("  - advanced_architecture_results.json")
        
    except Exception as e:
        print(f"\nError running advanced examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()