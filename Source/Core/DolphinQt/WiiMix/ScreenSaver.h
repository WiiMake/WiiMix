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

class WiiMixScreenSaver: public QFrame {
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
    explicit WiiMixScreenSaver(QObject *parent = nullptr);
    void CreateLayout();
    void DeleteLayout();
};