#include <settingsdialog.h>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::ToggleShowHide()
{
	if (!isVisible())
	{
		setVisible(true);
	}
	else
		setVisible(false);
}
