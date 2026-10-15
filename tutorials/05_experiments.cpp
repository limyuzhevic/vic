// Tutorial 5: Creating Custom Experiments
#include "src/experiment/Experiment.hpp"
#include "src/experiment/ExperimentRunner.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

class CustomExperiment : public nlm::Experiment {
public:
    std::string getType() const override {
        return "custom";
    }
    
    bool initialize(const nlm::Config& config) override {
        // Initialize brain
        brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
        
        if (!brain->initialize()) {
            return false;
        }
        
        // Initialize experiment runner
        experiment_runner = std::make_shared<nlm::ExperimentRunner>();
        
        return true;
    }
    
    void runStep(float dt) override {
        brain->step(dt);
        
        // Collect experiment data
        ExperimentData data;
        data.timestamp = brain->getSimulationTime();
        data.neuronActivity = brain->getNeuronActivity();
        data.spikeCount = brain->getSpikeCount();
        data.memoryContent = brain->getMemoryContent();
        
        // Log experiment data
        experiment_runner->logData(data);
    }
    
    ExperimentData collectData() const override {
        ExperimentData data;
        data.timestamp = brain->getSimulationTime();
        data.neuronActivity = brain->getNeuronActivity();
        data.spikeCount = brain->getSpikeCount();
        data.memoryContent = brain->getMemoryContent();
        return data;
    }
    
    nlm::AnalysisResults analyze(const std::vector<ExperimentData>& data) override {
        nlm::AnalysisResults results;
        
        // Calculate average firing rate
        float total_spikes = 0.0f;
        float total_neurons = 0.0f;
        
        for (const auto& experiment_data : data) {
            total_spikes += experiment_data.spike_count;
            total_neurons += experiment_data.neuron_count;
        }
        
        if (data.size() > 0) {
            results.average_firing_rate = total_spikes / total_neurons;
            results.total_spike_count = static_cast<int>(total_spikes);
        }
        
        return results;
    }
    
    std::string generateReport(const nlm::AnalysisResults& results) override {
        std::stringstream report;
        
        report << "Custom Experiment Results" << std::endl;
        report << "=========================" << std::endl;
        report << "Average firing rate: " << results.average_firing_rate << std::endl;
        report << "Total spike count: " << results.total_spike_count << std::endl;
        
        return report.str();
    }
    
    void configure(const std::map<std::string, std::string>& parameters) override {
        for (const auto& param : parameters) {
            if (param.first == "neuron_count") {
                brain->updateConfig(param.first, std::stoi(param.second));
            } else if (param.first == "region_count") {
                brain->updateConfig(param.first, std::stoi(param.second));
            } else if (param.first == "connection_probability") {
                brain->updateConfig(param.first, std::stof(param.second));
            }
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::ExperimentRunner> experiment_runner;
};

int main() {
    std::cout << "Tutorial 5: Creating Custom Experiments" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Create custom experiment
    auto experiment = std::make_shared<CustomExperiment>();
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("plasticity_learning_rate", 0.01);
    
    // Initialize experiment
    if (!experiment->initialize(*config)) {
        std::cerr << "✗ Failed to initialize experiment!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Custom experiment initialized successfully!" << std::endl;
    
    // Run experiment
    if (!experiment->run(*config)) {
        std::cerr << "✗ Failed to run experiment!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Custom experiment completed successfully!" << std::endl;
    
    // Collect experiment results
    auto results = experiment->collectResults();
    
    // Analyze experiment results
    auto analysis = experiment->analyzeResults(results);
    
    // Generate experiment report
    std::string report = experiment->generateReport(analysis);
    std::cout << "Experiment Report:" << std::endl;
    std::cout << report << std::endl;
    
    return 0;
}