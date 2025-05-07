// Programme client pour les agences
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <id_agence>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int agency_id = atoi(argv[1]);

    // Connexion au serveur
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur\n");

    // Boucle du menu
    while (1) {
        printf("\nChoisissez une option:\n");
        printf("1. Réserver des places\n");
        printf("2. Annuler une réservation\n");
        printf("3. Demander la facture\n");
        printf("4. Consulter les vols\n");
        printf("5. Quitter\n");
        printf("Votre choix: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // Consomme le \n

        char buffer[256];
        if (choice == 1) {
            int ref, seats;
            printf("Entrez la référence du vol: ");
            scanf("%d", &ref);
            printf("Entrez le nombre de places: ");
            scanf("%d", &seats);
            getchar();
            snprintf(buffer, sizeof(buffer), "RESERVE %d %d %d", ref, agency_id, seats);
            send(sock, buffer, strlen(buffer), 0);
            char response[256];
            int bytes = recv(sock, response, sizeof(response) - 1, 0);
            if (bytes > 0) {
                response[bytes] = '\0';
                if (strcmp(response, "SUCCESS") == 0) {
                    printf("Réservation réussie\n");
                } else {
                    printf("Échec de la réservation\n");
                }
            } else {
                printf("Erreur de communication avec le serveur\n");
            }
        } else if (choice == 2) {
            int ref, seats;
            printf("Entrez la référence du vol: ");
            scanf("%d", &ref);
            printf("Entrez le nombre de places à annuler: ");
            scanf("%d", &seats);
            getchar();
            snprintf(buffer, sizeof(buffer), "CANCEL %d %d %d", ref, agency_id, seats);
            send(sock, buffer, strlen(buffer), 0);
            char response[256];
            int bytes = recv(sock, response, sizeof(response) - 1, 0);
            if (bytes > 0) {
                response[bytes] = '\0';
                if (strcmp(response, "SUCCESS") == 0) {
                    printf("Annulation réussie\n");
                } else {
                    printf("Échec de l'annulation\n");
                }
            } else {
                printf("Erreur de communication avec le serveur\n");
            }
        } else if (choice == 3) {
            snprintf(buffer, sizeof(buffer), "INVOICE %d", agency_id);
            send(sock, buffer, strlen(buffer), 0);
            char response[256];
            int bytes = recv(sock, response, sizeof(response) - 1, 0);
            if (bytes > 0) {
                response[bytes] = '\0';
                printf("Facture: %s\n", response);
            } else {
                printf("Erreur de communication avec le serveur\n");
            }
        } else if (choice == 4) {
            snprintf(buffer, sizeof(buffer), "CONSULT");
            send(sock, buffer, strlen(buffer), 0);
            char response[8192];
            int bytes = recv(sock, response, sizeof(response) - 1, 0);
            if (bytes > 0) {
                response[bytes] = '\0';
                printf("Liste des vols:\n%s", response);
            } else {
                printf("Erreur lors de la réception de la liste des vols\n");
            }
        } else if (choice == 5) {
            break;
        } else {
            printf("Choix invalide\n");
        }
    }

    close(sock);
    printf("Déconnexion\n");
    return 0;
}