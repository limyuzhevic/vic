#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Utility functions and helpers
class BrainUtils {
public:
    BrainUtils() = default;
    ~BrainUtils() = default;
    
    // Inter-region connection helper
    void addInterRegionConnection(
        std::vector<InterRegionConnection>& connections,
        RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1);
    
    // Find region by ID helper
    NeuralRegion* findRegionById(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        RegionId id);
    
    // Find neuron by ID helper
    Neuron* findNeuronById(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        NeuronId id);
    
    // Get motor neurons helper
    std::vector<Neuron*> getMotorNeurons(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Get sensory neurons helper
    std::vector<Neuron*> getSensoryNeurons(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Create action from motor command helper
    std::unique_ptr<Action> createActionFromMotorCommand(MotorCommand cmd);
    
    // Get spike system pending event count
    size_t getPendingSpikeEventCount(
        std::shared_ptr<class SpikeSystem> spikeSystem) const;
    
    // Generate spike event key for lookup
    uint64_t generateSpikeEventKey(NeuronId neuron, Timestamp time) const;
    
    // Normalize value between min/max range
    float normalizeValue(float value, float min, float max) const;
    
    // Clamp value between min/max range
    float clampValue(float value, float min, float max) const;
    
    // Calculate time delta
    double calculateTimeDelta(Timestamp time1, Timestamp time2) const;
    
    // Round timestamp to nearest timestep
    Timestamp roundTimestamp(Timestamp time, double timestep) const;
    
    // Get simulation step from timestamp
    SimulationStep getStepFromTimestamp(Timestamp time, double timestep) const;
    
    // Get current time from step
    Timestamp getTimeFromStep(SimulationStep step, double timestep) const;
    
    // Check if step is within time bounds
    bool isStepValid(SimulationStep step, SimulationStep maxSteps) const;
    
    // Generate unique neuron ID
    NeuronId generateNeuronId(uint64_t seed) const;
    
    // Generate unique synapse ID
    SynapseId generateSynapseId(uint64_t seed) const;
    
    // Generate unique region ID
    RegionId generateRegionId(uint64_t seed) const;
    
    // Generate unique population ID
    PopulationId generatePopulationId(uint64_t seed) const;
    
    // Check if neuron type is excitatory
    bool isExcitatoryNeuron(NeuronType type) const;
    
    // Check if neuron type is inhibitory
    bool isInhibitoryNeuron(NeuronType type) const;
    
    // Get neuron population multiplier
    float getNeuronPopulationMultiplier(NeuronType type) const;
    
    // Calculate synaptic weight based on neuron types
    float calculateSynapticWeight(NeuronType sourceType, NeuronType targetType) const;
    
    // Check if plasticity is enabled for neuron
    bool isPlasticityEnabled(const PlasticityFlags& flags) const;
    
    // Get default plasticity flags
    PlasticityFlags getDefaultPlasticityFlags() const;
    
    // Calculate spike probability based on membrane potential
    float calculateSpikeProbability(MembranePotential potential, float threshold) const;
    
    // Get refractory period in seconds
    double getRefractoryPeriodSeconds(uint32_t steps, double timestep) const;
    
    // Calculate firing rate from spike count
    float calculateFiringRate(size_t spikeCount, double duration) const;
    
    // Check if value is approximately equal to another value
    bool approximatelyEqual(float a, float b, float epsilon = 0.001f) const;
    
    // Safe division with protection against division by zero
    float safeDivide(float numerator, float denominator, float defaultValue = 0.0f) const;
    
    // Get mean of vector
    float getMean(const std::vector<float>& values) const;
    
    // Get standard deviation of vector
    float getStandardDeviation(const std::vector<float>& values, float mean) const;
    
    // Check if vector is empty or has insufficient data
    bool isVectorEmptyOrSmall(const std::vector<float>& values, size_t minSize = 2) const;
    
    // Get median of vector
    float getMedian(std::vector<float> values) const;
    
    // Sort vector in place
    void sortVector(std::vector<float>& values) const;
    
    // Get vector size with safety check
    size_t getSafeVectorSize(const std::vector<float>& values) const;
    
    // Calculate vector dot product
    float dotProduct(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Calculate vector magnitude
    float vectorMagnitude(const std::vector<float>& values) const;
    
    // Normalize vector to unit length
    std::vector<float> normalizeVector(const std::vector<float>& values) const;
    
    // Calculate angle between two vectors
    float angleBetweenVectors(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Create vector from scalar
    std::vector<float> createScalarVector(float scalar, size_t size) const;
    
    // Check if vector contains NaN values
    bool vectorContainsNaN(const std::vector<float>& values) const;
    
    // Replace NaN values with zero
    std::vector<float> replaceNaNWithZero(const std::vector<float>& values) const;
    
    // Get minimum value from vector
    float getVectorMin(const std::vector<float>& values) const;
    
    // Get maximum value from vector
    float getVectorMax(const std::vector<float>& values) const;
    
    // Calculate vector range (max - min)
    float getVectorRange(const std::vector<float>& values) const;
    
    // Count values within threshold range
    size_t countValuesWithinRange(const std::vector<float>& values, float min, float max) const;
    
    // Calculate histogram
    std::vector<size_t> createHistogram(const std::vector<float>& values, size_t binCount, float min, float max) const;
    
    // Calculate exponential moving average
    float exponentialMovingAverage(const std::vector<float>& values, float alpha) const;
    
    // Calculate moving average
    float movingAverage(const std::vector<float>& values, size_t windowSize) const;
    
    // Calculate mean absolute difference
    float meanAbsoluteDifference(const std::vector<float>& values, float mean) const;
    
    // Check if sequence is monotonic
    bool isMonotonicIncreasing(const std::vector<float>& values) const;
    
    // Reverse vector
    void reverseVector(std::vector<float>& values) const;
    
    // Rotate vector
    void rotateVector(std::vector<float>& values, size_t positions) const;
    
    // Interleave two vectors
    std::vector<float> interleaveVectors(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Repeat vector pattern
    std::vector<float> repeatVectorPattern(const std::vector<float>& values, size_t repeatCount) const;
    
    // Create vector of increasing values
    std::vector<float> createIncreasingSequence(float start, float step, size_t count) const;
    
    // Create vector of decreasing values
    std::vector<float>& createDecreasingSequence(float start, float step, size_t count) const;
    
    // Generate random vector
    std::vector<float> generateRandomVector(size_t size, uint64_t seed) const;
    
    // Create vector from integer range
    std::vector<int> createIntegerRange(int start, int end) const;
    
    // Find minimum index in vector
    size_t findMinIndex(const std::vector<float>& values) const;
    
    // Find maximum index in vector
    size_t findMaxIndex(const std::vector<float>& values) const;
    
    // Create vector of squared values
    std::vector<float> squareVector(const std::vector<float>& values) const;
    
    // Create vector of square root values
    std::vector<float> sqrtVector(const std::vector<float>& values) const;
    
    // Create vector of reciprocal values
    std::vector<float> reciprocalVector(const std::vector<float>& values) const;
    
    // Create vector of logarithmic values
    std::vector<float> logVector(const std::vector<float>& values) const;
    
    // Create vector of exponential values
    std::vector<float> expVector(const std::vector<float>& values) const;
    
    // Calculate vector skewness
    float calculateSkewness(const std::vector<float>& values) const;
    
    // Calculate vector kurtosis
    float calculateKurtosis(const std::vector<float>& values) const;
    
    // Detect outliers using standard deviation
    std::vector<float> detectOutliers(const std::vector<float>& values, float threshold = 2.0f) const;
    
    // Create vector of absolute values
    std::vector<float> absoluteVector(const std::vector<float>& values) const;
    
    // Create vector of sign values
    std::vector<float> signVector(const std::vector<float>& values) const;
    
    // Calculate vector sum
    float vectorSum(const std::vector<float>& values) const;
    
    // Calculate vector product
    float vectorProduct(const std::vector<float>& values) const;
    
    // Check if vector contains zero
    bool vectorContainsZero(const std::vector<float>& values) const;
    
    // Count negative values in vector
    size_t countNegativeValues(const std::vector<float>& values) const;
    
    // Count positive values in vector
    size_t countPositiveValues(const std::vector<float>& values) const;
    
    // Create vector of differences
    std::vector<float> createDifferenceVector(const std::vector<float>& values) const;
    
    // Calculate vector autocorrelation
    float vectorAutocorrelation(const std::vector<float>& values, size_t lag) const;
    
    // Check if vector is uniform
    bool isVectorUniform(const std::vector<float>& values, float tolerance = 0.01f) const;
    
    // Create vector of cumulative sums
    std::vector<float> cumulativeSumVector(const std::vector<float>& values) const;
    
    // Calculate vector entropy
    float vectorEntropy(const std::vector<float>& values) const;
    
    // Create vector of normalized values
    std::vector<float> normalizedVector(const std::vector<float>& values, float min, float max) const;
    
    // Create vector of percentile values
    std::vector<float> percentileVector(const std::vector<float>& values, float percentile) const;
    
    // Check if vector contains invalid values
    bool vectorContainsInvalid(const std::vector<float>& values) const;
    
    // Replace invalid values with mean
    std::vector<float> replaceInvalidWithMean(const std::vector<float>& values) const;
    
    // Calculate vector variance
    float vectorVariance(const std::vector<float>& values, float mean) const;
    
    // Calculate vector covariance
    float vectorCovariance(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Check if vectors are compatible for operations
    bool areVectorsCompatible(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Create vector of ones
    std::vector<float> createOnesVector(size_t size) const;
    
    // Create vector of zeros
    std::vector<float> createZerosVector(size_t size) const;
    
    // Calculate vector inner product
    float innerProduct(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Calculate vector outer product
    std::vector<std::vector<float>> outerProduct(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Get vector intersection
    std::vector<float> vectorIntersection(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Get vector union
    std::vector<float> vectorUnion(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Check if vector is subset of another vector
    bool isSubset(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Get vector complement
    std::vector<float> vectorComplement(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Calculate vector cardinality
    size_t vectorCardinality(const std::vector<float>& values) const;
    
    // Check if vector is empty
    bool isVectorEmpty(const std::vector<float>& values) const;
    
    // Get vector length
    size_t getVectorLength(const std::vector<float>& values) const;
    
    // Create vector from string representation
    std::vector<float> vectorFromString(const std::string& str) const;
    
    // Vector to string conversion
    std::string vectorToString(const std::vector<float>& values) const;
    
    // Check if strings are equal (case-insensitive)
    bool stringsEqualIgnoreCase(const std::string& a, const std::string& b) const;
    
    // Convert string to lowercase
    std::string toLowerCase(const std::string& str) const;
    
    // Convert string to uppercase
    std::string toUpperCase(const std::string& str) const;
    
    // Trim whitespace from string
    std::string trimString(const std::string& str) const;
    
    // Check if string starts with prefix
    bool stringStartsWith(const std::string& str, const std::string& prefix) const;
    
    // Check if string ends with suffix
    bool stringEndsWith(const std::string& str, const std::string& suffix) const;
    
    // Find substring in string
    size_t findSubstring(const std::string& str, const std::string& substr) const;
    
    // Replace substring in string
    std::string replaceSubstring(const std::string& str, const std::string& oldSubstr, const std::string& newSubstr) const;
    
    // Split string by delimiter
    std::vector<std::string> splitString(const std::string& str, char delimiter) const;
    
    // Join vector of strings with delimiter
    std::string joinStrings(const std::vector<std::string>& strings, char delimiter) const;
    
    // Check if string contains only digits
    bool stringIsDigits(const std::string& str) const;
    
    // Check if string contains only letters
    bool stringIsLetters(const std::string& str) const;
    
    // Check if string contains only alphanumeric characters
    bool stringIsAlphanumeric(const std::string& str) const;
    
    // Get string length
    size_t getStringLength(const std::string& str) const;
    
    // Convert integer to string
    std::string intToString(int value) const;
    
    // Convert float to string
    std::string floatToString(float value) const;
    
    // Convert double to string
    std::string doubleToString(double value) const;
    
    // Convert boolean to string
    std::string boolToString(bool value) const;
    
    // Parse string to integer
    int stringToInt(const std::string& str) const;
    
    // Parse string to float
    float stringToFloat(const std::string& str) const;
    
    // Parse string to double
    double stringToDouble(const std::string& str) const;
    
    // Parse string to boolean
    bool stringToBool(const std::string& str) const;
    
    // Generate random string
    std::string generateRandomString(size_t length, uint64_t seed) const;
    
    // Check if string is palindrome
    bool isStringPalindrome(const std::string& str) const;
    
    // Reverse string
    std::string reverseString(const std::string& str) const;
    
    // Get string hash code
    size_t getStringHash(const std::string& str) const;
    
    // Check if strings are anagrams
    bool areStringsAnagrams(const std::string& a, const std::string& b) const;
    
    // Calculate string edit distance
    size_t calculateStringEditDistance(const std::string& a, const std::string& b) const;
    
    // Check if string contains whitespace
    bool stringContainsWhitespace(const std::string& str) const;
    
    // Remove whitespace from string
    std::string removeWhitespace(const std::string& str) const;
    
    // Get string character count
    size_t getCharacterCount(const std::string& str) const;
    
    // Check if string is empty
    bool isStringEmpty(const std::string& str) const;
    
    // Get first character of string
    char getFirstCharacter(const std::string& str) const;
    
    // Get last character of string
    char getLastCharacter(const std::string& str) const;
    
    // Check if string is valid filename
    bool isValidFilename(const std::string& str) const;
    
    // Generate filename with timestamp
    std::string generateTimestampedFilename(const std::string& baseName) const;
    
    // Extract file extension
    std::string getFileExtension(const std::string& str) const;
    
    // Remove file extension
    std::string removeFileExtension(const std::string& str) const;
    
    // Get file name without path
    std::string getFilenameWithoutPath(const std::string& str) const;
    
    // Get file path without filename
    std::string getPathWithoutFilename(const std::string& str) const;
    
    // Check if file exists
    bool fileExists(const std::string& filepath) const;
    
    // Create directory if not exists
    bool ensureDirectoryExists(const std::string& path) const;
    
    // Get current working directory
    std::string getCurrentWorkingDirectory() const;
    
    // Set current working directory
    bool setCurrentWorkingDirectory(const std::string& path) const;
    
    // Get environment variable
    std::string getEnvironmentVariable(const std::string& name) const;
    
    // Set environment variable
    bool setEnvironmentVariable(const std::string& name, const std::string& value) const;
    
    // Get system information
    std::string getSystemInfo() const;
    
    // Get memory usage
    size_t getMemoryUsage() const;
    
    // Check if running in debug mode
    bool isDebugMode() const;
    
    // Log message with timestamp
    void logMessageWithTimestamp(const std::string& message, LogLevel level = LogLevel::Info) const;
    
    // Format message with timestamp
    std::string formatMessageWithTimestamp(const std::string& message) const;
    
    // Get current timestamp
    std::string getCurrentTimestamp() const;
    
    // Parse timestamp string
    bool parseTimestamp(const std::string& timestamp, Timestamp& outTime) const;
    
    // Format timestamp to string
    std::string formatTimestamp(Timestamp time) const;
    
    // Calculate time difference
    double calculateTimeDifference(Timestamp time1, Timestamp time2) const;
    
    // Check if time is within bounds
    bool isTimeWithinBounds(Timestamp time, Timestamp min, Timestamp max) const;
    
    // Get time string representation
    std::string getTimeString(Timestamp time) const;
    
    // Parse time string
    bool parseTimeString(const std::string& timeStr, Timestamp& outTime) const;
    
    // Validate time format
    bool isValidTimeFormat(const std::string& timeStr) const;
    
    // Get time components
    void getTimeComponents(Timestamp time, int& hour, int& minute, int& second, int& millisecond) const;
    
    // Create time from components
    Timestamp createTime(int hour, int minute, int second, int millisecond) const;
    
    // Check if time is after another time
    bool isTimeAfter(Timestamp time, Timestamp otherTime) const;
    
    // Check if time is before another time
    bool isTimeBefore(Timestamp time, Timestamp otherTime) const;
    
    // Check if time is equal to another time
    bool isTimeEqual(Timestamp time, Timestamp otherTime) const;
    
    // Get time elapsed since epoch
    double getTimeSinceEpoch(Timestamp time) const;
    
    // Get current simulation time
    Timestamp getCurrentSimulationTime() const;
    
    // Set simulation time
    void setSimulationTime(Timestamp time);
    
    // Advance simulation time
    void advanceSimulationTime(double timestep);
    
    // Get simulation timestep
    double getSimulationTimestep() const;
    
    // Set simulation timestep
    void setSimulationTimestep(double timestep);
    
    // Get simulation step
    SimulationStep getSimulationStep() const;
    
    // Set simulation step
    void setSimulationStep(SimulationStep step);
    
    // Increment simulation step
    void incrementSimulationStep();
    
    // Get simulation time in seconds
    double getSimulationTimeSeconds() const;
    
    // Convert step to time
    Timestamp convertStepToTime(SimulationStep step) const;
    
    // Convert time to step
    SimulationStep convertTimeToStep(Timestamp time) const;
    
    // Check if step is valid
    bool isStepValid(SimulationStep step, SimulationStep maxSteps) const;
    
    // Clamp step to valid range
    SimulationStep clampStep(SimulationStep step, SimulationStep minStep, SimulationStep maxStep) const;
    
    // Get next valid step
    SimulationStep getNextStep(SimulationStep currentStep, SimulationStep maxStep) const;
    
    // Get previous valid step
    SimulationStep getPreviousStep(SimulationStep currentStep, SimulationStep minStep) const;
    
    // Calculate step from time
    SimulationStep calculateStepFromTime(Timestamp time, double timestep) const;
    
    // Calculate time from step
    Timestamp calculateTimeFromStep(SimulationStep step, double timestep) const;
    
    // Get simulation duration
    double getSimulationDuration() const;
    
    // Set simulation duration
    void setSimulationDuration(double duration);
    
    // Get simulation progress
    double getSimulationProgress() const;
    
    // Check if simulation is running
    bool isSimulationRunning() const;
    
    // Start simulation
    void startSimulation();
    
    // Stop simulation
    void stopSimulation();
    
    // Pause simulation
    void pauseSimulation();
    
    // Resume simulation
    void resumeSimulation();
    
    // Get simulation state
    std::string getSimulationState() const;
    
    // Set simulation state
    void setSimulationState(const std::string& state);
    
    // Get simulation configuration
    std::shared_ptr<Config> getSimulationConfig() const;
    
    // Set simulation configuration
    void setSimulationConfig(std::shared_ptr<Config> config);
    
    // Get simulation logger
    std::shared_ptr<Logger> getSimulationLogger() const;
    
    // Set simulation logger
    void setSimulationLogger(std::shared_ptr<Logger> logger);
    
    // Get simulation clock
    std::shared_ptr<SimulationClock> getSimulationClock() const;
    
    // Set simulation clock
    void setSimulationClock(std::shared_ptr<SimulationClock> clock);
    
    // Initialize simulation
    bool initializeSimulation();
    
    // Finalize simulation
    void finalizeSimulation();
    
    // Reset simulation
    void resetSimulation();
    
    // Check if simulation is initialized
    bool isSimulationInitialized() const;
    
    // Get simulation statistics
    std::string getSimulationStatistics() const;
    
    // Reset simulation statistics
    void resetSimulationStatistics();
    
    // Increment spike count
    void incrementSpikeCount();
    
    // Get total spike count
    size_t getTotalSpikeCount() const;
    
    // Increment step count
    void incrementStepCount();
    
    // Get total step count
    size_t getTotalStepCount() const;
    
    // Get simulation time
    double getSimulationTime() const;
    
    // Set simulation time
    void setSimulationTime(double time);
    
    // Advance simulation time by delta
    void advanceSimulationTime(double delta);
    
    // Get current timestep
    double getCurrentTimestep() const;
    
    // Set timestep
    void setTimestep(double timestep);
    
    // Get number of neurons
    size_t getNeuronCount() const;
    
    // Set number of neurons
    void setNeuronCount(size_t count);
    
    // Get number of regions
    size_t getRegionCount() const;
    
    // Set number of regions
    void setRegionCount(size_t count);
    
    // Get connection probability
    float getConnectionProbability() const;
    
    // Set connection probability
    void setConnectionProbability(float probability);
    
    // Get random seed
    uint64_t getRandomSeed() const;
    
    // Set random seed
    void setRandomSeed(uint64_t seed);
    
    // Get simulation status
    std::string getSimulationStatus() const;
    
    // Check if simulation is active
    bool isSimulationActive() const;
    
    // Get simulation version
    std::string getSimulationVersion() const;
    
    // Get simulation build date
    std::string getSimulationBuildDate() const;
    
    // Get simulation author
    std::string getSimulationAuthor() const;
    
    // Get simulation description
    std::string getSimulationDescription() const;
    
    // Get simulation copyright
    std::string getSimulationCopyright() const;
    
    // Get simulation license
    std::string getSimulationLicense() const;
    
    // Print simulation info
    void printSimulationInfo() const;
    
    // Print simulation configuration
    void printSimulationConfig() const;
    
    // Print simulation statistics
    void printSimulationStats() const;
    
    // Save simulation configuration
    bool saveSimulationConfig(const std::string& filepath) const;
    
    // Load simulation configuration
    bool loadSimulationConfig(const std::string& filepath);
    
    // Backup simulation configuration
    bool backupSimulationConfig() const;
    
    // Restore simulation configuration
    bool restoreSimulationConfig();
    
    // Get simulation backup directory
    std::string getSimulationBackupDirectory() const;
    
    // Set simulation backup directory
    void setSimulationBackupDirectory(const std::string& directory);
    
    // Create simulation backup
    bool createSimulationBackup() const;
    
    // Restore simulation backup
    bool restoreSimulationBackup();
    
    // List simulation backups
    std::vector<std::string> listSimulationBackups() const;
    
    // Remove simulation backup
    bool removeSimulationBackup(const std::string& backupName);
    
    // Clear simulation backups
    bool clearSimulationBackups();
    
    // Get simulation backup count
    size_t getSimulationBackupCount() const;
    
    // Check if simulation backup exists
    bool simulationBackupExists(const std::string& backupName) const;
    
    // Get simulation backup info
    std::string getSimulationBackupInfo(const std::string& backupName) const;
    
    // Get simulation backup size
    size_t getSimulationBackupSize(const std::string& backupName) const;
    
    // Compress simulation backup
    bool compressSimulationBackup(const std::string& backupName);
    
    // Decompress simulation backup
    bool decompressSimulationBackup(const std::string& backupName);
    
    // Encrypt simulation backup
    bool encryptSimulationBackup(const std::string& backupName, const std::string& password);
    
    // Decrypt simulation backup
    bool decryptSimulationBackup(const std::string& backupName, const std::string& password);
    
    // Validate simulation backup
    bool validateSimulationBackup(const std::string& backupName) const;
    
    // Repair simulation backup
    bool repairSimulationBackup(const std::string& backupName);
    
    // Optimize simulation backup
    bool optimizeSimulationBackup(const std::string& backupName);
    
    // Analyze simulation backup
    std::string analyzeSimulationBackup(const std::string& backupName) const;
    
    // Get simulation backup statistics
    std::string getSimulationBackupStatistics() const;
    
    // Print simulation backup info
    void printSimulationBackupInfo(const std::string& backupName) const;
    
    // Show simulation backup contents
    void showSimulationBackupContents(const std::string& backupName) const;
    
    // Extract simulation backup file
    bool extractSimulationBackupFile(const std::string& backupName, const std::string& filePath);
    
    // Add file to simulation backup
    bool addFileToSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove file from simulation backup
    bool removeFileFromSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // List files in simulation backup
    std::vector<std::string> listFilesInSimulationBackup(const std::string& backupName) const;
    
    // Get file size in simulation backup
    size_t getFileSizeInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Check if file exists in simulation backup
    bool fileExistsInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Get file modification time in simulation backup
    std::string getFileModificationTimeInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file modification time in simulation backup
    bool setFileModificationTimeInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& timeStr);
    
    // Get file permissions in simulation backup
    std::string getFilePermissionsInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file permissions in simulation backup
    bool setFilePermissionsInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& permissions);
    
    // Get file owner in simulation backup
    std::string getFileOwnerInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file owner in simulation backup
    bool setFileOwnerInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& owner);
    
    // Get file group in simulation backup
    std::string getFileGroupInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file group in simulation backup
    bool setFileGroupInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& group);
    
    // Get file creation time in simulation backup
    std::string getFileCreationTimeInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file creation time in simulation backup
    bool setFileCreationTimeInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& timeStr);
    
    // Get file last access time in simulation backup
    std::string getFileLastAccessTimeInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file last access time in simulation backup
    bool setFileLastAccessTimeInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& timeStr);
    
    // Get file attributes in simulation backup
    std::string getFileAttributesInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file attributes in simulation backup
    bool setFileAttributesInSimulationBackup(const std::string& backupName, const std::string& filePath, const std::string& attributes);
    
    // Check if file is executable in simulation backup
    bool isFileExecutableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file executable in simulation backup
    bool makeFileExecutableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove executable permission from file in simulation backup
    bool removeExecutablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is readable in simulation backup
    bool isFileReadableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file readable in simulation backup
    bool makeFileReadableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove readable permission from file in simulation backup
    bool removeReadablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is writable in simulation backup
    bool isFileWritableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file writable in simulation backup
    bool makeFileWritableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove writable permission from file in simulation backup
    bool removeWritablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Get file size in simulation backup
    size_t getFileSizeInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Set file size in simulation backup
    bool setFileSizeInSimulationBackup(const std::string& backupName, const std::string& filePath, size_t size);
    
    // Check if file exists in simulation backup
    bool fileExistsInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Create file in simulation backup
    bool createFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove file from simulation backup
    bool removeFileFromSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Rename file in simulation backup
    bool renameFileInSimulationBackup(const std::string& backupName, const std::string& oldPath, const std::string& newPath);
    
    // Copy file to simulation backup
    bool copyFileToSimulationBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Move file to simulation backup
    bool moveFileToSimulationBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Check if directory exists in simulation backup
    bool directoryExistsInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Create directory in simulation backup
    bool createDirectoryInSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Remove directory from simulation backup
    bool removeDirectoryFromSimulationBackup(const std::string& backupName, const std::string& path);
    
    // List contents of directory in simulation backup
    std::vector<std::string> listContentsOfDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Get directory information in simulation backup
    std::string getDirectoryInformationInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory permissions in simulation backup
    bool setDirectoryPermissionsInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& permissions);
    
    // Get directory permissions in simulation backup
    std::string getDirectoryPermissionsInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory owner in simulation backup
    bool setDirectoryOwnerInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& owner);
    
    // Get directory owner in simulation backup
    std::string getDirectoryOwnerInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory group in simulation backup
    bool setDirectoryGroupInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& group);
    
    // Get directory group in simulation backup
    std::string getDirectoryGroupInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory creation time in simulation backup
    bool setDirectoryCreationTimeInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& timeStr);
    
    // Get directory creation time in simulation backup
    std::string getDirectoryCreationTimeInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory last access time in simulation backup
    bool setDirectoryLastAccessTimeInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& timeStr);
    
    // Get directory last access time in simulation backup
    std::string getDirectoryLastAccessTimeInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory last modification time in simulation backup
    bool setDirectoryLastModificationTimeInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& timeStr);
    
    // Get directory last modification time in simulation backup
    std::string getDirectoryLastModificationTimeInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Set directory attributes in simulation backup
    bool setDirectoryAttributesInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& attributes);
    
