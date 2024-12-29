#ifndef CLIENT_H
#define CLIENT_H

/**
 * Démarre le client et se connecte à un serveur.
 * 
 * Cette fonction initialise la connexion au serveur en utilisant l'adresse IP 
 * fournie. Elle gère également l'affichage à l'aide du renderer SDL pour l'interface 
 * du client.
 * 
 * @param server_ip : L'adresse IP du serveur auquel le client doit se connecter.
 * @param renderer : Le renderer SDL utilisé pour l'affichage de l'interface utilisateur.
 */
void start_client(const char *server_ip, SDL_Renderer *renderer);

#endif // CLIENT_H
