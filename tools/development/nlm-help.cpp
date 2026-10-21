#!/usr/bin/env g++
// NLM Help Tool
// Comprehensive help documentation for NLM

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

using namespace nlm;

namespace fs = std::filesystem;

struct HelpTopic {
    std::string title;
    std::string content;
    std::vector<std::string> subtopics;
};

struct ExampleCommand {
    std::string command;
    std::string description;
    std::string output;
};

class NLMHelp {
private:
    std::map<std::string, HelpTopic> helpTopics;
    std::string helpFile;
    bool showExamples;
    bool showAdvanced;
    bool exportFormat;
    std::string exportFile;
    
public:
    NLMHelp() {
        helpFile = "help.md";
        showExamples = false;
        showAdvanced = false;
        exportFormat = false;
        exportFile = "";
        
        initializeHelpTopics();
    }
    
    void initializeHelpTopics() {
        // Main help topic
        helpTopics["main"] = {
            "NLM Command-Line Interface Help",
            "NLM (熙然) - Neural Learning Machine\n"
            "=====================================\n"
            "NLM is an experimental artificial developmental brain system.\n"
            "This document describes the command-line tools available for NLM.\n"
            "\n"
            "Getting Started:\n"
            "  - Run './nlm' for the main NLM simulation\n"
            "  - Run './nlm-help' for this help system\n"
            "  - Run './nlm-setup --all' to set up the environment\n"
            "  - Run './nlm-test --all' to run all tests\n"
            "\n"
            "Need more help?\n"
            "  - Check the documentation in the docs/ directory\n"
            "  - Use './nlm-info --help' for detailed options\n"
            "  - Use './nlm-validate --help' for validation options\n"
            "  - Use './nlm-config --help' for configuration options\n"
            "\n",
            {"basic", "advanced", "tools", "examples", "troubleshooting"}
        };
        
        // Basic tools help
        helpTopics["basic"] = {
            "Basic Command-Line Tools",
            "The following basic tools are available for configuration and validation:\n"
            "\n"
            "=== nlm-config ===\n"
            "Tool for creating and validating NLM configuration files.\n"
            "\n"
            "Options:\n"
            "  --create <file>          Create new configuration file\n"
            "  --validate <file>        Validate existing configuration file\n"
            "  --list-templates          List available configuration templates\n"
            "  --set <key>=<value>       Set configuration value\n"
            "  --remove <key>            Remove configuration key\n"
            "\n"
            "Examples:\n"
            "  nlm-config --create config.cfg\n"
            "  nlm-config --validate config.cfg\n"
            "  nlm-config --set random_seed=42\n"
            "  nlm-config --list-templates\n"
            "\n",
            {}
        };
        
        // Configuration template examples
        helpTopics["examples"] = {
            "Command-Line Examples",
            "Here are some useful examples for using NLM command-line tools:\n"
            "\n"
            "Example 1: Create a basic configuration file\n"
            "```bash\n"
            "nlm-config --create basic_config.cfg\n"
            "```\n"
            "\n"
            "Example 2: Validate a configuration file\n"
            "```bash\n"
            "nlm-config --validate basic_config.cfg\n"
            "```\n"
            "\n"
            "Example 3: Run NLM with custom configuration\n"
            "```bash\n"
            "./nlm --config my_config.cfg\n"
            "```\n"
            "\n"
            "Example 4: Run quick validation and tests\n"
            "```bash\n"
            "./nlm-validate --config config.cfg --brain-tests\n"
            "```\n"
            "\n"
            "Example 5: Save a simulation checkpoint\n"
            "```bash\n"
            "nlm-save --save \"Experiment checkpoint 1\"\n"
            "```\n"
            "\n"
            "Example 6: Load a simulation checkpoint\n"
            "```bash\n"
            "nlm-load --load checkpoint_20240101_120000.nckpt\n"
            "```\n"
            "\n"
            "Example 7: Run performance benchmarks\n"
            "```bash\n"
            "./nlm-benchmark --detailed --neurons 1000 --steps 100\n"
            "```\n"
            "\n"
            "Example 8: Monitor NLM in real-time\n"
            "```bash\n"
            "./nlm-monitor --brain NLM_Brain --interval 2.0\n"
            "```\n"
            "\n"
            "Example 9: Run all tests\n"
            "```bash\n"
            "./nlm-test --all --verbose\n"
            "```\n"
            "\n"
            "Example 10: Setup development environment\n"
            "```bash\n"
            "./nlm-setup --all\n"
            "```\n"
            "\n",
            {}
        };
        
        // Advanced tools help
        helpTopics["advanced"] = {
            "Advanced Command-Line Tools",
            "The following advanced tools provide additional capabilities:\n"
            "\n"
            "=== nlm-explore ===\n"
            "Interactive exploration of brain parameters and behavior.\n"
            "Allows real-time adjustment of neural parameters and observation of effects.\n"
            "\n"
            "=== nlm-plot ===\n"
            "Generate visualization plots from simulation data.\n"
            "Create graphs of neural activity, weight changes, and other metrics.\n"
            "\n"
            "=== nlm-compare ===\n"
            "Compare different simulation configurations.\n"
            "Run multiple configurations side-by-side and compare results.\n"
            "\n"
            "=== nlm-analyze ===\n"
            "Deep analysis of simulation results.\n"
            "Perform statistical analysis and extract insights from simulation data.\n"
            "\n"
            "=== nlm-export ===\n"
            "Export brain state for external analysis.\n"
            "Save brain state in formats compatible with other analysis tools.\n"
            "\n"
            "=== nlm-import ===\n"
            "Import external data into NLM.\n"
            "Load pre-trained models or external datasets into NLM.\n"
            "\n"
            "=== nlm-profiler ===\n"
            "Performance profiling and optimization.\n"
            "Identify performance bottlenecks and optimize NLM execution.\n"
            "\n"
            "=== nlm-monitor ===\n"
            "Real-time monitoring of simulation state.\n"
            "Monitor neural activity, plasticity, and other metrics in real-time.\n"
            "\n",
            {"exploration", "visualization", "comparison", "analysis", "export", "import", "profiling", "monitoring"}
        };
        
        // Development tools help
        helpTopics["tools"] = {
            "Development Tools",
            "Tools for development and testing:\n"
            "\n"
            "=== nlm-profiler ===\n"
            "Performance profiling and optimization. Identify bottlenecks and optimize code.\n"
            "\n"
            "=== nlm-monitor ===\n"
            "Real-time monitoring of simulation state. Track neural activity and metrics.\n"
            "\n"
            "=== nlm-test ===\n"
            "Run specific test suites. Test neural connectivity, plasticity, memory.\n"
            "\n"
            "=== nlm-setup ===\n"
            "Setup development environment. Configure build, install dependencies.\n"
            "\n"
            "=== nlm-help ===\n"
            "This comprehensive help documentation. Access help for all commands.\n"
            "\n"
            "=== Python Bindings ===\n"
            "Python interface for NLM. Use NLM from Python scripts.\n"
            "Available as 'import nlm' or 'from nlm import *'.\n"
            "\n"
            "=== Scripting Tools ===\n"
            "Python bindings for command-line integration.\n"
            "JSON configuration file generators.\n"
            "Simulation script templates.\n"
            "Experiment macros and presets.\n"
            "\n",
            {}
        };
        
        // Troubleshooting help
        helpTopics["troubleshooting"] = {
            "Troubleshooting",
            "Common issues and their solutions:\n"
            "\n"
            "Q: \"Cannot create executable files\"\n"
            "A: Check file permissions. On Linux/macOS, try: chmod +x *.cpp\n"
            "\n"
            "Q: \"Configuration file not found\"\n"
            "A: Use './nlm-config --create config.cfg' to create one first.\n"
            "\n"
            "Q: \"Program crashes with segmentation fault\"\n"
            "A: Ensure you have enough memory. Check for buffer overflows in code.\n"
            "\n"
            "Q: \"Too slow or unresponsive\"\n"
            "A: Run './nlm-profiler --detailed' to identify bottlenecks.\n"
            "\n"
            "Q: \"Python bindings not working\"\n"
            "A: Ensure pybind11 is installed. Install with: pip install pybind11.\n"
            "\n"
            "Q: \"Cannot compile tools\"\n"
            "A: Run './nlm-setup --all' to check environment and install dependencies.\n"
            "\n"
            "Q: \"Memory usage too high\"\n"
            "A: Reduce neuron count in configuration: nlm-config --set neuron_count=500\n"
            "\n"
            "Q: \"Need help with specific command\"\n"
            "A: Use './nlm-help <command>' for specific command help.\n"
            "\n"
            "Q: \"Running out of disk space\"\n"
            "A: Clean up checkpoint files: find . -name \"*.nckpt\" -delete\n"
            "\n"
            "Q: \"Cannot import external data\"\n"
            "A: Check file format. Use './nlm-import --help' for supported formats.\n"
            "\n",
            {}
        };
    }
    
