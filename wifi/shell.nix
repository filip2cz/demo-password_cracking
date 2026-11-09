{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  # Nástroje potřebné pro demonstraci odposlechu a prolamování WPA2-PSK
  buildInputs = with pkgs; [
    hashcat
    hcxtools
    aircrack-ng
    wireshark-cli
    iw
  ];

  # Příkaz provedený při spuštění nix-shell
  shellHook = ''
    # Vymazání potfile pro reset historie prolomených hesel
    rm -f ~/.local/share/hashcat/hashcat.potfile ~/.hashcat/hashcat.potfile 2>/dev/null

    echo "=========================================================="
    echo " Demo 2: Prolamování Wi-Fi (WPA2-PSK 4-way handshake)"
    echo " Předmět: 4IZ112 - FIS VŠE"
    echo "=========================================================="
    echo ""
    echo "Užitečné příkazy pro demonstraci:"
    echo ""
    echo " 1) Zjištění názvu WiFi modulu
    echo "    ip add"
    echo ""
    echo " 2) Spuštění monitorovacího režimu na WiFi modulu (např. wlp8s0)"
    echo "    sudo airmon-ng start wlp8s0"
    echo ""
    echo " 3) Zachycení 4-way handshake"
    echo "    sudo airodump-ng -c 6 --essid "Demo-Crack-WiFi" -w zive_demo wlp8s0mon"
    echo ""
    echo " 4) Prolomení"
    echo "    aircrack-ng -w wordlist.txt zive_demo-01.cap"
    echo "=========================================================="
  '';
}
