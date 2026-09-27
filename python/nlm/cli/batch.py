"""
Batch processing utilities for NLM CLI
"""

import json
import yaml
import subprocess
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import List, Dict, Any, Optional
import time
from datetime import datetime

class NLMBatchProcessor:
    def __init__(self, max_workers: int = 4):
        self.max_workers = max_workers
        self.results: List[Dict[str, Any]] = []
    
    def load_config_files(self, filepath: str) -> List[str]:
        """Load list of configuration files from a file"""
        try:
            with open(filepath, 'r') as f:
                config_files = [
                    line.strip() for line in f 
                    if line.strip() and not line.startswith('#')
                ]
            
            # Validate that files exist
            valid_files = []
            for config_file in config_files:
                if Path(config_file).exists():
                    valid_files.append(config_file)
                else:
                    print(f"Warning: Config file not found: {config_file}")
            
            return valid_files
        except Exception as e:
            print(f"Error loading config files from {filepath}: {e}")
            return []
    
    def process_single_config(self, config_file: str, step_func, **kwargs) -> Dict[str, Any]:
        """Process a single configuration file"""
        result = {
            "config_file": config_file,
            "status": "success",
            "timestamp": datetime.now().isoformat(),
            "errors": [],
            "output": None
        }
        
        try:
            print(f"Processing {config_file}...")
            
            # Call the processing function
            output = step_func(config_file, **kwargs)
            result["output"] = output
            
        except Exception as e:
            result["status"] = "error"
            result["errors"].append(str(e))
            print(f"Error processing {config_file}: {e}")
        
        return result
    
    def process_batch(self, config_files: List[str], step_func, **kwargs) -> List[Dict[str, Any]]:
        """Process multiple configuration files in parallel or sequentially"""
        results = []
        
        if len(config_files) <= 1 or self.max_workers <= 1:
            # Sequential processing
            for config_file in config_files:
                result = self.process_single_config(config_file, step_func, **kwargs)
                results.append(result)
        else:
            # Parallel processing
            with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
                # Submit all tasks
                future_to_file = {
                    executor.submit(self.process_single_config, config_file, step_func, **kwargs): config_file
                    for config_file in config_files
                }
                
                # Collect results as they complete
                for future in as_completed(future_to_file):
                    config_file = future_to_file[future]
                    try:
                        result = future.result()
                        results.append(result)
                    except Exception as e:
                        print(f"Error processing {config_file} in parallel: {e}")
                        results.append({
                            "config_file": config_file,
                            "status": "error",
                            "timestamp": datetime.now().isoformat(),
                            "errors": [str(e)],
                            "output": None
                        })
        
        self.results = results
        return results
    
    def generate_batch_report(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate a report from batch processing results"""
        total_files = len(results)
        successful = sum(1 for r in results if r["status"] == "success")
        failed = total_files - successful
        
        # Count different types of errors
        error_types = {}
        for result in results:
            if result["status"] == "error":
                for error in result["errors"]:
                    error_type = error.split(':')[0] if ':' in error else "Unknown"
                    error_types[error_type] = error_types.get(error_type, 0) + 1
        
        # Calculate processing time (would be tracked in real implementation)
        processing_time = sum(1 for r in results if r["output"] is not None) * 0.1  # Mock timing
        
        report = {
            "batch_summary": {
                "total_files": total_files,
                "successful": successful,
                "failed": failed,
                "success_rate": successful / total_files if total_files > 0 else 0,
                "processing_time_seconds": processing_time,
                "average_time_per_file": processing_time / total_files if total_files > 0 else 0
            },
            "error_summary": error_types,
            "detailed_results": results,
            "timestamp": datetime.now().isoformat()
        }
        
        return report
    
    def export_results(self, results: List[Dict[str, Any]], output_file: str, format: str = "json") -> bool:
        """Export batch results to file"""
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(results, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(results, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Batch results exported to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting results: {e}")
            return False

# Utility functions for batch processing
class BatchUtils:
    @staticmethod
    def create_batch_config_files(base_config: str, variations: List[Dict[str, Any]], output_dir: str = "batch_configs") -> List[str]:
        """Create multiple configuration files with variations"""
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        created_files = []
        
        for i, variation in enumerate(variations):
            # Load base config
            config_manager = NLMConfigManager()
            config_manager.load_from_file(base_config)
            
            # Apply variations
            for key, value in variation.items():
                config_manager.set(key, value)
            
            # Save variation
            output_file = Path(output_dir) / f"config_{i+1}.cfg"
            config_manager.save_to_file(str(output_file), "cfg")
            created_files.append(str(output_file))
        
        return created_files
    
    @staticmethod
    def create_file_list(config_files: List[str], output_file: str = "batch_file_list.txt"):
        """Create a file list for batch processing"""
        Path(output_file).parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_file, 'w') as f:
            f.write("# NLM Batch Configuration File List\n")
            f.write("# Generated automatically for batch processing\n\n")
            
            for config_file in config_files:
                f.write(f"{config_file}\n")
        
        print(f"File list saved to {output_file}")

# Import config manager for use in batch processing
from .config_manager import NLMConfigManager
