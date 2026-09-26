#include "NVMVisualization.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <chrono>

class NLMViewer : public QMainWindow {
    Q_OBJECT

public:
    NLMViewer(QWidget* parent = nullptr);
    ~NLMViewer() override;

private slots:
    void onVisualizationInitialized(bool success);
    void onVisualizationUpdate();
    void onVisualizationRender();
    void onInputEvent(const QString& eventType);
    void onExportClicked();
    void onImportClicked();
    void onThemeChanged(const QString& theme);
    void onQualityChanged(const QString& quality);
    void onRealTimeTogglesChanged(bool enabled);
    void onUpdateFrequencyChanged(int frequency);
    void onVisualizationModuleAdded(const QString& name);
    void onVisualizationModuleRemoved(const QString& name);
    void onDataSourceAdded(const QString& name);
    void onDataSourceRemoved(const QString& name);
    void onExportResults();
    void onImportResults();
    void onPerformanceAnalysisRequested();
    void onLearningAnalysisRequested();
    void onNetworkAnalysisRequested();
    void onSystemAnalysisRequested();

private:
    void setupUI();
    void setupVisualization();
    void setupControls();
    void setupMenus();
    void updateVisualizationData();
    void updateVisualizationControls();
    void loadVisualizationData();
    void saveVisualizationData();
    void exportVisualizationData();
    void importVisualizationData();
    void showPerformanceAnalysis();
    void showLearningAnalysis();
    void showNetworkAnalysis();
    void showSystemAnalysis();
    void showAbout();
    void closeEvent(QCloseEvent* event) override;

    QTabWidget* tabWidget;
    QWidget* mainWidget;
    QSplitter* mainSplitter;
    
    // Visualization components
    nlm::NVMVisualization* visualization;
    
    // Controls
    QComboBox* visualizationModeCombo;
    QComboBox* themeCombo;
    QComboBox* qualityCombo;
    QCheckBox* realTimeUpdatesCheck;
    QSpinBox* updateFrequencySpin;
    QProgressBar* performanceProgress;
    QLabel* fpsLabel;
    QLabel* memoryLabel;
    QLabel* cpuLabel;
    
    // Data source management
    QListWidget* dataSourceList;
    QLineEdit* dataSourceNameEdit;
    
    // Visualization module management
    QListWidget* visualizationModuleList;
    
    // Export/Import
    QPushButton* exportButton;
    QPushButton* importButton;
    
    // Analysis tabs
    QWidget* performanceAnalysisTab;
    QWidget* learningAnalysisTab;
    QWidget* networkAnalysisTab;
    QWidget* systemAnalysisTab;
    
    // Analysis widgets
    QTextEdit* performanceAnalysisText;
    QTextEdit* learningAnalysisText;
    QTextEdit* networkAnalysisText;
    QTextEdit* systemAnalysisText;
    
    QTimer* updateTimer;
    QTimer* renderTimer;
    std::chrono::steady_clock::time_point lastFrameTime;
    int frameCount;
    double currentFPS;
};

NLMViewer::NLMViewer(QWidget* parent)
    : QMainWindow(parent)
    , tabWidget(nullptr)
    , mainWidget(nullptr)
    , mainSplitter(nullptr)
    , visualization(nullptr)
    , updateTimer(nullptr)
    , renderTimer(nullptr)
    , frameCount(0)
    , currentFPS(0.0)
{
    setWindowTitle("NLM Visualization Viewer");
    setGeometry(100, 100, 1600, 900);
    
    setupUI();
    setupVisualization();
    setupControls();
    setupMenus();
    
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &NLMViewer::onVisualizationUpdate);
    updateTimer->start(16); // 60 Hz
    
    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, &NLMViewer::onVisualizationRender);
    renderTimer->start(16); // 60 Hz
}

NLMViewer::~NLMViewer() {
    if (visualization) {
        visualization->close();
    }
}

