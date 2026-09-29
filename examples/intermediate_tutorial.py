#!/usr/bin/env python3
"""
NLM Intermediate Tutorial

This tutorial builds upon the beginner concepts and introduces more advanced NLM features:

What you'll learn:
1. Agent-brain integration with world environments
2. Working memory and episodic memory usage
3. Prediction system implementation
4. Neuromodulation (reward, curiosity, novelty)
5. Developmental processes
6. Experiment design and metrics

Prerequisites:
- Python 3.8 or higher
- NLM Python bindings installed
- Basic understanding of neural networks
- Completed beginner_tutorial.py
"""

import sys
import os
import time
import numpy as np

# Add the src directory to the path for direct imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from sensory.SensoryInput import SensoryInput
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from core.Logger.Logger import NLM_LOG_INFO, NLM_LOG_ERROR
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    print("Make sure NLM is properly built and the Python bindings are installed.")
    sys.exit(1)

def print_header(title):
    """Print formatted header"""
    print("\n" + "=" * 80)
    print(f"  {title}")
    print("=" * 80)

def setup_experiment():
    """Setup a comprehensive experiment configuration"""
    print_header("Experiment Setup")
    
    print("\n1. Creating experiment configuration...")
    config = Config()
    
    print("\n2. Setting up for working memory experiment...")
    config.set("neuron_count", 1000)
    config.set("synapse_density", 0.1f)
    config.set("plasticity_rate", 0.1f)
    config.set("working_memory_capacity", 200)
    config.set("prediction_horizon", 5)
    config.set("development_enabled", true)
    config.set("reward_modulation_enabled", true)
    config.set("structural_plasticity_enabled", true)
    config.set("curiosity_enabled", true)
    config.set("novelty_threshold", 0.2f)
    
    print("\n3. Creating and initializing brain...")
    brain = Brain(config)
    if not brain.initialize():
        print("✗ Brain initialization failed!")
        return None, None, None
    
    print("\n4. Creating world environment...")
    world = SimpleWorld(64, 64)
    world.initialize()
    
    print("\n5. Creating agent brain...")
    agentBrain = AgentBrain(brain)
    agentBrain.initialize(world)
    
    print("\n6. Initial experiment setup complete!")
    print(f"   - Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Working memory capacity: {config.get('working_memory_capacity')}")
    print(f"   - Prediction horizon: {config.get('prediction_horizon')}")
    print(f"   - Development enabled: {config.get('development_enabled')}")
    
    return brain, world, agentBrain

def explore_memory_systems(brain):
    """Explore working memory and episodic memory systems"""
    print_header("Memory Systems Exploration")
    
    print("\n1. Examining working memory...")
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"   ✓ Working memory capacity: {working_memory.getCapacity()}")
        print(f"   ✓ Current entries: {working_memory.getEntryCount()}")
        
        print("\n2. Storing task-related information...")
        working_memory.store("current_goal", 1.0f)  # Goal: find object
        working_memory.store("exploration_level", 0.5f)  # Moderate exploration
        working_memory.store("time_step", 0.0f)  # Current time
        working_memory.store("reward_history", 0.3f)  # Recent reward average
        
        print("\n3. Testing memory retrieval and decay...")
        for i in range(5):
            goal = working_memory.retrieve("current_goal")
            print(f"   Time {i}: Goal={goal:.2f}, Entries={working_memory.getEntryCount()}")
            working_memory.store("time_step", float(i))  # Update time
            time.sleep(0.1)  # Simulate time passing
        
        print("\n4. Working memory experiment completed!")
    
    print("\n5. Examining episodic memory...")
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"   ✓ Episodic memory capacity: {episodic_memory.getCapacity()}")
        print(f"   ✓ Current episodes: {episodic_memory.getEpisodeCount()}")
        
        print("\n6. Storing experiences...")
        episodic_memory.store("object_found", 1.0f, 10.0f)  # action, reward
        episodic_memory.store("reward_prediction", 0.8f, 5.0f)
        episodic_memory.store("curiosity_drive", 0.6f, 7.5f)
        
        print("\n7. Testing episode retrieval...")
        episodes = episodic_memory.getAllEpisodes()
        for episode_id, episode in episodes.items():
            print(f"   Episode {episode_id}: Action={episode.action:.2f}, Reward={episode.reward:.2f}")
        
        print("\n8. Episodic memory experiment completed!")
    
    return working_memory, episodic_memory

