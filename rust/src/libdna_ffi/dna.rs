#[link(name = "dna")]
extern "C" {
    fn dna_version() -> ::std::os::raw::c_int;
    fn dna_ihash(key: u64) -> u64;
    fn dna_ihash_invert(key: u64) -> u64;
}

#[inline]
pub fn version() -> i32 {
    unsafe { dna_version() }
}

pub fn ihash(key: u64) -> u64 {
    unsafe { dna_ihash(key) }
}

pub fn ihash_invert(key: u64) -> u64 {
    unsafe { dna_ihash_invert(key) }
}
