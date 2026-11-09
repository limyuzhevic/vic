# Quick Start Examples for NLM Phase 6

## Example 1: Quick System Test

```bash
# Test basic integration with minimal settings
mkdir quick_test
cd quick_test
../nlm_phase6_demo --quick --checkpoint ./quick_checkpoint.bin
```

**Expected Output:**
- 100 neurons simulation
- 500 steps
- Basic memory integration test
- Checkpoint saved to `quick_checkpoint.bin`

## Example 2: Intermediate Research Experiment

```bash
# Configure and run an intermediate experiment
mkdir research_test
cd research_test

# Create custom config
cat > config.json << EOF
{
  "maxSteps": 3000,
  "neuronCount": 500,
  "regionCount": 2,
  "connectionProbability": 0.12,
  "enableCheckpointing": true,
  "enableReplay": true,
  "enableDevelopment": true,
  "checkpointPath": "./research_checkpoint.bin"
}
EOF

# Run experiment with custom config
../nlm_phase6_demo --neurons 500 --steps 3000 --checkpoint ./research_checkpoint.bin
```

**Expected Features:**
- Memory replay functionality enabled
- Developmental plasticity
- Checkpoint/restart capability
- ~3000 simulation steps

## Example 3: Advanced Performance Analysis

```bash
# Advanced experiment with full feature set
mkdir advanced_analysis
cd advanced_analysis

# Run with advanced configuration
../nlm_phase6_demo --neurons 800 --steps 5000 --checkpoint ./advanced_checkpoint.bin

# Monitor during execution (Ctrl+C to stop early)
# Watch for progress updates every 1000 steps
```

**Expected Features:**
- 800 neurons for complex interactions
- 5000 simulation steps for convergence
- Full checkpointing and replay
- Performance metrics collected

## Example 4: Comparative Experiment

```bash
#!/bin/bash
# Run comparative experiments with different settings

mkdir comparative_test
cd comparative_test

echo "Running comparative experiments..."

# Experiment 1: Minimal
../nlm_phase6_demo --neurons 100 --steps 500 --checkpoint ./comp1.bin \
    2>&1 | tee ./comp1.log

# Experiment 2: Medium
../nlm_phase6_demo --neurons 300 --steps 1500 --checkpoint ./comp2.bin \
    2>&1 | tee ./comp2.log

# Experiment 3: Large
../nlm_phase6_demo --neurons 600 --steps 3000 --checkpoint ./comp3.bin \
    2>&1 | tee ./comp3.log

echo "All experiments completed. Results in ./ comparative_test/"
```

## Example 5: Continuous Learning Experiment

```bash
# Continuous learning with incremental task introduction
mkdir continuous_learning
cd continuous_learning

# Create multiple checkpoint files for incremental learning
for task in task1 task2 task3 task4 task5; do
    echo "Running ${task}..."
    ../nlm_phase6_demo --neurons 200 --steps 1000 --checkpoint ./${task}_checkpoint.bin \
        2>&1 | grep -E "(Step|reward|firing|WorkingMem)" | tee -a ./${task}.log
    echo "${task} completed. Ready for next task."
    sleep 2
    echo "----------------------------------------"
done
```

## Example 6: Visualization Setup

```bash
# Set up for visualization and analysis
mkdir viz_setup
cd viz_setup

# Create directory structure for outputs
mkdir -p visualizations metrics checkpoints

# Run experiment with visualization enabled
../nlm_phase6_demo --full --checkpoint ./visualizations/main_checkpoint.bin

# After completion, analyze outputs:
# - visualizations/ : Generated neural activity visualizations
# - metrics/ : Performance metric CSV files
# - checkpoints/ : Checkpoint files for analysis
```

## Example 7: Scripted Experiment Pipeline

