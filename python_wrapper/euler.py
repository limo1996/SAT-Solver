from __future__ import print_function
import os
import subprocess
import tarfile

import click


class EulerTester(object):
    def __init__(self, folder, nethz_username):
        self.folder = folder
        self.nethz_username = nethz_username

    def package_tar(self):
        tar = tarfile.open('to_euler.tar', 'w')
        tar.add('run_me_on_euler.sh')
        tar.add('bsub_script.sh')
        tar.add(os.path.join(os.pardir, 'CMakeLists.txt'),
                arcname='CMakeLists.txt')
        tar.add(os.path.join(os.pardir, 'src'), arcname='src')
        tar.add(os.path.join(os.pardir, self.folder),
                arcname='parallel_tests')
        tar.close()
        return tar

    def run_test(self):
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
