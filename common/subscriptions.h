#ifndef COMMON_SUBSCRIPTIONS_H_
#define COMMON_SUBSCRIPTIONS_H_

#include <uthash.h>

static const char *subscription_type_str[] = {
    "channel", "direct", "private",
};

enum subscription_type {
    SUBSCRIPTION_CHANNEL,
    SUBSCRIPTION_DIRECT,
    SUBSCRIPTION_PRIVATE,
};

struct subscription {
    char* id;
    char* name;
    enum subscription_type type;
    UT_hash_handle hh;
};

void common_subscription_add(struct subscription** subscriptions, const char* id, const char* name, enum subscription_type type);
struct subscription* common_subscription_new(const char* id, const char* name, enum subscription_type type);
void common_subscriptions_free(struct subscription* subscriptions);
void common_subscriptions_const_walk(const struct subscription* subscriptions, void (*func)(const struct subscription*));

#endif
