#include <iostream>

class UniquePtr {

private:

    int* ptr = nullptr;

public:

    explicit UniquePtr(int* p = nullptr) : ptr(p) {}

    ~UniquePtr() {

        delete ptr;
    }

    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) noexcept {

        ptr = other.ptr;
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {

        if (this == &other) {

            return *this;
        }

        delete ptr;
        ptr = other.ptr;
        other.ptr = nullptr;

        return *this;
    }

    int* get() const {

        return ptr;
    }

    int* release() {

        int* temp = ptr;
        ptr = nullptr;

        return temp;
    }

    void reset(int* p = nullptr) {

        if (p == ptr) {

            return;
        }

        delete ptr;
        ptr = p;
    }

    int& operator*() const {

        return *ptr;
    }

    int* operator->() const {

        return ptr;
    }
};

class ControlBlock {

public:

    int* ptr;
    int shared_count;
    int weak_count;

    ControlBlock(int* p = nullptr) {

        ptr = p;
        shared_count = 1;
        weak_count = 0;
    }
};

class WeakPtr;

class SharedPtr {

    friend class WeakPtr;

private:

    ControlBlock* cb;

public:

    SharedPtr() {

        cb = nullptr;
    }

    SharedPtr(int* p) {

        if (p != nullptr) {

            cb = new ControlBlock(p);

        } else {

            cb = nullptr;
        }
    }

    ~SharedPtr() {

        if (cb != nullptr) {

            --cb->shared_count;

            if (cb->shared_count == 0) {

                delete cb->ptr;
                cb->ptr = nullptr;

                if (cb->weak_count == 0) {

                    delete cb;
                    cb = nullptr;
                }
            }
        }
    }

    SharedPtr(const SharedPtr& other) {

        cb = other.cb;

        if (cb != nullptr) {

            ++cb->shared_count;
        }
    }

    SharedPtr(SharedPtr&& other) noexcept {

        cb = other.cb;
        other.cb = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {

        if (this != &other) {

            if (cb != nullptr) {

                --cb->shared_count;

                if (cb->shared_count == 0) {

                    delete cb->ptr;
                    cb->ptr = nullptr;

                    if (cb->weak_count == 0) {

                        delete cb;
                        cb = nullptr;
                    }
                }
            }

            cb = other.cb;
            other.cb = nullptr;
        }

        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) {

        if (this != &other) {

            if (cb != nullptr) {

                --cb->shared_count;

                if (cb->shared_count == 0) {

                    delete cb->ptr;
                    cb->ptr = nullptr;

                    if (cb->weak_count == 0) {

                        delete cb;
                        cb = nullptr;
                    }
                }
            }

            cb = other.cb;

            if (cb != nullptr) {

                ++cb->shared_count;
            }
        }

        return *this;
    }

    int* get() const {

        if (cb == nullptr) {

            return nullptr;
        }

        return cb->ptr;
    }

    int use_count() const {

        if (cb != nullptr) {

            return cb->shared_count;
        }

        return 0;
    }

    void reset(int* p = nullptr) {

        if (cb != nullptr) {

            --cb->shared_count;

            if (cb->shared_count == 0) {

                delete cb->ptr;
                cb->ptr = nullptr;

                if (cb->weak_count == 0) {

                    delete cb;
                    cb = nullptr;
                }
            }
        }

        if (p != nullptr) {

            cb = new ControlBlock(p);

        } else {

            cb = nullptr;
        }
    }

    int& operator*() const {

        return *cb->ptr;
    }

    int* operator->() const {

        return cb->ptr;
    }
};

class WeakPtr {

private:

    ControlBlock* cb;

public:

    WeakPtr() {

        cb = nullptr;
    }

    WeakPtr(const SharedPtr& shared) {

        cb = shared.cb;

        if (cb != nullptr) {

            ++cb->weak_count;
        }
    }

    ~WeakPtr() {

        if (cb != nullptr) {

            --cb->weak_count;

            if (cb->shared_count == 0 && cb->weak_count == 0) {

                delete cb;
                cb = nullptr;
            }
        }
    }

    bool expired() const {

        if (cb == nullptr || cb->shared_count == 0) {

            return true;
        }

        return false;
    }

    SharedPtr lock() const {

        if (expired()) {

            return SharedPtr();
        }

        SharedPtr sp;
        sp.cb = cb;

        ++sp.cb->shared_count;

        return sp;
    }

    WeakPtr(const WeakPtr& other) {

        cb = other.cb;

        if (cb != nullptr) {

            ++cb->weak_count;
        }
    }

    WeakPtr(WeakPtr&& other) noexcept {

        cb = other.cb;
        other.cb = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& other) {

        if (this != &other) {

            if (cb != nullptr) {

                --cb->weak_count;

                if (cb->shared_count == 0 && cb->weak_count == 0) {

                    delete cb;
                    cb = nullptr;
                }
            }

            cb = other.cb;

            if (cb != nullptr) {

                ++cb->weak_count;
            }
        }

        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {

        if (this != &other) {

            if (cb != nullptr) {

                --cb->weak_count;

                if (cb->shared_count == 0 && cb->weak_count == 0) {

                    delete cb;
                }
            }

            cb = other.cb;
            other.cb = nullptr;
        }

        return *this;
    }
};

int main() {

    std::cout << "=== 1. Testing Basic SharedPtr & WeakPtr Operations ===" << std::endl;

    SharedPtr sp1(new int(42));
    WeakPtr wp = sp1;

    std::cout << "sp1 use_count: " << sp1.use_count() << std::endl;
    std::cout << "wp is expired? " << wp.expired() << std::endl;

    {
        SharedPtr sp2 = wp.lock();
        std::cout << "Value locked by sp2: " << *sp2 << std::endl;
        std::cout << "use_count inside block: " << sp1.use_count() << std::endl;
    }

    std::cout << "use_count outside block: " << sp1.use_count() << std::endl;

    std::cout << "\n=== 2. Testing Expiration ===" << std::endl;
    {
        SharedPtr sp3(new int(100));
        wp = sp3;
        std::cout << "sp3 use_count: " << sp3.use_count() << std::endl;
    }

    std::cout << "wp is expired after sp3 destruction? " << wp.expired() << std::endl;

    SharedPtr sp_locked = wp.lock();

    if (sp_locked.get() == nullptr) {

        std::cout << "Lock failed as expected: target memory was already freed." << std::endl;
    }


    return 0;
}
