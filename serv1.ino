#include <iarduino_OLED_txt.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IRremote.h> // Припустимо, що ви використовуєте бібліотеку IRremote

iarduino_OLED_txt myOLED(0x3C);

extern uint8_t SmallFontRus[];
extern uint8_t MediumFontRus[];

#define okpin 15 // на портабле а на клок 16
#define backpin 14
#define UP_BUTTON_PIN 12
#define DOWN_BUTTON_PIN 11
#define IR_RECEIVEE_PIN 0 // Приймач іЧ
#define IR_SENDE_PIN 5    // Передавач іЧ

// Для підключення до NTP сервера
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// Об'єкти бібліотеки IRremote
IRrecv irReceiver(IR_RECEIVEE_PIN);
IRsend irSender(IR_SENDE_PIN);


const char *wifiSSIDs[] = {"WiFi2", "defaulnt", "WiFi3", "WiFi4", "WiFi5", "WiFi6", "WiFi7", "WiFi8", "WiFi9", "WiFi10"};
const char *wifiPasswords[] = {"password2", "wificonnecting.def", "password3", "password4", "password5", "password6", "password7", "password8", "password9", "password10"};

int selectedMenuItem = 1;
int selectedwMenuItem = 0;
int autofliper = 1; // Змінна для автоматичного зміщення пункту меню
byte menuloop = 1;
unsigned long lastAutoflipMillis = 0; // Зберігаємо час останнього автоматичного зміщення
unsigned long lastSyncTime = 0; // Зберігаємо час останньої синхронізації інтернет-годинника
unsigned long syncInterval = 30000; // Інтервал синхронізації інтернет-годинника (1 хвилина(60000))(30 секунд(30000))
char UTC = 0;//your time zone (utc)
int houers;


void irMenu() {myOLED.clrScr();
  menuloop = 1;
  while (menuloop == 1) {
    if (WiFi.status() != WL_CONNECTED) {myOLED.print(F("V"), 70, 0);}
    if (WiFi.status() == WL_CONNECTED) {myOLED.print(F("W"), 70, 0);}
    myOLED.print(F("IR Receiver/Transmitter"), 10, 1);
    myOLED.print(F("exit"), 20, 2);
    myOLED.print(F("Прийняти"), 20, 3);
    myOLED.print(F("Надiслати"), 20, 4);

    myOLED.print(F(" "), 0, selectedMenuItem - 1);
    myOLED.print(F(">"), 0, selectedMenuItem);
    myOLED.print(F(" "), 0, selectedMenuItem + 1);

    if (autofliper == 1) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastAutoflipMillis >= 2000) { // Кожні 2 секунди
        lastAutoflipMillis = currentMillis;
        selectedMenuItem += 1;
      }
    }

    if (selectedMenuItem > 4) {
      selectedMenuItem = 1;
      myOLED.print(F(" "), 0, 4);
    }
    if (selectedMenuItem < 1) {
      selectedMenuItem = 4;
    }

    if (digitalRead(okpin) == HIGH) {
      handleIrSubMenu();
      delay(500); // Затримка для запобігання повторенню натискань
    }

    delay(300);
  }
}

void handleIrSubMenu() {
  switch (selectedMenuItem) {
    case 1:
      menu();
      break;
    case 2:
      menu();
      break;
    case 3:
      receiveIRSignal();
      break;
    case 4:
      sendIRSignal();
      break;
    default:
      break;
  }
}

void receiveIRSignal() {
  // Чекаємо на сигнал від пульта
  if (irReceiver.decode()) {
    // Виводимо код сигналу у консоль
    Serial.print("Received IR signal: ");
   unsigned int irgata = irReceiver.decodedIRData.decodedRawData;
    Serial.println(irReceiver.decodedIRData.decodedRawData, HEX);
    myOLED.print(irgata, 5, 0);
    delay(3000);
    // Очищаємо буфер приймача
    irReceiver.resume();
    
  }
}

