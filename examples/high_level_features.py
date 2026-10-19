"""
Example demonstrating the new high-level convenience methods and batch operations
for the NLM Python API.
"""

import sys
sys.path.insert(0, '.')

from pynlm import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    # New convenience classes
    BrainContextManager, SimulationRunner, BatchOperations, ConvenienceMethods,
    ConfigBuilder, SimulationBuilder, ExperimentLogger, ResultsAnalyzer
)

def example_context_managers():
    """Example using context managers for resource management."""
    print("=== Example: Context Managers ===")
    
    # Create configuration
    config = createDefaultConfig()
    
    # Using BrainContextManager for automatic resource management
    with BrainContextManager(config) as brainCtx:
        brain = brainCtx.getBrain()
        print(f"Brain created: {brain}")
        
        # Use the brain
        brain.logStatus()
    
    print("Brain context manager exited, resources cleaned up\n")
    
    # Example with SimulationRunner
    world = createSimpleWorld()
    world.configure(100, 100, 64, 64)
    
    agentBrain = createAgentBrain(brain)
    runner = SimulationRunner(brain, world, agentBrain)
    
    runner.start(100)
    print(f"Simulation started: maxSteps=100, currentStep=0")
    
    # Run a few steps
    for step in range(5):
        if not runner.step():
            break
        print(f"Step {runner.getCurrentStep()} completed")
    
    runner.stop()
    print(f"Simulation stopped after {runner.getCurrentStep()} steps\n")

