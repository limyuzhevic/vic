# NLM Advanced Usage Guide

## Advanced Agent Behaviors

### 1. Hierarchical Learning with Developmental Stages

```python
import pynlm
import time

def run_hierarchical_learning():
    """Demonstrates hierarchical learning where agents progress through developmental stages"""
    
    config = pynlm.createDefaultConfig()
    
    # Configure for longer developmental timeline
    config.set("simulation_timestep", 0.001)
    config.set("brain.development_interval", 1000)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with enhanced plasticity
    agent = pynlm.createAgentBrain(brain)
    
    # Configure for hierarchical learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    print("Starting hierarchical learning simulation...")
    print("This demonstrates learning across developmental stages:")
    
    for step in range(5000):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        # Simple reward based on exploration
        if step % 100 == 0:
            reward = 0.5
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log developmental progress
        if step % 500 == 0:
            stage = agent.getDevelopmentalStage()
            print(f"Step {step}: {stage.name} stage, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"dopamine: {agent.getNeuromodulationLevel():.3f}")
            
            # Observe stage-specific behavior changes
            if stage == pynlm.DevelopmentalStage.Initial:
                print("  -> Early exploration phase with high plasticity")
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                print("  -> Critical learning period - maximizing synaptic changes")
            elif stage == pynlm.DevelopmentalStage.Maturation:
                print("  -> Maturation - refining learned behaviors")
            elif stage == pynlm.DevelopmentalStage.Adult:
                print("  -> Adult behavior - stable, optimized performance")
    
    print(f"\nSimulation complete! Final stage: {agent.getDevelopmentalStage().name}")
    return brain, agent

# Run hierarchical learning
run_hierarchical_learning()
```

### 2. Neuromodulation-Driven Learning

```python
import pynlm

def run_neuromodulation_demo():
    """Demonstrates how dopamine and novelty drive learning"""
    
    config = pynlm.createDefaultConfig()
    
    # Configure brain for neuromodulation experiments
    config.set("brain.neuron_count", 500)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("Neuromodulation Learning Demo")
    print("==========================")
    
    # Phase 1: Exploration (high novelty)
    print("\nPhase 1: Exploration Phase")
    print("- Novelty detection: High")
    print("- Curiosity: High")
    print("- Dopamine: Low baseline")
    
    for step in range(300):
        world.update(0.1)
        
        # Random exploration actions
        import random
        if random.random() < 0.3:  # 30% exploration
            curiosity_cmd = agent.selectWithCuriosity(None)
            if curiosity_cmd != pynlm.MotorCommand.Wait:
                world.applyMotorCommand(curiosity_cmd, world.getSimulationTime())
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Log neuromodulation state
        if step % 50 == 0:
            print(f"  Step {step}: novelty={agent.getNoveltyLevel():.3f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"dopamine={agent.getNeuromodulationLevel():.3f}")
    
    # Phase 2: Reward-based learning
    print("\nPhase 2: Reward-Based Learning")
    print("- Novelty detection: Low (familiar environment)")
    print("- Curiosity: Moderate")
    print("- Dopamine: Phasic responses to rewards")
    
    for step in range(300, 600):
        world.update(0.1)
        
        # Process normally, let STDP learn from patterns
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply structured rewards
        if step % 100 == 10:  # Small reward after 100 steps
            agent.applyRewardModulation(1.0, 0.0)
        elif step % 100 == 90:  # Mild punishment
            agent.applyRewardModulation(-0.5, 0.0)
        
        # Log neuromodulation state
        if step % 50 == 0:
            print(f"  Step {step}: novelty={agent.getNoveltyLevel():.3f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"dopamine={agent.getNeuromodulationLevel():.3f}")
    
    # Phase 3: Integration
    print("\nPhase 3: Integration of All Signals")
    print("- Balanced novelty and reward learning")
    print("- Curiosity-driven exploration modulates plasticity")
    
    for step in range(600, 900):
        world.update(0.1)
        
        # Complex decision making
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply both reward and prediction error
        reward = 1.0 if step % 150 == 0 else 0.0
        predicted = 0.5  # Simple prediction
        agent.applyRewardModulation(reward, predicted)
        
        # Log integration effects
        if step % 50 == 0:
            plasticity = brain.getDopamine().getPlasticityFactor() if brain.getDopamine() else 1.0
            print(f"  Step {step}: novelty={agent.getNoveltyLevel():.3f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"plasticity={plasticity:.3f}")
    
    print(f"\nNeuromodulation demo complete!")
    return brain, agent

# Run neuromodulation demo
run_neuromodulation_demo()
```

