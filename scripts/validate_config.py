# NLM Configuration Validation Script
# Validates NLM configuration files against schema
# and provides recommendations for improvements

import json
import sys
import os
from pathlib import Path

# Add scripts directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'scripts'))

class ConfigValidator:
    def __init__(self):
        self.issues = []
        self.warnings = []
        self.recommendations = []
    
    def validate_config(self, config_file):
        """Validate a configuration file."""
        print(f"Validating configuration file: {config_file}")
        
        if not os.path.exists(config_file):
            print(f"❌ ERROR: Configuration file not found: {config_file}")
            return False
        
        # Read and parse configuration
        config_content = self.read_config_file(config_file)
        if config_content is None:
            return False
        
        # Parse sections
        sections = self.parse_sections(config_content)
        
        # Validate sections
        self.validate_sections(sections)
        
        # Check for issues
        if self.issues:
            print("\n❌ VALIDATION FAILED:")
            for issue in self.issues:
                print(f"  ERROR: {issue}")
        
        if self.warnings:
            print("\n⚠️  VALIDATION WARNINGS:")
            for warning in self.warnings:
                print(f"  WARNING: {warning}")
        
        if self.recommendations:
            print("\n💡 RECOMMENDATIONS:")
            for recommendation in self.recommendations:
                print(f"  {recommendation}")
        
        # Report results
        if not self.issues:
            print("\n✅ VALIDATION PASSED")
            return True
        else:
            print("\n❌ VALIDATION FAILED")
            return False
    
    def read_config_file(self, config_file):
        """Read configuration file content."""
        try:
            with open(config_file, 'r') as f:
                return f.read()
        except Exception as e:
            self.issues.append(f"Failed to read configuration file: {e}")
            return None
    
    def parse_sections(self, content):
        """Parse configuration sections from content."""
        sections = {}
        
        # Simple section parser
        lines = content.split('\n')
        current_section = "default"
        
        for line in lines:
            line = line.strip()
            
            if line.startswith('[') and line.endswith(']'):
                current_section = line[1:-1]
                sections[current_section] = {}
            elif '=' in line and not line.startswith('#'):
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                if current_section not in sections:
                    sections[current_section] = {}
                
                sections[current_section][key] = value
        
        return sections
    
    def validate_sections(self, sections):
        """Validate configuration sections."""
        
        # Check required sections
        required_sections = ["simulation.timing", "simulation.topology"]
        for section in required_sections:
            if section not in sections:
                self.warnings.append(f"Missing recommended section: {section}")
        
        # Validate simulation.timing section
        if "simulation.timing" in sections:
            self.validate_timing_section(sections["simulation.timing"])
        
        # Validate simulation.topology section
        if "simulation.topology" in sections:
            self.validate_topology_section(sections["simulation.topology"])
        
        # Validate logging section
        if "logging.level" in sections:
            self.validate_logging_section(sections["logging.level"])
        
        # Check for common issues
        self.check_common_issues(sections)
    
    def validate_timing_section(self, timing_section):
        """Validate simulation.timing section."""
        
        # Check simulation_timestep
        if "simulation_timestep" in timing_section:
            try:
                timestep = float(timing_section["simulation_timestep"])
                if timestep <= 0.0:
                    self.issues.append("simulation_timestep must be positive")
                elif timestep > 0.1:
                    self.warnings.append("simulation_timestep is unusually large (> 0.1)")
            except ValueError:
                self.issues.append("simulation_timestep must be a number")
        
        # Check max_simulation_steps
        if "max_simulation_steps" in timing_section:
            try:
                steps = int(timing_section["max_simulation_steps"])
                if steps < 1:
                    self.issues.append("max_simulation_steps must be positive")
                elif steps > 1000000:
                    self.warnings.append("max_simulation_steps is unusually large (> 1,000,000)")
            except ValueError:
                self.issues.append("max_simulation_steps must be an integer")
    
    def validate_topology_section(self, topology_section):
        """Validate simulation.topology section."""
        
        # Check neuron_count
        if "neuron_count" in topology_section:
            try:
                neurons = int(topology_section["neuron_count"])
                if neurons < 1:
                    self.issues.append("neuron_count must be positive")
                elif neurons > 1000000:
                    self.warnings.append("neuron_count is unusually large (> 1,000,000)")
            except ValueError:
                self.issues.append("neuron_count must be an integer")
        
        # Check connection_probability
        if "connection_probability" in topology_section:
            try:
                prob = float(topology_section["connection_probability"])
                if prob < 0.0 or prob > 1.0:
                    self.issues.append("connection_probability must be between 0.0 and 1.0")
            except ValueError:
                self.issues.append("connection_probability must be a number")
        
        # Check region_count
        if "region_count" in topology_section:
            try:
                regions = int(topology_section["region_count"])
                if regions < 1:
                    self.issues.append("region_count must be positive")
            except ValueError:
                self.issues.append("region_count must be an integer")
    
    def validate_logging_section(self, logging_section):
        """Validate logging section."""
        
        # Check log_level
        if "log_level" in logging_section:
            level = logging_section["log_level"].lower()
            valid_levels = ["trace", "debug", "info", "warning", "error", "critical", "fatal"]
            if level not in valid_levels:
                self.warnings.append(f"log_level '{level}' is not a standard level. Consider using: {', '.join(valid_levels)}")
        
        # Check log_to_file
        if "log_to_file" in logging_section:
            value = logging_section["log_to_file"].lower()
            if value not in ["true", "false", "yes", "no", "on", "off"]:
                self.warnings.append(f"log_to_file value '{value}' may not be a valid boolean")
    
    def check_common_issues(self, sections):
        """Check for common configuration issues."""
        
        # Check for duplicate dopamine_baseline (found in default.cfg)
        if "neuromodulation.dopamine" in sections:
            dopamine = sections["neuromodulation.dopamine"]
            if "dopamine_baseline" in dopamine:
                count = sum(1 for key in dopamine.keys() if key == "dopamine_baseline")
                if count > 1:
                    self.warnings.append("Duplicate 'dopamine_baseline' values found. Check for duplicate lines.")
        
        # Check for missing section comments
        for section_name, section in sections.items():
            # Check if section has meaningful content
            if len(section) == 0:
                self.warnings.append(f"Section '{section_name}' is empty")
        
        # Check for obviously wrong values
        if "simulation.topology" in sections:
            topology = sections["simulation.topology"]
            if "neuron_count" in topology and "region_count" in topology:
                try:
                    neurons = int(topology["neuron_count"])
                    regions = int(topology["region_count"])
                    if neurons < regions:
                        self.recommendations.append("Consider increasing neuron_count relative to region_count for better regional diversity")
                except ValueError:
                    pass
        
        # Check for enhanced logging configuration
        if "logging.level" in sections:
            logging = sections["logging.level"]
            has_enhanced = any(key.startswith("enhanced_" ) for key in logging.keys())
            if not has_enhanced:
                self.recommendations.append("Consider enabling enhanced logging features for better debugging and monitoring")
    
    def generate_recommendation_report(self):
        """Generate recommendation report."""
        report = "# Configuration Recommendations\n\n"
        
        if self.issues:
            report += "## Issues Found\n\n"
            for issue in self.issues:
                report += f"- **ERROR**: {issue}\n"
            report += "\n"
        
        if self.warnings:
            report += "## Warnings\n\n"
            for warning in self.warnings:
                report += f"- **WARNING**: {warning}\n"
            report += "\n"
        
        if self.recommendations:
            report += "## Recommendations\n\n"
            for recommendation in self.recommendations:
                report += f"- **{recommendation}**\n"
            report += "\n"
        
        if not (self.issues or self.warnings or self.recommendations):
            report += "All checks passed. Configuration is in good shape!\n"
        
        return report

