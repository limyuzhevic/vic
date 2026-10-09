#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>

namespace nlm {

// Environmental conditions structure
struct EnvironmentalConditions {
    // Physical conditions
    float temperature;     // Temperature in Celsius
    float humidity;        // Humidity percentage (0-100)
    float pressure;        // Atmospheric pressure (hPa)
    float lightLevel;      // Light intensity (0-1)
    float windSpeed;       // Wind speed (m/s)
    float windDirection;   // Wind direction (degrees)
    
    // Environmental state
    float timeOfDay;       // Time of day (0-24)
    float dayNightCycle;   // Day/night cycle (0=night, 1=day)
    float season;          // Season (0-4, representing seasons)
    
    // Environmental hazards
    float radiationLevel;  // Radiation level (0-1)
    float pollutionLevel;  // Pollution level (0-1)
    float visibility;      // Visibility distance (meters)
    
    EnvironmentalConditions() 
        : temperature(20.0f), humidity(50.0f), pressure(1013.25f)
        , lightLevel(1.0f), windSpeed(0.0f), windDirection(0.0f)
        , timeOfDay(12.0f), dayNightCycle(1.0f), season(2.0f) // Spring
        , radiationLevel(0.0f), pollutionLevel(0.0f), visibility(100.0f) {}
};

// Environmental event structure
struct EnvironmentalEvent {
    std::string type;           // Event type (storm, sunrise, etc.)
    float timestamp;            // Event time
    float duration;             // Event duration
    float intensity;            // Event intensity (0-1)
    std::map<std::string, float> parameters; // Event-specific parameters
    bool active;                // Whether event is currently active
    
    EnvironmentalEvent() : timestamp(0.0f), duration(0.0f), intensity(0.0f), active(false) {}
    EnvironmentalEvent(const std::string& t, float ts, float dur, float inten)
        : type(t), timestamp(ts), duration(dur), intensity(inten), active(true) {}
};

// Environmental feedback from sensing
struct EnvironmentalFeedback {
    std::vector<float> sensorReadings;    // Raw sensor readings
    std::vector<float> interpretedValues; // Interpreted environmental values
    float timestamp;                      // When reading was taken
    bool valid;                           // Whether reading is valid
    
    EnvironmentalFeedback() : timestamp(0.0f), valid(false) {}
};

// Environment state manager for tracking and updating environmental conditions
class EnvironmentStateManager {
public:
    EnvironmentStateManager() = default;
    ~EnvironmentStateManager() = default;
    
    // Initialize environment with starting conditions
    void initialize(const EnvironmentalConditions& initialConditions) {
        currentConditions_ = initialConditions;
        updateDerivedValues();
    }
    
    // Update environmental conditions based on physics and events
    void update(float dt, const std::vector<EnvironmentalEvent>& events) {
        // Update time-based changes
        updateTimeBasedChanges(dt);
        
        // Apply active events
        applyEvents(dt, events);
        
        // Update feedback systems
        updateFeedbackSystems(dt);
        
        // Generate environmental predictions
        generateEnvironmentalPredictions();
    }
    
    // Get current environmental conditions
    const EnvironmentalConditions& getCurrentConditions() const {
        return currentConditions_;
    }
    
    // Get environmental feedback
    const EnvironmentalFeedback& getFeedback() const {
        return feedback_;
    }
    
    // Add or update an environmental event
    void addEvent(const EnvironmentalEvent& event) {
        events_.push_back(event);
    }
    
    // Remove expired events
    void cleanupExpiredEvents(float currentTime) {
        events_.erase(
            std::remove_if(events_.begin(), events_.end(),
                [currentTime](const EnvironmentalEvent& event) {
                    return event.active && currentTime > event.timestamp + event.duration;
                }),
            events_.end()
        );
    }
    
    // Check if environment is suitable for certain conditions
    bool isSuitableForActivity(float requiredTemp, float requiredHumidity, float requiredLight) const {
        float tempDiff = std::abs(currentConditions_.temperature - requiredTemp);
        float humidityDiff = std::abs(currentConditions_.humidity - requiredHumidity);
        float lightDiff = std::abs(currentConditions_.lightLevel - requiredLight);
        
        return tempDiff < 10.0f && humidityDiff < 20.0f && lightDiff < 0.2f;
    }
    
