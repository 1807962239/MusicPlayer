#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>

#include "CCVec2f.h" //将点类包含进来
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include "MusicSlider.h" //包含进度条类
#include "VolumeChangeSlider.h" //包含音量条
#include <QTimer>
#include <vlc/vlc.h> //导入libvlc库
#include <QDragEnterEvent>
#include <QDropEvent>

#define MATH_PI 3.1415926f //定义一个常数pi

//枚举播放器的播放状态
enum MusicPlayerStatus
{
    MUSICPLAYER_STATUS_PLAY,
    MUSICPLAYER_STATUS_PAUSE,
    MUSICPLAYER_STATUS_STOPED
};

//歌曲信息结构体
typedef struct musicInformation
{
    QString musicTitle;
    QString musicAuthor;
}MusicInfo;

//核心播放功能界面

class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    virtual ~PlayerWidget();

private: //成员函数
    bool isPointInRect(float x,float y); //判断鼠标是否在唱针头的四边形中
    float caculateMouseMoveAngle(float x,float y); //计算鼠标移动的角度
    void updateCurrentStylusPosition(); //更新唱针头顶点坐标
    void loadWidget(); //初始化页面其他控件
    void initMusicPlayerInstance(); //初始化libvlc播放器实例
    void setMusicInformation(MusicInfo* info); //设定歌曲信息
    void playMusicOnVlcMediaPlayerEngine(); //用vlc播放音乐
    void resetPlayStatusOnNewFile(); //导入新音乐时刷新状态
    void setAndUpdateStylusAngle(float aValue); //设置更新唱针旋转角度
    void resetAllWhileMusicPlayEnded(); //一曲放完后重置状态

protected: //重写事件
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private: //成员变量
    //已知[唱针的轴与唱针四个端点的连线]与垂直线的初始角度，将此角度转化为弧度
    float m_angleA=15.0f*MATH_PI/180;
    float m_angleB=11.0f*MATH_PI/180;
    float m_angleC=6.0f*MATH_PI/180;
    float m_angleD=11.0f*MATH_PI/180;

    //已知唱针的轴与四个端点的连线的长度（固定值）
    float m_lengthA=357.0f;
    float m_lengthB=370.0f;
    float m_lengthC=318.0f;
    float m_lengthD=302.0f;

    //记录四个端点的初始坐标
    CCVec2f m_posA;
    CCVec2f m_posB;
    CCVec2f m_posC;
    CCVec2f m_posD;

    //记录四个端点的当前坐标
    CCVec2f m_posACur;
    CCVec2f m_posBCur;
    CCVec2f m_posCCur;
    CCVec2f m_posDCur;

    //记录唱针旋转的角度
    float m_stylusAngle=0.0f;
    //记录唱片旋转角度
    float m_rotateAngle=0.0f;

    //记录鼠标点击时的位置
    QPoint m_lastMousePos=QPoint(0.0f,0.0f);

    //记录鼠标移动的角度
    float m_mouseMoveAngle=0.0f;
    //记录鼠标松开时的角度
    float m_mouseLastAngle=0.0f;
    //记录鼠标最后一次在正确范围的移动角度(目前还不知道实际作用)
    float m_mouseOverAngle=0.0f;

    //记录鼠标是否在唱针头的四边形区间内
    bool m_bMouseInStylusRect=false;
    //（目前还不知道作用）
    bool m_bOverAngle=false;
    //唱针是否在手动移动
    bool m_bStylusMoving=false;

    //当前播放的时间进度
    QLabel* m_pCurrentTimeLabel;
    //剩余时间
    QLabel* m_pAllTimeLabel;

    //上一首按钮
    QPushButton* m_pPreMusicButton;
    //下一首按钮
    QPushButton* m_pNextMusicButton;
    //播放按钮
    QPushButton* m_pPlayMusicButton;

    //进度条
    MusicSlider* m_pMusicSlider;

    //音量条
    VolumeChangeSlider* m_pVolumeChangeSlider;

    //定时器
    QTimer* m_pMusicPlayTimer;
    bool m_bTimerPlaying=false;

    //播放器状态枚举对象
    MusicPlayerStatus m_eMusicPlayerStatus;

    //libvlc相关对象
    //实例
    libvlc_instance_t* m_pMusicPlayerInstance;
    //播放器
    libvlc_media_player_t* m_pVlcMediaPlayer;


signals:
    //放置或切换歌曲时的更新歌曲信息信号
    void updateMusicMetaInformation(MusicInfo* info);
private slots:
    //播放按钮点击
    void onPlayMusicButtonClicked();
    //定时器运行
    void onMusicTimerProcess();
    //音量条改变
    void onVolumeSliderValueChanged(int value);
    //进度条改变
    void onMusicSeekValueChanged(int value);

};

#endif // PLAYERWIDGET_H
