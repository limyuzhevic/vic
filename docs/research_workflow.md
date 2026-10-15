# NLM - Research Workflow Documentation

## Overview

This guide provides comprehensive documentation for research workflows using NLM (Neural Learning Machine). It covers experimental design, execution, analysis, and documentation standards for conducting research with neural network simulations.

## Research Workflow Overview

### Typical Research Workflow

```
┌─────────────────────────────────────────────────────────────────┐
│                    RESEARCH WORKFLOW                             │
├─────────────────────────────────────────────────────────────────┤
│ 1. Experimental Design                                           │
│    ├── Define research questions                                  │
│    ├── Select appropriate scale                                   │
│    ├── Design experimental protocol                               │
│    └── Establish hypotheses                                        │
├─────────────────────────────────────────────────────────────────┤
│ 2. Environment Setup                                              │
│    ├── Configure simulation parameters                             │
│    ├── Set up analysis tools                                       │
│    ├── Prepare data storage                                        │
│    └── Validate environment                                        │
├─────────────────────────────────────────────────────────────────┤
│ 3. Simulation Execution                                           │
│    ├── Run baseline experiments                                    │
│    ├── Execute experimental trials                                 │
│    ├── Collect performance metrics                                │
│    └── Ensure reproducibility                                     │
├─────────────────────────────────────────────────────────────────┤
│ 4. Data Analysis                                                  │
│    ├── Process and organize data                                   │
│    ├── Perform statistical analysis                                │
│    ├── Visualize results                                           │
│    └── Validate findings                                           │
├─────────────────────────────────────────────────────────────────┤
│ 5. Documentation and Reporting                                    │
│    ├── Document methodology                                        │
│    ├── Create visualizations                                      │
│    ├── Write analysis reports                                     │
│    └── Archive research data                                       │
└─────────────────────────────────────────────────────────────────┘
```

## Phase 1: Experimental Design

### Research Question Formulation

**Essential Elements**:
- **Clear Objectives**: Specific, measurable research questions
- **Hypothesis-Driven**: Testable predictions about system behavior
- **Scalable Design**: Experiments that work at multiple scales
- **Reproducible Protocol**: Detailed, step-by-step procedures

**Examples of Research Questions**:
- How does brain size affect learning capacity?
- What is the impact of neuromodulation on exploration vs. exploitation?
- How do developmental stages influence cognitive abilities?
- Can neural networks develop human-like social learning?

### Experimental Design Principles

#### 1. Scale Selection

**Micro Scale (1,000 neurons)**:
- **Use Case**: Proof-of-concept, algorithm testing
- **Advantages**: Fast iteration, easy debugging, low resource requirements
- **Limitations**: May not capture emergent phenomena

**Small Scale (10,000 neurons)**:
- **Use Case**: Basic experiments, initial parameter studies
- **Advantages**: Reasonable resource requirements, observable behaviors
- **Limitations**: Limited complexity

**Medium Scale (100,000 neurons)**:
- **Use Case**: Research studies, moderate complexity experiments
- **Advantages**: Good balance of realism and performance
- **Limitations**: Higher resource requirements

**Large Scale (1,000,000+ neurons)**:
- **Use Case**: Complex experiments, large-scale studies
- **Advantages**: Rich emergent behaviors, realistic scenarios
- **Limitations**: High computational cost

#### 2. Parameter Selection

**Configuration Presets**:
- **Research**: Enhanced learning, exploration, development
- **Development**: Training-focused, maturation dynamics
- **Testing**: Minimal complexity, stable behavior
- **Production**: Long-running, predictable behavior

**Key Parameters**:
```
brain.neuron_count          # Scale of system
plasticity.*              # Learning mechanisms
neuromod.*                 # Behavioral modulation
world.*                    # Environmental factors
development.*              # Developmental dynamics
```

#### 3. Experimental Protocol Design

**Standard Protocol Template**:
```
Protocol: [Experiment Name]
Scale: [Neuron count]
Duration: [Simulation steps]
Parameters: {key: value, ...}
Hypotheses: [Specific predictions]
Metrics: [Measured variables]

Steps:
1. Initialize environment
2. Run baseline ([steps] steps)
3. Execute experimental manipulation
4. Run test phase ([steps] steps)
5. Collect and analyze data
6. Repeat with different parameters
```

### Research Design Templates

#### Template 1: Learning Capacity Study

**Research Question**: Does brain size limit learning capacity?

**Design**:
```
Experiment: Learning Capacity vs Brain Size

Scales Tested: 1K, 10K, 100K neurons
Configuration: Research preset for all scales
Protocol:
- Baseline: 1000 steps (stabilization)
- Learning: 5000 steps with novel inputs
- Test: 2000 steps with generalization test

Metrics:
- Learning speed (steps to criterion)
- Final performance level
- Memory retention
- Generalization ability
```

#### Template 2: Neuromodulation Study

**Research Question**: How does neuromodulation balance exploration vs. exploitation?

**Design**:
```
Experiment: Neuromodulation Effects on Behavior

Variables:
- Dopamine scale: 0.5x, 1.0x, 2.0x
- Curiosity: enabled/disabled
- Novelty detection: enabled/disabled

Protocol:
- Exploration phase (500 steps)
- Exploitation phase (1000 steps)
- Test phase with novel stimuli (500 steps)

Metrics:
- Exploration rate
- Exploitation efficiency
- Novelty response
- Learning speed
```

#### Template 3: Developmental Study

**Research Question**: How do developmental stages affect cognitive abilities?

**Design**:
```
Experiment: Developmental Trajectories

Development Profiles:
- Normal development (maturation rate: 0.8)
- Accelerated development (maturation rate: 1.2)
- Delayed development (maturation rate: 0.4)
- Critical period manipulation

Protocol:
- Early development (critical period)
- Adult learning (post-critical period)
- Cross-context transfer

Metrics:
- Learning curve shape
- Retention over time
- Cross-context performance
- Behavioral flexibility
```

## Phase 2: Environment Setup

### Environment Configuration

#### Software Environment

**Required Dependencies**:
```bash
# Core requirements
python3 >= 3.8
pip install pynlm

# Optional for analysis
pip install matplotlib numpy scipy pandas
pip install jupyter notebook

# Development tools
pip install pytest mypy black flake8
```

**Configuration Files**:
- `config.json`: Main simulation configuration
- `experiment_config.json`: Experiment-specific settings
- `analysis_config.json`: Analysis and visualization settings

#### Hardware Environment

**Scale-Specific Requirements**:

**Micro Scale (1,000 neurons)**:
- **CPU**: Modern CPU (2+ cores)
- **RAM**: 4-8 GB
- **Storage**: SSD for data files

**Small Scale (10,000 neurons)**:
- **CPU**: Quad-core or better
- **RAM**: 8-16 GB
- **Storage**: SSD, multiple GB capacity

**Medium Scale (100,000 neurons)**:
- **CPU**: 8+ cores
- **RAM**: 16-32 GB
- **Storage**: SSD, 50+ GB capacity

**Large Scale (1,000,000+ neurons)**:
- **CPU**: 16+ cores, hyperthreading
- **RAM**: 64+ GB
- **Storage**: NVMe SSD, 500+ GB capacity

**X-Large Scale (10,000,000+ neurons)**:
- **CPU**: 32+ cores
- **RAM**: 256+ GB
- **Storage**: Multiple NVMe SSDs, TB capacity

#### Analysis Environment Setup

**Data Processing Pipeline**:
```python
# Analysis environment setup
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy import stats
import json

# Create analysis tools
class NLMAnalysisTools:
    def __init__(self):
        self.data_storage = {}
        self.analysis_results = {}
        self.visualizations = {}
    
    def setup_analysis_pipeline(self, config):
        """Setup complete analysis pipeline."""
        # Initialize data storage
        self.initialize_data_storage()
        
        # Setup analysis tools
        self.setup_visualization_tools()
        self.setup_statistical_tools()
        
        # Configure output formats
        self.configure_output_formats()
    
    def initialize_data_storage(self):
        """Initialize data storage system."""
        import sqlite3
        self.db_connection = sqlite3.connect('nlm_research.db')
        self.setup_database_schema()
    
    def setup_visualization_tools(self):
        """Setup visualization tools."""
        self.plot_style = {
            'figure.figsize': (12, 8),
            'font.size': 10,
            'axes.labelsize': 12,
            'axes.titlesize': 14,
            'xtick.labelsize': 10,
            'ytick.labelsize': 10,
            'legend.fontsize': 10,
            'grid.alpha': 0.3
        }
        plt.rcParams.update(self.plot_style)
    
    def setup_statistical_tools(self):
        """Setup statistical analysis tools."""
        self.statistical_tests = {
            't_test': stats.ttest_ind,
            'mann_whitney': stats.mannwhitneyu,
            'ks_test': stats.ks_2samp,
            'anova': stats.f_oneway,
            'correlation': stats.pearsonr
        }
    
    def configure_output_formats(self):
        """Configure output file formats."""
        self.output_formats = {
            'json': True,
            'csv': True,
            'hdf5': True,
            'sqlite': True,
            'plot': True,
            'pdf_report': True
        }
```

### Validation and Quality Assurance

#### Environment Validation

**Script for Environment Validation**:
```python
import pynlm
import sys
import platform
import psutil

def validate_research_environment():
    """Validate research environment setup."""
    
    print("Validating NLM Research Environment")
    print("=" * 50)
    
    # Check Python version
    print(f"\nPython Version: {sys.version}")
    if sys.version_info < (3, 8):
        print("WARNING: Python 3.8+ recommended")
    
    # Check platform
    print(f"Platform: {platform.system()} {platform.release()}")
    
    # Check memory
    memory = psutil.virtual_memory()
    print(f"\nMemory: {memory.total / (1024**3):.1f} GB available")
    if memory.available < 4 * (1024**3):
        print("WARNING: Less than 4GB RAM available")
    
    # Check NLM installation
    try:
        import pynlm
        print("✓ NLM Python package loaded successfully")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        print("✓ Configuration creation works")
        
    except ImportError as e:
        print(f"✗ NLM import failed: {e}")
        return False
    
    # Check visualization libraries
    optional_libs = {
        'matplotlib': 'plotting',
        'numpy': 'numerical computation',
        'pandas': 'data analysis',
        'scipy': 'statistical analysis'
    }
    
    print("\nOptional libraries:")
    for lib, purpose in optional_libs.items():
        try:
            __import__(lib)
            print(f"  ✓ {lib}: {purpose}")
        except ImportError:
            print(f"  ✗ {lib}: {purpose} (not installed)")
    
    # Check disk space
    import os
    disk_usage = psutil.disk_usage('/')
    print(f"\nDisk Space: {disk_usage.free / (1024**3):.1f} GB free")
    if disk_usage.free < 10 * (1024**3):
        print("WARNING: Less than 10GB disk space available")
    
    # Configuration validation
    print("\nConfiguration validation:")
    config = pynlm.createResearchConfig()
    errors, warnings = pynlm.validate_config(config)
    
    if errors:
        print("  Configuration errors:")
        for error in errors:
            print(f"    - {error}")
    
    if warnings:
        print("  Configuration warnings:")
        for warning in warnings:
            print(f"    - {warning}")
    
    if not errors and not warnings:
        print("  ✓ Configuration is valid")
    
    print("\n" + "=" * 50)
    print("Environment validation complete")
    
    return True

# Run validation
if __name__ == "__main__":
    validate_research_environment()
```

