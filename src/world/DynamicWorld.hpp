// Dynamic world with environmental changes and evolution
#pragma once

#include "SimpleWorld.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

namespace nlm {

// Environmental factor that can change over time
struct EnvironmentalFactor {
    std::string factorId;
    std::string name;
    float currentValue;
    float minValue;
    float maxValue;
    float changeRate;
    float variability;
    std::string unit;
    std::string description;
    
    EnvironmentalFactor() : currentValue(0.0f), minValue(0.0f), maxValue(0.0f),
                           changeRate(0.0f), variability(0.0f) {}
    
    EnvironmentalFactor(const std::string& id, const std::string& n, float current,
                      float min, float max, float rate, float var, const std::string& unit,
                      const std::string& desc)
        : factorId(id), name(n), currentValue(current), minValue(min), maxValue(max),
          changeRate(rate), variability(var), unit(unit), description(desc) {}
    
    void update(double timestep);
    void setValue(float value) { currentValue = std::clamp(value, minValue, maxValue); }
    float getNormalizedValue() const { return (currentValue - minValue) / (maxValue - minValue); }
};

// World event that can occur
struct WorldEvent {
    enum class EventType {
        WeatherChange,
        TemperatureShift,
        SeasonChange,
        ResourceSpawn,
        HazardSpawn,
        PredatorArrival,
        PreyMigration,
        GeologicalChange,
        AtmosphericChange,
        BiologicalChange
    };
    
    EventType type;
    std::string eventId;
    double timestamp;
    std::string description;
    std::unordered_map<std::string, float> parameters;
    float severity;  // 0.0-1.0
    float impact;    // Effect on agents (0.0-1.0)
    
    WorldEvent() : type(EventType::WeatherChange), eventId(""), timestamp(0.0),
                   severity(0.0f), impact(0.0f) {}
};

// Ecological system with populations and interactions
struct EcologicalSystem {
    struct Population {
        std::string speciesId;
        std::string name;
        float populationSize;
        float birthRate;
        float deathRate;
        float growthRate;
        float carryingCapacity;
        float ageDistributionMean;
        float ageDistributionStd;
        std::unordered_map<std::string, float> diet;  // species -> consumption rate
        std::unordered_map<std::string, float> predators;  // species -> predation rate
        std::unordered_map<std::string, float> prey;  // species -> prey availability
        
        Population() : populationSize(0.0f), birthRate(0.0f), deathRate(0.0f),
                      growthRate(0.0f), carryingCapacity(0.0f), ageDistributionMean(0.0f),
                      ageDistributionStd(0.0f) {}
    };
    
    std::unordered_map<std::string, Population> populations;
    std::vector<WorldEvent> eventQueue;
    
    void update(double timestep);
    void addPopulation(const Population& population);
    void removePopulation(const std::string& speciesId);
    float getPopulationSize(const std::string& speciesId) const;
    void interact(const std::string& predatorSpecies, const std::string& preySpecies);
};

// Weather system
struct WeatherSystem {
    enum class WeatherCondition {
        Clear,
        Sunny,
        Cloudy,
        Rainy,
        Stormy,
        Snowy,
        Foggy,
        Windy,
        Drought,
        Flood
    };
    
    WeatherCondition currentCondition;
    float temperature;           // Celsius
    float humidity;              // 0.0-1.0
    float pressure;              // kPa
    float windSpeed;             // m/s
    float windDirection;         // degrees
    float visibility;            // meters
    float precipitation;         // mm/hour
    float UVIndex;               // 0.0-11.0+
    
    WeatherSystem()
        : currentCondition(WeatherCondition::Clear), temperature(20.0f),
          humidity(0.5f), pressure(101.3f), windSpeed(0.0f),
          windDirection(0.0f), visibility(1000.0f), precipitation(0.0f), UVIndex(5.0f) {}
    
    void update(double timestep);
    float getTemperatureEffect() const;
    float getHumidityEffect() const;
    float getWindEffect() const;
    float getPrecipitationEffect() const;
};

// Temporal system with time cycles
struct TemporalSystem {
    enum class TimeOfDay {
        Dawn,
        Morning,
        Noon,
        Afternoon,
        Evening,
        Night
    };
    
    enum class Season {
        Spring,
        Summer,
        Autumn,
        Winter
    };
    
    TimeOfDay timeOfDay;
    Season season;
    float timeOfDayProgress;     // 0.0-1.0
    float seasonProgress;        // 0.0-1.0
    float dayLength;             // hours
    float nightLength;           // hours
    double simulationTime;       // hours since start
    double realTimeOffset;       // offset for reproducibility
    
    TemporalSystem()
        : timeOfDay(TimeOfDay::Morning), season(Season::Spring),
          timeOfDayProgress(0.5f), seasonProgress(0.0f), dayLength(12.0f),
          nightLength(12.0f), simulationTime(0.0), realTimeOffset(0.0) {}
    
    void update(double timestep);
    float getLightingLevel() const;
    float getTemperatureSeasonality() const;
    float getPrecipitationSeasonality() const;
};

// Dynamic world with environmental changes and evolution
class DynamicWorld : public SimpleWorld {
public:
    DynamicWorld();
    ~DynamicWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight,
                   bool enableWeather, bool enableEcology, bool enableSeasons);
    
