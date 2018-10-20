#include <obs-module.h>
#include <obs-frontend-api.h>
#include <Testplugin.h>
#include <QtCore>

OBS_DECLARE_MODULE()

TestPlugin* testPlugin;
int count;

void TestPlugin::update()
{
	QFile qfile_NewFile("D:\\Test.txt");
	qfile_NewFile.open(QIODevice::ReadWrite);

	QTextStream qts_NewFile(&qfile_NewFile);
	count++;
	obs_audio_info ObsAudioInformation;
	obs_get_audio_info(&ObsAudioInformation);
	
	qts_NewFile << "test" + QString::number(count) + " Speakers=" + QString::number(ObsAudioInformation.speakers) << endl;

	
	qfile_NewFile.close();
	QTimer::singleShot(1000, testPlugin, SLOT(update()));
}

bool obs_module_load()
{
	testPlugin = new TestPlugin();
	count = 0;

	QTimer::singleShot(100, testPlugin, SLOT(update()));
	QMessageBox::information(0, "TestPlugin", "Modul geladen.", 0, 0);
	return true;
}
