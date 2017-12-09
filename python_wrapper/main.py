import click

from euler import EulerTester
from test import Tester


def get_netz_username():
    nethz_username = raw_input('please enter your nethz_account name : ')
    return nethz_username


@click.command()
@click.option('--mode', type=click.Choice(['integration-test-local',
                                           'parallel-test-local',
                                           'parallel-test-euler',
                                           'stealing-test-local']),
              prompt='please specify a mode ('
                     'integration-test-local | parallel-test-local | parallel-test-euler | stealing-test-local)')
@click.option('--nethz_username', default='asdf')
@click.option('--cnf_folder', default='')
@click.option('--cdcl/--no-cdcl', default=False, help='run with cdcl')
def main(mode, nethz_username, cnf_folder, cdcl):
    if mode == 'integration-test-local':
        test_folder = 'integration_tests' if cnf_folder == '' else cnf_folder
        tester = Tester(test_folder, parallel=False, stealing=False, cdcl=cdcl)
    elif mode == 'parallel-test-local':
        test_folder = 'parallel_tests' if cnf_folder == '' else cnf_folder
        tester = Tester(test_folder, parallel=True, stealing=False, cdcl=cdcl)
    elif mode == 'stealing-test-local':
        test_folder = 'parallel_tests' if cnf_folder == '' else cnf_folder
        tester = Tester(test_folder, parallel=False, stealing=True, cdcl=cdcl)
    else:
        test_folder = 'parallel_tests' if cnf_folder == '' else cnf_folder
        if nethz_username == 'asdf':
            nethz_username = get_netz_username()
        # number of cores
        num_nodes = [4, 8]
        # number of runs per formula
        num_runs = 2
        # timeout per formula in seconds
        timeout = 10
        # overall runtime in minutes
        overall_runtime_minutes = 10
        tester = EulerTester(test_folder, nethz_username, num_nodes, num_runs,
                             timeout, overall_runtime_minutes)

    if mode != 'parallel-test-euler':
        print('found ' + str(len(tester.files)) + ' test files in '
              + tester.folder)
    tester.run_test()


if __name__ == '__main__':
    main()
