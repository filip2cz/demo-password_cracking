{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  # Dependencies needed for the demonstration
  buildInputs = with pkgs; [
    hashcat
    john
  ];

  # Command executed upon entering nix-shell
  shellHook = ''
    # Remove existing potfiles to reset cracked hashes history
    rm -f ~/.local/share/hashcat/hashcat.potfile ~/.hashcat/hashcat.potfile ~/.john/john.pot 2>/dev/null

    echo "=========================================================="
    echo " Demo prostředí pro prolamování hesel"
    echo "=========================================================="
    echo ""
    echo "Užitečné příkazy:"
    echo ""
    echo "   Spuštění Hashcatu pro MD5 (ignoruje uživatelská jména):"
    echo "   hashcat -m 0 --username hashes.txt wordlist.txt"
    echo ""
    echo "   Spuštění John the Ripper pro MD5:"
    echo "   john --format=Raw-MD5 --wordlist=wordlist.txt hashes.txt"
    echo "=========================================================="
  '';
}