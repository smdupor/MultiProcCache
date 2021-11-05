import pandas as pd
import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib as mpl
import os


expname = "bs_"

data = pd.read_csv(r'./blocksize_experiment.csv')
df = pd.DataFrame(data, columns=['Protocol', 'Size', 'Associativity', 'BlockSize', 'Reads', 'ReadMisses', 'Writes',
                                 'WriteMisses', 'MissRate', 'WriteBacks', 'CcTransfers', 'MemoryTraffic', 'Interventions',
                                 'Invalidations', 'Flushes', 'BusRdX'])
print(df)

dfP = df.pivot(index='BlockSize', columns='Protocol', values=['ReadMisses', 'WriteMisses', 'MissRate', 'WriteBacks',
                                                         'CcTransfers', 'MemoryTraffic', 'Interventions',
                                                         'Invalidations', 'Flushes', 'BusRdX'])


print(dfP)

# create valid markers from mpl.markers
valid_markers = ([item[0] for item in mpl.markers.MarkerStyle.markers.items() if
                  item[1] is not 'nothing' and not item[1].startswith('tick') and not item[1].startswith('caret')])

# valid_markers = mpl.markers.MarkerStyle.filled_markers
# print(valid_markers)
# markers = np.random.choice(valid_markers, df.shape[1], replace=False)

df=dfP
ax=df.plot(kind='line', y='ReadMisses', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Read Misses (Quantity)")
plt.title("Read Misses vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "readmiss" +".png"
fig.savefig(outFname)




ax=df.plot(kind='line', y='WriteMisses', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Write Misses (Quantity)")
plt.title("Write Misses vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "writemiss" + ".png"
fig.savefig(outFname)






ax=df.plot(kind='line', y='MissRate', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Miss Rate (%)")
plt.title("Mean Miss Rate vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "meanmiss" + ".png"
fig.savefig(outFname)


ax=df.plot(kind='line', y='WriteBacks', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("WriteBacks (Quantity)")
plt.title("Writebacks vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "writeback" + ".png"
fig.savefig(outFname)


ax=df.plot(kind='line', y='MemoryTraffic', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Global Memory Traffic (Transactions)")
plt.title("Global Memory Traffic vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "memtraffic" + ".png"
fig.savefig(outFname)


ax=df.plot(kind='line', y='Interventions', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Interventions (Quantity)")
plt.title("Interventions vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "intervention" + ".png"
fig.savefig(outFname)



ax=df.plot(kind='line', y='Invalidations', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Global Invalidations (Quantity)")
plt.title("Global Invalidations vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "invalidation" + ".png"
fig.savefig(outFname)


ax=df.plot(kind='line', y='Flushes', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("Flushes (Quantity)")
plt.title("Flushes vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "flushes" + ".png"
fig.savefig(outFname)



ax=df.plot(kind='line', y='BusRdX', markersize='10')
for i, line in enumerate(ax.get_lines()):
    if i < 1:
        line.set_marker(valid_markers[i])
    else:
        line.set_marker(valid_markers[i+6])
    if i is 2:
        line.set_linestyle('dashed')

plt.xlabel("Block Size (Bytes)")
plt.ylabel("BusRdX (Quantity of Signals)")
plt.title("BusRdX Signals vs. Block Size")
plt.legend()
ax.set_xticks([64, 128, 256])
#plt.show()
fig = plt.gcf()
fig.set_size_inches(8, 5, forward=True)
outFname = expname + "busrdx" + ".png"
fig.savefig(outFname)