/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

QString tr(const char *text)
{
    return QApplication::translate(text, text);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextEdit *editor = new QTextEdit;

//! [0]
    QTextDocument *document = editor->document();
    QTextCursor redCursor(document);
//! [0] //! [1]
    QTextCursor blueCursor(document);
//! [1]

    QTextCharFormat redFormat(redCursor.charFormat());
    redFormat.setForeground(Qt::red);
    QTextCharFormat blueFormat(blueCursor.charFormat());
    blueFormat.setForeground(Qt::blue);

    redCursor.setCharFormat(redFormat);
    blueCursor.setCharFormat(blueFormat);

    for (int i = 0; i < 20; ++i) {
        if (i % 2 == 0)
            redCursor.insertText(tr("%1 ").arg(i), redFormat);
        if (i % 5 == 0)
            blueCursor.insertText(tr("%1 ").arg(i), blueFormat);
    }

    editor->setWindowTitle(tr("Text Document Cursors"));
    editor->resize(320, 480);
    editor->show();
    return app.exec();
}