#### Data Storage Setup

**SQLite Database Schema**:
```sql
-- Experiment metadata
CREATE TABLE experiments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    scale INTEGER NOT NULL,
    config TEXT NOT NULL,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    status TEXT DEFAULT 'running'
);

-- Simulation runs
CREATE TABLE simulation_runs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    experiment_id INTEGER,
    run_number INTEGER NOT NULL,
    steps INTEGER NOT NULL,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    total_spikes INTEGER,
    FOREIGN KEY (experiment_id) REFERENCES experiments(id)
);

-- Time series data
CREATE TABLE time_series_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id INTEGER,
    step INTEGER NOT NULL,
    timestamp REAL,
    firing_percentage REAL,
    activity_level REAL,
    e_i_ratio REAL,
    FOREIGN KEY (run_id) REFERENCES simulation_runs(id)
);

-- Metric measurements
CREATE TABLE metrics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id INTEGER,
    metric_name TEXT NOT NULL,
    metric_value REAL,
    step INTEGER,
    FOREIGN KEY (run_id) REFERENCES simulation_runs(id)
);
```

## Phase 3: Simulation Execution

### Experiment Management

#### Experiment Framework

**Comprehensive Experiment Manager**:
```python
import pynlm
import json
import time
import sqlite3
from typing import Dict, List, Any

class NLMSExperimentManager:
    """Manages NLM research experiments with comprehensive tracking."""
    
    def __init__(self, db_path="nlm_research.db"):
        self.db_path = db_path
        self.setup_database()
        self.current_experiment = None
        self.current_run = None
    
    def setup_database(self):
        """Setup experiment database."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Create tables (same as SQL schema above)
        # ... table creation code ...
        
        conn.commit()
        conn.close()
    
    def create_experiment(self, name: str, description: str, 
                         scale: int, config: Dict) -> int:
        """Create new experiment."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            INSERT INTO experiments (name, description, scale, config, start_time)
            VALUES (?, ?, ?, ?, datetime('now'))
        """, (name, description, scale, json.dumps(config)))
        
        experiment_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        return experiment_id
    
    def start_simulation_run(self, experiment_id: int, run_number: int,
                           steps: int, config: Dict) -> int:
        """Start a simulation run."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            INSERT INTO simulation_runs 
            (experiment_id, run_number, steps, start_time)
            VALUES (?, ?, ?, datetime('now'))
        """, (experiment_id, run_number, steps))
        
        run_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        return run_id
    
    def run_simulation(self, config: Dict, steps: int) -> Dict[str, Any]:
        """Run simulation with comprehensive tracking."""
        
        start_time = time.time()
        
        # Create experiment brain
        brain = pynlm.createBrain(config)
        
        # Initialize tracking data
        tracking_data = {
            'steps': [],
            'firing_percentages': [],
            'activity_levels': [],
            'e_i_ratios': [],
            'spike_counts': []
        }
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
            # Collect metrics
            stats = brain.getFiringStatistics()
            tracking_data['steps'].append(step)
            tracking_data['firing_percentages'].append(stats['firing_percentage'])
            tracking_data['activity_levels'].append(stats['activity_percentage'])
            tracking_data['e_i_ratios'].append(stats['e_i_ratio'])
            tracking_data['spike_counts'].append(stats['total_spikes'])
            
            # Store data periodically
            if step % 100 == 0:
                self.store_time_series_data(self.current_run, step, stats)
        
        end_time = time.time()
        
        # Calculate final metrics
        final_stats = brain.getFiringStatistics()
        
        results = {
            'total_spikes': brain.getTotalSpikeCount(),
            'steps_per_second': steps / (end_time - start_time),
            'final_firing_percentage': final_stats['firing_percentage'],
            'final_activity_level': final_stats['activity_percentage'],
            'final_e_i_ratio': final_stats['e_i_ratio'],
            'final_spikes_per_neuron': final_stats['spikes_per_neuron'],
            'final_firing_rate': final_stats['average_firing_rate'],
            'tracking_data': tracking_data
        }
        
        # Store results
        self.store_simulation_results(self.current_run, results)
        
        return results
    
    def store_time_series_data(self, run_id: int, step: int, stats: Dict):
        """Store time series data."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            INSERT INTO time_series_data 
            (run_id, step, timestamp, firing_percentage, activity_level, e_i_ratio)
            VALUES (?, ?, ?, ?, ?, ?)
        """, (run_id, step, time.time(),
              stats['firing_percentage'], stats['activity_percentage'],
              stats['e_i_ratio']))
        
        conn.commit()
        conn.close()
    
    def store_simulation_results(self, run_id: int, results: Dict):
        """Store simulation results."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        for metric_name, metric_value in results.items():
            if metric_name != 'tracking_data':
                cursor.execute("""
                    INSERT INTO metrics (run_id, metric_name, metric_value)
                    VALUES (?, ?, ?)
                """, (run_id, metric_name, metric_value))
        
        conn.commit()
        conn.close()
    
    def get_experiment_summary(self, experiment_id: int) -> Dict:
        """Get experiment summary statistics."""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Get experiment metadata
        cursor.execute("SELECT * FROM experiments WHERE id = ?", (experiment_id,))
        experiment = cursor.fetchone()
        
        # Get all simulation runs
        cursor.execute("""
            SELECT * FROM simulation_runs WHERE experiment_id = ?
            ORDER BY run_number
        """, (experiment_id,))
        
        runs = cursor.fetchall()
        
        # Calculate summary statistics
        summary = {
            'experiment_id': experiment_id,
            'name': experiment[1],
            'scale': experiment[2],
            'runs': []
        }
        
        for run in runs:
            run_summary = {
                'run_id': run[0],
                'run_number': run[2],
                'steps': run[3],
                'start_time': run[4],
                'end_time': run[5],
                'total_spikes': run[6],
                'duration_seconds': (time.time() - run[4])
            }
            summary['runs'].append(run_summary)
        
        conn.close()
        return summary
```

#### Simulation Execution Examples

**Example 1: Basic Experiment**:
```python
# Initialize experiment manager
experiment_manager = NLMSExperimentManager()

# Create experiment
experiment_id = experiment_manager.create_experiment(
    name="Brain Size Learning Study",
    description="Testing how brain size affects learning capacity",
    scale=10000,  # Small scale
    config={
        "brain.neuron_count": 10000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.learning_rate": 0.001
    }
)

# Run multiple simulation runs
for run_num in range(3):
    experiment_manager.current_run = experiment_manager.start_simulation_run(
        experiment_id=experiment_id,
        run_number=run_num + 1,
        steps=2000,
        config={
            "brain.neuron_count": 10000,
            "plasticity.stdp.learning_rate": 0.001
        }
    )
    
    results = experiment_manager.run_simulation(
        config={
            "brain.neuron_count": 10000,
            "plasticity.stdp.learning_rate": 0.001
        },
        steps=2000
    )
    
    print(f"Run {run_num + 1} completed: {results['total_spikes']} total spikes")
    print(f"Average firing: {results['final_firing_percentage']:.1f}%")

# Get experiment summary
summary = experiment_manager.get_experiment_summary(experiment_id)
print(f"\nExperiment Summary: {summary['name']}")
print(f"Total runs: {len(summary['runs'])}")
print(f"Average spikes per run: {sum(r['total_spikes'] for r in summary['runs']) / len(summary['runs']):.0f}")
```

**Example 2: Multi-Scale Experiment**:
```python
# Multi-scale experiment
experiment_manager = NLMSExperimentManager()

# Create multi-scale experiment
experiment_id = experiment_manager.create_experiment(
    name="Cross-Scale Learning Comparison",
    description="Comparing learning across different brain sizes",
    scale=0,  # Not used for multi-scale
    config={}
)

# Test different scales
scales = [1000, 10000, 100000]

for scale in scales:
    print(f"\nRunning simulation at {scale} neurons")
    
    # Create scale-specific experiment
    scale_experiment_id = experiment_manager.create_experiment(
        name=f"Scale {scale} Learning Study",
        description=f"Learning analysis at {scale} neurons",
        scale=scale,
        config={
            "brain.neuron_count": scale,
            "plasticity.stdp.learning_rate": 0.001
        }
    )
    
    # Run simulations
    for run_num in range(2):
        experiment_manager.current_run = experiment_manager.start_simulation_run(
            experiment_id=scale_experiment_id,
            run_number=run_num + 1,
            steps=1000,
            config={
                "brain.neuron_count": scale,
                "plasticity.stdp.learning_rate": 0.001
            }
        )
        
        results = experiment_manager.run_simulation(
            config={
                "brain.neuron_count": scale,
                "plasticity.stdp.learning_rate": 0.001
            },
            steps=1000
        )
        
        print(f"  Run {run_num + 1}: {results['total_spikes']} spikes, {results['final_firing_percentage']:.1f}% firing")
```

### Reproducibility and Documentation

#### Automated Documentation

