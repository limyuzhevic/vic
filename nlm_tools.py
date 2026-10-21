#!/usr/bin/env python3
"""
NLM Experiment Management Tools

Additional tools for managing NLM experiments, configurations, and analysis.
"""

import json
import os
import subprocess
import sys
from pathlib import Path
from datetime import datetime

def export_experiment_results(args):
    """Export experiment results in multiple formats"""
    print(f"Exporting experiment results from: {args.input}")
    
    if not os.path.exists(args.input):
        print(f"ERROR: Input file not found: {args.input}")
        return False
    
    # Parse input file to determine format
    file_ext = os.path.splitext(args.input)[1].lower()
    
    # Read data
    data = None
    if file_ext == '.csv':
        import pandas as pd
        df = pd.read_csv(args.input)
        
        # Create metadata
        data = {
            "metadata": {
                "source_file": args.input,
                "exported_at": datetime.now().isoformat(),
                "record_count": len(df),
                "version": "1.0"
            },
            "experiments": []
        }
        
        # Process CSV data into experiment format
        for _, row in df.iterrows():
            exp = {
                "name": f"Experiment_Step_{int(row['Step'])}",
                "seed": args.seed if hasattr(args, 'seed') else 42,
                "step": int(row['Step']),
                "neuron_count": int(row['NeuronCount']),
                "synapse_count": int(row['SynapseCount']),
                "firing_count": int(row['FiringCount']),
                "avg_firing_rate": float(row['AvgFiringRate']),
                "timestamp": datetime.now().isoformat()
            }
            data["experiments"].append(exp)
    
    elif file_ext == '.json':
        with open(args.input, 'r') as f:
            data = json.load(f)
    
    else:
        print(f"ERROR: Unsupported file format: {file_ext}")
        print("Supported formats: .csv, .json")
        return False
    
    # Export to multiple formats
    export_formats = args.format if hasattr(args, 'format') else ['json', 'csv']
    
    for fmt in export_formats:
        output_file = f"{os.path.splitext(args.input)[0]}_exported.{fmt}"
        
        try:
            if fmt == 'json':
                with open(output_file, 'w') as f:
                    json.dump(data, f, indent=2)
                print(f"✓ Exported JSON: {output_file}")
            
            elif fmt == 'csv' and 'experiments' in data:
                import pandas as pd
                df = pd.DataFrame(data['experiments'])
                df.to_csv(output_file, index=False)
                print(f"✓ Exported CSV: {output_file}")
            
            elif fmt == 'md':
                # Markdown report
                md_file = f"{os.path.splitext(args.input)[0]}_report.md"
                with open(md_file, 'w') as f:
                    f.write(f"# NLM Experiment Results Report\n")
                    f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
                    f.write(f"## Summary\n")
                    f.write(f"- Total experiments: {len(data.get('experiments', []))}\n")
                    f.write(f"- Source file: {args.input}\n\n")
                    
                    if 'experiments' in data:
                        f.write(f"## Detailed Results\n")
                        f.write(f"| Step | Neuron Count | Synapse Count | Firing Count | Avg Firing Rate |\n")
                        f.write(f"|------|--------------|---------------|---------------|-----------------|\n")
                        
                        for exp in data['experiments']:
                            f.write(f"| {exp['step']} | {exp['neuron_count']} | {exp['synapse_count']} | {exp['firing_count']} | {exp['avg_firing_rate']:.4f} |\n")
                
                print(f"✓ Exported Markdown report: {md_file}")
        
        except Exception as e:
            print(f"ERROR: Failed to export {fmt}: {e}")
            continue
    
    return True

def generate_config_template(args):
    """Generate a configuration template for NLM experiments"""
    print("Generating NLM configuration template...")
    
    template = {
        "metadata": {
            "description": "NLM (Neural Learning Machine) Configuration",
            "version": "1.0",
            "author": "NLM Research Team",
            "created": datetime.now().isoformat()
        },
        "experiment_settings": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "max_steps": 10000,
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1,
            "enable_checkpointing": True,
            "enable_replay": True,
            "enable_development": True,
            "checkpoint_dir": "./checkpoints",
            "checkpoint_interval": 1000,
            "max_checkpoints": 10
        },
        "brain_parameters": {
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "replay_interval": 100,
            "consolidation_interval": 1000
        },
        "memory_parameters": {
            "working_memory_capacity": 100,
            "working_memory_decay_rate": 0.01,
            "episodic_memory_max_episodes": 1000,
            "associative_memory_threshold": 0.5
        },
        "neuromodulation_parameters": {
            "dopamine_base_level": 0.1,
            "dopamine_decay_rate": 0.01,
            "curiosity_threshold": 0.5,
            "novelty_detection_threshold": 0.1
        },
        "output_settings": {
            "save_results": True,
            "results_format": ["csv", "json"],
            "log_level": "info",
            "log_file": "nlm_experiment.log"
        }
    }
    
    output_file = args.output if args.output else "nlm_config_template.json"
    
    try:
        with open(output_file, 'w') as f:
            json.dump(template, f, indent=2)
        
        print(f"✓ Configuration template saved to: {output_file}")
        print("\nTo use this template:")
        print("1. Copy the file to your project root")
        print("2. Modify parameters as needed")
        print("3. Load in your experiment code using JSON parser")
        
        return True
        
    except Exception as e:
        print(f"ERROR: Failed to generate template: {e}")
        return False