void NLMViewer::setupUI() {
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    
    mainSplitter = new QSplitter(Qt::Horizontal, mainWidget);
    
    tabWidget = new QTabWidget(mainSplitter);
    
    // Create analysis tabs
    performanceAnalysisTab = new QWidget();
    learningAnalysisTab = new QWidget();
    networkAnalysisTab = new QWidget();
    systemAnalysisTab = new QWidget();
    
    // Setup analysis tab contents
    QVBoxLayout* perfLayout = new QVBoxLayout(performanceAnalysisTab);
    performanceAnalysisText = new QTextEdit();
    performanceAnalysisText->setReadOnly(true);
    perfLayout->addWidget(performanceAnalysisText);
    
    QVBoxLayout* learnLayout = new QVBoxLayout(learningAnalysisTab);
    learningAnalysisText = new QTextEdit();
    learningAnalysisText->setReadOnly(true);
    learnLayout->addWidget(learningAnalysisText);
    
    QVBoxLayout* networkLayout = new QVBoxLayout(networkAnalysisTab);
    networkAnalysisText = new QTextEdit();
    networkAnalysisText->setReadOnly(true);
    networkLayout->addWidget(networkAnalysisText);
    
    QVBoxLayout* systemLayout = new QVBoxLayout(systemAnalysisTab);
    systemAnalysisText = new QTextEdit();
    systemAnalysisText->setReadOnly(true);
    systemLayout->addWidget(systemAnalysisText);
    
    tabWidget->addTab(performanceAnalysisTab, "Performance Analysis");
    tabWidget->addTab(learningAnalysisTab, "Learning Analysis");
    tabWidget->addTab(networkAnalysisTab, "Network Analysis");
    tabWidget->addTab(systemAnalysisTab, "System Integration Analysis");
    
    mainSplitter->addWidget(tabWidget);
    
    QVBoxLayout* rightPanel = new QVBoxLayout();
    mainSplitter->addWidget(new QWidget()); // Empty spacer
    
    mainSplitter->setSizes({1200, 200});
    
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(mainSplitter);
}

void NLMViewer::setupVisualization() {
    visualization = new nlm::NVMVisualization(this);
    
    // Try to load a sample brain for visualization
    // In a real implementation, you would load the actual brain data from file
    // For now, we'll just initialize the visualization
    visualization->initialize();
    
    connect(visualization, &nlm::NVMVisualization::visualizationInitialized,
            this, &NLMViewer::onVisualizationInitialized);
}