void sendIRSignal() {myOLED.clrScr();
delay(700);
  menuloop = 1;
  while (menuloop == 1) {
    // Список сигналів для відправки
    const char *irSignals[] = {"Signal1", "Signal2", "Signal3", "Signal4", "Signal5", "Signal6", "Signal7"};
    myOLED.print(F("Виберiть сигнал"), 5, 0);

    myOLED.print(F("11"), 20, 1);
    myOLED.print(F("12"), 20, 2);
    myOLED.print(F("13"), 20, 3);
    myOLED.print(F("14"), 20, 4);
    myOLED.print(F("15"), 20, 5);
    myOLED.print(F("16"), 20, 6);
    myOLED.print(F("17"), 20, 7);

    myOLED.print(F(" "), 0, selectedMenuItem - 1);
    myOLED.print(F(">"), 0, selectedMenuItem);
    myOLED.print(F(" "), 0, selectedMenuItem + 1);

    if (autofliper == 1) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastAutoflipMillis >= 2000) { // Кожні 2 секунди
        lastAutoflipMillis = currentMillis;
        selectedMenuItem += 1;
      }
    }

    if (selectedMenuItem > 7) {
      selectedMenuItem = 1;
      myOLED.print(F(" "), 0, 7);
    }
    if (selectedMenuItem < 1) {
      selectedMenuItem = 7;
    }

    if (digitalRead(okpin) == HIGH) {myOLED.clrScr();
      // Надсилаємо вибраний іЧ-сигнал
      int signalIndex = selectedMenuItem - 1;
      if (signalIndex >= 0 && signalIndex < sizeof(irSignals) / sizeof(irSignals[0])) {
        const char *selectedSignal = irSignals[signalIndex];
      irSender.sendNEC(strtoul(selectedSignal, NULL, 16), 32);


        myOLED.print(F("Надiслано: "), 5, 0);
        myOLED.print(selectedSignal, 5, 1);
        delay(1000);
        myOLED.clrScr();
        break; // Виходимо з циклу після надсилання сигналу
      }
    }

    delay(300);
  }
}


void handleOkButton() {
  switch (selectedMenuItem) {
    case 1:
      
      break;
    // Додайте обробку для інших пунктів меню за аналогією
    case 2:
      internetClock();
      break;
    case 3:
      irMenu();
      break;
    case 4:
      irMenu();
      break;
    default:
      break;
  }
}

void internetClock() {myOLED.clrScr();
  while(1){
    delay(1000);
   
   
  if (WiFi.status() != WL_CONNECTED) {
    myOLED.print(F("Немає інтернету"), 5, 0);
    menuWifi(); // Спроба підключення до наступної WiFi мережі
  } else {
    // Оновлення часу з інтернету
   
    myOLED.setFont(MediumFontRus);
    myOLED.print(houers, 35, 4);
    myOLED.print(F(":"), 57, 4);
    myOLED.print(timeClient.getMinutes(), 70, 4);
    myOLED.setFont(SmallFontRus);
  }

  // Check if it's time to sync from Internet
  if (millis() - lastSyncTime > syncInterval && WiFi.status() == WL_CONNECTED) {
    Serial.println(F("Синхронізація часу з Iнтернету..."));
  timeClient.update();
  lastSyncTime = millis();
  houers =  timeClient.getHours();
  houers += UTC;
    myOLED.clrScr();
  }
  
if (digitalRead(okpin) == HIGH) {
      delay(1000); // Затримка для запобігання повторенню натискань
      if (digitalRead(okpin) == HIGH) { // Якщо кнопку ще тримають, вийти з циклу
        break;
        return;
      }
    }
  }
}

