#include "ColorManager.hpp"
#include <cmath>
#include <iostream>

namespace nlm {
    
    bool ColorManager::colorsEnabled_ = true;
    bool ColorManager::trueColorSupported_ = false;
    std::string ColorManager::currentScheme_ = "default";
    std::array<std::string, 16> ColorManager::basicColors_;
    std::array<std::string, 16> ColorManager::brightColors_;
    
    void ColorManager::initializeColorArrays() {
        // Standard colors (basic)
        basicColors_[0] = "\033[30m"; // Black
        basicColors_[1] = "\033[31m"; // Red
        basicColors_[2] = "\033[32m"; // Green
        basicColors_[3] = "\033[33m"; // Yellow
        basicColors_[4] = "\033[34m"; // Blue
        basicColors_[5] = "\033[35m"; // Magenta
        basicColors_[6] = "\033[36m"; // Cyan
        basicColors_[7] = "\033[37m"; // White
        
        // Bright colors
        brightColors_[0] = "\033[90m"; // Bright Black
        brightColors_[1] = "\033[91m"; // Bright Red
        brightColors_[2] = "\033[92m"; // Bright Green
        brightColors_[3] = "\033[93m"; // Bright Yellow
        brightColors_[4] = "\033[94m"; // Bright Blue
        brightColors_[5] = "\033[95m"; // Bright Magenta
        brightColors_[6] = "\033[96m"; // Bright Cyan
        brightColors_[7] = "\033[97m"; // Bright White
    }
    
    bool ColorManager::detectTrueColorSupport() {
        // Check if terminal supports 24-bit color
        const char* term = std::getenv("TERM");
        if (!term) return false;
        
        std::string termName = term;
        return termName.find("256color") != std::string::npos || 
               termName.find("truecolor") != std::string::npos;
    }
    
    std::string ColorManager::getColorCode(Color color, bool bright) {
        if (!colorsEnabled_) return "";
        
        int colorIndex = static_cast<int>(color);
        if (bright && colorIndex < 8) {
            return brightColors_[colorIndex];
        } else if (!bright && colorIndex < 8) {
            return basicColors_[colorIndex];
        }
        
        return "";
    }
    
    std::string ColorManager::getBackgroundCode(Background bg, bool bright) {
        if (!colorsEnabled_) return "";
        
        int bgIndex = static_cast<int>(bg) - 40; // Convert background to similar index
        if (bgIndex >= 0 && bgIndex < 8) {
            if (bright) {
                return "\033[" + std::to_string(100 + bgIndex) + "m";
            } else {
                return "\033[" + std::to_string(40 + bgIndex) + "m";
            }
        }
        
        return "";
    }
    
    std::string ColorManager::getStyleCode(const std::string& style) {
        if (!colorsEnabled_) return "";
        
        if (style == "reset") return "\033[0m";
        if (style == "bold") return "\033[1m";
        if (style == "dim") return "\033[2m";
        if (style == "italic") return "\033[3m";
        if (style == "underline") return "\033[4m";
        
        return "";
    }
    
    std::string ColorManager::combineCodes(const std::vector<std::string>& codes) {
        std::string result;
        for (const auto& code : codes) {
            if (!code.empty()) {
                result += code;
            }
        }
        return result;
    }
    
    std::string ColorManager::getNeuronColor(float activity, float maxActivity) {
        if (!colorsEnabled_) return "\033[37m";
        
        float normalized = activity / maxActivity;
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        
        if (normalized < 0.33f) {
            return getColorCode(Color::Blue);
        } else if (normalized < 0.66f) {
            return getColorCode(Color::Yellow);
        } else {
            return getColorCode(Color::Red);
        }
    }
    
    std::string ColorManager::getSynapseColor(float weight, float maxWeight) {
        if (!colorsEnabled_) return "\033[37m";
        
        float normalized = (weight + maxWeight) / (2.0f * maxWeight);
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        
        if (normalized < 0.33f) {
            return getColorCode(Color::Green);
        } else if (normalized < 0.66f) {
            return getColorCode(Color::White);
        } else {
            return getColorCode(Color::Magenta);
        }
    }
    
    std::string ColorManager::getRegionColor(size_t regionId) {
        static const std::vector<Color> regionColors = {
            Color::Blue, Color::Green, Color::Yellow, Color::Red,
            Color::Magenta, Color::Cyan, Color::BrightBlue, Color::BrightGreen
        };
        
        size_t colorIndex = regionId % regionColors.size();
        return getColorCode(regionColors[colorIndex]);
    }
    
    std::string ColorManager::getPopulationColor(size_t popId) {
        static const std::vector<Color> popColors = {
            Color::Blue, Color::Green, Color::Yellow, Color::Red,
            Color::Magenta, Color::Cyan
        };
        
        size_t colorIndex = popId % popColors.size();
        return getColorCode(popColors[colorIndex]);
    }
    
