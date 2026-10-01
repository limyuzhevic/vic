# ToolSystemAdvanced Documentation

## Overview

This document provides comprehensive documentation for the ToolSystemAdvanced class, which offers advanced Python APIs for the ToolSystem, including tool information, execution, and pipeline APIs.

## Introduction

The ToolSystemAdvanced class provides enhanced access to ToolSystem functionality through Python-friendly APIs, including comprehensive tool information, execution, and pipeline capabilities.

## Key Features

### Tool Information APIs
- **Available Tools**: Get list of available tools
- **Tool Information**: Get detailed information about specific tools
- **Tool Parameters**: Get tool parameters
- **Tool Returns**: Get tool return values

### Tool Execution APIs
- **Tool Execution**: Execute specific tools with parameters
- **Tool Pipeline**: Run complex tool pipelines
- **Tool Output**: Get tool execution output
- **Tool Logs**: Get tool execution logs

## API Reference

### Constructor
```python
def __init__(self):
    """Initialize ToolSystemAdvanced
    """
```

### Tool Information APIs

#### Tool Information

**`getAvailableTools()`**
```python
def getAvailableTools(self):
    """Get available tools
    
    Returns:
        list: List of available tools
    """
```

**`getToolInfo(tool_name)`**
```python
def getToolInfo(self, tool_name):
    """Get tool information
    
    Args:
        tool_name (str): Name of the tool
    
    Returns:
        dict: Tool information
    """
```

**`getToolParameters(tool_name)`**
```python
def getToolParameters(self, tool_name):
    """Get tool parameters
    
    Args:
        tool_name (str): Name of the tool
    
    Returns:
        list: List of tool parameters
    """
```

**`getToolReturns(tool_name)`**
```python
def getToolReturns(self, tool_name):
    """Get tool returns
    
    Args:
        tool_name (str): Name of the tool
    
    Returns:
        list: List of tool return values
    """
```

### Tool Execution APIs

#### Tool Execution

**`executeTool(tool_name, parameters)`**
```python
def executeTool(self, tool_name, parameters):
    """Execute a tool
    
    Args:
        tool_name (str): Name of the tool to execute
        parameters (dict): Parameters for the tool
    
    Returns:
        dict: Tool execution result
    """
```

**`runToolPipeline(tools, parameters)`**
```python
def runToolPipeline(self, tools, parameters):
    """Run a tool pipeline
    
    Args:
        tools (list): List of tool names to execute
        parameters (dict): Parameters for the tools
    
    Returns:
        dict: Pipeline execution result
    """
```

**`getToolOutput(tool_name)`**
```python
def getToolOutput(self, tool_name):
    """Get tool output
    
    Args:
        tool_name (str): Name of the tool
    
    Returns:
        dict: Tool output
    """
```

**`getToolLogs(tool_name)`**
```python
def getToolLogs(self, tool_name):
    """Get tool logs
    
    Args:
        tool_name (str): Name of the tool
    
    Returns:
        list: List of tool log entries
    """
```

### Python Integration APIs

#### Version and Information APIs

**`getPythonBindingsVersion()`**
```python
def getPythonBindingsVersion(self):
    """Get Python bindings version
    
    Returns:
        str: Python bindings version
    """
```

**`getPythonBindingsInfo()`**
```python
def getPythonBindingsInfo(self):
    """Get Python bindings information
    
    Returns:
        dict: Python bindings information including version, platform, python version, and features
    """
```

### Resource Monitoring APIs

#### Memory Usage APIs

**`getMemoryUsage()`**
```python
def getMemoryUsage(self):
    """Get memory usage
    
    Returns:
        int: Memory usage in bytes
    """
```

### Export APIs

#### Tool List Export

**`exportToolList(filepath)`**
```python
def exportToolList(self, filepath):
    """Export tool list to file
    
    Args:
        filepath (str): Path to export file
    
    Returns:
        str: Export result message
    """
```

## Usage Examples

### Basic Tool System Usage

```python
from nlm import ToolSystemAdvanced

# Create and initialize tool system
tool_system = ToolSystemAdvanced()

# Get available tools
print(f"Available Tools: {len(tool_system.getAvailableTools())}")
```

### Tool Information Usage

