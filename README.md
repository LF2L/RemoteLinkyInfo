# RemoteLinkyInfo

## Description
Ecran déporté pour visualser les informations générées par le compteur Linky (trame générée par le compteur conecté Linky).
Alarme en cas de consommation critique par rapport au contrat souscrit.
![remote Linky info exterieur](/images/DSC09149.JPG)
![remote Linky info interieur](/images/DSC09140.JPG)

## Fonctions
* Afficher les informations sur un écran LCD
* Bouton pour afficher les informations non affichées
* Alarme en cas de consommation critique par rapport au contrat souscrit.
* Bouton pour déclencher le délestage d’appareils consommateurs pas utile

## Composants
* Node MCU ESP8266
* écran LCD 16x2
* module I2C pour l'écran
* 2 boutons poussoirs
* 1 buzzer 
* 2 résistances 

## Schéma d'assemblage électronique
![schéma assemblage](/images/RemoteLCD.png)

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

### Fonctionnement
* Afficher les informations sur un écran LCD
L’écran déporté réalise une requête sur un service REST du serveur qui stock les informations du compteur pour recevoir les dernières information du compteur et les stocker localement jusqu’à la prochaine requête. Des informations peuvent être calculé par l’objet ou par le serveur qui envoie les informations (dépend de l’architecture de la bdd) . 
* Bouton pour déclencher le délestage d’appareils consommateurs pas utile
Réalise une requête sur le service REST fourni par le serveur local pour que celui ci envoie une requête aux objets connectés qui permette de contrôler la consommation.

## To do 
* Changer le mécanisme d’interaction avec le serveur : Envoie des informations de l’objet pour recevoir les informations du compteur
* Généraliser l’affichage des informations reçues. Pour le moment, les données attendues sont codées « en dur ».
