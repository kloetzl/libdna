from libdna import dna4
import argparse


def main():
    parser = argparse.ArgumentParser(
        prog="random", description="Create a random string of DNA"
    )
    parser.add_argument("-s", type=int, help="seed", default=1729)
    parser.add_argument("-l", type=int, help="length", default=80)

    args = parser.parse_args()
    seed = args.s
    length = args.l

    print(f">rnd {seed=}")
    print(dna4.random(length, seed))


if __name__ == "__main__":
    main()
