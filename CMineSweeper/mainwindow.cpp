#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qutils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(this->windowTitle()+" v"+CURRENT_VERSION);
    this->ui->pushButton->setStyleSheet(this->buttonQSS);
    // 按钮池初始化
    butsPoolInit();
    // 绑定所有槽函数
    connects();
    // 初始化
    modeActionInit();
    iniCheck();
    modeInitialize();
    // 初始化计时器显示
    this->ui->lcdNumber_2->display("000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 绑定所有槽函数
void MainWindow::connects() {
    // 模式切换
    connect(this->ui->action9_9,
            &QAction::triggered,
            this,
            &MainWindow::switch2Level1);
    connect(this->ui->action16_16,
            &QAction::triggered,
            this,
            &MainWindow::switch2Level2);
    connect(this->ui->action16_30,
            &QAction::triggered,
            this,
            &MainWindow::switch2Level3);
    // 重开
    connect(this->ui->pushButton,
            &QPushButton::clicked,
            this,
            &MainWindow::restart);
}

// 模式切换
void MainWindow::switch2Level1() {
    // Disable updates
    this->setUpdatesEnabled(false);
    // 初始化所有按钮
    butsInit(9, 9);
    // Enable updates
    this->setUpdatesEnabled(true);
    // 设置窗口大小
    this->setMinimumSize(level_1_minimal_size);
    this->resize(level_1_minimal_size);
    // 显示雷数
    minesNum = 10;
    currentMinesNum = 10;
    this->ui->lcdNumber->display("0"+QString::number(minesNum));
    restart();
}

void MainWindow::switch2Level2() {
    // Disable updates
    this->setUpdatesEnabled(false);
    // 初始化所有按钮
    butsInit(16, 16);
    // Enable updates
    this->setUpdatesEnabled(true);
    // 设置窗口大小
    this->setMinimumSize(level_2_minimal_size);
    this->resize(level_2_minimal_size);
    // 显示雷数
    minesNum = 40;
    currentMinesNum = 40;
    this->ui->lcdNumber->display("0"+QString::number(minesNum));
    restart();
}

void MainWindow::switch2Level3() {
    // Disable updates
    this->setUpdatesEnabled(false);
    // 初始化所有按钮
    butsInit(16, 30);
    // Enable updates
    this->setUpdatesEnabled(true);
    // 设置窗口大小
    this->setMinimumSize(level_3_minimal_size);
    this->resize(level_3_minimal_size);
    // 显示雷数
    minesNum = 99;
    currentMinesNum = 99;
    this->ui->lcdNumber->display("0"+QString::number(minesNum));
    restart();
}

// 初始化 Mode Actions
void MainWindow::modeActionInit() {
    this->modeActions->addAction(this->ui->action9_9);
    this->modeActions->addAction(this->ui->action16_16);
    this->modeActions->addAction(this->ui->action16_30);
    this->modeActions->addAction(this->ui->actionDIY);
    this->modeActions->setExclusive(true);
}

// 检查初始化配置文件
void MainWindow::iniCheck() {
    QFileInfo int_file(this->iniFile);
    // 不存在就新建一个默认的ini文件
    if (!int_file.isFile()) {
        // 打开文件并指定为ini格式
        QSettings*  m_IniFile = new QSettings(this->iniFile, QSettings::IniFormat);
        //通过setValue函数将键值对放在相对于的节下面
        m_IniFile->setValue("PreSettings/mode", "9_9");
        delete m_IniFile;
    }
}

// 初始化设置上一次设置的模式
void MainWindow::modeInitialize() {
    // 打开文件并指定为ini格式
    QSettings*  m_IniFile = new QSettings(this->iniFile, QSettings::IniFormat);
    // 通过Value函数将节下相对应的键值读取出来
    QString pre_mode = "9_9";

    try {
        pre_mode = m_IniFile->value("PreSettings/mode").toString();
    }
    catch(const char* msg) {
        qDebug() << msg;
        switch2Level1();
    }

    // 调整对应模式
    if (pre_mode == "9_9") {
        this->ui->action9_9->setChecked(true);
        switch2Level1();
    }
    else if (pre_mode == "16_16") {
        this->ui->action16_16->setChecked(true);
        switch2Level2();
    }
    else if (pre_mode == "16_30") {
        this->ui->action16_30->setChecked(true);
        switch2Level3();
    }
    else {
        this->ui->actionDIY->setChecked(true);
    }
}

// 初始化按钮池
void MainWindow::butsPoolInit() {
    for (auto i = 0; i < 30; i++) {
        QVector<CPushButton*> tmp;
        for (auto j = 0; j < 30; j++) {
            CPushButton* but = new CPushButton(this);
            but->setCursor(Qt::PointingHandCursor);
            but->setStyleSheet(this->buttonQSS);
            but->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            // 插入窗口布局来显示
            this->ui->gridLayout->addWidget(but, i, j);
            // 设置不可见
            but->setVisible(false);
            // 绑定点击槽函数
            connect(but, &CPushButton::clicked, [this, i, j]() {
                this->clickSlot(i, j);
            });
            // 绑定反敲槽函数
            connect(but, &CPushButton::rightClicked, [this, i, j]() {
                this->flagSlot(i, j);
            });
            // 插入 Vector 以便统一管理
            tmp.push_back(but);
        }
        this->butsPool.push_back(tmp);
    }
}

// 初始化所有按钮
void MainWindow::butsInit(int width, int height) {
    this->ui->lcdNumber_2->display(0);
    currentTime = 0;
    // 删除已有计时器
    if (timer != nullptr) {
        timer->stop();
        timer = nullptr;
    }
    // 原本的按钮设置为不可见
    for (auto &r:buts) {
        for (auto &b:r) {
            b->setVisible(false);
        }
    }
    // 清空地雷图
    this->minesMap.clear();
    // 清理原来的容器
    this->buts.clear();
    // 构建新的
    for (auto i = 0; i < width; i++) {
        QVector<CPushButton*> tmp;
        for (auto j = 0; j < height; j++) {
            CPushButton* but = butsPool[i][j];
            // 设置可见
            but->setVisible(true);
            // 使能按钮
            but->setEnabled(true);
            but->setStyleSheet(this->buttonQSS);
            // 插入 Vector 以便统一管理
            tmp.push_back(but);
        }
        this->buts.push_back(tmp);
    }
    // 复位点击标志
    firstClicked = false;
}

// 初始化地雷图
void MainWindow::mapInit(int mines, int ii, int jj) {
    minesMap.clear();
    // 生成位置矩阵
    QVector<QVector<int>> posMat;
    for (auto i = 0; i < this->buts.size(); i++) {
        QVector<int> tmp;
        QVector<short> minesTmp;
        for (auto j = 0; j < this->buts[0].size(); j++) {
            tmp.push_back(i*100+j);
            minesTmp.push_back(0);
        }
        posMat.push_back(tmp);
        minesMap.push_back(minesTmp);
    }
    // 随机生成雷的位置
    srand(QTime::currentTime().msec());
    QVector<int> flatVec = flatten(posMat);
    removeElement(flatVec, ii*100+jj);
    QVector<int> randomElements = selectRandomElements(flatVec, mines);
    // 生成雷图
    for (auto pos:randomElements) {
        int x = int(pos/100);
        int y = pos%100;
        minesMap[x][y] = -1;
        // 影响周围的8个地块
        for (auto t = -1; t <= 1; t++) {
            for (auto tt = -1; tt <= 1; tt++) {
                // 跳过自己减少不必要操作
                if (t == 0 && tt == 0) {
                    continue;
                }
                // 如果存在且非雷则影响
                if (0 <= x+t && x+t < minesMap.size()) {
                    if (0 <= y+tt && y+tt < minesMap[0].size()) {
                        if (minesMap[x+t][y+tt] != -1) {
                            minesMap[x+t][y+tt]++;
                        }
                    }
                }
            }
        }
    }
}

// 按钮被点击的响应槽函数
void MainWindow::clickSlot(int i, int j) {
    // 第一次点击则设置点击标志位 并开始计时
    if (!firstClicked) {
        firstClicked = true;
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
        timer->start(1000);
        // 初始化雷图
        mapInit(minesNum, i, j);
    }
    // 看看是不是翻到雷了
    if (this->minesMap[i][j] == -1) {
        gameOver();
        return;
    }
    else if (this->minesMap[i][j] >= 0) {
        displayBox(i, j);
    }
    // 翻开
    openBox(i, j);
    // 递归完统一重绘
    this->setUpdatesEnabled(true);
    // 点击完判断是不是赢了
    if (isSuccess()) {
        gameSuccess();
    }
}

// 计时器更新函数
void MainWindow::onTimeOut() {
    currentTime++;
    if (currentTime < 10) {
        this->ui->lcdNumber_2->display("00"+QString::number(currentTime));
    }
    else if (currentTime < 100) {
        this->ui->lcdNumber_2->display("0"+QString::number(currentTime));
    }
    else if (currentTime < 1000) {
        this->ui->lcdNumber_2->display(QString::number(currentTime));
    }
}

// 重新开始
void MainWindow::restart() {
    // 重开按钮改回笑脸
    this->ui->pushButton->setText("🙂");
    // 复原标志位 清除当前计时器
    this->firstClicked = false;
    currentTime = 0;
    this->ui->lcdNumber_2->display("000");
    // 删除已有计时器
    if (timer != nullptr) {
        timer->stop();
        timer = nullptr;
    }
    // 复原当前雷数
    currentMinesNum = minesNum;
    this->ui->lcdNumber->display("0"+QString::number(currentMinesNum));
    // 还原所有按钮状态
    for (auto &r:this->buts) {
        for (auto &b:r) {
            b->setEnabled(true);
            b->setStyleSheet(this->buttonQSS);
            b->setText("");
        }
    }
    // 复原雷图
    minesMap.clear();
}

// 标记按钮响应槽函数
void MainWindow::flagSlot(int i, int j) {
    this->setUpdatesEnabled(false);
    // 第一次点击则设置点击标志位 并开始计时
    if (!firstClicked) {
        firstClicked = true;
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
        timer->start(1000);
        // 初始化雷图
        mapInit(minesNum, i, j);
    }
    QSize originalSize = buts[i][j]->size();
    // 插旗
    if (this->buts[i][j]->text() == "") {
        // 如果雷数已经是零了就拒绝这次操作
        if (currentMinesNum == 0) {
            this->setUpdatesEnabled(true);
            return;
        }
        currentMinesNum--;
        this->minesNumDisplay();
        this->buts[i][j]->setText("🚩");
    }
    // 问号
    else if (this->buts[i][j]->text() == "🚩") {
        // 还原一个雷数
        currentMinesNum++;
        this->minesNumDisplay();
        this->buts[i][j]->setText("❓");
    }
    // 还原
    else {
        this->buts[i][j]->setText("");
    }
    buts[i][j]->resize(originalSize);
    this->setUpdatesEnabled(true);
}

// 翻开一个地块
void MainWindow::openBox(int i, int j) {
    this->setUpdatesEnabled(false);
    // 翻到雷就return不管
    if (this->minesMap[i][j] == -1) {
        return;
    }
    // 否则递归显示
    // 影响周围的8个地块
    for (auto t = -1; t <= 1; t++) {
        for (auto tt = -1; tt <= 1; tt++) {
            // 跳过自己防止递归堆栈溢出
            if (t == 0 && tt == 0) {
                continue;
            }
            // 如果存在且为空则递归
            if (0 <= i+t && i+t < minesMap.size()) {
                if (0 <= j+tt && j+tt < minesMap[0].size()) {
                    if (this->minesMap[i+t][j+tt] == 0) {
                        // 如果没翻开过则翻开 防止走回头路
                        if (this->buts[i+t][j+tt]->isEnabled()) {
                            displayBox(i+t, j+tt);
                            openBox(i+t, j+tt);
                        }
                    }
                    // 非空非雷直接显示
                    else if (this->minesMap[i+t][j+tt] > 0) {
                        // 显示地块
                        displayBox(i+t, j+tt);
                    }
                }
            }
        }
    }
}

// 显示一个地块
void MainWindow::displayBox(int i, int j) {
    if (!this->buts[i][j]->isEnabled()) {
        return;
    }
    this->buts[i][j]->setStyleSheet(this->openQSS);
    // 雷
    if (this->minesMap[i][j] == -1) {
        this->buts[i][j]->setText("💣");
    }
    // 空
    else if (this->minesMap[i][j] == 0) {
        this->buts[i][j]->setText("");
    }
    // 不是雷
    else {
        updateStyleSheet(this->buts[i][j], numFonts[this->minesMap[i][j]-1]);
        this->buts[i][j]->setText(QString::number(this->minesMap[i][j]));
    }
    this->buts[i][j]->setEnabled(false);
}

// 游戏结束
void MainWindow::gameOver() {
    // 重开按钮改到哭脸
    this->ui->pushButton->setText("😭");
    // 停下当前计时器
    if (timer != nullptr) {
        timer->stop();
    }
    QMessageBox::critical(this, "翻到雷啦！",
                          "丸辣！这是雷！！！");
    // 显示所有块块
    for (auto i = 0; i < this->buts.size(); i++) {
        for (auto j = 0; j < this->buts[0].size(); j++) {
            displayBox(i, j);
        }
    }
    return;
}

// 成功清掉所有雷
void MainWindow::gameSuccess() {
    // 重开按钮改到酷脸
    this->ui->pushButton->setText("😎");
    // 停下当前计时器
    if (timer != nullptr) {
        timer->stop();
    }
    // 显示所有块块
    for (auto i = 0; i < this->buts.size(); i++) {
        for (auto j = 0; j < this->buts[0].size(); j++) {
            displayBox(i, j);
        }
    }
    QMessageBox::information(this, "成啦！",
                          "通关力！！！");
    return;
}

// 判断是否成功
bool MainWindow::isSuccess() {
    for (auto i = 0; i < this->buts.size(); i++) {
        for (auto j = 0; j < this->buts[0].size(); j++) {
            if (this->buts[i][j]->isEnabled() && this->minesMap[i][j] != -1) {
                return false;
            }
        }
    }
    return true;
}

// 雷数显示
void MainWindow::minesNumDisplay() {
    if (currentMinesNum < 10) {
        this->ui->lcdNumber->display("00"+QString::number(currentMinesNum));
    }
    else if (currentMinesNum < 100) {
        this->ui->lcdNumber->display("0"+QString::number(currentMinesNum));
    }
    else {
        this->ui->lcdNumber->display(QString::number(currentMinesNum));
    }
}
