#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include "PlayerWidget.h" //播放器页面

//主界面

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();


protected: //重写事件
    void paintEvent(QPaintEvent *event);
private: //成员函数
    void loadTopButton();
    void loadTopLabels();
    void loadPlayerWidget();
private: //成员变量
    //因为会被别的成员函数调用，所以要创建成员变量
    QLabel* m_pTitleLabel; //歌名
    QLabel* m_pAuthorLabel; //歌手

    PlayerWidget* m_pPlayerWidget; //播放器页面
private slots:
    //更新歌曲信息的显示
    void onUpdateMusicMetaInformation(MusicInfo* info);
};
#endif // MAINWIDGET_H
