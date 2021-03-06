/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

// #define QBBSCREEN_DEBUG

#include "qbbscreen.h"
#include "qbbrootwindow.h"
#include "qbbwindow.h"

#include <QDebug>
#include <QtCore/QThread>
#include <QtGui/QWindowSystemInterface>

#include <errno.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

QBBScreen::QBBScreen(screen_context_t context, screen_display_t display, int screenIndex)
    : mContext(context),
      mDisplay(display),
      mPosted(false),
      mUsingOpenGL(false),
      mPrimaryDisplay(screenIndex == 0),
      mKeyboardHeight(0),
      mScreenIndex(screenIndex)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::QBBScreen";
#endif

    // cache initial orientation of this display
    // TODO: use ORIENTATION environment variable?
    errno = 0;
    int result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_ROTATION, &mStartRotation);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display rotation, errno=%d", errno);
    }

    mCurrentRotation = mStartRotation;

    // cache size of this display in pixels
    // TODO: use WIDTH and HEIGHT environment variables?
    errno = 0;
    int val[2];
    result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_SIZE, val);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display size, errno=%d", errno);
    }

    mCurrentGeometry = mStartGeometry = QRect(0, 0, val[0], val[1]);

    // cache size of this display in millimeters
    errno = 0;
    result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_PHYSICAL_SIZE, val);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display physical size, errno=%d", errno);
    }

    // Peg the DPI to 96 (for now) so fonts are a reasonable size. We'll want to match
    // everything with a QStyle later, and at that point the physical size can be used
    // instead.
    {
        static const int dpi = 96;
        int width = mCurrentGeometry.width() / dpi * qreal(25.4) ;
        int height = mCurrentGeometry.height() / dpi * qreal(25.4) ;

        mCurrentPhysicalSize = mStartPhysicalSize = QSize(width,height);
    }

    // We only create the root window if we are not the primary display.
    if (mPrimaryDisplay)
        mRootWindow = QSharedPointer<QBBRootWindow>(new QBBRootWindow(this));
}

QBBScreen::~QBBScreen()
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::~QBBScreen";
#endif
}

static int defaultDepth()
{
    static int defaultDepth = 0;
    if (defaultDepth == 0) {
        // check if display depth was specified in environment variable;
        // use default value if no valid value found
        defaultDepth = qgetenv("QBB_DISPLAY_DEPTH").toInt();
        if (defaultDepth != 16 && defaultDepth != 32) {
            defaultDepth = 32;
        }
    }
    return defaultDepth;
}

void QBBScreen::ensureDisplayCreated()
{
    if (!mRootWindow)
        mRootWindow = QSharedPointer<QBBRootWindow>(new QBBRootWindow(this));
}

void QBBScreen::newWindowCreated(screen_window_t window)
{
    Q_ASSERT(thread() == QThread::currentThread());
    screen_display_t display = 0;
    if (screen_get_window_property_pv(window, SCREEN_PROPERTY_DISPLAY, (void**)&display) != 0) {
        qWarning("QBBScreen: Failed to get screen for window, errno=%d", errno);
        return;
    }

    if (display == nativeDisplay()) {
        // A window was created on this screen. If we don't know about this window yet, it means
        // it was not created by Qt, but by some foreign library like the multimedia renderer, which
        // creates an overlay window when playing a video.
        // Treat all foreign windows as overlays here.
        if (!findWindow(window))
            addOverlayWindow(window);
    }
}

void QBBScreen::windowClosed(screen_window_t window)
{
    Q_ASSERT(thread() == QThread::currentThread());
    removeOverlayWindow(window);
}

QRect QBBScreen::availableGeometry() const
{
    // available geometry = total geometry - keyboard
    return QRect(mCurrentGeometry.x(), mCurrentGeometry.y(),
                 mCurrentGeometry.width(), mCurrentGeometry.height() - mKeyboardHeight);
}

int QBBScreen::depth() const
{
    return defaultDepth();
}

/*!
    Check if the supplied angles are perpendicular to each other.
*/
static bool isOrthogonal(int angle1, int angle2)
{
    return ((angle1 - angle2) % 180) != 0;
}

