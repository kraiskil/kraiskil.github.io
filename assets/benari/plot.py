import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import numpy as np

df = pd.read_csv('log.csv')


# Coerce matplotlib to draw x labels as integers
# in the centre of the columns.
# Idea is to have different axis for data and labels :)
# Based on a nice reply to a good question on SO
# https://stackoverflow.com/a/53017960/2365669
binlabels=list(range(2,21)) # [2 - 20]
bincentres = np.arange(2, 22) - 0.5 # [1.5 : 1 : 20.5]
data = np.array(df['result'])
plt.hist(data, bincentres)
plt.xticks(binlabels)

plt.savefig("plot.png")
plt.show()

print("Minimum value found: %d" % df['result'].min())
print("Maximum value found: %d" % df['result'].max())
