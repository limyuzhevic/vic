# Context Manager Example
"""
Example demonstrating context managers for simulation management.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Context Manager Example ===")
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Example using simulation_step context manager
    print("Running simulation with context manager:")
    for step_num in range(5):
        with pynlm.simulation_step(world, brain, agent, 0.1) as step_result:
            print(f"Step {step_num}: Spikes generated: {step_result['spikes_generated']}")
            print(f"           World time: {step_result['world_time']:.2f}")
            print(f"           Success: {step_result['success']}")
    
    # Example using agent_episode context manager
    print("\nRunning agent episode with context manager:")
    with pynlm.agent_episode(world, brain, agent, max_steps=10, timestep=0.1) as (step_result, episode_data):
        print(f"Episode running...")
    
    print(f"Episode completed with {episode_data['total_steps']} steps")
    print(f"Total reward: {episode_data['total_reward']:.3f}")

if __name__ == "__main__":
    main()
