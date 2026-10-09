#include "SudokuCell.h"
#include <QMouseEvent>
#include <QFont>
#include <algorithm>
#include <iostream>

SudokuCell::SudokuCell(int row, int col, QWidget* parent)
    : QPushButton(parent), row(row), col(col), number(0),
    original(false), conflict(false), highlight(false) {
    setFixedSize(50, 50);
    setFont(QFont("Arial", 16));
    setStyleSheet("QPushButton { border: 1px solid #aaa; }");
    connect(this, &QPushButton::clicked, [this]() { emit clicked(this); });
}

void SudokuCell::mousePressEvent(QMouseEvent* e) {
    // 只在左键点击时发射信号
    if (e->button() == Qt::LeftButton) {
        emit clicked(this);
    }
    QPushButton::mousePressEvent(e);
}

void SudokuCell::setNumber(int num, bool isOriginal) {
    number = num;
    original = isOriginal;

    if (num == 0) {
        setText("");
    } else {
        setText(QString::number(num));
    }

    setFont(QFont("Arial", 16)); // 重置字体

    if (original) {
        setStyleSheet("QPushButton {"
                      "  border: 1px solid #aaa;"
                      "  background-color: #f0f0f0;"
                      "  color: #000;"
                      "  font-weight: bold;"
                      "}");
    } else {
        updateStyle();
    }
}

void SudokuCell::setConflict(bool conflict) {
    this->conflict = conflict;
    updateStyle();
}

void SudokuCell::setHighlight(bool highlight) {
    this->highlight = highlight;
    updateStyle();
}

void SudokuCell::setPossibleNumbers(const std::vector<int>& nums) {
    possibleNumbers = nums;
    if (number == 0 && !nums.empty()) {
        QString text;
        for (int i = 0; i < 9; i++) {
            int num = i + 1;
            if (std::find(nums.begin(), nums.end(), num) != nums.end()) {
                text += QString::number(num);
            } else {
                text += " ";
            }

            if ((i + 1) % 3 == 0) {
                text += "\n";
            } else {
                text += " ";
            }
        }
        setText(text);
        setFont(QFont("Arial", 8));
    } else {
        // 恢复原始显示
        setNumber(number, original);
    }
}

void SudokuCell::updateStyle() {
    QString style = "QPushButton { border: 1px solid #aaa; ";

    if (conflict) {
        style += "background-color: #ffcccc; ";
    } else if (highlight) {
        style += "background-color: #e6f7ff; ";
    } else if (!original) {
        style += "background-color: #ffffff; ";
    }

    if (!original) {
        style += "color: #0066cc; ";
    }

    style += "}";
    setStyleSheet(style);
}