```bash
#!/bin/bash
# Automated experiment pipeline for research

PIPELINE_DIR="pipeline_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$PIPELINE_DIR"
cd "$PIPELINE_DIR"

echo "Starting NLM Phase 6 experiment pipeline..."

# Create parameter combinations
python3 << EOF
import json
import os

experiments = []

# Quick tests
for neurons in [100, 200]:
    for steps in [500, 1000]:
        experiments.append({
            "name": f"quick_{neurons}_neurons_{steps}_steps",
            "neurons": neurons,
            "steps": steps,
            "checkpoint": f"quick_{neurons}_{steps}.bin"
        })

# Intermediate tests  
for neurons in [500, 750]:
    for steps in [2000, 3000]:
        experiments.append({
            "name": f"intermediate_{neurons}_{steps}",
            "neurons": neurons,
            "steps": steps,
            "checkpoint": f"intermediate_{neurons}_{steps}.bin"
        })

# Save experiment plan
with open("experiment_plan.json", "w") as f:
    json.dump(experiments, f, indent=2)

print(f"Generated {len(experiments)} experiment configurations")
EOF

# Run experiments sequentially
python3 << EOF
import subprocess
import time
import json

with open("experiment_plan.json", "r") as f:
    experiments = json.load(f)

for i, exp in enumerate(experiments):
    print(f"\n[{i+1}/{len(experiments)}] Running {exp['name']}...")
    
    cmd = [
        "../nlm_phase6_demo",
        "--neurons", str(exp['neurons']),
        "--steps", str(exp['steps']),
        "--checkpoint", exp['checkpoint']
    ]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
        
        with open(f"{exp['name']}.log", "w") as log_file:
            log_file.write(result.stdout)
            if result.stderr:
                log_file.write("\nSTDERR:\n" + result.stderr)
        
        print(f"Completed {exp['name']} successfully")
        
    except subprocess.TimeoutExpired:
        print(f"Warning: {exp['name']} timed out after 5 minutes")
    except Exception as e:
        print(f"Error in {exp['name']}: {e}")
    
    # Small delay between experiments
    time.sleep(10)
EOF

echo "Pipeline completed in $PIPELINE_DIR"
echo "All results in: $(pwd)"
```

## Example 8: Error Handling and Recovery

```bash
# Robust experiment runner with error handling and recovery
mkdir robust_runner
cd robust_runner

# Create robust runner script
#!/bin/bash
MAX_RETRIES=3
RECOVERY_WAIT=30

for experiment in baseline comparative advanced; do
    echo "Running $experiment experiment..."
    
    attempt=0
    success=false
    
    while [ $attempt -lt $MAX_RETRIES ] && [ $success = false ]; do
        echo "Attempt $((attempt+1)) of $MAX_RETRIES"
        
        if ../nlm_phase6_demo --checkpoint ./$experiment.bin; then
            success=true
            echo "$experiment completed successfully"
        else
            attempt=$((attempt+1))
            if [ $attempt -lt $MAX_RETRIES ]; then
                echo "Experiment failed, waiting $RECOVERY_WAIT seconds before retry..."
                sleep $RECOVERY_WAIT
            else
                echo "Experiment $experiment failed after $MAX_RETRIES attempts"
                echo "Recovery: Attempting to load last checkpoint..."
                ../nlm_phase6_demo --checkpoint ./$experiment.bin --load-only
            fi
        fi
    done
done
```

## Example 9: Monitoring and Analysis

```bash
# Set up monitoring during experiment runs
mkdir monitoring
cd monitoring

# Create monitoring script
cat > monitor.sh << EOF
# Monitor system resources during experiment runs

EXPERIMENT="primary"
CHECKPOINT="$EXPERIMENT/bin"

# Start experiment in background
../nlm_phase6_demo --checkpoint $CHECKPOINT > experiment.log 2>&1 &
EXPERIMENT_PID=$!

echo "Monitoring experiment $EXPERIMENT (PID: $EXPERIMENT_PID)"

echo "=== SYSTEM MONITORING START "$(date)""" >> monitor.log

# Monitor memory and CPU every 10 seconds
while kill -0 $EXPERIMENT_PID 2>/dev nemusíte; do
    TIMESTAMP=$(date +%s)
    MEM_INFO=$(grep MemAvailable /proc/meminfo | awk '{print $2}')
    CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | sed 's/%us,.*//')
    
    echo "[$TIMESTAMP] Memory: ${MEM_INFO}KB, CPU: $CPU_USAGE%, Experiment: $(ps -p $EXPERIMENT_PID >/dev/null && echo RUNNING || echo STOPPED)" >> monitor.log
    
    sleep 10
done

# Experiment finished
wait $EXPERIMENT_PID
EXPERIMENT_RESULT=$?

echo "Experiment finished with result: $EXPERIMENT_RESULT"

echo "=== SYSTEM MONITORING END "$(date)"" >> monitor.log

# Generate summary report
python3 << EOF
import pandas as pd
import matplotlib.pyplot as plt

# Read experiment log and monitoring data
experiment_log = pd.read_csv('experiment.log', sep='\t') if 'experiment.log' else pd.DataFrame()
monitor_data = pd.read_csv('monitor.log')

# Generate summary
print("Experiment Summary:")
print(f"Duration: {len(monitor_data)} monitoring points")
print(f"Average Memory Usage: {monitor_data['MemUsage'].mean()}%")
print(f"Peak Memory Usage: {monitor_data['MemUsage'].max()}%")
print(f"Average CPU Usage: {monitor_data['CPUUsage'].mean()}%")
EOF
EOF

chmod +x monitor.sh

# Run monitoring
./monitor.sh
```

