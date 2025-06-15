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

Phase d'établissement de connexion : L'envoi du SYN fonctionne mais pas l'envoi du SYNACK et du ACK. Si le SYNACK n'est pas réçu au début, la réémission de fonctionne pas.

Phase de transfert de données : fonctionnelle

Négociation du % de pertes admissibles durant l'établissement de connexion : non implémenté

### v4.2

Gestion de l'asynchronisme : non implémenté













