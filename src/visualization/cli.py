#!/usr/bin/env python3
"""
NLM Visualization and Monitoring CLI
====================================

Command-line interface for NLM visualization and monitoring tools.
Provides commands for:
- Running visualization systems
- Monitoring brain activity
- Exporting visualization data
- Managing web interfaces
- Running comprehensive demos
"""

import argparse
import sys
import os
import time
import json
import threading
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    import pynlm
    from .visualization import VisualizationSystem, RealTimeMonitor
    from .visualization_interface import VisualizationInterface
    HAS_NLM = True
except ImportError as e:
    print(f"Warning: Could not import NLM modules: {e}")
    print("Some functionality will be limited.")
    HAS_NLM = False

class NLMVizCLI:
    """
    Command-line interface for NLM visualization and monitoring.
    """
    
    def __init__(self):
        """Initialize CLI with default settings"""
        self.viz_system = None
        self.monitor = None
        self.viz_interface = None
        self.running = False
        
    def create_visualization_system(self, backend="software", config_file=None):
        """
        Create visualization system with specified backend.
        
        Args:
            backend: Visualization backend (software, opengl, vulkan)
            config_file: Configuration file path
        """
        if not HAS_NLM:
            print("Error: NLM not available")
            return False
            
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Load configuration if specified
            if config_file and os.path.exists(config_file):
                config.loadFromFile(config_file)
            
            # Set visualization-specific configuration
            config.set("visualization.backend", backend)
            config.set("visualization.fps", 60.0)
            config.set("visualization.show_network", True)
            config.set("visualization.show_memory", True)
            config.set("visualization.show_neuromodulation", True)
            config.set("visualization.show_plasticity", True)
            config.set("visualization.show_development", True)
            config.set("visualization.show_prediction", True)
            
            # Create visualization system
            self.viz_system = VisualizationSystem()
            
            # Initialize with configuration
            success = self.viz_system.initialize(config)
            
            if success:
                print(f"✓ Visualization system created with backend: {backend}")
                return True
            else:
                print("✗ Failed to initialize visualization system")
                return False
                
        except Exception as e:
            print(f"✗ Error creating visualization system: {e}")
            return False
    
    def create_monitor(self, update_interval=1.0, config_file=None):
        """
        Create real-time monitor.
        
        Args:
            update_interval: Update interval in seconds
            config_file: Configuration file path
        """
        if not HAS_NLM:
            print("Error: NLM not available")
            return False
            
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Load configuration if specified
            if config_file and os.path.exists(config_file):
                config.loadFromFile(config_file)
            
            # Create monitor
            self.monitor = RealTimeMonitor()
            
            # Initialize monitor
            success = self.monitor.initialize(config)
            
            if success:
                # Set update interval
                self.monitor.setUpdateInterval(1.0 / update_interval)
                print(f"✓ Monitor created with update interval: {update_interval}s")
                return True
            else:
                print("✗ Failed to initialize monitor")
                return False
                
        except Exception as e:
            print(f"✗ Error creating monitor: {e}")
            return False
    
    def create_combined_interface(self, brain=None, backend="software"):
        """
        Create combined visualization interface with brain and monitor.
        
        Args:
            brain: NLM Brain object (if None, create default)
            backend: Visualization backend
        """
        if not HAS_NLM:
            print("Error: NLM not available")
            return False
            
        try:
            # Create brain if not provided
            if brain is None:
                config = pynlm.createDefaultConfig()
                config.set("brain.neuronCount", 100)
                brain = pynlm.createBrain(config)
                brain.initialize()
                print(f"✓ Default brain created with {brain.getTotalNeuronCount()} neurons")
            
            # Create monitor
            self.create_monitor()
            
            # Create visualization interface
            self.viz_interface = VisualizationInterface()
            
            # Initialize with brain and monitor
            success = self.viz_interface.initialize(brain, self.monitor, 
                                                    VisualizationSystem.BackendType.SOFTWARE_RENDERER)
            
            if success:
                print("✓ Combined visualization interface created")
                return True
            else:
                print("✗ Failed to create combined interface")
                return False
                
        except Exception as e:
            print(f"✗ Error creating combined interface: {e}")
            return False
    
    def start_visualization(self, fps=60.0):
        """
        Start visualization.
        
        Args:
            fps: Frames per second
        """
        if not self.viz_interface:
            print("Error: No visualization interface created")
            return False
            
        try:
            self.viz_interface.startVisualization(fps)
            self.running = True
            print(f"✓ Visualization started with {fps} FPS")
            return True
            
        except Exception as e:
            print(f"✗ Error starting visualization: {e}")
            return False
    
    def stop_visualization(self):
        """Stop visualization"""
        if not self.viz_interface:
            print("Error: No visualization interface created")
            return False
            
        try:
            self.viz_interface.stopVisualization()
            self.running = False
            print("✓ Visualization stopped")
            return True
            
        except Exception as e:
            print(f"✗ Error stopping visualization: {e}")
            return False
    
    def update_data(self, brain=None):
        """
        Update visualization data.
        
        Args:
            brain: NLM Brain object (if None, use stored brain)
        """
        if not self.viz_interface:
            print("Error: No visualization interface created")
            return False
            
        try:
            # Use provided brain or stored brain
            current_brain = brain if brain is not None else getattr(self.viz_interface, 'brain', None)
            
            if current_brain:
                self.viz_interface.update(current_brain)
                print("✓ Visualization data updated")
            else:
                print("Warning: No brain data available for update")
            
            return True
            
        except Exception as e:
            print(f"✗ Error updating visualization data: {e}")
            return False
    
    def render_frame(self):
        """Render visualization frame"""
        if not self.viz_interface:
            print("Error: No visualization interface created")
            return False
            
        try:
            self.viz_interface.render()
            return True
            
        except Exception as e:
            print(f"✗ Error rendering frame: {e}")
            return False
    
    def export_visualization(self, filepath, format="png"):
        """
        Export visualization to file.
        
        Args:
            filepath: Output file path
            format: Export format (png, jpg, svg, etc.)
        """
        if not self.viz_interface:
            print("Error: No visualization interface created")
            return False
            
        try:
            success = self.viz_interface.exportVisualization(filepath, format)
            
            if success:
                print(f"✓ Visualization exported to: {filepath}")
            else:
                print(f"✗ Failed to export visualization to: {filepath}")
            
            return success
            
        except Exception as e:
            print(f"✗ Error exporting visualization: {e}")
            return False
    
    def run_demo(self, num_steps=50, fps=30.0):
        """
        Run comprehensive visualization demo.
        
        Args:
            num_steps: Number of simulation steps
            fps: Visualization frames per second
        """
        print(f"\nRunning NLM visualization demo...")
        print(f"Steps: {num_steps}, FPS: {fps}")
        
        # Create combined interface
        if not self.viz_interface:
            success = self.create_combined_interface()
            if not success:
                print("✗ Failed to create visualization interface for demo")
                return False
        
        # Start visualization
        if not self.start_visualization(fps):
            print("✗ Failed to start visualization for demo")
            return False
        
        # Run simulation steps
        for step in range(num_steps):
            try:
                # Update data
                self.update_data()
                
                # Render frame
                self.render_frame()
                
                # Display progress
                if step % 10 == 0:
                    print(f"  Step {step}/{num_steps} completed")
                
                # Small delay to control simulation speed
                time.sleep(1.0 / fps)
                
            except KeyboardInterrupt:
                print("\nDemo interrupted by user")
                break
            except Exception as e:
                print(f"Error in demo step {step}: {e}")
                continue
        
        # Stop visualization
        self.stop_visualization()
        
        print(f"✓ Demo completed: {num_steps} steps visualized")
        return True
    
    def run_monitoring(self, duration=60.0, interval=1.0):
        """
        Run monitoring session.
        
        Args:
            duration: Monitoring duration in seconds
            interval: Monitoring interval in seconds
        """
        if not self.monitor:
            print("Error: No monitor created")
            return False
            
        print(f"\nRunning monitoring session...")
        print(f"Duration: {duration}s, Interval: {interval}s")
        
        # Start monitoring
        self.monitor.startMonitoring(1.0 / interval)
        
        # Monitor for specified duration
        start_time = time.time()
        end_time = start_time + duration
        
        while time.time() < end_time and self.monitor.isMonitoring():
            try:
                # Get latest metrics
                metrics = self.monitor.getLatestMetrics()
                
                # Display metrics
                print(f"Time: {time.time() - start_time:.1f}s | "
                      f"Firing Rate: {metrics.avgFiringRate:.2f}Hz | "
                      f"Active Neurons: {metrics.activeNeuronCount} | "
                      f"Spikes: {metrics.totalSpikeCount}")
                
                # Wait for next interval
                time.sleep(interval)
                
            except KeyboardInterrupt:
                print("\nMonitoring interrupted by user")
                break
            except Exception as e:
                print(f"Error in monitoring: {e}")
                continue
        
        # Stop monitoring
        self.monitor.stopMonitoring()
        
        print(f"✓ Monitoring session completed: {duration}s")
        return True
    
    def export_data(self, filepath):
        """
        Export visualization data.
        
        Args:
            filepath: Output file path
        """
        # Collect data
        export_data = {
            'timestamp': time.time(),
            'visualization_system': {
                'backend': 'software',
                'fps': 60.0,
                'active': self.viz_interface.isRunning() if self.viz_interface else False
            },
            'monitor': {
                'active': self.monitor.isMonitoring() if self.monitor else False,
                'update_interval': self.monitor.getUpdateInterval() if self.monitor else 0
            }
        }
        
        # Write to file
        try:
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2)
            
            print(f"✓ Data exported to: {filepath}")
            return True
            
        except Exception as e:
            print(f"✗ Error exporting data: {e}")
            return False
    
    def cleanup(self):
        """Clean up resources"""
        print("Cleaning up resources...")
        
        if self.viz_interface:
            self.viz_interface.close()
            print("✓ Visualization interface closed")
        
        if self.monitor:
            self.monitor.shutdown()
            print("✓ Monitor shutdown")
        
        self.running = False
        print("✓ Cleanup completed")


