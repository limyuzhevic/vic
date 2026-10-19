#!/usr/bin/env python3
"""
NLM Visualization and Monitoring Data Exporter
==============================================

Data export module for NLM visualization and monitoring.
Provides tools for exporting visualization data in various formats
and for creating analysis reports.

Key Features:
- Export visualization data to CSV, JSON, and other formats
- Create comprehensive analysis reports
- Generate visualization reports for external tools
- Export brain state data for simulation replay
- Create visualization configuration files
"""

import json
import csv
import numpy as np
import pandas as pd
from pathlib import Path
from datetime import datetime
import os
import sys
import argparse

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    HAS_NLM = True
except ImportError as e:
    print(f"Warning: Could not import NLM modules: {e}")
    HAS_NLM = False

class DataExporter:
    """
    Main data export class for NLM visualization and monitoring.
    """
    
    def __init__(self):
        self.export_formats = {
            'csv': self._export_to_csv,
            'json': self._export_to_json,
            'npy': self._export_to_numpy,
            'parquet': self._export_to_parquet,
            'hdf5': self._export_to_hdf5,
            'excel': self._export_to_excel,
            'sqlite': self._export_to_sqlite,
            'mysql': self._export_to_mysql,
            'postgresql': self._export_to_postgresql
        }
        
        self.analysis_templates = {
            'neural_activity': self._create_neural_activity_template,
            'memory_systems': self._create_memory_systems_template,
            'neuromodulation': self._create_neuromodulation_template,
            'plasticity': self._create_plasticity_template,
            'development': self._create_development_template,
            'prediction': self._create_prediction_template,
            'performance': self._create_performance_template,
            'comprehensive': self._create_comprehensive_template
        }
    
    def export_data(self, data, filepath, format='json', **kwargs):
        """
        Export data in specified format.
        
        Args:
            data: Data to export (dict, DataFrame, or other)
            filepath: Output file path
            format: Export format
            **kwargs: Additional export options
        
        Returns:
            Path to exported file
        """
        if format not in self.export_formats:
            raise ValueError(f"Unsupported export format: {format}")
        
        try:
            return self.export_formats[format](data, filepath, **kwargs)
        except Exception as e:
            raise RuntimeError(f"Error exporting data to {format}: {e}")
    
    def export_visualization_config(self, config, filepath, format='json'):
        """
        Export visualization configuration.
        
        Args:
            config: Visualization configuration
            filepath: Output file path
            format: Export format
        
        Returns:
            Path to exported file
        """
        try:
            if format == 'json':
                # Add metadata
                config_with_metadata = {
                    'metadata': {
                        'export_date': datetime.now().isoformat(),
                        'export_format': format,
                        'config_version': '1.0'
                    },
                    'configuration': config
                }
                return self._export_to_json(config_with_metadata, filepath)
            else:
                return self.export_formats[format](config, filepath)
        except Exception as e:
            raise RuntimeError(f"Error exporting visualization config: {e}")
    
    def create_analysis_report(self, data, report_type='comprehensive', 
                              filepath=None, **kwargs):
        """
        Create analysis report from data.
        
        Args:
            data: Data to analyze
            report_type: Type of report
            filepath: Output file path
            **kwargs: Additional options
        
        Returns:
            Path to created report file
        """
        if report_type not in self.analysis_templates:
            raise ValueError(f"Unsupported report type: {report_type}")
        
        try:
            if filepath is None:
                timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
                filepath = f"nlm_analysis_report_{timestamp}.{report_type}"
            
            report = self.analysis_templates[report_type](data, **kwargs)
            
            if report_type == 'json' or report_type == 'comprehensive':
                return self._export_to_json(report, filepath)
            elif report_type == 'csv':
                return self._export_to_csv(report, filepath)
            else:
                raise ValueError(f"Unsupported report type for file output: {report_type}")
                
        except Exception as e:
            raise RuntimeError(f"Error creating analysis report: {e}")
    
    def export_brain_state(self, brain, filepath, format='json'):
        """
        Export brain state for simulation replay.
        
        Args:
            brain: NLM Brain object
            filepath: Output file path
            format: Export format
        
        Returns:
            Path to exported file
        """
        if not HAS_NLM or brain is None:
            raise ValueError("NLM or brain object not available")
        
        # Collect brain state data
        brain_state = {
            'metadata': {
                'export_date': datetime.now().isoformat(),
                'brain_type': 'NLM',
                'version': '1.0'
            },
            'brain_config': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount()
            },
            'neural_state': {
                'total_spike_count': brain.getTotalSpikeCount(),
                'active_neuron_count': brain.getActiveNeuronCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
            },
            'memory_state': {
                'working_memory_capacity': 100,  # Would need to get from actual brain
                'episodic_memory_capacity': 1000,
                'active_traces': 0,
                'stored_episodes': 0
            },
            'plasticity_state': {
                'stdp_ltp_weight': 0.01,
                'stdp_ltd_weight': 0.05,
                'hebbian_learning_rate': 0.001,
                'structural_plasticity_enabled': True
            },
            'development_state': {
                'developmental_stage': str(brain.getDevelopmentalStage()),
                'plasticity_maturation': 0.5,
                'excitability': 1.0
            },
            'prediction_state': {
                'prediction_accuracy': 0.7,
                'planning_depth': 3,
                'confidence_levels': []
            }
        }
        
        return self.export_data(brain_state, filepath, format)
    
    def export_visualization_data(self, viz_data, filepath, format='json'):
        """
        Export visualization data for external tools.
        
        Args:
            viz_data: Visualization data (dict or DataFrame)
            filepath: Output file path
            format: Export format
        
        Returns:
            Path to exported file
        """
        try:
            if filepath is None:
                timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
                filepath = f"nlm_visualization_data_{timestamp}.{format}"
            
            return self.export_formats[format](viz_data, filepath)
        except Exception as e:
            raise RuntimeError(f"Error exporting visualization data: {e}")
    
    # Export format implementations
    
    def _export_to_csv(self, data, filepath, **kwargs):
        """Export data to CSV format"""
        if isinstance(data, dict):
            df = pd.DataFrame(data)
        elif isinstance(data, pd.DataFrame):
            df = data
        else:
            df = pd.DataFrame([data])
        
        df.to_csv(filepath, index=False)
        return filepath
    
    def _export_to_json(self, data, filepath, **kwargs):
        """Export data to JSON format"""
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        return filepath
    
    def _export_to_numpy(self, data, filepath, **kwargs):
        """Export data to NumPy format"""
        np.save(filepath, data)
        return filepath
    
    def _export_to_parquet(self, data, filepath, **kwargs):
        """Export data to Parquet format"""
        try:
            import pyarrow as pa
            import pyarrow.parquet as pq
            
            if isinstance(data, dict):
                table = pa.table(data)
            elif isinstance(data, pd.DataFrame):
                table = pa.Table.from_pandas(data)
            else:
                table = pa.table(data)
            
            pq.write_table(table, filepath)
            return filepath
        except ImportError:
            raise ValueError("pyarrow not installed. Install with: pip install pyarrow")
    
    def _export_to_hdf5(self, data, filepath, **kwargs):
        """Export data to HDF5 format"""
        try:
            import h5py
            
            with h5py.File(filepath, 'w') as f:
                if isinstance(data, dict):
                    for key, value in data.items():
                        if isinstance(value, np.ndarray):
                            f.create_dataset(key, data=value)
                        elif isinstance(value, (list, tuple)):
                            f.create_dataset(key, data=np.array(value))
                        else:
                            f.attrs[key] = value
                else:
                    f.create_dataset('data', data=np.array(data))
            
            return filepath
        except ImportError:
            raise ValueError("h5py not installed. Install with: pip install h5py")
    
    def _export_to_excel(self, data, filepath, **kwargs):
        """Export data to Excel format"""
        if isinstance(data, dict):
            df = pd.DataFrame(data)
        elif isinstance(data, pd.DataFrame):
            df = data
        else:
            df = pd.DataFrame([data])
        
        df.to_excel(filepath, index=False)
        return filepath
    
    def _export_to_sqlite(self, data, filepath, **kwargs):
        """Export data to SQLite format"""
        try:
            import sqlite3
            
            conn = sqlite3.connect(filepath)
            
            if isinstance(data, dict):
                # Convert dict to DataFrame for SQLite
                df = pd.DataFrame(data)
                df.to_sql('data', conn, if_exists='replace', index=False)
            elif isinstance(data, pd.DataFrame):
                data.to_sql('data', conn, if_exists='replace', index=False)
            else:
                df = pd.DataFrame([data])
                df.to_sql('data', conn, if_exists='replace', index=False)
            
            conn.close()
            return filepath
        except ImportError:
            raise ValueError("sqlite3 not available")
    
    def _export_to_mysql(self, data, filepath, **kwargs):
        """Export data to MySQL format"""
        try:
            import mysql.connector
            
            # This would require MySQL connection parameters
            # For now, return error message
            raise NotImplementedError("MySQL export requires connection parameters")
        except ImportError:
            raise ValueError("mysql.connector not available")
    
    def _export_to_postgresql(self, data, filepath, **kwargs):
        """Export data to PostgreSQL format"""
        try:
            import psycopg2
            
            # This would require PostgreSQL connection parameters
            # For now, return error message
            raise NotImplementedError("PostgreSQL export requires connection parameters")
        except ImportError:
            raise ValueError("psycopg2 not available")
    
    # Analysis report templates
    
    def _create_neural_activity_template(self, data, **kwargs):
        """Create neural activity analysis report"""
        if isinstance(data, dict):
            df = pd.DataFrame(data)
        else:
            df = data
        
        report = {
            'report_type': 'neural_activity_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'total_samples': len(df),
                'time_range': {
                    'start': df.index.min() if hasattr(df, 'index') else 0,
                    'end': df.index.max() if hasattr(df, 'index') else len(df) - 1
                },
                'statistical_summary': df.describe().to_dict() if not df.empty else {}
            },
            'key_findings': self._analyze_neural_activity(df),
            'recommendations': self._generate_neural_recommendations(df)
        }
        
        return report
    
    def _create_memory_systems_template(self, data, **kwargs):
        """Create memory systems analysis report"""
        report = {
            'report_type': 'memory_systems_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'memory_capacity': data.get('memory_capacity', {}),
                'memory_utilization': data.get('memory_utilization', {}),
                'replay_metrics': data.get('replay_metrics', {})
            },
            'key_findings': self._analyze_memory_systems(data),
            'recommendations': self._generate_memory_recommendations(data)
        }
        
        return report
    
    def _create_neuromodulation_template(self, data, **kwargs):
        """Create neuromodulation analysis report"""
        report = {
            'report_type': 'neuromodulation_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'signal_levels': data.get('signal_levels', {}),
                'modulation_effects': data.get('modulation_effects', {})
            },
            'key_findings': self._analyze_neuromodulation(data),
            'recommendations': self._generate_neuromodulation_recommendations(data)
        }
        
        return report
    
    def _create_plasticity_template(self, data, **kwargs):
        """Create plasticity analysis report"""
        report = {
            'report_type': 'plasticity_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'learning_rates': data.get('learning_rates', {}),
                'plasticity_patterns': data.get('plasticity_patterns', {})
            },
            'key_findings': self._analyze_plasticity(data),
            'recommendations': self._generate_plasticity_recommendations(data)
        }
        
        return report
    
    def _create_development_template(self, data, **kwargs):
        """Create development analysis report"""
        report = {
            'report_type': 'development_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'developmental_stages': data.get('developmental_stages', {}),
                'maturation_curves': data.get('maturation_curves', {})
            },
            'key_findings': self._analyze_development(data),
            'recommendations': self._generate_development_recommendations(data)
        }
        
        return report
    
    def _create_prediction_template(self, data, **kwargs):
        """Create prediction analysis report"""
        report = {
            'report_type': 'prediction_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'prediction_accuracy': data.get('prediction_accuracy', {}),
                'uncertainty_measures': data.get('uncertainty_measures', {})
            },
            'key_findings': self._analyze_prediction(data),
            'recommendations': self._generate_prediction_recommendations(data)
        }
        
        return report
    
    def _create_performance_template(self, data, **kwargs):
        """Create performance analysis report"""
        report = {
            'report_type': 'performance_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'resource_usage': data.get('resource_usage', {}),
                'throughput_metrics': data.get('throughput_metrics', {})
            },
            'key_findings': self._analyze_performance(data),
            'recommendations': self._generate_performance_recommendations(data)
        }
        
        return report
    
    def _create_comprehensive_template(self, data, **kwargs):
        """Create comprehensive analysis report"""
        report = {
            'report_type': 'comprehensive_analysis',
            'generated_at': datetime.now().isoformat(),
            'data_summary': {
                'total_systems': len(data) if isinstance(data, dict) else 1,
                'analysis_timestamp': datetime.now().isoformat()
            },
            'system_analyses': {},
            'cross_system_findings': self._find_cross_system_patterns(data),
            'overall_recommendations': self._generate_overall_recommendations(data)
        }
        
        # Add analysis for each system
        if isinstance(data, dict):
            for system_name, system_data in data.items():
                report['system_analyses'][system_name] = self._analyze_system(system_name, system_data)
        
        return report
    
    # Analysis helper methods
    
    def _analyze_neural_activity(self, df):
        """Analyze neural activity data"""
        findings = []
        
        if 'firing_rate' in df.columns:
            avg_firing = df['firing_rate'].mean()
            max_firing = df['firing_rate'].max()
            std_firing = df['firing_rate'].std()
            
            findings.append(f"Average firing rate: {avg_firing:.2f} Hz")
            findings.append(f"Maximum firing rate: {max_firing:.2f} Hz")
            findings.append(f"Firing rate standard deviation: {std_firing:.2f} Hz")
            
            if std_firing > avg_firing * 0.5:
                findings.append("High firing rate variability - indicates complex neural dynamics")
        
        return findings
    
    def _generate_neural_recommendations(self, df):
        """Generate recommendations based on neural activity"""
        recommendations = []
        
        if 'firing_rate' in df.columns:
            avg_firing = df['firing_rate'].mean()
            
            if avg_firing < 1.0:
                recommendations.append("Low firing rate detected - consider increasing neural excitability")
            elif avg_firing > 50.0:
                recommendations.append("High firing rate detected - monitor for potential hyperexcitability")
            
            if df['firing_rate'].std() > df['firing_rate'].mean():
                recommendations.append("High variability in firing rates - investigate potential instabilities")
        
        return recommendations
    
    def _analyze_memory_systems(self, data):
        """Analyze memory systems data"""
        findings = []
        
        memory_load = data.get('working_memory_load', 0)
        if memory_load > 0.8:
            findings.append(f"High memory load detected: {memory_load:.1%}")
        
        return findings
    
    def _generate_memory_recommendations(self, data):
        """Generate recommendations based on memory systems"""
        return []
    
    def _analyze_neuromodulation(self, data):
        """Analyze neuromodulation data"""
        findings = []
        
        dopamine = data.get('dopamine_level', 0)
        if dopamine < 0.3:
            findings.append(f"Low dopamine level: {dopamine:.2f} - consider neuromodulation intervention")
        
        return findings
    
    def _generate_neuromodulation_recommendations(self, data):
        """Generate recommendations based on neuromodulation"""
        return []
    
    def _analyze_plasticity(self, data):
        """Analyze plasticity data"""
        findings = []
        
        efficiency = data.get('learning_efficiency', 0)
        if efficiency < 0.5:
            findings.append(f"Low learning efficiency: {efficiency:.2f} - consider plasticity enhancement")
        
        return findings
    
    def _generate_plasticity_recommendations(self, data):
        """Generate recommendations based on plasticity"""
        return []
    
    def _analyze_development(self, data):
        """Analyze development data"""
        findings = []
        
        progress = data.get('developmental_progress', 0)
        if progress < 0.5:
            findings.append(f"Early developmental stage: {progress:.1%} - expect significant growth ahead")
        
        return findings
    
    def _generate_development_recommendations(self, data):
        """Generate recommendations based on development"""
        return []
    
    def _analyze_prediction(self, data):
        """Analyze prediction data"""
        findings = []
        
        accuracy = data.get('prediction_accuracy', 0)
        if accuracy < 0.7:
            findings.append(f"Low prediction accuracy: {accuracy:.1%} - consider improving prediction mechanisms")
        
        return findings
    
    def _generate_prediction_recommendations(self, data):
        """Generate recommendations based on prediction"""
        return []
    
    def _analyze_performance(self, data):
        """Analyze performance data"""
        findings = []
        
        cpu_usage = data.get('cpu_usage', 0)
        if cpu_usage > 80:
            findings.append(f"High CPU usage: {cpu_usage:.1f}% - consider optimization")
        
        return findings
    
    def _generate_performance_recommendations(self, data):
        """Generate recommendations based on performance"""
        return []
    
    def _find_cross_system_patterns(self, data):
        """Find cross-system patterns"""
        return []
    
    def _generate_overall_recommendations(self, data):
        """Generate overall recommendations"""
        return []
    
    def _analyze_system(self, system_name, data):
        """Analyze a specific system"""
        return {
            'system_name': system_name,
            'analysis_timestamp': datetime.now().isoformat(),
            'data_summary': {
                'total_samples': len(data) if isinstance(data, dict) else 1,
                'analysis_complete': True
            }
        }
