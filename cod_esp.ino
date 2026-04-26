#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "OCTA";       
const char* password = "12345670"; 

WebServer server(80);
String vals[13] = {"0","0","0","0","0","0","0","0","0","0","0","0","0"};

const char PAGE_HTML[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { background: #0b0e14; color: #fff; font-family: 'Segoe UI', sans-serif; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }
  
  #status-bar { 
    position: sticky; top: 0; width: 100%; padding: 15px 0; 
    text-align: center; font-weight: bold; font-size: 1.1em; 
    z-index: 1000; transition: 0.5s; text-transform: uppercase;
  }
  .st-safe { background: #238636; }
  .st-warn { background: #d29922; color: black; }
  .st-danger { background: #f85149; animation: blink 1s infinite; }
  @keyframes blink { 0% {opacity: 1;} 50% {opacity: 0.7;} 100% {opacity: 1;} }

  .container { width: 95%; max-width: 500px; padding: 20px 0; display: flex; flex-direction: column; gap: 15px; }
  
  .card { background: #1a202c; border-radius: 15px; padding: 20px; border-left: 10px solid #30363d; display: flex; justify-content: space-between; align-items: center; }
  .label { color: #8b949e; font-size: 0.85em; text-transform: uppercase; font-weight: bold; }
  .val { font-size: 1.7em; font-weight: 900; color: #00ffcc; }

  .gps-card { background: #161b22; border: 2px solid #4285F4; border-radius: 15px; padding: 20px; text-align: center; }
  .maps-link { 
    display: inline-flex; align-items: center; justify-content: center;
    background: #4285F4; color: white; text-decoration: none; 
    padding: 15px; border-radius: 50px; font-weight: bold; width: 90%; margin-top: 10px;
  }
</style>
</head><body>
  <div id="status-bar" class="st-safe">SISTEM ONLINE</div>
  <div class="container" id="content"></div>

  <script>
    const labels = ["T-Medie", "Gaz", "Apă", "Sol", "LDR", "T1", "T2", "Lux", "Lat", "Lng", "H1", "H2", "H-Medie"];

    function update() {
      fetch('/data').then(r => r.json()).then(d => {
        let h = "";
        
        // Logica Status Bar
        let gas = parseFloat(d.v1);
        let sBar = document.getElementById('status-bar');
        if(gas > 800) { sBar.className="st-danger"; sBar.innerText="🚨 PERICOL: GAZ/FUM!"; }
        else if(gas > 500) { sBar.className="st-warn"; sBar.innerText="⚠️ ATENȚIE: PARAMETRI RIDICAȚI"; }
        else { sBar.className="st-safe"; sBar.innerText="SISTEM OPTIM"; }

        for(let i=0; i<13; i++) {
          if(i == 8) continue; 
          if(i == 9) { 
             let lat = d.v8; let lng = d.v9;
             // Verificam daca avem coordonate valide (nu 0)
             let coordsText = (lat == "0.000000" || lat == "0") ? "CĂUTARE SATELIȚI..." : lat + " , " + lng;
             
             h += `<div class="gps-card">
                    <div style="color:#8b949e; font-size:0.8em; margin-bottom:5px;">LOCAȚIE GPS LIVE</div>
                    <div style="font-weight:bold; margin-bottom:10px;">${coordsText}</div>
                    <a href="https://www.google.com/maps?q=${lat},${lng}" target="_blank" class="maps-link">
                      🗺️ VEZI PE HARTĂ
                    </a>
                  </div>`;
             continue;
          }
          h += `<div class="card"><span class="label">${labels[i]}</span><span class="val">${d['v'+i]}</span></div>`;
        }
        document.getElementById('content').innerHTML = h;
      });
    }
    setInterval(update, 1000);
  </script>
</body></html>
)=====";

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.on("/", []() { server.send(200, "text/html", PAGE_HTML); });
  server.on("/data", []() {
    String j = "{";
    for(int i=0; i<13; i++) { j += "\"v" + String(i) + "\":\"" + vals[i] + "\"" + (i < 12 ? "," : ""); }
    j += "}";
    server.send(200, "application/json", j);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial2.available()) {
    String s = Serial2.readStringUntil('\n');
    s.trim();
    if (s.startsWith("DATA:")) {
      s.remove(0, 5);
      int lastPos = 0, count = 0;
      while (count < 12) {
        int pos = s.indexOf(',', lastPos);
        if (pos == -1) break;
        vals[count++] = s.substring(lastPos, pos);
        lastPos = pos + 1;
      }
      vals[12] = s.substring(lastPos);
    }
  }
}