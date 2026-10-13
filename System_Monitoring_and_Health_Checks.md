# System Monitoring and Health Checks

This document provides information about the system monitoring and health checking infrastructure implemented for the NLM (熙然) brain simulation.

## Overview

The system monitoring and health checks provide comprehensive tools for monitoring the health and performance of the NLM brain simulation system. They include:

1. **System Health Monitor** - Comprehensive health checking and status monitoring
2. **Alert System** - Real-time alerting for system issues
3. **Resource Monitoring** - Continuous monitoring of system resources
4. **Health Reporting** - Automated health reports and diagnostics
5. **Failure Detection** - Early detection of potential failures

## Key Features

### System Health Monitor
- **Component health checks**: Verify functionality of all brain components
- **Resource health checks**: Monitor system resource usage
- **Integration health checks**: Verify connections between systems
- **Performance health checks**: Monitor performance metrics
- **Automated recovery**: Trigger recovery actions when issues detected

### Alert System
- **Multi-level alerting**: Different alert levels (INFO, WARNING, ERROR, CRITICAL)
- **Multiple output channels**: Console, file, email, webhooks
- **Alert aggregation**: Group related alerts to reduce noise
- **Escalation policies**: Automatically escalate unresolved alerts
- **Custom alert conditions**: Configurable alert triggers

### Resource Monitoring
- **Real-time monitoring**: Continuous monitoring of system resources
- **Historical tracking**: Track resource usage over time
- **Threshold-based alerts**: Trigger alerts when resources exceed thresholds
- **Trend analysis**: Identify resource usage patterns
- **Capacity planning**: Plan for future resource needs

### Health Reporting
- **Automated reporting**: Generate health reports at regular intervals
- **Executive summaries**: High-level health status for stakeholders
- **Detailed diagnostics**: Detailed technical information for developers
- **Export capabilities**: Export reports in multiple formats (HTML, JSON, CSV)
- **Custom reports**: Configurable report templates

### Failure Detection
- **Anomaly detection**: Detect unusual behavior patterns
- **Dependency monitoring**: Monitor dependencies between components
- **Timeout detection**: Detect slow or hanging operations
- **Error rate monitoring**: Monitor error rates and trigger alerts
- **Recovery verification**: Verify that recovery actions are successful

## Usage Examples

### Basic Health Monitoring

```cpp
// Initialize health monitor
HealthMonitor monitor;

// Start monitoring
monitor.start();

// Check system health
HealthMonitor::HealthReport report = monitor.checkHealth();

// Process health report
if (report.status == HealthMonitor::HealthStatus::Critical) {
    // Take action
    alertSystem.trigger(AlertSystem::Severity::Critical,
                       "System health is critical");
}
```

### Custom Health Checks

```cpp
// Add custom health check
monitor.addHealthCheck("neuromodulation", []() {
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    
    if (!dopamine || !curiosity) {
        return HealthMonitor::HealthCheckResult::Failed("Neuromodulation systems not available");
    }
    
    if (dopamine->getLevel() < 0.1f) {
        return HealthMonitor::HealthCheckResult::Warning("Dopamine level is low");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});
```

### Alert Configuration

```cpp
// Configure alert system
AlertSystem alertSystem;

// Add alert handler
alertSystem.addAlertHandler([](const AlertSystem::Alert& alert) {
    if (alert.severity == AlertSystem::Severity::Critical) {
        // Send email notification
        emailService.send("System Alert", alert.message);
    } else if (alert.severity == AlertSystem::Severity::Error) {
        // Log to system log
        syslog(LOG_ERR, alert.message.c_str());
    } else {
        // Log to info log
        logger.info(alert.message);
    }
});

// Configure alert thresholds
alertSystem.setThreshold(AlertSystem::Severity::Warning, "cpu_usage", 80.0f);
alertSystem.setThreshold(AlertSystem::Severity::Error, "memory_usage", 90.0f);
alertSystem.setThreshold(AlertSystem::Severity::Critical, "response_time", 1000ms);
```

## Integration with NLM Brain

### Brain Health Monitoring

```cpp
// Initialize brain health monitor
BrainHealthMonitor brainMonitor;

// Configure health checks
brainMonitor.addHealthCheck("memory_systems", []() {
    if (!brain->getWorkingMemory() || !brain->getEpisodicMemory()) {
        return HealthMonitor::HealthCheckResult::Failed("Memory systems not available");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

brainMonitor.addHealthCheck("plasticity", []() {
    if (!brain->getSTDP() || !brain->getHebbian()) {
        return HealthMonitor::HealthCheckResult::Failed("Plasticity systems not available");
    }
    
    // Check if plasticity is active
    if (brain->getSTDP()->getLTPWeight() <= 0.0f) {
        return HealthMonitor::HealthCheckResult::Warning("STDP LTP weight is zero");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

brainMonitor.addHealthCheck("development", []() {
    if (!brain->getDevelopmentSystem()) {
        return HealthMonitor::HealthCheckResult::Warning("Development system not available");
    }
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    if (stage == DevelopmentalStage::Adult && brain->getPlasticityLevel() < 0.1f) {
        return HealthMonitor::HealthCheckResult::Info("Brain is in adult stage with low plasticity");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});
```

