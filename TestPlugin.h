#pragma once

#include <QtWidgets>

namespace OBS_Module
{
	class TestPlugin;
}

class TestPlugin : public QWidget
{
	Q_OBJECT

private:

public:
	TestPlugin(QWidget* parent = nullptr) : QWidget(parent)
	{
	};
	~TestPlugin()
	{
	};

	public slots :
	void update();
};
