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

void PubSub::publish(std::string key) {
    this->publish(key, nullptr);
}

void PubSub::publish(std::string key, const std::shared_ptr<Data> &param) {
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

unsigned int PubSub::subscribe(std::string key, std::function<void(const std::shared_ptr<Data> &p)> func) {
    unsigned int pubsubId = ++this->subscribeIdCounter;
    this->subscribers[key][pubsubId] = func;
    return pubsubId;
}

void PubSub::unsubscribe(std::string key, unsigned int subscribeId) {
    this->subscribers[key].erase(subscribeId);
}

void PubSub::unsubscribeAll(std::string key) {
    this->subscribers.erase(key);
}

void PubSub::propagate(const std::weak_ptr<PubSub> childPubsub) {
    unsigned int pubsubId = childPubsub.lock()->pubsubId;
    childPubsub.lock()->parentPubsubs[this->pubsubId] = shared_from_this();
    this->childPubsubs[pubsubId] = childPubsub;
}

void PubSub::stopPropagete(const std::weak_ptr<PubSub> childPubsub) {
    childPubsub.lock()->parentPubsubs.erase(this->pubsubId);
    this->childPubsubs.erase(childPubsub.lock()->pubsubId);
}
