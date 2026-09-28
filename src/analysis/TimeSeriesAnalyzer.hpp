#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../core/Types/Types.hpp"
#include "../experiments/ExperimentMetrics.hpp"

namespace nlm {

/**
 * @brief Time series analysis class
 * 
 * This class provides comprehensive time series analysis capabilities for neural
 * recording data, including power spectrum computation, stationarity detection,
 * autocorrelation analysis, ARIMA model fitting, regime change detection,
 * and future activity prediction.
 * 
 * Designed for:
 * - Statistical analysis - Hypothesis testing and validation
 * - Machine learning - Activity patterns as ML features
 * - Performance analysis - Bottleneck identification and optimization
 * - Research visualization - Analysis for visualization
 */
class TimeSeriesAnalyzer {
public:
    /**
     * @brief ARIMA model configuration
     */
    struct ARIMAConfig {
        int p;  // Autoregressive order
        int d;  // Differencing order
        int q;  // Moving average order
        float penalty;  // Information criterion penalty (AIC/BIC)
        int maxLag;     // Maximum lag for ACF/PACF
        bool stationary; // Whether to enforce stationarity
        
        ARIMAConfig() : p(1), d(1), q(1), penalty(1.0f), maxLag(40), stationary(true) {}
        ARIMAConfig(int p_order, int d_order, int q_order)
            : p(p_order), d(d_order), q(q_order), penalty(1.0f), maxLag(40), stationary(true) {}
    };

    /**
     * @brief Regime change detection result
     */
    struct RegimeChangeResult {
        SimulationStep changePoint;
        Timestamp changeTime;
        float changeMagnitude;
        std::string regimeBefore;
        std::string regimeAfter;
        float confidence;
        std::map<std::string, float> changeMetrics;
    };

    /**
     * @brief Prediction result
     */
    struct PredictionResult {
        std::vector<float> predictedValues;
        std::vector<float> predictionErrors;
        std::vector<float> confidenceIntervals;
        SimulationStep forecastSteps;
        float meanSquaredError;
        float meanAbsoluteError;
        float rSquared;
    };

    /**
     * @brief Constructor
     * 
     * @param timeSeriesData Time series data to analyze (vector of values)
     * @param samplingRate Sampling rate in Hz (samples per second)
     */
    TimeSeriesAnalyzer(
        const std::vector<float>& timeSeriesData,
        float samplingRate = 1000.0f
    );

    /**
     * @brief Destructor
     */
    ~TimeSeriesAnalyzer();

    /**
     * @brief Copy constructor
     */
    TimeSeriesAnalyzer(const TimeSeriesAnalyzer& other);

    /**
     * @brief Copy assignment operator
     */
    TimeSeriesAnalyzer& operator=(const TimeSeriesAnalyzer& other);

