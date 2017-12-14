import sys
import click

from time_plotter import TimePlotter
from comm_plotter import CommPlotter

@click.command()
@click.option('-f', default='../cnfs/steal_vs_master')
@click.option('--mode', type=click.Choice(['steal_vs_master',
                                           'seq_vs_master']),
              prompt='please specify a mode ('
                      'steal_vs_master | seq_vs_master')
def main(f, mode):
    if(mode == 'steal_vs_master'):
        plotter = CommPlotter(f)
    else:
        plotter = TimePlotter(f)

if __name__ == '__main__':
    main()