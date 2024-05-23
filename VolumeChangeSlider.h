#ifndef VOLUMECHANGESLIDER_H
#define VOLUMECHANGESLIDER_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>

//自定义音量条

class VolumeSliderHandle: public QWidget
{
    Q_OBJECT
public :
    explicit VolumeSliderHandle(QWidget* parent=0);


private:
    QPoint  lastMousePosition;

signals:
    void  volumeSliderHandleMoveSignal(int posX);
    void  volumeSliderHandleMoveDoneSignal();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void paintEvent(QPaintEvent* event);

};

class VolumeChangeSlider : public QWidget
{
    Q_OBJECT
public:
    explicit VolumeChangeSlider(QWidget *parent = nullptr);


    //非鼠标拖拽设置音量
    void    SetVolumeSliderProgressValue(int value);

private:
    int                  m_currentPosX=0;
    QPoint               m_originalPosition;

    VolumeSliderHandle*   m_pVolumeChangeHandle;

signals:
    void  updateMusicVolumeValue(int value);

private slots:

    void onVolumeSliderHandleMove(int posX);
    void onVolumeSliderHandleMoveDone();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);

};

#endif // VOLUMECHANGESLIDER_H
