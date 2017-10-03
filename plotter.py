import numpy as np
import matplotlib.pyplot as plt

data = np.genfromtxt("Data.txt")

image = plt.imshow(data, cmap = "Greys")
plt.colorbar(image)

plt.savefig("Complete.pdf")