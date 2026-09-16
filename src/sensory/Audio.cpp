#include "Audio.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>
#include <queue>
#include <random>
#include <complex>
#include <mutex>
#include <thread>

namespace nlm {

// Forward declarations for neural processing components
class AudioFeatureExtractor;
class FrequencyAnalyzer;
class SoundLocalizer;
class TemporalProcessor;
class SpeechMusicRecognizer;
class AudioFeatureMap;

struct AudioProcessor::Impl {
    // Neural processing components
    std::unique_ptr<AudioFeatureExtractor> featureExtractor;
    std::unique_ptr<FrequencyAnalyzer> freqAnalyzer;
    std::unique_ptr<SoundLocalizer> localizer;
    std::unique_ptr<TemporalProcessor> temporalProcessor;
    std::unique_ptr<SpeechMusicRecognizer> speechMusicRecognizer;
    std::unique_ptr<AudioFeatureMap> featureMap;
    
    // Brain integration
    Brain* brain;
    std::vector<Neuron*> audioNeurons;
    std::vector<std::unique_ptr<NeuralPopulation>> audioPopulations;
    
    // Audio processing buffers
    std::vector<float> inputBuffer;
    std::vector<float> features;
    size_t featureDimensions;
    std::vector<float> frequencyFeatures;
    std::vector<float> spatialFeatures;
    std::vector<float> temporalFeatures;
    std::vector<float> spectralFeatures;
    std::vector<float> hierarchicalFeatures;
    
    // Audio parameters
    size_t sampleRate;
    size_t bufferSize;
    size_t numChannels;
    std::vector<std::vector<float>> binauralBuffers;
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;
    
    // Temporal processing
    std::vector<float> temporalBuffer;
    size_t maxTemporalDepth;
    float temporalSmoothing;
    std::vector<float> envelopeData;
    std::vector<float> rhythmData;
    
    // Spectral processing
    std::vector<std::vector<float>> spectrogram;
    size_t fftSize;
    std::vector<float> melFilterBank;
    std::vector<float> criticalBands;
    
    // State management
    bool isInitialized;
    size_t lastStep;
    float sensitivityThreshold;
    float noiseLevel;
    float localizationAccuracy;
    
    // Statistics
    size_t totalFeaturesProcessed;
    size_t speechSegmentsDetected;
    size_t musicSegmentsDetected;
    float averageSoundLevel;
    float localizationConfidence;
    
    // Thread safety
    mutable std::mutex processingMutex;
    
    // Receptive fields for frequency and spatial processing
    std::vector<std::vector<float>> frequencyReceptiveFields;
    std::vector<std::vector<float>> spatialReceptiveFields;
    
    Impl() : 
        featureExtractor(nullptr),
        freqAnalyzer(nullptr),
        localizer(nullptr),
        temporalProcessor(nullptr),
        speechMusicRecognizer(nullptr),
        featureMap(nullptr),
        brain(nullptr),
        featureDimensions(0),
        sampleRate(44100),
        bufferSize(1024),
        numChannels(2),
        isInitialized(false),
        lastStep(0),
        sensitivityThreshold(0.1f),
        noiseLevel(0.05f),
        localizationAccuracy(0.8f),
        totalFeaturesProcessed(0),
        speechSegmentsDetected(0),
        musicSegmentsDetected(0),
        averageSoundLevel(0.0f),
        localizationConfidence(0.0f) {}
    
    ~Impl() {
        // Clean up neural populations
        audioPopulations.clear();
        audioNeurons.clear();
        
        // Release brain reference
        brain = nullptr;
        
        // Clear processing buffers
        inputBuffer.clear();
        features.clear();
        frequencyFeatures.clear();
        spatialFeatures.clear();
        temporalFeatures.clear();
        spectralFeatures.clear();
        hierarchicalFeatures.clear();
        temporalBuffer.clear();
        envelopeData.clear();
        rhythmData.clear();
        
        // Clear audio buffers
        leftChannel.clear();
        rightChannel.clear();
        for (auto& binauralBuf : binauralBuffers) {
            binauralBuf.clear();
        }
        binauralBuffers.clear();
        
        // Clear spectral data
        for (auto& row : spectrogram) {
            row.clear();
        }
        spectrogram.clear();
        melFilterBank.clear();
        criticalBands.clear();
        
        // Clear receptive fields (for frequency analysis)
        frequencyReceptiveFields.clear();
        spatialReceptiveFields.clear();
    }
    
    // Thread-safe processing
    void lockProcessing() const { processingMutex.lock(); }
    void unlockProcessing() const { processingMutex.unlock(); }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {
    NLM_LOG_INFO("AudioProcessor: Initializing neural audio processor");
}

AudioProcessor::~AudioProcessor() {
    NLM_LOG_INFO("AudioProcessor: Shutting down neural audio processor");
}

void AudioProcessor::initialize(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain || !config) {
        NLM_LOG_ERROR("AudioProcessor::initialize: Invalid parameters - brain or config is null");
        return;
    }
    
    pImpl->brain = brain;
    
    // Get audio configuration
    pImpl->sampleRate = config->getOr<size_t>("audio_sample_rate", 44100);
    pImpl->bufferSize = config->getOr<size_t>("audio_buffer_size", 1024);
    pImpl->numChannels = config->getOr<size_t>("audio_channels", 2);
    pImpl->maxTemporalDepth = config->getOr<size_t>("audio_temporal_depth", 5);
    pImpl->temporalSmoothing = config->getOr<float>("audio_temporal_smoothing", 0.9f);
    pImpl->sensitivityThreshold = config->getOr<float>("audio_sensitivity_threshold", 0.1f);
    pImpl->noiseLevel = config->getOr<float>("audio_noise_level", 0.05f);
    pImpl->localizationAccuracy = config->getOr<float>("audio_localization_accuracy", 0.8f);
    pImpl->fftSize = config->getOr<size_t>("audio_fft_size", 1024);
    