### 3. Working Memory and Episodic Memory

```python
import pynlm

def run_memory_integration_demo():
    """Demonstrates working memory, episodic memory, and prediction integration"""
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify memory systems are present
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    prediction_sys = brain.getPredictionSystem()
    
    print("Memory Integration Demo")
    print("======================")
    print(f"Working Memory Capacity: {working_mem.getCapacity() if working_mem else 'N/A'}")
    print(f"Episodic Memory Max Episodes: {episodic_mem.getMaxEpisodes() if episodic_mem else 'N/A'}")
    
    # Create agent for world interaction
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("\nSimulating experience accumulation...")
    
    # Step through and accumulate experiences
    for step in range(1000):
        world.update(0.1)
        
        # Store in working memory
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and maintains working memory
        brain.step(step)
        
        # Occasionally capture episodes
        if step % 50 == 0 and episodic_mem:
            # Create an episodic memory item
            episode = pynlm.MemoryItem()
            episode.timestamp = step
            episode.reward = agent.getNeuromodulationLevel()
            
            # Capture active neurons
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for pop in region.getPopulations():
                        for neuron in pop.getNeurons():
                            if neuron.isFiring():
                                episode.neuron_ids.append(neuron.getId())
            
            episodic_mem.storeEpisode(episode)
            print(f"  Episode captured at step {step}: {len(episode.neuron_ids)} active neurons")
        
        # Apply reward learning
        reward = 0.5 if step % 100 < 50 else 0.0  # Varying reward schedule
        agent.applyRewardModulation(reward, 0.0)
        
        # Show memory statistics periodically
        if step % 100 == 0:
            if working_mem:
                print(f"  Step {step}: Working memory traces: {working_mem.getActiveTraces()}")
            if episodic_mem:
                print(f"  Step {step}: Total episodic memories: {episodic_mem.getEpisodeCount()}")
    
    # Demonstrate memory replay
    print("\nMemory Replay Phase...")
    if episodic_mem:
        # Replay important episodes (earlier in sequence are typically more important)
        episodes = episodic_mem.getEpisodesForReplay(3)
        for i, episode in enumerate(episodes):
            print(f"  Replaying episode {i}: stored at step {episode.timestamp}")
            episodic_mem.replayEpisode(episode)
    
    print(f"\nMemory integration demo complete!")
    print(f"Total episodic memories stored: {episodic_mem.getEpisodeCount() if episodic_mem else 0}")
    return brain, agent

# Run memory demo
run_memory_integration_demo()
```

### 4. Advanced STDP and Hebbian Learning

```python
import pynlm

def run_advanced_plasticity_demo():
    """Demonstrates advanced plasticity rules working together"""
    
    config = pynlm.createDefaultConfig()
    
    # Configure for advanced plasticity experiments
    config.set("brain.neuron_count", 800)
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.02)
    config.set("stdp_tau", 30.0)  # Longer time constant
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get plasticity controllers
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    
    # Configure advanced Hebbian parameters
    if hebbian:
        hebbian.setLearningRate(0.02)
        hebbian.setTimeConstant(25.0)  # Match STDP time constant
        hebbian.setCovarianceThreshold(0.1)
    
    print("Advanced Plasticity Demo")
    print("======================")
    print(f"STDP LTP weight: {stdp.getLTPWeight() if stdp else 0.0}")
    print(f"STDP LTD weight: {stdp.getLTDWeight() if stdp else 0.0}")
    print(f"Hebbian learning rate: {hebbian.getLearningRate() if hebbian else 0.0}")
    print(f"Hebbian time constant: {hebbian.getTimeConstant() if hebbian else 0.0}")
    
    # Create agent with world interaction
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    print("\nTesting synergistic plasticity rules...")
    print("Simulating correlated spike patterns...")
    
    for step in range(800):
        world.update(0.1)
        
        # Generate correlated activity to test Hebbian learning
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Inject correlated input to specific neuron groups
        if step % 5 == 0:  # Every 5 steps
            # Correlated input to sensory neurons to test Hebbian co-activation
            for i in range(5):
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
            
            # Temporal pattern to test STDP (pre before post)
            if step % 20 < 10:  # Pattern repeats every 20 steps
                # Create temporal pattern for STDP
                if step % 20 == 0:  # Pre-synaptic spike
                    for i in range(3):
                        brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 8.0)
                elif step % 20 == 5:  # Post-synaptic spike (after pre)
                    for i in range(3):
                        brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 8.0)
        
        brain.step(step)
        
        # Apply reward modulation to test reward-modulated plasticity
        if step % 100 == 0:
            reward = 1.0
            predicted = 0.0
            agent.applyRewardModulation(reward, predicted)
        
        # Show plasticity statistics
        if step % 100 == 0:
            print(f"\nStep {step}:")
            if stdp:
                print(f"  STDP weights - LTP: {stdp.getLTPWeight():.3f}, LTD: {stdp.getLTDWeight():.3f}")
            if hebbian:
                print(f"  Hebbian - rate: {hebbian.getLearningRate():.3f}, "
                      f"time_constant: {hebbian.getTimeConstant():.1f}")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Average weight: {brain.getExcitationInhibitionRatio():.3f}")
    
    print(f"\nAdvanced plasticity demo complete!")
    print("Both STDP and Hebbian rules worked synergistically")
    return brain, agent

# Run advanced plasticity demo
run_advanced_plasticity_demo()
```

