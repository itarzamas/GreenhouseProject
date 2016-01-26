#ifndef _SMS_MODULE_H
#define _SMS_MODULE_H

#include "AbstractModule.h"
#include "Settings.h"

enum SMSOperation // какой ответ на какую операцию мы ждём?
{
  opIdle, // свободны
  opCheckReady, // ждём ответ на запрос AT+CPAS
  opCheckRegistration, // ждём ответ на запрос AT+CREG?
  opEchoOff, // ждём ответ на запрос ATE0
  opAONEnable, // ждём ответ на включение АОН, AT+CLIP=1
  opSetPDUEncoding, // ждём ответ на запрос установки кодировки PDU, AT+CMGF=0
  opSetSMSOutput, // ждём ответ на запрос установки отображения входящих СМС сразу в порт, AT+CNMI=2,2
  opWaitForIncomingData, // просто ждём входящих данных
  opWaitForSMSSendComplete // ждём, когда отправится СМС
  
};

class SMSModule : public AbstractModule, public Stream // модуль поддержки управления по SMS
{
  private:
    GlobalSettings* Settings;

    SMSOperation currentOperation; // какую команду мы запросили?
    String currentCommand; // какую команду надо отправить модулю NEOWAY?
    String neowayAnswer; // текущий ответ от модуля NEOWAY

    bool isModuleReady; // готов модуль к работе или нет?
    bool isModuleRegistered; // зарегистрирован ли модуль у оператора?
    int16_t needToWaitTimer; // таймер ожидания до запроса следующей команды

    String incomingData; // входящие данные, которые модуль сыпет при приёме SMS, например

    void SendToNeoway(const String& s, bool addNewLine=true);
    bool IsNeowayAnswerCompleted(const String& s, bool& isOkAnswer); // проверяем, полный ответ или нет?
    bool IsNeowayReady(); // проверяет, готов ли модуль к работе?
    void FetchNeowayAnswer(bool& isOkAnswer); // получает данные до тех пор, пока не будет получена строка OK или ERROR

    String streamAnswer; // ответ от другого модуля будет здесь

    void ParseIncomingSMS(const String& sms);
    
  public:
    SMSModule() : AbstractModule(F("SMS")) {}

    bool ExecCommand(const Command& command);
    void Setup();
    void Update(uint16_t dt);

    void SendStatToCaller(const String& phoneNum);
    void SendSMS(const String& sms);


    // переопределяем фунции Stream тут
    virtual int available(){ return false; };
    virtual int read(){ return -1;};
    virtual int peek(){return -1;};
    virtual void flush(){};

    virtual size_t print(const String &s);
    virtual size_t println(const String &s);
    virtual size_t write(uint8_t toWr);
        

};


#endif
