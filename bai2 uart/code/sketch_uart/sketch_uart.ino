#include <SPI.h>
#include <UIPEthernet.h> // Sử dụng thư viện UIPEthernet

// --- Cấu hình mạng ---
// Đặt địa chỉ MAC (phải là duy nhất trong mạng của bạn)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Đặt địa chỉ IP tĩnh cho Arduino (phải phù hợp với mạng của bạn)
// Ví dụ: nếu modem của bạn là 192.168.0.1, hãy dùng 192.168.0.177
IPAddress ip(192.168.2.27); 

// Khởi tạo máy chủ web trên cổng 80 (cổng HTTP tiêu chuẩn)
EthernetServer server(80);

void setup() {
  // Bắt đầu Serial Monitor để gỡ lỗi
  Serial.begin(9600);
  while (!Serial) {
    ; // Chờ cổng serial kết nối
  }

  Serial.println("Dang khoi dong may chu web...");

  // Khởi động kết nối Ethernet và máy chủ
  // Sơ đồ của bạn dùng Pin 10 làm CS (Chip Select),
  // đây là chân SS mặc định cho SPI trên Uno,
  // vì vậy thư viện UIPEthernet sẽ tự động sử dụng nó.
  Ethernet.begin(mac, ip);
  server.begin();

  // Kiểm tra xem phần cứng ENC28J60 có được tìm thấy không
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Khong tim thay module Ethernet ENC28J60. Kiem tra lai day noi!");
    while (true) {
      delay(1); // Dừng vĩnh viễn
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Day mang chua duoc cam.");
  }

  // In địa chỉ IP của máy chủ ra Serial Monitor
  Serial.print("May chu web dang chay tai IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Lắng nghe các client (trình duyệt) kết nối đến
  EthernetClient client = server.available();
  
  if (client) { // Nếu có một client kết nối
    Serial.println("Co client moi ket noi!");
    
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) { // Nếu client có gửi dữ liệu
        char c = client.read();
        Serial.write(c); // In yêu cầu của client ra Serial
        
        // Khi nhận được một dòng trống (kết thúc HTTP request)
        if (c == '\n' && currentLineIsBlank) {
          
          // --- Bắt đầu gửi phản hồi HTTP ---
          
          // Header HTTP
          client.println("HTTP/1.1 200 OK");
          // Quan trọng: Thêm charset=UTF-8 để hiển thị tiếng Việt
          client.println("Content-Type: text/html; charset=UTF-8");
          client.println("Connection: close");  // Báo client đóng kết nối sau khi gửi
          client.println(); // Dòng trống bắt buộc
          
          // --- Nội dung trang web (HTML) ---
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Arduino Web Server</title>");
          // Thêm meta charset để đảm bảo
          client.println("<meta charset='UTF-8'>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Chao cac ban</h1>"); // Dòng chữ bạn yêu cầu
          client.println("</body>");
          client.println("</html>");
          
          break; // Kết thúc vòng lặp while
        }
        
        // Theo dõi xem dòng hiện tại có trống không
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    
    // Đợi một chút để client nhận dữ liệu
    delay(1);
    
    // Đóng kết nối
    client.stop();
    Serial.println("Client da ngat ket noi.");
  }
}