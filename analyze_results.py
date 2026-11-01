# NLM Advanced Profiler - Examples and Tutorials

## Introduction

This document provides comprehensive examples and tutorials for using the NLM Advanced Profiler to measure, analyze, and optimize NLM neural network performance.

## Quick Start Examples

### Example 1: Basic Performance Profiling

```bash
# Run basic profiling with default settings
./advanced_profiler --neurons 1000 --steps 1000

# This will profile a 1000-neuron network for 1000 steps
# Output includes: execution time, firing rates, memory usage, and optimization recommendations
```

### Example 2: Detailed Performance Analysis

```bash
# Run with detailed timing and memory tracking
./advanced_profiler --neurons 5000 --steps 2000 --detailed-timing --memory-tracking

# Benefits:
# - Precise step timing measurements
# - Memory usage tracking
# - CPU utilization monitoring
# - Network density calculations
# - Activity ratio analysis
```

### Example 3: Export Results to File

```bash
# Run profiler and export detailed results to JSON
./advanced_profiler --neurons 2000 --steps 3000 --output my_results.json --format json

# Run profiler and export detailed text report
./advanced_profiler --neurons 3000 --steps 5000 --output my_report.txt --format detailed
```

### Example 4: Performance Comparison

```bash
# Create a comparison script using the profiler multiple times
cat > compare_performance.sh << 'EOF'
#!/bin/bash
echo "Performance Comparison Results"
echo "=====================================" >> comparison_results.txt

echo "Configuration: 1000 neurons, 1000 steps" >> comparison_results.txt
./advanced_profiler --neurons 1000 --steps 1000 --output temp1.json --format json >> comparison_results.txt 2>&1

echo "" >> comparison_results.txt
echo "Configuration: 5000 neurons, 2000 steps" >> comparison_results.txt
./advanced_profiler --neurons 5000 --steps 2000 --output temp2.json --format json >> comparison_results.txt 2>&1

echo "" >> comparison_results.txt
echo "Configuration: 10000 neurons, 3000 steps" >> comparison_results.txt
./advanced_profiler --neurons 10000 --steps 3000 --output temp3.json --format json >> comparison_results.txt 2>&1

echo "Results saved to comparison_results.txt" >> comparison_results.txt
echo "Parse with Python: python parse_results.py comparison_results.txt" >> comparison_results.txt
EOF

chmod +x compare_performance.sh
./compare_performance.sh
```

## Advanced Usage Examples

### Example 5: Comprehensive Performance Analysis

```bash
# Run comprehensive analysis with all features enabled
./advanced_profiler --neurons 5000 --steps 5000 --detailed-timing --memory-tracking \
    --output comprehensive_analysis.json --format json

# Expected output includes:
# - Neural network metrics (neurons, synapses, density)
# - Performance metrics (steps/sec, spikes/step, firing rates)
# - Resource utilization (CPU, memory usage)
# - Scaling analysis (neurons/sec, synapses/sec, efficiency)
# - Optimization recommendations based on bottlenecks
# - Learning capacity and stability assessments
```

### Example 6: Batch Profiling

```bash
# Profile multiple network configurations
for neurons in 500 1000 2000 5000; do
    for steps in 1000 2000 5000; do
        echo "Profiling: ${neurons} neurons, ${steps} steps"
        ./advanced_profiler --neurons ${neurons} --steps ${steps} \
            --output profile_${neurons}_${steps}.json --format json
    done
done

# Generate summary report
echo "Generating performance summary..."
cat > summary_report.txt << 'EOF'
NLM Advanced Profiler Performance Summary
=========================================
Generated: $(date)

Performance Scaling Analysis:
=============================

EOF

# Add analysis of scaling trends
python analyze_scaling.py profiles_*.json >> summary_report.txt
```

### Example 7: Real-time Profiling

