/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/examples/plugin_example.py
Name: Plugin system example
Summary: Plugin system example demonstrating NLM plugins

"""
Plugin system example for NLM Python interface.

This example demonstrates the plugin system and custom plugins.
"""

import pynlm
import asyncio
import time

class CustomVisualizationPlugin(pynlm.VisualizationPlugin):
    """Custom visualization plugin"""
    
    def _initialize_plugin(self):
        self.register_event_handler(pynlm.EventType.STATE_CHANGED, self._on_state_change)
    
    def _enable_plugin(self):
        print(f"   Custom visualization plugin enabled: {self.info.name}")
    
    def _disable_plugin(self):
        print(f"   Custom visualization plugin disabled: {self.info.name}")
    
    def _unload_plugin(self):
        print(f"   Custom visualization plugin unloaded: {self.info.name}")
    
    def _on_state_change(self, event):
        print(f"   [Plugin {self.info.name}] State changed: {event.event_type}")

class CustomAnalysisPlugin(pynlm.ComputationPlugin):
    """Custom analysis plugin"""
    
    def _initialize_plugin(self):
        self.register_event_handler(pynlm.EventType.SIMULATION_STOPPED, self._on_simulation_stop)
    
    def _enable_plugin(self):
        print(f"   Custom analysis plugin enabled: {self.info.name}")
    
    def _disable_plugin(self):
        print(f"   Custom analysis plugin disabled: {self.info.name}")
    
    def _unload_plugin(self):
        print(f"   Custom analysis plugin unloaded: {self.info.name}")
    
    def analyze_neural_activity(self, data):
        """Analyze neural activity"""
        neurons = data.get('neurons', [])
        if not neurons:
            return {}
        
        # Calculate statistics
        activities = [n.get('activity', 0) for n in neurons]
        
        return {
            'mean_activity': sum(activities) / len(activities),
            'max_activity': max(activities),
            'min_activity': min(activities),
            'activity_std': (sum((x - sum(activities) / len(activities)) ** 2 for x in activities) / len(activities)) ** 0.5,
            'total_activity': sum(activities)
        }
    
    def _on_simulation_stop(self, event):
        print(f"   [Plugin {self.info.name}] Simulation stopped")

class CustomIntegrationPlugin(pynlm.IntegrationPlugin):
    """Custom integration plugin"""
    
    def _initialize_plugin(self):
        self.register_event_handler(pynlm.EventType.SIMULATION_STARTED, self._on_simulation_start)
    
    def _enable_plugin(self):
        print(f"   Custom integration plugin enabled: {self.info.name}")
    
    def _disable_plugin(self):
        print(f"   Custom integration plugin disabled: {self.info.name}")
    
    def _unload_plugin(self):
        print(f"   Custom integration plugin unloaded: {self.info.name}")
    
    def integrate_with_database(self, data):
        """Integrate data with database"""
        # Simulate database integration
        print(f"   [Plugin {self.info.name}] Integrating with database...")
        
        # In a real implementation, this would connect to a database
        # and store the data
        
        return {
            'status': 'success',
            'records_inserted': len(data) if isinstance(data, list) else 1,
            'timestamp': time.time()
        }
    
    def _on_simulation_start(self, event):
        print(f"   [Plugin {self.info.name}] Simulation started - initiating database integration")

class CustomUtilityPlugin(pynlm.UtilityPlugin):
    """Custom utility plugin"""
    
    def _initialize_plugin(self):
        pass
    
    def _enable_plugin(self):
        print(f"   Custom utility plugin enabled: {self.info.name}")
    
    def _disable_plugin(self):
        print(f"   Custom utility plugin disabled: {self.info.name}")
    
    def _unload_plugin(self):
        print(f"   Custom utility plugin unloaded: {self.info.name}")
    
    def format_brain_data(self, data):
        """Format brain data for output"""
        formatted = {
            'timestamp': time.time(),
            'version': '1.0.0',
            'data': data,
            'formatted_by': self.info.name
        }
        
        return formatted
    
    def export_to_file(self, data, filepath):
        """Export data to file"""
        import json
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        return {
            'status': 'success',
            'filepath': filepath,
            'size': len(data)
        }

