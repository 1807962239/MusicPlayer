#include "MusicSlider.h"

//进度条把手

//进度条把手尺寸
#define MUSICSLIDERHANDLE_WIDTH 20
#define MUSICSLIDERHANDLE_HEIGHT 20

MusicSliderHandle::MusicSliderHandle(QWidget* parent)
    :QWidget{parent}
{
    setGeometry(0,0,MUSICSLIDERHANDLE_WIDTH,MUSICSLIDERHANDLE_HEIGHT);
}
void MusicSliderHandle::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton){
        //globalPosition获取的是QPointF
        m_lastMousePosition=event->globalPosition().toPoint();
    }
    return;
}
void MusicSliderHandle::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons()&Qt::LeftButton)
    {
        const QPoint position=event->globalPosition().toPoint()-m_lastMousePosition;
        emit musicSliderHandleMoveSignal(position.x());
    }
}
void MusicSliderHandle::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit musicSliderHandleMoveDoneSignal();
}

void MusicSliderHandle::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.save();
    painter.drawPixmap(rect(),QPixmap(":/images/Resources/musicSeekButton.png"));
    painter.restore();
}

//进度条主体

//进度条尺寸
#define MUSICSLIDER_WIDTH 344
#define MUSICSLIDER_HEIGHT 14



MusicSlider::MusicSlider(QWidget *parent)
    : QWidget{parent}
{
    setGeometry(68,17,MUSICSLIDER_WIDTH,26);

    m_pMusicSliderHandle=new MusicSliderHandle(this);
    connect(m_pMusicSliderHandle,&MusicSliderHandle::musicSliderHandleMoveSignal,this,&MusicSlider::onSliderHandleMove);
    connect(m_pMusicSliderHandle,&MusicSliderHandle::musicSliderHandleMoveDoneSignal,this,&MusicSlider::onSliderHandleMoveDone);

    //进度条把手位置就是进度条初始位置
    m_originalPosition=m_pMusicSliderHandle->pos();
    //初始化横向起点
    m_currentPosX=m_originalPosition.rx();
}
//槽函数
void MusicSlider::onSliderHandleMove(int posX)
{
    m_bHandleMoving=true;
    if(m_pMusicSliderHandle==NULL)
    {
        return;
    }
    //获取当前坐标点
    int realPosX=m_currentPosX+posX;

    //如果在范围内，则移动
    if((realPosX<=324)&&(realPosX>=0))
    {
        m_pMusicSliderHandle->move(realPosX,m_originalPosition.ry());
        update();
    }


}
void MusicSlider::onSliderHandleMoveDone()
{
    if(m_pMusicSliderHandle==NULL)
    {
        return;
    }
    //松开时的横坐标值
    m_currentPosX=m_pMusicSliderHandle->pos().x();
    //将坐标数据转化为音频进度数据
    float seekValue =m_currentPosX / 324.0 *100;
    emit updateMusicSeekValue((int)seekValue);
    //松开时将是否手动滑动设为false
    m_bHandleMoving=false;
}
//事件重写
void MusicSlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //绘制进度条背景
    painter.save();
    painter.drawPixmap(QRect(0,3,MUSICSLIDER_WIDTH,MUSICSLIDER_HEIGHT),QPixmap(":/images/Resources/musicProgressBg.png"));
    painter.restore();

    //绘制进度条
    painter.save();
    painter.setPen(QPen(QColor(136,135,140),1)); //设置笔
    painter.setBrush(QColor(136,135,140)); //设置填充
    //使进度条有一个平滑的开始
    painter.drawEllipse(0,3,14,14);
    //画走过的进度条
    painter.drawRect(6,3,m_pMusicSliderHandle->pos().x(),MUSICSLIDER_HEIGHT);
    painter.restore();
}
//实现在进度条上直接点击某一位置调节进度
void MusicSlider::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if(event->button()==Qt::LeftButton)
    {
        QPoint currentPos=event->pos();
        if((currentPos.x()<=326)&&(currentPos.x()>=0))
        {
            m_pMusicSliderHandle->move(currentPos.x(),m_originalPosition.ry());
            m_currentPosX=m_pMusicSliderHandle->pos().x();
        }
        update();
        //将坐标数据转化为音频进度数据
        float seekValue =m_currentPosX / 324.0*100 ;
        emit updateMusicSeekValue((int)seekValue);
    }
}
//成员函数
void MusicSlider::setUpMusicSliderValue(int aValue)
{
    //手动拉进度条时，不随时间再变动
    if(m_bHandleMoving==true)
    {
        return;
    }
    float realValue=aValue*3.24;
    m_pMusicSliderHandle->move((int)realValue,m_pMusicSliderHandle->pos().y());
}