def main():
    """Main function."""
    if len(sys.argv) < 2:
        print("Usage: python validate_config.py <config_file>")
        print("\nExamples:")
        print("  python validate_config.py configs/enhanced.cfg")
        print("  python validate_config.py configs/default.cfg")
        print("\nThis script validates NLM configuration files and provides")
        print("recommendations for improvements and best practices.")
        sys.exit(1)
    
    config_file = sys.argv[1]
    validator = ConfigValidator()
    
    # Validate configuration
    success = validator.validate_config(config_file)
    
    # Generate recommendation report
    report = validator.generate_recommendation_report()
    
    # Save report
    report_file = config_file.replace('.cfg', '_validation_report.md')
    try:
        with open(report_file, 'w') as f:
            f.write(report)
        print(f"\nRecommendation report saved to: {report_file}")
    except Exception as e:
        print(f"\nWarning: Could not save recommendation report: {e}")
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)

# NLM Configuration Validation Script
# Validates NLM configuration files against schema
# and provides recommendations for improvements

import json
import sys
import os
from pathlib import Path

# Add scripts directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'scripts'))

class ConfigValidator:
    def __init__(self):
        self.issues = []
        self.warnings = []
        self.recommendations = []
    
    def validate_config(self, config_file):
        """Validate a configuration file."""
        print(f"Validating configuration file: {config_file}")
        
        if not os.path.exists(config_file):
            print(f"❌ ERROR: Configuration file not found: {config_file}")
            return False
        
        # Read and parse configuration
        config_content = self.read_config_file(config_file)
        if config_content is None:
            return False
        
        # Parse sections
        sections = self.parse_sections(config_content)
        
        # Validate sections
        self.validate_sections(sections)
        
        # Check for issues
        if self.issues:
            print("\n❌ VALIDATION FAILED:")
            for issue in self.issues:
                print(f"  ERROR: {issue}")
        
        if self.warnings:
            print("\n⚠️  VALIDATION WARNINGS:")
            for warning in self.warnings:
                print(f"  WARNING: {warning}")
        
        if self.recommendations:
            print("\n💡 RECOMMENDATIONS:")
            for recommendation in self.recommendations:
                print(f"  {recommendation}")
        
        # Report results
        if not self.issues:
            print("\n✅ VALIDATION PASSED")
            return True
        else:
            print("\n❌ VALIDATION FAILED")
            return False
    
    def read_config_file(self, config_file):
        """Read configuration file content."""
        try:
            with open(config_file, 'r') as f:
                return f.read()
        except Exception as e:
            self.issues.append(f"Failed to read configuration file: {e}")
            return None
    
    def parse_sections(self, content):
        """Parse configuration sections from content."""
        sections = {}
        
        # Simple section parser
        lines = content.split('\n')
        current_section = "default"
        
        for line in lines:
            line = line.strip()
            
            if line.startswith('[') and line.endswith(']'):
                current_section = line[1:-1]
                sections[current_section] = {}
            elif '=' in line and not line.startswith('#'):
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                if current_section not in sections:
                    sections[current_section] = {}
                
                sections[current_section][key] = value
        
        return sections
    
    def validate_sections(self, sections):
        """Validate configuration sections."""
        
        # Check required sections
        required_sections = ["simulation.timing", "simulation.topology"]
        for section in required_sections:
            if section not in sections:
                self.warnings.append(f"Missing recommended section: {section}")
        
        # Validate simulation.timing section
        if "simulation.timing" in sections:
            self.validate_timing_section(sections["simulation.timing"])
        
        # Validate simulation.topology section
        if "simulation.topology" in sections:
            self.validate_topology_section(sections["simulation.topology"])
        
        # Validate logging section
        if "logging.level" in sections:
            self.validate_logging_section(sections["logging.level"])
        
        # Check for common issues
        self.check_common_issues(sections)
    
    def validate_timing_section(self, timing_section):
        """Validate simulation.timing section."""
        
        # Check simulation_timestep
        if "simulation_timestep" in timing_section:
            try:
                timestep = float(timing_section["simulation_timestep"])
                if timestep <= 0.0:
                    self.issues.append("simulation_timestep must be positive")
                elif timestep > 0.1:
                    self.warnings.append("simulation_timestep is unusually large (> 0.1)")
            except ValueError:
                self.issues.append("simulation_timestep must be a number")
        
        # Check max_simulation_steps
        if "max_simulation_steps" in timing_section:
            try:
                steps = int(timing_section["max_simulation_steps"])
                if steps < 1:
                    self.issues.append("max_simulation_steps must be positive")
                elif steps > 1000000:
                    self.warnings.append("max_simulation_steps is unusually large (> 1,000,000)")
            except ValueError:
                self.issues.append("max_simulation_steps must be an integer")
    
    def validate_topology_section(self, topology_section):
        """Validate simulation.topology section."""
        
        # Check neuron_count
        if "neuron_count" in topology_section:
            try:
                neurons = int(topology_section["neuron_count"])
                if neurons < 1:
                    self.issues.append("neuron_count must be positive")
                elif neurons > 1000000:
                    self.warnings.append("neuron_count is unusually large (> 1,000,000)")
            except ValueError:
                self.issues.append("neuron_count must be an integer")
        
        # Check connection_probability
        if "connection_probability" in topology_section:
            try:
                prob = float(topology_section["connection_probability"])
                if prob < 0.0 or prob > 1.0:
                    self.issues.append("connection_probability must be between 0.0 and 1.0")
            except ValueError:
                self.issues.append("connection_probability must be a number")
        
        # Check region_count
        if "region_count" in topology_section:
            try:
                regions = int(topology_section["region_count"])
                if regions < 1:
                    self.issues.append("region_count must be positive")
            except ValueError:
                self.issues.append("region_count must be an integer")
    
    def validate_logging_section(self, logging_section):
        """Validate logging section."""
        
        # Check log_level
        if "log_level" in logging_section:
            level = logging_section["log_level"].lower()
            valid_levels = ["trace", "debug", "info", "warning", "error", "critical", "fatal"]
            if level not in valid_levels:
                self.warnings.append(f"log_level '{level}' is not a standard level. Consider using: {', '.join(valid_levels)}")
        
        # Check log_to_file
        if "log_to_file" in logging_section:
            value = logging_section["log_to_file"].lower()
            if value not in ["true", "false", "yes", "no", "on", "off"]:
                self.warnings.append(f"log_to_file value '{value}' may not be a valid boolean")
    
    def check_common_issues(self, sections):
        """Check for common configuration issues."""
        
        # Check for duplicate dopamine_baseline (found in default.cfg)
        if "neuromodulation.dopamine" in sections:
            dopamine = sections["neuromodulation.dopamine"]
            if "dopamine_baseline" in dopamine:
                count = sum(1 for key in dopamine.keys() if key == "dopamine_baseline")
                if count > 1:
                    self.warnings.append("Duplicate 'dopamine_baseline' values found. Check for duplicate lines.")
        
        # Check for missing section comments
        for section_name, section in sections.items():
            # Check if section has meaningful content
            if len(section) == 0:
                self.warnings.append(f"Section '{section_name}' is empty")
        
        # Check for obviously wrong values
        if "simulation.topology" in sections:
            topology = sections["simulation.topology"]
            if "neuron_count" in topology and "region_count" in topology:
                try:
                    neurons = int(topology["neuron_count"])
                    regions = int(topology["region_count"])
                    if neurons < regions:
                        self.recommendations.append("Consider increasing neuron_count relative to region_count for better regional diversity")
                except ValueError:
                    pass
        
        # Check for enhanced logging configuration
        if "logging.level" in sections:
            logging = sections["logging.level"]
            has_enhanced = any(key.startswith("enhanced_" ) for key in logging.keys())
            if not has_enhanced:
                self.recommendations.append("Consider enabling enhanced logging features for better debugging and monitoring")
    
    def generate_recommendation_report(self):
        """Generate recommendation report."""
        report = "# Configuration Recommendations\n\n"
        
        if self.issues:
            report += "## Issues Found\n\n"
            for issue in self.issues:
                report += f"- **ERROR**: {issue}\n"
            report += "\n"
        
        if self.warnings:
            report += "## Warnings\n\n"
            for warning in self.warnings:
                report += f"- **WARNING**: {warning}\n"
            report += "\n"
        
        if self.recommendations:
            report += "## Recommendations\n\n"
            for recommendation in self.recommendations:
                report += f"- **{recommendation}**\n"
            report += "\n"
        
        if not (self.issues or self.warnings or self.recommendations):
            report += "All checks passed. Configuration is in good shape!\n"
        
        return report

def main():
    """Main function."""
    if len(sys.argv) < 2:
        print("Usage: python validate_config.py <config_file>")
        print("\nExamples:")
        print("  python validate_config.py configs/enhanced.cfg")
        print("  python validate_config.py configs/default.cfg")
        print("\nThis script validates NLM configuration files and provides")
        print("recommendations for improvements and best practices.")
        sys.exit(1)
    
    config_file = sys.argv[1]
    validator = ConfigValidator()
    
    # Validate configuration
    success = validator.validate_config(config_file)
    
    # Generate recommendation report
    report = validator.generate_recommendation_report()
    
    # Save report
    report_file = config_file.replace('.cfg', '_validation_report.md')
    try:
        with open(report_file, 'w') as f:
            f.write(report)
        print(f"\nRecommendation report saved to: {report_file}")
    except Exception as e:
        print(f"\nWarning: Could not save recommendation report: {e}")
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()

# Set executable permissions
import os
os.chmod("validate_config.py", 0o755)
