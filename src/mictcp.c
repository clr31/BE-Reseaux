#include <mictcp.h>
#include <api/mictcp_core.h>
#include <time.h>
#include <netdb.h>
#define MAX_SIZE 5
#define TIMEOUT 100
#define SIZE_WINDOW 30

/* modifier pour l'envoie du synack*/

mic_tcp_sock sockets[MAX_SIZE] ;
int seq ;
int expected_seq ;
int window[SIZE_WINDOW] ;
int * last ;
float loss_accept = 0.2 ;

void init_window() { 
    for(int i=0; i<SIZE_WINDOW; i++) {
        if((i%7)==1) {
            window[i] = 1 ;
        }
        else {
            window[i] = 0 ;
        }
    }
}

int sum(int tab[]) {
    int sum = 0 ;
    for(int i=0; i<SIZE_WINDOW; i++) {
        sum += tab[i] ;
    }
    return sum ;
}

void update_window(int perte) { // 1 pour une perte et 0 pour un succès
    if(last==&window[SIZE_WINDOW-1]) {
        last = window ;
    }
    else {
        last++ ;
    }
    *last = perte ;
}

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
   set_loss_rate(20);
   init_window() ; 
   last = window ;
   
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
    sockets[socket].state = WAIT_SYN ;
    clock_t deb = clock() ;
    clock_t fin ;
    while(!(sockets[socket].state == ESTABLISHED)) {
        //mesure timeout
        /*fin = clock() ;
        diff = (deb - fin)/1000 ;
        if(sockets[socket].state==WAIT_ACK && diff > TIMEOUT) {
            mic_tcp_pdu synack ;
            synack.header.source_port = sockets[0].local_addr.port ;
            synack.header.dest_port = sockets[0].remote_addr.port ;
            synack.header.ack = 1 ;
            synack.header.syn = 1 ;
            synack.header.seq_num = 0 ;
            synack.header.ack_num = pdu.header.seq_num + 1 ;
            synack.payload.size = 0 ;
            synack.header.ack_num = expected_seq ;
            printf("envoi synack\n") ;
            IP_send(synack, remote_addr) ;
        }*/
        //if state = wait_ack
        //si timeout -> IP_send (synack)
    }

    /*while(!(sockets[socket].state==SYN_RECEIVED)) {
        printf("début while syn\n ") ;
        if((ret = IP_recv(psyn,&(sockets[socket].local_addr.ip_addr),&(addr->ip_addr),TIMEOUT*10))>=0) {
            printf("dans if ip recv\n") ;
            printf("retour IP recv : %d\n", ret) ;
            if(psyn->header.syn) {
                printf("Reçu SYN de port %d\n", psyn->header.source_port);
                sockets[socket].state = SYN_RECEIVED ;
            }
            else{
                printf("pas un syn\n") ;
            } 
            printf("apres test syn.header.syn\n") ;
        }
        else{
            printf("retour IP recv : %d\n", ret) ;
            printf("rien reçu\n") ;
        } 
    }*/
    
    //printf("fin while syn\n") ;

    //addr->port = psyn->header.source_port ; //??

    /*mic_tcp_pdu synack ;
    synack.header.source_port = sockets[socket].local_addr.port ;
    synack.header.dest_port = addr->port ;
    synack.header.syn = 1 ;
    synack.header.ack = 1 ;
    synack.payload.size = 0 ;*/
    /*mic_tcp_pdu ack ;
    ack.payload.size = 0 ;
    sockets[socket].state = WAIT_ACK ;*/

    /*while(!(sockets[socket].state==ESTABLISHED)) {
        printf("début while ack\n ") ;
        //IP_send(synack,addr->ip_addr) ;
        if(IP_recv(&ack,&(sockets[socket].local_addr.ip_addr),&(addr->ip_addr),TIMEOUT*10)>=0) {
            if(ack.header.ack) {
                printf("Reçu ACK de port %d\n", ack.header.source_port);
                seq = ack.header.ack ;
                sockets[socket].state = ESTABLISHED ;
            }
        }
    }
    printf("fin while ack\n ") ;*/

    //sockets[socket].remote_addr = *addr ;
    expected_seq = 20 ;

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
    //sockets[socket].local_addr.port = 3027 ;
    //sockets[socket].local_addr.ip_addr.addr = "127.0.0.1" ;
    //sockets[socket].local_addr.ip_addr.addr_size = 10 ;
    mic_tcp_sock_addr local_addr ;
    struct hostent * hp ;
    if((hp = gethostbyname("localhost")) == NULL) {
        printf("erreur gethostbyname\n") ;
        exit(1) ;
    }
    //local_addr.ip_addr.addr = "127.0.0.1" ;
    local_addr.ip_addr.addr = hp->h_addr ;
    //local_addr.ip_addr.addr_size = 10 ;
    local_addr.ip_addr.addr_size = hp->h_length ;
    local_addr.port = 3027 ;
    mic_tcp_bind(socket,local_addr) ;

    mic_tcp_pdu syn ;
    printf("port source : %d\n", sockets[socket].local_addr.port) ;
    syn.header.source_port = sockets[socket].local_addr.port ;
    syn.header.dest_port = addr.port ;
    syn.header.seq_num = seq ;
    syn.header.syn = 1 ;
    syn.header.ack = 0 ;
    syn.header.fin = 0 ;
    syn.payload.size = 0 ;
    mic_tcp_pdu synack ;
    int ret ;
    
    sockets[socket].state = WAIT_SYNACK ;
    while(!(sockets[socket].state==SYNACK_RECEIVED)) {
        printf("début while synack\n ") ;
        IP_send(syn,addr.ip_addr) ;
        printf("après IP send\n") ;
        if((ret = IP_recv(&synack,&(sockets[socket].local_addr.ip_addr),&(addr.ip_addr),TIMEOUT*10))>=0) {
            if(synack.header.syn && synack.header.ack) {
                printf("Reçu SYNACK de port %d\n", synack.header.source_port);
                seq = synack.header.ack_num ;
                sockets[socket].state = SYNACK_RECEIVED ;
            }
        }
        printf("retour ip recv : %d\n", ret) ;
    }

    printf("fin while synack\n ") ;

    mic_tcp_pdu ack ;
    ack.header.source_port = sockets[socket].local_addr.port ;
    ack.header.dest_port = addr.port ;
    ack.header.seq_num = seq ;
    ack.header.ack = 1 ;
    ack.header.syn = 0 ;
    ack.header.fin = 0 ;
    ack.payload.size = 0 ;
    IP_send(ack,addr.ip_addr) ;
    
    /*sockets[socket].state = WAIT_SYNACK ;
    while(!(sockets[socket].state== SYNACK_RECEIVED)) {
        printf("début while synack\n ") ;
        IP_send(syn,addr.ip_addr) ;
        printf("après IP send\n") ;
        if(IP_recv(&synack,&(sockets[socket].local_addr.ip_addr),&(addr.ip_addr),TIMEOUT*10)>=0) {
            if(synack.header.syn && synack.header.ack) {
                printf("Reçu SYNACK de port %d\n", synack.header.source_port);
                seq = synack.header.ack_num ;
                sockets[socket].state = SYNACK_RECEIVED ;
            }
        }
    }*/

    
    
    sockets[socket].state = ESTABLISHED ;
    seq = 20 ;

    return ret ;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send(int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_pdu pdu ;
    pdu.header.source_port = sockets[mic_sock].local_addr.port;
    pdu.header.dest_port = sockets[mic_sock].remote_addr.port ;
    pdu.header.seq_num = seq ;
    printf("seq : %d\n", seq) ;
    pdu.payload.data = mesg;
    pdu.payload.size = mesg_size;
    pdu.header.syn = 0 ;
    pdu.header.ack = 0 ;
    pdu.header.fin = 0 ;
    mic_tcp_pdu * pack = malloc(sizeof(mic_tcp_pdu)) ;
    pack->payload.size = 0 ;
    int ack_received = 0 ;
    int effective_send = -1 ;
    int ret = -2 ;
    int loss_ok = 0 ;
    sockets[mic_sock].local_addr.ip_addr.addr_size = 0;
    sockets[mic_sock].remote_addr.ip_addr.addr_size = 0;


    while(!ack_received && !loss_ok) {
        effective_send = IP_send(pdu, sockets[mic_sock].remote_addr.ip_addr);
        if((ret = IP_recv(pack, &(sockets[mic_sock].local_addr.ip_addr), &(sockets[mic_sock].remote_addr.ip_addr), TIMEOUT))>=0) {
            printf("reception : ack ? %d, ack num : %d, seq : %d\n", pack->header.ack, pack->header.ack_num, seq) ;
            if(pack->header.ack==1 && pack->header.ack_num==(seq+1)) { 
                ack_received = 1 ;
                update_window(0) ; //mise à jour fenêtre
                printf("ack num : %d\n",pack->header.ack_num) ;
                seq = pack->header.ack_num ;
                printf("taux perte, %f\n", ((float) sum(window))/30) ;
            }
            else {
                update_window(1) ; //mise à jour fenêtre
                seq = pack->header.ack ;
                printf("taux perte, %f\n", ((float) sum(window))/30) ;
                if(((float) sum(window))/30 <= (loss_accept + 0.001)) { 
                    loss_ok = 1 ;
                    printf("perte ok \n") ;
                }
                else {
                    printf("reemission\n") ;
                }
            }
        }
        else {
            update_window(1) ; //mise à jour fenêtre
            printf("taux perte, %f\n", ((float) sum(window))/30) ;
            if(((float) sum(window))/30 <= (loss_accept + 0.001)) { //test perte acceptable
                loss_ok = 1 ;
                printf("perte ok\n") ;
            } //si oui fin boucle
            //sinon continue
            else {
                printf("reemission\n") ;
            }
        }
        printf("valeur retour ip recv : %d\n", ret) ;
    }
    printf("sortie while\n") ;
    
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
    payload.data = mesg ;
    payload.size = max_mesg_size ;
    int lu = app_buffer_get(payload) ;

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
    printf("dans process received pdu\n") ;

    if(pdu.header.syn && pdu.header.ack && sockets[0].state == ESTABLISHED) {
        printf("dans réémission ack\n") ;
        app_buffer_put(pdu.payload) ;
        mic_tcp_pdu ack ;
        ack.header.source_port = sockets[0].local_addr.port ;
        ack.header.dest_port = sockets[0].remote_addr.port ;
        ack.header.ack = 1 ;
        ack.header.syn = 0 ;
        ack.header.fin = 0 ;
        ack.header.seq_num = seq ;
        ack.header.ack_num = pdu.header.seq_num ;
        ack.payload.size = 0 ;
        ack.header.ack_num = expected_seq ;
        IP_send(ack, remote_addr) ;
    }

    if(pdu.header.syn && sockets[0].state == WAIT_ACK) {
        printf("syn reçu\n") ;
        app_buffer_put(pdu.payload) ;
        mic_tcp_pdu synack ;
        synack.header.source_port = sockets[0].local_addr.port ;
        synack.header.dest_port = sockets[0].remote_addr.port ;
        synack.header.ack = 1 ;
        synack.header.syn = 1 ;
        synack.header.fin = 0 ;
        synack.header.seq_num = 0 ;
        synack.header.ack_num = pdu.header.seq_num + 1 ;
        synack.payload.size = 0 ;
        synack.header.ack_num = expected_seq ;
        printf("envoi synack, remote addr : %s, port dest paquet: %d\n", remote_addr.addr, sockets[0].remote_addr.port) ;
        IP_send(synack, remote_addr) ;
        printf("apres ip send\n") ;
        sockets[0].state = WAIT_ACK ;
    }

    printf("seq reçu : %d, seq attendu : %d\n",pdu.header.seq_num,expected_seq) ;
    if(pdu.header.seq_num==expected_seq) {
        printf("num seq ok\n") ;

    if(pdu.header.syn && sockets[0].state == WAIT_SYN) {
        printf("syn reçu\n") ;
        app_buffer_put(pdu.payload) ;
        expected_seq++ ;

        sockets[0].remote_addr.ip_addr = remote_addr ;
        sockets[0].remote_addr.port = pdu.header.source_port ;

        mic_tcp_pdu synack ;
        synack.header.source_port = sockets[0].local_addr.port ;
        synack.header.dest_port = sockets[0].remote_addr.port ;
        synack.header.ack = 1 ;
        synack.header.syn = 1 ;
        synack.header.fin = 0 ;
        synack.header.seq_num = 0 ;
        synack.header.ack_num = pdu.header.seq_num + 1 ;
        synack.payload.size = 0 ;
        synack.header.ack_num = expected_seq ;
        printf("envoi synack, remote addr : %s, port dest paquet: %d\n", remote_addr.addr, sockets[0].remote_addr.port) ;
        IP_send(synack, remote_addr) ;
        sockets[0].state = WAIT_ACK ;
    }
    else if(pdu.header.ack && sockets[0].state == WAIT_ACK) {
        printf("ack reçu\n") ;
        app_buffer_put(pdu.payload) ;
        sockets[0].state = ESTABLISHED ;
    } 
    else if (sockets[0].state == ESTABLISHED) {
        app_buffer_put(pdu.payload) ;
        printf("expected seq : %d\n", expected_seq) ;
        expected_seq++ ;
        mic_tcp_pdu ack ;
        ack.header.source_port = sockets[0].local_addr.port ;
        ack.header.dest_port = sockets[0].remote_addr.port ;
        ack.header.ack = 1 ;
        ack.header.syn = 0 ;
        ack.header.fin = 0 ;
        ack.header.seq_num = seq ;
        ack.header.ack_num = expected_seq ;
        ack.payload.size = 0 ;
        IP_send(ack, remote_addr) ;
    }
    
    }
    else {
        mic_tcp_pdu ack ;
        ack.header.source_port = sockets[0].local_addr.port ;
        ack.header.dest_port = sockets[0].remote_addr.port ;
        ack.header.ack = 1 ;
        ack.header.syn = 0 ;
        ack.header.fin = 0 ;
        ack.header.seq_num = seq ;
        ack.header.ack_num = expected_seq ;
        ack.payload.size = 0 ;
        IP_send(ack, remote_addr) ;
        seq++ ;
    }

}