```bash
# Real-time profiling with continuous monitoring
cat > realtime_profiler.sh << 'EOF'
#!/bin/bash

PROFILING=false
OUTPUT_FILE="realtime_profile.txt"

echo "Starting real-time profiler..."
echo "Press 'q' to quit, 's' to toggle profiling, 'p' to pause" > $OUTPUT_FILE
echo "Time		Neurons/sec	CPU%	Memory(MB)" >> $OUTPUT_FILE

while true; do
    if [ "$PROFILING" = true ]; then
        # Run quick profile
        ./advanced_profiler --neurons 1000 --steps 100 --no-detailed-timing \
            --output temp_profile.json --format json 2>&1 | tail -n 20
        
        # Extract metrics (simplified)
        timestamp=$(date '+%Y-%m-%d %H:%M:%S')
        neurons_per_sec=$(echo "100 / 0.1" | bc -l)  # Placeholder
        cpu_percent="50"  # Placeholder
        memory_mb="25"    # Placeholder
        
        echo "$timestamp	$neurons_per_sec	$cpu_percent	$memory_mb" >> $OUTPUT_FILE
    fi
    
    read -t 1 -n 1 key
    case $key in
        q) break;;
        s) PROFILING=!$PROFILING;;
        p) echo "Profiling paused";;
    esac
done

echo "Real-time profiler stopped. Results saved to $OUTPUT_FILE"
EOF

chmod +x realtime_profiler.sh
./realtime_profiler.sh
```

## Python Integration Examples

### Example 8: Python Performance Analysis

