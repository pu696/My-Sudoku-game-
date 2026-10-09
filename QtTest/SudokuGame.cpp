#include "SudokuGame.h"
#include <QStyleFactory>
#include <QPalette>
#include <QTime>
#include <QToolBar>
#include <QStatusBar>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDataStream>
#include <QFile>
#include <QInputDialog>
#include <sstream>


SudokuGame::SudokuGame(QWidget *parent)
    : QMainWindow(parent), selectedCell(nullptr), elapsedTime(0),
    currentStrategy(new MediumStrategy()), boardLayout(nullptr)
{
    // 初始化单元格数组
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cells[i][j] = nullptr;
        }
    }

    gameTimer = new QTimer(this);
    setWindowTitle("数独游戏");
    setMinimumSize(600, 500);

    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // 游戏板布局
    QWidget* boardWidget = new QWidget;
    boardLayout = new QGridLayout(boardWidget);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(5, 5, 5, 5);

    // SudokuGame.cpp
    // 创建数独格子
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cells[i][j] = new SudokuCell(i, j, boardWidget); // 设置父对象
            boardLayout->addWidget(cells[i][j], i, j);
            connect(cells[i][j], &SudokuCell::clicked, this, &SudokuGame::cellSelected);
        }
    }

    // 添加粗边框（修复覆盖问题）
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            QFrame* box = new QFrame(boardWidget);
            box->setFrameShape(QFrame::Box);
            box->setLineWidth(2);
            box->setStyleSheet("border-color: #000;");
            boardLayout->addWidget(box, i*3, j*3, 3, 3);
            box->lower(); // 确保在单元格下方
        }
    }

    mainLayout->addWidget(boardWidget, 3);

    // 右侧面板
    QWidget* sidePanel = new QWidget;
    QVBoxLayout* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setAlignment(Qt::AlignTop);

    // 计时器标签
    timerLabel = new QLabel("时间: 00:00");
    timerLabel->setFont(QFont("Arial", 14));
    sideLayout->addWidget(timerLabel);

    // 数字按钮
    QGridLayout* numberLayout = new QGridLayout;
    numberLayout->setSpacing(5);
    for (int i = 0; i < 9; i++) {
        numberButtons[i] = new QPushButton(QString::number(i+1));
        numberButtons[i]->setFixedSize(40, 40);
        numberButtons[i]->setFont(QFont("Arial", 14));
        connect(numberButtons[i], &QPushButton::clicked, [this, i]() { numberButtonClicked(i+1); });
        numberLayout->addWidget(numberButtons[i], i/3, i%3);
    }

    QPushButton* clearButton = new QPushButton("清除");
    clearButton->setFixedSize(40, 40);
    connect(clearButton, &QPushButton::clicked, [this]() { numberButtonClicked(0); });
    numberLayout->addWidget(clearButton, 3, 1);

    sideLayout->addLayout(numberLayout);
    sideLayout->addSpacing(20);

    // 功能按钮
    QPushButton* newButton = new QPushButton("新游戏");
    connect(newButton, &QPushButton::clicked, this, &SudokuGame::newGame);
    sideLayout->addWidget(newButton);

    QPushButton* hintButton = new QPushButton("提示");
    connect(hintButton, &QPushButton::clicked, this, &SudokuGame::showHints);
    sideLayout->addWidget(hintButton);

    QPushButton* solveButton = new QPushButton("求解");
    connect(solveButton, &QPushButton::clicked, this, &SudokuGame::solveGame);
    sideLayout->addWidget(solveButton);

    QPushButton* checkButton = new QPushButton("检查答案");
    connect(checkButton, &QPushButton::clicked, this, &SudokuGame::checkSolution);
    sideLayout->addWidget(checkButton);

    QPushButton* resetButton = new QPushButton("重置");
    connect(resetButton, &QPushButton::clicked, this, &SudokuGame::resetGame);
    sideLayout->addWidget(resetButton);

    mainLayout->addWidget(sidePanel, 1);

    // 创建菜单
    createActions();
    createMenus();

    // 状态栏
    statusBar()->showMessage("准备开始新游戏");

    // 计时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &SudokuGame::updateTimer);

    // 开始新游戏
    newGame();
}

