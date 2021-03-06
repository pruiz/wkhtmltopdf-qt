/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_XSLTNodeTest_H
#define Patternist_XSLTNodeTest_H

#include "qanynodetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Represents @c node() in patterns in XSL-T, which
     * are just like how @c node() usually is, except that it doesn't
     * match document nodes.
     *
     * @see <a href="http://www.w3.org/TR/xslt20/#pattern-semantics">XSL
     * Transformations (XSLT) Version 2.0, 5.5.3 The Meaning of a Pattern</a>
     * @ingroup Patternist_types
     * @author Frans Englich <frans.englich@nokia.com>
     * @since 4.5
     */
    class XSLTNodeTest : public AnyNodeType
    {
    public:
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;
        virtual bool itemMatches(const Item &item) const;

        virtual ItemType::Ptr xdtSuperType() const;

    protected:
        friend class BuiltinTypes;

        /**
         * This constructor does nothing, but exists in order to make it impossible to
         * instantiate this class from anywhere but from BuiltinTypes.
         */
        inline XSLTNodeTest()
        {
        }
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
