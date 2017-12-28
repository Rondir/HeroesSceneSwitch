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

void SettingsDialog::btn_SetHeroesGameScene_clicked()
{
  if ((ui->list_HeroesScenes->selectedItems().count() > 0) &&
    (ui->list_HeroesScenes->currentItem()->text() != ui->eb_HeroesMenuScene->text()))
  {
    QString selectedScene = ui->list_HeroesScenes->currentItem()->text();
    ui->eb_HeroesGameScene->setText(selectedScene);
  }
}

void SettingsDialog::btn_SetHeroesMenuScene_clicked()
{
  if ((ui->list_HeroesScenes->selectedItems().count() > 0) &&
    (ui->list_HeroesScenes->currentItem()->text() != ui->eb_HeroesGameScene->text()))
  {
    QString selectedScene = ui->list_HeroesScenes->currentItem()->text();
    ui->eb_HeroesMenuScene->setText(selectedScene);
  }
}