void QBBScreen::setRotation(int rotation)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::setRotation, o=" << rotation;
#endif

    // check if rotation changed
    if (mCurrentRotation != rotation) {
        // update rotation of root window
        if (mRootWindow)
            mRootWindow->setRotation(rotation);

        const QRect previousScreenGeometry = geometry();

        // swap dimensions if we've rotated 90 or 270 from initial orientation
        if (isOrthogonal(mStartRotation, rotation)) {
            mCurrentGeometry = QRect(0, 0, mStartGeometry.height(), mStartGeometry.width());
            mCurrentPhysicalSize = QSize(mStartPhysicalSize.height(), mStartPhysicalSize.width());
        } else {
            mCurrentGeometry = QRect(0, 0, mStartGeometry.width(), mStartGeometry.height());
            mCurrentPhysicalSize = mStartPhysicalSize;
        }

        // resize app window if we've rotated 90 or 270 from previous orientation
        if (isOrthogonal(mCurrentRotation, rotation)) {

#if defined(QBBSCREEN_DEBUG)
            qDebug() << "QBBScreen::setRotation - resize, s=" << mCurrentGeometry.size();
#endif
            if (mRootWindow)
                mRootWindow->resize(mCurrentGeometry.size());

            if (mPrimaryDisplay)
                resizeWindows(previousScreenGeometry);
        } else {
            // TODO: find one global place to flush display updates
#if defined(QBBSCREEN_DEBUG)
            qDebug() << "QBBScreen::setRotation - flush";
#endif
            // force immediate display update if no geometry changes required
            if (mRootWindow)
                mRootWindow->flush();
        }

        // save new rotation
        mCurrentRotation = rotation;

        // TODO: check if other screens are supposed to rotate as well and/or whether this depends
        // on if clone mode is being used.
        // Rotating only the primary screen is what we had in the navigator event handler before refactoring
        if (mPrimaryDisplay)
            QWindowSystemInterface::handleScreenGeometryChange(mScreenIndex);

        // Flush everything, so that the windows rotations are applied properly.
        // Needed for non-maximized windows
        screen_flush_context(mContext, 0);
    }
}

void QBBScreen::resizeNativeWidgetWindow(QBBWindow *w, const QRect &previousScreenGeometry) const
{
    const qreal relativeX = static_cast<qreal>(w->geometry().topLeft().x()) / previousScreenGeometry.width();
    const qreal relativeY = static_cast<qreal>(w->geometry().topLeft().y()) / previousScreenGeometry.height();
    const qreal relativeWidth = static_cast<qreal>(w->geometry().width()) / previousScreenGeometry.width();
    const qreal relativeHeight = static_cast<qreal>(w->geometry().height()) / previousScreenGeometry.height();

    const QRect windowGeometry(relativeX * geometry().width(), relativeY * geometry().height(),
            relativeWidth * geometry().width(), relativeHeight * geometry().height());

    w->widget()->setGeometry(windowGeometry);
}

/*!
  Resize the given window to fit the screen geometry
*/
void QBBScreen::resizeTopLevelWindow(QBBWindow *w, const QRect &previousScreenGeometry) const
{
    QRect windowGeometry = w->geometry();

    const qreal relativeCenterX = static_cast<qreal>(w->geometry().center().x()) / previousScreenGeometry.width();
    const qreal relativeCenterY = static_cast<qreal>(w->geometry().center().y()) / previousScreenGeometry.height();
    const QPoint newCenter(relativeCenterX * geometry().width(), relativeCenterY * geometry().height());

    windowGeometry.moveCenter(newCenter);

    // adjust center position in case the window
    // is clipped
    if (!geometry().contains(windowGeometry)) {
        const int x1 = windowGeometry.x();
        const int y1 = windowGeometry.y();
        const int x2 = x1 + windowGeometry.width();
        const int y2 = y1 + windowGeometry.height();

        if (x1 < 0) {
            const int centerX = qMin(qAbs(x1) + windowGeometry.center().x(),
                                        geometry().center().x());

            windowGeometry.moveCenter(QPoint(centerX, windowGeometry.center().y()));
        }

        if (y1 < 0) {
            const int centerY = qMin(qAbs(y1) + windowGeometry.center().y(),
                                        geometry().center().y());

            windowGeometry.moveCenter(QPoint(windowGeometry.center().x(), centerY));
        }

        if (x2 > geometry().width()) {
            const int centerX = qMax(windowGeometry.center().x() - (x2 - geometry().width()),
                                        geometry().center().x());

            windowGeometry.moveCenter(QPoint(centerX, windowGeometry.center().y()));
        }

        if (y2 > geometry().height()) {
            const int centerY = qMax(windowGeometry.center().y() - (y2 - geometry().height()),
                                        geometry().center().y());

            windowGeometry.moveCenter(QPoint(windowGeometry.center().x(), centerY));
        }
    }

    // at this point, if the window is still clipped,
    // it means that it's too big to fit on the screen,
    // so we need to proportionally shrink it
    if (!geometry().contains(windowGeometry)) {
        QSize newSize = windowGeometry.size();
        newSize.scale(geometry().size(), Qt::KeepAspectRatio);
        windowGeometry.setSize(newSize);

        if (windowGeometry.x() < 0)
            windowGeometry.moveCenter(QPoint(geometry().center().x(), windowGeometry.center().y()));

        if (windowGeometry.y() < 0)
            windowGeometry.moveCenter(QPoint(windowGeometry.center().x(), geometry().center().y()));
    }

    w->widget()->setGeometry(windowGeometry);
}

