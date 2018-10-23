#include <obs-module.h>
#include <obs-frontend-api.h>
#include <heroesSceneSwitch.h>
#include <settingsdialog.h>
#include <shlobj.h>

OBS_DECLARE_MODULE()

SettingsDialog* Settings_Dialog;
SceneSwitch* Scene_Switch;
QFileSystemWatcher fileSystemWatcher;
bool boolTest = true;
bool notSaving = true;

// Function to write the debug-file
void Debug(QString Textmessage, bool CreateNew = false)
{
	if (Scene_Switch->config.Debug || CreateNew)
	{
		QTime TimeStamp = QTime::currentTime();
		QString Filename = "HeroDebug.txt";
		QFile File(Filename);
		QFlag Filemode = QIODevice::Append;
		if (CreateNew == true)
		{
			Filemode = QIODevice::WriteOnly;
		}

		if (File.open(Filemode))
		{
			QTextStream ts(&File);
			ts << TimeStamp.toString() << ": " << Textmessage << endl;
		}
		File.close();
	}
}

// Function that returns the Scenes as QList<QString>
QList<QString> getSceneList()
{
	QList<QString> SceneNames;
	char **liste = obs_frontend_get_scene_names();
	while (*liste != 0)
		SceneNames.append(QString(*liste++));
	return SceneNames;
}

// Setup the Configuration Dialog
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

// Save the Plugin Configuration
void HeroesSceneConfig_Save()
{
	if (notSaving)
	{
		notSaving = false;
		Debug("Save Config..");
		QFile fileConf(Scene_Switch->config.HeroesSceneConfig_Path);
		fileConf.open(QIODevice::ReadWrite);
		QTextStream tsConf(&fileConf);
		QString confMessage;

		tsConf << "HeroesMenuScene = \"" << Scene_Switch->config.HeroesMenuScene << "\"" << endl;
		tsConf << "HeroesGameScene = \"" << Scene_Switch->config.HeroesGameScene << "\"" << endl;
		tsConf << "TempPath = \"" << Scene_Switch->config.TempFolder << "\"" << endl;
		for (int i = 0; i < Scene_Switch->config.ReplayFolders.count(); i++)
		{
			tsConf << "ReplaysPath = \"" << Scene_Switch->config.ReplayFolders[i] << "\"" << endl;
		}

		if (Scene_Switch->config.Autostart)
			confMessage = "HeroesAutostart = \"on\"";
		else
			confMessage = "HeroesAutostart = \"off\"";
		tsConf << confMessage << endl;

		if (Scene_Switch->config.Debug)
			confMessage = "Debug = \"on\"";
		else
			confMessage = "Debug = \"off\"";
		tsConf << confMessage;

		fileConf.close();
		notSaving = true;
	}
}

void HeroesSceneConfig_Reset()
{
	Debug("Reset Config..");
	// Get path for Temp and MyDocuments folder.
	TCHAR pathTempFolder[MAX_PATH];
	GetTempPathW(MAX_PATH, pathTempFolder);

	WCHAR pathMyDocuments[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, pathMyDocuments);

	// Build replayspath..
	QList<QString> ReplayList;
	QDir qpathReplays(QString::fromStdWString(pathMyDocuments) + "/Heroes of the Storm/Accounts/");
	qpathReplays.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	qpathReplays = qpathReplays.absolutePath() + "/" + qpathReplays.entryList()[0];

	for (int i = 0; i < qpathReplays.entryList().count(); i++)
	{
		if (qpathReplays.entryList()[i].contains("hero", Qt::CaseInsensitive))
		{
			qpathReplays = qpathReplays.absolutePath() + "/" + qpathReplays.entryList()[i] + "/Replays/Multiplayer";
			ReplayList.append(qpathReplays.absolutePath().replace("/", "\\"));
		}
	}
	ReplayList.removeDuplicates();

	// Initalize standard values.
	Scene_Switch->config.TempFolder = QString::fromStdWString(pathTempFolder) + "Heroes of the Storm\\TempWriteReplayP1";
	Scene_Switch->config.ReplayFolders = ReplayList;
	Scene_Switch->config.HeroesMenuScene = "HeroesMenu";
	Scene_Switch->config.HeroesGameScene = "HeroesGame";
	Scene_Switch->config.Autostart = true;
	Scene_Switch->config.Debug = false;

}

// Load the Plugin Configuration
void HeroesSceneConfig_Load()
{
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
			else if (line.contains("TempPath"))
			{
				QStringList value = line.split("\"");
				Scene_Switch->config.TempFolder = value[1];
			}
			else if (line.contains("ReplaysPath"))
			{
				QStringList value = line.split("\"");
				Scene_Switch->config.ReplayFolders.append(value[1]);
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
		Scene_Switch->config.ReplayFolders.removeDuplicates();
		fileConf.close();
	}


	if (Scene_Switch->config.HeroesMenuScene.isEmpty() || Scene_Switch->config.HeroesGameScene.isEmpty())
	{
		HeroesSceneConfig_Reset();
	}

	Scene_Switch->config.Enabled = Scene_Switch->config.Autostart;

	Debug("Menu Scene = " + Scene_Switch->config.HeroesMenuScene);
	Debug("Game Scene = " + Scene_Switch->config.HeroesGameScene);
	Debug("Autostart = " + QString::number(Scene_Switch->config.Autostart));
	Debug("");
	HeroesSceneConfig_Save();
}

