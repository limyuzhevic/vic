"""
Advanced NLM Simulation Techniques

This example demonstrates advanced features and techniques for using the enhanced
NLM Python bindings in production environments and complex simulation scenarios.
"""

from pynlm import AdvancedBrain, Config, World, AgentBrain, NLMTypes
import asyncio
import time


def simulation_monitoring():
    """Advanced simulation monitoring and diagnostics."""
    print("=== Advanced Simulation Monitoring ===\n")
    
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(
        max_steps=200,
        enable_monitoring=True,
        monitoring_interval=0.1
    )
    
    # Setup visualization if available
    brain.visualization.setup_plotting('matplotlib')
    
    print("Starting simulation with monitoring...")
    
    # Run episode with monitoring
    results = brain.run_episode(max_steps=100, verbose=True)
    
    # Get comprehensive diagnostics
    diagnostics = brain.get_diagnostics()
    print(f"\nComprehensive Diagnostics:")
    print(f"  Simulation State: {diagnostics['simulation_state']}")
    print(f"  Performance Stats: {diagnostics['performance']}")
    print(f"  Configuration Parameters: {len(diagnostics['configuration'])}")
    print(f"  Registered Callbacks: {len(diagnostics['callbacks'])}")
    
    # Generate report
    report = brain.monitoring.generate_report()
    print(f"\nPerformance Report:\n{report}")
    
    # Check for alerts
    status = brain.monitoring.get_system_status()
    print(f"\nSystem Status:")
    print(f"  Total Alerts: {status['alert_count']}")
    if status['warning_alerts']:
        print(f"  Warnings: {len(status['warning_alerts'])}")
    if status['critical_alerts']:
        print(f"  Critical: {len(status['critical_alerts'])}")


