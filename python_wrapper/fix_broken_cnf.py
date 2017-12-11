import click
import os


def read_data(file_name):
    data = []
    with open(file_name, 'r') as f:
        data = [line for line in f]
    res = []
    acc = ''
    for l in data:
        if l.startswith('c') or l.startswith('p'):
            res.append(l)
        elif l.startswith('\n'):
            continue
        elif l.endswith(' 0') or l.endswith(' 0\n') or l.endswith(' 0 \n'):
            l_ = ' '.join(l.split()) + '\n'
            res.append(acc + l_)
            acc = ''
        else:
            acc += l.replace('\n', '')

    return list(map(lambda l: l.lstrip(' '), res))


def write_data(file_name, data):
    with open(file_name, 'w') as f:
        for d in data:
            f.write(d)


@click.command()
@click.option('--cnf', prompt='pass a cnf file')
def main(cnf):
    file_mod = cnf + '_properly_formatted.cnf'
    data = read_data(cnf)
    write_data(file_mod, data)


if __name__ == '__main__':
    main()
