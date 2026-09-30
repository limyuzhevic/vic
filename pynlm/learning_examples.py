"""
NLM Learning Examples
=====================

This module contains examples demonstrating learning mechanisms
and cognitive development in the NLM brain simulation framework.
"""

from .pynlm import create_brain, create_simple_world, create_agent, load_config_template
def basic_learning_example():
    """Example 1: Basic reward-based learning.
    
    This demonstrates the fundamental learning mechanism where the agent
    learns to maximize rewards through trial and error.
    """
    print("Example 1: Basic Reward-Based Learning")
    print("-" * 35)
    
    # Create learning-ready agent
    brain = create_brain({"neuron_count": 100})
    world = create_simple_world(width=12, height=12, vision_width=6, vision_height=6)
    agent = create_agent(brain, world)
    
    # Enable learning
    agent.enable_feature('reward_modulation', True)
    
    print(f"Initialized learning agent with {brain.get_stats()['total_neurons']} neurons")
    
    # Run learning simulation
    steps = 200
    action_preferences = {}
    reward_history = []
    
    # Track action preferences
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Compute and apply reward
        # Reward based on action type (arbitrary for demonstration)
        action_type = action.getType()
        if action_type == "MoveForward":
            reward = 1.0
        elif action_type in ["TurnLeft", "TurnRight"]:
            reward = 0.5
        elif action_type == "Wait":
            reward = -0.5
        else:
            reward = 0.0
        
        agent.apply_reward(reward)
        
        # Track action preferences
        if action_type not in action_preferences:
            action_preferences[action_type] = 0
        action_preferences[action_type] += 1
        
        reward_history.append(reward)
        
        if step % 40 == 0:
            print(f"Step {step}: Action={action.getType()}, Reward={reward:.1f}, "
                  f"Curiosity={agent.get_curiosity_level():.2f}")
    
    # Analyze learning results
    print(f"\nLearning Analysis ({steps} steps):")
    print(f"  - Action preferences: {action_preferences}")
    print(f"  - Most preferred action: {max(action_preferences.items(), key=lambda x: x[1])}")
    print(f"  - Average reward: {sum(reward_history)/len(reward_history):.2f}")
    print(f"  - Positive actions: {sum(1 for r in reward_history if r > 0)}")
    print(f"  - Negative actions: {sum(1 for r in reward_history if r < 0)}")
    print(f"  - Neutral actions: {sum(1 for r in reward_history if r == 0)}")
    
    # Learning assessment
    positive_ratio = sum(1 for r in reward_history if r > 0) / len(reward_history)
    print(f"  - Learning success: {'Good' if positive_ratio > 0.6 else 'Fair' if positive_ratio > 0.4 else 'Needs improvement'}")
    print()


def curiosity_exploration_example():
    """Example 2: Curiosity-driven exploration.
    
    This demonstrates how curiosity drives exploration of novel situations,
    leading to more efficient learning and discovery.
    """
    print("Example 2: Curiosity-Driven Exploration")
    print("-" * 35)
    
    # Create curiosity-enabled agent
    config = load_config_template('fast')
    brain = create_brain(config)
    world = create_simple_world(width=18, height=18, vision_width=8, vision_height=8)
    agent = create_agent(brain, world)
    
    # Enable curiosity and learning
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('curiosity', True)
    agent.enable_feature('structural_plasticity', True)
    
    print(f"Curiosity exploration agent initialized")
    print(f"Initial curiosity level: {agent.get_curiosity_level():.2f}")
    
    # Run curiosity simulation
    steps = 150
    exploration_log = []
    curiosity_driven_actions = 0
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Compute curiosity-enhanced reward
        base_reward = 0.1
        novelty_factor = agent.get_novelty_level() * 0.3
        curiosity_factor = agent.get_curiosity_level() * 0.4
        
        # Action-specific curiosity bonus
        action_type = action.getType()
        action_curiosity = {
            "MoveForward": 0.2,
            "Look": 0.3,
            "TurnLeft": 0.1,
            "TurnRight": 0.1,
            "Interact": 0.5,
            "Wait": -0.1
        }.get(action_type, 0.0)
        
        reward = base_reward + novelty_factor + curiosity_factor + action_curiosity
        
        # Track curiosity-driven exploration
        if agent.get_novelty_level() > 0.6 or agent.get_curiosity_level() > 0.7:
            curiosity_driven_actions += 1
            exploration_log.append({
                'step': step,
                'novelty': agent.get_novelty_level(),
                'curiosity': agent.get_curiosity_level(),
                'action': action_type,
                'reward': reward
            })
        
        agent.apply_reward(reward, 0.0)
        
        if step % 30 == 0:
            print(f"Step {step}: Curios={agent.get_curiosity_level():.2f}, "
                  f"Novel={agent.get_novelty_level():.2f}, Action={action_type}, Reward={reward:.2f}")
    
    # Analyze curiosity-driven learning
    print(f"\nCuriosity Analysis:")
    print(f"  - Total curiosity-driven exploration: {curiosity_driven_actions}")
    print(f"  - Exploration rate: {curiosity_driven_actions/steps:.2%}")
    print(f"  - Average curiosity: {sum(d['curiosity'] for d in exploration_log)/max(1, len(exploration_log)):.2f}")
    print(f"  - Average novelty: {sum(d['novelty'] for d in exploration_log)/max(1, len(exploration_log)):.2f}")
    
    # Curiosity effectiveness
    if exploration_log:
        high_curiosity_steps = sum(1 for d in exploration_log if d['curiosity'] > 0.8)
        high_novelty_steps = sum(1 for d in exploration_log if d['novelty'] > 0.7)
        print(f"  - High curiosity exploration: {high_curiosity_steps} steps")
        print(f"  - High novelty exploration: {high_novelty_steps} steps")
    
    print(f"  - Curiosity learning effectiveness: {'High' if curiosity_driven_actions > steps * 0.3 else 'Medium' if curiosity_driven_actions > steps * 0.2 else 'Low'}")
    print()