## Example 10: Integration with Other Tools

```bash
# Integrate NLM with analysis tools
cd integration_example

# Create Python analysis script
cat > analyze_results.py << EOF
#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path

class NLMResultsAnalyzer:
    def __init__(self, checkpoint_dir):
        self.checkpoint_dir = Path(checkpoint_dir)
        
    def load_metrics_from_checkpoint(self, checkpoint_file):
        """Extract metrics from NLM checkpoint files"""
        # This would interface with the actual NLM checkpoint format
        # For demonstration, we'll simulate loading
        data = {
            'step': range(0, 1000, 100),
            'reward': np.random.rand(10) * 10,
            'firing_rate': np.random.rand(10) * 50,
            'working_memory': np.random.rand(10) * 100,
            'episodic_episodes': np.random.randint(0, 50, 10)
        }
        return pd.DataFrame(data)
    
    def generate_performance_report(self, results_df):
        """Generate comprehensive performance report"""
        report = {
            'total_reward': results_df['reward'].sum(),
            'avg_firing_rate': results_df['firing_rate'].mean(),
            'max_reward': results_df['reward'].max(),
            'memory_efficiency': results_df['working_memory'].mean() / 
                                 results_df['episodic_episodes'].mean()
        }
        return report
    
    def create_visualization(self, results_df, output_path):
        """Create performance visualizations"""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Reward progression
        axes[0, 0].plot(results_df['step'], results_df['reward'])
        axes[0, 0].set_title('Reward Progression')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Total Reward')
        
        # Firing rate
        axes[0, 1].plot(results_df['step'], results_df['firing_rate'])
        axes[0, 1].set_title('Firing Rate')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Hz')
        
        # Working memory
        axes[1, 0].plot(results_df['step'], results_df['working_memory'])
        axes[1, 0].set_title('Working Memory Size')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Trace Count')
        
        # Episodic memory
        axes[1, 1].plot(results_df['step'], results_df['episodic_episodes'])
        axes[1, 1].set_title('Episodic Memory Episodes')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Episode Count')
        
        plt.tight_layout()
        plt.savefig(output_path, dpi=150, bbox_inches=' well')        return fig

# Usage example
analyzer = NLMResultsAnalyzer("./results")
checkpoint_file = "./results/checkpoint.bin"
results = analyzer.load_metrics_from_checkpoint(checkpoint_file)
report = analyzer.generate_performance_report(results)
analyzer.create_visualization(results, "performance_analysis.png")

print("Performance Report:")
for key, value in report.items():
    print(f"  {key}: {value:.2f}")
EOF

# Usage
python3 analyze_results.py
```

## Tips and Best Practices

### 1. Experiment Configuration
- Use `--quick` for initial system tests
- Use `--full` for comprehensive integration testing
- Always include `--checkpoint` for critical experiments

### 2. System Resources
- Monitor memory usage for >500 neurons
- Consider system load during batch runs
- Plan for checkpoint recovery

### 3. Experiment Design
- Increase neuron count for complex behaviors
- Use longer step counts for memory consolidation
- Combine with different configuration profiles

### 4. Result Analysis
- Export metrics for detailed analysis
- Use visualization tools for pattern recognition
- Document experimental parameters and conditions

## Quick Reference

| Command | Description | Neurons | Steps | Features |
|---------|-------------|---------|-------|----------|
| `--quick` | Fast system test | 100 | 500 | Basic |
| `--full` | Comprehensive test | 1000 | 5000 | Full |
| Custom | Custom configuration | Custom | Custom | Configurable |

**Save this file as `quick_examples.md` in your project directory.**