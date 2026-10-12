#include "ExperimentResults.hpp"
#include <sstream>
#include <iomanip>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace nlm {

// Generic ExperimentResults implementation
template<typename T>
void ExperimentResults::setMetric(const std::string& name, const T& value) {
    metrics[name] = static_cast<double>(value);
}

template<typename T>
T ExperimentResults::getMetric(const std::string& name) const {
    auto it = metrics.find(name);
    if (it != metrics.end()) {
        return static_cast<T>(it->second);
    }
    return T();
}

bool ExperimentResults::hasMetric(const std::string& name) const {
    return metrics.find(name) != metrics.end();
}

std::string ExperimentResults::toCSV() const {
    std::ostringstream oss;
    oss << "timestamp,experiment_name,seed,description\n";
    oss << timestamp << "," << experimentName << "," << seed << "," << description << "\n\n";
    
    if (!config.empty()) {
        oss << "config\n";
        for (const auto& pair : config) {
            oss << pair.first << "," << pair.second << "\n";
        }
        oss << "\n";
    }
    
    if (!stepData.empty()) {
        oss << "step_data\n";
        for (const auto& pair : stepData) {
            oss << pair.first << "," << pair.second << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ExperimentResults::toJSON() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.Key("timestamp"); writer.String(timestamp.c_str());
    writer.Key("experimentName"); writer.String(experimentName.c_str());
    writer.Key("seed"); writer.Uint64(seed);
    writer.Key("description"); writer.String(description.c_str());
    
    if (!metrics.empty()) {
        writer.Key("metrics");
        writer.StartObject();
        for (const auto& pair : metrics) {
            writer.Key(pair.first.c_str());
            writer.Double(pair.second);
        }
        writer.EndObject();
    }
    
    writer.EndObject();
    
    return buffer.GetString();
}

bool ExperimentResults::isValid() const {
    return !experimentName.empty() && seed > 0;
}

void ExperimentResults::reset() {
    timestamp.clear();
    experimentName.clear();
    seed = 0;
    description.clear();
    metrics.clear();
    config.clear();
    notes.clear();
    stepData.clear();
    
    if (!std::is_same<T, Phase3Results>::value && !std::is_same<T, Phase4Results>::value) {
        // This is the base class, reset should not be called directly
        // but subclasses should override
    }
}

// Phase3Results implementation
std::string Phase3Results::toCSV() const {
    std::ostringstream oss;
    oss << ExperimentResults::toCSV(); // Include header
    oss << "totalReward,steps,finalNovelty,finalCuriosity,predictionError,developmentalAge,developmentalStage,totalSynapses,averageWeight,maxWeight,totalSpikes,averageFiringRate,forwardCount,backwardCount,turnLeftCount,turnRightCount,interactCount,waitCount\n";
    oss << totalReward << "," << steps << "," << finalNovelty << "," << finalCuriosity << ","
        << predictionError << "," << developmentalAge << "," << developmentalStage << ","
        << totalSynapses << "," << averageWeight << "," << maxWeight << ","
        << totalSpikes << "," << averageFiringRate << ","
        << forwardCount << "," << backwardCount << "," << turnLeftCount << "," << turnRightCount << ","
        << interactCount << "," << waitCount << "\n";
    return oss.str();
}

std::string Phase3Results::toJSON() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.Key("base");
    writer.StartObject();
    writer.Key("timestamp"); writer.String(timestamp.c_str());
    writer.Key("experimentName"); writer.String(experimentName.c_str());
    writer.Key("seed"); writer.Uint64(seed);
    writer.Key("description"); writer.String(description.c_str());
    writer.EndObject();
    
    writer.Key("specific");
    writer.StartObject();
    writer.Key("totalReward"); writer.Double(totalReward);
    writer.Key("steps"); writer.Int(steps);
    writer.Key("finalNovelty"); writer.Double(finalNovelty);
    writer.Key("finalCuriosity"); writer.Double(finalCuriosity);
    writer.Key("predictionError"); writer.Double(predictionError);
    writer.Key("developmentalAge"); writer.Double(developmentalAge);
    writer.Key("developmentalStage"); writer.String(developmentalStage.c_str());
    writer.Key("totalSynapses"); writer.Uint64(totalSynapses);
    writer.Key("averageWeight"); writer.Double(averageWeight);
    writer.Key("maxWeight"); writer.Double(maxWeight);
    writer.Key("totalSpikes"); writer.Uint64(totalSpikes);
    writer.Key("averageFiringRate"); writer.Double(averageFiringRate);
    writer.Key("forwardCount"); writer.Int(forwardCount);
    writer.Key("backwardCount"); writer.Int(backwardCount);
    writer.Key("turnLeftCount"); writer.Int(turnLeftCount);
    writer.Key("turnRightCount"); writer.Int(turnRightCount);
    writer.Key("interactCount"); writer.Int(interactCount);
    writer.Key("waitCount"); writer.Int(waitCount);
    writer.EndObject();
    
    writer.EndObject();
    
    return buffer.GetString();
}

void Phase3Results::reset() {
    ExperimentResults::reset();
    totalReward = 0.0f;
    steps = 0;
    finalNovelty = 0.0f;
    finalCuriosity = 0.0f;
    predictionError = 0.0f;
    developmentalAge = 0.0f;
    developmentalStage = "Initial";
    totalSynapses = 0;
    averageWeight = 0.0f;
    maxWeight = 0.0f;
    totalSpikes = 0;
    averageFiringRate = 0.0f;
    forwardCount = 0;
    backwardCount = 0;
    turnLeftCount = 0;
    turnRightCount = 0;
    interactCount = 0;
    waitCount = 0;
}

// Phase4Results implementation
std::string Phase4Results::toCSV() const {
    std::ostringstream oss;
    oss << ExperimentResults::toCSV(); // Include header
    oss << "predictionErrorInitial,predictionErrorFinal,predictionAccuracyImprovement,workingMemoryRetentionInitial,workingMemoryRetentionFinal,workingMemoryCapacity,episodicRecallAccuracy,episodesStored,episodicInfluence,associationStrength,associationTransfer,conceptsFormed,conceptStability,generalizationAbility,attentionSelectivity,distractionResistance,planningAccuracy,multiStepSuccess,planningConfidence,selfPredictionAccuracy,bodyAwareness,selfOtherDistinction,imitationAccuracy,socialPredictionAccuracy,observationsFromOthers,totalReward,behaviorImprovement,transferPerformance\n";
    oss << predictionErrorInitial << "," << predictionErrorFinal << "," << predictionAccuracyImprovement << "," << workingMemoryRetentionInitial << "," << workingMemoryRetentionFinal << "," << workingMemoryCapacity << "," << episodicRecallAccuracy << "," << episodesStored << "," << episodicInfluence << "," << associationStrength << "," << associationTransfer << "," << conceptsFormed << "," << conceptStability << "," << generalizationAbility << "," << attentionSelectivity << "," << distractionResistance << "," << planningAccuracy << "," << multiStepSuccess << "," << planningConfidence << "," << selfPredictionAccuracy << "," << bodyAwareness << "," << selfOtherDistinction << "," << imitationAccuracy << "," << socialPredictionAccuracy << "," << observationsFromOthers << "," << totalReward << "," << behaviorImprovement << "," << transferPerformance << "\n";
    return oss.str();
}

std::string Phase4Results::toJSON() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.Key("base");
    writer.StartObject();
    writer.Key("timestamp"); writer.String(timestamp.c_str());
    writer.Key("experimentName"); writer.String(experimentName.c_str());
    writer.Key("seed"); writer.Uint64(seed);
    writer.Key("description"); writer.String(description.c_str());
    writer.EndObject();
    
    writer.Key("specific");
    writer.StartObject();
    writer.Key("predictionErrorInitial"); writer.Double(predictionErrorInitial);
    writer.Key("predictionErrorFinal"); writer.Double(predictionErrorFinal);
    writer.Key("predictionAccuracyImprovement"); writer.Double(predictionAccuracyImprovement);
    writer.Key("workingMemoryRetentionInitial"); writer.Double(workingMemoryRetentionInitial);
    writer.Key("workingMemoryRetentionFinal"); writer.Double(workingMemoryRetentionFinal);
    writer.Key("workingMemoryCapacity"); writer.Double(workingMemoryCapacity);
    writer.Key("episodicRecallAccuracy"); writer.Double(episodicRecallAccuracy);
    writer.Key("episodesStored"); writer.Uint64(episodesStored);
    writer.Key("episodicInfluence"); writer.Double(episodicInfluence);
    writer.Key("associationStrength"); writer.Double(associationStrength);
    writer.Key("associationTransfer"); writer.Double(associationTransfer);
    writer.Key("conceptsFormed"); writer.Uint64(conceptsFormed);
    writer.Key("conceptStability"); writer.Double(conceptStability);
    writer.Key("generalizationAbility"); writer.Double(generalizationAbility);
    writer.Key("attentionSelectivity"); writer.Double(attentionSelectivity);
    writer.Key("distractionResistance"); writer.Double(distractionResistance);
    writer.Key("planningAccuracy"); writer.Double(planningAccuracy);
    writer.Key("multiStepSuccess"); writer.Double(multiStepSuccess);
    writer.Key("planningConfidence"); writer.Double(planningConfidence);
    writer.Key("selfPredictionAccuracy"); writer.Double(selfPredictionAccuracy);
    writer.Key("bodyAwareness"); writer.Double(bodyAwareness);
    writer.Key("selfOtherDistinction"); writer.Double(selfOtherDistinction);
    writer.Key("imitationAccuracy"); writer.Double(imitationAccuracy);
    writer.Key("socialPredictionAccuracy"); writer.Double(socialPredictionAccuracy);
    writer.Key("observationsFromOthers"); writer.Uint64(observationsFromOthers);
    writer.Key("totalReward"); writer.Double(totalReward);
    writer.Key("behaviorImprovement"); writer.Double(behaviorImprovement);
    writer.Key("transferPerformance"); writer.Double(transferPerformance);
    writer.EndObject();
    
    writer.EndObject();
    
    return buffer.GetString();
}

void Phase4Results::reset() {
    ExperimentResults::reset();
    predictionErrorInitial = 0.0f;
    predictionErrorFinal = 0.0f;
    predictionAccuracyImprovement = 0.0f;
    workingMemoryRetentionInitial = 0.0f;
    workingMemoryRetentionFinal = 0.0f;
    workingMemoryCapacity = 0.0f;
    episodicRecallAccuracy = 0.0f;
    episodesStored = 0;
    episodicInfluence = 0.0f;
    associationStrength = 0.0f;
    associationTransfer = 0.0f;
    conceptsFormed = 0;
    conceptStability = 0.0f;
    generalizationAbility = 0.0f;
    attentionSelectivity = 0.0f;
    distractionResistance = 0.0f;
    planningAccuracy = 0.0f;
    multiStepSuccess = 0.0f;
    planningConfidence = 0.0f;
    selfPredictionAccuracy = 0.0f;
    bodyAwareness = 0.0f;
    selfOtherDistinction = 0.0f;
    imitationAccuracy = 0.0f;
    socialPredictionAccuracy = 0.0f;
    observationsFromOthers = 0;
    totalReward = 0.0f;
    behaviorImprovement = 0.0f;
    transferPerformance = 0.0f;
}

} // namespace nlm
