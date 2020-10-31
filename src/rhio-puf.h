#pragma once

#ifdef PUF_MAX_EVENTS
const int _PUF_MAX_EVENTS = PUF_MAX_EVENTS;
#else
const int _PUF_MAX_EVENTS = 10;
#endif

#ifdef PUF_MAX_CALLBACKS
const int _PUF_MAX_CALLBACKS = PUF_MAX_CALLBACKS;
#else
const int _PUF_MAX_CALLBACKS = 10;
#endif

typedef unsigned char pufcb;

class Puf {
 public:
  Puf();
  unsigned char reset();
  unsigned char on(unsigned char eventName, pufcb (*callback)(int param));
  unsigned char emit(unsigned char eventName, int param);
  unsigned char emit(unsigned char *eventNames, int len, int param);
  unsigned char off(unsigned char eventName, pufcb (*callback)(int param));

#ifndef UNIT_TEST
  // Make private methods as public when running test,
  // so it can be externally tested
 private:
#endif
  unsigned char eventNames[_PUF_MAX_EVENTS];
  unsigned char (*callbacks[_PUF_MAX_EVENTS][_PUF_MAX_CALLBACKS])(int param);

  char findEventName(unsigned char eventName);
  char findNextIndex(unsigned char eventName);
  char findNextCallback(int eventIndex);
  bool eventHasCallback(unsigned char eventName, pufcb (*callback)(int param));
};
