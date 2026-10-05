#include "DynamicWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

namespace nlm {

// EnvironmentalFactor implementation
void EnvironmentalFactor::update(double timestep) {
    // Add random variation
    float variation = (rand() / (float)RAND_MAX - 0.5f) * 2.0f * variability;
    float change = changeRate * timestep + variation;
    
    currentValue += change;
    currentValue = std::clamp(currentValue, minValue, maxValue);
}

// EcologicalSystem implementation
void EcologicalSystem::update(double timestep) {
    for (auto& pair : populations) {
        Population& population = pair.second;
        
        // Validate population parameters
        if (population.populationSize < 0.0f) {
            NLM_LOG_WARNING("Ecological system: Population " + population.speciesId + 
                          " has negative size: " + std::to_string(population.populationSize));
            population.populationSize = 0.0f;
        }
        
        // Calculate growth based on current size and carrying capacity
        float growth = population.growthRate * population.populationSize *
                      (1.0f - population.populationSize / population.carryingCapacity);
        
        // Validate growth rate
        if (std::isnan(growth) || std::isinf(growth)) {
            NLM_LOG_ERROR("Ecological system: Invalid growth rate for " + population.speciesId);
            growth = 0.0f;
        }
        
        // Apply predation
        float predationLoss = 0.0f;
        for (const auto& predatorPair : predators) {
            const std::string& preySpecies = pair.first;
            float predationRate = predatorPair.second;
            
            if (preySpecies == population.speciesId) {
                continue; // Can't prey on own species
            }
            
            auto preyIt = populations.find(preySpecies);
            if (preyIt != populations.end()) {
                float availablePrey = preyIt->second.populationSize;
                if (availablePrey < 0.0f) {
                    NLM_LOG_WARNING("Ecological system: Population " + preySpecies + 
                                  " has negative size: " + std::to_string(availablePrey));
                    availablePrey = 0.0f;
                }
                
                float consumption = std::min(availablePrey, population.populationSize * predationRate * timestep);
                if (consumption < 0.0f || std::isnan(consumption) || std::isinf(consumption)) {
                    NLM_LOG_ERROR("Ecological system: Invalid predation consumption for " + population.speciesId);
                    consumption = 0.0f;
                }
                
                predationLoss += consumption;
            }
        }
        
        // Apply birth and death
        float birth = population.birthRate * population.populationSize * timestep;
        float death = population.deathRate * population.populationSize * timestep;
        
        // Validate birth and death rates
        if (std::isnan(birth) || std::isinf(birth) || std::isnan(death) || std::isinf(death)) {
            NLM_LOG_ERROR("Ecological system: Invalid birth/death rates for " + population.speciesId);
            birth = 0.0f;
            death = 0.0f;
        }
        
        population.populationSize += birth - death - predationLoss;
        
        // Ensure population doesn't go negative
        if (population.populationSize < 0.0f) {
            NLM_LOG_WARNING("Ecological system: Population " + population.speciesId + 
                          " would go negative, setting to zero");
            population.populationSize = 0.0f;
        }
        
        // Update age distribution (simplified)
        population.ageDistributionMean += 0.5f * timestep;  // Aging rate
        
        // Validate age distribution
        if (population.ageDistributionMean < 0.0f || 
            population.ageDistributionMean > 100.0f ||
            std::isnan(population.ageDistributionMean) || std::isinf(population.ageDistributionMean)) {
            NLM_LOG_ERROR("Ecological system: Invalid age distribution for " + population.speciesId);
            population.ageDistributionMean = 1.0f;
        }
    }
    
    // Process queued events
    for (const WorldEvent& event : eventQueue) {
        processEvent(event);
    }
    eventQueue.clear();
}

void EcologicalSystem::addPopulation(const Population& population) {
    populations[population.speciesId] = population;
}

void EcologicalSystem::removePopulation(const std::string& speciesId) {
    populations.erase(speciesId);
}

float EcologicalSystem::getPopulationSize(const std::string& speciesId) const {
    auto it = populations.find(speciesId);
    return (it != populations.end()) ? it->second.populationSize : 0.0f;
}

void EcologicalSystem::interact(const std::string& predatorSpecies, const std::string& preySpecies) {
    auto predatorIt = populations.find(predatorSpecies);
    auto preyIt = populations.find(preySpecies);
    
    if (predatorIt != populations.end() && preyIt != populations.end()) {
        Population& predator = predatorIt->second;
        Population& prey = preyIt->second;
        
        // Predator consumes prey
        float consumption = std::min(predator.populationSize * 0.1f * predator.growthRate * 0.1f,
                                    prey.populationSize * 0.05f);
        
        prey.populationSize -= consumption;
        predator.populationSize += consumption * 0.5f;  // 50% efficiency
        
        // Ensure populations don't go negative
        prey.populationSize = std::max(0.0f, prey.populationSize);
        predator.populationSize = std::max(0.0f, predator.populationSize);
    }
}

void EcologicalSystem::processEvent(const WorldEvent& event) {
    // Process ecological events
    // Simplified implementation
    auto it = populations.find(event.parameters.at("species"));
    if (it != populations.end()) {
        Population& population = it->second;
        
        if (event.type == WorldEvent::EventType::ResourceSpawn) {
            population.populationSize *= (1.0f + event.impact);
        } else if (event.type == WorldEvent::EventType::HazardSpawn) {
            population.populationSize *= (1.0f - event.impact);
        }
    }
}

// WeatherSystem implementation
void WeatherSystem::update(double timestep) {
    // Update weather based on seasonal and temporal patterns
    // Simplified implementation
    
    // Temperature varies with time of day
    float dayTemperature = 20.0f + 10.0f * sin(temporalSystem_.timeOfDayProgress * 2.0f * M_PI);
    
    // Humidity varies
    float humidity = 0.5f + 0.3f * sin(temporalSystem_.timeOfDayProgress * 4.0f * M_PI);
    humidity = std::clamp(humidity, 0.0f, 1.0f);
    
    // Precipitation varies with weather condition
    switch (currentCondition) {
        case WeatherCondition::Clear:
            precipitation = 0.0f;
            break;
        case WeatherCondition::Rainy:
            precipitation = 1.0f + rand() / (float)RAND_MAX;
            break;
        case WeatherCondition::Stormy:
            precipitation = 5.0f + rand() / (float)RAND_MAX * 5.0f;
            break;
        case WeatherCondition::Snowy:
            precipitation = 2.0f + rand() / (float)RAND_MAX * 3.0f;
            break;
        default:
            precipitation = 0.5f + rand() / (float)RAND_MAX;
            break;
    }
    
    // Wind varies with storm conditions
    if (currentCondition == WeatherCondition::Stormy) {
        windSpeed = 20.0f + rand() / (float)RAND_MAX * 30.0f;
        windDirection = rand() / (float)RAND_MAX * 360.0f;
    } else if (currentCondition == WeatherCondition::Windy) {
        windSpeed = 10.0f + rand() / (float)RAND_MAX * 20.0f;
    } else {
        windSpeed = 0.0f;
    }
    
    // UV index varies with sun exposure and clouds
    UVIndex = (currentCondition == WeatherCondition::Clear || 
              currentCondition == WeatherCondition::Sunny) ? 
              10.0f : 
              5.0f * (1.0f - humidity);
    
    // Update visibility based on weather and humidity
    visibility = 1000.0f;
    if (currentCondition == WeatherCondition::Foggy) {
        visibility = 100.0f + rand() / (float)RAND_MAX * 200.0f;
    } else if (currentCondition == WeatherCondition::Stormy) {
        visibility = 200.0f + rand() / (float)RAND_MAX * 300.0f;
    }
}

float WeatherSystem::getTemperatureEffect() const {
    // Return effect based on temperature for agents
    if (temperature < 0.0f) {
        return 0.5f;  // Negative effect
    } else if (temperature < 10.0f) {
        return 0.8f;
    } else if (temperature < 25.0f) {
        return 1.0f;  // Optimal
    } else if (temperature < 35.0f) {
        return 0.9f;
    } else {
        return 0.7f;  // Stress at high temperatures
    }
}

float WeatherSystem::getHumidityEffect() const {
    // Return effect based on humidity for agents
    if (humidity > 0.8f) {
        return 0.8f;  // High humidity stress
    } else if (humidity > 0.6f) {
        return 0.9f;
    } else if (humidity > 0.4f) {
        return 1.0f;  // Optimal
    } else {
        return 0.95f;  // Low humidity stress
    }
}

float WeatherSystem::getWindEffect() const {
    // Return effect based on wind speed for agents
    if (windSpeed > 20.0f) {
        return 0.6f;  // High wind stress
    } else if (windSpeed > 10.0f) {
        return 0.8f;
    } else if (windSpeed > 5.0f) {
        return 0.9f;
    } else if (windSpeed > 1.0f) {
        return 0.95f;
    } else {
        return 1.0f;  // No wind effect
    }
}

float WeatherSystem::getPrecipitationEffect() const {
    // Return effect based on precipitation for agents
    if (precipitation > 10.0f) {
        return 0.5f;  // Heavy rain stress
    } else if (precipitation > 5.0f) {
        return 0.7f;
    } else if (precipitation > 1.0f) {
        return 0.85f;
    } else if (precipitation > 0.1f) {
        return 0.9f;
    } else {
        return 1.0f;  // No precipitation effect
    }
}

// TemporalSystem implementation
void TemporalSystem::update(double timestep) {
    // Update time of day
    timeOfDayProgress += timestep / (dayLength + nightLength);
    if (timeOfDayProgress >= 1.0f) {
        timeOfDayProgress = 0.0f;
        
        // Update season if needed
        seasonProgress += 1.0f / 4.0f;  // 4 seasons per year
        if (seasonProgress >= 1.0f) {
            seasonProgress = 0.0f;
            
            // Cycle through seasons
            switch (season) {
                case Season::Spring:
                    season = Season::Summer;
                    break;
                case Season::Summer:
                    season = Season::Autumn;
                    break;
                case Season::Autumn:
                    season = Season::Winter;
                    break;
                case Season::Winter:
                    season = Season::Spring;
                    break;
            }
        }
        
        // Update time of day
        switch (timeOfDay) {
            case TimeOfDay::Dawn:
                timeOfDay = TimeOfDay::Morning;
                break;
            case TimeOfDay::Morning:
                timeOfDay = TimeOfDay::Noon;
                break;
            case TimeOfDay::Noon:
                timeOfDay = TimeOfDay::Afternoon;
                break;
            case TimeOfDay::Afternoon:
                timeOfDay = TimeOfDay::Evening;
                break;
            case TimeOfDay::Evening:
                timeOfDay = TimeOfDay::Night;
                break;
            case TimeOfDay::Night:
                timeOfDay = TimeOfDay::Dawn;
                break;
        }
    }
    
    // Update simulation time
    simulationTime += timestep;
}

float TemporalSystem::getLightingLevel() const {
    // Return lighting level based on time of day
    switch (timeOfDay) {
        case TimeOfDay::Dawn:
            return 0.2f;
        case TimeOfDay::Morning:
            return 0.4f;
        case TimeOfDay::Noon:
            return 1.0f;
        case TimeOfDay::Afternoon:
            return 0.8f;
        case TimeOfDay::Evening:
            return 0.4f;
        case TimeOfDay::Night:
            return 0.1f;
        default:
            return 0.5f;
    }
}

float TemporalSystem::getTemperatureSeasonality() const {
    // Return temperature effect based on season
    switch (season) {
        case Season::Spring:
            return 0.9f;  // Moderate
        case Season::Summer:
            return 1.1f;  // Warm
        case Season::Autumn:
            return 0.95f; // Cool
        case Season::Winter:
            return 0.7f;  // Cold
        default:
            return 1.0f;
    }
}

float TemporalSystem::getPrecipitationSeasonality() const {
    // Return precipitation effect based on season
    switch (season) {
        case Season::Spring:
            return 1.0f;  // Moderate rain
        case Season::Summer:
            return 1.2f;  // Summer storms
        case Season::Autumn:
            return 1.1f;  // Autumn rain
        case Season::Winter:
            return 1.3f;  // Winter snow/rain
        default:
            return 1.0f;
    }
}

// DynamicWorld implementation
DynamicWorld::DynamicWorld()
    : enableWeather_(true), enableEcology_(true), enableSeasons_(true),
      worldTime_(0.0), totalResources_(1000.0f), resourceRegenerationRate_(0.01f),
      hazardSpawnRate_(0.001f)
{
    NLM_LOG_INFO("DynamicWorld initialized with environmental dynamics");
}

DynamicWorld::~DynamicWorld() {
    NLM_LOG_INFO("DynamicWorld destroyed");
}

void DynamicWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight,
                          bool enableWeather, bool enableEcology, bool enableSeasons) {
    SimpleWorld::configure(width, height, visionWidth, visionHeight);
    enableWeather_ = enableWeather;
    enableEcology_ = enableEcology;
    enableSeasons_ = enableSeasons;
    
    NLM_LOG_INFO("DynamicWorld configured: " + std::to_string(width) + "x" + std::to_string(height) +
                 " weather: " + std::to_string(enableWeather_) +
                 " ecology: " + std::to_string(enableEcology_) +
                 " seasons: " + std::to_string(enableSeasons_));
}