    // Create neural populations for audio processing
    createAudioPopulations(config);
    
    // Initialize feature extraction components
    pImpl->featureExtractor = std::make_unique<AudioFeatureExtractor>();
    pImpl->freqAnalyzer = std::make_unique<FrequencyAnalyzer>();
    pImpl->localizer = std::make_unique<SoundLocalizer>();
    pImpl->temporalProcessor = std::make_unique<TemporalProcessor>();
    pImpl->speechMusicRecognizer = std::make_unique<SpeechMusicRecognizer>();
    pImpl->featureMap = std::make_unique<AudioFeatureMap>(config->getOr<size_t>("audio_feature_map_size", 256));
    
    // Create processing filters and receptive fields
    createAudioFilters(config);
    
    pImpl->isInitialized = true;
    NLM_LOG_INFO("AudioProcessor: Neural audio processor initialized successfully");
    NLM_LOG_INFO("AudioProcessor: Created " + std::to_string(pImpl->audioPopulations.size()) + 
                 " audio populations, " + std::to_string(pImpl->audioNeurons.size()) + " audio neurons");
}

void AudioProcessor::createAudioPopulations(std::shared_ptr<Config> config) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("AudioProcessor::createAudioPopulations: Brain not available");
        return;
    }
    
    // Get population configuration
    size_t totalAudioNeurons = config->getOr<size_t>("audio_neurons", 400);
    size_t frequencyNeurons = config->getOr<size_t>("frequency_analysis_neurons", 150);
    size_t spatialNeurons = config->getOr<size_t>("sound_localization_neurons", 100);
    size_t temporalNeurons = config->getOr<size_t>("temporal_processing_neurons", 80);
    size_t speechNeurons = config->getOr<size_t>("speech_recognition_neurons", 50);
    size_t musicNeurons = config->getOr<size_t>("music_recognition_neurons", 50);
    
    // Create neural populations for different audio processing stages
    NeuralRegion* defaultRegion = pImpl->brain->getRegion(RegionId(2));  // Different region from vision
    if (!defaultRegion) {
        defaultRegion = pImpl->brain->addRegion("Audio_Region");
    }
    
    if (defaultRegion) {
        // Frequency analysis population (processes pitch and spectral content)
        size_t freqNeuronsCount = frequencyNeurons;
        PopulationId freqPopId = defaultRegion->addPopulation(freqNeuronsCount, NeuronType::Sensory);
        auto* freqPop = defaultRegion->getPopulation(freqPopId);
        if (freqPop) {
            for (auto* neuron : freqPop->getNeurons()) {
                pImpl->audioNeurons.push_back(neuron);
            }
            pImpl->audioPopulations.push_back(std::unique_ptr<NeuralPopulation>(freqPop));
        }
        
        // Sound localization population (processes spatial audio information)
        size_t spatialNeuronsCount = spatialNeurons;
        PopulationId spatialPopId = defaultRegion->addPopulation(spatialNeuronsCount, NeuronType::Internal);
        auto* spatialPop = defaultRegion->getPopulation(spatialPopId);
        if (spatialPop) {
            for (auto* neuron : spatialPop->getNeurons()) {
                pImpl->audioNeurons.push_back(neuron);
            }
            pImpl->audioPopulations.push_back(std::unique_ptr<NeuralPopulation>(spatialPop));
        }
        
        // Temporal processing population (processes rhythm and timing)
        size_t temporalNeuronsCount = temporalNeurons;
        PopulationId temporalPopId = defaultRegion->addPopulation(temporalNeuronsCount, NeuronType::Internal);
        auto* temporalPop = defaultRegion->getPopulation(temporalPopId);
        if (temporalPop) {
            for (auto* neuron : temporalPop->getNeurons()) {
                pImpl->audioNeurons.push_back(neuron);
            }
            pImpl->audioPopulations.push_back(std::unique_ptr<NeuralPopulation>(temporalPop));
        }
        
        // Speech recognition population
        size_t speechNeuronsCount = speechNeurons;
        PopulationId speechPopId = defaultRegion->addPopulation(speechNeuronsCount, NeuronType::Internal);
        auto* speechPop = defaultRegion->getPopulation(speechPopId);
        if (speechPop) {
            for (auto* neuron : speechPop->getNeurons()) {
                pImpl->audioNeurons.push_back(neuron);
            }
            pImpl->audioPopulations.push_back(std::unique_ptr<NeuralPopulation>(speechPop));
        }
        
        // Music recognition population
        size_t musicNeuronsCount = musicNeurons;
        PopulationId musicPopId = defaultRegion->addPopulation(musicNeuronsCount, NeuronType::Internal);
        auto* musicPop = defaultRegion->getPopulation(musicPopId);
        if (musicPop) {
            for (auto* neuron : musicPop->getNeurons()) {
                pImpl->audioNeurons.push_back(neuron);
            }
            pImpl->audioPopulations.push_back(std::unique_ptr<NeuralPopulation>(musicPop));
        }
        
        // Connect neurons based on processing hierarchy
        establishAudioConnections(freqPop, spatialPop, temporalPop, speechPop, musicPop);
    }
}

