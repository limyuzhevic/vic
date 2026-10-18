"""
Example: Advanced Actions with Brain Integration

This example demonstrates advanced action systems with brain integration,
showing how to create, execute, and manage action sequences with validation.
"""

from nlm import (
    Brain, Config, create_brain, create_default_config,
    AdvancedActions, ActionSequence, ActionTemplate,
    ActionType, create_action, create_action_sequence, create_action_template,
    create_advanced_actions, brain_context, action_context, sequence_context
)

def main():
    """Run the advanced actions example."""
    print("=== Advanced Actions Example ===")
    
    # Create brain and advanced actions system
    print("1. Creating brain and advanced actions...")
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    advanced_actions = create_advanced_actions()
    print("   Created brain and advanced actions system")
    
    # Create action template
    print("2. Creating action template...")
    template = create_action_template(
        "MoveRobot", 
        "Robot movement with speed and direction parameters"
    )
    print(f"   Created template: {template}")
    
    # Register the template
    advanced_actions.register_template(template)
    print("   Registered action template")
    
    # Create action sequence
    print("3. Creating action sequence...")
    sequence = create_action_sequence("RobotMovement")
    print(f"   Created sequence: {sequence}")
    
    # Add actions to sequence using context manager
    print("4. Adding actions to sequence...")
    with action_context(ActionType.MoveForward) as move_action:
        sequence.add_action(move_action)
        print("   Added MoveForward action")
    
    with action_context(ActionType.TurnRight) as turn_action:
        sequence.add_action(turn_action)
        print("   Added TurnRight action")
    
    # Add more actions to the sequence
    sequence.add_action(create_action(ActionType.Look))
    print("   Added Look action")
    
    # Execute the action sequence
    print("5. Executing action sequence...")
    results = sequence.execute()
    print(f"   Sequence executed {len(results)} actions")
    
    # Display execution results
    print("6. Action execution results:")
    for i, result in enumerate(results):
        print(f"   Action {i}: {'Success' if result.success else 'Failed'} (reward: {result.reward})")
    
    # Create action from template
    print("7. Creating action from template...")
    template_action = advanced_actions.create_from_template(
        "MoveRobot",
        {"speed": 1.5, "direction": 90.0}
    )
    print(f"   Created action from template: {template_action}")
    
    # Execute action with validation
    print("8. Executing action with validation...")
    from nlm import ActionValidator
    validator = ActionValidator()
    action_result = advanced_actions.execute_action(template_action, validator)
    print(f"   Action result: {'Success' if action_result.success else 'Failed'}")
    
    # Get statistics
    print("9. Execution statistics:")
    stats = advanced_actions.get_stats()
    for key, value in stats.items():
        print(f"   {key}: {value}")
    
    # Demonstrate context manager for brain
    print("10. Using brain context manager...")
    with brain_context(config) as managed_brain:
        managed_brain.step(0)
        managed_brain.step(1)
        print("   Simulated 2 steps in managed context")
    
    print("\n=== Advanced Actions Example Complete ===")
    print("\nKey points demonstrated:")
    print("- Creating and managing action templates")
    print("- Creating and executing action sequences")
    print("- Using context managers for action and sequence creation")
    print("- Executing actions with validation")
    print("- Accessing execution statistics")
    print("- Using brain context manager for automatic resource management")

if __name__ == "__main__":
    main()
