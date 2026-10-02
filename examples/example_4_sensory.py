import pynlm

def test_sensory_processing():
    """Test the complete sensory processing pipeline."""
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Create brain with sensory capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation and collect sensory data
    sensory_data = []
    motor_commands = []
    
    print("Testing sensory processing pipeline...")
    
    for step in range(100):
        # Update world (this creates the percept)
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Collect sensory data
        vision_data = percept.getVision()
        internal_signals = percept.getInternal()
        proprioception = percept.getProprioception()
        
        sensory_data.append({
            'step': step,
            'vision_data': vision_data,
            'internal_signals': internal_signals,
            'proprioception': proprioception,
            'timestamp': percept.getTimestamp()
        })
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        motor_commands.append(motor_cmd)
        
        # Apply to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Analyze sensory data
    print("\nSensory Processing Analysis:")
    print(f"Total steps: {len(sensory_data)}")
    print(f"Vision data shape: {len(sensory_data[0]['vision_data']) if sensory_data else 0} (expected 256 for 16x16x1)")
    print(f"Average internal signals: {sum(len(d['internal_signals']) for d in sensory_data if d['internal_signals']) / len(sensory_data) if sensory_data else 0:.3f}")
    
    # Check vision data ranges
    vision_values = []
    for d in sensory_data:
        vision_values.extend(d['vision_data'])
    
    print(f"Vision data range: {min(vision_values):.3f} to {max(vision_values):.3f}")
    
    # Check motor command distribution
    motor_counts = {}
    for cmd in motor_commands:
        motor_counts[str(cmd)] = motor_counts.get(str(cmd), 0) + 1
    
    print(f"\nMotor command distribution:")
    for cmd, count in motor_counts.items():
        percentage = (count / len(motor_commands)) * 100
        print(f"  {cmd}: {count} times ({percentage:.1f}%)")
    
    return sensory_data, motor_commands

if __name__ == "__main__":
    test_sensory_processing()