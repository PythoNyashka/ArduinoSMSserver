#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

// создаём объект mySerial и передаём номера управляющих пинов RX и TX
SoftwareSerial mySerial(8, 9);

// введите ниже MAC-адрес и IP-адрес вашего контроллера;
// IP-адрес будет зависеть от вашей локальной сети:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 1, 111);

// инициализируем библиотеку Ethernet Server, указывая нужный порт
// (по умолчанию порт для HTTP – это «80»):
EthernetServer server(80);

// задаем переменные для клиента:
char linebuf[80];
int charcount = 0;

void setup() {

  // открываем последовательную коммуникацию на скорости 9600 бод:
  Serial.begin(9600);

  // открываем Serial-соединение с GPRS Shield
  mySerial.begin(9600);

  // запускаем Ethernet-коммуникацию и сервер:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");  //  "сервер на "
  Serial.println(Ethernet.localIP());

}

// Показываем веб-страницу
void dashboardPage(EthernetClient &client) {
  client.println("HTTP/1.1 200 OK");
}

// Показываем веб-страницу
void errorPage(EthernetClient &client) {
  client.println("HTTP/1.1 400 Bad Request");
}


void loop() {
  // прослушиваем входящих клиентов:
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");  //  "новый клиент"
    memset(linebuf, 0, sizeof(linebuf));
    charcount = 0;
    // HTTP-запрос заканчивается пустой строкой:
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // считываем HTTP-запрос, символ за символом:
        linebuf[charcount] = c;
        if (charcount < sizeof(linebuf) - 1) charcount++;
        // если вы дошли до конца строки (т.е. если получили
        // символ новой строки), это значит,
        // что HTTP-запрос завершен, и вы можете отправить ответ:
        if (c == '\n' && currentLineIsBlank) {
          errorPage(client);
          break;
        }
        else if (c == '\n') {
          if (strstr(linebuf, "GET /sendsms") > 0) {
            String buf(linebuf);
            String mur = getValue(buf, '?', 1);
            mur.replace(" HTTP/1.1", "");

            String tel = getValue(mur, '&', 0);
            tel.replace("tel=", "");
            String telwith = "+" + tel;

            String msg = getValue(mur, '&', 1);
            msg.replace("msg=", "");
            msg.replace("%20", " ");

            Serial.println(mur);
            Serial.println(telwith);
            Serial.println(msg);

            String tosend = telwith + ":" + msg;

            mySerial.println(tosend);

            dashboardPage(client);
          }
          // если получили символ новой строки...
          currentLineIsBlank = true;
          memset(linebuf, 0, sizeof(linebuf));
          charcount = 0;
        }
        else if (c != '\r') {
          // если получили какой-то другой символ...
          currentLineIsBlank = false;
        }
      }
    }
    // даем веб-браузеру время на получение данных:
    delay(1);
    // закрываем соединение:
    client.stop();
    Serial.println("client disonnected");  //  "Клиент отключен"
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