def reinforcement_learning_example():
    """Example 3: Advanced reinforcement learning.
    
    This demonstrates more sophisticated learning with prediction errors,
    future rewards, and goal-directed behavior.
    """
    print("Example 3: Advanced Reinforcement Learning")
    print("-" * 35)
    
    # Create advanced learning agent
    brain = create_brain({
        "neuron_count": 250,
        "connection_probability": 0.3,
        "stdp_ltp_weight": 0.04,
        "stdp_ltd_weight": 0.035
    })
    world = create_simple_world(width=20, height=20, vision_width=8, vision_height=8)
    agent = create_agent(brain, world)
    
    # Enable all learning features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('structural_plasticity', True)
    agent.enable_feature('curiosity', True)
    agent.enable_feature('development', True)
    
    print(f"Advanced RL agent with prediction error learning")
    
    # Run RL simulation
    steps = 300
    rl_metrics = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Compute reward with prediction error
        curiosity_bonus = agent.get_curiosity_level() * 0.2
        novelty_bonus = agent.get_novelty_level() * 0.3
        prediction_error = agent.get_prediction_error()
        
        # Reward based on action and prediction error
        base_reward = 0.2
        rl_reward = base_reward + curiosity_bonus + novelty_bonus + abs(prediction_error) * 0.5
        
        # Apply reward with prediction
        agent.apply_reward(rl_reward, prediction_error)
        
        # Store RL metrics
        rl_metrics.append({
            'step': step,
            'action': action.getType(),
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level(),
            'prediction_error': prediction_error,
            'reward': rl_reward
        })
        
        # Update development
        agent.update_development(0.1)
        
        if step % 50 == 0:
            print(f"Step {step}: Action={action.getType()}, PE={prediction_error:.4f}, "
                  f"Curios={agent.get_curiosity_level():.2f}, Novel={agent.get_novelty_level():.2f}, "
                  f"Reward={rl_reward:.2f}")
    
    # Analyze RL performance
    print(f"\nRL Performance Analysis:")
    
    # Prediction error analysis
    avg_prediction_error = sum(m['prediction_error'] for m in rl_metrics) / len(rl_metrics)
    avg_curiosity = sum(m['curiosity'] for m in rl_metrics) / len(rl_metrics)
    avg_novelty = sum(m['novelty'] for m in rl_metrics) / len(rl_metrics)
    
    print(f"  - Average prediction error: {avg_prediction_error:.4f}")
    print(f"  - Average curiosity: {avg_curiosity:.2f}")
    print(f"  - Average novelty: {avg_novelty:.2f}")
    
    # Action distribution
    action_dist = {}
    for m in rl_metrics:
        action_dist[m['action']] = action_dist.get(m['action'], 0) + 1
    
    print(f"  - Action distribution: {action_dist}")
    
    # Learning progression
    early_steps = rl_metrics[:100]
    late_steps = rl_metrics[-100:]
    
    early_pe = sum(m['prediction_error'] for m in early_steps) / len(early_steps)
    late_pe = sum(m['prediction_error'] for m in late_steps) / len(late_steps)
    
    print(f"  - Prediction error progression: {early_pe:.4f} → {late_pe:.4f}")
    print(f"  - Prediction error reduction: {(early_pe - late_pe)/early_pe:.1%}")
    
    # RL learning quality
    pe_reduction = (early_pe - late_pe) / early_pe if early_pe > 0 else 0
    if pe_reduction > 0.5:
        learning_quality = "Excellent"
    elif pe_reduction > 0.2:
        learning_quality = "Good"
    elif pe_reduction > 0.05:
        learning_quality = "Fair"
    else:
        learning_quality = "Needs improvement"
    
    print(f"  - RL learning quality: {learning_quality}")
    print()


