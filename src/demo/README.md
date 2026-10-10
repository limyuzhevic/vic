# NLM Debug and Profiling Demo

## Overview

This demo application showcases the comprehensive debugging and profiling infrastructure implemented for the NLM (Neural Learning Machine) system. It demonstrates all the debugging, profiling, diagnostic, monitoring, and visualization features that have been added to support development, testing, and analysis of NLM applications.

## Features Demonstrated

### 1. Debug Infrastructure
- **Debug Logging**: Multi-level logging (TRACE, DEBUG, INFO, WARN, ERROR, FATAL) with metadata
- **Debug Session**: Pause/resume/step debugging, call stack capture, variable inspection
- **Breakpoint Management**: Conditional breakpoints with hit counting
- **Variable Inspection**: Real-time variable tracking and memory inspection

### 2. Profiling Infrastructure
- **Performance Profiling**: Operation-level timing and CPU usage tracking
- **Memory Profiling**: Allocation/deallocation tracking and leak detection
- **CPU Profiling**: Thread-aware CPU usage measurement
- **I/O Profiling**: Read/write operation tracking
- **Network Profiling**: Connection and traffic analysis
- **Algorithm Profiling**: Algorithm execution timing and comparison

### 3. Diagnostic Tools
- **System Diagnostics**: System health checking and resource analysis
- **Component Diagnostics**: Component state tracking and health checking
- **Configuration Diagnostics**: Configuration validation and error reporting
- **Dependency Diagnostics**: Dependency discovery and health checking
- **Error Diagnostics**: Error analysis and pattern detection
- **Performance Diagnostics**: Performance bottleneck detection and trend analysis

### 4. Monitoring Infrastructure
- **Real-Time Monitoring**: Continuous data collection and target management
- **Historical Monitoring**: Data storage and time-based querying
- **Alert Management**: Alert rule management and notification
- **Report Generation**: Automated diagnostic report generation
- **Trend Analysis**: Performance trend detection and anomaly analysis

### 5. Visualization Infrastructure
- **Debug Visualization**: Debug entry and call stack visualization
- **Performance Visualization**: Performance data and timeline visualization
- **Memory Visualization**: Memory usage and allocation visualization
- **Thread Visualization**: Thread activity and CPU usage visualization
- **Network Visualization**: Network activity and connection visualization

## Running the Demo

### Prerequisites
- C++20 compatible compiler
- CMake 3.16 or higher
- Make or Ninja build system

### Build Instructions

1. **Create Build Directory**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure CMake**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```

3. **Build the Demo**
   ```bash
   cmake --build . --target nlm_debug_demo
   ```

4. **Run the Demo**
   ```bash
   ./nlm_debug_demo
   ```

### Build Options
- `CMAKE_BUILD_TYPE=Debug`: Enable debug features and symbols
- `CMAKE_BUILD_TYPE=Release`: Optimized build for performance
- `NLM_DEBUG=1`: Force enable debug features
- `NLM_PROFILE=1`: Force enable profiling features
- `NLM_MONITOR=1`: Force enable monitoring features

## Demo Scenarios

### Scenario 1: Basic Debug Features
- Initialize debug logging with TRACE level
- Log messages with different severity levels
- Add metadata to debug entries
- Set up and test breakpoints
- Capture and inspect variables
- Generate debug reports

### Scenario 2: Performance Profiling
- Profile database query operations
- Track algorithm execution time
- Monitor memory allocation patterns
- Analyze I/O performance
- Profile network operations
- Compare algorithm performance

### Scenario 3: System Diagnostics
- Register components for diagnostics
- Validate configuration settings
- Scan for dependencies
- Log and analyze errors
- Track performance metrics
- Generate diagnostic reports

### Scenario 4: Real-Time Monitoring
- Set up monitoring targets
- Configure sampling intervals
- Generate monitoring data
- Evaluate alerts based on thresholds
- Generate monitoring reports
- Analyze trends

### Scenario 5: Visualization
- Visualize debug entries and timelines
- Create performance charts and heatmaps
- Display memory allocation patterns
- Show thread activity and CPU usage
- Visualize network traffic
- Generate comprehensive reports

## Technical Details

### Code Structure

#### Debug Infrastructure (`include/nlm/DebugInfrastructure.*`)
- `DebugLogger`: Enhanced logging with multiple levels
- `DebugSession`: Debugging state and control
- `BreakpointManager`: Breakpoint management
- `VariableInspector`: Variable and memory inspection
- `StackTrace`: Detailed stack trace information

#### Diagnostic Tools (`include/nlm/DiagnosticTools.*`)
- `SystemDiagnoser`: System-wide diagnostics
- `ComponentDiagnoser`: Component-specific diagnostics
- `ConfigurationDiagnoser`: Configuration validation
- `DependencyDiagnoser`: Dependency analysis
- `ErrorDiagnoser`: Error analysis and pattern detection
- `PerformanceDiagnoser`: Performance analysis and bottleneck detection

#### Monitoring Infrastructure (`include/nlm/MonitoringInfrastructure.*`)
- `RealTimeMonitor`: Real-time data collection
- `HistoricalMonitor`: Historical data storage
- `AlertManager`: Alert management and notification
- `ReportGenerator`: Diagnostic report generation
- `TrendAnalyzer`: Performance trend analysis

#### Visualization Infrastructure (`include/nlm/VisualizationInfrastructure.*`)
- `DebugVisualizer`: Debug information visualization
- `PerformanceVisualizer`: Performance metrics visualization
- `MemoryVisualizer`: Memory usage visualization
- `ThreadVisualizer`: Thread activity visualization
- `NetworkVisualizer`: Network activity visualization

### API Examples

#### Debug Logging
```cpp
DebugLogger& logger = DebugLogger::getGlobal();
logger.setLevel(DebugLevel::DEBUG);
logger.debug("Starting simulation", "brain.cpp", 42, "Brain::initialize");
```

#### Breakpoint Management
```cpp
BreakpointManager& bpManager = BreakpointManager::getGlobal();
int bpId = bpManager.addBreakpoint("brain.cpp", 123, "spike_count > 100");
bpManager.enableBreakpoint(bpId, true);
```

#### Performance Profiling
```cpp
PerformanceProfiler& perfProfiler = PerformanceProfiler::getGlobal();
perfProfiler.startProfiling("brain_step", "neural");
// ... code to profile ...
double duration = perfProfiler.stopProfiling("brain_step");
```

#### System Diagnostics
```cpp
SystemDiagnoser& sysDiagnoser = SystemDiagnoser::getGlobal();
sysDiagnoser.enableLogging(true);
bool isHealthy = sysDiagnoser.checkSystemHealth();
std::vector<std::string> issues = sysDiagnoser.getSystemHealthIssues();
```

#### Alert Management
```cpp
AlertManager& alertManager = AlertManager::getGlobal();
alertManager.addAlertRule("high_cpu", "system", "cpu_usage > 80.0", "high", "High CPU detected");
alertManager.evaluateAlerts(monitoringData);
```

#### Visualization
```cpp
DebugVisualizationData debugData;
debugData.sessionId = "debug_session_1";
debugData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(60);
debugData.endTime = std::chrono::system_clock::now();

