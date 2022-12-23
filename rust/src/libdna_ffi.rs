pub mod dna;
pub mod dna4;
pub mod dnax;

// mod util {}

use std::os::raw::c_char;

#[inline]
fn to_ptrs(forward: &str) -> (*const c_char, *const c_char)
{
    let size = forward.len();
    let begin = forward.as_ptr() as *const c_char;
    let end = unsafe { begin.offset(size as isize) };
    (begin, end)    
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

fn create_string_and_overwrite<F>(capacity: usize, filler: F) -> String
where
    F: Fn(*mut c_char) -> *mut c_char,
{
    let mut buffer = Vec::<u8>::with_capacity(capacity); // uninitalized
    let dest = buffer.as_mut_ptr() as *mut c_char;
    let up_to = filler(dest);
    unsafe {
    	let length = up_to.offset_from(dest) as usize;
        buffer.set_len(length);
        String::from_utf8_unchecked(buffer)
    }
}
