import numpy as np


def main():
    N = 50_000
    
    prng = np.random.default_rng()
    dirs = prng.choice(["forward", "down", "up"], size=N, shuffle=False)
    mags = prng.integers(low=1, high=9, size=N, endpoint=True)
    stacked = np.c_[dirs, mags]
    np.savetxt("02.txt", stacked, delimiter=" ", fmt="%s")


if __name__ == "__main__":
    main()
