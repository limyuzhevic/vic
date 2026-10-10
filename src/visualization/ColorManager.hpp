#pragma once

#include <string>
#include <vector>
#include <array>

namespace nlm {

// ANSI escape codes for terminal colors
const std::string ANSI_RESET = "\033[0m";
const std::string ANSI_BOLD = "\033[1m";
const std::string ANSI_DIM = "\033[2m";
const std::string ANSI_ITALIC = "\033[3m";
const std::string ANSI_UNDERLINE = "\033[4m";

class ColorManager {
public:
    enum class Color {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97
    };
    
    enum class Background {
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
        BrightBlack = 100,
        BrightRed = 101,
        BrightGreen = 102,
        BrightYellow = 103,
        BrightBlue = 104,
        BrightMagenta = 105,
        BrightCyan = 106,
        BrightWhite = 107
    };
    
    struct Style {
        std::string prefix;
        std::string suffix;
        bool reset;
    };
    
    static std::string getColorCode(Color color, bool bright = false);
    static std::string getBackgroundCode(Background bg, bool bright = false);
    static std::string getStyleCode(const std::string& style);
    static std::string combineCodes(const std::vector<std::string>& codes);
    
    // Neural network specific colors
    static std::string getNeuronColor(float activity, float maxActivity = 1.0f);
    static std::string getSynapseColor(float weight, float maxWeight = 1.0f);
    static std::string getRegionColor(size_t regionId);
    static std::string getPopulationColor(size_t popId);
    
    // Memory system colors
    static std::string getWorkingMemoryColor(float strength);
    static std::string getEpisodicMemoryColor(size_t episodeAge);
    static std::string getAssociativeMemoryColor(float associationStrength);
    
    // Activity visualization colors
    static std::string getFiringRateColor(float firingRate);
    static std::string getActivityGradient(float value, float min = 0.0f, float max = 1.0f);
    static std::string getSpikeColor(bool isActive, int spikeCount = 0);
    
    // Statistical visualization colors
    static std::string getStatisticColor(float value, float mean, float variance);
    static std::string getGradientColor(float value, float low, float high);
    
    // Debug colors
    static std::string getDebugColor(const std::string& level);
    static std::string getHighlightColor();
    static std::string getWarningColor();
    static std::string getErrorColor();
    
    // Transparency/opacity for overlays
    static std::string getAlphaCode(float alpha);
    
    // Predefined color schemes
    static void setColorScheme(const std::string& scheme);
    static std::string applyScheme(const std::string& baseColor, const std::string& scheme);
    
    // Utility functions
    static bool isColorEnabled();
    static void enableColors(bool enable);
    static void force256Colors();
    static void forceTrueColors();
    
private:
    static bool colorsEnabled_;
    static bool trueColorSupported_;
    static std::string currentScheme_;
    static std::array<std::string, 16> basicColors_;
    static std::array<std::string, 16> brightColors_;
    
    static void initializeColorArrays();
    static bool detectTrueColorSupport();
};

} // namespace nlm
