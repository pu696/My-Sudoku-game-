#ifndef SUDOKUGAME_H
#define SUDOKUGAME_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "CSudoku.h"
#include "SudokuCell.h"
#include "DifficultyStrategy.h"

class SudokuCell;

class SudokuGame : public QMainWindow {
    Q_OBJECT

public:
    explicit SudokuGame(QWidget *parent = nullptr);
    ~SudokuGame();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void newGame();
    void loadGame();
    void saveGame();
    void solveGame();
    void resetGame();
    void checkSolution();
    void showHints();
    void setDifficultyEasy();
    void setDifficultyMedium();
    void setDifficultyHard();
    void aboutGame();
    void cellSelected(SudokuCell* cell);
    void numberButtonClicked(int num);
    void updateTimer();

private:
    void createActions();
    void createMenus();
    void createGameBoard();
    void createNumberPanel();
    void updateBoard();
    void highlightConflicts();
    void clearHighlights();
    void highlightRelatedCells(int row, int col);
    bool saveGameState(const QString& filename);
    bool loadGameState(const QString& filename);

    CSudoku sudoku;
    SudokuCell* selectedCell;
    QTimer* gameTimer;
    int elapsedTime;
    DifficultyStrategy* currentStrategy;

    // UI 组件
    QGridLayout* boardLayout;
    SudokuCell* cells[9][9];
    QPushButton* numberButtons[9];
    QLabel* timerLabel;

    // 菜单
    QMenu* fileMenu;
    QMenu* gameMenu;
    QMenu* helpMenu;

    // 动作
    QAction* newGameAction;
    QAction* loadGameAction;
    QAction* saveGameAction;
    QAction* saveStateAction;
    QAction* loadStateAction;
    QAction* exitAction;
    QAction* solveAction;
    QAction* resetAction;
    QAction* checkAction;
    QAction* hintAction;
    QAction* easyAction;
    QAction* mediumAction;
    QAction* hardAction;
    QAction* aboutAction;
};

#endif // SUDOKUGAME_H
