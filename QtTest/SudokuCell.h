#ifndef SUDOKUCELL_H
#define SUDOKUCELL_H

#include <QPushButton>
#include <vector>

class SudokuCell : public QPushButton {
    Q_OBJECT

public:
    SudokuCell(int row, int col, QWidget* parent = nullptr);

    int getRow() const { return row; }
    int getCol() const { return col; }
    void setNumber(int num, bool isOriginal);
    int getNumber() const { return number; }
    bool isOriginal() const { return original; }
    void setConflict(bool conflict);
    void setHighlight(bool highlight);
    void setPossibleNumbers(const std::vector<int>& nums);

signals:
    void clicked(SudokuCell* cell);

protected:
    void mousePressEvent(QMouseEvent* e) override;

private:
    void updateStyle();

    int row;
    int col;
    int number;
    bool original;
    bool conflict;
    bool highlight;
    std::vector<int> possibleNumbers;
};

#endif // SUDOKUCELL_H
