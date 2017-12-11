from __future__ import print_function
import os
import subprocess
import tarfile
import time

import click


class EulerTester(object):
    def __init__(self, folder, nethz_username, num_nodes, num_runs, timeout,
                 overall_runtime_minutes):
        self.folder = folder
        self.nethz_username = nethz_username
        self.num_nodes = num_nodes
        self.num_runs = num_runs
        self.timeout = timeout
        self.overall_runtime_minutes = overall_runtime_minutes
        self.euler_folder_name = 'sat_solver_' + time.strftime(
            '%Y-%m-%d_%H-%M')
        print('Euler parameters:')
        print('           num_nodes: {}'.format(self.num_nodes))
        print('            num_runs: {}'.format(self.num_runs))
        print('             timeout: {} [seconds]'.format(self.timeout))
        print(' max_overall_runtime: {} [minutes]'
              .format(self.overall_runtime_minutes))
        print('         test_folder: {}'.format(self.folder))

    def package_tar(self, basepath, script):
        tar = tarfile.open('{}/to_euler.tar'.format(basepath), 'w')
        tar.add('{}/run_me_on_euler.sh'.format(basepath))
        tar.add('{}/{}'.format(basepath, script))
        tar.add('{}/../CMakeLists.txt'.format(basepath),
                arcname='CMakeLists.txt')
        tar.add('{}/../src'.format(basepath), arcname='src')
        tar.add('{}/../cnfs/{}'.format(basepath, self.folder),
                arcname=self.folder)
        tar.add('{}/num_nodes.txt'.format(basepath))
        tar.add('{}/test_folder.txt'.format(basepath))
        tar.add('{}/num_runs.txt'.format(basepath))
        tar.add('{}/timeout.txt'.format(basepath))
        tar.add('{}/overall_runtime_minutes.txt'.format(basepath))
        tar.close()
        return tar

    def run_test(self, basepath='.', script='bsub_script.sh'):
        test_folder_file = open('{}/test_folder.txt'.format(basepath), 'w')
        test_folder_file.truncate()
        test_folder_file.write(str(self.folder))
        test_folder_file.close()
        num_nodes_string = '4'
        if not len(self.num_nodes) is 0:
            num_nodes_string = list(map(lambda n: str(n), self.num_nodes))
            num_nodes_string = " ".join(num_nodes_string)
        num_nodes_file = open('{}/num_nodes.txt'.format(basepath), 'w')
        num_nodes_file.truncate()
        num_nodes_file.write(num_nodes_string)
        num_nodes_file.close()

        num_runs_file = open('{}/num_runs.txt'.format(basepath), 'w')
        num_runs_file.truncate()
        num_runs_file.write(str(self.num_runs))
        num_runs_file.close()

        num_runs_file = open('{}/timeout.txt'.format(basepath), 'w')
        num_runs_file.truncate()
        num_runs_file.write(str(self.timeout))
        num_runs_file.close()

        runtime_file = open('{}/overall_runtime_minutes.txt'.format(basepath),
                            'w')
        runtime_file.truncate()
        runtime_file.write(str(self.overall_runtime_minutes))
        runtime_file.close()

        self.run_test_for_num_nodes(basepath, script=script)

        os.remove('{}/test_folder.txt'.format(basepath))
        os.remove('{}/num_nodes.txt'.format(basepath))
        os.remove('{}/num_runs.txt'.format(basepath))
        os.remove('{}/timeout.txt'.format(basepath))
        os.remove('{}/overall_runtime_minutes.txt'.format(basepath))
        print('once the job is finished, run the following command to download the results file:')
        print('scp -o PreferredAuthentications=password -o PubkeyAuthentication=no {0}@euler.ethz.ch:{1}/{2}/measurements.tar .'.format(self.nethz_username, self.euler_folder_name, self.folder))

    def run_test_for_num_nodes(self, basepath, script='bsub_script.sh'):
        tar = self.package_tar(basepath, script)

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
            'mkdir {}'.format(self.euler_folder_name),
            'mv to_euler.tar {}/'.format(self.euler_folder_name),
            'cd {}'.format(self.euler_folder_name),
            'echo "#########################################################"',
            'echo "unpacking tar archive..."',
            'tar -xvf to_euler.tar > /dev/null',
            'echo "calling run_me_on_euler.sh..."',
            './run_me_on_euler.sh {}'.format(script),
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
