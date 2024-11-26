#include "ScreenSaver.h"
#include <QFrame>
#include <QTimeLine>
#include <QLabel>
#include <QWidget>
#include <QStackedLayout>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QAudioOutput>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QString>
#include <QGraphicsItemAnimation>
#include "Common/FileUtil.h"

WiiMixScreenSaver::WiiMixScreenSaver(QWidget* parent) {}

void WiiMixScreenSaver::CreateLayout() {
    printf("kiyyokfiohkjoiyut\n");
    layout = new QStackedLayout();
    mediaPlayer = new QMediaPlayer();
    std::string trailerPath = File::GetSysDirectory() + "Resources/trailer.mp4";
    mediaPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(trailerPath)));
    mediaPlayer->setLoops(100000);
    audioOutput = new QAudioOutput();
    mediaPlayer->setAudioOutput(audioOutput);
    videoItem = new QGraphicsVideoItem();
    mediaPlayer->setVideoOutput(videoItem);
    graphicsScene = new QGraphicsScene();
    videoItem->setSize(QSizeF(1920, 1080));
    graphicsScene->addItem(videoItem);
    graphicsView = new QGraphicsView();
    graphicsView->setContentsMargins(0, 0, 0, 0);
    graphicsView->setFixedSize(1920, 1080);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->fitInView(videoItem);
    graphicsView->setAlignment(Qt::AlignCenter);
    textItem = new QGraphicsTextItem(QStringLiteral("Press Start"));
    font = new QFont();
    font->setPointSize(60);
    font->setFamily(QStringLiteral("Joystix"));
    font->setCapitalization(QFont::AllUppercase);
    textItem->setFont(*font);
    QTextOption textOption = textItem->document()->defaultTextOption();
    graphicsView->setScene(graphicsScene);
    textOption.setAlignment(Qt::AlignCenter);
    outline = new QGraphicsDropShadowEffect();
    outline->setOffset(5, 5);
    outline->setBlurRadius(10);
    outline->setColor(Qt::black);
    textItem->setGraphicsEffect(outline);
    textItem->setHtml(QStringLiteral("<style> h2 {text-align: center; text-shadow: black, 5px, 10px;} </style> <h2>Press Start</h2>"));
    textItem->setY(540 - textItem->font().pointSize() / 2);
    //textItem->setX(550);
    printf("%d\n", 540 - textItem->font().pointSize() / 2);
    animation = new QGraphicsItemAnimation();
    animation->setItem(textItem);
    timer = new QTimeLine(1000);
    timer->setFrameRange(0, 100);
    animation->setTimeLine(timer);
    for (int i = 0; i < 100; i++) {
        animation->setScaleAt(((double) i) / 100, 1 + (double) i / 300, 1 + (double) i / 300);
        animation->setPosAt(((double) i) / 100, QPointF(550 - i * 4 / 3, 540 - textItem->font().pointSize() / 2 - i / 3));
    }
    graphicsScene->addItem(textItem);
    layout->addWidget(graphicsView);
    setLayout(layout);
    mediaPlayer->play();
    timer->start();
    connect(timer, &QTimeLine::finished, this, &WiiMixScreenSaver::changeDirection);
    layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    // this->setFixedSize(1920, 1080);
    this->setWindowState(Qt::WindowFullScreen);
    this->show();
}

void WiiMixScreenSaver::DeleteLayout() {
    layout->setCurrentIndex(-1);
}