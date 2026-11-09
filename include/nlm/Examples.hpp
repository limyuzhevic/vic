# NLM Examples and Educational Materials
# Comprehensive examples, tutorials, and documentation for using NLM

#ifndef NLM_EXAMPLES_HPP
#define NLM_EXAMPLES_HPP

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Example 1: Basic Brain Setup
void example_basic_brain_setup();

// Example 2: Simulation Loop
void example_simulation_loop();

// Example 3: Memory Integration
void example_memory_integration();

// Example 4: Neuromodulation
void example_neuromodulation();

// Example 5: Development Stages
void example_development_stages();

// Example 6: Checkpointing
void example_checkpointing();

// Example 7: Complete Integration
void example_complete_integration();

// Example 8: Performance Comparison
void example_performance_comparison();

// Example 9: Configuration Management
void example_configuration_management();

// Example 10: Advanced Debugging
void example_advanced_debugging();

// Tutorial: Getting Started
class NLMGettingStarted {
public:
    static void tutorial_basic_usage();
    static void tutorial_configuration();
    static void tutorial_simulation();
    static void tutorial_memory();
};

// Tutorial: Intermediate
class NLMIntermediate {
public:
    static void tutorial_neuromodulation();
    static void tutorial_development();
    static void tutorial_advanced_features();
};

// Tutorial: Advanced
class NLMAdvanced {
public:
    static void tutorial_custom_networks();
    static void tutorial_optimization();
    static void tutorial_research_experiments();
};

// Example configurations
class ExampleConfigurations {
public:
    static std::shared_ptr<Config> create_basic_config();
    static std::shared_ptr<Config> create_advanced_config();
    static std::shared_ptr<Config> create_research_config();
    static std::shared_ptr<Config> create_performance_config();
};

// Example output generators
class ExampleOutput {
public:
    static std::string generate_basic_example_output();
    static std::string generate_neuromodulation_example_output();
    static std::string generate_development_example_output();
    static std::string generate_memory_example_output();
    static std::string generate_performance_example_output();
};

// Example validation
class ExampleValidator {
public:
    static bool validate_example_1();
    static bool validate_example_2();
    static bool validate_example_3();
    static bool validate_example_4();
    static bool validate_example_5();
    static bool validate_example_6();
    static bool validate_example_7();
    static bool validate_example_8();
    static bool validate_example_9();
    static bool validate_example_10();
};

// Complete example runner
class ExampleRunner {
public:
    enum class ExampleType {
        BasicSetup,
        SimulationLoop,
        MemoryIntegration,
        Neuromodulation,
        DevelopmentStages,
        Checkpointing,
        CompleteIntegration,
        PerformanceComparison,
        ConfigurationManagement,
        AdvancedDebugging
    };
    
    static bool run_example(ExampleType type);
    static bool run_all_examples();
    static bool run_example_range(ExampleType start, ExampleType end);
    static std::vector<ExampleType> get_available_examples();
};

// Documentation generator
class DocumentationGenerator {
public:
    static std::string generate_markdown_documentation();
    static std::string generate_html_documentation();
    static std::string generate_latex_documentation();
    static std::string generate_python_documentation();
};

// Example metadata
struct ExampleMetadata {
    std::string name;
    std::string description;
    std::string difficulty;
    std::string category;
    std::vector<std::string> prerequisites;
    std::vector<std::string> learningObjectives;
    double estimatedTime;
    std::string outputFormat;
    bool isValidated;
};

// Example database
class ExampleDatabase {
private:
    std::vector<ExampleMetadata> examples;
    
public:
    ExampleDatabase();
    void add_example(const ExampleMetadata& metadata);
    ExampleMetadata* get_example(const std::string& name);
    std::vector<ExampleMetadata> get_examples_by_difficulty(const std::string& difficulty);
    std::vector<ExampleMetadata> get_examples_by_category(const std::string& category);
    std::vector<ExampleMetadata> get_all_examples();
    size_t get_example_count() const;
    bool save_to_file(const std::string& filepath) const;
    bool load_from_file(const std::string& filepath);
};

// Global example database instance
extern ExampleDatabase g_exampleDatabase;

// Convenience macros
#define NLM_EXAMPLE_RUN(name) nlm::ExampleRunner::run_example(nlm::ExampleRunner::ExampleType::name)
#define NLM_EXAMPLE_VALIDATE(name) nlm::ExampleValidator::name()

} // namespace nlm

#endif // NLM_EXAMPLES_HPP
