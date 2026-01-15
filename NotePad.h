#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QtWidgets>
#include <QStyle> 
#include <QStatusBar>
#include <QtConcurrent> // 引入并发模块
#include <QFutureWatcher> // 用于监控后台任务

class NotePad : public QMainWindow
{
	Q_OBJECT

public:
	NotePad(QWidget* parent = nullptr);
	~NotePad();

private slots:
	// 基础功能
	void NewFile();
	void Open();
	void Save();
	void SaveAs();
	void Quit();
	void FindText();

	// 增强功能
	void SelectFont();

	// 一键清空按钮回调
	void ClearAll();

	// QtConcurrent 回调槽函数
	void OnFileLoaded();

private:
	// UI 创建辅助函数
	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateStatusBar();

	// 逻辑辅助
	void SaveFile(const QString& fileName);
	void FindNext();
	//void loadFile(const QString& fileName);

private:
	// 核心控件
	QTextEdit* textEdit;
	QString currentFile; // 记录当前打开的文件路径
	QPushButton* clearButton; // 一键清空按钮
	// 查找对话框的指针
	QDialog* findDialog = nullptr;
	QLineEdit* findLineEdit = nullptr;
	//QProgressBar* progressBar; // 进度条

	// 动作 (Actions)
	QAction* newAction;
	QAction* openAction;
	QAction* saveAction;
	QAction* saveAsAction;
	QAction* exitAction;
	QAction* fontAction;
	QAction* findAction;

	// 菜单
	QMenu* fileMenu;
	QMenu* editMenu;
	QMenu* formatMenu; // 格式菜单

	// 并发相关
	// QFutureWatcher 用于监听 QtConcurrent::run 的执行状态
	QFutureWatcher<QString> fileLoaderWatcher;
};