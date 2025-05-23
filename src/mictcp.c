#include <mictcp.h>
#include <api/mictcp_core.h>
#define MAX_SIZE 5
#define TIMEOUT 100

mic_tcp_sock sockets[MAX_SIZE] ;
int seq ;
int expected_seq ;

/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */
int mic_tcp_socket(start_mode sm)
{
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
   if(initialize_components(sm)==-1) {
        printf("Erreur initialisation composantsde l'API\n") ;
        return -1 ;
   } /* Appel obligatoire */
   set_loss_rate(50);
   
   //pour les versions suivantes faire une fonction qui peut créer plusieurs sockets
   //boucle while qui teste l'état ?
   sockets[0].fd = 0 ;
   sockets[0].state = IDLE ;

   //initialisation des numéros de séquence
   seq = 0 ;
   expected_seq = 0 ;

   return 0 ;
}

/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");

   sockets[socket].local_addr = addr ;
   printf("port num assigné début : %d\n", addr.port) ;
   return 0;
}

/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr* addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    sockets[socket].remote_addr = addr ;
    return 0;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send (int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_pdu pdu ;
    pdu.header.source_port = sockets[mic_sock].local_addr.port;
    pdu.header.dest_port = sockets[mic_sock].remote_addr.port ;
    pdu.header.seq_num = seq ;
    pdu.payload.data = mesg;
    pdu.payload.size = mesg_size;
    mic_tcp_pdu * pack = malloc(sizeof(mic_tcp_pdu)) ;
    pack->payload.size = 0 ;
    mic_tcp_ip_addr * paddr_recv = malloc(sizeof(mic_tcp_ip_addr)) ;
    paddr_recv->addr = malloc(sizeof(char)*1000) ;
    mic_tcp_ip_addr * paddr_local = malloc(sizeof(mic_tcp_ip_addr)) ;
    paddr_local->addr = malloc(sizeof(char)*1000) ;
    int ack_received = 0 ;
    int effective_send = -1 ;
    int ret = -2 ;

    while(!ack_received) {
        effective_send = IP_send(pdu, sockets[mic_sock].remote_addr.ip_addr);
        if((ret = IP_recv(pack, paddr_local, paddr_recv, TIMEOUT))>=0) {
            printf("reception : ack ? %d, seq ack : %d, seq : %d\n", pack->header.ack, pack->header.seq_num, seq) ;
            if(pack->header.ack==1 && pack->header.seq_num==seq) { 
                ack_received = 1 ;
            }
        }
        printf("valeur retour ip recv : %d\n", ret) ;
    }
    printf("sortie while\n") ;
    seq = (seq+1) % 2 ;
    
    return effective_send;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */
int mic_tcp_recv (int socket, char* mesg, int max_mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_payload payload ;
    //payload.data = mesg ;
    payload.size = max_mesg_size ;
    int lu = app_buffer_get(payload) ;
    memcpy(mesg,payload.data,lu) ;

    return lu;
}

/*
 * Permet de réclamer la destruction d’un socket.
 * Engendre la fermeture de la connexion suivant le modèle de TCP.
 * Retourne 0 si tout se passe bien et -1 en cas d'erreur
 */
int mic_tcp_close (int socket)
{
    printf("[MIC-TCP] Appel de la fonction :  "); printf(__FUNCTION__); printf("\n");
    sockets[socket].state = CLOSED ;
    return 0;
}

/*
 * Traitement d’un PDU MIC-TCP reçu (mise à jour des numéros de séquence
 * et d'acquittement, etc.) puis insère les données utiles du PDU dans
 * le buffer de réception du socket. Cette fonction utilise la fonction
 * app_buffer_put().
 */
void process_received_PDU(mic_tcp_pdu pdu, mic_tcp_ip_addr local_addr, mic_tcp_ip_addr remote_addr)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");

    if(pdu.header.dest_port != sockets[0].local_addr.port) { //ok pour un seul socket à 
    // partir de plusieurs faire un test qui parcours tous les éléments du tableau de 
    // sockets pour vérifier si le port de destination du pdu correspond à un unique 
    // socket
        printf("port destination du pdu : %d\n", pdu.header.dest_port) ;
        printf("port local : %d\n", sockets[0].local_addr.port) ;
        printf("Erreur je sors\n") ;
        exit(-1) ;
    }
    mic_tcp_pdu ack ;
    ack.header.source_port = sockets[0].local_addr.port ;
    ack.header.dest_port = sockets[0].remote_addr.port ;
    ack.header.ack = 1 ;
    ack.header.seq_num = pdu.header.seq_num ;
    ack.payload.size = 0 ;
    if(pdu.header.seq_num==expected_seq) {
        app_buffer_put(pdu.payload) ;
        expected_seq = (expected_seq+1) % 2 ;
    }
    ack.header.ack_num = expected_seq ;
    IP_send(ack, remote_addr) ;
}
