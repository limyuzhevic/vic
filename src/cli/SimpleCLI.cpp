#include "SimpleCLI.h"

// Helper function implementations
void printUserFriendlyError(const std::string& error) {
    std::cerr << "Error: " << error << std::endl;
    std::cerr << std::endl;
    std::cerr << "Use '--help' for usage information." << std::endl;
}

// Integer parsing implementation
bool parseInt(const std::string& str, int64_t& result, int64_t min, int64_t max) {
    try {
        size_t pos;
        result = std::stoll(str, &pos);
        if (pos != str.length()) {
            return false;
        }
        if (result < min || result > max) {
            return false;
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Float parsing implementation
bool parseFloat(const std::string& str, float& result, float min, float max) {
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        if (pos != str.length()) {
            return false;
        }
        if (val < min || val > max) {
            return false;
        }
        result = val;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}