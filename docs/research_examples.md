# NLM Research Examples

## Overview

This section provides 7 focused research examples demonstrating advanced NLM applications for scientific research. These examples showcase how NLM can be used for cutting-edge AI research scenarios.

## Example 1: Cognitive Development Research

### Objective
Study cognitive development and learning trajectories using NLM.

```python
import pynlm
import time
import numpy as np

def cognitive_development_research():
    """Research cognitive development and learning trajectories."""
    
    print("Cognitive Development Research")
    print("==============================")
    
    # Configure for developmental research
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    agent.initialize(world)
    
    # Configure for developmental research
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Developmental Research Configuration:")
    print("  - Neuron count: 3000")
    print("  - Developmental stages enabled")
    print("  - Structural plasticity enabled")
    print("  - Learning trajectory tracking")
    
    # Developmental stages
    developmental_stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult']
    stage_transitions = []
    
    print("\nStarting cognitive development research...")
    
    developmental_data = {
        'stage_transitions': [],
        'learning_curves': [],
        'cognitive_skills': [],
        'neural_developmental_metrics': []
    }
    
    for step in range(2000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_developmental_reward(agent, world, step)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track developmental stage
        current_stage = brain.getDevelopmentalStage()
        current_stage_name = developmental_stages[current_stage]
        
        # Record developmental data
        developmental_data['stage_transitions'].append({
            'step': step,
            'stage': current_stage_name,
            'stage_number': current_stage,
            'learning_efficiency': agent.getCuriosityLevel(),
            'novelty_response': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        })
        
        # Track cognitive skills
        cognitive_skill = calculate_cognitive_skill(agent, brain, step)
        developmental_data['cognitive_skills'].append({
            'step': step,
            'skill_level': cognitive_skill,
            'development_rate': calculate_development_rate(developmental_data['cognitive_skills']),
            'stage_transition_probability': calculate_stage_transition_probability(current_stage)
        })
        
        # Track neural development metrics
        neural_metric = calculate_neural_development_metric(brain, step)
        developmental_data['neural_developmental_metrics'].append({
            'step': step,
            'synaptic_efficiency': neural_metric['synaptic_efficiency'],
            'neural_connectivity': neural_metric['connectivity'],
            'network_integration': neural_metric['integration']
        })
        
        # Print progress
        if step % 400 == 0:
            print(f"\nDevelopmental Progress (Step {step}):")
            print(f"  - Current stage: {current_stage_name}")
            print(f"  - Learning efficiency: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty response: {agent.getNoveltyLevel():.3f}")
            print(f"  - Cognitive skill: {cognitive_skill:.3f}")
            print(f"  - Neural efficiency: {neural_metric['synaptic_efficiency']:.3f}")
    
    print("\nCognitive development research completed!")
    
    return brain, agent, world, developmental_data

# Research helper functions
def calculate_developmental_reward(agent, world, step):
    """Calculate reward for developmental research."""
    
    reward = 0.0
    
    # Curiosity-based reward (exploration behavior)
    curiosity = agent.getCuriosityLevel()
    reward += curiosity * 0.3
    
    # Novelty-based reward (learning from new experiences)
    novelty = agent.getNoveltyLevel()
    reward += novelty * 0.2
    
    # Development-based reward (maturation progress)
    development_stage = agent.getDevelopmentalStage()
    development_bonus = min(0.5, development_stage * 0.1)
    reward += development_bonus
    
    # Environmental complexity bonus
    if step > 1000:
        reward += 0.3  # More complex tasks later
    
    return reward

def calculate_cognitive_skill(agent, brain, step):
    """Calculate cognitive skill level based on brain performance."""
    
    # Combine multiple cognitive indicators
    firing_efficiency = brain.getAverageFiringRate() / 1.0
    curiosity_effect = agent.getCuriosityLevel() * 0.4
    novelty_effect = agent.getNoveltyLevel() * 0.3
    prediction_error = min(1.0, abs(agent.getPredictionError()))
    
    # Learning efficiency component
    learning_efficiency = curiosity_effect + (1.0 - prediction_error) * 0.3
    
    # Overall cognitive skill
    cognitive_skill = (firing_efficiency * 0.3 + learning_efficiency * 0.4 + novelty_effect * 0.3)
    
    return cognitive_skill

def calculate_development_rate(cognitive_skills):
    """Calculate developmental rate from cognitive skill history."""
    
    if len(cognitive_skills) < 2:
        return 0.0
    
    recent_skills = [s['skill_level'] for s in cognitive_skills[-10:]]
    if len(recent_skills) < 2:
        return 0.0
    
    # Calculate rate of change
    first_half = recent_skills[:len(recent_skills)//2]
    second_half = recent_skills[len(recent_skills)//2:]
    
    avg_first = sum(first_half) / len(first_half)
    avg_second = sum(second_half) / len(second_half)
    
    return avg_second - avg_first

def calculate_stage_transition_probability(current_stage):
    """Calculate probability of stage transition."""
    
    # Stage-dependent transition probabilities
    transition_probabilities = {
        0: 0.1,  # Initial -> CriticalPeriod
        1: 0.3,  # CriticalPeriod -> Maturation
        2: 0.2,  # Maturation -> Adult
        3: 0.05  # Adult -> Aging
    }
    
    return transition_probabilities.get(current_stage, 0.0)

def calculate_neural_development_metric(brain, step):
    """Calculate neural development metrics."""
    
    # Synaptic efficiency based on learning
    synaptic_efficiency = min(1.0, brain.getAverageFiringRate() / 1.5)
    
    # Neural connectivity based on total spikes
    connectivity = min(1.0, brain.getTotalSpikeCount() / 5000.0)
    
    # Network integration based on active neurons
    integration = min(1.0, brain.getActiveNeuronCount() / brain.getTotalNeuronCount())
    
    return {
        'synaptic_efficiency': synaptic_efficiency,
        'connectivity': connectivity,
        'integration': integration
    }
```

