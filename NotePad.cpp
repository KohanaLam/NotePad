#include "NotePad.h"


NotePad::NotePad(QWidget* parent) // done
	: QMainWindow(parent)
{
	resize(800, 600);
	setWindowTitle("Super NotePad");

	// 1. 初始化 UI 组件
	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();

	// 2. 设置中心部件与布局
	QWidget* container = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(container);

	textEdit = new QTextEdit();
	// 设置字体稍微大一点
	textEdit->setFont(QFont("Consolas", 12));

	layout->addWidget(textEdit);

	QHBoxLayout* buttonLayout = new QHBoxLayout();

	// 弹簧，在左边，把右边的按钮推向最右侧
	buttonLayout->addStretch();

	clearButton = new QPushButton("Clear", this);
	clearButton->setFixedSize(60, 35); // 固定大小，使其不变化
	clearButton->setStyleSheet(
		// 1. 正常状态：浅粉色
		"QPushButton {"
		"  background-color: #FFB6C1;"  // LightPink (浅粉色)
		"  color: white;"               // 白色文字
		"  border-radius: 8px;"        // 圆角大一点更可爱
		"  font-family: 'Microsoft YaHei';"
		"  font-size: 16px;"
		"  font-weight: bold;"
		"}"

		// 2. 鼠标悬停状态：颜色稍微加深
		"QPushButton:hover {"
		"  background-color: #FF69B4;"  // HotPink (热粉色)
		"}"

		// 3. 鼠标按下状态：更有点击感
		"QPushButton:pressed {"
		"  background-color: #C71585;"  // MediumVioletRed (深紫罗兰红)
		"  padding-left: 5px;"          // 点击时文字位移，模拟按下效果
		"  padding-top: 5px;"
		"}"
	);
	buttonLayout->addWidget(clearButton);
	layout->addLayout(buttonLayout);

	// --- 重点：Layout 的 Margin (布局内边距) ---
	// 这会让 textEdit 不会紧贴着窗口边缘，留出 10px 的呼吸空间
	layout->setContentsMargins(15, 15, 15, 15);
	layout->setSpacing(10);

	setCentralWidget(container);

	//连接清空按钮和槽
	connect(clearButton, &QPushButton::clicked, this, &NotePad::ClearAll);

	// 3. 连接并发监视器的信号
	// 当后台线程读完文件，watcher 会发出 finished 信号，我们连接到 onFileLoaded 更新 UI
	connect(&fileLoaderWatcher, &QFutureWatcher<QString>::finished, this, &NotePad::OnFileLoaded);
}

NotePad::~NotePad() {}


void NotePad::CreateActions()
{
	// --- 修改点：使用 style()->standardIcon 获取系统内置图标 ---

	// 1. 新建 (New) -> 使用“文件”图标
	const QIcon newIcon = style()->standardIcon(QStyle::SP_FileIcon);
	newAction = new QAction(newIcon, "&New", this);
	newAction->setShortcut(QKeySequence::New);
	connect(newAction, &QAction::triggered, this, &NotePad::NewFile);

	// 2. 打开 (Open) -> 使用“打开文件夹”图标
	const QIcon openIcon = style()->standardIcon(QStyle::SP_DirOpenIcon);
	openAction = new QAction(openIcon, "&Open...", this);
	openAction->setShortcut(QKeySequence::Open);
	connect(openAction, &QAction::triggered, this, &NotePad::Open);

	// 3. 保存 (Save) -> 使用“保存”图标 (通常是软盘)
	const QIcon saveIcon = style()->standardIcon(QStyle::SP_DialogSaveButton);
	saveAction = new QAction(saveIcon, "&Save", this);
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, &QAction::triggered, this, &NotePad::Save);

	saveAsAction = new QAction("Save &As...", this);
	connect(saveAsAction, &QAction::triggered, this, &NotePad::SaveAs);

	const QIcon exitIcon = style()->standardIcon(QStyle::SP_DialogCloseButton);
	exitAction = new QAction(exitIcon, "&Exit", this);
	exitAction->setShortcut(QKeySequence::Quit);
	connect(exitAction, &QAction::triggered, this, &NotePad::Quit);

	// 查找
	findAction = new QAction("&Find", this);
	findAction->setShortcut(QKeySequence::Find);
	connect(findAction, &QAction::triggered, this, &NotePad::FindText);

	fontAction = new QAction("Select &Font...", this);
	connect(fontAction, &QAction::triggered, this, &NotePad::SelectFont);
}

void NotePad::CreateMenus() // done
{
	fileMenu = menuBar()->addMenu("&File");
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
	fileMenu->addAction(saveAction);
	fileMenu->addAction(saveAsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	editMenu = menuBar()->addMenu("&Edit");
	// QTextEdit 自带了 undo/redo/copy/paste 功能，可以直接利用
	editMenu->addAction("Undo", textEdit, &QTextEdit::undo, QKeySequence::Undo);
	editMenu->addAction("Redo", textEdit, &QTextEdit::redo, QKeySequence::Redo);
	editMenu->addSeparator();
	editMenu->addAction("Cut", textEdit, &QTextEdit::cut, QKeySequence::Cut);
	editMenu->addAction("Copy", textEdit, &QTextEdit::copy, QKeySequence::Copy);
	editMenu->addAction("Paste", textEdit, &QTextEdit::paste, QKeySequence::Paste);
	editMenu->addSeparator();
	editMenu->addAction(findAction);

	formatMenu = menuBar()->addMenu("F&ormat");
	formatMenu->addAction(fontAction);
}

// 加工具栏，带图标
void NotePad::CreateToolBars() //done
{
	QToolBar* fileToolBar = addToolBar("File");
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);
	fileToolBar->addAction(exitAction);
}

