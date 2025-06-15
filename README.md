# BE RESEAU
## TPs BE Reseau - 3 MIC

## Commandes de compilation

Pour compiler mictcp et générer les exécutables des applications de test depuis le code source fourni, taper :

    make

Deux applications de test sont fournies, tsock_texte et tsock_video, elles peuvent être lancées soit en mode puits, soit en mode source selon la syntaxe suivante:

    Usage: ./tsock_texte [-p|-s destination] port
    Usage: ./tsock_video [-p|-s] [-t (tcp|mictcp)] 

    Choisir un port différent de 3027.

## Implémentation de mictcp v1 

Phase de transfert de données sans garantie de fiabilité : fonctionnelle

## Implémentation de mictcp v2

Phase de transfert de données avec garantie de fiabilité totale via un mécanisme de reprise des pertes Stop and Wait : fonctionnelle

## Implémentation de mictcp v3

Phase de transfert de données avec garantie de fiabilité partielle avec % des pertes admissibles défini de façon statique : fonctionnelle

## Implémentation de mictcp v4
### v4.1

Phase d'établissement de connexion : L'envoi du SYN fonctionne (réception côté server). Le SYN-ACK est envoyé côté server mais pas forcément reçu côté client. Le problème n'intervient pas toujours lors de l'exécution. Soit la phase d'établissement de connexion s'effectue sans problèmes soit le SYN-ACK n'est jamais reçu (réémissions infinies). Si leSYN-ACK est reçu, l'envoie du ACK ne semble pas poser de problèmes.

Phase de transfert de données : fonctionnelle

Négociation du % de pertes admissibles durant l'établissement de connexion : non implémenté

### v4.2

Gestion de l'asynchronisme : non implémenté













