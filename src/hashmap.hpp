// TODO: write our own hash map stuff because we want to learn how the
// hashing algorithms work.
// there's also other parts of this codebase where we use the hash maps.
// use this struct there.
//
// we could also make this version a specialization on a more generic hash map.
//

template <typename T_ValueKind>
struct PPL_HashMapWithStringKey_Element
{
    char *key;
    T_ValueKind value;
};

template <typename T_ValueKind>
struct PPL_HashMapWithStringKey;

// iterator for hash map.
template <typename T_ValueKind>
class PPL_HashMapWithStringKey_ListType {
public:
    PPL_HashMapWithStringKey_Element<T_ValueKind> operator*() { return *this->elem; }

    bool operator!=(PPL_HashMapWithStringKey_ListType &other) const { return (other.elem != this->elem); }

    // `this++`.
    PPL_HashMapWithStringKey_ListType operator++(int);

    PPL_HashMapWithStringKey_Element<T_ValueKind> *elem;
    PPL_HashMapWithStringKey<T_ValueKind>         *parent;

    PPL_HashMapWithStringKey_ListType(
        PPL_HashMapWithStringKey<T_ValueKind> *parent, PPL_HashMapWithStringKey_Element<T_ValueKind> *inval) :
        elem(inval), parent(parent)
    {}
};

// TODO: some of this hash map is untested. maybe we should verify that the endVal is good.
template <typename T_ValueKind>
struct PPL_HashMapWithStringKey
{

    PPL_HashMapWithStringKey_ListType<T_ValueKind> begin()
    {
        auto start = PPL_HashMapWithStringKey_ListType<T_ValueKind> { this, table_internal ? &table_internal[0] : nullptr};

        while( start.elem && (start.elem->key == nullptr || *start.elem->key == 0) ) start.elem += 1;
        
        return start;
    };
    
    PPL_HashMapWithStringKey_ListType<T_ValueKind>& end()
    {
        return this->endVal;
    };
    
    // return TRUE if hash map contains item at key, FALSE otherwise.
    bool get(const char *key, T_ValueKind *dst)
    {
        size_t i = stbds_shgeti(table_internal, key);
        bool bResult = i != -1;
        if (bResult)
        {
            *dst = table_internal[i].value;
        }
        return bResult;
    }
    
    size_t count() const
    {
        return stbds_shlen(table_internal);
    }

    void del(const char *key)
    {
        stbds_del(table_internal, key);
        update_end();
    }
    
    void put(const char *str, T_ValueKind value)
    {
        stbds_shput(table_internal, str, value);
        update_end();
    }

    PPL_HashMapWithStringKey_ListType<T_ValueKind> endVal = {this, nullptr};
    PPL_HashMapWithStringKey_Element<T_ValueKind> *table_internal = nullptr;
    
    ~PPL_HashMapWithStringKey()
    {
        stbds_shfree(table_internal);
    }
private:

    void update_end()
    {
        size_t end = stbds_shlen(table_internal);
        endVal = PPL_HashMapWithStringKey_ListType<T_ValueKind> {this, &table_internal[end-1] + 1};
    }
    
};

template <typename T_ValueKind>
PPL_HashMapWithStringKey_ListType<T_ValueKind> PPL_HashMapWithStringKey_ListType<T_ValueKind>::operator++(int)
{
    auto r = *this;
    
    this->elem += 1;
    
    while( true )
    {
        const bool bNotAtEnd = *this != parent->endVal;
        if (!bNotAtEnd) break;
        
        const bool bNullKey = (this->elem->key == nullptr);
        const bool bEmptyKey = *this->elem->key == 0;
        
        if (!bNullKey && !bEmptyKey)
        {
            break;
        }
        
        this->elem += 1;
    }
    
    return r;
}