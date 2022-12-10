fn complement(x: char) -> char {
    let xx = x as u32;
    let foo = if xx & 2 != 0 { 4 } else { 21 };
    let c = xx ^ foo;
    return (c as u8) as char;
}

pub fn revcomp(forward: &str) -> String {
    use std::iter::FromIterator;

    String::from_iter(forward.chars().rev().map(complement))
}

pub fn count_mismatches(s1: &str, s2: &str) -> u64 {
    let mut mm: u64 = 0;
    for (a, b) in s1.bytes().zip(s2.bytes()) {
        if a != b {
            mm += 1;
        }
    }
    mm
}

pub fn find_first_mismatch(s1: &str, s2: &str) -> u64 {
    let mut ii = 0;
    for (a, b) in s1.bytes().zip(s2.bytes()) {
        if a != b {
            break;
        }
        ii += 1;
    }
    ii
}
