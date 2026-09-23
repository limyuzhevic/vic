#!/usr/bin/env python3
"""
NLM Python Bindings - Advanced Examples

Advanced examples demonstrating the full power of the NLM Python bindings,
including performance optimization, memory management, and sophisticated simulations.
"""

import pynlm as nlm
import sys
import tempfile
import os

def advanced_simulation_with_performance():
    """Advanced simulation with performance optimization"""
    print("=== Advanced Simulation with Performance Optimization ===")
    
    # Create optimized brain
    brain = nlm.create_brain()
    optimized_brain = nlm.optimize_brain_for_performance(brain)
    print(f"Optimized brain: {optimized_brain}")
    
    # Create world
    world = nlm.create_world()
    world.configure(200, 200, 40, 40)
    
    # Enable memory optimizations
    optimized_brain = nlm.enable_memory_optimization(optimized_brain)
    
    # Initialize
    optimized_brain.initialize()
    
    # Run benchmark simulation
    import time
    start_time = time.time()
    
    for step in range(100):
        percept = world.get_sensory_percept()
        optimized_brain.receive_sensory_input(percept)
        action = optimized_brain.produce_action()
        world.update(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Completed 100 simulation steps in {duration:.2f} seconds")
    print(f"Average step time: {duration/100:.4f} seconds")
    
    # Debug performance
    nlm.debug_brain_connections(optimized_brain)
    efficiency = nlm.analyze_brain_efficiency(optimized_brain)
    print(f"Brain efficiency: {efficiency * 100:.1f}%")
    
    print("=== Advanced Performance Simulation Complete ===\n")

def memory_efficient_agent_training():
    """Memory-efficient agent training with checkpointing"""
    print("=== Memory-Efficient Agent Training ===")
    
    # Create training environment
    world = nlm.create_training_environment(100, 100, 20, 20)
    
    # Create advanced agent
    agent = nlm.create_advanced_agent(
        config=nlm.create_config(),
        world=world
    )
    
    # Training loop with periodic checkpoints
    checkpoint_interval = 20
    
    for episode in range(50):
        # Run episode
        agent.reset()
        agent.initialize(world)
        
        for step in range(checkpoint_interval):
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            action = agent.decode_motor_command()
            world.update(step)
        
        # Save checkpoint periodically
        if (episode + 1) % checkpoint_interval == 0:
            filepath = f"checkpoint_episode_{episode + 1}.json"
            agent.get_brain().save(filepath)
            print(f"Checkpoint saved: {filepath}")
        
        # Load and validate checkpoint
        if episode > 0 and episode % 10 == 0:
            filepath = f"checkpoint_episode_{episode - checkpoint_interval + 1}.json"
            if os.path.exists(filepath):
                config = nlm.create_config()
                loaded_brain = nlm.create_brain(config)
                loaded_brain.load(filepath)
                print(f"Loaded checkpoint from: {filepath}")
    
    print("=== Memory-Efficient Training Complete ===\n")

def custom_agent_with_all_systems():
    """Create custom agent with all systems enabled and custom behavior"""
    print("=== Custom Agent with All Systems ===")
    
    # Create custom configuration
    config = nlm.create_config()
    
    # Create world
    world = nlm.create_world()
    world.configure(150, 150, 30, 30)
    
    # Add custom world objects
    for x in range(0, 150, 20):
        for y in range(0, 150, 20):
            if (x + y) % 40 == 0:
                world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, 50.0))
            elif (x + y) % 40 == 20:
                world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, 0.3))
    
    # Create advanced agent with all systems
    agent = nlm.create_advanced_agent(config, world)
    
    # Custom behavior: reward-based learning with curiosity
    for step in range(50):
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Custom action selection based on neuromodulation levels
        curiosity = agent.get_curiosity_level()
        neuromodulation = agent.get_neuromodulation_level()
        
        if curiosity > 0.7:  # High curiosity: explore
            action = agent.decode_motor_command()
        elif neuromodulation > 0.5:  # High neuromodulation: exploit
            action = agent.decode_motor_command()
        else:  # Balanced: random exploration
            import random
            action_types = list(nlm.ActionType)
            action_type = random.choice(action_types)
            action = nlm.Action(action_type)
        
        # Apply custom reward modulation
        reward = 1.0 if action.get_type() == nlm.ActionType.Eat else 0.1
        predicted_reward = curiosity * neuromodulation
        agent.apply_reward_modulation(reward, predicted_reward)
        
        world.update(step)
        
        # Update development
        agent.update_development(step)
        
        # Check developmental stage
        stage = agent.get_developitional_stage()
        if stage == nlm.DevelopmentalStage.Aging:
            print("Brain reached aging stage - adapting behavior")
    
    print("=== Custom Agent with All Systems Complete ===\n")