void NLMViewer::setupControls() {
    QWidget* controlWidget = new QWidget(mainSplitter);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlWidget);
    
    // Visualization controls
    QGroupBox* visualizationGroup = new QGroupBox("Visualization Controls");
    QVBoxLayout* vizGroupLayout = new QVBoxLayout(visualizationGroup);
    
    QHBoxLayout* modeLayout = new QHBoxLayout();
    modeLayout->addWidget(new QLabel("Mode:"));
    visualizationModeCombo = new QComboBox();
    visualizationModeCombo->addItems({"3D", "2D", "Topological", "Activity"});
    modeLayout->addWidget(visualizationModeCombo);
    vizGroupLayout->addLayout(modeLayout);
    
    QHBoxLayout* themeLayout = new QHBoxLayout();
    themeLayout->addWidget(new QLabel("Theme:"));
    themeCombo = new QComboBox();
    themeCombo->addItems({"Default", "Dark", "Light", "High Contrast"});
    themeLayout->addWidget(themeCombo);
    vizGroupLayout->addLayout(themeLayout);
    
    QHBoxLayout* qualityLayout = new QHBoxLayout();
    qualityLayout->addWidget(new QLabel("Quality:"));
    qualityCombo = new QComboBox();
    qualityCombo->addItems({"Low", "Medium", "High", "Ultra"});
    qualityLayout->addWidget(qualityCombo);
    vizGroupLayout->addLayout(qualityLayout);
    
    realTimeUpdatesCheck = new QCheckBox("Real-time Updates");
    realTimeUpdatesCheck->setChecked(true);
    vizGroupLayout->addWidget(realTimeUpdatesCheck);
    
    QHBoxLayout* freqLayout = new QHBoxLayout();
    freqLayout->addWidget(new QLabel("Update Frequency (Hz):"));
    updateFrequencySpin = new QSpinBox();
    updateFrequencySpin->setRange(1, 120);
    updateFrequencySpin->setValue(60);
    freqLayout->addWidget(updateFrequencySpin);
    vizGroupLayout->addLayout(freqLayout);
    
    controlLayout->addWidget(visualizationGroup);
    
    // Performance monitoring
    QGroupBox* performanceGroup = new QGroupBox("Performance Monitoring");
    QVBoxLayout* perfGroupLayout = new QVBoxLayout(performanceGroup);
    
    QHBoxLayout* fpsLayout = new QHBoxLayout();
    fpsLayout->addWidget(new QLabel("FPS:"));
    fpsLabel = new QLabel("0");
    fpsLayout->addWidget(fpsLabel);
    perfGroupLayout->addLayout(fpsLayout);
    
    QHBoxLayout* memoryLayout = new QHBoxLayout();
    memoryLayout->addWidget(new QLabel("Memory:"));
    memoryLabel = new QLabel("0 MB");
    memoryLayout->addWidget(memoryLabel);
    perfGroupLayout->addLayout(memoryLayout);
    
    QHBoxLayout* cpuLayout = new QHBoxLayout();
    cpuLayout->addWidget(new QLabel("CPU:"));
    cpuLabel = new QLabel("0%");
    cpuLayout->addWidget(cpuLabel);
    perfGroupLayout->addLayout(cpuLayout);
    
    performanceProgress = new QProgressBar();
    performanceProgress->setRange(0, 100);
    performanceProgress->setValue(0);
    perfGroupLayout->addWidget(performanceProgress);
    
    controlLayout->addWidget(performanceGroup);
    
    // Data source management
    QGroupBox* dataSourceGroup = new QGroupBox("Data Sources");
    QVBoxLayout* dataSourceGroupLayout = new QVBoxLayout(dataSourceGroup);
    
    QHBoxLayout* dataSourceListLayout = new QHBoxLayout();
    dataSourceList = new QListWidget();
    dataSourceListLayout->addWidget(dataSourceList);
    dataSourceNameEdit = new QLineEdit();
    dataSourceNameEdit->setPlaceholderText("Enter data source name");
    dataSourceListLayout->addWidget(dataSourceNameEdit);
    dataSourceGroupLayout->addLayout(dataSourceListLayout);
    
    QPushButton* addDataSourceButton = new QPushButton("Add Data Source");
    connect(addDataSourceButton, &QPushButton::clicked, this, &NLMViewer::onDataSourceAdded);
    dataSourceGroupLayout->addWidget(addDataSourceButton);
    
    controlLayout->addWidget(dataSourceGroup);
    
    // Export/Import
    QGroupBox* exportImportGroup = new QGroupBox("Export/Import");
    QVBoxLayout* exportImportGroupLayout = new QVBoxLayout(exportImportGroup);
    
    exportButton = new QPushButton("Export Visualization Data");
    connect(exportButton, &QPushButton::clicked, this, &NLMViewer::onExportClicked);
    exportImportGroupLayout->addWidget(exportButton);
    
    importButton = new QPushButton("Import Visualization Data");
    connect(importButton, &QPushButton::clicked, this, &NLMViewer::onImportClicked);
    exportImportGroupLayout->addWidget(importButton);
    
    QPushButton* exportResultsButton = new QPushButton("Export Analysis Results");
    connect(exportResultsButton, &QPushButton::clicked, this, &NLMViewer::onExportResults);
    exportImportGroupLayout->addWidget(exportResultsButton);
    
    QPushButton* importResultsButton = new QPushButton("Import Analysis Results");
    connect(importResultsButton, &QPushButton::clicked, this, &NLMViewer::onImportResults);
    exportImportGroupLayout->addWidget(importResultsButton);
    
    controlLayout->addWidget(exportImportGroup);
    
    // Analysis buttons
    QGroupBox* analysisGroup = new QGroupBox("Analysis");
    QVBoxLayout* analysisGroupLayout = new QVBoxLayout(analysisGroup);
    
    QPushButton* performanceAnalysisButton = new QPushButton("Show Performance Analysis");
    connect(performanceAnalysisButton, &QPushButton::clicked, this, &NLMViewer::onPerformanceAnalysisRequested);
    analysisGroupLayout->addWidget(performanceAnalysisButton);
    
    QPushButton* learningAnalysisButton = new QPushButton("Show Learning Analysis");
    connect(learningAnalysisButton, &QPushButton::clicked, this, &NLMViewer::onLearningAnalysisRequested);
    analysisGroupLayout->addWidget(learningAnalysisButton);
    
    QPushButton* networkAnalysisButton = new QPushButton("Show Network Analysis");
    connect(networkAnalysisButton, &QPushButton::clicked, this, &NLMViewer::onNetworkAnalysisRequested);
    analysisGroupLayout->addWidget(networkAnalysisButton);
    
    QPushButton* systemAnalysisButton = new QPushButton("Show System Analysis");
    connect(systemAnalysisButton, &QPushButton::clicked, this, &NLMViewer::onSystemAnalysisRequested);
    analysisGroupLayout->addWidget(systemAnalysisButton);
    
    controlLayout->addWidget(analysisGroup);
    
    controlWidget->setLayout(controlLayout);
    mainSplitter->insertWidget(1, controlWidget);
    
    // Connect signals
    connect(visualizationModeCombo, &QComboBox::currentTextChanged, this, &NLMViewer::onThemeChanged);
    connect(themeCombo, &QComboBox::currentTextChanged, this, &NLMViewer::onThemeChanged);
    connect(qualityCombo, &QComboBox::currentTextChanged, this, &NLMViewer::onQualityChanged);
    connect(realTimeUpdatesCheck, &QCheckBox::stateChanged, this, &NLMViewer::onRealTimeTogglesChanged);
    connect(updateFrequencySpin, &QSpinBox::valueChanged, this, &NLMViewer::onUpdateFrequencyChanged);
}

