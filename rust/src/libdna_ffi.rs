pub mod dna;
pub mod dna4;
pub mod dnax;

// mod util {}

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
