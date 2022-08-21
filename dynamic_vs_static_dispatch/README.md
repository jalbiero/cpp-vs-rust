# C++ vs Rust 

## Dynamic and static dispatch

The idea of this document is to show how the different types of dispatches are implemented in C++ and Rust.

### Part 1: Dynamic dispatch

Quoting [wikipedia](https://en.wikipedia.org/wiki/Dynamic_dispatch), "dynamic dispatch is the process of selecting which implementation of a polymorphic operation (method or function) to call at run time."

One way to perform a dynamic dispatch is to use a [vtable](https://en.wikipedia.org/wiki/Virtual_method_table). In C++, a vtable is automatically created for a class/struct when a virtual method is declared. A vtable consumes extra space, and a call dispatched through it, is slower compared to a non virtual call. That is why we often want static polymorphism (which is not applicable to all scenarios). In Rust the vtable is not associated to the interface (trait), but to the pointer that carries the object (a [fat pointer](https://stackoverflow.com/questions/57754901/what-is-a-fat-pointer)), that is why we can use the same trait for dynamic or static polymorphism. 

I will use a simple and contrived example, a calculator with 2 operations (addition and multiplication):

#### C++ 

```C++
struct Operation {
    virtual double calculate(double a, double b) const = 0;
    virtual std::string name() const = 0;

    virtual ~Operation() {}
};

class Add: public Operation {
public:
    double calculate(double a, double b) const override {
        return a + b; 
    }

    std::string name() const override {
        return " + ";
    }
};

class Mul: public Operation {
public:
    double calculate(double a, double b) const override {
        return a * b; 
    }

    std::string name() const override {
        return " * ";
    }
};
```

#### Rust 

In rust "interfaces" are defined via Traits (it is an equivalent concept to an abstract base class, but not exactly the same) 

```rust
trait Operation {
    fn calculate(&self, a: f64, b: f64) -> f64;
    fn name(&self) -> String;
}

struct Add;
struct Mul;

impl Operation for Add {
    fn calculate(&self, a: f64, b: f64) -> f64 {
        a + b
    }

    fn name(&self) -> String {
        String::from(" + ")
    }
}

impl Operation for Mul {
    fn calculate(&self, a: f64, b: f64) -> f64 {
        a * b
    }

    fn name(&self) -> String {
        String::from(" * ")
    }
}
```

#### Dispatching
Now let's do the dynamic dispatch:

##### C++:

```C++
void do_the_math_dynamically(const Operation& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << a << op.name() << b << " = " << result << '\n';
}

int main() {
    do_the_math_dynamically(Add{}, 1.0, 2.0);
    do_the_math_dynamically(Mul{}, 4.0, 5.0); 
}
```

##### Rust

```rust
fn do_the_math_dynamically(op: &dyn Operation, a: f64, b: f64) {
    let result = op.calculate(a, b);
    println!("{}{}{} = {}", a, op.name(), b, result);
}

fn main() {
    do_the_math_dynamically(&Add{}, 1.0, 2.0);
    do_the_math_dynamically(&Mul{}, 4.0, 5.0);
}
```

#### Key points

- In C++ the key point for dynamic dispatching is the `virtual` keyword when defining a method in a class/struct. 

- On the other side, in Rust the key point is the `dyn` keyword at the moment of annotating the type of a variable.


### Part 2: Static dispatch

On the contrary of dynamic dispatch, a static dispatch does not require a vtable or a fat pointer, the call is straight forward. The compiler performs type validation to ensure that types are compatible. 

In versions of C++ prior to 20, this is often referred as [duck typing](https://en.wikipedia.org/wiki/Duck_typing) (do not confuse with dynamic duck typing like in Python, in C++ this is purely static). In C++, the dispatch is implemented with templates (with the help of [concepts](https://en.wikipedia.org/wiki/Concepts_(C%2B%2B))). In Rust with Traits (as in dynamic dispatch). 

#### Rust
In this section I will start with Rust because we do not need extra definitions (the `Operation` trait remains the same), so we can just do the dispatching with minor changes:

```rust
fn do_the_math_statically<T: Operation> (op: &T, a: f64, b: f64) {
    let result = op.calculate(a, b);
    println!("{}{}{} = {}", a, op.name(), b, result);
}

fn main() {
    do_the_math_statically(&Add{}, 1.0, 2.0);
    do_the_math_statically(&Mul{}, 4.0, 5.0);
}
```

Rust will perform a static dispatch in the generic function `do_the_math_statically` with any type that implements the `Operation` trait. This is validated at compile time, no fat pointers are needed (note the absence of `dyn` keyword)

#### C++

In 'old' C++, a static dispatch is done with templates: 

```C++
template<typename T>
void do_the_math_statically(const T& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << a << op.name() << b << " = " << result << '\n';
}
```

The problem with this approach is that `T` can be of any type, even those that do not have a `calculate` or `name` methods. The compiler will emit a bunch of awful errors if that happens. In order to solve this (help the compiler to narrow the allowed types) we need [concepts](https://en.wikipedia.org/wiki/Concepts_(C%2B%2B)) (a C++20 feature). Also with static dispatch, the type do not need to be polymorphic (no virtual functions). So, the C++ equivalent to Rust traits (for static dispatching) will be the following:

```C++
template<typename T>
concept OperationConcept = requires(T t) {
    { t.calculate(double{}, double{}) } -> std::same_as<double>;
    { t.name()                        } -> std::same_as<std::string>;
};

class NonPolyAdd {
public:
    double calculate(double a, double b) const {
        return a + b; 
    }

    std::string name() const {
        return " + ";
    }
};

class NonPolyMul {
public:
    double calculate(double a, double b) const {
        return a * b; 
    }

    std::string name() const {
        return " * ";
    }
};
```

Note that we do not need a base abstract class (`Operation`). The static equivalent to it is the concept (`OperationConcept`). That is why any class that have `calculate` and `name` methods (with the same signature) can be used (even polymorphic instances like the one present in the dynamic section).

Now our static dispatch will be the following:

```C++
template<OperationConcept T>
void do_the_math_statically(const T& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << a << op.name() << b << " = " << result << '\n';
}

int main() {
    do_the_math_statically(NonPolyAdd{}, 1.0, 2.0);
    do_the_math_statically(NonPolyMul{}, 4.0, 5.0);
}
```

#### Key points

- The Rust key point for static dispatching is not to use the `dyn` keyword, but use generic types as a replacement. Each generic type should be annotated with the desired trait. 

- In C++ the key point is to replace the base abstract class (interface) with a concept, which turns unnecessary the virtual keyword (and the resulting vtable generation).
