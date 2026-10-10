cmake_minimum_required(VERSION 3.16)

# Core module
add_subdirectory(core)

# Neural system modules
add_subdirectory(brain)
add_subdirectory(dynamics)
add_subdirectory(plasticity)

# Cognitive and behavioral modules
add_subdirectory(cognition)
add_subdirectory(memory)

# Perceptual and motor modules
add_subdirectory(sensory)
add_subdirectory(motor)
add_subdirectory(prediction)

# Environmental and world modules
add_subdirectory(environment)
add_subdirectory(world)

# Development and neuromodulation
add_subdirectory(neuromodulation)
add_subdirectory(development)

# Agent and visualization
add_subdirectory(agent)
add_subdirectory(visualization)

# Experiments
add_subdirectory(experiments)