### 5. Complex Agent Behavior with Multiple Modulators

```python
import pynlm

def run_complex_behavior_demo():
    """Demonstrates complex agent behavior with all neuromodulators interacting"""
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get all neuromodulators
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print("Complex Behavior Demo")
    print("=====================")
    print("Testing integration of all neuromodulators:")
    print("- Dopamine: Reward prediction error")
    print("- Curiosity: Exploration motivation")
    print("- Novelty: Unpredictability detection")
    print("- Prediction Error: Learning from mistakes")
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width(25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("\nSimulating complex decision-making...")
    
    # Complex state tracking
    reward_history = []
    novelty_history = []
    curiosity_history = []
    
    for step in range(1000):
        world.update(0.1)
        
        # Get current neuromodulator state
        current_dopamine = dopamine.getLevel() if dopamine else 0.0
        current_novelty = novelty.getLevel() if novelty else 0.0
        current_curiosity = curiosity.getLevel() if curiosity else 0.0
        current_prediction_error = prediction_error.getLevel() if prediction_error else 0.0
        
        # Complex decision making based on all signals
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Decision heuristic combining all neuromodulators
        action_choice = make_complex_decision(
            current_dopamine, current_novelty, current_curiosity,
            current_prediction_error, step
        )
        
        world.applyMotorCommand(action_choice, world.getSimulationTime())
        
        # Complex reward calculation
        reward = calculate_complex_reward(world, step)
        predicted = predict_complex_reward(world, step)
        
        # Apply all neuromodulators
        agent.applyRewardModulation(reward, predicted)
        
        if dopamine:
            dopamine.update(0.1)
        if curiosity:
            curiosity.update(0.1)
        if novelty:
            novelty.update(0.1)
        if prediction_error:
            # Set prediction error based on reward difference
            prediction_error.setLevel(abs(reward - predicted))
        
        # Log state
        reward_history.append(reward)
        novelty_history.append(current_novelty)
        curiosity_history.append(current_curiosity)
        
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Action: {action_choice.name}")
            print(f"  Reward: {reward:.2f}, Predicted: {predicted:.2f}")
            print(f"  Dopamine: {current_dopamine:.3f}, Novelty: {current_novelty:.3f}, "
                  f"Curiosity: {current_curiosity:.3f}, PE: {current_prediction_error:.3f}")
            
            # Analyze pattern
            analyze_behavior_pattern(reward_history, novelty_history, curiosity_history)
    
    print(f"\nComplex behavior demo complete!")
    print("Demonstrated integration of multiple neuromodulatory systems")
    return brain, agent

def make_complex_decision(dopamine, novelty, curiosity, prediction_error, step):
    """Make complex decision based on neuromodulator states"""
    
    # Weighted decision based on neuromodulator states
    # This simulates the complex decision-making in the brain
    
    weights = {
        'dopamine': 0.3 * dopamine,
        'novelty': 0.4 * novelty,
        'curiosity': 0.2 * curiosity,
        'prediction_error': 0.1 * abs(prediction_error)
    }
    
    total_weight = sum(weights.values())
    if total_weight == 0:
        return pynlm.MotorCommand.Wait
    
    # Normalize weights
    for key in weights:
        weights[key] /= total_weight
    
    # Decision based on weighted combination
    import random
    choice = random.random()
    
    # High curiosity = more exploration
    if curiosity > 0.7 and random.random() < 0.3:
        return pynlm.MotorCommand.TurnLeft if random.random() < 0.5 else pynlm.MotorCommand.TurnRight
    
    # High novelty = explore new areas
    if novelty > 0.6 and random.random() < 0.2:
        return pynlm.MotorCommand.MoveForward
    
    # High prediction error = try something new
    if abs(prediction_error) > 0.5 and random.random() < 0.25:
        return pynlm.MotorCommand.Interact
    
    # Default behavior based on dopamine (approach vs avoid)
    if dopamine > 0.5:  # High dopamine = approach behavior
        return pynlm.MotorCommand.MoveForward
    else:  # Low dopamine = cautious behavior
        return pynlm.MotorCommand.Wait

def calculate_complex_reward(world, step):
    """Calculate complex reward based on multiple factors"""
    
    reward = 0.0
    
    # Simple exploration bonus
    if step < 100:
        reward += 0.5
    
    # Consistent action reward
    if step % 50 == 0:
        reward += 1.0
    
    # Novelty bonus (simulate discovering something new)
    if step % 137 == 0:  # Prime number for "random" novelty
        reward += 0.8
    
    # Predictable pattern penalty (to encourage exploration)
    if step % 7 == 0:  # Pattern
        reward -= 0.3
    
    return max(0.0, reward)  # Reward can't be negative

def predict_complex_reward(world, step):
    """Simple prediction based on recent history"""
    
    # Predict based on step number pattern
    if step % 100 == 10:
        return 1.5  # Expect high reward
    elif step % 100 == 90:
        return 0.0  # Expect no reward
    else:
        return 0.5  # Expect moderate reward

def analyze_behavior_pattern(reward_history, novelty_history, curiosity_history):
    """Analyze behavior pattern from neuromodulator states"""
    
    if len(reward_history) < 50:
        return
    
    # Look for patterns
    avg_reward = sum(reward_history[-50:]) / 50
    avg_novelty = sum(novelty_history[-50:]) / 50
    avg_curiosity = sum(curiosity_history[-50:]) / 50
    
    # Pattern analysis
    if avg_reward > 0.8 and avg_novelty < 0.3:
        print("  Pattern detected: Consistent reward learning")
    elif avg_novelty > 0.6 and avg_curiosity > 0.5:
        print("  Pattern detected: High exploration phase")
    elif avg_reward < 0.2 and avg_novelty > 0.4:
        print("  Pattern detected: Frustration-exploration cycle")
    elif avg_curiosity > 0.7 and avg_novelty < 0.2:
        print("  Pattern detected: Sustained exploration with low novelty")

# Run complex behavior demo
run_complex_behavior_demo()
```

