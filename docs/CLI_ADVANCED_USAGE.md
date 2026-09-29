# NLM CLI Advanced Usage Guide

## Advanced Configuration Management

### Configuration Templates and Variables

Templates support variable substitution using the `{variable}` syntax:

```bash
nlm config generate quick_sim -o custom_config.yaml \
  --neuron_count=5000 \
  --max_steps=20000 \
  --learning_rate=0.005
```

Available variables for templates:
- `neuron_count`: Number of neurons in the simulation
- `max_steps`: Maximum number of simulation steps
- `learning_rate`: Neural learning rate
- `window_size`: Memory window size
- `enable_development`: Enable developmental processes
- `enable_curiosity`: Enable curiosity-driven learning
- `enable_checkpointing`: Enable checkpointing for long simulations
- `enable_multithreading`: Enable multithreaded processing
- `enable_vectorization`: Enable vector operations
- `train_on_gpu`: Use GPU acceleration
- `gpu_count`: Number of GPUs to use

### Configuration File Formats

The CLI supports multiple configuration file formats:

#### YAML Configuration
```yaml
# config.yaml
template: large_scale
neuron_count: 100000
max_steps: 1000000
enable_checkpointing: true
enable_multithreading: true
learning_rate: 0.001
output:
  directory: "./results"
  format: "json"
  compress: true
```

#### JSON Configuration
```json
{
  "template": "experiment",
  "experiment_name": "my_experiment",
  "neuron_count": 5000,
  "max_steps": 5000,
  "enable_curiosity": true,
  "learning_rate": 0.01,
  "output": {
    "directory": "./results",
    "format": "hdf5"
  }
}
```

### Configuration Validation and Schema

Validate configuration files with schema checking:

```bash
# Validate configuration with strict schema checking
nlm config validate --strict my_config.yaml

# Validate and show warnings
nlm config validate --warn my_config.yaml
```

## Advanced Command Usage

### Run Command Advanced Options

#### Checkpointing and Recovery
```bash
# Run with checkpointing for long simulations
nlm run checkpoint long_config.yaml --checkpoint-interval=1000 --recovery-point=checkpoint_50

# Resume from checkpoint
nlm run checkpoint config.yaml --resume-from=checkpoint_50
```

#### Parallel Processing
```bash
# Run batch of simulations in parallel
nlm run batch batch_config.yaml --parallel --max-threads=8

# Run quick simulations in parallel
nlm run quick quick_config.yaml --parallel --thread-count=4
```

### Experiment Command Advanced Features

#### Experiment Dependencies
```bash
# Create experiment with dependencies
nlm experiment create exp1 --config config1.yaml
nlm experiment create exp2 --config config2.yaml --depends-on=exp1
nlm experiment create exp3 --config config3.yaml --depends-on=exp2
```

#### Experiment Parameters
```bash
# Run experiment with specific parameters
nlm experiment run my_experiment --param1=value1 --param2=value2 --param3=value3

# Run experiment with parameter sweep
nlm experiment run sweep_experiment --param sweep_range="1,2,3,4,5"
```

### Benchmark Command Advanced Options

#### Custom Benchmarking
```bash
# Run custom benchmark suite
nlm benchmark basic custom_config.yaml --test-mode="memory,performance,throughput"

# Run scalability test with custom metrics
nlm benchmark scalability config.yaml --metrics="time,memory,cpu" --iterations=10
```

#### Profiling
```bash
# Detailed profiling with sampling
nlm benchmark profile config.yaml --sampling-interval=100 --profile-memory --profile-cpu

# Generate performance report with custom metrics
nlm benchmark report config.yaml --metrics="throughput,efficiency,scaling" --output=report.pdf
```

## Analysis and Visualization

### Advanced Analysis Techniques

#### Multi-dimensional Analysis
```bash
# Analyze results from multiple dimensions
nlm analyze results ./results --dimensions="time,memory,performance" --correlation
nlm analyze patterns ./results --pattern-type="clustering,trending,anomalies"
```

#### Comparative Analysis
```bash
# Compare multiple experiment runs
nlm analyze compare ./results/run1 ./results/run2 ./results/run3 --similarity
nlm analyze trends ./results/run1 ./results/run2 --trend-type="linear,exponential,periodic"
```

### Advanced Visualization Options

#### Custom Visualization Types
```bash
# Generate custom visualization types
nlm visualize metrics ./results --type="heatmap,bar,scatter"

# Create multi-panel visualizations
nlm visualize results ./results --type="dashboard" --panels="learning,performance,memory"

# Export visualizations in different formats
nlm visualize metrics ./results --type="plotly" --format="html" --output="interactive_dashboard.html"
```

