#include "main_window.hpp"
#include <QWidget>
#include <QTime>
#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    // Setup UI
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    scene_ = new TreeScene(this);
    view_ = new QGraphicsView(scene_);
    view_->setRenderHint(QPainter::Antialiasing);
    
    // Controls Layout
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    
    runButton_ = new QPushButton("Run Random Example", this);
    connect(runButton_, &QPushButton::clicked, this, &MainWindow::onRunExampleClicked);
    
    runAllButton_ = new QPushButton("Run All Examples", this);
    connect(runAllButton_, &QPushButton::clicked, this, &MainWindow::onRunAllClicked);
    
    splitButton_ = new QPushButton("Create Split", this);
    connect(splitButton_, &QPushButton::clicked, this, &MainWindow::onSplitClicked);
    
    resetButton_ = new QPushButton("Reset Tree", this);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    
    controlsLayout->addWidget(runButton_);
    controlsLayout->addWidget(runAllButton_);
    controlsLayout->addWidget(splitButton_);
    controlsLayout->addWidget(resetButton_);
    
    // Speed Slider
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    QLabel* sliderLabel = new QLabel("Speed:", this);
    speedSlider_ = new QSlider(Qt::Horizontal, this);
    speedSlider_->setRange(10, 1000); // 10ms to 1000ms
    speedSlider_->setValue(500); // Default 500ms
    speedSlider_->setInvertedAppearance(true); // Left is slow (high delay), Right is fast (low delay)? 
    // Usually Right is Fast. So Low Delay.
    // Let's make Right = Fast (Low Delay).
    // Range 10 (Fast) to 1000 (Slow).
    // InvertedAppearance false: Left=10, Right=1000.
    // We want Left=Slow(1000), Right=Fast(10).
    speedSlider_->setInvertedAppearance(true); 
    
    connect(speedSlider_, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    
    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(speedSlider_);
    
    statusLabel_ = new QLabel("Ready", this);
    
    mainLayout->addWidget(view_);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addWidget(statusLabel_);
    
    resize(900, 700);
    
    // Timer for Run All
    runAllTimer_ = new QTimer(this);
    connect(runAllTimer_, &QTimer::timeout, this, [this]() {
        const Dataset& ds = decisionTree_.getDataset();
        if (currentRunIndex_ >= ds.totalContainers()) {
            runAllTimer_->stop();
            statusLabel_->setText("Finished running all examples.");
            return;
        }
        
        const DataContainer& sample = ds.getContainer(currentRunIndex_);
        
        // Run logic on backend to update counts
        decisionTree_.getHeadNode()->runInput(sample);
        
        // Start visual traversal
        scene_->startTraversal(decisionTree_.getHeadNode(), sample);
        
        // Update visuals (sizes)
        scene_->updateNodeVisuals();
        
        statusLabel_->setText(QString("Running sample %1/%2").arg(currentRunIndex_ + 1).arg(ds.totalContainers()));
        
        currentRunIndex_++;
    });
    
    // Setup Logic
    // Initially run tree to populate root with samples so splitting is possible
    decisionTree_.runTree();
    
    scene_->setRootNode(decisionTree_.getHeadNode());
    
    connect(scene_, &TreeScene::traversalStep, this, &MainWindow::onTraversalStep);
    connect(scene_, &TreeScene::traversalFinished, this, &MainWindow::onTraversalFinished);
}

void MainWindow::onRunExampleClicked() {
    const Dataset& ds = decisionTree_.getDataset();
    if (ds.totalContainers() == 0) {
        statusLabel_->setText("No data loaded!");
        return;
    }
    
    int idx = QRandomGenerator::global()->bounded(ds.totalContainers());
    const DataContainer& sample = ds.getContainer(idx);
    
    statusLabel_->setText(QString("Running sample %1 (Label: %2)").arg(idx).arg(QString::fromStdString(sample.getLabel())));
    
    scene_->startTraversal(decisionTree_.getHeadNode(), sample);
}

void MainWindow::onRunAllClicked() {
    // Reset tree counts
    decisionTree_.resetTree();
    scene_->updateNodeVisuals(); // Reset sizes to small
    
    currentRunIndex_ = 0;
    runAllTimer_->start(speedSlider_->value()); 
    statusLabel_->setText("Starting run of all examples...");
}

void MainWindow::onSplitClicked() {
    try {
        decisionTree_.makeSplits();
        scene_->setRootNode(decisionTree_.getHeadNode());
        statusLabel_->setText("Attempted to split nodes.");
    } catch (const std::exception& e) {
        statusLabel_->setText(QString("Error splitting: %1").arg(e.what()));
    }
}

void MainWindow::onResetClicked() {
    runAllTimer_->stop();
    decisionTree_.resetTree();
    
    // We might want to reset structure too? "Reset Tree" usually implies back to root?
    // The user said "reset the tree".
    // If they want to clear splits, we need a way to clear children.
    // DecisionTree::makeHeadNode() resets the structure.
    decisionTree_.makeHeadNode();
    
    // Re-run to populate root?
    // Usually we want a fresh start.
    // But if we don't run anything, root has 0 samples.
    // Let's just reset structure.
    
    scene_->setRootNode(decisionTree_.getHeadNode());
    statusLabel_->setText("Tree reset to initial state.");
}

void MainWindow::onSpeedChanged(int value) {
    if (runAllTimer_->isActive()) {
        runAllTimer_->setInterval(value);
    }
}

void MainWindow::onTraversalStep(int nodeId) {
    // Optional: Update status or log
}

void MainWindow::onTraversalFinished(int nodeId) {
    // statusLabel_->setText(statusLabel_->text() + " - Finished at Node " + QString::number(nodeId));
}
