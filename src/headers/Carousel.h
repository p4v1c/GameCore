#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <QWidget>
#include <vector>
#include "EmulatorWidget.h"
#include "ControllerManager.h"

class Carousel : public QWidget {
    Q_OBJECT

public:
    explicit Carousel(QWidget *parent = nullptr);
    void scanEmulators(const QString &path);

public slots:
    void handleControllerButton(int button);
    void handleControllerAxis(int axis, int value);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateVisibleWidgets();
    void updateSelection();

    std::vector<EmulatorWidget*> emuWidgets;
    int selectedIndex;
    int lastAxisValue; // Déclaration de la variable manquante
};

#endif // CAROUSEL_H
