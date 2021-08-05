import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy
from scipy.stats import norm

df0 = pd.read_csv("./times_log_1m_v1.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df1 = pd.read_csv("./times_log_1m_v2.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df2 = pd.read_csv("./times_log_1m_v3.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df3 = pd.read_csv("./times_log_1,25m_v0.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df4 = pd.read_csv("./times_log_1,25m_v1.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
df5 = pd.read_csv("./times_log_1,25m_v2.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
print(df0)

# concat
# hue = categorie

#sns.histplot(df0["tRound1"]-df0["tReply1"])
#sns.histplot(df1["tRound1"]-df1["tReply1"])
#sns.histplot(df2["tRound1"]-df2["tReply1"])
#sns.histplot(df3["tRound1"]-df3["tReply1"])
#sns.histplot(df4["tRound1"]-df4["tReply1"])
sns.histplot(df5["tRound1"]-df5["tReply1"])

g = scipy.stats.normaltest(df0["tRound1"]-df0["tReply1"])
print(g)
mean, var, skew, kurz = norm.stats()
plt.show()
