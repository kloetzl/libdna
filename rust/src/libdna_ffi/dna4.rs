use std::os::raw::c_char;

#[link(name = "dna")]
extern "C" {
    fn dna4_revcomp(begin: *const c_char, end: *const c_char, out: *mut c_char) -> *mut c_char;
    fn dna4_count_mismatches(
        begin: *const c_char,
        end: *const c_char,
        other: *const c_char,
    ) -> usize;
    fn dna4_fill_random(dest: *mut c_char, end: *mut c_char, seed: u32);
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

pub fn revcomp(forward: &str) -> String {
    let size = forward.len();
    let begin = forward.as_ptr() as *const c_char;
    let end = unsafe { begin.offset(size as isize) };
    create_and_overwrite(size, |ptr| {
        let dest = ptr as *mut c_char;
        unsafe {
            dna4_revcomp(begin, end, dest);
        }
        size
    })
}

pub fn count_mismatches(forward: &str, other: &str) -> usize {
    let begin = forward.as_ptr() as *const c_char;
    let other = other.as_ptr() as *const c_char;
    unsafe {
        let end = begin.offset(forward.len() as isize) as *const c_char;
        dna4_count_mismatches(begin, end, other)
    }
}

pub fn random(size: usize, seed: u32) -> String {
    create_and_overwrite(size, |ptr| {
        let dest = ptr as *mut c_char;
        unsafe {
            dna4_fill_random(dest, dest.offset(size as isize), seed);
        }
        size
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
}
