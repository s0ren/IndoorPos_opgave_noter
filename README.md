# IndoorPos_opgave_noter

Noter og links til elementer af projektopgaven __"Positionsbestemmelse af enheder uden GPS dækning"__

Jeg laver noterne her fordi det er hurtigere.

# Formål

Hvordan kan man bestemme hvor en enhed befinder sig, uden at have adgang til GPS?
Inde i bygninger er GPS signaler ikke særligt tilgængelige, og/eller særlige nøjagtige.
Så hvad kan man så gøre?
Og kan vi få oplysninger om hvor mange personer som er tilstede i et område?

## Produktmål

I denne opgave er det ideelle mål at lave et heatmap.
Iddelt mål, skal forstås sådan, at det slet ikke er sikkert at det kan nås at lave et færdigt heatmap, men at sigter mod at indsamle data der skal bruges til at lave et.

## Procesmål

Beskriv fremgangsmåde og teknologier der kan bruges til at nå målet.

Der er flere forskellige teknologier og strategier at vælge, og I skal lave noter om hvilke i har fundet oplysniger om, og lave forsøg med at implementere en eller flere. 
Implementerirngerne kan starte som nogle forsøg på hurtigt at se om det er muligt at bruge teknologien. Hvis det ikke er, kan man gå videre til noget andet. Hvis der er gode resultater, kan det bruges til at kombinere med flere.

Beskriv hvad I finder, hvad I prøver af, og hvordan den nye viden kan bruges.

## Hovedproblematikker

Der er et antal hovedproblematikker:

- Triangulering
    - Hvordan ser matematikken ud?
- Afstand mellem målestation og mobil
    - RSSI
        - Kan findes med "sniffing" i wifi modulet promicius mode 
    - Roundtrip
- Kommunikation mellem målestationer
    - ESP MESH
    - ESP NOW

## Persondata

Når vi indsamler data om alle enheder i "luften", så er nogen af disse data noget der har med perosner at gøre. Det gør at vi skal overveje hvordan vi behandler disse oplysninger.
Projektet kommer muligvis i berøring med Persondata lovgivningen, som igen refererer til EUs GDPR.

- Hvilke disse data er personlige?
    - Er de følsomme?
    - Hele tiden, eller nogen gange.
    - Er f.eks. en AMC-adresse følsom personlig data?
- Beriges data ved at indsamle f.eks. MAC adressse, sammen med tid og sted?
    - Hvad gør det ved data-ansvaret
- Kan vi begrænse hvilke oplysninger der opsamles, i forhold til formålet?

# Forslag til teknologier at kigge på

1.  prøv først med at opsamle RSSI ved at lytte til net pakker, i promiscuis mode
2.  prøv at lave en tilnærmet afstandsberegning
3.  undersøg hvordan afstand/RSSI kan udveklses mellem ESP32-enhederne
    1.  Kan man bruge ESP-MESH mens enheden sniffer?
    2.  Kan man bruge ESP-NOW?
        -  Kan det sikres/krypteres
4.  Med afstand/signalstyrke/RSSI fra flere stationer, Hvordan kan man så beregne (men tilnærmet nøjagtighed), hvor mobilen befinder sig?

## Links

### Generelt

* <https://www.bitbrain.com/blog/indoor-positioning-system>

### Math

* <https://link.springer.com/article/10.1186/s13673-020-00236-8>


### Persondata og GDPR

* <https://pentests.dk/docs/gdpr-developers-guide/>
  * Original (på engelsk) <https://github.com/LINCnil/GDPR-Developer-Guide>

## Kode eksempler 

### WiFi sniffter

* En demo af en wifi sniffer lavet med generativ AI (Claude). [demo_kode/Proximity sniffer](./demo_kode/Proximity%20sniffer/)

* en anden wifi sniffer <https://github.com/ETS-PoliTO/esp32-sniffer>

### MQTT & MongoDB server setup

Et docker-compose projekt som kører Mosquitto MQTT og MongoDB, samt en tilhørende mqtt-subscriber, til at overføre fra MQTT til mongoDB. Se mappen [demo_kode/mqtt_mongdb_server/](/demo_kode/mqtt_mongdb_server/).