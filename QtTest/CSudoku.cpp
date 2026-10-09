#include "CSudoku.h"
#include "DifficultyStrategy.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <set>
#include <ctime>

CSudoku::CSudoku() {
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    board = std::vector<std::vector<int>>(9, std::vector<int>(9, 0));
    original = std::vector<std::vector<bool>>(9, std::vector<bool>(9, false));
}

CSudoku::~CSudoku() {}

void CSudoku::loadBoard(const std::vector<std::vector<int>>& input) {
    if (input.size() != 9 || input[0].size() != 9) {
        throw std::invalid_argument("Invalid board size");
    }

    board = input;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            original[i][j] = (board[i][j] != 0);
        }
    }
}

void CSudoku::setOriginalCells(const std::vector<std::vector<bool>>& original) {
    this->original = original;
}

bool CSudoku::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<std::vector<int>> newBoard(9, std::vector<int>(9, 0));
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < 9) {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;

        while (ss >> cell && col < 9) {
            if (cell == "." || cell == "0") {
                newBoard[row][col] = 0;
            } else {
                try {
                    newBoard[row][col] = std::stoi(cell);
                } catch (...) {
                    newBoard[row][col] = 0;
                }
            }
            col++;
        }
        row++;
    }

    file.close();
    loadBoard(newBoard);
    return true;
}

bool CSudoku::solve() {
    if (!isValidBoard()) {
        std::cerr << "警告: 尝试求解无效数独" << std::endl;
        return false;
    }

    std::vector<std::vector<int>> backup = board;
    if (backtrackSolve(0, 0)) {
        return true;
    }

    board = backup;
    return false;
}

bool CSudoku::backtrackSolve(int row, int col) {
    // 如果到达最后一行，说明已解决
    if (row == 9) {
        return true;
    }

    // 如果到达行尾，转到下一行
    if (col == 9) {
        return backtrackSolve(row + 1, 0);
    }

    // 如果当前单元格已有数字，跳过
    if (board[row][col] != 0) {
        return backtrackSolve(row, col + 1);
    }

    // 创建1-9的数字列表并随机排序
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::random_shuffle(numbers.begin(), numbers.end());

    // 尝试每个数字
    for (int num : numbers) {
        if (isSafe(row, col, num)) {
            board[row][col] = num;

            // 递归解决下一个单元格
            if (backtrackSolve(row, col + 1)) {
                return true;
            }

            // 回溯
            board[row][col] = 0;
        }
    }

    return false;
}

bool CSudoku::isSafe(int row, int col, int num) const {
    // 检查行
    for (int c = 0; c < 9; c++) {
        if (c != col && board[row][c] == num) {
            return false;
        }
    }

    // 检查列
    for (int r = 0; r < 9; r++) {
        if (r != row && board[r][col] == num) {
            return false;
        }
    }

    // 检查3x3宫格
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int checkRow = startRow + r;
            int checkCol = startCol + c;
            if (checkRow != row && checkCol != col && board[checkRow][checkCol] == num) {
                return false;
            }
        }
    }

    return true;
}

bool CSudoku::placeNumber(int row, int col, int num) {
    if (row < 0 || row >= 9 || col < 0 || col >= 9 ||
        num < 1 || num > 9 || original[row][col]) {
        return false;
    }

    board[row][col] = num;
    return true;
}

void CSudoku::removeNumber(int row, int col) {
    if (row < 0 || row >= 9 || col < 0 || col >= 9 || original[row][col]) {
        return;
    }
    board[row][col] = 0;
}

void CSudoku::reset() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!original[i][j]) {
                board[i][j] = 0;
            }
        }
    }
}

const std::vector<std::vector<int>>& CSudoku::getBoard() const {
    return board;
}

bool CSudoku::isSolved() const {
    // 首先检查是否有空格
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) {
                return false;
            }
        }
    }

    // 然后检查整个棋盘是否有效
    return isValidBoard();
}

bool CSudoku::isOriginalCell(int row, int col) const {
    return original[row][col];
}

bool CSudoku::isValidPlacement(int row, int col, int num) const {
    if (num == 0) return true;
    return isSafe(row, col, num);
}

