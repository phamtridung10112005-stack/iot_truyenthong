const int buttonPin = 2;
const int ledPin = 13;
const long longPressDuration = 3000;
const long clickTimeout = 1500;
const int debounceDelay = 50;

enum Mode { OPERATION, CONFIG };
Mode currentMode = OPERATION;
long currentBaudRate = 9600;
long newBaudRate = 9600;

int lastButtonState = HIGH;
unsigned long buttonPressTime = 0;
int clickCount = 0;
unsigned long lastClickTime = 0;
bool buttonIsPressed = false;
bool longPressHandled = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(currentBaudRate);
  Serial.println("--- He thong khoi dong ---");
  Serial.print("Che do: VAN HANH | Baud: ");
  Serial.println(currentBaudRate);
  digitalWrite(ledPin, LOW);
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(debounceDelay);
    buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) {
      buttonPressTime = millis();
      buttonIsPressed = true;
      longPressHandled = false;
    }
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    if (buttonIsPressed && !longPressHandled) {
      if (currentMode == CONFIG) {
        clickCount++;
        lastClickTime = millis();
        Serial.print("Da nhan: ");
        Serial.println(clickCount);
        blinkLed(1, 50);
      }
    }
    buttonIsPressed = false;
  }

  if (buttonIsPressed && !longPressHandled && (millis() - buttonPressTime > longPressDuration)) {
    if (currentMode == OPERATION) {
      enterConfigMode();
    } else {
      exitConfigMode();
    }
    longPressHandled = true;
  }

  lastButtonState = buttonState;

  if (currentMode == OPERATION) {
    runOperationMode();
  } else {
    runConfigMode();
  }
}

void runOperationMode() {
  static unsigned long lastOpTime = 0;
  if (millis() - lastOpTime > 2000) {
    Serial.println("Dang hoat dong binh thuong...");
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    lastOpTime = millis();
  }
}

void runConfigMode() {
  if (clickCount > 0 && (millis() - lastClickTime > clickTimeout)) {
    Serial.print("Xu ly ");
    Serial.print(clickCount);
    Serial.println(" lan nhan:");
    
    if (clickCount == 1) {
      newBaudRate = 9600;
      Serial.println(">> Da chon: 9600 bps");
      blinkLed(1, 250);
    } else if (clickCount == 2) {
      newBaudRate = 115200;
      Serial.println(">> Da chon: 115200 bps");
      blinkLed(2, 250);
    } else {
      Serial.println(">> So lan nhan khong hop le!");
      blinkLed(3, 100);
    }
    
    Serial.println("--- Vui long chon lai hoac nhan giu de thoat ---");
    clickCount = 0;
  }
}

void enterConfigMode() {
  currentMode = CONFIG;
  Serial.println("\n--- DA VAO CHE DO CAU HINH ---");
  Serial.println("Nhan 1 lan: 9600 | Nhan 2 lan: 115200");
  Serial.println("Nhan giu 3s de thoat.");
  blinkLed(5, 75);
  clickCount = 0;
}

void exitConfigMode() {
  currentMode = OPERATION;
  Serial.println("\n--- DA THOAT CHE DO CAU HINH ---");

  if (newBaudRate != currentBaudRate) {
    currentBaudRate = newBaudRate;
    Serial.print("Ap dung Baud Rate moi: ");
    Serial.println(currentBaudRate);
    Serial.end();
    Serial.begin(currentBaudRate);
    Serial.println("Serial da khoi dong lai!");
  }
  
  Serial.println("Che do: VAN HANH");
  blinkLed(2, 300);
}

void blinkLed(int count, int duration) {
  for (int i = 0; i < count; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW);
    delay(duration);
  }
}