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
#ifndef TST_MARKDOWNEDIT_H
#define TST_MARKDOWNEDIT_H

#include <QTest>
#include <memory>

namespace ghostwriter
{
class MarkdownEditor;
class MarkdownDocument;
class ColorScheme;
}
class MarkDownEditTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void multilineEditionTest();

private:
    std::unique_ptr<ghostwriter::MarkdownEditor> m_edit;
    std::unique_ptr<ghostwriter::MarkdownDocument> m_mdDocument;
    std::unique_ptr<ghostwriter::ColorScheme> m_colors;
};

#endif // TST_MARKDOWNEDIT_H
