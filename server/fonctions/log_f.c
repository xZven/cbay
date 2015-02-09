

void log_inscription(struct user_t * client, struct server_t * server)
{
	struct log_t event;

	event.item_uid 		= 0;
	event.seller_uid 	= client->uid;
	event.buyer_uid 	= client->uid;
	event.event 		= 'i';
	time(&event.date);
	
	encode_log(&event, server);
	
}

void log_connect(struct user_t * client, struct server_t * server)
{
	struct log_t event;

	event.item_uid 		= 0;
	event.seller_uid 	= client->uid;
	event.buyer_uid 	= client->uid;
	event.event 		= 'c';
	time(&event.date);
	
	encode_log(&event, server);
}

void log_bid_up(struct user_t * client,struct object_t * item, struct server_t * server)
{
	struct log_t event;

	event.item_uid 		= item->uid;
	event.seller_uid 	= item->vendeur;
	event.buyer_uid 	= 0;
	event.event 		= 'b';
	time(&event.date);
	
	encode_log(&event, server);

}

void log_cancel_bid(struct user_t * client,struct object_t * item, struct server_t * server)
{
	struct log_t event;

	event.item_uid 		= item->uid;
	event.seller_uid 	= item->vendeur;
	event.buyer_uid 	= 0;
	event.event 		= 'l';
	time(&event.date);
	
	encode_log(&event, server);
}

