#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "EnvironmentStateManager.hpp"

namespace nlm {

// Weather condition enum
enum class WeatherCondition {
    CLEAR,
    PARTLY_CLOUDY,
    CLOUDY,
    RAINY,
    STORMY,
    SNOWY,
    FOGGY,
    WINDY,
    DARK,
    HOT,
    COLD,
    POLLUTED
};

// Weather effect parameters
struct WeatherEffect {
    WeatherCondition condition;
    float intensity;              // Weather intensity (0-1)
    float duration;               // Duration in seconds
    float startTime;              // Start time of current weather
    float remainingDuration;      // Remaining duration
    std::vector<float> parameters; // Weather-specific parameters
    
    WeatherEffect() : condition(WeatherCondition::CLEAR), intensity(0.0f), 
                      duration(0.0f), startTime(0.0f), remainingDuration(0.0f) {}
    
    WeatherEffect(WeatherCondition cond, float inten, float dur)
        : condition(cond), intensity(inten), duration(dur),
          startTime(0.0f), remainingDuration(dur) {}
    
    // Update remaining duration
    void update(float dt) {
        remainingDuration -= dt;
        if (remainingDuration <= 0.0f) {
            remainingDuration = 0.0f;
        }
    }
    
    // Check if weather is active
    bool isActive() const {
        return remainingDuration > 0.0f;
    }
    
    // Calculate effect value at current time
    float getEffectValue(float timeSinceStart) const {
        if (!isActive()) return 0.0f;
        
        float progress = 1.0f - remainingDuration / duration;
        float effect = intensity * progress * (1.0f - progress) * 4.0f; // Smooth bell curve
        return effect;
    }
};

// Weather forecast
struct WeatherForecast {
    float timestamp;                    // Time of forecast
    std::vector<WeatherEffect> forecast; // Forecasted weather conditions
    float temperatureTrend;             // Temperature change trend
    float precipitationTrend;           // Precipitation change trend
    
    WeatherForecast() : timestamp(0.0f), temperatureTrend(0.0f), precipitationTrend(0.0f) {}
};

// Weather system for managing atmospheric conditions
class WeatherSystem {
public:
    WeatherSystem() = default;
    ~WeatherSystem() = default;
    
    // Initialize weather system
    void initialize() {
        currentTime_ = 0.0f;
        weatherHistory_.clear();
        activeEffects_.clear();
        
        // Start with clear weather
        addWeatherEffect(WeatherCondition::CLEAR, 0.0f, 0.0f); // Clear weather
    }
    
    // Update weather system
    void update(float dt, const EnvironmentalConditions& currentConditions) {
        currentTime_ += dt;
        
        // Update active weather effects
        for (auto& effect : activeEffects_) {
            effect.update(dt);
        }
        
        // Clean up expired effects
        activeEffects_.erase(
            std::remove_if(activeEffects_.begin(), activeEffects_.end(),
                [](const WeatherEffect& effect) {
                    return !effect.isActive();
                }),
            activeEffects_.end()
        );
        
        // Generate weather based on time and conditions
        generateWeather(currentConditions);
        
        // Store weather history
        storeWeatherHistory(currentConditions);
    }
    
    // Get current weather conditions
    WeatherCondition getCurrentWeather() const {
        if (activeEffects_.empty()) return WeatherCondition::CLEAR;
        
        // Return the most intense active weather condition
        WeatherCondition dominantWeather = WeatherCondition::CLEAR;
        float maxIntensity = 0.0f;
        
        for (const auto& effect : activeEffects_) {
            if (effect.intensity > maxIntensity) {
                maxIntensity = effect.intensity;
                dominantWeather = effect.condition;
            }
        }
        
        return dominantWeather;
    }
    
    // Add weather effect
    void addWeatherEffect(WeatherCondition condition, float intensity, float duration) {
        // Check if similar weather already exists
        for (auto& effect : activeEffects_) {
            if (effect.condition == condition) {
                // Update existing effect
                effect.intensity = std::max(effect.intensity, intensity);
                effect.duration = duration;
                effect.startTime = currentTime_;
                effect.remainingDuration = duration;
                return;
            }
        }
        
        // Add new effect
        WeatherEffect newEffect(condition, intensity, duration);
        newEffect.startTime = currentTime_;
        newEffect.remainingDuration = duration;
        activeEffects_.push_back(newEffect);
        
        // Generate weather event
        EnvironmentalEvent event = createWeatherEvent(condition, intensity, duration);
        addWeatherEvent(event);
    }
    
    // Remove all weather effects
    void clearWeather() {
        activeEffects_.clear();
    }
    