std::string debugReport = DebugVisualizer::getGlobal().generateDebugReport(debugData);
```

## Configuration

### Environment Variables
- `NLM_DEBUG_LEVEL`: Set debug logging level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
- `NLM_DEBUG_ENABLED`: Enable/disable debugging (true/false)
- `NLM_PROFILE_ENABLED`: Enable/disable profiling (true/false)
- `NLM_MONITOR_ENABLED`: Enable/disable monitoring (true/false)

### Command Line Options
- `--debug-level LEVEL`: Set debug logging level
- `--debug-enabled`: Enable debugging
- `--profile-enabled`: Enable profiling
- `--monitor-enabled`: Enable monitoring

## Testing

### Running Tests
```bash
# Build and run demo
./build/nlm_debug_demo

# Or build with specific options
cmake -DNLM_DEBUG=1 -DNLM_PROFILE=1 -DNLM_MONITOR=1 ..
make
./nlm_debug_demo
```

### Expected Output
The demo should produce output showing:
- Debug logging messages at different levels
- Performance profiling results
- Diagnostic information and reports
- Alert generation and management
- Visualization output
- System health checks

## Performance Considerations

### Memory Usage
- Debug logging with metadata increases memory usage
- Variable inspection requires additional memory for tracking
- Performance profiling adds overhead proportional to number of operations

### CPU Usage
- Breakpoint checking adds small overhead
- Call stack capture can be expensive for deep stacks
- Profiling adds timing overhead

### Recommendations
- Use debug features in development environments only
- Profile selectively during performance analysis
- Configure monitoring sampling intervals appropriately
- Monitor resource usage when using debugging features

## Common Issues and Solutions

### Debug Logger Not Working
1. Check if debug logging is enabled
2. Verify debug level is set correctly
3. Ensure proper initialization

### Breakpoints Not Triggering
1. Verify breakpoint is enabled
2. Check conditional expression syntax
3. Ensure debugger is properly integrated

### Performance Profiling Not Recording
1. Verify profiling is enabled
2. Check if start/stop methods are called correctly
3. Verify target operation name matches

## Future Enhancements

### Suggested Improvements
1. **Enhanced Visualization**
   - Real-time graphing capabilities
   - 3D visualization support
   - Custom chart generation

2. **Advanced Profiling**
   - GPU profiling support
   - Network latency analysis
   - I/O pattern recognition

3. **Extended Diagnostics**
   - Machine learning-based anomaly detection
   - Automated performance optimization suggestions
   - Predictive maintenance capabilities

4. **Integration**
   - Web-based debugging interface
   - Mobile debugging support
   - Cloud-based profiling services

## Conclusion

The NLM Debug and Profiling Demo provides a comprehensive showcase of the debugging and profiling infrastructure implemented for the NLM system. It demonstrates how developers can use these features to:

- Debug NLM applications effectively
- Profile and optimize performance
- Diagnose system and component issues
- Monitor system behavior in real-time
- Visualize complex data and patterns

This infrastructure supports the full development lifecycle from initial debugging through performance analysis and production monitoring, making it an essential tool for NLM developers and researchers.