void menuWifi(){ myOLED.clrScr();
delay(300);
   menuloop = 1;
  while (menuloop = 1) {
    if (WiFi.status() != WL_CONNECTED){myOLED.print(F("V"), 70, 0);}
    if (WiFi.status() == WL_CONNECTED){myOLED.print(F("W"), 70, 0);
    return;}
    myOLED.print(wifiSSIDs[0], 20, 1);
    myOLED.print(wifiSSIDs[1], 20, 2);
    myOLED.print(wifiSSIDs[2], 20, 3);
    myOLED.print(wifiSSIDs[3], 20, 4);
    myOLED.print(wifiSSIDs[4], 20, 5);
    myOLED.print(wifiSSIDs[5], 20, 6);
    myOLED.print(wifiSSIDs[6], 20, 7);

    myOLED.print(F(" "), 0, selectedwMenuItem);
    myOLED.print(F(">"), 0, selectedwMenuItem + 1);
    myOLED.print(F(" "), 0, selectedwMenuItem + 2);

  /*  if (digitalRead(UP_BUTTON_PIN) == HIGH) {
      selectedMenuItem--;
      delay(500); // Затримка для запобігання повторенню натискань
    }

    if (digitalRead(DOWN_BUTTON_PIN) == HIGH) {
      selectedMenuItem++;
      delay(500); // Затримка для запобігання повторенню натискань
    }
*/
    if (autofliper == 1) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastAutoflipMillis >= 2000) { // Кожні 2 секунди
        lastAutoflipMillis = currentMillis;
        selectedwMenuItem += 1;
      }
    }

    if (selectedwMenuItem > 20) {
      selectedMenuItem = 1;
      
    }
    if (selectedwMenuItem < 1) {
      selectedMenuItem = 20;
    }

    if (digitalRead(okpin) == HIGH) {
      connectToWiFi();
      delay(500); // Затримка для запобігання повторенню натискань
    }

    delay(300);
    
  }
}
void connectToWiFi() { myOLED.clrScr();
delay(200);
  int numNetworks = sizeof(wifiSSIDs) / sizeof(wifiSSIDs[0]);
  int selectedNetwork = 0; // Початкова точка доступу, яка вибрана
 for (int i = 0; i < numNetworks; ++i) {
    myOLED.print("Спроба пiдключення", 5, 0);
    myOLED.print(wifiSSIDs[selectedwMenuItem], 5, 1);

    WiFi.begin(wifiSSIDs[selectedwMenuItem], wifiPasswords[selectedwMenuItem]);

    int attemptCounter = 0;
    while (WiFi.status() != WL_CONNECTED && attemptCounter < 30) {
      delay(500);
      Serial.print(".");
      myOLED.print(".", 0, 4);
      myOLED.print(".", attemptCounter, 4);
      myOLED.print(".", 31, 4);
      attemptCounter++;
  if (digitalRead(okpin) == HIGH) {
      delay(1000); // Затримка для запобігання повторенню натискань
      if (digitalRead(okpin) == HIGH) { // Якщо кнопку ще тримають, вийти з циклу
        menu(); }}
    }

    if (WiFi.status() == WL_CONNECTED) {
      myOLED.print("Пiдключено до", 5, 0);
      myOLED.print(wifiSSIDs[selectedwMenuItem], 5, 1);
      return; // Виходить з функції, якщо підключено до WiFi
    } else {
      myOLED.print("Не вдалося пiдключитися", 5, 0);
      delay(500);
      selectedwMenuItem = i;
      myOLED.clrScr();
    }
  }

}


void menu() {//menur
  myOLED.clrScr();
delay(1000);
menuloop = 1;
  while (menuloop = 1) {
    if (WiFi.status() != WL_CONNECTED) {myOLED.print(F("V"), 70, 0);}
    if (WiFi.status() == WL_CONNECTED) {myOLED.print(F("W"), 70, 0);}
    myOLED.print(F("Пункт меню 2"), 20, 1);
    myOLED.print(F("Clock & WIFI"), 20, 2);
    myOLED.print(F("IR send/recive"), 20, 3);
    myOLED.print(F("433Mhz menu"), 20, 4);
    myOLED.print(F("Games"), 20, 5);
    myOLED.print(F("Пункт меню 6"), 20, 6);
    myOLED.print(F("Пункт меню 7"), 20, 7);

    myOLED.print(F(" "), 0, selectedMenuItem - 1);
    myOLED.print(F(">"), 0, selectedMenuItem);
    myOLED.print(F(" "), 0, selectedMenuItem + 1);

  /*  if (digitalRead(UP_BUTTON_PIN) == HIGH) {
      selectedMenuItem--;
      delay(500); // Затримка для запобігання повторенню натискань
    }

    if (digitalRead(DOWN_BUTTON_PIN) == HIGH) {
      selectedMenuItem++;
      delay(500); // Затримка для запобігання повторенню натискань
    }
*/
    if (autofliper == 1) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastAutoflipMillis >= 2000) { // Кожні 2 секунди
        lastAutoflipMillis = currentMillis;
        selectedMenuItem += 1;
      }
    }

    if (selectedMenuItem > 7) {
      selectedMenuItem = 1;
      myOLED.print(F(" "), 0, 7);
    }
    if (selectedMenuItem < 1) {
      selectedMenuItem = 7;
    }

    if (digitalRead(okpin) == HIGH) {
      handleOkButton();
      delay(500); // Затримка для запобігання повторенню натискань
    }

    delay(300);
    
  }
}

void setup() {
  Serial.begin(115200);
  myOLED.begin();
  myOLED.setFont(SmallFontRus);
  myOLED.clrScr();
  myOLED.print(F("AP: "), 5, 0);
  myOLED.setCoding(TXT_UTF8);
  myOLED.print(F("beta"), OLED_C, 4);
  delay(800);
  myOLED.clrScr();
  pinMode(okpin, INPUT_PULLUP);
  timeClient.update();
  irReceiver.enableIRIn();
  menu();
}

void loop() {
  // Ваш існуючий код для обробки HTTP запитів або іншої логіки
 
  if (millis() - lastSyncTime > syncInterval) {
    // Оновлення інтернет-годинника кожну хвилину
    timeClient.update();
    lastSyncTime = millis();
  }
}

void syncTimeFromInternet() {
  
}
