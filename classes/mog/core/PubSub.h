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
        class Param {
        public:
            template <class T/*, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler*/>
            Param(T data) {
                this->data = shared_ptr<T>(new T(data));
            }
            template <class T/*, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler*/>
            T get() {
                return *static_pointer_cast<T>(this->data).get();
            }
            template <class T/*, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler*/>
            T get() const {
                return *static_pointer_cast<T>(this->data).get();
            }
            DataType getType() {
                return this->data->type;
            }
        private:
            shared_ptr<Data> data;
        };
        
        void publish(string key);
        void publish(string key, const PubSub::Param &param);
        template <class T/*, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler*/>
        void publish(string key, T value) {
            this->publish(key, Param(value));
        }
        
        unsigned int subscribe(string key, function<void(const Param &p)> func);
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
        
        map<string, map<unsigned int, function<void(const Param &p)>>> subscribers;
        map<unsigned int, weak_ptr<PubSub>> childPubsubs;
        map<unsigned int, weak_ptr<PubSub>> parentPubsubs;
    };
}

#endif /* PubSub_h */