### Neuromodulation Health Monitoring

```cpp
// Monitor neuromodulation systems
brainMonitor.addHealthCheck("dopamine", []() {
    auto* dopamine = brain->getDopamine();
    if (!dopamine) {
        return HealthMonitor::HealthCheckResult::Failed("Dopamine system not available");
    }
    
    float level = dopamine->getLevel();
    if (level < 0.05f) {
        return HealthMonitor::HealthCheckResult::Warning("Dopamine level is very low: " + std::to_string(level));
    } else if (level < 0.2f) {
        return HealthMonitor::HealthCheckResult::Info("Dopamine level is low: " + std::to_string(level));
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

brainMonitor.addHealthCheck("curiosity", []() {
    auto* curiosity = brain->getCuriosity();
    if (!curiosity) {
        return HealthMonitor::HealthCheckResult::Failed("Curiosity system not available");
    }
    
    float level = curiosity->getLevel();
    if (level < 0.01f) {
        return HealthMonitor::HealthCheckResult::Info("Curiosity level is very low: " + std::to_string(level));
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});
```

## Health Check Categories

### Memory System Health

```cpp
brainMonitor.addHealthCheck("working_memory", []() {
    auto* wm = brain->getWorkingMemory();
    if (!wm) {
        return HealthMonitor::HealthCheckResult::Failed("Working memory not available");
    }
    
    size_t capacity = wm->getCapacity();
    size_t currentSize = wm->getCurrentSize();
    
    if (currentSize > capacity * 0.9f) {
        return HealthMonitor::HealthCheckResult::Warning("Working memory usage is high: " + 
                                                     std::to_string(currentSize) + "/" + std::to_string(capacity));
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

brainMonitor.addHealthCheck("episodic_memory", []() {
    auto* em = brain->getEpisodicMemory();
    if (!em) {
        return HealthMonitor::HealthCheckResult::Failed("Episodic memory not available");
    }
    
    size_t episodeCount = em->getEpisodeCount();
    size_t maxEpisodes = em->getMaxEpisodes();
    
    if (episodeCount > maxEpisodes * 0.95f) {
        return HealthMonitor::HealthCheckResult::Warning("Episodic memory usage is high: " + 
                                                     std::to_string(episodeCount) + "/" + std::to_string(maxEpisodes));
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});
```

### Plasticity System Health

```cpp
brainMonitor.addHealthCheck("stdp", []() {
    auto* stdp = brain->getSTDP();
    if (!stdp) {
        return HealthMonitor::HealthCheckResult::Failed("STDP system not available");
    }
    
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    if (ltpWeight <= 0.0f || ltdWeight <= 0.0f) {
        return HealthMonitor::HealthCheckResult::Failed("STDP weights are not positive");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

brainMonitor.addHealthCheck("hebbian", []() {
    auto* hebbian = brain->getHebbian();
    if (!hebbian) {
        return HealthMonitor::HealthCheckResult::Failed("Hebbian system not available");
    }
    
    // Check Hebbian parameters
    return HealthMonitor::HealthCheckResult::Passed();
});
```

### Development System Health

```cpp
brainMonitor.addHealthCheck("development", []() {
    auto* dev = brain->getDevelopmentSystem();
    if (!dev) {
        return HealthMonitor::HealthCheckResult::Failed("Development system not available");
    }
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Check development system parameters
    if (stage == DevelopmentalStage::Initial) {
        return HealthMonitor::HealthCheckResult::Info("Brain is in initial developmental stage");
    } else if (stage == DevelopmentalStage::CriticalPeriod) {
        return HealthMonitor::HealthCheckResult::Info("Brain is in critical period");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});
```

## Alert Categories

### Performance Alerts

```cpp
// Configure performance alerts
alertSystem.setThreshold(AlertSystem::Severity::Warning, "firing_rate", 0.1f, "average");
alertSystem.setThreshold(AlertSystem::Severity::Error, "firing_rate", 0.01f, "average");
alertSystem.setThreshold(AlertSystem::Severity::Critical, "firing_rate", 0.001f, "average");

alertSystem.setThreshold(AlertSystem::Severity::Warning, "spike_throughput", 100000.0f);
alertSystem.setThreshold(AlertSystem::Severity::Error, "spike_throughput", 10000.0f);
alertSystem.setThreshold(AlertSystem::Severity::Critical, "spike_throughput", 1000.0f);
```

