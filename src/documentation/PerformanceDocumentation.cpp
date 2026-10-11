// PerformanceDocumentation.cpp implementation

#include "PerformanceDocumentation.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>

namespace nlm {
namespace documentation {

// Example implementations for missing methods

} // namespace documentation
} // namespace nlm

// Example usage and demonstration
void demonstrate_documentation() {
    std::cout << "=== NLM Performance Documentation Demonstration ===" << std::endl;
    
    // Generate documentation
    std::cout << "Generating full documentation..." << std::endl;
    std::string documentation = nlm::documentation::generate_full_documentation();
    
    // Print to file
    std::ofstream file("performance_documentation.md");
    if (file.is_open()) {
        file << documentation;
        file.close();
        std::cout << "Documentation saved to performance_documentation.md" << std::endl;
    }
    
    // Generate specific sections
    std::cout << std::endl << "Generating API documentation..." << std::endl;
    std::string api_doc = nlm::documentation::generate_api_documentation();
    std::cout << "API documentation generated (first 500 chars):" << std::endl;
    std::cout << api_doc.substr(0, 500) << "..." << std::endl;
    
    std::cout << std::endl << "Generating tutorial documentation..." << std::endl;
    std::string tutorial_doc = nlm::documentation::generate_tutorial_documentation();
    std::cout << "Tutorial documentation generated (first 500 chars):" << std::endl;
    std::cout << tutorial_doc.substr(0, 500) << "..." << std::endl;
}

// Command-line interface for documentation
void documentation_cli(int argc, char* argv[]) {
    std::cout << "NLM Documentation CLI" << std::endl;
    std::cout << "Usage: nlm_documentation [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --demo                    Run documentation demonstration" << std::endl;
    std::cout << "  --generate                Generate full documentation" << std::endl;
    std::cout << "  --api                     Generate API documentation" << std::endl;
    std::cout << "  --configuration           Generate configuration documentation" << std::endl;
    std::cout << "  --tutorial                Generate tutorial documentation" << std::endl;
    std::cout << "  --examples                Generate example documentation" << std::endl;
    std::cout << "  --best-practices          Generate best practice documentation" << std::endl;
    std::cout << "  --setup                  Generate setup documentation" << std::endl;
    std::cout << "  --troubleshooting         Generate troubleshooting documentation" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_documentation();
        } else if (arg == "--generate") {
            std::cout << nlm::documentation::generate_full_documentation();
        } else if (arg == "--api") {
            std::cout << nlm::documentation::generate_api_documentation();
        } else if (arg == "--configuration") {
            std::cout << nlm::documentation::generate_configuration_documentation();
        } else if (arg == "--tutorial") {
            std::cout << nlm::documentation::generate_tutorial_documentation();
        } else if (arg == "--examples") {
            std::cout << nlm::documentation::generate_example_documentation();
        } else if (arg == "--best-practices") {
            std::cout << nlm::documentation::generate_best_practice_documentation();
        } else if (arg == "--setup") {
            std::cout << nlm::documentation::generate_setup_documentation();
        } else if (arg == "--troubleshooting") {
            std::cout << nlm::documentation::generate_troubleshooting_documentation();
        }
    }
}