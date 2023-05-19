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
#include "outlineproxymodel.h"

#include "outlinemodel.h"

#include <QDebug>

namespace ghostwriter
{
OutlineProxyModel::OutlineProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
{
    //connect(this, &OutlineProxyModel::levelChanged, this, &OutlineProxyModel::invalidateFilter);
    //connect(this, &OutlineProxyModel::patternChanged, this, &OutlineProxyModel::invalidateFilter);
    setDynamicSortFilter(true);
}

int OutlineProxyModel::level() const
{
    return m_level;
}

void OutlineProxyModel::setLevel(int newLevel)
{
    if (m_level == newLevel)
        return;
    m_level = newLevel;
    emit levelChanged();
    invalidateFilter();
}

QString OutlineProxyModel::pattern() const
{
    return m_pattern;
}

void OutlineProxyModel::setPattern(const QString &newPattern)
{
    if (m_pattern == newPattern)
        return;
    m_pattern = newPattern;
    emit patternChanged();
    invalidateFilter();
}

bool OutlineProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    auto sourceIdx= sourceModel()->index(sourceRow, 0, sourceParent);
    auto idx = sourceIdx;
    int i = 0;

    while(idx.isValid())
    {
        ++i;
        idx = idx.parent();
    }
    auto info  = static_cast<OutlineInfo*>(sourceIdx.internalPointer());

    //qDebug() << "filter: " << info << i << level();

    bool isAllowed = i <= level();

    if(!info)
        return isAllowed;

    bool hasPattern = m_pattern.isEmpty() ? true : info->hasPattern(m_pattern);

    return isAllowed && hasPattern;
}
}
