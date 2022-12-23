use super::*;
use std::os::raw::c_char;

#[link(name = "dna")]
extern "C" {
    fn dnax_revcomp(
        table: *const c_char,
        begin: *const c_char,
        end: *const c_char,
        out: *mut c_char,
    ) -> *mut c_char;
    fn dnax_translate(begin: *const c_char, end: *const c_char, dest: *mut c_char) -> *mut c_char;
    fn dnax_extract_dna4(
        begin: *const c_char,
        end: *const c_char,
        dest: *mut c_char,
    ) -> *mut c_char;

    static dnax_revcomp_table: [c_char; 256usize];
}

pub fn revcomp(table: &[i8; 256], forward: &str) -> String {
    let (begin, end) = to_ptrs(forward);
    let table_ptr = table.as_ptr() as *const c_char;
    create_string_and_overwrite(forward.len(), |dest| unsafe {
        dnax_revcomp(table_ptr, begin, end, dest)
    })
}

pub fn translate(forward: &str) -> String {
    let (begin, end) = to_ptrs(forward);
    create_string_and_overwrite(forward.len(), |dest| unsafe {
        dnax_translate(begin, end, dest)
    })
}

pub fn extract_dna4(forward: &str) -> String {
    let (begin, end) = to_ptrs(forward);
    create_string_and_overwrite(forward.len(), |dest| unsafe {
        dnax_extract_dna4(begin, end, dest)
    })
}

#[cfg(test)]
#[allow(non_snake_case)]
mod tests {
    use super::*;

    #[test]
    fn Trevcomp() {
        let forward = "ACGTacgtACGT!";
        let backward = revcomp(unsafe { &dnax_revcomp_table }, forward);
        assert_eq!(backward, "ACGTacgtACGT");
    }
}
