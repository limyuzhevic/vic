#include "NLMImprovedAPI.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>

namespace nlm {
namespace Utils {

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string format_duration(double seconds) {
    std::stringstream ss;
    if (seconds < 60.0) {
        ss << std::fixed << std::setprecision(3) << seconds << "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remaining = seconds - minutes * 60.0;
        ss << minutes << "m " << std::fixed << std::setprecision(1) << remaining << "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        int minutes = static_cast<int>((seconds - hours * 3600.0) / 60.0);
        double remaining = seconds - hours * 3600.0 - minutes * 60.0;
        ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(1) << remaining << "s";
    }
    return ss.str();
}

std::string format_number(size_t num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

bool file_exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::string get_temp_filename(const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << std::hex << std::time(nullptr) << std::rand() << ".tmp";
    return ss.str();
}

bool safe_write_file(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << content;
        return file.good();
    } catch (...) {
        return false;
    }
}

uint64_t seed_from_time() {
    return static_cast<uint64_t>(std::time(nullptr));
}

uint64_t seed_from_string(const std::string& str) {
    std::hash<std::string> hasher;
    return hasher(str);
}

double current_time_seconds() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

std::string timestamp_string() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string neuron_type_to_string(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

NeuronType string_to_neuron_type(const std::string& str) {
    std::string lower = to_lower(trim(str));
    if (lower == "excitatory") return NeuronType::Excitatory;
    if (lower == "inhibitory") return NeuronType::Inhibitory;
    if (lower == "modulatory") return NeuronType::Modulatory;
    if (lower == "sensory") return NeuronType::Sensory;
    if (lower == "motor") return NeuronType::Motor;
    if (lower == "internal") return NeuronType::Internal;
    return NeuronType::Internal; // Default
}

std::string action_type_to_string(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

ActionType string_to_action_type(const std::string& str) {
    std::string lower = to_lower(trim(str));
    if (lower == "moveforward") return ActionType::MoveForward;
    if (lower == "movebackward") return ActionType::MoveBackward;
    if (lower == "moveleft") return ActionType::MoveLeft;
    if (lower == "moveright") return ActionType::MoveRight;
    if (lower == "turnleft") return ActionType::TurnLeft;
    if (lower == "turnright") return ActionType::TurnRight;
    if (lower == "look") return ActionType::Look;
    if (lower == "lookup") return ActionType::LookUp;
    if (lower == "lookdown") return ActionType::LookDown;
    if (lower == "interact") return ActionType::Interact;
    if (lower == "eat") return ActionType::Eat;
    if (lower == "drink") return ActionType::Drink;
    if (lower == "rest") return ActionType::Rest;
    if (lower == "wait") return ActionType::Wait;
    if (lower == "custom") return ActionType::Custom;
    return ActionType::Wait; // Default
}

} // namespace Utils

} // namespace nlm