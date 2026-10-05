"""
NLM Advanced Simulation Example

This example demonstrates advanced NLM simulation features including:
- Real-time monitoring and control
- Performance profiling
- Event-driven simulation
- Advanced configuration management
- Plugin system integration
- Comprehensive error handling
- Visualization and data export

This example is designed for advanced users who want to leverage
the full capabilities of the NLM framework.
"""

import sys
import time
import threading
import json
import numpy as np
from typing import Dict, List, Any, Optional

# Add the utils module to path
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_41d79b90-42a5-496b-a5aa-84bdfca07d69/python')

# Import NLM components
try:
    from nlm_utils import (
        AdvancedConfig,
        MetricsCollector,
        EventSystem,
        EnhancedErrorHandler,
        InputValidator,
        SimulationSafetyChecker,
        VisualizationHelper,
        PluginManager,
        NLMContext,
        PluginInterface,
        AdvancedSimulationController,
        SimulationScheduler,
        SimulationExporter,
        PerformanceMonitor,
        ConfigBuilder,
        Event
    )
    NLM_UTILS_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM utilities not available: {e}")
    NLM_UTILS_AVAILABLE = False

# Import nlm_py with fallback
try:
    import nlm_py
    NLM_PY_AVAILABLE = True
except ImportError:
    print("Warning: NLM Python bindings not available. Using mock objects.")
    NLM_PY_AVAILABLE = False
    nlm_py = None

if NLM_PY_AVAILABLE:
    import nlm_py as nlm
else:
    # Create mock objects for testing when bindings not available
    class MockNeuron:
        def __init__(self, neuron_id):
            self.neuron_id = neuron_id
            self.type = nlm.NeuronType.Internal if NLM_PY_AVAILABLE else 'Internal'
            self.membrane_potential = -70.0
            self.threshold = -55.0
            self.firing_rate = 0.0
            self.refractory_remaining = 0
        
        def getId(self):
            return self.neuron_id
        
        def getType(self):
            return self.type
        
        def getMembranePotential(self):
            return self.membrane_potential
        
        def setMembranePotential(self, potential):
            self.membrane_potential = potential
        
        def getThreshold(self):
            return self.threshold
    
    class MockBrain:
        def __init__(self, config=None):
            self.config = config or {}
            self.neurons = [MockNeuron(i) for i in range(config.get('neuron_count', 100))]
            self.simulation_step = 0
            self.total_spikes = 0
        
        def initializeForSimulation(self):
            print("Mock brain initialized for simulation")
            return True
        
        def step(self, step=None, time=None):
            self.simulation_step += 1
            # Simulate some activity
            for neuron in self.neurons:
                if np.random.random() < 0.1:  # 10% chance of spiking
                    self.total_spikes += 1
            return True
        
        def getSimulationStep(self):
            return self.simulation_step
        
        def getNeuronCount(self):
            return len(self.neurons)
        
        def getTotalSpikeCount(self):
            return self.total_spikes
        
        def getAverageFiringRate(self):
            if self.simulation_step == 0:
                return 0.0
            return self.total_spikes / self.simulation_step
    
    class MockConfig:
        def __init__(self):
            self.params = {}
        
        def set(self, key, value):
            self.params[key] = value
        
        def get(self, key, default=None):
            return self.params.get(key, default)
    
    # Create mock nlm module
    class MockNLM:
        class NeuronType:
            Internal = 'Internal'
            Excitatory = 'Excitatory'
            Inhibitory = 'Inhibitory'
            Sensory = 'Sensory'
            Motor = 'Motor'
        
        class FiringState:
            Resting = 'Resting'
            Active = 'Active'
            Refractory = 'Refractory'
    
    nlm = MockNLM()
    nlm.Brain = MockBrain
    nlm.Config = MockConfig

def print_header(title):
    """Print formatted header"""
    print("\n" + "="*80)
    print(f"NLM Advanced Simulation Example: {title}")
    print("="*80)