#### Real-time Visualization
```bash
# Start real-time visualization server
nlm visualize real-time ./results --port=8080 --update-interval=1000

# Connect to remote visualization
nlm visualize connect remote_server --host="192.168.1.100" --port=8080
```

## Script Generation and Automation

### Advanced Script Templates

#### Bash Script Generation
```bash
# Generate sophisticated bash script
nlm generate script --template bash_template --output run_experiment.sh \
  --neuron_count=1000 \
  --max_steps=10000 \
  --enable-gpu=true \
  --output-dir="./batch_results"

# Generate script with error handling
nlm generate script --template robust_bash --output robust_script.sh \
  --retry-attempts=3 \
  --timeout=3600 \
  --cleanup-on-error=true
```

#### Docker Script Generation
```bash
# Generate Docker file with specific requirements
nlm generate docker --config config.yaml --output Dockerfile \
  --base-image="tensorflow/tensorflow:latest-gpu" \
  --cuda-version="11.0" \
  --dependencies="python3-pip,git" \
  --environment="development"
```

#### SLURM Job Generation
```bash
# Generate SLURM job for cluster submission
nlm generate slurm --config config.yaml --output job.slurm \
  --nodes=4 \
  --ntasks-per-node=8 \
  --cpus-per-task=16 \
  --mem=128G \
  --time=72:00:00 \
  --partition=gpu
```

#### Kubernetes Manifest Generation
```bash
# Generate Kubernetes deployment
nlm generate kubernetes --config config.yaml --output k8s.yaml \
  --replicas=3 \
  --cpu-request="500m" \
  --cpu-limit="2000m" \
  --memory-request="1Gi" \
  --memory-limit="8Gi" \
  --image-pull-secrets="registry-secret"
```

## Integration with External Systems

### Performance Monitoring Integration

#### Real-time Monitoring
```bash
# Enable real-time performance monitoring
nlm run basic config.yaml --monitor --metrics="cpu,memory,network" --interval=1000

# Generate performance reports
nlm benchmark report config.yaml --generate-graphs --output=performance_dashboard.pdf
```

#### Resource Management
```bash
# Set resource limits and quotas
nlm run batch config.yaml --resource-manager=slurm --queue=high_priority --time=24:00:00

# Automatic scaling based on system load
nlm run basic config.yaml --auto-scale --min-threads=2 --max-threads=16 --scale-threshold=80
```

### Logging and Monitoring Integration

#### Advanced Logging
```bash
# Configure logging with multiple outputs
nlm run basic config.yaml --log-level=debug --log-file=simulation.log --log-console=true

# Enable structured logging
nlm run basic config.yaml --log-format=json --log-structured=true --log-metrics
```

#### Log Analysis
```bash
# Analyze simulation logs
nlm analyze logs simulation.log --format="performance,errors,warnings" --output=analysis.json

# Generate log reports
nlm analyze log-report simulation.log --generate-summary --output=log_report.pdf
```

## Distributed Computing

### Cluster Integration

#### MPI-based Parallel Processing
```bash
# Run distributed simulation
nlm run batch config.yaml --distributed-backend=mpi --num-machines=4 --ranks-per-machine=8

# Configure distributed execution
nlm generate slurm --config config.yaml --output job.slurm \
  --mpi-install=system \
  --modules="intelmpi/2019" \
  --extra-mpi-args="-x NCCL_DEBUG=INFO"
```

#### Cloud Integration
```bash
# Generate cloud deployment scripts
nlm generate docker --config config.yaml --output Dockerfile.cloud \
  --cloud-provider=aws \
  --instance-type=p3.8xlarge \
  --volume-size=500 \
  --ssh-key="your-key.pem"

# Generate Kubernetes deployment
nlm generate kubernetes --config config.yaml --output k8s.yaml \
  --namespace=ml-research \
  --service-account=nlm-sa \
  --image-pull-secrets=aws-ecr
```

### Workflow Automation

#### Experiment Pipelines
```bash
# Create automated experiment pipeline
nlm pipeline create my_pipeline \
  --stages="preprocess,run,analyze,visualize" \
  --workflow-file=automate.py

# Execute pipeline
nlm pipeline run my_pipeline --config=pipeline_config.yaml
```

#### Automated Parameter Sweeps
```bash
# Run automated parameter sweep
nlm experiment create parameter_sweep --config base_config.yaml
nlm experiment run parameter_sweep --parameters="learning_rate=[0.001,0.01,0.1],batch_size=[32,64,128]"
```

## Configuration File Examples