### Memory Alerts

```cpp
// Configure memory alerts
alertSystem.setThreshold(AlertSystem::Severity::Warning, "memory_usage", 80.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Error, "memory_usage", 90.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Critical, "memory_usage", 95.0f, "percentage");

alertSystem.setThreshold(AlertSystem::Severity::Warning, "allocation_rate", 1000000.0f, "per_second");
alertSystem.setThreshold(AlertSystem::Severity::Error, "allocation_rate", 10000000.0f, "per_second");
```

### System Alerts

```cpp
// Configure system alerts
alertSystem.setThreshold(AlertSystem::Severity::Warning, "cpu_usage", 80.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Error, "cpu_usage", 90.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Critical, "cpu_usage", 95.0f, "percentage");

alertSystem.setThreshold(AlertSystem::Severity::Warning, "disk_usage", 80.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Error, "disk_usage", 90.0f, "percentage");
alertSystem.setThreshold(AlertSystem::Severity::Critical, "disk_usage", 95.0f, "percentage");
```

## Health Check Results

### Health Check Result Types

```cpp
enum class HealthCheckResultType {
    Passed,      // Health check passed
    Info,        // Health check passed with info message
    Warning,     // Health check passed with warning message
    Failed,      // Health check failed
    Unknown      // Health check could not be determined
};

struct HealthCheckResult {
    HealthCheckResultType type;
    std::string message;
    std::map<std::string, std::string> metadata;
    
    static HealthCheckResult Passed() {
        return {HealthCheckResultType::Passed, ""};
    }
    
    static HealthCheckResult Info(const std::string& message) {
        return {HealthCheckResultType::Info, message};
    }
    
    static HealthCheckResult Warning(const std::string& message) {
        return {HealthCheckResultType::Warning, message};
    }
    
    static HealthCheckResult Failed(const std::string& message) {
        return {HealthCheckResultType::Failed, message};
    }
};
```

### Health Report Structure

```cpp
struct HealthReport {
    HealthStatus status;                           // Overall health status
    std::vector<std::string> warnings;            // Warning messages
    std::vector<std::string> errors;             // Error messages
    std::vector<std::string> info;               // Info messages
    float healthScore;                           // Health score (0.0 to 1.0)
    std::chrono::system_clock::time_point timestamp; // Report timestamp
    std::map<std::string, std::string> metadata;  // Additional metadata
    
    std::string getStatusString() const {
        switch (status) {
            case HealthStatus::Healthy: return "HEALTHY";
            case HealthStatus::Warning: return "WARNING";
            case HealthStatus::Critical: return "CRITICAL";
            case HealthStatus::Unknown: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
};
```

## Alert System

### Alert Severity Levels

```cpp
enum class AlertSeverity {
    Info,        // Informational alert
    Warning,     // Warning alert
    Error,       // Error alert
    Critical     // Critical alert
};
```

### Alert Types

```cpp
struct Alert {
    AlertSeverity severity;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::map<std::string, std::string> metadata;
    
    std::string getSeverityString() const {
        switch (severity) {
            case AlertSeverity::Info: return "INFO";
            case AlertSeverity::Warning: return "WARNING";
            case AlertSeverity::Error: return "ERROR";
            case AlertSeverity::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
};
```

### Alert Handlers

```cpp
// Alert handler function type
using AlertHandler = std::function<void(const Alert&)>;

// Add alert handler
alertSystem.addAlertHandler([](const Alert& alert) {
    // Handle alert
    switch (alert.severity) {
        case AlertSeverity::Info:
            // Log info
            logger.info(alert.message);
            break;
        case AlertSeverity::Warning:
            // Log warning
            logger.warning(alert.message);
            break;
        case AlertSeverity::Error:
            // Log error
            logger.error(alert.message);
            break;
        case AlertSeverity::Critical:
            // Send email notification
            emailService.send("Critical System Alert", alert.message);
            // Log error
            logger.error("CRITICAL: " + alert.message);
            break;
    }
});
```

## Integration Examples

### Complete Health Monitoring Setup

