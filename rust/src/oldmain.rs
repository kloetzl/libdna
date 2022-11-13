
pub mod libdna_ffi; // "import" libdnaffi
pub mod native;

fn main() {
	use crate::libdna_ffi::dna;
	println!("{:?}", dna::version());

    let forward = String::from("AACTAATT");
    let backward = native::revcomp(&forward);
    println!("{:?}", forward);
    println!("{:?}", backward);
    let backward = dna::revcomp(&forward);
    println!("{:?}", backward);
}
