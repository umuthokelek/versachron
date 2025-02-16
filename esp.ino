#include <Wire.h> // I2C protokolu ile LCD ile iletisim kurmak icin
#include <LiquidCrystal_I2C.h> // I2C protokolu ile LCD ekranin kontrolu icin

// LCD ekranı başlatmak için I2C protokolü üzerinden bağlantıyı tanımlar.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Donanım pin tanımları
const int rheostatPin = 34; // Rheostat için analog giriş pini (ADC)
const int buttonPin = 14;   // Buton için dijital giriş pini (EXT)
const int ledPin = 13;      // LED için dijital çıkış pini

// Değişken tanımları
int totalSeconds = 0;          // Zamanlayıcı toplam saniye değeri
bool rheostatMode = true;      // Rheostat modu açık mı? (true: rheostat, false: geri sayım)
bool countdownRunning = false; // Geri sayım çalışıyor mu?
volatile bool buttonPressed = false; // Butona basıldığını izlemek için bayrak (interrupt ile kontrol edilir)
unsigned long lastMillis = 0;  // millis() zaman takibi için değişken

// Buton kesme (interrupt) işlevi
void IRAM_ATTR handleButtonPress() {
    buttonPressed = true; // Butona basıldığını gösteren bayrağı aktif et
}

void setup() {
    Serial.begin(115200); // Seri haberleşmeyi başlat
    lcd.init();           // LCD ekranı başlat
    lcd.backlight();      // LCD arka ışığını aç

    // Buton ve LED pinlerinin giriş ve çıkış olarak ayarlanması
    pinMode(buttonPin, INPUT_PULLUP); // Buton için dahili pull-up direnci aktif et
    pinMode(ledPin, OUTPUT);         // LED çıkışı tanımla
    digitalWrite(ledPin, LOW);       // LED başlangıçta kapalı olsun

    // Butona kesme (interrupt) bağla
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);

    // LCD ekranın ilk satırını başlat
    lcd.setCursor(0, 0);
    lcd.print("Timer: 00:00");
    clearSecondLine(); // LCD'nin ikinci satırını temizle
}

void loop() {
    // Kesme dışında buton kontrolü
    if (buttonPressed) {
        buttonPressed = false; // Bayrağı sıfırla
        rheostatMode = !rheostatMode; // Rheostat ve geri sayım modu arasında geçiş yap

        if (rheostatMode) {
            countdownRunning = false; // Rheostat moduna geçildiğinde geri sayımı durdur
            digitalWrite(ledPin, LOW); // LED'i kapat
            clearSecondLine();        // LCD'nin ikinci satırını temizle
        } else {
            countdownRunning = true; // Geri sayım modunu başlat
            clearSecondLine();       // LCD'nin ikinci satırını temizle
        }
    }

    // Rheostat modunda zamanlayıcıyı ayarla
    if (rheostatMode && !countdownRunning) {
        int analogValue = analogRead(rheostatPin); // Rheostat'tan gelen analog sinyali oku
        int mappedValue = map(analogValue, 0, 4095, 0, 3600); // Değeri 0-3600 saniye aralığına dönüştür
        totalSeconds = mappedValue; // Zamanlayıcı değerini güncelle
        displayTime("Set Timer: "); // LCD'de ayarlanan zamanı göster
    }

    // Geri sayım modunda zamanlayıcıyı çalıştır
    if (countdownRunning && totalSeconds > 0 && millis() - lastMillis >= 1000) {
        lastMillis = millis(); // Son zamanı güncelle
        totalSeconds--;        // Zamanlayıcıyı bir saniye azalt
        displayTime("Countdown: "); // LCD'de geri sayımı göster
    }

    // Geri sayım bittiğinde işlemleri gerçekleştir
    if (countdownRunning && totalSeconds == 0) {
        countdownRunning = false; // Geri sayımı durdur
        lcd.setCursor(0, 1);      // LCD'nin ikinci satırına yaz
        lcd.print("Time's up!     ");
        digitalWrite(ledPin, HIGH); // LED'i yak
    }

    // GUI üzerinden gelen seri komutları kontrol et
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Seri bağlantıdan komut al
        command.trim(); // Komutu temizle
        if (command.startsWith("SET:")) {
            totalSeconds = command.substring(4).toInt(); // Zamanlayıcıyı GUI'den gelen değerle ayarla
            countdownRunning = false;                   // Geri sayımı başlatma
            rheostatMode = false;                       // GUI moduna geç
            digitalWrite(ledPin, LOW);                  // LED'i kapat
            clearSecondLine();                          // LCD'yi temizle
            displayTime("Set Timer: ");                 // Zamanlayıcıyı göster
        } else if (command == "START") {
            countdownRunning = true;    // Geri sayımı başlat
            digitalWrite(ledPin, LOW);  // LED'i kapat
            clearSecondLine();          // LCD'yi temizle
        } else if (command == "STOP") {
            countdownRunning = false;   // Geri sayımı durdur
            digitalWrite(ledPin, LOW);  // LED'i kapat
        }
    }
}

// LCD ekranında zamanı göstermek için fonksiyon
void displayTime(const char *label) {
    int minutes = totalSeconds / 60; // Dakika olarak hesapla
    int seconds = totalSeconds % 60; // Saniye olarak hesapla

    lcd.setCursor(0, 0); // LCD'nin birinci satırına git
    lcd.print(label);    // Etiketi yaz
    if (minutes < 10) lcd.print("0"); // Dakikaları iki haneli göster
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0"); // Saniyeleri iki haneli göster
    lcd.print(seconds);
}

// LCD ekranının ikinci satırını temizlemek için fonksiyon
void clearSecondLine() {
    lcd.setCursor(0, 1); // LCD'nin ikinci satırına git
    lcd.print("                "); // İkinci satırı boşluklarla temizle
}