def example_batch_operations():
    """Example using batch operations for multiple steps and sensory inputs."""
    print("=== Example: Batch Operations ===")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    
    # Batch step operations
    steps = [1, 2, 3, 4, 5, 10, 20, 50]
    times = [1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 5.0, 10.0]
    
    executed = BatchOperations.runSteps(brain, steps, times)
    print(f"Executed {len(executed)} steps: {executed}")
    
    # Batch sensory input processing
    from pynlm import Vision, Audio, InternalSignals
    
    vision = Vision(64, 64, 3)
    vision.setData([0.1] * 64 * 64 * 3)
    
    audio = Audio(44100, 44100 // 50)  # 50ms of audio
    audio.setData([0.0] * audio.getNumSamples())
    
    internal = InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(0.8)
    
    inputs = [vision, audio, internal]
    processed = BatchOperations.processSensoryInputs(brain, inputs)
    print(f"Processed {len(processed)} sensory inputs")
    
    # Get batch statistics
    stats = BatchOperations.getBatchStats(brain, 0, 50)
    print(f"Batch statistics: {stats}\n")

def example_convenience_methods():
    """Example using convenience methods for common workflows."""
    print("=== Example: Convenience Methods ===")
    
    # Configure brain from Python dict
    config_dict = {
        "neuronCount": "1000",
        "synapticConnectionProbability": "0.1",
        "maxFiringRate": "50.0",
        "enableSTDP": "true",
        "enableRewardModulation": "false"
    }
    
    config = createDefaultConfig()
    ConvenienceMethods.configureBrain(config, config_dict)
    print("Brain configured from dictionary")
    
    # Quick simulation
    print("Running quick simulation...")
    quickStats = ConvenienceMethods.quickSimulation(500, 100)
    print(f"Quick simulation stats: {quickStats}")
    
    # Run complete agent episode
    world = createSimpleWorld()
    world.configure(200, 200, 128, 128)
    world.setAgentStart(100, 100)
    
    brain = createBrain(config)
    agentBrain = createAgentBrain(brain)
    
    episodeStats = ConvenienceMethods.runEpisode(agentBrain, world, 1000)
    print(f"Episode statistics: {episodeStats}\n")

def example_builder_pattern():
    """Example using builder pattern for configuration."""
    print("=== Example: Builder Pattern ===")
    
    # ConfigBuilder example
    config = (ConfigBuilder()
              .set("neuronCount", 2000)
              .set("synapseCount", 50000)
              .set("simulationSteps", 500)
              .set("enableDevelopment", "true")
              .set("enableCuriosity", "true")
              .add("learningRate", 0.01)
              .add("learningRate", 0.001)
              .build())
    
    print("Configuration built using ConfigBuilder")
    
    # SimulationBuilder example
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(256, 256, 256, 256)
    
    agentBrain = createAgentBrain(brain)
    
    simulation = (SimulationBuilder()
                  .withWorldSize(256, 256)
                  .withConfig({"test": "value"})
                  .withBrain(brain)
                  .withAgentBrain(agentBrain)
                  .build())
    
    print(f"Simulation built with world size {simulation[0].getWidth()}x{simulation[0].getHeight()}")
    print(f"AgentBrain configured: {simulation[2] is not None}\n")

def example_experiment_management():
    """Example using experiment management utilities."""
    print("=== Example: Experiment Management ===")
    
    # Create experiment logger
    logger = ExperimentLogger()
    
    # Record metrics
    logger.recordMetric("accuracy", 0.85)
    logger.recordMetric("loss", 0.15)
    logger.recordMetric("learning_rate", 0.001)
    
    # Record step data
    config = createDefaultConfig()
    brain = createBrain(config)
    
    for step in range(0, 100, 10):
        logger.recordStep(brain, step)
    
    # Add notes
    logger.addNote("Experiment started with 1000 neurons")
    logger.addNote("Training completed successfully")
    
    # Analyze results
    experiment = logger.getExperiment()
    analysis = ResultsAnalyzer.analyze(experiment)
    print(f"Experiment analysis: {analysis}")
    
    metrics = logger.getMetrics()
    metricsAnalysis = ResultsAnalyzer.analyze(metrics)
    print(f"Metrics analysis: {metricsAnalysis}\n")

def example_advanced_workflow():
    """Example combining all new features for an advanced workflow."""
    print("=== Example: Advanced Workflow ===")
    
    # Build configuration
    config = (ConfigBuilder()
              .set("neuronCount", 5000)
              .set("enableDevelopment", "true")
              .set("enableCuriosity", "true")
              .build())
    
    # Create components
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(500, 500, 128, 128)
    world.setAgentStart(250, 250)
    
    agentBrain = createAgentBrain(brain)
    
    # Setup experiment logger
    logger = ExperimentLogger()
    logger.addNote("Advanced workflow test started")
    
    # Create simulation runner
    runner = SimulationRunner(brain, world, agentBrain)
    runner.start(200)
    
    # Run simulation with batch operations
    batchSteps = [i * 10 for i in range(1, 21)]  # Steps: 10, 20, 30, ..., 200
    BatchOperations.runSteps(brain, batchSteps)
    
    # Process batch sensory inputs
    from pynlm import Vision
    visionBatch = [Vision(128, 128, 3) for _ in range(5)]
    for i, vis in enumerate(visionBatch):
        vis.setData([float(j) / 255.0 for j in range(128 * 128 * 3)])
    
    BatchOperations.processSensoryInputs(brain, visionBatch)
    
    # Run episode
    episodeStats = ConvenienceMethods.runEpisode(agentBrain, world, 100)
    logger.addNote(f"Episode completed: {episodeStats['steps_completed']} steps")
    
    # Record final metrics
    logger.recordMetric("final_accuracy", 0.95)
    logger.recordMetric("training_time", 125.5)
    
    # Analyze everything
    experiment = logger.getExperiment()
    finalAnalysis = ResultsAnalyzer.analyze(experiment)
    print(f"Final experiment analysis: {finalAnalysis}")
    
    runner.stop()
    print("Advanced workflow completed successfully!\n")

if __name__ == "__main__":
    print("NLM Python API - High-Level Convenience Methods Examples")
    print("=" * 60)
    
    example_context_managers()
    example_batch_operations()
    example_convenience_methods()
    example_builder_pattern()
    example_experiment_management()
    example_advanced_workflow()
    
    print("All examples completed successfully!")
