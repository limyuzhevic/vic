Help system for comprehensive command-line interface

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm::cli {

class HelpSystem {
private:
    std::string program_name;
    std::string program_description;
    std::string version;
    std::string license;
    std::map<std::string, std::string> examples;
    std::map<std::string, std::vector<std::string>> command_groups;
    std::map<std::string, std::string> command_descriptions;
    std::map<std::string, std::string> command_parameters;
    std::map<std::string, std::vector<std::string>> command_examples;
    std::shared_ptr<Config> config;
    
public:
    HelpSystem() : version("1.0.0"), license("MIT") {}
    
    void setProgramInfo(const std::string& name, const std::string& desc, const std::string& ver = "1.0.0") {
        program_name = name;
        program_description = desc;
        version = ver;
    }
    
    void setLicense(const std::string& lic) {
        license = lic;
    }
    
    void addCommand(const std::string& group, const std::string& command,
                   const std::string& description, const std::string& parameters = "") {
        command_groups[group].push_back(command);
        command_descriptions[command] = description;
        command_parameters[command] = parameters;
    }
    
    void addExample(const std::string& category, const std::string& example) {
        examples[category] = example;
    }
    
    void addExample(const std::string& command, const std::string& example) {
        command_examples[command].push_back(example);
    }
    
    void setConfig(std::shared_ptr<Config> cfg) {
        config = cfg;
    }
    
    void showHelp() const {
        std::cout << std::endl;
        printBanner();
        std::cout << std::endl;
        
        // Program overview
        std::cout << program_name << " " << version << std::endl;
        std::cout << program_description << std::endl;
        std::cout << std::endl;
        
        // Usage
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program_name;
        
        // Common options
        std::cout << " [--config <file>] [--output <file>] [--verbose] [--quiet]";
        std::cout << " [--help] [--version]" << std::endl;
        std::cout << std::endl;
        
        // Command groups
        if (!command_groups.empty()) {
            std::cout << "Commands:" << std::endl;
            for (const auto& group : command_groups) {
                std::cout << "  " << group.first << ":" << std::endl;
                for (const auto& command : group.second) {
                    std::cout << "    " << command;
                    
                    // Print command description
                    auto desc_it = command_descriptions.find(command);
                    if (desc_it != command_descriptions.end()) {
                        std::cout << " - " << desc_it->second;
                        
                        // Print parameters if available
                        auto param_it = command_parameters.find(command);
                        if (param_it != command_parameters.end() && !param_it->second.empty()) {
                            std::cout << " [" << param_it->second << "]";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Examples
        if (!examples.empty()) {
            std::cout << "Examples:" << std::endl;
            for (const auto& example : examples) {
                std::cout << "  " << example.first << ":" << std::endl;
                std::cout << "    $ " << program_name << " " << example.second << std::endl;
                std::cout << std::endl;
            }
        }
        
        // Command-specific examples
        if (!command_examples.empty()) {
            std::cout << "Command Examples:" << std::endl;
            for (const auto& cmd_examples : command_examples) {
                std::cout << "  " << cmd_examples.first << ":" << std::endl;
                for (const auto& example : cmd_examples.second) {
                    std::cout << "    $ " << program_name << " " << example << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Configuration options
        std::cout << "Configuration Options:" << std::endl;
        std::cout << "  --config <file>    Load configuration from file (JSON, YAML, or key=value)" << std::endl;
        std::cout << "  --output <file>    Output results to file" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --quiet             Suppress non-essential output" << std::endl;
        std::cout << std::endl;
        
        // Environment variables
        std::cout << "Environment Variables:" << std::endl;
        std::cout << "  NLM_CONFIG         Path to default configuration file" << std::endl;
        std::cout << "  NLM_LOG_LEVEL      Logging level (debug, info, warning, error)" << std::endl;
        std::cout << "  NLM_SEED           Random seed for reproducible results" << std::endl;
        std::cout << std::endl;
        
        // License
        std::cout << "License:" << std::endl;
        std::cout << "  " << license << std::endl;
        std::cout << std::endl;
        
        // Additional help
        showAdditionalHelp();
    }
    
    void showVersion() const {
        std::cout << program_name << " " << version << std::endl;
        std::cout << std::endl;
        std::cout << program_description << std::endl;
        std::cout << std::endl;
        std::cout << "License: " << license << std::endl;
    }
    
    void showConfigHelp() const {
        std::cout << program_name << " Configuration Help" << std::endl;
        std::cout << "=" << std::string(40, '=') << std::endl;
        std::cout << std::endl;
        
        if (config) {
            std::cout << config->help() << std::endl;
        } else {
            std::cout << "Configuration not available." << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Configuration file formats:" << std::endl;
        std::cout << "  JSON:  config.json (with .json extension)" << std::endl;
        std::cout << "  YAML:  config.yaml or config.yml" << std::endl;
        std::cout << "  Plain: config.txt or .cfg (key=value format)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Example configuration snippets:" << std::endl;
        std::cout << std::endl;
        std::cout << "JSON example:" << std::endl;
        std::cout << "{\n";
        std::cout << "  \"brain\": {\n";
        std::cout << "    \"neuron_count\": 1000,\n";
        std::cout << "    \"connection_probability\": 0.1\n";
        std::cout << "  },\n";
        std::cout << "  \"plasticity\": {\n";
        std::cout << "    \"stdp_ltp_weight\": 0.01,\n";
        std::cout << "    \"stdp_ltd_weight\": 0.012\n";
        std::cout << "  }\n";
        std::cout << "}" << std::endl;
        std::cout << std::endl;
        
        std::cout << "YAML example:" << std::endl;
        std::cout << "brain:\n";
        std::cout << "  neuron_count: 1000\n";
        std::cout << "  connection_probability: 0.1\n";
        std::cout << "plasticity:\n";
        std::cout << "  stdp_ltp_weight: 0.01\n";
        std::cout << "  stdp_ltd_weight: 0.012" << std::endl;
    }
    
    void showErrorHelp(const std::string& error) const {
        std::cout << "Error: " << error << std::endl;
        std::cout << std::endl;
        std::cout << "Try running with --help for usage information." << std::endl;
    }
    
protected:
    virtual void showAdditionalHelp() const {
        std::cout << "Additional Resources:" << std::endl;
        std::cout << "  Documentation: https://nlm.ai/docs" << std::endl;
        std::cout << "  GitHub:        https://github.com/nlm/nlm" << std::endl;
        std::cout << "  Issues:        https://github.com/nlm/nlm/issues" << std::endl;
        std::cout << std::endl;
    }
    
private:
    void printBanner() const {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements real spiking neural computation.      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        )" << std::endl;
    }
};