## Advanced Configuration Examples

### 1. Performance-Optimized Configuration

```python
# Create configuration for maximum performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger network
config.set("brain.synapse_density", 0.05)  # Sparse connectivity
config.set("simulation_timestep", 0.0005)  # Faster simulation
config.set("stdp_ltp_weight", 0.015)
config.set("stdp_ltd_weight", 0.012)
config.set("stdp_tau", 15.0)
config.set("synaptogenesis_rate", 0.00005)
config.set("pruning_rate", 0.000005)

# This configuration is optimized for:
# - Large-scale neural simulations
# - Detailed plasticity learning
# - Realistic biological parameters
```

### 2. Developmental Learning Configuration

```python
# Configure for developmental learning across stages
config = pynlm.createDefaultConfig()
config.set("brain.development_interval", 500)  # Slower development
config.set("brain.initial_plasticity", 1.0)
config.set("brain.adult_plasticity", 0.1)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# This configuration emphasizes:
# - Long-term developmental progression
# - Strong early learning (high plasticity)
# - Gradual maturation to stable adult behavior
```

### 3. Social Learning Configuration

```python
# Configure for social learning capabilities
config = pynlm.createDefaultConfig()
config.set("brain.social_learning_rate", 0.3)
config.set("brain.imitation_enabled", True)
config.set("brain.observation_window", 100)
config.set("neuromod.social_attention", 0.8)

# Enable advanced social cognitive features
# - Observation and imitation
# - Social reinforcement learning
# - Theory of mind development
```

## Programming Patterns

### 1. Agent Lifecycle Management

