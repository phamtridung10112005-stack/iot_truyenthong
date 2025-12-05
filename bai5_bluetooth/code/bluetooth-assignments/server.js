const express = require("express");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const cors = require("cors");

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static("public")); // Phục vụ file trong thư mục public

let port;
let isOpen = false;
// Biến lưu trữ dữ liệu cảm biến mới nhất
let currentData = { temp: 0, hum: 0 };

// === 1. KẾT NỐI SERIAL ===
app.post("/connect", (req, res) => {
  if (isOpen) return res.json({ status: "OK", msg: "Already connected" });

  try {
    // Cấu hình cổng COM (LƯU Ý: Sửa 'COM2' thành cổng ảo nhận dữ liệu của bạn)
    port = new SerialPort({
      path: "COM2", 
      baudRate: 9600,
    });

    // Chỉ cần thấy xuống dòng (\n) là nhận, không cần bắt buộc \r
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

    port.on("open", () => {
      isOpen = true;
      console.log("✅ Đã mở cổng COM2");
    });

    // Xử lý dữ liệu nhận được từ Arduino
    parser.on("data", (line) => {
      // Arduino gửi: "28.50,70.00"
      const parts = line.split(","); 
      if (parts.length === 2) {
       currentData.temp = parseFloat(parts[0]).toFixed(1);
        currentData.hum = parseFloat(parts[1]).toFixed(1);
        console.log(`Received -> Temp: ${currentData.temp}, Hum: ${currentData.hum}`);
      }
    });

    port.on("close", () => {
      isOpen = false;
      console.log("❌ Cổng COM đã đóng");
    });

    res.json({ status: "OK" });
  } catch (err) {
    res.json({ status: "ERROR", msg: err.message });
  }
});

// === 2. API LẤY DỮ LIỆU CHO WEB ===
app.get("/data", (req, res) => {
  res.json(currentData);
});

// === 3. NGẮT KẾT NỐI ===
app.post("/disconnect", (req, res) => {
  if (port && isOpen) {
    port.close();
    isOpen = false;
    currentData = { temp: 0, hum: 0 };
  }
  res.json({ status: "OK" });
});

// Chạy server
app.listen(3000, () => console.log("Server chạy tại: http://localhost:3000"));