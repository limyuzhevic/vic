#!/usr/bin/env python3
"""
Advanced NLM example: Agent navigation with world interaction

This example demonstrates advanced usage of the NLM Python bindings
including sensory processing, motor control, and world interaction.
"""

import random
from nlm import (
    Brain, Config, SimpleWorld, AgentBrain, Vision, Audio, InternalSignals,
    SensoryPercept, Action, ActionType, Reward,
    create_default_config, create_brain, create_simple_world,
    create_agent_brain, save_brain_checkpoint, get_brain_statistics
)

def create_random_sensory(percept: SensoryPercept, timestep: int):
    """Create random sensory data for testing"""
    # Create vision data (simulated camera input)
    vision_data = [random.uniform(0, 1) for _ in range(320 * 240 * 3)]
    vision = Vision()
    vision.setData(vision_data)
    
    # Create audio data (simulated sound)
    audio_data = [random.uniform(-1, 1) for _ in range(1000)]
    audio = Audio(sample_rate=44100)
    audio.setData(audio_data)
    
    # Create internal signals
    touch = InternalSignals()
    for _ in range(10):
        touch.addSignal(random.uniform(0, 1))
    
    internal = InternalSignals()
    for _ in range(5):
        internal.addSignal(random.uniform(-0.5, 0.5))
    
    proprioception = InternalSignals()
    for _ in range(3):
        proprioception.addSignal(random.uniform(-1, 1))
    
    # Combine into percept
    percept.setVision(vision)
    percept.setTouch(touch)
    percept.setInternal(internal)
    percept.setProprioception(proprioception)
    percept.setAudio(audio)
    
    return percept

class SimpleAgent:
    """Simple agent that interacts with the world"""
    
    def __init__(self, agent_brain: AgentBrain, world: SimpleWorld):
        self.agent_brain = agent_brain
        self.world = world
        self.energy = 100.0
        self.score = 0.0
        self.steps = 0
    
    def run_step(self) -> Action:
        """Run one step of the agent"""
        # Get sensory percept
        percept = self.world.get_sensory_percept()
        
        # Create random sensory data (in real scenario, this would come from world)
        random_percept = SensoryPercept()
        create_random_sensory(random_percept, self.steps)
        
        # Process sensory input
        self.agent_brain.processSensoryInput(random_percept)
        
        # Decode motor command
        action = self.agent_brain.decode_motor_command()
        
        # Apply the action
        self.world.applyMotorCommand(action, self.steps)
        
        # Get agent body for state update
        body = self.world.get_agent_body()
        self.energy = body.energy
        
        # Update score based on action
        if action.getType() == ActionType.Wait:
            self.score += 0.0  # No reward for waiting
        elif action.getType() in [ActionType.MoveForward, ActionType.MoveLeft, ActionType.MoveRight]:
            self.score += 0.5  # Small reward for movement
        
        self.steps += 1
        return action
    
    def get_state(self) -> dict:
        """Get current agent state"""
        return {
            'energy': self.energy,
            'score': self.score,
            'steps': self.steps,
            'developmental_stage': self.agent_brain.get_developmental_stage().name,
            'curiosity_level': self.agent_brain.get_curiosity_level(),
            'novelty_level': self.agent_brain.get_novelty_level(),
            'prediction_error': self.agent_brain.get_prediction_error(),
        }

def main():
    """Main demonstration function"""
    print("=== NLM Agent Navigation Example ===\n")
    
    # Create simulation components
    print("1. Creating simulation components...")
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    agent_brain = create_agent_brain(brain, world)
    
    # Configure world
    world.configure(640, 480, 320, 240)
    world.set_agent_start(100, 100)
    world.set_max_energy(100.0)
    
    # Initialize components
    print("2. Initializing components...")
    brain.initialize()
    agent_brain.initialize(world)
    
    # Create agent
    agent = SimpleAgent(agent_brain, world)
    
    # Run simulation
    print("3. Running agent simulation for 20 steps...")
    actions_taken = []
    
    for step in range(20):
        action = agent.run_step()
        actions_taken.append(action)
        
        if step % 5 == 0:
            state = agent.get_state()
            print(f"   Step {step}: Action={action.getName()}, Energy={state['energy']:.1f}, Score={state['score']:.1f}")
    
    # Display final results
    print("\n4. Final Results:")
    agent_state = agent.get_state()
    print(f"   Total steps: {agent_state['steps']}")
    print(f"   Final energy: {agent_state['energy']:.1f}")
    print(f"   Final score: {agent_state['score']:.1f}")
    print(f"   Developmental stage: {agent_state['developmental_stage']}")
    
    # Show action summary
    print("\n5. Action Summary:")
    action_counts = {}
    for action in actions_taken:
        action_name = action.getName()
        action_counts[action_name] = action_counts.get(action_name, 0) + 1
    
    for action_name, count in sorted(action_counts.items(), key=lambda x: x[1], reverse=True):
        print(f"   {action_name}: {count} times")
    
    # Save brain checkpoint
    print("\n6. Saving brain checkpoint...")
    metadata = {
        'episode': 1,
        'final_score': agent_state['score'],
        'agent_type': 'simple_agent'
    }
    success = save_brain_checkpoint(brain, 'agent_checkpoint.json', metadata)
    if success:
        print("   Checkpoint saved successfully!")
    else:
        print("   Warning: Failed to save checkpoint")
    
    # Show brain statistics
    print("\n7. Brain Statistics:")
    stats = get_brain_statistics(brain)
    print(f"   Total neurons: {stats['neuron_count']['total']}")
    print(f"   Total synapses: {stats['synapse_count']['total']}")
    print(f"   Average firing rate: {stats['activity']['average_firing_rate']:.3f}")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
