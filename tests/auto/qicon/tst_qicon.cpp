/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qicon.h>

#if defined(Q_OS_SYMBIAN)
#define SRCDIR "."
#endif
#include <qiconengine.h>

Q_DECLARE_METATYPE(QSize)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QIcon : public QObject
{
    Q_OBJECT
public:
    tst_QIcon();

private slots:
    void init();
    void cleanup();

    void actualSize_data(); // test with 1 pixmap
    void actualSize();
    void actualSize2_data(); // test with 2 pixmaps with different aspect ratio
    void actualSize2();
    void svgActualSize();
    void isNull();
    void swap();
    void bestMatch();
    void cacheKey();
    void detach();
    void svg();
    void addFile();
    void availableSizes();
    void name();
    void streamAvailableSizes_data();
    void streamAvailableSizes();
    void fromTheme();

    void task184901_badCache();
    void task223279_inconsistentAddFile();
    void task239461_custom_iconengine_crash();
    void highdpi();
    
private:
    QString oldCurrentDir;

    const static QIcon staticIcon;
};

// Creating an icon statically should not cause a crash.
// But we do not officially support this. See QTBUG-8666
const QIcon tst_QIcon::staticIcon = QIcon::fromTheme("edit-find");

void tst_QIcon::init()
{
    QString srcdir(QLatin1String(SRCDIR));
    if (!srcdir.isEmpty()) {
        oldCurrentDir = QDir::current().absolutePath();
        QDir::setCurrent(srcdir);
    }
}

void tst_QIcon::cleanup()
{
    if (!oldCurrentDir.isEmpty()) {
        QDir::setCurrent(oldCurrentDir);
    }
}

tst_QIcon::tst_QIcon()
{
}

void tst_QIcon::actualSize_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QSize>("argument");
    QTest::addColumn<QSize>("result");

    // square image
    QTest::newRow("resource0") << ":/image.png" << QSize(128, 128) << QSize(128, 128);
    QTest::newRow("resource1") << ":/image.png" << QSize( 64,  64) << QSize( 64,  64);
    QTest::newRow("resource2") << ":/image.png" << QSize( 32,  64) << QSize( 32,  32);
    QTest::newRow("resource3") << ":/image.png" << QSize( 16,  64) << QSize( 16,  16);
    QTest::newRow("resource4") << ":/image.png" << QSize( 16,  128) << QSize( 16,  16);
    QTest::newRow("resource5") << ":/image.png" << QSize( 128,  16) << QSize( 16,  16);
    QTest::newRow("resource6") << ":/image.png" << QSize( 150,  150) << QSize( 128,  128);
    // rect image
    QTest::newRow("resource7") << ":/rect.png" << QSize( 20,  40) << QSize( 20,  40);
    QTest::newRow("resource8") << ":/rect.png" << QSize( 10,  20) << QSize( 10,  20);
    QTest::newRow("resource9") << ":/rect.png" << QSize( 15,  50) << QSize( 15,  30);
    QTest::newRow("resource10") << ":/rect.png" << QSize( 25,  50) << QSize( 20,  40);

    const QString prefix = QLatin1String(SRCDIR) + QLatin1String("/");
    QTest::newRow("external0") << prefix + "image.png" << QSize(128, 128) << QSize(128, 128);
    QTest::newRow("external1") << prefix + "image.png" << QSize( 64,  64) << QSize( 64,  64);
    QTest::newRow("external2") << prefix + "image.png" << QSize( 32,  64) << QSize( 32,  32);
    QTest::newRow("external3") << prefix + "image.png" << QSize( 16,  64) << QSize( 16,  16);
    QTest::newRow("external4") << prefix + "image.png" << QSize( 16,  128) << QSize( 16,  16);
    QTest::newRow("external5") << prefix + "image.png" << QSize( 128,  16) << QSize( 16,  16);
    QTest::newRow("external6") << prefix + "image.png" << QSize( 150,  150) << QSize( 128,  128);
    // rect image
    QTest::newRow("external7") << ":/rect.png" << QSize( 20,  40) << QSize( 20,  40);
    QTest::newRow("external8") << ":/rect.png" << QSize( 10,  20) << QSize( 10,  20);
    QTest::newRow("external9") << ":/rect.png" << QSize( 15,  50) << QSize( 15,  30);
    QTest::newRow("external10") << ":/rect.png" << QSize( 25,  50) << QSize( 20,  40);
}