SudokuGame::~SudokuGame() {
    delete currentStrategy;
}

void SudokuGame::createActions() {
    newGameAction = new QAction("新游戏", this);
    newGameAction->setShortcut(QKeySequence::New);
    connect(newGameAction, &QAction::triggered, this, &SudokuGame::newGame);

    loadGameAction = new QAction("加载数独", this);
    loadGameAction->setShortcut(QKeySequence::Open);
    connect(loadGameAction, &QAction::triggered, this, &SudokuGame::loadGame);

    saveGameAction = new QAction("保存数独", this);
    saveGameAction->setShortcut(QKeySequence::Save);
    connect(saveGameAction, &QAction::triggered, this, &SudokuGame::saveGame);

    saveStateAction = new QAction("保存游戏状态", this);
    saveStateAction->setShortcut(Qt::CTRL | Qt::Key_S);
    connect(saveStateAction, &QAction::triggered, [this]() {
        QString filename = QFileDialog::getSaveFileName(this, "保存游戏状态", "", "游戏状态 (*.sav)");
        if (!filename.isEmpty()) {
            if (saveGameState(filename)) {
                statusBar()->showMessage("游戏状态已保存: " + filename);
            } else {
                QMessageBox::warning(this, "错误", "无法保存游戏状态");
            }
        }
    });

    loadStateAction = new QAction("加载游戏状态", this);
    loadStateAction->setShortcut(Qt::CTRL | Qt::Key_L);
    connect(loadStateAction, &QAction::triggered, [this]() {
        QString filename = QFileDialog::getOpenFileName(this, "加载游戏状态", "", "游戏状态 (*.sav)");
        if (!filename.isEmpty()) {
            if (loadGameState(filename)) {
                statusBar()->showMessage("游戏状态已加载: " + filename);
            } else {
                QMessageBox::warning(this, "错误", "无法加载游戏状态");
            }
        }
    });

    exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    solveAction = new QAction("求解", this);
    solveAction->setShortcut(Qt::Key_F5);
    connect(solveAction, &QAction::triggered, this, &SudokuGame::solveGame);

    resetAction = new QAction("重置", this);
    resetAction->setShortcut(Qt::Key_F2);
    connect(resetAction, &QAction::triggered, this, &SudokuGame::resetGame);

    checkAction = new QAction("检查答案", this);
    checkAction->setShortcut(Qt::Key_F8);
    connect(checkAction, &QAction::triggered, this, &SudokuGame::checkSolution);

    hintAction = new QAction("提示", this);
    hintAction->setShortcut(Qt::Key_F1);
    connect(hintAction, &QAction::triggered, this, &SudokuGame::showHints);

    easyAction = new QAction("简单", this);
    connect(easyAction, &QAction::triggered, this, &SudokuGame::setDifficultyEasy);

    mediumAction = new QAction("中等", this);
    connect(mediumAction, &QAction::triggered, this, &SudokuGame::setDifficultyMedium);

    hardAction = new QAction("困难", this);
    connect(hardAction, &QAction::triggered, this, &SudokuGame::setDifficultyHard);

    aboutAction = new QAction("关于", this);
    connect(aboutAction, &QAction::triggered, this, &SudokuGame::aboutGame);
}

