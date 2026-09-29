// CoreNLMUtils.cpp - Implementation of core utilities
#include "CoreNLMUtils.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace nlm {
namespace utils {

// TimeUtils implementation
const std::chrono::time_point<std::chrono::system_clock> TimeUtils::startupTime_ = 
    std::chrono::system_clock::now();

// Additional utility functions that need implementation

// String case conversion utilities
std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(trim(token));
    }
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& strings, char delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

int StringUtils::stringToInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(str);
    } catch (...) {
        return defaultValue;
    }
}

int64_t StringUtils::stringToInt64(const std::string& str, int64_t defaultValue) {
    try {
        return std::stoll(str);
    } catch (...) {
        return defaultValue;
    }
}

double StringUtils::stringToDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(str);
    } catch (...) {
        return defaultValue;
    }
}

float StringUtils::stringToFloat(const std::string& str, float defaultValue) {
    try {
        return static_cast<float>(std::stod(str));
    } catch (...) {
        return defaultValue;
    }
}

bool StringUtils::stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

std::string StringUtils::intToString(int value) {
    return std::to_string(value);
}

std::string StringUtils::int64ToString(int64_t value) {
    return std::to_string(value);
}

std::string StringUtils::doubleToString(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string StringUtils::floatToString(float value, int precision) {
    return doubleToString(static_cast<double>(value), precision);
}

// String format implementation
void StringUtils::formatInternal(std::stringstream& ss, const std::string& formatStr, 
                                const std::string& value, int index, Args... args) {
    size_t pos = formatStr.find("{}");
    if (pos != std::string::npos) {
        ss << formatStr.substr(0, pos);
        ss << value;
        formatInternal(ss, formatStr.substr(pos + 2), args...);
    } else {
        ss << formatStr;
        if (sizeof...(args) > 0) {
            formatInternal(ss, "", args...);
        }
    }
}

// MathUtils implementations
template<typename T>
T MathUtils::lerp(T start, T end, float t) {
    return start + (end - start) * t;
}

template<typename T>
T MathUtils::mapRange(T value, T fromMin, T fromMax, T toMin, T toMax) {
    if (fromMax == fromMin) return toMin;
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

template<typename T>
T MathUtils::ema(T current, T previous, float alpha) {
    return previous * (1.0f - alpha) + current * alpha;
}

template<typename T>
T MathUtils::movingAverage(const std::vector<T>& values, size_t windowSize) {
    if (values.empty()) return T(0);
    size_t actualWindow = std::min(windowSize, values.size());
    T sum = T(0);
    for (size_t i = 0; i < actualWindow; ++i) {
        sum += values[values.size() - actualWindow + i];
    }
    return sum / static_cast<T>(actualWindow);
}

template<typename T>
T MathUtils::standardDeviation(const std::vector<T>& values) {
    if (values.size() < 2) return T(0);
    
    T mean = std::accumulate(values.begin(), values.end(), T(0)) / static_cast<T>(values.size());
    T sumSq = T(0);
    
    for (const T& val : values) {
        T diff = val - mean;
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq / static_cast<T>(values.size() - 1));
}

// CollectionUtils implementations
template<typename T, typename Predicate>
bool CollectionUtils::contains(const std::vector<T>& collection, Predicate predicate) {
    return std::find_if(collection.begin(), collection.end(), predicate) != collection.end();
}

template<typename T, typename Predicate>
size_t CollectionUtils::findIndex(const std::vector<T>& collection, Predicate predicate) {
    auto it = std::find_if(collection.begin(), collection.end(), predicate);
    return (it != collection.end()) ? (it - collection.begin()) : static_cast<size_t>(-1);
}

template<typename T, typename Predicate>
bool CollectionUtils::removeIf(std::vector<T>& collection, Predicate predicate) {
    auto it = std::remove_if(collection.begin(), collection.end(), predicate);
    bool removed = (it != collection.end());
    if (removed) {
        collection.erase(it, collection.end());
    }
    return removed;
}

template<typename T>
bool CollectionUtils::addUnique(std::vector<T>& collection, const T& value) {
    if (!contains(collection, [&value](const T& item) { return item == value; })) {
        collection.push_back(value);
        return true;
    }
    return false;
}

template<typename T>
void CollectionUtils::shuffle(std::vector<T>& collection) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(collection.begin(), collection.end(), gen);
}

template<typename T>
void CollectionUtils::reverse(std::vector<T>& collection) {
    std::reverse(collection.begin(), collection.end());
}

template<typename T>
void CollectionUtils::removeDuplicates(std::vector<T>& collection) {
    std::sort(collection.begin(), collection.end());
    auto last = std::unique(collection.begin(), collection.end());
    collection.erase(last, collection.end());
}

template<typename T>
std::vector<T> CollectionUtils::mergeSorted(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size() + b.size());
    std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    return result;
}