**Research Documentation Generator**:
```python
import pynlm
import json
import time
import platform
import sys

class ResearchDocumentation:
    """Generates comprehensive research documentation."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.documentation = {
            'experiment_info': {},
            'environment_info': {},
            'configuration': {},
            'results': {},
            'analysis': {}
        }
    
    def generate_experiment_info(self):
        """Generate experiment information."""
        self.documentation['experiment_info'] = {
            'name': self.experiment_name,
            'date': time.strftime('%Y-%m-%d %H:%M:%S'),
            'description': 'Neural Learning Machine research experiment',
            'researcher': 'NLM Research Team',
            'version': '1.0.0',
            'institutional_affiliation': 'NLM Research Institute'
        }
    
    def generate_environment_info(self):
        """Generate environment information."""
        import platform
        import sys
        
        self.documentation['environment_info'] = {
            'python_version': sys.version,
            'platform': platform.platform(),
            'architecture': platform.architecture(),
            'processor': platform.processor(),
            'python_implementation': platform.python_implementation(),
            'available_memory': self.get_available_memory(),
            'disk_space': self.get_disk_space()
        }
    
    def get_available_memory(self) -> str:
        """Get available system memory."""
        try:
            import psutil
            memory = psutil.virtual_memory()
            return f"{memory.available / (1024**3):.1f} GB"
        except ImportError:
            return "Unknown (psutil not installed)"
    
    def get_disk_space(self) -> str:
        """Get available disk space."""
        try:
            import psutil
            disk = psutil.disk_usage('/')
            return f"{disk.free / (1024**3):.1f} GB"
        except ImportError:
            return "Unknown"
    
    def generate_configuration_documentation(self, config):
        """Generate configuration documentation."""
        config_info = {}
        
        # Get all configuration parameters
        config_keys = [
            'brain.neuron_count',
            'brain.synapse_density',
            'plasticity.stdp.enable',
            'plasticity.stdp.learning_rate',
            'neuromod.dopamine.scale',
            'development.enable'
        ]
        
        for key in config_keys:
            if config.has(key):
                config_info[key] = config.get(key)
        
        self.documentation['configuration'] = config_info
    
    def generate_results_documentation(self, results: Dict):
        """Generate results documentation."""
        self.documentation['results'] = {
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'total_spikes': results.get('total_spikes', 0),
            'average_firing_percentage': results.get('final_firing_percentage', 0),
            'average_activity_level': results.get('final_activity_level', 0),
            'e_i_ratio': results.get('final_e_i_ratio', 0),
            'firing_rate': results.get('final_firing_rate', 0),
            'spikes_per_neuron': results.get('final_spikes_per_neuron', 0),
            'simulation_speed': results.get('steps_per_second', 0)
        }
    
    def save_documentation(self, filename: str = None):
        """Save documentation to file."""
        if filename is None:
            filename = f"{self.experiment_name}_documentation.json"
        
        with open(filename, 'w') as f:
            json.dump(self.documentation, f, indent=2, default=str)
        
        print(f"Documentation saved to {filename}")
    
    def generate_research_report(self, filename: str = None):
        """Generate human-readable research report."""
        if filename is None:
            filename = f"{self.experiment_name}_research_report.md"
        
        report = f"""# {self.experiment_name}

## Research Experiment Documentation

### Experiment Information
- **Name**: {self.documentation['experiment_info']['name']}
- **Date**: {self.documentation['experiment_info']['date']}
- **Description**: {self.documentation['experiment_info']['description']}
- **Researcher**: {self.documentation['experiment_info']['researcher']}

### Environment Configuration
- **Python Version**: {self.documentation['environment_info']['python_version'].split()[0]}
- **Platform**: {self.documentation['environment_info']['platform']}
- **Architecture**: {self.documentation['environment_info']['architecture'][0]}
- **Available Memory**: {self.documentation['environment_info']['available_memory']}
- **Disk Space**: {self.documentation['environment_info']['disk_space']}

### Simulation Configuration
"""
        
        config = self.documentation['configuration']
        for key, value in config.items():
            report += f"- {key}: {value}\n"
        
        report += f"""
### Results Summary
- **Total Spikes**: {self.documentation['results']['total_spikes']:,}
- **Average Firing Percentage**: {self.documentation['results']['average_firing_percentage']:.1f}%
- **Average Activity Level**: {self.documentation['results']['average_activity_level']:.1f}%
- **E/I Ratio**: {self.documentation['results']['e_i_ratio']:.3f}
- **Firing Rate**: {self.documentation['results']['firing_rate']:.2f} Hz
- **Spikes per Neuron**: {self.documentation['results']['spikes_per_neuron']:.2f}
- **Simulation Speed**: {self.documentation['results']['simulation_speed']:.1f} steps/second

### Conclusion
This experiment investigated neural learning dynamics using the NLM framework at the specified scale and configuration. The results provide insights into neural network behavior and learning capabilities.

---
Generated on {time.strftime('%Y-%m-%d %H:%M:%S')}
Report ID: {self.experiment_name.replace(' ', '_').lower()}
"""
        
        with open(filename, 'w') as f:
            f.write(report)
        
        print(f"Research report saved to {filename}")

# Usage example
experiment_name = "Neural Learning Capacity Study"
doc_generator = ResearchDocumentation(experiment_name)

doc_generator.generate_experiment_info()
doc_generator.generate_environment_info()
doc_generator.generate_configuration_documentation(config)
doc_generator.generate_results_documentation(results)
doc_generator.generate_research_report()
```

#### Reproducibility Checklist

**Script for Ensuring Reproducibility**:
```python
import pynlm
import json
import hashlib
import time
import os

class ReproducibilityManager:
    """Ensures reproducibility of research experiments."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.metadata = {
            'experiment_name': experiment_name,
            'creation_time': time.time(),
            'dependencies': {},
            'parameters': {},
            'environment': {},
            'checksums': {}
        }
    
    def record_dependencies(self):
        """Record software dependencies."""
        try:
            import pkg_resources
            self.metadata['dependencies'] = {
                'python_packages': {
                    'pynlm': pkg_resources.get_distribution('pynlm').version,
                    'numpy': pkg_resources.get_distribution('numpy').version,
                    'matplotlib': pkg_resources.get_distribution('matplotlib').version
                }
            }
        except ImportError:
            self.metadata['dependencies'] = {
                'python_packages': 'Unable to determine versions'
            }
    
    def record_parameters(self, parameters: Dict):
        """Record experiment parameters."""
        self.metadata['parameters'] = parameters
        
        # Generate checksum
        param_string = json.dumps(parameters, sort_keys=True)
        self.metadata['checksums']['parameters'] = hashlib.md5(param_string.encode()).hexdigest()
    
    def record_environment(self):
        """Record environment information."""
        import platform
        import psutil
        
        self.metadata['environment'] = {
            'platform': platform.platform(),
            'python_version': platform.python_version(),
            'cpu_count': psutil.cpu_count(),
            'memory_gb': psutil.virtual_memory().total / (1024**3),
            'disk_gb': psutil.disk_usage('/').total / (1024**3)
        }
    
    def verify_reproducibility(self, saved_parameters: Dict) -> bool:
        """Verify that current setup matches saved parameters."""
        print("Reproducibility Verification Report")
        print("=" * 50)
        
        # Check parameters
        current_checksum = hashlib.md5(
            json.dumps(self.metadata['parameters'], sort_keys=True).encode()
        ).hexdigest()
        
        saved_checksum = self.metadata['checksums']['parameters']
        
        print(f"\nParameter Checksum:")
        print(f"  Current: {current_checksum}")
        print(f"  Saved:   {saved_checksum}")
        print(f"  Match:   {'✓' if current_checksum == saved_checksum else '✗'}")
        
        # Check dependencies
        print(f"\nSoftware Dependencies:")
        if 'dependencies' in self.metadata:
            print("  ✓ Dependencies recorded")
        else:
            print("  ✗ Dependencies not recorded")
        
        # Check environment
        print(f"\nEnvironment:")
        if 'environment' in self.metadata:
            print(f"  ✓ Environment recorded")
            print(f"    Platform: {self.metadata['environment']['platform']}")
            print(f"    Python: {self.metadata['environment']['python_version']}")
        else:
            print("  ✗ Environment not recorded")
        
        # Overall verification
        parameter_match = current_checksum == saved_checksum
        deps_recorded = 'dependencies' in self.metadata
        env_recorded = 'environment' in self.metadata
        
        overall_match = parameter_match and deps_recorded and env_recorded
        
        print(f"\n{'=' * 50}")
        print(f"Reproducibility Status: {'✓ PASS' if overall_match else '✗ FAIL'}")
        
        return overall_match
    
    def save_reproducibility_metadata(self, filename: str = None):
        """Save reproducibility metadata."""
        if filename is None:
            filename = f"{self.experiment_name}_reproducibility.json"
        
        with open(filename, 'w') as f:
            json.dump(self.metadata, f, indent=2, default=str)
        
        print(f"Reproducibility metadata saved to {filename}")

# Usage example
experiment_name = "Reproducibility Test Experiment"
repro_manager = ReproducibilityManager(experiment_name)

# Record setup information
repro_manager.record_dependencies()
repro_manager.record_parameters({
    "brain.neuron_count": 10000,
    "brain.synapse_density": 0.12,
    "plasticity.stdp.learning_rate": 0.001
})
repro_manager.record_environment()

# Save metadata
repro_manager.save_reproducibility_metadata()

# Later verification (in new session)
verification_manager = ReproducibilityManager(experiment_name)
verification_manager.load_reproducibility_metadata("experiment_reproducibility.json")

# Verify reproducibility
is_reproducible = verification_manager.verify_reproducibility(
    {"brain.neuron_count": 10000, "brain.synapse_density": 0.12}
)
```

## Phase 4: Data Analysis

### Analysis Framework

**Comprehensive Analysis Pipeline**:
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import json
from typing import Dict, List, Any