void tst_QIcon::actualSize()
{
    QFETCH(QString, source);
    QFETCH(QSize, argument);
    QFETCH(QSize, result);

    {
        QPixmap pixmap(source);
        QIcon icon(pixmap);
        QCOMPARE(icon.actualSize(argument), result);
        QCOMPARE(icon.pixmap(argument).size(), result);
    }

    {
        QIcon icon(source);
        QCOMPARE(icon.actualSize(argument), result);
        QCOMPARE(icon.pixmap(argument).size(), result);
    }
}

void tst_QIcon::actualSize2_data()
{
    QTest::addColumn<QSize>("argument");
    QTest::addColumn<QSize>("result");

    // two images - 128x128 and 20x40. Let the games begin
    QTest::newRow("trivial1") << QSize( 128,  128) << QSize( 128,  128);
    QTest::newRow("trivial2") << QSize( 20,  40) << QSize( 20,  40);

    // QIcon chooses the one with the smallest area to choose the pixmap
    QTest::newRow("best1") << QSize( 100,  100) << QSize( 100,  100);
    QTest::newRow("best2") << QSize( 20,  20) << QSize( 10,  20);
    QTest::newRow("best3") << QSize( 15,  30) << QSize( 15,  30);
    QTest::newRow("best4") << QSize( 5,  5) << QSize( 2,  5);
    QTest::newRow("best5") << QSize( 10,  15) << QSize( 7,  15);
}

void tst_QIcon::actualSize2()
{
    QIcon icon;
    const QString prefix = QLatin1String(SRCDIR) + QLatin1String("/");

    icon.addPixmap(QPixmap(prefix + "image.png"));
    icon.addPixmap(QPixmap(prefix + "rect.png"));

    QFETCH(QSize, argument);
    QFETCH(QSize, result);

    QCOMPARE(icon.actualSize(argument), result);
    QCOMPARE(icon.pixmap(argument).size(), result);
}

void tst_QIcon::svgActualSize()
{
    const QString prefix = QLatin1String(SRCDIR) + QLatin1String("/");
    QIcon icon(prefix + "rect.svg");
    QCOMPARE(icon.actualSize(QSize(16, 16)), QSize(16, 2));
    QCOMPARE(icon.pixmap(QSize(16, 16)).size(), QSize(16, 2));

    QPixmap p(16, 16);
    p.fill(Qt::cyan);
    icon.addPixmap(p);

    QCOMPARE(icon.actualSize(QSize(16, 16)), QSize(16, 16));
    QCOMPARE(icon.pixmap(QSize(16, 16)).size(), QSize(16, 16));

    QCOMPARE(icon.actualSize(QSize(16, 14)), QSize(16, 2));
    QCOMPARE(icon.pixmap(QSize(16, 14)).size(), QSize(16, 2));
}

