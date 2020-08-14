#include "subscriptions.h"

void common_subscription_add(struct subscription** subscriptions, const char* rid, const char* name, enum subscription_type type, size_t unread, char alert)
{
    struct subscription * subscription = common_subscription_new(rid, name, type, unread, alert);
    HASH_ADD_KEYPTR(hh, *subscriptions, subscription->rid, strlen(rid), subscription);
}

struct subscription* common_subscription_new(const char* rid, const char* name, enum subscription_type type, size_t unread, char alert)
{
    struct subscription* subscription = malloc(sizeof(struct subscription));
    subscription->rid = malloc(strlen(rid) + 1);
    strcpy(subscription->rid, rid);
    subscription->name = malloc(strlen(name) + 1);
    strcpy(subscription->name, name);
    subscription->type = type;
    subscription->unread = unread;
    subscription->alert = alert;
    return subscription;
}

void common_subscriptions_free(struct subscription* subscriptions)
{
    struct subscription *sub, *tmp;

    HASH_ITER(hh, subscriptions, sub, tmp) {
        HASH_DEL(subscriptions, sub);
        free(sub->rid);
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

size_t common_subscriptions_count(const struct subscription* subscriptions)
{
    return HASH_COUNT(subscriptions);
}