def simulation_benchmarking():
    """Comprehensive benchmarking of different configurations"""
    print("=== Simulation Benchmarking ===")
    
    configurations = [
        {"name": "Small Brain", "size": 500},
        {"name": "Medium Brain", "size": 1000},
        {"name": "Large Brain", "size": 2000},
        {"name": "Large World", "size": 1000, "world_size": (200, 200, 50, 50)},
    ]
    
    import time
    
    for config in configurations:
        print(f"\nBenchmarking: {config['name']}")
        
        # Create brain
        if "size" in config:
            brain = nlm.create_brain_for_development_stage(nlm.DevelopmentalStage.Adult)
        else:
            brain = nlm.create_brain()
        
        # Create world
        if "world_size" in config:
            world = nlm.create_world()
            world.configure(*config["world_size"])
        else:
            world = nlm.create_world()
            world.configure(100, 100, 20, 20)
        
        # Initialize
        brain.initialize()
        
        # Benchmark
        steps = 50 if config["name"] != "Large World" else 25
        start_time = time.time()
        
        for step in range(steps):
            percept = world.get_sensory_percept()
            brain.receive_sensory_input(percept)
            action = brain.produce_action()
            world.update(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"  Completed {steps} steps in {duration:.2f} seconds")
        print(f"  Average step time: {duration/steps:.4f} seconds")
        print(f"  Performance: {steps/duration:.1f} steps/second")
    
    print("=== Benchmarking Complete ===\n")

def real_world_scenario_simulation():
    """Simulate a real-world scenario with agent-environment interaction"""
    print("=== Real-World Scenario Simulation ===")
    
    # Create environment: maze with resources and hazards
    world = nlm.create_world()
    world.configure(100, 100, 20, 20)
    
    # Create maze walls (impassable)
    for x in [20, 40, 60, 80]:
        for y in [20, 40, 60, 80]:
            world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Wall, 0.0))
    
    # Create resources scattered around
    import random
    for i in range(10):
        x = random.randint(10, 90)
        y = random.randint(10, 90)
        world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, 100.0))
    
    # Create hazards
    for i in range(5):
        x = random.randint(30, 70)
        y = random.randint(30, 70)
        world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, 0.5))
    
    # Create advanced agent
    agent = nlm.create_advanced_agent(
        config=nlm.create_config(),
        world=world
    )
    
    # Place agent in center
    agent.get_brain().get_config().set("agent_start_x", 50)
    agent.get_brain().get_config().set("agent_start_y", 50)
    
    # Run scenario: explore and collect resources
    scenario_steps = 100
    resources_collected = 0
    hazards_encountered = 0
    
    for step in range(scenario_steps):
        # Get current position
        body = world.get_agent_body()
        x, y = body.x, body.y
        
        # Check if agent is at a resource or hazard
        for obj in world.get_objects():
            if abs(obj.x - x) < 1.0 and abs(obj.y - y) < 1.0:
                if obj.type == nlm.WorldObjectType.Resource:
                    resources_collected += 1
                    print(f"Step {step}: Collected resource at ({x:.1f}, {y:.1f})")
                    world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Empty))
                elif obj.type == nlm.WorldObjectType.Hazard:
                    hazards_encountered += 1
                    print(f"Step {step}: Encountered hazard at ({x:.1f}, {y:.1f})")
                    world.add_object(nlm.WorldObject(x, y, nlm.WorldObjectType.Empty))
        
        # Process sensory input and select action
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Action selection based on curiosity and neuromodulation
        curiosity = agent.get_curiosity_level()
        neuromodulation = agent.get_neuromodulation_level()
        
        if curiosity > 0.8:  # High exploration
            action = nlm.Action(nlm.ActionType.Look)
        elif neuromodulation > 0.6:  # High exploitation
            action = agent.decode_motor_command()
        else:  # Balanced approach
            import random
            actions = [nlm.ActionType.MoveForward, nlm.ActionType.MoveLeft, 
                      nlm.ActionType.MoveRight, nlm.ActionType.TurnLeft, 
                      nlm.ActionType.TurnRight]
            action_type = random.choice(actions)
            action = nlm.Action(action_type)
        
        # Apply action and get reward
        action_result = world.apply_motor_command(action, step)
        agent.apply_reward_modulation(action_result.reward, agent.get_prediction_error())
        
        world.update(step)
        
        # Update development
        agent.update_development(step)
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"Step {step}: Pos({x:.1f}, {y:.1f}), Resources: {resources_collected}, Hazards: {hazards_encountered}")
    
    print(f"\nScenario Results:")
    print(f"  Total steps: {scenario_steps}")
    print(f"  Resources collected: {resources_collected}")
    print(f"  Hazards encountered: {hazards_encountered}")
    print(f"  Success rate: {resources_collected / max(1, resources_collected + hazards_encountered) * 100:.1f}%")
    
    print("=== Real-World Scenario Complete ===\n")

