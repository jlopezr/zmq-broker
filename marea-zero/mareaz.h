#ifndef _MAREAZ_H_
#define _MAREAZ_H_

struct mz_ctx_t { };

struct mz_msg_t { 
    const char* topic;
    const char* data;
    int data_len;
};

mz_msg_t* mz_msg_new();
void mz_msg_destroy(mz_msg_t** msg);
void mz_msg_str(mz_msg_t* msg, const char* data);
mz_msg_t* mz_msg_dup(mz_msg_t* msg)

mz_ctx_t* mz_ctx_new();
void mz_ctx_destroy(mz_ctx_t** ctx);

void mz_publish(mz_ctx_t* ctx, const char* topic, char* data, int len);
void mz_publish_str(mz_ctx_t* ctx, const char* topic, char* data);
void mz_publish_msg(mz_ctx_t* ctx, mz_msg_t* msg);
void mz_unpublish(mz_ctx_t* ctx, const char* topic);
//TODO mmm... No hay unpublish en GMSEC? => Como dejamos de publicar!
//     Entiendo que ya no dejamos de publicar hasta que se para.

void mz_subscribe(mz_ctx_t* ctx, const char* topic);
void mz_unsubscribe(mz_ctx_t* ctx, const char* topic);
mz_msg_t* mz_get_message();

void mz_subscribe_cb(mz_ctx_t* ctx, const char* topic, mz_callback_t* cb);
void mz_unsubscribe_cb(mz_ctx_t* ctx, const char* topic, mz_callback_t* cb);

#endif
