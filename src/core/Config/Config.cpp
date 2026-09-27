#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \\t\\r\\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \\t\\r\\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Sleep/rest cycle configuration methods
bool Config::getEnableSleepRestCycle() const {
    return getOr<bool>("enable_sleep_rest_cycle", false);
}

void Config::setEnableSleepRestCycle(bool enable) {
    set("enable_sleep_rest_cycle", enable);
}

bool Config::getEnableReplayDuringSleep() const {
    return getOr<bool>("enable_replay_during_sleep", true);
}

void Config::setEnableReplayDuringSleep(bool enable) {
    set("enable_replay_during_sleep", enable);
}

bool Config::getEnableDevelopmentDuringSleep() const {
    return getOr<bool>("enable_development_during_sleep", true);
}

void Config::setEnableDevelopmentDuringSleep(bool enable) {
    set("enable_development_during_sleep", enable);
}

bool Config::getEnablePredictionUpdateSleep() const {
    return getOr<bool>("enable_prediction_update_sleep", true);
}

void Config::setEnablePredictionUpdateSleep(bool enable) {
    set("enable_prediction_update_sleep", enable);
}

size_t Config::getSleepStageDuration() const {
    return getOr<size_t>("sleep_stage_duration", 1000);
}

void Config::setSleepStageDuration(size_t duration) {
    set("sleep_stage_duration", duration);
}

size_t Config::getSleepCycleLength() const {
    return getOr<size_t>("sleep_cycle_length", 10000);
}

void Config::setSleepCycleLength(size_t length) {
    set("sleep_cycle_length", length);
}

float Config::getNREMToREMRatio() const {
    return getOr<float>("nrem_to_rem_ratio", 4.0f);
}

void Config::setNREMToREMRatio(float ratio) {
    set("nrem_to_rem_ratio", ratio);
}

float Config::getMemoryConsolidationStrength() const {
    return getOr<float>("memory_consolidation_strength", 0.3f);
}

void Config::setMemoryConsolidationStrength(float strength) {
    set("memory_consolidation_strength", strength);
}

float Config::getNeuromodulationSleepLow() const {
    return getOr<float>("neuromodulation_sleep_low", 0.1f);
}

void Config::setNeuromodulationSleepLow(float low) {
    set("neuromodulation_sleep_low", low);
}

float Config::getNeuromodulationSleepHigh() const {
    return getOr<float>("neuromodulation_sleep_high", 0.5f);
}

void Config::setNeuromodulationSleepHigh(float high) {
    set("neuromodulation_sleep_high", high);
}

float Config::getAttentionSleepSuppression() const {
    return getOr<float>("attention_sleep_suppression", 0.2f);
}

void Config::setAttentionSleepSuppression(float suppression) {
    set("attention_sleep_suppression", suppression);
}

float Config::getDevelopmentRateSleep() const {
    return getOr<float>("development_rate_sleep", 0.5f);
}

void Config::setDevelopmentRateSleep(float rate) {
    set("development_rate_sleep", rate);
}

float Config::getPredictionUpdateStrength() const {
    return getOr<float>("prediction_update_strength", 0.3f);
}
void Config::setPredictionUpdateStrength(float strength) {
    set("prediction_update_strength", strength);
}

// Neuromodulator (ACh, NE, 5-HT) configuration methods
float Config::getNeuromodulationAChBaseline() const {
    return getOr<float>("neuromodulation_ach_baseline", 0.0f);
}

void Config::setNeuromodulationAChBaseline(float baseline) {
    set("neuromodulation_ach_baseline", baseline);
}

float Config::getNeuromodulationAChPeak() const {
    return getOr<float>("neuromodulation_ach_peak", 1.0f);
}

void Config::setNeuromodulationAChPeak(float peak) {
    set("neuromodulation_ach_peak", peak);
}

float Config::getNeuromodulationAChDecayRate() const {
    return getOr<float>("neuromodulation_ach_decay_rate", 0.1f);
}

void Config::setNeuromodulationAChDecayRate(float decayRate) {
    set("neuromodulation_ach_decay_rate", decayRate);
}

float Config::getNeuromodulationNEActivationThreshold() const {
    return getOr<float>("neuromodulation_ne_activation_threshold", 0.1f);
}

void Config::setNeuromodulationNEActivationThreshold(float threshold) {
    set("neuromodulation_ne_activation_threshold", threshold);
}

float Config::getNeuromodulationNESleepLow() const {
    return getOr<float>("neuromodulation_ne_sleep_low", 0.1f);
}

void Config::setNeuromodulationNESleepLow(float low) {
    set("neuromodulation_ne_sleep_low", low);
}

float Config::getNeuromodulationNESleepHigh() const {
    return getOr<float>("neuromodulation_ne_sleep_high", 0.5f);
}

void Config::setNeuromodulationNESleepHigh(float high) {
    set("neuromodulation_ne_sleep_high", high);
}

float Config::getNeuromodulation5HTBaseline() const {
    return getOr<float>("neuromodulation_5ht_baseline", 0.0f);
}

void Config::setNeuromodulation5HTBaseline(float baseline) {
    set("neuromodulation_5ht_baseline", baseline);
}

float Config::getNeuromodulation5HTReleaseRate() const {
    return getOr<float>("neuromodulation_5ht_release_rate", 1.0f);
}

void Config::setNeuromodulation5HTReleaseRate(float releaseRate) {
    set("neuromodulation_5ht_release_rate", releaseRate);
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
