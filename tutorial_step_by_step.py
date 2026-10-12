#!/usr/bin/env python3
"""
tutorial_step_by_step.py

Step-by-Step Tutorial for NLM Python API
========================================

This is a comprehensive step-by-step tutorial that teaches users how to use the
improved NLM Python API effectively. It covers basic concepts, intermediate
techniques, and advanced features through practical examples.

Key Concepts Covered:
- API introduction and setup
- Basic brain configuration
- Agent setup and configuration
- Simulation loop implementation
- Learning process monitoring
- Performance analysis
- Optimization techniques

Learning Objectives:
1. Understand NLM Python API structure
2. Set up basic brain and agent configurations
3. Implement simulation loops
4. Monitor and analyze learning progress
5. Optimize performance
6. Use convenience functions effectively

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import numpy as np
from typing import List, Dict, Tuple

# Step 1: Introduction and Setup
print("=" * 70)
print("STEP 1: INTRODUCTION TO NLM PYTHON API")
print("=" * 70)

print("""
The NLM Python API provides enhanced bindings for the NLM (Neural Learning Machine)
framework, offering convenience functions and higher-level abstractions that
make neural simulation more accessible and powerful.

Key Benefits:
1. **Convenience Functions**: Pre-built functions for common operations
2. **Better Error Handling**: Improved exception management
3. **Type Safety**: Enhanced type conversions and validation
4. **Performance Optimization**: Built-in benchmarking and optimization tools
5. **Documentation**: Comprehensive help and examples

Let's start by setting up your environment...
""")

# Step 2: Basic Setup
print("\n" + "=" * 70)
print("STEP 2: BASIC SETUP AND CONFIGURATION")
print("=" * 70)

print("""
Let's create a basic brain configuration and initialize the system.
""")

# Import the API
import pynlm as nlm

# Create configuration
config = nlm.createDefaultConfig()
print(f"✓ Created default configuration")

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()
print(f"✓ Created and initialized brain with {brain.getTotalNeuronCount()} neurons")

# Create world
world = nlm.createStandardEnvironment()
print(f"✓ Created standard environment ({world.getWidth()}x{world.getHeight()} units)")

# Step 3: Advanced Configuration
print("\n" + "=" * 70)
print("STEP 3: ADVANCED CONFIGURATION WITH CONVENIENCE FUNCTIONS")
print("=" * 70)

print("""
Now let's use the convenience functions to create specialized configurations.
""")

# Create learning-optimized configuration
learning_config = nlm.createLearningConfig()
print(f"✓ Created learning-optimized configuration")

# Create simulation-optimized configuration
simulation_config = nlm.createSimulationConfig()
print(f"✓ Created simulation-optimized configuration")

# Create development-optimized configuration
development_config = nlm.createDevelopmentConfig()
print(f"✓ Created development-optimized configuration")

# Setup brain for learning
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, world)
print(f"✓ Set up brain for learning with neuromodulation")

# Step 4: Agent Configuration
print("\n" + "=" * 70)
print("STEP 4: AGENT CONFIGURATION")
print("=" * 70)

print("""
Create and configure agents with specialized capabilities.
""")

# Create different types of agents
standard_agent = nlm.createStandardAgent(brain)
exploration_agent = nlm.createExplorationAgent(brain)
learning_agent = nlm.createLearningAgent(brain)

print(f"✓ Created standard agent")
print(f"✓ Created exploration agent")
print(f"✓ Created learning agent")

# Configure neuromodulation
nlm.setupRewardModulation(learning_agent, learningRate=0.1)
nlm.setupCuriosityModulation(learning_agent, noveltyWeight=0.7, errorWeight=0.3)
nlm.setupPredictionErrorModulation(learning_agent, precisionWeight=0.5)

print(f"✓ Configured neuromodulation systems")

# Step 5: Simulation Loop Implementation
print("\n" + "=" * 70)
print("STEP 5: SIMULATION LOOP IMPLEMENTATION")
print("=" * 70)

print("""
Implement a complete simulation loop with monitoring.
""")

def run_simulation_step(agent, world, brain, step):
    """Execute one step of the simulation loop."""
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Process through brain
    brain.step(step)
    
    # Decode and execute action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

def run_complete_simulation(agent, world, brain, num_steps):
    """Run a complete simulation with progress monitoring."""
    print(f"  Running {num_steps}-step simulation...")
    
    start_time = time.time()
    
    for step in range(num_steps):
        run_simulation_step(agent, world, brain, step)
        
        # Monitor progress every 100 steps
        if (step + 1) % 100 == 0:
            progress = (step + 1) / num_steps * 100
            elapsed = time.time() - start_time
            steps_per_sec = (step + 1) / elapsed if elapsed > 0 else 0
            print(f"    Progress: {progress:.1f}%, "
                  f"Speed: {steps_per_sec:.1f} steps/sec")
    
    total_time = time.time() - start_time
    avg_speed = num_steps / total_time if total_time > 0 else 0
    
    return {
        'total_steps': num_steps,
        'total_time': total_time,
        'average_speed': avg_speed,
        'final_reward': world.getTotalReward()
    }

# Step 6: Learning Progress Monitoring
print("\n" + "=" * 70)
print("STEP 6: LEARNING PROGRESS MONITORING")
print("=" * 70)

print("""
Monitor learning progress using analysis functions.
""")

# Create simulation for learning monitoring
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, world)

agent = nlm.createLearningAgent(brain)
agent.initialize(world)

# Setup neuromodulation
nlm.setupRewardModulation(agent, learningRate=0.1)
nlm.setupCuriosityModulation(agent, noveltyWeight=0.7, errorWeight=0.3)

# Run learning simulation
learning_results = run_complete_simulation(agent, world, brain, 500)

print(f"  ✓ Simulation completed: {learning_results['total_steps']} steps")
print(f"  ✓ Average performance: {learning_results['average_speed']:.1f} steps/sec")
print(f"  ✓ Final reward: {learning_results['final_reward']:.2f}")

# Analyze learning progress
rewards = [learning_results['final_reward'] / learning_results['total_steps']] * learning_results['total_steps']
learning_progress = nlm.trackLearningProgress(rewards, evaluationInterval=100)
convergence = nlm.calculateConvergence(rewards, threshold=0.01, windowSize=50)

print(f"  ✓ Learning progress tracked")
print(f"  ✓ Convergence detected: {convergence}")

# Step 7: Performance Analysis
print("\n" + "=" * 70)
print("STEP 7: PERFORMANCE ANALYSIS")
print("=" * 70)

print("""
Analyze performance using benchmarking tools.
""")

# Benchmark brain performance
brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=200)
print(f"  ✓ Brain benchmark: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")

# Benchmark simulation performance
simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=100)
print(f"  ✓ Simulation benchmark: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")

# Step 8: Advanced Techniques
print("\n" + "=" * 70)
print("STEP 8: ADVANCED TECHNIQUES")
print("=" * 70)

print("""
Explore advanced features and optimization techniques.
""")

# Validation
brain_issues = nlm.validateBrainDynamics(brain, duration=50)
world_issues = nlm.validateWorldState(world)

print(f"  ✓ Brain validation: {'✓ Passed' if not brain_issues else '⚠ Issues found'}")
print(f"  ✓ World validation: {'✓ Passed' if not world_issues else '⚠ Issues found'}")

# Configuration optimization
optimized_config = nlm.optimizeForSpeed(learning_config)
config_comparison = nlm.compareConfigs(learning_config, optimized_config)

print(f"  ✓ Configuration optimization completed")

# Step 9: Real-world Example - Resource Collection
print("\n" + "=" * 70)
print("STEP 9: REAL-WORLD EXAMPLE - RESOURCE COLLECTION")
print("=" * 70)

print("""
Create a practical example of resource collection behavior.
""")

# Setup resource collection environment
resource_world = nlm.createResourceWorld(width=100.0, height=100.0, numResources=10, resourceRadius=3.0)
nlm.setupAgentStartPosition(resource_world, x=50.0, y=50.0)

# Add resources
for i in range(5):
    nlm.spawnResource(resource_world, x=np.random.uniform(20, 80), y=np.random.uniform(20, 80), value=np.random.uniform(5, 20))

# Create agent for resource collection
resource_brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(resource_brain, resource_world)
resource_agent = nlm.createStandardAgent(resource_brain)
resource_agent.initialize(resource_world)

# Run resource collection simulation
resource_results = run_complete_simulation(resource_agent, resource_world, resource_brain, 200)

print(f"  ✓ Resource collection simulation completed")
print(f"  ✓ Resources found: {resource_world.getResourceCount()}")
print(f"  ✓ Simulation time: {resource_results['total_time']:.2f} seconds")

# Step 10: Summary and Next Steps
print("\n" + "=" * 70)
print("STEP 10: SUMMARY AND NEXT STEPS")
print("=" * 70)

print("""
Congratulations! You've completed the step-by-step tutorial.

