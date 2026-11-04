/*
 * Enhanced NLM Python Bindings
 * 
 * This file provides an enhanced Python API for the NLM (Neural Learning Machine) neural simulation framework.
 * It wraps the C++ implementation with a more Pythonic interface while maintaining backward compatibility.
 * 
 * Key Features:
 * - Simplified configuration API with factory functions
 * - Enhanced wrapper classes for brain, agent, and world
 * - Preset configurations for common use cases
 * - Comprehensive error handling and validation
 * - Pythonic API design with fluent builders
 * - Backward compatibility with existing code
 * 
 * The enhanced bindings include:
 * 1. Enhanced configuration factory with presets
 * 2. Configuration builder for fluent API
 * 3. Enhanced brain wrapper with statistics and state management
 * 4. Enhanced agent wrapper with sensory processing and neuromodulation
 * 5. Enhanced world wrapper with configuration and percept access
 * 6. Convenience functions and example scenarios
 * 
 * Usage:
 * 
 * import nlm
 * 
 * # Create a brain with preset configuration
 * brain = nlm.create_enhanced_brain(preset="learning")
 * brain.initialize()
 * 
 * # Run simulation
 * brain.run_simulation(1000)
 * 
 * # Get statistics
 * stats = brain.get_statistics()
 * 
 * # Create world and agent
 * world = nlm.create_enhanced_world()
 * agent = nlm.create_enhanced_agent(brain)
 * 
 * # Initialize agent with world
 * agent.initialize(world)
 * 
 * # Run interaction loop
 * for step in range(100):
 *     world.update_world(0.1)
 *     percept = world.get_sensory_percept()
 *     agent.process_sensory_input(percept)
 *     brain.step(step)
 *     action = agent.decode_motor_command()
 *     world.apply_motor_command(action, world.get_simulation_time())
 *
 * Features:
 * - Configuration factories and builders
 * - Preset configurations (fast, detailed, learning, stable)
 * - Comprehensive error handling and validation
 * - Statistics collection and monitoring
 * - State save/load functionality
 * - Neuromodulation control
 * - Development tracking
 * - Pythonic API design
 * - Backward compatibility
 * - Example scenarios
 */

#include "EnhancedPythonBindings.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void initializeEnhancedBindings(py::module& m);

PYBIND11_MODULE(pynlm, m) {
    // Initialize both old and enhanced bindings
    initializeEnhancedBindings(m);
}