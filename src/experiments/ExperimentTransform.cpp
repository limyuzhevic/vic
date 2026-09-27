#include "ExperimentExporter.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

std::string transformExperimentToCSV(const Experiment& experiment, const ExportOptions& options) {
    std::ostringstream oss;
    
    if (options.prettyPrint) {
        oss << "Experiment: " << experiment.getName() << "\n";
        oss << "Description: " << experiment.getDescription() << "\n";
        oss << "Version: " << experiment.getVersion() << "\n";
        oss << "Seed: " << experiment.getSeed() << "\n";
        oss << "Start Step: " << experiment.getStartStep() << "\n";
        oss << "End Step: " << experiment.getEndStep() << "\n";
        oss << "\nMetrics:\n";
        
        for (const auto& metric : experiment.getMetrics()) {
            oss << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        if (options.includeStepData) {
            oss << "\nStep Data:\n";
            oss << "Step,Neurons,Synapses,FiringCount,AvgFiringRate\n";
            for (const auto& step : experiment.getStepData()) {
                oss << step.step << ","
                    << step.neuronCount << ","
                    << step.synapseCount << ","
                    << step.firingCount << ","
                    << std::fixed << std::setprecision(4) << step.avgFiringRate << "\n";
            }
        }
        
        if (options.includeMetadata) {
            oss << "\nNotes:\n";
            for (const auto& note : experiment.getNotes()) {
                oss << "  - " << note << "\n";
            }
        }
    } else {
        // Compact format
        oss << "Experiment:" << experiment.getName() << ",";
        oss << "Description:" << experiment.getDescription() << ",";
        oss << "Version:" << experiment.getVersion() << ",";
        oss << "Seed:" << experiment.getSeed() << ",";
        oss << "Metrics:";
        
        bool firstMetric = true;
        for (const auto& metric : experiment.getMetrics()) {
            if (!firstMetric) oss << ";";
            oss << metric.first << "=" << metric.second;
            firstMetric = false;
        }
        
        if (options.includeStepData) {
            oss << ",Steps:";
            firstMetric = true;
            for (const auto& step : experiment.getStepData()) {
                if (!firstMetric) oss << ";";
                oss << step.step << "(" << step.neuronCount << "," << step.synapseCount 
                    << "," << step.firingCount << "," << step.avgFiringRate << ")";
                firstMetric = false;
            }
        }
        
        oss << ";Timestamp:" << ExperimentExporter::getTimestamp(options.timestampFormat);
    }
    
    return oss.str();
}

std::string transformExperimentToJSON(const Experiment& experiment, const ExportOptions& options) {
    json jsonData;
    
    jsonData["experiment_name"] = experiment.getName();
    jsonData["description"] = experiment.getDescription();
    jsonData["version"] = experiment.getVersion();
    jsonData["seed"] = experiment.getSeed();
    jsonData["start_step"] = experiment.getStartStep();
    jsonData["end_step"] = experiment.getEndStep();
    
    if (options.includeMetadata && !experiment.getNotes().empty()) {
        jsonData["notes"] = experiment.getNotes();
    }
    
    if (options.includeMetrics && !experiment.getMetrics().empty()) {
        json jsonMetrics;
        for (const auto& metric : experiment.getMetrics()) {
            jsonMetrics[metric.first] = metric.second;
        }
        jsonData["metrics"] = jsonMetrics;
    }
    
    if (options.includeStepData && !experiment.getStepData().empty()) {
        json jsonSteps;
        for (size_t i = 0; i < experiment.getStepData().size(); ++i) {
            const auto& step = experiment.getStepData()[i];
            json jsonStep;
            jsonStep["step"] = step.step;
            jsonStep["neuron_count"] = step.neuronCount;
            jsonStep["synapse_count"] = step.synapseCount;
            jsonStep["firing_count"] = step.firingCount;
            jsonStep["avg_firing_rate"] = step.avgFiringRate;
            jsonSteps[i] = jsonStep;
        }
        jsonData["step_data"] = jsonSteps;
    }
    
    jsonData["export_timestamp"] = ExperimentExporter::getTimestamp(options.timestampFormat);
    
    return jsonData.dump(options.prettyPrint ? 2 : 0);
}

std::string transformExperimentToYAML(const Experiment& experiment, const ExportOptions& options) {
    YAML::Emitter yaml;
    
    yaml << YAML::BeginMap;
    
    yaml << YAML::Key << "experiment_name";
    yaml << YAML::Value << experiment.getName();
    
    yaml << YAML::Key << "description";
    yaml << YAML::Value << experiment.getDescription();
    
    yaml << YAML::Key << "version";
    yaml << YAML::Value << experiment.getVersion();
    
    yaml << YAML::Key << "seed";
    yaml << YAML::Value << experiment.getSeed();
    
    yaml << YAML::Key << "start_step";
    yaml << YAML::Value << experiment.getStartStep();
    
    yaml << YAML::Key << "end_step";
    yaml << YAML::Value << experiment.getEndStep();
    
    if (options.includeMetadata && !experiment.getNotes().empty()) {
        yaml << YAML::Key << "notes";
        yaml << YAML::Value << YAML::BeginSeq;
        for (const auto& note : experiment.getNotes()) {
            yaml << note;
        }
        yaml << YAML::EndSeq;
    }
    
    if (options.includeMetrics && !experiment.getMetrics().empty()) {
        yaml << YAML::Key << "metrics";
        yaml << YAML::Value << YAML::BeginMap;
        for (const auto& metric : experiment.getMetrics()) {
            yaml << YAML::Key << metric.first;
            yaml << YAML::Value << metric.second;
        }
        yaml << YAML::EndMap;
    }
    
    if (options.includeStepData && !experiment.getStepData().empty()) {
        yaml << YAML::Key << "step_data";
        yaml << YAML::Value << YAML::BeginSeq;
        for (const auto& step : experiment.getStepData()) {
            yaml << YAML::BeginMap;
            yaml << YAML::Key << "step";
            yaml << YAML::Value << step.step;
            yaml << YAML::Key << "neuron_count";
            yaml << YAML::Value << step.neuronCount;
            yaml << YAML::Key << "synapse_count";
            yaml << YAML::Value << step.synapseCount;
            yaml << YAML::Key << "firing_count";
            yaml << YAML::Value << step.firingCount;
            yaml << YAML::Key << "avg_firing_rate";
            yaml << YAML::Value << step.avgFiringRate;
            yaml << YAML::EndMap;
        }
        yaml << YAML::EndSeq;
    }
    
    yaml << YAML::Key << "export_timestamp";
    yaml << YAML::Value << ExperimentExporter::getTimestamp(options.timestampFormat);
    
    yaml << YAML::EndMap;
    
    return yaml.c_str();
}

} // namespace nlm
