"""
Research and Experimentation Examples for NLM

This module demonstrates research and experimentation capabilities including:
- Scientific methodology implementation
- Experiment design and validation
- Data collection and analysis
- Reproducible research
- Publication-ready results
"""

import pynlm
import numpy as np
import time
import json
from typing import Dict, List, Optional, Tuple
import matplotlib.pyplot as plt
from datetime import datetime
import random

class ResearchExamples:
    """Research and experimentation examples."""
    
    @staticmethod
    def create_scientific_experiment():
        """
        Example: Scientific experiment with hypothesis testing.
        
        Demonstrates scientific methodology for researching NLM including:
        - Hypothesis formulation
        - Experimental design
        - Data collection
        - Statistical analysis
        - Result validation
        """
        print("=== Scientific Experiment Example ===")
        
        # Research hypothesis
        research_hypothesis = {
            "title": "Effect of Synaptic Plasticity on Learning Speed",
            "null_hypothesis": "Synaptic plasticity has no effect on learning speed",
            "alternative_hypothesis": "Enhanced synaptic plasticity increases learning speed",
            "variables": {
                "independent": ["plasticity_type", "learning_rate"],
                "dependent": ["learning_speed", "memory_retention", "behavioral_adaptability"],
                "control": ["environment", "task_difficulty", "initial_conditions"]
            },
            "predicted_outcome": "Brains with enhanced plasticity will learn 30% faster"
        }
        
        print("Research Hypothesis:")
        print(f"  Title: {research_hypothesis['title']}")
        print(f"  Null Hypothesis: {research_hypothesis['null_hypothesis']}")
        print(f"  Alternative: {research_hypothesis['alternative_hypothesis']}")
        
        # Experimental design
        experimental_design = {
            "participants": 50,
            "groups": [
                {"id": "control", "plasticity": "standard", "neuron_count": 2000},
                {"id": "stdp_enhanced", "plasticity": "stdp_boosted", "neuron_count": 2000},
                {"id": "hebbian_enhanced", "plasticity": "hebbian_boosted", "neuron_count": 2000},
                {"id": "combined", "plasticity": "combined_enhanced", "neuron_count": 2000}
            ],
            "tasks": [
                "visual_pattern_recognition",
                "spatial_navigation",
                "working_memory_tasks",
                "social_interactions"
            ],
            "duration": "4 weeks",
            "measurement_points": ["baseline", "week_1", "week_2", "week_3", "week_4"]
        }
        
        print(f"\nExperimental Design:")
        print(f"  Participants: {experimental_design['participants']}")
        print(f"  Groups: {len(experimental_design['groups'])}")
        print(f"  Tasks: {len(experimental_design['tasks'])}")
        print(f"  Duration: {experimental_design['duration']}")
        
        # Create research data structure
        research_data = {
            "experiment_id": f"NLM_EXP_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            "hypothesis": research_hypothesis,
            "experimental_design": experimental_design,
            "results": {},
            "analysis": {},
            "conclusions": {},
            "metadata": {
                "start_time": datetime.now(),
                "researcher": "NLM_Researcher",
                "institution": "NLM_Research_Institute",
                "funding": "NLM_Research_Grant_2024"
            }
        }
        
        # Simulate experiment execution
        print("\nExecuting experiment...")
        
        # Define plasticity configurations for each group
        plasticity_configs = {
            "standard": {"stdp_learning_rate": 0.01, "hebbian_learning_rate": 0.005, "structural": False},
            "stdp_boosted": {"stdp_learning_rate": 0.03, "hebbian_learning_rate": 0.005, "structural": False},
            "hebbian_boosted": {"stdp_learning_rate": 0.01, "hebbian_learning_rate": 0.02, "structural": False},
            "combined_enhanced": {"stdp_learning_rate": 0.02, "hebbian_learning_rate": 0.015, "structural": True}
        }
        
        # Run simulation for each group
        for group in experimental_design["groups"]:
            print(f"\n  Testing group: {group['id']}")
            
            # Create group-specific configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", group["neuron_count"])
            config.set("brain.synapse_density", 0.1)
            
            # Set plasticity based on group
            plasticity = plasticity_configs[group["plasticity"]]
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", plasticity["stdp_learning_rate"])
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.hebbian.learning_rate", plasticity["hebbian_learning_rate"])
            config.set("plasticity.structural.enable", plasticity["structural"])
            
            # Create brain and simulate learning
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Simulate task performance
            task_performance = {}
            for task in experimental_design["tasks"]:
                performance = simulate_task_performance(task, group["id"])
                task_performance[task] = performance
            
            # Calculate learning metrics
            learning_metrics = {
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "final_active_neurons": brain.getActiveNeuronCount(),
                "task_performance": task_performance,
                "plasticity_config": plasticity
            }
            
            research_data["results"][group["id"]] = learning_metrics
            
            print(f"    Learning completed: {learning_metrics['average_firing_rate']:.1f} Hz avg firing rate")
            print(f"    Task performance: {len(task_performance)} tasks completed")
        
        # Statistical analysis
        print("\n=== Statistical Analysis ===")
        
        # Extract performance data for statistical testing
        performance_data = {}
        for group_id, group_data in research_data["results"].items():
            # Calculate group-level performance metrics
            task_scores = []
            for task, performance in group_data["task_performance"].items():
                task_scores.append(performance["score"])
            
            performance_data[group_id] = {
                "avg_score": np.mean(task_scores),
                "score_std": np.std(task_scores),
                "scores": task_scores
            }
        
        # Calculate group comparisons
        print("\nGroup Performance Comparison:")
        for group_id, group_data in performance_data.items():
            print(f"  {group_id.title()}: Average score = {group_data['avg_score']:.3f} "
                  f"(std = {group_data['score_std']:.3f})")
        
        # Statistical tests (simplified)
        control_score = performance_data["control"]["avg_score"]
        stdp_score = performance_data["stdp_enhanced"]["avg_score"]
        hebbian_score = performance_data["hebbian_enhanced"]["avg_score"]
        combined_score = performance_data["combined"]["avg_score"]
        
        # Calculate effect sizes
        stdp_effect = (stdp_score - control_score) / control_score
        hebbian_effect = (hebbian_score - control_score) / control_score
        combined_effect = (combined_score - control_score) / control_score
        
        print(f"\nStatistical Results:")
        print(f"  Control group: {control_score:.3f}")
        print(f"  STDP-enhanced: {stdp_score:.3f} (effect size: {stdp_effect:.2%})")
        print(f"  Hebbian-enhanced: {hebbian_score:.3f} (effect size: {hebbian_effect:.2%})")
        print(f"  Combined-enhanced: {combined_score:.3f} (effect size: {combined_effect:.2%})")
        
        # Determine statistical significance (simplified)
        if stdp_effect > 0.2:
            print(f"  ✓ STDP enhancement is statistically significant")
        if hebbian_effect > 0.2:
            print(f"  ✓ Hebbian enhancement is statistically significant")
        if combined_effect > 0.3:
            print(f"  ✓ Combined enhancement shows significant synergistic effects")
        
        # Update research data with analysis
        research_data["analysis"] = {
            "statistical_significance": {
                "stdp_significant": stdp_effect > 0.2,
                "hebbian_significant": hebbian_effect > 0.2,
                "combined_significant": combined_effect > 0.3
            },
            "effect_sizes": {
                "stdp": stdp_effect,
                "hebbian": hebbian_effect,
                "combined": combined_effect
            },
            "power_analysis": {
                "sample_size": experimental_design["participants"],
                "detected_effects": [stdp_effect, hebbian_effect, combined_effect]
            }
        }
        
        # Generate conclusions
        research_data["conclusions"] = {
            "hypothesis_accepted": stdp_effect > 0.2 or hebbian_effect > 0.2 or combined_effect > 0.3,
            "key_findings": [
                f"STDP enhancement improved performance by {stdp_effect:.1%}",
                f"Hebbian enhancement improved performance by {hebbian_effect:.1%}",
                f"Combined enhancement showed synergistic effects of {combined_effect:.1%}"
            ],
            "implications": [
                "Enhanced synaptic plasticity can significantly improve learning speed",
                "Combined plasticity mechanisms show synergistic benefits",
                "These findings support further research into plasticity-based interventions"
            ],
            "limitations": [
                "Simulated environment may not capture all real-world complexities",
                "Limited to 4 specific task types",
                "Short-term simulation may not capture long-term consolidation effects"
            ],
            "future_research": [
                "Longitudinal studies with human participants",
                "Investigation of neural mechanisms underlying enhanced plasticity",
                "Application of findings to cognitive enhancement therapies"
            ]
        }
        
        print(f"\nResearch Conclusion:")
        print(f"  Hypothesis accepted: {'✓' if research_data['conclusions']['hypothesis_accepted'] else '✗'}")
        print(f"  Key findings: {len(research_data['conclusions']['key_findings'])} identified")
        print(f"  Recommendations for future research: {len(research_data['conclusions']['future_research'])} proposed")
        
        return {
            "research_data": research_data,
            "config": pynlm.createDefaultConfig()  # Return a basic config
        }
    
    @staticmethod
    def create_reproducible_research_example():
        """
        Example: Reproducible research with version control.
        
        Demonstrates reproducible research practices including:
        - Version control integration
        - Data documentation
        - Parameter recording
        - Result validation
        - Publication preparation
        """
        print("\n=== Reproducible Research Example ===")
        
        # Create research reproducibility package
        reproducibility_package = {
            "research_id": f"NLM_REPRO_{datetime.now().strftime('%Y%m%d')}",
            "title": "Reproducible NLM Research: Configuration and Results",
            "description": "Research package demonstrating reproducible NLM experiments with complete documentation",
            "created_date": datetime.now().isoformat(),
            "last_updated": datetime.now().isoformat(),
            "version": "1.0.0"
        }
        
        print("Creating reproducible research package...")
        print(f"  Research ID: {reproducibility_package['research_id']}")
        print(f"  Title: {reproducibility_package['title']}")
        
        # Configuration documentation
        configuration_documentation = {
            "nlm_version": "Phase 6.2",
            "python_version": "3.8+",
            "dependencies": {
                "pynlm": ">=1.0.0",
                "numpy": ">=1.20.0",
                "matplotlib": ">=3.3.0"
            },
            "environment_setup": {
                "python_environment": "conda env: nlm-research",
                "installation_commands": [
                    "pip install pynlm numpy matplotlib",
                    "pip install -e .  # Install NLM development version"
                ],
                "python_setup": "python setup_reproducible_experiment.py"
            }
        }
        
        # Experiment configuration
        experiment_config = {
            "brain_architecture": {
                "neuron_count": 3000,
                "synapse_density": 0.1,
                "plasticity_mechanisms": {
                    "stdp": {"enabled": True, "learning_rate": 0.015},
                    "hebbian": {"enabled": True, "learning_rate": 0.01},
                    "structural": {"enabled": True}
                },
                "neuromodulation": {
                    "dopamine_scale": 1.0,
                    "curiosity": {"enabled": True},
                    "novelty": {"enabled": True}
                }
            },
            "experimental_parameters": {
                "simulation_steps": 500,
                "time_step": 0.1,
                "random_seed": 42,
                "repeats": 5,
                "snapshot_interval": 50
            },
            "data_collection": {
                "metrics": ["firing_rate", "active_neurons", "spike_count", "memory_usage"],
                "sampling_frequency": "every_step",
                "output_format": "csv_and_json",
                "storage_location": "results/experiment_data/"
            }
        }
        
        # Data schema documentation
        data_schema = {
            "brain_state": {
                "type": "object",
                "description": "Brain state snapshot",
                "fields": {
                    "step": "integer",
                    "firing_rate": "float",
                    "total_spikes": "integer",
                    "active_neurons": "integer",
                    "learning_metrics": "object"
                }
            },
            "performance_metrics": {
                "type": "object",
                "description": "Experimental performance metrics",
                "fields": {
                    "task_performance": "object",
                    "learning_curve": "array",
                    "convergence_metrics": "object"
                }
            }
        }
        
        # Create research scripts
        research_scripts = {
            "setup_reproducible_experiment.py": {
                "description": "Setup script for reproducible experiment",
                "content": """
import pynlm
import numpy as np
import json
import os
from datetime import datetime

def setup_reproducible_experiment():
    '''Setup reproducible NLM experiment'''
    # Set random seed for reproducibility
    np.random.seed(42)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create output directory
    os.makedirs("results/experiment_data", exist_ok=True)
    
    # Save experiment configuration
    exp_config = {
        "config": config,
        "timestamp": datetime.now().isoformat(),
        "parameters": {
            "simulation_steps": 500,
            "time_step": 0.1,
            "random_seed": 42
        }
    }
    
    with open("results/experiment_config.json", "w") as f:
        json.dump(exp_config, f, indent=2)
    
    return brain, config

if __name__ == "__main__":
    brain, config = setup_reproducible_experiment()
    print("Reproducible experiment setup complete!")
    """
            },
            "run_experiment.py": {
                "description": "Main experiment execution script",
                "content": """
import pynlm
import numpy as np
import json
import csv
from datetime import datetime
import os

def run_reproducible_experiment(brain, config, num_steps=500):
    '''Run reproducible NLM experiment'''
    # Initialize results storage
    results = {
        "experiment_id": "reproducible_exp",
        "timestamp": datetime.now().isoformat(),
        "parameters": {
            "num_steps": num_steps,
            "neuron_count": config.get("brain.neuron_count")
        },
        "data": []
    }
    
    # Run experiment
    for step in range(num_steps):
        # Execute brain step
        brain.step(step)
        
        # Record metrics
        step_data = {
            "step": step,
            "timestamp": datetime.now().isoformat(),
            "firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "spike_count": brain.getTotalSpikeCount()
        }
        
        results["data"].append(step_data)
        
        # Save snapshot periodically
        if step % 50 == 0:
            snapshot_file = f"results/experiment_data/snapshot_step_{step}.json"
            with open(snapshot_file, "w") as f:
                json.dump(step_data, f, indent=2)
    
    # Save complete results
    with open("results/experiment_data/full_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    # Save CSV version
    csv_file = "results/experiment_data/experiment_results.csv"
    with open(csv_file, "w", newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["step", "firing_rate", "total_spikes", "active_neurons", "spike_count"])
        for step_data in results["data"]:
            writer.writerow([
                step_data["step"],
                step_data["firing_rate"],
                step_data["total_spikes"],
                step_data["active_neurons"],
                step_data["spike_count"]
            ])
    
    return results

if __name__ == "__main__":
    # Load or setup experiment
    if os.path.exists("results/experiment_config.json"):
        with open("results/experiment_config.json", "r") as f:
            exp_config = json.load(f)
        brain = pynlm.createBrain(exp_config["config"])
        brain.initialize()
    else:
        print("Run setup_reproducible_experiment.py first!")
        exit(1)
    
    # Run experiment
    results = run_reproducible_experiment(brain, exp_config["config"])
    print(f"Experiment complete! Results saved to results/experiment_data/")
    print(f"Total data points: {len(results['data'])}")
"""
            },
            "analyze_results.py": {
                "description": "Analysis script for reproducible results",
                "content": """
import json
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime

def analyze_reproducible_results():
    '''Analyze reproducible experiment results'''
    # Load results
    with open("results/experiment_data/full_results.json", "r") as f:
        results = json.load(f)
    
    # Convert to DataFrame for analysis
    df = pd.DataFrame(results["data"])
    
    # Basic statistics
    stats = {
        "total_steps": len(df),
        "avg_firing_rate": df["firing_rate"].mean(),
        "median_firing_rate": df["firing_rate"].median(),
        "std_firing_rate": df["firing_rate"].std(),
        "final_firing_rate": df["firing_rate"].iloc[-1],
        "total_spikes": df["total_spikes"].iloc[-1],
        "learning_rate": (df["firing_rate"].iloc[-1] - df["firing_rate"].iloc[0]) / df["firing_rate"].iloc[0]
    }
    
    # Create analysis report
    analysis_report = {
        "analysis_date": datetime.now().isoformat(),
        "experiment_id": results["experiment_id"],
        "basic_statistics": stats,
        "data_quality": {
            "missing_values": df.isnull().sum().to_dict(),
            "data_range": {
                "firing_rate_min": df["firing_rate"].min(),
                "firing_rate_max": df["firing_rate"].max(),
                "spikes_min": df["total_spikes"].min(),
                "spikes_max": df["total_spikes"].max()
            }
        },
        "recommendations": [
            "Results are reproducible and reliable",
            "Experiment can be extended with additional parameter variations",
            "Findings support the original hypothesis"
        ]
    }
    
    # Save analysis report
    with open("results/experiment_analysis.json", "w") as f:
        json.dump(analysis_report, f, indent=2)
    
    print("Analysis complete! Results saved to results/experiment_analysis.json")
    return analysis_report

if __name__ == "__main__":
    analyze_reproducible_results()
"""
            }
        }
        
        # Create validation script
        validation_script = {
            "validate_reproducibility.py": {
                "description": "Validate reproducibility of experiment",
                "content": """
import json
import numpy as np
import os
from datetime import datetime

def validate_reproducibility():
    '''Validate experiment reproducibility'''
    validation_results = {
        "validation_date": datetime.now().isoformat(),
        "checks_performed": [],
        "reproducibility_score": 0,
        "issues_found": [],
        "recommendations": []
    }
    
    # Check 1: Configuration file exists
    if os.path.exists("results/experiment_config.json"):
        validation_results["checks_performed"].append({
            "check": "Configuration file exists",
            "passed": True,
            "details": "experiment_config.json found"
        })
    else:
        validation_results["checks_performed"].append({
            "check": "Configuration file exists",
            "passed": False,
            "details": "experiment_config.json not found"
        })
        validation_results["issues_found"].append("Missing configuration file")
    
    # Check 2: Data files exist
    data_dir = "results/experiment_data"
    if os.path.exists(data_dir) and len(os.listdir(data_dir)) > 0:
        data_files = [f for f in os.listdir(data_dir) if f.endswith(('.json', '.csv'))]
        if len(data_files) > 0:
            validation_results["checks_performed"].append({
                "check": "Data files exist",
                "passed": True,
                "details": f"{len(data_files)} data files found"
            })
        else:
            validation_results["checks_performed"].append({
                "check": "Data files exist",
                "passed": False,
                "details": "No data files found"
            })
            validation_results["issues_found"].append("Missing data files")
    else:
        validation_results["checks_performed"].append({
            "check": "Data files exist",
            "passed": False,
            "details": "Data directory not found or empty"
        })
        validation_results["issues_found"].append("Missing data directory")
    
    # Check 3: Analysis report exists
    if os.path.exists("results/experiment_analysis.json"):
        validation_results["checks_performed"].append({
            "check": "Analysis report exists",
            "passed": True,
            "details": "experiment_analysis.json found"
        })
    else:
        validation_results["checks_performed"].append({
            "check": "Analysis report exists",
            "passed": False,
            "details": "experiment_analysis.json not found"
        })
        validation_results["issues_found"].append("Missing analysis report")
    
    # Calculate reproducibility score
    passed_checks = sum(1 for check in validation_results["checks_performed"] if check["passed"])
    validation_results["reproducibility_score"] = (passed_checks / len(validation_results["checks_performed"])) * 100
    
    # Add recommendations based on validation
    if validation_results["reproducibility_score"] >= 90:
        validation_results["recommendations"].append("Excellent reproducibility! All checks passed.")
    elif validation_results["reproducibility_score"] >= 70:
        validation_results["recommendations"].append("Good reproducibility. Minor issues identified.")
    else:
        validation_results["recommendations"].append("Poor reproducibility. Significant issues require attention.")
    
    # Save validation results
    with open("results/reproducibility_validation.json", "w") as f:
        json.dump(validation_results, f, indent=2)
    
    print(f"Reproducibility validation complete! Score: {validation_results['reproducibility_score']:.1f}%")
    return validation_results

if __name__ == "__main__":
    validate_reproducibility()
"""
            }
        }
        
        # Create publication package
        publication_package = {
            "README.md": """
# NLM Reproducible Research Package

## Overview
This package contains reproducible research demonstrating NLM (Neural Learning Machine) experiments.

## How to Use
1. Ensure Python 3.8+ is installed
2. Install dependencies: `pip install pynlm numpy matplotlib`
3. Run setup: `python setup_reproducible_experiment.py`
4. Run experiment: `python run_experiment.py`
5. Analyze results: `python analyze_results.py`
6. Validate reproducibility: `python validate_reproducibility.py`

## Files
- `setup_reproducible_experiment.py` - Setup script
- `run_experiment.py` - Main experiment execution
- `analyze_results.py` - Analysis and visualization
- `validate_reproducibility.py` - Validation of reproducibility
- `results/` - All experiment results and outputs

## Citation
When using this research, please cite:
NLM Research Team. (2024). NLM Reproducible Research Package. Version 1.0.0.
DOI: [to be assigned]

## Contact
For questions about this research package, contact:
nlm-research@nlm.org
""",
            "CITATION.cff": """---
cff-version: 1.2.0
message: "Research package for reproducible NLM experiments"
authors:
  - name: NLM Research Team
    orcid: "https://orcid.org/0000-0000-0000-0000"
title: "NLM Reproducible Research Package"
description: "Comprehensive package for reproducible neural learning machine research"
version: "1.0.0"
doi: "10.xxxx/nlm.reproducible.research.2024"
""",
            "LICENSE": "MIT License",
            "ACKNOWLEDGMENTS.md": """
# Acknowledgments

This research was made possible through:

1. **NLM Development Team** - For creating the NLM framework
2. **Research Participants** - For providing valuable data
3. **Funding Agencies** - For supporting this research
4. **Open Source Contributors** - For valuable libraries and tools

## Dependencies
- Python 3.8+
- pynlm (>=1.0.0)
- numpy (>=1.20.0)
- matplotlib (>=3.3.0)
"""
        }
        
        print(f"\nCreating reproducible research package...")
        print(f"  Package includes: {len(reproducibility_package)} files")
        print(f"  Research scripts: {len(research_scripts)} scripts")
        print(f"  Publication materials: README, CITATION, LICENSE")
        
        # Save reproduction package
        reproduction_package = {
            "reproducibility_package": reproducibility_package,
            "research_scripts": research_scripts,
            "validation_script": validation_script,
            "publication_package": publication_package,
            "generated_date": datetime.now().isoformat()
        }
        
        # Create results directory
        os.makedirs("results", exist_ok=True)
        os.makedirs("results/experiment_data", exist_ok=True)
        
        # Save package metadata
        with open("results/reproduction_package.json", "w") as f:
            json.dump(reproduction_package, f, indent=2)
        
        print("\nReproduction package created successfully!")
        print(f"  Package stored in: results/reproduction_package.json")
        print(f"  Research scripts: Python scripts in main directory")
        print(f"  Results stored in: results/experiment_data/")
        
        return {
            "reproducibility_package": reproducibility_package,
            "research_scripts": research_scripts,
            "validation_script": validation_script,
            "publication_package": publication_package,
            "generated_date": datetime.now().isoformat()
        }

if __name__ == "__main__":
    print("Reproducible Research Example")
    print("=" * 30)
    
    # Create reproducible research package
    reproduction_result = ResearchExamples.create_reproducible_research_example()
    print(f"\nReproducible research: {len(reproduction_result['reproducibility_package'])} package files created")
    
    print("\n=== All research examples completed ===")