def create_cli():
    """Create and return CLI parser"""
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Monitoring CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Subparsers for different commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create visualization system command
    viz_parser = subparsers.add_parser('create-viz', help='Create visualization system')
    viz_parser.add_argument('--backend', choices=['software', 'opengl', 'vulkan'], 
                           default='software', help='Visualization backend')
    viz_parser.add_argument('--config', help='Configuration file path')
    
    # Create monitor command
    monitor_parser = subparsers.add_parser('create-monitor', help='Create monitor')
    monitor_parser.add_argument('--interval', type=float, default=1.0, 
                               help='Update interval in seconds')
    monitor_parser.add_argument('--config', help='Configuration file path')
    
    # Create combined interface command
    combined_parser = subparsers.add_parser('create-combined', help='Create combined interface')
    combined_parser.add_argument('--neurons', type=int, default=100,
                                help='Number of neurons in default brain')
    
    # Start visualization command
    start_parser = subparsers.add_parser('start-viz', help='Start visualization')
    start_parser.add_argument('--fps', type=float, default=60.0, help='Frames per second')
    
    # Stop visualization command
    stop_parser = subparsers.add_parser('stop-viz', help='Stop visualization')
    
    # Update data command
    update_parser = subparsers.add_parser('update', help='Update visualization data')
    
    # Render command
    render_parser = subparsers.add_parser('render', help='Render visualization frame')
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export visualization')
    export_parser.add_argument('filepath', help='Output file path')
    export_parser.add_argument('--format', default='png', help='Export format')
    
    # Demo command
    demo_parser = subparsers.add_parser('demo', help='Run visualization demo')
    demo_parser.add_argument('--steps', type=int, default=50, help='Number of steps')
    demo_parser.add_argument('--fps', type=float, default=30.0, help='Frames per second')
    
    # Monitor command
    monitor_session_parser = subparsers.add_parser('monitor', help='Run monitoring session')
    monitor_session_parser.add_argument('--duration', type=float, default=60.0, help='Duration in seconds')
    monitor_session_parser.add_argument('--interval', type=float, default=1.0, help='Interval in seconds')
    
    # Export data command
    export_data_parser = subparsers.add_parser('export-data', help='Export data')
    export_data_parser.add_argument('filepath', help='Output file path')
    
    # Cleanup command
    cleanup_parser = subparsers.add_parser('cleanup', help='Cleanup resources')
    
    return parser, subparsers


