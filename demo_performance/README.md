# NLM Performance Monitoring Demo

This demo shows how to use the PerformanceMonitor in NLM simulations.

## Features Demonstrated

1. **Resource Monitoring**: Tracks CPU usage, memory usage, and file descriptors
2. **Progress Tracking**: Monitors simulation progress and estimates time remaining
3. **Neural Metrics**: Collects detailed neural activity metrics (firing rates, spikes, etc.)
4. **Custom Metrics**: Allows adding custom metrics for specific use cases
5. **Performance Alerts**: Automatically detects and alerts on performance issues
6. **Historical Data**: Stores historical data for analysis
7. **Data Export**: Exports metrics to CSV format for analysis

## Usage

1. Build with NLM libraries:
   ```bash
   mkdir build && cd build
   cmake ../demo_performance
   make
   ```

2. Run the demo:
   ```bash
   ./performance_demo
   ```

3. Press 'q' to quit during simulation

## Files

- `performance_demo.cpp`: Main demo program
- `metrics.csv`: Exported performance metrics (generated)
- `CMakeLists.txt`: Build configuration

## Metrics Collected

The demo tracks:
- Simulation progress (step, percentage, time remaining)
- Neural activity (firing rates, spike counts, plasticity events)
- System resources (memory, CPU usage)
- Custom metrics (temperature, pressure, humidity)

Alerts are generated for:
- Memory usage exceeding 1GB
- CPU usage exceeding 80%
- Long simulation times exceeding 24 hours
