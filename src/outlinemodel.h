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
#ifndef OUTLINEMODEL_H
#define OUTLINEMODEL_H

#include <QAbstractItemModel>
#include <QPointer>
#include <memory>

namespace ghostwriter
{
class OutlineInfo
{
public:
    OutlineInfo(int docPosition = -1, const QString& title = QString(), OutlineInfo* parent = nullptr);
    QString title() const;
    int childrenCount() const;
    OutlineInfo* childAt(int i) const;
    OutlineInfo* parent() const;
    int documentPosition() const;


    void setTitle(const QString& title);
    void addChild(int pos, const QString& title);
    void removeChild(int i);
    void setParent(OutlineInfo* parent);
    int indexOf(OutlineInfo* child);

    bool hasPattern(const QString& pattern);

    OutlineInfo *find(const QString& a);

private:
    QString m_title;
    int m_documentPosition;
    OutlineInfo* m_parent;
    std::vector<std::unique_ptr<OutlineInfo>> m_children;
};

class OutlineModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum CustomRole {
        DocumentPositionRole = Qt::UserRole+1
    };
    explicit OutlineModel(QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertOutline(int blockPos, const QString& outlineParent, const QString& newOutline);

    void reset();

private:
    std::unique_ptr<OutlineInfo> m_root;
};
}
#endif // OUTLINEMODEL_H