### Advanced Configuration (config_advanced.yaml)
```yaml
# Advanced configuration for large-scale experiments
experiment:
  name: large_scale_experiment
  description: Large-scale neural network simulation
  
parallel:
  enabled: true
  method: mpi
  num_machines: 4
  ranks_per_machine: 8
  
memory:
  strategy: hierarchical
  enable_compression: true
  compression_ratio: 0.5
  
performance:
  enable_profiling: true
  profiling_interval: 1000
  enable_metrics_collection: true
  metrics: ["cpu", "memory", "gpu", "network"]
  
output:
  format: hdf5
  compression: lz4
  chunk_size: 1000
  
monitoring:
  enable_real_time: true
  metrics_endpoint: "http://metrics-server:9090"
  alerting: true
  alert_thresholds:
    cpu_usage: 90
    memory_usage: 85
    gpu_usage: 95
    
logging:
  level: debug
  format: json
  structured: true
  file_rotation: true
  max_file_size: 100MB
  backup_count: 10
```

### Docker Compose Configuration (docker-compose.yaml)
```yaml
version: '3.8'
services:
  nlm-simulator:
    build: .
    image: nlm/simulator:latest
    ports:
      - "8080:8080"
    environment:
      - NLM_LOG_LEVEL=info
      - NLM_OUTPUT_DIR=/results
    volumes:
      - ./configs:/configs
      - ./results:/results
    command: ["nlm", "run", "basic", "/configs/config.yaml"]
    
  nlm-monitor:
    image: nlm/monitor:latest
    ports:
      - "9090:9090"
    environment:
      - MONITOR_PORT=9090
    depends_on:
      - nlm-simulator
```

## Performance Optimization

### Command-line Optimization

#### Performance Flags
```bash
# Enable performance optimizations
nlm run quick config.yaml --optimize --vectorization --multithreading

# Configure resource usage
nlm run basic config.yaml --thread-count=8 --memory-limit=16GB --gpu-enabled=true
```

#### Caching and Preloading
```bash
# Enable result caching
nlm run basic config.yaml --cache-results --cache-dir=./cache

# Preload configuration and data
nlm run basic config.yaml --preload-config --preload-data=true
```

### Output Optimization

#### Efficient Output Formats
```bash
# Use efficient output formats
nlm run basic config.yaml --output-format=hdf5 --compress=true

# Configure output compression
nlm run basic config.yaml --output-compression=lz4 --compression-level=9
```

## Debugging and Troubleshooting

### Advanced Debugging Techniques

#### Debug Mode
```bash
# Enable comprehensive debugging
nlm run basic config.yaml --debug --debug-level=verbose --debug-output=debug.log

# Debug with detailed tracing
nlm run basic config.yaml --trace --trace-level=detailed --trace-output=trace.json
```

#### Log Analysis
```bash
# Analyze simulation logs with filters
nlm analyze logs simulation.log --filter="level=error or level=warning" --count

# Generate debug reports
nlm analyze debug-report simulation.log --generate-stack-traces --output=debug_report.pdf
```

### Performance Profiling

#### Detailed Profiling
```bash
# Profile with detailed metrics
nlm benchmark profile config.yaml --profile-cpu --profile-memory --profile-network --output=profile_results.json

# Generate performance analysis
nlm benchmark report config.yaml --generate-flame-graph --generate-call-graph --output=performance_analysis.pdf
```

## Security and Access Control

### Secure Configuration

#### Password-protected Configuration
```bash
# Use encrypted configuration files
nlm config encrypt config.yaml --output=encrypted_config.enc --password=secret
nlm config decrypt encrypted_config.enc --output=config.yaml --password=secret
```

#### Access Control
```bash
# Set file permissions
nlm config validate config.yaml --restrict-permissions=true --owner=nlmuser --group=nlmsystem

# Audit configuration access
nlm config audit config.yaml --log-access=true --output=access_log.json
```

## Migration and Upgrade

### Configuration Migration

#### Migrate Old Configurations
```bash
# Migrate configuration from previous version
nlm config migrate old_config.yaml --from-version=0.3.0 --to-version=0.4.0 --output=migrated_config.yaml

# Upgrade configuration schema
nlm config upgrade config.yaml --schema-version=0.4.0 --apply-upgrades
```

#### Data Migration
```bash
# Migrate results between formats
nlm analyze migrate results.json --from-format=json --to-format=hdf5 --output=results.hdf5

# Convert legacy logs
nlm analyze migrate legacy.log --from-format=text --to-format=json --output=structured.log
```

## Custom Extensions

### Plugin System

#### Add Custom Commands
```cpp
// Example: Adding a custom command plugin
class CustomCommand : public CLICommand {
public:
    std::string getName() const override { return "custom"; }
    std::string getDescription() const override { return "Custom command example"; }
    int execute(const std::vector<std::string>& args) override {
        // Custom command implementation
        return 0;
    }
};
```

