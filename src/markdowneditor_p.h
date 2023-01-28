/***************************************************************************
 *	Copyright (C) 2023 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MARKDOWNEDIT_P_H
#define MARKDOWNEDIT_P_H

#include <QGridLayout>
#include <QRegularExpression>
#include <QTimer>

#include "markdowneditor.h"
#include "markdownhighlighter.h"

namespace ghostwriter
{
// Need to be in order of decreasing length
enum MarkupType {
    MarkupType_None = -1,
    MarkupType_BoldItalic, // Special case used for checking if bold + italic are both on
    MarkupType_Bold,
    MarkupType_StrikeThrough,
    MarkupType_Italic,
    MarkupType_Count
};

class MarkdownEditorPrivate
{
    Q_DECLARE_PUBLIC(MarkdownEditor)

public:
    MarkdownEditorPrivate(MarkdownEditor *q_ptr)
        : q_ptr(q_ptr)
    {
        ;
    }

    ~MarkdownEditorPrivate()
    {
        ;
    }

    typedef enum { BlockTypeNone, BlockTypeQuote, BlockTypeCode } BlockType;

    static const int CursorWidth = 2;
    const QString lineBreakChar = QString::fromUtf8("↵");

    // We use only image MIME types that are web-friendly so that any inserted
    // or pasted images can be displayed in the live preview.
    static const QStringList webMimeTypes;

    static QStringList imageReadFormats;
    static QStringList imageWriteFormats;
    static QString imageOpenFilter;
    static QString imageSaveFilter;

    MarkdownEditor *q_ptr;

    MarkdownDocument *textDocument;
    MarkdownHighlighter *highlighter;
    QGridLayout *preferredLayout;
    bool autoMatchEnabled;
    bool bulletPointCyclingEnabled;
    bool hemingwayModeEnabled;
    FocusMode focusMode;
    QBrush fadeColor;
    QColor blockColor;
    QColor whitespaceRenderColor;
    bool insertSpacesForTabs;
    int tabWidth;
    EditorWidth editorWidth;
    InterfaceStyle editorCorners;
    QRegularExpression emptyBlockquoteRegex;
    QRegularExpression emptyNumberedListRegex;
    QRegularExpression emptyBulletListRegex;
    QRegularExpression emptyTaskListRegex;
    QRegularExpression blockquoteRegex;
    QRegularExpression numberedListRegex;
    QRegularExpression bulletListRegex;
    QRegularExpression taskListRegex;

    // Used for auto-insert and pairing.
    QHash<QChar, QChar> markupPairs;

    // Used for filtering paired characters.
    QHash<QChar, bool> autoMatchFilter;

    // Used for determining if whitespace is allowed between paired
    // characters when autopairing.
    QHash<QChar, QChar> nonEmptyMarkupPairs;

    bool mouseButtonDown;
    QColor cursorColor;
    bool textCursorVisible;
    QTimer *cursorBlinkTimer;

    // data for multiline cursor.
    bool eraseList = true;
    int moveCount = 0;
    QList<QRect> cursorRects;

    // Timers used to determine when typing has paused.
    QTimer *typingTimer;
    QTimer *scaledTypingTimer;

    bool typingHasPaused;
    bool scaledTypingHasPaused;

    // Use these flags to keep from sending the typingPaused() and
    // typingPausedScaled() signals: multiple times after they have
    // already been sent the first time after a pause in the user's
    // typing.
    //
    bool typingPausedSignalSent;
    bool typingPausedScaledSignalSent;

    void toggleCursorBlink();
    void parseDocument();

    void handleCarriageReturn();
    bool handleBackspaceKey();
    void insertPrefixForBlocks(const QString &prefix);
    void createNumberedList(const QChar marker);
    bool insertPairedCharacters(const QChar firstChar);
    bool handleEndPairCharacterTyped(const QChar ch);
    bool handleWhitespaceInEmptyMatch(const QChar whitespace);
    void insertFormattingMarkup(const MarkupType markupType);
    QString priorIndentation();
    QString priorMarkdownBlockItemStart(const QRegularExpression &itemRegex, QRegularExpressionMatch &match);

    bool insideBlockArea(const QTextBlock &block, BlockType &type) const;
    bool atBlockAreaStart(const QTextBlock &block, BlockType &type) const;
    bool atBlockAreaEnd(const QTextBlock &block, const BlockType type) const;
    bool atCodeBlockStart(const QTextBlock &block) const;
    bool atCodeBlockEnd(const QTextBlock &block) const;
    bool isBlockquote(const QTextBlock &block) const;
    bool isCodeBlock(const QTextBlock &block) const;

    static QStringList buildImageReaderFormats();
    static QStringList buildImageWriterFormats();
    static QString buildImageFilters(const QStringList &mimeTypes, bool includeWildcardImages = false);
};
} // namespace ghostwriter
#endif // MARKDOWNEDIT_P_H
