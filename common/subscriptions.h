#ifndef COMMON_SUBSCRIPTIONS_H_
#define COMMON_SUBSCRIPTIONS_H_

#include <uthash.h>

enum subscription_type {
    SUBSCRIPTION_CHANNEL,
    SUBSCRIPTION_DIRECT,
    SUBSCRIPTION_PRIVATE,
};

struct subscription {
    char* rid;
    char* name;
    enum subscription_type type;
    size_t unread;
    UT_hash_handle hh;
};

void common_subscription_add(struct subscription** subscriptions, const char* id, const char* name, enum subscription_type type, size_t unread);
struct subscription* common_subscription_new(const char* rid, const char* name, enum subscription_type type, size_t unread);
void common_subscriptions_free(struct subscription* subscriptions);
void common_subscriptions_const_walk(const struct subscription* subscriptions, void (*func)(const struct subscription*));
size_t common_subscriptions_count(const struct subscription* subscriptions);

#endif
