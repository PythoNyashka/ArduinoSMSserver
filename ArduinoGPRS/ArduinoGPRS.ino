// библиотека для работы с GPRS устройством
#include <GPRS_Shield_Arduino.h>

// библиотека для эмуляции Serial порта
// она нужна для работы библиотеки GPRS_Shield_Arduino
#include <SoftwareSerial.h>

// создаём объект mySerial и передаём номера управляющих пинов RX и TX
SoftwareSerial mySerial(10, 11);

// создаём объект класса GPRS и передаём в него объект mySerial 
GPRS gprs(mySerial);
// можно указать дополнительные параметры — пины PK и ST
// по умолчанию: PK = 2, ST = 3
// GPRS gprs(mySerial, 2, 3);

void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
	Serial.begin(9600);
  // открываем Serial-соединение с GPRS Shield
	mySerial.begin(9600);
  // ждём, пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
	while (!Serial) {
	}
	Serial.print("Serial init OK\r\n");
  // включаем GPRS шилд
	gprs.powerOn();
  // проверяем есть ли связь с GPRS устройством
	while (!gprs.init()) {
    // если связи нет, ждём 1 секунду
    // и выводим сообщение об ошибке
    // процесс повторяется в цикле
    // пока не появится ответ от GPRS устройства
		delay(1000);
		Serial.print("Init error\r\n");
	}
  // выводим сообщение об удачной инициализации GPRS Shield
	Serial.println("GPRS init success");
  // отправляем сообщение по указанному номеру с заданным текстом
	//gprs.sendSMS("+79106446908", "Hello SMS from Amperka!");
}

void loop()
{
	while (Serial.available()) {
	    String ttydata = Serial.readString(); // ttyData - информация с серийного порта
	    String telString = getValue(ttydata, ':', 0);
	    String msgString = getValue(ttydata, ':', 1);
	    char tel[50];
	    char msg[50];
	    telString.toCharArray(tel, 50);
	    msgString.toCharArray(msg, 50);
	    gprs.sendSMS(tel, msg);
	}
}

String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
