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
		 bool Autostart;
		 bool Enabled;
		 bool Debug;
	};

public:
	Config config;
	QFileSystemWatcher* filewatch;
	QString TempFolder;
	SceneSwitch(QWidget* parent = nullptr) : QWidget(parent)
	{
		filewatch = new QFileSystemWatcher();
	};

	~SceneSwitch()	{};

public slots :
	void FilesModified(const QString& directory);
	void update();
};

#endif // HeroesSceneSwitch_H