template<typename T>
void CollectionUtils::rotateLeft(std::vector<T>& collection, size_t positions) {
    if (collection.empty()) return;
    positions = positions % collection.size();
    std::rotate(collection.begin(), collection.begin() + positions, collection.end());
}

template<typename T>
void CollectionUtils::rotateRight(std::vector<T>& collection, size_t positions) {
    if (collection.empty()) return;
    rotateLeft(collection, collection.size() - (positions % collection.size()));
}

template<typename T>
T CollectionUtils::max(const std::vector<T>& collection) {
    return *std::max_element(collection.begin(), collection.end());
}

template<typename T>
T CollectionUtils::min(const std::vector<T>& collection) {
    return *std::min_element(collection.begin(), collection.end());
}

template<typename T>
T CollectionUtils::sum(const std::vector<T>& collection) {
    return std::accumulate(collection.begin(), collection.end(), T(0));
}

template<typename T>
T CollectionUtils::product(const std::vector<T>& collection) {
    T result = T(1);
    for (const T& value : collection) {
        result *= value;
    }
    return result;
}

template<typename T, typename Predicate>
bool CollectionUtils::all(const std::vector<T>& collection, Predicate predicate) {
    return std::all_of(collection.begin(), collection.end(), predicate);
}

template<typename T, typename Predicate>
bool CollectionUtils::any(const std::vector<T>& collection, Predicate predicate) {
    return std::any_of(collection.begin(), collection.end(), predicate);
}

template<typename T, typename Predicate>
size_t CollectionUtils::countIf(const std::vector<T>& collection, Predicate predicate) {
    return static_cast<size_t>(std::count_if(collection.begin(), collection.end(), predicate));
}

template<typename T>
std::pair<T, T> CollectionUtils::minMax(const std::vector<T>& collection) {
    if (collection.empty()) {
        return std::make_pair(T(0), T(0));
    }
    T minVal = *std::min_element(collection.begin(), collection.end());
    T maxVal = *std::max_element(collection.begin(), collection.end());
    return std::make_pair(minVal, maxVal);
}

template<typename T>
T CollectionUtils::median(std::vector<T> values) {
    if (values.empty()) return T(0);
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    if (size % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2;
    } else {
        return values[size / 2];
    }
}

template<typename T>
T CollectionUtils::mode(const std::vector<T>& collection) {
    if (collection.empty()) return T(0);
    std::unordered_map<T, int> frequencyMap;
    for (const T& value : collection) {
        frequencyMap[value]++;
    }
    
    T modeValue;
    int maxFrequency = 0;
    for (const auto& pair : frequencyMap) {
        if (pair.second > maxFrequency) {
            maxFrequency = pair.second;
            modeValue = pair.first;
        }
    }
    return modeValue;
}

} // namespace utils
} // namespace nlm

