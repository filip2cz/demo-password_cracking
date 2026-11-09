# DEMO 2: Prolamování Wi-Fi (WPA2-PSK 4-way handshake)

Předmět: [4IZ112 - Informatika](https://insis.vse.cz/katalog/syllabus.pl?predmet=221715;) na [FIS VŠE](https://fis.vse.cz/)

Toto demo demonstruje princip autentizace v bezdrátových sítích **WPA2-Personal (WPA2-PSK)**, zachycení tzv. **4cestného handshake (4-way handshake)** a jeho následné prolomení pomocí offline slovníkového útoku.

---

## Proč WPA2-PSK a ne zastaralý WEP?

1. **Kompatibilita hardware a OS**: Moderní mikrokontroléry (ESP32-C3) ani moderní telefony (Android / iOS) již WEP v režimu AP nepodporují z důvodu jeho kritické nebezpečnosti.
2. **Kryptografický rozdíl v prolomení**:
   * **WEP** má vadný návrh inicializačních vektorů (IV) v šifře RC4. Neláme se přes slovník, ale pomocí statistické kryptoanalýzy (FMS/PTW útok), k čemuž je nutné nasbírat desetitisíce až statisíce datových paketů.
   * **WPA2-PSK** je zranitelné právě vůči **slovníkovému útoku (Dictionary Attack)**. K prolomení stačí zachytit pouhých několik rámců při připojení zařízení (handshake). Samotný útok pak probíhá zcela offline na počítači útočníka.

---

## Teoretický princip útoku

Při připojování klienta (např. mobilního telefonu) k přístupovému bodu (ESP32) probíhá tzv. **4-way handshake** (rámce protokolu EAPOL-Key):

```
+---------------+                              +---------------+
| Klient (STA)  |                              |    AP (ESP)   |
+---------------+                              +---------------+
        |                                              |
        |<--------- 1. Zpráva: ANonce (od AP) ---------|
        |                                              |
        |-- 2. Zpráva: SNonce + MIC (od klienta) ----->|
        |                                              |
        |<--------- 3. Zpráva: GTK + MIC --------------|
        |                                              |
        |-- 4. Zpráva: Potvrzení (ACK) --------------->|
        v                                              v
```

1. Obě strany znají SSID sítě a heslo (Pre-Shared Key). Z nich spočítají klíč `PMK`:
   $$\text{PMK} = \text{PBKDF2}(\text{HMAC-SHA1}, \text{heslo}, \text{SSID}, 4096, 256)$$
2. Výměnou náhodných čísel `ANonce` a `SNonce` a znalostí MAC adres obě strany odvodí dočasný šifrovací klíč `PTK` (Pairwise Transient Key).
3. Částí tohoto klíče (`KCK`) se spočítá kontrolní kód zprávy `MIC` (Message Integrity Code).
4. **Zranitelnost**: Pokud útočník tyto rámce odposlechne ze vzduchu, může vzít slovník hesel (`wordlist.txt`), pro každé slovo spočítat zkušební PMK, odvodit PTK a ověřit, zda vypočítaný MIC odpovídá hodnotě zachycené v paketu. Pokud ano, nalezl platné heslo.

Protože heslo `12345678` je běžné a nachází se v souboru `wordlist.txt`, útok proběhne úspěšně během zlomku sekundy.

---

## Potřebný software a prostředí

Ve složce je připraven soubor `shell.nix`, který ve správci balíčků [Nix](https://nixos.org/) či v [NixOS](https://nixos.org/) nainstaluje vše potřebné:
- [hashcat](https://hashcat.net/hashcat/) – moderní nástroj pro rychlé prolomení hesel pomocí GPU
- [hcxtools](https://github.com/ZerBea/hcxtools) – nástroje pro převod `.pcap` do hashcat formátu `22000`
- [aircrack-ng](https://www.aircrack-ng.org/) – klasická sada pro audit Wi-Fi sítí

Aktivace prostředí:
```bash
nix-shell
```

---

## Soubory v repozitáři

* `esp32_ap/esp32_ap.ino`: Zdrojový kód pro ESP32-C3. Vytvoří AP `Demo-Crack-WiFi` s heslem `12345678` a webovým serverem na `http://192.168.4.1`.
* `wordlist.txt`: Vzorový slovník hesel obsahující i heslo `12345678`.
* `vzorovy_handshake.pcap`: Předem zachycený vzorový handshake pro síť `Demo-Crack-WiFi`. Slouží jako spolehlivá záloha pro přednášku, pokud by selhal živý odposlech nebo hardware nepodporoval monitor mód.
* `shell.nix`: Deklarativní konfigurace prostředí pro Nix/NixOS.

---

## Průběh demonstrace

### Varianta A: Rychlá demonstrace se vzorovým souborem (pojistka)

Pokud na přednášce nemáte Wi-Fi adaptér s podporou režimu monitoru, nebo chcete demonstraci urychlit:

#### 1. Prolomení pomocí Aircrack-ng:
```bash
aircrack-ng -w wordlist.txt vzorovy_handshake.pcap
```

#### 2. Prolomení pomocí Hashcat (GPU):
```bash
# 1. Převod pcap na formát hashcatu 22000
hcxpcapngtool -o handshake.hc22000 vzorovy_handshake.pcap

# 2. Spuštění slovníkového útoku
hashcat -m 22000 handshake.hc22000 wordlist.txt
```

---

### Varianta B: Živá demonstrace s ESP32-C3 a telefonem

#### 1. Příprava ESP32-C3
1. Nahrajte kód ze složky `esp32_ap/` do desky ESP32-C3 (pomocí Arduino IDE nebo PlatformIO).
2. Po zapnutí ESP32 začne vysílat Wi-Fi síť:
   * **SSID**: `Demo-Crack-WiFi`
   * **Heslo**: `12345678`
   * **Kanál**: 6

#### 2. Živý odposlech z notebooku
1. Přepněte bezdrátovou kartu do režimu monitoru:
   ```bash
   sudo airmon-ng start <wifi_rozhrani>
   ```
2. Spusťte zachytávání paketů na kanálu 6 a filtrujte na SSID sítě:
   ```bash
   sudo airodump-ng -c 6 --essid "Demo-Crack-WiFi" -w zive_demo <monitor_rozhrani>
   ```

#### 3. Připojení telefonu
1. Na mobilním telefonu otevřete nastavení Wi-Fi a připojte se k síti `Demo-Crack-WiFi`.
2. V `airodump-ng` se v pravém horním rohu objeví hlášení: `WPA handshake: <MAC_ESP32>`.
3. V prohlížeči telefonu otevřete adresu `http://192.168.4.1`:
   * Načte se demo stránka, která každé 2.5 sekundy posílá požadavek na `/api/traffic`, což demonstruje živý a stabilní síťový provoz.

#### 4. Prolomení živě zachyceného hesla
Ukončete zachytávání (`Ctrl+C`) a spusťte prolomení:
```bash
aircrack-ng -w wordlist.txt zive_demo-01.cap
```
Nebo přes Hashcat:
```bash
hcxpcapngtool -o zive_demo.hc22000 zive_demo-01.cap
hashcat -m 22000 zive_demo.hc22000 wordlist.txt
```

---

## Poznámka k minimální délce hesel (WPA2)

Standard IEEE 802.11i (WPA2) striktně vyžaduje hesla o délce **8 až 63 znaků**.
* Nástroje `hashcat` i `aircrack-ng` proto automaticky filtrují a přeskakují slova ze slovníku kratší než 8 znaků (např. `heslo`, `admin`, `123456`).
* V našem `wordlist.txt` je 14 z 20 hesel kratších než 8 znaků – Hashcat je označí jako `Rejected: 14/20 (70.00%)` a testuje pouze zbývající validní kandidáty.
* Heslo `12345678` má přesně 8 znaků a je okamžitě nalezeno.
