# LPRS2 Emulator
Grupni projekat iz predmeta LPRS2. 

**Autori**: *Marko Đorđević, Radomir Zlatković, Aleksa Heler*

## Uvod
Ideja je bila napraviti igricu za emulator koja je slična [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D). Kako bi mogli napraviti 3D igru u 2D emulatoru, koristimo Raycasting algoritam (ne raytracing). Kako bi pojednostavili sebi problem, i kako nije neophodno imati vertikalnost u igri (spratovi, stepenice, skokovi), možemo mapu predstaviti 2D matricom celih brojeva, gde broj označava tip zida, i ako je 0 znači prazno polje po kojem se igrač može kretati.

Kako bismo dobili 3D efekat, potrebno je zidove nacrtati tako da im je visina obrnuto proporcionalna udaljenosti od kamere. To radimo tako što crtamo jednu po jednu vertikalnu liniju. Za svaku vertikalnu liniju bacimo zrak u tom smeru i gledamo kad ce udariti u prvi zid i time dobijemo udaljenost, i za to koristimo relativno jednostavan "**Digital Differential Analysis**" ([DDA](https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm))) algoritam.

DDA je relativno brz i koristi se za pretragu koje kvadrate zrak (ray) pogađa. Mi ga koristimo da nađemo koji kvadrat naše mape je zrak pogodio i zaustavimo algoritam u tom trenutku i nacrtamo vertikalnu liniju čija visina odgovara udaljenosti zida tj. dužini zraka. Ideju za ovaj algoritam nam je dao [Lode Vandevenne](https://lodev.org/), i deo implementacije je odrađen po njegovom [tutorijalu](https://lodev.org/cgtutor/raycasting.html).

TODO: dodati objašnjenje za celu igricu, ne samo raycasting.

## Timeline
- [ ] Proof of concept
  - [X] Render bez tekstura sa dve boje (1bit color indexing)
  - [X] Render bez tekstura sa vise boja (4bit color indexing)
  - [ ] Render sa teksturama (učitavanje iz fajla)
  - [ ] Učitavanje mape iz fajla (idealno slike, ali može bilo koji fajl)
- [ ] Igra
  - [ ] Do kraja implementiran renderer
    - [ ] Naše funkcije sin() i cos() umesto *math.h*
    - [ ] Fixed point aritmetika umesto floating point
  - [ ] Modularne mape i teksture
  - [ ] Napredne kontrole igrača (drugačije kretanje, konstantna brzina nezavisno od FPS, pucanje)
  - [ ] UI (životi, municija, rezultat...)
  - [ ] Items (bure sa: municijom, životima... powerup?)
  - [ ] AI (neprijatelji, kretanje neprijatelja, interakcija sa njima)


## Pokretanje

``` sudo ./waf prerequisites ``` - install waf prerequisites

``` sudo ./waf configure ``` - configure project

``` sudo ./waf build run --app=intro ``` - run the app

## Proof of concept
