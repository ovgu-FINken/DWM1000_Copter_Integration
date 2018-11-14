#include "thread_logic.h"
EventQueue IRQqueue(32 * EVENTS_EVENT_SIZE);
EventQueue DWMqueue(16 * EVENTS_EVENT_SIZE);
Thread t_irq;