    void printHelp(const std::string& topic = "main") {
        if (helpTopics.find(topic) == helpTopics.end()) {
            std::cout << "Error: Unknown help topic: " << topic << std::endl;
            printAvailableTopics();
            return;
        }
        
        const HelpTopic& topicInfo = helpTopics[topic];
        
        std::cout << "=== " << topicInfo.title << " ===" << std::endl;
        std::cout << topicInfo.content;
        
        if (!topicInfo.subtopics.empty()) {
            std::cout << "Subtopics:" << std::endl;
            for (const auto& subtopic : topicInfo.subtopics) {
                std::cout << "  " << subtopic << std::endl;
            }
            std::cout << "\nUse './nlm-help " << topic << "-subtopic' to see details for a specific subtopic." << std::endl;
        }
    }
    
    void printAvailableTopics() {
        std::cout << "=== Available Help Topics ===" << std::endl;
        std::cout << std::endl;
        
        int topicNum = 1;
        for (const auto& pair : helpTopics) {
            const std::string& topic = pair.first;
            const HelpTopic& topicInfo = pair.second;
            
            std::cout << topicNum << ". " << topicInfo.title << std::endl;
            
            if (showExamples) {
                std::cout << "   " << topicInfo.content.substr(0, 100) << "..." << std::endl;
            }
            
            topicNum++;
        }
        
        std::cout << std::endl;
        std::cout << "Use './nlm-help <topic>' to see details for a specific topic." << std::endl;
    }
    
