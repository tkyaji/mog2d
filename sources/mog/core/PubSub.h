#ifndef PubSub_h
#define PubSub_h

#include <string>
#include <functional>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include "mog/core/Data.h"

namespace mog {
    class PubSub : public std::enable_shared_from_this<PubSub> {
    public:
        void publish(std::string key);
        void publish(std::string key, const std::shared_ptr<Data> &param);
        
        unsigned int subscribe(std::string key, std::function<void(const std::shared_ptr<Data> &p)> func);
        void unsubscribe(std::string key, unsigned int subscribeId);
        void unsubscribeAll(std::string key);
        
        void propagate(const std::weak_ptr<PubSub> childPubsub);
        void stopPropagete(const std::weak_ptr<PubSub> childPubsub);
        
        PubSub();
        ~PubSub();
        
    private:
        unsigned int pubsubId = 0;
        static unsigned int pubsubInstanceId;
        unsigned int subscribeIdCounter = 0;
        
        std::map<std::string, std::map<unsigned int, std::function<void(const std::shared_ptr<Data> &p)>>> subscribers;
        std::map<unsigned int, std::weak_ptr<PubSub>> childPubsubs;
        std::map<unsigned int, std::weak_ptr<PubSub>> parentPubsubs;
    };
}

#endif /* PubSub_h */
