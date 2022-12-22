#[link(name = "dna")]
extern "C" {
    fn dna_version() -> ::std::os::raw::c_int;
    fn dna4_revcomp(begin: *const u8, end: *const u8, out: *mut u8) -> *mut u8;
    // fn dna_ihash(key: u64) -> u64;
    // fn dna_ihash_invert(key: u64) -> u64;
    fn dna4_count_mismatches(
        begin: *const ::std::os::raw::c_char,
        end: *const ::std::os::raw::c_char,
        other: *const ::std::os::raw::c_char,
    ) -> usize;
    fn dna4_fill_random(
        dest: *mut ::std::os::raw::c_char,
        end: *mut ::std::os::raw::c_char,
        seed: u32,
    );
}

#[inline]
pub fn version() -> i32 {
    unsafe { dna_version() }
}

#[inline]
fn create_and_overwrite<F>(capacity: usize, filler: F) -> String
where
    F: Fn(*mut u8) -> usize,
{
    let mut buffer = Vec::<u8>::with_capacity(capacity); // uninitalized
    let dest = buffer.as_mut_ptr();
    let up_to = filler(dest);
    unsafe {
        buffer.set_len(up_to);
        String::from_utf8_unchecked(buffer)
    }
}

pub fn revcomp2(forward: &str) -> String {
    let size = forward.len();
    let begin = forward.as_ptr();
    let end = unsafe { begin.offset(size as isize) };
    create_and_overwrite(size, |ptr| {
        unsafe {
            dna4_revcomp(begin, end, ptr);
        }
        size
    })
}

#[inline]
pub fn revcomp(forward: &str) -> String {
    let size = forward.len();
    let begin = forward.as_ptr();
    let mut buffer = Vec::<u8>::with_capacity(size); // uninitalized
    let dest = buffer.as_mut_ptr();
    unsafe {
        let end = begin.offset(size as isize);
        dna4_revcomp(begin, end, dest);
        buffer.set_len(size);
        String::from_utf8_unchecked(buffer)
    }
}

#[test]
fn Trevcomp() {
    let forward = "ACGT";
    assert_eq!(forward, revcomp(&forward));
}

pub fn count_mismatches(forward: &str, other: &str) -> usize {
    let begin = forward.as_ptr() as *const ::std::os::raw::c_char;
    let other = other.as_ptr() as *const ::std::os::raw::c_char;
    unsafe {
        let end = begin.offset(forward.len() as isize) as *const ::std::os::raw::c_char;
        dna4_count_mismatches(begin, end, other)
    }
}

#[test]
fn Tcount_mismatches() {
    let s1 = "ACGTACGTACGT";
    let s2 = "ACGTTCGTACGA";
    assert_eq!(count_mismatches(&s1, &s2), 2);
}

pub fn random(size: usize, seed: u32) -> String {
    create_and_overwrite(size, |ptr| {
        let dest = ptr as *mut i8;
        unsafe {
            dna4_fill_random(dest, dest.offset(size as isize), seed);
        }
        size
    })
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
