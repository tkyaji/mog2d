#ifndef PubSub_h
#define PubSub_h

#include <string>
#include <functional>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include "mog/core/Data.h"

using namespace std;

namespace mog {
    class PubSub : public enable_shared_from_this<PubSub> {
    public:
        void publish(string key);
        void publish(string key, const std::shared_ptr<Data> &param);
        
        unsigned int subscribe(string key, function<void(const std::shared_ptr<Data> &p)> func);
        void unsubscribe(string key, unsigned int subscribeId);
        void unsubscribeAll(string key);
        
        void propagate(const weak_ptr<PubSub> childPubsub);
        void stopPropagete(const weak_ptr<PubSub> childPubsub);
        
        PubSub();
        ~PubSub();
        
    private:
        unsigned int pubsubId = 0;
        static unsigned int pubsubInstanceId;
        unsigned int subscribeIdCounter = 0;
        
        map<string, map<unsigned int, function<void(const std::shared_ptr<Data> &p)>>> subscribers;
        map<unsigned int, weak_ptr<PubSub>> childPubsubs;
        map<unsigned int, weak_ptr<PubSub>> parentPubsubs;
    };
}

#endif /* PubSub_h */