```python
def episode_based_training(config, num_episodes=100):
    """Agent training across multiple episodes"""
    
    for episode in range(num_episodes):
        # Initialize fresh agent for each episode
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Configure agent for episode
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run episode
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Track performance
            if step % 100 == 0:
                agent.applyRewardModulation(1.0, 0.0)
        
        # Save checkpoint if episode performance was good
        if episode % 10 == 0:
            brain.save(f"checkpoints/episode_{episode}.bin")
        
        # Log episode statistics
        print(f"Episode {episode}: completed")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Final stage: {agent.getDevelopmentalStage().name}")
        
        # Reset for next episode
        brain.reset()
    
    return brain
```

### 2. Memory Consolidation Strategy

```python
def long_term_memory_formation(agent, world, steps):
    """Implement sophisticated memory consolidation"""
    
    consolidation_schedule = [
        (100, "immediate"),      # Consolidate frequently used memories
        (500, "standard"),      # Regular consolidation
        (1000, "deep"),         # Deep consolidation
        (2000, "reconstruction") # Reconstruct and reinforce
    ]
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent.getBrain().step(step)
        
        # Determine consolidation based on step
        current_time = agent.getBrain().getTotalSpikeCount()
        
        for threshold, method in consolidation_schedule:
            if current_time % threshold == 0:
                if method == "immediate":
                    # Immediate consolidation for recent memories
                    consolidate_recent_memories(agent)
                elif method == "standard":
                    # Standard consolidation
                    standard_consolidation(agent)
                elif method == "deep":
                    # Deep processing and integration
                    deep_processing(agent)
                elif method == "reconstruction":
                    # Memory reconstruction and strengthening
                    memory_reconstruction(agent)
                break
        
        # Log consolidation status
        if step % 500 == 0:
            print(f"Step {step}: Consolidation method active")
    
    return agent
```

### 3. Plasticity Rule Scheduling

```python
def adaptive_plasticity_schedule(agent, world, steps):
    """Adapt plasticity rules based on learning progress"""
    
    learning_phase = "exploration"
    phase_transitions = [
        (100, "exploitation"),
        (500, "refinement"),
        (1000, "optimization")
    ]
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent.getBrain().step(step)
        
        # Update learning phase based on progress
        for threshold, new_phase in phase_transitions:
            if step >= threshold:
                learning_phase = new_phase
                break
        
        # Adapt plasticity based on phase
        if learning_phase == "exploration":
            # High exploration plasticity
            agent.enableStructuralPlasticity(True)
            agent.enableCuriosity(True)
            # Configure for high STDP rates
            if agent.getBrain().getSTDP():
                agent.getBrain().getSTDP().setLTPWeight(0.03)
                agent.getBrain().getSTDP().setLTDWeight(0.025)
        
        elif learning_phase == "exploitation":
            # Balance exploration and exploitation
            agent.enableStructuralPlasticity(True)
            agent.enableCuriosity(True)
            # Moderate STDP rates
            if agent.getBrain().getSTDP():
                agent.getBrain().getSTDP().setLTPWeight(0.02)
                agent.getBrain().getSTDP().setLTDWeight(0.015)
        
        elif learning_phase == "refinement":
            # Fine-tune existing knowledge
            agent.enableStructuralPlasticity(True)
            agent.enableCuriosity(True)
            # Lower STDP rates for refinement
            if agent.getBrain().getSTDP():
                agent.getBrain().getSTDP().setLTPWeight(0.01)
                agent.getBrain().getSTDP().setLTDWeight(0.008)
        
        elif learning_phase == "optimization":
            # Stabilize and optimize
            agent.enableStructuralPlasticity(False)  # Stop structural changes
            agent.enableCuriosity(False)  # Reduce exploration
            # Minimal STDP for maintenance
            if agent.getBrain().getSTDP():
                agent.getBrain().getSTDP().setLTPWeight(0.005)
                agent.getBrain().getSTDP().setLTDWeight(0.004)
        
        # Log phase
        if step % 200 == 0:
            print(f"Phase: {learning_phase} at step {step}")
    
    return agent
```

## Advanced Debugging and Monitoring

### 1. Neuromodulator Visualization

