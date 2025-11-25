#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "../decision_tree/decision_tree.hpp"
#include "../dataset/dataset.hpp"
#include "tree_scene.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onRunExampleClicked();
    void onRunAllClicked();
    void onSplitClicked();
    void onResetClicked();
    void onSpeedChanged(int value);
    void onTraversalStep(int nodeId);
    void onTraversalFinished(int nodeId);

private:
    DecisionTree decisionTree_;
    // Dataset dataset_; // Removed to use decisionTree_.getDataset()
    
    TreeScene* scene_;
    QGraphicsView* view_;
    QPushButton* runButton_;
    QPushButton* runAllButton_;
    QPushButton* splitButton_;
    QPushButton* resetButton_;
    QSlider* speedSlider_;
    QLabel* statusLabel_;
    
    QTimer* runAllTimer_;
    int currentRunIndex_;
};
