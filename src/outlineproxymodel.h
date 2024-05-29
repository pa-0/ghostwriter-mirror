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
#ifndef OUTLINEPROXYMODEL_H
#define OUTLINEPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace ghostwriter
{
class OutlineProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
public:
    OutlineProxyModel(QObject* parent =nullptr);

    //QModelIndex parent(const QModelIndex &index) const override;

    int level() const;
    void setLevel(int newLevel);
    QString pattern() const;
    void setPattern(const QString &newPattern);

signals:
    void levelChanged();
    void patternChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    int m_level;
    QString m_pattern;
};
}
#endif // OUTLINEPROXYMODEL_H
