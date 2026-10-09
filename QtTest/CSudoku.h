#ifndef CSUDOKU_H
#define CSUDOKU_H

#include <vector>
#include <string>
#include "DifficultyStrategy.h"

class CSudoku {
public:
    CSudoku();
    ~CSudoku();

    // 加载数独
    void loadBoard(const std::vector<std::vector<int>>& board);
    bool loadFromFile(const std::string& filename);

    // 数独操作
    bool solve();
    void reset();
    bool placeNumber(int row, int col, int num);
    void removeNumber(int row, int col);

    // 获取状态
    const std::vector<std::vector<int>>& getBoard() const;
    bool isSolved() const;
    bool isValidPlacement(int row, int col, int num) const;
    bool isOriginalCell(int row, int col) const;
    std::vector<std::pair<int, int>> getConflicts() const;
    std::vector<int> getPossibleNumbers(int row, int col) const;

    // 生成数独
    void generatePuzzle(DifficultyStrategy* strategy);

    // 文件操作
    bool saveToFile(const std::string& filename) const;
    void setOriginalCells(const std::vector<std::vector<bool>>& original);

private:
    // 回溯法求解
    bool backtrackSolve(int row, int col);

    // 辅助函数
    bool isValidBoard() const;
    bool findEmptyCell(int& row, int& col) const;
    bool isSafe(int row, int col, int num) const;
    void clearBoard();
    void generateCompleteBoard();
    void removeNumbers(int count);

    // 数独存储
    std::vector<std::vector<int>> board;
    std::vector<std::vector<bool>> original;
};

#endif // CSUDOKU_H
