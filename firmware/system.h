#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

// =====================================================
//  CONFIGURAÇÃO GLOBAL
// =====================================================

#define SERIAL_MONITOR_ENABLED 1

// =====================================================
//  DEBUG MACROS
// =====================================================

#if SERIAL_MONITOR_ENABLED
  #define LOG(x)   Serial.print(x)
  #define LOGLN(x) Serial.println(x)
#else
  #define LOG(x)
  #define LOGLN(x)
#endif

#endif