def compare_experiments(args):
    """Compare multiple experiment results"""
    print(f"Comparing experiment results: {args.files}")
    
    if len(args.files) < 2:
        print("ERROR: Need at least 2 experiment files to compare")
        return False
    
    experiments = []
    for file in args.files:
        if not os.path.exists(file):
            print(f"ERROR: File not found: {file}")
            return False
        
        try:
            if file.endswith('.csv'):
                import pandas as pd
                df = pd.read_csv(file)
                exp_name = os.path.splitext(os.path.basename(file))[0]
                
                exp_data = {
                    "name": exp_name,
                    "file": file,
                    "total_steps": len(df),
                    "avg_neuron_count": df['NeuronCount'].mean(),
                    "avg_synapse_count": df['SynapseCount'].mean(),
                    "avg_firing_rate": df['AvgFiringRate'].mean(),
                    "max_firing_rate": df['AvgFiringRate'].max(),
                    "total_firing_count": df['FiringCount'].sum()
                }
                
                experiments.append(exp_data)
                
            elif file.endswith('.json'):
                with open(file, 'r') as f:
                    data = json.load(f)
                    exp_name = data.get('name', os.path.splitext(os.path.basename(file))[0])
                    
                    exp_data = {
                        "name": exp_name,
                        "file": file,
                        "metrics": data.get('metrics', {}),
                        "step_count": len(data.get('experiments', []))
                    }
                    
                    experiments.append(exp_data)
        
        except Exception as e:
            print(f"ERROR: Failed to load {file}: {e}")
            return False
    
    # Generate comparison report
    report_file = "experiment_comparison.md"
    
    try:
        with open(report_file, 'w') as f:
            f.write(f"# NLM Experiment Comparison Report\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            f.write(f"## Compared Experiments\n")
            f.write(f"| Experiment | File | Steps | Avg Neurons | Avg Synapses | Avg Firing Rate | Max Firing Rate | Total Spikes |\n")
            f.write(f"|------------|------|-------|--------------|--------------|-----------------|-----------------|---------------|\n")
            
            for exp in experiments:
                if 'avg_neuron_count' in exp:  # CSV data
                    f.write(f"| {exp['name']} | {exp['file']} | {exp['total_steps']} | {exp['avg_neuron_count']:.1f} | {exp['avg_synapse_count']:.1f} | {exp['avg_firing_rate']:.4f} | {exp['max_firing_rate']:.4f} | {exp['total_firing_count']} |\n")
                else:  # JSON data
                    f.write(f"| {exp['name']} | {exp['file']} | {exp['step_count']} | - | - | - | - | - |\n")
        
        print(f"✓ Comparison report saved to: {report_file}")
        return True
        
    except Exception as e:
        print(f"ERROR: Failed to generate comparison report: {e}")
        return False

def batch_run_experiments(args):
    """Run multiple experiments in batch mode"""
    print(f"Batch running experiments: {args.experiment_names}")
    
    if not hasattr(args, 'experiment_names') or not args.experiment_names:
        print("ERROR: No experiment names provided for batch run")
        return False
    
    # This would interface with the NLM C++ experiment runner
    # For now, provide guidance
    print("\nTo run experiments in batch mode:")
    print("1. Prepare configuration files for each experiment")
    print("2. Use the enhanced ExperimentRunner::runBatch() method")
    print("3. Configure different parameters for each experiment")
    print("4. Use saveResults() to collect results from all experiments")
    print("5. Analyze results using the comparison tools")
    
    return True

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Experiment Management Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Tools for managing NLM experiments, configurations, and analysis.

Examples:
  nlm_tools.py export-results results.csv              # Export results to multiple formats
  nlm_tools.py generate-config --output myconfig.json  # Create config template
  nlm_tools.py compare-experiments file1.csv file2.csv   # Compare experiments
  nlm_tools.py batch-run --experiments exp1 exp2         # Run experiments in batch
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Export results command
    parser_export = subparsers.add_parser("export-results", help="Export experiment results")
    parser_export.add_argument("input", help="Input results file (CSV or JSON)")
    parser_export.add_argument("--format", nargs="+", choices=['csv', 'json', 'md'], 
                             help="Export formats (default: all supported)")
    parser_export.add_argument("--seed", type=int, default=42, help="Experiment seed for naming")
    
    # Generate config command
    parser_config = subparsers.add_parser("generate-config", help="Generate configuration template")
    parser_config.add_argument("--output", help="Output file path")
    
    # Compare experiments command
    parser_compare = subparsers.add_parser("compare-experiments", help="Compare multiple experiments")
    parser_compare.add_argument("files", nargs="+", help="Experiment files to compare")
    
    # Batch run command
    parser_batch = subparsers.add_parser("batch-run", help="Run experiments in batch")
    parser_batch.add_argument("--experiments", nargs="+", help="Experiment names to run")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if args.command == "export-results":
        success = export_experiment_results(args)
    elif args.command == "generate-config":
        success = generate_config_template(args)
    elif args.command == "compare-experiments":
        success = compare_experiments(args)
    elif args.command == "batch-run":
        success = batch_run_experiments(args)
    else:
        parser.print_help()
        success = False
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
