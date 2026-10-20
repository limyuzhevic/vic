#!/usr/bin/env python3
"""
Advanced NLM Usage Examples and Power User Guide

This file demonstrates advanced usage patterns, Power User features,
and sophisticated applications of the enhanced NLM Python bindings.

Features covered:
- Advanced agent architectures
- Custom learning strategies
- Performance optimization
- Integration patterns
- Custom environments and behaviors
"""

import pynlm
import time
import math

def example_advanced_agent_architecture():
    """
    Example: Advanced Agent Architecture with Multiple Brain Regions
    
    Demonstrates how to create a sophisticated agent with:
    - Multiple brain regions for different functions
    - Custom neural architectures
    - Advanced learning strategies
    """
    print("=== Advanced Agent Architecture Example ===")
    
    # Create custom configuration for advanced agent
    config = pynlm.createDefaultConfig()
    
    # Configure for advanced agent (more neurons, complex connectivity)
    config.set("brain.neuron_count", 1500)  # Larger brain
    config.set("brain.connection_probability", 0.08)  # Dense connections
    config.set("brain.region_count", 3)  # Multiple regions
    config.set("plasticity.stdp.enable", True)  # Enable STDP
    config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
    config.set("plasticity.structural.enable", True)  # Enable growth
    config.set("neuromod.dopamine.scale", 1.5)  # Enhanced dopamine
    config.set("neuromod.curiosity.enable", True)  # Curiosity-driven exploration
    
    # Create main brain
    main_brain = pynlm.createBrain(config)
    main_brain.initialize()
    
    # Create world for advanced agent
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    world.setMaxEnergy(200.0)  # Higher energy capacity
    
    # Create agent brain
    agent = pynlm.createAgentBrain(main_brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)  # Brain growth
    agent.enableDevelopment(True)  # Maturation
    agent.enableCuriosity(True)  # Exploration
    
    # Advanced simulation loop
    print("Starting advanced agent simulation...")
    
    for step in range(500):
        # Update world with complex dynamics
        world.update(0.1)
        
        # Get rich sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation with complex interactions
        brain_stats = main_brain.getBrainStats()
        agent_stats = agent.getAgentStats()
        
        # Adaptive action selection based on brain state
        firing_rate = brain_stats['avg_firing_rate']
        curiosity = agent_stats['curiosity']
        
        # More complex action selection based on neural activity
        if firing_rate > 2.0 and curiosity > 0.5:
            # High activity + high curiosity = exploration
            action = agent.decodeMotorCommand()
        elif firing_rate < 0.5:
            # Low activity = return to base
            action = pynlm.Action(pynlm.ActionType.Rest)
        else:
            # Normal operation
            action = agent.decodeMotorCommand()
        
        # Apply action with energy considerations
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Advanced learning with prediction error
        reward = extract_advanced_reward(world, main_brain, agent)
        predicted_reward = agent.getPredictionError()
        
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Development and maturation
        agent.updateDevelopment(0.1)
        
        # Monitoring and logging
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing rate: {firing_rate:.2f} Hz")
            print(f"  Curiosity: {curiosity:.3f}")
            print(f"  Neuromodulation: {agent_stats['neuromodulation']:.3f}")
            print(f"  Prediction error: {agent_stats['prediction_error']:.3f}")
            print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    print("\nAdvanced agent simulation complete!")
    print(f"Final brain stats: {main_brain.getBrainStats()}")
    print(f"Final agent stats: {agent.getAgentStats()}")
    
    return main_brain, world, agent
