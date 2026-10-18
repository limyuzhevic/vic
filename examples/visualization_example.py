"""
Example: Visualization and Brain State Exploration

This example demonstrates visualization capabilities and brain state exploration,
showing how to create, configure, and use visualization systems.
"""

from nlm import (
    Brain, Config, create_brain, create_default_config,
    SimpleWorld, create_simple_world,
    AdvancedVisualization, VisualizationConfig,
    create_advanced_config, DynamicConfigManager,
    brain_context, world_context, NLMError
)

def main():
    """Run the visualization example."""
    print("=== Visualization and Brain State Exploration Example ===")
    
    # Create world and visualization
    print("1. Creating world and visualization...")
    world = create_simple_world(100.0, 100.0)
    
    # Create visualization config
    viz_config = VisualizationConfig()
    viz_config.backend = "opengl"
    viz_config.enable_real_time = True
    viz_config.render_mode = "solid"
    viz_config.color_scheme = "thermal"
    viz_config.refresh_rate = 30.0
    
    print(f"   Created visualization config: {viz_config}")
    
    # Create advanced visualization
    advanced_viz = AdvancedVisualization()
    advanced_viz.set_config(viz_config)
    print("   Created advanced visualization")
    
    # Create brain and visualize
    print("2. Creating brain with visualization...")
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    print(f"   Created brain: {brain}")
    print(f"   Visualization enabled: {advanced_viz.get_config().enable_real_time}")
    print(f"   Visualization backend: {advanced_viz.get_config().backend}")
    print(f"   Visualization refresh rate: {advanced_viz.get_config().refresh_rate} FPS")
    
    # Create neural visualization data
    print("3. Creating neural visualization data...")
    from nlm import NeuronVisualization, SynapseVisualization, RegionVisualization
    
    neurons = [
        NeuronVisualization(0.0, 0.0, 0.0, 0.5, 0.2, "#FF0000", True, False),
        NeuronVisualization(1.0, 0.0, 0.0, 0.8, 0.1, "#00FF00", False, True),
        NeuronVisualization(0.0, 1.0, 0.0, 0.6, 0.3, "#0000FF", True, False),
    ]
    
    synapses = [
        SynapseVisualization(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.7, "#808080", True),
        SynapseVisualization(1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.5, "#808080", False),
    ]
    
    regions = [
        RegionVisualization(0.0, 0.0, 0.0, 2.0, 2.0, 2.0, "#808080", 0.5, "Cortex"),
        RegionVisualization(3.0, 0.0, 0.0, 2.0, 2.0, 2.0, "#FF8000", 0.3, "Hippocampus"),
    ]
    
    print(f"   Created {len(neurons)} neurons for visualization")
    print(f"   Created {len(synapses)} synapses for visualization")
    print(f"   Created {len(regions)} regions for visualization")
    
    # Update visualization
    print("4. Updating visualization with neural data...")
    advanced_viz.update_neural_data(neurons, synapses, regions)
    print("   Visualization updated with neural data")
    
    # Get visualization statistics
    print("5. Visualization statistics:")
    viz_stats = advanced_viz.get_stats()
    print(f"   FPS: {viz_stats.fps}")
    print(f"   Triangles rendered: {viz_stats.trianglesRendered}")
    print(f"   Vertices rendered: {viz_stats.verticesRendered}")
    print(f"   Draw calls: {viz_stats.drawCalls}")
    print(f"   Memory usage: {viz_stats.memoryUsage} MB")
    print(f"   Update time: {viz_stats.updateTime:.3f} seconds")
    
    # Test visualization modes
    print("6. Testing visualization modes...")
    from nlm import VisualizationMode
    
    original_mode = advanced_viz.get_visualization_mode()
    print(f"   Original mode: {original_mode}")
    
    # Toggle visualization mode
    advanced_viz.toggle_visualization_mode()
    current_mode = advanced_viz.get_visualization_mode()
    print(f"   Toggled mode: {current_mode}")
    
    # Set a specific visualization mode
    advanced_viz.set_visualization_mode(VisualizationMode.ActivityHeatmap)
    print(f"   Set mode to ActivityHeatmap: {advanced_viz.get_visualization_mode()}")
    
    # Test real-time updates
    print("7. Testing real-time updates...")
    advanced_viz.start_real_time_updates()
    print("   Started real-time updates")
    
    # Simulate some real-time activity
    for i in range(3):
        neurons[0].activation = 0.1 + i * 0.3
        neurons[1].activation = 0.9 - i * 0.2
        advanced_viz.update_neural_data(neurons, synapses, regions)
        time.sleep(0.1)
    
    # Check if real-time updates are working
    if advanced_viz.is_real_time_updates_enabled():
        print("   Real-time updates are working")
    
    advanced_viz.stop_real_time_updates()
    print("   Stopped real-time updates")
    
    # Save visualization
    print("8. Saving visualization...")
    if advanced_viz.save_current_view("neural_network.png"):
        print("   Visualization saved as 'neural_network.png'")
    
    # Export visualization in different formats
    print("9. Testing export formats...")
    from nlm import ExportFormat
    
    # Test PNG export
    if advanced_viz.save_current_view("neural_network.png", ExportFormat.PNG):
        print("   PNG export successful")
    
    # Test JSON export
    if advanced_viz.save_current_view("neural_network.json", ExportFormat.JSON):
        print("   JSON export successful")
    
    # Create configuration management
    print("10. Testing configuration management...")
    config_manager = DynamicConfigManager()
    
    # Update configuration
    config_manager.update_config("visualization.refresh_rate", 60.0)
    config_manager.update_config("visualization.backend", "vulkan")
    
    print("   Updated visualization configuration")
    print("   Refresh rate: 60.0 FPS")
    print("   Backend: vulkan")
    
    # Create configuration snapshot
    config_manager.create_snapshot("default_viz_config")
    print("   Created configuration snapshot")
    
    # Restore from snapshot
    config_manager.restore_snapshot("default_viz_config")
    print("   Restored configuration from snapshot")
    
    print("\n=== Visualization Example Complete ===")
    print("\nKey points demonstrated:")
    print("- Creating and configuring visualization systems")
    print("- Creating neural visualization data (neurons, synapses, regions)")
    print("- Updating visualization with neural data")
    print("- Getting and displaying visualization statistics")
    print("- Testing different visualization modes")
    print("- Testing real-time updates")
    print("- Saving visualization in different formats")
    print("- Using configuration management for visualization settings")
    print("- Creating and managing configuration snapshots")

if __name__ == "__main__":
    import time
    main()