void AudioProcessor::establishAudioConnections(NeuralPopulation* freqPop, NeuralPopulation* spatialPop, 
                                             NeuralPopulation* temporalPop, NeuralPopulation* speechPop, 
                                             NeuralPopulation* musicPop) {
    if (!pImpl->brain) return;
    
    // Create hierarchical connections:
    // Raw audio -> Frequency analysis
    // Raw audio -> Sound localization
    // Raw audio -> Temporal processing
    // Frequency analysis -> Speech recognition
    // Frequency analysis -> Music recognition
    // Temporal processing -> Speech/music rhythm detection
    
    if (freqPop && speechPop) {
        for (auto* freqNeuron : freqPop->getNeurons()) {
            for (auto* speechNeuron : speechPop->getNeurons()) {
                float weight = calculateConnectionWeight(freqNeuron, speechNeuron);
                if (weight > 0.1f) {
                    pImpl->brain->injectCurrent(speechNeuron->getId(), weight * 3.0f);
                }
            }
        }
    }
    
    if (freqPop && musicPop) {
        for (auto* freqNeuron : freqPop->getNeurons()) {
            for (auto* musicNeuron : musicPop->getNeurons()) {
                float weight = calculateConnectionWeight(freqNeuron, musicNeuron);
                if (weight > 0.1f) {
                    pImpl->brain->injectCurrent(musicNeuron->getId(), weight * 2.5f);
                }
            }
        }
    }
    
    if (temporalPop && speechPop) {
        for (auto* temporalNeuron : temporalPop->getNeurons()) {
            for (auto* speechNeuron : speechPop->getNeurons()) {
                float weight = calculateConnectionWeight(temporalNeuron, speechNeuron);
                if (weight > 0.1f) {
                    pImpl->brain->injectCurrent(speechNeuron->getId(), weight * 2.0f);
                }
            }
        }
    }
    
    if (temporalPop && musicPop) {
        for (auto* temporalNeuron : temporalPop->getNeurons()) {
            for (auto* musicNeuron : musicPop->getNeurons()) {
                float weight = calculateConnectionWeight(temporalNeuron, musicNeuron);
                if (weight > 0.1f) {
                    pImpl->brain->injectCurrent(musicNeuron->getId(), weight * 2.0f);
                }
            }
        }
    }
}

float AudioProcessor::calculateConnectionWeight(Neuron* source, Neuron* destination) const {
    if (!source || !destination) return 0.0f;
    
    const auto& sourceState = source->getState();
    const auto& destState = destination->getState();
    
    // Weight based on similarity in activation levels
    float sourceActivation = std::abs(sourceState.membranePotential - sourceState.restingPotential);
    float destActivation = std::abs(destState.membranePotential - destState.restingPotential);
    
    float similarity = 1.0f - std::abs(sourceActivation - destActivation) / 100.0f;
    
    // Weight also depends on neuron types and connection history
    float baseWeight = 0.2f + similarity * 0.8f;
    
    // Add noise for biological realism
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> noiseDist(-0.1f, 0.1f);
    
    return std::max(0.0f, baseWeight + noiseDist(rng));
}

void AudioProcessor::createAudioFilters(std::shared_ptr<Config> config) {
    // Create frequency receptive fields for spectral analysis
    pImpl->frequencyReceptiveFields.resize(32);  // Critical bands
    float totalBandwidth = pImpl->sampleRate / 2.0f;  // Nyquist frequency
    
    for (size_t i = 0; i < 32; ++i) {
        pImpl->frequencyReceptiveFields[i].resize(pImpl->fftSize, 0.0f);
        float bandwidth = totalBandwidth / 32.0f;
        float centerFreq = bandwidth * (i + 0.5f);
        
        // Create Gaussian-shaped filter for each frequency band
        for (size_t j = 0; j < pImpl->fftSize; ++j) {
            float freq = (j * totalBandwidth) / pImpl->fftSize;
            float sigma = bandwidth * 0.5f;  // Bandwidth of the filter
            pImpl->frequencyReceptiveFields[i][j] = std::exp(-0.5f * std::pow((freq - centerFreq) / sigma, 2.0f));
        }
    }
    
    // Create spatial receptive fields for binaural processing
    pImpl->spatialReceptiveFields.resize(4);  // Front, back, left, right
    for (size_t i = 0; i < 4; ++i) {
        pImpl->spatialReceptiveFields[i].resize(pImpl->bufferSize, 0.0f);
        // Simple spatial weighting patterns
        for (size_t j = 0; j < pImpl->bufferSize; ++j) {
            if (i == 0) {  // Front
                pImpl->spatialReceptiveFields[i][j] = 1.0f - (j / static_cast<float>(pImpl->bufferSize - 1)) * 0.5f;
            } else if (i == 1) {  // Back
                pImpl->spatialReceptiveFields[i][j] = (j / static_cast<float>(pImpl->bufferSize - 1)) * 0.5f;
            } else if (i == 2) {  // Left
                pImpl->spatialReceptiveFields[i][j] = std::sin(2.0f * 3.14159f * j / pImpl->bufferSize) * 0.3f + 0.7f;
            } else {  // Right
                pImpl->spatialReceptiveFields[i][j] = std::cos(2.0f * 3.14159f * j / pImpl->bufferSize) * 0.3f + 0.7f;
            }
        }
    }
}

