// Brain Modularity Demonstration
// This file demonstrates the improved modularity of the Brain class
// by showing how processing steps can be organized and executed independently

#include "BrainProcessingPipeline.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {
namespace modularity_demo {

// Demonstration of processing pipeline usage
void demonstrateProcessingPipeline(Brain* brain) {
    std::cout << "=== Brain Processing Pipeline Demonstration ===" << std::endl;
    std::cout << "This demonstrates how the Brain class can be made more modular" << std::endl;
    std::cout << "using the processing pipeline pattern." << std::endl;
    std::cout << std::endl;
    
    // Create a processing pipeline
    DefaultBrainProcessingPipeline pipeline;
    
    // Show pipeline statistics
    std::cout << pipeline.getPipelineStatistics() << std::endl;
    
    // Get all step names
    auto stepNames = pipeline.getStepNames();
    std::cout << "Available processing steps:" << std::endl;
    for (size_t i = 0; i < stepNames.size(); ++i) {
        std::cout << "  " << i+1 << ". " << stepNames[i] << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate executing a subset of steps
    std::cout << "Executing a subset of processing steps (first 5 steps):" << std::endl;
    
    std::vector<std::string> selectedSteps = {
        "ProcessDelayedSpikeEvents",
        "UpdateNeurons", 
        "DetectSpikes",
        "UpdateWorkingMemory",
        "ApplyNeuromodulation"
    };
    
    auto results = pipeline.executeStepSubset(
        selectedSteps,
        1,  // currentStep
        0.001,  // currentTime
        brain
    );
    
    // Display results
    std::cout << std::endl;
    std::cout << "Processing Step Results:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.stepName << " - " << result.duration.count() << "us" << std::endl;
        std::cout << "    Status: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
        if (!result.message.empty()) {
            std::cout << "    Message: " << result.message << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Calculate total time
    auto totalTime = std::accumulate(
        results.begin(), results.end(), std::chrono::microseconds(0),
        [](auto sum, const auto& result) {
            return sum + result.duration;
        }
    );
    
    std::cout << "Total processing time: " << totalTime.count() << "us" << std::endl;
    std::cout << std::endl;
    
    // Demonstrate adding a custom processing step
    std::cout << "Demonstrating custom processing step addition:" << std::endl;
    
    // Create a custom processing step function
    auto customStep = [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        return ProcessingStepResult{"CustomProcessingStep", std::chrono::microseconds(100), true, "Custom step executed successfully"};
    };
    
    pipeline.addProcessingStep("CustomProcessingStep", customStep);
    
    // Execute the custom step
    auto customResult = pipeline.executeStep(
        "CustomProcessingStep",
        1,
        0.001,
        brain
    );
    
    std::cout << "Custom step result: " << (customResult.success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== End of Brain Processing Pipeline Demonstration ===" << std::endl;
}

}} // namespace nlm::modularity_demo

namespace nlm {

// Function to demonstrate improved modularity
void demonstrateImprovedModularity() {
    std::cout << "=== NLM Brain Modularity Improvements ===" << std::endl;
    std::cout << std::endl;
    std::cout << "The Brain class has been improved with better modularity through:" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Processing Pipeline Pattern:" << std::endl;
    std::cout << "   - Each processing step is a separate method" << std::endl;
    std::cout << "   - Steps can be executed independently or in sequence" << std::endl;
    std::cout << "   - Custom steps can be added easily" << std::endl;
    std::cout << std::endl;
    std::cout << "2. Command Pattern:" << std::endl;
    std::cout << "   - Each processing step is a command object" << std::endl;
    std::cout << "   - Commands can be executed, queued, or skipped" << std::endl;
    std::cout << "   - Better error handling and isolation" << std::endl;
    std::cout << std::endl;
    std::cout << "3. Single Responsibility Principle:" << std::endl;
    std::cout << "   - Each method has a single, well-defined purpose" << std::endl;
    std::cout << "   - Easier to understand, test, and maintain" << std::endl;
    std::cout << "   - Reduced coupling between systems" << std::endl;
    std::cout << std::endl;
    std::cout << "4. Strategy Pattern:" << std::endl;
    std::cout << "   - Different processing strategies can be plugged in" << std::endl;
    std::cout << "   - Processing pipeline can be reordered" << std::endl;
    std::cout << "   - Custom pipelines can be created" << std::endl;
    std::cout << std::endl;
    std::cout << "These improvements make the Brain class more:
    - Modular: Each component has a single responsibility
    - Maintainable: Easier to understand and modify
    - Testable: Each step can be tested independently
    - Extensible: New processing steps can be added easily
    - Flexible: Different processing pipelines can be created
    === End of Modularity Improvements Demonstration ===" << std::endl;
}

}} // namespace nlm