import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy
from scipy.stats import norm
import numpy as np

tsfreq = 499.2e6 * 128 # Timestamp counter frequency
speedOfLight = 299792458.0 # Speed of light in m/s
MAGIC_RANGE_OFFSET = 153.7 - 0.869707882 + 1.478

df0 = pd.read_csv("./data/times_1m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df1 = pd.read_csv("./data/times_1.20m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df2 = pd.read_csv("./data/times_1.40m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df3 = pd.read_csv("./data/times_1.60m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df4 = pd.read_csv("./data/times_1.80m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df5 = pd.read_csv("./data/times_2.00m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
print(df0)

nr = range(0,10001)

g = [df0, df1, df2, df3, df4, df5]

# for df in g:
#     df["nr"] = nr
#     plt.figure()
#     sns.lineplot(y=(df["tRound1"]-df["tReply1"]), x=df0["nr"])
#     sns.lineplot(y=(df["tRound2"]-df["tReply2"]), x=df0["nr"])
#     avg = ((df["tRound1"]-df["tReply1"]) + (df["tRound2"]-df["tReply2"]))/2
#     sns.lineplot(y=avg, x=df0["nr"])
#     plt.figure()
#     sns.histplot(avg)
#     print(np.average(np.array(avg)))

# tPropTick = (double)((tRound1 * tRound2) - (tReply1 * tReply2)) / (tRound1 + tReply1 + tRound2 + tReply2);
for df in g:
    avg = ((df["tRound1"]-df["tReply1"]) + (df["tRound2"]-df["tReply2"]))/4
    avg = ((speedOfLight * avg / tsfreq) - MAGIC_RANGE_OFFSET ) * 0.90
    g = scipy.stats.normaltest(avg)
    print(g)
    mean = np.average(avg)
    var = np.var(avg)
    print(f"mean:{mean} \t var:{var}")
plt.show()
