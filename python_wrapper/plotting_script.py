import sys
from plotter import Plotter

if len(sys.argv) > 1:
    p = Plotter(sys.argv[1])
else:
    print("Please specify path to folder as second argument!")