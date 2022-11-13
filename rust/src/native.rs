
fn complement(x: char) -> char {
    let xx = x as u32;
    let foo = if xx & 2 != 0 { 4 } else { 21 };
    let c = xx ^ foo;
    return (c as u8) as char;
}

pub fn revcomp(forward: &str) -> String {
    use std::iter::FromIterator;

    return String::from_iter(forward.chars().rev().map(complement));
}
