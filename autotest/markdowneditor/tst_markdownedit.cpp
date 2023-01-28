/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "tst_markdownedit.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTest>
#include <QTextStream>
#include <QThread>

#include "../../src/markdowneditor.h"
#include "../../src/markdowneditor_p.h"

using namespace ghostwriter;
/**
 * Unit test for the AsyncTextWriter class.
 */

void MarkDownEditTest::initTestCase()
{
    m_mdDocument = std::make_unique<MarkdownDocument>();
    m_colors = std::make_unique<ColorScheme>();
    m_edit = std::make_unique<MarkdownEditor>(m_mdDocument.get(), *(m_colors.get()));
}

/**
 * OBJECTIVE:
 *      Test all cases for multiline edition
 *
 * INPUTS:
 *      - Markdown text and modifications
 *
 * EXPECTED RESULTS:
 *      - Markdown text is modified as expected.
 *      - Cursors are in expected number and at the right position
 */
void MarkDownEditTest::multilineEditionTest()
{
    m_mdDocument->setPlainText(QString("* line1\n* line2\n* line3\n* line4\n* line5\n"));

    m_edit->show();
    // set pointer at the first character
    m_edit->setTextCursor(m_edit->cursorForPosition({0, 0}));

    QTest::keyClick(m_edit.get(), Qt::Key_Right);
    QTest::keyClick(m_edit.get(), Qt::Key_Down);
    QTest::keyClick(m_edit.get(), Qt::Key_Right);

    auto d = m_edit->d_ptr.data();

    QCOMPARE(d->moveCount, 0);
    QCOMPARE(d->cursorRects.size(), 1);

    // Keyboard combo to edit the second line of text and 2 lines under.
    QTest::keyClick(m_edit.get(), Qt::Key_Down, Qt::ShiftModifier | Qt::AltModifier);
    QTest::keyClick(m_edit.get(), Qt::Key_Down, Qt::ShiftModifier | Qt::AltModifier);

    QCOMPARE(d->moveCount, 2);
    QCOMPARE(d->cursorRects.size(), 3);

    // typing a word and check that it has been added on 3 lines.
    QTest::keyClicks(m_edit.get(), "test");
    QCOMPARE(m_mdDocument->toPlainText(), QString("* line1\n* testline2\n* testline3\n* testline4\n* line5\n"));

    // Go to the end of the line but before the \n and add 0 on 3 lines
    QTest::keyClick(m_edit.get(), Qt::Key_Right, Qt::ControlModifier);
    QTest::keyClicks(m_edit.get(), "0");
    QCOMPARE(m_mdDocument->toPlainText(), QString("* line1\n* testline20\n* testline30\n* testline40\n* line5\n"));

    // Remove number on those 3 lines
    QTest::keyClick(m_edit.get(), Qt::Key_Backspace);
    QTest::keyClick(m_edit.get(), Qt::Key_Backspace);
    QCOMPARE(m_mdDocument->toPlainText(), QString("* line1\n* testline\n* testline\n* testline\n* line5\n"));

    // Go back to the beginning and then move to the last line
    m_edit->setTextCursor(m_edit->cursorForPosition({0, 0}));
    QTest::keyClick(m_edit.get(), Qt::Key_Right);
    QTest::keyClick(m_edit.get(), Qt::Key_Right);
    QTest::keyClick(m_edit.get(), Qt::Key_Down);
    QTest::keyClick(m_edit.get(), Qt::Key_Down);
    QTest::keyClick(m_edit.get(), Qt::Key_Down);
    QTest::keyClick(m_edit.get(), Qt::Key_Down);

    QCOMPARE(d->moveCount, 0);
    QCOMPARE(d->cursorRects.size(), 1);

    // Select all lines from the bottom to the top
    QTest::keyClick(m_edit.get(), Qt::Key_Up, Qt::ShiftModifier | Qt::AltModifier);
    QTest::keyClick(m_edit.get(), Qt::Key_Up, Qt::ShiftModifier | Qt::AltModifier);
    QTest::keyClick(m_edit.get(), Qt::Key_Up, Qt::ShiftModifier | Qt::AltModifier);
    QTest::keyClick(m_edit.get(), Qt::Key_Up, Qt::ShiftModifier | Qt::AltModifier);
    QTest::keyClick(m_edit.get(), Qt::Key_Up, Qt::ShiftModifier | Qt::AltModifier);

    QCOMPARE(d->moveCount, -4);
    QCOMPARE(d->cursorRects.size(), 5);

    QTest::keyClick(m_edit.get(), Qt::Key_Left);
    QTest::keyClick(m_edit.get(), Qt::Key_Delete);
    QTest::keyClick(m_edit.get(), Qt::Key_Backspace);

    QCOMPARE(d->moveCount, -4);
    QCOMPARE(d->cursorRects.size(), 5);
    QCOMPARE(m_mdDocument->toPlainText(), QString("line1\ntestline\ntestline\ntestline\nline5\n"));
}

QTEST_MAIN(MarkDownEditTest)
#include "tst_markdownedit.moc"