Key Concepts Learned:
1. ✅ NLM Python API structure and convenience functions
2. ✅ Basic and advanced configuration setup
3. ✅ Agent configuration and neuromodulation
4. ✅ Simulation loop implementation
5. ✅ Learning progress monitoring
6. ✅ Performance analysis and benchmarking
7. ✅ Advanced techniques and optimization
8. ✅ Real-world application example

Next Steps:
1. Explore example_learning_brain.py for detailed learning examples
2. Try example_development_brain.py for developmental processes
3. Use example_neural_analysis.py for advanced analysis
4. Create custom projects using example_template_project.py
5. Implement reproducible research with example_reproducible_research.py
6. Scale up with example_parallel_simulation.py
""")

print("\n" + "=" * 70)
print("TUTORIAL COMPLETE - YOU'RE NOW READY TO USE NLM PYTHON API!")
print("=" * 70)

print("""
The NLM Python API provides powerful tools for neural simulation and learning.
With these examples and the convenience functions, you can:

• Create sophisticated neural systems
• Implement learning algorithms
• Analyze neural activity
• Optimize performance
• Reproduce research results
• Scale to large simulations

Start building your own neural systems today!
""")

# Show available example files
print("\nAvailable Example Files:")
example_files = [
    "example_basic_brain.py - Basic brain creation and simulation",
    "example_agent_simulation.py - Complete agent simulation",
    "example_learning_brain.py - Learning with neuromodulation",
    "example_development_brain.py - Brain development processes",
    "example_world_interaction.py - Complex environment interaction",
    "example_neural_analysis.py - Neural activity analysis",
    "example_performance_benchmarking.py - Performance measurement",
    "recipe_learning_optimization.py - Learning optimization recipes",
    "recipe_agent_training.py - Agent training recipes",
    "example_template_project.py - Project template",
    "example_reproducible_research.py - Reproducible research",
    "example_parallel_simulation.py - Parallel simulation"
]

for file_info in example_files:
    print(f"  📄 {file_info}")

print(f"\nTotal examples available: {len(example_files)}")
print(f"Use any example as a starting point for your projects!")

if __name__ == "__main__":
    print("Tutorial completed successfully!")
else:
    print("Tutorial script executed as standalone script.")