#### Extend CLI with New Features
```bash
# Load custom plugins
nlm run basic config.yaml --plugin=custom_plugin.so --plugin-path=./plugins

# Use extended command set
nlm extended --list-commands
nlm extended custom-command --help
```

## Best Practices

### Performance Best Practices

1. **Use appropriate configurations for your hardware**: Large simulations should use `large_scale` template, quick tests should use `quick_sim` template
2. **Monitor resource usage**: Use `--thread-count` and `--memory-limit` flags to control resource usage
3. **Leverage parallel processing**: Use `--parallel` and `--thread-count` for batch operations
4. **Optimize output formats**: Use HDF5 for large datasets, JSON for machine-readable output

### Workflow Best Practices

1. **Separate configuration and execution**: Generate configurations first, then run simulations
2. **Use automation scripts**: Generate scripts for repetitive tasks
3. **Implement proper error handling**: Use `--verbose` and `--quiet` flags appropriately
4. **Version control configurations**: Keep configuration files in version control
5. **Backup important results**: Use `--output-dir` to organize and backup results

### Security Best Practices

1. **Secure configuration files**: Use encryption for sensitive configurations
2. **Restrict file permissions**: Set appropriate permissions on configuration files
3. **Audit access logs**: Enable audit logging for security monitoring
4. **Use secure connections**: Use HTTPS for remote monitoring and visualization
5. **Regular updates**: Keep NLM CLI and dependencies updated

## Reference

### Command Reference

#### Quick Reference
```bash
# Core commands
nlm run basic [config]                    # Run basic simulation
nlm run quick [config]                    # Run quick test
nlm run long [config]                     # Run long simulation
nlm run batch [config]                    # Run batch of simulations
nlm run checkpoint [config]               # Run with checkpointing

nlm config generate [template]            # Generate config from template
nlm config validate [file]                # Validate configuration
nlm config merge [file1] [file2] -o out   # Merge configurations
nlm config export [file] --format fmt      # Export configuration
nlm config import [file] --format fmt      # Import configuration

nlm experiment create [name] --config c    # Create experiment
nlm experiment run [name] [options]       # Run experiment
nlm experiment list                       # List experiments
nlm experiment status [name]               # Show experiment status
nlm experiment analyze [name]              # Analyze experiment
nlm experiment export [name] --format fmt  # Export experiment data

nlm benchmark basic [config]              # Basic benchmark
nlm benchmark scalability [config]        # Scalability test
nlm benchmark memory [config]             # Memory benchmark
nlm benchmark profile [config]            # Detailed profiling
nlm benchmark report [config]             # Generate report

nlm analyze results [dir]                 # Analyze results
nlm analyze patterns [dir]                # Pattern analysis
nlm analyze trends [dir]                  # Trend analysis
nlm analyze compare [dir1] [dir2]         # Compare results
nlm analyze report [dir] --format fmt     # Generate analysis report

nlm visualize metrics [dir]               # Visualize metrics
nlm visualize memory [dir]                # Visualize memory usage
nlm visualize learning [dir]              # Visualize learning curves
nlm visualize network [dir]               # Visualize network structure
nlm visualize results [dir] --type t      # Generate visualizations
```

#### Environment Variables
```bash
export NLM_HOME=/path/to/nlm          # NLM installation directory
export NLM_LOG_LEVEL=info            # Logging level
export NLM_OUTPUT_DIR=./results       # Default output directory
export NLM_THREAD_COUNT=4            # Number of threads
```

#### Exit Codes
```bash
0    # Success
1    # General error
2    # Usage error
3    # Configuration error
4    # File not found error
5    # Permission error
6    # Resource limit exceeded
7    # Timeout error
8    # Interrupted by user
```

## Additional Resources

### Documentation
- [NLM Documentation](https://docs.nlm.ai) - Complete NLM documentation
- [API Reference](https://api.nlm.ai) - NLM API documentation
- [Examples Gallery](https://examples.nlm.ai) - Example configurations and scripts

### Community
- [NLM Discord](https://discord.nlm.ai) - Community chat
- [NLM GitHub](https://github.com/nml/nlm) - GitHub repository
- [NLM Mailing List](https://mail.nlm.ai) - Mailing list for announcements

### Support
- [NLM Support](https://support.nlm.ai) - Technical support
- [Bug Tracker](https://bugs.nlm.ai) - Report bugs
- [Feature Requests](https://features.nlm.ai) - Request new features

---

*Last updated: $(date)
NLM Neural Network Simulator v0.4.0*