    void printCommandHelp() {
        std::cout << "=== Command-Line Tool Help ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Basic Tools:" << std::endl;
        std::cout << "  nlm-config    - Configuration file creation and validation" << std::endl;
        std::cout << "  nlm-validate  - Validate configuration and test brain functionality" << std::endl;
        std::cout << "  nlm-info      - Display system information and capabilities" << std::endl;
        std::cout << "  nlm-benchmark - Performance benchmarking suite" << std::endl;
        std::cout << "  nlm-save      - Save simulation checkpoints" << std::endl;
        std::cout << "  nlm-load      - Load simulation checkpoints" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Advanced Tools:" << std::endl;
        std::cout << "  nlm-explore    - Interactive exploration of brain parameters" << std::endl;
        std::cout << "  nlm-plot      - Generate visualization plots" << std::endl;
        std::cout << "  nlm-compare   - Compare different simulation configurations" << std::endl;
        std::cout << "  nlm-analyze   - Deep analysis of simulation results" << std::endl;
        std::cout << "  nlm-export    - Export brain state for external analysis" << std::endl;
        std::cout << "  nlm-import    - Import external data into NLM" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Development Tools:" << std::endl;
        std::cout << "  nlm-profiler  - Performance profiling and optimization" << std::endl;
        std::cout << "  nlm-monitor   - Real-time monitoring of simulation state" << std::endl;
        std::cout << "  nlm-test      - Run specific test suites" << std::endl;
        std::cout << "  nlm-setup     - Setup development environment" << std::endl;
        std::cout << "  nlm-help      - This help documentation" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Scripting Tools:" << std::endl;
        std::cout << "  Python bindings for command-line integration" << std::endl;
        std::cout << "  JSON configuration file generators" << std::endl;
        std::cout << "  Simulation script templates" << std::endl;
        std::cout << "  Experiment macros and presets" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Usage: ./nlm-help [topic]" << std::endl;
        std::cout << "Topics: main, basic, advanced, tools, examples, troubleshooting" << std::endl;
    }
    
