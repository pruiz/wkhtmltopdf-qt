/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "expectedeventlist.h"
#include <QDebug>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QtTest/QtTest>

ExpectedEventList::ExpectedEventList(QObject *target)
    : QObject(target), eventCount(0)
{
    target->installEventFilter(this);
    debug = !qgetenv("NATIVEDEBUG").isEmpty();
}

ExpectedEventList::~ExpectedEventList()
{
    qDeleteAll(eventList);
}

void ExpectedEventList::append(QEvent *e)
{
    eventList.append(e);
    ++eventCount;
}

void ExpectedEventList::timerEvent(QTimerEvent *)
{
    timer.stop();
    QAbstractEventDispatcher::instance()->interrupt();
}

bool ExpectedEventList::waitForAllEvents(int maxEventWaitTime)
{
    if (eventList.isEmpty())
        return true;

    int eventCount = eventList.size();
    timer.start(maxEventWaitTime, this);

    while (timer.isActive()) {
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        if (eventList.isEmpty())
            return true;

        if (eventCount < eventList.size()){
            eventCount = eventList.size();
            timer.start(maxEventWaitTime, this);
        }
    }

    int eventListNr = eventCount - eventList.size() + 1;
    qWarning() << "Stopped waiting for expected event nr" << eventListNr;
    return false;
}

void ExpectedEventList::compareMouseEvents(QEvent *received, QEvent *expected)
{
    QMouseEvent *e1 = static_cast<QMouseEvent *>(received);
    QMouseEvent *e2 = static_cast<QMouseEvent *>(expected);
    if (e1->pos() == e2->pos()
            && (e1->globalPos() == e2->globalPos())
            && (e1->button() == e2->button())
            && (e1->buttons() == e2->buttons())
            && (e1->modifiers() == e2->modifiers()))
        return; // equal

    int eventListNr = eventCount - eventList.size();
    if (!debug) {
        qWarning() << "Expected event" << eventListNr << "differs from received event:";
        QCOMPARE(e1->pos(), e2->pos());
        QCOMPARE(e1->globalPos(), e2->globalPos());
        QCOMPARE(e1->button(), e2->button());
        QCOMPARE(e1->buttons(), e2->buttons());
        QCOMPARE(e1->modifiers(), e2->modifiers());
    } else {
        qWarning() << "*** FAIL *** : Expected event" << eventListNr << "differs from received event:";
        qWarning() << "Received:" << e1 << e1->globalPos();
        qWarning() << "Expected:" << e2 << e2->globalPos();
    }
}

void ExpectedEventList::compareKeyEvents(QEvent *event1, QEvent *event2)
{
    QKeyEvent *e1 = static_cast<QKeyEvent *>(event1);
    QKeyEvent *e2 = static_cast<QKeyEvent *>(event2);
    Q_UNUSED(e1);
    Q_UNUSED(e2);
}

bool ExpectedEventList::eventFilter(QObject *, QEvent *received)
{
    if (debug)
        qDebug() << received;
    if (eventList.isEmpty())
        return false;

    QEvent *expected = eventList.first();
    if (expected->type() == received->type()) {
        eventList.removeFirst();
        switch (received->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::MouseButtonDblClick:
            case QEvent::NonClientAreaMouseButtonPress:
            case QEvent::NonClientAreaMouseButtonRelease:
            case QEvent::NonClientAreaMouseButtonDblClick:
            case QEvent::NonClientAreaMouseMove: {
                compareMouseEvents(received, expected);
                break;
            }
            case QEvent::KeyPress: {
                break;
            }
            case QEvent::KeyRelease: {
                break;
            }
            case QEvent::Resize: {
                break;
            }
            case QEvent::WindowActivate: {
                break;
            }
            case QEvent::WindowDeactivate: {
                break;
            }
            default:
                break;
        }
        if (eventList.isEmpty())
            QAbstractEventDispatcher::instance()->interrupt();
    }

    return false;
}

