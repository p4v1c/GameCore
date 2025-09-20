#include "../headers/EmulatorWidget.h"
#include "../headers/Emulator.h"
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

EmulatorWidget::EmulatorWidget(const Emulator &emu, QWidget *parent)
    : QWidget(parent), m_emulatorName(emu.name) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(50);

  // Définit une taille fixe pour l'ensemble du widget de l'émulateur
  setFixedSize(QSize(200, 250));

  nameLabel = new QLabel(emu.platform);
  nameLabel->setAlignment(Qt::AlignCenter);
  nameLabel->setStyleSheet("color: qlineargradient(spread:pad, x1:0, y1:0, "
                           "x2:1, y2:0, stop:0 #BBBBBB, stop:1 #DDDDDD);"
                           "font-weight: bold;"
                           "font-size: 28px;"
                           "border-radius: 15px;"
                           "font-family: 'Arial';");

  layout->addWidget(nameLabel, 0, Qt::AlignHCenter);

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
  iconLabel->setStyleSheet("background-color:#444444; border-radius: 50px;");
  layout->addWidget(iconLabel, 0, Qt::AlignHCenter);
}

QLabel *EmulatorWidget::getNameLabel() const { return nameLabel; }
QLabel *EmulatorWidget::getIconLabel() const { return iconLabel; }
QString EmulatorWidget::getEmulatorName() const { return m_emulatorName; }
