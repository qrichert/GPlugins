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

#include "GPlugins.h"
#include <QPluginLoader>

GPlugins::GPlugins()
{
	setWindowTitle("Multiuso - Gestionnaire des plugins");
	setWindowIcon(QIcon(":/icones/mainIcon.png"));

	listePlugins = new QListWidget;
		connect(listePlugins, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(supprimerPlugin(QListWidgetItem *)));

	QLabel *infos = new QLabel(" Double-cliquez sur un plugin pour le supprimer. ");

	QPushButton *ajouter = new QPushButton("Ajouter...");
		ajouter->setIcon(QIcon(":/icones/ajouter.png"));
		connect(ajouter, SIGNAL(clicked()), this, SLOT(ajouterPlugin()));

	QGridLayout *layout = new QGridLayout(this);
		layout->addWidget(listePlugins, 0, 0, 1, 6);
		layout->addWidget(new QWidget, 1, 0, 1, 1);
		layout->addWidget(infos, 1, 1, 1, 3);
		layout->addWidget(ajouter, 1, 4, 1, 1);
		layout->addWidget(new QWidget, 1, 5, 1, 1);
		layout->setContentsMargins(0, 0, 0, 7);

	resize(minimumWidth(), 250);

	noms << "";
	noms.clear();

	chargerPlugins();
}

void GPlugins::chargerPlugins()
{
	listePlugins->clear();
	noms.clear();

	QStringList listPlugins = QDir(Multiuso::pluginsDirPath()).entryList();
		listPlugins.removeOne(".");
		listPlugins.removeOne("..");

	for (int i = 0; i < listPlugins.size(); i++)
	{
		QPluginLoader chargeur(Multiuso::pluginsDirPath() + listPlugins.value(i));

		if (QObject *nouveauPlugin = chargeur.instance())
		{
			QFileInfo fileInfo(Multiuso::pluginsDirPath() + listPlugins.value(i));

			QSettings settings(fileInfo.absolutePath() + "/" + fileInfo.baseName() +
					"/" + fileInfo.baseName() + ".ini", QSettings::IniFormat);

			int type = settings.value("type").toInt();

			QString nomPlugin;
			QString iconePlugin;

			if (type == 1)
			{
				BaseAction *pluginAction = qobject_cast<BaseAction *>(nouveauPlugin);
					nomPlugin = pluginAction->nom();
					iconePlugin = pluginAction->icone();
			}

			else if (type == 2)
			{
				BaseDockWidget *pluginDockWidget = qobject_cast<BaseDockWidget *>(nouveauPlugin);
					nomPlugin = pluginDockWidget->nom();
					iconePlugin = pluginDockWidget->icone();
			}

			else if (type == 3)
			{
				BaseOnglet *pluginOnglet = qobject_cast<BaseOnglet *>(nouveauPlugin);
					nomPlugin = pluginOnglet->nom();
					iconePlugin = pluginOnglet->icone();
			}

			else if (type == 4)
			{
				BaseWidget *pluginWidget = qobject_cast<BaseWidget *>(nouveauPlugin);
					nomPlugin = pluginWidget->nom();
					iconePlugin = pluginWidget->icone();
			}

			if (!noms.contains(nomPlugin))
			{
				noms << nomPlugin;

				QListWidgetItem *item = new QListWidgetItem(nomPlugin);
					item->setIcon(QIcon(iconePlugin));
					item->setToolTip(Multiuso::pluginsDirPath() + listPlugins.value(i));

				listePlugins->addItem(item);
			}
			
			nouveauPlugin = NULL;
		}
		
		chargeur.unload();
	}
}

void GPlugins::ajouterPlugin()
{
	QString nomPlugin = QFileDialog::getOpenFileName(this, "Multiuso - Gestionnaire des plugins", QDir::homePath(), "Plugin (*)");

	if (nomPlugin.isEmpty())
		return;

	QPluginLoader chargeur(nomPlugin);

	if (QObject *nouveauPlugin = chargeur.instance())
	{
		nouveauPlugin = NULL;
		chargeur.unload();

		QFile plugin(nomPlugin);
			plugin.copy(Multiuso::pluginsDirPath() + QFileInfo(plugin).fileName());
			plugin.remove();

		QDir dossier = QDir(Multiuso::addSlash(QFileInfo(plugin).path()) + QFileInfo(plugin).baseName());

		Multiuso::copyDirectory(dossier, Multiuso::pluginsDirPath());
		Multiuso::removeDirectory(dossier);

		QPluginLoader secondLoader(Multiuso::pluginsDirPath() + QFileInfo(plugin).fileName());

		if (QObject *realPlugin = secondLoader.instance())
			realPlugin = NULL;
		
		else
			QMessageBox::critical(this, "Multiuso - Gestionnaire des plugins", "Impossible d'ajouter ce plugin !<br /><strong>Raison : " + secondLoader.errorString() + "</strong>");
		
		chargerPlugins();
		
		secondLoader.unload();
	}

	else
	{
		QMessageBox::critical(this, "Multiuso - Gestionnaire des plugins", "Impossible d'ajouter ce plugin !");
	}
	
	chargeur.unload();
}

void GPlugins::supprimerPlugin(QListWidgetItem *item)
{
	QFile plugin(item->toolTip());

	int reponse = QMessageBox::warning(this, "Multiuso - Gestionnaire des plugins", "Voulez-vous vraiment supprimer « " + item->text() + " » de façon définitive ?",
			QMessageBox::Yes | QMessageBox::No);

	if (reponse == QMessageBox::No)
		return;

	plugin.remove();

	Multiuso::removeDirectory(QDir(Multiuso::pluginsDirPath() + QFileInfo(plugin).baseName()));

	chargerPlugins();
}

void GPlugins::closeEvent(QCloseEvent *event)
{
	int reponse = QMessageBox::question(this, "Multiuso - Gestionnaire des plugins", "Voulez-vous quitter le gestionnaire des plugins et lancer Multiuso ?",
			QMessageBox::Yes | QMessageBox::No);

	if (reponse == QMessageBox::Yes)
	{
		QProcess::startDetached(Multiuso::openCommand() + Multiuso::appDirPath() + "/Multiuso" + Multiuso::currentSuffix());	
		event->accept();
		qApp->quit();
	}
	
	else
	{
		event->ignore();
	}
}