```python
tool_system = ToolSystemAdvanced()

# Get available tools
tools = tool_system.getAvailableTools()
print("Available Tools:")
for tool in tools:
    print(f"  - {tool['name']}: {tool['description']}")

# Get tool information for brain analyzer
tool_info = tool_system.getToolInfo('brain_analyzer')
if tool_info:
    print(f"\nBrain Analyzer Tool Info:")
    print(f"  Name: {tool_info['name']}")
    print(f"  Description: {tool_info['description']}")
    print(f"  Parameters: {tool_info['parameters']}")
    print(f"  Returns: {tool_info['returns']}")
```

### Tool Parameters Usage

```python
tool_system = ToolSystemAdvanced()

# Get tool parameters for brain analyzer
params = tool_system.getToolParameters('brain_analyzer')
print(f"Brain Analyzer Parameters: {params}")

# Get tool returns for brain analyzer
returns = tool_system.getToolReturns('brain_analyzer')
print(f"Brain Analyzer Returns: {returns}")
```

### Tool Execution Usage

```python
tool_system = ToolSystemAdvanced()

# Get available tools
tools = tool_system.getAvailableTools()
print("Available Tools:")
for tool in tools:
    print(f"  - {tool['name']}: {tool['description']}")

# Execute brain analyzer tool
tool_name = 'brain_analyzer'
parameters = {
    'brain_id': 'brain_123',
    'analysis_type': 'performance'
}
result = tool_system.executeTool(tool_name, parameters)
print(f"\nBrain Analyzer Result:")
print(f"  Status: {result['status']}")
print(f"  Execution Time: {result['execution_time']:.3f}s")
print(f"  Result: {result['result']}")
```

### Tool Pipeline Usage

```python
tool_system = ToolSystemAdvanced()

# Create tool pipeline
pipeline_tools = ['brain_analyzer', 'memory_profiler', 'performance_profiler']
parameters = {
    'brain_id': 'brain_123',
    'analysis_type': 'performance'
}

# Run tool pipeline
pipeline_result = tool_system.runToolPipeline(pipeline_tools, parameters)
print(f"\nPipeline Results:")
print(f"  Tools Executed: {pipeline_result['tools_executed']}")
print(f"  Pipeline Success: {pipeline_result['pipeline_success']}")
print(f"  Total Execution Time: {pipeline_result['total_execution_time']:.3f}s")
print(f"  Average Execution Time: {pipeline_result['average_execution_time']:.3f}s")

# Check individual tool results
for i, result in enumerate(pipeline_result['results'], 1):
    print(f"  Tool {i}: {result['tool_name']} - {result['status']} ({result['execution_time']:.3f}s)")
```

### Tool Output Usage

```python
tool_system = ToolSystemAdvanced()

# Get tool output
tool_name = 'brain_analyzer'
output = tool_system.getToolOutput(tool_name)
print(f"\nBrain Analyzer Output:")
print(f"  Tool Name: {output['tool_name']}")
print(f"  Last Execution: {output['last_execution']}")
print(f"  Output Data: {output['output_data']}")
print(f"  Status: {output['status']}")
```

### Tool Logs Usage

```python
tool_system = ToolSystemAdvanced()

# Get tool logs
tool_name = 'brain_analyzer'
logs = tool_system.getToolLogs(tool_name)
print(f"\nBrain Analyzer Logs:")
for log_entry in logs:
    print(f"  [{log_entry['timestamp']}] {log_entry['level']}: {log_entry['message']}")
```

### Export/Import Usage

```python
tool_system = ToolSystemAdvanced()

# Export tool list
export_file = "tool_list.json"
export_result = tool_system.exportToolList(export_file)
print(f"Tool list exported to {export_file}")
```

## Performance Tips

### 1. Tool Execution Optimization

```python
tool_system = ToolSystemAdvanced()

# Get available tools
tools = tool_system.getAvailableTools()

# Optimize tool execution based on tool characteristics
for tool in tools:
    tool_info = tool_system.getToolInfo(tool['name'])
    if tool_info:
        # Check tool complexity
        if len(tool_info['parameters']) > 10:
            print(f"{tool['name']}: Complex tool with {len(tool_info['parameters'])} parameters")
        else:
            print(f"{tool['name']}: Simple tool with {len(tool_info['parameters'])} parameters")
```

### 2. Tool Pipeline Optimization

