#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <iostream>
using namespace std;
#include "ui_NotePad.h"

class NotePad : public QMainWindow
{
    Q_OBJECT

public:
    NotePad(QWidget* parent = nullptr);

private slots:
    void open();
    void save();
    void quit();

private:
    QAction* openAction;
    QAction* saveAction;
    QAction* exitAction;

    QMenu* fileMenu;
    QMenu* editMenu;
    QTextEdit* textEdit;

};

