import csv
import sys

import matplotlib.pyplot as plt


def load_points(filename):
    re_list, im_list = [], []
    with open(filename, "r", newline="") as f:
        for row in csv.reader(f):
            if not row:
                continue
            try:
                re_list.append(float(row[0]))
                im_list.append(float(row[1]))
            except (ValueError, IndexError):
                continue
    return re_list, im_list


def main():
    filename = sys.argv[1] if len(sys.argv) > 1 else "output/data.csv"
    re_vals, im_vals = load_points(filename)

    if not re_vals:
        print("No data")
        return

    plt.figure(figsize=(8, 6))
    plt.hexbin(re_vals, im_vals, gridsize=200, cmap="inferno", mincnt=1)
    plt.colorbar(label="points per cell")
    plt.title("Mandelbrot set (density)")
    plt.xlabel("Re(c)")
    plt.ylabel("Im(c)")
    plt.gca().set_aspect("equal", adjustable="box")
    plt.tight_layout()
    plt.savefig("output/diagram.png", dpi=150)
    plt.show()


if __name__ == "__main__":
    main()
