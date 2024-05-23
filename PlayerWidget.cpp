#include "PlayerWidget.h"
#include <QDebug>
#include <QMimeData> //事件包含的数据
#include <QDir>

//宏定义控件尺寸
//窗体尺寸
#define PLAYERWIDGET_WIDTH 480
#define PLAYERWIDGET_HEIGHT 756
//按钮尺寸
#define PLAYERBUTTON_WIDTH 80
#define PLAYERBUTTON_HEIGHT 80
//唱针轴的坐标
#define STYLUSAXIS_POS_X 440
#define STYLUSAXIS_POS_Y 144



PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget{parent}
{
    setGeometry(0,100,PLAYERWIDGET_WIDTH,PLAYERWIDGET_HEIGHT);

    //初始化控件
    loadWidget();

    //初始化端点初始坐标
    float stylusRad=m_stylusAngle*MATH_PI/180; //唱针旋转弧度
    //点A到轴的距离乘sin(旋转弧度+初始弧度)等于点A的x与轴的x的差值
    //进而算出A的初始x值
    float posAX=STYLUSAXIS_POS_X-sin(m_angleA+stylusRad)*m_lengthA;
    //点A到轴的距离乘cos(旋转弧度+初始弧度)等于点A的y与轴的y的差值
    //进而算出A的初始y值
    float posAY=STYLUSAXIS_POS_Y+cos(m_angleA+stylusRad)*m_lengthA;
    //同理算出其他
    float posBX=STYLUSAXIS_POS_X-sin(m_angleB+stylusRad)*m_lengthB;
    float posBY=STYLUSAXIS_POS_Y+cos(m_angleB+stylusRad)*m_lengthB;
    float posCX=STYLUSAXIS_POS_X-sin(m_angleC+stylusRad)*m_lengthC;
    float posCY=STYLUSAXIS_POS_Y+cos(m_angleC+stylusRad)*m_lengthC;
    float posDX=STYLUSAXIS_POS_X-sin(m_angleD+stylusRad)*m_lengthD;
    float posDY=STYLUSAXIS_POS_Y+cos(m_angleD+stylusRad)*m_lengthD;
    //最后初始化坐标
    m_posA.set(posAX,posAY);
    m_posB.set(posBX,posBY);
    m_posC.set(posCX,posCY);
    m_posD.set(posDX,posDY);
    //再初始化一下当前坐标
    m_posACur=m_posA;
    m_posBCur=m_posB;
    m_posCCur=m_posC;
    m_posDCur=m_posD;

    //初始化播放器
    initMusicPlayerInstance();

    //使此界面可以接收拖拽
    setAcceptDrops(true);

}
PlayerWidget::~PlayerWidget()
{
    //手动释放vlc组件
    //释放播放器
    if(m_pVlcMediaPlayer)
    {
        libvlc_media_player_release(m_pVlcMediaPlayer);
        m_pVlcMediaPlayer=NULL;
        qDebug()<<"player_release";
    }
    //释放实例
    if(m_pMusicPlayerInstance)
    {
        libvlc_release(m_pMusicPlayerInstance);
        m_pMusicPlayerInstance=NULL;
        qDebug()<<"instance_release";
    }
}