void NLMViewer::setupMenus() {
    QMenuBar* menuBar = this->menuBar();
    
    QMenu* fileMenu = menuBar->addMenu("File");
    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    QMenu* viewMenu = menuBar->addMenu("View");
    QAction* toggleControlsAction = viewMenu->addAction("Toggle Controls");
    connect(toggleControlsAction, &QAction::triggered, [this]() {
        QWidget* controlWidget = mainSplitter->widget(1);
        if (controlWidget) {
            controlWidget->setVisible(!controlWidget->isVisible());
        }
    });
    
    QMenu* helpMenu = menuBar->addMenu("Help");
    QAction* aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &NLMViewer::showAbout);
}

void NLMViewer::onVisualizationInitialized(bool success) {
    if (success) {
        QMessageBox::information(this, "Success", "Visualization initialized successfully!");
    } else {
        QMessageBox::warning(this, "Warning", "Failed to initialize visualization!");
    }
}

void NLMViewer::onVisualizationUpdate() {
    if (visualization && visualization->isActive()) {
        visualization->update(1.0 / 60.0); // 60 Hz
        updateVisualizationData();
    }
    
    updateVisualizationControls();
}

void NLMViewer::onVisualizationRender() {
    if (visualization && visualization->isActive()) {
        visualization->render();
    }
    
    // Update FPS counter
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count();
    frameCount++;
    
    if (elapsed >= 1000) {
        currentFPS = (frameCount * 1000.0) / elapsed;
        frameCount = 0;
        lastFrameTime = currentTime;
        
        fpsLabel->setText(QString::number(currentFPS, 'f', 1));
    }
}

void NLMViewer::onVisualizationRender() {
    if (visualization && visualization->isActive()) {
        visualization->render();
    }
}

void NLMViewer::onInputEvent(const QString& eventType) {
    if (visualization) {
        std::map<std::string, float> parameters;
        visualization->handleInput(eventType.toStdString(), parameters);
    }
}

void NLMViewer::onExportClicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Export Visualization Data", "", "JSON Files (*.json);;All Files (*.*)");
    if (!filename.isEmpty()) {
        if (visualization) {
            if (visualization->exportData(filename.toStdString())) {
                QMessageBox::information(this, "Success", "Visualization data exported successfully!");
            } else {
                QMessageBox::warning(this, "Warning", "Failed to export visualization data!");
            }
        }
    }
}

void NLMViewer::onImportClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Import Visualization Data", "", "JSON Files (*.json);;All Files (*.*)");
    if (!filename.isEmpty()) {
        if (visualization) {
            if (visualization->importData(filename.toStdString())) {
                QMessageBox::information(this, "Success", "Visualization data imported successfully!");
            } else {
                QMessageBox::warning(this, "Warning", "Failed to import visualization data!");
            }
        }
    }
}

void NLMViewer::onThemeChanged(const QString& theme) {
    if (visualization) {
        visualization->setTheme(theme.toStdString());
    }
}

void NLMViewer::onQualityChanged(const QString& quality) {
    if (visualization) {
        visualization->setQuality(quality.toStdString());
    }
}

void NLMViewer::onRealTimeTogglesChanged(bool enabled) {
    if (visualization) {
        visualization->setRealTimeUpdates(enabled);
    }
}

