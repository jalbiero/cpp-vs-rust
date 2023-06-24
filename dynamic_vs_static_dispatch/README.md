# C++ vs Rust 

- [C++ vs Rust](#c-vs-rust)
  - [Dynamic and static dispatch](#dynamic-and-static-dispatch)
    - [Part 1: Dynamic dispatch](#part-1-dynamic-dispatch)
      - [Example](#example)
        - [C++](#c)
        - [Rust](#rust)
      - [Dispatch](#dispatch)
        - [C++:](#c-1)
        - [Rust](#rust-1)
      - [Key points](#key-points)
    - [Part 2: Static dispatch](#part-2-static-dispatch)
      - [Example](#example-1)
      - [Dispatch](#dispatch-1)
        - [Rust](#rust-2)
        - [C++](#c-2)
      - [Key points](#key-points-1)

## Dynamic and static dispatch

The idea is to show how C++ and Rust implement both types of dispatching.

### Part 1: Dynamic dispatch

Quoting [wikipedia](https://en.wikipedia.org/wiki/Dynamic_dispatch), "dynamic dispatch is the process of selecting which implementation of a polymorphic operation (method or function) to call at run time."

One way to implement it, is through a [vtable](https://en.wikipedia.org/wiki/Virtual_method_table). In C++, a vtable is automatically created for a class/struct when a virtual method is declared. A vtable consumes extra space, and a call dispatched through it, is slower compared to a non virtual call. That is why we often want static polymorphism (which is not applicable to all scenarios). In Rust the vtable is not associated to the interface (trait), but to the pointer that carries the object (a [fat pointer](https://stackoverflow.com/questions/57754901/what-is-a-fat-pointer)), that is why we can use the same trait for dynamic or static polymorphism. 

#### Example
I will use a simple and contrived example, a calculator with 2 operations (addition and multiplication):

##### C++ 

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

##### Rust 

In Rust "interfaces" are defined via Traits (it is an equivalent concept to an abstract base class, but not exactly the same) 

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

#### Dispatch
Now let's do the dynamic dispatch:

##### C++:

```C++
void do_the_math_dynamically(const Operation& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << "Dynamic dispatch: " << a << op.name() << b << " = " << result << '\n';
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
    println!("Dynamic dispatch: {}{}{} = {}", a, op.name(), b, result);
}

fn main() {
    do_the_math_dynamically(&Add{}, 1.0, 2.0);
    do_the_math_dynamically(&Mul{}, 4.0, 5.0);
}
```

#### Key points

- C++: Use the `virtual` keyword when defining a method in a class/struct. 
- Rust: Use the `dyn` keyword at the moment of annotating the type of a variable with a trait (see also [trait objects](https://doc.rust-lang.org/reference/types/trait-object.html#trait-objects)).


### Part 2: Static dispatch

Contrary to dynamic dispatch, a static dispatch does not require a vtable or a fat pointer, the call is straight forward because the target (method or function) is selected at compile time. Also, the compiler performs a type validation to ensure the involved types are compatible (*). 

In C++, the dispatch is implemented with templates, and, in recent version, with the help of [concepts][1]. In Rust the implementation is done with Traits (as in dynamic dispatch). 

(*) In versions of C++ prior to 20, this is often referred as [duck typing](https://en.wikipedia.org/wiki/Duck_typing) (do not confuse with dynamic duck typing like in Python, in C++ this is purely static) because the compiler does not check the involved types. See the C++ implementation later in this section for an example and a deeper explanation.

#### Example

In order to make a comparisson between dynamic and static dispatch, the same previous example will be re used.

#### Dispatch

##### Rust
I will start with Rust because we do not need extra definitions (the `Operation` trait remains the same), so we can just code the dispatching with minor changes:

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

Rust will perform a static dispatch in the generic function `do_the_math_statically` with any type that implements the `Operation` trait. This is validated at compile time, no fat pointers/vtable are needed (note the absence of `dyn` keyword)

##### C++

In 'old' C++, a static dispatch is done just with templates: 

```C++
template<typename T>
void do_the_math_statically(const T& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << "Static dispatch (old style): " << a << op.name() << b << " = " << result << '\n';
}
```

The problem with this approach is that `T` can be of any type, even those that do not have a `calculate` or `name` methods can be used. The compiler will emit a bunch of awful errors if that happens. In order to solve this (help the compiler to narrow the allowed types and emit useful error messages) we need [concepts][1], a C++20 feature. Also, with static dispatch, the type does not need to be polymorphic (no virtual functions). So the C++ equivalent to Rust traits (for static dispatching) will be the following:

```C++
// This is the static replacement for 'struct Operation'
template<typename T>
concept OperationConcept = requires(T const t) {
    { t.calculate(double{}, double{}) } -> std::same_as<double>;
    { t.name()                        } -> std::same_as<std::string>;
};

// Non polymorphic types, no need for inheritance and virtual functions:

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

Note that we do not need an abstract base class (`Operation`). The static equivalent of it is the concept `OperationConcept`. That is why any class that have `calculate` and `name` methods (with the same signature) can be used (even polymorphic instances like the ones present in the dynamic section). 

Now our static dispatch will be the following:

```C++
template<OperationConcept T>
void do_the_math_statically(const T& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << "Static dispatch: " <<  a << op.name() << b << " = " << result << '\n';
}

int main() {
    do_the_math_statically(NonPolyAdd{}, 1.0, 2.0);
    do_the_math_statically(NonPolyMul{}, 4.0, 5.0);
}
```

#### Key points

- Rust: `dyn` keyword is not necessary, generic types are used as a replacement. Each generic type should be annotated with the desired trait.
- C++: Replace abstract base classes (interfaces) with concepts. Use them with template parameters (note that `typename` keyword was replaced by the concept name, `OperationConcept`). Inheritance and virtual functions are not necessary. **Note**: See the provided [source code](c++/dynamic_and_static_dispatch.cpp) for alternative versions of `OperationConcept` and `do_the_math_statically` (they are commented out).


[1]: https://en.wikipedia.org/wiki/Concepts_(C%2B%2B)
