/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "message.h"

//! [Message class implementation]
Message::Message()
{
}

Message::Message(const Message &other)
{
    m_body = other.m_body;
    m_headers = other.m_headers;
}

Message::~Message()
{
}
//! [Message class implementation]

Message::Message(const QString &body, const QStringList &headers)
{
    m_body = body;
    m_headers = headers;
}

//! [custom type streaming operator]
QDebug operator<<(QDebug dbg, const Message &message)
{
    QStringList pieces = message.body().split("\r\n", QString::SkipEmptyParts);
    if (pieces.isEmpty())
        dbg.nospace() << "Message()";
    else if (pieces.size() == 1)
        dbg.nospace() << "Message(" << pieces.first() << ")";
    else
        dbg.nospace() << "Message(" << pieces.first() << " ...)";
    return dbg.maybeSpace();
}
//! [custom type streaming operator]

//! [getter functions]
QString Message::body() const
{
    return m_body;
}

QStringList Message::headers() const
{
    return m_headers;
}
//! [getter functions]
