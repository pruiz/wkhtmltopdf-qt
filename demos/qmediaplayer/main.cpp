/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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
***************************************************************************/

#include <QtGui>
#include "mediaplayer.h"

int main (int argc, char *argv[])
{
    Q_INIT_RESOURCE(mediaplayer);
    QApplication app(argc, argv);
    app.setApplicationName("Media Player");
    app.setOrganizationName("Qt");
    app.setQuitOnLastWindowClosed(true);

    bool hasSmallScreen =
#ifdef Q_OS_SYMBIAN
        /* On Symbian, we always want fullscreen. One reason is that it's not
         * possible to launch any demos from the fluidlauncher due to a
         * limitation in the emulator. */
        true
#else
        false
#endif
    ;

    QString fileString;
    const QStringList args(app.arguments());
    /* We have a minor problem here, we accept two arguments, both are
     * optional:
     * - A file name
     * - the option "-small-screen", so let's try to cope with that.
     */
    for (int i = 0; i < args.count(); ++i) {
        const QString &at = args.at(i);

        if (at == QLatin1String("-small-screen"))
            hasSmallScreen = true;
        else if (i > 0) // We don't want the app name.
            fileString = at;
    }

    MediaPlayer player(fileString, hasSmallScreen);

    if (hasSmallScreen)
        player.showMaximized();
    else
        player.show();

    return app.exec();
}

