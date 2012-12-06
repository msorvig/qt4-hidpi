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
    pixmap2X = QPixmap(":/qticon@2x.png");
    pixmapLarge = QPixmap(":/qticon_large.png");

    image1X = QImage(":/qticon.png");
    image2X = QImage(":/qticon@2x.png");
    imageLarge = QImage(":/qticon_large.png");

    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
}

void PixmapPainter::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::gray));

    int pixmapPointSize = 64;
    int y = 30;
    int dy = 150;

    int x = 10;
    int dx = 80;
    // draw at point
//          qDebug() << "paint pixmap" << pixmap1X.devicePixelRatio();
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

class Labels : public QWidget
{
public:
    Labels();

    QPixmap pixmap1X;
    QPixmap pixmap2X;
    QPixmap pixmapLarge;
    QIcon qtIcon;
};

Labels::Labels()
{
    pixmap1X = QPixmap(":/qticon.png");
    pixmap2X = QPixmap(":/qticon@2x.png");
    pixmapLarge = QPixmap(":/qticon_large.png");

    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
    setWindowIcon(qtIcon);
    setWindowTitle("Labels");

    QLabel *label1x = new QLabel();
    label1x->setPixmap(pixmap1X);
    QLabel *label2x = new QLabel();
    label2x->setPixmap(pixmap2X);
    QLabel *labelIcon = new QLabel();
    labelIcon->setPixmap(qtIcon.pixmap(QSize(64,64)));
    QLabel *labelLarge = new QLabel();
    labelLarge->setPixmap(pixmapLarge);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(label1x); //expected low-res on high-dpi displays
    layout->addWidget(label2x);
    layout->addWidget(labelIcon);
    layout->addWidget(labelLarge); // expected large size and low-res
    setLayout(layout);
}

class MainWindow : public QMainWindow
{
public:
    MainWindow();

    QIcon qtIcon;
    QIcon qtIcon1x;
    QIcon qtIcon2x;

    QToolBar *fileToolBar;
};

MainWindow::MainWindow()
{
    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
    qtIcon1x.addFile(":/qticon.png");
    qtIcon2x.addFile(":/qticon@2x.png");
    setWindowIcon(qtIcon);
    setWindowTitle("MainWindow");

    fileToolBar = addToolBar(tr("File"));
//    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileToolBar->addAction(new QAction(qtIcon, QString("1x and 2x"), this));
    fileToolBar->addAction(new QAction(qtIcon1x, QString("1x"), this));
    fileToolBar->addAction(new QAction(qtIcon2x, QString("2x"), this));
}


class StandardIcons : public QWidget
{
public:
    void paintEvent(QPaintEvent *event)
    {
        int x = 10;
        int y = 10;
        int dx = 50;
        int dy = 50;
        int maxX = 500;

        for (int iconIndex = QStyle::SP_TitleBarMenuButton; iconIndex < QStyle::SP_MediaVolumeMuted; ++iconIndex) {
            QIcon icon = qApp->style()->standardIcon(QStyle::StandardPixmap(iconIndex));
            QPainter p(this);
            p.drawPixmap(x, y, icon.pixmap(dx - 5, dy - 5));
            if (x + dx > maxX)
                y+=dy;
            x = ((x + dx) % maxX);
        }
    };
};

class ImageDrawing : public QWidget
{
public:
    void paintEvent(QPaintEvent *event)
    {
        QPainter p(this);

        {   // Paint to target rect:
            QImage image = QImage(":/qticon_large.png");
            QRect target(10,10, 64, 64);
            p.drawImage(target, image);
        }

        {   // Paint to target point:
            QImage image = QImage(":/qticon_large.png");
            image.setDevicePixelRatio(2.0);
            QPoint target(100,10);
            p.drawImage(target, image);
        }

        {
            // Paint to target rect calculated from image size
            QImage image = QImage(":/qticon_large.png");
            image.setDevicePixelRatio(2.0);
            QRect target(QPoint(200,10), image.size() / image.devicePixelRatio());
            p.drawImage(target, image);
        }

        {   // Paint to target point using @2x image
            QImage image = QImage(":/qticon@2x.png");
            QPoint target(300,10);
            p.drawImage(target, image);
        }
    };
};


int main(int argc, char **argv)
{
    qputenv("QT_HIGHDPI_AWARE", "1");
    QApplication app(argc, argv);

    PixmapPainter pixmapPainter;

//  Enable for lots of pixmap drawing
//    pixmapPainter.show();

    Labels label;
    label.resize(200, 200);
//    label.show();

    MainWindow mainWindow;
//    mainWindow.show();

    StandardIcons icons;
    icons.resize(510, 510);
//    icons.show();
    
    ImageDrawing imageDrawing;
    imageDrawing.show();
    

    return app.exec();
}
