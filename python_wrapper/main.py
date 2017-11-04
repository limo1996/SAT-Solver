import click

from euler import EulerTester
from test import Tester


def get_netz_username():
    nethz_username = raw_input('please enter your nethz_account name : ')
    return nethz_username


@click.command()
@click.option('--mode', type=click.Choice(['integration-test-local',
                                           'parallel-test-local',
                                           'parallel-test-euler']),
              prompt='please specify a mode ('
                     'integration-test-local | parallel-test-local | parallel-test-euler)')
@click.option('--nethz_username', default='asdf')
@click.option('--cnf_folder', default='')
def main(mode, nethz_username, cnf_folder):
    if mode == 'integration-test-local':
        test_folder = 'integration_tests' if cnf_folder == '' else cnf_folder
        tester = Tester(test_folder, parallel=False)
    elif mode == 'parallel-test-local':
        test_folder = 'parallel_tests' if cnf_folder == '' else cnf_folder
        tester = Tester(test_folder, parallel=True)
    else:
        test_folder = 'parallel_tests'if cnf_folder == '' else cnf_folder
        if nethz_username == 'asdf':
            nethz_username = get_netz_username()
        tester = EulerTester(test_folder, nethz_username)

    if mode != 'parallel-test-euler':
        print('found ' + str(len(tester.files)) + ' test files in '
              + tester.folder)
    tester.run_test()


if __name__ == '__main__':
    main()
