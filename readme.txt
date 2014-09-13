- treba vhodnym sposobom doplnit nacitanie cascades do triedy blinker.cpp
- viac o slotoch a signaloch je v komentoch v triedach


main.cpp
	- registruje *Mat ako novy typ objektu pre posielanie frejmov
	- spusta vlakno blinkera, ktory sleduje subject a posiela zmurky a frejmy
	- v hlavnom vlakne bezi GUI
blinker.cpp
	- riesi zmurknutia, posiela frejm
mainwindow.cpp
	- gui, ktore prepaja buttony s logikou