void AudioProcessor::process(const Audio& input) {
    pImpl->lockProcessing();
    
    if (!pImpl->isInitialized) {
        pImpl->unlockProcessing();
        NLM_LOG_WARN("AudioProcessor::process: Not initialized, using basic feature extraction");
        // Fallback to basic processing
        pImpl->features = input.getData();
        pImpl->featureDimensions = input.getDimensions();
        return;
    }
    
    // Get input data
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        pImpl->unlockProcessing();
        NLM_LOG_ERROR("AudioProcessor::process: Empty input data");
        return;
    }
    
    // Separate binaural channels if available
    separateBinauralChannels(inputData);
    
    // Update temporal buffer
    updateTemporalBuffer(inputData);
    
    // Apply neural processing pipeline
    processInput(inputData);
    
    // Extract frequency features
    extractFrequencyFeatures(inputData);
    
    // Extract spatial features
    extractSpatialFeatures(inputData);
    
    // Extract temporal features
    extractTemporalFeatures(inputData);
    
    // Extract spectral features using FFT-like neural mechanisms
    extractSpectralFeatures(inputData);
    
    // Detect speech and music
    detectSpeechAndMusic(inputData);
    
    // Generate hierarchical features
    generateHierarchicalFeatures();
    
    // Apply noise reduction and enhancement
    enhanceAndReduceNoise();
    
    // Update feature maps
    updateFeatureMaps();
    
    // Integrate with brain systems
    integrateWithBrain(inputData);
    
    // Store statistics
    pImpl->totalFeaturesProcessed++;
    
    pImpl->unlockProcessing();
}