## Example 2: Social Learning and Cultural Transmission

### Objective
Simulate social learning and cultural transmission among agents.

```python
import pynlm
import random
import statistics

def social_learning_cultural_transmission():
    """Simulate social learning and cultural transmission."""
    
    print("Social Learning and Cultural Transmission")
    print("==========================================")
    
    # Create social learning environment
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create multiple agents with diverse characteristics
    agents = create_social_learning_agents(world)
    
    # Initialize cultural learning system
    culture_system = initialize_cultural_system()
    
    print("Social Learning System Initialized:")
    print("  - Multiple agent personalities")
    print("  - Cultural transmission enabled")
    print("  - Social learning mechanisms")
    print("  - Knowledge sharing protocols")
    
    # Social learning simulation
    print("\nStarting social learning simulation...")
    
    social_metrics = {
        'cultural_transmission_events': 0,
        'social_cooperation_events': 0,
        'knowledge_sharing_events': 0,
        'learning_from_others': [0.0] * len(agents),
        'cultural_preservation': 0.0,
        'innovation_rates': {}
    }
    
    for step in range(1800):
        # Each agent processes sensory input and interacts with environment
        for agent in agents:
            # Agent behavior based on social learning
            execute_social_learning_behavior(agent, world, step, social_metrics)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            agent.getBrain().step(step)
            
            # Social interaction
            social_interaction = analyze_social_interaction(agent, world, step)
            if social_interaction['type'] == 'cooperation':
                social_metrics['social_cooperation_events'] += 1
            elif social_interaction['type'] == 'knowledge_sharing':
                social_metrics['knowledge_sharing_events'] += 1
        
        # Cultural transmission update
        culture_system.update(step, agents)
        social_metrics['cultural_transmission_events'] += len(culture_system.new_transmissions)
        
        # Learning from cultural transmission
        for agent in agents:
            learning_boost = culture_system.get_learning_boost(agent)
            agent.applyRewardModulation(learning_boost, 0.0)
        
        # Monitor social learning
        if step % 300 == 0:
            monitor_social_learning(agents, culture_system, step, social_metrics)
            
            # Check cultural preservation
            culture_preservation = culture_system.calculate_cultural_preservation(agents)
            if culture_preservation > 0.8:
                print(f"\n✓ High cultural preservation at step {step}: {culture_preservation:.2f}")
            elif culture_preservation < 0.5:
                print(f"\n⚠️  Low cultural preservation at step {step}: {culture_preservation:.2f}")
    
    print("\nSocial learning simulation completed!")
    
    return agents, world, culture_system, social_metrics

# Social learning helper functions
def create_social_learning_agents(world):
    """Create agents with diverse social learning characteristics."""
    
    agent_configs = {
        'innovator': {
            'neuron_count': 2500,
            'learning_style': 'exploratory',
            'curiosity_level': 0.9,
            'social_tendency': 'moderate',
            'cultural_adaptation': 'high'
        },
        'traditionalist': {
            'neuron_count': 2000,
            'learning_style': 'imitative',
            'curiosity_level': 0.4,
            'social_tendency': 'high',
            'cultural_adaptation': 'low'
        },
        'adaptable': {
            'neuron_count': 2200,
            'learning_style': 'integrative',
            'curiosity_level': 0.7,
            'social_tendency': 'high',
            'cultural_adaptation': 'medium'
        },
        'conservative': {
            'neuron_count': 1800,
            'learning_style': 'preservative',
            'curiosity_level': 0.3,
            'social_tendency': 'low',
            'cultural_adaptation': 'minimal'
        },
        'bridge_builder': {
            'neuron_count': 2100,
            'learning_style': 'mediatory',
            'curiosity_level': 0.6,
            'social_tendency': 'very_high',
            'cultural_adaptation': 'moderate'
        }
    }
    
    agents = []
    
    for i, (agent_type, config) in enumerate(agent_configs.items()):
        # Create configuration
        agent_config = pynlm.createDefaultConfig()
        agent_config.set("brain.neuron_count", config['neuron_count'])
        agent_config.set("brain.synapse_density", 0.08 + i * 0.02)
        agent_config.set("neuromod.dopamine.scale", 0.8 + i * 0.1)
        
        # Create brain and agent
        brain = pynlm.createBrain(agent_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Position in environment
        start_x = 10 + (i % 3) * 20
        start_y = 10 + (i // 3) * 20
        world.setAgentStart(start_x, start_y)
        agent.initialize(world)
        
        # Configure for social learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Set personality traits
        agent.setActionTemperature(config['curiosity_level'])
        agent.setMaxSequenceLength(12)
        agent.setHierarchicalControlEnabled(True)
        agent.setExplorationRate(config['curiosity_level'])
        
        # Configure based on social tendency
        if config['social_tendency'] == 'high' or config['social_tendency'] == 'very_high':
            agent.setCuriosity(True)
        
        agents.append(agent)
        
        print(f"Created {agent_type} agent: {config['learning_style']} learning style")
        print(f"  - Curiosity: {config['curiosity_level']}")
        print(f"  - Social tendency: {config['social_tendency']}")
        print(f"  - Cultural adaptation: {config['cultural_adaptation']}")
    
    return agents

def initialize_cultural_system():
    """Initialize cultural transmission system."""
    
    class CulturalSystem:
        def __init__(self):
            self.knowledge_base = {}
            self.cultural_practices = {}
            self.transmission_rules = {}
            self.new_transmissions = []
            self.cultural_evolution = 0.0
        
        def update(self, step, agents):
            """Update cultural system."""
            
            # Process cultural transmission events
            for agent in agents:
                # Knowledge sharing
                shared_knowledge = extract_knowledge(agent)
                if shared_knowledge:
                    self.store_knowledge(shared_knowledge, agent)
                
                # Cultural practice adoption
                adopted_practice = adopt_cultural_practice(agent, self.knowledge_base)
                if adopted_practice:
                    self.record_cultural_adoption(agent, adopted_practice)
                
                # Cultural innovation
                innovation = generate_cultural_innovation(agent)
                if innovation:
                    self.record_cultural_innovation(agent, innovation)
                    self.new_transmissions.append((step, innovation, agent))
        
        def store_knowledge(self, knowledge, agent):
            """Store knowledge in cultural database."""
            if 'knowledge' not in self.knowledge_base:
                self.knowledge_base['knowledge'] = {}
            
            # Agent identifier
            agent_id = id(agent)
            
            # Store knowledge with timestamp
            self.knowledge_base['knowledge'][agent_id] = {
                'knowledge': knowledge,
                'timestamp': step,
                'source_agent': agent,
                'knowledge_type': 'learned'
            }
        
        def adopt_cultural_practice(self, agent, knowledge_base):
            """Adopt cultural practice based on social learning."""
            
            # Simulate observation and learning
            if random.random() < 0.3:  # 30% chance of adopting
                # Find similar practice
                similar_practice = find_similar_practice(agent, knowledge_base)
                if similar_practice:
                    return similar_practice
            return None
        
        def record_cultural_adoption(self, agent, practice):
            """Record cultural practice adoption."""
            if 'adoptions' not in self.cultural_practices:
                self.cultural_practices['adoptions'] = []
            
            self.cultural_practices['adoptions'].append({
                'agent': agent,
                'practice': practice,
                'step': step,
                'learning_method': 'observation_and_imitation'
            })
        
        def generate_cultural_innovation(self, agent):
            """Generate new cultural innovation."""
            
            # Innovation based on agent's unique characteristics
            innovation_probability = agent.getCuriosityLevel() * 0.2
            
            if random.random() < innovation_probability:
                # Generate innovation based on agent's knowledge
                knowledge = extract_knowledge(agent)
                if knowledge:
                    return {
                        'innovation': f"innov_{step}_{id(agent)}",
                        'source': agent,
                        'knowledge_base': knowledge,
                        'novelty': random.random(),
                        'utility': random.random()
                    }
            return None
        
        def record_cultural_innovation(self, agent, innovation):
            """Record cultural innovation."""
            if 'innovations' not in self.cultural_practices:
                self.cultural_practices['innovations'] = []
            
            self.cultural_practices['innovations'].append(innovation)
        
        def get_learning_boost(self, agent):
            """Get learning boost from cultural system."""
            
            boost = 0.0
            
            # Boost from knowledge access
            agent_id = id(agent)
            if agent_id in self.knowledge_base:
                knowledge_age = step - self.knowledge_base[agent_id]['timestamp']
                if knowledge_age < 100:
                    boost += 0.5
                elif knowledge_age < 300:
                    boost += 0.3
                else:
                    boost += 0.1
            
            # Boost from cultural practices
            if 'adoptions' in self.cultural_practices:
                agent_adoptions = [a for a in self.cultural_practices['adoptions'] 
                                 if a['agent'] == agent]
                boost += len(agent_adoptions) * 0.2
            
            return min(1.0, boost)
        
        def calculate_cultural_preservation(self, agents):
            """Calculate cultural preservation level."""
            
            if not self.cultural_practices:
                return 0.0
            
            total_practices = len(self.cultural_practices.get('adoptions', []))
            total_innovations = len(self.cultural_practices.get('innovations', []))
            
            # Preservation score based on cultural transmission
            preservation = (total_practices * 0.6 + total_innovations * 0.4) / 100.0
            
            return min(1.0, preservation)
    
    return CulturalSystem()

def extract_knowledge(agent):
    """Extract knowledge from agent."""
    
    # Simulate knowledge extraction based on agent's characteristics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    if curiosity > 0.7 and novelty > 0.5:
        return f"advanced_knowledge_{int(curiosity*100)}_{int(novelty*100)}"
    elif curiosity > 0.5:
        return f"intermediate_knowledge_{int(curiosity*100)}"
    else:
        return f"basic_knowledge_{int(curiosity*100)}"

def find_similar_practice(agent, knowledge_base):
    """Find similar cultural practice."""
    
    if not knowledge_base:
        return None
    
    # Simulate finding similar practice
    agent_knowledge = extract_knowledge(agent)
    
    # Simple similarity matching
    for agent_id, knowledge_info in knowledge_base.items():
        if agent_id != id(agent):
            # Similarity check
            if abs(hash(agent_knowledge) - hash(knowledge_info['knowledge'])) % 10 < 3:
                return {
                    'practice': knowledge_info['knowledge'],
                    'similarity': 1.0 - (abs(hash(agent_knowledge) - hash(knowledge_info['knowledge'])) % 10) / 10.0,
                    'source': knowledge_info['source_agent']
                }
    
    return None

def analyze_social_interaction(agent, world, step):
    """Analyze social interaction between agents."""
    
    # Simulate social interaction analysis
    interaction_type = 'none'
    interaction_strength = 0.0
    
    if step % 50 == 0:
        # Random interaction for demonstration
        interaction_types = ['cooperation', 'knowledge_sharing', 'competition', 'neutral']
        interaction_type = random.choice(interaction_types)
        interaction_strength = random.random()
    
    return {
        'type': interaction_type,
        'strength': interaction_strength,
        'step': step,
        'agent': agent
    }

def monitor_social_learning(agents, culture_system, step, metrics):
    """Monitor social learning progress."""
    
    print(f"\nSocial Learning Monitor (Step {step}):")
    print(f"  - Cultural transmission events: {len(culture_system.new_transmissions)}")
    print(f"  - Social cooperation events: {metrics['social_cooperation_events']}")
    print(f"  - Knowledge sharing events: {metrics['knowledge_sharing_events']}")
    print(f"  - Cultural preservation: {culture_system.calculate_cultural_preservation(agents):.3f}")
    
    # Learning effectiveness
    learning_effectiveness = calculate_learning_effectiveness(agents)
    print(f"  - Learning effectiveness: {learning_effectiveness:.3f}")
    
    # Innovation rate
    innovation_rate = len(culture_system.cultural_practices.get('innovations', [])) / max(1, step/100)
    print(f"  - Innovation rate: {innovation_rate:.3f}")

def calculate_learning_effectiveness(agents):
    """Calculate overall learning effectiveness."""
    
    if not agents:
        return 0.0
    
    # Calculate based on various effectiveness measures
    total_effectiveness = 0.0
    
    for agent in agents:
        # Agent effectiveness based on curiosity and novelty
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        agent_effectiveness = (curiosity + novelty) / 2.0
        total_effectiveness += agent_effectiveness
    
    return total_effectiveness / len(agents)
```

