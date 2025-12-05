/*
  Project: DHT11 Monitor to WebApp
  Library: Bonezegei_DHT11
  Modified for: Node.js Serial Communication
*/

#include <Bonezegei_DHT11.h>

// Cấu hình chân DHT11 nối vào Pin 13 (theo sơ đồ Proteus)
Bonezegei_DHT11 dht(13);

void setup() {
  // Tốc độ Serial bắt buộc là 9600 để khớp với file server.js
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Lấy dữ liệu từ cảm biến
  if (dht.getData()) {
    float tempDeg = dht.getTemperature();  // Lấy nhiệt độ (C)
    int hum = dht.getHumidity();           // Lấy độ ẩm (%)
   
    // --- PHẦN QUAN TRỌNG NHẤT ---
 
    
    Serial.print(tempDeg);
     Serial.print(",");
    Serial.println(hum);
  }
 
  // Đợi 2 giây cho lần đọc tiếp theo
  delay(2000);
}