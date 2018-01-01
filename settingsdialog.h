#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets>
#include <QDialog>
#include <ui_settingsdialog.h>

namespace Ui
{
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

private:
	Ui::SettingsDialog *ui;

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();
	void showEvent(QShowEvent * event);
	void ToggleShowHide();

private slots:
	void on_SettingsDialog_accepted();
	void on_SettingsDialog_rejected(){};
};

#endif // SETTINGSDIALOG_H
