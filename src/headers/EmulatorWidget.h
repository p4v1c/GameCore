#ifndef EMULATORWIDGET_H
#define EMULATORWIDGET_H

#include "Emulator.h"
#include <QLabel>
#include <QWidget>

class EmulatorWidget : public QWidget {
    Q_OBJECT

public:
    explicit EmulatorWidget(const Emulator &emu, QWidget *parent = nullptr);

    QLabel *getNameLabel() const;
    QLabel *getIconLabel() const;
    QString getEmulatorName() const; // Ajout de cette méthode

private:
    QLabel *nameLabel;
    QLabel *iconLabel;
    QString m_emulatorName; // Ajout de cette variable membre
};

#endif // EMULATORWIDGET_H