void DynamicWorld::addEnvironmentalFactor(const EnvironmentalFactor& factor) {
    environmentalFactors_[factor.factorId] = factor;
    NLM_LOG_DEBUG("Added environmental factor: " + factor.name);
}

void DynamicWorld::removeEnvironmentalFactor(const std::string& factorId) {
    if (environmentalFactors_.find(factorId) != environmentalFactors_.end()) {
        environmentalFactors_.erase(factorId);
        NLM_LOG_DEBUG("Removed environmental factor: " + factorId);
    }
}

float DynamicWorld::getEnvironmentalFactor(const std::string& factorId) const {
    auto it = environmentalFactors_.find(factorId);
    return (it != environmentalFactors_.end()) ? it->second.currentValue : 0.0f;
}

void DynamicWorld::setEnvironmentalFactor(const std::string& factorId, float value) {
    auto it = environmentalFactors_.find(factorId);
    if (it != environmentalFactors_.end()) {
        it->second.setValue(value);
    }
}

void DynamicWorld::queueEvent(const WorldEvent& event) {
    eventQueue_.push_back(event);
    NLM_LOG_DEBUG("Queued world event: " + std::to_string(static_cast<int>(event.type)));
}

WorldEvent DynamicWorld::getNextEvent() const {
    WorldEvent event;
    if (!eventQueue_.empty()) {
        event = eventQueue_.front();
    }
    return event;
}