    /**
     * @brief Move constructor
     */
    TimeSeriesAnalyzer(TimeSeriesAnalyzer&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    TimeSeriesAnalyzer& operator=(TimeSeriesAnalyzer&& other) noexcept;

    /**
     * @brief Get time series data
     * 
     * @return Vector of time series values
     */
    std::vector<float> getTimeSeriesData() const;

    /**
     * @brief Get sampling rate
     * 
     * @return Sampling rate in Hz
     */
    float getSamplingRate() const;

    /**
     * @brief Set sampling rate
     * 
     * @param rate Sampling rate in Hz
     */
    void setSamplingRate(float rate);

    /**
     * @brief Set time series data
     * 
     * @param data New time series data
     */
    void setTimeSeriesData(const std::vector<float>& data);

    /**
     * @brief Compute power spectrum
     * 
     * Computes the power spectral density of the time series using periodogram,
     * Welch's method, or other spectral estimation techniques. Identifies
     * dominant frequencies and power distribution across frequency bands.
     * 
     * @param method Spectral estimation method ("periodogram", "welch", "blackman")
     * @param frequencyBand Frequency band of interest (0-1 normalized)
     * @return Map containing frequencies and corresponding power values
     */
    std::map<float, float> computePowerSpectrum(
        const std::string& method = "welch",
        float frequencyBand = 1.0f
    );

    /**
     * @brief Detect stationarity
     * 
     * Tests for stationarity in the time series using various statistical tests
     * including Augmented Dickey-Fuller test, KPSS test, and others. Returns
     * information about the p-values and test statistics.
     * 
     * @param testType Type of stationarity test ("adf", "kpss", "phillips")
     * @return Map containing stationarity test results
     */
    std::map<std::string, float> detectStationarity(
        const std::string& testType = "adf"
    );

    /**
     * @brief Compute autocorrelation
     * 
     * Computes the autocorrelation function of the time series, showing how
     * the time series correlates with itself at different time lags. Also
     * computes partial autocorrelation function.
     * 
     * @param maxLag Maximum lag to compute autocorrelation for
     * @param normalize Whether to normalize autocorrelation to [-1, 1]
     * @return Map containing lags and corresponding autocorrelation values
     */
    std::map<SimulationStep, float> computeAutoCorrelation(
        SimulationStep maxLag = 50,
        bool normalize = true
    );

    /**
     * @brief Fit ARIMA model
     * 
     * Fits an ARIMA (AutoRegressive Integrated Moving Average) model to the
     * time series data. Uses information criteria (AIC/BIC) to select optimal
     * model parameters.
     * 
     * @param config ARIMA model configuration
     * @return Fitted ARIMA model parameters
     */
    std::map<std::string, float> fitARIMA(const ARIMAConfig& config);

    /**
     * @brief Predict future values
     * 
     * Predicts future values of the time series using the fitted ARIMA model
     * or other prediction methods. Provides confidence intervals and error estimates.
     * 
     * @param steps Number of steps to predict forward
     * @param confidence Confidence level for prediction intervals (0-1)
     * @return Prediction results including predicted values and confidence intervals
     */
    PredictionResult predictFuture(
        SimulationStep steps,
        float confidence = 0.95f
    );

    /**
     * @brief Detect regime changes
     * 
     * Detects regime changes in the time series using change-point detection
     * algorithms such as CUSUM, Bayesian change-point detection, or sliding
     * window statistics. Identifies periods where the statistical properties
     * of the time series change.
     * 
     * @param method Change-point detection method
     * @param sensitivity Sensitivity parameter for change detection
     * @return Vector of regime change detection results
     */
    std::vector<RegimeChangeResult> detectRegimes(
        const std::string& method = "cusum",
        float sensitivity = 0.95f
    );

    /**
     * @brief Compute statistical moments
     * 
     * Computes various statistical moments of the time series including mean,
     * variance, skewness, kurtosis, and higher-order moments.
     * 
     * @return Map containing statistical moments and their values
     */
    std::map<std::string, float> computeStatisticalMoments();

    /**
     * @brief Compute frequency domain features
     * 
     * Computes frequency domain features including dominant frequency, spectral
     * centroid, spectral entropy, and other frequency-based metrics.
     * 
     * @return Map containing frequency domain features
     */
    std::map<std::string, float> computeFrequencyDomainFeatures();

    /**
     * @brief Compute time-frequency representation
     * 
     * Computes time-frequency representation using wavelet transform or
     * short-time Fourier transform to analyze non-stationary time series.
     * 
     * @param method Time-frequency method ("wavelet", "stft")
     * @param windowSize Window size for time-frequency analysis
     * @return 2D vector representing time-frequency power
     */
    std::vector<std::vector<float>> computeTimeFrequencyRepresentation(
        const std::string& method = "wavelet",
        SimulationStep windowSize = 50
    );

    /**
     * @brief Generate forecasting model
     * 
     * Generates a comprehensive forecasting model including model selection,
     * parameter estimation, and model validation. Returns model information
     * and performance metrics.
     * 
     * @return Map containing forecasting model information
     */
    std::map<std::string, std::vector<float>> generateForecastingModel();

    /**
     * @brief Validate model
     * 
     * Validates the time series model using cross-validation or out-of-sample
     * testing. Returns validation metrics and model performance statistics.
     * 
     * @param testFraction Fraction of data to use for testing (0-1)
     * @return Map containing model validation metrics
     */
    std::map<std::string, float> validateModel(
        float testFraction = 0.2f
    );

    /**
     * @brief Export analysis results
     * 
     * @param filepath Path to save analysis results (CSV/JSON format)
     * @return true if export was successful, false otherwise
     */
    bool exportResults(const std::string& filepath);

    /**
     * @brief Import time series data
     * 
     * @param filepath Path to time series data file (CSV format)
     * @param samplingRate Sampling rate to use if not in file
     * @return true if import was successful, false otherwise
     */
    bool importTimeSeries(const std::string& filepath, float samplingRate = 0.0f);

    /**
     * @brief Get analysis summary
     * 
     * @return String summary of time series analysis
     */
    std::string getAnalysisSummary() const;

    /**
     * @brief Reset analyzer
     */
    void reset();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
