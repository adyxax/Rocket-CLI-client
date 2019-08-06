#include "subscriptions.h"

void common_subscription_add(struct subscription** subscriptions, const char* id, const char* name, enum subscription_type type)
{
    struct subscription * subscription = common_subscription_new(id, name, type);
    HASH_ADD_KEYPTR(hh, *subscriptions, subscription->id, strlen(id), subscription);
}

struct subscription* common_subscription_new(const char* id, const char* name, enum subscription_type type)
{
    struct subscription* subscription = malloc(sizeof(struct subscription));
    subscription->id = malloc(strlen(id) + 1);
    strcpy(subscription->id, id);
    subscription->name = malloc(strlen(name) + 1);
    strcpy(subscription->name, name);
    subscription->type = type;
    return subscription;
}

void common_subscriptions_free(struct subscription* subscriptions)
{
    struct subscription *sub, *tmp;

    HASH_ITER(hh, subscriptions, sub, tmp) {
        HASH_DEL(subscriptions, sub);
        free(sub->id);
        free(sub->name);
        free(sub);
    }
}

void common_subscriptions_const_walk(const struct subscription* subscriptions, void (*func)(const struct subscription*))
{
    const struct subscription *sub, *tmp;

    HASH_ITER(hh, subscriptions, sub, tmp) {
        func(sub);
    }
}