    // Get weather effect parameters
    const WeatherEffect* getWeatherEffect(WeatherCondition condition) const {
        for (const auto& effect : activeEffects_) {
            if (effect.condition == condition) {
                return &effect;
            }
        }
        return nullptr;
    }
    
    // Calculate weather impact on agents
    float calculateWeatherImpact(float baseValue) const {
        float totalImpact = baseValue;
        
        for (const auto& effect : activeEffects_) {
            float effectValue = effect.getEffectValue(currentTime_ - effect.startTime);
            
            switch (effect.condition) {
                case WeatherCondition::STORMY:
                    totalImpact *= (1.0f - effectValue * 0.3f); // Reduced visibility, slower movement
                    break;
                case WeatherCondition::RAINY:
                    totalImpact *= (1.0f - effectValue * 0.2f); // Slippery, reduced vision
                    break;
                case WeatherCondition::WINDY:
                    totalImpact *= (1.0f - effectValue * 0.1f); // Wind resistance
                    break;
                case WeatherCondition::CLOUDY:
                case WeatherCondition::FOGGY:
                    totalImpact *= (1.0f - effectValue * 0.1f); // Reduced visibility
                    break;
                case WeatherCondition::DARK:
                    totalImpact *= (1.0f - effectValue * 0.5f); // Poor visibility
                    break;
                case WeatherCondition::HOT:
                    totalImpact *= (1.0f + effectValue * 0.1f); // Heat stress
                    break;
                case WeatherCondition::COLD:
                    totalImpact *= (1.0f - effectValue * 0.1f); // Slower metabolism
                    break;
                case WeatherCondition::POLLUTED:
                    totalImpact *= (1.0f + effectValue * 0.2f); // Health effects
                    break;
                case WeatherCondition::SNOWY:
                    totalImpact *= (1.0f + effectValue * 0.3f); // Slower movement, cold
                    break;
                default:
                    break;
            }
        }
        
        return totalImpact;
    }
    
    // Get weather name as string
    std::string getWeatherName(WeatherCondition condition) const {
        switch (condition) {
            case WeatherCondition::CLEAR: return "Clear";
            case WeatherCondition::PARTLY_CLOUDY: return "Partly Cloudy";
            case WeatherCondition::CLOUDY: return "Cloudy";
            case WeatherCondition::RAINY: return "Rainy";
            case WeatherCondition::STORMY: return "Stormy";
            case WeatherCondition::SNOWY: return "Snowy";
            case WeatherCondition::FOGGY: return "Foggy";
            case WeatherCondition::WINDY: return "Windy";
            case WeatherCondition::DARK: return "Dark";
            case WeatherCondition::HOT: return "Hot";
            case WeatherCondition::COLD: return "Cold";
            case WeatherCondition::POLLUTED: return "Polluted";
            default: return "Unknown";
        }
    }
    
    // Get all active weather effects
    const std::vector<WeatherEffect>& getActiveEffects() const {
        return activeEffects_;
    }
    
    // Generate weather based on time and conditions
    void generateWeather(const EnvironmentalConditions& conditions) {
        // Generate weather based on time of day
        float timeFactor = conditions.timeOfDay / 24.0f;
        
        if (conditions.dayNightCycle > 0.5f) {
            // Daytime weather
            if (conditions.temperature > 30.0f) {
                addWeatherEffect(WeatherCondition::HOT, 0.7f, 3600.0f); // Hot weather for 1 hour
            } else if (conditions.temperature < 10.0f) {
                addWeatherEffect(WeatherCondition::COLD, 0.5f, 1800.0f); // Cold weather for 30 min
            } else if (conditions.windSpeed > 15.0f) {
                addWeatherEffect(WeatherCondition::WINDY, 0.6f, 900.0f); // Windy for 15 min
            } else if (conditions.humidity > 70.0f) {
                if (conditions.lightLevel < 0.3f) {
                    addWeatherEffect(WeatherCondition::FOGGY, 0.4f, 1200.0f);
                } else {
                    addWeatherEffect(WeatherCondition::RAINY, 0.5f, 600.0f);
                }
            } else if (conditions.lightLevel < 0.2f) {
                addWeatherEffect(WeatherCondition::DARK, 0.9f, 0.0f); // Night (permanent until sunrise)
            } else {
                addWeatherEffect(WeatherCondition::CLEAR, 0.0f, 0.0f); // Clear weather
            }
        } else {
            // Nighttime weather
            addWeatherEffect(WeatherCondition::DARK, 0.9f, 0.0f); // Night
        }
        
        // Generate random weather events
        if (shouldGenerateWeatherEvent(timeFactor)) {
            generateRandomWeatherEvent();
        }
    }
    
