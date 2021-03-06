/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVELISTMODEL_P_P_H
#define QDECLARATIVELISTMODEL_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qdeclarativelistmodel_p.h"

#include "qdeclarative.h"
#include "private/qdeclarativeengine_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeOpenMetaObject;
class QScriptEngine;
class QDeclarativeListModelWorkerAgent;
struct ModelNode;

class FlatListModel
{
public:
    FlatListModel(QDeclarativeListModel *base);
    ~FlatListModel();

    QHash<int,QVariant> data(int index, const QList<int> &roles) const;
    QVariant data(int index, int role) const;

    QList<int> roles() const;
    QString toString(int role) const;

    int count() const;
    void clear();
    void remove(int index);
    bool append(const QScriptValue&);
    bool insert(int index, const QScriptValue&);
    QScriptValue get(int index) const;
    void set(int index, const QScriptValue&, QList<int> *roles);
    void setProperty(int index, const QString& property, const QVariant& value, QList<int> *roles);
    void move(int from, int to, int count);

private:    
    friend class QDeclarativeListModelWorkerAgent;
    friend class QDeclarativeListModel;

    bool addValue(const QScriptValue &value, QHash<int, QVariant> *row, QList<int> *roles);

    QScriptEngine *m_scriptEngine;
    QHash<int, QString> m_roles;
    QHash<QString, int> m_strings;
    QList<QHash<int, QVariant> > m_values;
    QDeclarativeListModel *m_listModel;
};

class NestedListModel
{
public:
    NestedListModel(QDeclarativeListModel *base);
    ~NestedListModel();

    QHash<int,QVariant> data(int index, const QList<int> &roles, bool *hasNested = 0) const;
    QVariant data(int index, int role) const;

    QList<int> roles() const;
    QString toString(int role) const;

    int count() const;
    void clear();
    void remove(int index);
    bool append(const QScriptValue&);
    bool insert(int index, const QScriptValue&);
    QScriptValue get(int index) const;
    void set(int index, const QScriptValue&, QList<int> *roles);
    void setProperty(int index, const QString& property, const QVariant& value, QList<int> *role);
    void move(int from, int to, int count);

    QVariant valueForNode(ModelNode *, bool *hasNested = 0) const;
    void checkRoles() const;

    ModelNode *_root;
    QDeclarativeListModel *m_listModel;

private:
    mutable QStringList roleStrings;
    mutable bool _rolesOk;
};


class ModelObject : public QObject
{
    Q_OBJECT
public:
    ModelObject();
    void setValue(const QByteArray &name, const QVariant &val);

private:
    QDeclarativeOpenMetaObject *_mo;
};

struct ModelNode
{
    ModelNode();
    ~ModelNode();

    QList<QVariant> values;
    QHash<QString, ModelNode *> properties;

    QDeclarativeListModel *model(const NestedListModel *model) {
        if (!modelCache) { 
            modelCache = new QDeclarativeListModel;
            QDeclarativeEngine::setContextForObject(modelCache,QDeclarativeEngine::contextForObject(model->m_listModel));
            modelCache->m_nested->_root = this;  // ListModel defaults to nestable model
        }
        return modelCache;
    }

    ModelObject *object(const NestedListModel *model) {
        if (!objectCache) {
            objectCache = new ModelObject();
            QHash<QString, ModelNode *>::iterator it;
            for (it = properties.begin(); it != properties.end(); ++it) {
                objectCache->setValue(it.key().toUtf8(), model->valueForNode(*it));
            }
        }
        return objectCache;
    }


    void setObjectValue(const QScriptValue& valuemap);
    void setListValue(const QScriptValue& valuelist);
    void setProperty(const QString& prop, const QVariant& val);
    static void dump(ModelNode *node, int ind);

    QDeclarativeListModel *modelCache;
    ModelObject *objectCache;
    bool isArray;
};


QT_END_NAMESPACE

Q_DECLARE_METATYPE(ModelNode *)

QT_END_HEADER

#endif // QDECLARATIVELISTMODEL_P_P_H

