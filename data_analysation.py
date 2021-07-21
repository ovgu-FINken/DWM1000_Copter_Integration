import pandas as pd
import seaborn as sns 
import matplotlib.pyplot as plt
import scipy 


df = pd.read_csv("./log.txt", names=["tRound1", "tReply1", "tRound2", "tReply2"])
print(df)

sns.histplot(df["tRound1"]-df["tReply1"])
g = scipy.stats.normaltest(df["tRound1"]-df["tReply1"])
print(g)
plt.show()

