#include "VolumeChangeSlider.h"
#include <QPainter>
#include <QDebug>

#define VOLUMESLIDERHANDLE_WIDTH   31
#define VOLUMESLIDERHANDLE_HEIGHT    48

VolumeSliderHandle::VolumeSliderHandle(QWidget* parent):QWidget(parent)
{
    setGeometry(0,0,VOLUMESLIDERHANDLE_WIDTH,VOLUMESLIDERHANDLE_HEIGHT);
}


void VolumeSliderHandle::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        lastMousePosition = event->globalPosition().toPoint();
    }
}

void VolumeSliderHandle::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        //通过点击和当前的屏幕坐标获取移动距离
        const QPoint position =  event->globalPosition().toPoint() - lastMousePosition;
        emit volumeSliderHandleMoveSignal(position.x());

    }
}

void VolumeSliderHandle::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit volumeSliderHandleMoveDoneSignal();
}
void VolumeSliderHandle::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform );

    painter.save();
    painter.drawPixmap(rect(),QPixmap(":/images/Resources/volumeButton.png"));
    painter.restore();

}


#define VOLUMESEEKSLIDER_WIDTH   414
#define VOLUMESEEKSLIDER_HEIGHT  45
#define MUSICSCREEN_HEIGHT     756

VolumeChangeSlider::VolumeChangeSlider(QWidget *parent) : QWidget(parent)
{
    setGeometry(38,MUSICSCREEN_HEIGHT-79,VOLUMESEEKSLIDER_WIDTH,VOLUMESEEKSLIDER_HEIGHT+5);
    //在音量条控件中初始化把手控件
    m_pVolumeChangeHandle= new VolumeSliderHandle(this);
    connect(m_pVolumeChangeHandle,SIGNAL(volumeSliderHandleMoveSignal(int)),this,SLOT(onVolumeSliderHandleMove(int)));
    connect(m_pVolumeChangeHandle,SIGNAL(volumeSliderHandleMoveDoneSignal()),this,SLOT(onVolumeSliderHandleMoveDone()));



    //用把手位置初始化初始位置
    m_originalPosition = m_pVolumeChangeHandle->pos();
    m_currentPosX = m_originalPosition.rx();
}

void VolumeChangeSlider::SetVolumeSliderProgressValue(int aValue)
{
    float realValue = aValue *3.84;
     m_pVolumeChangeHandle->move((int)realValue,m_pVolumeChangeHandle->pos().y());
     m_currentPosX = realValue;

     //音量条改变时修改音量
     emit updateMusicVolumeValue(aValue);
}
void VolumeChangeSlider::onVolumeSliderHandleMove(int aValue)
{
    if(m_pVolumeChangeHandle == NULL){
        return;
    }

    //新的x坐标为当前坐标+移动坐标
    int realPosX= m_currentPosX + aValue;

    if((realPosX <= 384) && (realPosX >= 0))
    {
        //横向移动，纵向保持初始高度
         m_pVolumeChangeHandle->move(realPosX,m_originalPosition.ry());

         update();
    }

}
void VolumeChangeSlider::onVolumeSliderHandleMoveDone()
{
    if(m_pVolumeChangeHandle == NULL){
        return;
    }

    m_currentPosX = m_pVolumeChangeHandle->pos().x();

    //将坐标数据转化为音量数据
    //这里384必须加小数点，不然整数/整数结果会四舍五入为0
    float volumeValue =m_currentPosX / 384.0 * 100;
    emit updateMusicVolumeValue((int)volumeValue);
    qDebug()<<"CURRENT0:"<<(int)volumeValue;
}
void VolumeChangeSlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform );

    painter.save();
    painter.drawPixmap(QRect(0,0,VOLUMESEEKSLIDER_WIDTH,VOLUMESEEKSLIDER_HEIGHT),QPixmap(":/images/Resources/volumeBackground.png"));
    painter.restore();

    painter.save();
    painter.setPen(QPen(QColor(138, 139, 143), 1));
    painter.setBrush(QColor(138, 139, 143));
    //画平滑的矩形在音量条开始位置
    painter.drawRoundedRect(13, 16, 14,14,2,2);
    painter.drawRect(18, 16, m_pVolumeChangeHandle->pos().x(),14);
    painter.restore();
}

void VolumeChangeSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
         QPoint currentPos =event->pos();
         if((currentPos.x() <= 385) && (currentPos.x() >= 0))
         {
             m_pVolumeChangeHandle->move(currentPos.x(),m_originalPosition.ry());
             m_currentPosX = m_pVolumeChangeHandle->pos().x();
         }
         update();

         //将坐标数据转化为音量数据
         float volumeValue =m_currentPosX / 384.0 * 100;
         emit updateMusicVolumeValue((int)volumeValue);
         qDebug()<<"CURRENT1:"<<(int)volumeValue;
    }
}


