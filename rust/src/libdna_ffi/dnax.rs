#[link(name = "dna")]
extern "C" {
    fn dnax_revcomp(table: *const u8, begin: *const u8, end: *const u8, out: *mut u8) -> *mut u8;
}

pub fn revcomp(table: &[u8; 256], forward: &str) -> String {
    let size = forward.len();
    let mut ret = String::with_capacity(size); // zero initalized?
    let begin = forward.as_ptr();
    let dest = ret.as_mut_ptr();
    unsafe {
        let end = begin.offset(size as isize);
        dnax_revcomp(table.as_ptr(), begin, end, dest);
    }
    ret
}
