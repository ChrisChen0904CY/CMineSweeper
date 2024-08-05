#ifndef QUTILS_H
#define QUTILS_H

#include <QWidget>
#include <QRegularExpression>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QDebug>
#include <QVector>
#include <QLabel>
#include <QTimer>
#include <algorithm>
#include <random>

// 解析某一个具体的样式
QMap<QString, QString> parseStyleSettings(const QString &style);
// 解析整个样式表
QMap<QString, QMap<QString, QString>> parseStyleSheet(const QString &style);
// 根据 QSS 解析表构建 QSS 字符串
QString generateQSS(QMap<QString, QMap<QString, QString>> styleMap);
// 在控件原本的 QSS 基础上 更新其 QSS
void updateStyleSheet(QWidget* item, QString new_style);
// 给一个 QLabel 设置图片背景进行展示
void setupLabel(QLabel* label, QString path);
// 将 Hex 颜色码转为 RGB 编码
QVector<int> hex2rgb(QString hexColor);

// 将二维 QVector 扁平化为一维 QVector
template<typename T>
QVector<T> flatten(const QVector<QVector<T>>& vec) {
    QVector<T> result;
    for (const auto& innerVec : vec) {
        result += innerVec;
    }
    return result;
}

// 移除指定元素
template<typename T>
void removeElement(QVector<T>& vec, const T& element) {
    vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
}

// 随机选择 n 个元素
template<typename T>
QVector<T> selectRandomElements(QVector<T>& vec, int n) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vec.begin(), vec.end(), g);
    return vec.mid(0, n);
}

#endif // QUTILS_H