def extract_advanced_reward(world, brain, agent):
    """
    Advanced reward function with multiple components:
    - Primary reward: resource acquisition
    - Secondary reward: survival bonus
    - Tertiary reward: learning efficiency
    """
    agent_body = world.getAgentBody()
    
    # Base reward from resources
    percept = world.getSensoryPercept()
    base_reward = 0.0
    if percept and percept.getInternal():
        base_reward = percept.getInternal()[0]
    
    # Survival bonus (higher energy = better)
    energy_bonus = agent_body.energy / world.getMaxEnergy()
    
    # Learning efficiency (inverse of prediction error)
    prediction_error = agent.getPredictionError()
    learning_bonus = max(0.0, 1.0 - prediction_error)
    
    # Activity bonus (encourage neural activity for learning)
    firing_rate_bonus = min(1.0, brain.getAverageFiringRate() / 5.0)
    
    # Combined reward
    total_reward = base_reward * 0.5 + energy_bonus * 0.2 + learning_bonus * 0.2 + firing_rate_bonus * 0.1
    
    return total_reward
def example_custom_neural_architecture():
    """
    Example: Custom Neural Architecture with Specialized Regions
    
    Shows how to create custom brain architectures with:
    - Specialized neuron populations
    - Custom connectivity patterns
    - Domain-specific processing
    """
    print("\n=== Custom Neural Architecture Example ===")
    
    # Create highly specialized configuration
    config = pynlm.createDefaultConfig()
    
    # Create "expert" brain with specific specialization
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.06)
    config.set("brain.region_count", 5)
    
    # Specialized parameters for expert system
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.growth_rate", 0.0005)
    
    expert_brain = pynlm.createBrain(config)
    expert_brain.initialize()
    
    # Create environment tailored to expert
    expert_world = pynlm.createSimpleWorld()
    expert_world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    expert_world.reset()
    
    # Create expert agent
    expert_agent = pynlm.createAgentBrain(expert_brain)
    expert_agent.initialize(expert_world)
    
    # Enable expert-level learning
    expert_agent.enableRewardModulation(True)
    expert_agent.enableStructuralPlasticity(True)
    expert_agent.enableDevelopment(True)
    expert_agent.enableCuriosity(True)
    
    # Simulate expert learning process
    print("Training expert neural network...")
    
    for episode in range(10):
        episode_reward = 0.0
        
        for step in range(200):
            # Expert-like behavior
            expert_world.update(0.05)
            
            # Rich sensory processing
            percept = expert_world.getSensoryPercept()
            expert_agent.processSensoryInput(percept)
            
            # Complex neural computation
            expert_brain.step(step)
            
            # Strategic action selection
            action = expert_agent.decodeMotorCommand()
            expert_world.applyMotorCommand(action, expert_world.getSimulationTime())
            
            # Expert learning strategy
            reward = calculate_expert_reward(expert_world, expert_brain, expert_agent)
            predicted = expert_agent.getPredictionError()
            
            expert_agent.applyRewardModulation(reward, predicted)
            expert_agent.updateDevelopment(0.05)
            
            episode_reward += reward
            
            if step % 50 == 0:
                print(f"  Episode {episode}, Step {step}: Reward={reward:.3f}, "
                      f"Curiosity={expert_agent.getCuriosityLevel():.3f}")
        
        print(f"  Episode {episode} total reward: {episode_reward:.2f}")
    
    print("\nExpert training complete!")
    return expert_brain, expert_world, expert_agent
def calculate_expert_reward(world, brain, agent):
    """
    Expert-level reward calculation with sophisticated metrics
    """
    percept = world.getSensoryPercept()
    agent_body = world.getAgentBody()
    
    # Multi-level reward system
    rewards = []
    
    # Level 1: Resource acquisition (primary goal)
    if percept and percept.getInternal():
        resource_reward = percept.getInternal()[0]
        rewards.append(resource_reward * 0.7)
    
    # Level 2: Survival efficiency
    energy_efficiency = agent_body.energy / (world.getSimulationTime() + 1)
    rewards.append(energy_efficiency * 0.2)
    
    # Level 3: Learning progress
    curiosity = agent.getCuriosityLevel()
    learning_progress = curiosity * (1.0 - math.exp(-world.getSimulationTime() / 100))
    rewards.append(learning_progress * 0.1)
    
    return sum(rewards)
