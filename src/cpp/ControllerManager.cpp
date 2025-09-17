#include "../headers/ControllerManager.h"
#include <QDebug>
#include <SDL.h>

ControllerManager::ControllerManager(QObject *parent)
    : QObject(parent), gameController(nullptr) {
  if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
    qDebug() << "Erreur d'initialisation de SDL:" << SDL_GetError();
  }
}

ControllerManager::~ControllerManager() {
  if (gameController) {
    SDL_GameControllerClose(gameController);
  }
  SDL_Quit();
}

void ControllerManager::initialize() { scanForControllers(); }

void ControllerManager::scanForControllers() {
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      if (!gameController) {
        gameController = SDL_GameControllerOpen(i);
        if (gameController) {
          QString controllerName = SDL_GameControllerName(gameController);
          qDebug() << "Manette détectée et connectée:" << controllerName;
          emit controllerConnected(controllerName);
          return;
        }
      }
    }
  }
}

// This is the ONLY processEvents() definition needed.
void ControllerManager::processEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_CONTROLLERDEVICEADDED) {
      qDebug() << "Nouvelle manette branchée.";
      scanForControllers();
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
      qDebug() << "Manette déconnectée.";
      if (gameController) {
        SDL_GameControllerClose(gameController);
        gameController = nullptr;
      }
      emit controllerDisconnected();
    } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
      emit buttonPressed(event.cbutton.button);
    } else if (event.type == SDL_CONTROLLERAXISMOTION) {
      emit axisMoved(event.caxis.axis, event.caxis.value);
    }
  }
}
