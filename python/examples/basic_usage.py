"""
Basic NLM Neural Simulation Usage

This example demonstrates the essential features of the enhanced NLM Python bindings
for getting started with neural simulation development.
"""

from pynlm import create_simulation, AdvancedBrain, Config, World, AgentBrain


def basic_usage():
    """Basic simulation setup and execution."""
    print("=== Basic NLM Simulation Example ===\n")
    
    # Create simulation components
    brain, world, agent_brain = create_simulation()
    
    print(f"Brain: {brain}")
    print(f"World: {world}")
    print(f"AgentBrain: {agent_brain}\n")
    
    # Configure simulation
    brain.configure_simulation(
        max_steps=100,
        simulation_speed=1.0,
        enable_visualization=False,
        enable_monitoring=True
    )
    
    print("Configuration:")
    print(brain._config.summary())
    print()
    
    # Run a simple episode
    print("Running simulation episode...")
    results = brain.run_episode(max_steps=50, verbose=True)
    
    print(f"Episode Results:")
    print(f"  Steps completed: {results['steps_completed']}")
    print(f"  Total reward: {results['total_reward']}")
    print(f"  Success: {results['success']}")
    print(f"  Diagnostics: {results['diagnostics']}")
    print()
    
    # Show monitoring data
    print("Monitoring Report:")
    print(brain.monitoring.generate_report())