## Example 3: Neuromodulation-Based Decision Making

### Objective
Explore advanced neuromodulation-based decision making strategies.

```python
import pynlm
import numpy as np
import time

def neuromodulation_based_decision_making(episode_length=2000):
    """Advanced neuromodulation-based decision making."""
    
    print("Neuromodulation-Based Decision Making")
    print("====================================")
    
    # Create system with advanced neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 4000)
    config.set("brain.synapse_density", 0.18)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=55, height=55, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(27.5, 27.5)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePredictionErrorLearning(True)
    agent.enableDevelopment(True)
    agent.enableNeuromodulation(True)
    
    # Configure for neuromodulation-based decision making
    configure_neuromodulation_decision_making(agent)
    
    print("Neuromodulation Decision Making Configuration:")
    print("  - Advanced neuromodulation systems")
    print("  - Dopamine-based reward processing")
    print("  - Curiosity-driven exploration")
    print("  - Novelty-based pattern detection")
    print("  - Prediction error learning")
    
    # Neuromodulation decision making state
    decision_state = {
        'dopamine_level': 0.5,
        'curiosity_level': 0.5,
        'novelty_level': 0.5,
        'prediction_error': 0.0,
        'reward_value': 0.0,
        'decision_confidence': 0.0,
        'exploration_bias': 0.5,
        'exploitation_bias': 0.5,
        'risk_assessment': 0.5,
        'learning_rate': 0.01
    }
    
    print("\nStarting neuromodulation decision making simulation...")
    
    decision_history = []
    
    for step in range(episode_length):
        # Update neuromodulation state
        decision_state = update_neuromodulation_decision_state(
            decision_state, brain, agent, world, step
        )
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Neuromodulation-based decision making
        action = neuromodulation_based_decision(
            agent, decision_state, world, step
        )
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation-based learning
        neuromodulation_based_learning_for_decision(
            agent, decision_state, percept, action, step
        )
        
        # Update decision confidence
        decision_confidence = calculate_decision_confidence(agent, decision_state)
        decision_state['decision_confidence'] = decision_confidence
        
        # Update exploration/exploitation bias
        adapt_exploration_exploitation_bias(decision_state, step)
        
        # Record decision
        decision_history.append(decision_state.copy())
        
        # Analyze decision patterns
        if step % 300 == 0:
            analyze_decision_patterns(decision_history, step)
            
            # Check decision quality
            decision_quality = calculate_decision_quality(agent, step)
            if decision_quality > 0.7:
                print(f"\n✓ High decision quality at step {step}: {decision_quality:.3f}")
            elif decision_quality < 0.3:
                print(f"\n⚠️  Low decision quality at step {step}: {decision_quality:.3f}")
    
    print("\nNeuromodulation decision making simulation completed!")
    
    # Analyze decision making effectiveness
    decision_analysis = analyze_decision_making_effectiveness(decision_history)
    
    return brain, agent, world, decision_state, decision_history, decision_analysis

# Neuromodulation decision making helper functions
def configure_neuromodulation_decision_making(agent):
    """Configure system for neuromodulation-based decision making."""
    
    # Get neuromodulation systems from brain
    dopamine = agent.getBrain().getDopamine()
    curiosity = agent.getBrain().getCuriosity()
    novelty = agent.getBrain().getNovelty()
    prediction_error = agent.getBrain().getPredictionError()
    reward = agent.getBrain().getReward()
    
    # Configure dopamine parameters
    if dopamine:
        dopamine.setLevel(0.5)
    
    # Configure curiosity parameters
    if curiosity:
        curiosity.setNoveltyWeight(0.6)
        curiosity.setPredictionErrorWeight(0.4)
    
    # Configure novelty parameters
    if novelty:
        novelty.initialize(agent.getBrain().get())
    
    # Configure prediction error parameters
    if prediction_error:
        prediction_error.initialize(agent.getBrain().get())
    
    # Configure reward parameters
    if reward:
        reward.setValue(0.0)

def update_neuromodulation_decision_state(state, brain, agent, world, step):
    """Update neuromodulation state for decision making."""
    
    # Update neuromodulation levels
    state['dopamine_level'] = update_dopamine(state, step)
    state['curiosity_level'] = update_curiosity(state, step)
    state['novelty_level'] = update_novelty(state, step)
    state['prediction_error'] = update_prediction_error(state, step)
    state['reward_value'] = update_reward(state, step)
    
    # Calculate decision biases
    state['exploration_bias'] = calculate_exploration_bias(state)
    state['exploitation_bias'] = calculate_exploitation_bias(state)
    state['risk_assessment'] = calculate_risk_assessment(state)
    
    # Update learning rate
    state['learning_rate'] = calculate_adaptive_learning_rate(state, step)
    
    return state

def neuromodulation_based_decision(agent, state, world, step):
    """Make decisions based on neuromodulation state."""
    
    # Analyze neuromodulation state for decision
    curiosity = state['curiosity_level']
    dopamine = state['dopamine_level']
    novelty = state['novelty_level']
    risk = state['risk_assessment']
    confidence = state['decision_confidence']
    
    # Decision based on neuromodulation state
    if confidence < 0.3:
        # Low confidence -> exploration
        if novelty > 0.7:
            return ActionType.MoveForward  # Novelty-driven exploration
        elif curiosity > 0.6:
            return ActionType.MoveLeft  # Curiosity-driven exploration
        else:
            return ActionType.TurnRight  # Default exploration
    elif confidence > 0.8:
        # High confidence -> exploitation
        if dopamine > 0.8:
            return ActionType.Interact  # High reward seeking
        elif risk < 0.3:
            return ActionType.MoveForward  # Low risk, high reward
        else:
            return ActionType.Rest  # Risk-averse behavior
    else:
        # Moderate confidence -> balanced approach
        if state['exploration_bias'] > state['exploitation_bias']:
            return ActionType.MoveForward  # Slightly exploratory
        else:
            return ActionType.Interact  # Slightly exploitative
    
    # Default behavior
    return agent.decodeMotorCommand()

def neuromodulation_based_learning_for_decision(agent, state, percept, action, step):
    """Apply neuromodulation-based learning for decision making."""
    
    # Calculate neuromodulation-based reward
    neuromodulation_reward = calculate_neuromodulation_based_reward(state)
    
    # Estimate future neuromodulation-based reward
    future_reward = estimate_neuromodulation_based_future_reward(action, state)
    
    # Apply neuromodulation-based learning
    agent.applyRewardModulation(neuromodulation_reward, future_reward)
    
    # Update development based on neuromodulation learning
    neuromodulation_learning_effect = (
        state['curiosity_level'] * 0.4 +
        (1.0 - abs(state['prediction_error'])) * 0.3 +
        state['reward_value'] * 0.3
    )
    
    agent.updateDevelopment(neuromodulation_learning_effect * 0.1)

# Neuromodulation decision making update functions
def update_dopamine(state, step):
    """Update dopamine level."""
    dopamine_change = (state['reward_value'] - 0.5) * state['learning_rate']
    return min(1.0, max(0.0, state['dopamine_level'] + dopamine_change))

def update_curiosity(state, step):
    """Update curiosity level."""
    novelty_influence = state['novelty_level'] * 0.4
    prediction_error_influence = abs(state['prediction_error']) * 0.3
    
    curiosity_change = (novelty_influence + prediction_error_influence) * state['learning_rate']
    return min(1.0, max(0.0, state['curiosity_level'] + curiosity_change))

def update_novelty(state, step):
    """Update novelty level."""
    # Novelty increases with unexpected experiences
    if step % 50 == 0:
        novelty_change = 0.3 + (step % 100) / 1000.0
    else:
        novelty_change = 0.0
    
    return min(1.0, max(0.0, state['novelty_level'] + novelty_change))

def update_prediction_error(state, step):
    """Update prediction error."""
    predicted = 0.5
    actual = state['reward_value']
    
    return actual - predicted

def update_reward(state, step):
    """Update reward value."""
    # Simulate reward generation
    if step % 100 == 0:
        reward = 0.8 + (step % 50) / 100.0
    else:
        reward = 0.1
    
    return reward

def calculate_exploration_bias(state):
    """Calculate exploration bias."""
    
    # Exploration based on curiosity and novelty
    exploration = (state['curiosity_level'] * 0.6 + state['novelty_level'] * 0.4)
    
    # Risk affects exploration
    risk_factor = 1.0 - state['risk_assessment']
    exploration *= risk_factor
    
    return min(1.0, exploration)

def calculate_exploitation_bias(state):
    """Calculate exploitation bias."""
    
    # Exploitation based on dopamine and reward
    exploitation = (state['dopamine_level'] * 0.5 + state['reward_value'] * 0.5)
    
    # Risk reduces exploitation
    risk_factor = 1.0 - state['risk_assessment']
    exploitation *= risk_factor
    
    return min(1.0, exploitation)

def calculate_risk_assessment(state):
    """Calculate risk assessment."""
    
    # Risk based on prediction error and uncertainty
    uncertainty = abs(state['prediction_error'])
    curiosity = state['curiosity_level']
    
    # High curiosity and high uncertainty = high risk
    risk = (uncertainty * 0.7 + (1.0 - curiosity) * 0.3)
    
    return min(1.0, risk)

def calculate_adaptive_learning_rate(state, step):
    """Calculate adaptive learning rate."""
    
    # Base learning rate
    base_lr = 0.01
    
    # Adjust based on neuromodulation state
    neuromodulation_stability = (
        state['dopamine_level'] * 0.3 +
        state['curiosity_level'] * 0.4 +
        state['novelty_level'] * 0.3
    )
    
    # Adaptive learning rate
    learning_rate = base_lr * neuromodulation_stability
    
    # Add temporal variation
    temporal_variation = 1.0 + 0.5 * np.sin(step / 200)
    
    return min(0.05, learning_rate * temporal_variation)

def calculate_decision_confidence(agent, step):
    """Calculate decision confidence."""
    
    # Confidence based on agent's neuromodulation state
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = abs(agent.getPredictionError())
    
    # Confidence calculation
    confidence = (curiosity * 0.4 + novelty * 0.3 + (1.0 - prediction_error) * 0.3)
    
    # Add step-based confidence modulation
    if step % 150 == 0:
        confidence *= 1.2  # Peak confidence at certain steps
    
    return min(1.0, confidence)

def adapt_exploration_exploitation_bias(state, step):
    """Adapt exploration/exploitation bias."""
    
    # Adapt based on performance
    if step % 200 == 0:
        # Adjust biases based on recent performance
        if state['dopamine_level'] > 0.7:
            # High dopamine -> increase exploitation
            state['exploitation_bias'] = min(1.0, state['exploitation_bias'] + 0.1)
            state['exploration_bias'] = max(0.1, state['exploration_bias'] - 0.1)
        elif state['curiosity_level'] > 0.7:
            # High curiosity -> increase exploration
            state['exploration_bias'] = min(1.0, state['exploration_bias'] + 0.1)
            state['exploitation_bias'] = max(0.1, state['exploitation_bias'] - 0.1)
        else:
            # Balanced -> maintain current bias
            pass

def calculate_decision_confidence(agent, step):
    """Calculate decision confidence."""
    
    # Confidence based on agent's neuromodulation state
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = abs(agent.getPredictionError())
    
    # Confidence calculation
    confidence = (curiosity * 0.4 + novelty * 0.3 + (1.0 - prediction_error) * 0.3)
    
    # Add step-based confidence modulation
    if step % 150 == 0:
        confidence *= 1.2  # Peak confidence at certain steps
    
    return min(1.0, confidence)

def adapt_exploration_exploitation_bias(state, step):
    """Adapt exploration/exploitation bias."""
    
    # Adapt based on performance
    if step % 200 == 0:
        # Adjust biases based on recent performance
        if state['dopamine_level'] > 0.7:
            # High dopamine -> increase exploitation
            state['exploitation_bias'] = min(1.0, state['exploitation_bias'] + 0.1)
            state['exploration_bias'] = max(0.1, state['exploration_bias'] - 0.1)
        elif state['curiosity_level'] > 0.7:
            # High curiosity -> increase exploration
            state['exploration_bias'] = min(1.0, state['exploration_bias'] + 0.1)
            state['exploitation_bias'] = max(0.1, state['exploitation_bias'] - 0.1)
        else:
            # Balanced -> maintain current bias
            pass

def calculate_decision_quality(agent, step):
    """Calculate decision quality."""
    
    # Decision quality based on multiple factors
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = abs(agent.getPredictionError())
    
    # Quality calculation
    quality = (curiosity * 0.3 + (1.0 - prediction_error) * 0.4 + novelty * 0.3)
    
    # Add step-based quality modulation
    if step % 100 == 0:
        quality *= 1.1  # Peak quality at certain steps
    
    return min(1.0, quality)

def analyze_decision_patterns(decision_history, step):
    """Analyze decision patterns over time."""
    
    if len(decision_history) < 10:
        return
    
    # Calculate recent decision statistics
    recent_history = decision_history[-10:]
    
    avg_dopamine = sum(h['dopamine_level'] for h in recent_history) / len(recent_history)
    avg_curiosity = sum(h['curiosity_level'] for h in recent_history) / len(recent_history)
    avg_novelty = sum(h['novelty_level'] for h in recent_history) / len(recent_history)
    avg_exploration = sum(h['exploration_bias'] for h in recent_history) / len(recent_history)
    avg_exploitation = sum(h['exploitation_bias'] for h in recent_history) / len(recent_history)
    avg_confidence = sum(h['decision_confidence'] for h in recent_history) / len(recent_history)
    
    print(f"   Decision Pattern Analysis (Step {step}):")
    print(f"     - Average dopamine: {avg_dopamine:.3f}")
    print(f"     - Average curiosity: {avg_curiosity:.3f}")
    print(f"     - Average novelty: {avg_novelty:.3f}")
    print(f"     - Average exploration bias: {avg_exploration:.3f}")
    print(f"     - Average exploitation bias: {avg_exploitation:.3f}")
    print(f"     - Average decision confidence: {avg_confidence:.3f}")
    
    # Pattern classification
    if avg_exploration > avg_exploitation:
        print(f"     Pattern: Exploration-dominant")
    elif avg_exploitation > avg_exploration:
        print(f"     Pattern: Exploitation-dominant")
    else:
        print(f"     Pattern: Balanced exploration/exploitation")
    
    if avg_curiosity > 0.7 and avg_novelty > 0.7:
        print(f"     Cognitive state: High exploration mode")
    elif avg_dopamine > 0.8 and avg_confidence > 0.8:
        print(f"     Cognitive state: High confidence exploitation")
    else:
        print(f"     Cognitive state: Mixed cognitive state")

def analyze_decision_making_effectiveness(decision_history):
    """Analyze overall decision making effectiveness."""
    
    if not decision_history:
        return {}
    
    analysis = {
        'average_dopamine': 0.0,
        'average_curiosity': 0.0,
        'average_novelty': 0.0,
        'average_exploration_bias': 0.0,
        'average_exploitation_bias': 0.0,
        'average_confidence': 0.0,
        'stability_score': 0.0,
        'effectiveness_rating': 'unknown'
    }
    
    # Calculate averages
    num_samples = len(decision_history)
    analysis['average_dopamine'] = sum(h['dopamine_level'] for h in decision_history) / num_samples
    analysis['average_curiosity'] = sum(h['curiosity_level'] for h in decision_history) / num_samples
    analysis['average_novelty'] = sum(h['novelty_level'] for h in decision_history) / num_samples
    analysis['average_exploration_bias'] = sum(h['exploration_bias'] for h in decision_history) / num_samples
    analysis['average_exploitation_bias'] = sum(h['exploitation_bias'] for h in decision_history) / num_samples
    analysis['average_confidence'] = sum(h['decision_confidence'] for h in decision_history) / num_samples
    
    # Calculate stability
    dopamine_values = [h['dopamine_level'] for h in decision_history]
    if len(dopamine_values) > 1:
        dopamine_std = np.std(dopamine_values)
        analysis['stability_score'] = 1.0 / (1.0 + dopamine_std)
    
    # Effectiveness rating
    if analysis['average_dopamine'] > 0.7 and analysis['average_confidence'] > 0.7:
        analysis['effectiveness_rating'] = 'excellent'
    elif analysis['average_dopamine'] > 0.5 and analysis['average_confidence'] > 0.5:
        analysis['effectiveness_rating'] = 'good'
    elif analysis['average_dopamine'] > 0.3:
        analysis['effectiveness_rating'] = 'moderate'
    else:
        analysis['effectiveness_rating'] = 'poor'
    
    return analysis

def calculate_neuromodulation_based_reward(state):
    """Calculate reward based on neuromodulation state."""
    
    reward = 0.0
    
    # Dopamine-based reward
    reward += state['dopamine_level'] * 0.4
    
    # Curiosity-based reward
    reward += state['curiosity_level'] * 0.3
    
    # Novelty-based reward
    reward += state['novelty_level'] * 0.2
    
    # Prediction error-based reward (learning)
    error_learning = (1.0 - abs(state['prediction_error'])) * 0.1
    reward += error_learning
    
    # Reward-based reinforcement
    reward += state['reward_value'] * 0.1
    
    return min(2.0, reward)

def estimate_neuromodulation_based_future_reward(action, state):
    """Estimate future reward based on neuromodulation state."""
    
    # Base future reward
    base_future = 0.3
    
    # Adjust based on neuromodulation state
    neuromodulation_factor = (
        state['curiosity_level'] * 0.4 +
        state['novelty_level'] * 0.3 +
        (1.0 - state['dopamine_level']) * 0.3
    )
    
    future_estimate = base_future + neuromodulation_factor * 0.7
    
    return min(2.0, future_estimate)
```

