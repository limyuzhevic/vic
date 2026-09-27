"""
Export utilities for NLM CLI
"""

import json
import yaml
import numpy as np
from pathlib import Path
from typing import Dict, Any, List, Optional
from datetime import datetime

class NLMCliExporter:
    def __init__(self):
        self.export_configs = {}
    
    def export_simulation_results(self, results: Dict[str, Any], output_file: str, 
                                format: str = "json", include_metadata: bool = True) -> bool:
        """Export simulation results to file"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "results": results
            }
            
            if include_metadata:
                export_data["metadata"] = {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_export",
                    "version": "0.1.0"
                }
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Simulation results exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting simulation results: {e}")
            return False
    
    def export_batch_results(self, results: List[Dict[str, Any]], output_file: str,
                           format: str = "json", generate_summary: bool = True) -> bool:
        """Export batch processing results to file"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "batch_results": results,
                "export_info": {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_batch_export",
                    "version": "0.1.0",
                    "total_batches": len(results)
                }
            }
            
            if generate_summary:
                summary = self._generate_batch_summary(results)
                export_data["summary"] = summary
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Batch results exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting batch results: {e}")
            return False
    
    def _generate_batch_summary(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate a summary of batch processing results"""
        if not results:
            return {"error": "No results to summarize"}
        
        total_files = len(results)
        successful = sum(1 for r in results if r.get("status") == "success")
        failed = total_files - successful
        
        summary = {
            "total_batches": total_files,
            "successful": successful,
            "failed": failed,
            "success_rate": successful / total_files if total_files > 0 else 0,
            "timestamp": datetime.now().isoformat()
        }
        
        # Add time statistics if available
        durations = [r.get("duration", 0) for r in results if isinstance(r.get("duration"), (int, float))]
        if durations:
            summary["average_duration"] = sum(durations) / len(durations)
            summary["min_duration"] = min(durations)
            summary["max_duration"] = max(durations)
        
        return summary
    
    def export_configurations(self, configurations: List[Dict[str, Any]], output_file: str,
                            format: str = "json", include_metadata: bool = True) -> bool:
        """Export configuration files"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "configurations": configurations,
                "export_info": {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_config_export",
                    "version": "0.1.0",
                    "total_configs": len(configurations)
                }
            }
            
            if include_metadata:
                export_data["export_info"]["generated_by"] = "NLM CLI"
                export_data["export_info"]["description"] = "Configuration export for NLM batch processing"
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Configurations exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting configurations: {e}")
            return False
    
    def export_presets(self, presets: List[Dict[str, Any]], output_file: str,
                      format: str = "json") -> bool:
        """Export preset configurations"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "presets": presets,
                "export_info": {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_preset_export",
                    "version": "0.1.0",
                    "total_presets": len(presets)
                }
            }
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Presets exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting presets: {e}")
            return False
    
    def export_comparisons(self, comparisons: List[Dict[str, Any]], output_file: str,
                          format: str = "json") -> bool:
        """Export comparison results"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "comparisons": comparisons,
                "export_info": {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_comparison_export",
                    "version": "0.1.0",
                    "total_comparisons": len(comparisons)
                }
            }
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Comparisons exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting comparisons: {e}")
            return False
    
    def export_validation_results(self, validation_results: Dict[str, Any], output_file: str,
                                format: str = "json") -> bool:
        """Export validation results"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            export_data = {
                "validation_results": validation_results,
                "export_info": {
                    "exported_at": datetime.now().isoformat(),
                    "format": "nlm_cli_validation_export",
                    "version": "0.1.0"
                }
            }
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Validation results exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting validation results: {e}")
            return False

# Create instance and export built-in presets
def export_builtin_presets(output_dir: str = "exported_presets"):
    """Export built-in presets to files"""
    from .presets import BuiltInPresets, NLMPresets
    
    presets_dir = Path(output_dir)
    presets_dir.mkdir(parents=True, exist_ok=True)
    
    builtin_presets = BuiltInPresets()
    exporter = NLMCliExporter()
    
    # Export each built-in preset
    preset_names = ['quick_test', 'research', 'minimal', 'development', 'performance']
    
    for name in preset_names:
        # Create config from built-in preset
        if name == 'quick_test':
            config = builtin_presets.get_quick_test_config()
        elif name == 'research':
            config = builtin_presets.get_research_config()
        elif name == 'minimal':
            config = builtin_presets.get_minimal_config()
        elif name == 'development':
            config = builtin_presets.get_development_config()
        elif name == 'performance':
            config = builtin_presets.get_performance_config()
        
        # Export to YAML file
        output_file = presets_dir / f"{name}.yaml"
        success = exporter.export_configurations([config], str(output_file), "yaml")
        
        if success:
            print(f"Exported built-in preset '{name}' to {output_file}")

# Initialize and export presets on module import
export_builtin_presets()