/*!
  Adjust windows to the new screen geometry.
*/
void QBBScreen::resizeWindows(const QRect &previousScreenGeometry)
{
    Q_FOREACH (QBBWindow *w, mChildren) {

        if (w->widget()->windowState() & Qt::WindowFullScreen || w->widget()->windowState() & Qt::WindowMaximized)
            continue;

        if (w->widget()->parent()) {
            // This is a native (non-alien) widget window
            resizeNativeWidgetWindow(w, previousScreenGeometry);
        } else {
            // This is a toplevel window
            resizeTopLevelWindow(w, previousScreenGeometry);
        }
    }
}

QBBWindow *QBBScreen::findWindow(screen_window_t windowHandle)
{
    Q_FOREACH (QBBWindow *window, mChildren) {
        QBBWindow * const result = window->findWindow(windowHandle);
        if (result)
            return result;
    }

    return 0;
}

void QBBScreen::addWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::addWindow=" << window;
#endif

    if (mChildren.contains(window))
        return;

    // Ensure that the desktop window is at the bottom of the zorder.
    // If we do not do this then we may end up activating the desktop
    // when the navigator service gets an event that our window group
    // has been activated (see QBBScreen::activateWindowGroup()).
    // Such a situation would strangely break focus handling due to the
    // invisible desktop widget window being layered on top of normal
    // windows
    if (window->widget()->windowType() == Qt::Desktop)
        mChildren.push_front(window);
    else
        mChildren.push_back(window);
    updateHierarchy();
}

void QBBScreen::removeWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::removeWindow=" << window;
#endif

    const int numWindowsRemoved = mChildren.removeAll(window);
    if (numWindowsRemoved > 0)
        updateHierarchy();
}

void QBBScreen::raiseWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::raise window=" << window;
#endif

    removeWindow(window);
    mChildren.push_back(window);
    updateHierarchy();
}

void QBBScreen::lowerWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::lower window=" << window;
#endif

    removeWindow(window);
    mChildren.push_front(window);
    updateHierarchy();
}

void QBBScreen::updateHierarchy()
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::updateHierarchy";
#endif

    QList<QBBWindow*>::const_iterator it;
    int topZorder = 1; // root window is z-order 0, all "top" level windows are "above" it

    for (it = mChildren.constBegin(); it != mChildren.constEnd(); ++it) {
        (*it)->updateZorder(topZorder);
    }

    topZorder++;
    Q_FOREACH (screen_window_t overlay, mOverlays) {
        // Do nothing when this fails. This can happen if we have stale windows in mOverlays,
        // which in turn can happen because a window was removed but we didn't get a notification
        // yet.
        screen_set_window_property_iv(overlay, SCREEN_PROPERTY_ZORDER, &topZorder);
        topZorder++;
    }

    // After a hierarchy update, we need to force a flush on all screens.
    // Right now, all screens share a context.
    screen_flush_context( mContext, 0 );
}

void QBBScreen::onWindowPost(QBBWindow* window)
{
    Q_UNUSED(window)

    // post app window (so navigator will show it) after first child window
    // has posted; this only needs to happen once as the app window's content
    // never changes
    if (!mPosted && mRootWindow) {
        mRootWindow->post();
        mPosted = true;
    }
}

void QBBScreen::keyboardHeightChanged(int height)
{
    if (height == mKeyboardHeight)
        return;

    mKeyboardHeight = height;

    QWindowSystemInterface::handleScreenAvailableGeometryChange(mScreenIndex);
}

void QBBScreen::addOverlayWindow(screen_window_t window)
{
    mOverlays.append(window);
    updateHierarchy();
}

void QBBScreen::removeOverlayWindow(screen_window_t window)
{
    const int numOverlaysRemoved = mOverlays.removeAll(window);
    if (numOverlaysRemoved > 0)
        updateHierarchy();
}

void QBBScreen::activateWindowGroup(const QByteArray &id)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif

    if (!rootWindow() || id != rootWindow()->groupName())
        return;

    if (!mChildren.isEmpty()) {
        // We're picking up the last window of the list here
        // because this list is ordered by stacking order.
        // Last window is effectively the one on top.
        QWidget * const window = mChildren.last()->widget();
        QWindowSystemInterface::handleWindowActivated(window);
    }
}

void QBBScreen::deactivateWindowGroup(const QByteArray &id)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif

    if (!rootWindow() || id != rootWindow()->groupName())
        return;

    QWindowSystemInterface::handleWindowActivated(0);
}

QT_END_NAMESPACE
