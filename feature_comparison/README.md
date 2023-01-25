# Feature comparison

The idea is to list a set of features that are comparable/similar in both languages. This could be especially helpful for C++ developers that are learning Rust (like me). In some ways, this file is my memory help about the relationship between C++ and Rust.

Please note that this is a work in progress. 

## Containers / Collections

C++ | Rust 
--  | -- 
[std::string](https://en.cppreference.com/w/cpp/string/basic_string) | [std::string::String](https://doc.rust-lang.org/std/string/struct.String.html) 
[std::vector](https://en.cppreference.com/w/cpp/container/vector) | [std::vec::Vec](https://doc.rust-lang.org/std/vec/struct.Vec.html) 
[std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) | [std::collections::HashMap](https://doc.rust-lang.org/std/collections/struct.HashMap.html)
[std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) | [Primitive type tuple](https://doc.rust-lang.org/std/primitive.tuple.html) e.g. (type1, type2, ..., typeN) or [Tuple structs](https://doc.rust-lang.org/stable/std/keyword.struct.html)

## Classic collection operations (C++ new std::range operations are left out)

Generic name | C++ | Rust
-- | --  | -- 
Map | [std::transform](https://en.cppreference.com/w/cpp/algorithm/transform) | [std::iter::Iterator::map](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.map)
Filter | [std::copy_if](https://en.cppreference.com/w/cpp/algorithm/copy) | [std::iter::Iterator::filter](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.filter)
Reduce | [std::accumulate](https://en.cppreference.com/w/cpp/algorithm/accumulate) or [std::reduce](https://en.cppreference.com/w/cpp/algorithm/reduce) | [std::iter::Iterator::reduce](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.reduce)

## Raw strings

C++ | Rust
--  | -- 
const char* | &str

## Raw arrays

C++ | Rust
--  | -- 
int a = [1, 2, 3]; <br>[std::array](https://en.cppreference.com/w/cpp/container/array)<int, 3> a = { 1, 2, 3 }; | let a = [1, 2, 3];

## Slices

C++ | Rust
--  | -- 
[std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) | [&str](https://doc.rust-lang.org/std/primitive.str.html)
[std::span](https://en.cppreference.com/w/cpp/container/span) | [&[]](https://doc.rust-lang.org/std/primitive.slice.html)

## Objects

C++ | Rust
--  | -- 
constructor | By convention, a method named "new" (but can be anything) without the _self_ parameter (a sort of static method in C++)
destructor  | Trait [std::ops::Drop](https://doc.rust-lang.org/std/ops/trait.Drop.html)

## Smart pointers

C++ | Rust
--  | -- 
[std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) | [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)
[std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) | [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) or [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html)
[std::weak_ptr](https://en.cppreference.com/w/cpp/memory/weak_ptr) | [std::rc::Weak](https://doc.rust-lang.org/std/rc/struct.Weak.html)