    // Calculate environmental stress on agents
    float calculateEnvironmentalStress() const {
        float stress = 0.0f;
        
        // Temperature stress (optimal range: 15-25°C)
        if (currentConditions_.temperature < 15.0f) {
            stress += (15.0f - currentConditions_.temperature) / 10.0f;
        } else if (currentConditions_.temperature > 25.0f) {
            stress += (currentConditions_.temperature - 25.0f) / 10.0f;
        }
        
        // Humidity stress (optimal range: 40-60%)
        if (currentConditions_.humidity < 40.0f) {
            stress += (40.0f - currentConditions_.humidity) / 40.0f;
        } else if (currentConditions_.humidity > 60.0f) {
            stress += (currentConditions_.humidity - 60.0f) / 40.0f;
        }
        
        // Light stress (optimal range: 0.3-0.7)
        if (currentConditions_.lightLevel < 0.3f) {
            stress += (0.3f - currentConditions_.lightLevel) / 0.3f;
        } else if (currentConditions_.lightLevel > 0.7f) {
            stress += (currentConditions_.lightLevel - 0.7f) / 0.3f;
        }
        
        // Pollution stress
        stress += currentConditions_.pollutionLevel * 2.0f;
        
        // Radiation stress
        stress += currentConditions_.radiationLevel * 1.5f;
        
        return stress;
    }
    
    // Get time of day
    float getTimeOfDay() const {
        return currentConditions_.timeOfDay;
    }
    
    // Get day/night cycle
    float getDayNightCycle() const {
        return currentConditions_.dayNightCycle;
    }
    
    // Check if it's daytime
    bool isDaytime() const {
        return currentConditions_.dayNightCycle > 0.5f;
    }
    
    // Check if it's night
    bool isNight() const {
        return currentConditions_.dayNightCycle < 0.5f;
    }
    
    // Get seasonal information
    float getSeason() const {
        return currentConditions_.season;
    }
    
    // Get weather conditions based on current state
    std::string getWeatherCondition() const {
        if (currentConditions_.lightLevel < 0.2f) return "night";
        if (currentConditions_.lightLevel > 0.8f && currentConditions_.windSpeed < 5.0f) return "sunny";
        if (currentConditions_.windSpeed > 10.0f) return "windy";
        if (currentConditions_.humidity > 70.0f) return "humid";
        if (currentConditions_.temperature > 30.0f) return "hot";
        if (currentConditions_.temperature < 10.0f) return "cold";
        if (currentConditions_.lightLevel < 0.6f) return "partly_cloudy";
        return "normal";
    }
    
private:
    EnvironmentalConditions currentConditions_;
    EnvironmentalFeedback feedback_;
    std::vector<EnvironmentalEvent> events_;
    
    // Update derived values based on current conditions
    void updateDerivedValues() {
        // Calculate day/night cycle based on time of day
        if (currentConditions_.timeOfDay >= 6.0f && currentConditions_.timeOfDay <= 18.0f) {
            currentConditions_.dayNightCycle = 1.0f;
        } else {
            currentConditions_.dayNightCycle = 0.0f;
        }
        
        // Calculate light level based on time of day and day/night cycle
        if (currentConditions_.dayNightCycle > 0.0f) {
            if (currentConditions_.timeOfDay < 6.0f) {
                // Morning (6-18)
                currentConditions_.lightLevel = std::sin((currentConditions_.timeOfDay - 6.0f) * M_PI / 12.0f) * 0.8f + 0.2f;
            } else {
                // Afternoon (6-18)
                currentConditions_.lightLevel = std::sin((18.0f - currentConditions_.timeOfDay) * M_PI / 12.0f) * 0.8f + 0.2f;
            }
        } else {
            currentConditions_.lightLevel = 0.0f; // Night
        }
        
        // Update visibility based on light and weather
        currentConditions_.visibility = 100.0f * currentConditions_.lightLevel;
        if (currentConditions_.pollutionLevel > 0.5f) {
            currentConditions_.visibility *= (1.0f - currentConditions_.pollutionLevel);
        }
    }
    
