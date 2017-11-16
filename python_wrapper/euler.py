from __future__ import print_function
import os
import subprocess
import tarfile

import click


class EulerTester(object):
    def __init__(self, folder, nethz_username, num_nodes):
        self.folder = folder
        self.nethz_username = nethz_username
        self.num_nodes = num_nodes

    def package_tar(self):
        tar = tarfile.open('to_euler.tar', 'w')
        tar.add('run_me_on_euler.sh')
        tar.add('bsub_script.sh')
        tar.add(os.path.join(os.pardir, 'CMakeLists.txt'),
                arcname='CMakeLists.txt')
        tar.add(os.path.join(os.pardir, 'src'), arcname='src')
        tar.add(os.path.join(os.path.join(os.pardir, 'cnfs'), self.folder),
                arcname=self.folder)
        tar.add('num_nodes.txt')
        tar.add('test_folder.txt')
        tar.close()
        return tar

    def run_test(self):
        test_folder_file = open('test_folder.txt', 'w')
        test_folder_file.truncate()
        test_folder_file.write(str(self.folder))
        test_folder_file.close()
        num_nodes_string = '4'
        if not len(self.num_nodes) is 0:
            num_nodes_string = list(map(lambda n: str(n), self.num_nodes))
            num_nodes_string = " ".join(num_nodes_string)
        num_nodes_file = open('num_nodes.txt', 'w')
        num_nodes_file.truncate()
        num_nodes_file.write(num_nodes_string)
        num_nodes_file.close()

        self.run_test_for_num_nodes()

        os.remove('test_folder.txt')
        os.remove('num_nodes.txt')
        print('once the job is finished, run the following command to download the results file:')
        print('scp -o PreferredAuthentications=password -o PubkeyAuthentication=no {0}@euler.ethz.ch:sat_solver/{1}/time_measurements.tar .'.format(self.nethz_username, self.folder))

    def run_test_for_num_nodes(self):
        tar = self.package_tar()

        scp_process = subprocess.Popen(['scp',
                                        '-o PreferredAuthentications=password',
                                        '-o PubkeyAuthentication=no',
                                        tar.name,
                                        '{0}@euler.ethz.ch:'.format(
                                            self.nethz_username)])
        scp_process.wait()

        ssh_process = subprocess.Popen(['ssh',
                                        '-o PreferredAuthentications=password',
                                        '-o PubkeyAuthentication=no',
                                        '{0}@euler.ethz.ch'.format(
                                            self.nethz_username)],
                                       stdin=subprocess.PIPE,
                                       stdout=subprocess.PIPE,
                                       universal_newlines=True,
                                       bufsize=0)
        commands = [
            'rm -rf sat_solver',
            'mkdir sat_solver',
            'mv to_euler.tar sat_solver/',
            'cd sat_solver',
            'echo "#########################################################"',
            'echo "unpacking tar archive..."',
            'tar -xvf to_euler.tar > /dev/null',
            'echo "calling run_me_on_euler.sh..."',
            './run_me_on_euler.sh',
            'echo "END"'
        ]
        for c in commands:
            ssh_process.stdin.write('{0}\n'.format(c))

        ssh_process.stdin.close()

        for line in ssh_process.stdout:
            if line == "END\n":
                break
            print(line, end='')

        for line in ssh_process.stdout:
            if line == "END\n":
                break
            print(line, end='')


@click.command()
@click.option('--folder', default='parallel_tests')
@click.option('--nethz_username',
              prompt='please enter your nethz_account name')
def main(folder, nethz_username):
    e = EulerTester(folder, nethz_username)
    e.run_test()


if __name__ == '__main__':
    main()
