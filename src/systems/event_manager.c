#include <string.h>
#include <systems.h>

void InitEventManager(struct Systems* systems) {
  EventManager* em = &systems->eventManager;
<<<<<<< HEAD
  
  // Clear the queue and memory
  em->eventCounter = 0;

=======

  // Initialize event queue
  em->eventCounter = 0;
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  memset(em->eventQueue, 0, sizeof(em->eventQueue));
}

void PushEvent(struct Systems* systems, EventType type, EventData data) {
<<<<<<< HEAD

  EventManager* em = &systems->eventManager;
  
  // Just overwrite them
  if (em->eventCounter < MAX_EVENTS_QUANTITY) {

    em->eventQueue[em->eventCounter].type = type;
    em->eventQueue[em->eventCounter].data = data;

    em->eventCounter++;
  } else { 
=======
  EventManager* em = &systems->eventManager;

  // Add event to queue if there's space
  if (em->eventCounter < MAX_EVENTS_QUANTITY) {
    em->eventQueue[em->eventCounter].type = type;
    em->eventQueue[em->eventCounter].data = data;
    em->eventCounter++;
  }
  else { 
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
    TraceLog(LOG_WARNING, "Event queue is full! Event lost.");
  }
}

void ProcessGameEvents(struct Systems* systems) {
  EventManager* ev = &systems->eventManager;

<<<<<<< HEAD
  for (int i = 0; i < ev->eventCounter; i++) {
    Event event = ev->eventQueue[i];

    AudioManagerOnEvent(systems, event); 
    EffectSystemOnEvent(systems, event);
  }

  ClearEventManager(ev);
}

void ClearEventManager(EventManager* em) {
=======
  // Process all events in the queue
  for (int i = 0; i < ev->eventCounter; i++) {
    Event event = ev->eventQueue[i];
    AudioManagerOnEvent(systems, event); 
    EffectSystemOnEvent(systems, event);
  }
}

void ClearEventManager(EventManager* em) {
  // Reset event counter (queue will be overwritten on next PushEvent)
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  em->eventCounter = 0;
}




