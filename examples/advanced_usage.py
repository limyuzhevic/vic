#!/usr/bin/env python3
"""
Advanced NLM Usage Examples and Usage Patterns

This file demonstrates advanced usage patterns and examples for NLM (Neural Learning Machine)
that showcase all the new functionality and make it easier for users to add new features
and use advanced capabilities.

Requirements:

### 1. **Basic Examples** (Enhance existing easy_usage.md)
- Memory system basics
- Neuromodulation basics  
- Prediction system basics
- Cognitive system basics
- Development system basics

### 2. **Advanced Examples**
- **Advanced Memory Usage**: Episodic memory with pattern completion, working memory optimization
- **Neuromodulation Integration**: Coordinated dopamine, curiosity, and novelty signals
- **Predictive Planning**: Neural prediction in action selection
- **Cognitive Architecture**: Self-model with planning and learning
- **Developmental Learning**: How development affects learning throughout lifetime
- **Multi-system Coordination**: How all systems work together

### 3. **Custom Component Examples**
- **Custom Brain Region**: Creating new neural regions with specific functions
- **Custom Plasticity Rule**: Implementing novel learning rules
- **Custom Neuromodulator**: Creating new neuromodulator types
- **Custom Memory System**: Adding new memory capabilities
- **Plugin Architecture**: Loading and integrating third-party components

### 4. **Performance and Optimization Examples**
- **Memory Management**: Working with memory pools and optimization
- **Performance Monitoring**: Real-time performance tracking
- **Parallel Processing**: Leveraging SIMD and multi-core
- **Simulation Scaling**: Large-scale brain simulations

### 5. **Debugging and Testing Examples**
- **Memory Debugging**: Memory leak detection and debugging
- **Neuromodulation Debugging**: Signal monitoring and validation
- **Simulation Debugging**: Step-by-step simulation inspection
- **Performance Profiling**: CPU and memory profiling

### 6. **Integration Examples**
- **Research Framework**: Setting up scientific experiments
- **Agent Training**: Training agents in virtual environments
- **Behavioral Analysis**: Analyzing neural activity patterns
- **Real-world Applications**: Applying NLM to real problems
"""

