# Context Manager Example
"""
Example demonstrating all context managers available.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Context Managers Deep Dive ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test simulation_step context manager
    print("1. Testing simulation_step context manager:")
    for i in range(3):
        with pynlm.simulation_step(world, brain, agent, 0.2) as result:
            print(f"   Step {i}: {result['spikes_generated']} spikes generated")
    
    # Test agent_episode context manager
    print("\n2. Testing agent_episode context manager:")
    with pynlm.agent_episode(world, brain, agent, max_steps=5, timestep=0.1) as (step_result, episode_data):
        print(f"   Episode started...")
        print(f"   Initial steps: {len(episode_data['steps'])}")
    
    print(f"   Episode completed with {episode_data['total_steps']} steps")
    print(f"   Total reward: {episode_data['total_reward']:.3f}")
    
    # Test world_session context manager
    print("\n3. Testing world_session context manager:")
    with pynlm.world_session(world, agent) as session_data:
        print(f"   Session started...")
        for step in range(5):
            world.update(0.1)
            if agent:
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
    
    print(f"   Session completed with {session_data['steps_processed']} steps")

if __name__ == "__main__":
    main()