void NLMViewer::onUpdateFrequencyChanged(int frequency) {
    if (visualization) {
        visualization->setUpdateRate(frequency);
    }
}

void NLMViewer::onVisualizationModuleAdded(const QString& name) {
    if (visualization) {
        // Create a new visualization module
        auto module = std::make_unique<nlm::VisualizationComponent>();
        visualization->addVisualizationModule(std::move(module));
    }
}

void NLMViewer::onVisualizationModuleRemoved(const QString& name) {
    if (visualization) {
        visualization->removeVisualizationModule(name.toStdString());
    }
}

void NLMViewer::onDataSourceAdded(const QString& name) {
    if (!name.isEmpty()) {
        dataSourceList->addItem(name);
        if (visualization) {
            // In a real implementation, you would connect to the data source
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please enter a data source name!");
    }
}

void NLMViewer::onDataSourceRemoved(const QString& name) {
    QListWidgetItem* item = dataSourceList->currentItem();
    if (item) {
        delete dataSourceList->takeItem(dataSourceList->row(item));
    }
}

void NLMViewer::onExportResults() {
    QString filename = QFileDialog::getSaveFileName(this, "Export Analysis Results", "", "JSON Files (*.json);;All Files (*.*)");
    if (!filename.isEmpty()) {
        // In a real implementation, you would export the analysis results
        QMessageBox::information(this, "Information", "Analysis results export feature will be implemented!");
    }
}

void NLMViewer::onImportResults() {
    QString filename = QFileDialog::getOpenFileName(this, "Import Analysis Results", "", "JSON Files (*.json);;All Files (*.*)");
    if (!filename.isEmpty()) {
        // In a real implementation, you would import the analysis results
        QMessageBox::information(this, "Information", "Analysis results import feature will be implemented!");
    }
}

void NLMViewer::onPerformanceAnalysisRequested() {
    if (visualization) {
        // In a real implementation, you would run the performance analysis
        performanceAnalysisText->setPlainText("Performance analysis results will be displayed here!");
    }
}

void NLMViewer::onLearningAnalysisRequested() {
    if (visualization) {
        // In a real implementation, you would run the learning analysis
        learningAnalysisText->setPlainText("Learning analysis results will be displayed here!");
    }
}

void NLMViewer::onNetworkAnalysisRequested() {
    if (visualization) {
        // In a real implementation, you would run the network analysis
        networkAnalysisText->setPlainText("Network analysis results will be displayed here!");
    }
}

void NLMViewer::onSystemAnalysisRequested() {
    if (visualization) {
        // In a real implementation, you would run the system analysis
        systemAnalysisText->setPlainText("System analysis results will be displayed here!");
    }
}

void NLMViewer::showAbout() {
    QMessageBox::about(this, "About NLM Visualization Viewer",
        "NLM Visualization Viewer\n\n"
        "A comprehensive visualization and analysis tool for the enhanced NLM system.\n\n"
        "Features:\n"
        "- Neural network visualization\n"
        "- Memory system visualization\n"
        "- Neuromodulation visualization\n"
        "- Plasticity visualization\n"
        "- Development visualization\n"
        "- Performance analysis\n"
        "- Learning behavior analysis\n"
        "- Network dynamics analysis\n"
        "- System integration analysis\n\n"
        "Version 1.0.0");
}

void NLMViewer::updateVisualizationData() {
    // Update visualization data from the NLM system
    if (visualization) {
        // In a real implementation, you would update the visualization data from the NLM system
    }
}

void NLMViewer::updateVisualizationControls() {
    // Update visualization controls based on current state
    if (visualization) {
        // Update performance metrics
        auto metrics = visualization->getSystemIntegrationData().performance;
        fpsLabel->setText(QString::number(metrics.fps, 'f', 1));
        memoryLabel->setText(QString::number(metrics.memoryUsage) + " MB");
        cpuLabel->setText(QString::number(metrics.cpuUsage) + "%");
        performanceProgress->setValue(static_cast<int>(metrics.cpuUsage));
    }
}

void NLMViewer::closeEvent(QCloseEvent* event) {
    if (updateTimer) {
        updateTimer->stop();
    }
    if (renderTimer) {
        renderTimer->stop();
    }
    if (visualization) {
        visualization->close();
    }
    event->accept();
}

#include "NLMViewer.moc"
