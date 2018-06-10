#ifndef MY_LIST
#define MY_LIST

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

   public:
    list() noexcept { loop.next = loop.prev = &loop; }

    list(list<T> const& other) : list() {
        if (&other.loop != nullptr) {
            node_base* cur = other.loop.next;
            while (cur != &other.loop) {
                push_back(static_cast<node*>(cur)->value);
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
    using const_reverse_iterator =
        std::reverse_iterator<list_iterator<T const>>;

   private:
    template <typename U>
    struct list_iterator
        : public std::iterator<std::bidirectional_iterator_tag, U> {
       public:
        friend class list<T>;
        list_iterator() = default;
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
        U& operator*() { return static_cast<node*>(ptr)->value; }

        template <typename Z>
        bool operator==(list_iterator<Z> const& other) const {
            return ptr == other.ptr;
        }
        template <typename Z>
        bool operator!=(list_iterator<Z> const& other) const {
            return ptr != other.ptr;
        }

       private:
        list_iterator(node_base* p) : ptr(p) {}
        node_base* ptr;
    };

   public:
    iterator begin() { return iterator(loop.next); }
    const_iterator begin() const { return iterator(loop.next); }

    iterator end() { return iterator(&loop); }
    const_iterator end() const {
        return const_iterator(const_cast<node_base*>(&loop));
    }

    const_iterator cend() const { return const_iterator(&loop); }
    const_iterator cbegin() const { return const_iterator(&loop->next); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    void push_back(T const& value) {
        node_base* last = loop.prev;
        loop.prev = new node(value, last, &loop);
        last->next = loop.prev;
    }

    void pop_back() {
        assert(&loop != loop.next);
        node_base* to_del = loop.prev;
        loop.prev->prev->next = &loop;
        loop.prev = loop.prev->prev;
        delete to_del;
    }

    T& back() {
        assert(&loop != loop.next);
        return static_cast<node*>(loop.prev)->value;
    }
    T const& back() const {
        assert(&loop != &loop->next);
        return static_cast<node*>(&loop->prev)->value;
    }

    void push_front(T const& value) {
        node_base* first = loop.next;
        loop.next = new node(value, &loop, first);
        first->prev = loop.next;
    }
    void pop_front() {
        assert(&loop != loop.next);
        node_base* to_del = loop.next;
        loop.next->next->prev = &loop;
        loop.next = loop.next->next;
        delete to_del;
    }

    T& front() {
        assert(&loop != loop.next);
        return static_cast<node*>(loop.next)->value;
    }
    T const& front() const {
        assert(loop != &loop->next);
        return static_cast<node*>(&loop->next)->value;
    }

    bool empty() const { return &loop == loop.next; }

    void clear() {
        node_base* cur = loop.next;
        while (cur != &loop) {
            node_base* to_del = cur;
            cur = cur->next;
            delete to_del;
        }
        loop.next = loop.prev = &loop;
    }

    iterator insert(const_iterator pos, T const& value) {
        auto p1 = pos;
        p1.ptr->prev = new node(value, p1.ptr->prev, p1.ptr);
        p1.ptr->prev->prev->next = p1.ptr->prev;
        return iterator(p1.ptr->prev);
    }

    iterator erase(const_iterator pos) {
        assert(&loop != loop.next);
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
    auto a_left = a.loop.prev;
    auto a_right = a.loop.next;

    auto b_left = b.loop.prev;
    auto b_right = b.loop.next;

    a_left->next = &b.loop;
    a_right->prev = &b.loop;

    b_left->next = &a.loop;
    b_right->prev = &a.loop;

    std::swap(a.loop, b.loop);
}

}  // namespace my

#endif  // MY_LIST
