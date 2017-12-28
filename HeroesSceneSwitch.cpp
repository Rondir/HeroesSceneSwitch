#include <obs-module.h>
#include <obs-frontend-api.h>
#include <heroesSceneSwitch.h>
#include <settingsdialog.h>
#include <QtWidgets>
#include <string>

OBS_DECLARE_MODULE()

SettingsDialog* Settings_Dialog;
SceneSwitch* Scene_Switch;

void Debug(QString Textmessage, bool CreateNew = false)
{
	if (Scene_Switch->config.Debug)
	{
		QTime TimeStamp = QTime::currentTime();
		QString filename = "HeroDebug.txt";
		QFile file(filename);
		QFlag filemode = QIODevice::Append;
		if (CreateNew == true)
		{
			filemode = QIODevice::WriteOnly;
		}

		if (file.open(filemode))
		{
			QTextStream stream(&file);
			stream << TimeStamp.toString() << ": " << Textmessage << endl;
		}
		file.close();
	}
}

QList<QString> getSceneList()
{
	QList<QString> SceneNames;
	char **liste = obs_frontend_get_scene_names();
	while (*liste != 0)
		SceneNames.append(QString(*liste++));
	return SceneNames;
}

void StartFileSystemWatcher()
{
	QDir watcherdir(QString::fromUtf8(getenv("USERPROFILE")) + "/Documents/Heroes of the Storm/Accounts/");

	for each (QString SubDirName in watcherdir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
	{
		QDir SubDir(watcherdir.absolutePath() + "/" + SubDirName);
		for each (QString SubSubDirName in SubDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
		{
			QDir RejoinDir = SubDir.absolutePath() + "/" + SubSubDirName + "/Saves/Rejoin";
			Scene_Switch->filewatch->addPath(RejoinDir.absolutePath());
			Scene_Switch->filewatch->addPath(SubDir.absolutePath() + "/" + SubSubDirName + "/Replays/Multiplayer");
		}
	}
	Scene_Switch->filewatch->directories().removeDuplicates();
	Scene_Switch->filewatch->files().removeDuplicates();

	Debug("");
	Debug("---------Watching---------");
	Debug("Directories:");
	for each (QString directory in Scene_Switch->filewatch->directories())
	{
		Debug(directory);
	}
	Debug("--------------------------\n");

	Debug("Connect Watcher..");
	QObject::connect(Scene_Switch->filewatch, SIGNAL(directoryChanged(QString)), Scene_Switch, SLOT(FilesModified(QString)));
}

void SetupUI()
{
	QAction* menu_action = (QAction*)obs_frontend_add_tools_menu_qaction("Heroes Scene Switch");
	Settings_Dialog = new SettingsDialog(Scene_Switch);
	auto menu_cb = []
	{
		Settings_Dialog->ToggleShowHide();
	};
	menu_action->connect(menu_action, &QAction::triggered, menu_cb);
}

void HeroesSceneConfig_Save()
{
	Debug("Save Config..");
	QFile fileConf(Scene_Switch->config.HeroesSceneConfig_Path);
	fileConf.open(QIODevice::ReadWrite);
	QTextStream tsConf(&fileConf);
	QString confMessage;

	tsConf << "HeroesMenuScene = \"" << Scene_Switch->config.HeroesMenuScene << "\"" << endl;

	tsConf << "HeroesGameScene = \"" << Scene_Switch->config.HeroesGameScene << "\"" << endl;

	if (Scene_Switch->config.Autostart)
		confMessage = "HeroesAutostart = \"on\"";
	else
		confMessage = "HeroesAutostart = \"off\"";
	tsConf << confMessage << endl;

	if (Scene_Switch->config.Debug)
		confMessage = "Debug = \"on\"";
	else
		confMessage = "Debug = \"off\"";
	tsConf << confMessage << endl;

	fileConf.close();
}

void HeroesSceneConfig_Load()
{
	// Initalize Standard Values
	Scene_Switch->config.HeroesMenuScene = "HeroesMenu";
	Scene_Switch->config.HeroesGameScene = "HeroesGame";
	Scene_Switch->config.Autostart = false;
	Scene_Switch->config.Debug = true;

	Debug("Heroes Scene Switch loading..", true);

	// Create Settings Directory, if not exist.
	QDir ModuleConfigPath(obs_module_config_path(""));
	if (!ModuleConfigPath.exists()) ModuleConfigPath.mkpath(".");

	Debug("Load Config..");
	QString FilePath = obs_module_config_path("settings.txt");
	FilePath = FilePath.replace("\\", "/");
	Scene_Switch->config.HeroesSceneConfig_Path = FilePath;

	Debug("Config File = " + Scene_Switch->config.HeroesSceneConfig_Path);
	QFile fileConf(Scene_Switch->config.HeroesSceneConfig_Path);

	if (fileConf.open(QIODevice::ReadOnly))
	{
		QTextStream tsConf(&fileConf);
		while (!tsConf.atEnd())
		{
			QString line = tsConf.readLine();

			if (line.contains("HeroesMenuScene"))
			{
				QStringList value = line.split("\"");
				Scene_Switch->config.HeroesMenuScene = value[1];
			}
			else if (line.contains("HeroesGameScene"))
			{
				QStringList value = line.split("\"");
				Scene_Switch->config.HeroesGameScene = value[1];
			}
			else if (line.contains("HeroesAutostart"))
			{
				QStringList value = line.split("\"");
				if (qstrcmp(value[1].toStdString().c_str(), "on") == 0)
					Scene_Switch->config.Autostart = true;
				else if (qstrcmp(value[1].toStdString().c_str(), "off") == 0)
					Scene_Switch->config.Autostart = false;
			}
			else if (line.contains("Debug"))
			{
				QStringList value = line.split("\"");
				if (qstrcmp(value[1].toStdString().c_str(), "on") == 0)
					Scene_Switch->config.Debug = true;
				else if (qstrcmp(value[1].toStdString().c_str(), "off") == 0)
					Scene_Switch->config.Debug = false;
			}
		}
		fileConf.close();
	}

	Scene_Switch->config.Enabled = Scene_Switch->config.Autostart;

	Debug("Menu Scene = " + Scene_Switch->config.HeroesMenuScene);
	Debug("Game Scene = " + Scene_Switch->config.HeroesGameScene);
	Debug("Autostart = " + QString::number(Scene_Switch->config.Autostart));
	Debug("");
}

void SceneSwitch::FilesModified(const QString& directory)
{
	Debug("Trigger: \"" + directory + "\"");
	if (Scene_Switch->config.Enabled)
	{
		obs_source_t *mySource = 0;
		obs_source_t *currentScene = obs_frontend_get_current_scene();
		QString newScenename;

		QList<QString> SceneNames = getSceneList();
		
		Debug("Available Scenes:");
		for each (QString Scene in SceneNames)
		{
			if (Scene.compare(obs_source_get_name(currentScene)) == 0)
				Debug(Scene + " (current)");
			else
				Debug(Scene);
		}
		Debug("\n");

		if (directory.contains("Replays/Multiplayer"))
		{
			newScenename = Scene_Switch->config.HeroesMenuScene;
			if (SceneNames.contains(newScenename))
			{
				Debug("Set new Scenename to " + newScenename);
				mySource = obs_get_source_by_name(newScenename.toStdString().c_str());
			}
			else
				Debug("Invalid Scenename: " + newScenename);
		}
		else if (directory.contains("Saves/Rejoin"))
		{
			newScenename = Scene_Switch->config.HeroesGameScene;
			if (SceneNames.contains(newScenename))
			{
				Debug("Set new Scenename to " + newScenename);
				mySource = obs_get_source_by_name(newScenename.toStdString().c_str());
			}
			else
				Debug("Invalid Scenename: " + newScenename);
		}

		if (mySource != currentScene)
		{
			obs_frontend_set_current_scene(mySource);
			Debug("Scene changed to: " + QString::fromStdString(obs_source_get_name(mySource)));

		}

		obs_source_release(mySource);
		obs_source_release(currentScene);
	}
	return;
}

void SettingsDialog::showEvent(QShowEvent * event)
{
	Debug("Open Settings Dialog");
	Settings_Dialog->setFixedSize(Settings_Dialog->size());

	ui->list_HeroesScenes->clear();
	ui->list_HeroesScenes->addItems(getSceneList());
	ui->cb_HeroesAutostart->setChecked(Scene_Switch->config.Autostart);
	ui->cb_HeroesStarted->setChecked(Scene_Switch->config.Enabled);
	ui->cb_Debug->setChecked(Scene_Switch->config.Debug);
	ui->eb_HeroesMenuScene->setText(Scene_Switch->config.HeroesMenuScene);
	ui->eb_HeroesGameScene->setText(Scene_Switch->config.HeroesGameScene);
}

void SettingsDialog::on_SettingsDialog_accepted()
{
	Debug("Settings Dialog accepted");
	Scene_Switch->config.Autostart = ui->cb_HeroesAutostart->isChecked();
	Scene_Switch->config.Enabled = ui->cb_HeroesStarted->isChecked();
	Scene_Switch->config.Debug = ui->cb_Debug->isChecked();
	Scene_Switch->config.HeroesMenuScene = ui->eb_HeroesMenuScene->text();
	Scene_Switch->config.HeroesGameScene = ui->eb_HeroesGameScene->text();
	HeroesSceneConfig_Save();
}

bool obs_module_load()
{
	Scene_Switch = new SceneSwitch((QMainWindow*)obs_frontend_get_main_window());

	HeroesSceneConfig_Load();
	SetupUI();
	StartFileSystemWatcher();

	return true;
}