def developmental_learning_example():
    """Example 4: Developmental learning over time.
    
    This demonstrates how learning capabilities develop and change
    over the agent's lifespan, showing maturation and specialization.
    """
    print("Example 4: Developmental Learning")
    print("-" * 35)
    
    # Create developing brain
    brain = create_brain({
        "neuron_count": 200,
        "enable_development": True,
        "synaptogenesis_rate": 0.0005,
        "pruning_rate": 0.00005
    })
    world = create_simple_world(width=15, height=15, vision_width=6, vision_height=6)
    agent = create_agent(brain, world)
    
    # Enable all development features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('structural_plasticity', True)
    agent.enable_feature('development', True)
    agent.enable_feature('curiosity', True)
    
    print(f"Developmental learning simulation")
    
    # Track developmental progression
    learning_metrics = []
    for phase in range(6):  # 6 developmental phases
        phase_steps = 40
        phase_metrics = []
        
        print(f"  Phase {phase + 1}/6:")
        
        for step in range(phase_steps):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory(percept)
            brain.step(phase * phase_steps + step)
            action = agent.decode_action()
            world.apply_action(action, world.get_simulation_time())
            
            # Compute reward based on developmental state
            curiosity_bonus = agent.get_curiosity_level() * 0.3
            novelty_bonus = agent.get_novelty_level() * 0.2
            reward = 0.15 + curiosity_bonus + novelty_bonus
            
            agent.apply_reward(reward, 0.0)
            agent.update_development(0.1)
            
            # Record metrics
            metric = {
                'phase': phase + 1,
                'step': phase * phase_steps + step,
                'development_stage': str(agent.get_development_stage()),
                'curiosity': agent.get_curiosity_level(),
                'novelty': agent.get_novelty_level(),
                'prediction_error': agent.get_prediction_error(),
                'action': action.getType(),
                'reward': reward
            }
            
            learning_metrics.append(metric)
            phase_metrics.append(metric)
            
            if step % 10 == 0:
                print(f"    Step {step}: Dev={agent.get_development_stage()}, "
                      f"Curios={agent.get_curiosity_level():.2f}, Novel={agent.get_novelty_level():.2f}")
        
        # Analyze phase
        avg_curiosity = sum(m['curiosity'] for m in phase_metrics) / len(phase_metrics)
        avg_novelty = sum(m['novelty'] for m in phase_metrics) / len(phase_metrics)
        avg_prediction_error = sum(m['prediction_error'] for m in phase_metrics) / len(phase_metrics)
        
        developmental_phases.append({
            'phase': phase + 1,
            'average_curiosity': avg_curiosity,
            'average_novelty': avg_novelty,
            'average_prediction_error': avg_prediction_error,
            'steps': phase_steps
        })
        
        print(f"    Average curiosity: {avg_curiosity:.2f}")
        print(f"    Average novelty: {avg_novelty:.2f}")
    
    # Analyze developmental progression
    print(f"\nDevelopmental Analysis:")
    
    # Developmental stage progression
    stages = [p['average_curiosity'] for p in developmental_phases]
    print(f"  - Curiosity progression: {[f'{s:.2f}' for s in stages]}")
    
    # Learning efficiency
    for p in developmental_phases:
        phase_metrics = [m for m in learning_metrics if m['phase'] == p['phase']]
        if phase_metrics:
            phase_reward = sum(m['reward'] for m in phase_metrics)
            p['total_reward'] = phase_reward
            print(f"  - Phase {p['phase']}: {p['total_reward']:.1f} total reward, "
                  f"curiosity={p['average_curiosity']:.2f}")
    
    # Developmental optimization
    best_phase = max(developmental_phases, key=lambda x: x['average_curiosity'])
    print(f"  - Peak curiosity at phase: {best_phase['phase']}")
    
    # Developmental trajectory
    if len(stages) >= 3:
        early_avg = sum(stages[:3]) / 3
        late_avg = sum(stages[-3:]) / 3
        if late_avg > early_avg * 1.2:
            trajectory = "Maturation"
        elif late_avg > early_avg * 1.05:
            trajectory = "Steady growth"
        else:
            trajectory = "Stable"
        print(f"  - Developmental trajectory: {trajectory}")
    
    print()


if __name__ == "__main__":
    print("NLM Learning Examples")
    print("=====================")
    print()
    
    try:
        basic_learning_example()
        curiosity_exploration_example()
        reinforcement_learning_example()
        developmental_learning_example()
        
        print("All learning examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