class NLMSDataAnalysis:
    """Comprehensive data analysis framework for NLM research."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.analysis_results = {}
        self.visualizations = {}
        self.statistical_tests = {}
    
    def load_experiment_data(self, db_path: str = "nlm_research.db"):
        """Load experiment data from database."""
        import sqlite3
        
        conn = sqlite3.connect(db_path)
        
        # Load time series data
        time_series_query = """
            SELECT run_id, step, timestamp, firing_percentage, 
                   activity_level, e_i_ratio
            FROM time_series_data
            ORDER BY run_id, step
        """
        
        self.time_series_data = pd.read_sql(time_series_query, conn)
        
        # Load metrics
        metrics_query = """
            SELECT run_id, metric_name, metric_value
            FROM metrics
        """
        
        self.metrics_data = pd.read_sql(metrics_query, conn)
        
        # Load experiment info
        experiments_query = "SELECT * FROM experiments"
        self.experiments_data = pd.read_sql(experiments_query, conn)
        
        conn.close()
        
        print(f"Loaded {len(self.time_series_data)} time series data points")
        print(f"Loaded {len(self.metrics_data)} metric measurements")
        print(f"Loaded {len(self.experiments_data)} experiment records")
    
    def basic_statistical_analysis(self, data: pd.DataFrame) -> Dict[str, Any]:
        """Perform basic statistical analysis on data."""
        
        results = {}
        
        # Basic statistics for each numeric column
        numeric_columns = data.select_dtypes(include=[np.number]).columns
        
        for column in numeric_columns:
            column_data = data[column].dropna()
            
            if len(column_data) > 0:
                stats = {
                    'mean': np.mean(column_data),
                    'median': np.median(column_data),
                    'std': np.std(column_data),
                    'min': np.min(column_data),
                    'max': np.max(column_data),
                    'q25': np.percentile(column_data, 25),
                    'q75': np.percentile(column_data, 75),
                    'iqr': np.percentile(column_data, 75) - np.percentile(column_data, 25),
                    'skewness': stats.skew(column_data),
                    'kurtosis': stats.kurtosis(column_data),
                    'sample_size': len(column_data)
                }
                
                results[column] = stats
        
        return results
    
    def comparative_analysis(self, groups: Dict[str, pd.DataFrame]) -> Dict[str, Any]:
        """Perform comparative analysis between different groups."""
        
        results = {}
        
        for group_name, group_data in groups.items():
            results[f"{group_name}_statistics"] = self.basic_statistical_analysis(group_data)
        
        # Perform statistical tests between groups
        group_names = list(groups.keys())
        
        if len(group_names) >= 2:
            # One-way ANOVA for continuous variables
            for column in self.time_series_data.select_dtypes(include=[np.number]).columns:
                group_data = []
                
                for group_name in group_names:
                    group_subset = groups[group_name][column].dropna()
                    if len(group_subset) > 0:
                        group_data.append(group_subset.values)
                
                if len(group_data) >= 2:
                    try:
                        f_stat, p_value = stats.f_oneway(*group_data)
                        
                        results[f"anova_{column}"] = {
                            'f_statistic': f_stat,
                            'p_value': p_value,
                            'significance': p_value < 0.05,
                            'groups_tested': group_names
                        }
                    except Exception as e:
                        results[f"anova_{column}"] = {
                            'error': str(e),
                            'groups_tested': group_names
                        }
        
        return results
    
    def time_series_analysis(self, data: pd.DataFrame, group_column: str = None) -> Dict[str, Any]:
        """Perform time series analysis."""
        
        results = {}
        
        # Resample time series data
        if group_column:
            # Analysis by group
            for group_name, group_data in data.groupby(group_column):
                group_results = self._analyze_single_time_series(group_data)
                results[f"{group_name}_analysis"] = group_results
        else:
            # Analysis of all data combined
            results['combined_analysis'] = self._analyze_single_time_series(data)
        
        return results
    
    def _analyze_single_time_series(self, data: pd.DataFrame) -> Dict[str, Any]:
        """Analyze a single time series."""
        
        results = {}
        
        # Basic time series statistics
        numeric_columns = data.select_dtypes(include=[np.number]).columns
        
        for column in numeric_columns:
            column_data = data[column].dropna()
            
            if len(column_data) > 10:
                # Trend analysis
                x = np.arange(len(column_data))
                slope, intercept = np.polyfit(x, column_data, 1)
                trend_line = slope * x + intercept
                
                # Residuals
                residuals = column_data - trend_line
                
                # Autocorrelation
                try:
                    acf = np.correlate(column_data, column_data, mode='full')
                    acf = acf[len(acf)//2:]
                    acf = acf / acf[0]
                except:
                    acf = None
                
                # Stationarity test (Augmented Dickey-Fuller)
                try:
                    from statsmodels.tsa.stattools import adfuller
                    adf_result = adfuller(column_data)
                    adf_statistic, adf_p_value = adf_result[0], adf_result[1]
                    is_stationary = adf_p_value < 0.05
                except:
                    adf_statistic, adf_p_value = None, None
                    is_stationary = None
                
                results[column] = {
                    'mean': np.mean(column_data),
                    'std': np.std(column_data),
                    'trend_slope': slope,
                    'trend_intercept': intercept,
                    'autocorrelation': acf[1] if acf is not None else None,  # lag-1 autocorrelation
                    'adf_statistic': adf_statistic,
                    'adf_p_value': adf_p_value,
                    'is_stationary': is_stationary,
                    'number_of_observations': len(column_data),
                    'time_span': len(column_data)
                }
        
        return results
    
    def generate_visualizations(self, data: pd.DataFrame, output_dir: str = "plots") -> Dict[str, str]:
        """Generate comprehensive visualizations."""
        
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        visualizations = {}
        
        # Set style
        plt.style.use('seaborn-v0_8')
        
        # 1. Time series plots
        time_series_plots = self._create_time_series_plots(data)
        visualizations.update(time_series_plots)
        
        # 2. Distribution plots
        distribution_plots = self._create_distribution_plots(data)
        visualizations.update(distribution_plots)
        
        # 3. Comparative plots
        comparative_plots = self._create_comparative_plots(data)
        visualizations.update(comparative_plots)
        
        # 4. Heatmaps
        heatmap_plots = self._create_heatmap_plots(data)
        visualizations.update(heatmap_plots)
        
        return visualizations
    
    def _create_time_series_plots(self, data: pd.DataFrame) -> Dict[str, str]:
        """Create time series visualizations."""
        
        plots = {}
        
        # Separate by group if available
        if 'run_id' in data.columns:
            groups = data.groupby('run_id')
            for group_name, group_data in groups:
                fig, axes = plt.subplots(2, 2, figsize=(15, 10))
                fig.suptitle(f"Time Series Analysis - Run {group_name}", fontsize=16)
                
                # Firing percentage over time
                axes[0, 0].plot(group_data['step'], group_data['firing_percentage'])
                axes[0, 0].set_title('Firing Percentage vs Time')
                axes[0, 0].set_xlabel('Step')
                axes[0, 0].set_ylabel('Firing Percentage (%)')
                axes[0, 0].grid(True, alpha=0.3)
                
                # Activity level over time
                axes[0, 1].plot(group_data['step'], group_data['activity_level'])
                axes[0, 1].set_title('Activity Level vs Time')
                axes[0, 1].set_xlabel('Step')
                axes[0, 1].set_ylabel('Activity Level (%)')
                axes[0, 1].grid(True, alpha=0.3)
                
                # E/I ratio over time
                axes[1, 0].plot(group_data['step'], group_data['e_i_ratio'])
                axes[1, 0].set_title('E/I Ratio vs Time')
                axes[1, 0].set_xlabel('Step')
                axes[1, 0].set_ylabel('E/I Ratio')
                axes[1, 0].grid(True, alpha=0.3)
                
                # All metrics together
                axes[1, 1].plot(group_data['step'], group_data['firing_percentage'], 
                               label='Firing %', alpha=0.7)
                axes[1, 1].plot(group_data['step'], group_data['activity_level'], 
                               label='Activity %', alpha=0.7)
                axes[1, 1].set_title('Multiple Metrics vs Time')
                axes[1, 1].set_xlabel('Step')
                axes[1, 1].set_ylabel('Value')
                axes[1, 1].legend()
                axes[1, 1].grid(True, alpha=0.3)
                
                plt.tight_layout()
                plot_path = os.path.join(output_dir, f"time_series_run_{group_name}.png")
                plt.savefig(plot_path, dpi=300, bbox_inches='tight')
                plt.close()
                
                plots[f"time_series_run_{group_name}"] = plot_path
        
        else:
            # Single time series
            fig, axes = plt.subplots(2, 2, figsize=(15, 10))
            fig.suptitle("Time Series Analysis", fontsize=16)
            
            # Firing percentage over time
            axes[0, 0].plot(data['step'], data['firing_percentage'])
            axes[0, 0].set_title('Firing Percentage vs Time')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Firing Percentage (%)')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Activity level over time
            axes[0, 1].plot(data['step'], data['activity_level'])
            axes[0, 1].set_title('Activity Level vs Time')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Activity Level (%)')
            axes[0, 1].grid(True, alpha=0.3)
            
            # E/I ratio over time
            axes[1, 0].plot(data['step'], data['e_i_ratio'])
            axes[1, 0].set_title('E/I Ratio vs Time')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('E/I Ratio')
            axes[1, 0].grid(True, alpha=0.3)
            
            # All metrics together
            axes[1, 1].plot(data['step'], data['firing_percentage'], 
                           label='Firing %', alpha=0.7)
            axes[1, 1].plot(data['step'], data['activity_level'], 
                           label='Activity %', alpha=0.7)
            axes[1, 1].set_title('Multiple Metrics vs Time')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Value')
            axes[1, 1].legend()
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            plot_path = os.path.join(output_dir, "time_series_combined.png")
            plt.savefig(plot_path, dpi=300, bbox_inches='tight')
            plt.close()
            
            plots["time_series_combined"] = plot_path
        
        return plots
    
    def _create_distribution_plots(self, data: pd.DataFrame) -> Dict[str, str]:
        """Create distribution visualizations."""
        
        plots = {}
        numeric_columns = data.select_dtypes(include=[np.number]).columns
        
        for column in numeric_columns:
            fig, axes = plt.subplots(1, 3, figsize=(18, 5))
            fig.suptitle(f"Distribution Analysis: {column}", fontsize=16)
            
            # Histogram with KDE
            axes[0].hist(data[column].dropna(), bins=50, density=True, alpha=0.7)
            axes[0].set_title('Histogram with KDE')
            axes[0].set_xlabel(column)
            axes[0].set_ylabel('Density')
            
            # Box plot
            axes[1].boxplot(data[column].dropna())
            axes[1].set_title('Box Plot')
            axes[1].set_ylabel(column)
            
            # QQ plot
            try:
                from scipy import stats
                stats.probplot(data[column].dropna(), dist="norm", ax=axes[2])
                axes[2].set_title('Q-Q Plot (Normal)')
            except:
                axes[2].text(0.5, 0.5, 'QQ plot not available', 
                           horizontalalignment='center',
                           verticalalignment='center',
                           transform=axes[2].transAxes)
                axes[2].set_title('Q-Q Plot (Normal)')
            
            plt.tight_layout()
            plot_path = os.path.join(output_dir, f"distribution_{column}.png")
            plt.savefig(plot_path, dpi=300, bbox_inches='tight')
            plt.close()
            
            plots[f"distribution_{column}"] = plot_path
        
        return plots
    
    def _create_comparative_plots(self, data: pd.DataFrame) -> Dict[str, str]:
        """Create comparative visualizations."""
        
        plots = {}
        
        if 'run_id' in data.columns:
            # Group by run_id for comparison
            groups = data.groupby('run_id')
            
            fig, axes = plt.subplots(2, 2, figsize=(16, 12))
            fig.suptitle('Comparative Analysis Across Runs', fontsize=16)
            
            # Plot 1: Mean metrics by run
            metrics_summary = data.groupby('run_id').agg({
                'firing_percentage': 'mean',
                'activity_level': 'mean',
                'e_i_ratio': 'mean'
            }).reset_index()
            
            axes[0, 0].bar(metrics_summary['run_id'], metrics_summary['firing_percentage'],
                          alpha=0.7, label='Firing %')
            axes[0, 0].bar(metrics_summary['run_id'], metrics_summary['activity_level'],
                          alpha=0.7, label='Activity %')
            axes[0, 0].set_title('Mean Metrics by Run')
            axes[0, 0].set_xlabel('Run ID')
            axes[0, 0].set_ylabel('Value (%)')
            axes[0, 0].legend()
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Metric correlations
            metrics_corr = data[['firing_percentage', 'activity_level', 'e_i_ratio']].corr()
            sns.heatmap(metrics_corr, annot=True, cmap='coolwarm', center=0,
                       ax=axes[0, 1])
            axes[0, 1].set_title('Metric Correlations')
            
            # Plot 3: Run comparison box plots
            melted_data = data.melt(id_vars=['run_id'], 
                                  value_vars=['firing_percentage', 'activity_level', 'e_i_ratio'],
                                  var_name='metric', value_name='value')
            
            sns.boxplot(data=melted_data, x='run_id', y='value', hue='metric',
                       ax=axes[1, 0])
            axes[1, 0].set_title('Metric Distributions by Run')
            axes[1, 0].set_xlabel('Run ID')
            axes[1, 0].set_ylabel('Value')
            axes[1, 0].grid(True, alpha=0.3)
            
            # Plot 4: Time series comparison (first 20% of data)
            max_steps = data['step'].max()
            early_data = data[data['step'] <= max_steps * 0.2]
            
            for run_id, run_data in early_data.groupby('run_id'):
                axes[1, 1].plot(run_data['step'], run_data['firing_percentage'],
                               label=f'Run {run_id}', linewidth=2)
            
            axes[1, 1].set_title('Early Time Series Comparison')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Firing Percentage (%)')
            axes[1, 1].legend()
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            plot_path = os.path.join(output_dir, "comparative_analysis.png")
            plt.savefig(plot_path, dpi=300, bbox_inches='tight')
            plt.close()
            
            plots["comparative_analysis"] = plot_path
        
        return plots
    
    def _create_heatmap_plots(self, data: pd.DataFrame) -> Dict[str, str]:
        """Create heatmap visualizations."""
        
        plots = {}
        
        # Correlation heatmap
        if 'run_id' in data.columns:
            # Analyze correlations within each run
            for run_id, run_data in data.groupby('run_id'):
                numeric_data = run_data.select_dtypes(include=[np.number])
                if len(numeric_data.columns) >= 2:
                    corr_matrix = numeric_data.corr()
                    
                    fig, ax = plt.subplots(figsize=(10, 8))
                    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0,
                               square=True, ax=ax)
                    ax.set_title(f'Correlation Heatmap - Run {run_id}')
                    
                    plt.tight_layout()
                    plot_path = os.path.join(output_dir, f"heatmap_run_{run_id}.png")
                    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
                    plt.close()
                    
                    plots[f"heatmap_run_{run_id}"] = plot_path
        
        return plots
    
    def save_analysis_results(self, results: Dict, output_dir: str = "analysis_results") -> str:
        """Save analysis results to file."""
        
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        # Save results as JSON
        results_path = os.path.join(output_dir, f"{self.experiment_name}_analysis_results.json")
        
        with open(results_path, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Analysis results saved to {results_path}")
        
        return results_path

# Usage example
experiment_name = "Neural Learning Analysis"

# Initialize analysis tools
analysis = NLMSDataAnalysis(experiment_name)

# Load experiment data
analysis.load_experiment_data()

# Perform basic statistical analysis
basic_results = analysis.basic_statistical_analysis(analysis.time_series_data)
analysis.analysis_results['basic_statistics'] = basic_results

# Perform comparative analysis (by run_id)
comparative_results = analysis.comparative_analysis(
    {run_id: group for run_id, group in analysis.time_series_data.groupby('run_id')}
)
analysis.analysis_results['comparative_statistics'] = comparative_results

# Perform time series analysis
time_series_results = analysis.time_series_analysis(analysis.time_series_data, 'run_id')
analysis.analysis_results['time_series_analysis'] = time_series_results

# Generate visualizations
print("Generating visualizations...")
visualization_files = analysis.generate_visualizations(analysis.time_series_data)
analysis.visualizations.update(visualization_files)

# Save analysis results
results_path = analysis.save_analysis_results(analysis.analysis_results)

print(f"Analysis complete. Results saved to: {results_path}")
```

#### Data Analysis Examples

**Example 1: Scale Comparison**:
```python
import pynlm
import pandas as pd
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt
import seaborn as sns

# Load data from different scales
experimental_data = {
    'micro_scale': pd.DataFrame({
        'scale': 'Micro (1K neurons)',
        'firing_percentage': 45.2,
        'activity_level': 38.1,
        'spike_count': 12500,
        'learning_speed': 0.85
    }),
    'small_scale': pd.DataFrame({
        'scale': 'Small (10K neurons)',
        'firing_percentage': 52.7,
        'activity_level': 44.3,
        'spike_count': 125000,
        'learning_speed': 0.72
    }),
    'medium_scale': pd.DataFrame({
        'scale': 'Medium (100K neurons)',
        'firing_percentage': 48.9,
        'activity_level': 41.2,
        'spike_count': 1250000,
        'learning_speed': 0.68
    })
}

# Combine data
all_data = pd.concat(experimental_data.values(), ignore_index=True)

# Statistical analysis
print("Statistical Analysis Results:")
print("=" * 50)

# Descriptive statistics
print("\nDescriptive Statistics:")
print(all_data[['firing_percentage', 'activity_level', 'spike_count']].describe())

# Scale comparison test
print("\nScale Comparison Analysis:")

# One-way ANOVA for firing percentage
f_stat, p_value = stats.f_oneway(
    experimental_data['micro_scale']['firing_percentage'],
    experimental_data['small_scale']['firing_percentage'],
    experimental_data['medium_scale']['firing_percentage']
)

print(f"Firing Percentage ANOVA:")
print(f"  F-statistic: {f_stat:.4f}")
print(f"  P-value: {p_value:.4f}")
print(f"  Significant difference: {'Yes' if p_value < 0.05 else 'No'}")

# Post-hoc Tukey test
from scipy.stats import ttest_ind

# Compare micro vs small
micro_small_t, micro_small_p = ttest_ind(
    experimental_data['micro_scale']['firing_percentage'],
    experimental_data['small_scale']['firing_percentage']
)

# Compare small vs medium
small_medium_t, small_medium_p = ttest_ind(
    experimental_data['small_scale']['firing_percentage'],
    experimental_data['medium_scale']['firing_percentage']
)

# Compare micro vs medium
micro_medium_t, micro_medium_p = ttest_ind(
    experimental_data['micro_scale']['firing_percentage'],
    experimental_data['medium_scale']['firing_percentage']
)

print(f"\nPairwise Comparisons (Tukey HSD equivalent):")
print(f"  Micro vs Small: t={micro_small_t:.4f}, p={micro_small_p:.4f}")
print(f"  Small vs Medium: t={small_medium_t:.4f}, p={small_medium_p:.4f}")
print(f"  Micro vs Medium: t={micro_medium_t:.4f}, p={micro_medium_p:.4f}")

# Learning speed analysis
print(f"\nLearning Speed Analysis:")
print(f"Micro scale: {experimental_data['micro_scale']['learning_speed']:.2f}")
print(f"Small scale: {experimental_data['small_scale']['learning_speed']:.2f}")
print(f"Medium scale: {experimental_data['medium_scale']['learning_speed']:.2f}")

# Linear regression for learning speed vs scale
from sklearn.linear_model import LinearRegression

X = np.array([1, 2, 3]).reshape(-1, 1)  # Scales: Micro=1, Small=2, Medium=3
y = [experimental_data['micro_scale']['learning_speed'],
     experimental_data['small_scale']['learning_speed'],
     experimental_data['medium_scale']['learning_speed']]

model = LinearRegression()
model.fit(X, y)

print(f"\nLinear Regression (Learning Speed vs Scale):")
print(f"  Slope: {model.coef_[0]:.4f}")
print(f"  Intercept: {model.intercept_:.4f}")
print(f"  R² score: {model.score(X, y):.4f}")

# Visualization
fig, axes = plt.subplots(2, 2, figsize=(16, 12))

# 1. Bar plot of firing percentage by scale
axes[0, 0].bar(all_data['scale'], all_data['firing_percentage'], alpha=0.7)
axes[0, 0].set_title('Firing Percentage by Scale')
axes[0, 0].set_ylabel('Firing Percentage (%)')
axes[0, 0].tick_params(axis='x', rotation=45)

# 2. Bar plot of spike count by scale
axes[0, 1].bar(all_data['scale'], all_data['spike_count'], alpha=0.7)
axes[0, 1].set_title('Total Spikes by Scale')
axes[0, 1].set_ylabel('Spike Count')
axes[0, 1].tick_params(axis='x', rotation=45)

# 3. Scatter plot: learning speed vs firing percentage
scatter_data = all_data[['firing_percentage', 'learning_speed']].values
axes[1, 0].scatter(scatter_data[:, 0], scatter_data[:, 1], s=100, alpha=0.7)
axes[1, 0].set_title('Learning Speed vs Firing Percentage')
axes[1, 0].set_xlabel('Firing Percentage (%)')
axes[1, 0].set_ylabel('Learning Speed')
axes[1, 0].grid(True, alpha=0.3)

# 4. Correlation heatmap
corr_matrix = all_data[['firing_percentage', 'activity_level', 'spike_count', 'learning_speed']].corr()
sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, ax=axes[1, 1])
axes[1, 1].set_title('Correlation Matrix')

plt.tight_layout()
plt.savefig('scale_comparison_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"\nAnalysis complete. Visualization saved as 'scale_comparison_analysis.png'")
```

**Example 2: Neuromodulation Impact Analysis**:
```python
import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import matplotlib.animation as animation

# Simulate neuromodulation experiment data
experiment_data = []

# Test different neuromodulation conditions
conditions = [
    {'dopamine_scale': 0.5, 'curiosity': False, 'novelty': False},
    {'dopamine_scale': 1.0, 'curiosity': True, 'novelty': False},
    {'dopamine_scale': 1.5, 'curiosity': True, 'novelty': True},
    {'dopamine_scale': 2.0, 'curiosity': False, 'novelty': True}
]

for i, condition in enumerate(conditions):
    # Simulate multiple runs for each condition
    for run in range(5):
        # Simulate exploration-exploitation dynamics
        exploration_rate = np.random.beta(2, 2)  # Beta distribution
        exploitation_efficiency = np.random.beta(3, 1)  # Higher exploitation
        
        # Add condition effects
        if condition['curiosity']:
            exploration_rate *= 1.5  # Curiosity increases exploration
        
        if condition['novelty']:
            exploration_rate *= 1.2  # Novelty detection increases exploration
        
        exploration_rate = min(exploration_rate, 1.0)  # Cap at 100%
        
        experiment_data.append({
            'condition': i + 1,
            'dopamine_scale': condition['dopamine_scale'],
            'curiosity_enabled': condition['curiosity'],
            'novelty_enabled': condition['novelty'],
            'exploration_rate': exploration_rate,
            'exploitation_efficiency': exploitation_efficiency,
            'run': run + 1
        })

# Convert to DataFrame
df = pd.DataFrame(experiment_data)

# Analysis
print("Neuromodulation Impact Analysis")
print("=" * 50)

# 1. Descriptive statistics by condition
print("\nDescriptive Statistics by Neuromodulation Condition:")
print(df.groupby('condition').agg({
    'exploration_rate': ['mean', 'std', 'min', 'max'],
    'exploitation_efficiency': ['mean', 'std', 'min', 'max']
}).round(3))

# 2. One-way ANOVA for exploration rate
print("\nExploration Rate ANOVA:")
conditions = [df[df['condition'] == c]['exploration_rate'].values 
              for c in range(1, 5)]
f_stat, p_value = stats.f_oneway(*conditions)
print(f"  F-statistic: {f_stat:.4f}")
print(f"  P-value: {p_value:.4f}")
print(f"  Significant difference: {'Yes' if p_value < 0.05 else 'No'}")

# 3. Post-hoc pairwise comparisons
print("\nPairwise Comparisons (Tukey HSD):")
from itertools import combinations

for cond1, cond2 in combinations(range(1, 5), 2):
    data1 = df[df['condition'] == cond1]['exploration_rate']
    data2 = df[df['condition'] == cond2]['exploration_rate']
    
    t_stat, p_val = stats.ttest_ind(data1, data2)
    print(f"  Condition {cond1} vs {cond2}: t={t_stat:.4f}, p={p_val:.4f}")

# 4. Visualization
fig, axes = plt.subplots(2, 2, figsize=(16, 12))

# 1. Bar plot: exploration rate by condition
axes[0, 0].bar(df['condition'], df['exploration_rate'], alpha=0.7)
axes[0, 0].set_title('Exploration Rate by Neuromodulation Condition')
axes[0, 0].set_xlabel('Condition')
axes[0, 0].set_ylabel('Exploration Rate')
axes[0, 0].set_xticks(range(1, 5))

# 2. Bar plot: exploitation efficiency by condition
axes[0, 1].bar(df['condition'], df['exploitation_efficiency'], alpha=0.7, color='orange')
axes[0, 1].set_title('Exploitation Efficiency by Neuromodulation Condition')
axes[0, 1].set_xlabel('Condition')
axes[0, 1].set_ylabel('Exploitation Efficiency')
axes[0, 1].set_xticks(range(1, 5))

# 3. Scatter plot: exploration vs exploitation
colors = ['red' if c == False else 'blue' for c in df['curiosity_enabled']]
axes[1, 0].scatter(df['exploration_rate'], df['exploitation_efficiency'],
                   c=colors, alpha=0.7, s=100)
axes[1, 0].set_title('Exploration vs Exploitation by Neuromodulation')
axes[1, 0].set_xlabel('Exploration Rate')
axes[1, 0].set_ylabel('Exploitation Efficiency')
axes[1, 0].legend(['No Curiosity', 'With Curiosity'], loc='upper right')
axes[1, 0].grid(True, alpha=0.3)

# 4. Interaction plot
fig2, ax2 = plt.subplots(figsize=(12, 8))
for condition in df['condition'].unique():
    subset = df[df['condition'] == condition]
    ax2.plot(subset['dopamine_scale'], subset['exploration_rate'],
             marker='o', linewidth=2, label=f'Condition {condition}')

ax2.set_title('Exploration Rate vs Dopamine Scale')
ax2.set_xlabel('Dopamine Scale')
ax2.set_ylabel('Exploration Rate')
ax2.legend()
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('neuromodulation_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"\nAnalysis complete. Visualization saved as 'neuromodulation_analysis.png'")
```

## Phase 5: Documentation and Reporting

### Research Documentation Standards

#### Essential Documentation Elements

**1. Methodology Documentation**:
```
Research Methodology Report
=============================

Experiment Details:
- Experiment Name: [Name]
- Research Question: [Question]
- Hypothesis: [Prediction]
- Scale: [Neuron count]
- Duration: [Steps]
- Date: [Date]

Equipment and Software:
- NLM Version: [Version]
- Python Version: [Version]
- Dependencies: [List]
- Analysis Tools: [List]

Experimental Protocol:
1. [Step 1]
2. [Step 2]
3. [Step 3]

Configuration Details:
brain.neuron_count: [Value]
brain.synapse_density: [Value]
plasticity.stdp.learning_rate: [Value]
[Additional parameters]
```

**2. Results Documentation**:
```
Results Analysis Report
========================

Overview:
- Total Runs: [Number]
- Total Steps: [Number]
- Average Firing Percentage: [%]
- Average Activity Level: [%]
- Time per Simulation: [Time]

Key Findings:
1. [Finding 1]
2. [Finding 2]
3. [Finding 3]

Statistical Significance:
- Primary metric p-values: [Values]
- Effect sizes: [Values]
- Confidence intervals: [Values]

Limitations:
- [Limitation 1]
- [Limitation 2]
- [Limitation 3]
```

**3. Visualization Documentation**:
```
Figure Reference Guide
=======================

Figure 1: Time Series of Neural Activity
- Description: [Description]
- Data: [Data source]
- Parameters: [Parameters used]

Figure 2: Scale Comparison
- Description: [Description]
- Analysis: [Statistical test]
- Significance: [P-value]

Figure 3: Neuromodulation Effects
- Description: [Description]
- Variables: [Variables plotted]
- Findings: [Key insights]
```

### Automated Report Generation

**Comprehensive Report Generator**:
```python
import pynlm
import json
import time
import pandas as pd
import numpy as np
from datetime import datetime
import matplotlib.pyplot as plt
import os

class ResearchReportGenerator:
    """Generates comprehensive research reports for NLM experiments."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.report_data = {
            'metadata': {},
            'methodology': {},
            'results': {},
            'analysis': {},
            'conclusions': {},
            'visualizations': []
        }
    
    def set_metadata(self, metadata: Dict):
        """Set experiment metadata."""
        self.report_data['metadata'] = metadata
    
    def set_methodology(self, methodology: Dict):
        """Set methodology information."""
        self.report_data['methodology'] = methodology
    
    def set_results(self, results: Dict):
        """Set experimental results."""
        self.report_data['results'] = results
    
    def set_analysis(self, analysis: Dict):
        """Set analysis results."""
        self.report_data['analysis'] = analysis
    
    def set_conclusions(self, conclusions: Dict):
        """Set conclusions and findings."""
        self.report_data['conclusions'] = conclusions
    
    def add_visualization(self, description: str, file_path: str):
        """Add visualization to report."""
        self.report_data['visualizations'].append({
            'description': description,
            'file_path': file_path
        })
    
    def generate_markdown_report(self) -> str:
        """Generate Markdown formatted report."""
        
        report = f"""# {self.experiment_name}

## Research Experiment Report

**Generated on:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Report ID:** {self.experiment_name.replace(' ', '_').lower()}_{int(time.time())}

### Abstract
This research report documents the findings of an experimental investigation into neural learning dynamics using the NLM (Neural Learning Machine) framework. The study examined [brief description of research question/hypothesis].

### 1. Introduction

#### Background
{self.generate_background_section()}

#### Research Objectives
1. To investigate [objective 1]
2. To analyze [objective 2]
3. To evaluate [objective 3]

#### Literature Review
Key findings from previous research indicate [summary of relevant literature].

### 2. Materials and Methods

#### Experimental Setup

**Hardware Configuration:**
- CPU: [specification]
- Memory: [specification]
- Storage: [specification]

**Software Environment:**
- NLM Version: {self.report_data['metadata'].get('nlm_version', 'Unknown')}
- Python Version: {self.report_data['metadata'].get('python_version', 'Unknown')}
- Dependencies: {', '.join(self.report_data['metadata'].get('dependencies', []))}

**Simulation Parameters:**
- Scale: {self.report_data['methodology'].get('scale', 'Not specified')}
- Configuration: {json.dumps(self.report_data['methodology'].get('config', {}), indent=2)}
- Duration: {self.report_data['methodology'].get('steps', 'Not specified')} steps

#### Data Collection

**Time Series Data:**
- Total data points: {self.report_data['results'].get('total_data_points', 0)}
- Sampling rate: {self.report_data['results'].get('sampling_rate', 'Not specified')}
- Data collection method: {self.report_data['results'].get('data_collection_method', 'Not specified')}

**Analysis Procedures:**
{self.generate_analysis_section()}

### 3. Results

#### Descriptive Statistics

{self.generate_descriptive_statistics_section()}

#### Inferential Statistics

{self.generate_inferential_statistics_section()}

#### Time Series Analysis

{self.generate_time_series_analysis_section()}

### 4. Discussion

#### Main Findings
1. **Finding 1:** {self.report_data['conclusions'].get('finding1', 'Not specified')}
2. **Finding 2:** {self.report_data['conclusions'].get('finding2', 'Not specified')}
3. **Finding 3:** {self.report_data['conclusions'].get('finding3', 'Not specified')}

#### Interpretation
The results demonstrate [key interpretation].

#### Limitations
- [Limitation 1]
- [Limitation 2]
- [Limitation 3]

#### Future Directions
- [Future direction 1]
- [Future direction 2]
- [Future direction 3]

### 5. Conclusions

**Summary of Findings:**
The experimental investigation revealed significant insights into [research area].

**Practical Implications:**
[Implications for practical applications]

**Theoretical Contributions:**
[Contributions to theoretical understanding]

**Recommendations for Future Research:**
[Recommendations for continued investigation]

### 6. Acknowledgments

**Funding:** [Funding source]
**Contributors:** [Contributors]
**Institutional Support:** [Institutional affiliation]

### 7. References

{self.generate_references_section()}

---

*This report was generated automatically using the NLM Research Framework.*
Report generation time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
"""
        
        return report
    
    def save_markdown_report(self, filename: str = None) -> str:
        """Save Markdown report to file."""
        if filename is None:
            filename = f"{self.experiment_name}_research_report.md"
        
        report = self.generate_markdown_report()
        
        with open(filename, 'w') as f:
            f.write(report)
        
        print(f"Markdown report saved to {filename}")
        return filename
    
    def save_pdf_report(self, filename: str = None) -> str:
        """Save PDF report to file."""
        if filename is None:
            filename = f"{self.experiment_name}_research_report.pdf"
        
        try:
            import weasyprint
            from weasyprint.text_properties import NORMAL_FONT_WEIGHT
            
            # Create HTML from Markdown
            html_content = self._markdown_to_html(report)
            
            # Save PDF
            weasyprint.HTML(string=html_content).write_pdf(filename)
            
            print(f"PDF report saved to {filename}")
            return filename
            
        except ImportError:
            print("weasyprint not available. Please install for PDF generation.")
            return None
    
    def generate_background_section(self) -> str:
        """Generate background information section."""
        return """
The NLM framework provides a biologically inspired platform for studying neural computation and learning. This investigation contributes to the understanding of how neural networks develop complex behaviors through mechanisms of synaptic plasticity, neuromodulation, and developmental processes.

Key concepts explored in this study include:
- Neural plasticity and learning dynamics
- The role of neuromodulation in behavior regulation
- Scale-dependent properties of neural systems
- Emergent phenomena in multi-scale simulations

This research addresses fundamental questions about how simple neural mechanisms can give rise to complex cognitive capabilities.
"""
    
    def generate_analysis_section(self) -> str:
        """Generate analysis procedures section."""
        return """
Data Analysis Pipeline:
1. Time series preprocessing and quality control
2. Statistical normalization and outlier detection
3. Descriptive statistical analysis
4. Inferential statistical testing
5. Machine learning model application
6. Visualization and interpretation

Statistical Tests Applied:
- One-way ANOVA for group comparisons
- Post-hoc Tukey tests for pairwise comparisons
- Linear regression for continuous relationships
- Non-parametric tests where assumptions violated
"""
    
    def generate_descriptive_statistics_section(self) -> str:
        """Generate descriptive statistics section."""
        
        if not self.report_data['results']:
            return "Descriptive statistics not available."
        
        results = self.report_data['results']
        
        stats_text = ""
        
        if 'firing_statistics' in results:
            firing_stats = results['firing_statistics']
            stats_text += f"""
Firing Statistics:
- Mean Firing Percentage: {firing_stats.get('firing_percentage_mean', 0):.2f}%
- Median Firing Percentage: {firing_stats.get('firing_percentage_median', 0):.2f}%
- Standard Deviation: {firing_stats.get('firing_percentage_std', 0):.2f}%
- Range: {firing_stats.get('firing_percentage_min', 0):.2f}% - {firing_stats.get('firing_percentage_max', 0):.2f}%

"""
        
        if 'activity_statistics' in results:
            activity_stats = results['activity_statistics']
            stats_text += f"""
Activity Statistics:
- Mean Activity Level: {activity_stats.get('activity_level_mean', 0):.2f}%
- Median Activity Level: {activity_stats.get('activity_level_median', 0):.2f}%
- Standard Deviation: {activity_stats.get('activity_level_std', 0):.2f}%
- Range: {activity_stats.get('activity_level_min', 0):.2f}% - {activity_stats.get('activity_level_max', 0):.2f}%

"""
        
        return stats_text
    
    def generate_inferential_statistics_section(self) -> str:
        """Generate inferential statistics section."""
        
        if not self.report_data['analysis']:
            return "Inferential statistics not available."
        
        analysis = self.report_data['analysis']
        
        stats_text = ""
        
        if 'anova_results' in analysis:
            anova_results = analysis['anova_results']
            stats_text += """
ANOVA Results:
"""
            for metric, result in anova_results.items():
                if isinstance(result, dict) and 'f_statistic' in result:
                    stats_text += f"""
- {metric}:
  - F-statistic: {result['f_statistic']:.4f}
  - P-value: {result['p_value']:.4f}
  - Significance: {'' if result['p_value'] < 0.05 else 'Not '}significant (p < 0.05)

"""
        
        if 'posthoc_results' in analysis:
            posthoc_results = analysis['posthoc_results']
            stats_text += """
Post Hoc Comparisons:
"""
            for comparison, result in posthoc_results.items():
                if isinstance(result, dict) and 'p_value' in result:
                    stats_text += f"""
- {comparison}:
  - t-statistic: {result.get('t_statistic', 'N/A'):.4f}
  - P-value: {result['p_value']:.4f}
  - Significance: {'' if result['p_value'] < 0.05 else 'Not '}significant (p < 0.05)

"""
        
        return stats_text
    
    def generate_time_series_analysis_section(self) -> str:
        """Generate time series analysis section."""
        
        if not self.report_data['analysis']:
            return "Time series analysis not available."
        
        analysis = self.report_data['analysis']
        
        if 'time_series_results' in analysis:
            ts_results = analysis['time_series_results']
            
            stats_text = "\nTime Series Analysis Results:\n"
            
            for metric, result in ts_results.items():
                if isinstance(result, dict):
                    stats_text += f"""
**{metric}:**
- Mean: {result.get('mean', 0):.4f}
- Standard Deviation: {result.get('std', 0):.4f}
- Trend Slope: {result.get('trend_slope', 0):.6f}
- Autocorrelation (lag-1): {result.get('autocorrelation', 0):.4f}
- Stationarity: {result.get('is_stationary', 'Unknown')}
- Number of Observations: {result.get('number_of_observations', 0)}

"""
            
            return stats_text
        
        return "Time series analysis not available."
    
    def generate_references_section(self) -> str:
        """Generate references section."""
        return """
1. Doe, J. (2023). Neural Learning Machine Framework. NLM Research Institute.

2. Smith, A. (2022). Spiking Neural Networks and Learning. Journal of Computational Neuroscience.

3. Johnson, B., & Lee, C. (2021). Neuromodulation in Neural Systems. Neural Computation Review.

4. Williams, R. (2020). Scale-Dependent Properties of Neural Networks. PLOS Computational Biology.

5. Martinez, L., & Garcia, M. (2019). Developmental Learning in Artificial Neural Networks. IEEE Transactions on Neural Systems.
"""

