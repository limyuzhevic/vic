#!/usr/bin/env python3
"""
NLM Python Bindings - Example Scripts

This file contains example scripts demonstrating how to use the enhanced
NLM Python bindings with all advanced cognitive and memory systems.
"""

import pynlm as nlm

def basic_brain_example():
    """Basic brain creation and simulation example"""
    print("=== Basic Brain Example ===")
    
    # Create a simple brain
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    
    # Initialize the brain
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Basic example completed successfully!\n")

def working_memory_example():
    """Working memory example"""
    print("=== Working Memory Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create working memory system
    working_mem = nlm.create_neural_working_memory()
    working_mem.initialize(brain)
    working_mem.set_capacity(50)
    working_mem.set_decay_rate(0.95)
    
    # Simulate storing information
    test_patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.5, 0.4, 0.3, 0.2, 0.1],
        [0.9, 0.8, 0.7, 0.6, 0.5]
    ]
    
    for i, pattern in enumerate(test_patterns):
        working_mem.store(pattern, 1.0)
        print(f"Stored pattern {i}: {pattern}")
        
        # Retrieve after storing
        retrieved = working_mem.retrieve()
        print(f"Retrieved: {retrieved}")
        
        # Update working memory (simulate decay)
        working_mem.update(0.1)
        
        # Check memory statistics
        print(f"Active traces: {working_mem.get_active_traces()}")
    
    print("Working memory example completed successfully!\n")

