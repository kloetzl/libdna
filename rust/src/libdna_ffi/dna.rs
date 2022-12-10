#[link(name = "dna")]
extern "C" {
    fn dna_version() -> i32; // int != i32?
}

#[link(name = "dna")]
extern "C" {
    fn dna4_revcomp(begin: *const u8, end: *const u8, out: *mut u8) -> *mut u8;
}

pub fn version() -> i32 {
    unsafe { dna_version() }
}

pub fn revcomp(forward: &str) -> String {
    let size = forward.len();
    let mut ret = String::with_capacity(size); // zero initalized?
    let begin = forward.as_ptr();
    let dest = ret.as_mut_ptr();
    unsafe {
        let end = begin.offset(size as isize);
        dna4_revcomp(begin, end, dest);
    }
    ret
}
