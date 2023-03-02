import numpy;
import matplotlib.pyplot as plt;
import matplotlib.ticker;
import os
import csv

fileNames = [];
spiecesmap = [];


def loadData(path):
    data = [];
    # load the files in the data dir
    for dirpath, dirs, files in os.walk(path):
        for file in files:
            print(file);
            fileNames.append(file);
            f_trim = file.find("species_");
            l_trim = file.find(".csv");
            spiecesmap.append(file[f_trim+8:l_trim])
            fileData = []
            with open(os.path.join(path, file), newline="\n") as f:
                fileInternal = csv.reader(f, delimiter="\t")
                for row in fileInternal:
                    if len(fileData) == 0:
                        for i in range(len(row)):
                            fileData.append([]);
                    for i in range(len(row)):
                        fileData[i].append(row[i])
            data.append(fileData)
    return data


data = loadData('./data');
print(data);

def plot3d(i, data, currentCounter, currentSpecies):


    fig = plt.figure(figsize=(10, 10));

    ax = plt.axes(projection='3d');

    x_axis = numpy.array(data[i][2][1:], dtype=float);
    y_axis = numpy.array(data[i][3][1:], dtype=float);
    z_axis = numpy.array(data[i][1][1:], dtype=float);

    ax.set_xlabel(data[i][2][:1][0]);
    ax.set_ylabel(data[i][3][:1][0]);
    ax.set_zlabel("Species " + data[i][0][1:2][0] + " value.");



    ax.set_title("Species: " + spiecesmap[i] );
    formatter = matplotlib.ticker.EngFormatter(useMathText=True);
    # formatter.set_scientific(True);
    plt.ticklabel_format(style='sci', axis='both', scilimits=(0, 0))

    x_tick_label = ['{:5.2e}'.format(x) for x in x_axis]
    ax.set_xticklabels(x_tick_label, rotation=0)

    y_tick_label = ['{:5.2e}'.format(x) for x in y_axis]
    ax.set_yticklabels(y_tick_label, rotation=0)

    z_tick_label = ['{:5.2e}'.format(x) for x in z_axis]
    ax.set_zticklabels(z_tick_label, rotation=0)

    ax.plot3D(x_axis, y_axis, z_axis);

    # ax.xaxis.set_major_formatter(formatter);
    # ax.xaxis.set_minor_formatter(formatter);
    # ax.yaxis.set_major_formatter(formatter);
    # ax.zaxis.set_major_formatter(formatter);

    print(fileNames[i] + "_3d.pdf")
    plt.savefig(fileNames[i] + "_3d.pdf")


def plot2d(i, data, currentCounter, currentSpecies):
    fig = plt.figure(figsize=(10, 10));

    ax = plt.axes();

    x_axis = numpy.array(data[i][1][1:], dtype=float);
    y_axis = numpy.array(data[i][3][1:], dtype=float);

    ax.set_xlabel("Species " + data[i][0][1:2][0] + " value.");
    ax.set_ylabel(data[i][3][:1][0]);


    ax.set_title("Species: " + spiecesmap[i]);
    formatter = matplotlib.ticker.EngFormatter(useMathText=True);
    # formatter.set_scientific(True);
    plt.ticklabel_format(style='sci', axis='both', scilimits=(0, 0))
    ax.plot(x_axis, y_axis);

    x_tick_label = ['{:5.2e}'.format(x) for x in x_axis]
    ax.set_xticklabels(x_tick_label, rotation=0)

    y_tick_label = ['{:5.2e}'.format(x) for x in y_axis]
    ax.set_yticklabels(y_tick_label, rotation=0)
    # ax.xaxis.set_major_formatter(formatter);
    # ax.xaxis.set_minor_formatter(formatter);
    # ax.yaxis.set_major_formatter(formatter);
    # ax.zaxis.set_major_formatter(formatter);

    print(fileNames[i] + "_2d.pdf");
    plt.savefig(fileNames[i] + "_2d.pdf")


currentSpecies = [0];
currentCounter = [0];

for i in range(len(data)):
    # simple graph plot

    #plot3d(i, data, currentCounter, currentSpecies);
    plot2d(i, data, currentCounter, currentSpecies);