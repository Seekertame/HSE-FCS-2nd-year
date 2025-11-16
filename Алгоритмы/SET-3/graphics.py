import csv
import math
import matplotlib.pyplot as plt

N = []
S_wide = []
S_narrow = []
err_wide = []
err_narrow = []

with open("data.csv", newline="") as f:
    reader = csv.reader(f, delimiter=';')
    next(reader)
    for n, s_w, s_n, e_w, e_n in reader:
        N.append(int(n))
        S_wide.append(float(s_w))
        S_narrow.append(float(s_n))
        err_wide.append(float(e_w))
        err_narrow.append(float(e_n))

S_exact = 0.25 * math.pi + 1.25 * math.asin(0.8) - 1.0

# график 1: S(N)
plt.figure()
plt.plot(N, S_wide,   label="wide")
plt.plot(N, S_narrow, label="narrow")
plt.axhline(S_exact, linestyle="--", label="S_exact")
plt.xlabel("N (число точек)")
plt.ylabel("Оценка площади S")
plt.title("Оценка площади пересечения трёх кругов")
plt.grid(True)
plt.legend()
plt.savefig("area_vs_N.png", dpi=300)

# график 2: ошибка(N)
plt.figure()
plt.plot(N, err_wide,   label="wide")
plt.plot(N, err_narrow, label="narrow")
plt.xlabel("N (число точек)")
plt.ylabel("Относительная ошибка")
plt.title("Относительная ошибка метода Монте-Карло")
plt.yscale("log")
plt.grid(True)
plt.legend()
plt.savefig("error_vs_N.png", dpi=300)

plt.show()