# Usage example
experiment_name = "Comprehensive Neural Learning Research"
report_generator = ResearchReportGenerator(experiment_name)

# Set report data
report_generator.set_metadata({
    'nlm_version': '0.1.0',
    'python_version': '3.8.0',
    'dependencies': ['numpy', 'matplotlib', 'scipy', 'pandas'],
    'experiment_date': '2023-10-15',
    'researcher': 'Research Team'
})

report_generator.set_methodology({
    'scale': 10000,
    'config': {
        'brain.neuron_count': 10000,
        'brain.synapse_density': 0.12,
        'plasticity.stdp.learning_rate': 0.001
    },
    'steps': 5000,
    'sampling_rate': '10 steps'
})

report_generator.set_results({
    'total_data_points': 50000,
    'firing_statistics': {
        'firing_percentage_mean': 45.2,
        'firing_percentage_median': 43.8,
        'firing_percentage_std': 12.3
    },
    'activity_statistics': {
        'activity_level_mean': 41.5,
        'activity_level_median': 40.2,
        'activity_level_std': 11.8
    }
})

report_generator.set_analysis({
    'anova_results': {
        'firing_percentage': {
            'f_statistic': 3.456,
            'p_value': 0.032,
            'significance': True
        }
    },
    'posthoc_results': {
        'scale_comparison': {
            't_statistic': 2.134,
            'p_value': 0.045,
            'significance': True
        }
    }
})

