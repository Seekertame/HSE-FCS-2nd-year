import pandas as pd
import matplotlib.pyplot as plt

STEP = 1


def plot_for_array_type(df: pd.DataFrame, array_type: str) -> None:
    sub = df[df["array_type"] == array_type]

    quick = sub[sub["algorithm"] == "quick"].sort_values("size")
    intro = sub[sub["algorithm"] == "introsort"].sort_values("size")

    quick_sampled = quick.iloc[::STEP]
    intro_sampled = intro.iloc[::STEP]

    # QUICK SORT
    plt.figure(figsize=(8, 5))
    plt.plot(quick_sampled["size"], quick_sampled["time_ms"], label="quick")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"QUICK SORT — тип массива: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"quick_{array_type}.png", dpi=200)
    plt.close()

    # INTROSORT
    plt.figure(figsize=(8, 5))
    plt.plot(intro_sampled["size"], intro_sampled["time_ms"], label="introsort")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"INTROSORT — тип массива: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"introsort_{array_type}.png", dpi=200)
    plt.close()

    # Связная
    plt.figure(figsize=(8, 5))
    plt.plot(quick_sampled["size"], quick_sampled["time_ms"],
             label="quick", linewidth=2)
    plt.plot(intro_sampled["size"], intro_sampled["time_ms"],
             label="introsort", linewidth=2)
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"QUICK vs INTROSORT — тип массива: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"compare_{array_type}.png", dpi=200)
    plt.close()


def main() -> None:
    df = pd.read_csv("data_A3.csv")

    for t in ["random", "reversed", "almost_sorted"]:
        plot_for_array_type(df, t)

    print("Готово: quick_*, introsort_*, compare_* для random/reversed/almost_sorted.")


if __name__ == "__main__":
    main()
