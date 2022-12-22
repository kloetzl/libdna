use super::create_and_overwrite;
use std::os::raw::c_char;

#[link(name = "dna")]
extern "C" {
    fn dnax_revcomp(
        table: *const c_char,
        begin: *const c_char,
        end: *const c_char,
        out: *mut c_char,
    ) -> *mut c_char;
}

pub fn revcomp(table: &[i8; 256], forward: &str) -> String {
    let size = forward.len();
    let begin = forward.as_ptr() as *const c_char;
    let end = unsafe { begin.offset(size as isize) };
    let table_ptr = table.as_ptr() as *const c_char;
    create_and_overwrite(size, |ptr| {
        let dest = ptr as *mut c_char;
        unsafe {
            let dest_end = dnax_revcomp(table_ptr, begin, end, dest);
            dest_end.offset_from(dest) as usize
        }
    })
}