bool DynamicWorld::hasEvents() const {
    return !eventQueue_.empty();
}

void DynamicWorld::addPopulation(const EcologicalSystem::Population& population) {
    ecologicalSystem_.addPopulation(population);
}

void DynamicWorld::removePopulation(const std::string& speciesId) {
    ecologicalSystem_.removePopulation(speciesId);
}

float DynamicWorld::getSpeciesPopulation(const std::string& speciesId) const {
    return ecologicalSystem_.getPopulationSize(speciesId);
}

void DynamicWorld::updateEcologicalDynamics(double timestep) {
    ecologicalSystem_.update(timestep);
}

void DynamicWorld::setWeatherCondition(WeatherSystem::WeatherCondition condition) {
    weatherSystem_.currentCondition = condition;
    NLM_LOG_DEBUG("Weather condition changed to: " + std::to_string(static_cast<int>(condition)));
}

WeatherSystem::WeatherCondition DynamicWorld::getWeatherCondition() const {
    return weatherSystem_.currentCondition;
}

void DynamicWorld::updateWeather(double timestep) {
    if (enableWeather_) {
        weatherSystem_.update(timestep);
    }
}

void DynamicWorld::setSeason(TemporalSystem::Season season) {
    temporalSystem_.season = season;
    NLM_LOG_DEBUG("Season changed to: " + std::to_string(static_cast<int>(season)));
}