def configuration_example():
    """Advanced configuration examples."""
    print("=== Configuration Example ===\n")
    
    # Method 1: Create config and apply
    config = Config()
    config.configure(
        max_steps=500,
        simulation_speed=2.0,
        reward_threshold=0.5,
        enable_visualization=True,
        visualization_backend='matplotlib',
        enable_monitoring=True,
        monitoring_interval=0.05,
        enable_reward_modulation=True,
        enable_structural_plasticity=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    print("Custom Configuration:")
    print(config.summary())
    print()
    
    # Method 2: Create brain with custom config
    brain = AdvancedBrain(config=config, verbose=True)
    
    print(f"Brain with custom config: {brain}")
    print(f"Configured parameters: {brain._config.get_keys()}")
    print()


def context_manager_example():
    """Using context managers for resource management."""
    print("=== Context Manager Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Use context manager for automatic cleanup
    with brain.simulation_context() as sim:
        print(f"Simulation started in context manager: {sim}")
        print(f"Simulation running: {sim._simulation_running}")
        
        # Run a short episode
        results = sim.run_episode(max_steps=20, verbose=True)
        
        print(f"Episode completed: {results['success']}")
        print(f"Steps: {results['steps_completed']}")
    
    # Simulation should be automatically stopped after context
    print(f"Simulation after context: {sim._simulation_running}")
    print()


def callback_example():
    """Using callbacks for event-driven simulation control."""
    print("=== Callback Example ===\n")
    
    def before_episode_callback(brain):
        print("  [CALLBACK] Before episode: Resetting diagnostics")
        brain.reset_diagnostics()
    
    def step_callback(state):
        if state.step % 10 == 0:
            print(f"  [CALLBACK] Step {state.step}: "
                  f"Active neurons={state.neurons_active}, "
                  f"Reward={state.reward}")
    
    def after_episode_callback(brain, state):
        print(f"  [CALLBACK] After episode: "
              f"Total steps={state.step + 1}, "
              f"Final success={state.success}")
    
    def on_reward_callback(reward):
        if reward > 10:
            print(f"  [CALLBACK] High reward detected: {reward}")
    
    brain = AdvancedBrain(verbose=True)
    brain.add_callback('before_episode', before_episode_callback)
    brain.add_callback('step', step_callback)
    brain.add_callback('after_episode', after_episode_callback)
    
    # Run episode with callbacks
    results = brain.run_episode(max_steps=25, verbose=True)
    print(f"Final results: {results['success']}, steps={results['steps_completed']}")
    print()


def visualization_example():
    """Visualization and monitoring integration."""
    print("=== Visualization Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Setup visualization (matplotlib)
    if brain.visualization.setup_plotting('matplotlib'):
        print("Visualization backend (matplotlib) setup successful")
        
        # Create monitoring interface
        monitor = brain.visualization.create_training_monitor(brain)
        if monitor:
            print("Created training monitoring interface")
        
        # Create brain network plot
        network_plot = brain.visualization.create_brain_network_plot(brain)
        if network_plot:
            print("Created brain network visualization")
    else:
        print("Visualization setup failed (matplotlib not available)")
    
    print(f"Monitoring status: {brain.monitoring.get_performance_stats()}")
    print()


def multi_episode_example():
    """Running multiple episodes with results aggregation."""
    print("=== Multi-Episode Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Configure for multiple episodes
    brain.configure_simulation(
        max_steps=100,
        enable_visualization=False,
        enable_monitoring=True
    )
    
    # Run multiple episodes
    print("Running 5 episodes...")
    episode_results = brain.run_episodes(5, max_steps_per_episode=100)
    
    # Aggregate results
    total_steps = sum(r['steps_completed'] for r in episode_results)
    total_reward = sum(r['total_reward'] for r in episode_results)
    success_rate = sum(1 for r in episode_results if r['success']) / len(episode_results)
    
    print(f"Episode Results Summary:")
    print(f"  Total episodes: {len(episode_results)}")
    print(f"  Total steps across all episodes: {total_steps}")
    print(f"  Average steps per episode: {total_steps/len(episode_results):.1f}")
    print(f"  Total reward across all episodes: {total_reward:.2f}")
    print(f"  Average reward per episode: {total_reward/len(episode_results):.2f}")
    print(f"  Success rate: {success_rate*100:.1f}%")
    print()


def agent_example():
    """Complete agent-environment interaction."""
    print("=== Agent Example ===\n")
    
    # Create complete simulation
    brain, world, agent_brain = create_simulation()
    
    # Configure world
    world.configure(width=100.0, height=100.0, vision_width=20, vision_height=20)
    
    # Add some objects to world
    from pynlm import WorldObject, NLMTypes
    resource = WorldObject(50, 50, NLMTypes.WorldObjectType.RESOURCE, value=100.0, radius=5.0)
    hazard = WorldObject(80, 80, NLMTypes.WorldObjectType.HAZARD, value=-50.0, radius=3.0)
    
    world.add_object(resource)
    world.add_object(hazard)
    
    # Initialize agent
    agent_brain.initialize(world)
    
    print(f"Agent initialized in world: {world.get_width()}x{world.get_height()}")
    print(f"Agent position: ({world.agent_body.x}, {world.agent_body.y})")
    print(f"World objects: {len(world.objects)}")
    
    # Run simulation with agent
    brain.configure_simulation(max_steps=50, enable_monitoring=True)
    
    print("\nRunning agent simulation...")
    results = brain.run_episode(max_steps=50, verbose=True)
    
    print(f"\nAgent simulation results:")
    print(f"  Success: {results['success']}")
    print(f"  Total reward: {results['total_reward']}")
    print(f"  Monitoring: {results['metrics']['recent_alerts']} recent alerts")
    print()


def async_example():
    """Asynchronous operation example."""
    print("=== Async Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Run multiple steps asynchronously
    async def run_async_simulation():
        print("Starting async simulation...")
        
        # Run 3 steps asynchronously
        results = await brain.async_step(3)
        
        print("Async step results:")
        for i, result in enumerate(results):
            print(f"  Step {i+1}: {result}")
        
        return results
    
    # Run async function
    import asyncio
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    results = loop.run_until_complete(run_async_simulation())
    
    print(f"Async simulation completed with {len(results)} step results")
    print()


def main():
    """Run all examples."""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    print()
    
    basic_usage()
    configuration_example()
    context_manager_example()
    callback_example()
    visualization_example()
    multi_episode_example()
    agent_example()
    async_example()
    
    print("All examples completed successfully!")


if __name__ == "__main__":
    main()