/*

Copyright © 2009-2010 Quentin RICHERT

Multiuso is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Multiuso is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Multiuso.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef HEADER_GPLUGINS
#define HEADER_GPLUGINS

#include "../Multiuso/Multiuso.h"
#include "BaseAction.h"
#include "BaseDockWidget.h"
#include "BaseOnglet.h"
#include "BaseWidget.h"

class GPlugins : public QWidget
{
	Q_OBJECT

	public:
		GPlugins();

	protected:
		void closeEvent(QCloseEvent *event);

	public slots:
		void chargerPlugins();
		void ajouterPlugin();
		void supprimerPlugin(QListWidgetItem *item);

	private:
		QListWidget *listePlugins;
		QStringList noms;
};

#endif
