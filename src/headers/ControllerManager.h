#ifndef CONTROLLERMANAGER_H
#define CONTROLLERMANAGER_H

#include <QObject>
#include <SDL.h>

class ControllerManager : public QObject {
  Q_OBJECT

public:
  explicit ControllerManager(QObject *parent = nullptr);
  ~ControllerManager();

  void initialize();
  void scanForControllers();
  void processEvents();

signals:
  void controllerConnected(const QString &name);
  void controllerDisconnected();
  void buttonPressed(int button);
  void axisMoved(int axis, int value);
  void buttonAPressed();
  void buttonBPressed();
  void buttonXPressed();
  void buttonYPressed();

private:
  SDL_GameController *gameController;
};

#endif // CONTROLLERMANAGER_H
