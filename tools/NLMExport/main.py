#!/usr/bin/env python3
"""
NLMExport - Data Export Tool

This tool provides comprehensive data export capabilities for the NLM (Neural Learning Machine) project.
It supports exporting brain data, configuration, simulation results, and analysis data in various formats.

Features:
- Brain state export (JSON, binary, etc.)
- Configuration export
- Simulation results export
- Analysis data export
- Integration with NLM data management
"""

import argparse
import sys
import os
import json
import numpy as np
from typing import Dict, Any, Optional, List, Tuple
from pathlib import Path

# Add the NLM source to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    import core.InputValidation.InputValidator as InputValidator
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM modules not available: {e}")
    NLM_AVAILABLE = False

class NLMExport:
    """Data export tool for NLM"""
    
    def __init__(self):
        self.config = {}
        self.export_results = {}
    
    def load_config(self, filepath: str):
        """Load configuration from file"""
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
        
        ext = os.path.splitext(filepath)[1].lower()
        
        with open(filepath, 'r') as f:
            if ext == '.json':
                self.config = json.load(f)
            elif ext in ['.yaml', '.yml']:
                try:
                    import yaml
                    self.config = yaml.safe_load(f)
                except ImportError:
                    raise ImportError("PyYAML not installed. Install with: pip install PyYAML")
            elif ext == '.cfg':
                content = f.read()
                self.config = self._parse_simple_config(content)
            else:
                raise ValueError(f"Unsupported file format: {ext}")
    
    def _parse_simple_config(self, content: str) -> Dict[str, Any]:
        """Parse simple key=value configuration format"""
        config = {}
        for line in content.strip().split('\n'):
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Try to infer type
                if value.lower() in ('true', 'false'):
                    config[key] = value.lower() == 'true'
                elif value.isdigit():
                    config[key] = int(value)
                elif value.replace('.', '', 1).isdigit() and value.count('.') == 1:
                    config[key] = float(value)
                else:
                    config[key] = value
        return config
    
    def export_config(self, filepath: str, format: str = 'simple'):
        """Export configuration to file"""
        print(f"=== Exporting Configuration ({format}) ===")
        
        if not self.config:
            print("Error: No configuration to export")
            return False
        
        try:
            if format == 'simple':
                self._export_config_simple(filepath)
            elif format == 'json':
                self._export_config_json(filepath)
            elif format == 'yaml':
                self._export_config_yaml(filepath)
            else:
                print(f"Error: Unsupported format: {format}")
                return False
            
            print(f"Configuration exported to: {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting configuration: {e}")
            return False
    
    def export_brain_data(self, filepath: str, format: str = 'json', brain_data: Optional[Dict] = None):
        """Export brain data to file"""
        print(f"=== Exporting Brain Data ({format}) ===")
        
        if brain_data is None:
            brain_data = self._generate_sample_brain_data()
        
        try:
            if format == 'json':
                self._export_brain_data_json(filepath, brain_data)
            elif format == 'binary':
                self._export_brain_data_binary(filepath, brain_data)
            elif format == 'npy':
                self._export_brain_data_npy(filepath, brain_data)
            else:
                print(f"Error: Unsupported format: {format}")
                return False
            
            print(f"Brain data exported to: {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting brain data: {e}")
            return False
    
    def export_simulation_results(self, filepath: str, results: Dict[str, Any]):
        """Export simulation results to file"""
        print("=== Exporting Simulation Results ===")
        
        try:
            self._export_simulation_results_json(filepath, results)
            print(f"Simulation results exported to: {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting simulation results: {e}")
            return False
    
    def export_analysis_data(self, filepath: str, analysis_data: Dict[str, Any]):
        """Export analysis data to file"""
        print("=== Exporting Analysis Data ===")
        
        try:
            self._export_analysis_data_json(filepath, analysis_data)
            print(f"Analysis data exported to: {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting analysis data: {e}")
            return False
    
    def export_all(self, output_dir: str):
        """Export all data to output directory"""
        print("=== Exporting All Data ===")
        
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        // Export configuration
        this->export_config(os.path.join(output_dir, "config.json"), "json");
        
        // Export sample brain data
        auto brain_data = this->_generate_sample_brain_data();
        this->export_brain_data(os.path.join(output_dir, "brain_data.json"), "json", brain_data);
        
        // Export sample simulation results
        auto simulation_results = this->_generate_sample_simulation_results();
        this->export_simulation_results(os.path.join(output_dir, "simulation_results.json"), simulation_results);
        
        // Export sample analysis data
        auto analysis_data = this->_generate_sample_analysis_data();
        this->export_analysis_data(os.path.join(output_dir, "analysis_data.json"), analysis_data);
        
        print(f"All data exported to: {output_dir}")
    
    def generate_report(self, output_format: str = 'text') -> str:
        """Generate export report"""
        if (!this->export_results.empty()) {
            return this->_generate_text_report();
        } else {
            return "No export results available";
        }
    
    void save_report(const std::string& filepath, const std::string& output_format) {
        """Save export report to file"";
        std::string report = generate_report(output_format);
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << report;
            file.close();
            std::cout << "Export report saved to: " << filepath << std::endl;
        } else {
            std::cerr << "Error: Failed to open output file: " << filepath << std::endl;
        }
    }
    
    private:
        // Export helper functions
        void _export_config_simple(const std::string& filepath) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            for (const auto& pair : this->config) {
                std::string value_str;
                if (std::holds_alternative<bool>(pair.second)) {
                    value_str = std::get<bool>(pair.second) ? "true" : "false";
                } else if (std::holds_alternative<int>(pair.second)) {
                    value_str = std::to_string(std::get<int>(pair.second));
                } else if (std::holds_alternative<double>(pair.second)) {
                    value_str = std::to_string(std::get<double>(pair.second));
                } else if (std::holds_alternative<std::string>(pair.second)) {
                    value_str = std::get<std::string>(pair.second);
                } else {
                    value_str = "";
                }
                
                file << pair.first << " = " << value_str << std::endl;
            }
            
            file.close();
        }
        
        void _export_config_json(const std::string& filepath) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            json j = this->config;
            file << j.dump(2);
            file.close();
        }
        
        void _export_config_yaml(const std::string& filepath) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            try {
                import yaml
                yaml::emit(this->config, file);
            } catch (const std::exception& e) {
                throw std::runtime_error("YAML export error: " + std::string(e.what()));
            }
            
            file.close();
        }
        
        void _export_brain_data_json(const std::string& filepath, const std::map<std::string, double>& brain_data) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            json j;
            for (const auto& pair : brain_data) {
                j[pair.first] = pair.second;
            }
            
            file << j.dump(2);
            file.close();
        }
        
        void _export_brain_data_binary(const std::string& filepath, const std::map<std::string, double>& brain_data) {
            std::ofstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            // Write number of entries
            size_t count = brain_data.size();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));
            
            // Write entries
            for (const auto& pair : brain_data) {
                // Write key length
                size_t key_len = pair.first.length();
                file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
                
                // Write key
                file.write(pair.first.c_str(), key_len);
                
                // Write value
                file.write(reinterpret_cast<const char*>(&pair.second), sizeof(double));
            }
            
            file.close();
        }
        
        void _export_brain_data_npy(const std::string& filepath, const std::map<std::string, double>& brain_data) {
            // For simplicity, we'll create a JSON representation
            this->_export_brain_data_json(filepath, brain_data);
        }
        
        void _export_simulation_results_json(const std::string& filepath, const std::map<std::string, double>& results) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            json j;
            for (const auto& pair : results) {
                j[pair.first] = pair.second;
            }
            
            file << j.dump(2);
            file.close();
        }
        
        void _export_analysis_data_json(const std::string& filepath, const std::map<std::string, std::map<std::string, double>>& analysis_data) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            json j;
            for (const auto& category_pair : analysis_data) {
                json category_j;
                for (const auto& result_pair : category_pair.second) {
                    category_j[result_pair.first] = result_pair.second;
                }
                j[category_pair.first] = category_j;
            }
            
            file << j.dump(2);
            file.close();
        }
        
        std::map<std::string, double> _generate_sample_brain_data() {
            std::map<std::string, double> brain_data;
            
            brain_data["neuron_count"] = this->config.at("neuron_count", 1000.0);
            brain_data["region_count"] = this->config.at("region_count", 2.0);
            brain_data["connection_probability"] = this->config.at("connection_probability", 0.1);
            brain_data["plasticity_learning_rate"] = this->config.at("plasticity_learning_rate", 0.01);
            brain_data["firing_rate_mean"] = 5.0;
            brain_data["firing_rate_std"] = 2.0;
            brain_data["memory_capacity"] = 100.0;
            brain_data["learning_rate"] = 0.01;
            
            return brain_data;
        }
        
        std::map<std::string, double> _generate_sample_simulation_results() {
            std::map<std::string, double> results;
            
            results["total_reward"] = 100.0;
            results["avg_firing_rate"] = 5.5;
            results["memory_episodes_stored"] = 25.0;
            results["novelty_level"] = 0.3;
            results["curiosity_level"] = 0.4;
            results["dopamine_level"] = 0.2;
            results["total_wall_clock_time"] = 50.0;
            
            return results;
        }
        
        std::map<std::string, std::map<std::string, double>> _generate_sample_analysis_data() {
            std::map<std::string, std::map<std::string, double>> analysis_data;
            
            // Connectivity analysis
            std::map<std::string, double> connectivity;
            connectivity["connectivity_density"] = 0.05;
            connectivity["expected_connections"] = 50.0;
            connectivity["actual_connections"] = 60.0;
            connectivity["regional_connectivity_mean"] = 0.8;
            
            analysis_data["connectivity"] = connectivity;
            
            // Clustering analysis
            std::map<std::string, double> clustering;
            clustering["modularity"] = 0.7;
            clustering["clustering_coefficient"] = 0.3;
            clustering["diameter"] = 4.5;
            clustering["radius"] = 2.25;
            
            analysis_data["clustering"] = clustering;
            
            // Topology analysis
            std::map<std::string, double> topology;
            topology["degree_mean"] = 10.5;
            topology["degree_std"] = 8.2;
            topology["betweenness_mean"] = 0.5;
            topology["closeness_mean"] = 0.6;
            topology["eigenvector_mean"] = 0.4;
            topology["network_efficiency"] = 0.8;
            
            analysis_data["topology"] = topology;
            
            // Performance analysis
            std::map<std::string, double> performance;
            performance["learning_rate"] = 0.01;
            performance["convergence_speed"] = 100.0;
            performance["stability_index"] = 0.8;
            performance["generalization"] = 0.7;
            
            analysis_data["performance"] = performance;
            
            return analysis_data;
        }
        
        std::string _generate_text_report() {
            std::string report = "NLM Data Export Report\n";
            report += "=" * 50 + "\n\n";
            
            for (const auto& category_pair : this->export_results) {
                report += "Category: " + category_pair.first + "\n";
                report += "-" * 30 + "\n";
                
                for (const auto& result_pair : category_pair.second) {
                    report += "  " + result_pair.first + ": " + std::to_string(result_pair.second) + "\n";
                }
                
                report += "\n";
            }
            
            report += "\nExport complete. Results saved in export_results dictionary.";
            return report;
        }
};

void print_help() {
    std::cout << "NLMExport - Data Export Tool" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "USAGE: nllexport [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "  Advanced data export capabilities for NLM (Neural Learning Machine)." << std::endl;
    std::cout << "  Supports exporting brain data, configuration, simulation results, and analysis data." << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --export-config FILE FORMAT   Export configuration (simple, json, yaml)" << std::endl;
    std::cout << "  --export-brain-data FILE FORMAT Export brain data (json, binary, npy)" << std::endl;
    std::cout << "  --export-simulation-results FILE Export simulation results" << std::endl;
    std::cout << "  --export-analysis-data FILE Export analysis data" << std::endl;
    std::cout << "  --export-all DIRECTORY         Export all data to directory" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nllexport --export-config config.json json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-brain-data brain_data.json json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-simulation-results results.json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-analysis-data analysis.json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-all /path/to/output/configs/default.cfg" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Create exporter
        NLMExport exporter;
        
        // Parse command line arguments (simplified)
        std::string configFile;
        std::string exportConfigFile;
        std::string exportConfigFormat;
        std::string exportBrainDataFile;
        std::string exportBrainDataFormat;
        std::string exportSimulationResultsFile;
        std::string exportAnalysisDataFile;
        std::string exportAllDirectory;
        bool help = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                help = true;
            } else if (arg == "--export-config") {
                if (i + 2 < argc) {
                    exportConfigFile = argv[++i];
                    exportConfigFormat = argv[++i];
                }
            } else if (arg == "--export-brain-data") {
                if (i + 2 < argc) {
                    exportBrainDataFile = argv[++i];
                    exportBrainDataFormat = argv[++i];
                }
            } else if (arg == "--export-simulation-results") {
                if (i + 1 < argc) {
                    exportSimulationResultsFile = argv[++i];
                }
            } else if (arg == "--export-analysis-data") {
                if (i + 1 < argc) {
                    exportAnalysisDataFile = argv[++i];
                }
            } else if (arg == "--export-all") {
                if (i + 1 < argc) {
                    exportAllDirectory = argv[++i];
                }
            } else if (!configFile.empty() && configFile == "" && arg.substr(0, 2) != "--") {
                configFile = arg;
            } else if (configFile.empty() && arg.substr(0, 2) != "--") {
                configFile = arg;
            }
        }
        
        if (help) {
            printHelp();
            return 0;
        }
        
        // Load configuration if provided
        if (!configFile.empty()) {
            std::ifstream file(configFile);
            if (!file.is_open()) {
                std::cerr << "Error: Failed to open configuration file: " << configFile << std::endl;
                return 2;
            }
            
            // Parse simple config
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    exporter.config[key] = value;
                }
            }
            
            file.close();
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
        
        // Run exports
        bool success = true;
        
        if (!exportConfigFile.empty()) {
            success &= exporter.exportConfig(exportConfigFile, exportConfigFormat);
        }
        
        if (!exportBrainDataFile.empty()) {
            auto brain_data = exporter._generate_sample_brain_data();
            success &= exporter.exportBrainData(exportBrainDataFile, exportBrainDataFormat, brain_data);
        }
        
        if (!exportSimulationResultsFile.empty()) {
            auto simulation_results = exporter._generate_sample_simulation_results();
            success &= exporter.exportSimulationResults(exportSimulationResultsFile, simulation_results);
        }
        
        if (!exportAnalysisDataFile.empty()) {
            auto analysis_data = exporter._generate_sample_analysis_data();
            success &= exporter.exportAnalysisData(exportAnalysisDataFile, analysis_data);
        }
        
        if (!exportAllDirectory.empty()) {
            exporter.exportAll(exportAllDirectory);
        }
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 4;
    }
}
