#include <QtCore>
#include <QtGui>


class PixmapPainter : public QWidget
{
public:
    PixmapPainter();
    void paintEvent(QPaintEvent *event);

    QPixmap pixmap1X;
    QPixmap pixmap2X;
    QPixmap pixmapLarge;
    QImage image1X;
    QImage image2X;
    QImage imageLarge;
    QIcon qtIcon;
};


PixmapPainter::PixmapPainter()
{
    pixmap1X = QPixmap(":/qticon.png");
    qDebug() << "pixmap 1X size" << pixmap1X.size() << "scale" << pixmap1X.physicalDpiX() / pixmap1X.logicalDpiX();

    pixmap2X = QPixmap(":/qticon@2x.png");
    qDebug() << "pixmap 2X size" << pixmap2X.size() << "scale" << pixmap2X.physicalDpiX() / pixmap2X.logicalDpiX();

    pixmapLarge = QPixmap(":/qticon_large.png");
    qDebug() << "pixmap large size" << pixmapLarge.size() << "scale" << pixmapLarge.physicalDpiX() / pixmapLarge.logicalDpiX();


    image1X = QImage(":/qticon.png");
    qDebug() << "image 1X size" << image1X.size() << "scale" << image1X.physicalDpiX() / image1X.logicalDpiX();

    image2X = QImage(":/qticon@2x.png");
    qDebug() << "image 2X size" << image2X.size() << "scale" << image2X.physicalDpiX() / image2X.logicalDpiX();

    imageLarge = QImage(":/qticon_large.png");
    qDebug() << "image large size" << imageLarge.size() << "scale" << imageLarge.physicalDpiX() / imageLarge.logicalDpiX();

    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");

    qDebug() << "Pixmap size returned by qIcon for point sizes.";
    qDebug() << "Should be 2x upto 64x64 if any retina displays are connected, 1x otherwise";
    QPixmap pm32 = qtIcon.pixmap(QSize(32,32));
    qDebug() << "32x32" << pm32.size() << "scale" <<pm32.physicalDpiX() / pm32.logicalDpiX();
    QPixmap pm64 = qtIcon.pixmap(QSize(64,64));
    qDebug() << "64x64" << pm64.size() << "scale" <<pm64.physicalDpiX() / pm64.logicalDpiX();
    QPixmap pm128 = qtIcon.pixmap(QSize(128,128));
    qDebug() << "128x128" << pm128.size() << "scale" <<pm128.physicalDpiX() / pm128.logicalDpiX();
}

void PixmapPainter::paintEvent(QPaintEvent *event)
{
    qDebug() << "paint";
    QPainter p(this);
    p.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::gray));

    int pixmapPointSize = 64;
    int y = 30;
    int dy = 150;

    int x = 10;
    int dx = 80;
    // draw at point
          p.drawPixmap(x, y, pixmap1X);
    x+=dx;p.drawPixmap(x, y, pixmap2X);
    x+=dx;p.drawPixmap(x, y, pixmapLarge);
  x+=dx*2;p.drawPixmap(x, y, qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx;p.drawImage(x, y, image1X);
    x+=dx;p.drawImage(x, y, image2X);
    x+=dx;p.drawImage(x, y, imageLarge);

    // draw at 64x64 rect
    y+=dy;
    x = 10;
          p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmap1X);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmap2X);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmapLarge);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), image1X);
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), image2X);
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), imageLarge);


    // draw at 128x128 rect
    y+=dy - 50;
    x = 10;
               p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmap1X);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmap2X);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmapLarge);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), image1X);
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), image2X);
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), imageLarge);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    PixmapPainter pixmapPainter;
    pixmapPainter.show();
    
    return app.exec();
}