void tst_QIcon::isNull() {
    // test default constructor
    QIcon defaultConstructor;
    QVERIFY(defaultConstructor.isNull());

    // test copy constructor
    QVERIFY(QIcon(defaultConstructor).isNull());

    // test pixmap constructor
    QPixmap nullPixmap;
    QVERIFY(QIcon(nullPixmap).isNull());

    // test string constructor with empty string
    QIcon iconEmptyString = QIcon(QString());
    QVERIFY(iconEmptyString.isNull());
    QVERIFY(!iconEmptyString.actualSize(QSize(32, 32)).isValid());;

    // test string constructor with non-existing file
    QIcon iconNoFile = QIcon("imagedoesnotexist");
    QVERIFY(!iconNoFile.isNull());
    QVERIFY(!iconNoFile.actualSize(QSize(32, 32)).isValid());

    // test string constructor with non-existing file with suffix
    QIcon iconNoFileSuffix = QIcon("imagedoesnotexist.png");
    QVERIFY(!iconNoFileSuffix.isNull());
    QVERIFY(!iconNoFileSuffix.actualSize(QSize(32, 32)).isValid());

    const QString prefix = QLatin1String(SRCDIR) + QLatin1String("/");
    // test string constructor with existing file but unsupported format
    QIcon iconUnsupportedFormat = QIcon(prefix + "tst_qicon.cpp");
    QVERIFY(!iconUnsupportedFormat.isNull());
    QVERIFY(!iconUnsupportedFormat.actualSize(QSize(32, 32)).isValid());

    // test string constructor with existing file and supported format
    QIcon iconSupportedFormat = QIcon(prefix + "image.png");
    QVERIFY(!iconSupportedFormat.isNull());
    QVERIFY(iconSupportedFormat.actualSize(QSize(32, 32)).isValid());
}

void tst_QIcon::swap()
{
    QPixmap p1(1, 1), p2(2, 2);
    p1.fill(Qt::black);
    p2.fill(Qt::black);

    QIcon i1(p1), i2(p2);
    const qint64 i1k = i1.cacheKey();
    const qint64 i2k = i2.cacheKey();
    QVERIFY(i1k != i2k);
    i1.swap(i2);
    QCOMPARE(i1.cacheKey(), i2k);
    QCOMPARE(i2.cacheKey(), i1k);
}

void tst_QIcon::bestMatch()
{
    QPixmap p1(1, 1);
    QPixmap p2(2, 2);
    QPixmap p3(3, 3);
    QPixmap p4(4, 4);
    QPixmap p5(5, 5);
    QPixmap p6(6, 6);
    QPixmap p7(7, 7);
    QPixmap p8(8, 8);

    p1.fill(Qt::black);
    p2.fill(Qt::black);
    p3.fill(Qt::black);
    p4.fill(Qt::black);
    p5.fill(Qt::black);
    p6.fill(Qt::black);
    p7.fill(Qt::black);
    p8.fill(Qt::black);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 2; ++j) {
            QIcon::State state = (j == 0) ? QIcon::On : QIcon::Off;
            QIcon::State oppositeState = (state == QIcon::On) ? QIcon::Off
                                                              : QIcon::On;
            QIcon::Mode mode;
            QIcon::Mode oppositeMode;

            QIcon icon;

            switch (i) {
            case 0:
            default:
                mode = QIcon::Normal;
                oppositeMode = QIcon::Active;
                break;
            case 1:
                mode = QIcon::Active;
                oppositeMode = QIcon::Normal;
                break;
            case 2:
                mode = QIcon::Disabled;
                oppositeMode = QIcon::Selected;
                break;
            case 3:
                mode = QIcon::Selected;
                oppositeMode = QIcon::Disabled;
            }

            /*
                The test mirrors the code in
                QPixmapIconEngine::bestMatch(), to make sure that
                nobody breaks QPixmapIconEngine by mistake. Before
                you change this test or the code that it tests,
                please talk to the maintainer if possible.
            */
            if (mode == QIcon::Disabled || mode == QIcon::Selected) {
                icon.addPixmap(p1, oppositeMode, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p1.size());

                icon.addPixmap(p2, oppositeMode, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p2.size());

                icon.addPixmap(p3, QIcon::Active, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p3.size());

                icon.addPixmap(p4, QIcon::Normal, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p4.size());

                icon.addPixmap(p5, mode, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p5.size());

                icon.addPixmap(p6, QIcon::Active, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p6.size());

                icon.addPixmap(p7, QIcon::Normal, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p7.size());

                icon.addPixmap(p8, mode, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p8.size());
            } else {
                icon.addPixmap(p1, QIcon::Selected, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p1.size());

                icon.addPixmap(p2, QIcon::Disabled, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p2.size());

                icon.addPixmap(p3, QIcon::Selected, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p3.size());

                icon.addPixmap(p4, QIcon::Disabled, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p4.size());

                icon.addPixmap(p5, oppositeMode, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p5.size());

                icon.addPixmap(p6, mode, oppositeState);
                QVERIFY(icon.pixmap(100, mode, state).size() == p6.size());

                icon.addPixmap(p7, oppositeMode, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p7.size());

                icon.addPixmap(p8, mode, state);
                QVERIFY(icon.pixmap(100, mode, state).size() == p8.size());
            }
        }
    }
}

