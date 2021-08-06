import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy
from scipy.stats import norm
import numpy as np

df0 = pd.read_csv("./data/times_1m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df1 = pd.read_csv("./data/times_1.20m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df2 = pd.read_csv("./data/times_1.40m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df3 = pd.read_csv("./data/times_1.60m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df4 = pd.read_csv("./data/times_1.80m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df5 = pd.read_csv("./data/times_2.00m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
print(df0)

nr = range(0,10001)

g = [df0, df1, df2, df3, df4, df5]

for df in g:
    df["nr"] = nr
    plt.figure()
    sns.lineplot(y=(df["tRound1"]-df["tReply1"]), x=df0["nr"])
    sns.lineplot(y=(df["tRound2"]-df["tReply2"]), x=df0["nr"])
    avg = ((df["tRound1"]-df["tReply1"]) + (df["tRound2"]-df["tReply2"]))/2
    sns.lineplot(y=avg, x=df0["nr"])
    print(np.average(np.array(avg)))

# plt.figure("1m")
# sns.histplot(df0["tRound1"][4000:10000]-df0["tReply1"][4000:10000])
# avg1 = sum(df0["tRound1"][4000:10000]-df0["tReply1"][4000:10000]) / (10000-4000)
# plt.figure("1.2m")
# sns.histplot(df1["tRound1"][4000:10000]-df1["tReply1"][4000:10000])
# avg2 = sum(df1["tRound1"][4000:10000]-df1["tReply1"][4000:10000]) / (10000-4000)
# plt.figure("1.4m")
# sns.histplot(df2["tRound1"][4000:10000]-df2["tReply1"][4000:10000])
# avg3 = sum(df2["tRound1"][4000:10000]-df2["tReply1"][4000:10000]) / (10000-4000)
# plt.figure("1,6m")
# sns.histplot(df3["tRound1"][4000:10000]-df3["tReply1"][4000:10000])
# avg4 = sum(df3["tRound1"][4000:10000]-df3["tReply1"][4000:10000]) / (10000-4000)
# plt.figure("1.8m")
# sns.histplot(df4["tRound1"][4000:10000]-df4["tReply1"][4000:10000])
# avg5 = sum(df4["tRound1"][4000:10000]-df4["tReply1"][4000:10000]) / (10000-4000)
# plt.figure("2m")
# sns.histplot(df5["tRound1"][4000:10000]-df5["tReply1"][4000:10000])
# avg6 = sum(df5["tRound1"][4000:10000]-df5["tReply1"][4000:10000]) / (10000-4000)


# g = scipy.stats.normaltest(df0["tRound1"]-df0["tReply1"])
# print(g)
# mean, var, skew, kurz = norm.stats()
plt.show()