async def run_plugin_example():
    """Run plugin example"""
    print("=== NLM Plugin System Example ===\n")
    
    # Initialize system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create plugin manager
    print("2. Setting up plugin manager...")
    plugin_manager = interface.context.plugin_manager
    
    # Create custom plugins
    print("3. Creating custom plugins...")
    
    # Custom visualization plugin
    viz_plugin = CustomVisualizationPlugin(
        name='custom_visualization',
        version='1.0.0',
        description='Custom visualization plugin',
        author='Example Author',
        plugin_type=pynlm.PluginType.VISUALIZATION,
        dependencies=[],
        required_plugins=[],
        capabilities=['plot', 'chart', 'graph']
    )
    
    # Custom analysis plugin
    analysis_plugin = CustomAnalysisPlugin(
        name='custom_analysis',
        version='1.0.0',
        description='Custom analysis plugin',
        author='Example Author',
        plugin_type=pynlm.PluginType.UTILITY,
        dependencies=[],
        required_plugins=[],
        capabilities=['analyze', 'compute', 'statistics']
    )
    
    # Custom integration plugin
    integration_plugin = CustomIntegrationPlugin(
        name='custom_integration',
        version='1.0.0',
        description='Custom integration plugin',
        author='Example Author',
        plugin_type=pynlm.PluginType.INTEGRATION,
        dependencies=[],
        required_plugins=[],
        capabilities=['database', 'export', 'import']
    )
    
    # Custom utility plugin
    utility_plugin = CustomUtilityPlugin(
        name='custom_utility',
        version='1.0.0',
        description='Custom utility plugin',
        author='Example Author',
        plugin_type=pynlm.PluginType.UTILITY,
        dependencies=[],
        required_plugins=[],
        capabilities=['format', 'export', 'transform']
    )
    
    # Register plugins with plugin manager
    print("4. Registering plugins with plugin manager...")
    
    # Create plugin manager with custom plugins
    # In a real implementation, we would register these with the plugin manager
    print("   Note: In this example, we demonstrate plugin creation")
    print("   but direct plugin registration is handled by the plugin manager.")
    
    # Start simulation
    print("5. Starting simulation...")
    await interface.run_loop(duration=2.0, dt=0.1)
    
    # Test plugin functionality
    print("6. Testing plugin functionality...")
    
    # Get brain data for analysis
    brain_data = interface.get_state()
    
    # Use custom analysis plugin
    if analysis_plugin:
        analysis_result = analysis_plugin.analyze_neural_activity(brain_data)
        print(f"   Analysis result: {analysis_result}")
    
    # Use custom utility plugin
    if utility_plugin:
        formatted_data = utility_plugin.format_brain_data(brain_data)
        print(f"   Formatted data keys: {list(formatted_data.keys())}")
    
    # Test event emission
    print("7. Testing event-driven plugin functionality...")
    
    interface.emit_state_change_event('user', {'action': 'test', 'value': 42})
    
    await asyncio.sleep(0.5)
    
    # Create additional plugins
    print("8. Creating additional plugins...")
    
    # Create plugins using plugin manager
    plugins = [
        ('visualization_plugin', CustomVisualizationPlugin),
        ('analysis_plugin', CustomAnalysisPlugin),
        ('integration_plugin', CustomIntegrationPlugin),
        ('utility_plugin', CustomUtilityPlugin)
    ]
    
    loaded_plugins = []
    for plugin_name, plugin_class in plugins:
        # Create plugin instance
        plugin_info = pynlm.PluginInfo(
            name=plugin_name,
            version='1.0.0',
            description=f'Plugin {plugin_name}',
            author='Example Author',
            plugin_type=pynlm.PluginType.UTILITY,
            dependencies=[],
            required_plugins=[],
            capabilities=['example']
        )
        
        plugin = plugin_class(plugin_info)
        loaded_plugins.append((plugin_name, plugin))
        print(f"   Created plugin: {plugin_name}")
    
    # Test plugin methods
    print("9. Testing plugin methods...")
    
    for plugin_name, plugin in loaded_plugins:
        print(f"   Testing {plugin_name}...")
        
        # Test plugin info
        print(f"     Plugin name: {plugin.info.name}")
        print(f"     Plugin version: {plugin.info.version}")
        print(f"     Plugin capabilities: {plugin.info.capabilities}")
        
        # Test plugin configuration
        plugin.set_config('test_setting', 'test_value')
        print(f"     Plugin config test_setting: {plugin.get_config('test_setting')}")
    
    # Test event handlers
    print("10. Testing event handlers...")
    
    event_count = 0
    def test_event_handler(event):
        nonlocal event_count
        event_count += 1
        print(f"     Event handler called: {event.event_type} (count: {event_count})")
    
    # Register event handler with a plugin
    test_plugin = loaded_plugins[0][1]
    test_plugin.register_event_handler(pynlm.EventType.STATE_CHANGED, test_event_handler)
    
    # Emit events
    for i in range(3):
        interface.emit_state_change_event('test', {'iteration': i})
        await asyncio.sleep(0.1)
    
    print(f"   Total events processed: {event_count}")
    
    # Test plugin filtering
    print("11. Testing plugin filtering...")
    
    for plugin_name, plugin in loaded_plugins:
        # Check plugin capabilities
        for capability in plugin.info.capabilities:
            if plugin.get_capability(capability):
                print(f"     {plugin_name} has capability: {capability}")
    
    # Test plugin configuration
    print("12. Testing plugin configuration...")
    
    for plugin_name, plugin in loaded_plugins:
        plugin.set_config('test_mode', 'production')
        print(f"     {plugin_name} config test_mode: {plugin.get_config('test_mode')}")
    
    # Test plugin state
    print("13. Testing plugin state...")
    
    for plugin_name, plugin in loaded_plugins:
        print(f"     {plugin_name} state - initialized: {plugin.initialized}, enabled: {plugin.enabled}")
    
    # Test plugin events
    print("14. Testing plugin events...")
    
    # Emit more events to test plugin event handling
    for i in range(2):
        interface.emit_simulation_event(pynlm.EventType.SIMULATION_STARTED, 'test', {'duration': 1})
        interface.emit_simulation_event(pynlm.EventType.SIMULATION_STOPPED, 'test', {'reason': 'test'})
        await asyncio.sleep(0.2)
    
    # Get plugin statistics
    print("15. Getting plugin statistics...")
    
    plugin_stats = interface.get_plugin_statistics()
    print(f"   Total plugins: {plugin_stats.get('total_plugins', 0)}")
    print(f"   Active plugins: {plugin_stats.get('active_plugins', 0)}")
    print(f"   Plugin types: {plugin_stats.get('plugin_types', {})}")
    
    # Clean up
    print("16. Cleaning up...")
    
    # Disable plugins
    for plugin_name, plugin in loaded_plugins:
        try:
            plugin.disable()
        except Exception as e:
            print(f"   Error disabling {plugin_name}: {e}")
    
    # Unload plugins
    for plugin_name, plugin in loaded_plugins:
        try:
            plugin.unload()
        except Exception as e:
            print(f"   Error unloading {plugin_name}: {e}")
    
    # Reset system
    interface.reset_system()
    
    print("\n=== Plugin Example Summary ===")
    print(f"• Plugins created: {len(loaded_plugins)}")
    print(f"• Events processed: {event_count}")
    print(f"• Plugin statistics collected: {plugin_stats}")
    print(f"• Plugin capabilities tested: {len(plugin_stats.get('plugin_types', {}))}")
    
    print("\n=== Plugin Example Completed Successfully ===")

# Run the example
if __name__ == "__main__":
    asyncio.run(run_plugin_example())