report_generator.set_conclusions({
    'finding1': 'Learning capacity increases significantly with brain size.',
    'finding2': 'Neuromodulation improves exploration efficiency.',
    'finding3': 'Developmental processes enhance behavioral flexibility.'
})

# Generate and save reports
print("Generating comprehensive research reports...")
md_report = report_generator.save_markdown_report()
print(f"Markdown report: {md_report}")

# Optionally generate PDF
try:
    pdf_report = report_generator.save_pdf_report()
    print(f"PDF report: {pdf_report}")
except:
    print("PDF generation skipped (weasyprint not available)")

print(f"\nResearch reporting complete. Reports generated for {experiment_name}.")
```

## Research Workflow Best Practices

### Planning and Preparation

#### 1. Experimental Design Quality Assurance

**Checkpoints**:
- [ ] Research question is clear and specific
- [ ] Hypothesis is testable and falsifiable
- [ ] Experimental design controls for confounding variables
- [ ] Sample size is adequate for statistical power
- [ ] Data collection methods are reliable and valid

#### 2. Data Management Standards

**Data Organization**:
```
/experiment_name/
├── raw_data/
│   ├── run_01/
│   ├── run_02/
│   └── ...
├── processed_data/
│   ├── time_series/
│   ├── metrics/
│   └── visualizations/
├── analysis/
│   ├── scripts/
│   ├── results/
│   └── reports/
├── documentation/
│   ├── methodology/
│   ├── procedures/
│   └── references/
└── backup/
    ├── cloud_storage/
    └── local_storage/
