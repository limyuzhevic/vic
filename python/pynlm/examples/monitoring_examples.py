"""
NLM Brain System Monitoring Examples

This module provides comprehensive examples for using the NLM Brain System
monitoring interface, demonstrating all features and capabilities.
"""

import pynlm
import time
import threading
import json
from datetime import datetime
from pynlm.monitoring import (
    MonitoringInterface,
    MonitoringLevel,
    MonitoringMode
)
from pynlm.dashboard import Dashboard, DashboardWidgetType


def example_basic_monitoring():
    """Example: Basic monitoring setup and usage"""
    print("=== Basic Monitoring Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create monitoring interface
    print("2. Creating monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring with basic settings
    print("3. Starting monitoring (level=BASIC)...")
    if monitor.start_monitoring(
        level=MonitoringLevel.BASIC,
        mode=MonitoringMode.REAL_TIME,
        update_interval=2.0
    ):
        print("   ✓ Monitoring started successfully!")
        
        # Let it run for a few seconds
        print("4. Running monitoring for 5 seconds...")
        time.sleep(5)
        
        # Get monitoring data
        print("5. Getting monitoring data...")
        neural_data = monitor.monitoring_data['neural_activity']
        system_data = monitor.monitoring_data['system_resources']
        
        if neural_data:
            latest_neural = neural_data[-1]['data']
            print(f"   Latest neural activity:")
            print(f"     - Total neurons: {latest_neural.get('total_neurons', 'N/A')}")
            print(f"     - Active neurons: {latest_neural.get('active_neurons', 'N/A')}")
            print(f"     - Firing rate: {latest_neural.get('average_firing_rate', 0):.2f} Hz")
        
        if system_data:
            latest_system = system_data[-1]['data']
            print(f"   System resources:")
            print(f"     - CPU usage: {latest_system.get('cpu_percent', 0):.1f}%")
            print(f"     - Memory usage: {latest_system.get('memory_percent', 0):.1f}%")
        
        # Stop monitoring
        print("6. Stopping monitoring...")
        monitor.stop_monitoring()
        print("   ✓ Monitoring stopped successfully!")
    
    print("\n✓ Basic monitoring example completed!\n")


def example_dashboard_monitoring():
    """Example: Using dashboard with monitoring"""
    print("=== Dashboard Monitoring Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create dashboard
    print("2. Creating dashboard...")
    dashboard = Dashboard(interface)
    
    # Add various widgets
    print("3. Adding dashboard widgets...")
    
    # Neural activity metrics widget
    def neural_metrics():
        if interface.context.brain:
            state = interface.get_state()
            return {
                'neurons': state['total_neurons'],
                'active': state['active_neurons'],
                'firing_rate': state['average_firing_rate'],
                'spikes': state['total_spikes']
            }
        return {}
    
    dashboard.add_widget(
        DashboardWidgetType.METRICS,
        "Neural Activity",
        neural_metrics,
        refresh_interval=2.0,
        layout='web'
    )
    
    # System resources widget
    def system_metrics():
        import psutil
        process = psutil.Process()
        return {
            'cpu_percent': process.cpu_percent(),
            'memory_percent': process.memory_percent(),
            'threads': process.num_threads()
        }
    
    dashboard.add_widget(
        DashboardWidgetType.GAUGE,
        "System Resources",
        system_metrics,
        min_value=0,
        max_value=100,
        layout='web'
    )
    
    # Real-time plot widget
    def neural_plot():
        if interface.context.brain:
            state = interface.get_state()
            return {
                'time': time.time(),
                'neurons': state['total_neurons'],
                'active': state['active_neurons'],
                'firing_rate': state['average_firing_rate']
            }
        return {'time': time.time(), 'neurons': 0, 'active': 0, 'firing_rate': 0}
    
    dashboard.add_widget(
        DashboardWidgetType.PLOT,
        "Neural Activity Over Time",
        neural_plot,
        plot_type='line',
        x_axis='time',
        y_axis='value',
        layout='web'
    )
    
    # Historical data widget
    def historical_data():
        # Return historical data for visualization
        return {
            'labels': ['0s', '1s', '2s', '3s', '4s', '5s'],
            'values': [100, 95, 98, 102, 97, 101],
            'timestamps': [time.time() - i for i in range(6, 0, -1)]
        }
    
    dashboard.add_widget(
        DashboardWidgetType.HISTORICAL,
        "Activity History",
        historical_data,
        history_length=100,
        layout='web'
    )
    
    # Alarm widget
    def alarm_data():
        alerts = []
        if interface.context.brain:
            state = interface.get_state()
            if state['average_firing_rate'] > 100:
                alerts.append({
                    'type': 'high_firing_rate',
                    'severity': 'warning',
                    'message': f"High firing rate: {state['average_firing_rate']:.2f} Hz"
                })
        return alerts
    
    dashboard.add_widget(
        DashboardWidgetType.ALARM,
        "System Alerts",
        alarm_data,
        alert_rules=[],
        layout='web'
    )
    
    # Log widget
    def log_data():
        # Return log entries
        return {
            'logs': [
                {
                    'timestamp': time.time() - 10,
                    'message': 'System initialized',
                    'level': 'INFO',
                    'source': 'system'
                },
                {
                    'timestamp': time.time() - 5,
                    'message': 'Neural network created',
                    'level': 'INFO',
                    'source': 'brain'
                },
                {
                    'timestamp': time.time() - 2,
                    'message': 'Monitoring started',
                    'level': 'INFO',
                    'source': 'dashboard'
                }
            ]
        }
    
    dashboard.add_widget(
        DashboardWidgetType.LOG,
        "System Logs",
        log_data,
        max_entries=100,
        log_level='INFO',
        layout='web'
    )
    
    # Start dashboard
    print("4. Starting dashboard...")
    dashboard.start_dashboard(update_interval=2.0, auto_refresh=True)
    
    # Simulate some activity
    print("5. Running simulation for 10 seconds...")
    for i in range(5):
        interface.run_step(dt=0.5)
        time.sleep(1)
        print(f"   Step {i+1}/5 completed")
    
    # Get dashboard state
    print("6. Getting dashboard state...")
    state = dashboard.get_dashboard_state()
    print(f"   Dashboard has {state['total_widgets']} widgets")
    print(f"   Update interval: {state['update_interval']}s")
    print(f"   Auto refresh: {state['auto_refresh']}")
    
    # Export dashboard
    print("7. Exporting dashboard configuration...")
    try:
        dashboard.export_dashboard(format='json', filepath='dashboard_config.json')
        print("   ✓ Dashboard exported to dashboard_config.json")
    except Exception as e:
        print(f"   Note: Export failed (expected in demo): {e}")
    
    # Stop dashboard
    print("8. Stopping dashboard...")
    dashboard.stop_dashboard()
    print("   ✓ Dashboard stopped!")
    
    print("\n✓ Dashboard monitoring example completed!\n")


def example_advanced_monitoring():
    """Example: Advanced monitoring with multiple interfaces"""
    print("=== Advanced Monitoring Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create comprehensive monitoring interface
    print("2. Creating advanced monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring with comprehensive settings
    print("3. Starting advanced monitoring...")
    monitor.start_monitoring(
        level=MonitoringLevel.COMPREHENSIVE,
        mode=MonitoringMode.REAL_TIME,
        update_interval=1.0
    )
    
    # Let it run for demonstration
    print("4. Running advanced monitoring for 15 seconds...")
    start_time = time.time()
    
    while time.time() - start_time < 15:
        elapsed = time.time() - start_time
        remaining = 15 - elapsed
        print(f"   Monitoring... {elapsed:.1f}s elapsed, {remaining:.1f}s remaining", end='\r')
        time.sleep(1)
    
    print("\n5. Collecting monitoring data...")
    
    # Analyze neural activity
    neural_data = monitor.monitoring_data['neural_activity']
    if neural_data:
        print("   Neural Activity Analysis:")
        latest = neural_data[-1]['data']
        print(f"     - Current neurons: {latest.get('total_neurons', 'N/A')}")
        print(f"     - Active ratio: {latest.get('active_ratio', 0):.2%}")
        print(f"     - Firing rate: {latest.get('average_firing_rate', 0):.2f} Hz")
        
        # Calculate trends
        if len(neural_data) > 10:
            recent = [d['data'].get('average_firing_rate', 0) for d in neural_data[-10:]]
            trend = "increasing" if recent[-1] > recent[0] else "decreasing" if recent[-1] < recent[0] else "stable"
            print(f"     - Trend: {trend}")
    
    # Analyze system resources
    system_data = monitor.monitoring_data['system_resources']
    if system_data:
        print("\n   System Resource Analysis:")
        latest = system_data[-1]['data']
        print(f"     - CPU usage: {latest.get('cpu_percent', 0):.1f}%")
        print(f"     - Memory usage: {latest.get('memory_percent', 0):.1f}%")
        print(f"     - Thread count: {latest.get('thread_count', 'N/A')}")
    
    # Analyze alerts
    alert_count = monitor.alert_system.get_alert_count() if monitor.alert_system else 0
    print(f"\n   Alert System: {alert_count} alerts detected")
    
    # Analyze anomalies
    anomaly_count = monitor.anomaly_detector.get_anomaly_count() if monitor.anomaly_detector else 0
    print(f"   Anomaly Detection: {anomaly_count} anomalies detected")
    
    # Get recent alerts
    if monitor.alert_system:
        recent_alerts = monitor.alert_system.get_recent_alerts(5)
        if recent_alerts:
            print("\n   Recent Alerts:")
            for alert in recent_alerts:
                print(f"     - {alert['severity'].upper()}: {alert['description']}")
    
    # Get performance summary
    if monitor.performance_profiler:
        perf_summary = monitor.performance_profiler.get_performance_summary()
        if perf_summary:
            print("\n   Performance Summary:")
            print(f"     - Average CPU usage: {perf_summary.get('average_cpu_usage', 0):.1f}%")
            print(f"     - Average memory usage: {perf_summary.get('average_memory_usage', 0):.1f}%")
            print(f"     - Max CPU usage: {perf_summary.get('max_cpu_usage', 0):.1f}%")
            print(f"     - Data points collected: {perf_summary.get('data_points', 0)}")
    
    # Stop monitoring
    print("\n6. Stopping advanced monitoring...")
    monitor.stop_monitoring()
    print("   ✓ Advanced monitoring stopped!")
    
    print("\n✓ Advanced monitoring example completed!\n")


def example_cli_monitoring():
    """Example: Command-line interface monitoring"""
    print("=== CLI Monitoring Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create monitoring interface
    print("2. Creating monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring
    print("3. Starting monitoring...")
    monitor.start_monitoring(
        level=MonitoringLevel.DETAILED,
        update_interval=3.0
    )
    
    # Let it run to demonstrate CLI output
    print("4. Running CLI monitoring for 10 seconds...")
    print("   (In a real CLI, you would see live output here)")
    
    # Simulate some activity
    for i in range(3):
        interface.run_step(dt=1.0)
        time.sleep(2)
        print(f"   Step {i+1} completed")
    
    # Stop monitoring
    print("\n5. Stopping monitoring...")
    monitor.stop_monitoring()
    print("   ✓ CLI monitoring stopped!")
    
    print("\n✓ CLI monitoring example completed!\n")


def example_web_monitoring():
    """Example: Web-based monitoring setup"""
    print("=== Web Monitoring Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create monitoring interface
    print("2. Creating monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring
    print("3. Starting monitoring...")
    monitor.start_monitoring(
        level=MonitoringLevel.COMPREHENSIVE,
        update_interval=2.0
    )
    
    # Display web dashboard information
    print("4. Web Dashboard Setup:")
    print("   - Web dashboard would be available at: http://localhost:8080")
    print("   - Contains real-time neural activity monitoring")
    print("   - Includes system resource visualization")
    print("   - Provides alert and anomaly detection display")
    print("   - Supports historical data visualization")
    
    # Simulate some activity
    print("\n5. Running simulation for web dashboard...")
    for i in range(5):
        interface.run_step(dt=0.5)
        time.sleep(1)
        print(f"   Update {i+1}/5 completed")
    
    # Stop monitoring
    print("\n6. Stopping monitoring...")
    monitor.stop_monitoring()
    print("   ✓ Web monitoring stopped!")
    
    print("\n✓ Web monitoring example completed!\n")


def example_anomaly_detection():
    """Example: Anomaly detection in monitoring"""
    print("=== Anomaly Detection Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create monitoring interface with comprehensive level
    print("2. Creating monitoring interface with comprehensive level...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring
    print("3. Starting monitoring...")
    monitor.start_monitoring(
        level=MonitoringLevel.COMPREHENSIVE,
        update_interval=1.0
    )
    
    # Run simulation to generate data
    print("4. Running simulation to generate monitoring data...")
    for i in range(20):
        interface.run_step(dt=0.5)
        time.sleep(0.5)
    
    # Analyze anomalies
    print("5. Analyzing anomalies...")
    
    if monitor.anomaly_detector:
        anomaly_count = monitor.anomaly_detector.get_anomaly_count()
        print(f"   Detected {anomaly_count} anomalies")
        
        # Get detailed anomaly information
        # In a real implementation, you would process the anomaly history
        print("   Anomaly types typically detected:")
        print("     - CPU usage spikes")
        print("     - Memory usage anomalies")
        print("     - Neural activity patterns")
        print("     - Performance degradation")
        print("     - Resource contention")
    
    # Get performance anomalies
    if monitor.performance_profiler:
        perf_summary = monitor.performance_profiler.get_performance_summary()
        if perf_summary:
            print("\n   Performance metrics:")
            print(f"     - Average CPU usage: {perf_summary.get('average_cpu_usage', 0):.1f}%")
            print(f"     - Average memory usage: {perf_summary.get('average_memory_usage', 0):.1f}%")
            print(f"     - Sampling rate: {perf_summary.get('sampling_rate', 0):.1f} Hz")
    
    # Stop monitoring
    print("\n6. Stopping monitoring...")
    monitor.stop_monitoring()
    print("   ✓ Anomaly detection monitoring stopped!")
    
    print("\n✓ Anomaly detection example completed!\n")


def example_export_import():
    """Example: Export and import monitoring data"""
    print("=== Export/Import Example ===\n")
    
    # Initialize NLM system
    print("1. Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create monitoring interface
    print("2. Creating monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring
    print("3. Starting monitoring...")
    monitor.start_monitoring(
        level=MonitoringLevel.DETAILED,
        update_interval=2.0
    )
    
    # Let it run to collect data
    print("4. Collecting monitoring data for 10 seconds...")
    for i in range(5):
        interface.run_step(dt=1.0)
        time.sleep(1)
    
    # Export monitoring data
    print("5. Exporting monitoring data...")
    try:
        # In a real implementation, you would export to a file
        export_data = {
            'neural_activity': monitor.monitoring_data['neural_activity'],
            'system_resources': monitor.monitoring_data['system_resources'],
            'performance_metrics': monitor.monitoring_data['performance_metrics'],
            'timestamp': time.time(),
            'monitoring_level': monitor.monitoring_level.value
        }
        
        # Save to JSON file
        with open('monitoring_export.json', 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print("   ✓ Monitoring data exported to monitoring_export.json")
        print(f"   - Collected {len(export_data['neural_activity'])} neural activity samples")
        print(f"   - Collected {len(export_data['system_resources'])} system resource samples")
        print(f"   - Collected {len(export_data['performance_metrics'])} performance metrics samples")
        
    except Exception as e:
        print(f"   Note: Export simulation: {e}")
    
    # Stop monitoring
    print("\n6. Stopping monitoring...")
    monitor.stop_monitoring()
    print("   ✓ Monitoring stopped!")
    
    print("\n✓ Export/Import example completed!\n")


def main():
    """Run all examples"""
    print("NLM Brain System Monitoring Examples")
    print("=" * 50)
    print("\nThis script demonstrates the NLM Brain System monitoring")
    print("interface with various configurations and use cases.\n")
    
    examples = [
        ("Basic Monitoring", example_basic_monitoring),
        ("Dashboard Monitoring", example_dashboard_monitoring),
        ("Advanced Monitoring", example_advanced_monitoring),
        ("CLI Monitoring", example_cli_monitoring),
        ("Web Monitoring", example_web_monitoring),
        ("Anomaly Detection", example_anomaly_detection),
        ("Export/Import", example_export_import),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"   Error in {name} example: {e}")
            print(f"   Skipping {name} example.\n")
    
    print("All examples completed!")
    print("\nThe NLM Brain System monitoring interface provides:")
    print("• Real-time neural activity monitoring")
    print("• System resource tracking")
    print("• Memory usage monitoring")
    print("• Performance profiling")
    print("• Alert and anomaly detection")
    print("• Multiple interface support (CLI, Web, Terminal, API)")
    print("• Data export/import capabilities")
    print("• Extensible plugin system")
    print("• Customizable monitoring levels")


if __name__ == "__main__":
    main()