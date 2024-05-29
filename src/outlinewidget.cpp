/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QListWidgetItem>
#include <QString>
#include <QTextBlock>
#include <QVariant>
#include <QPointer>
#include <QTreeView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QDebug>
#include <QAbstractItemModelTester>

#include "outlinewidget.h"
#include "outlinemodel.h"
#include "outlineproxymodel.h"

namespace ghostwriter
{

bool containsHeader(const QTextBlock& block)
{
    auto text = block.text();
    auto next = block.next();
    auto text2 = next.text();
    auto list = text.split('\n');// warning about windows \r\n ?
    auto list2 = text2.split('\n');
    list << list2;

    bool isHeader= std::any_of(std::begin(list), std::end(list), [](const QString& line){
        return line.startsWith("#");// use regexp for better matching
    });

    isHeader |= std::any_of(std::begin(list), std::end(list), [](const QString& line){
        return line.startsWith("=") || line.startsWith("-");// use regexp for better matching
    });

    return isHeader;
}

class OutlineWidgetPrivate
{
    Q_DECLARE_PUBLIC(OutlineWidget)

public:
    OutlineWidgetPrivate(OutlineWidget *q_ptr, MarkdownEditor *editor)
        : q_ptr(q_ptr)
    {
        this->editor = editor;
        this->treeview = new QTreeView(q_ptr);
        this->treeview->setAlternatingRowColors(false);
        this->treeview->setHeaderHidden(true);
        this->model = new OutlineModel(q_ptr);
        this->searchPattern = new QLineEdit(q_ptr);
        this->headerLevel = new QSlider(Qt::Horizontal, q_ptr);
        this->headerLevel->setMinimum(1);
        this->headerLevel->setMaximum(6);
        this->headerLevel->setValue(6);

        this->proxy = new OutlineProxyModel(q_ptr);
        this->proxy->setSourceModel(this->model);

        this->treeview->setModel(this->proxy);

        //new QAbstractItemModelTester(this->model, QAbstractItemModelTester::FailureReportingMode::Fatal, q_ptr);
    }

    ~OutlineWidgetPrivate()
    {
        ;
    }

    OutlineWidget *q_ptr;
    QPointer<MarkdownEditor> editor;
    QTreeView* treeview;
    QLineEdit* searchPattern;
    QSlider* headerLevel;
    OutlineModel* model;
    OutlineProxyModel* proxy;

    /*
    * Invoked when the user selects one of the headings in the outline
    * in order to navigate to a different position in the document.
    */
    void onOutlineHeadingSelected(const QModelIndex &idx);

    void reloadOutline();

    /*
    * Gets the document position stored in the given item.
    */
    int documentPosition(const QModelIndex& item);

