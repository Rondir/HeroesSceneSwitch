#ifndef HeroesSceneSwitch_H
#define HeroesSceneSwitch_H

#include <QtWidgets>

namespace OBS_Module
{
	class SceneSwitch;
}

class SceneSwitch : public QWidget
{
	Q_OBJECT

private:
	struct Config
	{
		 QString HeroesSceneConfig_Path;
		 QString HeroesMenuScene;
		 QString HeroesGameScene;
		 QString TempFolder;
		 QList<QString> ReplayFolders;
		 bool Autostart;
		 bool Enabled;
		 bool Debug;
	};

public:
	Config config;
	bool boolIngame = false;
	SceneSwitch(QWidget* parent = nullptr) : QWidget(parent) {};
	~SceneSwitch()	{};

public slots :
	void update();
	void deleteTempFiles();
	void replayChanged(QString changedFile);
};

#endif // HeroesSceneSwitch_H