void tst_QIcon::cacheKey()
{
    QIcon icon1("image.png");
    qint64 icon1_key = icon1.cacheKey();
    QIcon icon2 = icon1;

    QVERIFY(icon2.cacheKey() == icon1.cacheKey());
    icon2.detach();
    QVERIFY(icon2.cacheKey() != icon1.cacheKey());
    QVERIFY(icon1.cacheKey() == icon1_key);
}

void tst_QIcon::detach()
{
    QImage img(32, 32, QImage::Format_ARGB32_Premultiplied);
    img.fill(0xffff0000);
    QIcon icon1(QPixmap::fromImage(img));
    QIcon icon2 = icon1;
    icon2.addFile("image.png", QSize(64, 64));

    QImage img1 = icon1.pixmap(64, 64).toImage();
    QImage img2 = icon2.pixmap(64, 64).toImage();
    QVERIFY(img1 != img2);

    img1 = icon1.pixmap(32, 32).toImage();
    img2 = icon2.pixmap(32, 32).toImage();
    QVERIFY(img1 == img2);
}

void tst_QIcon::svg()
{
    QIcon icon1("heart.svg");

    QVERIFY(!icon1.pixmap(32).isNull());
    QImage img1 = icon1.pixmap(32).toImage();
    QVERIFY(!icon1.pixmap(32, QIcon::Disabled).isNull());
    QImage img2 = icon1.pixmap(32, QIcon::Disabled).toImage();

    icon1.addFile("trash.svg", QSize(), QIcon::Disabled);
    QVERIFY(!icon1.pixmap(32, QIcon::Disabled).isNull());
    QImage img3 = icon1.pixmap(32, QIcon::Disabled).toImage();
    QVERIFY(img3 != img2);
    QVERIFY(img3 != img1);

    QPixmap pm("image.png");
    icon1.addPixmap(pm, QIcon::Normal, QIcon::On);
    QVERIFY(!icon1.pixmap(128, QIcon::Normal, QIcon::On).isNull());
    QImage img4 = icon1.pixmap(128, QIcon::Normal, QIcon::On).toImage();
    QVERIFY(img4 != img3);
    QVERIFY(img4 != img2);
    QVERIFY(img4 != img1);

    QIcon icon2;
    icon2.addFile("heart.svg");
    QVERIFY(icon2.pixmap(57).toImage() == icon1.pixmap(57).toImage());

    QIcon icon3("trash.svg");
    icon3.addFile("heart.svg");
    QVERIFY(icon3.pixmap(57).toImage() == icon1.pixmap(57).toImage());

    QIcon icon4("heart.svg");
    icon4.addFile("image.png", QSize(), QIcon::Active);
    QVERIFY(!icon4.pixmap(32).isNull());
    QVERIFY(!icon4.pixmap(32, QIcon::Active).isNull());
    QVERIFY(icon4.pixmap(32).toImage() == img1);
    QIcon pmIcon(pm);
    QVERIFY(icon4.pixmap(pm.size(), QIcon::Active).toImage() == pmIcon.pixmap(pm.size(), QIcon::Active).toImage());

#ifndef QT_NO_COMPRESS
    QIcon icon5("heart.svgz");
    QVERIFY(!icon5.pixmap(32).isNull());
#endif
}

