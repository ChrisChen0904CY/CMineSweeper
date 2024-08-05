#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// 各模式最小尺寸
#define level_1_minimal_size 300, 420
#define level_2_minimal_size 400, 426
#define level_3_minimal_size 600, 426

// 版本
#define CURRENT_VERSION "1.0.0"

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include <QActionGroup>
#include <QFileInfo>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>

#include "cpushbutton.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // 绑定所有槽函数
    void connects();
    // 初始化 Mode Actions
    void modeActionInit();
    // 初始化文件检查
    void iniCheck();
    // 初始化设置上一次设置的模式
    void modeInitialize();
    // 初始化按钮池
    void butsPoolInit();
    // 初始化所有按钮
    void butsInit(int width, int height);
    // 初始化地雷图
    void mapInit(int mines, int ii, int jj);
    // 翻开一个地块
    void openBox(int i, int j);
    // 显示一个地块
    void displayBox(int i, int j);
    // 游戏结束
    void gameOver();
    // 成功清掉所有雷
    void gameSuccess();
    // 判断是否成功
    bool isSuccess();
    // 雷数显示
    void minesNumDisplay();

private slots:
    // 模式切换
    void switch2Level1();
    void switch2Level2();
    void switch2Level3();
    // 按钮被点击的响应槽函数
    void clickSlot(int i, int j);
    // 标记按钮响应槽函数
    void flagSlot(int i, int j);
    // 计时器更新函数
    void onTimeOut();
    // 重新开始
    void restart();

private:
    Ui::MainWindow *ui;
    // 按钮对象池 --- 最多支持 30*30 地图
    QVector<QVector<CPushButton*>> butsPool;
    // 按钮矩阵
    QVector<QVector<CPushButton*>> buts;
    // 地雷图矩阵
    QVector<QVector<short>> minesMap;
    // 第一次点击
    bool firstClicked = false;
    // 模式切换 Action 组
    QActionGroup* modeActions = new QActionGroup(this);
    // 初始化配置文件
    QString iniFile = "./gameConfig.ini";
    // 按钮默认 QSS
    QString buttonQSS = R"(QPushButton {
    background-color: rgb(192, 192, 192);
    border: 1px solid rgb(128, 128, 128);
    border-top: 1px solid rgb(255, 255, 255);
    border-left: 1px solid rgb(255, 255, 255);
}

QPushButton:hover {
    background-color: rgb(222, 222, 222);
})";
    // 数字显示颜色
    QVector<QString> numFonts = {"QPushButton {color: rgb(0, 0, 255); font: 700 12pt;}",
                                 "QPushButton {color: rgb(0, 128, 0); font: 700 12pt;}",
                                 "QPushButton {color: rgb(255, 0, 0); font: 700 12pt;}",
                                 "QPushButton {color: rgb(0, 0, 128); font: 700 12pt;}",
                                 "QPushButton {color: rgb(128, 0, 0); font: 700 12pt;}",
                                 "QPushButton {color: rgb(0, 128, 128); font: 700 12pt;}",
                                 "QPushButton {color: rgb(0, 0, 0); font: 700 12pt;}",
                                 "QPushButton {color: rgb(129, 129, 129); font: 700 12pt;}"};
    // 被清扫后的按钮 QSS
    QString openQSS = "QPushButton {background-color: rgb(190, 190, 190); border: 1px solid rgb(141, 141, 141);}";
    // 计时器
    QTimer* timer = nullptr;
    // 当前时间
    int currentTime = 0;
    // 雷数
    int currentMinesNum = 0;
    int minesNum = 0;
};
#endif // MAINWINDOW_H