// Format function implementations
template std::string nlm::utils::StringUtils::format(const std::string& formatStr, const std::string& arg1);
template std::string nlm::utils::StringUtils::format(const std::string& formatStr, const std::string& arg1, const std::string& arg2);
template std::string nlm::utils::StringUtils::format(const std::string& formatStr, const std::string& arg1, const std::string& arg2, const std::string& arg3);
template std::string nlm::utils::StringUtils::format(const std::string& formatStr, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4);

// MathUtils template instantiations
namespace nlm::utils {
    template int MathUtils::lerp<int>(int, int, float);
    template double MathUtils::lerp<double>(double, double, float);
    template float MathUtils::lerp<float>(float, float, float);
    
    template int MathUtils::mapRange<int>(int, int, int, int, int);
    template double MathUtils::mapRange<double>(double, double, double, double, double);
    
    template int MathUtils::ema<int>(int, int, float);
    template double MathUtils::ema<double>(double, double, float);
    
    template int MathUtils::movingAverage<int>(const std::vector<int>&, size_t);
    template double MathUtils::movingAverage<double>(const std::vector<double>&, size_t);
    
    template int MathUtils::standardDeviation<int>(const std::vector<int>&);
    template double MathUtils::standardDeviation<double>(const std::vector<double>&);
}

// CollectionUtils template instantiations
namespace nlm::utils {
    template bool CollectionUtils::contains<int>(const std::vector<int>&, std::function<bool(int)>);
    template bool CollectionUtils::contains<double>(const std::vector<double>&, std::function<bool(double)>);
    template bool CollectionUtils::contains<std::string>(const std::vector<std::string>&, std::function<bool(std::string)>);
    
    template size_t CollectionUtils::findIndex<int>(const std::vector<int>&, std::function<bool(int)>);
    template size_t CollectionUtils::findIndex<double>(const std::vector<double>&, std::function<bool(double)>);
    template size_t CollectionUtils::findIndex<std::string>(const std::vector<std::string>&, std::function<bool(std::string)>);
    
    template bool CollectionUtils::removeIf<int>(std::vector<int>&, std::function<bool(int)>);
    template bool CollectionUtils::removeIf<double>(std::vector<double>&, std::function<bool(double)>);
    template bool CollectionUtils::removeIf<std::string>(std::vector<std::string>&, std::function<bool(std::string)>);
    
    template bool CollectionUtils::addUnique<int>(std::vector<int>&, const int&);
    template bool CollectionUtils::addUnique<double>(std::vector<double>&, const double&);
    template bool CollectionUtils::addUnique<std::string>(std::vector<std::string>&, const std::string&);
    
    template void CollectionUtils::shuffle<int>(std::vector<int>&);
    template void CollectionUtils::shuffle<double>(std::vector<double>&);
    template void CollectionUtils::shuffle<std::string>(std::vector<std::string>&);
    
    template void CollectionUtils::reverse<int>(std::vector<int>&);
    template void CollectionUtils::reverse<double>(std::vector<double>&);
    template void CollectionUtils::reverse<std::string>(std::vector<std::string>&);
    
    template void CollectionUtils::removeDuplicates<int>(std::vector<int>&);
    template void CollectionUtils::removeDuplicates<double>(std::vector<double>&);
    template void CollectionUtils::removeDuplicates<std::string>(std::vector<std::string>&);
    
    template std::vector<int> CollectionUtils::mergeSorted<int>(const std::vector<int>&, const std::vector<int>&);
    template std::vector<double> CollectionUtils::mergeSorted<double>(const std::vector<double>&, const std::vector<double>&);
    
    template void CollectionUtils::rotateLeft<int>(std::vector<int>&, size_t);
    template void CollectionUtils::rotateLeft<double>(std::vector<double>&, size_t);
    template void CollectionUtils::rotateLeft<std::string>(std::vector<std::string>&, size_t);
    
    template void CollectionUtils::rotateRight<int>(std::vector<int>&, size_t);
    template void CollectionUtils::rotateRight<double>(std::vector<double>&, size_t);
    template void CollectionUtils::rotateRight<std::string>(std::vector<std::string>&, size_t);
    