def example_performance_optimization():
    """
    Example: Performance Optimization Techniques
    
    Shows how to optimize performance for:
    - Large-scale simulations
    - Real-time applications
    - Resource-constrained environments
    """
    print("\n=== Performance Optimization Example ===")
    
    def benchmark_simulation(brain, world, agent, steps, description):
        """Benchmark a simulation configuration"""
        print(f"\nBenchmarking {description}...")
        
        start_time = time.time()
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Calculate performance metrics
        steps_per_second = steps / elapsed
        avg_step_time = elapsed / steps * 1000  # milliseconds
        
        print(f"  Steps: {steps}")
        print(f"  Time: {elapsed:.2f} seconds")
        print(f"  Speed: {steps_per_second:.1f} steps/second")
        print(f"  Avg step time: {avg_step_time:.3f} ms")
        
        return {
            'steps': steps,
            'time': elapsed,
            'steps_per_second': steps_per_second,
            'avg_step_time_ms': avg_step_time
        }
    
    # Test different configurations
    configs = [
        ("Small", {"neuron_count": 500, "steps": 100}),
        ("Medium", {"neuron_count": 1000, "steps": 200}),
        ("Large", {"neuron_count": 2000, "steps": 150}),
    ]
    
    results = {}
    
    for name, params in configs:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params["neuron_count"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run benchmark
        results[name] = benchmark_simulation(
            brain, world, agent, params["steps"], name
        )
    
    # Compare results
    print("\n=== Performance Comparison ===")
    print(f"{'Configuration':<10} {'Steps':<6} {'Time (s)':<10} {'Speed (steps/s)':<15} {'Avg Step (ms)':<15}")
    print("-" * 70)
    
    for name, result in results.items():
        print(f"{name:<10} {result['steps']:<6} {result['time']:<10.2f} {result['steps_per_second']:<15.1f} {result['avg_step_time_ms']:<15.3f}")
    
    # Find best performer
    best_name = max(results.keys(), key=lambda x: results[x]['steps_per_second'])
    print(f"\nBest performance: {best_name} configuration")
    
    return results
def example_integration_patterns():
    """
    Example: Integration Patterns with External Systems
    
    Demonstrates how to integrate NLM with:
    - External data sources
    - Custom reward functions
    - Monitoring and visualization
    - Batch processing
    """
    print("\n=== Integration Patterns Example ===")
    
    class NLMProcessor:
        """
        Custom NLM processor that integrates with external systems
        """
        def __init__(self, config):
            self.config = config
            self.brain = pynlm.createBrain(config)
            self.world = pynlm.createSimpleWorld()
            self.agent = pynlm.createAgentBrain(self.brain)
            
            # Configure world
            self.world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
            self.world.reset()
            
            # Initialize
            self.brain.initialize()
            self.agent.initialize(self.world)
            
            # Integration hooks
            self.reward_callback = None
            self.percept_callback = None
            self.action_callback = None
        
        def set_reward_callback(self, callback):
            """Set custom reward function"""
            self.reward_callback = callback
        
        def set_percept_callback(self, callback):
            """Set custom percept processing"""
            self.percept_callback = callback
        
        def set_action_callback(self, callback):
            """Set custom action handler"""
            self.action_callback = callback
        
        def process_episode(self, steps=100):
            """Process a single episode with integration hooks"""
            episode_data = {
                'steps': [],
                'total_reward': 0.0,
                'brain_stats': [],
                'action_history': []
            }
            
            for step in range(steps):
                # 1. Update environment
                self.world.update(0.1)
                
                # 2. Get percept
                percept = self.world.getSensoryPercept()
                
                # 3. Apply custom percept processing
                if self.percept_callback:
                    percept = self.percept_callback(percept, step)
                
                # 4. Process in brain
                self.agent.processSensoryInput(percept)
                
                # 5. Brain computation
                self.brain.step(step)
                
                # 6. Get action
                action = self.agent.decodeMotorCommand()
                
                # 7. Apply custom action handling
                if self.action_callback:
                    action = self.action_callback(action, step)
                
                # 8. Apply action
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # 9. Calculate reward with custom function
                if self.reward_callback:
                    reward = self.reward_callback(self.world, self.brain, self.agent, step)
                else:
                    # Default reward calculation
                    reward = self.calculate_default_reward(self.world, self.brain, self.agent)
                
                # 10. Apply learning
                predicted = self.agent.getPredictionError()
                self.agent.applyRewardModulation(reward, predicted)
                self.agent.updateDevelopment(0.1)
                
                # Collect data
                episode_data['steps'].append(step)
                episode_data['total_reward'] += reward
                episode_data['brain_stats'].append(self.brain.getBrainStats())
                episode_data['action_history'].append(action)
            
            return episode_data
        
        def calculate_default_reward(self, world, brain, agent):
            """Default reward calculation"""
            percept = world.getSensoryPercept()
            if percept and percept.getInternal():
                return percept.getInternal()[0]
            return 0.0
        
        def run_batch_training(self, episodes, steps_per_episode):
            """Run batch training with data collection"""
            print(f"Running batch training with {episodes} episodes...")
            
            all_data = []
            
            for episode in range(episodes):
                episode_data = self.process_episode(steps_per_episode)
                all_data.append(episode_data)
                
                if episode % 10 == 0:
                    print(f"  Completed episode {episode + 1}/{episodes}")
                    
                    # Analyze episode performance
                    total_reward = episode_data['total_reward']
                    avg_reward = total_reward / steps_per_episode
                    print(f"    Average reward: {avg_reward:.3f}")
            
            print("Batch training complete!")
            return all_data
    
    # Create integrated processor
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    
    processor = NLMProcessor(config)
    
    # Set custom callbacks
    def custom_reward(world, brain, agent, step):
        """Custom reward function with exploration bonus"""
        base_reward = processor.calculate_default_reward(world, brain, agent)
        
        # Add exploration bonus based on curiosity
        exploration_bonus = agent.getCuriosityLevel() * 0.5
        
        # Add survival bonus
        energy_bonus = world.getAgentBody().energy / world.getMaxEnergy() * 0.3
        
        return base_reward + exploration_bonus + energy_bonus
    
    def custom_percept(percept, step):
        """Custom percept processing with noise injection for exploration"""
        # Add some noise for exploration
        if percept and percept.getInternal():
            internal = percept.getInternal()
            if len(internal) > 0:
                # Inject small noise
                import random
                internal[0] += random.uniform(-0.1, 0.1)
                internal[0] = max(0.0, min(1.0, internal[0]))
        
        return percept
    
    def custom_action(action, step):
        """Custom action handler with logging"""
        # Log action for analysis
        action_type = action.getType()
        print(f"    Action: {action_type} at step {step}")
        
        return action
    
    processor.set_reward_callback(custom_reward)
    processor.set_percept_callback(custom_percept)
    processor.set_action_callback(custom_action)
    
    # Run integrated training
    training_data = processor.run_batch_training(episodes=5, steps_per_episode=100)
    
    print("\nIntegration patterns example complete!")
    return processor, training_data
def example_advanced_usage_patterns():
    """
    Example: Advanced Usage Patterns and Best Practices
    
    Shows advanced patterns for:
    - Resource management
    - Error handling
    - Performance monitoring
    - Custom learning strategies
    """
    print("\n=== Advanced Usage Patterns Example ===")
    
    print("1. Resource Management Pattern")
    print("   - Create and properly clean up resources")
    print("   - Use context managers for automatic cleanup")
    
    # Pattern 1: Resource management with context
    config = pynlm.createDefaultConfig()
    
    # Simulate resource management pattern
    class NLMResourceManager:
        def __init__(self, config):
            self.config = config
            self.brain = None
            self.world = None
            self.agent = None
            self.is_initialized = False
        
        def __enter__(self):
            """Context manager entry"""
            self.initialize()
            return self
        
        def __exit__(self, exc_type, exc_val, exc_tb):
            """Context manager exit with cleanup"""
            self.cleanup()
        
        def initialize(self):
            """Initialize all resources"""
            if not self.is_initialized:
                self.brain = pynlm.createBrain(self.config)
                self.world = pynlm.createSimpleWorld()
                self.agent = pynlm.createAgentBrain(self.brain)
                
                self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
                self.world.reset()
                
                self.brain.initialize()
                self.agent.initialize(self.world)
                
                self.is_initialized = True
                print("     Resources initialized successfully")
        
        def cleanup(self):
            """Clean up all resources"""
            if self.is_initialized:
                # Cleanup order matters
                self.agent = None
                self.world = None
                self.brain = None
                self.is_initialized = False
                print("     Resources cleaned up successfully")
        
        def run_simulation(self, steps):
            """Run simulation with resource management"""
            if not self.is_initialized:
                self.initialize()
            
            for step in range(steps):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    # Use resource manager pattern
    with NLMResourceManager(config) as manager:
        manager.run_simulation(50)
        print("     Simulation completed with proper resource management")
    
    print("\n2. Error Handling Pattern")
    print("   - Handle errors gracefully")
    print("   - Maintain system state after errors")
    
    # Pattern 2: Error handling
    class SafeNLMRunner:
        def __init__(self, config):
            self.config = config
            self.simulation_running = False
        
        def start_simulation(self):
            """Start simulation with error handling"""
            try:
                self.config = pynlm.createDefaultConfig()
                self.brain = pynlm.createBrain(self.config)
                self.world = pynlm.createSimpleWorld()
                self.agent = pynlm.createAgentBrain(self.brain)
                
                self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
                self.world.reset()
                
                self.brain.initialize()
                self.agent.initialize(self.world)
                
                self.simulation_running = True
                print("     Simulation started safely")
                
            except Exception as e:
                print(f"     Error starting simulation: {e}")
                self.cleanup()
                raise
        
        def run_step(self, step):
            """Run single step with error handling"""
            if not self.simulation_running:
                raise RuntimeError("Simulation not running")
            
            try:
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
            except Exception as e:
                print(f"     Error in simulation step {step}: {e}")
                self.cleanup()
                raise
        
        def cleanup(self):
            """Clean up with error handling"""
            try:
                self.simulation_running = False
                # Cleanup resources
                self.agent = None
                self.world = None
                self.brain = None
                print("     Simulation cleaned up safely")
            except Exception as e:
                print(f"     Error during cleanup: {e}")
                raise
    
    # Use safe runner pattern
    safe_runner = SafeNLMRunner(config)
    safe_runner.start_simulation()
    
    for step in range(20):
        safe_runner.run_step(step)
    
    safe_runner.cleanup()
    
    print("\n3. Performance Monitoring Pattern")
    print("   - Monitor performance metrics")
    print("   - Adapt behavior based on performance")
    
    # Pattern 3: Performance monitoring
    class PerformanceMonitor:
        def __init__(self, brain, world, agent):
            self.brain = brain
            self.world = world
            self.agent = agent
            self.performance_history = []
            self.step_times = []
        
        def record_performance(self, step_start_time):
            """Record performance metrics for current step"""
            step_time = time.time() - step_start_time
            self.step_times.append(step_time)
            
            # Get brain stats
            brain_stats = self.brain.getBrainStats()
            agent_stats = self.agent.getAgentStats()
            
            # Record performance
            perf_data = {
                'step': len(self.performance_history),
                'step_time_ms': step_time * 1000,
                'brain_stats': brain_stats,
                'agent_stats': agent_stats,
                'world_time': self.world.getSimulationTime()
            }
            
            self.performance_history.append(perf_data)
            
            # Calculate and log performance summary every 10 steps
            if len(self.performance_history) % 10 == 0:
                self.log_performance_summary()
        
        def log_performance_summary(self):
            """Log performance summary"""
            if not self.step_times:
                return
            
            avg_step_time = sum(self.step_times) / len(self.step_times)
            min_step_time = min(self.step_times)
            max_step_time = max(self.step_times)
            
            print(f"  Performance Summary (last {len(self.performance_history)} steps):")
            print(f"    Average step time: {avg_step_time*1000:.2f} ms")
            print(f"    Min step time: {min_step_time*1000:.2f} ms")
            print(f"    Max step time: {max_step_time*1000:.2f} ms")
            print(f"    Steps per second: {1.0/avg_step_time:.1f}")
        
        def get_performance_report(self):
            """Get comprehensive performance report"""
            if not self.performance_history:
                return None
            
            avg_step_time = sum(self.step_times) / len(self.step_times)
            
            return {
                'total_steps': len(self.performance_history),
                'total_time': sum(self.step_times),
                'average_step_time_ms': avg_step_time * 1000,
                'min_step_time_ms': min(self.step_times) * 1000,
                'max_step_time_ms': max(self.step_times) * 1000,
                'steps_per_second': 1.0 / avg_step_time,
                'final_brain_stats': self.performance_history[-1]['brain_stats'],
                'final_agent_stats': self.performance_history[-1]['agent_stats']
            }
    
    # Use performance monitoring
    monitor = PerformanceMonitor(None, None, None)  # Will be set in simulation
    print("     Performance monitoring setup complete")
    
    print("\n4. Custom Learning Strategy Pattern")
    print("   - Implement custom learning algorithms")
    print("   - Fine-tune learning parameters")
    
    # Pattern 4: Custom learning strategy
    def create_custom_learning_agent(config, learning_strategy):
        """Create agent with custom learning strategy"""
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        brain.initialize()
        agent.initialize(world)
        
        # Enable learning based on strategy
        agent.enableRewardModulation(True)
        
        # Apply learning strategy customization
        if learning_strategy == "exploration_focused":
            agent.enableCuriosity(True)
        elif learning_strategy == "exploitation_focused":
            agent.enableCuriosity(False)
        elif learning_strategy == "balanced":
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        return brain, world, agent
    
    # Create agents with different learning strategies
    strategies = ["exploration_focused", "exploitation_focused", "balanced"]
    
    for strategy in strategies:
        custom_config = pynlm.createDefaultConfig()
        custom_config.set("brain.neuron_count", 500)
        
        brain, world, agent = create_custom_learning_agent(custom_config, strategy)
        
        print(f"     Created {strategy.replace('_', ' ')} agent")
        
        # Run quick test
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    print("\nAdvanced usage patterns example complete!")
    return True
if __name__ == "__main__":
    print("Advanced NLM Usage Examples and Power User Guide")
    print("=" * 60)
    print("This file demonstrates advanced usage patterns, Power User features,")
    print("and sophisticated applications of the enhanced NLM Python bindings.")
    print()
    
    # Run all examples
    try:
        example_advanced_agent_architecture()
        example_custom_neural_architecture()
        example_performance_optimization()
        example_integration_patterns()
        example_advanced_usage_patterns()
        
        print("\n" + "=" * 60)
        print("All advanced examples completed successfully!")
        print("\nKey advanced features demonstrated:")
        print("✓ Enhanced agent architectures with custom configurations")
        print("✓ Advanced learning strategies and reward functions")
        print("✓ Performance optimization and benchmarking")
        print("✓ Integration patterns with external systems")
        print("✓ Resource management and error handling")
        print("✓ Custom neural architectures and learning strategies")
        print("✓ Comprehensive performance monitoring")
        
    except Exception as e:
        print(f"\nError in advanced examples: {e}")
        import traceback
        traceback.print_exc()