void tst_QIcon::addFile()
{
    QIcon icon;
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png"));
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png"));
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-128.png"));
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-16.png"), QSize(), QIcon::Selected);
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-32.png"), QSize(), QIcon::Selected);
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-128.png"), QSize(), QIcon::Selected);

#ifdef Q_OS_WINCE
    // WinCE only includes the 16x16 images for size reasons
    QVERIFY(icon.pixmap(16, QIcon::Normal).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png")).toImage());
    QVERIFY(icon.pixmap(16, QIcon::Selected).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-16.png")).toImage());
#elif defined(Q_OS_SYMBIAN)
    // Symbian only includes the 32x32 images for size reasons
    QVERIFY(icon.pixmap(32, QIcon::Normal).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png")).toImage());
    QVERIFY(icon.pixmap(32, QIcon::Selected).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-32.png")).toImage());
#else
    QVERIFY(icon.pixmap(16, QIcon::Normal).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png")).toImage());
    QVERIFY(icon.pixmap(32, QIcon::Normal).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png")).toImage());
    QVERIFY(icon.pixmap(128, QIcon::Normal).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-128.png")).toImage());
    QVERIFY(icon.pixmap(16, QIcon::Selected).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-16.png")).toImage());
    QVERIFY(icon.pixmap(32, QIcon::Selected).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-32.png")).toImage());
    QVERIFY(icon.pixmap(128, QIcon::Selected).toImage() ==
            QPixmap(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-save-128.png")).toImage());
#endif
}

static bool sizeLess(const QSize &a, const QSize &b)
{
    return a.width() < b.width();
}

void tst_QIcon::availableSizes()
{
    {
        QIcon icon;
        icon.addFile("image.png", QSize(32,32));
        icon.addFile("image.png", QSize(64,64));
        icon.addFile("image.png", QSize(128,128));
        icon.addFile("image.png", QSize(256,256), QIcon::Disabled);
        icon.addFile("image.png", QSize(16,16), QIcon::Normal, QIcon::On);

        QList<QSize> availableSizes = icon.availableSizes();
        QCOMPARE(availableSizes.size(), 3);
        qSort(availableSizes.begin(), availableSizes.end(), sizeLess);
        QCOMPARE(availableSizes.at(0), QSize(32,32));
        QCOMPARE(availableSizes.at(1), QSize(64,64));
        QCOMPARE(availableSizes.at(2), QSize(128,128));

        availableSizes = icon.availableSizes(QIcon::Disabled);
        QCOMPARE(availableSizes.size(), 1);
        QCOMPARE(availableSizes.at(0), QSize(256,256));

        availableSizes = icon.availableSizes(QIcon::Normal, QIcon::On);
        QCOMPARE(availableSizes.size(), 1);
        QCOMPARE(availableSizes.at(0), QSize(16,16));
    }

    {
        // checks that there are no availableSizes for scalable images.
        QIcon icon("heart.svg");
        QList<QSize> availableSizes = icon.availableSizes();
        QVERIFY(availableSizes.isEmpty());
    }

    {
        // even if an a scalable image contain added pixmaps,
        // availableSizes still should be empty.
        QIcon icon("heart.svg");
        icon.addFile("image.png", QSize(32,32));
        QList<QSize> availableSizes = icon.availableSizes();
        QVERIFY(availableSizes.isEmpty());
    }

#ifndef Q_OS_SYMBIAN
    const int defaultDimension = 16;
    QLatin1String standardIcon(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png");
#else
    // In Symbian, only 32 times 32 icons are included.
    const int defaultDimension = 32;
    QLatin1String standardIcon(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png");
#endif

    {
        // we try to load an icon from resources
        QIcon icon(standardIcon);
        QList<QSize> availableSizes = icon.availableSizes();
        QCOMPARE(availableSizes.size(), 1);
        QCOMPARE(availableSizes.at(0), QSize(defaultDimension, defaultDimension));
    }

    {
        // load an icon from binary data.
        QPixmap pix;
        QFile file(standardIcon);
        QVERIFY(file.open(QIODevice::ReadOnly));
        uchar *data = file.map(0, file.size());
        QVERIFY(data != 0);
        pix.loadFromData(data, file.size());
        QIcon icon(pix);

        QList<QSize> availableSizes = icon.availableSizes();
        QCOMPARE(availableSizes.size(), 1);
        QCOMPARE(availableSizes.at(0), QSize(defaultDimension, defaultDimension));
    }

    {
        // there shouldn't be available sizes for invalid images!
        QVERIFY(QIcon(QLatin1String("")).availableSizes().isEmpty());
        QVERIFY(QIcon(QLatin1String("non-existing.png")).availableSizes().isEmpty());
    }
}

void tst_QIcon::name()
{
    {
        // No name if icon does not come from a theme
        QIcon icon(":/image.png");
        QString name = icon.name();
        QVERIFY(name.isEmpty());
    }

    {
        // Getting the name of an icon coming from a theme should work
        QString searchPath = QLatin1String(":/icons");
        QIcon::setThemeSearchPaths(QStringList() << searchPath);
        QString themeName("testtheme");
        QIcon::setThemeName(themeName);

        QIcon icon = QIcon::fromTheme("appointment-new");
        QString name = icon.name();
        QCOMPARE(name, QLatin1String("appointment-new"));
    }
}

void tst_QIcon::streamAvailableSizes_data()
{
    QTest::addColumn<QIcon>("icon");

    QIcon icon;
    icon.addFile(":/image.png", QSize(32,32));
    QTest::newRow( "32x32" ) << icon;
    icon.addFile(":/image.png", QSize(64,64));
    QTest::newRow( "64x64" ) << icon;
    icon.addFile(":/image.png", QSize(128,128));
    QTest::newRow( "128x128" ) << icon;
    icon.addFile(":/image.png", QSize(256,256));
    QTest::newRow( "256x256" ) << icon;
}

void tst_QIcon::streamAvailableSizes()
{
    QFETCH(QIcon, icon);

    QByteArray ba;
    // write to QByteArray
    {
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        QDataStream stream(&buffer);
        stream << icon;
    }

    // read from QByteArray
    {
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadOnly);
        QDataStream stream(&buffer);
        QIcon i;
        stream >> i;
        QCOMPARE(i.isNull(), icon.isNull());
        QCOMPARE(i.availableSizes(), icon.availableSizes());
    }
}


static inline bool operator<(const QSize &lhs, const QSize &rhs)
{
    if (lhs.width() < rhs.width())
        return true;
    else if (lhs.width() == lhs.width())
        return lhs.height() < lhs.height();
    return false;
}

void tst_QIcon::task184901_badCache()
{
    QPixmap pm("image.png");
    QIcon icon(pm);

    //the disabled icon must have an effect (grayed)
    QVERIFY(icon.pixmap(32, QIcon::Normal).toImage() != icon.pixmap(32, QIcon::Disabled).toImage());

    icon.addPixmap(pm, QIcon::Disabled);
    //the disabled icon must now be the same as the normal one.
    QVERIFY( icon.pixmap(32, QIcon::Normal).toImage() == icon.pixmap(32, QIcon::Disabled).toImage() );
}

void tst_QIcon::fromTheme()
{
    QString searchPath = QLatin1String(":/icons");
    QIcon::setThemeSearchPaths(QStringList() << searchPath);
    QVERIFY(QIcon::themeSearchPaths().size() == 1);
    QCOMPARE(searchPath, QIcon::themeSearchPaths()[0]);

    QString themeName("testtheme");
    QIcon::setThemeName(themeName);
    QCOMPARE(QIcon::themeName(), themeName);

    // Test normal icon
    QIcon appointmentIcon = QIcon::fromTheme("appointment-new");
    QVERIFY(!appointmentIcon.isNull());
    QVERIFY(!appointmentIcon.availableSizes(QIcon::Normal, QIcon::Off).isEmpty());
    QVERIFY(appointmentIcon.availableSizes().contains(QSize(16, 16)));
    QVERIFY(appointmentIcon.availableSizes().contains(QSize(32, 32)));
    QVERIFY(appointmentIcon.availableSizes().contains(QSize(22, 22)));

    // Test icon from parent theme
    QIcon abIcon = QIcon::fromTheme("address-book-new");
    QVERIFY(!abIcon.isNull());
    QVERIFY(QIcon::hasThemeIcon("address-book-new"));
    QVERIFY(!abIcon.availableSizes().isEmpty());

    // Test non existing icon
    QIcon noIcon = QIcon::fromTheme("broken-icon");
    QVERIFY(noIcon.isNull());
    QVERIFY(!QIcon::hasThemeIcon("broken-icon"));

    // Test non existing icon with fallback
    noIcon = QIcon::fromTheme("broken-icon", abIcon);
    QVERIFY(noIcon.cacheKey() == abIcon.cacheKey());

    // Test svg-only icon
    noIcon = QIcon::fromTheme("svg-icon", abIcon);
    QVERIFY(!noIcon.availableSizes().isEmpty());

    QByteArray ba;
    // write to QByteArray
    {
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        QDataStream stream(&buffer);
        stream << abIcon;
    }

    // read from QByteArray
    {
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadOnly);
        QDataStream stream(&buffer);
        QIcon i;
        stream >> i;
        QCOMPARE(i.isNull(), abIcon.isNull());
        QCOMPARE(i.availableSizes(), abIcon.availableSizes());
    }

    // Make sure setting the theme name clears the state
    QIcon::setThemeName("");
    abIcon = QIcon::fromTheme("address-book-new");
    QVERIFY(abIcon.isNull());
}


void tst_QIcon::task223279_inconsistentAddFile()
{
#ifndef Q_OS_SYMBIAN
    const int defaultDimension = 16;
    const int invalidIconDimension = 32;
    QLatin1String standardIcon(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png");
#else
    // Only 32 times 32 icons are included in Symbian
    const int defaultDimension = 32;
    const int invalidIconDimension = 64;
    QLatin1String standardIcon(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png");
#endif

    QIcon icon1;
    icon1.addFile(standardIcon);
    icon1.addFile(QLatin1String("IconThatDoesntExist"), QSize(invalidIconDimension, invalidIconDimension));
    QPixmap pm1 = icon1.pixmap(invalidIconDimension, invalidIconDimension);

    QIcon icon2;
    icon2.addFile(standardIcon);
    icon2.addFile(QLatin1String("IconThatDoesntExist"));
    QPixmap pm2 = icon1.pixmap(invalidIconDimension, invalidIconDimension);

    QCOMPARE(pm1.isNull(), false);
    QCOMPARE(pm1.size(), QSize(defaultDimension, defaultDimension));
    QCOMPARE(pm1.isNull(), pm2.isNull());
    QCOMPARE(pm1.size(), pm2.size());
}


// During detach, v2 engines are cloned, while v1 engines are only
// passed on, so v1 engines need to be referenced counted. This test
// verifies that the engine is destroyed once and only once.

class IconEngine : public QIconEngine
{
public:
    ~IconEngine() { destructorCalled++; }
    virtual void paint(QPainter *, const QRect &, QIcon::Mode, QIcon::State) { }
    static int destructorCalled;
};
int IconEngine::destructorCalled = 0;

void tst_QIcon::task239461_custom_iconengine_crash()
{
    QIconEngine *engine = new IconEngine();
    {
        QIcon icon(engine);
        QIcon icon2 = icon;

        QPixmap pixmap(32, 32);
        icon.addPixmap(pixmap);
    }
    QCOMPARE(IconEngine::destructorCalled, 1);
}

void tst_QIcon::highdpi()
{
    QIcon icon;
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-16.png"));
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-32.png"));
    icon.addFile(QLatin1String(":/trolltech/styles/commonstyle/images/standardbutton-open-128.png"));
}

QTEST_MAIN(tst_QIcon)
#include "tst_qicon.moc"
