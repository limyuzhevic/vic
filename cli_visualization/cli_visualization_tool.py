#!/usr/bin/env python3
"""
CLI Visualization Tool for NLM System

This script provides command-line visualization capabilities for the Neural Learning Model (NLM) system.
It allows users to generate visualizations, analyze data, and export results via the command line.
"""

import argparse
import json
import sys
import os
from datetime import datetime
from typing import Dict, List, Any, Optional

# Add the parent directory to the Python path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    from src.visualization.NVMVisualization import NVMVisualization
    from src.analysis.PerformanceAnalyzer import PerformanceAnalyzer
    from src.analysis.LearningAnalyzer import LearningAnalyzer
    from src.analysis.NetworkAnalyzer import NetworkAnalyzer
    from src.analysis.SystemAnalyzer import SystemAnalyzer
except ImportError:
    print("Error: Could not import NLM modules. Please ensure the Python path is correctly set.")
    sys.exit(1)

class CLIVisualizationTool:
    def __init__(self):
        self.visualization = None
        self.performance_analyzer = None
        self.learning_analyzer = None
        self.network_analyzer = None
        self.system_analyzer = None
    
    def initialize(self):
        """Initialize the visualization tools."""
        try:
            # Initialize visualization
            self.visualization = NVMVisualization()
            if not self.visualization.initialize():
                print("Warning: Failed to initialize visualization")
            
            # Initialize analyzers
            self.performance_analyzer = PerformanceAnalyzer()
            self.learning_analyzer = LearningAnalyzer()
            self.network_analyzer = NetworkAnalyzer()
            self.system_analyzer = SystemAnalyzer()
            
            print("NLM Visualization Tool initialized successfully")
            return True
        except Exception as e:
            print(f"Error initializing NLM Visualization Tool: {e}")
            return False
    
    def generate_visualization(self, output_file: str, format: str = "json"):
        """Generate visualization data."""
        try:
            if not self.visualization:
                print("Error: Visualization not initialized")
                return False
            
            success = self.visualization.exportData(output_file, format)
            if success:
                print(f"Visualization data exported to {output_file}")
            else:
                print(f"Error: Failed to export visualization data to {output_file}")
            
            return success
        except Exception as e:
            print(f"Error generating visualization: {e}")
            return False
    
    def analyze_performance(self, data_file: str, output_file: str = None):
        """Analyze performance data."""
        try:
            if not self.performance_analyzer:
                print("Error: Performance analyzer not initialized")
                return False
            
            # Load performance data
            with open(data_file, 'r') as f:
                data = json.load(f)
            
            # Create performance data object
            from src.analysis.PerformanceAnalyzer import PerformanceData
            performance_data = PerformanceData(
                metrics=data.get('metrics', []),
                timestamps=data.get('timestamps', []),
                session_id=data.get('session_id', ''),
                benchmark_id=data.get('benchmark_id', '')
            )
            
            # Analyze performance
            self.performance_analyzer.addData(performance_data)
            
            # Generate analysis report
            report = self.performance_analyzer.generateReport()
            
            if output_file:
                with open(output_file, 'w') as f:
                    f.write(report)
                print(f"Performance analysis report written to {output_file}")
            else:
                print("Performance analysis report:")
                print(report)
            
            return True
        except Exception as e:
            print(f"Error analyzing performance: {e}")
            return False
    
    def analyze_learning(self, data_file: str, output_file: str = None):
        """Analyze learning data."""
        try:
            if not self.learning_analyzer:
                print("Error: Learning analyzer not initialized")
                return False
            
            # Load learning data
            with open(data_file, 'r') as f:
                data = json.load(f)
            
            # Create learning data object
            from src.analysis.LearningAnalyzer import LearningData
            learning_data = LearningData(
                metrics=data.get('metrics', []),
                timestamps=data.get('timestamps', []),
                session_id=data.get('session_id', ''),
                task_id=data.get('task_id', '')
            )
            
            # Analyze learning
            self.learning_analyzer.addData(learning_data)
            
            # Generate analysis report
            report = self.learning_analyzer.generateReport()
            
            if output_file:
                with open(output_file, 'w') as f:
                    f.write(report)
                print(f"Learning analysis report written to {output_file}")
            else:
                print("Learning analysis report:")
                print(report)
            
            return True
        except Exception as e:
            print(f"Error analyzing learning: {e}")
            return False
    
    def analyze_network(self, data_file: str, output_file: str = None):
        """Analyze network data."""
        try:
            if not self.network_analyzer:
                print("Error: Network analyzer not initialized")
                return False
            
            # Load network data
            with open(data_file, 'r') as f:
                data = json.load(f)
            
            # Create network data object
            from src.analysis.NetworkAnalyzer import NetworkData
            network_data = NetworkData(
                activities=data.get('activities', []),
                timestamps=data.get('timestamps', []),
                session_id=data.get('session_id', ''),
                network_id=data.get('network_id', '')
            )
            
            # Analyze network
            self.network_analyzer.addData(network_data)
            
            # Generate analysis report
            report = self.network_analyzer.generateReport()
            
            if output_file:
                with open(output_file, 'w') as f:
                    f.write(report)
                print(f"Network analysis report written to {output_file}")
            else:
                print("Network analysis report:")
                print(report)
            
            return True
        except Exception as e:
            print(f"Error analyzing network: {e}")
            return False
    
    def analyze_system(self, data_file: str, output_file: str = None):
        """Analyze system data."""
        try:
            if not self.system_analyzer:
                print("Error: System analyzer not initialized")
                return False
            
            # Load system data
            with open(data_file, 'r') as f:
                data = json.load(f)
            
            # Create system data object
            from src.analysis.SystemAnalyzer import SystemData
            system_data = SystemData(
                states=data.get('states', []),
                timestamps=data.get('timestamps', []),
                session_id=data.get('session_id', '')
            )
            
            # Analyze system
            self.system_analyzer.addData(system_data)
            
            # Generate analysis report
            report = self.system_analyzer.generateReport()
            
            if output_file:
                with open(output_file, 'w') as f:
                    f.write(report)
                print(f"System analysis report written to {output_file}")
            else:
                print("System analysis report:")
                print(report)
            
            return True
        except Exception as e:
            print(f"Error analyzing system: {e}")
            return False
    
    def create_template(self, name: str, description: str, data_file: str, output_file: str):
        """Create a visualization template."""
        try:
            from src.visualization.TemplateSystem import VisualizationTemplate
            
            # Load template data
            with open(data_file, 'r') as f:
                data = json.load(f)
            
            # Create template
            template = VisualizationTemplate(
                name=name,
                description=description,
                author='CLI User',
                version='1.0.0',
                data=data,
                parameters={},
                category='custom',
                isDefault=False,
                created=datetime.now().isoformat(),
                modified=datetime.now().isoformat(),
                metadata={'created_by': 'CLI', 'created_at': datetime.now().isoformat()}
            )
            
            # Save template
            if self.save_template(template, output_file):
                print(f"Template '{name}' created and saved to {output_file}")
                return True
            else:
                print(f"Error: Failed to save template to {output_file}")
                return False
        except Exception as e:
            print(f"Error creating template: {e}")
            return False
    
    def load_template(self, filename: str):
        """Load a visualization template."""
        try:
            from src.visualization.TemplateSystem import VisualizationTemplate
            
            # Load template
            if self.load_template_from_file(filename, template):
                print(f"Template '{template.name}' loaded from {filename}")
                return template
            else:
                print(f"Error: Failed to load template from {filename}")
                return None
        except Exception as e:
            print(f"Error loading template: {e}")
            return None
    
    def save_template(self, template, filename: str):
        """Save a visualization template."""
        try:
            from src.visualization.TemplateSystem import VisualizationTemplate
            
            # Save template to file
            if self.save_template_to_file(template, filename):
                return True
            else:
                print(f"Error: Failed to save template to {filename}")
                return False
        except Exception as e:
            print(f"Error saving template: {e}")
            return False
    
    def show_help(self):
        """Show help information."""
        help_text = """
NLM Visualization Tool

Usage: python cli_visualization_tool.py <command> [options]

Commands:
    init                    Initialize the visualization tool
    visualize <output_file>  Generate visualization data
    analyze performance <data_file> [output_file]  Analyze performance data
    analyze learning <data_file> [output_file]      Analyze learning data
    analyze network <data_file> [output_file]       Analyze network data
    analyze system <data_file> [output_file]        Analyze system data
    create template <name> <description> <data_file> <output_file>  Create a visualization template
    load template <filename>                                   Load a visualization template
    save template <template_file> <output_file>                Save a visualization template
    list templates                                           List all available templates
    help                                                     Show this help message

Options:
    --format FORMAT           Output format (default: json)
    --update-frequency FREQ    Update frequency in Hz (default: 60)
    --theme THEME             Theme (default: default)
    --quality QUALITY         Quality (default: high)

Examples:
    python cli_visualization_tool.py init
    python cli_visualization_tool.py visualize output.json --format json
    python cli_visualization_tool.py analyze performance perf_data.json perf_report.txt
    python cli_visualization_tool.py analyze learning learn_data.json learn_report.txt
    python cli_visualization_tool.py create template "My Template" "A custom template" template_data.json my_template.json
"""
        print(help_text)
    
    def list_templates(self):
        """List all available templates."""
        try:
            from src.visualization.TemplateSystem import TemplateSystem
            
            template_system = TemplateSystem()
            if template_system.loadTemplatesFromDirectory("templates"):
                templates = template_system.getTemplates()
                print(f"Found {len(templates)} templates:")
                for i, template in enumerate(templates, 1):
                    print(f"{i}. {template.name} ({template.category}) - {template.description}")
                    print(f"   Author: {template.author}")
                    print(f"   Version: {template.version}")
                    print(f"   Created: {template.created}")
                    print(f"   Modified: {template.modified}")
                    print()
            else:
                print("Error: Failed to load templates")
        except Exception as e:
            print(f"Error listing templates: {e}")
    
    def run_command(self, args):
        """Run a command."""
        if len(args) < 1:
            self.show_help()
            return False
        
        command = args[0]
        
        try:
            if command == "init":
                return self.initialize()
            elif command == "visualize":
                if len(args) < 2:
                    print("Error: Missing output file")
                    return False
                output_file = args[1]
                format = "json"
                if "--format" in args:
                    format_index = args.index("--format") + 1
                    if format_index < len(args):
                        format = args[format_index]
                return self.generate_visualization(output_file, format)
            elif command == "analyze":
                if len(args) < 2:
                    print("Error: Missing analysis type")
                    return False
                analysis_type = args[1]
                if len(args) < 3:
                    print("Error: Missing data file")
                    return False
                data_file = args[2]
                output_file = None
                if len(args) > 3:
                    output_file = args[3]
                
                if analysis_type == "performance":
                    return self.analyze_performance(data_file, output_file)
                elif analysis_type == "learning":
                    return self.analyze_learning(data_file, output_file)
                elif analysis_type == "network":
                    return self.analyze_network(data_file, output_file)
                elif analysis_type == "system":
                    return self.analyze_system(data_file, output_file)
                else:
                    print(f"Error: Unknown analysis type: {analysis_type}")
                    return False
            elif command == "create":
                if len(args) < 4:
                    print("Error: Missing parameters for create template")
                    return False
                if args[1] != "template":
                    print("Error: Invalid command for create")
                    return False
                name = args[2]
                description = args[3]
                data_file = args[4]
                output_file = args[5]
                return self.create_template(name, description, data_file, output_file)
            elif command == "load":
                if len(args) < 2:
                    print("Error: Missing filename")
                    return False
                if args[1] != "template":
                    print("Error: Invalid command for load")
                    return False
                filename = args[2]
                template = self.load_template(filename)
                return template is not None
            elif command == "save":
                if len(args) < 3:
                    print("Error: Missing parameters for save template")
                    return False
                if args[1] != "template":
                    print("Error: Invalid command for save")
                    return False
                template_file = args[2]
                output_file = args[3]
                from src.visualization.TemplateSystem import VisualizationTemplate
                template = VisualizationTemplate()
                if self.load_template_from_file(template_file, template):
                    return self.save_template(template, output_file)
                return False
            elif command == "list":
                if len(args) < 2:
                    print("Error: Missing parameter for list")
                    return False
                if args[1] != "templates":
                    print("Error: Invalid command for list")
                    return False
                return self.list_templates()
            elif command == "help":
                self.show_help()
                return True
            else:
                print(f"Error: Unknown command: {command}")
                return False
        except Exception as e:
            print(f"Error executing command: {e}")
            return False

    # Helper methods for loading/saving templates (simplified implementations)
    def load_template_from_file(self, filename: str, template):
        """Load template from file."""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
            
            template.name = data.get('name', '')
            template.description = data.get('description', '')
            template.author = data.get('author', '')
            template.version = data.get('version', '1.0.0')
            template.data = data.get('data', {})
            template.parameters = data.get('parameters', {})
            template.category = data.get('category', 'custom')
            template.isDefault = data.get('isDefault', False)
            template.created = data.get('created', '')
            template.modified = data.get('modified', '')
            template.metadata = data.get('metadata', {})
            
            return True
        except Exception as e:
            print(f"Error loading template from file: {e}")
            return False
    
    def save_template_to_file(self, template, filename: str):
        """Save template to file."""
        try:
            template_data = {
                'name': template.name,
                'description': template.description,
                'author': template.author,
                'version': template.version,
                'data': template.data,
                'parameters': template.parameters,
                'category': template.category,
                'isDefault': template.isDefault,
                'created': template.created,
                'modified': template.modified,
                'metadata': template.metadata
            }
            
            with open(filename, 'w') as f:
                json.dump(template_data, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving template to file: {e}")
            return False

def main():
    parser = argparse.ArgumentParser(description="NLM Visualization Tool")
    parser.add_argument("command", help="Command to execute")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Additional arguments")
    
    args = parser.parse_args()
    
    tool = CLIVisualizationTool()
    
    # Combine the command and additional arguments
    command_args = [args.command] + args.args
    
    # Parse options
    parsed_args = []
    i = 0
    while i < len(command_args):
        if command_args[i].startswith("--"):
            parsed_args.append(command_args[i])
            if i + 1 < len(command_args) and not command_args[i + 1].startswith("--"):
                parsed_args.append(command_args[i + 1])
                i += 1
        else:
            parsed_args.append(command_args[i])
        i += 1
    
    success = tool.run_command(parsed_args)
    
    if success:
        print("Command executed successfully")
        sys.exit(0)
    else:
        print("Command execution failed")
        sys.exit(1)

if __name__ == "__main__":
    main()