    template int CollectionUtils::max<int>(const std::vector<int>&);
    template double CollectionUtils::max<double>(const std::vector<double>&);
    template std::string CollectionUtils::max<std::string>(const std::vector<std::string>&);
    
    template int CollectionUtils::min<int>(const std::vector<int>&);
    template double CollectionUtils::min<double>(const std::vector<double>&);
    template std::string CollectionUtils::min<std::string>(const std::vector<std::string>&);
    
    template int CollectionUtils::sum<int>(const std::vector<int>&);
    template double CollectionUtils::sum<double>(const std::vector<double>&);
    template std::string CollectionUtils::sum<std::string>(const std::vector<std::string>&);
    
    template int CollectionUtils::product<int>(const std::vector<int>&);
    template double CollectionUtils::product<double>(const std::vector<double>&);
    
    template bool CollectionUtils::all<int>(const std::vector<int>&, std::function<bool(int)>);
    template bool CollectionUtils::all<double>(const std::vector<double>&, std::function<bool(double)>);
    template bool CollectionUtils::all<std::string>(const std::vector<std::string>&, std::function<bool(std::string)>);
    
    template bool CollectionUtils::any<int>(const std::vector<int>&, std::function<bool(int)>);
    template bool CollectionUtils::any<double>(const std::vector<double>&, std::function<bool(double)>);
    template bool CollectionUtils::any<std::string>(const std::vector<std::string>&, std::function<bool(std::string)>);
    
    template size_t CollectionUtils::countIf<int>(const std::vector<int>&, std::function<bool(int)>);
    template size_t CollectionUtils::countIf<double>(const std::vector<double>&, std::function<bool(double)>);
    template size_t CollectionUtils::countIf<std::string>(const std::vector<std::string>&, std::function<bool(std::string)>);
    
    template std::pair<int, int> CollectionUtils::minMax<int>(const std::vector<int>&);
    template std::pair<double, double> CollectionUtils::minMax<double>(const std::vector<double>&);
    template std::pair<std::string, std::string> CollectionUtils::minMax<std::string>(const std::vector<std::string>&);
    
    template int CollectionUtils::median<int>(std::vector<int>);
    template double CollectionUtils::median<double>(std::vector<double>);
    template std::string CollectionUtils::median<std::string>(std::vector<std::string>);
    
    template int CollectionUtils::mode<int>(const std::vector<int>&);
    template double CollectionUtils::mode<double>(const std::vector<double>&);
    template std::string CollectionUtils::mode<std::string>(const std::vector<std::string>&);
}

// ConfigManager template instantiations
namespace nlm {
    template std::optional<int> ConfigManager::getValidated<int>(const std::string& key) const;
    template std::optional<int64_t> ConfigManager::getValidated<int64_t>(const std::string& key) const;
    template std::optional<double> ConfigManager::getValidated<double>(const std::string& key) const;
    template std::optional<float> ConfigManager::getValidated<float>(const std::string& key) const;
    template std::optional<bool> ConfigManager::getValidated<bool>(const std::string& key) const;
    template std::optional<std::string> ConfigManager::getValidated<std::string>(const std::string& key) const;
}

// EnvironmentVariableLoader template instantiations
namespace nlm {
    template int EnvironmentVariableLoader::convertEnvValue<int>(const std::string& str);
    template int64_t EnvironmentVariableLoader::convertEnvValue<int64_t>(const std::string& str);
    template double EnvironmentVariableLoader::convertEnvValue<double>(const std::string& str);
    template float EnvironmentVariableLoader::convertEnvValue<float>(const std::string& str);
    template bool EnvironmentVariableLoader::convertEnvValue<bool>(const std::string& str);
    template std::string EnvironmentVariableLoader::convertEnvValue<std::string>(const std::string& str);
}

} // namespace nlm