    void exportHelp(const std::string& filename) {
        std::cout << "=== Exporting Help Documentation ===" << std::endl;
        std::cout << "Exporting help to: " << filename << std::endl;
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# NLM Help Documentation" << std::endl;
            file << "# Generated: " << time(0) << std::endl;
            file << "# Format: Markdown" << std::endl;
            file << std::endl;
            
            for (const auto& pair : helpTopics) {
                const std::string& topic = pair.first;
                const HelpTopic& topicInfo = pair.second;
                
                file << "## " << topicInfo.title << "\n\n";
                file << topicInfo.content << "\n";
                
                if (!topicInfo.subtopics.empty()) {
                    file << "### Subtopics\n\n";
                    for (const auto& subtopic : topicInfo.subtopics) {
                        file << "- " << subtopic << "\n";
                    }
                    file << "\n";
                }
                
                file << "---\n\n";
            }
            
            file.close();
            std::cout << "Help documentation exported successfully!" << std::endl;
        } else {
            std::cerr << "Error: Cannot create help file: " << filename << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --topic <topic>           Show help for specific topic" << std::endl;
        std::cout << "  --examples                Show examples" << std::endl;
        std::cout << "  --advanced                Show advanced topics" << std::endl;
        std::cout << "  --tools                   Show tools information" << std::endl;
        std::cout << "  --list-topics             List available help topics" << std::endl;
        std::cout << "  --export <file>           Export help to file" << std::endl;
        std::cout << "  --file-format <format>    Export file format (md, txt, html)" << std::endl;
        std::cout << std::endl;
        std::cout << "Topics: main, basic, advanced, tools, examples, troubleshooting" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-help";
    std::string topic;
    bool showExamples = false;
    bool showAdvanced = false;
    bool showTools = false;
    bool listTopics = false;
    std::string exportFile;
    std::string exportFormat = "md";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMHelp help;
            help.printHelp();
            return 0;
        } else if (arg == "--topic") {
            if (i + 1 < argc) {
                topic = argv[++i];
            } else {
                std::cerr << "Error: --topic requires a topic argument" << std::endl;
                return 1;
            }
        } else if (arg == "--examples") {
            showExamples = true;
        } else if (arg == "--advanced") {
            showAdvanced = true;
        } else if (arg == "--tools") {
            showTools = true;
        } else if (arg == "--list-topics") {
            listTopics = true;
        } else if (arg == "--export") {
            if (i + 1 < argc) {
                exportFile = argv[++i];
            } else {
                std::cerr << "Error: --export requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--file-format") {
            if (i + 1 < argc) {
                exportFormat = argv[++i];
            } else {
                std::cerr << "Error: --file-format requires a format argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMHelp help;
    help.showExamples = showExamples;
    help.showAdvanced = showAdvanced;
    
    if (listTopics) {
        help.printAvailableTopics();
        return 0;
    }
    
    if (!exportFile.empty()) {
        help.exportHelp(exportFile);
        return 0;
    }
    
    if (!topic.empty()) {
        help.printHelp(topic);
    } else if (showTools) {
        help.printCommandHelp();
    } else {
        help.printHelp();
    }
    
    return 0;
}
