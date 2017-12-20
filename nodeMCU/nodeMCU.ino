// #include สองบรรทัดด้านล่างนี้ต้องมีเสมอ
#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char *ssid = "GetOffMyLawn";         // ใส่ชื่อ WiFi SSID แทน SSID
const char *password = "kzar2448"; // ใส่รหสัผ่าน WiFi แทน PASSWORD

#define APPID "HwSyncLab"   // ให้ YOUR_APPID แทนที่ด้วย AppID
#define KEY "vtnrUFRthoQIYxp" // ให้ YOUR_KEY แทนที่ด้วย Key
#define SECRET "y2TcYPHSdhCHdM4oj90yS2ai1" // ให้ YOUR_SECRET แทนที่ด้วย Secret
#define ALIAS "nodeMCU"     // ตงั้ชื่อเล่นให้ device นี้ เป็น pieblink

#define D5 14
#define D6 12
#define D7 13
#define D8 15

int d7;
int d8;


WiFiClient client;

int timer = 0;
char state = 0;
MicroGear microgear(client); // ประกาศตัวแปร microgear

// สร้างฟังก์ชั ่นที่จะถูกเรียกเมื่อมีข้อความเข้ามา
void onMsghandler(char *topic, uint8_t *msg, unsigned int msglen)
{
    Serial.print("Incoming message -->");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    String m = (char *)msg;

    // ถ้าข้อความที่เข้ามาเป็น 1 ให้เปิด LED ถ้าเป็น 0 ให้ปิด LED
    if (m == "AUTOMATIC")
    {
        digitalWrite(D5, LOW); // LDR MODE
        Serial.println("changing to LDR mode");
        
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if(m == "MANUAL")
    {
        digitalWrite(D5, HIGH); // MANUAL MODE
        
        Serial.println("changing to Manual mode");
        
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if(m == "TOGGLE") // TOGGLE SWITCH
    {
      Serial.println("Toggling");
        if(digitalRead(D6) == HIGH)
        {
            digitalWrite(D6, LOW);
        }
        else
        {
            digitalWrite(D6,HIGH);
        }
    }
    else
    {
      Serial.println("invalid input");
    }
}

// สร้างฟังก์ชั ่นที่จะถูกเรียกเมื่อ Microgear เชื่อมต่อกบั NETPIE ส าเร็จ
void onConnected(char *attribute, uint8_t *msg, unsigned int msglen)
{
    Serial.println("Connected to NETPIE...");
    // เราอาจจะใช้โอกาสนี้ตงั้ชื่อหรือเปลี่ยนชื่อหรืออาจจะทา อะไรบางอย่างเช่น subscribe
    microgear.setAlias(ALIAS);
}

void setup()
{
    // ประกาศให้เวลามีข้อความเข้ามาให้ไปท าฟังก์ชั ่น onMsghandler()
    microgear.on(MESSAGE, onMsghandler);
    // ประกาศให้เมื่อเชื่อมต่อส าเร็จให้ไปท าฟังก์ชั ่น onConnected()
    microgear.on(CONNECTED, onConnected);
    Serial.begin(115200);
    Serial.println("Starting...");
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, INPUT);
    pinMode(D8, INPUT);

    digitalWrite(D5,0);
    digitalWrite(D6,0);

    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, LOW);
    // initiate Wifi
    if (WiFi.begin(ssid, password))
    {
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // initialize ตัวแปร microgear
    microgear.init(KEY, SECRET, ALIAS);
    // เชื่อมต่อไป NETPIE ไปยัง AppID ที่ก าหนด
    microgear.connect(APPID); // ฟังกช์ นั่ เชื่อมต่อ NETPIE
}

void loop()
{
    // เชค็ว่า Microgear ยังเชื่อมต่ออย่หรือ ู ไม่
    if (microgear.connected())
    {                                // ตรวจสอบการเชื่อมต่อ NETPIE
         
                                     // เราต้องเรียก microgear.loop() เป็นระยะ เพื่อรกัษาการเชื่อต่อ
        microgear.loop();
        if (timer >= 1000)
        {
           Serial.println("connected");// พิมพแ์ จ้งการเชื่อมต่อ NETPIE ส าเร็จ
        }
        timer += 100;
        if (timer >= 5000) {
          if(digitalRead(D7) == HIGH && digitalRead(D8) == LOW){
            Serial.println("Sending PROCESSING to Client...");
            microgear.chat("Client","PROCESSING");
          }
          else if(digitalRead(D7) == LOW && digitalRead(D8) == HIGH){
            Serial.println("Sending CLOSED to Client...");
            microgear.chat("Client","CLOSED");
          }
          else if(digitalRead(D7) == LOW && digitalRead(D8) == LOW){
            Serial.println("Sending OPEN to Client...");
            microgear.chat("Client","OPEN");
          }
          timer = 0;
        }
        
    }
    else
    {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000)
        {
            microgear.connect(APPID);
            timer = 0;
        }
        else
            timer += 100;
    }
    delay(100);
}
