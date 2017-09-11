#include "mog/core/PubSub.h"

using namespace mog;

unsigned int PubSub::pubsubInstanceId = 0;

PubSub::PubSub() {
    this->pubsubId = ++PubSub::pubsubInstanceId;
}

PubSub::~PubSub() {
    for (auto pair : this->parentPubsubs) {
        if (auto pubsub = pair.second.lock()) {
            pubsub->childPubsubs.erase(this->pubsubId);
        }
    }
}

void PubSub::publish(string key) {
    this->publish(key, PubSub::Param(Data()));
}

void PubSub::publish(string key, const PubSub::Param &param) {
    if (this->subscribers.count(key) > 0) {
        for (auto &sub : this->subscribers[key]) {
            sub.second(param);
        }
    }
    for (auto &pair : this->childPubsubs) {
        if (auto pubsub = pair.second.lock()) {
            pubsub->publish(key, param);
        }
    }
}

unsigned int PubSub::subscribe(string key, function<void(const Param &p)> func) {
    unsigned int pubsubId = ++this->subscribeIdCounter;
    this->subscribers[key][pubsubId] = func;
    return pubsubId;
}

void PubSub::unsubscribe(string key, unsigned int subscribeId) {
    this->subscribers[key].erase(subscribeId);
}

void PubSub::unsubscribeAll(string key) {
    this->subscribers.erase(key);
}

void PubSub::propagate(const weak_ptr<PubSub> childPubsub) {
    unsigned int pubsubId = childPubsub.lock()->pubsubId;
    childPubsub.lock()->parentPubsubs[this->pubsubId] = shared_from_this();
    this->childPubsubs[pubsubId] = childPubsub;
}

void PubSub::stopPropagete(const weak_ptr<PubSub> childPubsub) {
    childPubsub.lock()->parentPubsubs.erase(this->pubsubId);
    this->childPubsubs.erase(childPubsub.lock()->pubsubId);
}
