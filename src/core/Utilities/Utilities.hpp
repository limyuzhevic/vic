namespace nlm {

/**
 * Utility functions for NLM project
 * 
 * Common helper functions used throughout the NLM codebase
 */

/**
 * Parse command line argument with improved error handling
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @param flag The flag to search for (e.g., "--verbose")
 * @return true if flag is present, false otherwise
 */
bool hasCommandLineFlag(int argc, char** argv, const std::string& flag);

/**
 * Get command line argument value
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @param flag The flag to search for (e.g., "--neuron_count")
 * @param defaultValue Default value if flag not found
 * @return Parsed value or default
 */
template<typename T>
T getCommandLineValue(int argc, char** argv, const std::string& flag, const T& defaultValue);

/**
 * Check if string ends with suffix
 * 
 * @param str String to check
 * @param suffix Suffix to check for
 * @return true if str ends with suffix, false otherwise
 */
bool endsWith(const std::string& str, const std::string& suffix);

/**
 * Check if string starts with prefix
 * 
 * @param str String to check
 * @param prefix Prefix to check for
 * @return true if str starts with prefix, false otherwise
 */
bool startsWith(const std::string& str, const std::string& prefix);

/**
 * Trim whitespace from string
 * 
 * @param str String to trim
 * @return Trimmed string
 */
std::string trim(const std::string& str);

/**
 * Convert string to lowercase
 * 
 * @param str String to convert
 * @return Lowercase string
 */
std::string toLower(const std::string& str);

/**
 * Get current timestamp as string
 * 
 * @return Current timestamp in format YYYY-MM-DD HH:MM:SS
 */
std::string getCurrentTimestamp();

/**
 * Format floating point number with specified precision
 * 
 * @param value Value to format
 * @param precision Number of decimal places
 * @return Formatted string
 */
std::string formatFloat(float value, int precision = 6);

/**
 * Check if file exists
 * 
 * @param filepath Path to check
 * @return true if file exists, false otherwise
 */
bool fileExists(const std::string& filepath);

/**
 * Create directory if it doesn't exist
 * 
 * @param path Directory path
 * @return true if successful, false otherwise
 */
bool ensureDirectoryExists(const std::string& path);

/**
 * Get file extension
 * 
 * @param filepath Path to file
 * @return File extension including the dot (e.g., ".txt")
 */
std::string getFileExtension(const std::string& filepath);

/**
 * Generate a unique filename with timestamp
 * 
 * @param baseName Base filename
 * @param extension File extension including dot
 * @return Unique filename
 */
std::string generateUniqueFilename(const std::string& baseName, const std::string& extension = "");

/**
 * Clamp value between min and max
 * 
 * @param value Value to clamp
 * @param min Minimum value
 * @param max Maximum value
 * @return Clamped value
 */
template<typename T>
T clamp(T value, T min, T max);

/**
 * Linear interpolation between two values
 * 
 * @param a First value
 * @param b Second value
 * @param t Interpolation factor (0.0 to 1.0)
 * @return Interpolated value
 */
template<typename T>
T lerp(T a, T b, float t);

/**
 * Calculate mean of values
 * 
 * @param values Vector of values
 * @return Mean value
 */
template<typename T>
T mean(const std::vector<T>& values);

/**
 * Calculate standard deviation of values
 * 
 * @param values Vector of values
 * @param mean Pre-calculated mean
 * @return Standard deviation
 */
template<typename T>
T standardDeviation(const std::vector<T>& values, T mean);

/**
 * Safe division that handles division by zero
 * 
 * @param numerator Numerator
 * @param denominator Denominator
 * @param defaultValue Value to return if denominator is zero
 * @return numerator/denominator or defaultValue
 */
template<typename T>
T safeDivide(T numerator, T denominator, T defaultValue = 0);

} // namespace nlm
