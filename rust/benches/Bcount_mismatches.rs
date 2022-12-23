extern crate criterion;
extern crate libdna;

use crate::libdna::libdna_ffi;
use crate::libdna::native;
use criterion::{criterion_group, criterion_main, Criterion};

use std::os::raw::c_char;

const LENGTH: usize = 1_000_000;

fn mutc(c: char) -> char {
    const order: [char; 5] = ['A', 'C', 'G', 'T', 'A'];
    let index = ((c as u8 & 6) / 2 + 1) as usize;
    order[index]
}

#[cfg(test)]
fn does_mut() {
    assert!(mutc('A') != 'A');
}

fn mutate(forward: &str, stride: usize) -> String {
    use std::iter::FromIterator;

    String::from_iter(forward.bytes().enumerate().map(|(i, c)| {
        let cc = c as char;
        if i % stride == 0 {
            mutc(cc)
        } else {
            cc
        }
    }))
}

fn first_mismatch(a: &str, b: &str) -> usize {
    use std::iter::zip;

    for (i, (a, b)) in zip(a.bytes(), b.bytes()).enumerate() {
        if a != b {
            return i;
        }
    }
    a.len()
}

#[test]
fn t() {
    assert_eq!(first_mismatch("acgt", "acG"), 2);
}

#[link(name = "dna")]
extern "C" {
    fn dnax_find_first_mismatch(
        begin: *const c_char,
        end: *const c_char,
        other: *const c_char,
    ) -> *const c_char;
}

#[inline]
fn to_ptrs(forward: &str) -> (*const c_char, *const c_char) {
    let size = forward.len();
    let begin = forward.as_ptr() as *const c_char;
    let end = unsafe { begin.offset(size as isize) };
    (begin, end)
}

fn find_first_mismatch(a: &str, b: &str) -> usize {
    let (begin, end) = to_ptrs(a);
    let (other, _) = to_ptrs(b);
    let ptr = unsafe { dnax_find_first_mismatch(begin, end, other) };
    unsafe { ptr.offset_from(begin) as usize }
}

fn criterion_benchmark(c: &mut Criterion) {
    let forward = libdna_ffi::dna4::random(LENGTH, 1729);
    let other = mutate(&forward, 71);

    c.bench_function("native", |b| {
        b.iter(|| {
            let mut i = 0;
            while i < forward.len() {
                i += first_mismatch(&forward[i..], &other[i..]);
                i += 1;
            }
        })
    });

    c.bench_function("ffi", |b| {
        b.iter(|| {
            let mut i = 0;
            while i < forward.len() {
                i += find_first_mismatch(&forward[i..], &other[i..]);
                i += 1;
            }
        })
    });
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