void NotePad::CreateStatusBar() //done
{
	statusBar()->showMessage("Ready");

	//progressBar = new QProgressBar(this);
	//progressBar->setMaximumHeight(15); // 让它瘦一点
	//progressBar->setMaximumWidth(200); // 限制宽度

	// 默认隐藏
	//progressBar->hide();

	// addPermanentWidget 会将控件添加到状态栏的最右侧
	//statusBar()->addPermanentWidget(progressBar); // 让进度条在状态栏中持续可见，不会被临时消息覆盖。
}

void NotePad::NewFile() //done
{
	currentFile.clear();
	textEdit->clear();
	setWindowTitle("Untitled - Super NotePad");
}

// 核心：使用 QtConcurrent 读取文件
void NotePad::Open() // done
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
	if (fileName.isEmpty()) return;

	statusBar()->showMessage("Loading file in background...");
	textEdit->setEnabled(false); // 加载时禁止编辑

	// --- 新增：显示并设置进度条为“忙碌模式” ---
	//progressBar->show();
	//progressBar->setRange(0, 0); // 设置范围为 0-0，进度条会变成来回滚动的动画

	// 定义一个 Lambda 表达式（或者独立函数）在后台线程运行
	// 注意：后台线程不能操作 UI (如不能调用 textEdit->setText)
	QFuture<QString> future = QtConcurrent::run([fileName]() -> QString {
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return QString();
		}
		QTextStream in(&file);
		// 模拟大文件耗时 (可选，仅为了让你看到 Loading 效果)
		// QThread::sleep(1);
		return in.readAll();
		});

	// 将 Future 交给 Watcher
	currentFile = fileName;
	fileLoaderWatcher.setFuture(future);

	// 代码继续向下执行，主界面不会卡死
}

// --- 核心：读取完成后的回调 ---
void NotePad::OnFileLoaded() //done
{
	// 获取后台线程的结果
	QString content = fileLoaderWatcher.result();

	textEdit->setPlainText(content);
	textEdit->setEnabled(true);
	//progressBar->hide(); // 读完了就隐藏进度条
	setWindowTitle(currentFile + " - Super NotePad");
	statusBar()->showMessage("File Loaded", 2000);
}

void NotePad::Save() //done
{
	if (currentFile.isEmpty()) {
		SaveAs();
	}
	else {
		SaveFile(currentFile);
	}
}

void NotePad::SaveAs() //done
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
	if (!fileName.isEmpty()) {
		SaveFile(fileName);
	}
}

void NotePad::SaveFile(const QString& fileName) // done
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(this, "Error", "Cannot write file: " + file.errorString());
		return;
	}
	QTextStream out(&file);
	out << textEdit->toPlainText();
	file.close();

	currentFile = fileName;
	setWindowTitle(currentFile + " - Super NotePad");
	statusBar()->showMessage("File Saved", 2000);
}

void NotePad::SelectFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
	if (ok) {
		textEdit->setFont(font);
	}
}

void NotePad::ClearAll() // done
{
	if (textEdit->toPlainText().isEmpty()) return;

	QMessageBox messageBox;
	messageBox.setWindowTitle("clear the content");
	messageBox.setText("Are you sure you want to discard unsaved changes?");
	messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	messageBox.setDefaultButton(QMessageBox::No);
	if (messageBox.exec() == QMessageBox::Yes)
	{
		textEdit->clear();
		statusBar()->showMessage("content are all clear.");
	}
}

void NotePad::Quit() // done
{
	// 是否保存的逻辑
	QMessageBox messageBox;
	messageBox.setWindowTitle("Notepad");
	messageBox.setText("Do you want to save?");
	messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	messageBox.setDefaultButton(QMessageBox::No);
	if (messageBox.exec() == QMessageBox::Yes)
		Save();

	// 退出
	QApplication::quit();
}

// NotePad.cpp

// 1. 打开/创建查找对话框
void NotePad::FindText()
{
	// 如果对话框已经存在，直接显示并置顶，不再重复创建
	if (findDialog) {
		findDialog->show();
		findDialog->raise(); // 把窗口拉到最前面
		findDialog->activateWindow(); // 激活窗口焦点
		findLineEdit->setFocus();     // 让光标自动跳进输入框
		findLineEdit->selectAll();    // 选中已有文字，方便直接修改
		return;
	}

	// --- 第一次创建对话框 ---
	findDialog = new QDialog(this);
	findDialog->setWindowTitle("Find");

	// 创建布局和控件
	QVBoxLayout* layout = new QVBoxLayout(findDialog);
	findLineEdit = new QLineEdit(findDialog);
	QPushButton* findBtn = new QPushButton("Find Next", findDialog);

	layout->addWidget(findLineEdit);
	layout->addWidget(findBtn);

	// --- 关键点：连接信号 ---

	// 情况A: 点击按钮 -> 查找
	connect(findBtn, &QPushButton::clicked, this, &NotePad::FindNext);

	// 情况B: 在输入框里按回车 (returnPressed) -> 查找
	connect(findLineEdit, &QLineEdit::returnPressed, this, &NotePad::FindNext);

	// 显示对话框（使用 show() 而不是 exec()，这样它是非模态的，不会卡住主界面）
	findDialog->show();
}

// 2. 执行具体的查找逻辑
void NotePad::FindNext()
{
	QString term = findLineEdit->text(); // text()是获取这个QLineEdit里的文本内容
	if (term.isEmpty()) return;

	// 调用 textEdit 的查找
	bool found = textEdit->find(term);

	// 如果没找到
	if (!found) {
		// 尝试把光标移到开头再找一次（循环查找）
		textEdit->moveCursor(QTextCursor::Start);
		found = textEdit->find(term);

		if (!found) {
			QMessageBox::information(this, "Find", "Cannot find \"" + term + "\"");
		}
	}
}