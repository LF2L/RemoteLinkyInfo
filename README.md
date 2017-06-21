# RemoteLinkyInfo
Ecran déporté pour visualser les informations générées par le compteur Linky (trame générée par le compteur conecté Linky).
Alarme en cas de consommation critique par rapport au contrat souscrit.


## Composants
* Node MCU ESP8266
* écran LCD 16x2
* module I2C pour l'écran
* 2 boutons poussoirs
* 1 buzzer 
* 2 résistances 

## Boitiers 
* MDF 3mm

## Echanges d'informations
L'appareil va se connecter à un serveur qui centralise les informations concernant les objets connectés et le compteur Linky. Ce serveur stocke les informations concernants les objets connecté aisni que les informations produites par le compteur

### Informations récupérées 
L'écran déporter va interroger le serveur pour recuperer les informations suivantes au format JSON:
* IINST
* ISOUSC
* IMAX
* PTEC
* seuil

Pour plus de detail concernant les informations récupérées, reportez vous à la documentation technique ERDF-NOI-CPT_54E.
Le seuil est un variable modifiable sur le serveur qui permet d'ajuster le déclenchement de l'alarme et du délestage eventuel.

### Informations envoyées 
L'écran déporté envoie également des informations sur son état au serveur:
* IP
* id
* type d'objet connecté
