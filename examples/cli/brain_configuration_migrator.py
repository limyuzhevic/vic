#!/usr/bin/env python3
"""
Brain Configuration Migrator Example
Migrate brain configurations between different formats and versions.
"""

import sys
import os
import argparse
from pathlib import Path
from typing import List, Dict, Any, Optional
import json
import yaml
import xml.etree.ElementTree as ET
from xml.dom import minidom
import csv
import re
import time
from datetime import datetime

class BrainConfigMigrator:
    def __init__(self, args):
        self.args = args
        self.migration_history = []
        
        # Migration statistics
        self.stats = {
            "files_processed": 0,
            "successful_migrations": 0,
            "failed_migrations": 0,
            "start_time": time.time(),
            "end_time": 0.0
        }
        
        # Migration configuration
        self.migration_config = {
            "source_format": self.args.source_format or self.detect_format(args.input_file),
            "target_format": self.args.target_format or "json",
            "preserve_structure": self.args.preserve_structure,
            "convert_metadata": self.args.convert_metadata,
            "validation_level": self.args.validation_level or "strict",
            "backup_files": self.args.backup_files,
            "log_migrations": self.args.log_migrations
        }
        
        # Create output directory
        if self.args.output_dir:
            Path(self.args.output_dir).mkdir(parents=True, exist_ok=True)
        
        print("Brain Configuration Migrator initialized")
        print(f"Source format: {self.migration_config['source_format']}")
        print(f"Target format: {self.migration_config['target_format']}")
        print(f"Input file: {args.input_file}")
        print(f"Output directory: {self.args.output_dir or 'current directory'}")
    
    def detect_format(self, file_path: str) -> str:
        """Detect file format from extension."""
        path = Path(file_path)
        ext = path.suffix.lower()
        
        format_map = {
            '.json': 'json',
            '.yaml': 'yaml',
            '.yml': 'yaml',
            '.xml': 'xml',
            '.csv': 'csv',
            '.txt': 'txt',
            '.ini': 'ini',
            '.cfg': 'cfg'
        }
        
        return format_map.get(ext, 'unknown')
    
    def log_migration(self, message: str):
        """Log migration activity."""
        timestamp = datetime.now().isoformat()
        log_entry = {
            "timestamp": timestamp,
            "message": message
        }
        self.migration_history.append(log_entry)
        
        if self.migration_config["log_migrations"]:
            print(f"[{timestamp}] {message}")
    
    def backup_file(self, source_path: str) -> Optional[str]:
        """Create a backup of the source file."""
        if not self.migration_config["backup_files"]:
            return None
        
        try:
            source_file = Path(source_path)
            backup_dir = source_file.parent / "backups"
            backup_dir.mkdir(exist_ok=True)
            
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            backup_path = backup_dir / f"{source_file.stem}_backup_{timestamp}{source_file.suffix}"
            
            import shutil
            shutil.copy2(source_path, backup_path)
            
            self.log_migration(f"Created backup: {backup_path}")
            return str(backup_path)
            
        except Exception as e:
            self.log_migration(f"Error creating backup: {e}")
            return None
    
    def migrate(self) -> bool:
        """Run the migration process."""
        try:
            print("Starting brain configuration migration...")
            
            # Validate input file
            if not self.validate_input():
                return False
            
            # Perform migration
            success = self.perform_migration()
            
            # Generate report
            self.generate_migration_report()
            
            return success
            
        except Exception as e:
            self.log_migration(f"Error during migration: {e}")
            return False
    
    def validate_input(self) -> bool:
        """Validate input file and configuration."""
        self.log_migration("Validating input file...")
        
        # Check if input file exists
        if not Path(self.args.input_file).exists():
            self.log_migration(f"Error: Input file not found: {self.args.input_file}")
            return False
        
        # Validate source format
        source_format = self.migration_config["source_format"]
        if source_format == "unknown":
            self.log_migration(f"Error: Cannot determine source format from: {self.args.input_file}")
            return False
        
        # Validate target format
        if self.migration_config["target_format"] == "unknown":
            self.log_migration("Error: Unknown target format")
            return False
        
        # Check if source and target formats are the same
        if source_format == self.migration_config["target_format"]:
            self.log_migration(f"Warning: Source and target formats are the same: {source_format}")
        
        self.log_migration(f"Input validation passed: {source_format} -> {self.migration_config['target_format']}")
        return True
    
    def perform_migration(self) -> bool:
        """Perform the actual migration."""
        self.log_migration("Starting migration...")
        
        try:
            # Create backup if requested
            backup_path = self.backup_file(self.args.input_file)
            
            # Load source data based on format
            source_data = self.load_source_data()
            if source_data is None:
                return False
            
            # Transform data if needed
            transformed_data = self.transform_data(source_data)
            
            # Save target data
            success = self.save_target_data(transformed_data)
            
            # Update statistics
            self.stats["files_processed"] += 1
            if success:
                self.stats["successful_migrations"] += 1
            else:
                self.stats["failed_migrations"] += 1
            
            # Clean up backup if migration failed
            if not success and backup_path:
                try:
                    Path(backup_path).unlink()
                    self.log_migration(f"Removed failed migration backup: {backup_path}")
                except:
                    pass
            
            return success
            
        except Exception as e:
            self.log_migration(f"Error during migration: {e}")
            self.stats["failed_migrations"] += 1
            return False
    
    def load_source_data(self) -> Optional[Any]:
        """Load data from source file."""
        source_format = self.migration_config["source_format"]
        
        try:
            self.log_migration(f"Loading data from {source_format} file...")
            
            if source_format == "json":
                return self.load_json_data()
            elif source_format == "yaml":
                return self.load_yaml_data()
            elif source_format == "xml":
                return self.load_xml_data()
            elif source_format == "csv":
                return self.load_csv_data()
            elif source_format == "txt":
                return self.load_text_data()
            elif source_format == "ini":
                return self.load_ini_data()
            elif source_format == "cfg":
                return self.load_cfg_data()
            else:
                self.log_migration(f"Error: Unsupported source format: {source_format}")
                return None
                
        except Exception as e:
            self.log_migration(f"Error loading {source_format} data: {e}")
            return None
    
    def load_json_data(self) -> Dict[str, Any]:
        """Load JSON data."""
        with open(self.args.input_file, 'r') as f:
            return json.load(f)
    
    def load_yaml_data(self) -> Dict[str, Any]:
        """Load YAML data."""
        try:
            import yaml
            with open(self.args.input_file, 'r') as f:
                return yaml.safe_load(f)
        except ImportError:
            raise Exception("PyYAML is required for YAML migration")
    
    def load_xml_data(self) -> Dict[str, Any]:
        """Load XML data."""
        tree = ET.parse(self.args.input_file)
        root = tree.getroot()
        
        # Convert XML to dictionary
        result = {}
        for child in root:
            result[child.tag] = self.parse_xml_element(child)
        
        return result
    
    def parse_xml_element(self, element) -> Any:
        """Parse an XML element to a Python object."""
        # If element has no children, return text
        if len(element) == 0:
            return element.text
        
        # If element has children, convert to dictionary
        result = {}
        for child in element:
            child_data = self.parse_xml_element(child)
            if child.tag in result:
                # If multiple children with same tag, convert to list
                if not isinstance(result[child.tag], list):
                    result[child.tag] = [result[child.tag]]
                result[child.tag].append(child_data)
            else:
                result[child.tag] = child_data
        
        return result
    
    def load_csv_data(self) -> List[Dict[str, Any]]:
        """Load CSV data."""
        with open(self.args.input_file, 'r') as f:
            reader = csv.DictReader(f)
            return list(reader)
    
    def load_text_data(self) -> Dict[str, Any]:
        """Load text data."""
        with open(self.args.input_file, 'r') as f:
            content = f.read()
        
        # Try to parse as key-value pairs
        result = {"content": content}
        
        # Try to parse INI-style format
        for line in content.split('\n'):
            line = line.strip()
            if '=' in line and not line.startswith('#'):
                key, value = line.split('=', 1)
                result[key.strip()] = value.strip()
        
        return result
    
    def load_ini_data(self) -> Dict[str, Any]:
        """Load INI data."""
        result = {"sections": {}}
        
        current_section = None
        with open(self.args.input_file, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                elif line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1]
                    result["sections"][current_section] = {}
                elif '=' in line and current_section:
                    key, value = line.split('=', 1)
                    result["sections"][current_section][key.strip()] = value.strip()
        
        return result
    
    def load_cfg_data(self) -> Dict[str, Any]:
        """Load configuration file data."""
        # Similar to INI but with potentially different format
        return self.load_ini_data()
    
    def transform_data(self, source_data: Any) -> Any:
        """Transform data for target format."""
        self.log_migration("Transforming data for target format...")
        
        # Apply transformations based on configuration
        if self.migration_config.get("convert_metadata", True):
            source_data = self.convert_metadata(source_data)
        
        if not self.migration_config.get("preserve_structure", True):
            source_data = self.normalize_structure(source_data)
        
        return source_data
    
    def convert_metadata(self, data: Any) -> Any:
        """Convert metadata to standard format."""
        if isinstance(data, dict):
            result = data.copy()
            
            # Add standard metadata
            result["metadata"] = {
                "migrated": True,
                "migration_timestamp": datetime.now().isoformat(),
                "source_format": self.migration_config["source_format"],
                "target_format": self.migration_config["target_format"]
            }
            
            # Convert keys to lowercase for consistency
            if self.args.standardize_keys:
                result["metadata"]["standardized"] = True
                for key in list(result.keys()):
                    if key.isupper():
                        lower_key = key.lower()
                        result[lower_key] = result[key]
                        del result[key]
            
            return result
        return data
    
    def normalize_structure(self, data: Any) -> Any:
        """Normalize data structure."""
        if isinstance(data, dict):
            result = {}
            for key, value in data.items():
                # Convert keys to snake_case
                snake_key = re.sub(r'(?<!^)(?=[A-Z])', '_', key).lower()
                result[snake_key] = self.normalize_structure(value)
            return result
        elif isinstance(data, list):
            return [self.normalize_structure(item) for item in data]
        else:
            return data
    
    def save_target_data(self, data: Any) -> bool:
        """Save data to target format."""
        target_format = self.migration_config["target_format"]
        
        try:
            self.log_migration(f"Saving data to {target_format}...")
            
            # Determine output filename
            input_path = Path(self.args.input_file)
            if self.args.output_dir:
                output_dir = Path(self.args.output_dir)
            else:
                output_dir = input_path.parent
            
            if self.args.output_filename:
                output_filename = self.args.output_filename
            else:
                output_filename = f"{input_path.stem}_migrated.{target_format}"
            
            output_path = output_dir / output_filename
            
            # Save based on target format
            if target_format == "json":
                self.save_json_data(data, output_path)
            elif target_format == "yaml":
                self.save_yaml_data(data, output_path)
            elif target_format == "xml":
                self.save_xml_data(data, output_path)
            elif target_format == "csv":
                self.save_csv_data(data, output_path)
            elif target_format == "txt":
                self.save_text_data(data, output_path)
            else:
                self.log_migration(f"Error: Unsupported target format: {target_format}")
                return False
            
            self.log_migration(f"Successfully migrated to: {output_path}")
            return True
            
        except Exception as e:
            self.log_migration(f"Error saving target data: {e}")
            return False
    
    def save_json_data(self, data: Any, output_path: Path):
        """Save data as JSON."""
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)
    
    def save_yaml_data(self, data: Any, output_path: Path):
        """Save data as YAML."""
        try:
            import yaml
            with open(output_path, 'w') as f:
                yaml.dump(data, f, default_flow_style=False)
        except ImportError:
            raise Exception("PyYAML is required for YAML migration")
    
    def save_xml_data(self, data: Any, output_path: Path):
        """Save data as XML."""
        root = ET.Element("brain_config")
        
        self.append_to_xml(root, data)
        
        # Pretty print XML
        xml_str = ET.tostring(root, encoding='utf-8')
        parsed_xml = minidom.parseString(xml_str)
        pretty_xml = parsed_xml.toprettyxml(indent="  ")
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(pretty_xml)
    
    def append_to_xml(self, parent: ET.Element, data: Any):
        """Recursively append data to XML element."""
        if isinstance(data, dict):
            for key, value in data.items():
                child = ET.SubElement(parent, key)
                self.append_to_xml(child, value)
        elif isinstance(data, list):
            for item in data:
                item_element = ET.SubElement(parent, "item")
                self.append_to_xml(item_element, item)
        else:
            parent.text = str(data)
    
    def save_csv_data(self, data: Any, output_path: Path):
        """Save data as CSV."""
        if isinstance(data, list) and len(data) > 0 and isinstance(data[0], dict):
            with open(output_path, 'w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=data[0].keys())
                writer.writeheader()
                writer.writerows(data)
        else:
            raise Exception("Cannot save non-dictionary list as CSV")
    
    def save_text_data(self, data: Any, output_path: Path):
        """Save data as text."""
        with open(output_path, 'w') as f:
            if isinstance(data, dict):
                for key, value in data.items():
                    f.write(f"{key}: {value}\n")
            else:
                f.write(str(data))
    
    def generate_migration_report(self):
        """Generate a migration report."""
        self.stats["end_time"] = time.time()
        duration = self.stats["end_time"] - self.stats["start_time"]
        
        report = f"# Brain Configuration Migration Report\n"
        report += f"Generated: {datetime.now().isoformat()}\n\n"
        
        report += f"## Summary\n"
        report += f"- Files processed: {self.stats['files_processed']}\n"
        report += f"- Successful migrations: {self.stats['successful_migrations']}\n"
        report += f"- Failed migrations: {self.stats['failed_migrations']}\n"
        report += f"- Total duration: {duration:.2f} seconds\n\n"
        
        if self.migration_history:
            report += f"## Migration History\n"
            for entry in self.migration_history[-10:]:  # Show last 10 entries
                report += f"- {entry['timestamp']}: {entry['message']}\n"
            report += "\n"
        
        # Save report
        if self.args.output_dir:
            report_path = Path(self.args.output_dir) / "migration_report.md"
            with open(report_path, 'w') as f:
                f.write(report)
            self.log_migration(f"Migration report saved: {report_path}")
        
        # Print summary
        print(f"\nMigration complete!")
        print(f"Files processed: {self.stats['files_processed']}")
        print(f"Successful: {self.stats['successful_migrations']}")
        print(f"Failed: {self.stats['failed_migrations']}")
        print(f"Duration: {duration:.2f} seconds")
        
        if self.args.output_dir:
            print(f"Report saved to: {report_path}")

def main():
    parser = argparse.ArgumentParser(
        description="Brain Configuration Migrator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic migration from JSON to YAML
  brain_config_migrator.py --input config.json --output-dir ./results
  
  # Migration with validation
  brain_config_migrator.py --input old_config.ini --target-format json --validation-level strict
  
  # Migration with custom output filename
  brain_config_migrator.py --input config.xml --output-filename migrated_config.json
  
  # Migration with backup
  brain_config_migrator.py --input config.yaml --backup-files
  
  # Migration with metadata conversion
  brain_config_migrator.py --input old_config.txt --convert-metadata
"""
    )
    
    parser.add_argument(
        "input_file",
        help="Input configuration file to migrate"
    )
    parser.add_argument(
        "--source-format",
        choices=["json", "yaml", "xml", "csv", "txt", "ini", "cfg", "unknown"],
        help="Source file format (auto-detected if not specified)"
    )
    parser.add_argument(
        "--target-format",
        choices=["json", "yaml", "xml", "csv", "txt", "ini", "cfg"],
        default="json",
        help="Target file format (default: json)"
    )
    parser.add_argument(
        "--output-dir",
        help="Output directory for migrated files"
    )
    parser.add_argument(
        "--output-filename",
        help="Custom filename for the output file"
    )
    parser.add_argument(
        "--backup-files",
        action="store_true",
        help="Create backup of source file before migration"
    )
    parser.add_argument(
        "--preserve-structure",
        action="store_true",
        default=True,
        help="Preserve original structure (default: true)"
    )
    parser.add_argument(
        "--convert-metadata",
        action="store_true",
        default=True,
        help="Convert metadata to standard format (default: true)"
    )
    parser.add_argument(
        "--validation-level",
        choices=["none", "basic", "strict"],
        default="strict",
        help="Validation level (default: strict)"
    )
    parser.add_argument(
        "--log-migrations",
        action="store_true",
        default=True,
        help="Enable migration logging (default: true)"
    )
    parser.add_argument(
        "--standardize-keys",
        action="store_true",
        help="Standardize keys to lowercase"
    )
    
    args = parser.parse_args()
    
    migrator = BrainConfigMigrator(args)
    success = migrator.migrate()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())