def configuration_validation():
    """Configuration validation and error handling."""
    print("\n=== Configuration Validation Example ===\n")
    
    config = Config()
    
    # Add custom validation rules
    def validate_positive_reward(value):
        errors = []
        if isinstance(value, (int, float)) and value < 0:
            errors.append("Reward must be non-negative")
        return errors
    
    config.add_validation_rule('reward_threshold', validate_positive_reward)
    config.add_description('reward_threshold', 'Minimum reward required for success')
    
    # Test valid configuration
    try:
        valid_config = Config().configure(
            max_steps=100,
            simulation_speed=1.0,
            reward_threshold=0.5
        )
        print("✓ Valid configuration accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    # Test invalid configuration
    try:
        invalid_config = Config().configure(
            max_steps=100,
            simulation_speed=1.0,
            reward_threshold=-0.5  # Invalid
        )
        print("✗ Invalid configuration should have been rejected")
    except Exception as e:
        print(f"✓ Invalid configuration correctly rejected: {e}")
    
    # Show validation errors
    test_params = {'max_steps': -100, 'reward_threshold': -0.5}
    errors = config.get_validation_errors(test_params)
    print(f"\nValidation errors for test params:")
    for error in errors:
        print(f"  - {error}")


def callback_system_advanced():
    """Advanced callback system with complex event handling."""
    print("\n=== Advanced Callback System ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Complex callback with state
    class CallbackState:
        def __init__(self):
            self.step_count = 0
            self.max_reward = float('-inf')
            self.high_reward_steps = []
    
    callback_state = CallbackState()
    
    def advanced_step_callback(state):
        callback_state.step_count += 1
        callback_state.max_reward = max(callback_state.max_reward, state.reward)
        
        if state.reward > 10:
            callback_state.high_reward_steps.append(state.step)
            print(f"  🎯 High reward at step {state.step}: {state.reward}")
        
        # Log every 10th step
        if state.step % 10 == 0:
            print(f"  📊 Step {state.step}: "
                  f"Active={state.neurons_active}, "
                  f"Reward={state.reward:.2f}")
    
    def before_episode_advanced(brain):
        callback_state.step_count = 0
        callback_state.max_reward = float('-inf')
        callback_state.high_reward_steps = []
        print("  🔄 Episode reset for advanced tracking")
    
    def after_episode_advanced(brain, state):
        print(f"  ✅ Episode complete: "
              f"Steps={state.step + 1}, "
              f"Max Reward={callback_state.max_reward:.2f}, "
              f"High Reward Events={len(callback_state.high_reward_steps)}")
    
    # Register advanced callbacks
    brain.add_callback('before_episode', before_episode_advanced)
    brain.add_callback('step', advanced_step_callback)
    brain.add_callback('after_episode', after_episode_advanced)
    
    # Run simulation
    print("Running simulation with advanced callbacks...")
    results = brain.run_episode(max_steps=50, verbose=True)
    
    print(f"\nCallback Summary:")
    print(f"  Total steps processed: {callback_state.step_count}")
    print(f"  Maximum reward achieved: {callback_state.max_reward:.2f}")
    print(f"  High reward events: {len(callback_state.high_reward_steps)}")
    print(f"  High reward steps: {callback_state.high_reward_steps}")


def continuous_simulation():
    """Continuous simulation with real-time monitoring."""
    print("\n=== Continuous Simulation ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Configure for continuous simulation
    brain.configure_simulation(
        simulation_speed=0.5,
        enable_monitoring=True,
        enable_visualization=True
    )
    
    brain.visualization.setup_plotting('matplotlib')
    
    print("Starting continuous simulation (10 seconds max)...")
    print("Press Ctrl+C to stop early\n")
    
    # Run continuous simulation with time limit
    try:
        brain.run_continuous(duration=10.0)
        print("\n✅ Continuous simulation completed")
    except KeyboardInterrupt:
        print("\n⏹️  Continuous simulation stopped by user")
    
    # Final diagnostics
    final_diagnostics = brain.get_diagnostics()
    print(f"\nFinal Simulation Stats:")
    print(f"  Duration: {final_diagnostics['simulation_state']['current_step'] / brain._config.get('simulation_speed', 1.0):.2f}s")
    print(f"  Current step: {final_diagnostics['simulation_state']['current_step']}")
    print(f"  Monitoring active: {brain._config.get('enable_monitoring', True)}")


def agent_development_simulation():
    """Agent development simulation with neuroplasticity."""
    print("\n=== Agent Development Simulation ===\n")
    
    brain, world, agent_brain = create_simulation()
    
    # Configure for development simulation
    brain.configure_simulation(
        max_steps=150,
        enable_development=True,
        enable_curiosity=True,
        enable_structural_plasticity=True,
        enable_reward_modulation=True,
        enable_monitoring=True
    )
    
    # Setup agent
    world.configure(width=200.0, height=200.0, vision_width=30, vision_height=30)
    
    # Add complex environment
    from pynlm import WorldObject
    
    # Resources
    resource1 = WorldObject(50, 50, NLMTypes.WorldObjectType.RESOURCE, value=100.0, radius=8.0)
    resource2 = WorldObject(150, 50, NLMTypes.WorldObjectType.RESOURCE, value=150.0, radius=6.0)
    
    # Hazards
    hazard1 = WorldObject(100, 100, NLMTypes.WorldObjectType.HAZARD, value=-75.0, radius=4.0)
    hazard2 = WorldObject(180, 150, NLMTypes.WorldObjectType.HAZARD, value=-100.0, radius=5.0)
    
    # Walls
    wall1 = WorldObject(0, 100, NLMTypes.WorldObjectType.WALL, value=0.0, radius=2.0)
    wall2 = WorldObject(200, 100, NLMTypes.WorldObjectType.WALL, value=0.0, radius=2.0)
    
    for obj in [resource1, resource2, hazard1, hazard2, wall1, wall2]:
        world.add_object(obj)
    
    # Initialize agent
    agent_brain.initialize(world)
    agent_brain.enable_development(True)
    agent_brain.enable_curiosity(True)
    agent_brain.enable_structural_plasticity(True)
    
    print(f"Development simulation configured:")
    print(f"  World size: {world.get_width()}x{world.get_height()}")
    print(f"  Agent enabled capabilities: Development, Curiosity, Plasticity")
    print(f"  Max steps: {brain._config.get('max_steps', 150)}")
    
    # Run development simulation
    print("\nRunning development simulation...")
    
    def development_step_callback(state):
        if state.step % 20 == 0:
            print(f"  🌱 Step {state.step}: "
                  f"Dev stage: {agent_brain.get_developmental_stage().value if hasattr(agent_brain, 'get_developmental_stage') else 'N/A'}")
    
    brain.add_callback('step', development_step_callback)
    
    results = brain.run_episode(max_steps=150, verbose=True)
    
    print(f"\nDevelopment Simulation Results:")
    print(f"  Success: {results['success']}")
    print(f"  Total Reward: {results['total_reward']:.2f}")
    print(f"  Monitoring Alerts: {results['metrics']['recent_alerts']} recent")
    
    # Show final agent state
    final_body = world.get_agent_body()
    print(f"\nFinal Agent State:")
    print(f"  Position: ({final_body.x:.1f}, {final_body.y:.1f})")
    print(f"  Orientation: {final_body.orientation:.2f} rad")
    print(f"  Energy: {final_body.energy:.2f}/{final_body.health:.2f}")
    print(f"  Age: {final_body.age:.2f}")


def error_handling_example():
    """Comprehensive error handling and recovery."""
    print("\n=== Error Handling and Recovery ===\n")
    
    brain = AdvancedBrain(verbose=True)
    
    # Test error handling with invalid configuration
    print("1. Testing configuration validation...")
    try:
        config = Config()
        config.configure(max_steps=-100)  # Invalid
        print("   ✗ Configuration validation failed (should have caught negative steps)")
    except Exception as e:
        print(f"   ✓ Configuration validation caught error: {type(e).__name__}")
    
    # Test callback error handling
    print("\n2. Testing callback error handling...")
    
    def failing_callback(state):
        raise ValueError("Simulated callback failure")
    
    def error_callback(error, brain):
        print(f"   🚨 Error handled gracefully: {type(error).__name__}: {error}")
    
    brain.add_callback('step', failing_callback)
    brain.add_callback('on_error', error_callback)
    
    try:
        # This should trigger the error callback
        results = brain.run_episode(max_steps=10, verbose=False)
        print(f"   ✓ Episode completed despite callback error: {results['success']}")
    except Exception as e:
        print(f"   ❌ Episode failed: {e}")
    
    # Test cleanup on error
    print("\n3. Testing resource cleanup...")
    try:
        with brain.simulation_context(duration=1.0) as sim:
            # Simulate an error
            raise RuntimeError("Simulated simulation error")
    except RuntimeError as e:
        print(f"   ✓ Context manager cleaned up: {e}")
    
    print(f"   ✓ Simulation state after error: {brain._simulation_running}")


async def async_workflow_example():
    """Asynchronous workflow management."""
    print("\n=== Async Workflow Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(max_steps=100)
    
    async def run_async_workflow():
        print("Starting async workflow...")
        
        # Step 1: Prepare
        print("  📋 Step 1: Preparing simulation...")
        brain.reset()
        
        # Step 2: Run async steps
        print("  ⚡ Step 2: Running async steps...")
        async_results = await brain.async_step(5)
        
        print(f"    Async step results: {len(async_results)} batches")
        
        # Step 3: Collect metrics
        print("  📊 Step 3: Collecting metrics...")
        metrics = brain.monitoring.get_performance_stats()
        
        # Step 4: Generate report
        print("  📄 Step 4: Generating report...")
        report = brain.monitoring.generate_report()
        print(f"    Report generated ({len(report)} characters)")
        
        return async_results
    
    # Execute async workflow
    results = await run_async_workflow()
    print(f"✅ Async workflow completed successfully")


def main():
    """Run all advanced examples."""
    print("NLM Advanced Examples")
    print("=" * 60)
    print()
    
    simulation_monitoring()
    configuration_validation()
    callback_system_advanced()
    continuous_simulation()
    agent_development_simulation()
    error_handling_example()
    
    # Run async example
    print("\n" + "=" * 60)
    print("Running Async Workflow Example...")
    import asyncio
    asyncio.run(async_workflow_example())
    
    print("\n" + "=" * 60)
    print("All advanced examples completed successfully!")


if __name__ == "__main__":
    main()