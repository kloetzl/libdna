extern crate criterion;
extern crate libdna;
#[macro_use]
extern crate lazy_static;

use crate::libdna::libdna_ffi;
use crate::libdna::native;
use criterion::{criterion_group, criterion_main, Criterion};
use std::borrow::Borrow;

// use odd length to mess with alignment
const LENGTH: usize = 1_000_009;

lazy_static! {
    static ref COMPLEMENT: [u8; 256] = {
        let mut comp = [0; 256];
        for (v, a) in comp.iter_mut().enumerate() {
            *a = v as u8;
        }
        for (&a, &b) in b"AGCTYRWSKMDVHBN".iter().zip(b"TCGARYWSMKHBDVN".iter()) {
            comp[a as usize] = b;
            comp[a as usize + 32] = b + 32;  // lowercase variants
        }
        comp
    };

    static ref ICOMPLEMENT: [i8; 256] = {
        let mut comp = [0i8; 256];
        for index in 0..255 {
            comp[index] = COMPLEMENT[index] as i8
        }
        comp
    };
}

#[inline]
fn complement(a: u8) -> u8 {
    COMPLEMENT[a as usize]
}

fn revcomp<C, T>(text: T) -> Vec<u8>
where
    C: Borrow<u8>,
    T: IntoIterator<Item = C>,
    T::IntoIter: DoubleEndedIterator,
{
    text.into_iter()
        .rev()
        .map(|a| complement(*a.borrow()))
        .collect()
}

fn criterion_benchmark(c: &mut Criterion) {
    let forward = libdna_ffi::dna4::random(LENGTH, 1729);

    c.bench_function("native", |b| b.iter(|| native::revcomp(&forward)));
    c.bench_function("dna4_revcomp", |b| {
        b.iter(|| libdna_ffi::dna4::revcomp(&forward))
    });
    c.bench_function("dnax_revcomp", |b| {
        b.iter(|| libdna_ffi::dnax::revcomp(&ICOMPLEMENT, &forward))
    });
    c.bench_function("rust-bio", |b| {
        b.iter(|| bio::alphabets::dna::revcomp(forward.bytes()))
    });
    c.bench_function("rust-bio inlined", |b| b.iter(|| revcomp(forward.bytes())));
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