TemporalSystem::Season DynamicWorld::getSeason() const {
    return temporalSystem_.season;
}

void DynamicWorld::updateTime(double timestep) {
    if (enableSeasons_) {
        temporalSystem_.update(timestep);
    }
    worldTime_ += timestep;
}

float DynamicWorld::getDayProgress() const {
    return temporalSystem_.getLightingLevel();
}

float DynamicWorld::assessEnvironmentalImpact(float agentX, float agentY) const {
    float totalImpact = 1.0f;
    
    // Weather impact
    totalImpact *= weatherSystem_.getTemperatureEffect();
    totalImpact *= weatherSystem_.getHumidityEffect();
    totalImpact *= weatherSystem_.getWindEffect();
    totalImpact *= weatherSystem_.getPrecipitationEffect();
    
    // Seasonal impact
    totalImpact *= temporalSystem_.getTemperatureSeasonality();
    totalImpact *= temporalSystem_.getPrecipitationSeasonality();
    
    return totalImpact;
}

float DynamicWorld::assessWeatherImpact(float agentX, float agentY) const {
    float impact = 1.0f;
    impact *= weatherSystem_.getTemperatureEffect();
    impact *= weatherSystem_.getHumidityEffect();
    impact *= weatherSystem_.getWindEffect();
    impact *= weatherSystem_.getPrecipitationEffect();
    return impact;
}

