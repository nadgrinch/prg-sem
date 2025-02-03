README pro Semestrální práci v předmětu B3B36PRG
autor: Šimon Dratva (dratvsim)
verze: 1.0

Implementace
 * hlavní aplikace
 * grafické rozhraní

 * neimplementoval jsem vlastní module, používám ten referenční od pana profesora Faigla
 * přiložen jako prgsem-comp_module

Ovládání:
 * před spuštěním je potřeba vytvořit pipy, přiložen script, který je vytvoří v /tmp adresáři
 * spustit první hlavní aplikaci pro zachycení startup zprávy od modulu
 * spustit modul

 * pomocí klávesnice jak v terminálu, tak v grafickém okně ("nakliknuté")
  - při interaktivní  změně velikosti, rozsahu nebo parametru c, je interakce uskutečněna skrz vyskacovací okno
  - i když byla vyvolána z terminálu
  - některé akce jako třeba zoom pomocí kliknutí myši nejdou provést z terminálu, není informace o pozici myši
 * pomocí klávesy 'h' lze zobrazit celý seznam keybindů a krátký popis co jednotlivé klávesy dělají
  - v grafickém okně je nápověda psána kapitálkami pro lepší čitelnost

 * jednotlivé keybindy
  - q - ukončí aplikaci
  - s - nastaví parametry výpočtu fraktálu do modulu
  - c - vypočítá fraktál v hlavní aplikaci, vykreslení v celku
  - 1 - započne výpočet pomocí modulu, postupné vykreslování
  - a - abort, zastaví výpočet pomocí modulu
  - r - resetuje chunk id, cid na 0
  - l - smaže výpočetní buffer
  - p - zobrazí výpočetní buffer
  - x - zobrazí okno pro změnu parametrů výpočtu
  - m - zobrazí okno pro zapnutí animace +/- parametru c
  - z - přepínač povolení a nepovolení zoomu
  - levé tlačítko myši - zoom in v místě kliknutí (2x přiblížení)
  - pravé tlačítko myší - zoom out v místě klinutí (2x oddálení)
  - j - zobra okno pro uložení okna ve formátu .png nebo .jpg, uživatel volí

* vyskakovací okna v gui
 - okna uvítání a nápovědy lze zavřít pomocí kliknutí na 'OK' nebo zavřením okna (x v horní pravém rohu)
 - ostatní okna jsou zavřít pouze pomocí x , tím se daná akce i odvolá, neprovede se

Body:
 * základní implementace, dle zadání 
  - 10 bodů
 * ovládání pomocí grafického rozhraní SDL
  - 10 bodů
 * interaktivní volba parametrů c, rozsahu komplexní roviny a n
  - 2 body
 * animace fraktálu pomocí změny parametru c
  - 2 body
 * interaktivní volba velikosti obrázku
  - 2 body
 * interaktivní implementace zoomu
  - 2 body
 * uložení okna ve formátu .png nebo .jpg
  - 2 body
součet: 30 bodů