std::vector<std::pair<int, int>> CSudoku::getConflicts() const {
    std::vector<std::pair<int, int>> conflicts;

    // 检查行
    for (int row = 0; row < 9; row++) {
        std::vector<int> count(10, 0);
        for (int col = 0; col < 9; col++) {
            if (board[row][col] != 0) {
                count[board[row][col]]++;
            }
        }

        for (int col = 0; col < 9; col++) {
            if (board[row][col] != 0 && count[board[row][col]] > 1) {
                conflicts.push_back({row, col});
            }
        }
    }

    // 检查列
    for (int col = 0; col < 9; col++) {
        std::vector<int> count(10, 0);
        for (int row = 0; row < 9; row++) {
            if (board[row][col] != 0) {
                count[board[row][col]]++;
            }
        }

        for (int row = 0; row < 9; row++) {
            if (board[row][col] != 0 && count[board[row][col]] > 1) {
                conflicts.push_back({row, col});
            }
        }
    }

    // 检查宫格
    for (int box = 0; box < 9; box++) {
        int startRow = (box / 3) * 3;
        int startCol = (box % 3) * 3;
        std::vector<int> count(10, 0);

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int num = board[startRow + r][startCol + c];
                if (num != 0) {
                    count[num]++;
                }
            }
        }

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int num = board[startRow + r][startCol + c];
                if (num != 0 && count[num] > 1) {
                    conflicts.push_back({startRow + r, startCol + c});
                }
            }
        }
    }

    // 去重
    std::set<std::pair<int, int>> uniqueConflicts(conflicts.begin(), conflicts.end());
    return std::vector<std::pair<int, int>>(uniqueConflicts.begin(), uniqueConflicts.end());
}

std::vector<int> CSudoku::getPossibleNumbers(int row, int col) const {
    std::vector<int> possibilities;
    if (board[row][col] != 0) {
        return possibilities;
    }

    for (int num = 1; num <= 9; num++) {
        if (isSafe(row, col, num)) {
            possibilities.push_back(num);
        }
    }
    return possibilities;
}

bool CSudoku::isValidBoard() const {
    // 检查冲突
    auto conflicts = getConflicts();
    return conflicts.empty();
}

bool CSudoku::findEmptyCell(int& row, int& col) const {
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (board[row][col] == 0) {
                return true;
            }
        }
    }
    return false;
}

void CSudoku::clearBoard() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = 0;
            original[i][j] = false;
        }
    }
}

void CSudoku::generateCompleteBoard() {
    clearBoard();

    // 填充对角线宫格
    for (int box = 0; box < 3; box++) {
        std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        // 使用简单随机洗牌算法，避免std::shuffle问题
        for (int i = 8; i > 0; i--) {
            int j = std::rand() % (i + 1);
            std::swap(numbers[i], numbers[j]);
        }

        int startRow = box * 3;
        int startCol = box * 3;
        int index = 0;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                board[startRow + r][startCol + c] = numbers[index++];
                original[startRow + r][startCol + c] = true;
            }
        }
    }

    // 使用回溯法填充剩余部分
    if (!backtrackSolve(0, 0)) {
        // 如果第一次失败，尝试重新生成
        std::cerr << "第一次生成失败，尝试重新生成..." << std::endl;
        generateCompleteBoard();
    }

    // 标记所有单元格为原始
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            original[i][j] = true;
        }
    }
}

void CSudoku::removeNumbers(int count) {
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            positions.push_back({i, j});
        }
    }

    // 随机打乱位置
    for (int i = positions.size() - 1; i > 0; i--) {
        int j = std::rand() % (i + 1);
        std::swap(positions[i], positions[j]);
    }

    int removed = 0;
    for (const auto& pos : positions) {
        if (removed >= count) break;

        int row = pos.first;
        int col = pos.second;

        // 跳过已经移除的数字
        if (board[row][col] == 0) continue;

        int backup = board[row][col];
        board[row][col] = 0;
        original[row][col] = false;

        // 确保数独仍然有唯一解
        std::vector<std::vector<int>> tempBoard = board;
        if (solve()) {
            board = tempBoard; // 恢复移除后的状态
            removed++;
        } else {
            board[row][col] = backup;
            original[row][col] = true;
        }
    }
}

void CSudoku::generatePuzzle(DifficultyStrategy* strategy) {
    // 最多尝试3次生成有效的数独
    for (int attempt = 0; attempt < 3; attempt++) {
        generateCompleteBoard();
        removeNumbers(strategy->getRemovalCount());

        // 检查生成的谜题是否可解
        std::vector<std::vector<int>> backup = board;
        if (solve()) {
            board = backup; // 恢复未解状态
            return;
        }
    }

    // 如果3次尝试都失败，使用简单模式生成
    std::cerr << "无法生成有效数独，使用简单模式..." << std::endl;
    generateCompleteBoard();
    removeNumbers(35); // 简单难度
}

bool CSudoku::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) {
                file << ".";
            } else {
                file << board[i][j];
            }

            if (j < 8) {
                file << " ";
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}
