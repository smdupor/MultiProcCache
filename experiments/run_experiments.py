import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import os

version = "bimodal"
for t in range(0, 3):
    for size in [64, 128, 256]:
        command = "./smp_cache " + "1048576 8 "+ str(size) +" 4 " + str(t) + " ./trace/canneal.04t.longTrace"
        os.system(command)

exit()



data = pd.read_csv(r'./bimodal.csv')
df = pd.DataFrame(data, columns=['Trace', 'M', 'Misprediction Rate'])
print(df)

dfgcc = df.loc[df['Trace']=='gcc']
dfjpeg = df.loc[df['Trace']=='jpeg']
dfperl = df.loc[df['Trace']=='perl']


#df = df.pivot(index='M', columns='Trace', values='Misprediction Rate')

#print(df)
#render = df.plot(figsize=(5,3))

df=dfgcc
df.plot(x='M', y='Misprediction Rate', marker='x', markersize='5')
fig = plt.gcf()
fig.set_size_inches(8, 6, forward=True)

#print(np.mean(dfmult, axis=1))
plt.xlabel("M (bits)")
plt.ylabel("Misprediction Rate (%)")
plt.title("GCC, Bimodal")
plt.xticks(df['M'])
plt.legend()
outFname = "gcc-" + version + ".png"
fig.savefig(outFname)
#df.to_csv(r'g1raw.csv')
#fig.show()
#outFname = sys.argv[1]+".png"


df=dfjpeg
df.plot(x='M', y='Misprediction Rate', marker='x', markersize='5')
fig = plt.gcf()
fig.set_size_inches(8, 6, forward=True)

#print(np.mean(dfmult, axis=1))
plt.xlabel("M (bits)")
plt.ylabel("Misprediction Rate (%)")
plt.title("JPEG, Bimodal")
plt.xticks(df['M'])
plt.legend()
outFname = "jpeg-" + version + ".png"
fig.savefig(outFname)

df=dfperl
df.plot(x='M', y='Misprediction Rate', marker='x', markersize='5')
fig = plt.gcf()
fig.set_size_inches(8, 6, forward=True)

#print(np.mean(dfmult, axis=1))
plt.xlabel("M (bits)")
plt.ylabel("Misprediction Rate (%)")
plt.title("Perl, Bimodal")
plt.xticks(df['M'])
plt.legend()
outFname = "perl-" + version + ".png"
fig.savefig(outFname)