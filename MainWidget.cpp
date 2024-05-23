#include "MainWidget.h"

//用来获取屏幕尺寸
#include <QScreen>
#include <QGuiApplication>

#include <QPushButton>

//宏定义控件尺寸
//窗体尺寸
#define MAINWIDGET_WIDTH 480
#define MAINWIDGET_HEIGHT 856
//按钮尺寸
#define TOPBUTTON_WIDTH 70
#define TOPBUTTON_HEIGHT 50



MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    //初始化界面尺寸和位置
    QScreen *screen=QGuiApplication::primaryScreen();
    QRect rect=screen->geometry();
    int startX=(rect.width()-MAINWIDGET_WIDTH)/2;
    int startY=(rect.height()-MAINWIDGET_HEIGHT)/2;
    setGeometry(startX,startY,MAINWIDGET_WIDTH,MAINWIDGET_HEIGHT);
    setFixedSize(MAINWIDGET_WIDTH,MAINWIDGET_HEIGHT);

    //用初始化函数加载界面
    loadTopButton(); //初始化按钮
    loadTopLabels(); //初始化标签
    loadPlayerWidget(); //初始化播放页
}

MainWidget::~MainWidget()
{
}
//重写事件
void MainWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(rect(),QPixmap(":/images/Resources/musicPlayerPageBg.png")); //rect()是当前窗口的宽高以及大小
}

//成员函数
void MainWidget::loadTopButton()
{
    //菜单栏上的返回按钮初始化
    QPushButton* backButton=new QPushButton(this);
    backButton->setGeometry(18,37,TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT);
    backButton->setFixedSize(QSize(TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT)); //设置固定尺寸
    backButton->setIcon(QIcon(":/images/Resources/backButton.png")); //设定按钮图标
    backButton->setIconSize(QSize(TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT)); //设定按钮图标尺寸
    backButton->setAutoFillBackground(true); //使被包含的窗体可以绘制背景
    backButton->setFlat(true); //设置按钮无边框与背景融为一体
    backButton->setStyleSheet("QPushButton{background:transparent}"); //设定按钮透明背景使其完全显示图标

    //菜单栏上的列表按钮初始化
    QPushButton* listButton=new QPushButton(this);
    listButton->setGeometry(MAINWIDGET_WIDTH-18-TOPBUTTON_WIDTH,37,TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT);
    listButton->setFixedSize(QSize(TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT)); //设置固定尺寸
    listButton->setIcon(QIcon(":/images/Resources/musicListButton.png")); //设定按钮图标
    listButton->setIconSize(QSize(TOPBUTTON_WIDTH,TOPBUTTON_HEIGHT)); //设定按钮图标尺寸
    listButton->setAutoFillBackground(true); //使被包含的窗体可以绘制背景
    listButton->setFlat(true); //设置按钮无边框与背景融为一体
    listButton->setStyleSheet("QPushButton{background:transparent}"); //设定按钮透明背景使其完全显示图标

}
void MainWidget::loadTopLabels()
{
    //歌曲名标签初始化
    m_pTitleLabel=new QLabel(this);
    m_pTitleLabel->setGeometry(120,40,MAINWIDGET_WIDTH-240,30);
    m_pTitleLabel->setFont(QFont("Arial",20,50)); //设置字体，大小，粗细
    m_pTitleLabel->setStyleSheet("background:transparent;font-size:20px;color:white");
    m_pTitleLabel->setAlignment(Qt::AlignCenter); //设置标签内容对齐方式为居中对齐
    m_pTitleLabel->setText("");

    //歌手名标签初始化
    m_pAuthorLabel=new QLabel(this);
    m_pAuthorLabel->setGeometry(120,70,MAINWIDGET_WIDTH-240,20);
    m_pAuthorLabel->setFont(QFont("Arial",15,50));
    m_pAuthorLabel->setStyleSheet("background:transparent;font-size:15px;color:white");
    m_pAuthorLabel->setAlignment(Qt::AlignCenter);
    m_pAuthorLabel->setText("");
}
void MainWidget::loadPlayerWidget()
{
    m_pPlayerWidget=new PlayerWidget(this);
    connect(m_pPlayerWidget,&PlayerWidget::updateMusicMetaInformation,this,&MainWidget::onUpdateMusicMetaInformation);
}

//槽函数
void MainWidget::onUpdateMusicMetaInformation(MusicInfo* info)
{
    if(info==NULL)
    {
        return;
    }
    m_pTitleLabel->setText(info->musicTitle);
    m_pAuthorLabel->setText(info->musicAuthor);
}
