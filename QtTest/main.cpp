#include "SudokuGame.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序样式
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 设置深色主题
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);

    // 应用样式表
    app.setStyleSheet(
        "QMainWindow { background-color: #333; }"
        "QMenuBar { background-color: #444; color: white; }"
        "QMenuBar::item:selected { background-color: #555; }"
        "QMenu { background-color: #444; border: 1px solid #555; }"
        "QMenu::item:selected { background-color: #555; }"
        "QStatusBar { background-color: #333; color: #aaa; border-top: 1px solid #555; }"
        "QPushButton { background-color: #555; border: 1px solid #777; border-radius: 3px; padding: 5px; color: white; }"
        "QPushButton:hover { background-color: #666; }"
        "QPushButton:pressed { background-color: #444; }"
        "QLabel { color: #eee; }"
        "QDialog { background-color: #444; }"
        "QMessageBox { background-color: #444; }"
        "QMessageBox QLabel { color: #eee; }"
        );

    SudokuGame game;
    game.show();

    return app.exec();
}