//事件重写
void PlayerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //画唱片图案
    painter.save();
    //先让坐标系的起点移到唱片目标位置的中心点
    painter.translate(width()/2,326);
    //旋转坐标系使唱片根据变化的角度旋转
    painter.rotate(m_rotateAngle);
    //将唱片画在坐标系起点-w/2,-h/2的地方，使其中心恰好在平移后的坐标系起点
    painter.drawPixmap(QRect(-211,-211,422,422),QPixmap(":/images/Resources/cdBackground.png"));
    painter.restore();

    //画唱针
    painter.save();
    //先把坐标起始点移到唱针的旋转轴点
    painter.translate(STYLUSAXIS_POS_X,STYLUSAXIS_POS_Y);
    //旋转坐标系使唱针根据变化的角度旋转
    painter.rotate(m_stylusAngle);
    //按轴点距左边界和上边界的距离画出唱针
    painter.drawPixmap(QRect(-123,-48,211,417),QPixmap(":/images/Resources/cdPlayJoystick.png"));
    painter.restore();
}
void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        m_lastMousePos=event->pos();
        const QPoint currPos=event->pos(); //获取鼠标点击位置

        bool bInRect=isPointInRect(currPos.x(),currPos.y());

        if(bInRect){
            m_bMouseInStylusRect=true; //确定鼠标是否在四边形内点击的

        }
    }
}
void PlayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton){
        const QPoint currPos=event->pos();
        const QPoint diffPos=currPos-m_lastMousePos;

        //如果鼠标点击后移动的距离几乎没什么变化就不必进行后续唱针移动操作
        if(diffPos.x()==0||diffPos.y()==0){
            return;
        }

        //鼠标在唱针头中点击后移动则开始判断移动的距离和角度
        if(m_bMouseInStylusRect==true)
        {
            m_bStylusMoving=true;

            //获取移动角度
            m_mouseMoveAngle=caculateMouseMoveAngle(currPos.x(),currPos.y());

            //设定顺时针为正，如果向x正向移动，移动的角度是负的
            if(diffPos.x()>=0){
                m_mouseMoveAngle=-m_mouseMoveAngle;
            }
            //获取之前角度加上移动角度获取当前旋转角度
            float t_rotateAngle=m_mouseLastAngle+m_mouseMoveAngle;

            //如果超出了最大角度，则将移动角度重置为上次没超时移动的角度
            if(t_rotateAngle>=22){
                m_mouseMoveAngle=m_mouseOverAngle;
                return;
            }
            //如果反向超过了最大角度，则同样
            if(t_rotateAngle<=0){
                m_mouseMoveAngle=m_mouseOverAngle;
                return;
            }
            //没有超出最大角度才可给唱针移动角度赋值
            m_stylusAngle=t_rotateAngle;
            //刷新画面
            update();
        }
        //更新每一次移动角度没超的时候移动的角度
        m_mouseOverAngle=m_mouseMoveAngle;
    }
}
void PlayerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    //鼠标在唱针头中点击后松开
    if(m_bMouseInStylusRect==true){
        m_mouseLastAngle=m_mouseLastAngle+m_mouseMoveAngle;
        if(m_mouseLastAngle>=22){
            m_mouseLastAngle=22;
        }
        if(m_mouseLastAngle<=0){
            m_mouseLastAngle=0;
        }
        //鼠标结束移动后更新唱针头四边形的四个顶点位置
        updateCurrentStylusPosition();

        //这一步如果不在唱针头内，则在播放器任意位置松开鼠标都会控制播放进度
        //把角度转化为音频进度信息
        float rotateRate=m_stylusAngle/22.0*100;
        //控制播放进度
        onMusicSeekValueChanged((int)rotateRate);

    }

    m_bOverAngle=false;
    //重置鼠标点击状态
    m_bMouseInStylusRect=false;

    //唱针状态标记为结束手动移动
    m_bStylusMoving=false;


}
void PlayerWidget::dragEnterEvent(QDragEnterEvent* event)
{
    //判断是否为某一数据类型
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        //将抓取的数据设置为期望数据
        event->acceptProposedAction();
    }
}
void PlayerWidget::dropEvent(QDropEvent*event)
{
    //获取鼠标抓取数据中的第一个数据的路径
    QUrl url=event->mimeData()->urls().first();
    if(url.isEmpty())
    {
        qDebug()<<"url is empty";
        return;
    }

    //将url转为本地文件系统的文件路径 并确保特殊字符得到正确的编码和解码
    QByteArray byte=QByteArray(url.toString().toUtf8());
    QUrl encodeUrl(byte);
    encodeUrl=encodeUrl.fromEncoded(byte);
    QString mp3Path=encodeUrl.toLocalFile();

    //将路径的斜杠转化为本地系统所使用的斜杠
    mp3Path=QDir::toNativeSeparators(mp3Path);


    resetPlayStatusOnNewFile();

    //创建媒体，需要把路径转换为参数所需要的utf-8码的字节数组指针
    libvlc_media_t* media=libvlc_media_new_path(m_pMusicPlayerInstance,mp3Path.toUtf8().data());
    if(media==NULL)
    {
        qDebug()<<"media is NULL";
    }
    else
    {
        qDebug()<<"media init ok";
    }
    //解析媒体文件
    libvlc_media_parse(media);

    //获取媒体信息
    //获取音乐名
    char* musicTitle=libvlc_media_get_meta(media,libvlc_meta_Title);
    //获取音乐作者名
    char* musicArtist=libvlc_media_get_meta(media,libvlc_meta_Artist);

    //初始化音乐信息变量
    QString artistString=QString(musicArtist);
    QString titleString=QString(musicTitle);
    MusicInfo musicInfo;
    musicInfo.musicAuthor=artistString;
    musicInfo.musicTitle=titleString;
    setMusicInformation(&musicInfo);

    //根据媒体创建媒体播放器
    m_pVlcMediaPlayer=libvlc_media_player_new_from_media(media);
    if(m_pVlcMediaPlayer)
    {
        qDebug()<<"vlcPlayer init ok";
    }
    else
    {
        qDebug()<<"vlcPlayer no init";
    }

    libvlc_media_release(media);
}