```cpp
// Initialize monitoring systems
HealthMonitor healthMonitor;
AlertSystem alertSystem;
PerformanceMonitor performanceMonitor;

// Configure alert handlers
alertSystem.addAlertHandler([](const AlertSystem::Alert& alert) {
    // Send to logging system
    logger.log(alert.severity, alert.message);
    
    // Send to monitoring system
    performanceMonitor.record("alerts", 1.0f, {{"severity", alert.getSeverityString()}});
});

// Configure health checks
healthMonitor.addHealthCheck("brain", []() {
    // Check brain health
    if (!brain->isHealthy()) {
        return HealthMonitor::HealthCheckResult::Failed("Brain is not healthy");
    }
    return HealthMonitor::HealthCheckResult::Passed();
});

healthMonitor.addHealthCheck("resources", []() {
    auto resources = performanceMonitor.getSystemResources();
    
    if (resources.cpuUsage > 90.0f) {
        return HealthMonitor::HealthCheckResult::Warning("CPU usage is high: " + std::to_string(resources.cpuUsage) + "%");
    }
    
    if (resources.memoryUsage > 90.0f) {
        return HealthMonitor::HealthCheckResult::Warning("Memory usage is high: " + std::to_string(resources.memoryUsage) + "%");
    }
    
    return HealthMonitor::HealthCheckResult::Passed();
});

// Start monitoring
healthMonitor.start();
performanceMonitor.start();

// Run health checks periodically
while (true) {
    auto report = healthMonitor.checkHealth();
    
    // Process health report
    if (report.status == HealthMonitor::HealthStatus::Critical) {
        alertSystem.trigger(AlertSystem::Severity::Critical,
                           "System health is critical: " + report.getStatusString());
    } else if (report.status == HealthMonitor::HealthStatus::Warning) {
        alertSystem.trigger(AlertSystem::Severity::Warning,
                           "System health is warning: " + report.getStatusString());
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
```

### Health Dashboard Integration

```cpp
// Health dashboard handler
class HealthDashboard {
public:
    void handleHealthRequest(const HttpRequest& request) {
        auto report = healthMonitor.checkHealth();
        
        // Generate HTML response
        std::string html = generateHealthDashboardHTML(report);
        
        sendHttpResponse(request, html, "text/html");
    }
    
private:
    std::string generateHealthDashboardHTML(const HealthMonitor::HealthReport& report) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html><head><title>NLM Health Dashboard</title></head>\n";
        ss << "<body>\n";
        ss << "  <h1>NLM Health Dashboard</h1>\n";
        ss << "  <p>Generated: " << getCurrentTimestamp() << "</p>\n";
        ss << "  <p>Status: <strong>" << report.getStatusString() << "</strong></p>\n";
        ss << "  <p>Health Score: <strong>" << report.healthScore << "</strong></p>\n";
        ss << "  <h2>Warnings</h2>\n";
        ss << "  <ul>\n";
        for (const auto& warning : report.warnings) {
            ss << "    <li>" << warning << "</li>\n";
        }
        ss << "  </ul>\n";
        ss << "  <h2>Errors</h2>\n";
        ss << "  <ul>\n";
        for (const auto& error : report.errors) {
            ss << "    <li>" << error << "</li>\n";
        }
        ss << "  </ul>\n";
        ss << "</body></html>\n";
        
        return ss.str();
    }
};
```

## Benefits

### System Reliability
- **Early failure detection**: Detect issues before they cause problems
- **Proactive maintenance**: Address issues before they require intervention
- **Automated recovery**: Automatically recover from non-critical failures
- **Reduced downtime**: Minimize system downtime through rapid detection and recovery

### Performance Optimization
- **Performance monitoring**: Monitor performance metrics and identify bottlenecks
- **Resource optimization**: Optimize resource usage based on monitoring data
- **Capacity planning**: Plan for future resource needs based on usage patterns
- **Load balancing**: Balance load across system resources

### Operational Efficiency
- **Reduced operational overhead**: Automate routine monitoring and maintenance tasks
- **Improved incident response**: Faster response to system issues
- **Better documentation**: Automatically generate health reports and documentation
- **Enhanced collaboration**: Share health information across teams

## Future Enhancements

### Planned Features

1. **Machine Learning Integration**: Use ML to predict system failures
2. **Predictive Analytics**: Analyze historical data to predict future issues
3. **Automated Recovery**: Automatically recover from detected issues
4. **Integration with Cloud Monitoring**: Integrate with cloud monitoring services
5. **Real-time Visualization**: Real-time health dashboards

### Research Integration

The system monitoring and health checks also support research activities:

- **Performance Analysis**: Study system performance characteristics
- **Reliability Research**: Research system reliability and failure patterns
- **Health Monitoring Research**: Research health monitoring techniques
- **Failure Analysis Research**: Research failure detection and recovery

## Conclusion

The system monitoring and health checks provide comprehensive monitoring capabilities for the NLM brain simulation system. They help detect issues early, optimize performance, and ensure system reliability. The tools are designed to be flexible, extensible, and easy to use, while providing detailed health information for system management.

By integrating system monitoring and health checks into the development and deployment of NLM, we ensure that the brain simulation system maintains high reliability and performance while supporting complex research activities.
