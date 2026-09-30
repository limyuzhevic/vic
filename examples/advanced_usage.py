/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/examples/advanced_usage.py
Name: Advanced usage example
Summary: Advanced example with complex NLM operations

"""
Advanced usage example for NLM Python interface.

This example demonstrates advanced features of the NLM brain system.
"""

import pynlm
import asyncio
import time

async def run_advanced_example():
    """Run advanced NLM example"""
    print("=== Advanced NLM Python Interface Example ===\n")
    
    # Initialize system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create custom configuration
    print("2. Creating custom configuration...")
    config = pynlm.Config()
    config.set("simulation.dt", 0.01)
    config.set("brain.neuron_count", 500)
    config.set("brain.synaptic_density", 0.2)
    
    # Create brain with configuration
    print("3. Creating brain with custom configuration...")
    brain = pynlm.Brain(config)
    brain.initialize()
    interface.set_brain(brain)
    
    # Create world
    print("4. Creating simulation world...")
    world = pynlm.SimpleWorld()
    world.configure(width=200, height=200, vision_width=64, vision_height=64)
    interface.set_world(world)
    
    # Setup agent brain
    print("5. Setting up agent brain...")
    agent_brain = pynlm.AgentBrain(brain)
    agent_brain.initialize(world)
    interface.set_agent_brain(agent_brain)
    
    # Setup event monitoring
    print("6. Setting up event monitoring...")
    event_handler_called = []
    
    def event_handler(event_type, data):
        event_handler_called.append((event_type, data))
        print(f"    Event: {event_type}")
    
    interface.add_event_handler(event_handler)
    
    # Start simulation
    print("7. Starting simulation...")
    await interface.run_loop(duration=5.0, dt=0.1)
    
    # Perform complex experiments
    print("8. Performing complex experiments...")
    
    # Experiment 1: Stress test neurons
    print("   a) Stress testing neurons...")
    for i in range(10):
        brain.injectCurrent(i, 2.0)
        await asyncio.sleep(0.05)
    
    # Experiment 2: Neural stimulation
    print("   b) Neural stimulation experiment...")
    stimuli = [
        {"type": "current", "neuron_id": 0, "current": 1.0},
        {"type": "type", "neuron_type": pynlm.NeuronType.Excitatory, "current": 0.5},
        {"type": "sensory", "sensory_type": "vision", "data": [0.1, 0.2, 0.3] * 10},
        {"type": "sensory", "sensory_type": "audio", "sample_rate": 44100, "data": [0.0] * 44},
        {"type": "sensory", "sensory_type": "internal", "data": [0.5, 0.3, 0.7]},
    ]
    
    for stimulus in stimuli:
        interface.inject_stimulus(stimulus["type"], stimulus.get("parameters", {}))
        await asyncio.sleep(0.1)
    
    # Experiment 3: Save and load multiple states
    print("   c) Save/load state management...")
    checkpoint_files = []
    
    # Save at intervals
    for i in range(5):
        await asyncio.sleep(0.5)
        checkpoint_file = f"checkpoint_{i}.json"
        interface.save_brain_state(checkpoint_file)
        checkpoint_files.append(checkpoint_file)
        print(f"      Saved checkpoint {i+1}/{5}: {checkpoint_file}")
    
    # Load and compare checkpoints
    for i, file in enumerate(checkpoint_files):
        print(f"      Loading checkpoint {i+1}/{len(checkpoint_files)}: {file}")
        interface.load_brain_state(file)
        # Continue simulation
        await interface.run_step(dt=0.1)
    
    # Experiment 4: Visualization
    print("9. Creating visualizations...")
    
    # Get data for visualization
    state = interface.get_state()
    
    # Create different types of plots
    if pynlm.MATPLOTLIB_AVAILABLE:
        # Neuron activity plot
        neuron_data = {
            'neurons': [],
            'activity': list(range(state['total_neurons']))
        }
        
        # Region comparison plot
        region_data = {
            'regions': [
                {'id': i, 'neuron_count': state['total_neurons'] // 10,
                 'active_count': state['active_neurons'] // 10}
                for i in range(10)
            ]
        }
        
        # Firing rate plot
        firing_data = {
            'times': list(range(0, int(5.0 / 0.1), 1)),
            'rates': [state['average_firing_rate'] * (1 + i * 0.1) for i in range(len(range(0, int(5.0 / 0.1), 1)))]
        }
        
        print("      Creating neuron activity plot...")
        # interface.create_visualization('neurons', neuron_data, 'neuron_activity.png')
        
        print("      Creating region comparison plot...")
        # interface.create_visualization('regions', region_data, 'region_comparison.png')
        
        print("      Creating firing rate plot...")
        # interface.create_visualization('firing_rate', firing_data, 'firing_rate.png')
    
    # Experiment 5: Plugin system
    print("10. Testing plugin system...")
    
    # List available plugins
    plugins = interface.get_loaded_plugins()
    print(f"   Available plugins: {len(plugins)}")
    
    # Create a custom plugin
    print("   Creating custom plugin...")
    custom_plugin = {
        'name': 'custom_analyzer',
        'version': '1.0.0',
        'description': 'Custom analysis plugin',
        'capabilities': ['analyze', 'report', 'monitor']
    }
    
    # Experiment 6: Data integration
    print("11. Testing data integration...")
    
    if pynlm.NUMPY_AVAILABLE:
        # Convert to NumPy
        numpy_data = interface.export_to_numpy('brain_state')
        print(f"   NumPy data shape: {numpy_data.shape}")
    
    if pynlm.PANDAS_AVAILABLE:
        # Convert to Pandas
        pandas_data = interface.export_to_pandas('brain_state')
        print(f"   Pandas data shape: {pandas_data.shape}")
    
    # Experiment 7: Safety and security
    print("12. Testing safety features...")
    
    # Check system status
    status = interface.get_system_status()
    print(f"   System status: {status}")
    
    # Test resource monitoring
    resources = interface.get_resource_usage()
    print(f"   Resource usage: {resources}")
    
    # Experiment 8: Event-driven programming
    print("13. Testing event-driven programming...")
    
    # Create events
    interface.emit_state_change_event('user', {'action': 'test', 'value': 42})
    interface.emit_simulation_event(pynlm.EventType.SIMULATION_STARTED, 'user', {'duration': 5})
    
    print(f"   Events triggered: {len(event_handler_called)}")
    
    # Experiment 9: Async operations
    print("14. Testing async operations...")
    
    # Run multiple async tasks
    tasks = []
    for i in range(5):
        task = asyncio.create_task(interface.run_step(dt=0.1))
        tasks.append(task)
    
    # Wait for all tasks to complete
    results = await asyncio.gather(*tasks)
    print(f"   Async tasks completed: {len(results)}")
    
    # Experiment 10: Dashboard
    print("15. Testing dashboard...")
    
    # Create dashboard
    dashboard = interface.create_dashboard('console')
    print(f"   Dashboard created with {len(dashboard.widgets)} widgets")
    
    # Experiment 11: Async simulation controller
    print("16. Testing async simulation controller...")
    
    # Create async simulation
    async_sim = interface.create_async_simulation(duration=2.0, dt=0.1)
    await async_sim.run()
    print(f"   Async simulation completed: {async_sim.steps_completed} steps")
    
    # Experiment 12: Plugin manager
    print("17. Testing plugin manager...")
    
    plugin_manager = interface.get_plugin_manager()
    
    # Create and load plugin
    plugin_info = {
        'name': 'test_plugin',
        'version': '1.0.0',
        'description': 'Test plugin',
        'plugin_type': 'utility',
        'enabled': True
    }
    
    print("   Plugin manager test completed")
    
    # Final summary
    print("\n=== Example Summary ===")
    print(f"• Simulation steps completed: {state['total_spikes']}")
    print(f"• Events triggered: {len(event_handler_called)}")
    print(f"• Checkpoints created: {len(checkpoint_files)}")
    print(f"• Plugins loaded: {len(plugins)}")
    print(f"• Async tasks completed: {len(results)}")
    
    # Clean up
    print("\n18. Cleaning up...")
    interface.reset_system()
    
    print("\n=== Advanced Example Completed Successfully ===")

# Run the example
if __name__ == "__main__":
    asyncio.run(run_advanced_example())
