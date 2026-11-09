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
    echo " 1) Zjištění názvu WiFi interface"
    echo "    ip add"
    echo ""
    echo " 2) Vypnutí procesů, které překáží airmon-ng"
    echo "    sudo airmon-ng check kill"
    echo ""
    echo " 3) Spuštění monitorovacího režimu na WiFi modulu (např. wlo1)"
    echo "    sudo airmon-ng start wlo1"
    echo ""
    echo " 4) Zachycení 4-way handshake"
    echo "    sudo airodump-ng -c 6 --essid "Demo-Crack-WiFi" -w zive_demo wlo1mon"
    echo ""
    echo " 5) Prolomení"
    echo "    aircrack-ng -w wordlist.txt zive_demo-01.cap"
    echo ""
    echo " PO PREZENTACI: uvedení do normálního stavu"
    echo "    sudo aircrack-ng stop wlo1mon"
    echo "    sudo systemctl restart NetworkManager"
    echo "=========================================================="
  '';
}
