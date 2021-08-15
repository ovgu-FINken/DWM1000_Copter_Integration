import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy
from scipy.stats import norm
import numpy as np

from sklearn import datasets, linear_model
from sklearn.metrics import mean_squared_error, r2_score


tsfreq = 499.2e6 * 128 # Timestamp counter frequency
speedOfLight = 299792458.0 # Speed of light in m/s
MAGIC_RANGE_OFFSET = 153.7 - 0.869707882 + 1.478

df0 = pd.read_csv("./data/sync_error_test_boxed_mod3.txt", names=["tStartRound1", "tStartReply1", "tEndReply1", "tEndReply2", "tEndRound2"])
df1 = pd.read_csv("./data/sync_error_test_boxed_mod2.txt", names=["tStartRound1", "tStartReply1", "tEndReply1", "tEndReply2", "tEndRound2"])
df2 = pd.read_csv("./data/sync_error_test_boxed_2.txt", names=["tStartRound1", "tStartReply1", "tEndReply1", "tEndReply2", "tEndRound2"])
df3 = pd.read_csv("./data/sync_error_test_boxed_mod4.txt", names=["tStartRound1", "tStartReply1", "tEndReply1", "tEndReply2", "tEndRound2"])
print(df0)

time_sync_1 = df0["tStartRound1"]-df0["tStartReply1"]
time_sync_2 = []
time_sync_3 = []
time_sync_4 = []
time_sync_5 = []
for i in range(1, len(df0["tStartRound1"])):
    g = df0["tStartRound1"][i]-df0["tStartRound1"][i-1]
    h = df0["tStartReply1"][i]-df0["tStartReply1"][i-1]
    if((g-h)/g) > 0.1:
        continue
    time_sync_2.append((g-h)/g)
for i in range(1, len(df1["tStartRound1"])):
    g = df1["tStartRound1"][i]-df1["tStartRound1"][i-1]
    h = df1["tStartReply1"][i]-df1["tStartReply1"][i-1]
    if((g-h)/g) > 0.1:
        continue
    time_sync_3.append((g-h)/g)
for i in range(1, len(df2["tStartRound1"])):
    g = df2["tStartRound1"][i]-df2["tStartRound1"][i-1]
    h = df2["tStartReply1"][i]-df2["tStartReply1"][i-1]
    if((g-h)/g) > 0.1:
        continue
    time_sync_4.append((g-h)/g)

for i in range(1, len(df3["tStartRound1"])):
    g = df3["tStartRound1"][i]-df3["tStartRound1"][i-1]
    h = df3["tStartReply1"][i]-df3["tStartReply1"][i-1]
    if((g-h)/g) > 0.1:
        continue
    time_sync_5.append((g-h)/g)

time_sync_2 = np.array(time_sync_2)
time_sync_3 = np.array(time_sync_3)
time_sync_4 = np.array(time_sync_4)
time_sync_5 = np.array(time_sync_5)

plt.plot(time_sync_2)
plt.plot(time_sync_3)
plt.plot(time_sync_4)
plt.plot(time_sync_5)

print(time_sync_2)

nr = range(0,1001)

plt.show()