// Switch the scene by name
void SwitchToScene(QString newSceneName)
{
	if (Scene_Switch->config.Enabled)
	{
		obs_source_t *mySource = 0;
		obs_source_t *currentScene = obs_frontend_get_current_scene();
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

		if (SceneNames.contains(newSceneName))
		{
			Debug("Set new Scenename to " + newSceneName);
			mySource = obs_get_source_by_name(newSceneName.toStdString().c_str());
		}
		else
			Debug("Invalid Scenename: " + newSceneName);

		if (mySource != currentScene)
		{
			obs_frontend_set_current_scene(mySource);
			Debug("Scene changed to: " + QString::fromStdString(obs_source_get_name(mySource)));
		}

		obs_source_release(mySource);
		obs_source_release(currentScene);
	}
}

// Triggered every 1 second
// Check Heroes TempFolder exists
void SceneSwitch::update()
{
	int time = 1000;
	if (boolTest)
	{
		boolTest = false;
		QDir TempFolder(Scene_Switch->config.TempFolder);

		if (TempFolder.exists() && !boolIngame)
		{
			SwitchToScene(Scene_Switch->config.HeroesGameScene);
			boolIngame = true;
		}
		else if (!TempFolder.exists() && boolIngame)
		{
			SwitchToScene(Scene_Switch->config.HeroesMenuScene);
			
			boolIngame = false;
		}
		boolTest = true;
	}
	QTimer::singleShot(time, Scene_Switch, SLOT(update()));
	//Debug("Timer Intervall set to " + QString::number(time));
}

void SceneSwitch::deleteTempFiles()
{
	QDir dir(Scene_Switch->config.TempFolder);
	if (dir.exists())
	{
		dir.removeRecursively();
	}
}

void SceneSwitch::replayChanged(QString changedFile)
{
	//MessageBoxW(0, L"Replayes modified.", 0, 0);
	QTimer::singleShot(5000, Scene_Switch, SLOT(deleteTempFiles()));
}

void SettingsDialog::showEvent(QShowEvent *ev)
{
	Debug("Open Settings Dialog");
	Settings_Dialog->setFixedSize(Settings_Dialog->size());

	ui->chk_HeroesAutostart->setChecked(Scene_Switch->config.Autostart);
	ui->chk_HeroesStarted->setChecked(Scene_Switch->config.Enabled);
	ui->chk_Debug->setChecked(Scene_Switch->config.Debug);

	ui->cb_MenuScene->clear();
	ui->cb_MenuScene->addItems(getSceneList());
	ui->cb_MenuScene->setCurrentText(Scene_Switch->config.HeroesMenuScene);

	ui->cb_GameScene->clear();
	ui->cb_GameScene->addItems(getSceneList());
	ui->cb_GameScene->setCurrentText(Scene_Switch->config.HeroesGameScene);

	ui->cb_ReplaysPath->clear();
	ui->cb_ReplaysPath->addItems(Scene_Switch->config.ReplayFolders);

	ui->eb_TempPath->setText(Scene_Switch->config.TempFolder);
}

void SettingsDialog::on_reset_clicked()
{
	HeroesSceneConfig_Reset();
	HeroesSceneConfig_Save();
	SettingsDialog::showEvent(0);
}

void SettingsDialog::on_SettingsDialog_accepted()
{
	Debug("Settings Dialog accepted");
	Scene_Switch->config.Autostart = ui->chk_HeroesAutostart->isChecked();
	Scene_Switch->config.Enabled = ui->chk_HeroesStarted->isChecked();
	Scene_Switch->config.Debug = ui->chk_Debug->isChecked();
	Scene_Switch->config.HeroesMenuScene = ui->cb_MenuScene->currentText();
	Scene_Switch->config.HeroesGameScene = ui->cb_GameScene->currentText();
	Scene_Switch->config.TempFolder = ui->eb_TempPath->text();

	Scene_Switch->config.ReplayFolders.clear();
	for (int i = 0; i < ui->cb_ReplaysPath->count(); i++)
	{
		QDir newReplayPath(ui->cb_ReplaysPath->itemText(i));
		if (newReplayPath.exists())
		{
			Scene_Switch->config.ReplayFolders.append(newReplayPath.absolutePath().replace("/", "\\"));
		}
	}
	Scene_Switch->config.ReplayFolders.removeDuplicates();
	HeroesSceneConfig_Save();
}

bool obs_module_load()
{
	Scene_Switch = new SceneSwitch();

	HeroesSceneConfig_Reset();
	HeroesSceneConfig_Load();
	SetupUI();

	QTimer::singleShot(1000, Scene_Switch, SLOT(update()));
	Debug("Timer started..");

	fileSystemWatcher.addPaths(Scene_Switch->config.ReplayFolders);
	Scene_Switch->connect(&fileSystemWatcher, SIGNAL(directoryChanged(QString)), Scene_Switch, SLOT(replayChanged(QString)));
	
	return true;
}
