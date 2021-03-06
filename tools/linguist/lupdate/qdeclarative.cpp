/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "lupdate.h"

#include <translator.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "parser/qdeclarativejsengine_p.h"
#include "parser/qdeclarativejsparser_p.h"
#include "parser/qdeclarativejslexer_p.h"
#include "parser/qdeclarativejsnodepool_p.h"
#include "parser/qdeclarativejsastvisitor_p.h"
#include "parser/qdeclarativejsast_p.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>
#include <QStringList>

#include <iostream>
#include <cstdlib>

QT_BEGIN_NAMESPACE

using namespace QDeclarativeJS;

class FindTrCalls: protected AST::Visitor
{
public:
    void operator()(Translator *translator, const QString &fileName, AST::Node *node)
    {
        m_translator = translator;
        m_fileName = fileName;
        m_component = QFileInfo(fileName).baseName();   //matches qsTr usage in QScriptEngine
        accept(node);
    }

protected:
    using AST::Visitor::visit;
    using AST::Visitor::endVisit;

    void accept(AST::Node *node)
    { AST::Node::acceptChild(node, this); }

    virtual void endVisit(AST::CallExpression *node)
    {
        if (AST::IdentifierExpression *idExpr = AST::cast<AST::IdentifierExpression *>(node->base)) {
            if (idExpr->name->asString() == QLatin1String("qsTr") ||
                idExpr->name->asString() == QLatin1String("QT_TR_NOOP")) {
                if (node->arguments && AST::cast<AST::StringLiteral *>(node->arguments->expression)) {
                    AST::StringLiteral *literal = AST::cast<AST::StringLiteral *>(node->arguments->expression);
                    const QString source = literal->value->asString();

                    QString comment;
                    bool plural = false;
                    AST::ArgumentList *commentNode = node->arguments->next;
                    if (commentNode) {
                        literal = AST::cast<AST::StringLiteral *>(commentNode->expression);
                        comment = literal->value->asString();

                        AST::ArgumentList *nNode = commentNode->next;
                        if (nNode) {
                            AST::NumericLiteral *numLiteral = AST::cast<AST::NumericLiteral *>(nNode->expression);
                            if (numLiteral) {
                                plural = true;
                            }
                        }
                    }

                    TranslatorMessage msg(m_component, source,
                        comment, QString(), m_fileName,
                        node->firstSourceLocation().startLine, QStringList(),
                        TranslatorMessage::Unfinished, plural);
                    m_translator->extend(msg);
                }
            } else if (idExpr->name->asString() == QLatin1String("qsTranslate") ||
                       idExpr->name->asString() == QLatin1String("QT_TRANSLATE_NOOP")) {
                if (node->arguments && AST::cast<AST::StringLiteral *>(node->arguments->expression)) {
                    AST::StringLiteral *literal = AST::cast<AST::StringLiteral *>(node->arguments->expression);
                    const QString context = literal->value->asString();

                    QString source;
                    QString comment;
                    bool plural = false;
                    AST::ArgumentList *sourceNode = node->arguments->next;
                    if (sourceNode) {
                        literal = AST::cast<AST::StringLiteral *>(sourceNode->expression);
                        source = literal->value->asString();
                        AST::ArgumentList *commentNode = sourceNode->next;
                        if (commentNode) {
                            literal = AST::cast<AST::StringLiteral *>(commentNode->expression);
                            comment = literal->value->asString();

                            AST::ArgumentList *nNode = commentNode->next;
                            if (nNode) {
                                AST::NumericLiteral *numLiteral = AST::cast<AST::NumericLiteral *>(nNode->expression);
                                if (numLiteral) {
                                    plural = true;
                                }
                            }
                        }
                    }

                    TranslatorMessage msg(context, source,
                        comment, QString(), m_fileName,
                        node->firstSourceLocation().startLine, QStringList(),
                        TranslatorMessage::Unfinished, plural);
                    m_translator->extend(msg);
                }

            }
        }
    }

private:
    Translator *m_translator;
    QString m_fileName;
    QString m_component;
};

QString createErrorString(const QString &filename, const QString &code, Parser &parser)
{
    // print out error
    QStringList lines = code.split(QLatin1Char('\n'));
    lines.append(QLatin1String("\n")); // sentinel.
    QString errorString;

    foreach (const DiagnosticMessage &m, parser.diagnosticMessages()) {

        if (m.isWarning())
            continue;

        QString error = filename + QLatin1Char(':') + QString::number(m.loc.startLine)
                        + QLatin1Char(':') + QString::number(m.loc.startColumn) + QLatin1String(": error: ")
                        + m.message + QLatin1Char('\n');

        int line = 0;
        if (m.loc.startLine > 0)
            line = m.loc.startLine - 1;

        const QString textLine = lines.at(line);

        error += textLine + QLatin1Char('\n');

        int column = m.loc.startColumn - 1;
        if (column < 0)
            column = 0;

        column = qMin(column, textLine.length());

        for (int i = 0; i < column; ++i) {
            const QChar ch = textLine.at(i);
            if (ch.isSpace())
                error += ch.unicode();
            else
                error += QLatin1Char(' ');
        }
        error += QLatin1String("^\n");
        errorString += error;
    }
    return errorString;
}

bool loadQml(Translator &translator, const QString &filename, ConversionData &cd)
{
    cd.m_sourceFileName = filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        cd.appendError(QString::fromLatin1("Cannot open %1: %2")
            .arg(filename, file.errorString()));
        return false;
    }

    const QString code = QTextStream(&file).readAll();

    Engine driver;
    Parser parser(&driver);

    NodePool nodePool(filename, &driver);
    driver.setNodePool(&nodePool);

    Lexer lexer(&driver);
    lexer.setCode(code, /*line = */ 1);
    driver.setLexer(&lexer);

    if (parser.parse()) {
        FindTrCalls trCalls;
        trCalls(&translator, filename, parser.ast());
    } else {
        QString error = createErrorString(filename, code, parser);
        cd.appendError(error);
        return false;
    }
    return true;
}

QT_END_NAMESPACE
