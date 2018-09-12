#include <settingsdialog.h>
#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->btn_DialogButtons->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(on_reset_clicked()));
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
