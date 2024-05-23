#ifndef MUSICSLIDER_H
#define MUSICSLIDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>


//自定义进度条控件

//把手部分
class MusicSliderHandle : public QWidget
{
    Q_OBJECT
public:
    explicit MusicSliderHandle(QWidget *parent = nullptr);

private: //成员变量
    //鼠标点击位置
    QPoint m_lastMousePosition;

signals:
    //进度条拉动
    void musicSliderHandleMoveSignal(int posX);
    //进度条拉动结束
    void musicSliderHandleMoveDoneSignal();

protected: //事件重写
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);
};

//主体部分
class MusicSlider : public QWidget
{
    Q_OBJECT
public:
    explicit MusicSlider(QWidget *parent = nullptr);

    //成员函数
    //非手动设置进度条
    void setUpMusicSliderValue(int aValue);

private: //成员变量
    //当前进度条位置
    int m_currentPosX=0;
    //初始进度条坐标 用来在进度条移动时设定原始的纵坐标
    QPoint m_originalPosition;
    //进度条把手
    MusicSliderHandle* m_pMusicSliderHandle;
    //进度条是否正在被手动移动
    bool m_bHandleMoving=false;

private slots:
    void onSliderHandleMove(int posX);
    void onSliderHandleMoveDone();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);

signals:
    //进度条的值更新
    void updateMusicSeekValue(int value);
};

#endif // MUSICSLIDER_H
