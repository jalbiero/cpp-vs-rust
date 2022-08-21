// Tested with Rust 1.63.0 

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

fn do_the_math_dynamically(op: &dyn Operation, a: f64, b: f64) {
    let result = op.calculate(a, b);
    println!("Dynamic dispatch: {}{}{} = {}", a, op.name(), b, result);
}

fn do_the_math_statically<T: Operation> (op: &T, a: f64, b: f64) {
    let result = op.calculate(a, b);
    println!("Static dispatch: {}{}{} = {}", a, op.name(), b, result);
}

fn main() {
    do_the_math_dynamically(&Add{}, 1.0, 2.0);
    do_the_math_dynamically(&Mul{}, 4.0, 5.0);

    do_the_math_statically(&Add{}, 1.0, 2.0);
    do_the_math_statically(&Mul{}, 4.0, 5.0);
}