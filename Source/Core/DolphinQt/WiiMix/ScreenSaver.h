

#include <QFrame>
#include <QTimeLine>
#include <QLabel>
#include <QStackedLayout>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QAudioOutput>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QString>
#include <QGraphicsItemAnimation>

class ScreenSaver: public QFrame {
private:
    QTimeLine* timer;
    QStackedLayout* layout;
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    QGraphicsVideoItem* videoItem;
    QGraphicsScene* graphicsScene;
    QGraphicsView* graphicsView;
    QGraphicsTextItem* textItem;
    QFont* font;
    QGraphicsDropShadowEffect* outline;
    QGraphicsItemAnimation* animation;
    void changeDirection() {
        //printf("animation finished\n");
        if (timer->direction() == QTimeLine::Forward) {
            timer->setDirection(QTimeLine::Backward);
            timer->start();
        }
        else {
            timer->setDirection(QTimeLine::Forward);
            timer->start();
        }
    }
public:
    ScreenSaver() {
        layout = new QStackedLayout();
        mediaPlayer = new QMediaPlayer(this);
        mediaPlayer->setSource(QUrl::fromLocalFile(QStringLiteral("/Users/vdadvdad/CLionProjects/qtTest2/trailer.mp4")));
        mediaPlayer->setLoops(100000);
        audioOutput = new QAudioOutput(this);
        mediaPlayer->setAudioOutput(audioOutput);
        videoItem = new QGraphicsVideoItem();
        mediaPlayer->setVideoOutput(videoItem);
        graphicsScene = new QGraphicsScene();
        videoItem->setSize(QSizeF(1920, 1080));
        graphicsScene->addItem(videoItem);
        graphicsView = new QGraphicsView();
        graphicsView->setContentsMargins(0, 0, 0, 0);
        graphicsView->setFixedSize(1920, 1080);
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
        connect(timer, &QTimeLine::finished, this, &ScreenSaver::changeDirection);
        layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    }

    ~ScreenSaver() {
        delete timer;
        delete layout;
        delete mediaPlayer;
        delete audioOutput;
        delete videoItem;
        delete graphicsScene;
        delete graphicsView;
        delete textItem;
        delete font;
        delete outline;
        delete animation;
    }
};


