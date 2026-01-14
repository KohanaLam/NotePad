#include "NotePad.h"

NotePad::NotePad(QWidget *parent)
    : QMainWindow(parent)
{
    openAction = new QAction("&open", this); // &符号和快捷键有关
    saveAction = new QAction("&Save", this);
    exitAction = new QAction("&Exit", this);

    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    fileMenu = menuBar()->addMenu("File");
    editMenu = menuBar()->addMenu("Edit");
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    textEdit = new QTextEdit();
    setCentralWidget(textEdit);
    setWindowTitle("Notepad");
}

void NotePad::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "",
        "Text Files (*.txt);;C++ Files (*.cpp *.h)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, "Error", "could not open file");
            return;
        }
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
    }
}

void NotePad::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "",
        "Text Files (*.txt);;C++ Files (*.cpp *.h)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            // error message
            QMessageBox::critical(this, "Error", "could not write file");
        }
        else {
            QTextStream stream(&file);
            stream << textEdit->toPlainText();
            stream.flush();
            file.close();
        }
    }
}

void NotePad::quit()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("Notepad");
    messageBox.setText("Do you really want to quit?");
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if (messageBox.exec() == QMessageBox::Yes)
        qApp->quit();
}