class AdvancedSimulationManager:
    """Manages advanced NLM simulation with all features"""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.simulation_controller = None
        self.performance_monitor = PerformanceMonitor()
        self.metrics_collector = MetricsCollector()
        self.event_system = EventSystem()
        self.error_handler = EnhancedErrorHandler()
        self.safety_checker = SimulationSafetyChecker()
        self.visualization_helper = VisualizationHelper()
        self.plugin_manager = PluginManager()
        self.export_manager = SimulationExporter()
        self.config_builder = ConfigBuilder()
        
        self.simulation_running = False
        self.monitoring_active = False
        self.export_data = {}
    
    def setup_complete_config(self):
        """Setup complete advanced configuration"""
        print_header("1. Advanced Configuration Setup")
        
        print("\n1.1 Creating base configuration...")
        base_config = {
            'neuron_count': 5000,
            'simulation_timestep': 0.0002,
            'connection_probability': 0.15,
            'plasticity_learning_rate': 0.02,
            'stdp_ltp_weight': 0.04,
            'stdp_ltd_weight': 0.035,
            'stdp_time_constant': 25.0,
            'development_synaptogenesis_rate': 0.003,
            'development_pruning_rate': 0.0003,
            'dopamine_baseline': 0.15,
            'reward_discount_factor': 0.98,
            'environment_name': 'AdvancedGridWorld',
            'environment_width': 20,
            'environment_height': 20,
            'log_level': 'DEBUG',
            'log_to_file': True,
            'log_filename': 'advanced_nlm_simulation.log',
            'max_simulation_steps': 2000,
            'simulation_time_limit': 3600.0,
            'visualization_enabled': True,
            'visualization_update_rate': 60.0
        }
        
        self.config = AdvancedConfig()
        for key, value in base_config.items():
            self.config.set(key, value)
        
        # Validate configuration
        print("\n1.2 Validating configuration...")
        if self.config.is_valid():
            print("   ✓ Configuration is valid")
        else:
            errors = self.config.get_validation_errors()
            print(f"   ✗ Configuration has {len(errors)} errors:")
            for error in errors:
                print(f"      - {error}")
            return False
        
        print(f"\n1.3 Configuration summary:")
        print(f"   - Neurons: {self.config.get('neuron_count')}")
        print(f"   - Timestep: {self.config.get('simulation_timestep')}s")
        print(f"   - Connection probability: {self.config.get('connection_probability')}")
        print(f"   - Simulation steps: {self.config.get('max_simulation_steps')}")
        print(f"   - Environment: {self.config.get('environment_width')}x{self.config.get('environment_height')}")
        print(f"   - Log level: {self.config.get('log_level')}")
        
        # Setup error handling
        print("\n1.4 Setting up error handling...")
        
        def validation_error_handler(error, context):
            print(f"   🚨 Validation error: {error}")
            return self._handle_validation_error(error, context)
        
        def simulation_error_handler(error, context):
            print(f"   ⚡ Simulation error: {error}")
            return self._handle_simulation_error(error, context)
        
        def safety_error_handler(error, context):
            print(f"   🛡️ Safety error: {error}")
            return self._handle_safety_error(error, context)
        
        # Register error handlers
        self.error_handler.recovery_actions['validation'] = validation_error_handler
        self.error_handler.recovery_actions['simulation'] = simulation_error_handler
        self.error_handler.recovery_actions['safety'] = safety_error_handler
        
        # Setup input validator
        print("\n1.5 Setting up input validation...")
        validator = InputValidator()
        
        config_errors = validator.validate_brain_config(self.config.get_config_dict())
        if config_errors:
            print(f"   ⚠️  Configuration validation warnings:")
            for warning in config_errors:
                print(f"      - {warning}")
        
        # Setup safety checker
        print("\n1.6 Setting up safety monitoring...")
        self.safety_checker.emergency_stop = False
        
        # Setup metrics collector
        print("\n1.7 Setting up metrics collection...")
        self.metrics_collector.set_alert_threshold('spike_count', 100000)
        self.metrics_collector.set_alert_threshold('memory_usage', 12.0)
        
        def spike_count_alert(metric_name, value, threshold):
            print(f"   📊 Alert: {metric_name} ({value}) exceeds threshold ({threshold})")
        
        def memory_usage_alert(metric_name, value, threshold):
            print(f"   💾 Alert: {metric_name} ({value} GB) exceeds threshold ({threshold} GB)")
        
        self.metrics_collector.add_alert_callback(spike_count_alert)
        self.metrics_collector.add_alert_callback(memory_usage_alert)
        
        return True
    
    def _handle_validation_error(self, error, context):
        """Handle validation errors"""
        if context and 'brain' in context:
            try:
                context['brain'].reset()
                context['brain'].initialize()
                return True
            except Exception:
                pass
        return False
    
    def _handle_simulation_error(self, error, context):
        """Handle simulation errors"""
        if context and 'simulation_running' in context:
            context['simulation_running'] = False
            time.sleep(0.1)
            context['simulation_running'] = True
            return True
        return False
    
    def _handle_safety_error(self, error, context):
        """Handle safety errors"""
        print(f"   🚨 SAFETY VIOLATION: {error}")
        self.safety_checker.activate_emergency_stop()
        return False  # Don't recover from safety errors
    
    def setup_plugin_system(self):
        """Setup plugin system"""
        print_header("2. Plugin System Setup")
        
        print("\n2.1 Initializing plugin manager...")
        
        # Create NLM context
        nlm_context = NLMContext()
        nlm_context.config = self.config
        nlm_context.error_handler = self.error_handler
        nlm_context.metrics_collector = self.metrics_collector
        nlm_context.event_system = self.event_system
        nlm_context.state_manager = None  # Would be initialized in real scenario
        nlm_context.plugin_manager = self.plugin_manager
        
        # Create sample plugins (mock implementations)
        class MonitoringPlugin(PluginInterface):
            def initialize(self, context):
                print("   🔍 Monitoring plugin initialized")
                return True
            
            def execute(self, data, context):
                data['monitoring_timestamp'] = time.time()
                data['monitoring_cpu'] = np.random.uniform(10.0, 60.0)
                data['monitoring_memory'] = np.random.uniform(1.0, 8.0)
                return data
            
            def cleanup(self):
                print("   🔍 Monitoring plugin cleaned up")
        
        class AnalyticsPlugin(PluginInterface):
            def initialize(self, context):
                print("   📊 Analytics plugin initialized")
                return True
            
            def execute(self, data, context):
                data['analytics_computed'] = True
                data['analytics_complexity'] = np.random.uniform(0.5, 0.9)
                return data
            
            def cleanup(self):
                print("   📊 Analytics plugin cleaned up")
        
        class VisualizationPlugin(PluginInterface):
            def initialize(self, context):
                print("   🎨 Visualization plugin initialized")
                return True
            
            def execute(self, data, context):
                if 'brain_metrics' in data:
                    data['visualization_generated'] = True
                    data['visualization_type'] = 'spike_raster'
                return data
            
            def cleanup(self):
                print("   🎨 Visualization plugin cleaned up")
        
        # Register plugins
        monitoring_plugin = MonitoringPlugin()
        analytics_plugin = AnalyticsPlugin()
        visualization_plugin = VisualizationPlugin()
        
        nlm_context.add_plugin(monitoring_plugin)
        nlm_context.add_plugin(analytics_plugin)
        nlm_context.add_plugin(visualization_plugin)
        
        print("\n2.2 Plugin summary:")
        print(f"   - Loaded plugins: {len(nlm_context.plugins)}")
        print(f"   - Monitoring plugin: {'Active' if monitoring_plugin else 'Inactive'}")
        print(f"   - Analytics plugin: {'Active' if analytics_plugin else 'Inactive'}")
        print(f"   - Visualization plugin: {'Active' if visualization_plugin else 'Inactive'}")
        
        return nlm_context
    
    def setup_brain(self):
        """Setup brain with advanced configuration"""
        print_header("3. Brain Setup")
        
        print("\n3.1 Creating advanced brain...")
        
        # Create brain with advanced config
        self.brain = nlm.Brain(self.config)
        
        print("\n3.2 Initializing brain...")
        
        # Setup event handlers for brain operations
        def on_brain_step(event_type, event_data):
            step = event_data['step']
            spike_count = event_data['spike_count']
            
            # Update metrics
            self.metrics_collector.record_metric('step', step)
            self.metrics_collector.record_metric('spike_count', spike_count)
            
            # Log progress
            if step % 100 == 0:
                print(f"   Brain step {step}: {spike_count} spikes")
        
        def on_brain_error(event_type, event_data):
            error = event_data['error']
            context = event_data.get('context', {})
            
            print(f"   Brain error at step {context.get('step', 'unknown')}: {error}")
        
        self.event_system.addListener('brain_step', on_brain_step)
        self.event_system.addListener('brain_error', on_brain_error)
        
        # Initialize brain
        if self.brain.initializeForSimulation():
            print("   ✓ Brain initialized successfully")
            return True
        else:
            print("   ✗ Brain initialization failed")
            return False
    
    def setup_simulation_controller(self):
        """Setup advanced simulation controller"""
        print_header("4. Simulation Controller Setup")
        
        print("\n4.1 Creating simulation controller...")
        
        # Create simulation controller configuration
        simulation_config = {
            'brain': self.brain,
            'max_steps': self.config.get('max_simulation_steps', 2000),
            'timestep': self.config.get('simulation_timestep', 0.0002),
            'performance_monitoring': True,
            'event_driven': True,
            'auto_recovery': True,
            'safety_checks': True,
            'real_time_control': True,
            'monitoring_interval': 1.0
        }
        
        self.simulation_controller = AdvancedSimulationController(self.brain, simulation_config)
        
        # Setup event handlers
        print("\n4.2 Setting up event handlers...")
        
        def on_simulation_started(event_type, event_data):
            print(f"   🚀 Advanced simulation started")
            print(f"      Steps: {event_data['max_steps']}")
            print(f"      Timestep: {event_data['timestep']}s")
            print(f"      Duration: {event_data['max_steps'] * event_data['timestep']:.2f}s")
        
        def on_simulation_stopped(event_type, event_data):
            steps_completed = event_data['steps_completed']
            final_step = event_data['final_step']
            
            print(f"   🛑 Advanced simulation stopped")
            print(f"      Steps completed: {steps_completed}")
            print(f"      Final step: {final_step}")
            
            # Export simulation data
            self.export_simulation_data()
        
        def on_step_completed(event_type, event_data):
            step = event_data['step']
            brain_metrics = event_data['brain_metrics']
            performance_metrics = event_data['performance_metrics']
            
            # Log progress every 200 steps
            if step % 200 == 0:
                print(f"   Step {step:4d}: " +
                      f"Spikes {brain_metrics['spike_count']:8d} | " +
                      f"Rate {brain_metrics['firing_rate']:6.2f} Hz | " +
                      f"Stability {brain_metrics['stability']:5.2f} | " +
                      f"CPU {performance_metrics.get('cpu_usage_percent', 0.0):5.1f}%")
            
            # Store for export
            if step not in self.export_data:
                self.export_data[step] = {}
            
            self.export_data[step] = {
                'timestamp': event_data['timestamp'],
                'spike_count': brain_metrics['spike_count'],
                'active_neurons': brain_metrics['active_neurons'],
                'firing_rate': brain_metrics['firing_rate'],
                'stability': brain_metrics['stability'],
                'cpu_usage': performance_metrics.get('cpu_usage_percent', 0.0),
                'memory_usage': performance_metrics.get('memory_usage_gb', 0.0)
            }
        
        def on_error_recovery(event_type, event_data):
            error = event_data.get('error', 'Unknown error')
            recovered = event_data.get('recovered', False)
            
            if recovered:
                print(f"   🔄 Error recovered: {error}")
            else:
                print(f"   ⚠️ Error not recovered: {error}")
        
        def on_performance_alert(event_type, event_data):
            metric_name = event_data.get('metric_name', 'Unknown')
            value = event_data.get('value', 0)
            threshold = event_data.get('threshold', 0)
            
            print(f"   🚨 Performance alert: {metric_name} = {value:.2f} (threshold: {threshold:.2f})")
        
        self.simulation_controller.add_event_handler('simulation_started', on_simulation_started)
        self.simulation_controller.add_event_handler('simulation_stopped', on_simulation_stopped)
        self.simulation_controller.add_event_handler('step_completed', on_step_completed)
        self.simulation_controller.add_event_handler('error_recovery', on_error_recovery)
        self.simulation_controller.add_event_handler('performance_alert', on_performance_alert)
        
        print("\n4.3 Simulation controller setup complete")
        return True
    
    def start_monitoring(self):
        """Start comprehensive monitoring"""
        print_header("5. Monitoring Setup")
        
        print("\n5.1 Starting performance monitoring...")
        self.performance_monitor.start_monitoring()
        self.monitoring_active = True
        print("   ✓ Performance monitoring active")
        
        print("\n5.2 Setting up safety monitoring...")
        
        # Start safety monitoring thread
        def safety_monitor_loop():
            while self.simulation_running:
                try:
                    if self.brain:
                        # Check safety at each monitoring interval
                        step = self.brain.getSimulationStep()
                        violations = self.safety_checker.check_simulation_safety(
                            self.brain, step, self.config.get_config_dict()
                        )
                        
                        if violations:
                            for violation in violations:
                                print(f"   🛡️ Safety violation: {violation}")
                                
                                if "emergency" in violation.lower():
                                    self.safety_checker.activate_emergency_stop()
                                    break
                    
                    time.sleep(5.0)  # Check every 5 seconds
                except Exception as e:
                    print(f"   Error in safety monitoring: {e}")
                    break
        
        # Start safety monitoring thread
        self.safety_monitor_thread = threading.Thread(target=safety_monitor_loop)
        self.safety_monitor_thread.daemon = True
        self.safety_monitor_thread.start()
        
        print("   ✓ Safety monitoring active")
        
        print("\n5.3 Setting up visualization...")
        
        # Create sample visualization data
        if self.brain:
            # Generate spike data for visualization
            spike_data = np.random.binomial(1, 0.1, size=1000)
            time_steps = np.arange(len(spike_data))
            
            # Normalize for visualization
            normalized_spikes = self.visualization_helper.normalize_data(spike_data)
            
            # Create heat map (mock)
            heatmap_data = np.random.rand(50, 50)
            heatmap = self.visualization_helper.generate_heat_map(heatmap_data)
            
            # Create spike visualization (mock)
            spike_viz = self.visualization_helper.create_spike_visualization(spike_data)
            
            print(f"   ✓ Visualization data prepared")
            print(f"     - Spike data length: {len(spike_data)}")
            print(f"     - Heat map shape: {heatmap.shape}")
            print(f"     - Spike visualization shape: {spike_viz.shape}")
        
        return True
    
    def export_simulation_data(self):
        """Export simulation data"""
        print_header("6. Data Export")
        
        if not self.export_data:
            print("\n6.1 No simulation data to export")
            return
        
        print(f"\n6.2 Exporting {len(self.export_data)} simulation steps...")
        
        # Create export metadata
        export_metadata = {
            'export_timestamp': time.time(),
            'simulation_duration': len(self.export_data) * self.config.get('simulation_timestep', 0.0002),
            'neuron_count': self.config.get('neuron_count'),
            'environment': self.config.get('environment_name'),
            'export_version': '1.0.0',
            'format': 'json'
        }
        
        # Create comprehensive export data
        export_data = {
            'metadata': export_metadata,
            'configuration': self.config.get_config_dict(),
            'simulation_data': self.export_data,
            'metrics_summary': self.simulation_controller.get_metrics_summary() if self.simulation_controller else {}
        }
        
        # Export as JSON
        filename = f"advanced_simulation_{int(time.time())}.json"
        
        try:
            with open(filename, 'w') as f:
                json.dump(export_data, f, indent=2)
            
            print(f"   ✓ Exported to {filename}")
            print(f"   - Data points: {len(self.export_data)}")
            print(f"   - File size: {len(json.dumps(export_data)) / 1024:.1f} KB")
            
        except Exception as e:
            print(f"   ❌ Export failed: {e}")
    
    def run_simulation(self):
        """Run the advanced simulation"""
        print_header("7. Advanced Simulation Execution")
        
        if not self.brain:
            print("\n7.1 ❌ Brain not available")
            return False
        
        print("\n7.2 Starting advanced simulation...")
        
        try:
            # Start simulation controller
            if not self.simulation_controller.start():
                print("   ❌ Failed to start simulation")
                return False
            
            self.simulation_running = True
            
            print(f"   🎯 Running {self.simulation_controller.max_steps} simulation steps")
            print(f"   ⏱️  Estimated duration: {self.simulation_controller.max_steps * self.simulation_controller.dt:.1f}s")
            
            # Run simulation
            print("\n7.3 Simulation execution:")
            
            step_results = self.simulation_controller.run_steps(self.simulation_controller.max_steps)
            
            print(f"\n   ✅ Advanced simulation completed successfully!")
            print(f"   Steps executed: {len(step_results)}")
            
            # Get final progress
            final_progress = self.simulation_controller.get_progress()
            
            print(f"\n   📊 Final simulation summary:")
            print(f"     - Completion status: {'Completed' if final_progress['is_running'] == False else 'Running'}")
            print(f"     - Steps completed: {final_progress['current_step']}")
            print(f"     - Progress percentage: {final_progress['progress_percent']:.1f}%")
            print(f"     - Elapsed time: {final_progress['elapsed_time']:.1f}s")
            
            # Get performance metrics
            if self.performance_monitor:
                performance = self.performance_monitor.get_performance_summary()
                print(f"\n   ⚡ Performance metrics:")
                print(f"     - Total samples: {performance['total_samples']}")
                print(f"     - Average CPU usage: {performance['avg_cpu_usage']:.1f}%")
                print(f"     - Average memory usage: {performance['avg_memory_usage']:.1f} GB")
                print(f"     - Max CPU usage: {performance['max_cpu_usage']:.1f}%")
                print(f"     - Max memory usage: {performance['max_memory_usage']:.1f} GB")
            
            # Get error statistics
            error_stats = self.error_handler.get_error_stats()
            print(f"\n   🚨 Error statistics:")
            print(f"     - Total errors: {error_stats['total_errors']}")
            print(f"     - Error categories: {list(error_stats['error_counts'].keys())}")
            
            # Get metrics collector summary
            if self.metrics_collector:
                spike_summary = self.metrics_collector.get_metric_summary('spike_count')
                if spike_summary:
                    print(f"\n   📈 Metrics collection:")
                    print(f"     - Spike count mean: {spike_summary['mean']:.1f}")
                    print(f"     - Spike count std: {spike_summary['std']:.1f}")
                    print(f"     - Spike count max: {spike_summary['max']}")
            
            # Stop simulation
            self.simulation_controller.stop()
            self.simulation_running = False
            
            return True
            
        except Exception as e:
            print(f"\n   ❌ Simulation error: {e}")
            import traceback
            traceback.print_exc()
            
            # Try to stop simulation
            if self.simulation_controller:
                self.simulation_controller.stop()
            self.simulation_running = False
            
            return False
        finally:
            # Stop monitoring
            self.performance_monitor.stop_monitoring()
            self.monitoring_active = False
    
    def run_complete_example(self):
        """Run complete advanced simulation example"""
        print("NLM Advanced Simulation Example")
        print("===============================")
        
        try:
            # Step 1: Setup complete configuration
            if not self.setup_complete_config():
                print("\n❌ Configuration setup failed")
                return 1
            
            # Step 2: Setup plugin system
            nlm_context = self.setup_plugin_system()
            
            # Step 3: Setup brain
            if not self.setup_brain():
                print("\n❌ Brain setup failed")
                return 1
            
            # Step 4: Setup simulation controller
            if not self.setup_simulation_controller():
                print("\n❌ Simulation controller setup failed")
                return 1
            
            # Step 5: Start monitoring
            if not self.start_monitoring():
                print("\n❌ Monitoring setup failed")
                return 1
            
            # Step 6: Run simulation
            if not self.run_simulation():
                print("\n❌ Simulation execution failed")
                return 1
            
            # Step 7: Final summary
            print_header("Advanced Simulation Example Complete")
            
            print("\n🎉 ADVANCED SIMULATION EXAMPLE SUCCESSFULLY COMPLETED!")
            print("\nFeatures demonstrated:")
            print("✓ Advanced configuration management with validation")
            print("✓ Comprehensive error handling and recovery")
            print("✓ Real-time performance monitoring")
            print("✓ Safety checking and emergency controls")
            print("✓ Event-driven simulation architecture")
            print("✓ Plugin system integration")
            print("✓ Visualization and data export")
            print("✓ Metrics collection and analysis")
            print("✓ Real-time progress monitoring")
            print("✓ Comprehensive logging and reporting")
            print("✓ Automatic error recovery")
            print("✓ Safety violation detection")
            
            return 0
            
        except Exception as e:
            print(f"\n💥 Error in advanced simulation example: {e}")
            import traceback
            traceback.print_exc()
            return 1

def main():
    """Main function to run advanced simulation example"""
    manager = AdvancedSimulationManager()
    return manager.run_complete_example()

if __name__ == '__main__':
    sys.exit(main())