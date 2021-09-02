#include <vector>

#include <thread_pool/queue.h>

namespace thread_pool {
    class MDList {
        public:
            MDList(const uint32_t& depth):
                _nodes() {
                // initialize nodes pointers
                for (uint32_t i = 0; i < depth; i++) {
                    _nodes.emplace_back(new Queue());
                }
            }
            ~MDList() {
                // delete nodes
                for (auto n : _nodes) {
                    if (n) {
                        delete n;
                    }
                }
            }

            // add new task to the queue
            template<class F, class ... Args>
            auto Produce(const uint32_t& p, F&& function, Args&& ... args) {
                if (p < _nodes.size()) {
                    return _nodes.at(p)->Produce(std::bind(function, std::forward<Args>(args)...));
                }
            }
            // TODO(jpinyot): change comment
            // consume one task from the most biggest priority avalible
            void Consume() {
                for (const auto node : _nodes) {
                    if (node->TasksCount()) {
                        node->Consume();
                        break;
                    }
                }
            }

        private:
            // pointers to the nodes of the list
            // TODO(jpinyot): convert to a vector of unique_ptr
            std::vector<Queue*> _nodes;
    };  // class MDList
}  // namespace thread_pool
