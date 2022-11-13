extern crate criterion;
extern crate libdna;

use crate::libdna::native;
use crate::libdna::libdna_ffi;
use criterion::{criterion_group, criterion_main, Criterion};

const LENGTH: i32 = 1_000_000;

fn repeat(pattern: &str, k: i32) -> String {
    let mut s = pattern.to_string();
    for _ in 0..k {
        s.push_str(pattern);
    }
    s
}

fn criterion_benchmark(c: &mut Criterion) {
    let forward = repeat("ACGT", 1000000);
    c.bench_function("native", |b| b.iter(|| native::revcomp(&forward)));
    c.bench_function("ffi", |b| b.iter(|| libdna_ffi::dna::revcomp(&forward)));
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