def explore_prediction_system(brain):
    """Explore prediction system capabilities"""
    print_header("Prediction System Exploration")
    
    print("\n1. Checking prediction system...")
    prediction_system = brain.getPredictionSystem()
    if not prediction_system:
        print("   ✗ Prediction system not available!")
        return
    
    print("\n2. Testing temporal prediction...")
    # Learn that state A predicts state B
    prediction_system.learn("state_a", "state_b")
    prediction_system.learn("state_b", "state_c")
    prediction_system.learn("state_c", "state_a")
    
    print("\n3. Making predictions...")
    test_states = ["state_a", "state_b", "state_c", "unknown_state"]
    for state in test_states:
        prediction = prediction_system.predict(state)
        confidence = prediction_system.getConfidence(state)
        print(f"   State '{state}': Predicts '{prediction}' (confidence: {confidence:.2f})")
    
    print("\n4. Testing prediction error...")
    # Wrong prediction should have high error
    wrong_pred = prediction_system.predict("state_a")
    error = prediction_system.computePredictionError("state_c", wrong_pred)
    print(f"   Prediction error for wrong prediction: {error:.2f}")
    
    # Correct prediction should have low error
    correct_pred = "state_b"
    error = prediction_system.computePredictionError("state_b", correct_pred)
    print(f"   Prediction error for correct prediction: {error:.2f}")
    
    print("\n5. Prediction system exploration completed!")
    return prediction_system