//成员函数
//判断点是否在凸四边形中的算法
bool PlayerWidget::isPointInRect(float x,float y){
    const CCVec2f A=m_posACur;
    const CCVec2f B=m_posBCur;
    const CCVec2f C=m_posCCur;
    const CCVec2f D=m_posDCur;

    float a=(B.x-A.x)*(y-A.y)-(B.y-A.y)*(x-A.x);
    float b=(C.x-B.x)*(y-B.y)-(C.y-B.y)*(x-B.x);
    float c=(D.x-C.x)*(y-C.y)-(D.y-C.y)*(x-C.x);
    float d=(A.x-D.x)*(y-D.y)-(A.y-D.y)*(x-D.x);
    if((a>0&&b>0&&c>0&&d>0)||(a<0&&b<0&&c<0&&d<0)){
        return true;
    }
    return false;
}
//计算鼠标移动的角度
float PlayerWidget::caculateMouseMoveAngle(float x,float y)
{
    //唱针轴点
    const CCVec2f posAxis=CCVec2f(STYLUSAXIS_POS_X,STYLUSAXIS_POS_Y);
    //鼠标点击点
    const CCVec2f posMouseStart=CCVec2f(m_lastMousePos.x(),m_lastMousePos.y());
    //当前鼠标位置
    const CCVec2f posMouseCurr=CCVec2f(x,y);

    //计算向量
    const CCVec2f vectorA=CCVec2f(posMouseStart.x-posAxis.x,posMouseStart.y-posAxis.y);
    const CCVec2f vectorB=CCVec2f(posMouseCurr.x-posAxis.x,posMouseCurr.y-posAxis.y);

    //向量的积
    float mulValue=vectorA.x*vectorB.x+vectorA.y*vectorB.y;
    //向量的模(直接用封装好的类中的求模的函数)
    float molValueA=vectorA.length();
    float molValueB=vectorB.length();

    //最后计算夹角
    float cosValue=mulValue/(molValueA*molValueB);
    float angleAB=acos(cosValue)*180/MATH_PI;

    return angleAB;
}
//鼠标结束移动后更新唱针头四边形的四个顶点位置
void PlayerWidget::updateCurrentStylusPosition()
{
    //和顶点位置初始化几乎一模一样
    float stylusRad=m_stylusAngle*MATH_PI/180; //当前唱针旋转弧度

    float posAX=STYLUSAXIS_POS_X-sin(m_angleA+stylusRad)*m_lengthA;
    float posAY=STYLUSAXIS_POS_Y+cos(m_angleA+stylusRad)*m_lengthA;
    float posBX=STYLUSAXIS_POS_X-sin(m_angleB+stylusRad)*m_lengthB;
    float posBY=STYLUSAXIS_POS_Y+cos(m_angleB+stylusRad)*m_lengthB;
    float posCX=STYLUSAXIS_POS_X-sin(m_angleC+stylusRad)*m_lengthC;
    float posCY=STYLUSAXIS_POS_Y+cos(m_angleC+stylusRad)*m_lengthC;
    float posDX=STYLUSAXIS_POS_X-sin(m_angleD+stylusRad)*m_lengthD;
    float posDY=STYLUSAXIS_POS_Y+cos(m_angleD+stylusRad)*m_lengthD;

    m_posACur.set(posAX,posAY);
    m_posBCur.set(posBX,posBY);
    m_posCCur.set(posCX,posCY);
    m_posDCur.set(posDX,posDY);
}
void PlayerWidget::loadWidget()
{
    //初始化标签
    m_pCurrentTimeLabel=new QLabel(this);
    m_pCurrentTimeLabel->setGeometry(15,15,50,25);
    m_pCurrentTimeLabel->setFont(QFont("Arial",20,50));
    m_pCurrentTimeLabel->setStyleSheet("background: transparent;font-size: 20px;color: #ababab");
    //m_pCurrentTimeLabel->setAlignment(Qt::AlignLeft);
    m_pCurrentTimeLabel->setText("00:00");

    m_pAllTimeLabel=new QLabel(this);
    m_pAllTimeLabel->setGeometry(PLAYERWIDGET_WIDTH-65,15,50,25);
    m_pAllTimeLabel->setFont(QFont("Arial",20,50));
    m_pAllTimeLabel->setStyleSheet("background: transparent;font-size: 20px;color: #ababab");
    //m_pRemainTimeLabel->setAlignment(Qt::AlignLeft);
    m_pAllTimeLabel->setText("00:00");


    //初始化按钮
    m_pPreMusicButton=new QPushButton(this);
    m_pPreMusicButton->setGeometry(85,PLAYERWIDGET_HEIGHT-190,PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT);
    m_pPreMusicButton->setFixedSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pPreMusicButton->setIcon(QIcon(":/images/Resources/previousButton.png"));
    m_pPreMusicButton->setIconSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pPreMusicButton->setAutoFillBackground(true);
    m_pPreMusicButton->setFlat(true);
    m_pPreMusicButton->setStyleSheet("QPushButton{background:transparent}");

    m_pPlayMusicButton=new QPushButton(this);
    m_pPlayMusicButton->setGeometry(195,PLAYERWIDGET_HEIGHT-195,PLAYERBUTTON_WIDTH+16,PLAYERBUTTON_HEIGHT+16);
    m_pPlayMusicButton->setFixedSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pPlayMusicButton->setIcon(QIcon(":/images/Resources/playButton.png"));
    m_pPlayMusicButton->setIconSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pPlayMusicButton->setAutoFillBackground(true);
    m_pPlayMusicButton->setFlat(true);
    m_pPlayMusicButton->setStyleSheet("QPushButton{background:transparent}");
    connect(m_pPlayMusicButton,&QPushButton::clicked,this,&PlayerWidget::onPlayMusicButtonClicked);


    m_pNextMusicButton=new QPushButton(this);
    m_pNextMusicButton->setGeometry(310,PLAYERWIDGET_HEIGHT-190,PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT);
    m_pNextMusicButton->setFixedSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pNextMusicButton->setIcon(QIcon(":/images/Resources/nextButton.png"));
    m_pNextMusicButton->setIconSize(QSize(PLAYERBUTTON_WIDTH,PLAYERBUTTON_HEIGHT));
    m_pNextMusicButton->setAutoFillBackground(true);
    m_pNextMusicButton->setFlat(true);
    m_pNextMusicButton->setStyleSheet("QPushButton{background:transparent}");

    //初始化进度条
    m_pMusicSlider=new MusicSlider(this);
    connect(m_pMusicSlider,&MusicSlider::updateMusicSeekValue,this,&PlayerWidget::onMusicSeekValueChanged);

    //初始化音量条
    m_pVolumeChangeSlider=new VolumeChangeSlider(this);
    connect(m_pVolumeChangeSlider,&VolumeChangeSlider::updateMusicVolumeValue,this,&PlayerWidget::onVolumeSliderValueChanged);
    //设置初始音量为75
    m_pVolumeChangeSlider->SetVolumeSliderProgressValue(75);

    //初始化定时器
    m_pMusicPlayTimer=new QTimer(this);
    connect(m_pMusicPlayTimer,&QTimer::timeout,this,&PlayerWidget::onMusicTimerProcess);
    m_bTimerPlaying=false;

}
void PlayerWidget::initMusicPlayerInstance()
{
    //初始化播放器
    m_pVlcMediaPlayer=NULL;
    //初始化播放状态
    m_eMusicPlayerStatus=MUSICPLAYER_STATUS_STOPED;
    //初始化实例
    //参数分别为命令行参数个数和命令行类型参数
    m_pMusicPlayerInstance=libvlc_new(0,NULL);
    if(m_pMusicPlayerInstance==NULL)
    {
        qDebug()<<"init not ok";
    }
    else
    {
        qDebug()<<"init ok";
    }
}
void PlayerWidget::setMusicInformation(MusicInfo* info)
{
    if(info==NULL){
        return;
    }

    emit updateMusicMetaInformation(info);

    //qDebug()<<"音乐："<<info->musicTitle<<"作者："<<info->musicAuthor;
}
void PlayerWidget::playMusicOnVlcMediaPlayerEngine()
{

    if(m_eMusicPlayerStatus==MUSICPLAYER_STATUS_PLAY)
    {
        libvlc_media_player_pause(m_pVlcMediaPlayer);
        m_eMusicPlayerStatus=MUSICPLAYER_STATUS_PAUSE;
    }
    else if(m_eMusicPlayerStatus==MUSICPLAYER_STATUS_PAUSE)
    {
        libvlc_media_player_play(m_pVlcMediaPlayer);
        m_eMusicPlayerStatus=MUSICPLAYER_STATUS_PLAY;
    }
    else
    {
        if(m_pVlcMediaPlayer)
        {
        }
        libvlc_media_player_play(m_pVlcMediaPlayer);
        m_eMusicPlayerStatus=MUSICPLAYER_STATUS_PLAY;
    }
}
void PlayerWidget::resetPlayStatusOnNewFile()
{

    //如果当前有音乐正在播放则刷新状态
    //在每次创建媒体初始化播放器时检查，如果已经有播放器则先释放,防止内存泄露
    if(m_pVlcMediaPlayer)
    {
        libvlc_media_player_release(m_pVlcMediaPlayer);
        m_pVlcMediaPlayer=NULL;
    }
    if(m_bTimerPlaying==true)
    {
        if(m_pMusicPlayTimer->isActive())
        {
            m_pMusicPlayTimer->stop();
            m_bTimerPlaying=false;
        }
        m_pPlayMusicButton->setIcon(QIcon(":/images/Resources/playButton.png"));
    }
    if(m_eMusicPlayerStatus==MUSICPLAYER_STATUS_PLAY)
    {
        m_eMusicPlayerStatus=MUSICPLAYER_STATUS_PAUSE;
    }
}
void PlayerWidget::setAndUpdateStylusAngle(float aValue)
{
    if(m_bStylusMoving==true)
    {
        return;
    }
    //将0到100转化为0到22
    float realAngleValue=aValue*0.22;
    //更新角度与画面
    m_stylusAngle=realAngleValue;
    m_mouseLastAngle=m_stylusAngle;
    //更新矩形唱针头点位
    updateCurrentStylusPosition();
    update();
}
void PlayerWidget::resetAllWhileMusicPlayEnded()
{
    //重置进度条位置
    m_pMusicSlider->setUpMusicSliderValue(0);
    //重置计时器与图标
    if(m_bTimerPlaying==true)
    {
        if(m_pMusicPlayTimer->isActive())
        {
            m_pMusicPlayTimer->stop();
            m_bTimerPlaying=false;
        }
        m_pPlayMusicButton->setIcon(QIcon(":/images/Resources/playButton.png"));
    }
    //设置播放器状态
    m_eMusicPlayerStatus=MUSICPLAYER_STATUS_STOPED;
    //重置唱针位置
    setAndUpdateStylusAngle(0);
    //重置时间显示
    m_pCurrentTimeLabel->setText("00:00");
}
//槽函数
void PlayerWidget::onMusicTimerProcess()
{
    //刷新唱片旋转角度
    m_rotateAngle+=10.0f;
    if(m_rotateAngle>=360){
        m_rotateAngle=0.0f;
    }
    update();

    if(m_pVlcMediaPlayer==NULL)
    {
        return;
    }

    //获取播放状态
    libvlc_state_t currentState=libvlc_media_player_get_state(m_pVlcMediaPlayer);


    //获取播放进度并更新画面
    if(currentState==libvlc_Playing)
    {
        //进度值从0到100
        float prograssPos=libvlc_media_player_get_position(m_pVlcMediaPlayer)*100.0;
        //刷新进度条
        m_pMusicSlider->setUpMusicSliderValue((int)prograssPos);
        //刷新唱针
        setAndUpdateStylusAngle(prograssPos);
    }
    //如果正常播放结束
    else if(currentState==libvlc_Ended)
    {
        qDebug()<<"state changed"<<currentState;
        //重置播放器状态
        resetAllWhileMusicPlayEnded();
    }

    //播放时间刷新
    int currMTime=libvlc_media_player_get_time(m_pVlcMediaPlayer);
    QTime cTime=QTime(0,0,0);
    QTime currentTime=cTime.addMSecs(currMTime);

    int allMTime=libvlc_media_player_get_length(m_pVlcMediaPlayer);
    QTime aTime=QTime(0,0,0);
    QTime allTime=aTime.addMSecs(allMTime);

    m_pCurrentTimeLabel->setText(currentTime.toString("mm:ss"));
    m_pAllTimeLabel->setText(allTime.toString("mm:ss"));
}
void PlayerWidget::onPlayMusicButtonClicked()
{
    //如果未导入音乐，则播放器不存在，则不执行全部的播放逻辑
    if(m_pVlcMediaPlayer==NULL)
    {
        qDebug()<<"play error: vlc media player is null";
        return;
    }
    //定时器在运行
    if(m_bTimerPlaying==true)
    {
        if(m_pMusicPlayTimer->isActive())
        {
            m_pMusicPlayTimer->stop();
            m_bTimerPlaying=false;
        }
        m_pPlayMusicButton->setIcon(QIcon(":/images/Resources/playButton.png"));
    }
    //定时器没在运行
    else
    {
        m_pMusicPlayTimer->start(100);
        m_bTimerPlaying=true;
        m_pPlayMusicButton->setIcon(QIcon(":/images/Resources/pauseButton.png"));
    }
    //播放或暂停音乐
    playMusicOnVlcMediaPlayerEngine();
}
void PlayerWidget::onVolumeSliderValueChanged(int value)
{
    if(m_pVlcMediaPlayer==NULL)
    {
        return;
    }
    //播放器正在播放时调整音量
    if(libvlc_media_player_is_playing(m_pVlcMediaPlayer))
    {
        libvlc_audio_set_volume(m_pVlcMediaPlayer,value);
    }
}
void PlayerWidget::onMusicSeekValueChanged(int value)
{
    if(m_pVlcMediaPlayer==NULL)
    {
        return;
    }
    if(libvlc_media_player_is_playing(m_pVlcMediaPlayer))
    {
        float pos=value/100.0;
        libvlc_media_player_set_position(m_pVlcMediaPlayer,pos);
    }
}