    /*
    * Binary search of the outline tree.  Returns row of the matching
    * QListWidgetItem, or else -1 if the item with the given document
    * position is not found.
    *
    * If exactMatch is false and the item is not found, this method will
    * return the row number where the heading would belong if it were in
    * the tree (i.e., an ideal insertion point for a new heading).
    */
    int findHeading(int position, bool exactMatch = true);
};

OutlineWidget::OutlineWidget(MarkdownEditor *editor, QWidget *parent)
    : QWidget(parent),
      d_ptr(new OutlineWidgetPrivate(this, editor))
{
    Q_D(OutlineWidget);

    auto vbox = new QVBoxLayout(this);
    auto hbox = new QFormLayout(this);
    hbox->addRow(new QLabel(tr("Filter"),this), d->searchPattern);
    hbox->addRow(new QLabel(tr("Header"),this), d->headerLevel);

    vbox->addLayout(hbox);
    vbox->addWidget(d->treeview,1);
    setLayout(vbox);

    this->connect
    (
            d->treeview,
            &QTreeView::activated,
        [d](const QModelIndex& index) {
            d->onOutlineHeadingSelected(index);
        }
    );
    this->connect
    (
        d->treeview,
        &QTreeView::clicked,
        [d](const QModelIndex& index) {
            d->onOutlineHeadingSelected(index);
        }
    );
    this->connect
    (
        editor,
        &MarkdownEditor::cursorPositionChanged,
        this,
        &OutlineWidget::updateCurrentNavigationHeading
    );

    this->connect(
        d->headerLevel,
        &QSlider::valueChanged,
        d->proxy,
        &OutlineProxyModel::setLevel
    );

    this->connect(
        d->proxy,
        &OutlineProxyModel::levelChanged,
        this,
        [d, this](){
            emit headerLevelChanged(d->headerLevel->value());
        }
    );

    this->connect(
        d->searchPattern,
        &QLineEdit::textChanged,
        d->proxy,
        &OutlineProxyModel::setPattern
    );

    auto expandAll = [d](){
        d->treeview->expandAll();
    };

    this->connect(d->proxy, &OutlineProxyModel::levelChanged, this, expandAll);
    this->connect(d->proxy, &OutlineProxyModel::patternChanged, this, expandAll);

    this->connect
        (
            editor->document(),
            &MarkdownDocument::contentsChange,
            [d](int pos, int charsRemoved, int charsAdded) {
                auto doc = d->editor->document();
                auto block = doc->findBlock(pos);
                containsHeader(block);
                //qDebug() << pos << charsRemoved << charsAdded << "blockfmt:"<< block.blockFormat().headingLevel() << block.charFormat() << block.text();
                d->reloadOutline();
            }
            );

}

OutlineWidget::~OutlineWidget()
{
    ;
}

void OutlineWidget::updateCurrentNavigationHeading(int position)
{
    Q_D(OutlineWidget);

    qDebug() << "updateCurrentNavigationHeading" << position;

    // Make sure editor and document haven't been deleted.
    // Otherwise, application may crash on exit.
    //
    if (!d->editor) {
        return;
    }

    /*if ((this->count() > 0) && (position >= 0)) {
        // Find out in which subsection of the document the cursor presently is
        // located.
        //
        int row = d->findHeading(position, false);

        // If findHeading call recommended an insertion point for a new
        // heading rather than a matching row, then back up one row
        // for the actual heading under which the document position falls.
        //
        if
        (
            (row == this->count())
            ||
            (
                (row >= 0) &&
                (row < this->count()) &&
                (d->documentPosition(this->item(row)) != position)
            )
        ) {
            row--;
        }

        if (row >= 0) {
            QListWidgetItem *itemToHighlight = this->item(row);
            setCurrentItem(itemToHighlight);
            this->scrollToItem
            (
                itemToHighlight,
                QAbstractItemView::PositionAtCenter
            );
        } else {
            // Document position is before the first heading.  Deselect
            // any selected headings, and scroll to the top.
            //
            setCurrentItem(nullptr);
            this->scrollToTop();
        }
    }*/
}

void OutlineWidgetPrivate::onOutlineHeadingSelected(const QModelIndex &idx)
{
    Q_Q(OutlineWidget);

    // Make sure editor and document haven't been deleted.
    // Otherwise, application may crash on exit.
    //
    if (!editor) {
        return;
    }

    editor->navigateDocument(documentPosition(idx));
    emit q->headingNumberNavigated(idx.row() + 1);
}

void OutlineWidget::setHeaderLevel(int lvl)
{
    Q_D(OutlineWidget);

    if(d->headerLevel->value() == lvl)
        return;

    d->headerLevel->setValue(lvl);
    emit headerLevelChanged(lvl);
}

void OutlineWidgetPrivate::reloadOutline()
{
    Q_Q(OutlineWidget);

    // Make sure editor and document haven't been deleted.
    // Otherwise, application may crash on exit.
    //
    if (!editor) {
        return;
    }

    model->reset();

    if ((nullptr == editor) || (nullptr == editor->document())) {
        return;
    }

    MarkdownAST *ast = ((MarkdownDocument *) editor->document())->markdownAST();

    if (nullptr == ast) {
        return;
    }

    QVector<MarkdownNode *> headings = ast->headings();

    QHash<int, QString> lastTextByLevel;
    for (MarkdownNode *heading : headings) {
        QString headingText("   ");

        for (int i = 1; i < heading->headingLevel(); i++) {
            headingText += "    ";
        }

        QTextBlock block = editor->document()->findBlockByNumber(heading->startLine() - 1);

        QRegularExpression headingRegex("^\\s*#*(.*?)\\s*#*?\\s*$");
        QRegularExpressionMatch match = headingRegex.match(block.text());

        if (match.isValid() && match.hasMatch()) {
            headingText += match.captured(1);
        }

        if (block.isValid()) {
            //QListWidgetItem *item = new QListWidgetItem();
            //item->setText(headingText);
            //item->setData(DOCUMENT_POSITION_ROLE, QVariant::fromValue(block.position()));
            //q->insertItem(q->count(), item);

            QString previousText;
            if(heading->headingLevel() > 0)
            {
                previousText = lastTextByLevel[heading->headingLevel()-1];
            }

            model->insertOutline(block.position(), previousText ,headingText);

            lastTextByLevel[heading->headingLevel()] = headingText;
        }
    }

    q->updateCurrentNavigationHeading(editor->textCursor().position());
    this->treeview->expandAll();
}

int OutlineWidgetPrivate::documentPosition(const QModelIndex& item)
{
    return item.data(OutlineModel::DocumentPositionRole).value<int>();
}

int OutlineWidgetPrivate::findHeading(int position, bool exactMatch)
{
    Q_Q(OutlineWidget);
    return -1;

   /* int low = 0;
    int high = q->count() - 1;
    int mid = 0;

    while (low <= high) {
        mid = low + ((high - low) / 2);
        int itemPos = documentPosition(q->item(mid));

        // Check if desired heading at document position is at row "mid".
        if (itemPos == position) {
            return mid;
        }
        // Else if document position is greater than the current item's
        // position, ignore the first half of the list.
        //
        else if (itemPos < position) {
            mid++;
            low = mid;
        }
        // Else if document position is smaller, ignore the last half of the
        // list.
        //
        else {
            high = mid - 1;
        }
    }

    // Heading with desired document position is not present.

    if (exactMatch) {
        return -1;
    } else {
        return mid;
    }*/
}
} // namespace ghostwriter
