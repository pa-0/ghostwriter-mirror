/***********************************************************************
 *
 * Copyright (C) 2020-2021 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef STYLESHEETBUILDER_H
#define STYLESHEETBUILDER_H

#include <QFont>
#include <QString>

#include "3rdparty/QtAwesome/QtAwesome.h"
#include "colorscheme.h"

namespace ghostwriter
{
/**
 * A convenience class to generate stylesheets for the application
 * based on the provided color scheme.
 */
class StyleSheetBuilder
{
public:
    /**
     * Constructor.
     */
    StyleSheetBuilder(const ColorScheme &colors,
        const bool roundedCorners,
        const QFont& previewTextFont,
        const QFont& previewCodeFont);

    /**
     * Destructor.
     */
    ~StyleSheetBuilder();

    /**
     * Removes temporary files created by the style sheet builder.
     * Call on application exit.
     */
    static void clearCache();

    /**
     * Gets the QSS style sheet for the Qt main window widgets.
     */
    QString widgetStyleSheet();

    /**
     * Gets the CSS style sheet for display in the HTML live preview.
     */
    QString htmlPreviewStyleSheet();

private:
    static QString m_statIndicatorArrowIconPath;

    QtAwesome *m_awesome;
    QMap<QString, QVariant> m_styleSheetVariables;

    QString stringValueOf(const QString &variableName) const;
    QString lookupVariable(const QString &variableName, const QVariant &value);
    QString compileStyleSheet(const QString &path) const;
};
} // namespace ghostwriter

#endif // STYLESHEETBUILDER_H