```python
#!/usr/bin/env python3
"""
NLM Advanced Profiler Python Integration
Analyzes and visualizes profiler results
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pathlib import Path
import sys

def load_profiler_results(json_file):
    """Load profiler results from JSON file"""
    with open(json_file, 'r') as f:
        return json.load(f)

def analyze_performance_scaling(results_list):
    """Analyze performance scaling across different network sizes"""
    df = pd.DataFrame(results_list)
    
    print("Performance Scaling Analysis")
    print("=" * 50)
    print(f"Total profiles analyzed: {len(df)}")
    print()
    
    # Neurons per second analysis
    print("Neurons per Second:")
    print("-" * 30)
    neurons_per_sec = df.groupby('neurons')['steps_per_second'].mean()
    for neurons, rate in neurons_per_sec.items():
        print(f"  {neurons:6d} neurons: {rate:8.1f} neurons/sec")
    
    print()
    
    # Memory efficiency
    print("Memory Efficiency:")
    print("-" * 30)
    df['memory_efficiency'] = df['neurons'] / df['memory_usage_mb']
    efficiency = df['memory_efficiency'].mean()
    print(f"  Average memory efficiency: {efficiency:8.1f} neurons/MB")
    
    print()
    
    # Activity analysis
    print("Neural Activity:")
    print("-" * 30)
    print(f"  Average firing rate: {df['avg_firing_rate'].mean():.2f} Hz")
    print(f"  Maximum firing rate: {df['max_firing_rate'].max():.2f} Hz")
    print(f"  Activity range: {df['avg_firing_rate'].min():.2f} - {df['avg_firing_rate'].max():.2f} Hz")
    
    return df

def visualize_performance(df):
    """Create visualizations of performance metrics"""
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('NLM Advanced Profiler Performance Analysis', fontsize=16)
    
    # Plot 1: Neurons vs Steps/sec
    ax1 = axes[0, 0]
    colors = plt.cm.viridis(np.linspace(0, 1, len(df)))
    for idx, (i, row) in enumerate(df.iterrows()):
        ax1.scatter(row['neurons'], row['steps_per_second'], 
                   color=colors[idx], alpha=0.7, s=100)
        ax1.annotate(f"{row['neurons']}", 
                    xy=(row['neurons'], row['steps_per_second']),
                    xytext=(5, 5), textcoords='offset points', fontsize=8)
    
    ax1.set_xlabel('Number of Neurons')
    ax1.set_ylabel('Steps per Second')
    ax1.set_title('Performance Scaling')
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Memory Usage vs Neurons
    ax2 = axes[0, 1]
    sc2 = ax2.scatter(df['neurons'], df['memory_usage_mb'], 
                     c=df['cpu_usage_percent'], cmap='viridis', alpha=0.7, s=100)
    ax2.set_xlabel('Number of Neurons')
    ax2.set_ylabel('Memory Usage (MB)')
    ax2.set_title('Memory Usage vs Network Size')
    plt.colorbar(sc2, ax=ax2, label='CPU Usage (%)')
    ax2.grid(True, alpha=0.3)
    
    # Plot 3: Firing Rate Distribution
    ax3 = axes[1, 0]
    firing_rates = []
    labels = []
    for idx, (i, row) in enumerate(df.iterrows()):
        firing_rates.append(row['avg_firing_rate'])
        labels.append(f"{row['neurons']} neurons")
    
    bars = ax3.bar(range(len(firing_rates)), firing_rates, color=colors)
    ax3.set_xlabel('Profile Index')
    ax3.set_ylabel('Average Firing Rate (Hz)')
    ax3.set_title('Firing Rate Across Configurations')
    ax3.set_xticks(range(len(labels)))
    ax3.set_xticklabels(labels, rotation=45)
    ax3.grid(True, alpha=0.3)
    
    # Plot 4: Network Density vs Performance
    ax4 = axes[1, 1]
    scatter = ax4.scatter(df['network_density'], df['steps_per_second'],
                         c=df['activity_ratio'], cmap='plasma', alpha=0.7, s=100)
    ax4.set_xlabel('Network Density (synapses/neurons²)')
    ax4.set_ylabel('Steps per Second')
    ax4.set_title('Density vs Performance')
    plt.colorbar(scatter, ax=ax4, label='Activity Ratio (%)')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('performance_analysis.png', dpi=150, bbox_inches='tight')
    plt.show()
    
    print("Performance visualization saved to 'performance_analysis.png'")

def generate_recommendations(df):
    """Generate performance optimization recommendations"""
    recommendations = []
    
    # Memory bottleneck
    memory_avg = df['memory_usage_mb'].mean()
    if memory_avg > 100:
        recommendations.append({
            'issue': 'High memory usage detected',
            'recommendation': 'Consider reducing network size or enabling memory optimization features',
            'severity': 'high'
        })
    
    # CPU bottleneck
    cpu_avg = df['cpu_usage_percent'].mean()
    if cpu_avg > 80:
        recommendations.append({
            'issue': 'High CPU usage detected',
            'recommendation': 'Consider reducing network size or increasing parallelization',
            'severity': 'high'
        })
    
    # Low activity
    activity_avg = df['activity_ratio'].mean()
    if activity_avg < 1.0:
        recommendations.append({
            'issue': 'Low neural activity detected',
            'recommendation': 'Consider increasing input strength or adjusting neuron thresholds',
            'severity': 'medium'
        })
    
    # Low connectivity
    density_avg = df['network_density'].mean()
    if density_avg < 0.001:
        recommendations.append({
            'issue': 'Low network connectivity detected',
            'recommendation': 'Consider increasing connection probability in configuration',
            'severity': 'medium'
        })
    
    # Efficiency analysis
    df['efficiency'] = df['steps_per_second'] / df['neurons']
    efficiency_avg = df['efficiency'].mean()
    if efficiency_avg < 1.0:
        recommendations.append({
            'issue': 'Low computational efficiency',
            'recommendation': 'Consider optimizing network architecture or enabling vectorization',
            'severity': 'medium'
        })
    
    return recommendations

def main():
    if len(sys.argv) < 2:
        print("Usage: python analyze_results.py <json_file1> [json_file2 ...]")
        print("Example: python analyze_results.py profile_1000.json profile_5000.json")
        sys.exit(1)
    
    # Load all results
    results_list = []
    for json_file in sys.argv[1:]:
        try:
            result = load_profiler_results(json_file)
            results_list.extend(result['results'])
            print(f"Loaded {len(result['results'])} profiles from {json_file}")
        except Exception as e:
            print(f"Error loading {json_file}: {e}")
    
    if not results_list:
        print("No valid results found. Exiting.")
        sys.exit(1)
    
    # Analyze results
    df = analyze_performance_scaling(results_list)
    
    # Generate recommendations
    recommendations = generate_recommendations(df)
    
    print("\n" + "=" * 60)
    print("OPTIMIZATION RECOMMENDATIONS")
    print("=" * 60)
    
    if recommendations:
        for i, rec in enumerate(recommendations, 1):
            print(f"\n{i}. {rec['issue']}")
            print(f"   Severity: {rec['severity'].upper()}")
            print(f"   Recommendation: {rec['recommendation']}")
    else:
        print("\nNo optimization recommendations found. Performance is optimal.")
    
    # Create visualizations
    visualize_performance(df)
    
    print("\nAnalysis complete! Check 'performance_analysis.png' for visualizations.")

if __name__ == "__main__":
    main()