float DynamicWorld::assessEcologicalImpact(float agentX, float agentY) const {
    // Simplified ecological impact assessment
    // In a real implementation, this would consider predator/prey relationships,
    // resource availability, etc.
    return 1.0f;  // No ecological impact for now
}

void DynamicWorld::updateWorldDynamics(double timestep) {
    // Update temporal system
    updateTime(timestep);
    
    // Update weather system
    updateWeather(timestep);
    
    // Update ecological system
    if (enableEcology_) {
        updateEcologicalDynamics(timestep);
    }
    
    // Update environmental factors
    updateEnvironmentalFactors(timestep);
    
    // Process events
    processEvents(timestep);
    
    // Apply environmental effects
    applyEnvironmentalEffects(timestep);
    
    // Update world time
    worldTime_ += timestep;
}

void DynamicWorld::applyEnvironmentalEffects(double timestep) {
    // Apply environmental effects to agent
    float environmentalImpact = assessEnvironmentalImpact(0.0f, 0.0f);
    
    // Reduce agent energy based on environmental stress
    float energyLoss = (1.0f - environmentalImpact) * 0.1f * timestep;
    
    // In a real implementation, this would affect the agent's energy
    NLM_LOG_DEBUG("Environmental impact: " + std::to_string(environmentalImpact) +
                 ", energy loss: " + std::to_string(energyLoss));
}

void DynamicWorld::processEvents(double timestep) {
    // Process queued events
    if (!eventQueue_.empty()) {
        // Process all events
        for (const WorldEvent& event : eventQueue_) {
            // Process event based on type
            switch (event.type) {
                case WorldEvent::EventType::WeatherChange:
                    setWeatherCondition(static_cast<WeatherSystem::WeatherCondition>(
                        event.parameters.at("condition")));
                    break;
                case WorldEvent::EventType::TemperatureShift:
                    // Update temperature
                    break;
                case WorldEvent::EventType::SeasonChange:
                    setSeason(static_cast<TemporalSystem::Season>(
                        event.parameters.at("season")));
                    break;
                case WorldEvent::EventType::ResourceSpawn:
                    // Generate resources
                    generateResources(timestep * event.impact);
                    break;
                case WorldEvent::EventType::HazardSpawn:
                    // Create hazard
                    if (rand() / (float)RAND_MAX < event.severity) {
                        createHazard(WorldObject(event.parameters.at("x"),
                                               event.parameters.at("y"),
                                               WorldObjectType::Hazard,
                                               event.parameters.at("value")));
                    }
                    break;
                default:
                    break;
            }
        }
        
        eventQueue_.clear();
    }
}

void DynamicWorld::recordEvolutionaryPressure(const std::string& speciesId, float pressure) {
    evolutionaryPressures_[speciesId] = pressure;
}

void DynamicWorld::evolveSpecies(const std::string& speciesId, double timestep) {
    if (ecologicalSystem_.populations.find(speciesId) != 
        ecologicalSystem_.populations.end()) {
        
        Population& population = ecologicalSystem_.populations[speciesId];
        
        // Genetic adaptation based on evolutionary pressure
        float pressure = evolutionaryPressures_.count(speciesId) ? 
                        evolutionaryPressures_[speciesId] : 0.0f;
        
        // Adaptation reduces pressure over time
        evolutionaryPressures_[speciesId] *= (1.0f - 0.01f * timestep);
        
        // Species adapts to pressure
        population.growthRate *= (1.0f + pressure * 0.1f * timestep);
        population.carryingCapacity *= (1.0f + pressure * 0.05f * timestep);
        
        // Ensure values don't go out of bounds
        population.growthRate = std::clamp(population.growthRate, 0.01f, 5.0f);
        population.carryingCapacity = std::max(population.carryingCapacity, 1.0f);
    }
}