```python
def visualize_neuromodulators(brain, max_steps=1000):
    """Visualize neuromodulator activity over time"""
    
    dopamine_data = []
    curiosity_data = []
    novelty_data = []
    prediction_error_data = []
    
    for step in range(max_steps):
        # Update brain
        brain.step(step)
        
        # Collect neuromodulator data
        if brain.getDopamine():
            dopamine_data.append(brain.getDopamine().getLevel())
        if brain.getCuriosity():
            curiosity_data.append(brain.getCuriosity().getLevel())
        if brain.getNovelty():
            novelty_data.append(brain.getNovelty().getLevel())
        if brain.getPredictionErrorSignal():
            prediction_error_data.append(brain.getPredictionErrorSignal().getLevel())
        
        if step % 100 == 0:
            print(f"Step {step}: DA={dopamine_data[-1]:.3f}, "
                  f"Curiosity={curiosity_data[-1]:.3f}, "
                  f"Novelty={novelty_data[-1]:.3f}, "
                  f"PE={prediction_error_data[-1]:.3f}")
    
    # Generate plots or analysis
    print(f"\nNeuromodulator statistics over {max_steps} steps:")
    if dopamine_data:
        print(f"Dopamine - Mean: {sum(dopamine_data)/len(dopamine_data):.3f}, "
              f"Max: {max(dopamine_data):.3f}, "
              f"Min: {min(dopamine_data):.3f}")
    if curiosity_data:
        print(f"Curiosity - Mean: {sum(curiosity_data)/len(curiosity_data):.3f}, "
              f"Max: {max(curiosity_data):.3f}, "
              f"Min: {min(curiosity_data):.3f}")
    
    return {
        'dopamine': dopamine_data,
        'curiosity': curiosity_data,
        'novelty': novelty_data,
        'prediction_error': prediction_error_data
    }
```

### 2. Plasticity Analysis

```python
def analyze_plasticity_dynamics(brain, steps):
    """Analyze plasticity rule dynamics over time"""
    
    stdp_activity = []
    hebbian_activity = []
    eligibility_traces = []
    
    for step in range(steps):
        brain.step(step)
        
        # Analyze STDP
        if brain.getSTDP():
            # Get current synapse activity
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for synapse in region.getSynapses():
                        # Check if synapse is active
                        if not stdp_activity:
                            stdp_activity.append(0)
                        if synapse.getPreSpikeHistory().size() > 0:
                            stdp_activity[-1] += 1
        
        # Analyze Hebbian
        if brain.getHebbian():
            # Count correlations
            if not hebbian_activity:
                hebbian_activity.append(0)
            # Simplified analysis
            hebbian_activity[-1] += 1
        
        # Check eligibility traces
        total_eligibility = 0
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                for synapse in region.getSynapses():
                    total_eligibility += synapse.getEligibilityTrace()
        eligibility_traces.append(total_eligibility)
        
        if step % 100 == 0:
            print(f"Step {step}: STDP spikes={stdp_activity[-1] if stdp_activity else 0}, "
                  f"Hebbian correlations={hebbian_activity[-1] if hebbian_activity else 0}, "
                  f"Eligibility trace total={eligibility_traces[-1]:.3f}")
    
    return {
        'stdp_activity': stdp_activity,
        'hebbian_activity': hebbian_activity,
        'eligibility_traces': eligibility_traces
    }
```

## Performance Optimization Tips

### 1. Memory Management
```python
# For long-running simulations
brain = pynlm.createBrain(config)

# Use checkpointing for long simulations
brain.save("checkpoint_start.bin")

# Run simulation
for step in range(10000):
    # ... simulation code ...
    
    # Periodically save checkpoints
    if step % 1000 == 0:
        brain.save(f"checkpoint_{step}.bin")

# Restore from checkpoint if needed
brain.load("checkpoint_5000.bin")
```

### 2. Batch Processing
```python
# Process multiple simulations efficiently
import concurrent.futures

def run_multiple_simulations(config, num_simulations=10):
    """Run multiple simulations in parallel"""
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        futures = []
        
        for i in range(num_simulations):
            # Create config copy for each simulation
            sim_config = pynlm.createDefaultConfig()
            # Customize config
            
            future = executor.submit(run_single_simulation, sim_config, i)
            futures.append(future)
        
        # Collect results
        results = []
        for future in concurrent.futures.as_completed(futures):
            results.append(future.result())
        
        return results

def run_single_simulation(config, sim_id):
    """Run a single simulation"""
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # ... run simulation ...
    
    return {
        'simulation_id': sim_id,
        'final_stage': agent.getDevelopmentalStage(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    }
```

This advanced usage guide provides comprehensive examples for working with the NLM system, including complex agent behaviors, sophisticated learning strategies, memory management, and performance optimization techniques.