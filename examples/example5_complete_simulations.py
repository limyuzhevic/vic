#!/usr/bin/env python3
"""
Example 5: Complete Simulation Examples
Demonstrates complete simulation workflows with error handling and real-world scenarios
"""
import pynlm
import random

def complete_simulation_examples():
    """Demonstrate complete simulation workflows"""
    print("=== Example 5: Complete Simulation Examples ===\n")
    
    # 1. Neurogenesis Simulation
    print("1. Neurogenesis Simulation (Brain Development):")
    
    def run_neurogenesis_simulation():
        """Simulate brain development over multiple phases"""
        print("   Running neurogenesis simulation...")
        
        # Create brain for neurogenesis
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("development.initial_neurons", 50)
        config.set("development.maturation_rate", 0.02)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.stdp.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Track developmental stages
        developmental_stages = []
        neuron_counts = []
        firing_rates = []
        
        for step in range(100):
            brain.step(step)
            
            # Record metrics
            stage = brain.getDevelopmentalStage()
            total_neurons = brain.getTotalNeuronCount()
            firing_rate = brain.getAverageFiringRate()
            
            developmental_stages.append(stage)
            neuron_counts.append(total_neurons)
            firing_rates.append(firing_rate)
            
            if step % 20 == 0:
                print(f"     Step {step}: Stage={stage}, Neurons={total_neurons}, Rate={firing_rate:.4f}")
        
        # Analyze results
        final_stage = developmental_stages[-1]
        avg_neuron_count = sum(neuron_counts) / len(neuron_counts)
        max_firing_rate = max(firing_rates)
        
        print(f"   Neurogenesis complete: Final stage={final_stage}, "
              f"Avg neurons={avg_neuron_count:.0f}, Max rate={max_firing_rate:.4f}")
        return brain
    
    neurogenesis_brain = run_neurogenesis_simulation()
    print()
    
    # 2. Learning Simulation
    print("2. Learning Simulation (Reinforcement Learning):")
    
    def run_learning_simulation():
        """Simulate reinforcement learning with world interaction"""
        print("   Running learning simulation...")
        
        # Create agent for learning
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", True)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableCuriosity(True)
        
        # Run learning episodes
        learning_metrics = {
            "steps": [],
            "rewards": [],
            "curiosity": [],
            "prediction_errors": [],
            "action_counts": {}
        }
        
        for step in range(100):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Use internal energy as reward
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Track metrics
            step_metrics = {
                "step": step,
                "reward": reward,
                "curiosity": agent.getCuriosityLevel(),
                "prediction_error": agent.getPredictionError(),
                "action": str(action)
            }
            
            learning_metrics["steps"].append(step_metrics["step"])
            learning_metrics["rewards"].append(step_metrics["reward"])
            learning_metrics["curiosity"].append(step_metrics["curiosity"])
            learning_metrics["prediction_errors"].append(step_metrics["prediction_error"])
            
            action_key = step_metrics["action"]
            learning_metrics["action_counts"][action_key] = learning_metrics["action_counts"].get(action_key, 0) + 1
            
            if step % 20 == 0:
                print(f"     Step {step}: Reward={reward:.3f}, "
                      f"Curiosity={step_metrics['curiosity']:.3f}, "
                      f"Prediction Error={step_metrics['prediction_error']:.3f}")
        
        # Analyze learning
        total_reward = sum(learning_metrics["rewards"])
        avg_curiosity = sum(learning_metrics["curiosity"]) / len(learning_metrics["curiosity"])
        
        print(f"   Learning complete: Total reward={total_reward:.3f}, "
              f"Avg curiosity={avg_curiosity:.3f}, Unique actions={len(learning_metrics['action_counts'])}")
        return brain, agent, world
    
    learning_brain, learning_agent, learning_world = run_learning_simulation()
    print()
    
    # 3. Prediction and Planning Simulation
    print("3. Prediction and Planning Simulation:")
    
    def run_prediction_planning_simulation():
        """Simulate prediction and planning capabilities"""
        print("   Running prediction/planning simulation...")
        
        # Create brain with prediction system
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        config.set("prediction.enabled", True)
        config.set("cognition.planning.enabled", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Track prediction-related metrics
        prediction_metrics = {
            "prediction_accuracy": [],
            "planning_decisions": [],
            "confidence_levels": []
        }
        
        for step in range(50):
            brain.step(step)
            
            # Use prediction system (if available)
            prediction_system = brain.getPredictionSystem()
            if prediction_system:
                # Get prediction error
                prediction_error = brain.getPredictionError()
                prediction_metrics["prediction_accuracy"].append(1.0 - min(prediction_error, 1.0))
            else:
                prediction_metrics["prediction_accuracy"].append(0.5)
            
            # Get planning recommendations (if available)
            # Note: Planning would be integrated through AgentBrain
            prediction_metrics["planning_decisions"].append(f"Decision at step {step}")
            prediction_metrics["confidence_levels"].append(random.uniform(0.6, 0.9))
            
            if step % 10 == 0:
                print(f"     Step {step}: Prediction accuracy={prediction_metrics['prediction_accuracy'][-1]:.3f}")
        
        # Analyze prediction performance
        avg_accuracy = sum(prediction_metrics["prediction_accuracy"]) / len(prediction_metrics["prediction_accuracy"])
        avg_confidence = sum(prediction_metrics["confidence_levels"]) / len(prediction_metrics["confidence_levels"])
        
        print(f"   Prediction planning complete: "
              f"Avg accuracy={avg_accuracy:.3f}, "
              f"Avg confidence={avg_confidence:.3f}")
        return brain
    
    prediction_brain = run_prediction_planning_simulation()
    print()
    
    # 4. Memory Consolidation Simulation
    print("4. Memory Consolidation Simulation:")
    
    def run_memory_consolidation_simulation():
        """Simulate memory consolidation over time"""
        print("   Running memory consolidation simulation...")
        
        # Create brain with memory systems
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("memory.episodic.enable", True)
        config.set("memory.working.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        # Simulate experience accumulation and consolidation
        consolidation_results = []
        
        for step in range(30):
            brain.step(step)
            
            # Simulate new experience
            experience = f"Experience {step}"
            if working_memory:
                working_memory.store(experience)
            
            # Get working memory stats
            if working_memory:
                working_capacity = working_memory.getCapacity()
                working_load = working_memory.getLoad()
            else:
                working_capacity = "N/A"
                working_load = "N/A"
            
            # Simulate episodic memory encoding
            if episodic_memory:
                episodic_memory.encode(experience, step)
            
            # Get episodic memory stats
            if episodic_memory:
                episodic_count = episodic_memory.getEventCount()
                episodic_size = episodic_memory.getMemorySize()
            else:
                episodic_count = "N/A"
                episodic_size = "N/A"
            
            consolidation_results.append({
                "step": step,
                "working_capacity": working_capacity,
                "working_load": working_load,
                "episodic_count": episodic_count,
                "episodic_size": episodic_size
            })
            
            if step % 10 == 0:
                print(f"     Step {step}: Working load={working_load if 'N/A' not in str(working_load) else 'N/A'}, "
                      f"Episodic events={episodic_count if 'N/A' not in str(episodic_count) else 'N/A'}")
        
        # Get final memory statistics
        if episodic_memory:
            final_episodes = episodic_memory.getEventCount()
            final_size = episodic_memory.getMemorySize()
        
        print(f"   Memory consolidation complete: "
              f"Final episodes={final_episodes if 'N/A' not in str(final_episodes) else 'N/A'}, "
              f"Final size={final_size if 'N/A' not in str(final_size) else 'N/A'}")
        return brain
    
    memory_brain = run_memory_consolidation_simulation()
    print()
    
    # 5. Comprehensive simulation summary
    print("5. Simulation Summary:")
    
    simulations = [
        ("Neurogenesis", neurogenesis_brain),
        ("Learning", learning_brain),
        ("Prediction/Planning", prediction_brain),
        ("Memory Consolidation", memory_brain)
    ]
    
    total_neurons = sum(sim.getTotalNeuronCount() for _, sim in simulations)
    total_spikes = sum(sim.getTotalSpikeCount() for _, sim in simulations)
    
    print(f"   Simulations completed: {len(simulations)}")
    print(f"   Total neurons across all simulations: {total_neurons}")
    print(f"   Total spikes across all simulations: {total_spikes}")
    
    print("\n=== Complete simulation examples complete ===\n")

if __name__ == "__main__":
    complete_simulation_examples()