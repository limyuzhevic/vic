# NLM Research Workflow Examples

This file contains comprehensive research workflow examples for using NLM in scientific research scenarios.

```python
"""NLM Research Workflow Examples"""
import pynlm
import numpy as np

# Research Workflow 1: Memory Formation and Consolidation Study
# Study how working memory integrates with episodic memory
# Research question: How does working memory capacity affect episodic memory formation?

def memory_formation_study():
    """Study memory formation and consolidation processes."""
    
    print("=== MEMORY FORMATION AND CONSOLIDATION STUDY ===")
    print("Research Objective: Examine working memory → episodic memory integration")
    
    # Create experimental brain with varying working memory capacity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning systems for memory study
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Experimental parameters
    working_memory_capacity = brain.getWorkingMemory().getCapacity()
    num_trials = 500
    memory_trace_threshold = working_memory_capacity * 0.8
    
    print(f"Working memory capacity: {working_memory_capacity} traces")
    print(f"Total experimental trials: {num_trials}")
    
    # Run memory formation experiment
    episodic_episodes = []
    working_memory_utilization = []
    reward_learning_curves = []
    
    for trial in range(num_trials):
        # Environmental stimulus
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input (stores in working memory)
        agent.processSensoryInput(percept)
        brain.step(trial)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply modulation (learning from experience)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Track memory formation
        wm_utilization = brain.getWorkingMemory().getUtilization()
        working_memory_utilization.append(wm_utilization)
        
        # Record episodic episodes when formed
        if brain.getEpisodicMemory().getEpisodeCount() > len(episodic_episodes):
            # Capture new episode
            episode_data = {
                'trial': trial,
                'reward': reward,
                'wm_utilization': wm_utilization,
                'dopamine_level': brain.getDopamine().getLevel(),
                'neurons_active': brain.getActiveNeuronCount(),
                'firing_rate': brain.getAverageFiringRate()
            }
            episodic_episodes.append(episode_data)
        
        # Track reward learning
        if trial % 10 == 0:
            reward_learning_curves.append({
                'trial': trial,
                'reward': reward,
                'dopamine': brain.getDopamine().getLevel(),
                'curiosity': agent.getCuriosityLevel()
            })
        
        # Update development (affects memory consolidation)
        agent.updateDevelopment(0.1)
        
        # Progress report
        if trial % 100 == 0 and trial > 0:
            print(f"\nTrial {trial}:")
            print(f"  Working memory utilization: {wm_utilization:.1%}")
            print(f"  Episodic episodes formed: {len(episodic_episodes)}")
            print(f"  Average reward: {np.mean([e['reward'] for e in episodic_episodes]):.3f}")
    
    # Analyze memory consolidation
    print("\n=== MEMORY CONSOLIDATION ANALYSIS ===")
    
    # Calculate consolidation effectiveness
    if len(episodic_episodes) > 10:
        recent_episodes = episodic_episodes[-10:]
        earlier_episodes = episodic_episodes[:10] if len(episodic_episodes) >= 20 else episodic_episodes[:len(episodic_episodes)//2]
        
        recent_avg_reward = np.mean([e['reward'] for e in recent_episodes])
        earlier_avg_reward = np.mean([e['reward'] for e in earlier_episodes])
        
        print(f"Episodic memory formation rate: {len(episodic_episodes)/num_trials:.1%} of trials")
        print(f"Working memory utilization average: {np.mean(working_memory_utilization):.1%}")
        print(f"Memory consolidation effectiveness: {(recent_avg_reward - earlier_avg_reward):.3f}")
        
        # Memory replay analysis
        replay_count = brain.getEpisodicMemory().getReplayCount()
        consolidation_score = replay_count / max(1, len(episodic_episodes) - 10)
        print(f"Memory replay frequency: {replay_count}")
        print(f"Consolidation score: {consolidation_score:.3f}")
    
    return {
        'episodes': episodic_episodes,
        'wm_utilization': working_memory_utilization,
        'reward_learning': reward_learning_curves,
        'final_statistics': {
            'total_episodes': len(episodic_episodes),
            'working_memory_capacity': working_memory_capacity,
            'wm_utilization_avg': np.mean(working_memory_utilization) if working_memory_utilization else 0,
            'development_stage': brain.getDevelopmentalStage().name,
            'dopamine_level': brain.getDopamine().getLevel()
        }
    }

# Research Workflow 2: Prediction Error and Learning Study
# Study how prediction errors drive learning and neuromodulation

def prediction_error_learning_study():
    """Study prediction error-driven learning and neuromodulation."""
    
    print("\n=== PREDICTION ERROR AND LEARNING STUDY ===")
    print("Research Objective: Examine prediction error signals in learning")
    
    # Create prediction-focused brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent.initialize(world)
    
    # Enable prediction system and neuromodulation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Experimental parameters
    num_prediction_trials = 300
    prediction_window = 50
    
    # Track prediction error and learning
    prediction_errors = []
    reward_prediction_accuracy = []
    dopamine_traces = []
    curiosity_levels = []
    
    for trial in range(num_prediction_trials):
        # Get current sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process input and get prediction
        agent.processSensoryInput(percept)
        brain.step(trial)
        
        # Get prediction and compare with reality
        prediction = brain.getPredictionSystem()
        prediction.train(percept)  # Update prediction based on current input
        
        # Get prediction error
        pred_error = prediction.getError()
        expected_reward = prediction.getExpectedReward()
        actual_reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Track prediction accuracy
        pred_accuracy = 1.0 - abs(pred_error)
        reward_pred_accuracy = 1.0 - abs(expected_reward - actual_reward)
        
        prediction_errors.append(pred_error)
        reward_prediction_accuracy.append(reward_pred_accuracy)
        dopamine_traces.append(brain.getDopamine().getLevel())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        # Apply reward modulation based on prediction error
        agent.applyRewardModulation(actual_reward, pred_error)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development and track metrics
        agent.updateDevelopment(0.1)
        
        # Progress report
        if trial % 50 == 0 and trial > 0:
            print(f"\nTrial {trial}:")
            print(f"  Average prediction error: {np.mean(prediction_errors[-50:]):.4f}")
            print(f"  Average reward prediction accuracy: {np.mean(reward_prediction_accuracy[-50:]):.3f}")
            print(f"  Dopamine level: {np.mean(dopamine_traces[-50:]):.3f}")
            print(f"  Curiosity level: {np.mean(curiosity_levels[-50:]):.3f}")
    
    # Analyze prediction error dynamics
    print("\n=== PREDICTION ERROR DYNAMICS ANALYSIS ===")
    
    # Prediction error statistics
    mean_prediction_error = np.mean(prediction_errors)
    std_prediction_error = np.std(prediction_errors)
    prediction_error_variance = np.var(prediction_errors)
    
    # Reward prediction accuracy
    mean_reward_accuracy = np.mean(reward_prediction_accuracy)
    reward_accuracy_trend = reward_prediction_accuracy[-50:] - reward_prediction_accuracy[:-50] if len(reward_prediction_accuracy) > 50 else []
    
    print(f"Prediction error mean: {mean_prediction_error:.4f}")
    print(f"Prediction error standard deviation: {std_prediction_error:.4f}")
    print(f"Prediction error variance: {prediction_error_variance:.6f}")
    print(f"Reward prediction accuracy: {mean_reward_accuracy:.3f}")
    print(f"Dopamine-prediction error correlation: {np.corrcoef(dopamine_traces, prediction_errors)[0,1]:.3f}")
    print(f"Curiosity-prediction error correlation: {np.corrcoef(curiosity_levels, prediction_errors)[0,1]:.3f}")
    
    # Learning curve analysis
    learning_curve = []
    for i in range(0, len(prediction_errors), 10):
        segment = prediction_errors[i:i+10]
        learning_curve.append(np.mean(segment))
    
    return {
        'prediction_errors': prediction_errors,
        'reward_prediction_accuracy': reward_prediction_accuracy,
        'dopamine_traces': dopamine_traces,
        'curiosity_levels': curiosity_levels,
        'analysis': {
            'mean_prediction_error': mean_prediction_error,
            'std_prediction_error': std_prediction_error,
            'mean_reward_accuracy': mean_reward_accuracy,
            'dopamine_error_correlation': np.corrcoef(dopamine_traces, prediction_errors)[0,1],
            'learning_curve': learning_curve
        }
    }

# Research Workflow 3: Neuromodulation and Development Study
# Study how development stages affect neuromodulation and learning

def neuromodulation_development_study():
    """Study developmental changes in neuromodulation and learning."""
    
    print("\n=== NEUROMODULATION AND DEVELOPMENT STUDY ===")
    print("Research Objective: Examine developmental changes in neuromodulation")
    
    # Create development-focused brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 4000)
    config.set("brain.region_count", 2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableDevelopment(True)
    
    # Developmental stage tracking
    developmental_stages = [
        (0, 100, 'Initial'),
        (101, 500, 'CriticalPeriod'),
        (501, 1000, 'Maturation'),
        (1001, 2000, 'Adult'),
        (2001, 3000, 'Aging')
    ]
    
    # Track neuromodulation across development
    stage_data = {}
    neuromodulation_traces = []
    learning_metrics = []
    
    current_stage_start = 0
    current_stage_name = 'Initial'
    
    for trial in range(3000):
        # Update world and get perception
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(trial)
        
        # Get current developmental stage
        developmental_stage = brain.getDevelopmentalStage()
        
        # Check if we entered a new developmental stage
        if trial >= current_stage_start + developmental_stages[current_stage_start//101][1]:
            next_stage_idx = current_stage_start//101 + 1
            if next_stage_idx < len(developmental_stages):
                current_stage_start = developmental_stages[next_stage_idx][0]
                current_stage_name = developmental_stages[next_stage_idx][2]
        
        # Record neuromodulation data
        dopamine_level = brain.getDopamine().getLevel()
        acetylcholine_level = brain.getAcetylcholine().getLevel()
        norepinephrine_level = brain.getNorepinephrine().getLevel()
        serotonin_level = brain.getSerotonin().getLevel()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        
        neuromodulation_traces.append({
            'trial': trial,
            'stage': current_stage_name,
            'dopamine': dopamine_level,
            'acetylcholine': acetylcholine_level,
            'norepinephrine': norepinephrine_level,
            'serotonin': serotonin_level,
            'curiosity': curiosity_level,
            'novelty': novelty_level,
            'developmental_plasticity': brain.getDevelopmentSystem().getPlasticityRate()
        })
        
        # Get reward and apply modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Track learning metrics
        learning_metrics.append({
            'trial': trial,
            'stage': current_stage_name,
            'prediction_error': prediction_error,
            'learning_rate': dopamine_level * 0.1,  # Simplified learning rate estimate
            'exploration': curiosity_level,
            'memory_utilization': brain.getWorkingMemory().getUtilization()
        })
        
        # Progress report
        if trial % 500 == 0 and trial > 0:
            print(f"\nTrial {trial} ({current_stage_name}):")
            print(f"  Dopamine: {dopamine_level:.3f}")
            print(f"  Acetylcholine: {acetylcholine_level:.3f}")
            print(f"  Norepinephrine: {norepinephrine_level:.3f}")
            print(f"  Serotonin: {serotonin_level:.3f}")
            print(f"  Curiosity: {curiosity_level:.3f}")
            print(f"  Memory utilization: {brain.getWorkingMemory().getUtilization():.1%}")
    
    # Analyze developmental changes
    print("\n=== DEVELOPMENTAL CHANGE ANALYSIS ===")
    
    # Neuromodulation stage averages
    stage_averages = {}
    for stage_name in ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']:
        stage_trials = [m for m in neuromodulation_traces if m['stage'] == stage_name]
        if stage_trials:
            stage_averages[stage_name] = {
                'dopamine': np.mean([t['dopamine'] for t in stage_trials]),
                'acetylcholine': np.mean([t['acetylcholine'] for t in stage_trials]),
                'norepinephrine': np.mean([t['norepinephrine'] for t in stage_trials]),
                'serotonin': np.mean([t['serotonin'] for t in stage_trials]),
                'curiosity': np.mean([t['curiosity'] for t in stage_trials]),
                'novelty': np.mean([t['novelty'] for t in stage_trials]),
                'plasticity_rate': np.mean([t['developmental_plasticity'] for t in stage_trials])
            }
    
    for stage, averages in stage_averages.items():
        print(f"\n{stage} Stage:")
        print(f"  Dopamine: {averages['dopamine']:.3f}")
        print(f"  Acetylcholine: {averages['acetylcholine']:.3f}")
        print(f"  Norepinephrine: {averages['norepinephrine']:.3f}")
        print(f"  Serotonin: {averages['serotonin']:.3f}")
        print(f"  Curiosity: {averages['curiosity']:.3f}")
        print(f"  Plasticity rate: {averages['plasticity_rate']:.3f}")
    
    return {
        'neuromodulation_traces': neuromodulation_traces,
        'learning_metrics': learning_metrics,
        'stage_averages': stage_averages
    }

# Research Workflow 4: Advanced Network Analysis
# Study brain network topology and information flow

def advanced_network_analysis():
    """Perform advanced network analysis on brain connectivity."""
    
    print("\n=== ADVANCED NETWORK ANALYSIS ===")
    print("Research Objective: Analyze brain network topology and information flow")
    
    # Create large-scale brain for network analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 8000)
    config.set("brain.region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to establish stable connectivity
    for step in range(200):
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Simple interaction to build connectivity
        for substep in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step * 50 + substep)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Network analysis
    stats = brain.getEnhancedStatistics()
    topology = stats.getNetworkTopology()
    information_flow = stats.getInformationFlowAnalysis()
    functional_networks = stats.getFunctionalNetworkAnalysis()
    
    print("=== NETWORK TOPOLOGY ANALYSIS ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average clustering coefficient: {topology.getAverageClusteringCoefficient():.4f}")
    print(f"Average path length: {topology.getAveragePathLength():.4f}")
    print(f"Network efficiency: {topology.getNetworkEfficiency():.4f}")
    print(f"Small-world coefficient: {topology.getSmallWorldCoefficient():.4f}")
    print(f"Community count: {topology.getCommunityCount()}")
    print(f"Community distribution: {topology.getCommunityDistribution()}")
    
    print("\n=== INFORMATION FLOW ANALYSIS ===")
    print(f"Hub count (high betweenness centrality): {information_flow.getHubCount()}")
    print(f"Bottleneck count (critical nodes): {information_flow.getBottleneckCount()}")
    print(f"Flow efficiency: {information_flow.getFlowEfficiency():.4f}")
    print(f"Critical pathways: {information_flow.getCriticalPathways()}")
    print(f"Information bottlenecks: {information_flow.getInformationBottlenecks()}")
    
    print("\n=== FUNCTIONAL NETWORK ANALYSIS ===")
    print(f"Functional modules: {functional_networks.getModuleCount()}")
    print(f"Module connectivity: {functional_networks.getConnectivityLevel():.3f}")
    print(f"Module specialization: {functional_networks.getSpecializationLevel():.3f}")
    print(f"Cross-module integration: {functional_networks.getCrossModuleIntegration():.3f}")
    print(f"Information hubs: {functional_networks.getInformationHubs()}")
    
    # Network robustness analysis
    robustness_metrics = stats.getNetworkRobustnessAnalysis()
    print("\n=== NETWORK ROBUSTNESS ANALYSIS ===")
    print(f"Attack tolerance: {robustness_metrics.getAttackTolerance():.3f}")
    print(f"Failure recovery: {robustness_metrics.getFailureRecovery():.3f}")
    print(f"Redundancy level: {robustness_metrics.getRedundancyLevel():.3f}")
    print(f"Adaptive capacity: {robustness_metrics.getAdaptiveCapacity():.3f}")
    
    # Performance analysis
    performance_metrics = stats.getPerformanceMetrics()
    print("\n=== PERFORMANCE METRICS ===")
    print(f"Simulation efficiency: {performance_metrics.getSimulationEfficiency():.3f}")
    print(f"Memory efficiency: {performance_metrics.getMemoryEfficiency():.3f}")
    print(f"Information processing rate: {performance_metrics.getInformationProcessingRate():.2f} Hz")
    print(f"Learning speed: {performance_metrics.getLearningSpeed():.3f}")
    print(f"Adaptation rate: {performance_metrics.getAdaptationRate():.3f}")
    
    return {
        'topology': topology,
        'information_flow': information_flow,
        'functional_networks': functional_networks,
        'robustness': robustness_metrics,
        'performance': performance_metrics
    }

# Research Workflow 5: Multi-Agent Integration
# Study multiple agents interacting and learning together

def multi_agent_interaction_study():
    """Study multiple agents interacting and learning collaboratively."""
    
    print("\n=== MULTI-AGENT INTERACTION STUDY ===")
    print("Research Objective: Examine collaborative learning and social interaction")
    
    # Create multiple brains with social learning enabled
    num_agents = 3
    agents = []
    brains = []
    worlds = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000 + i * 500)
        config.set("brain.region_count", 2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=35, height=35, visionWidth=12, visionHeight=12)
        world.reset()
        
        # Set different starting positions for agents
        world.setAgentStart((i + 1) * 10.0, (i + 1) * 10.0)
        
        agent.initialize(world)
        
        # Enable social learning
        agent.enableRewardModulation(True)
        agent.enableSocialLearning(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
        worlds.append(world)
    
    # Multi-agent interaction simulation
    num_steps = 400
    interaction_metrics = []
    social_learning_events = []
    reward_distributions = []
    
    for step in range(num_steps):
        # Each agent processes sensory input
        for i, (agent, world) in enumerate(zip(agents, worlds)):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Social learning: observe other agents' actions
            for j, (other_agent, other_world) in enumerate(zip(agents, worlds)):
                if i != j:
                    # Observe other agent's action
                    other_action = other_agent.decodeMotorCommand()
                    
                    # Social learning: update beliefs based on other's success
                    if other_agent.getCuriosityLevel() > 0.3:
                        # Social learning event
                        social_learning_event = {
                            'observer': i,
                            'observed': j,
                            'observed_action': other_action,
                            'observed_success': other_world.getAgentBody().health > 50,
                            'step': step
                        }
                        social_learning_events.append(social_learning_event)
        
        # Each agent takes action based on learned behavior
        for i, (agent, world) in enumerate(zip(agents, worlds)):
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            percept = world.getSensoryPercept()
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            predicted_reward = agent.getPredictionError()
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track interaction metrics
            interaction_metrics.append({
                'step': step,
                'agent': i,
                'reward': reward,
                'dopamine': brains[i].getDopamine().getLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'social_learning_active': agent.isSocialLearningEnabled(),
                'population_health': world.getAgentBody().health
            })
        
        # Progress report
        if step % 80 == 0 and step > 0:
            print(f"\nStep {step}:")
            for i, (agent, world) in enumerate(zip(agents, worlds)):
                print(f"  Agent {i}: Reward {interaction_metrics[-1]['reward']:.2f}, "
                      f"Dopamine {brains[i].getDopamine().getLevel():.2f}, "
                      f"Curiosity {agent.getCuriosityLevel():.2f}, "
                      f"Health {world.getAgentBody().health:.1f}")
    
    # Social learning analysis
    print("\n=== SOCIAL LEARNING ANALYSIS ===")
    
    # Social learning event statistics
    social_learning_rate = len(social_learning_events) / max(1, num_steps * num_agents)
    successful_observations = sum(1 for event in social_learning_events if event['observed_success'])
    successful_imitation_rate = successful_observations / max(1, len(social_learning_events))
    
    print(f"Social learning events: {len(social_learning_events)}")
    print(f"Social learning rate: {social_learning_rate:.3f}")
    print(f"Successful observations: {successful_observations}")
    print(f"Successful imitation rate: {successful_imitation_rate:.3f}")
    
    # Collaborative behavior analysis
    final_rewards = [m['reward'] for m in interaction_metrics if m['step'] == num_steps - 1]
    avg_final_reward = np.mean(final_rewards)
    reward_variance = np.var(final_rewards)
    
    print(f"\nCollaborative behavior:")
    print(f"  Average final reward: {avg_final_reward:.2f}")
    print(f"  Reward variance across agents: {reward_variance:.3f}")
    print(f"  Reward inequality (Gini): {1 - 2 * (sum(final_rewards) - len(final_rewards) * min(final_rewards)) / (len(final_rewards) * sum(final_rewards)):.3f}")
    
    # Social network analysis
    agent_interactions = {}
    for event in social_learning_events:
        if event['observer'] not in agent_interactions:
            agent_interactions[event['observer']] = []
        agent_interactions[event['observer']].append(event['observed'])
    
    # Calculate social network metrics
    interaction_matrix = np.zeros((num_agents, num_agents))
    for observer, observed_list in agent_interactions.items():
        for observed in observed_list:
            interaction_matrix[observer][observed] = 1
    
    # Calculate social network properties
    total_interactions = np.sum(interaction_matrix)
    avg_interaction_degree = np.mean(np.sum(interaction_matrix, axis=1))
    network_density = total_interactions / (num_agents * (num_agents - 1))
    
    print(f"\nSocial network analysis:")
    print(f"  Total social interactions: {total_interactions}")
    print(f"  Average interaction degree: {avg_interaction_degree:.2f}")
    print(f"  Network density: {network_density:.3f}")
    
    return {
        'interaction_metrics': interaction_metrics,
        'social_learning_events': social_learning_events,
        'interaction_matrix': interaction_matrix,
        'social_network_metrics': {
            'total_interactions': total_interactions,
            'avg_interaction_degree': avg_interaction_degree,
            'network_density': network_density,
            'social_learning_rate': social_learning_rate,
            'successful_imitation_rate': successful_imitation_rate
        }
    }

# Main execution for research workflows
if __name__ == "__main__":
    print("NLM Research Workflow Examples")
    print("=" * 50)
    
    # Run memory formation study
    memory_results = memory_formation_study()
    
    # Run prediction error learning study  
    prediction_results = prediction_error_learning_study()
    
    # Run neuromodulation development study
    neuromodulation_results = neuromodulation_development_study()
    
    # Run advanced network analysis
    network_results = advanced_network_analysis()
    
    # Run multi-agent interaction study
    multi_agent_results = multi_agent_interaction_study()
    
    print("\n" + "=" * 50)
    print("All research workflow examples completed successfully!")
    print("\nExamples demonstrated:")
    print("✓ Memory formation and consolidation processes")
    print("✓ Prediction error-driven learning mechanisms")
    print("✓ Neuromodulation across developmental stages")
    print("✓ Advanced brain network topology analysis")
    print("✓ Multi-agent social learning and interaction")
    print("✓ Collaborative behavior and reward sharing")
    
    # Save results for further analysis
    import json
    with open("research_results.json", "w") as f:
        results = {
            'memory_formation': memory_results['final_statistics'],
            'prediction_learning': prediction_results['analysis'],
            'neuromodulation': neuromodulation_results['stage_averages'],
            'network_analysis': {
                'topology_metrics': {
                    'avg_clustering': network_results['topology'].getAverageClusteringCoefficient(),
                    'avg_path_length': network_results['topology'].getAveragePathLength(),
                    'network_efficiency': network_results['topology'].getNetworkEfficiency()
                },
                'performance_metrics': {
                    'efficiency': network_results['performance'].getSimulationEfficiency(),
                    'learning_speed': network_results['performance'].getLearningSpeed()
                }
            },
            'multi_agent': multi_agent_results['social_network_metrics']
        }
        json.dump(results, f, indent=2, default=str)
    
    print("Research results saved to research_results.json")
```

# Save to file
with open("examples/research_workflows.py", "w") as f:
    f.write("# NLM Research Workflow Examples\n\n")

print("Created comprehensive research workflow examples file")