    std::string ColorManager::getWorkingMemoryColor(float strength) {
        float normalized = std::min(1.0f, strength / 10.0f);
        float r = normalized;
        float g = 1.0f - normalized / 2.0f;
        float b = 0.5f;
        
        if (!trueColorSupported_) {
            if (normalized < 0.5f) {
                return getColorCode(Color::Green);
            } else {
                return getColorCode(Color::Yellow);
            }
        }
        
        int ri = static_cast<int>(r * 255);
        int gi = static_cast<int>(g * 255);
        int bi = static_cast<int>(b * 255);
        
        return "\033[38;2;" + std::to_string(ri) + ";" + 
               std::to_string(gi) + ";" + std::to_string(bi) + "m";
    }
    
    std::string ColorManager::getEpisodicMemoryColor(size_t episodeAge) {
        float ageFactor = std::min(1.0f, static_cast<float>(episodeAge) / 100.0f);
        
        if (ageFactor < 0.33f) {
            return getColorCode(Color::BrightGreen);
        } else if (ageFactor < 0.66f) {
            return getColorCode(Color::Green);
        } else {
            return getColorCode(Color::BrightBlack);
        }
    }
    
    std::string ColorManager::getAssociativeMemoryColor(float associationStrength) {
        float normalized = std::min(1.0f, associationStrength / 1.0f);
        
        if (normalized < 0.33f) {
            return getColorCode(Color::Blue);
        } else if (normalized < 0.66f) {
            return getColorCode(Color::Cyan);
        } else {
            return getColorCode(Color::BrightBlue);
        }
    }
    
    std::string ColorManager::getFiringRateColor(float firingRate) {
        if (!colorsEnabled_) return "\033[37m";
        
        if (firingRate < 0.5f) {
            return getColorCode(Color::Green);
        } else if (firingRate < 1.5f) {
            return getColorCode(Color::Yellow);
        } else if (firingRate < 5.0f) {
            return getColorCode(Color::Red);
        } else {
            return getColorCode(Color::BrightRed);
        }
    }
    
    std::string ColorManager::getActivityGradient(float value, float min, float max) {
        if (!colorsEnabled_) return "\033[37m";
        
        if (max <= min) return getColorCode(Color::White);
        
        float normalized = (value - min) / (max - min);
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        
        if (normalized < 0.33f) {
            return getColorCode(Color::Blue);
        } else if (normalized < 0.66f) {
            return getColorCode(Color::Green);
        } else {
            return getColorCode(Color::Red);
        }
    }
    
    std::string ColorManager::getSpikeColor(bool isActive, int spikeCount) {
        if (!colorsEnabled_) return "\033[37m";
        
        if (isActive) {
            return getColorCode(Color::BrightRed);
        } else if (spikeCount > 0) {
            return getColorCode(Color::Red);
        } else {
            return getColorCode(Color::Green);
        }
    }
    
    std::string ColorManager::getStatisticColor(float value, float mean, float variance) {
        if (variance <= 0.0f) return getColorCode(Color::White);
        
        float zScore = (value - mean) / std::sqrt(variance);
        
        if (std::abs(zScore) < 1.0f) {
            return getColorCode(Color::Green);
        } else if (std::abs(zScore) < 2.0f) {
            return getColorCode(Color::Yellow);
        } else {
            return getColorCode(Color::Red);
        }
    }
    
    std::string ColorManager::getGradientColor(float value, float low, float high) {
        if (value < low) return getColorCode(Color::Blue);
        if (value > high) return getColorCode(Color::Red);
        
        float normalized = (value - low) / (high - low);
        
        if (normalized < 0.33f) {
            return getColorCode(Color::Blue);
        } else if (normalized < 0.66f) {
            return getColorCode(Color::Green);
        } else {
            return getColorCode(Color::Red);
        }
    }
    
    std::string ColorManager::getDebugColor(const std::string& level) {
        if (level == "error") return getColorCode(Color::BrightRed);
        if (level == "warning") return getColorCode(Color::Yellow);
        if (level == "info") return getColorCode(Color::Green);
        if (level == "debug") return getColorCode(Color::Cyan);
        return getColorCode(Color::White);
    }
    
    std::string ColorManager::getHighlightColor() {
        return getColorCode(Color::BrightYellow) + ANSI_BOLD;
    }
    
    std::string ColorManager::getWarningColor() {
        return getColorCode(Color::Yellow);
    }
    
    std::string ColorManager::getErrorColor() {
        return getColorCode(Color::BrightRed);
    }
    
    std::string ColorManager::getAlphaCode(float alpha) {
        if (trueColorSupported_ && alpha < 1.0f) {
            return "\033[38;2;128;128;128;" + std::to_string(static_cast<int>(alpha * 255)) + "m";
        }
        return "";
    }
    
    void ColorManager::setColorScheme(const std::string& scheme) {
        currentScheme_ = scheme;
    }
    
    std::string ColorManager::applyScheme(const std::string& baseColor, const std::string& scheme) {
        if (scheme == "dark") {
            return "\033[1m" + baseColor;
        } else if (scheme == "light") {
            return "\033[2m" + baseColor;
        }
        return baseColor;
    }
    
    bool ColorManager::isColorEnabled() {
        return colorsEnabled_;
    }
    
    void ColorManager::enableColors(bool enable) {
        colorsEnabled_ = enable;
    }
    
    void ColorManager::force256Colors() {
        trueColorSupported_ = false;
    }
    
    void ColorManager::forceTrueColors() {
        trueColorSupported_ = true;
    }
    
} // namespace nlm
