# DEMO 1: crackování hesel z uniklé databáze

Potřebný software:
- [hashcat](https://hashcat.net/hashcat/)
- [John the Ripper](https://github.com/openwall/john/)

Pro zjednodušení byl přidán soubor `shell.nix`, jenž je možné využít pro automatickou instalaci všeho potřebného ve správci balíčků [Nix](https://nixos.org/), nebo v operačním systému [NixOS](https://nixos.org/).

## Použití

Crackování hesel pomocí hashcat s využitím wordlistu:
```bash
hashcat -m 0 --username hashes.txt wordlist.txt
```

Crackování hesel pomocí John the Ripper s využitím wordlistu:
```bash
john --format=Raw-MD5 --wordlist=wordlist.txt hashes.txt
```

## Soubory v repozitáři

`hashes.txt`: soubor reprezentující uniklou databázi hesel ve formátu:

```
uzivatelske_jmeno:MD5_hash_hesla
```

`wordlist.txt`: vzorový wordlist. Pro naše účely jsou v něm i hesla z hashes.txt, konkrétně uživatelů `radomir` a `admin`

## potfile
hashcat a John the Ripper si pamatují již vyluštěná hesla ve svém potfile souboru. Tyto hashe pak přeskakují, proto je třeba před každou demonstrací tenty potfile odstranit, na Linuxu např. příkazem:

```bash
rm -f ~/.local/share/hashcat/hashcat.potfile ~/.hashcat/hashcat.potfile ~/.john/john.pot
```

Při použití `shell.nix` se toto mazání provádi automaticky při jeho načtení.