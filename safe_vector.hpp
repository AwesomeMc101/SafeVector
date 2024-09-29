#include <initializer_list>

template<typename T> class SafeVector {
private:
    T* _data;
    int sz = 0;
    int r_sz = 0;
public:

    /* Set vector equal to arr */
    void operator =(std::initializer_list<T> n) {
        _data = (T*)malloc(sizeof(T) * n.size());
        for (int i = 0; i < n.size(); i++) {
            _data[i] = n.begin()[i];
        }
        sz = n.size();
    }

    /* Index */
    T operator[](unsigned int idx) {
        if (idx < sz) {
            return _data[idx];
        }

        if (sz) { return back(); }
        return *(new T);
    }

    /* Compare */
    bool operator==(SafeVector o_s) {
        if (o_s.size() != sz) { return false; }
        for (int n = 0; n < sz; n++) {
            if (o_s[n] != _data[n]) { return false; }
        }
        return true;
    }

    //Insert value v at location l
    void insert(T v, size_t l) {
        if (l > sz) { return; }
        T* nd = (T*)malloc(sizeof(T) * (sz + 1)); /* Alloc new array */
        memcpy(nd, _data, sizeof(T) * l); //Memcpy up to location l
        nd[l] = v; //Location l value set to inserted val
        memcpy((nd + l + 1), (_data + l), sizeof(T) * (sz - l)); //Copy starting at l+1, incrementing _data to +l, and size of size-l(as l size has already been written)

        free(_data); //Free data
        _data = nd; //change ptr
        sz++; //increment sz
    }
    //Insert array of values at location l
    void insert(std::initializer_list<T> il, size_t l) {
        if (l > sz) { return; }
        T* nd = (T*)malloc(sizeof(T) * (sz + il.size())); /* Alloc new array */
        memcpy(nd, _data, sizeof(T) * l); /* Copy data from BEFORE the insertion index over */
        memcpy(nd + l, il.begin(), sizeof(T) * il.size()); /* Copy inserted arr data in */
        memcpy(nd + l + il.size(), _data + l, sizeof(T) * (sz - l)); /* Copy data from AFTER insertion index over */

        free(_data); /* Free old allocated memory */
        _data = nd; /* Make data point to new array */
        sz += il.size(); /* Add sizeof list to sz */
    }

    //Erase amount len at location l
    void erase(size_t l, size_t len = 1) {
        T* nd = static_cast<T*>(malloc(sizeof(T) * (sz - len))); //Alloc new array with new calculated smaller size
        memcpy(nd, _data, sizeof(T) * l); //Copy old data up to location l
        memcpy(nd + l, _data + l + len, sizeof(T) * (sz - len - l)); /*Copy new data to nd + 1, starting from point of _data ptr incremented by l(already copied data) + len(data to skip),
                                                                     by size of size - len (we have *len* less data) - l (size of already copied data).*/

        free(_data);
        _data = nd;
        sz -= len;
    }

    //Erase final value
    void pop_back() {
        if (!sz) { return; } //Cannot pop back empty vector
        T* nd = static_cast<T*>(malloc(sizeof(T) * (sz - 1))); //Alloc new array with 1 less
        memcpy(nd, _data, sizeof(T) * (sz - 1)); //Copy old data over, excluding arr[sz-1]
        free(_data); //Free old arr
        _data = nd; //Change ptr
        sz--; //Decrement size
    }

    /* Push back with option for reserved memory. */
    void push_back(T v) {
        if (r_sz > sz) { //If reserve memory is available, just write,
            _data[sz] = v; //sz is a valid indice if we have excess reserved memory. Notice, the binary op is >, not >=
            sz++; //inc true sz.
            return; //Return, skip realloc
        }
        T* nd = (T*)malloc(sizeof(T) * (sz + 1)); /* Alloc new array */
        memcpy(nd, _data, (sz) * sizeof(T)); /* Copy new array over */
        nd[sz] = v; /* Set highest index to v. sz has not been incremented yet */
        free(_data); /* Free old array */
        _data = nd; /* Change data ptr */
        sz++; /* Inc size */
    }
    void emplace_back(T& v) {
        if (r_sz > sz) { //If reserve memory is available, just write,
            _data[sz] = v; //sz is a valid indice if we have excess reserved memory. Notice, the binary op is >, not >=
            sz++; //inc true sz.
            return; //Return, skip realloc
        }
        T* nd = (T*)malloc(sizeof(T) * (sz + 1)); /* Alloc new array */
        memcpy(nd, _data, (sz) * sizeof(T)); /* Copy new array over */
        nd[sz] = v; /* Set highest index to v. sz has not been incremented yet */
        free(_data); /* Free old array */
        _data = nd; /* Change data ptr */
        sz++; /* Inc size */
    }

    /* Reserve memory for push_back. */
    void reserve(unsigned long long n_s) { /* Reserve new full size. Not just the amount you plan to add. */
        if (n_s < sz) { return; }
        r_sz = n_s;
        T* nd = static_cast<T*>(malloc(sizeof(T) * n_s));
        memcpy(nd, _data, sizeof(T) * sz);

        free(_data);
        _data = nd;
    }

    /* Resize (no values) */
    void resize(unsigned long long __sz) { //No value change. Other than copied data, the excess size (if any) will be uninitialized data.
        T* nd = static_cast<T*>(malloc(sizeof(T) * __sz)); //New resized array 
        memcpy(nd, _data, sizeof(T) * (__sz > sz ? sz : __sz)); //Copy old data. Ternary operator ensures that if we resize to a smaller size, we only copy by the new size.

        free(_data);
        _data = nd;
        sz = __sz;
    }
    /* Resize (given value, optional overwrite) */
    void resize(unsigned long long __sz, T value, bool overwrite = false) {
        T* nd = static_cast<T*>(malloc(sizeof(T) * __sz)); //Alloc new array with new size

        if (!overwrite) {
            memcpy(nd, _data, sizeof(T) * (__sz > sz ? sz : __sz)); //Copy old data. Ternary operator ensures that if we resize to a smaller size, we only copy by the new size.
        }

        if (sz < __sz) { //Ensure we increased size. 
            for (int n = (overwrite ? 0 : sz); n < __sz; n++) { //Loop through the rest of the values. If overwrite is on, rewrite everything.
                nd[n] = value;
            }
        }

        free(_data); //Free old arr
        _data = nd; //Change ptr
        sz = __sz; //Change size
    }

    /* Legit just clear the vector. */
    void clear() {
        free(_data);
        sz = 0;
    }

    /* Resize without allocated/reserve memory. */
    void shrink_to_fit() {
        T* nd = static_cast<T*>(malloc(sizeof(T) * sz));
        memcpy(nd, _data, sizeof(T) * sz);
        free(_data);
        _data = nd;
        r_sz = sz;
    }

    size_t size() {
        return sz;
    }
    size_t reserved() {
        return r_sz;
    }

    T back() {
        return _data[sz - 1];
    }
    T front() {
        return (sz ? _data[0] : *(new T));
    }
    T* data() {
        return _data;
    }
};