def batch_processing_simulation():
    """Batch processing simulation for testing multiple scenarios"""
    print("=== Batch Processing Simulation ===")
    
    scenarios = [
        {"name": "Small World", "width": 50, "height": 50, "vision": 10},
        {"name": "Medium World", "width": 100, "height": 100, "vision": 20},
        {"name": "Large World", "width": 200, "height": 200, "vision": 40},
    ]
    
    all_results = []
    
    for scenario in scenarios:
        print(f"\nRunning scenario: {scenario['name']}")
        
        # Create environment
        world = nlm.create_world()
        world.configure(scenario["width"], scenario["height"], 
                       scenario["vision"], scenario["vision"])
        
        # Create agent
        agent = nlm.create_advanced_agent(
            config=nlm.create_config(),
            world=world
        )
        
        # Run simulation
        steps = 20
        start_time = time.time()
        
        for step in range(steps):
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            action = agent.decode_motor_command()
            world.update(step)
            
            agent.apply_reward_modulation(0.1, agent.get_prediction_error())
        
        end_time = time.time()
        duration = end_time - start_time
        
        result = {
            "scenario": scenario["name"],
            "duration": duration,
            "steps": steps,
            "performance": steps / duration,
            "world_size": (scenario["width"], scenario["height"])
        }
        all_results.append(result)
        
        print(f"  Completed {steps} steps in {duration:.2f} seconds")
        print(f"  Performance: {result['performance']:.1f} steps/second")
    
    # Summary
    print(f"\nBatch Processing Results:")
    print(f"{'Scenario':<15} {'Steps':<6} {'Duration(s)':<12} {'Performance':<15}")
    print("-" * 58)
    
    for result in all_results:
        print(f"{result['scenario']:<15} {result['steps']:<6} {result['duration']:<12.2f} "
              f"{result['performance']:<15.1f}")
    
    print("=== Batch Processing Complete ===\n")

def main():
    """Run all advanced examples"""
    print("NLM Python Bindings - Advanced Examples")
    print("=" * 50)
    print()
    
    import time
    import random
    
    examples = [
        advanced_simulation_with_performance,
        memory_efficient_agent_training,
        custom_agent_with_all_systems,
        simulation_benchmarking,
        real_world_scenario_simulation,
        batch_processing_simulation,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print(f"Skipping example {example.__name__}\n")
    
    print("=" * 50)
    print("All advanced examples completed!")
    print("\nFor more information, see:")
    print("- Advanced usage examples in the examples/ directory")
    print("- Performance documentation in the NLM docs")
    print("- Research paper on NLM architecture and algorithms")

if __name__ == "__main__":
    main()