def explore_neuromodulation(brain):
    """Explore neuromodulation systems"""
    print_header("Neuromodulation Exploration")
    
    print("\n1. Accessing neuromodulation systems...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    if not all([dopamine, curiosity, novelty, prediction_error]):
        print("   ✗ Some neuromodulation systems not available!")
        return
    
    print("\n2. Testing reward-based learning (Dopamine)...")
    print("   Applying reward prediction error signals...")
    for i in range(5):
        actual_reward = 1.0f if i % 2 == 0 else 0.0f
        predicted_reward = 0.5f
        dopamine.signalReward(actual_reward, predicted_reward)
        level = dopamine.getLevel()
        print(f"   Step {i}: Reward={actual_reward}, Prediction={predicted_reward}, Level={level:.2f}")
    
    print("\n3. Testing curiosity-driven exploration...")
    print("   Adjusting exploration motivation...")
    for i in range(5):
        curiosity_level = curiosity.getLevel()
        curiosity.increaseExploration()
        novelty_level = novelty.getLevel()
        
        print(f"   Step {i}: Curiosity={curiosity_level:.2f}, Novelty={novelty_level:.2f}")
        time.sleep(0.2)
    
    print("\n4. Testing prediction error...")
    print("   Computing prediction errors...")
    for i in range(3):
        # Simulate prediction error
        error = prediction_error.getLevel()
        print(f"   Prediction error level: {error:.2f}")
    
    print("\n5. Neuromodulation exploration completed!")
    return dopamine, curiosity, novelty, prediction_error

def explore_development(brain):
    """Explore developmental processes"""
    print_header("Developmental Processes Exploration")
    
    print("\n1. Checking developmental system...")
    development_system = brain.getDevelopmentSystem()
    if not development_system:
        print("   ✗ Development system not available!")
        return
    
    print("\n2. Examining current developmental stage...")
    current_stage = brain.getDevelopmentalStage()
    stage_names = {
        "INITIAL": "Initial (high plasticity)",
        "CRITICAL": "Critical period",
        "MATURATION": "Maturation",
        "ADULT": "Adult (stable)"
    }
    
    print(f"   Current stage: {current_stage}")
    print(f"   Stage description: {stage_names.get(str(current_stage), 'Unknown')}")
    
    print("\n3. Testing developmental progression...")
    print("   Simulating developmental progression...")
    
    for step in range(10):
        # Get current stage
        stage = brain.getDevelopmentalStage()
        
        # Get development system metrics
        if development_system:
            plasticity = development_system.getPlasticityModifier()
            maturity = development_system.getMaturityLevel()
            
            print(f"   Step {step}: Stage={stage}, Plasticity={plasticity:.2f}, Maturity={maturity:.2f}")
            
            # Apply developmental changes
            development_system.update(0.1f)  # 0.1 time step
            time.sleep(0.3)
        
        # Change developmental stage for demo
        if step % 5 == 4:
            brain.setDevelopmentalStage("CRITICAL")
    
    print("\n4. Developmental exploration completed!")
    return development_system

def run_learning_experiment(brain, world, agentBrain, duration=20):
    """Run a complete learning experiment"""
    print_header("Learning Experiment")
    
    print(f"\n1. Running {duration}-episode learning experiment...")
    print("   This experiment demonstrates learning in a dynamic environment.")
    
    total_reward = 0.0f
    episode_count = 0
    
    for episode in range(duration):
        print(f"\nEpisode {episode + 1}/{duration}:")
        
        # Reset world and agent for new episode
        world.reset()
        agentBrain.reset()
        
        # Initialize with random world state
        print("   Initializing world state...")
        # In a real experiment, world would have objects to find
        
        # Agent processes sensory input
        input_ = SensoryInput(10, 10)
        input_.setPixel(5, 5, 1.0f)  # Object at center
        
        agentBrain.processSensoryInput(input_)
        print("   ✓ Processed sensory input")
        
        # Run simulation steps
        for step in range(10):
            brain.step(step)
            
            if step % 3 == 0:
                action = brain.produceAction()
                if action:
                    world.applyAction(*action)  # Apply action to world
                    reward = world.getReward()
                    total_reward += reward
                    print(f"     Step {step}: Action={action.getType()}, Reward={reward:.2f}")
        
        # Apply neuromodulation based on episode outcome
        reward = world.getReward()
        predicted = agentBrain.getPredictionError()
        agentBrain.applyRewardModulation(reward, predicted)
        
        # Update development
        agentBrain.updateDevelopment(0.1f)
        
        episode_count += 1
        
        if (episode + 1) % 5 == 0:
            print(f"   Completed {episode + 1} episodes, Total reward: {total_reward:.2f}")
    
    print(f"\n2. Experiment completed!")
    print(f"   ✓ Total episodes: {episode_count}")
    print(f"   ✓ Total reward: {total_reward:.2f}")
    print(f"   ✓ Average reward per episode: {total_reward/episode_count:.2f}")
    
    return total_reward, episode_count

def explore_advanced_features(brain, world, agentBrain):
    """Explore advanced NLM features"""
    print_header("Advanced Features Exploration")
    
    print("\n1. Testing agent configuration options...")
    
    print("\n2. Enabling advanced features...")
    agentBrain.enableRewardModulation(true)
    agentBrain.enableStructuralPlasticity(true)
    agentBrain.enableDevelopment(true)
    agentBrain.enableCuriosity(true)
    
    print(f"\n3. Agent configuration:")
    print(f"   ✓ Reward modulation enabled: {agentBrain.isRewardModulationEnabled()}")
    print(f"   ✓ Structural plasticity enabled: {agentBrain.isStructuralPlasticityEnabled()}")
    print(f"   ✓ Development enabled: {agentBrain.isDevelopmentEnabled()}")
    print(f"   ✓ Curiosity enabled: {agentBrain.isCuriosityEnabled()}")
    
    print("\n4. Testing agent state...")
    neuromodulation = agentBrain.getNeuromodulationLevel()
    curiosity = agentBrain.getCuriosityLevel()
    novelty = agentBrain.getNoveltyLevel()
    prediction_error = agentBrain.getPredictionError()
    
    print(f"   Neuromodulation level: {neuromodulation:.2f}")
    print(f"   Curiosity level: {curiosity:.2f}")
    print(f"   Novelty level: {novelty:.2f}")
    print(f"   Prediction error: {prediction_error:.2f}")
    
    print("\n5. Advanced features exploration completed!")
    return neuromodulation, curiosity, novelty, prediction_error

def main():
    """Main intermediate tutorial function"""
    print_header("NLM Intermediate Tutorial")
    print("\nThis tutorial builds upon the beginner concepts and introduces:")
    print("  • Agent-brain integration with world environments")
    print("  • Working memory and episodic memory usage")
    print("  • Prediction system implementation")
    print("  • Neuromodulation (reward, curiosity, novelty)")
    print("  • Developmental processes")
    print("  • Experiment design and metrics")
    
    print("\nPrerequisites:")
    print("  • Python 3.8 or higher")
    print("  • NLM Python bindings installed")
    print("  • Completed beginner_tutorial.py")
    
    print("\nThis tutorial will run automatically unless interrupted.")
    print("\nPress Ctrl+C to skip any section.")
    
    try:
        # Setup experiment
        brain, world, agentBrain = setup_experiment()
        if not brain:
            print("\n✗ Failed to setup experiment. Exiting.")
            return
        
        # Explore different systems
        explore_memory_systems(brain)
        explore_prediction_system(brain)
        explore_neuromodulation(brain)
        explore_development(brain)
        
        # Run learning experiment
        total_reward, episodes = run_learning_experiment(brain, world, agentBrain, 15)
        
        # Explore advanced features
        explore_advanced_features(brain, world, agentBrain)
        
        # Summary
        print_header("Tutorial Summary")
        print("\nYou have successfully completed the intermediate NLM tutorial!")
        print("\nWhat you learned:")
        print("  1. ✓ Agent-brain integration with world environments")
        print("  2. ✓ Working memory for maintaining task information")
        print("  3. ✓ Episodic memory for storing experiences")
        print("  4. ✓ Prediction systems for temporal reasoning")
        print("  5. ✓ Neuromodulation (reward, curiosity, novelty)")
        print("  6. ✓ Developmental processes and plasticity")
        print("  7. ✓ Complete learning experiment")
        print("  8. ✓ Advanced configuration options")
        
        print(f"\nExperiment Results:")
        print(f"  • Completed {episodes} learning episodes")
        print(f"  • Total reward accumulated: {total_reward:.2f}")
        print(f"  • Average reward per episode: {total_reward/episodes:.2f}")
        
        print(f"\nNext Steps:")
        print("  • Try advanced_example.py for more complex scenarios")
        print("  • Experiment with different configuration parameters")
        print("  • Design your own experiments using this framework")
        
    except KeyboardInterrupt:
        print("\n\nTutorial interrupted by user.")
        print("You can continue with individual sections when ready.")
    except Exception as e:
        print(f"\n\nTutorial error: {e}")
        print("Please check your NLM installation and try again.")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n\nFatal error: {e}")
        print("Please check your NLM installation.")
        sys.exit(1)