```

#### 3. Version Control and Reproducibility

**Git Workflow**:
```bash
# Create experiment branch
git checkout -b experiment_name_YYYYMMDD

# Commit configuration
git add config/
git add experiment_config.json
git commit -m "Add experimental configuration"

# Commit analysis scripts
git add analysis/
git commit -m "Add analysis pipeline"

# Commit results
# (be careful with large result files)
git add results/
git commit -m "Add experimental results"

# Create tag for reproducibility
git tag -a v1.0-expt_name_YYYYMMDD -m "Experiment v1.0 - Initial run"
git push origin v1.0-expt_name_YYYYMMDD
```

### Quality Assurance

#### 1. Data Quality Checks

**Automated Validation Script**:
```python
import pynlm
import numpy as np
import pandas as pd

class DataQualityChecker:
    """Ensures data quality for NLM research."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.quality_issues = []
    
    def check_data_integrity(self, data: pd.DataFrame) -> Dict[str, Any]:
        """Check data integrity and quality."""
        
        issues = {
            'missing_values': [],
            'anomalies': [],
            'consistency': [],
            'completeness': []
        }
        
        # Check for missing values
        numeric_columns = data.select_dtypes(include=[np.number]).columns
        for column in numeric_columns:
            missing_count = data[column].isnull().sum()
            if missing_count > 0:
                issues['missing_values'].append({
                    'column': column,
                    'missing_count': missing_count,
                    'percentage': (missing_count / len(data)) * 100
                })
        
        # Check for anomalies (outliers)
        for column in numeric_columns:
            column_data = data[column].dropna()
            if len(column_data) > 0:
                # Z-score method for outlier detection
                z_scores = np.abs((column_data - np.mean(column_data)) / np.std(column_data))
                outliers = z_scores > 3  # 3 standard deviations
                
                if outliers.any():
                    outlier_indices = column_data.index[outliers]
                    issues['anomalies'].append({
                        'column': column,
                        'outlier_count': outliers.sum(),
                        'outlier_indices': outlier_indices.tolist()
                    })
        
        # Check data consistency
        if 'step' in data.columns:
            # Check for duplicate steps
            duplicate_steps = data['step'].duplicated().sum()
            if duplicate_steps > 0:
                issues['consistency'].append({
                    'issue': 'Duplicate step numbers found',
                    'count': duplicate_steps
                })
            
            # Check for sequential steps
            missing_steps = data['step'].diff().dropna()
            non_sequential = missing_steps != 1.0
            if non_sequential.any():
                issues['consistency'].append({
                    'issue': 'Non-sequential steps found',
                    'affected_steps': missing_steps[non_sequential].index.tolist()
                })
        
        # Check data completeness
        expected_columns = ['step', 'timestamp', 'firing_percentage', 'activity_level']
        missing_columns = [col for col in expected_columns if col not in data.columns]
        if missing_columns:
            issues['completeness'].append({
                'missing_columns': missing_columns
            })
        
        return issues
    
    def generate_quality_report(self, issues: Dict[str, Any]) -> str:
        """Generate data quality report."""
        
        report = f"""# Data Quality Report - {self.experiment_name}

## Quality Assessment Date
{pd.Timestamp.now().strftime('%Y-%m-%d %H:%M:%S')}

## Quality Issues Summary
"""
        
        total_issues = sum(len(v) for v in issues.values())
        
        report += f"""
**Total Issues Found:** {total_issues}
- Missing Values: {len(issues['missing_values'])}
- Data Anomalies: {len(issues['anomalies'])}
- Consistency Issues: {len(issues['consistency'])}
- Completeness Issues: {len(issues['completeness'])}

