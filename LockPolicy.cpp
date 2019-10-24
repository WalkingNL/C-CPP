#include <mutex>
 
    template<
        typename T,
        template<typename> typename AcquirePolicy,
        template<typename> typename ReleasePolicy
    >
    class RAII
    {
    public:
        typedef T  val_type;
        typedef T& ref_type;
    
    RAII(val_type h) : m_handle(h) { AcquirePolicy<T>::Execute(m_handle); }
    RAII(const RAII&) = delete;
    RAII(RAII&&) = delete;
    RAII& operator = (const RAII&) = delete;
    ~RAII() { ReleasePolicy<T>::Execute(m_handle); }
    
    constexpr operator ref_type () { return m_handle; }
    constexpr operator ref_type () const { return m_handle; }
    
    private:
        val_type m_handle;
    };

    template<typename T> struct LockPolicy { static void Execute(T t) { t.lock(); } };
    template<typename T> struct UnlockPolicy { static void Execute(T t) { t.unlock(); } };
    template<typename T> using scope_lock = RAII<T&, LockPolicy, UnlockPolicy>;

    template<typename T> struct NoOpPolicy { static void Execute(T) {} };

    template<typename T> struct PointerReleasePolicy { static void Execute(T ptr) { delete ptr; } };
    template<typename T> struct ArrayReleasePolicy { static void Execute(T ptr) { delete[] ptr; } };

    template<typename T> using arr_ptr_handle_t = RAII<T*, NoOpPolicy, ArrayReleasePolicy>;
    template<typename T> using ptr_handle_t = RAII<T*, NoOpPolicy, PointerReleasePolicy>;

    int main(int argc, char** argv)
    {

        std::mutex m;
        scope_lock<std::mutex> lock(m);

        ptr_handle_t<int> p1 = new int;
        arr_ptr_handle_t<int> p2 = new int [2];

        *p1 = 0xDEADBEEF;
        p2[1] = 0x8BADF00D;

        return 1;
    }
