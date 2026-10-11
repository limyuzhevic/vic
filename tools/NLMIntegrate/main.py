#!/usr/bin/env python3
"""
NLMIntegrate - Integration Validation Tool

This tool provides comprehensive integration validation and testing for the NLM (Neural Learning Machine) project.
It supports Phase 6 integration verification, integration testing automation, and integration report generation.

Features:
- Phase 6 integration verification
- Integration testing automation
- Integration validation and validation reports
- Integration health monitoring
- Integration with Phase 6 experiment framework
"""

import argparse
import sys
import os
import json
import time
from typing import Dict, Any, Optional, List, Tuple
from pathlib import Path

# Add the NLM source to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from experiments.Phase6IntegratedExperiment import Phase6IntegratedExperiment, Phase6Config, Phase6IntegrationResult
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM modules not available: {e}")
    NLM_AVAILABLE = False

class NLMIntegrate:
    """Integration validation tool for NLM"""
    
    def __init__(self):
        self.config = {}
        self.integration_results = {}
    
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
    
    def verify_integration(self) -> Dict[str, Any]:
        """Verify Phase 6 integration"""
        print("=== Phase 6 Integration Verification ===")
        
        if not NLM_AVAILABLE:
            print("NLM integration system not available, performing basic verification")
            return self._basic_integration_verification()
        
        try {
            // Create experiment
            Phase6IntegratedExperiment experiment;
            
            // Run integration verification
            bool verification_result = experiment.verifyIntegration();
            
            results = {
                'verification_passed': verification_result,
                'timestamp': time.time(),
                'experiment_type': 'Phase6Integration'
            };
            
            if (verification_result) {
                std::cout << "✓ Integration verification PASSED" << std::endl;
            } else {
                std::cout << "✗ Integration verification FAILED" << std::endl;
            }
            
            self.integration_results['verification'] = results;
            return results;
            
        } catch (const std::exception& e) {
            std::cout << "Error during integration verification: " << e.what() << std::endl;
            return {{"verification_passed", false, "error", std::string(e.what())}};
        }
    }
    
    void runIntegrationTest(const Phase6Config& config) {
        """Run Phase 6 integration test"";
        std::cout << "=== Phase 6 Integration Test ===" << std::endl;
        
        try {
            // Create experiment
            Phase6IntegratedExperiment experiment;
            
            // Run integration test
            Phase6IntegrationResult result = experiment.run(config);
            
            results = {{"timestamp", time.time()}};
            
            // Extract results
            results["total_reward"] = result.totalReward;
            results["avg_firing_rate"] = result.avgFiringRate;
            results["memory_episodes_stored"] = result.memoryEpisodesStored;
            results["novelty_level"] = result.noveltyLevel;
            results["curiosity_level"] = result.curiosityLevel;
            results["dopamine_level"] = result.dopamineLevel;
            
            // Integration status
            results["memory_working_memory_integrated"] = result.memoryWorkingMemoryIntegrated;
            results["memory_episodic_memory_integrated"] = result.memoryEpisodicMemoryIntegrated;
            results["neuromodulation_integrated"] = result.neuromodulationIntegrated;
            results["prediction_integrated"] = result.predictionIntegrated;
            results["development_integrated"] = result.developmentIntegrated;
            results["checkpointing_works"] = result.checkpointingWorks;
            results["replay_works"] = result.replayWorks;
            
            results["total_wall_clock_time"] = result.totalWallClockTime;
            results["total_reward_per_step"] = result.totalReward / result.totalWallClockTime;
            
            self.integration_results["test"] = results;
            
            // Print results
            std::cout << "=== Integration Test Results ===" << std::endl;
            std::cout << "Total reward: " << result.totalReward << std::endl;
            std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
            std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
            std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            
            std::cout << "=== Integration Status ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error during integration test: " << e.what() << std::endl;
            throw;
        }
    }
    
    void runQuickTest() {
        """Run quick integration test"";
        std::cout << "=== Quick Integration Test ===" << std::endl;
        
        // Create minimal config
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        // Run test
        runIntegrationTest(config);
    }
    
    void runComprehensiveTest() {
        """Run comprehensive integration test"";
        std::cout << "=== Comprehensive Integration Test ===" << std::endl;
        
        // Create larger config
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Run test
        runIntegrationTest(config);
    }
    
    void generateIntegrationReport(const std::string& outputFile) {
        """Generate integration report"";
        std::cout << "=== Generating Integration Report ===" << std::endl;
        
        // Collect all integration results
        std::map<std::string, std::map<std::string, double>> allResults;
        
        if (self.integration_results.find("verification") != self.integration_results.end()) {
            allResults["verification"] = self.integration_results.at("verification");
        }
        
        if (self.integration_results.find("test") != self.integration_results.end()) {
            allResults["test"] = self.integration_results.at("test");
        }
        
        // Generate report
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "NLM Integration Report" << std::endl;
            file << "===================" << std::endl;
            file << "Generated on: " << getCurrentTimestamp() << std::endl;
            file << std::endl;
            
            for (const auto& category : allResults) {
                file << "Category: " << category.first << std::endl;
                file << "-" * 20 << std::endl;
                
                for (const auto& result : category.second) {
                    file << "  " << result.first << ": " << result.second << std::endl;
                }
                
                file << std::endl;
            }
            
            file.close();
            std::cout << "Integration report saved to: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Failed to open output file: " << outputFile << std::endl;
        }
    }
    
    void healthCheck() {
        """Perform integration health check"";
        std::cout << "=== Integration Health Check ===" << std::endl;
        
        // Basic system health checks
        bool config_loaded = !self.config.empty();
        bool experiment_available = NLM_AVAILABLE;
        
        std::cout << "Configuration loaded: " << (config_loaded ? "Yes" : "No") << std::endl;
        std::cout << "Experiment framework available: " << (experiment_available ? "Yes" : "No") << std::endl;
        
        if (self.integration_results.empty()) {
            std::cout << "Integration results: None (run tests first)" << std::endl;
        } else {
            std::cout << "Integration results: Available" << std::endl;
            
            // Check verification status
            if (self.integration_results.find("verification") != self.integration_results.end()) {
                bool verification_passed = self.integration_results.at("verification").at("verification_passed");
                std::cout << "Integration verification: " << (verification_passed ? "PASSED" : "FAILED") << std::endl;
            }
            
            // Check test status
            if (self.integration_results.find("test") != self.integration_results.end()) {
                std::map<std::string, double> test_results = self.integration_results.at("test");
                
                bool memory_working = test_results.at("memory_working_memory_integrated");
                bool neuromodulation = test_results.at("neuromodulation_integrated");
                bool prediction = test_results.at("prediction_integrated");
                
                std::cout << "Memory systems integrated: " << (memory_working ? "Yes" : "No") << std::endl;
                std::cout << "Neuromodulation integrated: " << (neuromodulation ? "Yes" : "No") << std::endl;
                std::cout << "Prediction integrated: " << (prediction ? "Yes" : "No") << std::endl;
            }
        }
        
        // Overall health
        bool overall_health = config_loaded && experiment_available;
        std::cout << std::endl;
        std::cout << "Overall integration health: " << (overall_health ? "HEALTHY" : "UNHEALTHY") << std::endl;
    }
    
    std::map<std::string, double> _basic_integration_verification() {
        """Basic integration verification (when NLM not available)""";
        std::cout << "Performing basic integration verification..." << std::endl;
        
        std::map<std::string, double> results;
        
        // Check critical parameters
        std::vector<std::string> critical_params = {"neuron_count", "region_count", "connection_probability"};
        
        bool all_params_present = true;
        for (const auto& param : critical_params) {
            if (self.config.find(param) == self.config.end()) {
                std::cout << "Warning: Critical parameter not found: " << param << std::endl;
                all_params_present = false;
            }
        }
        
        results["critical_params_present"] = all_params_present ? 1.0 : 0.0;
        results["neuron_count"] = self.config.at("neuron_count", 0.0);
        results["region_count"] = self.config.at("region_count", 0.0);
        results["connection_probability"] = self.config.at("connection_probability", 0.0);
        results["verification_passed"] = all_params_present ? 1.0 : 0.0;
        
        return results;
    }
    
    std::string getCurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }
};

void printHelp() {
    std::cout << "NLMIntegrate - Integration Validation Tool" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "USAGE: nlmintegrate [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "  Advanced integration validation for NLM (Neural Learning Machine)." << std::endl;
    std::cout << "  Supports Phase 6 integration verification, testing automation, and reporting." << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --verify              Verify Phase 6 integration" << std::endl;
    std::cout << "  --quick-test          Run quick integration test" << std::endl;
    std::cout << "  --comprehensive-test  Run comprehensive integration test" << std::endl;
    std::cout << "  --all-tests           Run all integration tests" << std::endl;
    std::cout << "  --health-check        Perform integration health check" << std::endl;
    std::cout << "  --generate-report FILE Generate integration report" << std::endl;
    std::cout << "  --help, -h            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlmintegrate --verify configs/default.cfg" << std::endl;
    std::cout << "  nlmintegrate --quick-test configs/default.cfg" << std::endl;
    std::cout << "  nlmintegrate --comprehensive-test configs/default.cfg" << std::endl;
    std::cout << "  nlmintegrate --all-tests configs/default.cfg" << std::endl;
    std::cout << "  nlmintegrate --health-check configs/default.cfg" << std::endl;
    std::cout << "  nlmintegrate --generate-report report.txt configs/default.cfg" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Create integrator
        NLMIntegrate integrator;
        
        // Parse command line arguments (simplified)
        std::string configFile;
        bool verify = false;
        bool quickTest = false;
        bool comprehensiveTest = false;
        bool allTests = false;
        bool healthCheck = false;
        std::string generateReport;
        bool help = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                help = true;
            } else if (arg == "--verify") {
                verify = true;
            } else if (arg == "--quick-test") {
                quickTest = true;
            } else if (arg == "--comprehensive-test") {
                comprehensiveTest = true;
            } else if (arg == "--all-tests") {
                allTests = true;
            } else if (arg == "--health-check") {
                healthCheck = true;
            } else if (arg == "--generate-report") {
                if (i + 1 < argc) {
                    generateReport = argv[++i];
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
                    integrator.config[key] = value;
                }
            }
            
            file.close();
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
        
        // Run tests
        if (verify) {
            auto verification_result = integrator.verifyIntegration();
            if (!verification_result.at("verification_passed")) {
                return 1;
            }
        }
        
        if (quickTest) {
            integrator.runQuickTest();
        }
        
        if (comprehensiveTest) {
            integrator.runComprehensiveTest();
        }
        
        if (allTests) {
            if (verify) {
                auto verification_result = integrator.verifyIntegration();
                if (!verification_result.at("verification_passed")) {
                    return 1;
                }
            }
            
            integrator.runQuickTest();
            integrator.runComprehensiveTest();
        }
        
        if (healthCheck) {
            integrator.healthCheck();
        }
        
        if (!generateReport.empty()) {
            integrator.generateIntegrationReport(generateReport);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 4;
    }
}
