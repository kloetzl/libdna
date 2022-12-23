use super::*;
use std::os::raw::c_char;

#[link(name = "dna")]
extern "C" {
    fn dna4_revcomp(begin: *const c_char, end: *const c_char, out: *mut c_char) -> *mut c_char;
    fn dna4_count_mismatches(
        begin: *const c_char,
        end: *const c_char,
        other: *const c_char,
    ) -> usize;
    fn dna4_count_mismatches_rc(
        begin: *const c_char,
        end: *const c_char,
        other: *const c_char,
    ) -> usize;
    fn dna4_fill_random(dest: *mut c_char, end: *mut c_char, seed: u32);
    fn dna4_pack_2bits(begin: *const c_char, k: usize) -> u64;
    fn dna4_unpack_2bits(begin: *mut c_char, k: usize, packed: u64);
}

pub fn revcomp(forward: &str) -> String {
    let (begin, end) = to_ptrs(forward);
    create_string_and_overwrite(forward.len(), |dest| unsafe {
        dna4_revcomp(begin, end, dest)
    })
}

pub fn count_mismatches(forward: &str, other: &str) -> usize {
    let (begin, end) = to_ptrs(forward);
    let (other, _) = to_ptrs(other);
    unsafe { dna4_count_mismatches(begin, end, other) }
}

pub fn count_mismatches_rc(forward: &str, other: &str) -> usize {
    let (begin, end) = to_ptrs(forward);
    let (other, _) = to_ptrs(other);
    unsafe { dna4_count_mismatches_rc(begin, end, other) }
}

pub fn random(size: usize, seed: u32) -> String {
    create_string_and_overwrite(size, |dest| unsafe {
        let dest_end = dest.offset(size as isize);
        dna4_fill_random(dest, dest_end, seed);
        dest_end
    })
}

pub fn pack_2bits(forward: &str) -> u64 {
    assert!(forward.len() <= 32);
    let (begin, _) = to_ptrs(forward);
    unsafe { dna4_pack_2bits(begin, forward.len()) }
}

pub fn unpack_2bits(k: usize, packed: u64) -> String {
    assert!(k <= 32);
    create_string_and_overwrite(k, |dest| unsafe {
        dna4_unpack_2bits(dest, k, packed);
        dest.offset(k as isize)
    })
}

#[cfg(test)]
#[allow(non_snake_case)]
mod tests {
    use super::*;

    #[test]
    fn Trevcomp() {
        let forward = "ACGT";
        assert_eq!(forward, revcomp(&forward));
    }

    #[test]
    fn Tcount_mismatches() {
        let s1 = "ACGTACGTACGT";
        let s2 = "ACGTTCGTACGA";
        assert_eq!(count_mismatches(&s1, &s2), 2);
    }

    #[test]
    fn Trandom() {
        let tiny = random(6, 23);
        let small = random(26, 23);
        let large = random(10000, 23);

        assert_eq!(tiny, small[..6]);
        assert_eq!(small, large[..26]);

        let unrelated = random(10000, 24);
        assert!(large != unrelated);
    }

    #[test]
    fn pack() {
        let forward = "ACGTACGTTTCC";
        let packed = pack_2bits(forward);

        assert_eq!(packed, 0x1b1bf5);

        let unpacked = unpack_2bits(forward.len(), packed);
        assert_eq!(unpacked, forward);
    }
}
