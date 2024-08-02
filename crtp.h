
template<typename Derived>
class Base {
public:
    Derived& get() {
        return static_cast<Derived>(*this);
    }

    int getValue() {
        return get().getValue();
    }
private:
    Base(){}
    friend Derived;
};

class Derived: Base<Derived> {
    int getValue() {

    }
};

template<typename T>
void do_something(Base<T> &b) {
    b.getValue();
}