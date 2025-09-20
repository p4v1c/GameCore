#include "../headers/EmulatorWidget.h"
#include "../headers/Emulator.h"
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

EmulatorWidget::EmulatorWidget(const Emulator &emu, QWidget *parent)
    : QWidget(parent), m_emulatorName(emu.name) {
    // Le layout pour organiser les labels verticalement
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    // Le label pour le nom de la console (ex: "Switch")
    QLabel *nameLabel = new QLabel(emu.platform);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "color: qlineargradient(spread:pad, x1:0, y1:0, "
        "x2:1, y2:0, stop:0 #BBBBBB, stop:1 #DDDDDD);"
        "font-weight: bold;"
        "font-size: 28px;"
        "font-family: 'Arial';");

    layout->addWidget(nameLabel, 0, Qt::AlignHCenter);

    // Le label pour l'icône de l'émulateur
    QLabel *iconLabel = new QLabel();
    if (!emu.icon.isNull()) {
        iconLabel->setPixmap(emu.icon.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap placeholder(150, 150);
        placeholder.fill(Qt::gray);
        iconLabel->setPixmap(placeholder);
    }
    iconLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(iconLabel, 0, Qt::AlignHCenter);

    // Le label pour le nom de l'émulateur (ex: "yuzu")
    QLabel *emuLabel = new QLabel(emu.name);
    emuLabel->setAlignment(Qt::AlignCenter);
    emuLabel->setStyleSheet("color: #DDDDDD; font-size: 20px;");
    layout->addWidget(emuLabel, 0, Qt::AlignHCenter);

    setLayout(layout);
}

QString EmulatorWidget::getEmulatorName() const {
    return m_emulatorName;
}
