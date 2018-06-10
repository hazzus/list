#ifndef MY_LIST
#define MY_LIST

// exception safe
// не обязательно t DefaultConstructable
// default constructor - nothrow

#include <cassert>
#include <iterator>

namespace my {

template <typename T>
class list {
   private:
    struct node_base {
        node_base* next;
        node_base* prev;
        node_base() : next(nullptr), prev(nullptr) {}
        node_base(node_base* p) : next(nullptr), prev(p) {}
        node_base(node_base* p, node_base* n) : next(n), prev(p) {}
        virtual ~node_base() = default;
    };

    struct node : node_base {
        T value;

        node() = delete;
        node(T v) : node_base(), value(v) {}
        node(T v, node_base* p) : node_base(p), value(v) {}
        node(T v, node_base* p, node_base* n) : node_base(p, n), value(v) {}
    };

    node_base loop;
    node_base* loop_ptr = &loop;

   public:
    list() noexcept { loop_ptr->next = loop_ptr->prev = loop_ptr; }

    list(list<T> const& other) : list() {
        if (other.loop_ptr != nullptr) {
            node_base* cur = other.loop_ptr->next;
            while (cur != other.loop_ptr) {
                push_back(dynamic_cast<node*>(cur)->value);
                cur = cur->next;
            }
        }
    }

    list(std::initializer_list<T> init_list) : list() {
        for (auto x : init_list) {
            push_back(x);
        }
    }

    list<T>& operator=(list<T> const& other) {
        list<T> tmp(other);
        swap(tmp, *this);
        return *this;
    }

    ~list() { clear(); }

   private:
    template <typename U>
    struct list_iterator;

   public:
    using iterator = list_iterator<T>;
    using const_iterator = list_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<list_iterator<T>>;
    using reverse_const_iterator =
        std::reverse_iterator<list_iterator<T const>>;

   private:
    template <typename U>
    struct list_iterator
        : public std::iterator<std::bidirectional_iterator_tag, U> {
       public:
        friend class list<T>;
        list_iterator() = default;
        list_iterator(node_base* p) : ptr(p) {}
        list_iterator(list_iterator<T> const& other) : ptr(other.ptr) {}
        list_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }
        list_iterator operator++(int) {
            list_iterator<U> old(*this);
            ++*this;
            return old;
        }
        list_iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }
        list_iterator operator--(int) {
            list_iterator<U> old(*this);
            --*this;
            return old;
        }
        U& operator*() { return dynamic_cast<node*>(ptr)->value; }
        bool operator==(list_iterator<U> const& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(list_iterator<U> const& other) const {
            return ptr != other.ptr;
        }

       private:
        node_base* ptr;
    };

   public:
    iterator begin() {
        return iterator((loop_ptr == nullptr) ? nullptr : loop_ptr->next);
    }
    const_iterator begin() const {
        return iterator((loop_ptr == nullptr) ? nullptr : loop_ptr->next);
    }

    iterator end() { return iterator(loop_ptr); }
    const_iterator end() const { return const_iterator(loop_ptr); }

    const_iterator cend() const { return const_iterator(loop_ptr); }
    const_iterator cbegin() const {
        return iterator((loop_ptr == nullptr) ? nullptr : loop_ptr->next);
    }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_const_iterator rbegin() const { return reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    reverse_const_iterator rend() const {
        return reverse_const_iterator(begin());
    }

    void push_back(T const& value) {
        node_base* last = loop_ptr->prev;
        loop_ptr->prev = new node(value, last, loop_ptr);
        last->next = loop_ptr->prev;
    }

    void pop_back() {
        assert(loop_ptr != loop_ptr->next);
        node_base* to_del = loop_ptr->prev;
        loop_ptr->prev->prev->next = loop_ptr;
        loop_ptr->prev = loop_ptr->prev->prev;
        delete to_del;
    }

    T& back() {
        assert(loop_ptr != loop_ptr->next);
        return dynamic_cast<node*>(loop_ptr->prev)->value;
    }
    T const& back() const {
        assert(loop_ptr != loop_ptr->next);
        return dynamic_cast<node*>(loop_ptr->prev)->value;
    }

    void push_front(T const& value) {
        node_base* first = loop_ptr->next;
        loop_ptr->next = new node(value, loop_ptr, first);
        first->prev = loop_ptr->next;
    }
    void pop_front() {
        assert(loop_ptr != loop_ptr->next);
        node_base* to_del = loop_ptr->next;
        loop_ptr->next->next->prev = loop_ptr;
        loop_ptr->next = loop_ptr->next->next;
        delete to_del;
    }

    T& front() {
        assert(loop_ptr != loop_ptr->next);
        return dynamic_cast<node*>(loop_ptr->next)->value;
    }
    T const& front() const {
        assert(loop_ptr != loop_ptr->next);
        return dynamic_cast<node*>(loop_ptr->next)->value;
    }

    bool empty() const { return loop_ptr == loop_ptr->next; }

    void clear() {
        node_base* cur = loop_ptr->next;
        while (cur != loop_ptr) {
            node_base* to_del = cur;
            cur = cur->next;
            delete to_del;
        }
        loop_ptr->next = loop_ptr->prev = loop_ptr;
    }

    iterator insert(const_iterator pos, T const& value) {
        auto p1 = pos;
        p1.ptr->prev = new node(value, p1.ptr->prev, p1.ptr);
        p1.ptr->prev->prev->next = p1.ptr->prev;
        return iterator(p1.ptr->prev);
    }

    iterator erase(const_iterator pos) {
        assert(loop_ptr != loop_ptr->next);
        pos.ptr->prev->next = pos.ptr->next;
        pos.ptr->next->prev = pos.ptr->prev;
        iterator to_ret(pos.ptr->next);
        delete pos.ptr;
        return to_ret;
    }

    iterator erase(const_iterator begin, const_iterator end) {
        node_base n;
        n.next = begin.ptr;
        n.prev = end.ptr->prev;

        begin.ptr->prev->next = end.ptr;
        end.ptr->prev->next = &n;

        end.ptr->prev = begin.ptr->prev;
        begin.ptr->prev = &n;

        node_base* cur = n.next;
        while (cur != &n) {
            node_base* to_del = cur;
            cur = cur->next;
            delete to_del;
        }
        return iterator(n.next);
    }

    void splice(const_iterator pos, list<T>& other, const_iterator begin,
                const_iterator end) {
        node_base* to_con_left = begin.ptr->prev;

        pos.ptr->prev->next = begin.ptr;
        begin.ptr->prev = pos.ptr->prev;

        end.ptr->prev->next = pos.ptr;
        pos.ptr->prev = end.ptr->prev;

        end.ptr->prev = to_con_left;
        to_con_left->next = end.ptr;
    }

    template <typename U>
    friend void swap(list<U>& a, list<U>& b) noexcept;
};

template <typename U>
void swap(list<U>& a, list<U>& b) noexcept {
    auto a_left = a.loop_ptr->prev;
    auto a_right = a.loop_ptr->next;

    auto b_left = b.loop_ptr->prev;
    auto b_right = b.loop_ptr->next;

    a_left->next = b.loop_ptr;
    a_right->prev = b.loop_ptr;

    b_left->next = a.loop_ptr;
    b_right->prev = a.loop_ptr;

    std::swap(a.loop, b.loop);
}

}  // namespace my

#endif  // MY_LIST