    // Environmental factor management
    void addEnvironmentalFactor(const EnvironmentalFactor& factor);
    void removeEnvironmentalFactor(const std::string& factorId);
    float getEnvironmentalFactor(const std::string& factorId) const;
    void setEnvironmentalFactor(const std::string& factorId, float value);
    
    // Event management
    void queueEvent(const WorldEvent& event);
    WorldEvent getNextEvent() const;
    bool hasEvents() const;
    
    // Ecological system
    void addPopulation(const EcologicalSystem::Population& population);
    void removePopulation(const std::string& speciesId);
    float getSpeciesPopulation(const std::string& speciesId) const;
    void updateEcologicalDynamics(double timestep);
    
    // Weather system
    void setWeatherCondition(WeatherSystem::WeatherCondition condition);
    WeatherSystem::WeatherCondition getWeatherCondition() const;
    void updateWeather(double timestep);
    
    // Temporal system
    void setSeason(TemporalSystem::Season season);
    TemporalSystem::Season getSeason() const;
    void updateTime(double timestep);
    float getDayProgress() const;
    
    // Impact assessment
    float assessEnvironmentalImpact(float agentX, float agentY) const;
    float assessWeatherImpact(float agentX, float agentY) const;
    float assessEcologicalImpact(float agentX, float agentY) const;
    
    // World simulation
    void updateWorldDynamics(double timestep);
    void applyEnvironmentalEffects(double timestep);
    void processEvents(double timestep);
    
    // Adaptation and evolution
    void recordEvolutionaryPressure(const std::string& speciesId, float pressure);
    void evolveSpecies(const std::string& speciesId, double timestep);
    void generateNewSpecies(double timestep);
    
    // World state
    std::string getWorldStateSnapshot() const;
    void logEnvironmentalChanges() const;
    void visualizeWeatherSystem() const;
    void visualizeEcologicalSystem() const;
    
    // Resource generation
    void generateResources(double timestep);
    void consumeResources(double timestep);
    void naturalResourcesGrowth(double timestep);
    
    // Hazard dynamics
    void updateHazardDynamics(double timestep);
    void createHazard(const WorldObject& hazard);
    void removeHazard(float x, float y);
    
    // Biological interactions
    void biologicalInteractions(double timestep);
    void predatorPreyDynamics(double timestep);
    void symbioticRelationships(double timestep);
    
private:
    // Core systems
    WeatherSystem weatherSystem_;
    TemporalSystem temporalSystem_;
    EcologicalSystem ecologicalSystem_;
    
    // Environmental factors
    std::unordered_map<std::string, EnvironmentalFactor> environmentalFactors_;
    
    // Event queue
    std::vector<WorldEvent> eventQueue_;
    
    // World state
    bool enableWeather_;
    bool enableEcology_;
    bool enableSeasons_;
    double worldTime_;              // Accumulated world time
    
    // Evolutionary tracking
    std::unordered_map<std::string, float> evolutionaryPressures_;
    std::unordered_map<std::string, float> speciesAdaptationRates_;
    
    // Resource management
    float totalResources_;
    float resourceRegenerationRate_;
    
    // Hazard management
    std::vector<WorldObject> activeHazards_;
    float hazardSpawnRate_;
    
    // Helper functions
    void updateEnvironmentalFactors(double timestep);
    void processEventQueue(double timestep);
    void generateWeatherEvents(double timestep);
    void generateEcologicalEvents(double timestep);
    void updateSeasonalEffects(double timestep);
    
    void calculateResourceConsumption(double timestep);
    void calculateResourceGeneration(double timestep);
    void updateHazardSpawning(double timestep);
    
    std::string generateWeatherDescription() const;
    std::string generateEcologicalDescription() const;
    std::string generateTemporalDescription() const;
    
    // Event generation functions
    void generateWeatherEventsInternal(WeatherSystem::WeatherCondition fromCondition,
                                      WeatherSystem::WeatherCondition toCondition);
    void generateSeasonalEvents(TemporalSystem::Season fromSeason,
                               TemporalSystem::Season toSeason);
    void generateEcologicalEventsInternal();
    
    // Species interaction functions
    void simulatePredatorPreyDynamics(double timestep);
    void simulateSymbioticRelationships(double timestep);
    void simulateCompetition(double timestep);
    
    // Adaptation functions
    void adaptToEnvironmentalConditions(double timestep);
    void evolveSpeciesPopulation(const std::string& speciesId, double timestep);
    void generateGeneticVariation(const std::string& speciesId, double timestep);
    
    // Impact calculation functions
    float calculateWeatherImpact(float agentX, float agentY) const;
    float calculateEcologicalImpact(float agentX, float agentY) const;
    float calculateSeasonalImpact(float agentX, float agentY) const;
    
    // Visualization functions
    std::string generateWeatherMap() const;
    std::string generateEcologicalMap() const;
    std::string generateResourceMap() const;
    std::string generateHazardMap() const;
}; 

} // namespace nlm