```python
tool_system = ToolSystemAdvanced()

# Create optimized tool pipeline
pipeline_tools = ['brain_analyzer', 'memory_profiler', 'performance_profiler']
parameters = {
    'brain_id': 'brain_123',
    'analysis_type': 'performance'
}

# Run optimized pipeline
pipeline_result = tool_system.runToolPipeline(pipeline_tools, parameters)

# Analyze pipeline performance
total_time = pipeline_result['total_execution_time']
avg_time = pipeline_result['average_execution_time']

print(f"Pipeline Performance:")
print(f"  Total Time: {total_time:.3f}s")
print(f"  Average Time: {avg_time:.3f}s")
print(f"  Success Rate: {pipeline_result['pipeline_success']}")

# Optimize based on performance
if pipeline_result['total_execution_time'] > 10.0:
    print("Pipeline execution time is high. Consider optimizing tool sequence.")
```

## Best Practices

### 1. Tool Selection Best Practices

```python
tool_system = ToolSystemAdvanced()

# Get available tools
tools = tool_system.getAvailableTools()

# Select appropriate tools based on requirements
print("Tool Selection Criteria:")
for tool in tools:
    tool_info = tool_system.getToolInfo(tool['name'])
    if tool_info:
        complexity = 'High' if len(tool_info['parameters']) > 10 else 'Low'
        print(f"  - {tool['name']}: {tool['description']} (Complexity: {complexity})")
```

### 2. Tool Parameter Validation

```python
tool_system = ToolSystemAdvanced()

# Validate tool parameters before execution
tool_name = 'brain_analyzer'
parameters = {
    'brain_id': 'brain_123',
    'analysis_type': 'performance'
}

# Check tool information
tool_info = tool_system.getToolInfo(tool_name)
if tool_info:
    # Validate parameters
    required_params = tool_info.get('parameters', [])
    provided_params = parameters.keys()
    
    if set(required_params).issubset(set(provided_params)):
        print("Parameters are valid for tool execution.")
        result = tool_system.executeTool(tool_name, parameters)
        print(f"Tool execution successful: {result['status']}")
    else:
        print(f"Missing required parameters: {set(required_params) - set(provided_params)}")
```

### 3. Tool Pipeline Best Practices

```python
tool_system = ToolSystemAdvanced()

# Create tool pipeline
pipeline_tools = ['brain_analyzer', 'memory_profiler', 'performance_profiler']
parameters = {
    'brain_id': 'brain_123',
    'analysis_type': 'performance'
}

# Run tool pipeline
pipeline_result = tool_system.runToolPipeline(pipeline_tools, parameters)

# Analyze pipeline results
print(f"Pipeline Analysis:")
print(f"  Tools Executed: {pipeline_result['tools_executed']}")
print(f"  Pipeline Success: {pipeline_result['pipeline_success']}")
print(f"  Total Execution Time: {pipeline_result['total_execution_time']:.3f}s")
print(f"  Average Execution Time: {pipeline_result['average_execution_time']:.3f}s")

# Check for pipeline failures
if not pipeline_result['pipeline_success']:
    print("Pipeline execution failed. Check individual tool results.")
    for i, result in enumerate(pipeline_result['results'], 1):
        if result['status'] != 'success':
            print(f"  Tool {i} ({result['tool_name']}): {result['status']}")
```

### 4. Tool Output Analysis

```python
tool_system = ToolSystemAdvanced()

# Get tool output for analysis
tool_name = 'brain_analyzer'
output = tool_system.getToolOutput(tool_name)

# Analyze tool output
print(f"Tool Output Analysis:")
print(f"  Tool Name: {output['tool_name']}")
print(f"  Status: {output['status']}")
print(f"  Last Execution: {output['last_execution']}")

# Check for successful execution
if output['status'] == 'success':
    print(f"  Output Data Length: {len(output['output_data'])}")
    print("  Tool execution successful.")
else:
    print("  Tool execution failed.")
```

## Troubleshooting

### Common Issues and Solutions

1. **Tool Selection Issues**
   - **Issue**: Tool selection is difficult
   - **Solution**: Use `tool_system.getAvailableTools()` to get list of available tools

2. **Tool Execution Issues**
   - **Issue**: Tool execution fails
   - **Solution**: Use `tool_system.getToolInfo(tool_name)` to validate tool availability and parameters

3. **Tool Pipeline Issues**
   - **Issue**: Tool pipeline execution fails
   - **Solution**: Use `tool_system.runToolPipeline(tools, parameters)` to execute tool pipeline

4. **Tool Output Issues**
   - **Issue**: Tool output is missing or incomplete
   - **Solution**: Use `tool_system.getToolOutput(tool_name)` to get tool output

