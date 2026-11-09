/*
 * DEMO: Wi-Fi Access Point (WPA2-PSK) a webový server pro ESP32-C3
 * Předmět: 4IZ112 - Informatika, FIS VŠE
 * 
 * Tento kód vytvoří softwarový Access Point se zabezpečením WPA2-PSK
 * a spustí webový server, který slouží k demonstraci stabilního síťového provozu.
 */

#include <WiFi.h>
#include <WebServer.h>

// Konfigurace Wi-Fi Access Pointu
const char *AP_SSID = "Demo-Crack-WiFi";
const char *AP_PASS = "12345678"; // Min. 8 znaků pro WPA2
const int AP_CHANNEL = 6;         // Standardní nepřekrývající se kanál 2.4 GHz
const int AP_MAX_CLIENTS = 4;

WebServer server(80);

unsigned long requestCount = 0;

// HTML šablona s responzivním designem a automatickým generováním provozu
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="cs">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Demo | 4IZ112</title>
    <style>
        :root {
            --bg: #0f172a;
            --card: #1e293b;
            --primary: #38bdf8;
            --text: #f8fafc;
            --muted: #94a3b8;
            --success: #22c55e;
            --border: #334155;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: system-ui, -apple-system, sans-serif;
            background-color: var(--bg);
            color: var(--text);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 1rem;
        }
        .container {
            width: 100%;
            max-width: 480px;
            background: var(--card);
            border: 1px solid var(--border);
            border-radius: 1rem;
            padding: 1.5rem;
            box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.5);
        }
        .badge {
            display: inline-flex;
            align-items: center;
            gap: 0.5rem;
            padding: 0.25rem 0.75rem;
            border-radius: 9999px;
            font-size: 0.8rem;
            font-weight: 600;
            background: rgba(34, 197, 94, 0.15);
            color: var(--success);
            margin-bottom: 1rem;
        }
        .pulse {
            width: 8px;
            height: 8px;
            background: var(--success);
            border-radius: 50%;
            animation: pulse-dot 1.5s infinite;
        }
        @keyframes pulse-dot {
            0%, 100% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.4; transform: scale(1.3); }
        }
        h1 {
            font-size: 1.4rem;
            margin-bottom: 0.5rem;
            color: var(--text);
        }
        p.subtitle {
            font-size: 0.9rem;
            color: var(--muted);
            margin-bottom: 1.5rem;
        }
        .info-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 0.75rem;
            margin-bottom: 1.5rem;
        }
        .info-box {
            background: rgba(15, 23, 42, 0.6);
            border: 1px solid var(--border);
            border-radius: 0.5rem;
            padding: 0.75rem;
        }
        .info-label {
            font-size: 0.75rem;
            color: var(--muted);
            text-transform: uppercase;
            letter-spacing: 0.05em;
            margin-bottom: 0.25rem;
        }
        .info-value {
            font-size: 1.1rem;
            font-weight: 700;
            color: var(--primary);
            font-family: monospace;
        }
        .traffic-panel {
            background: rgba(15, 23, 42, 0.8);
            border-left: 3px solid var(--primary);
            padding: 1rem;
            border-radius: 0 0.5rem 0.5rem 0;
            margin-bottom: 1rem;
        }
        .traffic-title {
            font-size: 0.85rem;
            font-weight: 600;
            color: var(--text);
            margin-bottom: 0.25rem;
        }
        .traffic-desc {
            font-size: 0.8rem;
            color: var(--muted);
            margin-bottom: 0.75rem;
        }
        .ping-counter {
            font-family: monospace;
            font-size: 0.95rem;
            color: var(--primary);
        }
        footer {
            margin-top: 1.5rem;
            text-align: center;
            font-size: 0.75rem;
            color: var(--muted);
            border-top: 1px solid var(--border);
            padding-top: 1rem;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="badge">
            <span class="pulse"></span>
            Připojeno k ESP32 AP
        </div>
        <h1>4IZ112: Wi-Fi Crack Demo</h1>
        <p class="subtitle">Demonstrace odposlechu a prolamování WPA2-PSK</p>

        <div class="info-grid">
            <div class="info-box">
                <div class="info-label">SSID sítě</div>
                <div class="info-value">Demo-Crack-WiFi</div>
            </div>
            <div class="info-box">
                <div class="info-label">Zabezpečení</div>
                <div class="info-value">WPA2-PSK</div>
            </div>
            <div class="info-box">
                <div class="info-label">ESP32 IP</div>
                <div class="info-value" id="val-ip">192.168.4.1</div>
            </div>
            <div class="info-box">
                <div class="info-label">Doba běhu</div>
                <div class="info-value" id="val-uptime">0 s</div>
            </div>
        </div>

        <div class="traffic-panel">
            <div class="traffic-title">Generátor síťového provozu</div>
            <div class="traffic-desc">Telefon automaticky odesílá periodické HTTP požadavky pro udržení aktivního provozu na Wi-Fi.</div>
            <div class="ping-counter">
                Odesláno požadavků: <span id="req-count">1</span> (<span id="status-text">aktivní</span>)
            </div>
        </div>
    </div>

    <script>
        let counter = 1;
        const countEl = document.getElementById('req-count');
        const statusEl = document.getElementById('status-text');
        const uptimeEl = document.getElementById('val-uptime');

        // Pravidelné generování provozu každé 2.5 sekundy
        setInterval(() => {
            counter++;
            countEl.textContent = counter;
            
            fetch('/api/traffic')
                .then(r => r.json())
                .then(data => {
                    statusEl.textContent = 'přijato (' + data.clients + ' klientů)';
                    uptimeEl.textContent = data.uptime + ' s';
                })
                .catch(err => {
                    statusEl.textContent = 'chyba spojení';
                });
        }, 2500);
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    requestCount++;
    Serial.printf("[HTTP] Požadavek na / od klienta (celkem: %lu)\n", requestCount);
    server.send(200, "text/html", INDEX_HTML);
}

void handleTraffic() {
    requestCount++;
    unsigned long uptimeSeconds = millis() / 1000;
    int clientCount = WiFi.softAPgetStationNum();

    String json = "{";
    json += "\"status\":\"ok\",";
    json += "\"requests\":" + String(requestCount) + ",";
    json += "\"uptime\":" + String(uptimeSeconds) + ",";
    json += "\"clients\":" + String(clientCount);
    json += "}";

    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n==========================================");
    Serial.println("  ESP32-C3: Wi-Fi WPA2-PSK Crack Demo");
    Serial.println("  Předmět: 4IZ112 - FIS VŠE");
    Serial.println("==========================================");

    // Konfigurace Access Pointu s WPA2-PSK
    Serial.printf("[WiFi] Spouštím AP: %s (heslo: %s)\n", AP_SSID, AP_PASS);
    bool apStarted = WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, 0, AP_MAX_CLIENTS);

    if (apStarted) {
        IPAddress IP = WiFi.softAPIP();
        Serial.print("[WiFi] AP úspěšně spuštěn. IP adresa: ");
        Serial.println(IP);
        Serial.println("[WiFi] Připojte telefon a otevřete v prohlížeči: http://" + IP.toString());
    } else {
        Serial.println("[WiFi] CHYBA: Spuštění AP selhalo!");
    }

    // Registrace HTTP cest
    server.on("/", handleRoot);
    server.on("/api/traffic", handleTraffic);
    server.begin();
    Serial.println("[HTTP] Webový server naslouchá na portu 80");
    Serial.println("==========================================\n");
}

void loop() {
    server.handleClient();
}