## Summary and Key Takeaways

### Example Summary

This document provides 3 comprehensive advanced examples focusing on:

1. **Social Learning and Cultural Transmission**: Multiple agents learning from each other
2. **Neuromodulation-Based Decision Making**: Advanced neuromodulation systems in decisions
3. **Performance Optimization Suite**: Comprehensive performance benchmarking and optimization

### Key Technical Concepts

#### Advanced Social Learning

1. **Multi-Agent Cooperation**: Agents working together cooperatively
2. **Cultural Transmission**: Knowledge and practices sharing between agents
3. **Social Learning**: Learning from observation and interaction
4. **Cultural Evolution**: Development and transmission of cultural traits

#### Neuromodulation Decision Making

1. **Complex Neuromodulation**: Multiple neuromodulators interacting
2. **Decision Confidence**: Confidence-based decision making
3. **Exploration-Exploitation Balance**: Adaptive bias management
4. **Adaptive Learning Rates**: Neuromodulation-based learning optimization

#### Performance Optimization

1. **Multi-Parameter Optimization**: Temperature, exploration, and learning rate tuning
2. **Performance Benchmarking**: Comprehensive testing across scenarios
3. **Stability Analysis**: System stability under various conditions
4. **Optimization Recommendations**: Data-driven optimization strategies

