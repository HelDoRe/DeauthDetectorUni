   * dodac zegarek 
   * dodac date 
   * dodac config wifi przez web opcjonalnie, jakby czas pociagnac przez ntp 
   * data po wifi po uruchomieniu, potem spokoj. 
   * puscic na githuba 
   * przeportowac do platformio 
   * ToDo i ChangeLog 
   * naprawic display.getTextBounds 
   * pierwszy config: generowanie qr code do telefonu i WiFi AP i strona do ustawien (wifiManager to zalatwil) 
   * define DEBUG ON OFF a potem IFDEF, i jak lib od e-paper ma wyl/wl debuga i inne biblioteki, zeby czysty serial monitor byl
   * rozbic dane, song, includy itp. do .h
   * wyswietlanie - zrobic na rozne ekrany przez IFDEFy
   * porzadki w zmiennych, define, stringi (wyciagnac wyswietlane slowa do zmiennych i moze multilanguage pozniej sie zrobi)
   * dorzucic IFDEFy dla roznych urzadzen wyjsciowych
   * ogarnac wyglad kodu
   song grac jakos non blocking? update ePapera spowalnia granie, wiec albo to jakos inaczej, albo sprawdzic jak eInka przyspieszyc. 
   dodac obsluge przycisku (wolne GPIO D4) i ustawien w locie? 
   moze ustawienia przez WEB? Ale wtedy trza by bylo przerwac skan, i dac IP, bo polaczenie jest tylko do pierwszego razu NTP sync 
   dodac baterie? (zastanowic sie czy dac) 
   zapis danych zeby po utracie zasilania nie gubily sie 
   jakies foty i dokumentacje 
   skad inspiracja napisac dac linki (czesc jest w changelogu) 
   dioda power breathe - ze dziala, i plynne, a nie on/off 
   opisy/napisy - tez konfigurowalne, mozna dac swoje, tak jak i animacja buzki skanujacej (ograniczenie znakow zeby bylo) 
   niektore ustawienia - reset to defaults z poziomu web (zastanowic sie czy dac) 
   OTA? 
   w platformio.ini dac tez konfig pinow (i moze reszte co sie da bo rotation i invert wlazlo :) 
   