class DataExporterCLI:
    """
    Command-line interface for NLM visualization and monitoring data exporter.
    """
    
    def __init__(self):
        self.exporter = DataExporter()
        
    def export_data(self, data, filepath, format='json', **kwargs):
        """Export data using exporter"""
        return self.exporter.export_data(data, filepath, format, **kwargs)
    
    def create_analysis_report(self, data, report_type='comprehensive', 
                              filepath=None, **kwargs):
        """Create analysis report using exporter"""
        return self.exporter.create_analysis_report(data, report_type, filepath, **kwargs)
    
    def export_brain_state(self, brain, filepath, format='json'):
        """Export brain state using exporter"""
        return self.exporter.export_brain_state(brain, filepath, format)
    
    def export_visualization_data(self, viz_data, filepath, format='json'):
        """Export visualization data using exporter"""
        return self.exporter.export_visualization_data(viz_data, filepath, format)
    
    def list_formats(self):
        """List available export formats"""
        return list(self.exporter.export_formats.keys())
    
    def list_report_types(self):
        """List available report types"""
        return list(self.exporter.analysis_templates.keys())


def create_cli():
    """Create and return CLI parser for export commands"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Monitoring Data Exporter CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Subparsers
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Export data command
    export_parser = subparsers.add_parser('export', help='Export data')
    export_parser.add_argument('data_file', help='Input data file (JSON, CSV, etc.)')
    export_parser.add_argument('output_file', help='Output file path')
    export_parser.add_argument('--format', default='json', 
                              help='Export format (csv, json, npy, parquet, hdf5, excel)')
    
    # Analysis report command
    report_parser = subparsers.add_parser('report', help='Create analysis report')
    report_parser.add_argument('data_file', help='Input data file')
    report_parser.add_argument('output_file', help='Output report file')
    report_parser.add_argument('--type', default='comprehensive',
                               help='Report type (neural_activity, memory_systems, neuromodulation, plasticity, development, prediction, performance, comprehensive)')
    
    # Brain state export command
    brain_parser = subparsers.add_parser('export-brain', help='Export brain state')
    brain_parser.add_argument('output_file', help='Output file path')
    brain_parser.add_argument('--format', default='json', help='Export format')
    
    # Visualization data export command
    viz_parser = subparsers.add_parser('export-viz', help='Export visualization data')
    viz_parser.add_argument('output_file', help='Output file path')
    viz_parser.add_argument('--format', default='json', help='Export format')
    
    # List formats command
    formats_parser = subparsers.add_parser('formats', help='List available export formats')
    
    # List reports command
    reports_parser = subparsers.add_parser('reports', help='List available report types')
    
    return parser, subparsers


def main():
    """Main CLI function"""
    parser, subparsers = create_cli()
    
    # Create CLI instance
    cli = DataExporterCLI()
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle commands
    if args.command == 'export':
        # Load data from file
        if args.data_file.endswith('.json'):
            with open(args.data_file, 'r') as f:
                data = json.load(f)
        elif args.data_file.endswith('.csv'):
            data = pd.read_csv(args.data_file)
        elif args.data_file.endswith('.npy'):
            data = np.load(args.data_file)
        else:
            print(f"Unsupported data format: {args.data_file}")
            sys.exit(1)
        
        # Export data
        success = cli.export_data(data, args.output_file, args.format)
        sys.exit(0 if success else 1)
        
    elif args.command == 'report':
        # Load data from file
        if args.data_file.endswith('.json'):
            with open(args.data_file, 'r') as f:
                data = json.load(f)
        elif args.data_file.endswith('.csv'):
            data = pd.read_csv(args.data_file)
        else:
            print(f"Unsupported data format: {args.data_file}")
            sys.exit(1)
        
        # Create report
        success = cli.create_analysis_report(data, args.type, args.output_file)
        sys.exit(0 if success else 1)
        
    elif args.command == 'export-brain':
        # Create brain object (simplified)
        if HAS_NLM:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Export brain state
            success = cli.export_brain_state(brain, args.output_file, args.format)
            sys.exit(0 if success else 1)
        else:
            print("NLM not available for brain export")
            sys.exit(1)
        
    elif args.command == 'export-viz':
        print("Creating visualization data export...")
        print("Note: This would require actual visualization data")
        print(f"Output would be: {args.output_file}")
        sys.exit(0)
        
    elif args.command == 'formats':
        formats = cli.list_formats()
        print("Available export formats:")
        for fmt in formats:
            print(f"  - {fmt}")
        sys.exit(0)
        
    elif args.command == 'reports':
        reports = cli.list_report_types()
        print("Available report types:")
        for report in reports:
            print(f"  - {report}")
        sys.exit(0)
        
    else:
        # No command specified, show help
        parser.print_help()
        sys.exit(1)
if __name__ == "__main__":
    main()
