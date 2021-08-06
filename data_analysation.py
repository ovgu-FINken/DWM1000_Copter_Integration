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
    avg = (speedOfLight * avg / tsfreq)
    df["avg_dist"] = avg # double naming of the parameter ... avg_dist is the dist from double sided two way ranging here
    # avg = ((speedOfLight * avg / tsfreq) - MAGIC_RANGE_OFFSET ) * 0.90
    # g = scipy.stats.normaltest(avg)
    # print(g)
    mean = np.average(avg)
    var = np.var(avg)
    std = np.std(avg)
    print(f"mean:{mean} \t var:{var} \t std:{std}")

df0["solution"] = [1.0 for i in range(0, len(df0["avg_dist"]))]
df1["solution"] = [1.2 for i in range(0, len(df1["avg_dist"]))]
df2["solution"] = [1.4 for i in range(0, len(df2["avg_dist"]))]
df3["solution"] = [1.6 for i in range(0, len(df3["avg_dist"]))]
df4["solution"] = [1.8 for i in range(0, len(df4["avg_dist"]))]
df5["solution"] = [2.0 for i in range(0, len(df5["avg_dist"]))]

big_df = pd.concat([df0, df1, df2, df3, df4, df5])
small_df = pd.DataFrame()

print()
small_df["avg_dist"] = [np.average(df0["avg_dist"]), # here average_dist is the average of the average_dist for one distance
                    np.average(df1["avg_dist"]),
                    np.average(df2["avg_dist"]),
                    np.average(df3["avg_dist"]),
                    np.average(df4["avg_dist"]),
                    np.average(df5["avg_dist"])]
print(small_df)
small_df["reference"] = [1.0, 1.2, 1.4, 1.6, 1.8, 2.0]

X = np.array(big_df["avg_dist"]).reshape(-1, 1)
Y = np.array(big_df["solution"]).reshape(-1, 1)

regr = linear_model.LinearRegression()
regr.fit(X, Y)

print('Coefficients: \n', regr.coef_)
# The mean squared error
print(f'Intercept: \n {regr.intercept_}')

y_pred = regr.predict(X)

print('Mean squared error: %.2f'
    % mean_squared_error(Y, y_pred))
# The coefficient of determination: 1 is perfect prediction


plt.figure()
plt.scatter(X, Y)
plt.plot(X, y_pred, color='red')


print(regr.get_params(deep=True))

dist_in_m = np.array(big_df["avg_dist"])*regr.coef_ + regr.intercept_
# big_df["dist_in_m"] = dist_in_m

print(dist_in_m[0])
print(range(0, len(dist_in_m[0])))
big_df["dist_in_m"] = dist_in_m[0]

plt.figure()
plt.plot(range(0,len(dist_in_m[0])), dist_in_m[0])
plt.axhline(y=1.0)
plt.axhline(y=1.2)
plt.axhline(y=1.4)
plt.axhline(y=1.6)
plt.axhline(y=1.8)
plt.axhline(y=2.0)

# determine error distribution over all distances
error = big_df["dist_in_m"]-big_df["solution"]
normaltest = scipy.stats.normaltest(error)
print(normaltest)
avg_error = np.average(error)
var_error = np.var(error)
std_deriv_error = np.std(error)
min_error = min(error)
max_error = max(error)
print(f"average) error: {avg_error} \t varianz of error: {var_error} \t standart derivation: {std_deriv_error}")

plt.figure()
sns.boxplot(error)
plt.figure()
sns.histplot(error)

plt.show()
