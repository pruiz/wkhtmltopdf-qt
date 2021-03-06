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

#ifndef Patternist_UnionIterator_H
#define Patternist_UnionIterator_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the @c except operator. That is, the computation
     * of the sequence of nodes from one sequence, that doesn't appear in the
     * other.
     *
     * @ingroup Patternist_iterators
     */
    class UnionIterator : public Item::Iterator
    {
    public:
        /**
         * It is assumed that @p it1 and @p it2 are in document order and
         * without duplicates.
         */
        UnionIterator(const Item::Iterator::Ptr &it1,
                      const Item::Iterator::Ptr &it2);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual Item::Iterator::Ptr copy() const;

    private:
        inline Item nextFromFirstOperand()
        {
            ++m_position;
            m_current = m_node1;
            m_node1 = m_it1->next();
            return m_current;
        }

        const Item::Iterator::Ptr   m_it1;
        const Item::Iterator::Ptr   m_it2;
        Item                   m_current;
        xsInteger                   m_position;
        Item                   m_node1;
        Item                   m_node2;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
