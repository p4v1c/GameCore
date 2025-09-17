#include "../headers/EmulatorWidget.h"
#include "../headers/Emulator.h"
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

EmulatorWidget::EmulatorWidget(const Emulator &emu, QWidget *parent)
    : QWidget(parent) {

  // Le layout pour organiser les labels verticalement
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(10);

  // Le label pour le nom de la console (ex: "Switch")
  nameLabel = new QLabel(emu.platform);
  nameLabel->setAlignment(Qt::AlignCenter);
  nameLabel->setStyleSheet(
      "color: qlineargradient(spread:pad, x1:0, y1:0, "
      "x2:1, y2:0, stop:0 #BBBBBB, stop:1 #DDDDDD);"
      "font-weight: bold;"
      "font-size: 28px;"
      "font-family: 'Arial';"
      "border: 0px solid transparent;"); // Ajout de cette ligne pour enlever la
                                         // bordure

  layout->addWidget(nameLabel, 0, Qt::AlignHCenter);

  // Le label pour l'icône de l'émulateur
  iconLabel = new QLabel();
  if (!emu.icon.isNull()) {
    iconLabel->setPixmap(emu.icon.scaled(150, 150, Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
  } else {
    QPixmap placeholder(150, 150);
    placeholder.fill(Qt::gray);
    iconLabel->setPixmap(placeholder);
  }
  iconLabel->setAlignment(Qt::AlignCenter);
  iconLabel->setStyleSheet("border-radius:15px; background-color:#444444;");
  layout->addWidget(iconLabel, 0, Qt::AlignCenter);
}

// Getters
QLabel *EmulatorWidget::getNameLabel() const { return nameLabel; }

QLabel *EmulatorWidget::getIconLabel() const { return iconLabel; }
