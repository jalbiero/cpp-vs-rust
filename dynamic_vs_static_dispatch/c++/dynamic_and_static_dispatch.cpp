// This program requires C++20 or later

#include <concepts>
#include <iostream>
#include <string>

namespace dynamic_dispatch 
{
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
    
} // namespace dynamic_dispatch

namespace static_dispatch 
{
    // Replacement for Operation interface
    template<typename T>
    concept OperationConcept = requires(T const t) {
        { t.calculate(double{}, double{}) } -> std::same_as<double>;
        { t.name()                        } -> std::same_as<std::string>;

        // For a full signature check, the following can be used instead
        // of the previous ones (but these are harder to read)
        //  
        // { &T::calculate } -> std::convertible_to<double (T::*)(double, double) const>;
        // { &T::name      } -> std::convertible_to<std::string (T::*)() const>;
    };

    class Add { // Non polymorphic class, no virtual members
    public:
        double calculate(double a, double b) const {
            return a + b; 
        }

        std::string name() const {
            return " + ";
        }
    };

    class Mul { // Non polymorphic class, no virtual members
    public:
        double calculate(double a, double b) const {
            return a * b; 
        }

        std::string name() const {
            return " * ";
        }
    };
    
} // namespace static_dispatch


void do_the_math_dynamically(const dynamic_dispatch::Operation& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << "Dynamic dispatch: " << a << op.name() << b << " = " << result << '\n';
}

template<static_dispatch::OperationConcept T>
void do_the_math_statically(const T& op, double a, double b) {
    auto result = op.calculate(a, b);
    std::cout << "Static dispatch: " <<  a << op.name() << b << " = " << result << '\n';
}

// Old style static dispatch, no concepts
// template<typename T>
// void do_the_math_statically(const T& op, double a, double b) {
//     auto result = op.calculate(a, b);
//     std::cout << "Static dispatch (old style): " << a << op.name() << b << " = " << result << '\n';
// }

int main() {
    do_the_math_dynamically(dynamic_dispatch::Add{}, 1.0, 2.0);
    do_the_math_dynamically(dynamic_dispatch::Mul{}, 4.0, 5.0); 

    do_the_math_statically(static_dispatch::Add{}, 1.0, 2.0);
    do_the_math_statically(static_dispatch::Mul{}, 4.0, 5.0);
}
