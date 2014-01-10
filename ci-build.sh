#!/bin/bash

QTDIR=/opt/wkhtmltopdf
QT_LIBDIR=$QTDIR/lib
QT_PLUGINDIR=$QTDIR/plugins
QT_IMPORTDIR=$QTDIR/imports

sed -i s/OPT_CONFIRM_LICENSE=no/OPT_CONFIRM_LICENSE=yes/g configure

# Build shared libraries
./configure \
-prefix $QTDIR \
-libdir $QT_LIBDIR \
-plugindir $QT_PLUGINDIR \
-importdir $QT_IMPORTDIR \
-opensource \
-shared \
-fast \
-no-accessibility \
-no-stl \
-no-sql-ibase \
-no-sql-mysql \
-no-sql-odbc \
-no-sql-psql \
-no-sql-sqlite \
-no-sql-sqlite2 \
-no-qt3support \
-no-phonon \
-no-phonon-backend \
-webkit-debug \
-no-scripttools \
-system-zlib \
-system-libtiff \
-system-libpng \
-system-libmng \
-system-libjpeg \
-graphicssystem raster \
-xrender \
-nomake tools \
-nomake examples \
-nomake demos \
-nomake docs \
-nomake translations \
-no-opengl \
-no-dbus \
-no-multimedia \
-exceptions \
-xmlpatterns \
-no-mmx \
-no-3dnow \
-no-sse \
-no-sse2 \
-no-ssse3 \
-no-neon \
-openssl \
-debug

make -j 4

