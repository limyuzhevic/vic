# NLM (Neural Learning Machine) Python Bindings
# Complete Python API for the C++ NLM neural simulation framework
#
# This module provides comprehensive access to NLM's advanced neural computing capabilities
# through a clean, intuitive Python interface. The bindings expose the complete Phase 6
# integrated brain architecture including all cognitive, memory, and neuromodulation systems.
#
# Key Features:
# - Full brain simulation with spiking neural networks
# - Integrated memory systems (working, episodic, associative)
# - Advanced prediction and cognition modules
# - Neuromodulation (dopamine, curiosity, novelty detection)
# - Developmental brain stages
# - World interaction with agents
# - Structural plasticity and learning
# - Checkpointing and state persistence
#
# Usage Example:
#
#     import pynlm
#
#     # Create configuration
#     config = pynlm.createDefaultConfig()
#     config.set("brain.neuron_count", 1000)
#     config.set("brain.synapse_density", 0.1)
#
#     # Create brain and initialize
#     brain = pynlm.createBrain(config)
#     brain.initialize()
#
#     # Create agent and world
#     world = pynlm.createSimpleWorld()
#     world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
#     world.reset()
#
#     agent = pynlm.createAgentBrain(brain)
#     agent.initialize(world)
#
#     # Run simulation
#     for step in range(100):
#         world.update(0.1)
#         percept = world.getSensoryPercept()
#         agent.processSensoryInput(percept)
#         brain.step(step)
#         action = agent.decodeMotorCommand()
#         world.applyMotorCommand(action, world.getSimulationTime())
#
#         if step % 10 == 0:
#             print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
#
# Result: Your brain thought 42 times
#
# For more examples and detailed API documentation, see:
# - HOW_TO_USE.md - Complete usage guide
# - easy_usage.md - Beginner-friendly examples
# - docs/ARCHITECTURE.md - System architecture details
#
# This module is part of the NLM research project investigating
# brain-inspired artificial intelligence and computational neuroscience.