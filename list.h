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

    struct node : virtual node_base {
        T value;

        node() = delete;
        node(T v) : node_base(), value(v) {}
        node(T v, node_base* p) : node_base(p), value(v) {}
        node(T v, node_base* p, node_base* n) : node_base(p, n), value(v) {}
    };

    node_base loop;
    node_base* fake = &loop;

   public:
    list() noexcept { fake->next = fake->prev = fake; }

    list(list<T> const& other) : list() {
        if (other.fake != nullptr) {
            node_base* cur = other.fake->next;
            while (cur != other.fake) {
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
        U& operator*() { return dynamic_cast<node*>(ptr)->value; }

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
    iterator begin() {
        return iterator((fake == nullptr) ? nullptr : fake->next);
    }
    const_iterator begin() const {
        return iterator((fake == nullptr) ? nullptr : fake->next);
    }

    iterator end() { return iterator(fake); }
    const_iterator end() const { return const_iterator(fake); }

    const_iterator cend() const { return const_iterator(fake); }
    const_iterator cbegin() const {
        return const_iterator((fake == nullptr) ? nullptr : fake->next);
    }

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
        node_base* last = fake->prev;
        fake->prev = new node(value, last, fake);
        last->next = fake->prev;
    }

    void pop_back() {
        assert(fake != fake->next);
        node_base* to_del = fake->prev;
        fake->prev->prev->next = fake;
        fake->prev = fake->prev->prev;
        delete to_del;
    }

    T& back() {
        assert(fake != fake->next);
        return dynamic_cast<node*>(fake->prev)->value;
    }
    T const& back() const {
        assert(fake != fake->next);
        return dynamic_cast<node*>(fake->prev)->value;
    }

    void push_front(T const& value) {
        node_base* first = fake->next;
        fake->next = new node(value, fake, first);
        first->prev = fake->next;
    }
    void pop_front() {
        assert(fake != fake->next);
        node_base* to_del = fake->next;
        fake->next->next->prev = fake;
        fake->next = fake->next->next;
        delete to_del;
    }

    T& front() {
        assert(fake != fake->next);
        return dynamic_cast<node*>(fake->next)->value;
    }
    T const& front() const {
        assert(fake != fake->next);
        return dynamic_cast<node*>(fake->next)->value;
    }

    bool empty() const { return fake == fake->next; }

    void clear() {
        node_base* cur = fake->next;
        while (cur != fake) {
            node_base* to_del = cur;
            cur = cur->next;
            delete to_del;
        }
        fake->next = fake->prev = fake;
    }

    iterator insert(const_iterator pos, T const& value) {
        auto p1 = pos;
        p1.ptr->prev = new node(value, p1.ptr->prev, p1.ptr);
        p1.ptr->prev->prev->next = p1.ptr->prev;
        return iterator(p1.ptr->prev);
    }

    iterator erase(const_iterator pos) {
        assert(fake != fake->next);
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
    auto a_left = a.fake->prev;
    auto a_right = a.fake->next;

    auto b_left = b.fake->prev;
    auto b_right = b.fake->next;

    a_left->next = b.fake;
    a_right->prev = b.fake;

    b_left->next = a.fake;
    b_right->prev = a.fake;

    std::swap(a.loop, b.loop);
}

}  // namespace my

#endif  // MY_LIST
