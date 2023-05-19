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
#include "outlinemodel.h"
#include <QDebug>

namespace ghostwriter
{
//OutlineInfo
OutlineInfo::OutlineInfo(int docPosition, const QString &title, OutlineInfo* parent)
    : m_title(title),m_documentPosition(docPosition), m_parent(parent)
{

}

QString OutlineInfo::title() const
{
    return m_title;
}

int OutlineInfo::childrenCount() const
{
    return m_children.size();
}

OutlineInfo *OutlineInfo::childAt(int i) const
{
    if(i >= childrenCount() || i < 0)
        return nullptr;

    return m_children[i].get();
}

OutlineInfo *OutlineInfo::parent() const
{
    return m_parent;
}

int OutlineInfo::documentPosition() const
{
    return m_documentPosition;
}

void OutlineInfo::setTitle(const QString &title)
{
    m_title = title;
}

void OutlineInfo::addChild(int pos, const QString &title)
{
    m_children.push_back(std::make_unique<OutlineInfo>(pos, title, this));
}

void OutlineInfo::removeChild(int i)
{
    Q_ASSERT(!m_children.empty());
    Q_ASSERT(i < static_cast<int>(m_children.size()));
    m_children.erase(m_children.begin() + i);
}

void OutlineInfo::setParent(OutlineInfo *parent)
{
    m_parent = parent;
}

int OutlineInfo::indexOf(OutlineInfo* child)
{
    auto it = std::find_if(std::begin(m_children), std::end(m_children), [child](const std::unique_ptr<OutlineInfo>& temp){
        return child == temp.get();
    });
    if(it == std::end(m_children))
        return -1;
    return std::distance(std::begin(m_children), it);
}

bool OutlineInfo::hasPattern(const QString &pattern)
{
    return (title().contains(pattern) ||
            std::any_of(std::begin(m_children), std::end(m_children), [pattern](const std::unique_ptr<OutlineInfo>& temp){
                return temp->m_title.contains(pattern);
            }));
}

OutlineInfo *OutlineInfo::find(const QString& a)
{
    if(a == m_title)
        return this;
    else {
        for(auto& info : m_children)
        {
            auto res = info->find(a);
            if(res)
                return res;
        }
    }
    return nullptr;
}


//OutlineModel
OutlineModel::OutlineModel(QObject *parent)
    : QAbstractItemModel(parent), m_root(new OutlineInfo())
{
}

QModelIndex OutlineModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0)
        return QModelIndex();

    OutlineInfo* parentNode= nullptr;

    if(!parent.isValid())
        parentNode= m_root.get();
    else
        parentNode= static_cast<OutlineInfo*>(parent.internalPointer());

    auto childNode= parentNode->childAt(row);

    if(childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex OutlineModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto indexInfo= static_cast<OutlineInfo*>(index.internalPointer());

    if(!indexInfo)
        return {};

    auto parentInfo= indexInfo->parent();

    if(!parentInfo)
        return {};

    if(m_root->indexOf(indexInfo) > -1)
    {
        return {};
    }
    if(m_root->indexOf(parentInfo) > -1)
    {
        return createIndex(m_root->indexOf(parentInfo), 0, parentInfo);
    }

    return createIndex(parentInfo->parent()->indexOf(parentInfo), 0, parentInfo);
}

int OutlineModel::rowCount(const QModelIndex &parent) const
{
    OutlineInfo* info= nullptr;
    if(!parent.isValid())
        info= m_root.get();
    else
        info= static_cast<OutlineInfo*>(parent.internalPointer());

    return info->childrenCount();
}

int OutlineModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant OutlineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QVariant res;
    auto info = static_cast<OutlineInfo*>(index.internalPointer());
    if(role == Qt::DisplayRole)
    {
        res = info->title();
    }
    else if( role == DocumentPositionRole)
    {
        res = info->documentPosition();
    }

    return res;
}

bool OutlineModel::insertOutline(int blockPos, const QString& outlineParent, const QString& newOutline)
{
    auto parent = m_root->find(outlineParent);
    QList<OutlineInfo*> ancestors;

    ancestors.prepend(parent);

    auto loop = parent;
    while(loop->parent())
    {
        ancestors.prepend(loop->parent());
        loop = loop->parent();
    }

    QModelIndex parentIdx;
    for(auto& ancestor : ancestors)
    {
        int i = 0;
        if(!parentIdx.isValid())
        {
            i = m_root->indexOf(ancestor);
        }
        else
        {
            auto info = static_cast<OutlineInfo*>(parentIdx.internalPointer());
            i = info->indexOf(ancestor);
        }

        parentIdx = index(i,0, parentIdx);
    }

    beginInsertRows(parentIdx, parent->childrenCount(), parent->childrenCount());
    parent->addChild(blockPos, newOutline);
    endInsertRows();
    return true;
}

void OutlineModel::reset()
{
    beginResetModel();
    m_root.reset(new OutlineInfo());
    endResetModel();
}

}