"""
        
        # Detailed issues
        if issues['missing_values']:
            report += "## Missing Values Analysis\n"
            for issue in issues['missing_values']:
                report += f"""
### {issue['column']}
- Missing count: {issue['missing_count']}
- Missing percentage: {issue['percentage']:.2f}%

"""
        
        if issues['anomalies']:
            report += "## Data Anomalies Analysis\n"
            for issue in issues['anomalies']:
                report += f"""
### {issue['column']}
- Outlier count: {issue['outlier_count']}
- Outlier indices: {issue['outlier_indices']}

"""
        
        if issues['consistency']:
            report += "## Data Consistency Analysis\n"
            for issue in issues['consistency']:
                report += f"""
### {issue['issue']}
- Issue count: {issue.get('count', 'N/A')}

"""
        
        if issues['completeness']:
            report += "## Data Completeness Analysis\n"
            for issue in issues['completeness']:
                report += f"""
### Missing Columns
- Columns: {', '.join(issue['missing_columns'])}

"""
        
        # Quality recommendations
        report += "## Quality Recommendations\n"
        
        if issues['missing_values']:
            report += """
1. **Handle Missing Values:**
   - For <5% missing: Consider imputation
   - For 5-20% missing: Consider data collection
   - For >20% missing: Reconsider experiment design

2. **Imputation Methods:**
   - Mean/median imputation for continuous data
   - Forward/backward fill for time series
   - Multiple imputation for robust analysis
"""
        
        if issues['anomalies']:
            report += """
3. **Outlier Treatment:**
   - Investigate outliers for data entry errors
   - Consider removing legitimate extreme values
   - Use robust statistical methods for outlier-prone data
"""
        
        if issues['consistency']:
            report += """
4. **Data Consistency:**
   - Implement data validation during collection
   - Use automated checks in analysis pipeline
   - Document any data modifications
"""
        
        if issues['completeness']:
            report += """
5. **Data Completeness:**
   - Verify all expected data sources
   - Implement data validation scripts
   - Document data collection procedures
"""
        
        return report
    
    def check_reproducibility(self) -> Dict[str, Any]:
        """Check reproducibility requirements."""
        
        reproducibility_check = {
            'configuration_control': True,
            'environment_documentation': True,
            'data_integrity': True,
            'analysis_reproducibility': True,
            'documentation_completeness': True
        }
        
        return reproducibility_check

# Usage example
experiment_name = "Data Quality Assessment"
quality_checker = DataQualityChecker(experiment_name)

# Example with sample data
sample_data = pd.DataFrame({
    'step': range(1, 101),
    'timestamp': [time.time() + i*0.1 for i in range(100)],
    'firing_percentage': np.random.normal(45, 15, 100),
    'activity_level': np.random.normal(42, 12, 100),
    'e_i_ratio': np.random.uniform(0.5, 2.0, 100)
})

# Add some intentional issues for demonstration
sample_data.loc[10:15, 'firing_percentage'] = np.nan  # Missing values
sample_data.loc[0, 'firing_percentage'] = 500  # Outlier

# Check data quality
issues = quality_checker.check_data_integrity(sample_data)

# Generate quality report
quality_report = quality_checker.generate_quality_report(issues)
print(quality_report)

# Save quality report
with open(f"{experiment_name}_quality_report.md", 'w') as f:
    f.write(quality_report)

print(f"\nData quality assessment complete. Report saved as '{experiment_name}_quality_report.md'")
```

#### 2. Validation Scripts

**Validation Script Template**:
```python
#!/usr/bin/env python3
"""
NLM Research Experiment Validation Script

This script validates NLM research experiments according to standards.
"""

import sys
import os
import json
import argparse
from pathlib import Path
import subprocess

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Validate NLM research experiment"
    )
    
    parser.add_argument(
        "--experiment-dir",
        required=True,
        help="Path to experiment directory"
    )
    
    parser.add_argument(
        "--config-file",
        default="config.json",
        help="Path to experiment configuration file"
    )
    
    parser.add_argument(
        "--output-dir",
        required=True,
        help="Path to output validation results"
    )
    
    return parser.parse_args()

def validate_experiment_directory(experiment_dir: str) -> Dict[str, Any]:
    """Validate experiment directory structure."""
    
    expected_structure = {
        'raw_data/': ['run_01/', 'run_02/', 'run_03/'],
        'processed_data/': ['time_series/', 'metrics/'],
        'analysis/': ['scripts/', 'results/'],
        'documentation/': ['methodology/', 'references/']
    }
    
    validation_result = {
        'directory_exists': True,
        'structure_valid': True,
        'missing_directories': [],
        'expected_content': {}
    }
    
    for required_dir, expected_content in expected_structure.items():
        dir_path = os.path.join(experiment_dir, required_dir)
        
        if not os.path.exists(dir_path):
            validation_result['directory_exists'] = False
            validation_result['missing_directories'].append(required_dir)
        else:
            validation_result['expected_content'][required_dir] = {
                'exists': True,
                'content': []
            }
            
            if expected_content:
                for expected_item in expected_content:
                    item_path = os.path.join(dir_path, expected_item)
                    if os.path.exists(item_path):
                        validation_result['expected_content'][required_dir]['content'].append(
                            expected_item
                        )
    
    return validation_result

def validate_configuration_file(config_file: str) -> Dict[str, Any]:
    """Validate experiment configuration file."""
    
    validation_result = {
        'file_exists': os.path.exists(config_file),
        'json_valid': False,
        'required_keys': {},
        'warnings': []
    }
    
    if validation_result['file_exists']:
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            
            validation_result['json_valid'] = True
            
            # Check for required keys
            required_keys = ['experiment_name', 'scale', 'config']
            
            for key in required_keys:
                validation_result['required_keys'][key] = key in config
                
                if key not in config:
                    validation_result['warnings'].append(
                        f"Missing required configuration key: {key}"
                    )
            
        except json.JSONDecodeError as e:
            validation_result['warnings'].append(f"Invalid JSON in config file: {e}")
        except Exception as e:
            validation_result['warnings'].append(f"Error reading config file: {e}")
    
    return validation_result

def validate_data_files(data_dir: str) -> Dict[str, Any]:
    """Validate data files."""
    
    validation_result = {
        'directory_exists': os.path.exists(data_dir),
        'data_files_present': False,
        'file_count': 0,
        'total_size': 0
    }
    
    if validation_result['directory_exists']:
        # Count and validate data files
        data_files = []
        total_size = 0
        
        for root, dirs, files in os.walk(data_dir):
            for file in files:
                if file.endswith(('.csv', '.json', '.h5', '.hdf5', '.parquet')):
                    file_path = os.path.join(root, file)
                    data_files.append(file_path)
                    
                    try:
                        total_size += os.path.getsize(file_path)
                    except OSError:
                        pass
        
        validation_result['data_files_present'] = len(data_files) > 0
        validation_result['file_count'] = len(data_files)
        validation_result['total_size'] = total_size
        validation_result['data_files'] = data_files[:10]  # Show first 10 files
    
    return validation_result

def main():
    """Main validation function."""
    
    args = parse_arguments()
    
    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    
    # Run validation checks
    print("NLM Research Experiment Validation")
    print("=" * 50)
    
    # Directory validation
    print(f"\n1. Validating experiment directory: {args.experiment_dir}")
    dir_validation = validate_experiment_directory(args.experiment_dir)
    
    print(f"   Directory exists: {dir_validation['directory_exists']}")
    print(f"   Structure valid: {dir_validation['structure_valid']}")
    
    if dir_validation['missing_directories']:
        print(f"   Missing directories: {dir_validation['missing_directories']}")
    
    # Configuration validation
    print(f"\n2. Validating configuration file: {args.config_file}")
    config_path = os.path.join(args.experiment_dir, args.config_file)
    config_validation = validate_configuration_file(config_path)
    
    print(f"   File exists: {config_validation['file_exists']}")
    print(f"   JSON valid: {config_validation['json_valid']}")
    print(f"   Required keys: {config_validation['required_keys']}")
    
    if config_validation['warnings']:
        print(f"   Warnings:")
        for warning in config_validation['warnings']:
            print(f"     - {warning}")
    
    # Data files validation
    print(f"\n3. Validating data files")
    raw_data_dir = os.path.join(args.experiment_dir, 'raw_data')
    data_validation = validate_data_files(raw_data_dir)
    
    print(f"   Directory exists: {data_validation['directory_exists']}")
    print(f"   Data files present: {data_validation['data_files_present']}")
    print(f"   File count: {data_validation['file_count']}")
    print(f"   Total size: {data_validation['total_size'] / (1024**2):.2f} MB")
    
    if data_validation['data_files']:
        print(f"   Sample files: {', '.join(data_validation['data_files'][:5])}")
    
    # Generate validation report
    validation_report = {
        'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
        'experiment_directory': args.experiment_dir,
        'config_file': args.config_file,
        'output_directory': args.output_dir,
        'directory_validation': dir_validation,
        'config_validation': config_validation,
        'data_validation': data_validation,
        'overall_status': 'PASSED'
    }
    
    # Check for failures
    if (not dir_validation['directory_exists'] or 
        not dir_validation['structure_valid'] or
        not config_validation['file_exists'] or
        not config_validation['json_valid'] or
        not data_validation['data_files_present']):
        validation_report['overall_status'] = 'FAILED'
    
    # Save validation report
    report_path = os.path.join(args.output_dir, 'validation_report.json')
    
    with open(report_path, 'w') as f:
        json.dump(validation_report, f, indent=2)
    
    print(f"\n{'=' * 50}")
    print(f"Validation complete. Status: {validation_report['overall_status']}")
    print(f"Report saved to: {report_path}")
    
    # Return exit code
    sys.exit(0 if validation_report['overall_status'] == 'PASSED' else 1)

if __name__ == "__main__":
    main()
```

## Research Workflow Conclusion

This Research Workflow Documentation provides comprehensive guidance for conducting research with the NLM (Neural Learning Machine) framework. It covers:

1. **Experimental Design**: Research question formulation, experimental design principles, and design templates
2. **Environment Setup**: Hardware and software requirements, analysis environment setup, and validation procedures
3. **Simulation Execution**: Experiment management, reproducibility, and automated documentation
4. **Data Analysis**: Comprehensive analysis pipeline, statistical methods, and visualization techniques
5. **Documentation and Reporting**: Research documentation standards, report generation, and quality assurance
6. **Validation and Quality Assurance**: Data quality checks, validation scripts, and best practices

The workflow emphasizes reproducibility, rigorous methodology, and comprehensive documentation throughout the research process. It provides templates, scripts, and examples to help researchers conduct high-quality, reproducible NLM experiments from experimental design through data analysis and reporting.

The documentation supports researchers at all levels, from beginners conducting their first NLM experiments to experienced researchers implementing complex, multi-scale studies with advanced analysis techniques.

-----
*End of Research Workflow Documentation*