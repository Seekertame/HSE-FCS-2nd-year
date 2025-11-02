import math

XS = [0.0, 0.001, -0.001, 0.5, -0.5, 1.0, -1.0, 2.0, -2.0, 5.0, -5.0, 10.0, -10.0, 15.0, -15.0, 20.0, -20.0]

for i, x in enumerate(XS, 1):
    ref = math.sinh(x)
    print("-----")
    print(f"Test #{i}")
    print(f"x={x}")
    print(f"ref={ref}")
