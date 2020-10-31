#include "rhio-puf.h"

Puf::Puf() { reset(); }

unsigned char Puf::reset() {
  for (auto i = 0; i < _PUF_MAX_EVENTS; i++) {
    eventNames[i] = '\n';
    for (auto j = 0; j < _PUF_MAX_CALLBACKS; j++) {
      callbacks[i][j] = 0;
    }
  }
  return 0;
}

unsigned char Puf::on(unsigned char eventName, pufcb (*callback)(int param)) {
  int eventIndex = (int)findNextIndex(eventName);

  if (eventIndex == -1) {
    // the stack is full, no space for more events
    return 1;
  }

  // save event name
  eventNames[(unsigned char)eventIndex] = eventName;

  // save event callback
  int callbackIndex = (int)findNextCallback(eventIndex);
  if (callbackIndex == -1) {
    return 2;
  }

  // save callback
  callbacks[eventIndex][callbackIndex] = callback;

  return 0;
}

unsigned char Puf::emit(unsigned char eventName, int param) {
  // buscamos el indice en eventNames donde se encuentra eventName
  int eventIndex = -1;
  for (auto i = 0; i < _PUF_MAX_EVENTS; i++) {
    if (eventNames[i] == eventName) {
      eventIndex = i;
      break;
    }
  }
  // el evento {eventName} no se ha encontrado
  if (eventIndex == -1) return 1;

  for (auto j = 0; j < _PUF_MAX_CALLBACKS; j++) {
    if (callbacks[eventIndex][j] == 0) continue;
    // ejecutamos el callback
    unsigned char callbackErr = (*callbacks[eventIndex][j])(param);
    if (callbackErr != 0) {
      return callbackErr;
    }
  }

  return 0;
}

unsigned char Puf::emit(unsigned char *eventNames, int len, int param) {
  for (auto i = 0; i < len; i++) {
    unsigned char callbackErr = emit(eventNames[i], param);
    if (callbackErr > 0) {
      return callbackErr;
    }
  }

  return 0;
}

unsigned char Puf::off(unsigned char eventName, pufcb (*callback)(int param)) {
  // para eliminar un callback primero tenemos que deducir el indice
  // del eventName
  int eventIndex = findEventName(eventName);
  if (eventIndex == -1) return 1;

  // Con el indice del evento buscamos en el listado de eventNames y callbacks
  if (callbacks[eventIndex][0] == callback) {
    callbacks[eventIndex][0] = 0;
  }

  return 0;
}

/**
 * @brief Returns the index of the given eventName or -1 if not exists
 *
 * @param eventName
 * @return char
 */
char Puf::findEventName(unsigned char eventName) {
  for (auto i = 0; i < _PUF_MAX_EVENTS; i++) {
    if (eventNames[i] == eventName) {
      return i;
    }
  }
  return -1;
}

char Puf::findNextIndex(unsigned char eventName) {
  // si el evento existe devolvemos su indice
  char eventExists = findEventName(eventName);
  if (eventExists > -1) {
    return eventExists;
  }

  // si no existe devolvemos un nuevo indice
  for (auto i = 0; i < _PUF_MAX_EVENTS; i++) {
    if (eventNames[i] == '\n') {
      return i;
    }
  }

  // ni existe ni hay indices libres
  // no se puede añadir el evento a la lista
  return -1;
}

char Puf::findNextCallback(int eventIndex) {
  for (auto i = 0; i < _PUF_MAX_CALLBACKS; i++) {
    if (callbacks[eventIndex][i] == 0) {
      return i;
    }
  }
  return -1;
}

bool Puf::eventHasCallback(unsigned char eventName,
                           pufcb (*callback)(int param)) {
  // comprobamos primero si el evento existe
  int eventIndex = (int)findEventName(eventName);
  if (eventIndex == -1) return false;

  // comprobamos ahora si tiene el callback
  for (auto j = 0; j < _PUF_MAX_CALLBACKS; ++j) {
    if (callbacks[eventIndex][j] == callback) {
      // menea tu trasero
      return true;
    }
  }

  // :(
  return false;
}