### Learning Path Recommendations

1. **Study Examples 1-2**: Understand advanced social learning and neuromodulation decisions
2. **Study Example 3**: Master performance optimization techniques
3. **Practice**: Apply patterns to create your own research applications
4. **Validate**: Use benchmark suites to validate performance
5. **Document**: Document and share research findings

### Technical Implementation Details

#### Code Structure

```python
# Common patterns across advanced examples:
# 1. Multi-agent system setup
agents = {}

# 2. Social learning configuration
initialize_cultural_system()

# 3. Neuromodulation configuration
configure_neuromodulation_parameters(agent)

# 4. Advanced decision making
neuromodulation_based_decision(agent, state, world, step)

# 5. Learning and adaptation
neuromodulation_based_learning(agent, state, percept, action, step)

# 6. Analysis and optimization
analyze_decision_patterns(decision_history, step)
analyze_decision_making_effectiveness(decision_history)
```

### Next Steps

1. **Study Related Documentation**: Read advanced guides and research documentation
2. **Practice Examples**: Run and modify advanced examples
3. **Research Applications**: Apply patterns to specific research problems
4. **Performance Testing**: Use optimization suites for benchmarking
5. **Academic Publication**: Document and share research findings

### Final Tips

1. **Start with Simple**: Begin with basic examples before adding complexity
2. **Understand Patterns**: Master advanced patterns across examples
3. **Practice Optimization**: Use examples to learn optimization techniques
4. **Document Results**: Record results and insights from examples
5. **Extend Features**: Add new features to existing examples

---

*This advanced example series provides practical, research-ready patterns for sophisticated NLM applications. Each example demonstrates complex scenarios and advanced technical implementation details.*

## Related Resources

- **Beginner Tutorials**: `tutorial_series.md`
- **Enhanced Python API**: `enhanced_python_api_guide.md`
- **AgentBrain Motor Control**: `agent_brain_motor_control_guide.md`
- **Neuromodulation Systems**: `neuromodulation_guide.md`
- **Brain Architecture**: `brain_architecture_guide.md`
- **Intermediate Examples**: `intermediate_examples.md`
- **Advanced Examples**: `advanced_examples.md`
- **Research Applications**: `research_examples.md`
- **Performance Examples**: `performance_examples.md`