void AudioProcessor::separateBinauralChannels(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Separate input into left and right channels based on channel count
    pImpl->leftChannel.clear();
    pImpl->rightChannel.clear();
    
    if (pImpl->numChannels >= 2 && input.size() >= pImpl->bufferSize * 2) {
        // Stereo input
        for (size_t i = 0; i < pImpl->bufferSize; ++i) {
            pImpl->leftChannel.push_back(input[i * 2]);
            pImpl->rightChannel.push_back(input[i * 2 + 1]);
        }
    } else if (pImpl->numChannels == 1 && input.size() >= pImpl->bufferSize) {
        // Mono input - duplicate to stereo
        for (size_t i = 0; i < pImpl->bufferSize; ++i) {
            float monoValue = input[i];
            pImpl->leftChannel.push_back(monoValue);
            pImpl->rightChannel.push_back(monoValue);
        }
    } else {
        // Not enough data or unknown channel format
        NLM_LOG_WARN("AudioProcessor::separateBinauralChannels: Unexpected input format");
        // Use all input as left channel, empty right channel
        pImpl->leftChannel.assign(input.begin(), input.end());
        pImpl->rightChannel.clear();
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::updateTemporalBuffer(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    pImpl->inputBuffer = input;
    
    // Apply temporal smoothing
    if (!pImpl->temporalBuffer.empty()) {
        float alpha = pImpl->temporalSmoothing;
        size_t minSize = std::min(input.size(), pImpl->temporalBuffer.size());
        for (size_t i = 0; i < minSize; ++i) {
            pImpl->temporalBuffer[i] = alpha * pImpl->temporalBuffer[i] + (1.0f - alpha) * input[i];
        }
    } else {
        pImpl->temporalBuffer = input;
    }
    
    // Maintain temporal depth
    if (pImpl->temporalBuffer.size() > pImpl->maxTemporalDepth * pImpl->bufferSize) {
        pImpl->temporalBuffer.erase(pImpl->temporalBuffer.begin(), 
                                   pImpl->temporalBuffer.begin() + pImpl->bufferSize);
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::processInput(const std::vector<float>& input) {
    // Apply neural receptive field processing
    std::vector<float> processedInput = input;
    
    // Apply frequency and spatial filters
    for (size_t i = 0; i < pImpl->frequencyReceptiveFields.size(); ++i) {
        for (size_t j = 0; j < std::min(input.size(), static_cast<size_t>(pImpl->fftSize)); ++j) {
            processedInput[j] *= pImpl->frequencyReceptiveFields[i][j];
        }
    }
    
    // Pass processed input to feature extractor
    if (pImpl->featureExtractor) {
        pImpl->featureExtractor->process(processedInput);
    }
}

void AudioProcessor::extractFrequencyFeatures(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Use frequency analyzer neural population
    if (!pImpl->freqAnalyzer) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Create frequency feature map using neural spectral analysis
    pImpl->frequencyFeatures.clear();
    pImpl->frequencyFeatures.reserve(pImpl->sampleRate / 2);  // Up to Nyquist
    
    // Neural frequency analysis using population dynamics
    if (pImpl->inputBuffer.size() >= pImpl->bufferSize) {
        // Simplified FFT-like neural mechanism
        size_t halfSize = pImpl->fftSize / 2;
        for (size_t i = 0; i < halfSize && i < pImpl->bufferSize; ++i) {
            float magnitude = 0.0f;
            for (size_t j = 0; j < pImpl->fftSize && (i + j) < pImpl->bufferSize; ++j) {
                magnitude += std::abs(input[i + j]) * std::cos(2.0f * 3.14159f * i * j / pImpl->fftSize);
            }
            magnitude /= (pImpl->fftSize / 2);
            
            // Apply frequency receptive field weighting
            float weightedMag = magnitude;
            if (i < pImpl->frequencyReceptiveFields.size()) {
                float filterSum = std::accumulate(pImpl->frequencyReceptiveFields[i].begin(), 
                                                 pImpl->frequencyReceptiveFields[i].end(), 0.0f);
                if (filterSum > 0.0f) {
                    weightedMag *= filterSum / pImpl->frequencyReceptiveFields[i].size();
                }
            }
            
            pImpl->frequencyFeatures.push_back(weightedMag);
        }
    }
    
    // Pass frequency features to analyzer
    pImpl->freqAnalyzer->process(pImpl->frequencyFeatures);
    
    pImpl->unlockProcessing();
}

void AudioProcessor::extractSpatialFeatures(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Use sound localizer neural population
    if (!pImpl->localizer) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Extract spatial features from binaural processing
    pImpl->spatialFeatures.clear();
    
    if (pImpl->leftChannel.empty() || pImpl->rightChannel.empty()) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Interaural time difference (ITD)
    float itd = 0.0f;
    if (!pImpl->leftChannel.empty() && !pImpl->rightChannel.empty()) {
        size_t crossCorrMax = std::min(pImpl->leftChannel.size(), pImpl->rightChannel.size());
        float maxCorr = 0.0f;
        for (size_t i = 0; i < crossCorrMax; ++i) {
            float corr = pImpl->leftChannel[i] * pImpl->rightChannel[i];
            if (std::abs(corr) > std::abs(maxCorr)) {
                maxCorr = corr;
            }
        }
        itd = maxCorr * 1000.0f;  // Scale for neural processing
    }
    
    // Interaural level difference (ILD)
    float leftEnergy = std::accumulate(pImpl->leftChannel.begin(), pImpl->leftChannel.end(), 0.0f,
                                      [](float sum, float val) { return sum + val * val; }) / pImpl->leftChannel.size();
    float rightEnergy = std::accumulate(pImpl->rightChannel.begin(), pImpl->rightChannel.end(), 0.0f,
                                       [](float sum, float val) { return sum + val * val; }) / pImpl->rightChannel.size();
    float ild = std::log10(std::max(0.01f, leftEnergy) / std::max(0.01f, rightEnergy)) * 20.0f;
    
    // Apply spatial receptive fields
    std::vector<float> spatialWeighted;
    for (size_t i = 0; i < 4 && i < pImpl->spatialReceptiveFields.size(); ++i) {
        float feature = 0.0f;
        for (size_t j = 0; j < std::min(pImpl->bufferSize, input.size()); ++j) {
            feature += input[j] * pImpl->spatialReceptiveFields[i][j];
        }
        spatialWeighted.push_back(feature);
    }
    
    // Combine spatial features
    pImpl->spatialFeatures.push_back(itd);
    pImpl->spatialFeatures.push_back(ild);
    pImpl->spatialFeatures.insert(pImpl->spatialFeatures.end(), 
                                 spatialWeighted.begin(), spatialWeighted.end());
    
    // Pass spatial features to localizer
    pImpl->localizer->process(pImpl->spatialFeatures);
    
    pImpl->unlockProcessing();
}

void AudioProcessor::extractTemporalFeatures(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Use temporal processor neural population
    if (!pImpl->temporalProcessor) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Extract temporal features
    pImpl->temporalFeatures.clear();
    pImpl->envelopeData.clear();
    pImpl->rhythmData.clear();
    
    if (input.empty()) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Temporal envelope detection using neural mechanisms
    std::vector<float> envelope(input.size(), 0.0f);
    float attack = 0.01f;  // 10ms attack time
    float decay = 0.1f;   // 100ms decay time
    
    for (size_t i = 0; i < input.size(); ++i) {
        float absValue = std::abs(input[i]);
        
        if (envelope.empty()) {
            envelope[i] = absValue;
        } else {
            float target = absValue;
            float current = envelope[i - 1];
            
            if (target > current) {
                // Attack phase
                envelope[i] = current + (1.0f - std::exp(-attack * i)) * (target - current);
            } else {
                // Decay phase
                envelope[i] = current * std::exp(-decay * i);
            }
        }
    }
    
    pImpl->envelopeData = envelope;
    
    // Rhythm detection using neural oscillations
    std::vector<float> rhythmFeatures;
    size_t windowSize = std::min(static_cast<size_t>(1000), input.size() / 10);  // 10ms windows for 44.1kHz
    
    for (size_t i = 0; i < input.size() - windowSize; i += windowSize / 10) {  // 10% overlap
        std::vector<float> window(input.begin() + i, input.begin() + i + windowSize);
        
        // Calculate rhythm strength using zero-crossings
        int zeroCrossings = 0;
        for (size_t j = 1; j < window.size(); ++j) {
            if ((window[j - 1] < 0 && window[j] >= 0) || (window[j - 1] >= 0 && window[j] < 0)) {
                zeroCrossings++;
            }
        }
        
        float rhythmStrength = zeroCrossings / static_cast<float>(window.size()) * 1000.0f;
        rhythmFeatures.push_back(rhythmStrength);
    }
    
    // Smooth rhythm features
    if (!rhythmFeatures.empty()) {
        float rhythmSum = std::accumulate(rhythmFeatures.begin(), rhythmFeatures.end(), 0.0f);
        float avgRhythm = rhythmSum / rhythmFeatures.size();
        pImpl->rhythmData.assign(rhythmFeatures.size(), avgRhythm);
    }
    
    // Pass temporal features to processor
    pImpl->temporalProcessor->process(pImpl->temporalFeatures);
    
    pImpl->unlockProcessing();
}

void AudioProcessor::extractSpectralFeatures(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Neural spectral analysis using FFT-like mechanisms
    pImpl->spectralFeatures.clear();
    
    if (input.empty()) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Simplified neural FFT using population dynamics
    size_t fftSize = std::min(pImpl->fftSize, input.size());
    std::vector<float> spectrum(fftSize, 0.0f);
    
    // Neural FFT-like computation using butterfly operations
    for (size_t i = 0; i < fftSize; ++i) {
        float real = 0.0f;
        float imag = 0.0f;
        
        for (size_t j = 0; j < fftSize; ++j) {
            float angle = 2.0f * 3.14159f * i * j / fftSize;
            float magnitude = input[j];
            
            real += magnitude * std::cos(angle);
            imag -= magnitude * std::sin(angle);
        }
        
        spectrum[i] = std::sqrt(real * real + imag * imag);
    }
    
    // Apply mel filter bank
    if (!pImpl->melFilterBank.empty()) {
        pImpl->spectralFeatures.resize(pImpl->melFilterBank.size(), 0.0f);
        
        for (size_t i = 0; i < pImpl->melFilterBank.size(); ++i) {
            float melSum = 0.0f;
            size_t bandStart = static_cast<size_t>(pImpl->melFilterBank[i]);
            size_t bandEnd = (i < pImpl->melFilterBank.size() - 1) ? 
                static_cast<size_t>(pImpl->melFilterBank[i + 1]) : fftSize;
            
            for (size_t j = bandStart; j < bandEnd && j < spectrum.size(); ++j) {
                melSum += spectrum[j];
            }
            
            pImpl->spectralFeatures[i] = melSum / (bandEnd - bandStart);
        }
    } else {
        pImpl->spectralFeatures = spectrum;
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::detectSpeechAndMusic(const std::vector<float>& input) {
    pImpl->lockProcessing();
    
    // Use speech/music recognizer neural population
    if (!pImpl->speechMusicRecognizer) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Simple speech/music detection based on spectral characteristics
    std::vector<float> features;
    
    // Calculate spectral centroid (pitch-related, strong for speech)
    float spectralCentroid = 0.0f;
    float spectralMoment = 0.0f;
    
    if (!pImpl->frequencyFeatures.empty()) {
        float totalMag = std::accumulate(pImpl->frequencyFeatures.begin(), pImpl->frequencyFeatures.end(), 0.0f);
        if (totalMag > 0.0f) {
            float weightedSum = 0.0f;
            for (size_t i = 0; i < pImpl->frequencyFeatures.size(); ++i) {
                weightedSum += i * pImpl->frequencyFeatures[i];
            }
            spectralCentroid = weightedSum / totalMag;
        }
    }
    
    // Calculate spectral roll-off (frequency content distribution)
    float spectralRolloff = 0.0f;
    if (!pImpl->spectralFeatures.empty()) {
        float totalEnergy = std::accumulate(pImpl->spectralFeatures.begin(), pImpl->spectralFeatures.end(), 0.0f);
        float cumulativeEnergy = 0.0f;
        
        for (size_t i = 0; i < pImpl->spectralFeatures.size(); ++i) {
            cumulativeEnergy += pImpl->spectralFeatures[i];
            if (cumulativeEnergy / totalEnergy >= 0.85f && spectralRolloff == 0.0f) {
                spectralRolloff = i;
            }
        }
    }
    
    // Calculate zero-crossing rate (temporal activity)
    int zeroCrossings = 0;
    for (size_t i = 1; i < input.size(); ++i) {
        if ((input[i - 1] < 0 && input[i] >= 0) || (input[i - 1] >= 0 && input[i] < 0)) {
            zeroCrossings++;
        }
    }
    float zeroCrossingRate = zeroCrossings / static_cast<float>(input.size()) * 1000.0f;
    
    // Add features for recognizer
    features.push_back(spectralCentroid);
    features.push_back(spectralRolloff);
    features.push_back(zeroCrossingRate);
    
    // Estimate speech vs music
    float speechLikelihood = 0.0f;
    
    // High spectral centroid suggests speech (formants)
    if (spectralCentroid > 500.0f && spectralCentroid < 4000.0f) {
        speechLikelihood += 0.4f;
    }
    
    // Moderate zero-crossing rate suggests speech (syllable rate)
    if (zeroCrossingRate > 100.0f && zeroCrossingRate < 500.0f) {
        speechLikelihood += 0.3f;
    }
    
    // Spectral roll-off pattern
    if (spectralRolloff > 0.0f && spectralRolloff < 2000.0f) {
        speechLikelihood += 0.3f;
    }
    
    // Pass features to recognizer
    pImpl->speechMusicRecognizer->process(features);
    
    // Update statistics
    if (speechLikelihood > 0.5f) {
        pImpl->speechSegmentsDetected++;
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::generateHierarchicalFeatures() {
    pImpl->lockProcessing();
    
    // Generate hierarchical features from different processing stages
    pImpl->hierarchicalFeatures.clear();
    
    // Combine features from different stages
    size_t maxSize = std::max({
        pImpl->features.size(),
        pImpl->frequencyFeatures.size(),
        pImpl->spatialFeatures.size(),
        pImpl->temporalFeatures.size(),
        pImpl->spectralFeatures.size()
    });
    
    pImpl->hierarchicalFeatures.resize(maxSize, 0.0f);
    
    // Simple hierarchical combination
    float weightSum = 0.0f;
    size_t validFeatures = 0;
    
    for (size_t i = 0; i < maxSize; ++i) {
        float value = 0.0f;
        
        if (i < pImpl->features.size()) {
            value += pImpl->features[i];
            weightSum += 1.0f;
            validFeatures++;
        }
        
        if (i < pImpl->frequencyFeatures.size()) {
            value += pImpl->frequencyFeatures[i] * 0.3f;
            weightSum += 0.3f;
            validFeatures++;
        }
        
        if (i < pImpl->spatialFeatures.size()) {
            value += pImpl->spatialFeatures[i] * 0.4f;
            weightSum += 0.4f;
            validFeatures++;
        }
        
        if (i < pImpl->temporalFeatures.size()) {
            value += pImpl->temporalFeatures[i] * 0.2f;
            weightSum += 0.2f;
            validFeatures++;
        }
        
        if (i < pImpl->spectralFeatures.size()) {
            value += pImpl->spectralFeatures[i] * 0.3f;
            weightSum += 0.3f;
            validFeatures++;
        }
        
        if (weightSum > 0.0f) {
            pImpl->hierarchicalFeatures[i] = value / weightSum;
        }
    }
    
    // Update final features
    pImpl->features = pImpl->hierarchicalFeatures;
    pImpl->featureDimensions = pImpl->features.size();
    
    pImpl->unlockProcessing();
}

void AudioProcessor::enhanceAndReduceNoise() {
    pImpl->lockProcessing();
    
    if (pImpl->features.empty()) {
        pImpl->unlockProcessing();
        return;
    }
    
    // Noise reduction using spectral subtraction
    float noiseFloor = pImpl->noiseLevel;
    float enhancementFactor = 1.0f / (1.0f + noiseFloor);
    
    // Spectral subtraction for noise reduction
    for (float& feature : pImpl->features) {
        feature = std::max(0.0f, feature * enhancementFactor);
        
        // Apply dynamic range compression
        if (feature > 1.0f) {
            feature = 1.0f + std::log(feature - 1.0f) * 0.5f;
        }
    }
    
    // Calculate average sound level
    pImpl->averageSoundLevel = std::accumulate(pImpl->features.begin(), pImpl->features.end(), 0.0f) / pImpl->features.size();
    
    // Temporal smoothing
    if (!pImpl->temporalBuffer.empty() && pImpl->temporalBuffer.size() == pImpl->features.size()) {
        for (size_t i = 0; i < pImpl->features.size(); ++i) {
            pImpl->features[i] = pImpl->temporalSmoothing * pImpl->features[i] +
                                (1.0f - pImpl->temporalSmoothing) * pImpl->temporalBuffer[i];
        }
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::updateFeatureMaps() {
    pImpl->lockProcessing();
    
    if (pImpl->featureMap) {
        pImpl->featureMap->update(pImpl->features);
    }
    
    pImpl->unlockProcessing();
}

void AudioProcessor::integrateWithBrain(const std::vector<float>& input) {
    if (!pImpl->brain) return;
    
    // Inject audio processing into brain's sensory system
    pImpl->brain->receiveSensoryInput(reinterpret_cast<const SensoryInput&>(input));
    
    // Update working memory with audio features
    if (pImpl->brain->getWorkingMemory() && !pImpl->features.empty()) {
        // Store audio features in working memory
        for (size_t i = 0; i < std::min(pImpl->features.size(), static_cast<size_t>(100)); ++i) {
            float activation = pImpl->features[i] * 0.1f;  // Scale for neural injection
            // The features are already encoded in the audio neuron activity
        }
    }
    
    // Update attentional selection
    if (pImpl->brain->getAttention() && !pImpl->spatialFeatures.empty()) {
        // Create attentional signal based on spatial features
        float attentionStrength = *std::max_element(pImpl->spatialFeatures.begin(), pImpl->spatialFeatures.end());
        
        // Apply attentional modulation
        if (pImpl->brain->getDopamine()) {
            float modulation = attentionStrength * 0.5f;
            pImpl->brain->applyNeuromodulation(*pImpl->brain->getDopamine());
        }
    }
    
    // Update prediction system with audio predictions
    if (pImpl->brain->getPredictionSystem() && !pImpl->spectralFeatures.empty()) {
        // Feed spectral features into prediction system for error computation
        for (size_t i = 0; i < std::min(pImpl->spectralFeatures.size(), static_cast<size_t>(50)); ++i) {
            pImpl->brain->injectCurrent(NeuronId(1000 + i), pImpl->spectralFeatures[i] * 0.01f);
        }
    }
    
    // Update concept formation with audio patterns
    if (pImpl->brain->getConceptFormation() && !pImpl->hierarchicalFeatures.empty()) {
        // Store audio patterns for concept learning
        for (size_t i = 0; i < std::min(pImpl->hierarchicalFeatures.size(), static_cast<size_t>(75)); ++i) {
            pImpl->brain->injectCurrent(NeuronId(2000 + i), pImpl->hierarchicalFeatures[i] * 0.01f);
        }
    }
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

void AudioProcessor::reset() {
    pImpl->lockProcessing();
    
    // Reset all processing state
    pImpl->inputBuffer.clear();
    pImpl->features.clear();
    pImpl->frequencyFeatures.clear();
    pImpl->spatialFeatures.clear();
    pImpl->temporalFeatures.clear();
    pImpl->spectralFeatures.clear();
    pImpl->hierarchicalFeatures.clear();
    pImpl->temporalBuffer.clear();
    pImpl->envelopeData.clear();
    pImpl->rhythmData.clear();
    
    // Reset audio buffers
    pImpl->leftChannel.clear();
    pImpl->rightChannel.clear();
    for (auto& binauralBuf : pImpl->binauralBuffers) {
        binauralBuf.clear();
    }
    
    // Reset temporal processing
    pImpl->lastStep = 0;
    
    // Reset statistics
    pImpl->totalFeaturesProcessed = 0;
    pImpl->speechSegmentsDetected = 0;
    pImpl->musicSegmentsDetected = 0;
    pImpl->averageSoundLevel = 0.0f;
    pImpl->localizationConfidence = 0.0f;
    
    pImpl->unlockProcessing();
    
    NLM_LOG_INFO("AudioProcessor: Reset processing state");
}

// AudioFeatureExtractor implementation
class AudioProcessor::AudioFeatureExtractor {
public:
    void process(const std::vector<float>& input) {
        features.clear();
        
        // Extract basic audio features using neural mechanisms
        for (size_t i = 0; i < input.size(); ++i) {
            float value = input[i];
            
            // Multi-scale feature extraction
            features.push_back(value);  // Raw amplitude
            features.push_back(std::tanh(value * 2.0f));  // Non-linear distortion
            features.push_back(std::sin(value * 3.14159f));  // Harmonic content
            features.push_back(std::cos(value * 2.0f));  // Different frequency
            
            // Local statistics for context
            if (i > 0 && i < input.size() - 1) {
                float localMean = (input[i - 1] + input[i] + input[i + 1]) / 3.0f;
                features.push_back(value - localMean);  // Local contrast
            }
        }
    }
    
    const std::vector<float>& getFeatures() const { return features; }
    
private:
    std::vector<float> features;
};

// FrequencyAnalyzer implementation
class AudioProcessor::FrequencyAnalyzer {
public:
    void process(const std::vector<float>& input) {
        frequencyMap.clear();
        
        if (input.empty()) return;
        
        // Neural frequency analysis using population dynamics
        frequencyMap.reserve(input.size());
        for (float val : input) {
            // Apply neural frequency response
            float response = std::tanh(val * 2.0f) * std::abs(val);
            frequencyMap.push_back(response);
        }
    }
    
    const std::vector<float>& getFrequencyMap() const { return frequencyMap; }
    
private:
    std::vector<float> frequencyMap;
};

// SoundLocalizer implementation
class AudioProcessor::SoundLocalizer {
public:
    void process(const std::vector<float>& input) {
        spatialMap.clear();
        
        if (input.empty()) return;
        
        // Neural spatial processing for sound localization
        spatialMap.reserve(input.size());
        for (size_t i = 0; i < input.size(); ++i) {
            // Apply spatial receptive field processing
            float spatialWeight = 1.0f;
            if (i > 0 && i < input.size() - 1) {
                // Calculate interaural difference
                float left = input[i - 1];
                float right = input[i + 1];
                spatialWeight = std::abs(left - right) * 0.5f + 0.5f;
            }
            
            float processed = input[i] * spatialWeight;
            spatialMap.push_back(processed);
        }
    }
    
    const std::vector<float>& getSpatialMap() const { return spatialMap; }
    
private:
    std::vector<float> spatialMap;
};

// TemporalProcessor implementation
class AudioProcessor::TemporalProcessor {
public:
    void process(const std::vector<float>& input) {
        temporalMap.clear();
        
        if (input.empty()) return;
        
        // Neural temporal processing for rhythm and timing
        temporalMap.reserve(input.size());
        
        // Temporal envelope detection
        std::vector<float> envelope(input.size(), 0.0f);
        for (size_t i = 0; i < input.size(); ++i) {
            float absValue = std::abs(input[i]);
            if (i == 0) {
                envelope[i] = absValue;
            } else {
                envelope[i] = envelope[i - 1] * 0.9f + absValue * 0.1f;  // Low-pass filter
            }
        }
        
        // Rhythm detection using neural oscillators
        for (size_t i = 0; i < input.size(); ++i) {
            float temporalFeature = input[i];
            
            // Add envelope information
            temporalFeature += envelope[i] * 0.3f;
            
            // Add temporal context
            if (i > 10 && i < input.size() - 10) {
                float localAvg = std::accumulate(input.begin() + i - 5, input.begin() + i + 6, 0.0f) / 11.0f;
                temporalFeature += (input[i] - localAvg) * 0.2f;
            }
            
            temporalMap.push_back(temporalFeature);
        }
    }
    
    const std::vector<float>& getTemporalMap() const { return temporalMap; }
    
private:
    std::vector<float> temporalMap;
};

// SpeechMusicRecognizer implementation
class AudioProcessor::SpeechMusicRecognizer {
public:
    void process(const std::vector<float>& input) {
        recognitionMap.clear();
        
        if (input.empty()) return;
        
        // Neural speech/music recognition
        recognitionMap.reserve(input.size());
        
        for (size_t i = 0; i < input.size(); ++i) {
            float value = input[i];
            
            // Multi-pattern detection using neural mechanisms
            float speechPattern = 0.0f;
            float musicPattern = 0.0f;
            
            // Speech-like patterns (formant frequencies, rapid changes)
            if (i > 0 && i < input.size() - 1) {
                float diff = std::abs(input[i] - input[i - 1]);
                speechPattern += diff * 0.4f;
            }
            
            // Music-like patterns (harmonic structures, sustained tones)
            float smooth = 0.0f;
            if (i > 0 && i < input.size() - 1) {
                smooth = 1.0f - std::abs(input[i] - input[i + 1]);
            }
            musicPattern += smooth * 0.6f;
            
            // Combine patterns
            float combined = speechPattern * 0.7f + musicPattern * 0.3f;
            recognitionMap.push_back(combined);
        }
    }
    
    const std::vector<float>& getRecognitionMap() const { return recognitionMap; }
    
private:
    std::vector<float> recognitionMap;
};

// AudioFeatureMap implementation
class AudioProcessor::AudioFeatureMap {
public:
    explicit AudioFeatureMap(size_t size) : featureSize(size) {
        features.resize(size, 0.0f);
    }
    
    void update(const std::vector<float>& newFeatures) {
        if (newFeatures.size() == featureSize) {
            for (size_t i = 0; i < featureSize; ++i) {
                features[i] = (features[i] * 0.9f) + (newFeatures[i] * 0.1f);
            }
        } else {
            features = newFeatures;
            if (features.size() > featureSize) {
                features.resize(featureSize);
            } else if (features.size() < featureSize) {
                features.resize(featureSize, 0.0f);
            }
        }
    }
    
    const std::vector<float>& getFeatures() const { return features; }
    size_t getSize() const { return featureSize; }
    
private:
    std::vector<float> features;
    size_t featureSize;
};

} // namespace nlm
