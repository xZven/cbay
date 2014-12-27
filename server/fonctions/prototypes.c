/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

void welcome_message(int argc, char * argv[]);
void clean_b(char * buffer);
void load_server(struct server_t * server);
void screen_server(struct server_t server);
void init_socket(int * socklis, struct sockaddr_in * nom_server, struct server_t * server);
int rcv_socket(struct user_t * client, char * buffer);
bool send_socket(struct user_t * client, char * buffer);
bool error_msg(struct user_t * client, const char * error_code)
void shut_server(struct server_t * server);

