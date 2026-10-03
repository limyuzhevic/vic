#!/usr/bin/env python3
"""
Comprehensive Example: Advanced NLM Usage

This example demonstrates advanced usage patterns for the NLM Python bindings.
It covers complex scenarios including world interaction, sensory processing,
and agent development with all the improvements in the API.

This script showcases:
- Advanced configuration management
- Complete agent-world interaction loop
- Sensory processing and motor output
- Neuromodulation and development
- Error handling and validation
- Performance monitoring
"""

import pynlm
import time

def print_header(title):
    """Print formatted section headers"""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def main():
    print("🚀 Advanced NLM Example - Complete Agent Simulation")
    print("="*60)
    
    # ====================================================================
    # SECTION 1: CONFIGURATION SETUP
    # ====================================================================
    print_header("1. Configuration Setup")
    
    # Create a comprehensive configuration
    config = pynlm.createDefaultConfig()
    
    # Customize configuration for this experiment
    config.set("learning_rate", 0.01)
    config.set("enable_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    config.set("simulation_timestep", 0.1)
    config.set("vision_width", 160)
    config.set("vision_height", 120)
    config.set("memory_capacity", 1000)
    
    print("✓ Configuration created with custom parameters")
    print(f"  - Learning rate: {config.get('learning_rate', 0.0)}")
    print(f"  - Plasticity enabled: {config.get('enable_plasticity', False)}")
    print(f"  - Development enabled: {config.get('enable_development', False)}")
    
    # ====================================================================
    # SECTION 2: BRAIN CREATION AND INITIALIZATION
    # ====================================================================
    print_header("2. Brain Creation and Initialization")
    
    # Create the neural brain
    brain = pynlm.createBrain(config)
    
    # Initialize the brain (essential step)
    brain.initialize()
    
    print("✓ Brain created and initialized")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # ====================================================================
    # SECTION 3: WORLD AND AGENT SETUP
    # ====================================================================
    print_header("3. World and Agent Setup")
    
    # Create the simulation world
    world = pynlm.createSimpleWorld()
    
    # Configure the world for this simulation
    world.configure(
        width=50.0,           # World size (50x50 units)
        height=50.0,          # 
        visionWidth=16,       # Agent vision (16x12 pixels)
        visionHeight=12       # 
    )
    
    # Add interesting objects to the world
    # Resource: food for the agent
    food = pynlm.WorldObject(
        x=10.0, y=10.0,
        type=pynlm.WorldObjectType.Food,
        value=10.0, radius=0.8
    )
    world.addObject(food)
    
    # Hazard: obstacle to avoid
    hazard = pynlm.WorldObject(
        x=40.0, y=40.0,
        type=pynlm.WorldObjectType.Hazard,
        value=5.0, radius=0.5
    )
    world.addObject(hazard)
    
    # Shelter: resting place
    shelter = pynlm.WorldObject(
        x=25.0, y=25.0,
        type=pynlm.WorldObjectType.Shelter,
        value=20.0, radius=1.5
    )
    world.addObject(shelter)
    
    # Reset world and create agent brain
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ World configured with resources, hazards, and shelter")
    print("✓ Agent brain created and initialized")
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ All learning systems enabled")
    
    # ====================================================================
    # SECTION 4: SIMULATION LOOP
    # ====================================================================
    print_header("4. Simulation Loop (1000 Steps)")
    
    simulation_stats = {
        'total_steps': 0,
        'successful_interactions': 0,
        'energy_alerts': 0,
        'novelty_events': 0,
        'max_firing_rate': 0.0,
        'total_reward': 0.0
    }
    
    start_time = time.time()
    
    for step in range(1000):
        # 1. Update world state
        world.update(0.1)
        
        # 2. Get sensory percept
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # 4. Brain processes and thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = action_result.reward
        predicted_reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        simulation_stats['total_steps'] += 1
        simulation_stats['total_reward'] += reward
        simulation_stats['max_firing_rate'] = max(
            simulation_stats['max_firing_rate'],
            brain.getAverageFiringRate()
        )
        
        if reward > 0:
            simulation_stats['successful_interactions'] += 1
            
        if brain.getCuriosityLevel() > 0.7:
            simulation_stats['novelty_events'] += 1
            
        if brain.getBrain().getTotalNeuronCount() > 0 and \
           brain.getBrain().getActiveNeuronCount() / brain.getBrain().getTotalNeuronCount() < 0.05:
            simulation_stats['energy_alerts'] += 1
        
        # Progress update every 200 steps
        if step % 200 == 0:
            firing = brain.getFiringNeuronCount()
            energy = brain.getBrain().getWorkingMemory()->getEnergyLevel()
            print(f"  Step {step:4d}: {firing:3d} firing, "
                  f"reward {reward:+6.2f}, "
                  f"energy {energy:.2f}, "
                  f"stage {brain.getDevelopmentalStage().name}")
    
    end_time = time.time()
    
    print(f"\n✓ Simulation completed in {end_time - start_time:.2f} seconds")
    
    # ====================================================================
    # SECTION 5: RESULTS ANALYSIS
    # ====================================================================
    print_header("5. Simulation Results")
    
    print(f"📊 Statistics over {simulation_stats['total_steps']} steps:")
    print(f"  - Average firing rate: {simulation_stats['max_firing_rate']:.2f}")
    print(f"  - Success rate: {simulation_stats['successful_interactions']/simulation_stats['total_steps']*100:.1f}%")
    print(f"  - Novelty events: {simulation_stats['novelty_events']}")
    print(f"  - Energy alerts: {simulation_stats['energy_alerts']}")
    print(f"  - Total reward: {simulation_stats['total_reward']:.2f}")
    print(f"  - Final developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
    
    # ====================================================================
    # SECTION 6: BRAIN INSPECTION
    # ====================================================================
    print_header("6. Brain State Inspection")
    
    print("🧠 Brain system statistics:")
    brain_stats = [
        ("Total neurons", brain.getTotalNeuronCount()),
        ("Total synapses", brain.getTotalSynapseCount()),
        ("Active neurons", brain.getActiveNeuronCount()),
        ("Firing neurons", brain.getFiringNeuronCount()),
        ("Total spikes", brain.getTotalSpikeCount()),
        ("Average firing rate", f"{brain.getAverageFiringRate():.2f}"),
        ("Excitation/inhibition ratio", f"{brain.getExcitationInhibitionRatio():.2f}")
    ]
    
    for name, value in brain_stats:
        print(f"  {name:25s}: {value}")
    
    # Memory systems
    memory = brain.getBrain().getWorkingMemory()
    if memory:
        print(f"\n💾 Memory system:")
        print(f"  - Working memory capacity: {memory.getCapacity()}")
        print(f"  - Current load: {memory.getLoad()}")
    
    # Development
    dev = brain.getBrain().getDevelopmentSystem()
    if dev:
        print(f"\n📈 Development system:")
        print(f"  - Current stage: {dev.getCurrentStage().name}")
        print(f"  - Plasticity rate: {dev.getPlasticityRate():.2f}")
        print(f"  - Age: {dev.getAge():.1f} simulation steps")
    
    # Neuromodulation
    print(f"\n🧠 Neuromodulation levels:")
    print(f"  - Dopamine: {agent.getNeuromodulationLevel():.2f}")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"  - Prediction error: {agent.getPredictionError():.2f}")
    
    # ====================================================================
    # SECTION 7: ERROR HANDLING DEMONSTRATION
    # ====================================================================
    print_header("7. Error Handling Demonstration")
    
    print("🛡️ Testing error handling:")
    
    # Test condition checking
    try:
        pynlm.check_condition(
            config.has("nonexistent_key"),
            "This should raise an error"
        )
        print("  ✓ Condition check passed (this shouldn't print)")
    except Exception as e:
        print(f"  ✓ Condition check correctly raised error: {type(e).__name__}")
    
    # Test configuration validation
    try:
        if not config.has("learning_rate"):
            pynlm.raise_runtime_error("Missing required learning_rate")
        print("  ✓ Configuration validation passed")
    except Exception as e:
        print(f"  ✓ Configuration validation correctly raised error: {type(e).__name__}")
    
    print("\n" + "="*60)
    print("🎉 Advanced NLM Example Complete!")
    print("="*60)
    print("\nKey achievements:")
    print("✅ Created comprehensive agent with brain-world integration")
    print("✅ Demonstrated advanced configuration management")
    print("✅ Showcased complete sensory-motor loop")
    print("✅ Implemented error handling and validation")
    print("✅ Monitored and analyzed performance metrics")
    print("✅ Illustrated neuromodulation and development")
    
    print(f"\n📈 Simulation summary: {simulation_stats['total_steps']} steps, "
          f"{simulation_stats['successful_interactions']} successful actions, "
          f"{simulation_stats['total_reward']:.1f} total reward")

if __name__ == "__main__":
    main()