void DynamicWorld::generateNewSpecies(double timestep) {
    // Generate new species based on environmental conditions
    // Simplified implementation
    if (rand() / (float)RAND_MAX < 0.001f * timestep) {
        Population newSpecies;
        newSpecies.speciesId = "species_" + std::to_string(rand() % 1000);
        newSpecies.name = "New Species";
        newSpecies.populationSize = 10.0f;
        newSpecies.birthRate = 0.1f;
        newSpecies.deathRate = 0.05f;
        newSpecies.growthRate = 0.5f;
        newSpecies.carryingCapacity = 1000.0f;
        newSpecies.ageDistributionMean = 1.0f;
        newSpecies.ageDistributionStd = 0.5f;
        
        ecologicalSystem_.addPopulation(newSpecies);
        NLM_LOG_DEBUG("Generated new species: " + newSpecies.speciesId);
    }
}

std::string DynamicWorld::getWorldStateSnapshot() const {
    std::stringstream ss;
    ss << "=== Dynamic World State ===\n";
    ss << "Simulation Time: " << worldTime_ << " hours\n";
    ss << "Season: " << static_cast<int>(temporalSystem_.season) << " ("
       << temporalSystem_.seasonProgress * 100 << "% progress)\n";
    ss << "Time of Day: " << static_cast<int>(temporalSystem_.timeOfDay) << " ("
       << temporalSystem_.timeOfDayProgress * 100 << "% progress)\n";
    ss << "Weather: " << static_cast<int>(weatherSystem_.currentCondition) << "\n";
    ss << "Temperature: " << weatherSystem_.temperature << "°C\n";
    ss << "Humidity: " << weatherSystem_.humidity * 100 << "%\n";
    ss << "Wind: " << weatherSystem_.windSpeed << " m/s\n";
    ss << "Precipitation: " << weatherSystem_.precipitation << " mm/hour\n";
    ss << "UV Index: " << weatherSystem_.UVIndex << "\n";
    ss << "Visibility: " << weatherSystem_.visibility << " meters\n";
    
    ss << "\n--- Ecological System ---\n";
    for (const auto& pair : ecologicalSystem_.populations) {
        const Population& pop = pair.second;
        ss << pop.name << ": " << pop.populationSize << " (carrying capacity: " 
           << pop.carryingCapacity << ")\n";
    }
    
    ss << "\n--- Environmental Factors ---\n";
    for (const auto& pair : environmentalFactors_) {
        const EnvironmentalFactor& factor = pair.second;
        ss << factor.name << ": " << factor.currentValue << " " 
           << factor.unit << " (" << factor.description << ")\n";
    }
    
    ss << "\n--- Pending Events ---\n";
    ss << "Events queued: " << eventQueue_.size() << "\n";
    
    ss << "\n--- Resources ---\n";
    ss << "Total Resources: " << totalResources_ << "\n";
    
    ss << "\n--- Active Hazards ---\n";
    ss << "Hazards: " << activeHazards_.size() << "\n";
    
    return ss.str();
}

void DynamicWorld::logEnvironmentalChanges() const {
    NLM_LOG_INFO("Environmental Changes:\n" + getWorldStateSnapshot());
}

void DynamicWorld::visualizeWeatherSystem() const {
    std::string weatherMap = generateWeatherMap();
    NLM_LOG_INFO("Weather System Visualization:\n" + weatherMap);
}

void DynamicWorld::visualizeEcologicalSystem() const {
    std::string ecoMap = generateEcologicalMap();
    NLM_LOG_INFO("Ecological System Visualization:\n" + ecoMap);
}

void DynamicWorld::generateResources(double timestep) {
    totalResources_ += resourceRegenerationRate_ * timestep;
    NLM_LOG_DEBUG("Generated resources: " + std::to_string(resourceRegenerationRate_ * timestep) +
                 " (total: " + std::to_string(totalResources_) + ")");
}

void DynamicWorld::consumeResources(double timestep) {
    float consumption = totalResources_ * 0.01f * timestep;
    totalResources_ = std::max(0.0f, totalResources_ - consumption);
    NLM_LOG_DEBUG("Consumed resources: " + std::to_string(consumption) +
                 " (remaining: " + std::to_string(totalResources_) + ")");
}

