# PoliHack-V19-AltF4-Juniori
PoliHack AltF4

Nature Defense Network (NDN)
Sistem IoT pentru Monitorizarea si Prevenirea Dezastrelor Naturale
Nature Defense Network este o solutie de monitorizare ambientala in timp real, conceputa pentru a detecta semnele timpurii ale dezastrelor naturale (incendii de padure, inundatii, alunecari de teren) si pentru a oferi date precise despre locatia incidentului prin GPS.

Caracteristici Principale
Monitorizare Multi-Senzor: Temperatura, umiditate, gaze inflamabile/fum, nivelul apei si umiditatea solului.

Sistem de Alerta Vizuala si Sonora: Semafor de stare (Verde/Galben/Rosu) si buzzer pentru avertizare locala.

Interfata Web Live: Dashboard accesibil prin WiFi (ESP32) cu actualizare la secunda.

Localizare GPS Live: Afisarea coordonatelor exacte si integrare cu Google Maps pentru echipele de interventie.

Design Robust: Logica de functionare distribuita intre Arduino Mega (procesare senzori) si ESP32 (comunicatie web).

Hardware utilizat
Microcontrollere:

Arduino Mega 2560 (Procesare date senzori)

ESP32 (Server Web si conectivitate WiFi)

Senzori:

2x DHT11 (Temperatura si Umiditate aer)

MQ-2 / MQ-4 (Detectare Gaz/Fum)

MH-S (Nivel Apa - detectie inundatie)

Senzor Umiditate Sol (Prevenire seceta/alunecari de teren)

BH1750 (Senzor Lux - intensitate luminoasa digitala)

LDR (Rezistenta dependenta de lumina)

Neo-6M GPS (Localizare prin satelit)

Alte componente: LED-uri (Rosu, Galben, Verde), Buzzer, Rezistente.

Structura Datelor (Protocol Serial)
Datele sunt transmise de la Mega catre ESP32 sub forma de sir de caractere (CSV) etichetat:
DATA:TempMed,Gaz,Apa,Sol,LDR,T1,T2,Lux,Lat,Lng,H1,H2,HMed

Instalare si Utilizare
Conectare Hardware:

Conectati senzorii la pinii specificati in codul Arduino Mega.

Legati Serial1 (TX 18, RX 19) de pe Mega la Serial2 (RX 16, TX 17) de pe ESP32.

Asigurati un GND comun intre cele doua placi.

Configurare Software:

Incarcati codul de Arduino Mega folosind IDE-ul Arduino.

In codul pentru ESP32, introduceti datele WiFi (SSID: OCTA, Parola: 12345670).

Incarcati codul pe ESP32.

Accesare Dashboard:

Deschideti Serial Monitor pentru ESP32 pentru a afla adresa IP.

Introduceti adresa IP in orice browser de pe un dispozitiv conectat la aceeasi retea.

Logica de Alerta (Semafor)
Stare Verde: Parametri normali - Sistem stabil.

Stare Galbena: Gaz peste 500 sau Temperatura peste 40 grade Celsius - Atentie, risc crescut.

Stare Rosie: Gaz peste 800 sau Temperatura peste 55 grade Celsius - Alerta Critica (Buzzer activ).

Integrare Google Maps
Interfata web preia automat coordonatele de la modulul GPS. Utilizatorul poate accesa butonul "MAPS" care va deschide o fereastra noua cu locatia exacta a dispozitivului, facilitand coordonarea echipelor de interventie catre punctul de focar sau inundatie.

Autor
Proiect dezvoltat pentru monitorizarea sigurantei mediului inconjurator.
Nature Defense Network - Technology for a safer world