void SudokuGame::createMenus() {
    fileMenu = menuBar()->addMenu("文件");
    fileMenu->addAction(newGameAction);
    fileMenu->addAction(loadGameAction);
    fileMenu->addAction(saveGameAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveStateAction);
    fileMenu->addAction(loadStateAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    gameMenu = menuBar()->addMenu("游戏");
    gameMenu->addAction(solveAction);
    gameMenu->addAction(resetAction);
    gameMenu->addAction(checkAction);
    gameMenu->addAction(hintAction);
    gameMenu->addSeparator();

    QMenu* difficultyMenu = gameMenu->addMenu("难度");
    difficultyMenu->addAction(easyAction);
    difficultyMenu->addAction(mediumAction);
    difficultyMenu->addAction(hardAction);

    helpMenu = menuBar()->addMenu("帮助");
    helpMenu->addAction(aboutAction);
}

void SudokuGame::newGame() {
    sudoku.generatePuzzle(currentStrategy);
    updateBoard();
    gameTimer->stop();
    elapsedTime = 0;
    updateTimer();
    gameTimer->start(1000);
    statusBar()->showMessage("新游戏已开始");
}

void SudokuGame::loadGame() {
    QString filename = QFileDialog::getOpenFileName(this, "打开数独文件", "", "数独文件 (*.sdk);;所有文件 (*.*)");
    if (!filename.isEmpty()) {
        if (sudoku.loadFromFile(filename.toStdString())) {
            updateBoard();
            gameTimer->stop();
            elapsedTime = 0;
            updateTimer();
            gameTimer->start(1000);
            statusBar()->showMessage("数独已加载: " + filename);
        } else {
            QMessageBox::warning(this, "错误", "无法加载数独文件");
        }
    }
}

void SudokuGame::saveGame() {
    QString filename = QFileDialog::getSaveFileName(this, "保存数独文件", "", "数独文件 (*.sdk);;所有文件 (*.*)");
    if (!filename.isEmpty()) {
        if (!filename.endsWith(".sdk")) {
            filename += ".sdk";
        }
        if (sudoku.saveToFile(filename.toStdString())) {
            statusBar()->showMessage("数独已保存: " + filename);
        } else {
            QMessageBox::warning(this, "错误", "无法保存数独文件");
        }
    }
}

void SudokuGame::solveGame() {
    if (sudoku.solve()) {
        updateBoard();
        gameTimer->stop();
        statusBar()->showMessage("数独已解决");
        QMessageBox::information(this, "求解成功", "数独已成功求解！");
    } else {
        QMessageBox::warning(this, "错误", "此数独无解");
    }
}

void SudokuGame::resetGame() {
    sudoku.reset();
    updateBoard();
    gameTimer->stop();
    elapsedTime = 0;
    updateTimer();
    gameTimer->start(1000);
    statusBar()->showMessage("游戏已重置");
}

void SudokuGame::checkSolution() {
    if (sudoku.isSolved()) {
        gameTimer->stop();
        QMessageBox::information(this, "恭喜", "恭喜！您已正确完成数独！");
    } else {
        highlightConflicts();
        statusBar()->showMessage("存在错误，请检查高亮区域");
    }
}

void SudokuGame::showHints() {
    if(!selectedCell) return;
    if (selectedCell) {
        int row = selectedCell->getRow();
        int col = selectedCell->getCol();
        if (!sudoku.isOriginalCell(row, col)) {
            auto possibilities = sudoku.getPossibleNumbers(row, col);
            selectedCell->setPossibleNumbers(possibilities);
        }
    }
}

void SudokuGame::setDifficultyEasy() {

     if (currentStrategy) delete currentStrategy;
    currentStrategy = new EasyStrategy();
     newGame();
}

void SudokuGame::setDifficultyMedium() {
    if (currentStrategy) delete currentStrategy;
    currentStrategy = new MediumStrategy();
    newGame();
}

void SudokuGame::setDifficultyHard() {
    if (currentStrategy) delete currentStrategy;
    currentStrategy = new HardStrategy();
    newGame();
}

void SudokuGame::aboutGame() {
    QMessageBox::about(this, "关于数独游戏",
                       "<h2>数独游戏 v1.0</h2>"
                       "<p>基于Qt和C++开发的数独游戏</p>"
                       "<p>功能："
                       "<ul>"
                       "<li>新游戏（简单/中等/困难三种难度）</li>"
                       "<li>加载/保存数独</li>"
                       "<li>保存/加载游戏状态（包含计时）</li>"
                       "<li>提示功能</li>"
                       "<li>自动求解</li>"
                       "<li>答案检查</li>"
                       "</ul>"
                       "<p>© 2023 面向对象程序设计实践项目</p>");
}

void SudokuGame::cellSelected(SudokuCell* cell) {
    clearHighlights();

    selectedCell = cell;
    if (cell) {
        cell->setHighlight(true);
        highlightRelatedCells(cell->getRow(), cell->getCol());
    }
}

void SudokuGame::numberButtonClicked(int num) {
    if(!selectedCell) return;
    if (selectedCell) {
        int row = selectedCell->getRow();
        int col = selectedCell->getCol();

        if (num == 0) {
            sudoku.removeNumber(row, col);
        } else {
            if (!sudoku.isOriginalCell(row, col)) {
                sudoku.placeNumber(row, col, num);
            }
        }

        updateBoard();

        if (sudoku.isSolved()) {
            gameTimer->stop();
            QMessageBox::information(this, "恭喜", "恭喜！您已正确完成数独！");
        }
    }
}

void SudokuGame::updateTimer() {
    elapsedTime++;
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    timerLabel->setText(QString("时间: %1:%2")
                            .arg(minutes, 2, 10, QLatin1Char('0'))
                            .arg(seconds, 2, 10, QLatin1Char('0')));
}

void SudokuGame::keyPressEvent(QKeyEvent* event) {
    if(!selectedCell) return;
    if (selectedCell) {
        int key = event->key();
        if (key >= Qt::Key_1 && key <= Qt::Key_9) {
            numberButtonClicked(key - Qt::Key_0);
        } else if (key == Qt::Key_Backspace || key == Qt::Key_Delete || key == Qt::Key_0) {
            numberButtonClicked(0);
        }
    }
    QMainWindow::keyPressEvent(event);
}

void SudokuGame::updateBoard() {
    auto board = sudoku.getBoard();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cells[i][j]->setNumber(board[i][j], sudoku.isOriginalCell(i, j));
        }
    }
    highlightConflicts();
}