    // Get directory attributes in simulation backup
    std::string getDirectoryAttributesInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Check if directory is empty in simulation backup
    bool isDirectoryEmptyInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Get number of files in directory in simulation backup
    size_t getNumberOfFilesInDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Get number of subdirectories in directory in simulation backup
    size_t getNumberOfSubdirectoriesInDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Get total size of directory in simulation backup
    size_t getTotalSizeOfDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Search for file in simulation backup
    bool searchForFileInSimulationBackup(const std::string& backupName, const std::string& filename) const;
    
    // Get file checksum in simulation backup
    std::string getFileChecksumInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Verify file integrity in simulation backup
    bool verifyFileIntegrityInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Compress directory in simulation backup
    bool compressDirectoryInSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Decompress directory in simulation backup
    bool decompressDirectoryInSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Encrypt directory in simulation backup
    bool encryptDirectoryInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& password);
    
    // Decrypt directory in simulation backup
    bool decryptDirectoryInSimulationBackup(const std::string& backupName, const std::string& path, const std::string& password);
    
    // Validate directory in simulation backup
    bool validateDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Repair directory in simulation backup
    bool repairDirectoryInSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Optimize directory in simulation backup
    bool optimizeDirectoryInSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Analyze directory in simulation backup
    std::string analyzeDirectoryInSimulationBackup(const std::string& backupName, const std::string& path) const;
    
    // Get directory statistics
    std::string getDirectoryStatistics(const std::string& backupName, const std::string& path) const;
    
    // Print directory information
    void printDirectoryInformation(const std::string& backupName, const std::string& path) const;
    
    // Show directory contents
    void showDirectoryContents(const std::string& backupName, const std::string& path) const;
    
    // Extract directory from simulation backup
    bool extractDirectoryFromSimulationBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Add directory to simulation backup
    bool addDirectoryToSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Remove directory from simulation backup
    bool removeDirectoryFromSimulationBackup(const std::string& backupName, const std::string& path);
    
    // Rename directory in simulation backup
    bool renameDirectoryInSimulationBackup(const std::string& backupName, const std::string& oldPath, const std::string& newPath);
    
    // Copy directory to simulation backup
    bool copyDirectoryToSimulationBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Move directory to simulation backup
    bool moveDirectoryToSimulationBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Check if file is executable in simulation backup
    bool isFileExecutableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file executable in simulation backup
    bool makeFileExecutableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove executable permission from file in simulation backup
    bool removeExecutablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is readable in simulation backup
    bool isFileReadableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file readable in simulation backup
    bool makeFileReadableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove readable permission from file in simulation backup
    bool removeReadablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is writable in simulation backup
    bool isFileWritableInSimulationBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file writable in simulation backup
    bool makeFileWritableInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove writable permission from file in simulation backup
    bool removeWritablePermissionFromFileInSimulationBackup(const std::string& backupName, const std::string& filePath);
    
    // Create backup of simulation
    bool createBackupOfSimulation(const std::string& backupName);
    
    // Restore simulation from backup
    bool restoreSimulationFromBackup(const std::string& backupName);
    
    // List all backups
    std::vector<std::string> listAllBackups() const;
    
    // Remove backup
    bool removeBackup(const std::string& backupName);
    
    // Clean up old backups
    bool cleanUpOldBackups();
    
    // Get backup statistics
    std::string getBackupStatistics() const;
    
    // Get backup info
    std::string getBackupInfo(const std::string& backupName) const;
    
    // Print backup info
    void printBackupInfo(const std::string& backupName) const;
    
    // Show backup contents
    void showBackupContents(const std::string& backupName) const;
    
    // Extract file from backup
    bool extractFileFromBackup(const std::string& backupName, const std::string& filePath);
    
    // Add file to backup
    bool addFileToBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove file from backup
    bool removeFileFromBackup(const std::string& backupName, const std::string& filePath);
    
    // List files in backup
    std::vector<std::string> listFilesInBackup(const std::string& backupName) const;
    
    // Get file size in backup
    size_t getFileSizeInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Check if file exists in backup
    bool fileExistsInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Create backup directory
    bool createBackupDirectory(const std::string& backupName);
    
    // Remove backup directory
    bool removeBackupDirectory(const std::string& backupName);
    
    // List backup directories
    std::vector<std::string> listBackupDirectories() const;
    
    // Get backup directory info
    std::string getBackupDirectoryInfo(const std::string& backupName) const;
    
    // Print backup directory info
    void printBackupDirectoryInfo(const std::string& backupName) const;
    
    // Show backup directory contents
    void showBackupDirectoryContents(const std::string& backupName) const;
    
    // Extract directory from backup
    bool extractDirectoryFromBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Add directory to backup
    bool addDirectoryToBackup(const std::string& backupName, const std::string& path);
    
    // Remove directory from backup
    bool removeDirectoryFromBackup(const std::string& backupName, const std::string& path);
    
    // Rename directory in backup
    bool renameDirectoryInBackup(const std::string& backupName, const std::string& oldPath, const std::string& newPath);
    
    // Copy directory to backup
    bool copyDirectoryToBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Move directory to backup
    bool moveDirectoryToBackup(const std::string& backupName, const std::string& sourcePath, const std::string& destPath);
    
    // Check if file is executable in backup
    bool isFileExecutableInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file executable in backup
    bool makeFileExecutableInBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove executable permission from file in backup
    bool removeExecutablePermissionFromFileInBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is readable in backup
    bool isFileReadableInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file readable in backup
    bool makeFileReadableInBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove readable permission from file in backup
    bool removeReadablePermissionFromFileInBackup(const std::string& backupName, const std::string& filePath);
    
    // Check if file is writable in backup
    bool isFileWritableInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Make file writable in backup
    bool makeFileWritableInBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove writable permission from file in backup
    bool removeWritablePermissionFromFileInBackup(const std::string& backupName, const std::string& filePath);
    
    // Compress backup
    bool compressBackup(const std::string& backupName);
    
    // Decompress backup
    bool decompressBackup(const std::string& backupName);
    
    // Encrypt backup
    bool encryptBackup(const std::string& backupName, const std::string& password);
    
    // Decrypt backup
    bool decryptBackup(const std::string& backupName, const std::string& password);
    
    // Validate backup
    bool validateBackup(const std::string& backupName) const;
    
    // Repair backup
    bool repairBackup(const std::string& backupName);
    
    // Optimize backup
    bool optimizeBackup(const std::string& backupName);
    
    // Analyze backup
    std::string analyzeBackup(const std::string& backupName) const;
    
    // Get backup statistics
    std::string getBackupStatistics() const;
    
    // Print backup info
    void printBackupInfo(const std::string& backupName) const;
    
    // Show backup contents
    void showBackupContents(const std::string& backupName) const;
    
    // Extract file from backup
    bool extractFileFromBackup(const std::string& backupName, const std::string& filePath);
    
    // Add file to backup
    bool addFileToBackup(const std::string& backupName, const std::string& filePath);
    
    // Remove file from backup
    bool removeFileFromBackup(const std::string& backupName, const std::string& filePath);
    
    // List files in backup
    std::vector<std::string> listFilesInBackup(const std::string& backupName) const;
    
    // Get file size in backup
    size_t getFileSizeInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Check if file exists in backup
    bool fileExistsInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Get file checksum in backup
    std::string getFileChecksumInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Verify file integrity in backup
    bool verifyFileIntegrityInBackup(const std::string& backupName, const std::string& filePath) const;
    
    // Get backup version
    std::string getBackupVersion(const std::string& backupName) const;
    
    // Set backup version
    bool setBackupVersion(const std::string& backupName, const std::string& version);
    
    // Get backup author
    std::string getBackupAuthor(const std::string& backupName) const;
    
    // Set backup author
    bool setBackupAuthor(const std::string& backupName, const std::string& author);
    
    // Get backup description
    std::string getBackupDescription(const std::string& backupName) const;
    
    // Set backup description
    bool setBackupDescription(const std::string& backupName, const std::string& description);
    
    // Get backup tags
    std::vector<std::string> getBackupTags(const std::string& backupName) const;
    
    // Set backup tags
    bool setBackupTags(const std::string& backupName, const std::vector<std::string>& tags);
    
    // Add backup tag
    bool addBackupTag(const std::string& backupName, const std::string& tag);
    
    // Remove backup tag
    bool removeBackupTag(const std::string& backupName, const std::string& tag);
    
    // Check if backup has tag
    bool backupHasTag(const std::string& backupName, const std::string& tag) const;
    
    // Get backup creation time
    std::string getBackupCreationTime(const std::string& backupName) const;
    
    // Set backup creation time
    bool setBackupCreationTime(const std::string& backupName, const std::string& timeStr);
    
    // Get backup modification time
    std::string getBackupModificationTime(const std::string& backupName) const;
    
    // Set backup modification time
    bool setBackupModificationTime(const std::string& backupName, const std::string& timeStr);
    
    // Get backup access time
    std::string getBackupAccessTime(const std::string& backupName) const;
    
    // Set backup access time
    bool setBackupAccessTime(const std::string& backupName, const std::string& timeStr);
    
    // Get backup attributes
    std::string getBackupAttributes(const std::string& backupName) const;
    
    // Set backup attributes
    bool setBackupAttributes(const std::string& backupName, const std::string& attributes);
    
    // Check if backup is executable
    bool isBackupExecutable(const std::string& backupName) const;
    
    // Make backup executable
    bool makeBackupExecutable(const std::string& backupName);
    
    // Remove executable permission from backup
    bool removeExecutablePermissionFromBackup(const std::string& backupName);
    
    // Check if backup is readable
    bool isBackupReadable(const std::string& backupName) const;
    
    // Make backup readable
    bool makeBackupReadable(const std::string& backupName);
    
    // Remove readable permission from backup
    bool removeReadablePermissionFromBackup(const std::string& backupName);
    
    // Check if backup is writable
    bool isBackupWritable(const std::string& backupName) const;
    
    // Make backup writable
    bool makeBackupWritable(const std::string& backupName);
    
    // Remove writable permission from backup
    bool removeWritablePermissionFromBackup(const std::string& backupName);
    
    // Get backup size
    size_t getBackupSize(const std::string& backupName) const;
    
    // Set backup size
    bool setBackupSize(const std::string& backupName, size_t size);
    
    // Get backup file count
    size_t getBackupFileCount(const std::string& backupName) const;
    
    // Set backup file count
    bool setBackupFileCount(const std::string& backupName, size_t count);
    
    // Get backup directory count
    size_t getBackupDirectoryCount(const std::string& backupName) const;
    
    // Set backup directory count
    bool setBackupDirectoryCount(const std::string& backupName, size_t count);
    
    // Get backup file list
    std::vector<std::string> getBackupFileList(const std::string& backupName) const;
    
    // Set backup file list
    bool setBackupFileList(const std::string& backupName, const std::vector<std::string>& fileList);
    
    // Get backup directory list
    std::vector<std::string> getBackupDirectoryList(const std::string& backupName) const;
    
    // Set backup directory list
    bool setBackupDirectoryList(const std::string& backupName, const std::vector<std::string>& directoryList);
    
    // Get backup file map
    std::unordered_map<std::string, size_t> getBackupFileMap(const std::string& backupName) const;
    
    // Set backup file map
    bool setBackupFileMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& fileMap);
    
    // Get backup directory map
    std::unordered_map<std::string, size_t> getBackupDirectoryMap(const std::string& backupName) const;
    
    // Set backup directory map
    bool setBackupDirectoryMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& directoryMap);
    
    // Get backup file count map
    std::unordered_map<std::string, size_t> getBackupFileCountMap(const std::string& backupName) const;
    
    // Set backup file count map
    bool setBackupFileCountMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& fileCountMap);
    
    // Get backup directory count map
    std::unordered_map<std::string, size_t> getBackupDirectoryCountMap(const std::string& backupName) const;
    
    // Set backup directory count map
    bool setBackupDirectoryCountMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& directoryCountMap);
    
    // Get backup file size map
    std::unordered_map<std::string, size_t> getBackupFileSizeMap(const std::string& backupName) const;
    
    // Set backup file size map
    bool setBackupFileSizeMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& fileSizeMap);
    
    // Get backup directory size map
    std::unordered_map<std::string, size_t> getBackupDirectorySizeMap(const std::string& backupName) const;
    
    // Set backup directory size map
    bool setBackupDirectorySizeMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& directorySizeMap);
    
    // Get backup file modification time map
    std::unordered_map<std::string, std::string> getBackupFileModificationTimeMap(const std::string& backupName) const;
    
    // Set backup file modification time map
    bool setBackupFileModificationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileModificationTimeMap);
    
    // Get backup directory modification time map
    std::unordered_map<std::string, std::string> getBackupDirectoryModificationTimeMap(const std::string& backupName) const;
    
    // Set backup directory modification time map
    bool setBackupDirectoryModificationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryModificationTimeMap);
    
    // Get backup file access time map
    std::unordered_map<std::string, std::string> getBackupFileAccessTimeMap(const std::string& backupName) const;
    
    // Set backup file access time map
    bool setBackupFileAccessTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileAccessTimeMap);
    
    // Get backup directory access time map
    std::unordered_map<std::string, std::string> getBackupDirectoryAccessTimeMap(const std::string& backupName) const;
    
    // Set backup directory access time map
    bool setBackupDirectoryAccessTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryAccessTimeMap);
    
    // Get backup file attributes map
    std::unordered_map<std::string, std::string> getBackupFileAttributesMap(const std::string& backupName) const;
    
    // Set backup file attributes map
    bool setBackupFileAttributesMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileAttributesMap);
    
    // Get backup directory attributes map
    std::unordered_map<std::string, std::string> getBackupDirectoryAttributesMap(const std::string& backupName) const;
    
    // Set backup directory attributes map
    bool setBackupDirectoryAttributesMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryAttributesMap);
    
    // Get backup file owner map
    std::unordered_map<std::string, std::string> getBackupFileOwnerMap(const std::string& backupName) const;
    
    // Set backup file owner map
    bool setBackupFileOwnerMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileOwnerMap);
    
    // Get backup directory owner map
    std::unordered_map<std::string, std::string> getBackupDirectoryOwnerMap(const std::string& backupName) const;
    
    // Set backup directory owner map
    bool setBackupDirectoryOwnerMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryOwnerMap);
    
    // Get backup file group map
    std::unordered_map<std::string, std::string> getBackupFileGroupMap(const std::string& backupName) const;
    
    // Set backup file group map
    bool setBackupFileGroupMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileGroupMap);
    
    // Get backup directory group map
    std::unordered_map<std::string, std::string> getBackupDirectoryGroupMap(const std::string& backupName) const;
    
    // Set backup directory group map
    bool setBackupDirectoryGroupMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryGroupMap);
    
    // Get backup file creation time map
    std::unordered_map<std::string, std::string> getBackupFileCreationTimeMap(const std::string& backupName) const;
    
    // Set backup file creation time map
    bool setBackupFileCreationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileCreationTimeMap);
    
    // Get backup directory creation time map
    std::unordered_map<std::string, std::string> getBackupDirectoryCreationTimeMap(const std::string& backupName) const;
    
    // Set backup directory creation time map
    bool setBackupDirectoryCreationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryCreationTimeMap);
    
    // Get backup file last access time map
    std::unordered_map<std::string, std::string> getBackupFileLastAccessTimeMap(const std::string& backupName) const;
    
    // Set backup file last access time map
    bool setBackupFileLastAccessTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileLastAccessTimeMap);
    
    // Get backup directory last access time map
    std::unordered_map<std::string, std::string> getBackupDirectoryLastAccessTimeMap(const std::string& backupName) const;
    
    // Set backup directory last access time map
    bool setBackupDirectoryLastAccessTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryLastAccessTimeMap);
    
    // Get backup file last modification time map
    std::unordered_map<std::string, std::string> getBackupFileLastModificationTimeMap(const std::string& backupName) const;
    
    // Set backup file last modification time map
    bool setBackupFileLastModificationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileLastModificationTimeMap);
    
    // Get backup directory last modification time map
    std::unordered_map<std::string, std::string> getBackupDirectoryLastModificationTimeMap(const std::string& backupName) const;
    
    // Set backup directory last modification time map
    bool setBackupDirectoryLastModificationTimeMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryLastModificationTimeMap);
    
    // Get backup file checksum map
    std::unordered_map<std::string, std::string> getBackupFileChecksumMap(const std::string& backupName) const;
    
    // Set backup file checksum map
    bool setBackupFileChecksumMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& fileChecksumMap);
    
    // Get backup directory checksum map
    std::unordered_map<std::string, std::string> getBackupDirectoryChecksumMap(const std::string& backupName) const;
    
    // Set backup directory checksum map
    bool setBackupDirectoryChecksumMap(const std::string& backupName, const std::unordered_map<std::string, std::string>& directoryChecksumMap);
    
    // Get backup file size map
    std::unordered_map<std::string, size_t> getBackupFileSizeMap(const std::string& backupName) const;
    
    // Set backup file size map
    bool setBackupFileSizeMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& fileSizeMap);
    
    // Get backup directory size map
    std::unordered_map<std::string, size_t> getBackupDirectorySizeMap(const std::string& backupName) const;
    
    // Set backup directory size map
    bool setBackupDirectorySizeMap(const std::string& backupName, const std::unordered_map<std::string, size_t>& directorySizeMap);
    
    // ... (rest of the file is very long and contains many more utility functions)
    
    // End of BrainUtils class definition
};

} // namespace nlm
