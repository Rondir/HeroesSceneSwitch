#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <ui_settingsdialog.h>

namespace Ui
{
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();
	void showEvent(QShowEvent * event);
	void ToggleShowHide();

	private slots:
	void on_SettingsDialog_accepted();
	void on_SettingsDialog_rejected()
	{
	};
	void btn_SetHeroesGameScene_clicked();
	void btn_SetHeroesMenuScene_clicked();

private:
	Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