def main():
    """Main CLI function"""
    parser, subparsers = create_cli()
    
    # Create CLI instance
    cli = NLMVizCLI()
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle commands
    if args.command == 'create-viz':
        success = cli.create_visualization_system(args.backend, args.config)
        sys.exit(0 if success else 1)
        
    elif args.command == 'create-monitor':
        success = cli.create_monitor(args.interval, args.config)
        sys.exit(0 if success else 1)
        
    elif args.command == 'create-combined':
        success = cli.create_combined_interface()
        sys.exit(0 if success else 1)
        
    elif args.command == 'start-viz':
        success = cli.start_visualization(args.fps)
        sys.exit(0 if success else 1)
        
    elif args.command == 'stop-viz':
        success = cli.stop_visualization()
        sys.exit(0 if success else 1)
        
    elif args.command == 'update':
        success = cli.update_data()
        sys.exit(0 if success else 1)
        
    elif args.command == 'render':
        success = cli.render_frame()
        sys.exit(0 if success else 1)
        
    elif args.command == 'export':
        success = cli.export_visualization(args.filepath, args.format)
        sys.exit(0 if success else 1)
        
    elif args.command == 'demo':
        success = cli.run_demo(args.steps, args.fps)
        sys.exit(0 if success else 1)
        
    elif args.command == 'monitor':
        success = cli.run_monitoring(args.duration, args.interval)
        sys.exit(0 if success else 1)
        
    elif args.command == 'export-data':
        success = cli.export_data(args.filepath)
        sys.exit(0 if success else 1)
        
    elif args.command == 'cleanup':
        cli.cleanup()
        sys.exit(0)
        
    else:
        # No command specified, show help
        parser.print_help()
        sys.exit(1)
if __name__ == "__main__":
    main()
