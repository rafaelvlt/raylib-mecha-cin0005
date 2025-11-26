#include <string.h>
#include <systems.h>

void InitEventManager(struct Systems* systems) {
  EventManager* em = &systems->eventManager;
  
  // Clear the queue and memory
  em->eventCounter = 0;

  memset(em->eventQueue, 0, sizeof(em->eventQueue));
}

void PushEvent(struct Systems* systems, EventType type, EventData data) {

  EventManager* em = &systems->eventManager;
  
  // Just overwrite them
  if (em->eventCounter < MAX_EVENTS_QUANTITY) {

    em->eventQueue[em->eventCounter].type = type;
    em->eventQueue[em->eventCounter].data = data;

    em->eventCounter++;
  } else { 
    TraceLog(LOG_WARNING, "Event queue is full! Event lost.");
  }
}

void ProcessGameEvents(struct Systems* systems) {
  EventManager* ev = &systems->eventManager;

  for (int i = 0; i < ev->eventCounter; i++) {
    Event event = ev->eventQueue[i];

    AudioManagerOnEvent(systems, event); 
    EffectSystemOnEvent(systems, event);
  }
}

void ClearEventManager(EventManager* em) {
  em->eventCounter = 0;
}