    // Update time-based environmental changes
    void updateTimeBasedChanges(float dt) {
        // Update time of day
        currentConditions_.timeOfDay += dt / 3600.0f; // Hours
        if (currentConditions_.timeOfDay >= 24.0f) {
            currentConditions_.timeOfDay -= 24.0f;
        }
        
        // Update season slowly (not implemented for simplicity)
        // currentConditions_.season = std::fmod(currentConditions_.timeOfDay / 24.0f * 4.0f, 4.0f);
        
        // Apply temperature changes based on time of day and season
        updateTemperatureBasedOnTime();
        
        // Apply other time-based changes
        currentConditions_.pressure += 0.1f * dt; // Slow pressure change
        currentConditions_.pressure = std::max(900.0f, std::min(1100.0f, currentConditions_.pressure));
    }
    
    // Update temperature based on time of day
    void updateTemperatureBasedOnTime() {
        float baseTemp = 20.0f;
        
        // Seasonal variation
        float seasonalTemp = 0.0f;
        if (currentConditions_.season == 0.0f) seasonalTemp = -10.0f; // Winter
        else if (currentConditions_.season == 1.0f) seasonalTemp = 5.0f;  // Spring
        else if (currentConditions_.season == 3.0f) seasonalTemp = 10.0f; // Fall
        
        // Diurnal variation
        float diurnalTemp = 0.0f;
        if (currentConditions_.dayNightCycle > 0.0f) {
            float dayPercent = (currentConditions_.timeOfDay - 6.0f) / 12.0f;
            if (dayPercent < 0.0f) dayPercent = 0.0f;
            if (dayPercent > 1.0f) dayPercent = 1.0f;
            diurnalTemp = 15.0f * (1.0f - std::cos(dayPercent * M_PI)) / 2.0f;
        }
        
        currentConditions_.temperature = baseTemp + seasonalTemp + diurnalTemp;
    }
    
    // Apply active environmental events
    void applyEvents(float dt, const std::vector<EnvironmentalEvent>& events) {
        for (const auto& event : events) {
            if (!event.active) continue;
            
            if (event.type == "storm") {
                currentConditions_.lightLevel *= 0.1f;
                currentConditions_.windSpeed += event.intensity * 20.0f;
                currentConditions_.temperature += event.intensity * 5.0f - 2.5f;
                currentConditions_.humidity += event.intensity * 30.0f;
            } else if (event.type == "sunrise") {
                currentConditions_.dayNightCycle = 1.0f;
                currentConditions_.lightLevel = std::min(1.0f, currentConditions_.lightLevel + 0.1f * dt);
            } else if (event.type == "sunset") {
                currentConditions_.dayNightCycle = 0.0f;
                currentConditions_.lightLevel = std::max(0.0f, currentConditions_.lightLevel - 0.1f * dt);
            } else if (event.type == "temperature_change") {
                currentConditions_.temperature = event.parameters.at("temperature") * event.intensity;
            } else if (event.type == "pollution_event") {
                currentConditions_.pollutionLevel = event.parameters.at("pollution_level") * event.intensity;
            }
        }
    }
    
    // Update environmental feedback systems
    void updateFeedbackSystems(float dt) {
        // Generate sensor readings
        std::vector<float> readings;
        
        // Temperature sensor
        readings.push_back(currentConditions_.temperature);
        
        // Humidity sensor
        readings.push_back(currentConditions_.humidity);
        
        // Light sensor
        readings.push_back(currentConditions_.lightLevel);
        
        // Wind sensor
        readings.push_back(currentConditions_.windSpeed);
        
        // Pollution sensor
        readings.push_back(currentConditions_.pollutionLevel);
        
        feedback_.sensorReadings = readings;
        
        // Interpret readings
        std::vector<float> interpreted;
        for (float reading : readings) {
            // Simple interpretation (could be enhanced)
            if (reading > 0.0f && reading <= 20.0f) interpreted.push_back(reading / 20.0f);
            else if (reading > 20.0f && reading <= 100.0f) interpreted.push_back((reading - 20.0f) / 80.0f + 0.25f);
            else interpreted.push_back(1.0f);
        }
        
        feedback_.interpretedValues = interpreted;
        feedback_.timestamp = currentConditions_.timeOfDay;
        feedback_.valid = true;
    }
    
    // Generate environmental predictions
    void generateEnvironmentalPredictions() {
        // This could be expanded with machine learning or prediction algorithms
        // For now, store current state as prediction
        prediction_ = currentConditions_;
    }
    
    // Store for prediction
    EnvironmentalConditions prediction_;
};

} // namespace nlm