    // Check if weather event should be generated
    bool shouldGenerateWeatherEvent(float timeFactor) const {
        // Random weather generation based on time
        if (timeFactor < 0.1f || timeFactor > 0.9f) {
            return true; // Night hours can have storms
        }
        
        // Random chance for weather events
        return rand() % 100 < 5; // 5% chance per update
    }
    
    // Generate random weather event
    void generateRandomWeatherEvent() {
        int eventType = rand() % 10;
        
        switch (eventType) {
            case 0:
                addWeatherEffect(WeatherCondition::STORMY, 0.8f, 300.0f);
                break;
            case 1:
                addWeatherEffect(WeatherCondition::RAINY, 0.6f, 120.0f);
                break;
            case 2:
                addWeatherEffect(WeatherCondition::WINDY, 0.7f, 180.0f);
                break;
            case 3:
                addWeatherEffect(WeatherCondition::FOGGY, 0.5f, 240.0f);
                break;
            case 4:
                addWeatherEffect(WeatherCondition::CLOUDY, 0.4f, 60.0f);
                break;
            case 5:
                addWeatherEffect(WeatherCondition::SNOWY, 0.3f, 180.0f);
                break;
            case 6:
                addWeatherEffect(WeatherCondition::POLLUTED, 0.5f, 600.0f);
                break;
            default:
                // No event
                break;
        }
    }
    
    // Create weather event from effect
    EnvironmentalEvent createWeatherEvent(WeatherCondition condition, float intensity, float duration) const {
        EnvironmentalEvent event;
        
        switch (condition) {
            case WeatherCondition::STORMY:
                event.type = "storm";
                event.parameters["wind_speed"] = intensity * 30.0f;
                event.parameters["rain_intensity"] = intensity * 0.8f;
                break;
            case WeatherCondition::RAINY:
                event.type = "rain";
                event.parameters["rain_intensity"] = intensity * 0.5f;
                break;
            case WeatherCondition::WINDY:
                event.type = "wind";
                event.parameters["wind_speed"] = intensity * 20.0f;
                break;
            case WeatherCondition::FOGGY:
                event.type = "fog";
                event.parameters["visibility"] = (1.0f - intensity) * 100.0f;
                break;
            case WeatherCondition::DARK:
                event.type = "darkness";
                event.parameters["light_level"] = (1.0f - intensity) * 0.1f;
                break;
            case WeatherCondition::HOT:
                event.type = "temperature_change";
                event.parameters["temperature"] = 35.0f + intensity * 15.0f;
                break;
            case WeatherCondition::COLD:
                event.type = "temperature_change";
                event.parameters["temperature"] = 0.0f + intensity * 10.0f;
                break;
            case WeatherCondition::POLLUTED:
                event.type = "pollution_event";
                event.parameters["pollution_level"] = intensity * 0.8f;
                break;
            default:
                event.type = "weather_change";
                break;
        }
        
        event.timestamp = currentTime_;
        event.duration = duration;
        event.intensity = intensity;
        
        return event;
    }
    
    // Add weather event to environment
    void addWeatherEvent(const EnvironmentalEvent& event) {
        // This would be called by the real environment
        // For now, just store for reference
        weatherEvents_.push_back(event);
    }
    
    // Store weather history
    void storeWeatherHistory(const EnvironmentalConditions& conditions) {
        WeatherForecast forecast;
        forecast.timestamp = currentTime_;
        forecast.temperatureTrend = conditions.temperature - lastTemperature_;
        forecast.precipitationTrend = (conditions.humidity > 70.0f) ? 1.0f : 0.0f;
        forecast.forecast = activeEffects_;
        
        weatherHistory_.push_back(forecast);
        lastTemperature_ = conditions.temperature;
        
        // Limit history size
        if (weatherHistory_.size() > 100) {
            weatherHistory_.erase(weatherHistory_.begin());
        }
    }
    
    // Get weather forecast
    const WeatherForecast& getWeatherForecast() const {
        if (weatherHistory_.empty()) {
            static WeatherForecast emptyForecast;
            return emptyForecast;
        }
        return weatherHistory_.back();
    }
    
    // Get current time
    float getCurrentTime() const {
        return currentTime_;
    }
    
    // Clear weather events
    void clearWeatherEvents() {
        weatherEvents_.clear();
    }
    
private:
    float currentTime_;
    float lastTemperature_;
    std::vector<WeatherEffect> activeEffects_;
    std::vector<WeatherEvent> weatherEvents_;
    std::vector<WeatherForecast> weatherHistory_;
};

} // namespace nlm