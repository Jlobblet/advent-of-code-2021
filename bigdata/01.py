import numpy as np
import matplotlib.pyplot as plt


def dist():
    with open("../data/01.txt", "r") as f:
        data = np.array([int(l) for l in f.readlines()])

    plt.hist(np.diff(data))
    plt.show()


def main():
    N = 1_000_000
    spread = 50
    start = 200
    mids = np.linspace(start, start + N, N)
    lows = mids - spread
    highs = mids + spread
    
    prng = np.random.default_rng()
    big = prng.integers(lows, highs, endpoint=True)
    np.savetxt("01.txt", big, fmt="%d")


if __name__ == "__main__":
    main()
