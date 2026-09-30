#!/usr/bin/env python3
"""
Quick Start Guide for NLM Python Bindings - Enhanced Validation and Error Handling

This script provides a quick introduction to the 36 new validation and error handling
functions added to the NLM Python bindings. It walks through the most common use cases
and provides practical examples for getting started.
"""

import pynlm as nlm

def quick_start():
    print("NLM Python Bindings - Quick Start Guide")
    print("=" * 60)
    print()
    print("This guide introduces the enhanced validation and error handling")
    print("capabilities added to the NLM Python bindings.")
    print()
    
    # Set up validation
    nlm.error_handling.set_validation_config(
        default_level=nlm.ValidationLevel.Thorough,
        enable_logging=True,
        throw_on_error=False,
        throw_on_warning=False
    )
    
    print("1. Creating NLM Components")
    print("-" * 30)
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Configuration created")
    print("✓ Brain created")
    print("✓ World created")
    print("✓ Agent created and initialized")
    print()
    
    print("2. Basic Validation")
    print("-" * 30)
    
    # Parameter validation
    params = [0.1, 0.5, 0.9, 0.3]
    param_result = nlm.validation.validate_parameters(params, "neural initialization")
    print(f"Parameter validation: {param_result.isValid()} - {param_result.message}")
    
    # Configuration validation
    brain_config_result = nlm.validation.validate_brain_config(config)
    print(f"Brain config validation: {brain_config_result.isValid()} - {brain_config_result.message}")
    
    # World validation
    world_result = nlm.validation.validateWorldState(world)
    print(f"World state validation: {world_result.isValid()} - {world_result.message}")
    print()
    
    print("3. Safe Operations")
    print("-" * 30)
    
    # Safe step
    step_success = nlm.validation.safe_step(brain, 1, 0.1)
    print(f"Safe step: {step_success}")
    
    # Safe brain operation
    def brain_operation():
        brain.step(2)
        return "Operation completed"
    
    operation_success = nlm.validation.safe_brain_operation(brain, brain_operation)
    print(f"Safe brain operation: {operation_success}")
    print()
    
    print("4. Error Recovery")
    print("-" * 30)
    
    # Validate and safe operation in one
    combined_result = nlm.validation.validate_and_safe_step(brain, 1, 0.1)
    print(f"Combined validation and safe step: {combined_result.isValid()}")
    
    # Fix configuration issues
    bad_config = nlm.createDefaultConfig()
    bad_config.set("learningRate", 1.5)  # Invalid
    bad_config.set("neuronCount", 50)   # Invalid
    
    fixed = nlm.validation.fix_config_issues(bad_config)
    print(f"Configuration fixed: {fixed}")
    print(f"  After fix - learningRate: {bad_config.get('learningRate')}")
    print()
    
    print("5. Data Validation")
    print("-" * 30)
    
    # Validate sensory data
    percept = world.getSensoryPercept()
    sensory_result = nlm.validation.validateSensoryData(percept, "perception")
    print(f"Sensory data validation: {sensory_result.isValid()}")
    
    # Validate action
    action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.5])
    action_result = nlm.validation.validateAction(action, "movement")
    print(f"Action validation: {action_result.isValid()}")
    print()
    
    print("6. State Validation")
    print("-" * 30)
    
    # Validate brain state
    brain_state_result = nlm.validation.validateBrainState(brain)
    print(f"Brain state validation: {brain_state_result.isValid()}")
    
    # Validate agent state
    agent_state_result = nlm.validation.validateAgentState(agent)
    print(f"Agent state validation: {agent_state_result.isValid()}")
    print()
    
    print("7. Error Logging")
    print("-" * 30)
    
    # Create invalid data to generate warnings/errors
    invalid_params = [0.1, float('nan'), 0.3]
    invalid_result = nlm.validation.validate_parameters(invalid_params, "invalid test")
    print(f"Invalid parameters validation: {not invalid_result.isValid()}")
    print(f"  Errors found: {len(invalid_result.errors)}")
    print(f"  Warnings found: {len(invalid_result.warnings)}")
    print()
    
    print("8. Validation Levels")
    print("-" * 30)
    
    config_level = nlm.createDefaultConfig()
    config_level.set("learningRate", 0.5)
    
    for level in [nlm.ValidationLevel.Basic, nlm.ValidationLevel.Thorough, nlm.ValidationLevel.Strict]:
        result = nlm.validation.validate_config_values(config_level, level)
        print(f"{level} validation: {result.isValid()} - {result.message}")
    
    print()
    print("=" * 60)
    print("Quick Start Complete!")
    print()
    print("Key Points:")
    print("• All 36 validation functions are now available in nlm.validation module")
    print("• Error handling and recovery functions provide safe operations")
    print("• Validation results include comprehensive error messages and warnings")
    print("• Configuration issues can be automatically fixed")
    print("• Error logging helps with debugging and monitoring")
    print("• Different validation levels (Basic/Thorough/Strict) for different needs")
    print()
    print("For more examples, see:")
    print("• demo_validation_functions.py - Comprehensive demonstration")
    print("• test_validation_functions.py - Unit tests")
    print("• ENHANCED_FEATURES_SUMMARY.md - Detailed documentation")

if __name__ == "__main__":
    quick_start()