void DynamicWorld::naturalResourcesGrowth(double timestep) {
    // Natural resource growth based on seasonal factors
    float growth = totalResources_ * temporalSystem_.getPrecipitationSeasonality() * 0.005f * timestep;
    totalResources_ += growth;
    NLM_LOG_DEBUG("Natural resource growth: " + std::to_string(growth) +
                 " (total: " + std::to_string(totalResources_) + ")");
}

void DynamicWorld::updateHazardDynamics(double timestep) {
    // Update hazard states
    for (auto& hazard : activeHazards_) {
        // Hazard movement and effect updates
        // Simplified implementation
    }
    
    // Spawn new hazards
    if (rand() / (float)RAND_MAX < hazardSpawnRate_ * timestep) {
        float x = rand() / (float)RAND_MAX * getWidth();
        float y = rand() / (float)RAND_MAX * getHeight();
        createHazard(WorldObject(x, y, WorldObjectType::Hazard, -10.0f));
    }
}

void DynamicWorld::createHazard(const WorldObject& hazard) {
    activeHazards_.push_back(hazard);
    NLM_LOG_DEBUG("Created hazard at (" + std::to_string(hazard.x) + ", " +
                 std::to_string(hazard.y) + ") type: " +
                 std::to_string(static_cast<int>(hazard.type)));
}

void DynamicWorld::removeHazard(float x, float y) {
    for (auto it = activeHazards_.begin(); it != activeHazards_.end(); ++it) {
        float dx = it->x - x;
        float dy = it->y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < it->radius) {
            activeHazards_.erase(it);
            NLM_LOG_DEBUG("Removed hazard at (" + std::to_string(x) + ", " +
                         std::to_string(y) + ")");
            return;
        }
    }
}

void DynamicWorld::biologicalInteractions(double timestep) {
    // Update predator-prey dynamics
    predatorPreyDynamics(timestep);
    
    // Update symbiotic relationships
    symbioticRelationships(timestep);
    
    // Update competition
    simulateCompetition(timestep);
}

void DynamicWorld::predatorPreyDynamics(double timestep) {
    // Simplified predator-prey model
    for (auto& pair : ecologicalSystem_.populations) {
        Population& predator = pair.second;
        
        // Find prey species
        for (auto& preyPair : ecologicalSystem_.populations) {
            if (predator.speciesId == preyPair.first) {
                continue; // Can't prey on own species
            }
            
            Population& prey = preyPair.second;
            
            // Predation rate
            float predation = predator.populationSize * 0.1f * prey.populationSize * timestep;
            
            // Update populations
            prey.populationSize -= predation;
            predator.populationSize += predation * 0.3f;  // 30% efficiency
            
            // Ensure populations don't go negative
            prey.populationSize = std::max(0.0f, prey.populationSize);
            predator.populationSize = std::max(0.0f, predator.populationSize);
        }
    }
}

void DynamicWorld::symbioticRelationships(double timestep) {
    // Update symbiotic relationships
    // Simplified implementation
}

void DynamicWorld::simulateCompetition(double timestep) {
    // Update competition between species
    for (auto& pair1 : ecologicalSystem_.populations) {
        Population& species1 = pair1.second;
        
        for (auto& pair2 : ecologicalSystem_.populations) {
            if (species1.speciesId == pair2.first) {
                continue;
            }
            
            Population& species2 = pair2.second;
            
            // Competition for resources
            float competition = species1.populationSize * 0.05f * species2.populationSize * timestep;
            
            // Both species affected
            species1.populationSize -= competition * 0.5f;
            species2.populationSize -= competition * 0.5f;
            
            // Ensure populations don't go negative
            species1.populationSize = std::max(0.0f, species1.populationSize);
            species2.populationSize = std::max(0.0f, species2.populationSize);
        }
    }
}

void DynamicWorld::updateEnvironmentalFactors(double timestep) {
    for (auto& pair : environmentalFactors_) {
        EnvironmentalFactor& factor = pair.second;
        factor.update(timestep);
        
        // Apply factor effects to world state
        // This could affect resource generation, hazard behavior, etc.
        NLM_LOG_DEBUG("Environmental factor updated: " + factor.name + " = " +
                     std::to_string(factor.currentValue));
    }
}

