# Smart Pointer Implementation in C++

A custom implementation of three fundamental C++ smart-pointer concepts:

* `UniquePtr`
* `SharedPtr`
* `WeakPtr`

The project demonstrates how smart pointers manage dynamically allocated memory using **RAII**, **move semantics**, **reference counting**, and **weak references**.

---

## 📌 Project Overview

The goal of this project is to understand how C++ smart pointers work internally by implementing their basic behavior without using the standard library versions such as:

```cpp
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

The project manually handles memory ownership and object lifetime.

---

## 🧠 Implemented Classes

### `UniquePtr`

`UniquePtr` provides **exclusive ownership** of a dynamically allocated object.

Only one `UniquePtr` can own a specific memory location at a time.

#### Main features

* Automatic memory deallocation
* Copying is disabled
* Move constructor
* Move assignment operator
* `get()`
* `release()`
* `reset()`
* `operator*`
* `operator->`

Example:

```cpp
UniquePtr ptr(new int(42));

std::cout << *ptr << std::endl;
```

When `ptr` goes out of scope, the allocated memory is automatically released.

---

### `SharedPtr`

`SharedPtr` provides **shared ownership**.

Multiple `SharedPtr` objects can point to the same dynamically allocated object.

A `ControlBlock` is used to keep track of the number of owners.

```text
SharedPtr
    |
    v
ControlBlock
    |
    +---- ptr
    +---- shared_count
    +---- weak_count
```

#### Main features

* Shared ownership
* Reference counting
* Copy constructor
* Copy assignment
* Move constructor
* Move assignment
* `use_count()`
* `get()`
* `reset()`
* `operator*`
* `operator->`

Example:

```cpp
SharedPtr sp1(new int(42));
SharedPtr sp2 = sp1;

std::cout << sp1.use_count() << std::endl;
```

After copying, both pointers share the same control block and:

```text
shared_count = 2
```

When one `SharedPtr` is destroyed, the counter decreases.

The managed object is deleted only when:

```text
shared_count == 0
```

---

### `WeakPtr`

`WeakPtr` provides a **non-owning reference** to an object managed by `SharedPtr`.

Unlike `SharedPtr`, creating a `WeakPtr` does not increase `shared_count`.

Instead, it increases:

```text
weak_count
```

This allows a `WeakPtr` to observe an object without keeping it alive.

#### Main features

* Non-owning reference
* `expired()`
* `lock()`
* Copy constructor
* Copy assignment
* Move constructor
* Move assignment

Example:

```cpp
SharedPtr sp(new int(42));
WeakPtr wp = sp;

if (!wp.expired()) {
    SharedPtr locked = wp.lock();

    std::cout << *locked << std::endl;
}
```

---

## 🔗 Control Block

The `ControlBlock` is the central part of the `SharedPtr` / `WeakPtr` implementation.

It contains:

```cpp
class ControlBlock {
public:
    int* ptr;
    int shared_count;
    int weak_count;
};
```

### `ptr`

Stores the address of the dynamically allocated object.

### `shared_count`

Stores the number of active `SharedPtr` objects.

### `weak_count`

Stores the number of active `WeakPtr` objects.

---

## 🔄 Reference Counting

Suppose we create:

```cpp
SharedPtr sp1(new int(42));
```

Initially:

```text
shared_count = 1
weak_count   = 0
```

Then:

```cpp
SharedPtr sp2 = sp1;
```

Now:

```text
shared_count = 2
weak_count   = 0
```

If we create:

```cpp
WeakPtr wp = sp1;
```

the counters become:

```text
shared_count = 2
weak_count   = 1
```

Destroying `sp2`:

```text
shared_count = 1
weak_count   = 1
```

Destroying the last `SharedPtr`:

```text
shared_count = 0
weak_count   = 1
```

At this point the managed object is deleted, but the `ControlBlock` remains alive because the `WeakPtr` still needs it.

When the last `WeakPtr` is destroyed:

```text
shared_count = 0
weak_count   = 0
```

The `ControlBlock` can finally be deleted.

---

## 🔐 Ownership Model

The project demonstrates three different ownership models:

| Pointer     | Ownership  | Copyable | Moveable |
| ----------- | ---------- | -------- | -------- |
| `UniquePtr` | Exclusive  | ❌        | ✅        |
| `SharedPtr` | Shared     | ✅        | ✅        |
| `WeakPtr`   | Non-owning | ✅        | ✅        |

---

## 🔄 Move Semantics

The implementation uses move semantics to transfer ownership without copying the managed pointer.

For example:

```cpp
SharedPtr sp1(new int(42));
SharedPtr sp2 = std::move(sp1);
```

After the move:

```text
sp1 → nullptr

sp2 → object
```

This avoids creating an additional owner and allows efficient transfer of resources.

---

## 🧪 Testing

The `main()` function tests two important scenarios.

### 1. Basic `SharedPtr` / `WeakPtr` operations

```text
sp1 → object
wp  → same control block

wp.lock()
    ↓
sp2 → object
```

The test verifies:

* `use_count()`
* `WeakPtr::expired()`
* `WeakPtr::lock()`
* shared ownership

---

### 2. Weak pointer expiration

The program creates a temporary `SharedPtr`:

```cpp
{
    SharedPtr sp3(new int(100));
    wp = sp3;
}
```

When `sp3` leaves the scope, it is destroyed.

Therefore:

```text
shared_count = 0
```

The managed object is deleted.

However, `wp` still exists, so its control block remains alive.

Now:

```cpp
wp.expired()
```

returns:

```text
true
```

And:

```cpp
wp.lock()
```

returns an empty `SharedPtr`.

---

## ▶️ Compilation

Compile using `g++`:

```bash
g++ -std=c++11 main.cpp -o smart_pointers
```

Run:

```bash
./smart_pointers
```

---

## 📚 Concepts Practiced

This project focuses on several important C++ concepts:

* RAII
* Dynamic memory management
* Pointers
* Constructors and destructors
* Copy constructor
* Copy assignment operator
* Move constructor
* Move assignment operator
* `std::move`
* `nullptr`
* `const`
* `noexcept`
* Operator overloading
* Reference counting
* Resource ownership
* Control blocks
* Shared ownership
* Weak references
* Object lifetime

---

## 🎯 Purpose

This project was created as a learning exercise to understand what happens behind the scenes when using C++ smart pointers.

Instead of simply using:

```cpp
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

the project implements the core concepts manually.

The implementation is educational and intentionally simplified compared to the production-quality implementations provided by the C++ Standard Library.

---

## ⚠️ Disclaimer

This project is **for educational purposes**.

It is not intended to replace:

```cpp
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

in real-world applications.

The standard library implementations provide significantly more functionality, optimizations, and safety guarantees.

---

## 👨‍💻 Author

Created as a C++ learning project focused on:

**Memory Management • RAII • Smart Pointers • Move Semantics • Reference Counting**