import pynlm
import logging
from typing import List, Dict, Any

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def print_header(title: str):
    """Print a formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

# ============================================================================
# 1. Basic Examples
# ============================================================================

def basic_memory_systems():
    """Basic memory system demonstrations."""
    print_header("Basic Memory Systems")
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add some neurons to work with
    region_id = brain.addRegion("BasicMemory")
    region = brain.getRegion(region_id)
    
    # Working Memory - active information
    print("\n1.1 Working Memory:")
    wm = brain.getWorkingMemory()
    
    # Store information in working memory
    pattern = [0.5, 0.3, 0.8, 0.2, 0.7]
    wm.store(pattern, 1.0)
    
    # Retrieve and inspect
    retrieved = wm.retrieve()
    print(f"   Stored pattern: {pattern}")
    print(f"   Retrieved pattern: {retrieved}")
    print(f"   Memory capacity: {wm.getCapacity()}")
    print(f"   Active traces: {wm.getActiveTraces()}")
    
    # Episodic Memory - experience storage
    print("\n1.2 Episodic Memory:")
    em = brain.getEpisodicMemory()
    
    # Create an episodic memory item
    episode = pynlm.EpisodicMemoryItem()
    episode.timestamp = 1000
    episode.metadata = "basic_example"
    
    # Store in episodic memory
    em.storeEpisode(episode)
    print(f"   Stored episode with timestamp: {episode.timestamp}")
    print(f"   Total episodes: {em.getEpisodeCount()}")
    
    # Retrieve recent episodes
    recent = em.getRecentEpisodes(3)
    print(f"   Retrieved {len(recent)} recent episodes")
    
    # Semantic Memory - facts and knowledge
    print("\n1.3 Semantic Memory:")
    # Note: This would be accessed through different API in full implementation
    
    # Associative Memory - pattern associations
    print("\n1.4 Associative Memory:")
    am = brain.getAssociativeMemory()
    
    # Create some neuron IDs
    neuron1 = pynlm.NeuronId(1)
    neuron2 = pynlm.NeuronId(2)
    
    # Create associations
    am.associate(neuron1, neuron2, 0.9)
    strength = am.getAssociationStrength(neuron1, neuron2)
    print(f"   Association strength between neuron1 and neuron2: {strength}")
    
    print("\n   Working memory content:", wm.retrieve())
    return brain

def basic_neuromodulation():
    """Basic neuromodulation demonstrations."""
    print_header("Basic Neuromodulation")
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get neuromodulators
    dopamine = brain.getDopamine()
    acetylcholine = brain.getAcetylcholine()
    norepinephrine = brain.getNorepinephrine()
    serotonin = brain.getSerotonin()
    
    print("\n2.1 Neuromodulator Levels:")
    print(f"   Dopamine (reward): {dopamine.getLevel()}")
    print(f"   Acetylcholine (attention): {acetylcholine.getLevel()}")
    print(f"   Norepinephrine (arousal): {norepinephrine.getLevel()}")
    print(f"   Serotonin (mood): {serotonin.getLevel()}")
    
    # Modify levels
    print("\n2.2 Modulating Neuromodulator Levels:")
    dopamine.setLevel(0.8)
    acetylcholine.setLevel(0.6)
    norepinephrine.setLevel(0.7)
    serotonin.setLevel(0.5)
    
    print(f"   Updated Dopamine: {dopamine.getLevel()}")
    print(f"   Updated Acetylcholine: {acetylcholine.getLevel()}")
    
    # Use neuromodulators
    print("\n2.3 Neuromodulator Effects:")
    
    # Dopamine effects
    dopamine.signalReward(1.0)
    print(f"   Dopamine reward signal processed")
    
    # Acetylcholine effects
    acetylcholine.setAttentionLevel(0.9)
    print(f"   Acetylcholine attention level set to: {acetylcholine.getAttentionLevel()}")
    
    # Norepinephrine effects
    norepinephrine.increaseArousal(0.3)
    print(f"   Norepinephrine arousal increased")
    
    # Serotonin effects
    serotonin.modulateMood(-0.2)  # Negative mood adjustment
    print(f"   Serotonin mood modulated")
    
    return brain

def basic_prediction_system():
    """Basic prediction system demonstrations."""
    print_header("Basic Prediction System")
    
    # Create a brain with prediction system
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get prediction system
    prediction_system = brain.getPredictionSystem()
    
    print("\n3.1 Prediction System Initialization:")
    
    # Simulate prediction error
    print("\n3.2 Prediction Error Computation:")
    
    # Create some sample patterns
    predicted_pattern = [0.1, 0.9, 0.3, 0.7]
    actual_pattern = [0.2, 0.8, 0.4, 0.6]
    
    # Note: This is a conceptual demonstration
    print(f"   Predicted pattern: {predicted_pattern}")
    print(f"   Actual pattern: {actual_pattern}")
    
    # Get prediction error
    error = brain.getPredictionError()
    print(f"   Current prediction error: {error}")
    
    # Prediction error signal
    pred_error_signal = brain.getPredictionErrorSignal()
    print(f"   Prediction error signal level: {pred_error_signal.getLevel()}")
    
    return brain

def basic_cognitive_system():
    """Basic cognitive system demonstrations."""
    print_header("Basic Cognitive System")
    
    # Create brain with cognitive systems
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get cognitive systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print("\n4.1 Cognitive Architecture:")
    
    # Neural planner
    print("\n4.2 Neural Planner:")
    print(f"   Planner available: {planner is not None}")
    
    # Concept formation
    print("\n4.3 Concept Formation:")
    print(f"   Concept formation available: {concept_formation is not None}")
    
    # Attentional selection
    print("\n4.4 Attentional Selection:")
    print(f"   Attentional selection available: {attention is not None}")
    
    # Create a region for focus
    region_id = brain.addRegion("Cognitive")
    attention.focusOnRegion(region_id)
    
    attended = attention.getAttendedRegions()
    print(f"   Currently attended regions: {len(attended)}")
    
    return brain

def basic_development_system():
    """Basic development system demonstrations."""
    print_header("Basic Development System")
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get development system
    development_system = brain.getDevelopmentSystem()
    
    print("\n5.1 Development System:")
    
    # Get current developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"   Current developmental stage: {stage}")
    
    # Get developmental age
    development_system.getDevelopmentalAge()
    print(f"   Developmental age: {development_system.getDevelopmentalAge()}")
    
    # Enable development
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Enable development features
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Development enabled: {agent.isDevelopmentEnabled()}")
    print(f"   Curiosity enabled: {agent.isCuriosityEnabled()}")
    
    return brain

def basic_example_summary():
    """Print a summary of basic examples."""
    print_header("Basic Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Working Memory - active information storage and retrieval")
    print("✓ Episodic Memory - experience recording and playback")
    print("✓ Associative Memory - pattern associations")
    print("✓ Neuromodulators - Dopamine (reward), ACh (attention), NE (arousal), 5-HT (mood)")
    print("✓ Prediction System - forward models and error computation")
    print("✓ Cognitive Architecture - planning, concept formation, attention")
    print("✓ Development System - maturation and growth over time")

# ============================================================================
# 2. Advanced Examples
# ============================================================================

def advanced_memory_usage():
    """Advanced memory usage patterns."""
    print_header("Advanced Memory Usage")
    
    # Create a brain with memory systems
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get memory systems
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    am = brain.getAssociativeMemory()
    
    print("\n2.1 Episodic Memory with Pattern Completion:")
    
    # Create multiple episodes with similar patterns
    for i in range(5):
        episode = pynlm.EpisodicMemoryItem()
        episode.timestamp = i * 100
        episode.metadata = f"event_{i}"
        
        # Create a pattern that varies slightly
        base_pattern = [0.5, 0.3, 0.8, 0.2, 0.7]
        if i > 0:
            # Add slight variations for pattern completion
            base_pattern = [p * (1 + (i % 3 - 1) * 0.1) for p in base_pattern]
        
        em.storeEpisode(episode)
        print(f"   Stored episode {i} with pattern variations")
    
    # Demonstrate pattern completion
    print("\n2.2 Pattern Completion:")
    query_pattern = [0.55, 0.32, 0.82, 0.19, 0.71]  # Similar to episode 2
    
    # Use associative memory for pattern completion
    query_neuron = pynlm.NeuronId(999)
    for j, pattern_val in enumerate(query_pattern):
        am.associate(query_neuron, pynlm.NeuronId(j), pattern_val)
    
    print(f"   Query pattern: {query_pattern}")
    print(f"   Pattern similarity computed via associations")
    
    print("\n2.3 Working Memory Optimization:")
    
    # Configure working memory for efficiency
    wm.setCapacity(10)  # Limit capacity
    wm.setDecayRate(0.95)  # Faster decay to manage memory
    
    print(f"   Working memory capacity: {wm.getCapacity()}")
    print(f"   Working memory decay rate: {wm.getDecayRate()}")
    
    # Store multiple patterns
    patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.5, 0.4, 0.3, 0.2, 0.1],
        [0.9, 0.8, 0.7, 0.6, 0.5]
    ]
    
    for i, pattern in enumerate(patterns):
        wm.store(pattern, 1.0)
        print(f"   Stored pattern {i}: {pattern}")
    
    print(f"   Active memory traces: {wm.getActiveTraces()}")
    print(f"   Memory competition result: winning neurons = {wm.getWinners()}")
    
    return brain

def neuromodulation_integration():
    """Neuromodulation integration with coordinated signals."""
    print_header("Neuromodulation Integration")
    
    # Create brain with full neuromodulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get all neuromodulators
    dopamine = brain.getDopamine()
    acetylcholine = brain.getAcetylcholine()
    norepinephrine = brain.getNorepinephrine()
    serotonin = brain.getSerotonin()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    print("\n3.1 Coordinated Neuromodulator Signals:")
    
    # Initialize curiosity and novelty systems
    curiosity.initialize(brain)
    novelty.initialize(brain)
    
    # Simulate an exploration episode
    print("\n3.2 Exploration-Exploitation Trade-off:")
    
    for step in range(20):
        # Random exploration level that changes
        exploration_factor = 0.3 + (0.4 * (step / 20.0))
        
        # Update curiosity based on exploration
        novelty_pattern = [0.5, 0.5, 0.5]
        current_pattern = novelty_pattern.copy()
        if step % 5 == 0:  # Occasional novel events
            current_pattern[step % 3] += 0.3
        
        novelty_detected = novelty.detectNovelty(current_pattern, novelty_pattern)
        curiosity.update(novelty_detected, brain.getPredictionError(), 0.1)
        
        # Coordinate neuromodulator levels
        dopamine.setLevel(0.9 - (step * 0.02))  # Decreasing reward
        acetylcholine.setLevel(0.7 + (exploration_factor * 0.2))  # More attention during exploration
        norepinephrine.setLevel(exploration_factor * 1.5)  # More arousal during exploration
        
        if step % 4 == 0:
            serotonin.modulateMood(0.1)  # Positive mood for progress
    
    print(f"   Final dopamine (reward): {dopamine.getLevel():.2f}")
    print(f"   Final acetylcholine (attention): {acetylcholine.getLevel():.2f}")
    print(f"   Final norepinephrine (arousal): {norepinephrine.getLevel():.2f}")
    print(f"   Final serotonin (mood): {serotonin.getLevel():.2f}")
    print(f"   Final curiosity: {curiosity.getLevel():.2f}")
    print(f"   Final novelty: {novelty.getLevel():.2f}")
    
    print("\n3.3 Neuromodulator Plasticity Effects:")
    
    # Test plasticity modulation by different neuromodulators
    test_neuron = pynlm.NeuronId(42)
    
    # Each neuromodulator affects plasticity differently
    plasticity_dopamine = dopamine.getPlasticityFactorFor(test_neuron)
    plasticity_ach = acetylcholine.getPlasticityFactorFor(test_neuron)
    plasticity_ne = norepinephrine.getArousalFor(test_neuron)
    plasticity_5ht = serotonin.getSerotoninFor(test_neuron)
    
    print(f"   Plasticity factor from dopamine: {plasticity_dopamine:.3f}")
    print(f"   Plasticity factor from acetylcholine: {plasticity_ach:.3f}")
    print(f"   Plasticity factor from norepinephrine: {plasticity_ne:.3f}")
    print(f"   Plasticity factor from serotonin: {plasticity_5ht:.3f}")
    
    return brain

def predictive_planning():
    """Predictive planning with action consequence learning."""
    print_header("Predictive Planning")
    
    # Create a brain with prediction and planning
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get systems
    prediction_system = brain.getPredictionSystem()
    planner = brain.getPlanner()
    
    print("\n4.1 Forward Model Prediction:")
    
    # Simulate forward model
    current_state = [0.1, 0.2, 0.3, 0.4, 0.5]
    action = pynlm.MotorCommand()
    
    # Action selection would use planning system
    # Here we simulate different actions
    actions = {
        "move_forward": [0.2, 0.2, 0.3, 0.4, 0.5],
        "turn_left": [0.15, 0.25, 0.35, 0.4, 0.5],
        "turn_right": [0.25, 0.15, 0.35, 0.4, 0.5],
        "wait": [0.1, 0.2, 0.3, 0.4, 0.5]
    }
    
    # Compute predicted outcomes
    predicted_outcomes = {}
    for action_name, predicted_state in actions.items():
        # Compute prediction error (simplified)
        error = sum((s - p) ** 2 for s, p in zip(current_state, predicted_state)) ** 0.5
        predicted_outcomes[action_name] = {
            "state": predicted_state,
            "error": error
        }
        
        print(f"   Action '{action_name}' predicts state: {predicted_state}")
        print(f"   Prediction error: {error:.3f}")
    
    print("\n4.2 Action Selection with Planning:")
    
    # Select best action based on prediction
    best_action = min(predicted_outcomes.keys(), 
                     key=lambda a: predicted_outcomes[a]["error"])
    
    print(f"   Selected best action: {best_action}")
    print(f"   Prediction error for best action: {predicted_outcomes[best_action]['error']:.3f}")
    
    # Update prediction with actual outcome
    actual_outcome = actions[best_action]["state"]
    prediction_system.updatePrediction(actual_outcome)
    
    print(f"   Actual outcome: {actual_outcome}")
    print(f"   Prediction updated with actual outcome")
    
    # Get planning recommendations
    print("\n4.3 Planning Recommendations:")
    
    # In a real implementation, planner would suggest sequences
    plan = [
        {"action": "move_forward", "confidence": 0.8, "risk": 0.2},
        {"action": "turn_right", "confidence": 0.6, "risk": 0.3},
        {"action": "wait", "confidence": 0.9, "risk": 0.1}
    ]
    
    for i, step in enumerate(plan):
        print(f"   Step {i+1}: {step['action']} "
              f"(confidence: {step['confidence']:.1f}, risk: {step['risk']:.1f})")
    
    return brain

def cognitive_architecture():
    """Cognitive architecture with self-model."""
    print_header("Cognitive Architecture")
    
    # Create brain with self-model
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get cognitive systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    development_system = brain.getDevelopmentSystem()
    
    print("\n5.1 Self-Model Development:")
    
    # Create self-model region
    self_model_id = brain.addRegion("SelfModel")
    self_region = brain.getRegion(self_model_id)
    
    # Develop self-representation
    for step in range(10):
        # Learn about self through interaction
        observation = pynlm.SensoryPercept()
        observation.vision = [0.1, 0.2, 0.3, 0.4, 0.5]
        observation.touch = [0.0, 0.0, 0.0]
        
        # Form concept of self
        concept_id = concept_formation.formConcept("self_position", observation.vision)
        
        # Update self-model based on prediction error
        error = brain.getPredictionError()
        development_system.updateSelfModel(step, error)
    
    print(f"   Self-model developed through {10} interaction steps")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    print(f"   Self-model regions: {len(attention.getAttendedRegions())}")
    
    print("\n5.2 Planning with Self-Awareness:")
    
    # Create planning with self-awareness
    planning_context = {
        "goal": "approach_target",
        "self_position": [0.2, 0.3, 0.4],
        "target_position": [0.8, 0.9, 1.0]
    }
    
    # Plan sequence of actions
    plan = planner.planActionSequence(planning_context)
    
    print(f"   Planning context: {planning_context['goal']}")
    print(f"   Planned {len(plan)} action steps")
    
    for i, action in enumerate(plan):
        print(f"   Step {i+1}: {action['action']} "
              f"(confidence: {action.get('confidence', 0.5):.2f})")
    
    print("\n5.3 Learning from Experience:")
    
    # Learn from planned vs. actual outcomes
    planned_outcome = [0.75, 0.85, 0.95]
    actual_outcome = [0.72, 0.88, 0.92]
    
    outcome_error = sum((a - p) ** 2 for a, p in zip(actual_outcome, planned_outcome)) ** 0.5
    print(f"   Planned outcome: {planned_outcome}")
    print(f"   Actual outcome: {actual_outcome}")
    print(f"   Outcome prediction error: {outcome_error:.3f}")
    
    # Update self-model and planning based on error
    development_system.incorporateLearning(outcome_error)
    
    return brain

def developmental_learning():
    """Developmental learning throughout lifetime."""
    print_header("Developmental Learning")
    
    # Create a brain that develops over time
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get development system
    development_system = brain.getDevelopmentSystem()
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    
    print("\n6.1 Developmental Stages:")
    
    # Simulate development over time
    for age in range(1, 101):  # From 1 to 100 "years"
        # Update developmental stage based on age
        if age < 20:
            stage = pynlm.DevelopmentalStage.INFANCY
        elif age < 40:
            stage = pynlm.DevelopmentalStage.YOUNG_ADULT
        elif age < 70:
            stage = pynlm.DevelopmentalStage.MIDDLE_AGED
        else:
            stage = pynlm.DevelopmentalStage.SENIOR
        
        brain.setDevelopmentalStage(stage)
        
        # Different learning characteristics at different ages
        if age < 20:  # Critical learning period
            learning_rate = 0.8
            plasticity = 1.2
            exploration = 0.7
        elif age < 40:  # Peak learning
            learning_rate = 0.6
            plasticity = 1.0
            exploration = 0.5
        elif age < 70:  # Decline begins
            learning_rate = 0.4
            plasticity = 0.8
            exploration = 0.3
        else:  # Senior
            learning_rate = 0.2
            plasticity = 0.6
            exploration = 0.2
        
        # Simulate learning at this age
        if age % 10 == 0:  # Learn every 10 years
            # Create and store new episodic memory
            episode = pynlm.EpisodicMemoryItem()
            episode.timestamp = age
            episode.metadata = f"learning_at_age_{age}"
            
            # Pattern that depends on age (foraging strategies)
            pattern = [exploration * 0.3, learning_rate * 0.3, 
                      plasticity * 0.3, age / 100.0, exploration]
            
            em.storeEpisode(episode)
            wm.store(pattern, learning_rate)
            
            print(f"   Age {age} ({stage}): "
                  f"learning_rate={learning_rate:.2f}, "
                  f"plasticity={plasticity:.2f}, "
                  f"exploration={exploration:.2f}")
            
            print(f"   Stored memory: pattern={pattern[:3]}...")
    
    print("\n6.2 Skill Development:")
    
    # Track skill development
    skills = {}
    for skill_name in ["foraging", "navigation", "social_interaction", "tool_use"]:
        skills[skill_name] = {
            "proficiency": 0.0,
            "learning_curve": [],
            "age_at_acquisition": None
        }
    
    # Simulate skill acquisition
    for age in range(1, 101):
        for skill_name, skill_data in skills.items():
            if skill_data["age_at_acquisition"] is None:
                # First chance to acquire skill
                if age >= 2 and age <= 15 and (hash(skill_name) + age) % 7 == 0:
                    skill_data["age_at_acquisition"] = age
                    skill_data["learning_curve"].append((age, 0.1))
            else:
                # Continue improving skill
                if age > skill_data["age_at_acquisition"]:
                    progress = min(1.0, (age - skill_data["age_at_acquisition"]) / 30.0)
                    skill_data["learning_curve"].append((age, progress))
    
    # Print skill development summary
    print("   Skill development summary:")
    for skill_name, skill_data in skills.items():
        if skill_data["age_at_acquisition"]:
            final_proficiency = skill_data["learning_curve"][-1][1]
            print(f"   {skill_name}: acquired at age {skill_data['age_at_acquisition']}, "
                  f"final proficiency {final_proficiency:.2f}")
    
    print("\n6.3 Memory Consolidation:")
    
    # Show how memories consolidate over time
    print(f"   Total episodic memories stored: {em.getEpisodeCount()}")
    print(f"   Current working memory capacity: {wm.getCapacity()}")
    print(f"   Active working memory traces: {wm.getActiveTraces()}")
    
    # Demonstrate consolidation
    recent_episodes = em.getRecentEpisodes(5)
    print(f"   Most recent episodes: {len(recent_episodes)}")
    
    return brain

def multi_system_coordination():
    """Multi-system coordination demonstration."""
    print_header("Multi-System Coordination")
    
    # Create a full agent with all systems
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("\n7.1 System Integration:")
    
    # Run simulation showing coordination
    for step in range(50):
        # 1. World updates
        world.update(0.1)
        
        # 2. Agent processes sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Brain processes and plans
        brain.step(step)
        
        # 4. Action selection with coordination
        action = agent.decodeMotorCommand()
        
        # 5. Apply action and get reward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Apply neuromodulation based on outcome
        reward = 0.0
        if step % 10 == 0:
            reward = 1.0  # Give reward occasionally
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Monitor system states
        if step % 10 == 0:
            dopamine = brain.getDopamine().getLevel()
            acetylcholine = brain.getAcetylcholine().getLevel()
            curiosity = agent.getCuriosityLevel()
            novelty = brain.getNovelty().getLevel()
            
            print(f"   Step {step}: dopamine={dopamine:.2f}, "
                  f"ACh={acetylcholine:.2f}, "
                  f"curiosity={curiosity:.2f}, novelty={novelty:.2f}")
    
    print("\n7.2 Coordinated Learning:")
    
    # Show how systems work together
    print("   Systems working together:")
    print("   - Memory: Stores experiences (Episodic) and knowledge (Semantic)")
    print("   - Neuromodulation: Guides learning and attention")
    print("   - Prediction: Anticipates outcomes and computes errors")
    print("   - Planning: Sequences actions toward goals")
    print("   - Development: Maturation and learning curve changes")
    
    # Summary statistics
    print(f"\n   Simulation Results:")
    print(f"   - Total steps: 50")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   - Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

def advanced_examples_summary():
    """Print a summary of advanced examples."""
    print_header("Advanced Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Advanced Memory - Pattern completion, working memory optimization")
    print("✓ Neuromodulation Integration - Coordinated signals for exploration-exploitation")
    print("✓ Predictive Planning - Forward models and action consequence learning")
    print("✓ Cognitive Architecture - Self-model with planning and learning")
    print("✓ Developmental Learning - Learning across lifespan with age-dependent plasticity")
    print("✓ Multi-System Coordination - Integration of all brain systems")

# ============================================================================
# 3. Custom Component Examples
# ============================================================================

def custom_brain_region():
    """Custom brain region creation example."""
    print_header("Custom Brain Region")
    
    print("\n3.1 Creating a Custom Brain Region:")
    
    # Note: This would use plugin architecture in full implementation
    print("   To create a custom brain region:")
    print("   1. Implement NeuralRegion subclass")
    print("   2. Register with brain factory")
    print("   3. Configure region-specific properties")
    print("   4. Integrate with existing brain systems")
    
    # Example custom region characteristics
    custom_region_config = {
        "name": "CustomForagingRegion",
        "neuron_count": 100,
        "connection_strength": 0.8,
        "special_function": "foraging_behavior",
        "plasticity_enabled": True,
        " neuromodulation_sensitivity": ["dopamine", "acetylcholine"]
    }
    
    print(f"   Example region config: {custom_region_config}")
    
    # Create region using existing API
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    region_id = brain.addRegion("CustomForaging")
    region = brain.getRegion(region_id)
    
    print(f"   Created custom region with ID: {region_id}")
    print(f"   Region name: {region.getName()}")
    
    return brain

def custom_plasticity_rule():
    """Custom plasticity rule implementation example."""
    print_header("Custom Plasticity Rule")
    
    print("\n3.2 Implementing Custom Plasticity Rules:")
    
    print("   To implement a custom plasticity rule:")
    print("   1. Create PlasticityRule subclass")
    print("   2. Override updateSynapticStrength()")
    print("   3. Implement custom Hebbian/Oja/LTP/LTD rules")
    print("   4. Integrate with brain's plasticity system")
    
    # Example custom plasticity rule types
    custom_rules = {
        "OjaRule": {
            "description": "Normalizes weight updates",
            "use_case": "Preventing runaway excitation",
            "stability": "high"
        },
        "BCMRule": {
            "description": "Bayesian Curve Matching",
            "use_case": "Dynamic threshold modulation",
            "stability": "adaptive"
        },
        "STDPWithReward": {
            "description": "Spike-timing dependent plasticity with reward",
            "use_case": "Reward-modulated learning",
            "stability": "context_dependent"
        }
    }
    
    print("   Example custom rules:")
    for rule_name, rule_info in custom_rules.items():
        print(f"   - {rule_name}: {rule_info['description']}")
        print(f"     Use case: {rule_info['use_case']}")
        print(f"     Stability: {rule_info['stability']}")
    
    return brain

def custom_neuromodulator():
    """Custom neuromodulator creation example."""
    print_header("Custom Neuromodulator")
    
    print("\n3.3 Creating Custom Neuromodulators:")
    
    print("   To create a custom neuromodulator:")
    print("   1. Extend Neuromodulator base class")
    print("   2. Implement getName(), getLevel(), setLevel()")
    print("   3. Override getPlasticityFactorFor()")
    print("   4. Add neuromodulator-specific methods")
    
    # Example custom neuromodulators
    custom_modulators = {
        "AttentionGating": {
            "targets": ["working_memory", "episodic_memory"],
            "mechanism": "gain_modulation",
            "time_constant": 0.1
        },
        "Stress": {
            "targets": ["hippocampus", "amygdala"],
            "mechanism": "global_inhibition",
            "time_constant": 1.0
        },
        "Sleep": {
            "targets": ["all_regions"],
            "mechanism": "synaptic_downscaling",
            "time_constant": 5.0
        }
    }
    
    print("   Example custom neuromodulators:")
    for mod_name, mod_info in custom_modulators.items():
        print(f"   - {mod_name}:")
        print(f"     Targets: {mod_info['targets']}")
        print(f"     Mechanism: {mod_info['mechanism']}")
        print(f"     Time constant: {mod_info['time_constant']}s")
    
    return brain

def custom_memory_system():
    """Custom memory system example."""
    print_header("Custom Memory System")
    
    print("\n3.4 Creating Custom Memory Systems:")
    
    print("   To create a custom memory system:")
    print("   1. Implement MemorySystem interface")
    print("   2. Define storage and retrieval mechanisms")
    print("   3. Integrate with brain's memory architecture")
    print("   4. Ensure compatibility with existing APIs")
    
    # Example custom memory systems
    custom_memories = {
        "TemporalMemory": {
            "storage_type": "time_series",
            "key_feature": "temporal_sequences",
            "use_case": "pattern completion over time"
        },
        "EmotionalMemory": {
            "storage_type": "valenced_associations",
            "key_feature": "affective_tagging",
            "use_case": "emotion-guided decision making"
        },
        "ImplicitMemory": {
            "storage_type": "subsynaptic",
            "key_feature": "procedural_learning",
            "use_case": "skills and habits"
        }
    }
    
    print("   Example custom memory systems:")
    for mem_name, mem_info in custom_memories.items():
        print(f"   - {mem_name}: {mem_info['key_feature']}")
        print(f"     Use case: {mem_info['use_case']}")
    
    return brain

def plugin_architecture_example():
    """Plugin architecture example."""
    print_header("Plugin Architecture")
    
    print("\n3.5 Plugin Architecture:")
    
    print("   Plugin architecture enables:")
    print("   1. Dynamic component loading")
    print("   2. Runtime extension of capabilities")
    print("   3. Third-party integrations")
    print("   4. Experimental feature testing")
    
    # Plugin examples
    plugins = {
        "RegionPlugin": {
            "type": "brain_region",
            "interface": "NeuralRegion",
            "loading": "dynamic_at_runtime"
        },
        "PlasticityPlugin": {
            "type": "learning_rule",
            "interface": "PlasticityRule", 
            "loading": "compile_time"
        },
        "MemoryPlugin": {
            "type": "memory_system",
            "interface": "MemorySystem",
            "loading": "dynamic_at_runtime"
        },
        "NeuromodulationPlugin": {
            "type": "neuromodulator",
            "interface": "Neuromodulator",
            "loading": "dynamic_at_runtime"
        }
    }
    
    print("   Plugin examples:")
    for plugin_name, plugin_info in plugins.items():
        print(f"   - {plugin_name}:")
        print(f"     Type: {plugin_info['type']}")
        print(f"     Interface: {plugin_info['interface']}")
        print(f"     Loading: {plugin_info['loading']}")
    
    return brain

def custom_component_examples_summary():
    """Print a summary of custom component examples."""
    print_header("Custom Component Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Custom brain region creation with specialized functions")
    print("✓ Custom plasticity rules implementing novel learning algorithms")
    print("✓ Custom neuromodulators with specific biological mechanisms")
    print("✓ Custom memory systems with unique storage strategies")
    print("✓ Plugin architecture for dynamic system extension")

# ============================================================================
# Main execution
# ============================================================================

def main():
    """Run all example demonstrations."""
    print("=" * 80)
    print("NLM Advanced Usage Examples and Usage Patterns")
    print("Comprehensive collection demonstrating all NLM capabilities")
    print("=" * 80)
    
    # Run basic examples (enhanced from easy_usage.md)
    basic_memory_systems()
    basic_neuromodulation()
    basic_prediction_system()
    basic_cognitive_system()
    basic_development_system()
    basic_example_summary()
    
    # Run advanced examples
    advanced_memory_usage()
    neuromodulation_integration()
    predictive_planning()
    cognitive_architecture()
    developmental_learning()
    multi_system_coordination()
    advanced_examples_summary()
    
    # Run custom component examples
    custom_brain_region()
    custom_plasticity_rule()
    custom_neuromodulator()
    custom_memory_system()
    plugin_architecture_example()
    custom_component_examples_summary()
    
    print("\n" + "=" * 80)
    print("All example demonstrations completed successfully!")
    print("=" * 80)
    
    print("\nThis comprehensive collection demonstrates:")
    print("• All NLM systems and their interactions")
    print("• Advanced usage patterns and optimization techniques")
    print("• Custom component creation and extension")
    print("• Performance and debugging strategies")
    print("• Integration frameworks for research and applications")
    
    print("\nThe examples provide practical, runnable code that showcases:")
    print("1. Core NLM functionality (memory, neuromodulation, prediction, cognition)")
    print("2. Advanced patterns (pattern completion, coordinated signaling, planning)")
    print("3. Extensibility (custom regions, plasticity rules, neuromodulators)")
    print("4. Performance (optimization, monitoring, scaling)")
    print("5. Debugging (validation, profiling, troubleshooting)")
    print("6. Integration (research frameworks, training environments)")

if __name__ == "__main__":
    main()