void DynamicWorld::processEventQueue(double timestep) {
    for (WorldEvent& event : eventQueue_) {
        // Process event
        switch (event.type) {
            case WorldEvent::EventType::WeatherChange:
                // Update weather
                break;
            case WorldEvent::EventType::SeasonChange:
                // Update season
                break;
            case WorldEvent::EventType::ResourceSpawn:
                generateResources(timestep * event.impact);
                break;
            case WorldEvent::EventType::HazardSpawn:
                if (rand() / (float)RAND_MAX < event.severity) {
                    createHazard(WorldObject(event.parameters.at("x"),
                                           event.parameters.at("y"),
                                           WorldObjectType::Hazard,
                                           event.parameters.at("value")));
                }
                break;
            default:
                break;
        }
    }
    
    eventQueue_.clear();
}

void DynamicWorld::generateWeatherEvents(double timestep) {
    // Generate weather events based on current conditions
    if (enableWeather_) {
        // Simplified weather event generation
        if (rand() / (float)RAND_MAX < 0.01f * timestep) {
            WorldEvent event;
            event.type = WorldEvent::EventType::WeatherChange;
            event.eventId = "weather_" + std::to_string(rand() % 1000);
            event.timestamp = worldTime_;
            event.description = "Random weather change";
            event.severity = 0.5f;
            event.impact = 0.8f;
            event.parameters["condition"] = static_cast<float>(rand() % 8);
            
            queueEvent(event);
        }
    }
}

void DynamicWorld::generateEcologicalEvents(double timestep) {
    // Generate ecological events based on species interactions
    if (enableEcology_) {
        // Simplified ecological event generation
        if (rand() / (float)RAND_MAX < 0.005f * timestep) {
            WorldEvent event;
            event.type = WorldEvent::EventType::ResourceSpawn;
            event.eventId = "resource_" + std::to_string(rand() % 1000);
            event.timestamp = worldTime_;
            event.description = "Resource spawn event";
            event.severity = 0.3f;
            event.impact = 0.5f;
            event.parameters["x"] = rand() / (float)RAND_MAX * getWidth();
            event.parameters["y"] = rand() / (float)RAND_MAX * getHeight();
            event.parameters["value"] = 10.0f + rand() / (float)RAND_MAX * 20.0f;
            
            queueEvent(event);
        }
    }
}

void DynamicWorld::updateSeasonalEffects(double timestep) {
    // Update seasonal effects
    // Simplified implementation
}

std::string DynamicWorld::generateWeatherMap() const {
    std::stringstream ss;
    ss << "=== Weather Map ===\n";
    ss << "Temperature: " << weatherSystem_.temperature << "°C\n";
    ss << "Humidity: " << weatherSystem_.humidity * 100 << "%\n";
    ss << "Wind Speed: " << weatherSystem_.windSpeed << " m/s\n";
    ss << "Precipitation: " << weatherSystem_.precipitation << " mm/hour\n";
    ss << "UV Index: " << weatherSystem_.UVIndex << "\n";
    return ss.str();
}

std::string DynamicWorld::generateEcologicalMap() const {
    std::stringstream ss;
    ss << "=== Ecological Map ===\n";
    ss << "Species: " << ecologicalSystem_.populations.size() << "\n";
    for (const auto& pair : ecologicalSystem_.populations) {
        const Population& pop = pair.second;
        ss << "  " << pop.name << ": " << pop.populationSize << " (" 
           << (pop.populationSize / pop.carryingCapacity * 100) << "% capacity)\n";
    }
    return ss.str();
}

std::string DynamicWorld::generateResourceMap() const {
    std::stringstream ss;
    ss << "=== Resource Map ===\n";
    ss << "Total Resources: " << totalResources_ << "\n";
    ss << "Resource Regeneration Rate: " << resourceRegenerationRate_ << "\n";
    return ss.str();
}

std::string DynamicWorld::generateHazardMap() const {
    std::stringstream ss;
    ss << "=== Hazard Map ===\n";
    ss << "Active Hazards: " << activeHazards_.size() << "\n";
    for (const auto& hazard : activeHazards_) {
        ss << "  Hazard at (" << hazard.x << ", " << hazard.y << ") type: " 
           << static_cast<int>(hazard.type) << " value: " << hazard.value << "\n";
    }
    return ss.str();
}

} // namespace nlm