void SudokuGame::highlightConflicts() {
    clearHighlights();

    auto conflicts = sudoku.getConflicts();
    for (const auto& pos : conflicts) {
        int row = pos.first;
        int col = pos.second;
        cells[row][col]->setConflict(true);
    }
}

void SudokuGame::clearHighlights() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cells[i][j]->setConflict(false);
            cells[i][j]->setHighlight(false);
        }
    }
}

void SudokuGame::highlightRelatedCells(int row, int col) {
    // 高亮相同行
    for (int c = 0; c < 9; c++) {
        if (c != col) {
            cells[row][c]->setHighlight(true);
        }
    }

    // 高亮相同列
    for (int r = 0; r < 9; r++) {
        if (r != row) {
            cells[r][col]->setHighlight(true);
        }
    }

    // 高亮相同宫格
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int r = startRow; r < startRow + 3; r++) {
        for (int c = startCol; c < startCol + 3; c++) {
            if (r != row || c != col) {
                cells[r][c]->setHighlight(true);
            }
        }
    }
}

bool SudokuGame::saveGameState(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);

    // 保存数独状态
    auto board = sudoku.getBoard();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            out << board[i][j];
            out << sudoku.isOriginalCell(i, j);
        }
    }

    // 保存游戏状态
    out << elapsedTime;

    file.close();
    return true;
}

bool SudokuGame::loadGameState(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);

    // 加载数独状态
    std::vector<std::vector<int>> board(9, std::vector<int>(9));
    std::vector<std::vector<bool>> original(9, std::vector<bool>(9, false));

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> board[i][j];
            bool isOriginal;
            in >> isOriginal;
            original[i][j] = isOriginal;
        }
    }

    sudoku.loadBoard(board);
    sudoku.setOriginalCells(original);

    // 加载游戏状态
    in >> elapsedTime;

    file.close();

    updateBoard();
    updateTimer();
    gameTimer->start(1000);
    return true;
}
