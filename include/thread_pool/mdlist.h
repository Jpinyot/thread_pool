#include <vector>

#include <thread_pool/queue.h>

namespace thread_pool {
    class MDList {
        public:
            MDList(const uint32_t& depth):
                _nodes() {
                // initialize nodes pointers
                for (uint32_t i = 0; i <= depth; i++) {
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
                // set position to last if given position is to high
                int position = (p < _nodes.size())? p : (_nodes.size() - 1);
                // produce new Task
                return _nodes.at(position)->Produce(std::bind(function,
                            std::forward<Args>(args)...));
            }
            // consume one task from the highest priority avalible
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