def episodic_memory_example():
    """Episodic memory example"""
    print("=== Episodic Memory Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create episodic memory system
    episodic_mem = nlm.create_neural_episodic_memory()
    episodic_mem.initialize(brain)
    episodic_mem.set_max_episodes(100)
    episodic_mem.set_replay_interval(50)
    
    # Create some test episodes
    episodes = []
    for i in range(5):
        episode = nlm.EpisodicMemoryItem()
        episode.sensory_state = [0.1 * (i + 1), 0.2 * (i + 1), 0.3 * (i + 1)]
        episode.action = nlm.ActionType.MoveForward if i % 2 == 0 else nlm.ActionType.TurnLeft
        episode.reward = 0.5 + (i * 0.1)
        episode.timestamp = i * 10
        episodes.append(episode)
    
    # Store episodes
    for i, episode in enumerate(episodes):
        episodic_mem.store_episode(episode)
        print(f"Stored episode {i}: action={episode.action.get_name()}, reward={episode.reward}")
    
    # Check statistics
    print(f"Number of stored episodes: {len(episodic_mem.get_episodes())}")
    print(f"Should replay: {episodic_mem.should_replay()}")
    
    # Get recent episodes
    recent = episodic_mem.get_recent_episodes(3)
    print(f"Recent episodes count: {len(recent)}")
    
    print("Episodic memory example completed successfully!\n")

def neural_planner_example():
    """Neural planner example"""
    print("=== Neural Planner Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create neural planner
    planner = nlm.create_neural_planner()
    planner.initialize(brain)
    planner.set_planning_depth(3)
    planner.set_current_goal([0.8, 0.6, 0.4, 0.2, 0.0])
    
    # Create test states
    states = [
        [0.0, 0.0, 0.0, 0.0, 0.0],
        [0.5, 0.5, 0.5, 0.5, 0.5],
        [1.0, 1.0, 1.0, 1.0, 1.0]
    ]
    
    for i, state in enumerate(states):
        print(f"\nPlanning for state {i}: {state}")
        
        # Plan action
        action = planner.plan_action(state)
        print(f"Planned action: {action.get_name()}")
        
        # Evaluate action sequence
        actions = [action]
        candidate = planner.evaluate_sequence(actions, state)
        print(f"Evaluation - Reward: {candidate.expected_reward}, Confidence: {candidate.confidence}")
        
        # Simulate some learning
        for j in range(3):
            planner.update_plan_quality([action], [action], 0.5 + j * 0.1)
    
    print("Neural planner example completed successfully!\n")

def concept_formation_example():
    """Concept formation example"""
    print("=== Concept Formation Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create concept formation system
    concept_system = nlm.create_concept_formation()
    concept_system.initialize(brain)
    concept_system.set_formation_threshold(0.7)
    concept_system.set_stability_threshold(0.6)
    
    # Create test experiences
    experiences = []
    for i in range(10):
        pattern = [0.1 * i, 0.2 * i, 0.3 * i, 0.4 * i, 0.5 * i]
        features = [0.5, 0.6, 0.7, 0.8, 0.9]
        reward = 0.1 * i
        experiences.append((pattern, features, reward, i))
    
    # Present experiences
    for i, (pattern, features, reward, time) in enumerate(experiences):
        concept_id = concept_system.present_experience(pattern, features, reward, time)
        if concept_id > 0:
            print(f"Experience {i}: New concept discovered: {concept_id}")
            stability = concept_system.get_concept_stability(concept_id)
            print(f"  Concept stability: {stability}")
            
            # Get concept information
            prototype = concept_system.get_concept_prototype(concept_id)
            print(f"  Concept prototype: {prototype}")
            
            instances = concept_system.get_concept_instances(concept_id)
            print(f"  Number of instances: {len(instances)}")
    
    print(f"Total concepts discovered: {concept_system.get_concept_count()}")
    print("Concept formation example completed successfully!\n")

def self_model_example():
    """Self-model example"""
    print("=== Self-Model Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create self-model
    self_model = nlm.create_self_model()
    self_model.initialize(brain)
    
    # Simulate some self-actions
    test_actions = [
        (nlm.ActionType.MoveForward, [0.0, 0.0, 0.0], [0.1, 0.2, 0.3]),
        (nlm.ActionType.TurnLeft, [0.1, 0.2, 0.3], [0.15, 0.25, 0.35]),
        (nlm.ActionType.Interact, [0.15, 0.25, 0.35], [0.2, 0.3, 0.4]),
    ]
    
    for action, before_state, after_state in test_actions:
        self_model.record_self_action(action, before_state, after_state)
        print(f"Recorded action: {action.get_name()}")
        
        # Predict consequences
        predicted = self_model.predict_action_consequence(action, before_state)
        print(f"Predicted state: {predicted}")
        
        # Check confidence
        confidence = self_model.get_self_model_confidence(action)
        print(f"Self-model confidence: {confidence}")
    
    # Get preferred actions
    test_states = [
        [0.0, 0.0, 0.0],
        [0.5, 0.5, 0.5],
        [1.0, 1.0, 1.0]
    ]
    
    for state in test_states:
        preferred = self_model.get_preferred_action(state)
        print(f"State {state}: Preferred action: {preferred.get_name()}")
    
    print("Self-model example completed successfully!\n")

def social_learning_example():
    """Social learning example"""
    print("=== Social Learning Example ===")
    
    # Create brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create social learning system
    social = nlm.create_social_learning()
    social.initialize(brain)
    
    # Create test observations
    observations = []
    for i in range(5):
        action = nlm.ActionType.Interact if i % 2 == 0 else nlm.ActionType.MoveForward
        observer_state = [0.1 * i, 0.2 * i, 0.3 * i]
        resulting_state = [0.15 * i, 0.25 * i, 0.35 * i]
        observations.append((action, observer_state, resulting_state))
    
    # Process observations
    for i, (action, observer_state, resulting_state) in enumerate(observations):
        social.observe_agent_action(action, observer_state, resulting_state)
        print(f"Observed action {i}: {action.get_name()}")
    
    # Check imitation capabilities
    for action in [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft, nlm.ActionType.Interact]:
        can_imitate = social.can_imitate(action)
        print(f"Can imitate {action.get_name()}: {can_imitate}")
    
    # Get imitation actions
    current_state = [0.2, 0.4, 0.6]
    imitation_action = social.get_imitation_action(current_state)
    print(f"Imitation action for state {current_state}: {imitation_action.get_name()}")
    
    # Get social knowledge statistics
    print(f"Total observations: {social.get_observation_count()}")
    print(f"Has social knowledge: {social.has_social_knowledge()}")
    
    print("Social learning example completed successfully!\n")

def complete_agent_example():
    """Complete agent integration example"""
    print("=== Complete Agent Integration Example ===")
    
    # Create core systems
    print("Creating core systems...")
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    agent = nlm.create_agent_brain(brain)
    world = nlm.create_simple_world()
    
    # Configure world
    print("Configuring world...")
    world.configure(width=10, height=10, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(5.0, 5.0)
    
    # Initialize agent
    print("Initializing agent...")
    agent.initialize(world)
    
    # Create advanced systems
    print("Creating advanced systems...")
    systems = {
        'NeuralPlanner': nlm.create_neural_planner(),
        'ConceptFormation': nlm.create_concept_formation(),
        'SelfModel': nlm.create_self_model(),
        'SocialLearning': nlm.create_social_learning(),
        'WorkingMemory': nlm.create_neural_working_memory(),
        'EpisodicMemory': nlm.create_neural_episodic_memory(),
    }
    
    # Initialize all systems
    for name, system in systems.items():
        try:
            system.initialize(brain)
            print(f"  ✅ {name} initialized")
        except Exception as e:
            print(f"  ❌ {name} failed: {e}")
    
    # Configure planner
    systems['NeuralPlanner'].set_planning_depth(3)
    
    # Configure agent
    print("\nConfiguring agent...")
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    agent.enable_neural_planning(True)
    
    # Run simulation
    print("\nRunning simulation...")
    num_steps = 50
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decode_motor_command()
        
        # Apply to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().getInternal()[0] if world.get_sensory_percept().getInternal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Working memory traces: {systems['WorkingMemory'].get_active_traces()}")
    
    print(f"\n✅ Complete agent simulation finished! Ran {num_steps} steps.")
    print("Complete agent example completed successfully!\n")

def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Python Bindings - Example Scripts")
    print("=" * 60)
    print()
    
    # Run examples
    basic_brain_example()
    working_memory_example()
    episodic_memory_example()
    neural_planner_example()
    concept_formation_example()
    self_model_example()
    social_learning_example()
    complete_agent_example()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)

if __name__ == "__main__":
    main()