5. **Tool Log Issues**
   - **Issue**: Tool logs are missing or incomplete
   - **Solution**: Use `tool_system.getToolLogs(tool_name)` to get tool logs

### Debug Commands

```python
tool_system = ToolSystemAdvanced()

# Debug tool system
print(f"Python Bindings Version: {tool_system.getPythonBindingsVersion()}")

# Debug available tools
tools = tool_system.getAvailableTools()
print(f"Available Tools: {len(tools)}")
for tool in tools:
    print(f"  - {tool['name']}: {tool['description']}")

# Debug tool execution
tool_name = 'brain_analyzer'
parameters = {'brain_id': 'brain_123', 'analysis_type': 'performance'}
try:
    result = tool_system.executeTool(tool_name, parameters)
    print(f"Tool Execution Result: {result}")
except Exception as e:
    print(f"Tool Execution Failed: {e}")
```

## Migration Guide

### From Basic to Advanced API

If you're migrating from basic NLM Python bindings to advanced bindings, follow these steps:

1. **Upgrade Tool System Usage**
   ```python
   # Basic
   tool_system = ToolSystem()
   
   # Advanced
   tool_system = ToolSystemAdvanced()
   
   # Add advanced features
   tools = tool_system.getAvailableTools()
   tool_info = tool_system.getToolInfo('brain_analyzer')
   result = tool_system.executeTool('brain_analyzer', parameters)
   ```

2. **Upgrade Tool Execution**
   ```python
   # Basic
   tool_system = ToolSystem()
   result = tool_system.executeTool('brain_analyzer', parameters)
   
   # Advanced
   tool_system = ToolSystemAdvanced()
   
   # Add advanced features
   tools = tool_system.getAvailableTools()
   tool_info = tool_system.getToolInfo('brain_analyzer')
   pipeline_result = tool_system.runToolPipeline(tools, parameters)
   ```

3. **Upgrade Tool Pipeline**
   ```python
   # Basic
   tool_system = ToolSystem()
   pipeline_result = tool_system.runToolPipeline(['brain_analyzer', 'memory_profiler'], parameters)
   
   # Advanced
   tool_system = ToolSystemAdvanced()
   
   # Add advanced features
   tools = tool_system.getAvailableTools()
   pipeline_result = tool_system.runToolPipeline(tools, parameters)
   ```

## Conclusion

The ToolSystemAdvanced class provides comprehensive access to all tool systems through Python-friendly APIs. This documentation covers all major features including tool information, tool execution, tool pipeline, Python integration, resource monitoring, and export APIs.

By following the best practices outlined in this documentation, you can effectively use the ToolSystemAdvanced class to execute tools with high performance and reliability.

## See Also

- [NLM Python Bindings Documentation](https://nlm.org/docs/python-bindings) - General Python bindings documentation
- [BrainAdvanced Documentation](https://nlm.org/docs/brain-advanced) - BrainAdvanced documentation
- [AgentBrainAdvanced Documentation](https://nlm.org/docs/agent-advanced) - AgentBrainAdvanced documentation
- [ConfigAdvanced Documentation](https://nlm.org/docs/config-advanced) - ConfigAdvanced documentation
- [WorldAdvanced Documentation](https://nlm.org/docs/world-advanced) - WorldAdvanced documentation
- [SensoryInputAdvanced Documentation](https://nlm.org/docs/sensory-advanced) - SensoryInputAdvanced documentation
- [MemorySystemAdvanced Documentation](https://nlm.org/docs/memory-advanced) - MemorySystemAdvanced documentation
- [DevelopmentSystemAdvanced Documentation](https://nlm.org/docs/development-advanced) - DevelopmentSystemAdvanced documentation
- [NeuromodulationSystemAdvanced Documentation](https://nlm.org/docs/neuromodulation-advanced) - NeuromodulationSystemAdvanced documentation
- [PredictionSystemAdvanced Documentation](https://nlm.org/docs/prediction-advanced) - PredictionSystemAdvanced documentation
- [PerformanceSystemAdvanced Documentation](https://nlm.org/docs/performance-advanced) - PerformanceSystemAdvanced documentation
- [ExportSystemAdvanced Documentation](https://nlm.org/docs/export-advanced) - ExportSystemAdvanced documentation
- [NLM Examples](https://nlm.org/examples) - Code examples and tutorials
- [NLM Community](https://nlm.org/community) - Community support and forums

---

*Last updated: 2025-01-01*  
*Version: 0.1.0-advanced*  
*Copyright: NLM Development Team*