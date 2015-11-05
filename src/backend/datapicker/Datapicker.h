/***************************************************************************
    File                 : Datapicker.h
    Project              : LabPlot
    Description          : Aspect providing a container for storing image and data
                           in form of worksheet and spreadsheets
    --------------------------------------------------------------------
    Copyright            : (C) 2015 by Ankit Wagadre (wagadre.ankit@gmail.com)
    Copyright            : (C) 2015 Alexander Semke (alexander.semke@web.de)

 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef DATAPICKER_H
#define DATAPICKER_H

#include "backend/core/AbstractPart.h"
#include "backend/core/AbstractScriptingEngine.h"

class Spreadsheet;
class DataPickerCurve;
class Image;
class QXmlStreamWriter;
class XmlStreamReader;
class Transform;
class QPointF;
class QVector3D;

class Datapicker : public AbstractPart, public scripted {
    Q_OBJECT

    public:
        Datapicker(AbstractScriptingEngine* engine, const QString& name, const bool loading = false);

        virtual QIcon icon() const;
        virtual QMenu* createContextMenu();
        virtual QWidget* view() const;

        DataPickerCurve* activeCurve;
        Spreadsheet* currentSpreadsheet() const;
        Image* currentImage() const;
	Image* image() const;
        void setChildSelectedInView(int index, bool selected);
        QVector3D mapSceneToLogical(const QPointF&) const;
        QVector3D mapSceneLengthToLogical(const QPointF&) const;

        virtual void save(QXmlStreamWriter*) const;
        virtual bool load(XmlStreamReader*);

    public slots:
        virtual void childSelected(const AbstractAspect*);

    private:
        Transform* m_transform;
        Image* m_image;
        void init();

    private slots:
        virtual void childDeselected(const AbstractAspect*);
        void handleChildAspectAboutToBeRemoved(const AbstractAspect*);
        void handleChildAspectAdded(const AbstractAspect*);
        void handleAspectAboutToBeRemoved(const AbstractAspect*);

    signals:
        void datapickerItemSelected(int);
        void childAspectAdded(const AbstractAspect*);
};

#endif
