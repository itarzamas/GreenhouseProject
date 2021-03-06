#ifndef _MODULE_CONTROLLER_H
#define _MODULE_CONTROLLER_H

#include <Arduino.h>
#include "Globals.h"
#include "AbstractModule.h"
#include "CommandParser.h"
#include "TinyVector.h"
#include "Settings.h"
#include "AlarmDispatcher.h"


#ifdef USE_DS3231_REALTIME_CLOCK
#include "DS3231Support.h"
#endif

#ifdef USE_LOG_MODULE
#include "LogModule.h"
#endif

#include <SD.h>

class AbstractModule; // forward declaration
class AlertRule;
typedef Vector<AbstractModule*> ModulesVec;

typedef void (*CallbackUpdateFunc)(AbstractModule* mod);

class ModuleController
{
 private:
  ModulesVec modules; // список зарегистрированных модулей
  
  CommandParser* cParser; // парсер текстовых команд

  GlobalSettings settings; // глобальные настройки

  ReservationResolver* reservationResolver; // держатель списков резервирования

#ifdef USE_DS3231_REALTIME_CLOCK
  DS3231Clock _rtc; // часы реального времени
#endif

#ifdef USE_LOG_MODULE
  LogModule* logWriter;
#endif

#if defined(USE_WIFI_MODULE) || defined(USE_LOG_MODULE) || defined(USE_SMS_MODULE)
  bool sdCardInitFlag;
#endif

  void PublishToCommandStream(AbstractModule* module,const Command& sourceCommand); // публикация в поток команды

#ifdef USE_ALARM_DISPATCHER
  AlarmDispatcher alarmDispatcher;
#endif
  
public:
  ModuleController();

  void Setup(); // настраивает контроллер на работу (инициализация нужных железок и т.п.)
  void begin(); // начинаем работу

  // устанавливает обработчика списков резервирования
  void SetReservationResolver(ReservationResolver* rr) { reservationResolver = rr; }
  // возвращает состояние с зарезервированного списка для датчика модуля, с которого нет показаний
  OneState* GetReservedState(AbstractModule* sourceModule, ModuleStates sensorType, uint8_t sensorIndex);

  bool HasSDCard() 
  {
#if  defined(USE_WIFI_MODULE) || defined(USE_LOG_MODULE) || defined(USE_SMS_MODULE)
    return sdCardInitFlag;
#else
    return false;
#endif
  }
  #ifdef USE_DS3231_REALTIME_CLOCK
  // модуль реального времени
  DS3231Clock& GetClock();
  #endif

  #ifdef USE_LOG_MODULE
  void SetLogWriter(LogModule* lw) {logWriter = lw;}
  #endif

  void Log(AbstractModule* mod, const String& message); // добавляет строчку в лог действий

  // возвращает текущие настройки контроллера
  GlobalSettings* GetSettings() {return &settings;}
 
  size_t GetModulesCount() {return modules.size(); }
  AbstractModule* GetModule(size_t idx) {return modules[idx]; }
  AbstractModule* GetModuleByID(const String& id);

  void RegisterModule(AbstractModule* mod);
  void ProcessModuleCommand(const Command& c, AbstractModule* thisModule=NULL);
  
  void UpdateModules(uint16_t dt, CallbackUpdateFunc func);
  
  void CallRemoteModuleCommand(AbstractModule* mod, const String& command); // вызывает команду с другой коробочки

  void Publish(AbstractModule* module,const Command& sourceCommand); // каждый модуль по необходимости дергает этот метод для публикации событий/ответов на запрос

  void SetCommandParser(CommandParser* c) {cParser = c;};
  CommandParser* GetCommandParser() {return cParser;}

  void Alarm(AlertRule* rule); // обработчик тревог
  #ifdef USE_ALARM_DISPATCHER
    AlarmDispatcher* GetAlarmDispatcher(){ return &alarmDispatcher;}
  #endif
  
};

extern PublishStruct PublishSingleton; // сюда публикуем все ответы от всех модудей
extern ModuleController* MainController; // главный контроллер

#endif
